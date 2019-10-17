FalaiseArt Services
===================

Falaise's underlying Bayeux library provides services. Whilst many of
these are easily convertible to Art services, there are a few oddities.
These are due to the use of the `datatools::kernel` class, which provides
a second communication channel between a few services.

For example, the `datatools::variant_service` registers itself with the
kernel as part of its startup. The kernel (singleton, BTW) has member
functions to return both the service, or things it holds...

- TBD: where these functions are called behind the scenes in Bayeux.
  In principle, everywhere kernel is called. Appears confined to
  datatools, or highlevel programs only.
- The major one in terms of usability of Bayeux is use `variant_preprocessor`
  in `datatools::properties`. That affects parsing and use of config files

The `datatools::kernel` also holds its own `datatools::service_manager`, which
holds a `datatools::library_query_service` and a `datatools::urn_query_service`.
There're no direct "set" member functions for these, but there are backdoor
registrations via the kernel singleton. The `urn_query_service` is just returned
directly. The `library_query_service` is also return, as is a `library_info` object
which is obtained from the service.

The `library_query_service` is used in Falaise to register resource paths and so on.
These are likely used to resolve paths in the form "@falaise:<path>". Yes, used
in implementation of `datatools/utils.h". Could provide an Art service to wrap these,
but behind the scenes stuff in Bayeux would still need to use the kernel.

For the `datatools::urn_query_service`, it's constructed in `datatools::kernel::_initialize_urn_query_service_()`
as

```c++
datatools::urn_query_service & kUrnQuery = dynamic_cast<datatools::urn_query_service &>(_services_->load_no_init("bxDtKernUrnQuery","datatools::urn_query_service"));
kUrnQuery.initialize_simple();
```

that member function is always called as part of singleton init. The resulting
service is handed out via the `get/grab_urn_query` member functions. These are
used in Bayeux in two places:

- `datatools::urn_to_path_resolver_service`
  - Via the `initialize` and `kernel_push` member functions, the service adds itself
    to the kernels `urn_query_service` IAOI the properties config contains a "kernel.push"
    flag.
- `datatools::urn_db_service`
  - As above

In Falaise, both of these services are setup and registered with the kernel in the
`falaise::falaise_sys` singleton, specifically the `_initialize_urn_services_`
member function.

Suggests that we need a "Bayeux/FalaiseKernelService" that offers no interface and simply
exists to ensure the singleton is started. Other services can then depend on this, ensuring
that the singleton is available. However, may be some dependencies on other areas such
as `libinfo` and so on.

Try and Summarize
=================
1. Bayeux's kernel constructs the builtin "library_query_service" on initialization,
   and provides interfaces to access this, and, the "library_info" object the service
   holds.
   - In Bayeux, these are accessed by, other than the kernel itself:
     - datatools/utils.cc, in fetch_path_processor::process_impl(std::string& path),
       where it's used to resolve the input path if it matches the "^@mp" syntax.
     - Consequently, anything that calls any of the various "fetch_path" functions.
   - In Falaise, confined to its own Singleton and applications. The "fetch_path"
     functions are used widely in modules.
   - In terms of implementation, means we could
     - Provide an Art service to start the Bayeux kernel singleton
     - Provide a "PathResolver" service that holds the resulting "library_info"
       and provides an Adaptor interface to this.
     - PathResolver would need to depend on the Kernel service to guarantee
       presence of the singleton, and hence the library_info object.
2. Bayeux's kernel holds a variant_service and variant_repository, and accessors
   for them. The variant_repository is *separate* from that supplied in the variant_service.
   The methods `get/grab_variant_repository` return this separate repo.

   - In Bayeux, outside the kernel, only the grab_variant_repository is called
     and by the variant_repository itself, in its `system_export`, `system_discard`
     methods. These call the datatools::kernel::import_configuration_repository
     method with `this` (so imports itself), and clear_configuration_registry.
   - In Falaise, not used at all.

   There are also the `get/grab_effective_variant_repository` methods. These
   return the variant_repository held by the held `variant_service`, if present in
   the kernel.

   - In Bayeux itself, outside the kernel, get type is only called in datatools/configuration/io.cc
     by the _set_default_kernel_repository method of the variant_preprocessor class.
     So that repo is used by anything that uses variant_preprocessor. In Bayeux, that's
     properties and multi_properties.

   - In Falaise, not used at all.

   There are finally the get/grab/set_variant_service methods, these access/set the
   directly held variant_service. It doesn't go through the internal service_manager.

   - In Bayeux, outside the kernel, only called in variant_service itself.
     - get version only called in _do_variant_system_discard to ensure removal from
       kernel only removes itself. This discard method always called by stop()
       method
     - grab version not used at all
     - set version only called in _do_variant_system_export to set itself as the
       kernel's variant service. This export method always called by start() method
       of variant_service
   - In Falaise, not called at all.

   Provided we can reset the variant_service (i.e. between runs), we probably don't need
   to worry about interaction with the kernel. If we have to go through construct/destruct
   cycles, will need to additional communication with Kernel.

   Both flsimulate and flreconstruct explicitly create, configure and start a variant_service
   in:

   - In flsimulate, happens in do_flsimulate, immediately after FLSimulateArgs are filled.
     There are a couple of extra config steps using the service, using data in the FLSimulateArgs
     instance.
   - In flreconstruct, happens in do_pipeline, uses info in supplied FLReconstructParams
     instance. Only the "variantSubsystemParams" object is used. This is an instance
     of datatools::configuration::variant_service::config. So need to see how this
     object gets filled. Then easy to write an Art Service to read multi/properties
     and config the service. This is all in FLReconstructImpl.
3. Bayeux's kernel constructs a `urn_query_service` as part of initialization, and
   provides a get/grab_urn_query method to get this (held by internal service_manager)
   - In Bayeux, other than kernel itself, these are only called by urn_to_path_resolver_service.cc
     and urn_db_service.cc.
     - Both of these use the methods in the same way
     - An "is_kernel_pushed" method that checks if the object in question in a db/resolver in the kernel urn_query_service
     - A "kernel_push" method that adds the db/resolver to the kernel urn_query_service (with a name)
     - A "kernel_pop" method that removes the db/resolver from the kernel urn_query_service
     - The push/pop methods are called as part of the initialize/reset methods
     - A "kernel.push" flag in the input config sets whether to do this
     - Note that Bayeux's kernel setup creates its own urn DB/resolver, and adds these to the
       kernel! So the urn_query_service *has* to be in the kernel. Not totally clear why then the
       DB/resolver are services....
   - In Falaise, used in:
     - flreconstruct: FLReconstructCommandLine/do_help_pipeline_list,
     - flreconstruct: FLReconstructPipeline/ensure_core_services, to extract path to geometry config file
     - flreconstruct: FLReconstructImpl/do_postprocess
     - falaise/metadata_utils: in metadata_input::scan
     - flsimulate: FLSimulateArgs/do_postprocess, to resolve urns->paths in input parameters
     - flsimulate: FLSimulateUtils/list_of_simulation_setups, to lookup lists of urns
     - flsimulate: flsimulatecfgmain/do_configure, used to check urn info
   - In Falaise, there is also direct construction of a urn_db_service and urn_to_path_resolver service
     in falaise_sys/_initialize_urn_services_ (NB: relies on library_query_service as calls fetch_path_with_env!
     - Both are configured using hard-coded paths to config files, albeit using the "@falaise"
       mount point location, so are relocatable.
     - Both are held by the falaise_sys "kernel" in its internal service manager instance.
     - Both are installed/removed to/from the datatools kernel via explicit calls to the kernel_push, kernel_pop

Implementation
==============
Couple of important things:

1. We can make a service depend on others by holding an art::ServiceHandle to the
other service, obtaining it in the constructor.
2. A dependent service must still be declared in the FHiCL `services` table even if we
don't use it directly. Art does emit a helpful error message of we forget it.
3. Services are constructed in dependency, then alphanumeric order.
4. Services are destructed in reverse order to construction.
5. At least the preBeginRun service callbacks are called for each service in
   service construction order (not clear for post actions...)

Will need to consider these, as Art introduces the Run->Run transition, which
we don't have in Falaise.

In some cases, the kernel layer requires actual Bayeux services, so may need to
hold instances of these in the Art "wrapper" service rather than the object
the Bayeux service wraps (e.g. geometry manager in geometry service). It should
still be possible to write suitable interfaces, just with one extra layer of
indirection. A clear example of this is the variant_service.

KernelService
-------------
Should exist purely to manage the Bayeux kernel instance. Likely lives as long
as Art does, in which case just needs construction.

LibraryQueryService
---------------------
Simple wrapper of a library_info object, but, fundamentally both of these are
supplied by the datatools::kernel. All identified use cases require and use
this global library_info object.

So, this doesn't really exist in FalaiseArt, but Falaise does add its resource
path to this, and the associated urn_db/resolver service(s) to the urn_query_service.
The URNs require the libinfo to be present, because they use its functionality.

Treat this as a "ResourceService", that

- depends on KernelService
- Registers "some path" for looking up resources
- Adds the URN services to the kernel

Or split these into:

- ResourceService: just serves "@falaise" path(s), depends on KernelService
- URNService: just adds the db/path resolvers to the kernel, depends on ResourceService

First is simpler, closer to what we do already. Latter more decoupled.
Both illustrate coupling between URN and Resource/MountPoint system. Former
relies on resolving paths based on Mount Points, which much be present.
Might also have a URNService with different reolvers, but that's an
implementation detail (so maybe a service providing an interface).

For now, just add as a ResourceService. Simple, and can provide iface to
underlying kernel service(s). Will depend on how we split up resource files.
All under "@falaise" or more? Bayeux's kernel doesn't, like Qt's resources,
put a search path behind this. It's a single one at setup time. The
library_info can be provided with an env var that overrides that path,
but it's a full replace not prepend/append.

