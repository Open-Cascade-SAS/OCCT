// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <OSD.hxx>

#ifndef _WIN32

//---------- All Systems except Windows NT : ----------------------------------

# include <stdio.h>

#include <OSD_WhoAmI.hxx>
#include <OSD_SIGHUP.hxx>
#include <OSD_SIGINT.hxx>
#include <OSD_SIGQUIT.hxx>
#include <OSD_SIGILL.hxx>
#include <OSD_SIGKILL.hxx>
#include <OSD_SIGBUS.hxx>
#include <OSD_SIGSEGV.hxx>
#include <OSD_SIGSYS.hxx>
#include <OSD_Exception_CTRL_BREAK.hxx>
#include <Standard_NumericError.hxx>
#include <Standard_DivideByZero.hxx>
#include <Standard_Overflow.hxx>

#include <Standard_ErrorHandler.hxx>

// POSIX threads
#include <pthread.h>

#ifdef linux
#include <fenv.h>
static Standard_Boolean fFltExceptions = Standard_False;
#endif

// variable signalling that Control-C has been pressed (SIGINT signal)
static Standard_Boolean fCtrlBrk;

//const OSD_WhoAmI Iam = OSD_WPackage;

typedef void (ACT_SIGIO_HANDLER)(void) ;
ACT_SIGIO_HANDLER *ADR_ACT_SIGIO_HANDLER = NULL ;

#ifdef DECOSF1
typedef void (* SIG_PFV) (int);
#endif

#ifdef __GNUC__
# include <stdlib.h>
# include <stdio.h>
#else
#  ifdef SA_SIGINFO 
#    ifndef _AIX
#  include <sys/siginfo.h>
#     endif
#  endif
#endif
typedef void (* SIG_PFV) (int);

#include <signal.h>

#if !defined(__ANDROID__)
  #include <sys/signal.h>
#endif

#if defined(HAVE_PTHREAD_H) && defined(NO_CXX_EXCEPTION)
//============================================================================
//====  GetOldSigAction
//====     get previous 
//============================================================================

static struct sigaction *GetOldSigAction() 
{
  static struct sigaction oldSignals[NSIG];
  return oldSignals;
}

#ifdef SOLARIS
static sigfpe_handler_type *GetOldFPE()
{
  static sigfpe_handler_type aIEEEHandler[5] = { NULL, NULL, NULL, NULL, NULL } ;
  return aIEEEHandler;
}
#endif
#endif

//============================================================================
//==== Handler
//====     Catche the differents signals:
//====          1- The Fatal signals, which cause the end of process:
//====          2- The exceptions which are "signaled" by Raise.
//====     The Fatal Signals:
//====        SIGHUP, SIGINT, SIGQUIT, SIGILL, SIGKILL, SIGBUS, SIGSYS
//====     The Exceptions:
//====        SIGFPE
//====         (SUN versions)
//====           FPE_INTOVF_TRAP    // ..... integer overflow
//====           FPE_INTDIV_TRAP    // ..... integer divide by zero
//====           FPE_FLTINEX_TRAP   // ..... [floating inexact result]
//====           FPE_FLTDIV_TRAP    // ..... [floating divide by zero]
//====           FPE_FLTUND_TRAP    // ..... [floating underflow]
//====           FPE_FLTOPER_TRAP   // ..... [floating inexact result]
//====           FPE_FLTOVF_TRAP    // ..... [floating overflow]
//==== SIGSEGV is handled by "SegvHandler()"
//============================================================================
#ifdef SA_SIGINFO
static void Handler (const int theSignal, siginfo_t *theSigInfo, const Standard_Address theContext)
#else
static void Handler (const int theSignal)
#endif
{
  struct sigaction oldact, act;
  // re-install the signal
  if ( ! sigaction (theSignal, NULL, &oldact) ) {
    // cout << " signal is " << theSignal << " handler is " <<  oldact.sa_handler << endl;
    if (sigaction (theSignal, &oldact, &act)) perror ("sigaction");
  }
  else {
    perror ("sigaction");
  }

#if defined(HAVE_PTHREAD_H) && defined(NO_CXX_EXCEPTION)
  if (pthread_self() != getOCCThread()  || !Standard_ErrorHandler::IsInTryBlock()) {
    // use the previous signal handler
    // cout << "OSD::Handler: signal " << (int) theSignal << " occured outside a try block " <<  endl ;
    struct sigaction *oldSignals = GetOldSigAction();
    struct sigaction  asigacthandler =  oldSignals[theSignal >= 0 && theSignal < NSIG ? theSignal : 0];

    if (asigacthandler.sa_flags & SA_SIGINFO) {
      void (*aCurInfoHandle)(int, siginfo_t *, void *) = asigacthandler.sa_sigaction;

      siginfo_t * aSigInfo = NULL;
#ifdef SA_SIGINFO
      aSigInfo = theSigInfo;
#endif

      if (aSigInfo) {
        switch (aSigInfo->si_signo) {
          case SIGFPE:
          {
#ifdef SOLARIS
            sigfpe_handler_type *aIEEEHandlerTab = GetOldFPE();
            sigfpe_handler_type  aIEEEHandler = NULL;
            switch (aSigInfo->si_code) {
              case FPE_INTDIV_TRAP :
              case FPE_FLTDIV_TRAP :
               aIEEEHandler = aIEEEHandlerTab[1];
                break;
              case FPE_INTOVF_TRAP :
              case FPE_FLTOVF_TRAP :
                aIEEEHandler = aIEEEHandlerTab[2];
                break;
              case FPE_FLTUND_TRAP :
                aIEEEHandler = aIEEEHandlerTab[4];
                break;
              case FPE_FLTRES_TRAP :
                aIEEEHandler = aIEEEHandlerTab[3];
                break;
              case FPE_FLTINV_TRAP :
                aIEEEHandler = aIEEEHandlerTab[0];
                break;
              case FPE_FLTSUB_TRAP :
              default:
                break;
            }
            if (aIEEEHandler) {
              // cout << "OSD::Handler: calling previous IEEE signal handler with info" <<  endl ;
              (*aIEEEHandler) (theSignal, aSigInfo, theContext);
              return;
            }
#endif
            break;
          }
          default:
            break;
        }
      }
      if (aCurInfoHandle) {
        // cout << "OSD::Handler: calling previous signal handler with info " << aCurInfoHandle <<  endl ;
        (*aCurInfoHandle) (theSignal, aSigInfo, theContext);
#ifdef OCCT_DEBUG
        cerr << " previous signal handler return" <<  endl ;
#endif
        return;
      }
      else {
	// cout << "OSD::Handler: no handler with info for the signal" << endl;
      }
    }
    else {
      // no siginfo needed for the signal
      void (*aCurHandler) (int) = asigacthandler.sa_handler;
      if(aCurHandler) {
        // cout << "OSD::Handler: calling previous signal handler" <<  endl ;
        (*aCurHandler) (theSignal);
#ifdef OCCT_DEBUG
        cerr << " previous signal handler return" <<  endl ;
#endif
        return;
      }
    }
    // cout << " Signal occured outside a try block, but no handler for it" <<endl;
    return;
  }
#endif

  // cout << "OSD::Handler: signal " << (int) theSignal << " occured inside a try block " <<  endl ;
  if ( ADR_ACT_SIGIO_HANDLER != NULL )
    (*ADR_ACT_SIGIO_HANDLER)() ;
#ifdef linux
  if (fFltExceptions)
    feenableexcept (FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW);
    //feenableexcept (FE_INVALID | FE_DIVBYZERO);
#endif
  sigset_t set;
  sigemptyset(&set);
  switch (theSignal) {
  case SIGHUP:
    OSD_SIGHUP::NewInstance("SIGHUP 'hangup' detected.")->Jump();
    exit(SIGHUP);
    break;
  case SIGINT:
    // For safe handling of Control-C as stop event, arm a variable but do not
    // generate longjump (we are out of context anyway)
    fCtrlBrk = Standard_True;
    // OSD_SIGINT::NewInstance("SIGINT 'interrupt' detected.")->Jump();
    // exit(SIGINT);
    break;
  case SIGQUIT:
    OSD_SIGQUIT::NewInstance("SIGQUIT 'quit' detected.")->Jump();
    exit(SIGQUIT);
    break;
  case SIGILL:
    OSD_SIGILL::NewInstance("SIGILL 'illegal instruction' detected.")->Jump();
    exit(SIGILL);
    break;
  case SIGKILL:
    OSD_SIGKILL::NewInstance("SIGKILL 'kill' detected.")->Jump();
    exit(SIGKILL);
    break;
  case SIGBUS:
    sigaddset(&set, SIGBUS);
    sigprocmask(SIG_UNBLOCK, &set, NULL) ;
    OSD_SIGBUS::NewInstance("SIGBUS 'bus error' detected.")->Jump();
    exit(SIGBUS);
    break;
  case SIGSEGV:
    OSD_SIGSEGV::NewInstance("SIGSEGV 'segmentation violation' detected.")->Jump();
    exit(SIGSEGV);
    break;
#ifdef SIGSYS
  case SIGSYS:
    OSD_SIGSYS::NewInstance("SIGSYS 'bad argument to system call' detected.")->Jump();
    exit(SIGSYS);
    break;
#endif
  case SIGFPE:
    sigaddset(&set, SIGFPE);
    sigprocmask(SIG_UNBLOCK, &set, NULL) ;
#ifdef DECOSF1
    // Pour DEC/OSF1 SIGFPE = Division par zero.
    Standard_DivideByZero::NewInstance('')->Jump;
    break;
#endif
#if (!defined (__sun)) && (!defined(SOLARIS))
    Standard_NumericError::NewInstance("SIGFPE Arithmetic exception detected")->Jump();
    break;
#else
    // Reste SOLARIS
    if (aSigInfo) {
      switch(aSigInfo->si_code) {
      case FPE_FLTDIV_TRAP :
        Standard_DivideByZero::NewInstance("Floating Divide By Zero")->Jump();
        break;
      case FPE_INTDIV_TRAP :
        Standard_DivideByZero::NewInstance("Integer Divide By Zero")->Jump();
        break;
      case FPE_FLTOVF_TRAP :
        Standard_Overflow::NewInstance("Floating Overflow")->Jump();
        break;
      case FPE_INTOVF_TRAP :
        Standard_Overflow::NewInstance("Integer Overflow")->Jump();
        break;
      case FPE_FLTUND_TRAP :
        Standard_NumericError::NewInstance("Floating Underflow")->Jump();
        break;
      case FPE_FLTRES_TRAP:
        Standard_NumericError::NewInstance("Floating Point Inexact Result")->Jump();
        break;
      case FPE_FLTINV_TRAP :
        Standard_NumericError::NewInstance("Invalid Floating Point Operation")->Jump();
        break;
      default:
        Standard_NumericError::NewInstance("Numeric Error")->Jump();
        break;
      }
    } else {
      Standard_NumericError::NewInstance("SIGFPE Arithmetic exception detected")->Jump();
    }
#endif
    break;
#if defined (__sgi) || defined(IRIX)
  case SIGTRAP:
    sigaddset(&set, SIGTRAP);
    sigprocmask(SIG_UNBLOCK, &set, NULL) ;
    Standard_DivideByZero::NewInstance("SIGTRAP IntegerDivideByZero")->Jump(); break;
#endif
  default:
#ifdef OCCT_DEBUG
    cout << "Unexpected signal " << theSignal << endl ;
#endif
    break;
  }
}

//============================================================================
//==== SegvHandler
//============================================================================
#ifdef SA_SIGINFO

static void SegvHandler(const int theSignal,
                        siginfo_t *ip,
                        const Standard_Address theContext)
{
#ifdef NO_CXX_EXCEPTION
  if (!Standard_ErrorHandler::IsInTryBlock()) {
    Handler(theSignal, ip, theContext);
    return;
  }
#endif
#ifdef linux
  if (fFltExceptions)
    feenableexcept (FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW);
    //feenableexcept (FE_INVALID | FE_DIVBYZERO);
#endif
//  cout << "OSD::SegvHandler activated(SA_SIGINFO)" << endl ;
  if ( ip != NULL ) {
     sigset_t set;
     sigemptyset(&set);
     sigaddset(&set, SIGSEGV);
     sigprocmask (SIG_UNBLOCK, &set, NULL) ;
     void *address = ip->si_addr ;
     {
       char Msg[100];
       sprintf(Msg,"SIGSEGV 'segmentation violation' detected. Address %lx",
         (long ) address ) ;
       OSD_SIGSEGV::NewInstance(Msg)->Jump();
     }
  }
#ifdef OCCT_DEBUG
  else {
    cout << "Wrong undefined address." << endl ;
  }
#endif
  exit(SIGSEGV);
}

#elif defined (_hpux) || defined(HPUX)
// Not ACTIVE ? SA_SIGINFO is defined on SUN, OSF, SGI and HP (and Linux) !
// pour version 09.07

static void SegvHandler(const int theSignal,
                        siginfo_t *ip,
                        const Standard_Address theContext)
{
  unsigned long Space  ;
  unsigned long Offset ;
  char Msg[100] ;

  if ( theContext != NULL ) {
    Space = ((struct sigcontext *)theContext)->sc_sl.sl_ss.ss_cr20 ;
    Offset = ((struct sigcontext *)theContext)->sc_sl.sl_ss.ss_cr21 ;
//    cout << "Wrong address = " << hex(Offset) << endl ;
    {
      sprintf(Msg,"SIGSEGV 'segmentation violation' detected. Address %lx",Offset) ;
      OSD_SIGSEGV::Jump(Msg);
//    scp->sc_pcoq_head = scp->sc_pcoq_tail ;       Permettrait de continuer a
//    scp->sc_pcoq_tail = scp->sc_pcoq_tail + 0x4 ; l'intruction suivant le segv.
    }
  }
#ifdef OCCT_DEBUG
  else {
    cout << "Wrong undefined address." << endl ;
  }
#endif
  exit(SIGSEGV);
}

#endif

//============================================================================
//==== SetSignal 
//====     Set the differents signals:
//============================================================================

void OSD::SetSignal(const Standard_Boolean aFloatingSignal) 
{
  static int first_time = 3 ;
  struct sigaction act, oact;
  int              stat = 0;

  if( aFloatingSignal ) {
    //==== Enable the floating point exceptions ===============
#if defined (__sun) || defined (SOLARIS)
    sigfpe_handler_type PHandler = (sigfpe_handler_type) Handler ;
    stat = ieee_handler("set", "invalid",  PHandler);
    stat = ieee_handler("set", "division", PHandler) || stat;
    stat = ieee_handler("set", "overflow", PHandler) || stat;

    //stat = ieee_handler("set", "underflow", PHandler) || stat;
    //stat = ieee_handler("set", "inexact", PHandler) || stat;

    if (stat) {
#ifdef OCCT_DEBUG
      cerr << "ieee_handler does not work !!! KO " << endl;
#endif
    }
#elif defined (linux)
    feenableexcept (FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW);
    //feenableexcept (FE_INVALID | FE_DIVBYZERO);
    fFltExceptions = Standard_True;
#endif
  }
  else if ( first_time & 1 ) {
//    cout << "SetSignal( Standard_False ) is not implemented..." << endl ;
    first_time = first_time & (~ 1) ;
  }

#if defined (sgi) || defined (IRIX )
 if ( first_time & 2 ) {
   char *TRAP_FPE = getenv("TRAP_FPE") ;
   if ( TRAP_FPE == NULL ) {
#ifdef OCCT_DEBUG
     cout << "On SGI you must set TRAP_FPE environment variable : " << endl ;
     cout << "set env(TRAP_FPE) \"UNDERFL=FLUSH_ZERO;OVERFL=DEFAULT;DIVZERO=DEFAULT;INT_OVERFL=DEFAULT\" or" << endl ;
     cout << "setenv TRAP_FPE \"UNDERFL=FLUSH_ZERO;OVERFL=DEFAULT;DIVZERO=DEFAULT;INT_OVERFL=DEFAULT\"" << endl ;
#endif
//     exit(1) ;
     first_time = first_time & (~ 2) ;
   }
 }
#endif

  //==== Save the old Signal Handler, and set the new one ===================

  sigemptyset(&act.sa_mask) ;

#ifdef SA_RESTART
  act.sa_flags   = SA_RESTART ;
#else
  act.sa_flags   = 0 ;
#endif
#ifdef SA_SIGINFO
  act.sa_flags = act.sa_flags | SA_SIGINFO ;
  act.sa_sigaction = /*(void(*)(int, siginfo_t *, void*))*/ Handler;
#else
  act.sa_handler = /*(SIG_PFV)*/ Handler;
#endif

  //==== Always detected the signal "SIGFPE" =================================
  stat = sigaction(SIGFPE,&act,&oact);   // ...... floating point exception 
  if (stat) {
#ifdef OCCT_DEBUG
     cerr << "sigaction does not work !!! KO " << endl;
#endif
     perror("sigaction ");
  }

  //==== Detected the only the "free" signals ================================
  sigaction(SIGHUP,&act,&oact);    // ...... hangup  

#ifdef OBJS
  if(oact.sa_handler) 
	sigaction(SIGHUP,&oact,&oact);
#endif

  sigaction(SIGINT,&act,&oact);   // ...... interrupt   

#ifdef OBJS
  if(oact.sa_handler) 
	sigaction(SIGINT,&oact,&oact);
#endif
            
  sigaction(SIGQUIT,&act,&oact);  // ...... quit

#ifdef OBJS
  if(oact.sa_handler) 
	sigaction(SIGQUIT,&oact,&oact);
#endif

  sigaction(SIGILL,&act,&oact);   // ...... illegal instruction 

#ifdef OBJS
  if(oact.sa_handler) 
	sigaction(SIGILL,&oact,&oact);
#endif

  sigaction(SIGBUS,&act,&oact);   // ...... bus error 

#ifdef OBJS
  if(oact.sa_handler) 
	sigaction(SIGBUS,&oact,&oact);
#endif

#if (!defined (linux)) && (!defined(__linux__))
  sigaction(SIGSYS,&act,&oact);   // ...... bad argument to system call

# ifdef OBJS
  if(oact.sa_handler) 
	sigaction(SIGSYS,&oact,&oact);
# endif
#endif

#if defined (__sgi) || defined(IRIX)
  sigaction(SIGTRAP,&act,&oact);   // Integer Divide By Zero (IRIX)

# ifdef OBJS
  if(oact.sa_handler) 
	sigaction(SIGTRAP,&oact,&oact);
# endif
#endif

#ifdef SA_SIGINFO
  act.sa_sigaction = /*(void(*)(int, siginfo_t *, void*))*/ SegvHandler;
#else
  act.sa_handler = /*(SIG_PFV)*/ SegvHandler;
#endif

  if ( sigaction( SIGSEGV , &act , &oact ) )  // ...... segmentation violation
    perror("OSD::SetSignal sigaction( SIGSEGV , &act , &oact ) ") ;

#ifdef OBJS
  if(oact.sa_handler) 
	sigaction(SIGSEGV,&oact,&oact);
#endif
#if defined(__osf__) || defined(DECOSF1)
   struct sigaction action, prev_action;
   action.sa_handler = SIG_IGN;
   action.sa_mask = 0;
   action.sa_flags = 0;
   
   if (sigaction (SIGFPE, &action, &prev_action) == -1) {
     perror ("sigaction");
     exit (1);
   }
#endif

}

//============================================================================
//==== ControlBreak 
//============================================================================

void OSD :: ControlBreak () 
{
  if ( fCtrlBrk ) {
    fCtrlBrk = Standard_False;
    OSD_Exception_CTRL_BREAK::Raise ("*** INTERRUPT ***");
  }
}

#endif
