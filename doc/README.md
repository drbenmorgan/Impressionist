Using Impressionist
===================
The _art_ Framework
-------------------
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
data. We'll start by looking at the basic use of `art` and its scripting
language before looking at how to implement our own modules.


