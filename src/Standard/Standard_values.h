#ifndef _Standard_values_HeaderFile
# define _Standard_values_HeaderFile
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

#if defined(_MSC_VER) && !defined(WNT)
#error "Wrong compiler options has been detected. Add /DWNT option for proper compilation!!!!!"
#endif

#ifndef WNT
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef HAVE_LIMITS
# include <limits>
#elif defined (HAVE_LIMITS_H)
# include <limits.h>
#else
#error "check config.h file or compilation options: either HAVE_LIMITS or HAVE_LIMITS_H should be defined"
#endif
#endif


#ifdef WNT
# include <limits>
#endif

#if defined (__hpux) || defined (HPUX)
# ifdef MAXINT
#  undef MAXINT
# endif
#endif

#ifndef BITSPERBYTE
# define BITSPERBYTE	CHAR_BIT
#endif
#ifndef BITS
# define BITS(type)	(BITSPERBYTE * sizeof(type))
#endif

#endif

