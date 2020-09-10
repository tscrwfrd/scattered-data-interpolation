# scattered-data-interpolation

This is a replica of Scipy's griddata implementation.

The scatttered-data-interpolation (SDI) is current work that provides 
the same functionality of Scipy's interpolation functionality. Current 
devlopement efforts is only on linux and tested with gcc 4.8.5.  
**THIS IS A WORK IN PROGRESS.**

## Installation prerequisites

The following is necessary to use SDI

- [GNU Make][gmake] 

- [GCC compiler][gcc]

- [QHull][qhull]


[gmake]: https://www.gnu.org/software/make/
[gcc]: https://gcc.gnu.org/
[qhull]: http://www.qhull.org/

For QHull, run basic generic makefile with default settings.  Once compiled, SDI needs to know how to include "qhull_ra.h"
and link .so files libqhullstatic_r, libqhullstatic, and qhull_r.

## Future work
- Different grid interpolation procedures
- 3D convex hulls and interpolation procedures
- possible optimization applications
- Unit test cases

