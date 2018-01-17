CTG Exporter aims to export CTG books to a few formats.
  
  1. Export to Polyglot book format. This is the main feature.
  1. Export CTG extra book stats to leveldb format.

Usage:

  1. Run 'make'
  1. ./ctg_exporter 'examples/simple.ctg simple.bin' to test. Currently no ouput is written to simple.bin
  1. Simple CTG file with 3 book moves is in the examples folder (simple.ctg).
  
PLEASE NOTE: The code is not yet complete to export a CTG book to polyglot format. This will be done soon. Currently, only the ctg book position is printed out. 

Credits and License:
CTG parsing code was taken from https://github.com/AaronBecker/daydreamer. Code to parse CTG without using .cto and .ctb was taken from Sebastien Major. Code to output CTG to key value DBs was also derived from Sebastien Major.
