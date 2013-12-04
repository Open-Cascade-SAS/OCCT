/*
 Copyright (c) 1998-1999 Matra Datavision
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

#define HOSTLENGTH 20
#define ENGINELENGTH 131
#define SERVERLENGTH 80
#define MAXENGINE 32

struct START {
	int lvl, func, narg ;
	int prtcl ;
	char hostname[HOSTLENGTH+1] ;
	char engine[ENGINELENGTH+1] ;
} ;

struct STARTRSP {
	int lvl, func, narg ;
	char hostname[HOSTLENGTH+1] ;
	char server[SERVERLENGTH+1] ;
	char prtcl ;
	char serverd[SERVERLENGTH+1] ;
	char prtcld ;
} ;

//struct STARTED {
//	int lvl, func, narg ;
//	char engine[ENGINELENGTH+1] ;
//	char server[SERVERLENGTH+1] ;
//	int prtcl ;
//} ;

//struct STARTEDRSP {
//	int lvl, func, narg ;
//} ;
