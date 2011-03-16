// File:	Draw_BasicCommands.cxx
// Created:	Thu Feb 23 18:21:17 1995
// Author:	Remi LEQUETTE
//		<rle@bravox>

#include <Standard_Macro.hxx>
#include <Standard_Stream.hxx>
#include <Standard_SStream.hxx>

#include <Draw.ixx>
#include <Draw_Appli.hxx>
#include <Draw_Printer.hxx>

#include <Message.hxx>
#include <Message_Messenger.hxx>

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#if defined(HAVE_TIME_H) || defined(WNT)
# include <time.h>
#endif

#ifdef HAVE_SIGNAL_H
# include <signal.h>
#endif

#ifndef WNT
# include <sys/resource.h>
# ifdef HAVE_STRINGS_H
#  include <strings.h>
# endif
#else
//WNT
extern Standard_Boolean Draw_Batch;
#include <windows.h>
#include <winbase.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_LIMITS 
# include <limits>
#elif defined (HAVE_LIMITS_H)
# include <limits.h>
#endif

#ifdef WNT
# include <limits>
#endif

static clock_t MDTV_CPU_LIMIT;   // Cpu_limit in Sec.
static clock_t MDTV_CPU_CURRENT; // cpu time already used at last
                                 // cpulimit call. (sec.) 
//#define strcasecmp strcmp Already defined
#define RLIM_INFINITY   0x7fffffff
#endif

#include <Draw_Chronometer.hxx>

#if defined (__hpux) || defined ( HPUX )
#define RLIM_INFINITY   0x7fffffff
#define	RLIMIT_CPU	0
#endif



//=======================================================================
// chronom
//=======================================================================

extern Standard_Boolean Draw_Chrono;

static Standard_Integer chronom(Draw_Interpretor& di,
				Standard_Integer n,const char** a)
{
  if ((n == 1) || (*a[1] == '0') || (*a[1] == '1')) {
    if (n == 1)
      Draw_Chrono = !Draw_Chrono;
    else
      Draw_Chrono = (*a[1] == '1');

    if (Draw_Chrono) di << "Chronometers activated."<<"\n";
    else di << "Chronometers desactivated."<<"\n";
  }
  else {
    Handle(Draw_Drawable3D) D = Draw::Get(a[1]);
    Handle(Draw_Chronometer) C;
    if (!D.IsNull()) {
      C = Handle(Draw_Chronometer)::DownCast(D);
    }
    if (C.IsNull()) {
      C = new Draw_Chronometer();
    Draw::Set(a[1],C,Standard_False);
    }
    if (n <= 2) {
      C->Timer().Reset();
    }
    else {
      if (!strcasecmp(a[2],"reset"))
	C->Timer().Reset();
      if (!strcasecmp(a[2],"start"))
	C->Timer().Start();
      if (!strcasecmp(a[2],"stop"))
	C->Timer().Stop();
      if (!strcasecmp(a[2],"show"))
	C->Timer().Show();
    }
  }
  return 0;
}

static Standard_Integer dchronom(Draw_Interpretor& I,
				 Standard_Integer n,const char** a)
{
  if ((n == 1) || (*a[1] == '0') || (*a[1] == '1')) {
    if (n == 1)
      Draw_Chrono = !Draw_Chrono;
    else
      Draw_Chrono = (*a[1] == '1');

    if (Draw_Chrono) I << "Chronometers activated."<<"\n";
    else I << "Chronometers desactivated."<<"\n";
  }
  else {
    Handle(Draw_Drawable3D) D = Draw::Get(a[1]);
    Handle(Draw_Chronometer) C;
    if (!D.IsNull()) {
      C = Handle(Draw_Chronometer)::DownCast(D);
    }
    if (C.IsNull()) {
      C = new Draw_Chronometer();
    Draw::Set(a[1],C,Standard_False);
    }
    if (n <= 2) {
      C->Timer().Reset();
    }
    else {
      if (!strcasecmp(a[2],"reset"))
	C->Timer().Reset();
      if (!strcasecmp(a[2],"start"))
	C->Timer().Start();
      if (!strcasecmp(a[2],"stop"))
	C->Timer().Stop();
      if (!strcasecmp(a[2],"show")) {
	Standard_SStream ss;
	C->Timer().Show(ss);
	I << ss;
      }
    }
  }
  return 0;
}



//=======================================================================
//function : ifbatch
//purpose  : 
//=======================================================================

static Standard_Integer ifbatch(Draw_Interpretor& DI, Standard_Integer , const char** )
{
  if (Draw_Batch)
    DI << "1";
  else
    DI << "0";
  
  return 0;
}

//=======================================================================
//function : spy
//purpose  : 
//=======================================================================

extern Standard_Boolean Draw_Spying;
extern filebuf Draw_Spyfile;

static Standard_Integer spy(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (Draw_Spying) 
    Draw_Spyfile.close();
  Draw_Spying = Standard_False;
  if (n > 1) {
    if (!Draw_Spyfile.open(a[1],ios::out)) {
      di << "Cannot open "<<a[1]<<" for writing"<<"\n";
      return 1;
    }
    Draw_Spying = Standard_True;
  }
  return 0;
}

//=======================================================================
//function : wait
//purpose  : 
//=======================================================================

static Standard_Integer Draw_wait(Draw_Interpretor& , Standard_Integer n, const char** a)
{
  Standard_Integer w = 10;
  if (n > 1)
    w = atoi(a[1]);
  time_t ct = time(NULL) + w;
  while (time(NULL) < ct) {};
  return 0;
}


static Standard_Integer cpulimit(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
#ifndef WNT
  rlimit rlp;
  rlp.rlim_max = RLIM_INFINITY;
  if (n <= 1)
    rlp.rlim_cur = RLIM_INFINITY;
  else
    rlp.rlim_cur = atoi(a[1]);

  int status;
  status=setrlimit(RLIMIT_CPU,&rlp);
  if (status !=0)
    di << "status cpulimit setrlimit : " << status << "\n";

#else
//WNT
  static int first=1;
/*
  unsigned int __stdcall CpuFunc(void * );
  unsigned ThreadID;

  if (n <= 1) MDTV_CPU_LIMIT = RLIM_INFINITY;
  else {
  
          MDTV_CPU_LIMIT = atoi(a[1]);
          MDTV_CPU_CURRENT = clock()/1000;

          if (first) // Lancer le thread au 1er appel seulement.
          {
                  first=0 ;
                  _beginthreadex(NULL,0,CpuFunc,NULL,0,&ThreadID);
          }
  }

*/
#endif

  return 0;
}


void Draw::BasicCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean Done = Standard_False;
  if (Done) return;
  Done = Standard_True;

  const char* g = "DRAW General Commands";
  
  theCommands.Add("batch", "returns 1 in batch mode",
		  __FILE__,ifbatch,g);
  theCommands.Add("spy","spy [file], Save commands in file. no file close",
		  __FILE__,spy,g);
  theCommands.Add("wait","wait [time(10)], wait time seconds",
		  __FILE__,Draw_wait,g);
  theCommands.Add("cpulimit","cpulimit [nbseconds], no args remove limits",
		  __FILE__,cpulimit,g);
  theCommands.Add("chrono","chrono [ name start/stop/reset/show]",
		  __FILE__,chronom,g);
  theCommands.Add("dchrono","dchrono [ name start/stop/reset/show]",
		  __FILE__,dchronom,g);
}
