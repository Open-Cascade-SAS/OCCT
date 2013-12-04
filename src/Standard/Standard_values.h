/*
 Copyright (c) 1991-1999 Matra Datavision
 Copyright (c) 1999-2014 OPEN CASCADE SAS

 This file is part of Open CASCADE Technology software library.

 This library is free software; you can redistribute it and / or modify it
 under the terms of the GNU Lesser General Public version 2.1 as published
 by the Free Software Foundation, with special exception defined in the file
 OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
 distribution for complete text of the license and disclaimer of any warranty.

 Alternatively, this file may be used under the terms of Open CASCADE
 commercial license or contractual agreement.
*/

#ifndef _Standard_values_HeaderFile
# define _Standard_values_HeaderFile

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

