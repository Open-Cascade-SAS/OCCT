/*
 Copyright (c) 1998-1999 Matra Datavision
 Copyright (c) 1999-2012 OPEN CASCADE SAS

 The content of this file is subject to the Open CASCADE Technology Public
 License Version 6.5 (the "License"). You may not use the content of this file
 except in compliance with the License. Please obtain a copy of the License
 at http://www.opencascade.org and read it completely before using this file.

 The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
 main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.

 The Original Code and all software distributed under the License is
 distributed on an "AS IS" basis, without warranty of any kind, and the
 Initial Developer hereby disclaims all such warranties, including without
 limitation, any warranties of merchantability, fitness for a particular
 purpose or non-infringement. Please see the License for the specific terms
 and conditions governing the rights and limitations under the License.

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
