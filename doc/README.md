Using Impressionist
===================
Impressionist is built on FNAL's [`art` event processing framework](https://art.fnal.gov) used by Energy Frontier and Low Background experiments.
Much like _Falaise_, it processes event-based data by feeding events through
a sequence of _modules_ whose order and configuration is defined by the
user in a _pipeline script_. It is of interest to SuperNEMO because
it provides many features **not** supported by _Falaise_, namely:

- Composition of pipeline scripts to modularize workflows
- Experiment-defined data models, with ROOT persistency
- Management and storage of ancillary data such as geometry descriptions and calibration data
- Tracking of the _provenance_ of data as it is processed and later accessed.

The primary point of contact for users is the `art` executable which, like
`flreconstruct`, is run by supplying input/output source(s)/sink(s) for
data together which a script defining the workflow to be performed on that
data. 

Though the sections below are a largely self-contained introduction to _art_, they 
should be consulted together with the main _art_ framework and scripting 
documentation available at:

- [Overall Art Guide](https://cdcvs.fnal.gov/redmine/projects/art/wiki)
- [FHiCL Language Syntax and Semantics](https://cdcvs.fnal.gov/redmine/documents/327)
- [FHiCL C++ Binding](https://cdcvs.fnal.gov/redmine/projects/fhicl-cpp/wiki)
- [Art Framework FHiCL Grammar and Parameters](https://cdcvs.fnal.gov/redmine/projects/art/wiki/ART_framework_parameters)

We'll start by looking at the basic use of `art` and its pipeline scripting
language before looking at how to implement our own modules.

Getting Started
===============
In the following we'll assume you've setup and built Impressionist following the
guide on the main [README](../README.md). That setup sequence ensures that you
have all the tools needed, but to check that's the case, try running `art` as
follows:

```console
$ art --version
art 3.02.04
$
```

If `art <VERSIONNUMBER>` is printed, you're good to go. If there are any issues here, 
please raise an [Issue](https://github.com/SuperNEMO-DBD/Impressionist/issues)
or drop @drbenmorgan a mail.

The Art Command Line Interface
==============================
Basic Usage
-----------
Like all console programs, `art` provides several command line arguments
to control high level behaviour. These can be listed via:

```console
$ art --help

  Usage: art <-c <config-file>> <other-options> [<source-file>]+

  Basic options:
    -h [ --help ]                         produce help message
...
$
```

The most important ones to look at are the first three sections on `Basic options:`,
`Source options:`, and `Output options:`. Later sections are for more advanced
processing and debugging work. Our basic workflow with Art will thus very similar
to Falaise, looking something like:

```console
$ art [--source <inputfile>] -c PipelineScript.fcl [--output <outputfile>]
```

The source and output files are optional, because as we'll see we can specify these
in the pipeline script.

Just like Falaise, Art's functionality is implemented as a pipeline whose input,
processing, and output are controlled through a sequence of modules defined by
the FHiCL script. Art also implements the concept of "Services" that provide information
that is not strictly Event or Run level (for example, logging, profiling). These too 
can be configured by the FHiCL script.

Getting Help on Modules and Services
------------------------------------
Art provides several builtin modules/services for non-experiment specific
tasks. We can find out what modules and services are known to Art, later on
including those for SuperNEMO, by using the `--print-available <type>` argument. Here,
`<type>` refers to the type of task performed, the key types being:

- `source`: A module that creates events and feeds them into the pipeline,
  for example from a file.
- `module`: A module that handles processing of event data.
- `service`: A service living outside the pipeline and which may be accessed
  by `source` and `module` modules.
  
As of `art` 3.2.4, each of these prints the following:

```console
$ art --print-available source

===========================================================================================================================================
   module_type                 Provider    Source location
-------------------------------------------------------------------------------------------------------------------------------------------
1. EmptyEvent                  art         / [ external source ] /art/Framework/Modules/EmptyEvent_source.cc
2. EventProcessorTestSource    art         / [ external source ] /art/test/Framework/EventProcessor/EventProcessorTestSource_source.cc
3. RootInput                   art         / [ external source ] /art/root/io/RootInput_source.cc
4. SamplingInput               art         / [ external source ] /art/root/io/SamplingInput_source.cc
===========================================================================================================================================

$ art --print-available module

================================================================================================================================================
   module_type                 Type             Source location
------------------------------------------------------------------------------------------------------------------------------------------------
1. BlockingPrescaler           filter           / [ external source ] /art/Framework/Modules/BlockingPrescaler_module.cc
2. DataFlowDumper              output module    / [ external source ] /art/Framework/Modules/DataFlowDumper_module.cc
3. EventIDFilter               filter           / [ external source ] /art/Framework/Modules/EventIDFilter_module.cc
4. EventProcessorTestOutput    output module    / [ external source ] /art/test/Framework/EventProcessor/EventProcessorTestOutput_module.cc
5. FileDumperOutput            output module    / [ external source ] /art/Framework/Modules/FileDumperOutput_module.cc
6. Prescaler                   filter           / [ external source ] /art/Framework/Modules/Prescaler_module.cc
7. ProvenanceCheckerOutput     output module    / [ external source ] /art/Framework/Modules/ProvenanceCheckerOutput_module.cc
8. RandomNumberSaver           producer         / [ external source ] /art/Framework/Modules/RandomNumberSaver_module.cc
9. RootOutput                  output module    / [ external source ] /art/root/io/RootOutput_module.cc
================================================================================================================================================

$ art --print-available service

================================================================================================================================================
   module_type                 Type             Source location
------------------------------------------------------------------------------------------------------------------------------------------------
1. BlockingPrescaler           filter           / [ external source ] /art/Framework/Modules/BlockingPrescaler_module.cc
2. DataFlowDumper              output module    / [ external source ] /art/Framework/Modules/DataFlowDumper_module.cc
3. EventIDFilter               filter           / [ external source ] /art/Framework/Modules/EventIDFilter_module.cc
4. EventProcessorTestOutput    output module    / [ external source ] /art/test/Framework/EventProcessor/EventProcessorTestOutput_module.cc
5. FileDumperOutput            output module    / [ external source ] /art/Framework/Modules/FileDumperOutput_module.cc
6. Prescaler                   filter           / [ external source ] /art/Framework/Modules/Prescaler_module.cc
7. ProvenanceCheckerOutput     output module    / [ external source ] /art/Framework/Modules/ProvenanceCheckerOutput_module.cc
8. RandomNumberSaver           producer         / [ external source ] /art/Framework/Modules/RandomNumberSaver_module.cc
9. RootOutput                  output module    / [ external source ] /art/root/io/RootOutput_module.cc
================================================================================================================================================

$
```

We'll see in the coming sections how to use these in a pipeline script. Art's
scripting system provides descriptions and help on the configuration parameters 
available or required by a given module or service. For example, one of the first modules we'll
use is the `EmptyEvent` source module, and we can see how to configure this
using:

```console
$ art --print-description EmptyEvent

====================================================================================================

    module_type: EmptyEvent (or "art/Framework/Modules/EmptyEvent")

        provider: art
        source  : / [ external source ] /art/Framework/Modules/EmptyEvent_source.cc
        library : /cvmfs/larsoft.opensciencegrid.org/products/art/v3_02_04/slf7.x86_64.e17.debug/lib/libart_Framework_Modules_EmptyEvent_source.so

    Allowed configuration
    ---------------------

        ## Any parameters prefaced with '#' are optional.

        source: {

           module_type: EmptyEvent

           maxEvents: -1  # default

           maxSubRuns: -1  # default

           reportFrequency: 1  # default

           errorOnFailureToPut: false  # default

           processingMode: "RunsSubRunsAndEvents"  # default

           numberEventsInRun: -1  # default

           numberEventsInSubRun: -1  # default

           eventCreationDelay: 0  # default

           resetEventOnSubRun: true  # default

         # firstRun: <unsigned int>

         # firstSubRun: <unsigned int>

         # firstEvent: <unsigned int>

           ## The 'timestampPlugin' parameter must be a FHiCL table
           ## of the form:
           ## 
           ##   timestampPlugin: {
           ##     plugin_type: <plugin specification>
           ##     ...
           ##   }
           ## 
           ## See the notes in art/Framework/Core/EmptyEventTimestampPlugin.h
           ## for more details.

         # timestampPlugin: << delegated >>
        }

====================================================================================================

$
```

This output is quite involved, so we defer explaining it until we actually write our
own scripts. Try using ``--print-description`` on some of the other modules and services.
Later on when we write our own modules, we'll see how to add this documentation
for them.


Writing Pipeline Scripts with FHiCL
===================================
Art's event processing pipeline is driven by the user through a script written in
FHiCL (**F**ermilab **Hi**erarchical **C**onfiguration **L**anguage, pronounced "fickle").
[FHiCL's grammar](https://cdcvs.fnal.gov/redmine/attachments/29136/quick_start_v3.pdf) is
similar to JSON/YAML, with the advantage over _Falaise_'s `datatools::properties` language
that it is _hierarchical_ by design, and _composable_ (an overall pipeline can be "compiled"
from several "unit" scripts).

In the following sections we'll walk through the build up of a FHiCL script for _art_, illustrating
the schema for defining services, modules, pipeline, and I/O. All of these examples can be browsed
under the [top-level fcl/examples directory](../fcl/examples).

`zero.fcl`
----------
Art implements sensible defaults for processing, and so an empty (or rather, pure whitespace/comment)
file as shown in [../fcl/examples/zero.fcl](../fcl/examples/zero.fcl) is perfectly valid.
Checking the output of `art --help`, we can see that to pass a script to `art` via the `-c` argument:

```console
$ art -c examples/zero.fcl
INFO: provided configuration file 'examples/zero.fcl' is empty: 
using minimal defaults and command-line options.
INFO: using default process_name of "DUMMY".
%MSG-i MF_INIT_OK:  Early 21-May-2019 15:33:59 BST JobSetup
Messagelogger initialization complete.
%MSG
Begin processing the 1st record. run: 1 subRun: 0 event: 1 at 21-May-2019 15:33:59 BST

TrigReport ---------- Event  Summary ------------
TrigReport Events total = 1 passed = 1 failed = 0

TimeReport ---------- Time  Summary ---[sec]----
TimeReport CPU = 0.002838 Real = 0.002903

MemReport  ---------- Memory  Summary ---[base-10 MB]----
MemReport  VmPeak = 168.993 VmHWM = 34.9389

Art has completed and will exit with status 0.
$
```

Your output will of course differ slightly depnding on time and exact system CPU. Of course,
nothing much has happened, but note how we passed the script's path to `art` via _relative path_.
Art will search for the script we supply using the following rules:

- An absolute path will be used as provided, with execution stopped if the file does not exist
- A relative path will be:
  - Looked for relative to the working directory and used if found, otherwise
  - It will be searched for under each element of the `FHICL_FILE_PATH` environment variable, with:
    - The first existing matched file being used, or
    - Execution will be stopped if no existing matched file is found

The reason that suppliying `examples/zero.fcl` as a relative path works is that is has been found through searching 
`FHICL_FILE_PATH`. Impressionist sets this as part of its `setup_for_development` script so that running during the
compile/test cycle uses an FHiCL scripts supplied by the project.  As we'll see later, the use of `FHICL_FILE_PATH` 
also enables the _composability_ of FHiCL scripts by providing a C++ header like lookup.


`first.fcl`
-----------
The input to _art_'s pipeline is defined by the `source` [table](https://cdcvs.fnal.gov/redmine/projects/art/wiki/ART_framework_parameters) in the script.
As we don't have input files or other event source yet, we can use _art_'s
builtin `EmptyEvent` module to create, well, empty events. In the `source` table,
we use the `module_type` parameter to define `EmptyEvent` as the module type,
and leave any further parameters which unset (`EmptyEvent` provides sensible defaults for us):

```
source : {
  module_type : EmptyEvent
}
```

We can now pass this script to `art` using the `-c` argument:

```console
$ art -c examples/first.fcl
INFO: using default process_name of "DUMMY".
%MSG-i MF_INIT_OK:  Early 21-May-2019 16:11:38 BST JobSetup
Messagelogger initialization complete.
%MSG
Begin processing the 1st record. run: 1 subRun: 0 event: 1 at 21-May-2019 16:11:38 BST

TrigReport ---------- Event  Summary ------------
TrigReport Events total = 1 passed = 1 failed = 0

TimeReport ---------- Time  Summary ---[sec]----
TimeReport CPU = 0.003141 Real = 0.003227

MemReport  ---------- Memory  Summary ---[base-10 MB]----
MemReport  VmPeak = 168.993 VmHWM = 34.9266

Art has completed and will exit with status 0.
$
```

Here, only one event has been generated. We can change this on the command line
using the `-n` argument. For example, to run 10 events:

``` console
$ art -c examples/first.fcl -n 10
INFO: using default process_name of "DUMMY".
%MSG-i MF_INIT_OK:  Early 21-May-2019 16:13:00 BST JobSetup
Messagelogger initialization complete.
%MSG
Begin processing the 1st record. run: 1 subRun: 0 event: 1 at 21-May-2019 16:13:00 BST
Begin processing the 2nd record. run: 1 subRun: 0 event: 2 at 21-May-2019 16:13:00 BST
Begin processing the 3rd record. run: 1 subRun: 0 event: 3 at 21-May-2019 16:13:00 BST
Begin processing the 4th record. run: 1 subRun: 0 event: 4 at 21-May-2019 16:13:00 BST
Begin processing the 5th record. run: 1 subRun: 0 event: 5 at 21-May-2019 16:13:00 BST
Begin processing the 6th record. run: 1 subRun: 0 event: 6 at 21-May-2019 16:13:00 BST
Begin processing the 7th record. run: 1 subRun: 0 event: 7 at 21-May-2019 16:13:00 BST
Begin processing the 8th record. run: 1 subRun: 0 event: 8 at 21-May-2019 16:13:00 BST
Begin processing the 9th record. run: 1 subRun: 0 event: 9 at 21-May-2019 16:13:00 BST
Begin processing the 10th record. run: 1 subRun: 0 event: 10 at 21-May-2019 16:13:00 BST

TrigReport ---------- Event  Summary ------------
TrigReport Events total = 10 passed = 10 failed = 0

TimeReport ---------- Time  Summary ---[sec]----
TimeReport CPU = 0.004014 Real = 0.004123

MemReport  ---------- Memory  Summary ---[base-10 MB]----
MemReport  VmPeak = 168.993 VmHWM = 34.9266

Art has completed and will exit with status 0.
$
```

`second.fcl`
------------

This script extends `first.fcl` to show how the number of events to process can be
set in the FHiCL script. Art modules use FHiCL's [configuration validation](https://cdcvs.fnal.gov/redmine/projects/art/wiki/Configuration_validation_and_description)
system to provide documentation of parameters supported by the module and to validate the
supplied settings. Providing the module is known to Art, we can find out how to
configure a given module with the `--print-description <modulename>` argument to `art`.
For the `EmptyEvent` module, we can do:

``` console
$ art --print-description EmptyEvent

====================================================================================================

    module_type: EmptyEvent (or "art/Framework/Modules/EmptyEvent")

        provider: art
        source  : / [ external source ] /art/Framework/Modules/EmptyEvent_source.cc
        library : /cvmfs/larsoft.opensciencegrid.org/products/art/v3_02_04/slf7.x86_64.e17.debug/lib/libart_Framework_Modules_EmptyEvent_source.so

    Allowed configuration
    ---------------------

        ## Any parameters prefaced with '#' are optional.

        source: {

           module_type: EmptyEvent

           maxEvents: -1  # default

           maxSubRuns: -1  # default

           reportFrequency: 1  # default

           errorOnFailureToPut: false  # default

           processingMode: "RunsSubRunsAndEvents"  # default

           numberEventsInRun: -1  # default

           numberEventsInSubRun: -1  # default

           eventCreationDelay: 0  # default

           resetEventOnSubRun: true  # default

         # firstRun: <unsigned int>

         # firstSubRun: <unsigned int>

         # firstEvent: <unsigned int>

           ## The 'timestampPlugin' parameter must be a FHiCL table
           ## of the form:
           ## 
           ##   timestampPlugin: {
           ##     plugin_type: <plugin specification>
           ##     ...
           ##   }
           ## 
           ## See the notes in art/Framework/Core/EmptyEventTimestampPlugin.h
           ## for more details.

         # timestampPlugin: << delegated >>
        }

====================================================================================================

$
```

Thus to set the number of events, we add the `maxEvents` parameter to the `source:` table:

```
source: {
  module_type : EmptyEvent
  maxEvents   : 10
}
```

We can run this in `art` and get 10 events generated:

```console
$ art -c examples/second.fcl
INFO: using default process_name of "DUMMY".
%MSG-i MF_INIT_OK:  Early 21-May-2019 16:19:53 BST JobSetup
Messagelogger initialization complete.
%MSG
Begin processing the 1st record. run: 1 subRun: 0 event: 1 at 21-May-2019 16:19:54 BST
Begin processing the 2nd record. run: 1 subRun: 0 event: 2 at 21-May-2019 16:19:54 BST
Begin processing the 3rd record. run: 1 subRun: 0 event: 3 at 21-May-2019 16:19:54 BST
Begin processing the 4th record. run: 1 subRun: 0 event: 4 at 21-May-2019 16:19:54 BST
Begin processing the 5th record. run: 1 subRun: 0 event: 5 at 21-May-2019 16:19:54 BST
Begin processing the 6th record. run: 1 subRun: 0 event: 6 at 21-May-2019 16:19:54 BST
Begin processing the 7th record. run: 1 subRun: 0 event: 7 at 21-May-2019 16:19:54 BST
Begin processing the 8th record. run: 1 subRun: 0 event: 8 at 21-May-2019 16:19:54 BST
Begin processing the 9th record. run: 1 subRun: 0 event: 9 at 21-May-2019 16:19:54 BST
Begin processing the 10th record. run: 1 subRun: 0 event: 10 at 21-May-2019 16:19:54 BST

TrigReport ---------- Event  Summary ------------
TrigReport Events total = 10 passed = 10 failed = 0

TimeReport ---------- Time  Summary ---[sec]----
TimeReport CPU = 0.003759 Real = 0.003851

MemReport  ---------- Memory  Summary ---[base-10 MB]----
MemReport  VmPeak = 168.997 VmHWM = 34.943

Art has completed and will exit with status 0.

$
```

You can still pass the `-n` command line argument to override the number of events.

Part of the FHiCL validation system is helpful error messaging. For example,
if we spell the name of a parameter incorrectly, `art` will point to the location
and cause of the error:

``` console
$ art -c examples/second_error.fcl
INFO: using default process_name of "DUMMY".
%MSG-i MF_INIT_OK:  Early 21-May-2019 16:21:18 BST JobSetup
Messagelogger initialization complete.
%MSG
%MSG-s ArtException:  Early 21-May-2019 16:21:18 BST JobSetup
cet::exception caught in art
---- Configuration BEGIN
  
  
  Module label: source
  module_type : EmptyEvent
  
  Any parameters prefaced with '#' are optional.
  Unsupported parameters:
  
   + maxEvent                       [ /home/physics/phsdbc/sandbox/com.github/SuperNEMO-DBD/Impressionist.build/fcl/examples/second_error.fcl:14 ]
  
---- Configuration END
%MSG
Art has completed and will exit with status 9.
$
```

Try adding some of the other parameters listed in the documentation for `EmptyEvent` and see what happens!


`third.fcl`
-----------
So far our empty events are not processed and no results are output to file.
As with the number of events, output to file can be controlled by a command
line argument or FHiCL parameter. In the former case, we use the `-o` command
line argument to `art` which defaults output to ROOT format via the `RootOutput`
module. Though art output files are ROOT format, convention is to use the `.art`
extension to distinguish them from possible analysis level files.

Using our previous `second.fcl` file, we can output the events to file via:

```console
$ art -c examples/second.fcl -o second.art
INFO: using default process_name of "DUMMY".
%MSG-i MF_INIT_OK:  Early 21-May-2019 16:31:35 BST JobSetup
Messagelogger initialization complete.
%MSG
%MSG-i FastCloning:  RootOutput:out@Construction 21-May-2019 16:31:35 BST  ModuleConstruction
Initial fast cloning configuration (from default): true
%MSG
Begin processing the 1st record. run: 1 subRun: 0 event: 1 at 21-May-2019 16:31:35 BST
21-May-2019 16:31:35 BST  Opened output file with pattern "second.art"
%MSG-w FastCloning:  PostProcessEvent 21-May-2019 16:31:35 BST  run: 1 subRun: 0 event: 1
Fast cloning deactivated for this input file due to empty event tree and/or event limits.
%MSG
Begin processing the 2nd record. run: 1 subRun: 0 event: 2 at 21-May-2019 16:31:35 BST
Begin processing the 3rd record. run: 1 subRun: 0 event: 3 at 21-May-2019 16:31:35 BST
Begin processing the 4th record. run: 1 subRun: 0 event: 4 at 21-May-2019 16:31:35 BST
Begin processing the 5th record. run: 1 subRun: 0 event: 5 at 21-May-2019 16:31:35 BST
Begin processing the 6th record. run: 1 subRun: 0 event: 6 at 21-May-2019 16:31:35 BST
Begin processing the 7th record. run: 1 subRun: 0 event: 7 at 21-May-2019 16:31:35 BST
Begin processing the 8th record. run: 1 subRun: 0 event: 8 at 21-May-2019 16:31:35 BST
Begin processing the 9th record. run: 1 subRun: 0 event: 9 at 21-May-2019 16:31:35 BST
Begin processing the 10th record. run: 1 subRun: 0 event: 10 at 21-May-2019 16:31:35 BST
21-May-2019 16:31:36 BST  Closed output file "second.art"

TrigReport ---------- Event  Summary ------------
TrigReport Events total = 10 passed = 10 failed = 0

TrigReport ------ Modules in End-Path: end_path ------------
TrigReport  Trig Bit#        Run    Success      Error Name
TrigReport     0    0         10         10          0 out

TimeReport ---------- Time  Summary ---[sec]----
TimeReport CPU = 0.279851 Real = 0.350647

MemReport  ---------- Memory  Summary ---[base-10 MB]----
MemReport  VmPeak = 526.189 VmHWM = 161.817

Art has completed and will exit with status 0.
$ file second.art
second.art: ROOT file Version 61600 (Compression: 7)
$
```

We can also define the filename in the FHiCL file using the
`outputs` table. Note the plural as unlike `source` we can, if we wish, 
have multiple output modules (for example, selected/rejected event samples). 
However, because `outputs` is part of processing because of this we must also add 
the `physics` table so we can declare the use of the output module:

```
source: {
  module_type : EmptyEvent
  maxEvents   : 10
  firstRun    :  1
}

physics: {
  op: [ myOutput ]
}

outputs: {
  myOutput : {
    module_type : RootOutput
    fileName    : "third.art"
  }
}
```

The `physics` table is where the definitions of pipeline modules, their
order, and configuration parameters are defined. In `physics` we've added
a FHiCL list `op` to define the modules to be processed, in this case the
`myOutput` module we define later in the `outputs` table.
The name `op` is arbitrary and can be anything you like, as Art's syntax
can separate these lists from other settings as we'll see later. 
This seemingly odd way of defining things will help later on when we define 
more complex processing paths and multiple outputs.
The script can be run like the others:

```console
$ art -c examples/third.fcl
INFO: using default process_name of "DUMMY".
%MSG-i MF_INIT_OK:  Early 21-May-2019 16:41:42 BST JobSetup
Messagelogger initialization complete.
%MSG
%MSG-i FastCloning:  RootOutput:myOutput@Construction 21-May-2019 16:41:43 BST  ModuleConstruction
Initial fast cloning configuration (from default): true
%MSG
Begin processing the 1st record. run: 1 subRun: 0 event: 1 at 21-May-2019 16:41:43 BST
21-May-2019 16:41:43 BST  Opened output file with pattern "third.art"
%MSG-w FastCloning:  PostProcessEvent 21-May-2019 16:41:43 BST  run: 1 subRun: 0 event: 1
Fast cloning deactivated for this input file due to empty event tree and/or event limits.
%MSG
Begin processing the 2nd record. run: 1 subRun: 0 event: 2 at 21-May-2019 16:41:43 BST
Begin processing the 3rd record. run: 1 subRun: 0 event: 3 at 21-May-2019 16:41:43 BST
Begin processing the 4th record. run: 1 subRun: 0 event: 4 at 21-May-2019 16:41:43 BST
Begin processing the 5th record. run: 1 subRun: 0 event: 5 at 21-May-2019 16:41:43 BST
Begin processing the 6th record. run: 1 subRun: 0 event: 6 at 21-May-2019 16:41:43 BST
Begin processing the 7th record. run: 1 subRun: 0 event: 7 at 21-May-2019 16:41:43 BST
Begin processing the 8th record. run: 1 subRun: 0 event: 8 at 21-May-2019 16:41:43 BST
Begin processing the 9th record. run: 1 subRun: 0 event: 9 at 21-May-2019 16:41:43 BST
Begin processing the 10th record. run: 1 subRun: 0 event: 10 at 21-May-2019 16:41:43 BST
21-May-2019 16:41:43 BST  Closed output file "third.art"

TrigReport ---------- Event  Summary ------------
TrigReport Events total = 10 passed = 10 failed = 0

TrigReport ------ Modules in End-Path: end_path ------------
TrigReport  Trig Bit#        Run    Success      Error Name
TrigReport     0    0         10         10          0 myOutput

TimeReport ---------- Time  Summary ---[sec]----
TimeReport CPU = 0.281921 Real = 0.343624

MemReport  ---------- Memory  Summary ---[base-10 MB]----
MemReport  VmPeak = 526.189 VmHWM = 161.874

Art has completed and will exit with status 0.
$ file third.art
third.art: ROOT file Version 61600 (Compression: 7)
$
```

Even with the output file name in the fcl file, we can override it from
the command line with the `-o` argument:

``` console
$ art -c examples/third.fcl -o myfile.art
...
$ file myfile.art
myfile.art: ROOT file Version 61600 (Compression: 7)
$
```

