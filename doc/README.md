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
`FHICL_FILE_PATH`. Impressionist follows standard _art_ practice for compiling/testing projects by setting `FHICL_FILE_PATH` 
appropriately so that FHiCL scripts can be found reliably. As we'll see later, the use of `FHICL_FILE_PATH` also enables the
_composability_ of FHiCL scripts.


