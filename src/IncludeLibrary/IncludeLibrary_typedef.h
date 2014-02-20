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
