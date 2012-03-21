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

#ifndef __NTD_STSGRPDEF_H
# define __NTD_STSGRPDEF_H
		/* valeurs du champ type (a combiner) */

/* issssntt : 8 bits avec i : interrupt
                          s : identifieur de systeme
                          n : internal / systeme
                          t : Normal/Error/Warning */

#define PENDING		0	/* 2 bits de droite */
#define NORMAL		1
# ifdef ERROR
#  undef ERROR
# endif  /* ERROR */
#define ERROR		2
#define WARNING		3

#define INTERNAL	0	/* 1 bit */
#define SYSTEM		4

#define SYS_MASK      120	/* 4 bits */
#define SYS_VMS		0
#define SYS_DS		8
#define SYS_SUN        16
#define SYS_SGI        24
#define SYS_NEC        32
#define SYS_HPU        40
#define SYS_SOL        48
#define SYS_AO1        56
#define SYS_IBM        64
#define SYS_WNT        72
#define SYS_LIN        80
#define SYS_AIX        88
#define SYS_BSD        96
#define SYS_MAC       104

#define INTERRUPT     128	/* 1 bit */

		/* valeurs du champ grp */

#define GRP_OSD_VMS	1
#define GRP_OSD_UNX	2
#define GRP_NTD_VMS	3
#define GRP_NTD_UNX	4
#define GRP_ACS		5
#define GRP_SRV_MBX	6
#define GRP_SRV_CLT	7
#define GRP_SRV_LNK	8
#define GRP_SRV_RSP	9
#define GRP_SRV_SRV	10
#define GRP_SRV_OLB	11
#define GRP_TLI_LIB	12
#define GRP_OSD_WNT	13
#define	GRP_NTD_WNT	14
#define GRP_SRV_RPC	15
#define GRP_OSD_LIN	GRP_OSD_UNX
#define	GRP_NTD_LIN	GRP_NTD_UNX

#define GRP_SCS_PRC	16
#define GRP_SCS_ACP	17
#define GRP_SCS_SYN	18
#define GRP_SCS_PRS	19
#define GRP_SCS_ASP	20
#define GRP_SCS_ACA	21

#define GRP_BDS_LOC	32
#define GRP_BDS_REM	33
#define GRP_BDS_CNV	34
#define GRP_BDS_RCS	35
#define GRP_BDS_HDS	36
#define GRP_BDS_DBF	37
#define GRP_BDS_RMD	38
#define GRP_BDS_CLT	39
#define GRP_BDS_AST	40
#define GRP_BDS_ENT	41
#define GRP_BDS_ITM	42
#define GRP_BDS_ZNX	43
#define GRP_BDS_ACK	44

#define GRP_STARTER	64
#define GRP_FEI_CCA	65
#define GRP_FEI_SEL	66
#define GRP_FEI_ENG	67
#define GRP_FEI_CTL	68
#define GRP_FEI_SEGV    69


#define GRP_XXX         70

#define MAX_SRV_GRP GRP_SRV_RPC+1
#define MAX_DBS_GRP GRP_BDS_ACK-GRP_BDS_LOC+2

# if defined (__vms) || defined ( VMS )
#define SYS_CODE SYS_VMS
#define GRP_OSD GRP_OSD_VMS
#define GRP_NTD GRP_NTD_VMS

# elif defined (linux) || defined (LIN)
#define SYS_CODE SYS_LIN
#define GRP_OSD GRP_OSD_LIN
#define GRP_NTD GRP_NTD_LIN

# elif WNT
#define SYS_CODE SYS_WNT
#define GRP_OSD GRP_OSD_WNT
#define GRP_NTD GRP_NTD_WNT

# elif UNX
#define GRP_OSD GRP_OSD_UNX
#define GRP_NTD GRP_NTD_UNX

#  ifdef DS3
#define SYS_CODE SYS_DS

#  elif defined ( SUN )
#define SYS_CODE SYS_SUN

#  elif defined (sgi) || defined (SIL) || defined (IRIX)
#define SYS_CODE SYS_SGI

#  elif NEC
#define SYS_CODE SYS_NEC

#  elif defined (__hpux) || defined (HPUX)
#define SYS_CODE SYS_HPU

#  elif defined (sun) || defined ( SOLARIS ) || defined ( SOL )
#define SYS_CODE SYS_SOL

#  elif defined (__osf__) || defined ( AO1 ) || defined (DECOSF1)
#define SYS_CODE SYS_AO1

#  elif defined (_AIX) || defined (AIX)
#define SYS_CODE SYS_AIX

#  elif IBM
#define SYS_CODE SYS_IBM

# elif defined(__FreeBSD__)
#define SYS_CODE SYS_BSD

# elif defined(__APPLE__)
#define SYS_CODE SYS_MAC

#  endif
# endif

#endif  /* __NTD_STSGRPDEF_H */
