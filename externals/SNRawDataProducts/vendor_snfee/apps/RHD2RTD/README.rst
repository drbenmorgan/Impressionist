===========================================
The ``snfee-rhd2rtd`` program
===========================================

:Author: F.Mauger <mauger@lpccaen.in2p3.fr>, J.Hommet <hommet@lpccaen.in2p3.fr>
:Date: 2018-11-29

The ``snfee-rhd2rtd``  program converts  the *raw hit  data*
(``RHD``) from the SuperNEMO data acquisition program to the  SuperNEMO
data format  for  *raw trigger  data*
(``RTD``) as defined in the SNFrontEndElectronics_ library.

.. _SNFrontEndElectronics: https://gitlab.in2p3.fr/SuperNEMO-DBD/SNFrontEndElectronics

.. contents::

-------

The program
===========

The ``snfee-rhd2rtd`` reads  a bunch of files  storing ``RHD`` records
associated to a same run. The ``RHD`` records are packed according to their trigger identifier
into ``RTD`` records. The ``RTD``are finaly stored in output
files.  The I/O system use  the Falaise serialization system (based on
Bayeux/Boost serialization libraries).

The input  and output files must  use one of the  available formats in
Bayeux, automatically selected from the file extension:

* ``.xml``, ``.xml.gz``,  ``.xml.bz2`` : plain or  compressed XML text
  format,
* ``.txt``, ``.txt.gz``, ``.txt.bz2`` : plain or compressed ASCII text
  format,
* ``.data``,  ``.data.gz``,   ``.data.bz2``  :  plain   or  compressed
  portable binary format.

Usage
-----

Online help can be printed with:

.. code:: bash

   $ snfee-rhd2rtd --help
..

Example
-------

Processing a data file from a calorimeter crate
-----------------------------------------------

TBD
