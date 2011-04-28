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
#include <OSD_MAllocHook.hxx>

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

//=======================================================================
//function : mallochook
//purpose  : 
//=======================================================================

static Standard_Integer mallochook(Draw_Interpretor& di, Standard_Integer n,
                                   const char** a)
{
  if (n < 2)
  {
    di << "\
usage: mallochook cmd\n\
where cmd is one of:\n\
  set [<op>]      - set callback to malloc/free; op is one of the following:\n\
                    0 - set callback to NULL,\n\
                    1 - set callback OSD_MAllocHook::CollectBySize (default)\n\
                    2 - set callback OSD_MAllocHook::LogFileHandler\n\
  reset           - reset the CollectBySize handler\n\
  report1 [<outfile>]\n\
                  - write report from CollectBySize handler in <outfile>\n\
  open [<logfile>]\n\
                  - open file for writing the log with LogFileHandler\n\
  close           - close the log file with LogFileHandler\n\
  report2 [<flag>] [<logfile>] [<outfile>]\n\
                  - scan <logfile> written with LogFileHandler\n\
                    and make synthesized report in <outfile>; <flag> can be:\n\
                    0 - simple stats by sizes (default),\n\
                    1 - with alive allocation numbers\n\
By default <logfile> is \"mem-log.txt\", <outfile> is \"mem-stat.txt\""
      << "\n";
    return 0;
  }
  if (strcmp(a[1], "set") == 0)
  {
    int aType = (n > 2 ? atoi(a[2]) : 1);
    if (aType < 0 || aType > 2)
    {
      di << "unknown op of the command set" << "\n";
      return 1;
    }
    else if (aType == 0)
    {
      OSD_MAllocHook::SetCallback(NULL);
      di << "callback is unset" << "\n";
    }
    else if (aType == 1)
    {
      OSD_MAllocHook::SetCallback(OSD_MAllocHook::GetCollectBySize());
      di << "callback is set to CollectBySize" << "\n";
    }
    else //if (aType == 2)
    {
      OSD_MAllocHook::SetCallback(OSD_MAllocHook::GetLogFileHandler());
      di << "callback is set to LogFileHandler" << "\n";
    }
  }
  else if (strcmp(a[1], "reset") == 0)
  {
    OSD_MAllocHook::GetCollectBySize()->Reset();
    di << "CollectBySize handler is reset" << "\n";
  }
  else if (strcmp(a[1], "open") == 0)
  {
    const char* aFileName = (n > 2 ? a[2] : "mem-log.txt");
    if (!OSD_MAllocHook::GetLogFileHandler()->Open(aFileName))
    {
      di << "cannot create file " << aFileName << " for writing" << "\n";
      return 1;
    }
    di << "log file " << aFileName << " is opened for writing" << "\n";
  }
  else if (strcmp(a[1], "close") == 0)
  {
    OSD_MAllocHook::GetLogFileHandler()->Close();
    di << "log file is closed" << "\n";
  }
  else if (strcmp(a[1], "report1") == 0)
  {
    const char* aOutFile = "mem-stat.txt";
    if (n > 2)
      aOutFile = a[2];
    if (OSD_MAllocHook::GetCollectBySize()->MakeReport(aOutFile))
    {
      di << "report " << aOutFile << " has been created" << "\n";
    }
    else
    {
      di << "cannot create report " << aOutFile << "\n";
      return 1;
    }
  }
  else if (strcmp(a[1], "report2") == 0)
  {
    Standard_Boolean includeAlive = Standard_False;
    const char* aLogFile = "mem-log.txt";
    const char* aOutFile = "mem-stat.txt";
    if (n > 2)
    {
      includeAlive = (atoi(a[2]) != 0);
      if (n > 3)
      {
        aLogFile = a[3];
        if (n > 4)
          aOutFile = a[4];
      }
    }
    if (OSD_MAllocHook::LogFileHandler::MakeReport(aLogFile, aOutFile, includeAlive))
    {
      di << "report " << aOutFile << " has been created" << "\n";
    }
    else
    {
      di << "cannot create report " << aOutFile << " from the log file "
        << aLogFile << "\n";
      return 1;
    }
  }
  else
  {
    di << "unrecognized command " << a[1] << "\n";
    return 1;
  }
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
  theCommands.Add("mallochook",
                  "debug memory allocation/deallocation, w/o args for help",
                  __FILE__, mallochook, g);
}
