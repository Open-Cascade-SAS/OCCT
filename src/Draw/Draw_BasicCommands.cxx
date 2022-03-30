// Created on: 1995-02-23
// Created by: Remi LEQUETTE
// Copyright (c) 1995-1999 Matra Datavision
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


#include <Draw.hxx>
#include <Draw_Appli.hxx>
#include <Draw_Chronometer.hxx>
#include <Draw_Drawable3D.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Message_PrinterOStream.hxx>
#include <OSD.hxx>
#include <OSD_Chronometer.hxx>
#include <OSD_Environment.hxx>
#include <OSD_Exception_CTRL_BREAK.hxx>
#include <OSD_MAllocHook.hxx>
#include <OSD_MemInfo.hxx>
#include <OSD_Parallel.hxx>
#include <OSD_ThreadPool.hxx>
#include <Standard_Macro.hxx>
#include <Standard_SStream.hxx>
#include <Standard_Stream.hxx>
#include <Standard_Version.hxx>
#include <TCollection_AsciiString.hxx>

#include <OSD_PerfMeter.h>
#ifdef _WIN32

#include <windows.h>
#include <winbase.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits>

#define RLIM_INFINITY   0x7fffffff

static clock_t CPU_CURRENT; // cpu time already used at last
                            // cpulimit call. (sec.) 
#else /* _WIN32 */

#include <sys/resource.h>
#include <signal.h>
#include <unistd.h>

#if defined (__hpux) || defined ( HPUX )
#define RLIM_INFINITY   0x7fffffff
#define	RLIMIT_CPU	0
#endif

#endif /* _WIN32 */

extern Standard_Boolean Draw_Batch;

static clock_t CPU_LIMIT;   // Cpu_limit in Sec.
static OSD_Timer aTimer;

//=======================================================================
// chronom
//=======================================================================

extern Standard_Boolean Draw_Chrono;

static Standard_Integer dchronom (Draw_Interpretor& theDI,
                                  Standard_Integer theNbArgs,
                                  const char** theArgVec)
{
  if (theNbArgs == 1
   || (theNbArgs == 2
    && (*theArgVec[1] == '0'
     || *theArgVec[1] == '1')))
  {
    if (theNbArgs == 1)
    {
      Draw_Chrono = !Draw_Chrono;
    }
    else
    {
      Draw_Chrono = (*theArgVec[1] == '1');
    }

    theDI << (Draw_Chrono
           ? "Chronometers activated.\n"
           : "Chronometers deactivated.\n");
    return 0;
  }

  const char* aTimerName = theArgVec[1];
  Handle(Draw_Chronometer) aChronom;
  if (Handle(Draw_Drawable3D) aDrawable = Draw::Get (aTimerName))
  {
    aChronom = Handle(Draw_Chronometer)::DownCast (aDrawable);
  }
  if (aChronom.IsNull())
  {
    aChronom = new Draw_Chronometer();
    Draw::Set (aTimerName, aChronom, false);
  }

  if (theNbArgs <= 2)
  {
    aChronom->Timer().Reset();
    return 0;
  }

  const bool toShowCout = (TCollection_AsciiString (theArgVec[0]) == "chrono");
  int aNbPuts = false;
  for (Standard_Integer anIter = 2; anIter < theNbArgs; ++anIter)
  {
    TCollection_AsciiString anArg (theArgVec[anIter]);
    anArg.LowerCase();
    if (anArg == "-reset"
     || anArg == "reset")
    {
      aChronom->Timer().Reset();
    }
    else if (anArg == "-restart"
          || anArg == "restart")
    {
      aChronom->Timer().Restart();
    }
    else if (anArg == "-start"
          || anArg == "-resume"
          || anArg == "start")
    {
      aChronom->Timer().Start();
    }
    else if (anArg == "-stop"
          || anArg == "-pause"
          || anArg == "stop")
    {
      aChronom->Timer().Stop();
    }
    else if (anArg == "-show"
          || anArg == "show")
    {
      if (toShowCout)
      {
        aChronom->Timer().Show (std::cout);
      }
      else
      {
        Standard_SStream aStream;
        aChronom->Timer().Show (aStream);
        theDI << aStream;
      }
    }
    else if (anIter + 1 < theNbArgs
          && (anArg == "-counter"
           || anArg == "counter"))
    {
      Standard_Real aSeconds = 0.0, aCPUtime = 0.0;
      Standard_Integer aMinutes = 0, aHours = 0;
      aChronom->Timer().Show (aSeconds, aMinutes, aHours, aCPUtime);
      if (toShowCout)
      {
        std::cout << "COUNTER " << theArgVec[++anIter] << ": " << aCPUtime << "\n";
      }
      else
      {
        theDI << "COUNTER " << theArgVec[++anIter] << ": " << aCPUtime << "\n";
      }
    }
    else if (anArg == "-elapsed")
    {
      if (++aNbPuts > 1) { theDI << " "; }
      theDI << aChronom->Timer().ElapsedTime();
    }
    else if (anArg == "-cpu"
          || anArg == "-usercpu"
          || anArg == "-cpuuser")
    {
      if (++aNbPuts > 1) { theDI << " "; }
      theDI << aChronom->Timer().UserTimeCPU();
    }
    else if (anArg == "-systemcpu"
          || anArg == "-syscpu"
          || anArg == "-cpusystem"
          || anArg == "-cpusys")
    {
      if (++aNbPuts > 1) { theDI << " "; }
      theDI << aChronom->Timer().SystemTimeCPU();
    }
    else if (anArg == "-thread"
          || anArg == "-threadonly")
    {
      bool isThreadOnly = Draw::ParseOnOffIterator (theNbArgs, theArgVec, anIter);
      aChronom->Timer().Stop();
      aChronom->Timer().Reset();
      aChronom->Timer().SetThisThreadOnly (isThreadOnly);
    }
    else if (anArg == "-process")
    {
      bool isProcessTime = Draw::ParseOnOffIterator (theNbArgs, theArgVec, anIter);
      aChronom->Timer().Stop();
      aChronom->Timer().Reset();
      aChronom->Timer().SetThisThreadOnly (!isProcessTime);
    }
    else
    {
      theDI << "Syntax error at '" << theArgVec[anIter] << "'\n";
      return 1;
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
extern std::filebuf Draw_Spyfile;

static Standard_Integer spy(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (Draw_Spying) 
    Draw_Spyfile.close();
  Draw_Spying = Standard_False;
  if (n > 1) {
    if (!Draw_Spyfile.open(a[1],std::ios::out)) {
      di << "Cannot open "<<a[1]<<" for writing\n";
      return 1;
    }
    Draw_Spying = Standard_True;
  }
  return 0;
}

static Standard_Integer dlog(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n != 2 && n != 3)
  {
    Message::SendFail() << "Enable or disable logging: " << a[0] << " {on|off}\n"
                        << "Reset log: " << a[0] << " reset\n"
                        << "Get log content: " << a[0] << " get";
    return 1;
  }

  if (! strcmp (a[1], "on") && n == 2)
  {
    di.SetDoLog (Standard_True);
//    di.Log() << "dlog on" << std::endl; // for symmetry
  }
  else if (! strcmp (a[1], "off") && n == 2)
  {
    di.SetDoLog (Standard_False);
  }
  else if (! strcmp (a[1], "reset") && n == 2)
  {
    di.ResetLog();
  }
  else if (! strcmp (a[1], "get") && n == 2)
  {
    di << di.GetLog();
  }
  else if (! strcmp (a[1], "add") && n == 3)
  {
    di.AddLog (a[2]);
    di.AddLog ("\n");
  }
  else if (! strcmp (a[1], "status") && n == 2)
  {
    di << (di.GetDoLog() ? "on" : "off");
  }
  else {
    Message::SendFail() << "Unrecognized option(s): " << a[1];
    return 1;
  }
  return 0;
}

static Standard_Integer decho(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n != 2)
  {
    Message::SendFail() << "Enable or disable echoing: " << a[0] << " {on|off}";
    return 1;
  }

  if (! strcmp (a[1], "on"))
  {
    di.SetDoEcho (Standard_True);
  }
  else if (! strcmp (a[1], "off"))
  {
    di.SetDoEcho (Standard_False);
  }
  else {
    Message::SendFail() << "Unrecognized option: " << a[1];
    return 1;
  }
  return 0;
}

static Standard_Integer dbreak(Draw_Interpretor& di, Standard_Integer, const char**)
{
  try {
    OSD::ControlBreak();
  }
  catch (OSD_Exception_CTRL_BREAK const&) {
    di << "User pressed Control-Break";
    return 1; // Tcl exception
  }

  return 0;
}

static Standard_Integer dversion(Draw_Interpretor& di, Standard_Integer, const char**)
{
  // print OCCT version and OCCTY-specific macros used
  di << "Open CASCADE Technology " << OCC_VERSION_STRING_EXT << "\n";
#ifdef OCCT_DEBUG
  di << "Extended debug mode\n";
#elif defined(_DEBUG)
  di << "Debug mode\n";
#endif
#ifdef HAVE_TK
  di << "Tk enabled (HAVE_TK)\n";
#else
  di << "Tk disabled\n";
#endif
#ifdef HAVE_XLIB
  di << "Xlib enabled (HAVE_XLIB)\n";
#elif !defined(_WIN32)
  di << "Xlib disabled\n";
#endif
#ifdef HAVE_TBB
  di << "TBB enabled (HAVE_TBB)\n";
#else 
  di << "TBB disabled\n";
#endif
#ifdef HAVE_FREETYPE
  di << "FreeType enabled (HAVE_FREETYPE)\n";
#else
  di << "FreeType disabled\n";
#endif
#ifdef HAVE_FREEIMAGE
  di << "FreeImage enabled (HAVE_FREEIMAGE)\n";
#else
  di << "FreeImage disabled\n";
#endif
#ifdef HAVE_FFMPEG
  di << "FFmpeg enabled (HAVE_FFMPEG)\n";
#else
  di << "FFmpeg disabled\n";
#endif
#ifdef HAVE_OPENGL_EXT
  di << "OpenGL: enabled (HAVE_OPENGL_EXT)\n";
#endif
#ifdef HAVE_GLES2_EXT
  di << "OpenGL ES: enabled (HAVE_GLES2_EXT)\n";
#endif
#ifdef HAVE_OPENVR
  di << "OpenVR enabled (HAVE_OPENVR)\n";
#else
  di << "OpenVR disabled\n";
#endif
#ifdef HAVE_RAPIDJSON
  di << "RapidJSON enabled (HAVE_RAPIDJSON)\n";
#else
  di << "RapidJSON disabled\n";
#endif
#ifdef HAVE_DRACO
  di << "Draco enabled (HAVE_DRACO)\n";
#else
  di << "Draco disabled\n";
#endif
#ifdef HAVE_VTK
  di << "VTK enabled (HAVE_VTK)\n";
#else
  di << "VTK disabled\n";
#endif
#ifdef No_Exception
  di << "Exceptions disabled (No_Exception)\n";
#else
  di << "Exceptions enabled\n";
#endif

  // check compiler, OS, etc. using pre-processor macros provided by compiler
  // see "Pre-defined C/C++ Compiler Macros" http://sourceforge.net/p/predef/wiki/
  // note that only modern compilers that are known to be used for OCCT are recognized

  // compiler; note that GCC and MSVC are last as other compilers (e.g. Intel) can also define __GNUC__ and _MSC_VER
#if defined(__INTEL_COMPILER)
  di << "Compiler: Intel " << __INTEL_COMPILER << "\n";
#elif defined(__BORLANDC__)
  di << "Compiler: Borland C++ (__BORLANDC__ = " << __BORLANDC__ << ")\n";
#elif defined(__clang__)
  di << "Compiler: Clang " << __clang_major__ << "." << __clang_minor__ << "." << __clang_patchlevel__ << "\n";
#elif defined(__SUNPRO_C)
  di << "Compiler: Sun Studio (__SUNPRO_C = " << __SUNPROC_C << ")\n";
#elif defined(_MSC_VER)
  #if _MSC_VER < 1900
    di << "Compiler: MS Visual C++ " << (int)(_MSC_VER/100-6) << "." << (int)((_MSC_VER/10)-60-10*(int)(_MSC_VER/100-6)) << " (_MSC_FULL_VER = " << _MSC_FULL_VER << ")\n";
  #else
    di << "Compiler: MS Visual C++ " << (int)(_MSC_VER/100-5) << "." << (int)((_MSC_VER/10)-50-10*(int)(_MSC_VER/100-5)) << " (_MSC_FULL_VER = " << _MSC_FULL_VER << ")\n";
  #endif
#elif defined(__GNUC__)
  di << "Compiler: GCC " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__ << "\n";
#else
  di << "Compiler: unrecognized\n";
#endif

  // Cygwin and MinGW specifics
#if defined(__CYGWIN__)
  di << "Cygwin\n";
#endif
#if defined(__MINGW64__)
  di << "MinGW 64 " << __MINGW64_VERSION_MAJOR << "." << __MINGW64_VERSION_MINOR << "\n";
#elif defined(__MINGW32__)
  di << "MinGW 32 " << __MINGW32_MAJOR_VERSION << "." << __MINGW32_MINOR_VERSION << "\n";
#endif 

  // architecture
#if defined(__amd64) || defined(__x86_64) || defined(_M_AMD64)
  di << "Architecture: AMD64\n";
#elif defined(__i386) || defined(_M_IX86) || defined(__X86__)|| defined(_X86_)
  di << "Architecture: Intel x86\n";
#elif defined(_M_IA64) || defined(__ia64__)
  di << "Architecture: Intel Itanium (IA 64)\n";
#elif defined(__sparc__) || defined(__sparc)
  di << "Architecture: SPARC\n";
#elif defined(__aarch64__) && defined(__LP64__)
  di << "Architecture: ARM 64-bit\n";
#elif defined(__arm__) || defined(__arm64__)
  #if defined(__LP64__)
  di << "Architecture: ARM 64-bit\n";
  #else
  di << "Architecture: ARM 32-bit\n";
  #endif
#elif defined(__EMSCRIPTEN__)
  di << "Architecture: WASM "
  #if defined(__LP64__)
     << "64-bit"
  #else
     << "32-bit"
  #endif
  #if defined(__wasm_simd128__)
     << " SIMD128"
  #endif
     << "\n";
#else
  di << "Architecture: unrecognized\n";
#endif

  // OS
#if defined(_WIN32) || defined(__WINDOWS__) || defined(__WIN32__)
  di << "OS: Windows\n";
#elif defined(__APPLE__) || defined(__MACH__)
  di << "OS: Mac OS X\n";
#elif defined(__sun) 
  di << "OS: SUN Solaris\n";
#elif defined(__ANDROID__) /* must be before Linux */
  #include <android/api-level.h>
  di << "OS: Android (__ANDROID_API__ = " << __ANDROID_API__ << ")\n";
#elif defined(__QNXNTO__)
  di << "OS: QNX Neutrino\n";
#elif defined(__QNX__)
  di << "OS: QNX\n";
#elif defined(__linux__)
  di << "OS: Linux\n";
#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__)
  #include <sys/param.h>
  di << "OS: BSD (BSD = " << BSD << ")\n";
#elif defined(__EMSCRIPTEN__)
  di << "OS: WebAssembly (Emscripten SDK " << __EMSCRIPTEN_major__ << "." << __EMSCRIPTEN_minor__ << "." << __EMSCRIPTEN_tiny__
  #ifdef __EMSCRIPTEN_PTHREADS__
     << "; pthreads ON"
  #else
     << "; pthreads OFF"
  #endif
     << ")\n";
#else
  di << "OS: unrecognized\n";
#endif

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
    w = Draw::Atoi(a[1]);
  time_t ct = time(NULL) + w;
  while (time(NULL) < ct) {};
  return 0;
}

//=======================================================================
//function : cpulimit
//purpose  : 
//=======================================================================
#ifdef _WIN32
static unsigned int __stdcall CpuFunc (void * /*param*/)
{
  clock_t anElapCurrent;
  clock_t aCurrent;

  for(;;)
  {
    Sleep (5);
    Standard_Real anUserSeconds, aSystemSeconds;
    OSD_Chronometer::GetProcessCPU (anUserSeconds, aSystemSeconds);
    aCurrent = clock_t(anUserSeconds + aSystemSeconds);
    anElapCurrent = clock_t(aTimer.ElapsedTime());
    
    if (CPU_LIMIT > 0 && (aCurrent - CPU_CURRENT) >= CPU_LIMIT)
    {
      aTimer.Stop();
      if (IsDebuggerPresent())
      {
        std::cout << "Info: CPU limit (" << CPU_LIMIT << " sec) has been reached but ignored because of attached Debugger" << std::endl;
        return 0;
      }
      else
      {
        std::cout << "ERROR: Process killed by CPU limit (" << CPU_LIMIT << " sec)" << std::endl;
        ExitProcess (2);
      }
    }
    if (CPU_LIMIT > 0 && anElapCurrent >= CPU_LIMIT)
    {
      aTimer.Stop();
      if (IsDebuggerPresent())
      {
        std::cout << "Info: Elapsed limit (" << CPU_LIMIT << " sec) has been reached but ignored because of attached Debugger" << std::endl;
        return 0;
      }
      else
      {
        std::cout << "ERROR: Process killed by elapsed limit (" << CPU_LIMIT << " sec)" << std::endl;
        ExitProcess (2);
      }
    }
  }
}
#else
static void cpulimitSignalHandler (int)
{
  std::cout << "Process killed by CPU limit  (" << CPU_LIMIT << " sec)" << std::endl;
  exit(2);
}
static void *CpuFunc(void* /*threadarg*/)
{
  clock_t anElapCurrent;
  for(;;)
  {
    sleep (5);
    anElapCurrent = clock_t(aTimer.ElapsedTime());
    if (CPU_LIMIT >0 && (anElapCurrent) >= CPU_LIMIT) {
      std::cout << "Process killed by elapsed limit  (" << CPU_LIMIT << " sec)" << std::endl;
      exit(2);
    }
  }
  return NULL;
}
#endif

// Returns time in seconds defined by the argument string,
// multiplied by factor defined in environment variable
// CSF_CPULIMIT_FACTOR (if it exists, 1 otherwise)
static clock_t GetCpuLimit (const Standard_CString theParam)
{
  clock_t aValue = Draw::Atoi (theParam);

  OSD_Environment aEnv("CSF_CPULIMIT_FACTOR");
  TCollection_AsciiString aEnvStr = aEnv.Value();
  if (!aEnvStr.IsEmpty())
  {
    aValue *= Draw::Atoi (aEnvStr.ToCString());
  }
  return aValue;
}

static Standard_Integer cpulimit(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  static int aFirst = 1;
#ifdef _WIN32
  // Windows specific code
  unsigned int __stdcall CpuFunc (void *);
  unsigned aThreadID;

  if (n <= 1){
    CPU_LIMIT = RLIM_INFINITY;
  } else {
    CPU_LIMIT = GetCpuLimit (a[1]);
    Standard_Real anUserSeconds, aSystemSeconds;
    OSD_Chronometer::GetProcessCPU (anUserSeconds, aSystemSeconds);
    CPU_CURRENT = clock_t(anUserSeconds + aSystemSeconds);
    aTimer.Reset();
    aTimer.Start();
    if (aFirst) // Launch the thread only at the 1st call.
    {
      aFirst = 0;
      _beginthreadex (NULL, 0, CpuFunc, NULL, 0, &aThreadID);
    }
  }

#else 
  // Unix & Linux
  rlimit rlp;
  rlp.rlim_max = RLIM_INFINITY;
  if (n <= 1)
  {
    rlp.rlim_cur = RLIM_INFINITY;
  }
  else
  {
    rlp.rlim_cur = GetCpuLimit (a[1]);
  }

  CPU_LIMIT = (clock_t )rlp.rlim_cur;

  int aStatus = setrlimit (RLIMIT_CPU, &rlp);
  if (aStatus != 0)
  {
    di << "status cpulimit setrlimit : " << aStatus << "\n";
  }

  // set signal handler to print a message before death
  struct sigaction act, oact;
  memset (&act, 0, sizeof(act));
  act.sa_handler = cpulimitSignalHandler;
  sigaction (SIGXCPU, &act, &oact);

  // cpulimit for elapsed time
  aTimer.Reset();
  aTimer.Start();
  pthread_t cpulimitThread;
  if (aFirst) // Launch the thread only at the 1st call.
  {
    aFirst = 0;
    pthread_create(&cpulimitThread, NULL, CpuFunc, NULL);
  }
#endif
  di << "CPU and elapsed time limit set to " << (double)CPU_LIMIT << " seconds";
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
    int aType = (n > 2 ? Draw::Atoi(a[2]) : 1);
    if (aType < 0 || aType > 2)
    {
      di << "unknown op of the command set\n";
      return 1;
    }
    else if (aType == 0)
    {
      OSD_MAllocHook::SetCallback(NULL);
      di << "callback is unset\n";
    }
    else if (aType == 1)
    {
      OSD_MAllocHook::SetCallback(OSD_MAllocHook::GetCollectBySize());
      di << "callback is set to CollectBySize\n";
    }
    else //if (aType == 2)
    {
      OSD_MAllocHook::SetCallback(OSD_MAllocHook::GetLogFileHandler());
      di << "callback is set to LogFileHandler\n";
    }
  }
  else if (strcmp(a[1], "reset") == 0)
  {
    OSD_MAllocHook::GetCollectBySize()->Reset();
    di << "CollectBySize handler is reset\n";
  }
  else if (strcmp(a[1], "open") == 0)
  {
    const char* aFileName = (n > 2 ? a[2] : "mem-log.txt");
    if (!OSD_MAllocHook::GetLogFileHandler()->Open(aFileName))
    {
      di << "cannot create file " << aFileName << " for writing\n";
      return 1;
    }
    di << "log file " << aFileName << " is opened for writing\n";
  }
  else if (strcmp(a[1], "close") == 0)
  {
    OSD_MAllocHook::GetLogFileHandler()->Close();
    di << "log file is closed\n";
  }
  else if (strcmp(a[1], "report1") == 0)
  {
    const char* aOutFile = "mem-stat.txt";
    if (n > 2)
      aOutFile = a[2];
    if (OSD_MAllocHook::GetCollectBySize()->MakeReport(aOutFile))
    {
      di << "report " << aOutFile << " has been created\n";
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
      includeAlive = (Draw::Atoi(a[2]) != 0);
      if (n > 3)
      {
        aLogFile = a[3];
        if (n > 4)
          aOutFile = a[4];
      }
    }
    if (OSD_MAllocHook::LogFileHandler::MakeReport(aLogFile, aOutFile, includeAlive))
    {
      di << "report " << aOutFile << " has been created\n";
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

//==============================================================================
//function : dlocale
//purpose  :
//==============================================================================

static int dlocale (Draw_Interpretor& di, Standard_Integer n, const char** argv)
{
  int category = LC_ALL;
  if (n > 1)
  {
    const char *cat = argv[1];
    if ( ! strcmp (cat, "LC_ALL") ) category = LC_ALL;
    else if ( ! strcmp (cat, "LC_COLLATE") ) category = LC_COLLATE;
    else if ( ! strcmp (cat, "LC_CTYPE") ) category = LC_CTYPE;
    else if ( ! strcmp (cat, "LC_MONETARY") ) category = LC_MONETARY;
    else if ( ! strcmp (cat, "LC_NUMERIC") ) category = LC_NUMERIC;
    else if ( ! strcmp (cat, "LC_TIME") ) category = LC_TIME;
    else 
    {
      Message::SendFail() << "Error: cannot recognize argument " << cat << " as one of LC_ macros";
      return 1;
    }
  }
  const char* locale = (n > 2 ? argv[2] : NULL);
  const char* result = setlocale (category, locale);
  if (result)
    di << result;
  else 
    std::cout << "Error: unsupported locale specification: " << locale << std::endl;
  return 0;
}

//==============================================================================
//function : dmeminfo
//purpose  :
//==============================================================================

static int dmeminfo (Draw_Interpretor& theDI,
                     Standard_Integer  theArgNb,
                     const char**      theArgVec)
{
  if (theArgNb <= 1)
  {
    OSD_MemInfo aMemInfo;
    theDI << aMemInfo.ToString();
    return 0;
  }

  NCollection_Map<OSD_MemInfo::Counter> aCounters;
  for (Standard_Integer anIter = 1; anIter < theArgNb; ++anIter)
  {
    TCollection_AsciiString anArg (theArgVec[anIter]);
    anArg.LowerCase();
    if (anArg == "virt" || anArg == "v")
    {
      aCounters.Add (OSD_MemInfo::MemVirtual);
    }
    else if (anArg == "heap" || anArg == "h")
    {
      aCounters.Add (OSD_MemInfo::MemHeapUsage);
    }
    else if (anArg == "wset" || anArg == "w")
    {
      aCounters.Add (OSD_MemInfo::MemWorkingSet);
    }
    else if (anArg == "wsetpeak")
    {
      aCounters.Add (OSD_MemInfo::MemWorkingSetPeak);
    }
    else if (anArg == "swap")
    {
      aCounters.Add (OSD_MemInfo::MemSwapUsage);
    }
    else if (anArg == "swappeak")
    {
      aCounters.Add (OSD_MemInfo::MemSwapUsagePeak);
    }
    else if (anArg == "private")
    {
      aCounters.Add (OSD_MemInfo::MemPrivate);
    }
    else
    {
      theDI << "Syntax error at '" << theArgVec[anIter] << "'!\n";
      return 1;
    }
  }

  OSD_MemInfo aMemInfo (Standard_False);
  aMemInfo.SetActive (Standard_False);
  for (NCollection_Map<OSD_MemInfo::Counter>::Iterator aCountersIt (aCounters); aCountersIt.More(); aCountersIt.Next())
  {
    aMemInfo.SetActive (aCountersIt.Value(), Standard_True);
  }
  aMemInfo.Update();

  for (NCollection_Map<OSD_MemInfo::Counter>::Iterator aCountersIt (aCounters); aCountersIt.More(); aCountersIt.Next())
  {
    theDI << Standard_Real (aMemInfo.Value (aCountersIt.Value())) << " ";
  }
  theDI << "\n";
  return 0;
}

//==============================================================================
//function : dparallel
//purpose  :
//==============================================================================
static int dparallel (Draw_Interpretor& theDI,
                      Standard_Integer  theArgNb,
                      const char**      theArgVec)
{
  const Handle(OSD_ThreadPool)& aDefPool = OSD_ThreadPool::DefaultPool();
  if (theArgNb <= 1)
  {
    theDI << "NbLogicalProcessors: " << OSD_Parallel::NbLogicalProcessors() << "\n"
          << "NbThreads:           " << aDefPool->NbThreads() << "\n"
          << "NbDefThreads:        " << aDefPool->NbDefaultThreadsToLaunch() << "\n"
          << "UseOcct:             " << (OSD_Parallel::ToUseOcctThreads() ? 1 : 0);
    return 0;
  }

  for (Standard_Integer anIter = 1; anIter < theArgNb; ++anIter)
  {
    TCollection_AsciiString anArg (theArgVec[anIter]);
    anArg.LowerCase();
    if (anIter + 1 < theArgNb
     && (anArg == "-nbthreads"
      || anArg == "-threads"))
    {
      const Standard_Integer aVal = Draw::Atoi (theArgVec[++anIter]);
      aDefPool->Init (aVal);
    }
    else if (anIter + 1 < theArgNb
          && (anArg == "-nbdefthreads"
           || anArg == "-defthreads"
           || anArg == "-nbmaxdefthreads"
           || anArg == "-maxdefthreads"))
    {
      const Standard_Integer aVal = Draw::Atoi (theArgVec[++anIter]);
      if (aVal <= 0 || aVal > aDefPool->NbThreads())
      {
        Message::SendFail() << "Syntax error: maximum number of threads to use should be <= of threads in the pool";
        return 1;
      }
      aDefPool->SetNbDefaultThreadsToLaunch (aVal);
    }
    else if (anIter + 1 < theArgNb
          && (anArg == "-useocct"
           || anArg == "-touseocct"
           || anArg == "-occt"))
    {
      const Standard_Integer aVal = Draw::Atoi (theArgVec[++anIter]);
      OSD_Parallel::SetUseOcctThreads (aVal == 1);
      if (OSD_Parallel::ToUseOcctThreads() != (aVal == 1))
      {
        std::cout << "Warning: unable to switch threads library - no options available\n";
      }
    }
    else if (anIter + 1 < theArgNb
          && (anArg == "-usetbb"
           || anArg == "-tousetbb"
           || anArg == "-tbb"))
    {
      const Standard_Integer aVal = Draw::Atoi (theArgVec[++anIter]);
      OSD_Parallel::SetUseOcctThreads (aVal == 0);
      if (OSD_Parallel::ToUseOcctThreads() != (aVal == 0))
      {
        std::cout << "Warning: unable to switch threads library - no options available\n";
      }
    }
    else
    {
      Message::SendFail() << "Syntax error: unknown argument '" << anArg << "'";
      return 1;
    }
  }
  return 0;
}

//==============================================================================
//function : dperf
//purpose  :
//==============================================================================

static int dperf (Draw_Interpretor& theDI, Standard_Integer theArgNb, const char** theArgVec)
{
  // reset if argument is provided and it is not '0'
  int reset = (theArgNb > 1 ? theArgVec[1][0] != '0' && theArgVec[1][0] != '\0' : 0);
  char buffer[25600];
  perf_sprint_all_meters (buffer, 25600 - 1, reset);
  theDI << buffer;
  return 0;
}

//==============================================================================
//function : dsetsignal
//purpose  :
//==============================================================================

static int dsetsignal (Draw_Interpretor& theDI, Standard_Integer theArgNb, const char** theArgVec)
{
  OSD_SignalMode aMode = OSD_SignalMode_Set;
  Standard_Boolean aSetFPE = OSD::ToCatchFloatingSignals();
  Standard_Integer aStackLen = OSD::SignalStackTraceLength();

  // default for FPE signal is defined by CSF_FPE variable, if set
  OSD_Environment aEnv("CSF_FPE");
  TCollection_AsciiString aEnvStr = aEnv.Value();
  if (!aEnvStr.IsEmpty())
  {
    aSetFPE = (aEnvStr.Value(1) != '0');
  }

  // parse arguments
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anArg == "asis")
    {
      aMode = OSD_SignalMode_AsIs;
    }
    else if (anArg == "set")
    {
      aMode = OSD_SignalMode_Set;
    }
    else if (anArg == "unhandled")
    {
      aMode = OSD_SignalMode_SetUnhandled;
    }
    else if (anArg == "unset")
    {
      aMode = OSD_SignalMode_Unset;
    }
    else if (anArg == "1" || anArg == "on")
    {
      aSetFPE = Standard_True;
    }
    else if (anArg == "0" || anArg == "off")
    {
      aSetFPE = Standard_False;
    }
    else if (anArg == "default")
    {
    }
    else if (anArgIter + 1 < theArgNb
          && (anArg == "-stracktracelength"
           || anArg == "-stracktracelen"
           || anArg == "-stracklength"
           || anArg == "-stracklen"))
    {
      aStackLen = Draw::Atoi (theArgVec[++anArgIter]);
    }
    else
    {
      Message::SendFail() << "Syntax error: unknown argument '" << anArg << "'";
      return 1;
    }
  }

  OSD::SetSignal(aMode, aSetFPE);
  OSD::SetSignalStackTraceLength (aStackLen);

  // report actual status in the end
  const char* aModeStr = 0;
  switch (OSD::SignalMode())
  {
    default:
    case OSD_SignalMode_AsIs:         aModeStr = "asis";      break;
    case OSD_SignalMode_Set:          aModeStr = "set";       break;
    case OSD_SignalMode_SetUnhandled: aModeStr = "unhandled"; break;
    case OSD_SignalMode_Unset:        aModeStr = "unset";     break;
  }
  theDI << "Signal mode: " << aModeStr << "\n"
        << "Catch FPE: " << (OSD::ToCatchFloatingSignals() ? "1" : "0") << "\n"
        << "Stack Trace Length: " << aStackLen << "\n";
  return 0;
}

//==============================================================================
//function : dtracelevel
//purpose  :
//==============================================================================

static int dtracelevel (Draw_Interpretor& theDI,
                        Standard_Integer  theArgNb,
                        const char**      theArgVec)
{
  Message_Gravity aLevel = Message_Info;
  if (theArgNb < 1 || theArgNb > 2)
  {
    Message::SendFail() << "Error: wrong number of arguments! See usage:";
    theDI.PrintHelp (theArgVec[0]);
    return 1;
  }
  else if (theArgNb == 2)
  {
    TCollection_AsciiString aVal (theArgVec[1]);
    aVal.LowerCase();
    if (aVal == "trace")
    {
      aLevel = Message_Trace;
    }
    else if (aVal == "info")
    {
      aLevel = Message_Info;
    }
    else if (aVal == "warn"
          || aVal == "warning")
    {
      aLevel = Message_Warning;
    }
    else if (aVal == "alarm")
    {
      aLevel = Message_Alarm;
    }
    else if (aVal == "fail")
    {
      aLevel = Message_Fail;
    }
    else
    {
      Message::SendFail() << "Error: unknown gravity '" << theArgVec[1] << "'";
      return 1;
    }
  }

  Handle(Message_Messenger) aMessenger = Message::DefaultMessenger();
  if (aMessenger.IsNull())
  {
    Message::SendFail() << "Error: default messenger is unavailable";
    return 1;
  }

  Message_SequenceOfPrinters& aPrinters = aMessenger->ChangePrinters();
  if (aPrinters.Length() < 1)
  {
    Message::SendFail() << "Error: no printers registered in default Messenger";
    return 0;
  }

  for (Standard_Integer aPrinterIter = 1; aPrinterIter <= aPrinters.Length(); ++aPrinterIter)
  {
    Handle(Message_Printer)& aPrinter = aPrinters.ChangeValue (aPrinterIter);
    if (theArgNb == 1)
    {
      if (aPrinterIter == 1)
      {
        aLevel = aPrinter->GetTraceLevel();
      }
      else if (aLevel == aPrinter->GetTraceLevel())
      {
        continue;
      }

      switch (aPrinter->GetTraceLevel())
      {
        case Message_Trace:   theDI << "trace"; break;
        case Message_Info:    theDI << "info";  break;
        case Message_Warning: theDI << "warn";  break;
        case Message_Alarm:   theDI << "alarm"; break;
        case Message_Fail:    theDI << "fail";  break;
      }
      continue;
    }

    aPrinter->SetTraceLevel (aLevel);
  }

  return 0;
}

//==============================================================================
//function : ddebugtraces
//purpose  :
//==============================================================================
static int ddebugtraces (Draw_Interpretor& theDI, Standard_Integer theArgNb, const char** theArgVec)
{
  if (theArgNb < 2)
  {
    theDI << Standard_Failure::DefaultStackTraceLength();
    return 0;
  }
  else if (theArgNb != 2)
  {
    theDI << "Syntax error: wrong number of arguments";
    return 1;
  }

  Standard_Failure::SetDefaultStackTraceLength (Draw::Atoi (theArgVec[1]));
  return 0;
}

//==============================================================================
//function : dputs
//purpose  :
//==============================================================================
static int dputs (Draw_Interpretor& theDI,
                  Standard_Integer theArgNb,
                  const char** theArgVec)
{
  Standard_OStream* aStream = &std::cout;
  bool isNoNewline = false, toIntense = false;
  Message_ConsoleColor aColor = Message_ConsoleColor_Default;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anArg == "-nonewline")
    {
      isNoNewline = true;
    }
    else if (anArg == "stdcout")
    {
      aStream = &std::cout;
    }
    else if (anArg == "stdcerr")
    {
      aStream = &std::cerr;
    }
    else if (anArg == "-intense")
    {
      toIntense = true;
    }
    else if (anArg == "-black")
    {
      aColor = Message_ConsoleColor_Black;
    }
    else if (anArg == "-white")
    {
      aColor = Message_ConsoleColor_White;
    }
    else if (anArg == "-red")
    {
      aColor = Message_ConsoleColor_Red;
    }
    else if (anArg == "-blue")
    {
      aColor = Message_ConsoleColor_Blue;
    }
    else if (anArg == "-green")
    {
      aColor = Message_ConsoleColor_Green;
    }
    else if (anArg == "-yellow")
    {
      aColor = Message_ConsoleColor_Yellow;
    }
    else if (anArg == "-cyan")
    {
      aColor = Message_ConsoleColor_Cyan;
    }
    else if (anArg == "-magenta")
    {
      aColor = Message_ConsoleColor_Magenta;
    }
    else if (anArgIter + 1 == theArgNb)
    {
      if (!theDI.ToColorize())
      {
        toIntense = false;
        aColor = Message_ConsoleColor_Default;
      }
      if (toIntense || aColor != Message_ConsoleColor_Default)
      {
        Message_PrinterOStream::SetConsoleTextColor (aStream, aColor, toIntense);
      }

      *aStream << theArgVec[anArgIter];
      if (!isNoNewline)
      {
        *aStream << std::endl;
      }

      if (toIntense || aColor != Message_ConsoleColor_Default)
      {
        Message_PrinterOStream::SetConsoleTextColor (aStream, Message_ConsoleColor_Default, false);
      }
      return 0;
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << anArg << "'";
      return 1;
    }
  }

  Message::SendFail() << "Syntax error: wrong number of arguments";
  return 1;
}

void Draw::BasicCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean Done = Standard_False;
  if (Done) return;
  Done = Standard_True;

  std::ios::sync_with_stdio();

  const char* g = "DRAW General Commands";

  theCommands.Add("batch", "returns 1 in batch mode",
		  __FILE__,ifbatch,g);
  theCommands.Add("spy","spy [file], Save commands in file. no file close",
		  __FILE__,spy,g);
  theCommands.Add("wait","wait [time(10)], wait time seconds",
		  __FILE__,Draw_wait,g);
  theCommands.Add("cpulimit","cpulimit [nbseconds], no args remove limits",
		  __FILE__,cpulimit,g);

  const char* aChronoHelp =
            "chrono Name [-start] [-stop] [-reset] [-restart] [-counter Text]"
    "\n\t\t:             [-show] [-elapsed] [-userCPU] [-sysCPU]"
    "\n\t\t:             [-thread|-process {0|1}]"
    "\n\t\t: Operates named timer:"
    "\n\t\t:   -start   starts (resumes) timer"
    "\n\t\t:   -stop    stops  (pauses)  timer"
    "\n\t\t:   -reset   resets timer progress"
    "\n\t\t:   -restart resets and starts timer"
    "\n\t\t:   -show    prints timer progress"
    "\n\t\t:           ('dchrono' puts into Tcl, 'chrono' puts into std::cout)"
    "\n\t\t:   -elapsed prints elapsed    time in seconds"
    "\n\t\t:   -userCPU prints user   CPU time in seconds"
    "\n\t\t:   -sysCPU  prints system CPU time in seconds"
    "\n\t\t:   -counter prints 'COUNTER <Text>'"
    "\n\t\t:   -thread  stops timer and sets measuring of CPU time for this thread only (FALSE by default)"
    "\n\t\t:   -process stops timer and sets measuring of CPU time for all  threads (TRUE by default)"
    "\n\t\t: Without arguments enables / disables global timer for all DRAW commands."
    "\n\t\t:   chrono {0|1}"
    "\n\t\t: Typical usage:"
    "\n\t\t:   chrono t -restart"
    "\n\t\t:   <algorithm>"
    "\n\t\t:   chrono t -stop -show";
  theCommands.Add("chrono", aChronoHelp,
		  __FILE__,dchronom,g);
  theCommands.Add("dchrono", aChronoHelp,
		  __FILE__,dchronom,g);
  theCommands.Add("mallochook",
                  "debug memory allocation/deallocation, w/o args for help",
                  __FILE__, mallochook, g);
  theCommands.Add ("meminfo",
    "meminfo [virt|v] [heap|h] [wset|w] [wsetpeak] [swap] [swappeak] [private]"
    " : memory counters for this process",
	  __FILE__, dmeminfo, g);
  theCommands.Add("dperf","dperf [reset] -- show performance counters, reset if argument is provided",
		  __FILE__,dperf,g);
  theCommands.Add("dsetsignal",
            "dsetsignal [{asIs|set|unhandled|unset}=set] [{0|1|default=$CSF_FPE}]"
    "\n\t\t:            [-strackTraceLength Length]"
    "\n\t\t: Sets OSD signal handler, with FPE option if argument is given."
    "\n\t\t:  -strackTraceLength specifies length of stack trace to put into exceptions redirected from signals.",
		  __FILE__,dsetsignal,g);

  theCommands.Add("dparallel",
    "dparallel [-occt {0|1}] [-nbThreads Count] [-nbDefThreads Count]"
    "\n\t\t: Manages global parallelization parameters:"
    "\n\t\t:   -occt         use OCCT implementation or external library (if available)"
    "\n\t\t:   -nbThreads    specify the number of threads in default thread pool"
    "\n\t\t:   -nbDefThreads specify the upper limit of threads to be used for default thread pool"
    "\n\t\t:                 within single parallelization call (should be <= of overall number of threads),"
    "\n\t\t:                 so that nested algorithm can also use this pool",
      __FILE__,dparallel,g);

  // Logging commands; note that their names are hard-coded in the code
  // of Draw_Interpretor, thus should not be changed without update of that code!
  theCommands.Add("dlog", "manage logging of commands and output; run without args to get help",
		  __FILE__,dlog,g);
  theCommands.Add("decho", "switch on / off echo of commands to cout; run without args to get help",
		  __FILE__,decho,g);
  theCommands.Add("dtracelevel", "dtracelevel [trace|info|warn|alarm|fail]",
                  __FILE__, dtracelevel, g);
  theCommands.Add("ddebugtraces",
    "ddebugtraces nbTraces"
    "\n\t\t: Sets the number of lines for the stack trace within Standard_Failure constructor."
    "\n\t\t: Intended for debug purposes.",
    __FILE__, ddebugtraces, g);

  theCommands.Add("dbreak", "raises Tcl exception if user has pressed Control-Break key",
		  __FILE__,dbreak,g);
  theCommands.Add("dversion", "provides information on OCCT build configuration (version, compiler, OS, C library, etc.)",
		  __FILE__,dversion,g);
  theCommands.Add("dlocale", "set and / or query locate of C subsystem (function setlocale())",
		  __FILE__,dlocale,g);

  theCommands.Add("dputs",
            "dputs [-intense] [-black|-white|-red|-green|-blue|-yellow|-cyan|-magenta]"
    "\n\t\t:       [-nonewline] [stdcout|stdcerr] text"
    "\n\t\t: Puts text into console output",
                  __FILE__,dputs,g);
}
