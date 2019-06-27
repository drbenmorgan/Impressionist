===========================================
The ``snfee-rtd2root`` program
===========================================

:Author: F.Mauger <mauger@lpccaen.in2p3.fr>, J.Hommet <hommet@lpccaen.in2p3.fr>
:Date: 2018-12-03

The ``snfee-rtd2root``  program converts  the *raw trigger  data*
(``RTD``) from the SuperNEMO data acquisition program to ROOT format for fast
data analysis and displat.

.. _SNFrontEndElectronics: https://gitlab.in2p3.fr/SuperNEMO-DBD/SNFrontEndElectronics

.. contents::

-------

The program
===========

The ``snfee-rhd2root`` reads  a bunch of files  storing ``RTD`` records
associated to a same run. 

The input files must  use one of the  available formats in
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

   $ snfee-rtd2root --help
..

Example
-------

TBD
