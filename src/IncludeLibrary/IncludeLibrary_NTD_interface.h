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

#ifndef INCLUDELIBRARY_NTD_INTERFACE_H
#define INCLUDELIBRARY_NTD_INTERFACE_H

#if defined(__hpux) || defined(HPUX)
# include <dl.h>
#endif

#if defined(WNT) && !defined(HAVE_NO_DLL)
# ifndef __NTD_API
#  if defined(__NTD_DLL)
#   define __NTD_API __declspec( dllexport )
#  else
#   define __NTD_API __declspec( dllimport )
#  endif
# endif
# ifndef __Engine_API
#  if defined(__Engine_DLL)
#   define __Engine_API __declspec( dllexport )
#  else
#   define __Engine_API __declspec( dllimport )
#  endif
# endif
#else
# define __NTD_API
# define __Engine_API
#endif

#ifndef __INCLUDELIBRARY_SERVER_INTERFACE_H
#include <IncludeLibrary_server_interface.h>
#endif

#define ind_init_ntd           0
#define ind_movc3              1
#define ind_movc5              2
#define ind_ntd_connect        3
#define ind_ntd_disconnect     4
#define ind_ntd_free           5
#define ind_ntd_gravity        6
#define ind_ntd_read           7
#define ind_ntd_reada          8
#define ind_ntd_select         9
#define ind_ntd_write         10
#define ind_osd_dclast        11
#define ind_osd_exitodeb      12
#define ind_osd_go            13
#define ind_osd_host_addr     14
#define ind_osd_setimer       15
#define ind_osd_stop          16
#define ind_osd_synch         17
#define ind_srvsts            18
#define ind_osd_host_addr_tcp 19
#define ind_ntd_iserdisc      20
#define ind_Get_Engine_Path   21
#define ind_Act_Sigio_Handler 22
#define ind_Load_Engine       23
#define ind_osd_nodename_tcp  24
#define ind_strsts            25
#define ind_osd_getmsg        26
#define ind_osd_wpipe         27
#define ind_Env_Engine        28
#define nbr_address           29

#ifdef UNX
#define sigdeb sigdeb_
#define srvbck srvbck_
#define srvctl srvctl_
#define srwlnk srwlnk_
#define srwloc srwloc_
#define srvsts srvsts_
#define osd_getpid osd_getpid_
#define osd_getuic osd_getuic_
#define osd_geteuic osd_geteuic_
#define osd_nodename_tcp  osd_nodename_tcp_
#define osd_host_addr osd_host_addr_
#define osd_host_addr_tcp osd_host_addr_tcp_
#define osd_host_addr_dec osd_host_addr_dec_
#define __fastcall
#endif

#ifdef WNT
#define BEGINTHREADEX( lpsa, cbStack, lpStartAddr, lpvThreadParm, fdwCreate, lpIDThread ) \
         (  ( HANDLE )_beginthreadex (                   \
                       ( void* )( lpsa ),                \
                       ( unsigned )( cbStack ),          \
                       ( PTHREAD_START )( lpStartAddr ), \
                       ( void* )( lpvThreadParm ),       \
                       ( unsigned )( fdwCreate ),        \
                       ( unsigned* )( lpIDThread )       \
                      )                                  \
         )

#define osd_chown () 1

#endif

#if defined(sun) || defined(SOLARIS)
extern "C" int risc_return_() ;
#endif

extern int __NTD_API K_VERBOSE_INIT ;

extern char __NTD_API level_engine[81] ;

#ifdef WNT
extern OSVERSIONINFO __NTD_API NTD_OSVer;
#endif

extern void __NTD_API Get_Vector(int **K_VERBOSE ,
                                 void (***vector_addr)() ,
                                 int *nbr_addr ) ;

#if defined(__GNUC__) || defined(LIN)
#define Get_Vector_signatura "Get_Vector__FPPiPPPFv_vPi"
#define Get_Vector_signature "Get_Vector__FPPiPPPFv_vPi"
#elif defined(__osf__) || defined(DECOSF1)
#define Get_Vector_signatura "Get_Vector"
#define Get_Vector_signature "Get_Vector__XPPiPPPXv_vPi"
#elif defined(__sun) || defined(SOLARIS)
#define Get_Vector_signatura "Get_Vector"
#define Get_Vector_signature "__0FKGet_VectorPPiPPPFv_vPi"
#elif defined(__sgi) || defined(IRIX)
#define Get_Vector_signatura "Get_Vector__GPPiPPPGv_vPi"
#define Get_Vector_signature "Get_Vector__FPPiPPPFv_vPi"
#elif defined(__hpux) || defined(HPUX)
#define Get_Vector_signatura "Get_Vector"
#define Get_Vector_signature "Get_Vector__FPPiPPPFv_vPi"
#elif defined(WNT)
#define Get_Vector_signatura "Get_Vector"
#define Get_Vector_signature "?Get_Vector@@YAXPAPAHPAPAP6AXXZPAH@Z"
#elif defined(_AIX) || defined(AIX)
#define Get_Vector_signatura "Get_Vector"
#define Get_Vector_signature ".Get_Vector__FPPiPPPFv_vPi"
#endif

#ifndef WNT
extern void __NTD_API Act_Sigio_Handler() ;
#endif
extern void __NTD_API Set_Engine_Path(char *path);

#if defined(__Engine_DLL)
extern char __Engine_API *Get_Engine_Path(void);
#else
extern char __NTD_API *Get_Engine_Path(void);
#endif

extern int __NTD_API Env_Engine(int *lnk ,
                                int k_do_exec,
                                int K_START,
                                char *Engine_Path_Name,
                                char **Engine_Path ,
                                char **Engine_Name ) ;

extern int __NTD_API Load_Engine(char *Engine_Path,
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

extern int __NTD_API init_ntd(PSRV_CTL srvctl,
                              int k_synch,
                              int **uic,
                              char **usr,
                              int **pid,
                              int k_serpc);
#ifdef WNT
extern void __NTD_API done_ntd( void );
#endif
extern int __NTD_API ntd_listener(int k_synch ,
                                  char **server,
                                  PREAD_AST_MBX read_ast_mbx,
                                  PSTATUS sts,
                                  int *lnk);
extern int __NTD_API ntd_confirm(int id,
                                 int reply,
                                 PAST_ADDR_V ast,
                                 int param,
                                 PSTATUS sts);
extern int __NTD_API ntd_connect(int prtcl,
                                 char *node,
                                 char *server,
                                 int verbose,
                                 int k_synch,
                                 PAST_ADDR_PV astaddr,
                                 void *param,
                                 int *id,
                                 PSTATUS sts);
extern int __NTD_API ntd_disconnect(int id,
                                    int how,
                                    PAST_ADDR_PV astaddr,
                                    void *param,
                                    PSTATUS sts);
extern int __NTD_API ntd_resume(int *id,
                                int k_free,
                                PSTATUS sts);
extern int __NTD_API ntd_chown(int id,
                               int pid,
                               PSTATUS sts);
extern int __NTD_API ntd_synch(int CHAN,
                               PSTATUS status_synch);
extern int __NTD_API ntd_stop(int CHAN);
extern int __NTD_API ntd_read(int CHAN,
                              int *BUFFER,
                              int LEN,
                              PAST_ADDR_PV rpc_intr_read_ast,
                              void *,
                              int *READLEN,
                              PSTATUS rpc_status);
extern int __NTD_API ntd_reada(int CHAN,
                               int *BUFFER,
                               int LEN,
                               PAST_ADDR_PV rpc_intr_read_ast,
                               void *,
                               int *READLEN,
                               PSTATUS rpc_status);
extern int __NTD_API ntd_iserdisc(PSTATUS status);
extern int __NTD_API ntd_dassign(int CHAN,
                                 PSTATUS status);
extern int __NTD_API ntd_free(int *CHAN,
                              PSTATUS status);
extern int __NTD_API ntd_write(int CHAN,
                               int *BUFFER,
                               int LEN,
                               PAST_ADDR_PV rpc_write_ast,
                               void *,
                               int *LLEN,
                               PSTATUS rpc_statusa);
extern int __NTD_API ntd_select(int CHAN) ;
extern int __NTD_API do_lstr_a_synch(int k_synch) ;
extern int __NTD_API do_socket_a_synch(int sock_desc,int k_synch) ;
extern int __NTD_API ntd_go(int CHAN,
                            PSTATUS rpc_status);
extern int __NTD_API ntd_gravity(PSTATUS sts);
extern int __NTD_API ntd_message(void *numlnk,
                                 PSTATUS sts,
                                 char *msg,
                                 int lmax,
                                 int *lmsg);
#ifdef WNT
extern BOOL __NTD_API __fastcall ntd_ispipe ( int id ) ;
extern SOCKET __NTD_API ntd_sd ( int* id ) ;
#endif
extern int __NTD_API osd_dclast(PAST_ADDR_PV rpc_write_ast,
                                void *,
                                PSTATUS rpc_status);
extern int __NTD_API osd_dclastw(PAST_ADDR_PV rpc_write_ast,
                                 void *,
                                 PSTATUS rpc_statusa);
extern int __NTD_API osd_setimer(unsigned int delay,
                                 PAST_ADDR_PV timer_ast,
                                 void *,
                                 PSTATUS sts);
extern char __NTD_API *osd_getsymbol(char *) ;
#ifdef WNT
extern const char __NTD_API *osd_getmsg ( int );
#endif
extern void __NTD_API osd_hiber();
extern char __NTD_API *osd_getuname();
extern void __NTD_API osd_exitodeb();
extern void __NTD_API osd_child(char *);
extern void __NTD_API osd_wpipe() ;
extern int __NTD_API osd_nodename_dec(char *nodloc,
                                       int len,
                                       PSTATUS rpc_status);
extern int __NTD_API osd_nodename_tcp(char *nodloc,
                                       int len,
                                       PSTATUS rpc_status);
extern int __NTD_API osd_host_addr_tcp(char *node,
                                       unsigned char intadr[],
                                       PSTATUS sts);
extern int __NTD_API osd_host_addr( char intadr[] ) ;
extern int __NTD_API osd_getuic(int *retuic,
                                PSTATUS sts);
extern int __NTD_API osd_getpid();
extern int __NTD_API osd_geteuic(int *reteuic);
extern int __NTD_API osd_setuic(int *setuic);
extern int __NTD_API osd_usruic(char *usrnam,
                                int  retuic[],
                                PSTATUS status,
                                int lenusr);

#ifdef UNX
extern void osd_sigclrmask();
#endif

#if defined(UNX) || defined(WNT)
extern int __NTD_API osd_initsynch(PSTATUS sts);
extern void __NTD_API osd_go(PSYSSYNCH );
extern void __NTD_API osd_stop(PSYSSYNCH );
extern int __NTD_API osd_testsynch(PSYSSYNCH );
extern int __NTD_API osd_synch(PSYSSYNCH ,
                               PSTATUS );
extern void __NTD_API osd_disblast();
extern void __NTD_API osd_enblast();
#endif

extern void __NTD_API __fastcall rspreorder(PBUF_RSP buffer);
extern void __NTD_API __fastcall lreorder(unsigned int *hdr);
extern void __NTD_API __fastcall cmdreorder(PBUF BUFFER);
extern void __NTD_API __fastcall cnxassreorder(PCMD_BUFF BUFFER);
extern void __NTD_API __fastcall reorder(int l , char *p);
extern void __NTD_API __fastcall movc3(int srclen,
                                       char *srcadr,
                                       char *dstadr);
extern void __NTD_API __fastcall movc5(int srclen,
                                       char *srcadr,
                                       char fill,
                                       int dstlen,
                                       char *dstadr);
extern int __NTD_API srvsts(void *,
                            char *,
                            int,
                            PSTATUS sts,
                            int lg,
                            char *msg,
                            int *lgmsg);
extern int __NTD_API osd_realpath(char *path,
                                  char *resolved_path,
                                  int pathlen,
                                  PSTATUS sts);
extern int __NTD_API strsts(void *numlnk,
                            char code[],
                            char stat,
                            PSTATUS status,
                            int lgmsg,
                            char msgerr[],
                            int *lenerr);
extern void __NTD_API osd_putoutput(char *msg);
#ifdef UNX
extern int osd_sigaction(int sig ,
                         struct sigaction *act ,
                         struct sigaction *oact ) ;
#endif
extern int __NTD_API srvctl_ast(void *param,
                                PSTATUS status,
                                int *msg,
                                int *msglen);

#ifndef UNX
extern int Dnet_listener(char *server,
                         int isrv,
                         PREAD_AST_MBX func,
                         PSTATUS sts);
#endif
extern int __NTD_API Inet_listener(char *server,
                                   int isrv,
                                   int k_synch ,
                                   PREAD_AST_MBX func,
                                   PSTATUS sts);
#if !defined(__hpux) && !defined(HPUX) && !defined(__osf__) && !defined(DECOSF1) && !defined(WNT) && !defined(LIN) && !defined(linux) && !defined(AIX) && !defined(_AIX) && !defined(__FreeBSD__)
extern
#ifdef CSF1
       "C" {
#endif
            int  gethostname(char *, int) ;
#ifdef CSF1
	                                   }
#endif
#endif

#if DEBUG
extern void __NTD_API sigdeb() ;
extern void __NTD_API SIGDEB() ;
#endif

#ifdef WNT
extern void __NTD_API _debug_break ( char* );
#endif

#if DO_MALLOC
extern "C" {void __NTD_API *realloc(void *ptr_free , size_t byte_size);}
extern "C" {void __NTD_API *calloc(size_t nbr_elts,size_t byte_size);}
extern "C" {void __NTD_API *malloc(size_t byte_size);}
extern "C" {void __NTD_API free(void *pointer);}
#endif

#endif
