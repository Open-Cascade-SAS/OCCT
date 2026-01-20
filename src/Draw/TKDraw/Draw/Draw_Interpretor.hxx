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

#ifndef _Draw_Interpretor_HeaderFile
#define _Draw_Interpretor_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>
#include <Standard_Boolean.hxx>
#include <Draw_PInterp.hxx>
#include <Standard_SStream.hxx>
#include <Standard_Real.hxx>

class TCollection_AsciiString;
class TCollection_ExtendedString;

//! Provides an encapsulation of the TCL interpreter to define Draw commands.
class Draw_Interpretor
{

public:
  //! Global callback function definition
  typedef int (*CommandFunction)(Draw_Interpretor& theDI,
                                              int  theArgNb,
                                              const char**      theArgVec);

  //! Callback for TCL (interface)
  struct CallBackData
  {

    //! Main constructor
    CallBackData(Draw_Interpretor* theDI)
        : myDI(theDI)
    {
    }

    //! Destructor
    virtual ~CallBackData() {}

    //! Invoke function
    virtual int Invoke(Draw_Interpretor& theDI,
                                    int  theArgNb,
                                    const char**      theArgVec) = 0;

    Draw_Interpretor* myDI; //!< pointer to Draw Interpreter

    // make sure allocation and de-allocation is done by the same memory allocator
    DEFINE_STANDARD_ALLOC
  };

protected:
  //! Callback implementation for global function definition
  struct CallBackDataFunc : public CallBackData
  {

    CallBackDataFunc(Draw_Interpretor* theDI, CommandFunction theFunc)
        : CallBackData(theDI),
          myFunc(theFunc)
    {
    }

    virtual int Invoke(Draw_Interpretor& theDI,
                                    int  theArgNb,
                                    const char**      theArgVec)
    {
      return myFunc != NULL ? myFunc(theDI, theArgNb, theArgVec) : 1;
    }

    Draw_Interpretor::CommandFunction myFunc;
  };

  //! Callback implementation for class's method definition
  template <typename theObjHandle>
  struct CallBackDataMethod : public CallBackData
  {
    typedef typename theObjHandle::element_type element_type;
    typedef int (element_type::*methodType)(Draw_Interpretor&,
                                                         int,
                                                         const char**);

    CallBackDataMethod(Draw_Interpretor* theDI, const theObjHandle& theObjPtr, methodType theMethod)
        : CallBackData(theDI),
          myObjPtr(theObjPtr),
          myMethod(theMethod)
    {
    }

    virtual int Invoke(Draw_Interpretor& theDI,
                                    int  theArgNb,
                                    const char**      theArgVec)
    {
      return myMethod != NULL && !myObjPtr.IsNull()
               ? (myObjPtr.operator->()->*myMethod)(theDI, theArgNb, theArgVec)
               : 1;
    }

    theObjHandle myObjPtr;
    methodType   myMethod;
  };

public:
  //! Empty constructor
  Standard_EXPORT Draw_Interpretor();

  //! Initialize TCL interpreter
  Standard_EXPORT void Init();

  //! Creates a new command with name <theCommandName>, help string <theHelp> in group <theGroup>.
  //! @param theFunction callback implementation
  inline void Add(const char* theCommandName,
                  const char* theHelp,
                  CommandFunction  theFunction,
                  const char* theGroup = "User Commands")
  {
    Add(theCommandName, theHelp, "", theFunction, theGroup);
  }

  //! Creates a new command with name <theCommandName>, help string <theHelp> in group <theGroup>.
  //! @theFunction callback implementation
  //! @theFileName the name of the file that contains the implementation of the command
  inline void Add(const char* theCommandName,
                  const char* theHelp,
                  const char* theFileName,
                  CommandFunction  theFunction,
                  const char* theGroup = "User Commands")
  {
    CallBackDataFunc* aCallback = new CallBackDataFunc(this, theFunction);
    add(theCommandName, theHelp, theFileName, aCallback, theGroup);
  }

  //! Creates a new command with name <theCommandName>, help string <theHelp> in group <theGroup>.
  //! @param theObjPtr   callback class instance
  //! @param theMethod   callback implementation
  //! @param theFileName the name of the file that contains the implementation of the command
  template <typename theHandleType>
  inline void Add(
    const char*                                                         theCommandName,
    const char*                                                         theHelp,
    const char*                                                         theFileName,
    const theHandleType&                                                     theObjPtr,
    typename Draw_Interpretor::CallBackDataMethod<theHandleType>::methodType theMethod,
    const char*                                                         theGroup)
  {
    Draw_Interpretor::CallBackDataMethod<theHandleType>* aCallback =
      new Draw_Interpretor::CallBackDataMethod<theHandleType>(this, theObjPtr, theMethod);
    add(theCommandName, theHelp, theFileName, aCallback, theGroup);
  }

  //! Removes <theCommandName>, returns true if success (the command existed).
  Standard_EXPORT bool Remove(const char* const theCommandName);

public:
  Standard_EXPORT const char* Result() const;

  //! Resets the result to empty string
  Standard_EXPORT void Reset();

  //! Appends to the result
  Standard_EXPORT Draw_Interpretor& Append(const char* const theResult);

  inline Draw_Interpretor& operator<<(const char* const theResult)
  {
    return Append(theResult);
  }

  //! Appends to the result
  Standard_EXPORT Draw_Interpretor& Append(const TCollection_AsciiString& theResult);

  inline Draw_Interpretor& operator<<(const TCollection_AsciiString& theResult)
  {
    return Append(theResult);
  }

  //! Appends to the result
  Standard_EXPORT Draw_Interpretor& Append(const TCollection_ExtendedString& theResult);

  inline Draw_Interpretor& operator<<(const TCollection_ExtendedString& theResult)
  {
    return Append(theResult);
  }

  //! Appends to the result
  Standard_EXPORT Draw_Interpretor& Append(const int theResult);

  inline Draw_Interpretor& operator<<(const int theResult)
  {
    return Append(theResult);
  }

  //! Appends to the result
  Standard_EXPORT Draw_Interpretor& Append(const double theResult);

  inline Draw_Interpretor& operator<<(const double theResult) { return Append(theResult); }

  //! Appends to the result
  Standard_EXPORT Draw_Interpretor& Append(const Standard_SStream& theResult);

  inline Draw_Interpretor& operator<<(const Standard_SStream& theResult)
  {
    return Append(theResult);
  }

  //! Appends to the result the string as a list element
  Standard_EXPORT void AppendElement(const char* const theResult);

  //! Eval the script and returns OK = 0, ERROR = 1
  Standard_EXPORT int Eval(const char* const theScript);

  //! Eval the script and returns OK = 0, ERROR = 1
  //! Store the script in the history record.
  Standard_EXPORT int RecordAndEval(const char* const theScript,
                                                 const int theFlags = 0);

  //! Eval the content on the file and returns status
  Standard_EXPORT int EvalFile(const char* const theFileName);

  //! Eval the script "help command_name"
  Standard_EXPORT int PrintHelp(const char* const theCommandName);

  //! Returns True if the script is complete, no pending closing braces. (})
  Standard_EXPORT static bool Complete(const char* const theScript);

public:
  //! Destructor
  Standard_EXPORT ~Draw_Interpretor();

  Standard_EXPORT Draw_Interpretor(const Draw_PInterp& theInterp);

  Standard_EXPORT void Set(const Draw_PInterp& theInterp);

  Standard_EXPORT Draw_PInterp Interp() const;

  //! Enables or disables logging of all commands and their results
  Standard_EXPORT void SetDoLog(const bool theDoLog);

  //! Enables or disables eachoing of all commands and their results to cout
  Standard_EXPORT void SetDoEcho(const bool theDoEcho);

  //! Returns true if logging of commands is enabled
  Standard_EXPORT bool GetDoLog() const;

  //! Returns true if echoing of commands is enabled
  Standard_EXPORT bool GetDoEcho() const;

  //! Resets log (if opened) to zero size
  Standard_EXPORT void ResetLog();

  //! Writes a text string to the log (if opened);
  //! end of line is not appended
  Standard_EXPORT void AddLog(const char* const theStr);

  //! Returns current content of the log file as a text string
  Standard_EXPORT TCollection_AsciiString GetLog();

  //! Returns current value of the log file descriptor
  int GetLogFileDescriptor() { return myFDLog; }

  //! Return TRUE if console output should be colorized; TRUE by default.
  bool ToColorize() const { return myToColorize; }

  //! Set if console output should be colorized.
  Standard_EXPORT void SetToColorize(bool theToColorize);

protected:
  Standard_EXPORT void add(const char* theCommandName,
                           const char* theHelp,
                           const char* theFileName,
                           CallBackData*    theCallback,
                           const char* theGroup);

private:
  Draw_PInterp     myInterp;
  bool isAllocated;
  bool myDoLog;
  bool myDoEcho;
  bool myToColorize;
  int myFDLog; //!< file descriptor of log file

public:
  DEFINE_STANDARD_ALLOC
};

#endif // _Draw_Interpretor_HeaderFile
