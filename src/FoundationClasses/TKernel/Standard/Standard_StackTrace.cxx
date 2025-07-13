// Created on: 2020-11-30
// Copyright (c) 2020 OPEN CASCADE SAS
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

#include <Standard.hxx>

#include <Message.hxx>
#include <Standard_Mutex.hxx>

#include <Standard_WarningDisableFunctionCast.hxx>

#if defined(__APPLE__)
  #import <TargetConditionals.h>
#endif

#if defined(__EMSCRIPTEN__)
  #include <emscripten/emscripten.h>
#elif defined(__ANDROID__)
// #include <unwind.h>
#elif defined(__QNX__)
// #include <backtrace.h> // requires linking to libbacktrace
#elif !defined(_WIN32) && !(defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE)
  #include <execinfo.h>
#elif defined(_WIN32) && !defined(OCCT_UWP)

  #include <Standard_WarningsDisable.hxx>
  #include <dbghelp.h>
  #include <Standard_WarningsRestore.hxx>

//! This is a wrapper of DbgHelp library loaded dynamically.
//! DbgHelp is coming with Windows SDK, so that technically it is always available.
//! However, it's usage requires extra steps:
//! - .pdb files are necessary to resolve function names;
//!   Normal release DLLs without PDBs will show no much useful info.
//! - DbgHelp.dll and friends (SymSrv.dll, SrcSrv.dll) should be packaged with application;
//!   DbgHelp.dll coming with system might be of other incompatible version
//!   (some applications load it dynamically to avoid packaging extra DLL,
//!    with a extra hacks determining library version)
class Standard_DbgHelper
{
public: // dbgHelp.dll function types
  typedef BOOL(WINAPI* SYMINITIALIZEPROC)(HANDLE, PCSTR, BOOL);
  typedef BOOL(WINAPI* STACKWALK64PROC)(DWORD,
                                        HANDLE,
                                        HANDLE,
                                        LPSTACKFRAME64,
                                        PVOID,
                                        PREAD_PROCESS_MEMORY_ROUTINE64,
                                        PFUNCTION_TABLE_ACCESS_ROUTINE64,
                                        PGET_MODULE_BASE_ROUTINE64,
                                        PTRANSLATE_ADDRESS_ROUTINE64);
  typedef BOOL(WINAPI* SYMCLEANUPPROC)(HANDLE);
  typedef BOOL(WINAPI* SYMFROMADDRPROC)(HANDLE, DWORD64, PDWORD64, PSYMBOL_INFO);

public:
  //! Return global instance.
  static Standard_DbgHelper& GetDbgHelper()
  {
    static Standard_DbgHelper aDbgHelper;
    return aDbgHelper;
  }

  //! Return global mutex.
  static Standard_Mutex& Mutex()
  {
    static Standard_Mutex THE_MUTEX_LOCK;
    return THE_MUTEX_LOCK;
  }

public:
  SYMINITIALIZEPROC                SymInitialize;
  SYMCLEANUPPROC                   SymCleanup;
  STACKWALK64PROC                  StackWalk64;
  PFUNCTION_TABLE_ACCESS_ROUTINE64 SymFunctionTableAccess64;
  PGET_MODULE_BASE_ROUTINE64       SymGetModuleBase64;
  SYMFROMADDRPROC                  SymFromAddr;

  //! Return TRUE if library has been loaded.
  bool IsLoaded() const { return myDbgHelpLib != NULL; }

  //! Return loading error message.
  const char* ErrorMessage() const { return myError; }

private:
  //! Main constructor.
  Standard_DbgHelper()
      : SymInitialize(NULL),
        SymCleanup(NULL),
        StackWalk64(NULL),
        SymFunctionTableAccess64(NULL),
        SymGetModuleBase64(NULL),
        SymFromAddr(NULL),
        myDbgHelpLib(LoadLibraryW(L"DbgHelp.dll")),
        myError(NULL)
  {
    if (myDbgHelpLib == NULL)
    {
      myError = "Standard_DbgHelper, Failed to load DbgHelp.dll";
      return;
    }

    if ((SymInitialize = (SYMINITIALIZEPROC)GetProcAddress(myDbgHelpLib, "SymInitialize")) == NULL)
    {
      myError = "Standard_DbgHelper, Function not found in DbgHelp.dll: SymInitialize";
      unload();
      return;
    }
    if ((SymCleanup = (SYMCLEANUPPROC)GetProcAddress(myDbgHelpLib, "SymCleanup")) == NULL)
    {
      myError = "Standard_DbgHelper, Function not found in DbgHelp.dll: SymCleanup";
      unload();
      return;
    }
    if ((StackWalk64 = (STACKWALK64PROC)GetProcAddress(myDbgHelpLib, "StackWalk64")) == NULL)
    {
      myError = "Standard_DbgHelper, Function not found in DbgHelp.dll: StackWalk64";
      unload();
      return;
    }
    if ((SymFunctionTableAccess64 =
           (PFUNCTION_TABLE_ACCESS_ROUTINE64)GetProcAddress(myDbgHelpLib,
                                                            "SymFunctionTableAccess64"))
        == NULL)
    {
      myError = "Standard_DbgHelper, Function not found in DbgHelp.dll: SymFunctionTableAccess64";
      unload();
      return;
    }
    if ((SymGetModuleBase64 =
           (PGET_MODULE_BASE_ROUTINE64)GetProcAddress(myDbgHelpLib, "SymGetModuleBase64"))
        == NULL)
    {
      myError = "Standard_DbgHelper, Function not found in DbgHelp.dll: SymGetModuleBase64";
      unload();
      return;
    }
    if ((SymFromAddr = (SYMFROMADDRPROC)GetProcAddress(myDbgHelpLib, "SymFromAddr")) == NULL)
    {
      myError = "Standard_DbgHelper, Function not found in DbgHelp.dll: SymFromAddr";
      unload();
      return;
    }
  }

  //! Destructor.
  ~Standard_DbgHelper()
  {
    // we could unload library here, but don't do it as it is kept loaded
    // unload();
  }

  //! Unload library.
  void unload()
  {
    if (myDbgHelpLib != NULL)
    {
      FreeLibrary(myDbgHelpLib);
      myDbgHelpLib = NULL;
    }
  }

private:
  Standard_DbgHelper(const Standard_DbgHelper&);
  Standard_DbgHelper& operator=(const Standard_DbgHelper&);

private:
  HMODULE     myDbgHelpLib; //!< handle to DbgHelp
  const char* myError;      //!< loading error message
};

#endif

//=================================================================================================

Standard_Boolean Standard::StackTrace(char*     theBuffer,
                                      const int theBufferSize,
                                      const int theNbTraces = 10,
                                      void*     theContext,
                                      const int theNbTopSkip)
{
  (void)theContext;
  if (theBufferSize < 1 || theNbTraces < 1 || theBuffer == NULL || theNbTopSkip < 0)
  {
    return false;
  }

#if defined(__EMSCRIPTEN__)
  // theNbTraces is ignored
  // EM_LOG_JS_STACK?
  return emscripten_get_callstack(EM_LOG_C_STACK | EM_LOG_DEMANGLE | EM_LOG_NO_PATHS
                                    | EM_LOG_FUNC_PARAMS,
                                  theBuffer,
                                  theBufferSize)
         > 0;
#elif defined(__ANDROID__)
  Message::SendTrace("Standard::StackTrace() is not implemented for this platform");
  return false;
#elif defined(__QNX__)
  // bt_get_backtrace()
  Message::SendTrace("Standard::StackTrace() is not implemented for this platform");
  return false;
#elif defined(OCCT_UWP) || (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE)
  Message::SendTrace("Standard::StackTrace() is not implemented for this platform");
  return false;
#elif defined(_WIN32)
  // Each CPU architecture requires manual stack frame setup,
  // and 32-bit version requires also additional hacks to retrieve current context;
  // this implementation currently covers only x86_64 architecture.
  #if defined(_M_X64)
  int          aNbTraces  = theNbTraces;
  const HANDLE anHProcess = GetCurrentProcess();
  const HANDLE anHThread  = GetCurrentThread();
  CONTEXT      aCtx;
  if (theContext != NULL)
  {
    memcpy(&aCtx, theContext, sizeof(aCtx));
  }
  else
  {
    ++aNbTraces;
    memset(&aCtx, 0, sizeof(aCtx));
    aCtx.ContextFlags = CONTEXT_FULL;
    RtlCaptureContext(&aCtx);
  }

  // DbgHelp is not thread-safe library, hence global lock is used for serial access
  Standard_Mutex::Sentry aSentry(Standard_DbgHelper::Mutex());
  Standard_DbgHelper&    aDbgHelp = Standard_DbgHelper::GetDbgHelper();
  if (!aDbgHelp.IsLoaded())
  {
    strcat_s(theBuffer, theBufferSize, "\n==Backtrace==\n");
    strcat_s(theBuffer, theBufferSize, aDbgHelp.ErrorMessage());
    strcat_s(theBuffer, theBufferSize, "\n=============");
    return false;
  }

  aDbgHelp.SymInitialize(anHProcess, NULL, TRUE);

  DWORD        anImage = 0;
  STACKFRAME64 aStackFrame;
  memset(&aStackFrame, 0, sizeof(aStackFrame));

  anImage                      = IMAGE_FILE_MACHINE_AMD64;
  aStackFrame.AddrPC.Offset    = aCtx.Rip;
  aStackFrame.AddrPC.Mode      = AddrModeFlat;
  aStackFrame.AddrFrame.Offset = aCtx.Rsp;
  aStackFrame.AddrFrame.Mode   = AddrModeFlat;
  aStackFrame.AddrStack.Offset = aCtx.Rsp;
  aStackFrame.AddrStack.Mode   = AddrModeFlat;

  char         aModBuffer[MAX_PATH] = {};
  char         aSymBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(CHAR)];
  SYMBOL_INFO* aSymbol  = (SYMBOL_INFO*)aSymBuffer;
  aSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
  aSymbol->MaxNameLen   = MAX_SYM_NAME;

  int aTopSkip = theNbTopSkip + 1; // skip this function call and specified extra number
  strcat_s(theBuffer, theBufferSize, "\n==Backtrace==");
  for (int aLineIter = 0; aLineIter < aNbTraces; ++aLineIter)
  {
    BOOL aRes = aDbgHelp.StackWalk64(anImage,
                                     anHProcess,
                                     anHThread,
                                     &aStackFrame,
                                     &aCtx,
                                     NULL,
                                     aDbgHelp.SymFunctionTableAccess64,
                                     aDbgHelp.SymGetModuleBase64,
                                     NULL);
    if (!aRes)
    {
      break;
    }

    if (theContext == NULL && aTopSkip > 0)
    {
      --aTopSkip;
      continue;
    }
    if (aStackFrame.AddrPC.Offset == 0)
    {
      break;
    }

    strcat_s(theBuffer, theBufferSize, "\n");

    const DWORD64 aModuleBase = aDbgHelp.SymGetModuleBase64(anHProcess, aStackFrame.AddrPC.Offset);
    if (aModuleBase != 0 && GetModuleFileNameA((HINSTANCE)aModuleBase, aModBuffer, MAX_PATH))
    {
      strcat_s(theBuffer, theBufferSize, aModBuffer);
    }

    DWORD64 aDisp = 0;
    strcat_s(theBuffer, theBufferSize, "(");
    if (aDbgHelp.SymFromAddr(anHProcess, aStackFrame.AddrPC.Offset, &aDisp, aSymbol))
    {
      strcat_s(theBuffer, theBufferSize, aSymbol->Name);
    }
    else
    {
      strcat_s(theBuffer, theBufferSize, "???");
    }
    strcat_s(theBuffer, theBufferSize, ")");
  }
  strcat_s(theBuffer, theBufferSize, "\n=============");

  aDbgHelp.SymCleanup(anHProcess);
  return true;
  #else
  Message::SendTrace("Standard::StackTrace() is not implemented for this CPU architecture");
  return false;
  #endif
#else
  const int aTopSkip  = theNbTopSkip + 1; // skip this function call and specified extra number
  int       aNbTraces = theNbTraces + aTopSkip;
  void**    aStackArr = (void**)alloca(sizeof(void*) * aNbTraces);
  if (aStackArr == NULL)
  {
    return false;
  }

  aNbTraces = ::backtrace(aStackArr, aNbTraces);
  if (aNbTraces <= 1)
  {
    return false;
  }

  aNbTraces -= aTopSkip;
  char** aStrings = ::backtrace_symbols(aStackArr + aTopSkip, aNbTraces);
  if (aStrings == NULL)
  {
    return false;
  }

  const size_t aLenInit = strlen(theBuffer);
  size_t       aLimit   = (size_t)theBufferSize - aLenInit - 1;
  if (aLimit > 14)
  {
    strcat(theBuffer, "\n==Backtrace==");
    aLimit -= 14;
  }
  for (int aLineIter = 0; aLineIter < aNbTraces; ++aLineIter)
  {
    const size_t aLen = strlen(aStrings[aLineIter]);
    if (aLen + 1 >= aLimit)
    {
      break;
    }

    strcat(theBuffer, "\n");
    strcat(theBuffer, aStrings[aLineIter]);
    aLimit -= aLen + 1;
  }
  free(aStrings);
  if (aLimit > 14)
  {
    strcat(theBuffer, "\n=============");
  }
  return true;
#endif
}
