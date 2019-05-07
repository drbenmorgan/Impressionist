Impressionist
=============

Development and testing of an [Art](https://art.fnal.gov)-based event
processing pipeline for SuperNEMO.

Quickstart
==========
System setup
------------
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

Building Impressionist
----------------------
At present, Impressionist is a "build only" project, i.e. it is not
designed to be installed, simply run from where we build it. To do
this, create a build directory next to your Git clone, e.g.:

```console
$ ls
Impressionist.git
$ mkdir Impressionist.build
$ cd Impressionist.build
```

The build uses FNAL's combined UPS/CMake system known as cetbuildtools. Whilst
a little more involved that pure CMake, it allows easy testing by the artists if
required (a better decoupling of these systems is under development). We start
by sourcing the [`setup_for_development`](ups/setup_for_development) script to
load the build environment:

```console
$ source ../Impressionist.git/ups/setup_for_development -d
...
Please use "buildtool" for preference (see "buildtool -h" or "buildtool --usage"), or this cmake command:
env CC=gcc CXX=g++ FC=gfortran cmake -DCMAKE_INSTALL_PREFIX="/install/path" -DCMAKE_BUILD_TYPE=${CETPKG_TYPE} "${CETPKG_SOURCE}"
OR:
buildtool -I "/install/path" -bti [-jN]
$
```

The `-d` flag tells the script we want to compile in Debug mode, which is what
we want for preliminary studies. It's cleaner just to use the `buildtool` wrapper
around `cmake/make` at this point to build and test, so we do:

```console
$ buildtool -bt
INFO: unspecified or null install prefix defaulted to /dev/null.
INFO: Install prefix = /dev/null
INFO: CETPKG_TYPE = Debug

...

------------------------------------
INFO: Stage test successful.
------------------------------------

$
```

You should see the famililar CMake/Make output and final success.
If you make changes to the code, simply rerun `buildtool -bt` to reconfigure,
recompile and retest.

