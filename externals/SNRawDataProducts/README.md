# SNRawDataProducts
Prototype C/C++ Raw Data Model with Boost/Brio/ROOT I/O for SuperNEMO
Commissioning and Production.

Modularized from `SNFrontEndElectronics` project to study the requirements
outlined by Analysis Board and tracker commissioning tasks:

- Minimize coupling between the "Online" (DAQ/Electronics/Control) and
"Offline" (Data Management, Processing, Calibration, Reconstruction, and Analysis)
parts of the experiment.
  - At present, all Offline work requires use of, and linkage to, the full
    `SNFrontEndElectronics` project. This contains very low level Online
    electronic/control code, *and* the high level Raw Data Model.
- Provide clear Raw Data Model with tools for initial exploratory analysis
- Allow Raw Data Model to be used in the official "Falaise" data processing
  system.

This prototype demonstrates that it should be possible to separate the Data
Model and I/O out of `SNFrontEndElectronics` into a lightweight Data/I/O only
interface. Both `SNFrontEndElectronics` and `Falaise` could link to this library
without needing to know any details about each other. Prototype converters
are implemented to demonstrate conversion from raw binary data to a ROOT TTree holding
the Raw Data Model C++ classes directly (for fast interactive analysis), and
preliminary conversion to the BRIO (Storage of Boost binary data in ROOT) format
currently used for input to the "Falaise" data processing system.

# Quickstart
We assume you have a [standard install of the SuperNEMO software](https://github.com/SuperNEMO-DBD/brew).
To get the code and build the library and applications:

``` console
$ brew snemo-shell
...
snemo-shell> git clone https://github.com/drbenmorgan/SNRawDataProducts.git
snemo-shell> cd SNRawDataProducts
snemo-shell> mkdir build && cd build
snemo-shell> cmake ..
...
snemo-shell> make
...
```

If you encounter errors at the `cmake` or `make` steps, [raise an Issue](https://github.com/drbenmorgan/SNRawDataProducts.git/issues/new).
On a successful build, you will have the following library and applications in the
directory where you ran `cmake/make`:

- `libSNemoRawData.{so,dylib}`
  - C++ library of Raw Data Classes, plus Boost/ROOT dictionaries
- `crd2rhd`
  - Converts `CRD` raw data streamfiles to `RHD` format streamfiles
- `rhd2rtd`
  - Merges `RHD` streamfiles into offline `RTD` format streamfile
- `rtd2root`
  - Conversion of `RTD` streamfiles to a ROOT TTree with manual
    copying of data out of `RTD` Data Model objects into arbitrary branches.
- `rtd2asroot`
  - Conversion of `RTD` streamfiles to a ROOT TTree with the `RTD`
    Data Model objects stored directly in an "RTD" branch via
    ROOT dictionaries.
- `rtd2brio`
  - Conversion of `RTD` streamfile to the Brio format understood by the
    [Falaise](https://github.com/SuperNEMO-DBD/Falaise) offline software
    (*NB: currently lacks needed Metadata because `RTD` streamfiles do
    not yet contain this, or provide a way to obtain it*).

As this project is experimental, it should not be installed, but
all programs, plus interactive ROOT usage, can be run from the directory
holding the above programs.

# Using RTD Files for Commissioning Analysis/Production Processing
The top level "Offline" Data Model class is [`RRawTriggerData`](snfee/data/RRawTriggerData.h).
Each instance in any of the `RTD` files represents all data
recorded by the DAQ in a single Trigger. At present, the `RRawTriggerData`
class is composed of:

- An integer "RunID"
- An integer "TriggerID"
- A single [`trigger_record`](snfee/data/trigger_record.h) instance
- A vector of [`calo_hit_record`](snfee/data/calo_hit_record.h) instances
- A vector of [`tracker_hit_record`](snfee/data/tracker_hit_record.h) instances

See the links above for details of these classes (the code is the documentation at this point
in time). Given an `RTD` file in the current low level binary `.data.gz` format,
these may be read and `RRawTriggerData` objects obtained in the following
ways:

1. Convert an `RTD` file to BRIO and access in `flreconstruct`

   ``` console
   snemo-shell> ./rtd2brio -i /sps/nemo/snemo/snemo_data/raw_data/RTD/run_102/snemo_run-102_rtd.data.gz -o run_102.brio
   snemo-shell> flreconstruct -i run_102.brio
   ... fails for now ...
   ```

   The failure is due to a lack of Metadata, so need work and discussion
   to find out where this will come from. Note that BRIO files are
   just ROOT files holding two TTree "stores". They are not directly readable
   however, as the branches of these TTrees hold opaque buffers of
   `Boost.Serialization` binary data.

2. Convert an `RTD` file to a properly structured ROOT TFile/TTree and access in `root`

   ``` console
   snemo-shell> ./rtd2asroot -i /sps/nemo/snemo/snemo_data/raw_data/RTD/run_102/snemo_run-102_rtd.data.gz -o run_102.root
   ...
   snemo-shell> root setupSNemoRawDataObj
   ... will see Cling errors here, but can be ignored for now...
   root [0] TFile f("run_102.root")
   root [1] f.ls()
   TFile**		run_102.root
   TFile*		run_102.root
   KEY: TTree	RawTriggerData;13	SuperNEMO Raw Trigger Data
   KEY: TTree	RawTriggerData;12	SuperNEMO Raw Trigger Data
   ```

   Note that there are two namecycles here due to ROOT's checkpoiting to prevent data loss.
   No excess data is stored, and use of the `RawTriggerData` TTree is transparent:

   ```console
   root [2] TTree* t = (TTree*)f.Get("RawTriggerData")
   (TTree *) 0x377ff80
   root [3] t->GetEntries()
   (long long) 282801
   root [4] t->Print()
   ... long list of branches, their types, and stats
   ```

   You can also use `TBrowser` to open and explore the file interactively.

   The `RawTriggerData` `TTree` has a single master branch that stores `RRawTriggerData`
   instances in fully split mode. That means you can direct access the data
   you are interested in interactively, or through the [standard ROOT TTree/TBranch
   mechanisms](https://root.cern.ch/root/htmldoc/guides/users-guide/ROOTUsersGuide.html#trees)
   and the new [RDataFrame system](https://root.cern.ch/doc/master/group__tutorial__dataframe.html).
   For example, to read all RTD data in a ROOT script:

   ```c++
   #include "snfee/data/RRawTriggerData.h"

   void exampleRTDRead(const char* rtdFilename)
   {
     TFile rtdFile{rtdFilename};
     TTree* rtdTree = (TTree*)rtdFile.Get("RawTriggerData");
     auto rtdData = new snfee::data::RRawTriggerData{};
     rtdTree->SetBranchAddress("RTD",&rtdData);

     Long64_t nRTD = rtdTree->GetEntries();
     for(Long64_t i{0}; i < nRTD; ++i) {
       rtdTree->GetEntry(i);

       // Do what you need with rtdData instance
     }
   }
   ```

   then run it in `root`:

   ```console
   snemo-shell> root setupSNemoRawDataObj
   ... will see Cling errors here, but can be ignored for now...
   root [0] .L exampleRTDRead.C
   ... ignore Cling errors ...
   root [1] exampleRTDRead("/path/to/your/rtdfile.root")
   ```

   *One thing to note here is that in contrast to BRIO files, we **can**
   store all the Data Model classes without any need for `Boost.Serialization`.
   This could allow "Falaise" to use native ROOT I/O for
   improved usability and performance.*


3. In custom C++ programs:

   ```C++
   #include <snfee/io/multifile_data_reader.h>
   #include <snfee/data/rtdReformater.h>
   #include <snfee/data/raw_trigger_data.h>
   #include <snfee/data/RRawTriggerData.h>

   int main()
   {
     using multifile_data_reader = snfee::io::multifile_data_reader;
     using raw_trigger_data = snfee::data::raw_trigger_data;
     using RRawTriggerData = snfee::data::RRawTriggerData;

     multifile_data_reader::config_type cfg{{"somertdfile.data.gz"}};
     multifile_data_reader rtdReader{cfg};

    raw_trigger_data onlineRTD{};

     while(rtdReader.has_record_tag() && rtdReader.record_tag_is(raw_trigger_data::SERIAL_TAG)) {
       rtdReader.load(onlineRTD);
       RRawTriggerData offlineRTD = snfee::data::rtdOnlineToOffline(onlineRTD);

       // Do what you need with offlineRTD instance...
     }

     return 0;
   }
   ```

   Just compile and link to the `SNemoRawDataObj` library (Full support to be added, but
   see [examples/rtd2asroot/CMakeLists.txt](examples/rtd2asroot/CMakeLists.txt) for an
   example).

# Compatibility with existing `SNFrontEndElectronics` code/files
- The code under `snfee` was extracted from the copy of "SNFrontEndElectronics"
  distributed via CC-IN2P3 to SuperNEMO members.
- The `vendor_snfee` directory contains this code unmodified, extracted such that
  it holds all code from `snfee/{data,io}` plus any files needed to close
  `#include` dependencies.
- This project's copy of this code under `snfee/{data,io,model}` is modified
  as follows:
  - Dependency on `SNCabling` in `snfee/data/channel_id.h` commented out
    as we do not yet have this code, and it is not yet clear if it bridges
    to the "offline" side of the experiment.
    - This code is ultimately a copy conversion, so could be handled via a free
      function, or simpler intermediate object like a `std::tuple`.
  - Code seemingly relating to *analysis* or *presentation* is moved to
    `not_data` directory.
    - Similarly anything that appears unrelated to actual raw data structures
  - Code relating to specific I/O conversions like CRD2RHD or
    RHD2RTD is moved into directories for these applications.
    - See `programs/{crd2rhd,rhd2rtd,rtd2root}`

