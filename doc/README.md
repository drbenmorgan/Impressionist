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
