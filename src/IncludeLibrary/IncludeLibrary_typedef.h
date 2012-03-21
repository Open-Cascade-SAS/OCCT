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

#ifndef __INCLUDELIBRARY_TYPEDEF_H
#define __INCLUDELIBRARY_TYPEDEF_H

//#ifdef WNT
//#define LPARAM void*
//#endif

typedef void (*PVOID_FUNCTION)(void) ;

typedef void (*PUSR_SET_UIC)(char *usrnam,
                             int pid,
                             int *uic,
                             int *usruic,
                             int *k_chg_uic) ;

typedef int (*PSERVER) (PUSR_SET_UIC) ;

typedef int (*PSTART_SERVER)(void *astprm ,
                             PSTATUS status ,
                             int nbrtsk ,
                             char **server ,
                             int  k_fork ,
                             char *cputype ,
                             short num_func_intr );

typedef int (*PSRV_RSP)(void *,
                        PSTATUS status,
                        int *buffrsp,
                        int *lgrsp) ;

typedef int (*PSRV_RECEIVING)(int NUMLNK,
                              int seqbyt,
                              int *BUFACP,
                              int lg_func,
                              PSRV_RSP rpc_hdle,
                              void *,
                              PSTATUS status);
typedef void (*PGET_RECEIVING)(int *nbr_ret_func ,
                               PVOID_FUNCTION *TABRETFUNC ,
                               int nbrfunc ,
                               PVOID_FUNCTION * ,
                               int nbrptr ,
                               void **tabptr ,
                               int nbrval ,
                               int *tabval ) ;

typedef int (*PSRV_CTL)(void *inum,
                        int func,
                        int *bufcmd,
                        int sizcmd,
                        int sizrspbuf,
                        int k_synch ,
                        PSRV_RSP astadr,
                        void *,
                        PSTATUS status,
                        int *bufrsp,
                        int *sizrsp) ;

typedef int (*PAST_ADDR_PV)(void *) ;

typedef int (*PAST_ADDR_V)(int ) ;

typedef int (*PREAD_AST_MBX)(int lnk,
                             int limit,
                             int *ptr_uic,
                             char *ptr_usrnam,
                             int *ptr_pid) ;

#ifdef WNT
typedef unsigned ( __stdcall *PTHREAD_START ) ( void* );
#endif

typedef void (*PVOID_FUNCTION)(void) ;

typedef char *(*PGET_ENGINE_PATH)(void) ;

typedef void (*PGET_VECTOR)(int **K_VERBOSE ,
                            PVOID_FUNCTION ** ,
                            int *nbr_addr ) ;

typedef void (*POSD_EXITODEB)(void) ;

typedef int (*POSD_HOST_ADDR_TCP)(char *node,
                                  unsigned char *intadr,
                                  PSTATUS sts) ;

typedef int (*PNTD_ISERDISC)(PSTATUS status) ;

typedef void (*PACT_SIGIO_HANDLER)(void) ;

typedef int (*PENV_ENGINE)(int *lnk ,
                           int k_do_exec,
                           int K_START,
                           char *Engine_Path_Name,
                           char **Engine_Path ,
                           char **Engine_Name ) ;

typedef int (*PLOAD_ENGINE)(char *Engine_Path,
                            char *Engine_Name,
                            int KVERBOSEINIT ,
                            int *nbr_ret_func ,
                            PVOID_FUNCTION *TABRETFUNC ,
                            int nbrptrfunc ,
                            PVOID_FUNCTION *TABFUNC ,
                            int nbrptr ,
                            void **TABPTR ,
                            int nbrval ,
                            int *TABVAL );

typedef int (*POSD_NODENAME_TCP)(char *nodloc,
                                 int len,
                                 PSTATUS rpc_status ) ;

typedef int (*PSTRSTS)(void *numlnk,
                       char *code,
                       char stat,
                       PSTATUS status,
                       int lgmsg,
                       char *msgerr,
                       int *lenerr ) ;

typedef const char (*POSD_GETMSG)( int ) ;

typedef int (*PENGINE_CATEGORY)(int Category ) ;

typedef int (*PENGINE_ARGUMENT)(unsigned int Type,
                                void* Value) ;

typedef int (*PENGINE_SELECTOR)() ;

typedef int (*PENGINE_RETURN)(int*& Position,
                              unsigned int*& Type,
                              int*& NbElem,
                              int*& Length,
                              int**& Value) ;

#endif
