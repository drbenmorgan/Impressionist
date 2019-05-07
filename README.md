Impressionist
=============

Development and testing of an [Art](https://art.fnal.gov)-based event
processing pipeline for SuperNEMO.

Quickstart
==========
You will need a CentOS7 system with
[CVMFS](https://cernvm.cern.ch/portal/filesystem) and the `larsoft.opensciencegrid.org` repository enabled. To check availability:

```console
$ ls /cvmfs/larsoft.opensciencegrid.org
new_repository products
```

Distributions of Art require setup and use through FermiLab's [UPS](https://cdcvs.fnal.goc/redmine/projects/ups/wiki) configuration management tool.
This is similar to Environment Modules, albeit with a few specific
differences. To start using the Art development kit, we first setup ups:

```console
$ source /cvmfs/larsoft.opensciencegrid.org/products/setups
```



