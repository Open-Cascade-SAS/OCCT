/*
                     Copyright (C) 1991,1995 by

                      MATRA DATAVISION, FRANCE

This software is furnished in accordance with the terms and conditions
of the contract and with the inclusion of the above copyright notice
This software or any other copy thereof may not be provided or otherwise
be made available to any other person. No title to an ownership of the
software is hereby transferred

At the termination of the contract, the software and all copies of this
software must be deleted

Facility : CAS-CADE V1.3A

*/

#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

#ifdef WNT

# ifndef __MATH_WNT_H
#  define __MATH_WNT_H

__Standard_API double __cdecl acosh ( double );
__Standard_API double __cdecl asinh ( double );
__Standard_API double __cdecl atanh ( double );

# define M_SQRT1_2  7.0710678118654752440E-1
# define M_PI_2     1.57079632679489661923

# endif  /* __MATH_WNT_H */

#endif  /* WNT */

