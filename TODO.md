To-Do List
==========

- Add more verification experiments, including maybe:
  - atm-only
  - atm-clm
  - atm-clm-tem-gsolv
  - atm-clm-tem-chem (and eventually w/ 3D ocean)
  …only the last is set up at present.

- Implement automatic testing (e.g. with http://hudson-ci.org ) to see if
  check-ins break code.

- Populate data/ with helper/non-run-specific data files, or provide a tarball
  at a standard location and a script to pull them down.

- Documentation:
  - ./data/ files: purpose, sources, format
  - ./tests/run_fullchem: line-by-line explanation of name.dat, tem.go,
    eppaemis.dat.

- Add the MITgcm 3D-ocean to source control and the build system.
