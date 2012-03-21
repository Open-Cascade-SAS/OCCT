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
