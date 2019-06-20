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
Next...

