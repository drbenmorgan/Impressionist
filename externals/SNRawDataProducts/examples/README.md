# SNemoRawDataObj Examples

At present, this is just a playground for improved ROOT I/O programs
and scripts.

# `rtd2asroot`

Enhancement to `rtd2root` to use rootcling dictionaries of the main RTD
types. Will just have one branch for each of the data members in the current
`raw_trigger_data` implementation. Use direct branches because `raw_trigger_data`
holds several `std::shared_ptr` instances, which we probably can't/shouldn't
serialize in ROOT.

# TODO
Basic ROOT script to access branches/run/plot etc.

