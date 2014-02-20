/*
 Copyright (c) 1998-1999 Matra Datavision
 Copyright (c) 1999-2014 OPEN CASCADE SAS

 This file is part of Open CASCADE Technology software library.

 This library is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License version 2.1 as published
 by the Free Software Foundation, with special exception defined in the file
 OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
 distribution for complete text of the license and disclaimer of any warranty.

 Alternatively, this file may be used under the terms of Open CASCADE
 commercial license or contractual agreement.
*/

#ifndef __INCLUDE_LIBRARY_STATUS_H
# define __INCLUDE_LIBRARY_STATUS_H


#ifdef WNT
# define STRICT
# include <windows.h>
# include <stdio.h>
# include <process.h>
#else
#if !defined(__hpux) && !defined(HPUX) && !defined(__FreeBSD__)
# include <sys/select.h>
#endif
#endif

		/* Structure du compte rendu */

/* +++++++                              JPT JPT JPT
   ATTENTION au mode d'adressage:
   VMS et ULTRIX de la droite vers la gauche,
   SUN et SGI de la gauche vers la droite.

   Comme le compte rendu vient du FORTRAN qui est un tableau de 2
   entiers, la structure du 1er mot de 32 bits est differente.
*/

#if defined (vax) || defined(VMS)
#define RIGHTLEFT

#elif defined(i386) || defined(LININTEL)
#define RIGHTLEFT

#elif defined(__i386)
#define RIGHTLEFT

#elif defined(__vms)
#define RIGHTLEFT

#elif defined(WNT)
#define RIGHTLEFT

#elif defined(__ia64) || defined(DS)
#define RIGHTLEFT

#elif defined(DS3)
#define RIGHTLEFT

#elif defined(__alpha) || defined(AO1)
#define RIGHTLEFT

#elif defined(__osf__) || defined(DECOSF1)
#define RIGHTLEFT

#elif defined(__sparc) || defined(SUN)
#define LEFTRIGHT

#elif defined(SOL)
#define LEFTRIGHT

#elif defined(__sparc9) || defined(SOLARIS)
#define LEFTRIGHT

#elif defined(__mips) || defined(SIL)
#define LEFTRIGHT

#elif defined(sgi) || defined(IRIX)
#define LEFTRIGHT

#elif defined(ppc)
#define LEFTRIGHT

#elif defined(__hpux) || defined(HPUX)
#define LEFTRIGHT

#elif defined(NEC)
#define LEFTRIGHT

#elif defined(_AIX) || defined(AIX)
#define LEFTRIGHT

#endif

#ifdef RIGHTLEFT
struct _status {
	char	type ;
	char	grp ;
	short	info ;
	int	stat ;
} ;

#elif defined(LEFTRIGHT)
struct _status {
	short	info ;
	char	grp ;
	char	type ;
	int	stat ;
} ;
#endif

typedef struct _status STATUS , *PSTATUS ;

#endif  /* __INCLUDE_LIBRARY_STATUS_H */
