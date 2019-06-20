Reading Falaise/FLSimulate Output Data into Art
===============================================
FLSimulate outputs its data into "BRIO" format files. These are ROOT
files which put data into a set of TTrees (or "stores" in BRIO-speak).
Each TTree has a single branch that stores an opaque buffer
whose content is a binary stream of data for the stored type as
serialized by the Bayeux+Boost serialization system.

FLSimulate writes two stores:

- "GI" (**G**eneral **I**nformation)
  - Stored type: [`datatools::properties`](https://supernemo.org/Bayeux/classdatatools_1_1properties.html)
  - Number of entries: free
  - Intended that each entry is reconstituted and added to a [`datatools::multi_properties`](https://supernemo.org/Bayeux/classdatatools_1_1multi__properties.html) instance
  - Data in this instance describes the simulation setup
    - Exact entries to be documented...
- "ER" (**E**vent **R**ecord)
  - Stored type: [`datatools::things`](https://supernemo.org/Bayeux/classdatatools_1_1properties.html)
  - Number of entries: same as number generated
  - `datatools::things` maps a string identifier to a data product instance, and as output by `flsimulate` holds two products:
    - "EH": an instance of [`datatools::event_header`]()
    - "SD": an instance of [`mctools::simulated_data`]()
      - In turn, this holds:
        - An instance of [`geomtools::vector_3d`]() for the truth vertex
        - An instance of [`genbb::primary_event`]() for the truth primary particles
        - Collections of [`mctools::step_hits`]() for each of the tracker, main calorimeter, xwall calorimeter, and muon veto



