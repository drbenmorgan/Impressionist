Impressionist: Art for SuperNEMO
================================

Impressionist is a prototype of an Art-based data model and event data
processing system for SuperNEMO. In most cases, it simply provides a bridge
between the existing Falaise/Bayeux file formats, data models, and services
to their equivalents in Art.

The sections below describe the main use cases with basic demonstrations


Reading Falaise/FLSimulate Output Data into Art
===============================================
Read BRIO format data as output by [Falaise's flsimulate](https://supernemo.org/Falaise/usingflsimulate.html)
application into Art, reconstituting equivalent of "SD" data bank.
Implementation involves two pieces of code:

1. A C++ library providing the interface to the data products and
   a ROOT dictionary to enable serialization
2. An [Art Source Module](https://cdcvs.fnal.gov/redmine/projects/art/wiki/Writing_your_own_input_sources)
   to read flsimulate BRIO file and reconstitute data products into Art

Implementation details on these elements are available in [snemo/edm/README.md](../snemo/edm/README.md).

As we did for Art's own modules, we can get information about the
source module, `BrioInput`, from the command line:

```console
$ art --print-description BrioInput
====================================================================================================

    module_type: BrioInput (or "snemo/edm/BrioInput")

    provider: user
    source  : <system_specific>/Impressionist.git/snemo/edm/BrioInput_source.cc
    library : <system_specific>/Impressionist.build/lib/libsnemo_edm_BrioInput_source.so

    Allowed configuration
    ---------------------

    [ None provided ]

====================================================================================================

$
```

Since it's a very simple single file ready, it doesn't take any specialized configuration.

To use it to read an output file from `flsimulate`, `BrioInput` needs to be declared
as the `module_type` under `source` section of your art FHiCL script. using the info
printed above, we can write this as:

```
source: {
  module_type: BrioInput
}
```

or using the fully qualified name:

```
source: {
  module_type: "snemo/edm/BrioInput"
}
```

Note the quotes are needed for the fully qualified name! Write one of the above
to `brioinput_t.fcl` and run it in `art` using the `-s` flag to point to an `flsimulate`
output file:

```
$ art -s /my/flsimulate/file.brio -c brioinput_t.fcl
INFO: using default process_name of "DUMMY".
%MSG-i MF_INIT_OK:  Early 20-Jun-2019 16:14:36 BST JobSetup
Messagelogger initialization complete.
%MSG
Opening file /my/flsimulate/file.brio
New run

TrigReport ---------- Event  Summary ------------
TrigReport Events total = 100 passed = 100 failed = 0

TimeReport ---------- Time  Summary ---[sec]----
TimeReport CPU = 1.217760 Real = 1.576359

MemReport  ---------- Memory  Summary ---[base-10 MB]----
MemReport  VmPeak = 683.262 VmHWM = 218.96

Art has completed and will exit with status 0.
$
```

The number of events and timings will of course vary depending on your
file and system. To see that `art` is actually doing something, we can
use the `FileDumperOutput` output module to log information about the
data products. Change your `brioinput_t.fcl` file as follows:

```
source: {
  module_type: "BrioInput"
}

# Must have a path to output
physics: {
  outputPath: [ fileDumper ]
}

outputs: {
  fileDumper: {
    module_type: FileDumperOutput
  }
}
```

Rerun with your input file, and you should see much more output:

```console
$ art -s /my/flsimulate/file.brio -c brioinput_t.fcl
INFO: using default process_name of "DUMMY".
%MSG-i MF_INIT_OK:  Early 20-Jun-2019 16:24:03 BST JobSetup
Messagelogger initialization complete.
%MSG
Opening file /my/flsimulate/file.brio
New run
PRINCIPAL TYPE: Event
PROCESS NAME | MODULE LABEL | PRODUCT INSTANCE NAME | DATA PRODUCT TYPE.............. | PRODUCT FRIENDLY TYPE... | SIZE
DUMMY....... | SD.......... | gg................... | std::vector<snemo::BaseStepHit> | snemo::BaseStepHits..... | ..13
DUMMY....... | SD.......... | calo................. | std::vector<snemo::BaseStepHit> | snemo::BaseStepHits..... | ...1
DUMMY....... | SD.......... | gveto................ | std::vector<snemo::BaseStepHit> | snemo::BaseStepHits..... | ...0
DUMMY....... | SD.......... | ..................... | double......................... | double.................. | ...-
DUMMY....... | SD.......... | xcalo................ | std::vector<snemo::BaseStepHit> | snemo::BaseStepHits..... | ...0
DUMMY....... | SD.......... | ..................... | CLHEP::Hep3Vector.............. | CLHEP::Hep3Vector....... | ...-
DUMMY....... | SD.......... | ..................... | snemo::GenBBPrimaryEvent....... | snemo::GenBBPrimaryEvent | ...-

Total products (present, not present): 7 (7, 0).

...

PRINCIPAL TYPE: Event
PROCESS NAME | MODULE LABEL | PRODUCT INSTANCE NAME | DATA PRODUCT TYPE.............. | PRODUCT FRIENDLY TYPE... | SIZE
DUMMY....... | SD.......... | gg................... | std::vector<snemo::BaseStepHit> | snemo::BaseStepHits..... | ..39
DUMMY....... | SD.......... | calo................. | std::vector<snemo::BaseStepHit> | snemo::BaseStepHits..... | ...1
DUMMY....... | SD.......... | gveto................ | std::vector<snemo::BaseStepHit> | snemo::BaseStepHits..... | ...0
DUMMY....... | SD.......... | ..................... | double......................... | double.................. | ...-
DUMMY....... | SD.......... | xcalo................ | std::vector<snemo::BaseStepHit> | snemo::BaseStepHits..... | ...0
DUMMY....... | SD.......... | ..................... | CLHEP::Hep3Vector.............. | CLHEP::Hep3Vector....... | ...-
DUMMY....... | SD.......... | ..................... | snemo::GenBBPrimaryEvent....... | snemo::GenBBPrimaryEvent | ...-

Total products (present, not present): 7 (7, 0).

PRINCIPAL TYPE: Run
PROCESS NAME | MODULE LABEL | PRODUCT INSTANCE NAME | DATA PRODUCT TYPE..... | PRODUCT FRIENDLY TYPE. | SIZE
DUMMY....... | SD.......... | ..................... | snemo::MultiProperties | snemo::MultiProperties | ...-

Total products (present, not present): 1 (1, 0).


TrigReport ---------- Event  Summary ------------
TrigReport Events total = 100 passed = 100 failed = 0

TrigReport ------ Modules in End-Path: end_path ------------
TrigReport  Trig Bit#        Run    Success      Error Name
TrigReport     0    0        100        100          0 fileDumper

TimeReport ---------- Time  Summary ---[sec]----
TimeReport CPU = 1.280318 Real = 1.531156

MemReport  ---------- Memory  Summary ---[base-10 MB]----
MemReport  VmPeak = 686.055 VmHWM = 219.722

Art has completed and will exit with status 0.
$
```

Again, your output will differ depending on your system and file, but you
should see that we have 7 data products in each Event, plus one in the Run.

Finally, you can "convert" the BRIO files to Art's native ROOT format using
the FHiCL script:

```
source: {
  module_type: BrioInput
}
```

and running it as:

```console
$ art -s /my/flsimulate/file.brio -c brioinput_t.fcl -o file.art
...
```

The resultant Art/ROOT file can be queried using several helper
programs provided by the `art_root_io` package:

- `config_dumper`: this program will read an art/ROOT output file and print out configuration information for the process(es) that created that file.
- `file_info_dumper`: this program will read an art/ROOT output file and has the ability to print the list of events in the file, print the range of events, subruns, and runs that contributed to making the file, and provides access to the internal SQLite database, which can be saved to an external database.
- `count_events`: this program will read an art/ROOT output file and print out how many events are contained in that file.
- `product_sizes_dumper`: this program will read and art/ROOT output file and print out information about the sizes of products.
- `sam_metadata_dumper`: The sam_metadata_dumper application will read an art-ROOT format file, and extract the information for possible post-processing and upload to SAM.

See the [art suite programs](https://cdcvs.fnal.gov/redmine/projects/art/wiki) section of the art Wiki
for more information on these.


Accessing FLSimulate Data Products in Art Modules
=================================================
With data now present in the Events, downstream modules can consume
and filter on these data, or use them to produce further data products.

Consuming Data and Producing Plots with TFileService
----------------------------------------------------
The simplest model of data consumption is a so-called "Analyzer" module
which has read-only access to Run, Subrun, and Event data products.
A common use of an Analyzer module is to accumulate data into histograms
for later fitting/analysis, so they are often used with Art's `TFileService`. See the art Wiki's [Guide to writing and using services](https://cdcvs.fnal.gov/redmine/projects/art/wiki/Guide_to_writing_and_using_services) for
more details, but their use is almost identical to Bayeux's DPP services,
albeit easier! The implementation for our `ExSDAnalyzer` module
can be found in [snemo/examples/ExSDAnalyzer.cc](../snemo/examples/ExSDAnalyzer.cc), with the build declaration in [snemo/examples/CMakeLists.txt](../snemo/examples/CMakeLists.txt).

The core things to note about the implementation are:

1. Inheritance from `art::SharedAnalyzer` to support multithreading with
   `TFileService`:

   - https://cdcvs.fnal.gov/redmine/projects/art/wiki/Module_threading_types
   - https://cdcvs.fnal.gov/redmine/projects/art/wiki/TFileService#Using-TFileService-in-multi-threaded-art-jobs

2. Use of `art::InputTag` and `art::ProductToken` to declare the data
   products the module consumes and help users, and art, track and detect
   data product dependencies:

   - https://cdcvs.fnal.gov/redmine/projects/art/wiki/Declaring_products_to_consume

3. Use of `serialize` in `ExSDAnalyzer`s constructor to prevent data races:

   - https://cdcvs.fnal.gov/redmine/projects/art/wiki/TFileService#Using-TFileService-in-multi-threaded-art-jobs

4. Use of the `registerFileSwitchCallback` to gracefully allow file switching (e.g. you run over a single input file of 1000 events, but write your analysis objects to 10 files of 100 events each).

  - https://cdcvs.fnal.gov/redmine/projects/art/wiki/TFileService#Preparing-modules-for-file-switching-art-210-and-newer

5. Use of `TFileService` and `TFileDirectory` to make a directory tree
   of ROOT objects.

The module is built as part of Impressionist, and so can be queried
just as other modules:

```console
$ art --print-description ExSDAnalyzer

====================================================================================================

    module_type: ExSDAnalyzer (or "snemo/examples/ExSDAnalyzer")

        provider: user
        type    : analyzer
        source  : <system_specific>/Impressionist.git/snemo/examples/ExSDAnalyzer_module.cc
        library : <system_specific>/Impressionist.build/lib/libsnemo_examples_ExSDAnalyzer_module.so

    Allowed configuration
    ---------------------

        [ None provided ]

====================================================================================================
```

To use the module with Art, the following FHiCL script is provided
in [fcl/snemo/examples/sdanalyzer_t.fcl](../fcl/snemo/examples/sdanalyzer_t.fcl):

```
source: {
  module_type : BrioInput
}

physics: {
  # Analyzers have their own table
  analyzers: {
    myanalyzer: {
      module_type: ExSDAnalyzer
    }
  }

  # Analyzers go in an End Path
  outputPath: [ myanalyzer ]
}

# Must declare the TFileService to enable it
services: {
  TFileService: {
    # Write all output from any analyzer module here
    fileName: "SDAnalyzerTest.root"
  }
}
```

You can see that Analyzer modules are declared and configured in their
own `analyzers` table under the main processing `physics` table. Analyzers
are "event observing" (i.e. event read-only) so go on an end path,
though no entry in the `output` table is required. For more on
event processing paths in art, see the [wiki page on Paths](https://cdcvs.fnal.gov/redmine/projects/art/wiki/Paths).
Finally, to enable use of the `TFileService`, our script must add an
entry for it under the main `services` table. At minimum, we must supply
the `fileName` parameter for the ROOT file our objects will be written
to. You can run `art --print-description TFileService` to find more options.
Assuming you have an `flsimulate` BRIO file, you can run the analyzer
over it using:

```console
$ art -s /my/flsimulate/file.brio -c sdanalyzer_t.fcl
...
$ ls
... SDAnalyzerTest.root ...
```

You can open and browse the ROOT file as normal. Note the directory
created in the file includes the module label, in this case `myanalyzer`.
This means that many modules can use `TFileService`, and a nicely
arranged TFile/TDirectory structure can be created.

The file output by `TFileService` is *independent* of the primary
art output file, so you can do:

```console
$ art -s /my/flsimulate/file.brio -c sdanalyzer_t.fcl -o events.art
...
$ ls
... events.art SDAnalyzerTest.root ...
```

This can be useful for producing summary data for more detailed processing,
e.g. data quality metrics for reconstruction whilst running the
reconstruction. However, in some cases it may be more appropriate to
use the `Run` or  `SubRun` products to store such summaries so they
become part of the processing Provenance.



