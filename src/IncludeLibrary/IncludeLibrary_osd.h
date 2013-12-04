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

/* Status des erreurs osd internes */ 
#define OSD_MAXASTS	1

#ifdef WNT
#define NUM_ASTS_DEF 32
#define MAXASTS 10
#define NTDNOTINIT    11
#endif

//#ifdef WNT
//#define errno GetLastError()
//#endif

typedef struct _syssynch SYSSYNCH, *PSYSSYNCH ;
struct _syssynch {
#ifdef UNX
	int go ;
	int wait ;
#elif defined(WNT)
        HANDLE hObject;
#endif
} ;

typedef struct _dring DRING, *PDRING ;
struct _dring {
	DRING *next ;
	PAST_ADDR_PV ast ;
	void *param ;
	unsigned int time ;
} ;

#if defined(UNX) || defined(WNT)

#define STSCLR(X)	((X)->type = PENDING ,\
			 (X)->grp = 0 ,\
			 (X)->info = 0 ,\
			 (X)->stat = 0 )

#define STSSYSOSD(X)	((X)->type = SYS_CODE | SYSTEM | ERROR ,\
			 (X)->grp = GRP_OSD ,\
			 (X)->stat = errno)

#define STSSYSNTD(X)	((X)->type = SYS_CODE | SYSTEM | ERROR ,\
			 (X)->grp = GRP_NTD ,\
			 (X)->stat = errno)

#define STSINTOSD(X,Y)	((X)->type = INTERNAL | ERROR ,\
			 (X)->grp = GRP_OSD ,\
			 (X)->stat = (Y))

#define STSINTNTD(X,Y)	((X)->type = INTERNAL | ERROR ,\
			 (X)->grp = GRP_NTD ,\
			 (X)->stat = (Y))

#else

#define osd_initsynch(S) 1

#define osd_stop(X)	( (X)->go = 0 )

#define osd_go(X)	( ((X)->go == 0) ? ((X)->go = 1 , SYS$SETEF(0)) : 1 )

#define osd_synch(X,S)	(SYS$SYNCH(0, (X)) , 1)

#define osd_disblast()	SYS$SETAST(0)

#define osd_enblast()	SYS$SETAST(1)

#define osd_exit(X)	SYS$EXIT(X)

#endif
