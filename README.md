MIT Integrated Global System Model (IGSM)
=========================================

This file contains some information on building the IGSM from source. For more
on the contents of the source tree, see the commit history using `git log`,
or at http://github.com/mit-jp/igsm.


Contents
--------
* ``data/`` Common data files (grids, etc.) for running the model.
  * ``clm/`` Data for the [Community Land Model (CLM)][CLM].
  * ``data/`` Data for the atmospheric chemistry module (chem).
  * ``emimed/`` Predicted emissions data.
  * ``init/`` General initialization data.
    * ``46lat/`` Data specific to a 46-latitude-band grid.
    * ``forcing/`` Forcing data.
  * ``tem/`` Data for the [Terrestrial Ecosystem Model (TEM)][TEM].
* ``src/`` Source code for the modules of the model, including…
  * ``atm/`` atmosphere model.
  * ``chem/`` atmospheric chemistry model.
  * ``clm/`` CLM version 3.5.
  * ``inc/`` INClude files common to all modules.
  * ``meta/`` urban METAmodel.
  * ``ocn_3d/`` the [MIT General Circulation Model][MITgcm]
  * ``ocn_ml/`` two-dimensional, Multi-Layer (ML) OCeaN model.
  * ``ocm/`` Ocean Carbon Model.
  * ``tem/`` TEM version 4.4c.
* ``util/`` Utilities.
  * ``analysis/`` Extract data from output files.
  * ``eppa/`` Preprocessor for emissions data from the [MIT Economic Projection
    & Policy Analysis (EPPA) model, version 5][EPPA5].
* ``tests/`` Example input files for common use cases.
  * ``run_fullchem/`` 1990–2100 predicted-emissions case, full model, 2D ocean.


Build
-----
The following process has only been tested on svante.mit.edu — a Fedora Core 14
x86_64 system with [GNU Make 3.82][gmake]; [PGI][PGI] 9.0.4–13.8; and
[NetCDF][NetCDF] 3.6.2–4.2.

1. If building directly from version control, in the root directory type:

        $ autoreconf --install

2. Load the PGI compilers and the NetCDF libraries:

        $ module load pgi netcdf

3. In the root directory, type:

        $ cd build
        $ ../configure
        $ make

   An executable named `igsm22` is produced.

Some basic documentation of options (for example, choosing between historical or
predictive models) can be obtained by typing `../configure --help`; these should
be given when invoking configure in step 3.

Multiple builds can be produced from the same source code in this way, for
example in directories named `build1`, `build2` (or something more informative).
These directories may even be *outside* the directory containing this README
file; this would require giving a relative path when invoking configure (e.g.
`../igsm/configure` or similar). To see what options were passed to configure,
see the file `config.log` in any build directory.


Execute
-------

**N.B.** Before running any of the experiments in the `tests/` directory, copy
the entire directory out of the source tree into a separate location. This
avoids confusion when the experiments alter the files or create new ones, or
updates to the source also change the experiment input files.

### Preprocessing

The `emiprep` binary reads a file called `eppa5chm.put` from the current
directory and generates several files that are read by the IGSM. `eppa5chm.put`
can be obtained by copying the `chm.put` file produced by EPPA.

Only the EPPA 5 preprocessor is currently included. It is uncertain whether this
preprocessor also works for output from EPPA 4 or EPPA 6.

### Running

TODO add remaining instructions

[CLM]: http://www.cgd.ucar.edu/tss/clm
[TEM]: http://ecosystems.mbl.edu/TEM/
[MITgcm]: http://mitgcm.org/
[gmake]: http://www.gnu.org/software/make/
[PGI]: http://www.pgroup.com/
[NetCDF]: http://www.unidata.ucar.edu/software/netcdf/
[EPPA5]: http:/github.com/mit-jp/eppa5