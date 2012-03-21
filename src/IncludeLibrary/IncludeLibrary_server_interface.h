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

#ifndef __INCLUDE_LIBRARY_SERVER_INTERFACE_H
# define __INCLUDE_LIBRARY_SERVER_INTERFACE_H

# ifndef WNT
# include <signal.h>
# endif

# ifndef __INCLUDELIBRARY_TYPEDEF_H
# include <IncludeLibrary_typedef.h>
# endif

# if defined(WNT) && !defined(HAVE_NO_DLL)
#  ifndef __server_API
#   if defined(__Serpc_DLL) || defined(__Server_DLL)
#    define __server_API __declspec( dllexport )
#   else
#    define __server_API __declspec( dllimport )
#   endif
#  endif
#  ifndef __Engine_API
#   if defined(__Engine_DLL)
#    define __Engine_API __declspec( dllexport )
#   else
#    define __Engine_API __declspec( dllimport )
#   endif
#  endif
# else
#  define __server_API
#  define __Engine_API
# endif

extern int __server_API srv_error(int NUMLNK,
                                  PSTATUS status);

extern int __server_API srv_init(PSTART_SERVER start_serpc,
                                 void *param,
                                 char *nodloc,
                                 int  phyadr[]);

extern int __Engine_API srv_receiving(int NUMLNK,
                                      int seqbyt,
                                      int *BUFACP,
                                      int lg_func,
                                      PSRV_RSP rpc_hdle,
                                      void *,
                                      PSTATUS status);

extern int __server_API srv_status(char *msg,
                                   int param,
                                   PSTATUS rpc_status,
                                   int LEN,
                                   char *msga);

# ifdef CSF1

extern int srvinfo_clt(void *vastprm,
                       char *username);
extern int srvuid_clt(void *astprm);

# if defined(__GNUC__) || defined(LIN)
#define get_receiving_signatura "get_receiving__FPiPPFv_viT1iPPviT0"
#define get_receiving_signature "get_receiving__FPiPPFv_viT1iPPviT0"
#define get_receiving_signatur0 "get_receiving__FPiPPFv_viT1iPPviT0"
#define get_receiving_signatur1 "get_receiving__FPiPPFv_viT1iPPviT0"
# elif defined(__osf__) || defined(DECOSF1)
#define get_receiving_signatura "get_receiving"
#define get_receiving_signature "get_receiving__XPiPPXv_viPPXv_viPPviPi"
#define get_receiving_signatur0 "get_receiving__XPPXiiPiiPXPvP6STATUSPiPi_iPvP6STATUS_iiPPXv_viPPviPi"
#define get_receiving_signatur1 "get_receiving__XPPXiiPiiPXPvP7_statusPiPi_iPvP7_status_iiPPXv_viPPviPi"
#  elif defined(__sun) || defined(SOLARIS)
#define get_receiving_signatura "get_receiving"
#define get_receiving_signature "__0FNget_receivingPiPPFv_viTCTDPPvTDTB"
#define get_receiving_signatur0 "__0FNget_receivingPPFiTBPiTBPFPvP6GSTATUSPiTD_iPvP6GSTATUS_iiPPFv_vTCPPvTCPi"
#define get_receiving_signatur1 "__0FNget_receivingPPFiTBPiTBPFPvP6H_statusPiTD_iPvP6H_status_iiPPFv_vTCPPvTCPi"
#  elif defined(__sgi) || defined(IRIX)
#define get_receiving_signatura "get_receiving__GPiPPGv_viT2T3PPvT3T1"
#define get_receiving_signature "get_receiving__FPiPPFv_viT2T3PPvT3T1"
#define get_receiving_signatur0 "get_receiving__FPPFiT1PiT1PFPvP6STATUSPiT3_iPvP6STATUS_iiPPFv_vT2PPvT2Pi"
#define get_receiving_signatur1 "get_receiving__FPPFiT1PiT1PFPvP7_statusPiT3_iPvP7_status_iiPPFv_vT2PPvT2Pi"
#  elif defined(__hpux) || defined(HPUX)
#define get_receiving_signatura "get_receiving"
#define get_receiving_signature "get_receiving__FPiPPFv_viT2T3PPvT3T1"
#define get_receiving_signatur0 "get_receiving__FPPFiT1PiT1PFPvP6STATUSPiT3_iPvP6STATUS_iiPPFv_vT2PPvT2Pi"
#define get_receiving_signatur1 "get_receiving__FPPFiT1PiT1PFPvP7_statusPiT3_iPvP7_status_iiPPFv_vT2PPvT2Pi"
#  elif defined(WNT)
#define get_receiving_signatura "get_receiving"
#define get_receiving_signature "?get_receiving@@YAXPAHPAP6AXXZH1HPAPAXH0@Z"
#define get_receiving_signatur0 "?get_receiving@@YAXPAP6AHHHPAHHP6AHPAXPAUSTATUS@@00@Z12@ZP6APADXZP6AXXZH@Z"
#define get_receiving_signatur1 "?get_receiving@@YAXPAP6AHHHPAHHP6AHPAXPAU_status@@00@Z12@ZHPAP6AXXZHPAPAXH0@Z"
#  elif defined(_AIX) || defined(AIX)
#define get_receiving_signatura "get_receiving__FPiPPFv_viT2T3PPvT3T1"
#define get_receiving_signature "get_receiving__FPiPPFv_viT2T3PPvT3T1"
#define get_receiving_signatur0 "get_receiving__FPiPPFv_viT2T3PPvT3T1"
#define get_receiving_signatur1 "get_receiving__FPiPPFv_viT2T3PPvT3T1"
#  endif

extern void __Engine_API get_receiving(int *nbr_ret_func ,
                                       PVOID_FUNCTION *TABRETFUNC ,
                                       int ,                   // nbrfunc
#ifndef WNT
                                       PVOID_FUNCTION * ,
#else
                                       PVOID_FUNCTION *adr_func ,
#endif
                                       int ,                   // nbrptr
                                       void **tabptr ,
                                       int ,                   // nbrval
                                       int *tabval ) ;
/*                                       PSRV_RECEIVING *srv_receiving,
                                       int nbrfunc ,
                                       void (**adr_func)() ,
                                       int nbrptr ,
                                       void **tabptr ,
                                       int nbrval ,
                                       int *tabval ) ;*/
# endif
#endif  /* __INCLUDE_LIBRARY_SERVER_INTERFACE_H */
