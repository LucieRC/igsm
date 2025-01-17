MIT Integrated Global System Model (IGSM)
=========================================

This file contains some information for those making modifications to the IGSM
code or the build system. Users interested in simply compiling and running a
common version of the model should consult README.


Optional features
-----------------
The build system does not provide an interface to all features of the IGSM. In
particular, there are some preprocessor symbols that may be defined manually. To
do this, there are three options, in order of increasing permanence:

 1. In the build directory, edit the file `ctrparam.h`, adding custom #define
    statements where indicated. This must be repeated (or the file restored from
    a backup copy) every time the build is reconfigured.
 2. Edit the file `ctrparam.h.in`, adding custom #define statements at the
    bottom of the file, where indicated. Reconfigure the build. This must be
    repeated (or the file restored from a backup copy) every time auto(re)conf
    is run, and will affect all builds configured from the same script.
 3. Edit the file `configure.ac` near the comment "Unusual defines". Move the
    AC_DEFINE() calls between the "always ON" and "always OFF" sections as
    necessary. Run autoreconf and then reconfigure the build. This will also
    affect all builds configured from the same script.

For a list of available symbols, see configure.ac. For information on how to use
these, consult Jeff Scott <jscott@MIT.EDU> or Andrei Sokolov <sokolov@MIT.EDU>.
No other documentation is currently available.


Source overrides
----------------
The build system performs a "VPATH build", which allows selective replacement of
source code.

When configure is invoked in the build directory, a directory structure ("tree")
which mirrors `src/` is created, containing Makefiles but no source files. When
the build is started (with 'make'), the following occurs for each source file:

1. A file is sought in the build tree. If found, it is compiled.
2. A file is sought *outside* of the build tree, in the corresponding
   subdirectory of `src/`.

Thus (for example) in order to override the source file `src/atm/example.F`,
place a file named `example.F` (with changes) in the directory `build/src/atm`.

There are some exceptions/limitations to this system:

- MITgcm (`src/ocn_3d`) has its own build system. This works almost the same as
  a VPATH build, but it creates symlinks from the build tree to the source
  tree.

  To modify the MITgcm source, place a source file named `example.F` (with
  changes) in `build/src/ocn_3d`. The MITgcm build system will *not* create a
  symlink to `src/ocn_3d/example.F`, but will instead use the file in the build
  tree.

  Care must be taken not to accidentally edit the source tree by editing files
  which are actually symlinks. One way to do this is to execute `make clean` in
  `build/src/ocn_3d` directory, which will remove symlinks, but not user
  overrides.

- CLM (`src/clm`) has its own build system. When configure is invoked, the CLM
  source is copied into the build directory (`src/clm/` is copied to
  `build/src/clm`) and the IGSM build invokes the standard CLM build.

  To modify the source of CLM, place a file named `example.F90` in the
  directory `build/src/clm/bld/usr.src`. The CLM build system will use this
  file instead of the corresponding file in `src/clm/src`.

- It will not automatically detect *new* source files. In order to do this,
  please read through "Adding new sources" below.


Build system
------------
The build system uses the GNU Autotools suite: autoconf, automake, autoheader,
etc. Complete information on these can be found in their respective manuals:
- http://www.gnu.org/software/autoconf/manual/autoconf.html
- http://www.gnu.org/software/automake/manual/automake.html

In particular these should be consulted for the semantics of specific macros
like AC_*, AH_*, and AM_*, see the documentation for autoconf and automake.

A helpful diagram about the operation of Autotools is available at
[Wikipedia](https://en.wikipedia.org/wiki/GNU_build_system#Components).

Some key files are:

- `Makefile.am`, `Makefile.in`: these exist in all source directories.
  `Makefile.am` is preprocessed by auto(re)conf into files like `Makefile.in`.
  When the build is configured, actual `Makefile`s are produced from
  `Makefile.in` and placed in the build directory.
- `src/inc/ctrparam.h.in`: a template header file that is used to produce
  `ctrparam.h` when the build is configured. Unlike the `Makefile.in`, there is
  no `ctrparam.h.am`; instead, `ctrparam.h.in` is produced via autoheader
  inspecting the contents of `configure.ac`.
- `configure.ac`: this source file is processed into `configure` by
  auto(re)conf.

Some simple tasks are detailed below.

### Adding new sources

Automake requires an explicit listing of the source files to be compiled and
linked into each executable or library. For an explanation of why, see
http://www.gnu.org/software/automake/manual/automake.html#Wildcards.

For most IGSM modules, the files are listed, one per line, in `Makefile.am` in
the module directory. Like so:

    libatm_a_SOURCES = atmosphere.F \
            bgrghg.F \
            check_settings.F \
            chemglobal.F \
            (more files)

When a new source file is added, it must also be added to `Makefile.am`. For
easy reading, the lists are in alphabetical order. After this is done,
autoreconf must be run in the top-level directory to regenerate `Makefile.in`,
and any build(s) must be reconfigured with the new `configure` script.


Using Git
---------
If you make improvements to the IGSM code, you may wish to commit them to the
git repository so that others may to use them. To do this, you will need contact
Jeff Scott <jscott@MIT.EDU> to obtain commit access, and read the git
documentation (type `man git`, or read the free [Pro Git
book](http://git-scm.com/book/en/v2)).
