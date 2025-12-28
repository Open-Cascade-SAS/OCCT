// Created on: 2000-08-21
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Resource_Manager.hxx>
#include <ShapeProcess_Context.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>

#include <mutex>

#include <sys/stat.h>
IMPLEMENT_STANDARD_RTTIEXT(ShapeProcess_Context, Standard_Transient)

namespace
{
static std::mutex& GetShapeProcessMutex()
{
  static std::mutex THE_SHAPE_PROCESS_MUTEX;
  return THE_SHAPE_PROCESS_MUTEX;
}
} // namespace

//=================================================================================================

ShapeProcess_Context::ShapeProcess_Context()
{
  myMessenger = Message::DefaultMessenger();
  myTraceLev  = 1;
}

//=================================================================================================

ShapeProcess_Context::ShapeProcess_Context(const char* file, const char* scope)
{
  Init(file, scope);
  myMessenger = Message::DefaultMessenger();
  myTraceLev  = 1;
}

//=================================================================================================

bool ShapeProcess_Context::Init(const char* file, const char* scope)
{
  myScope.Nullify();
  if (file != nullptr && strlen(file) != 0)
  {
    myRC = LoadResourceManager(file);
  }
  else
  {
    myRC = new Resource_Manager();
  }
  if (scope && scope[0])
  {
    SetScope(scope);
  }
  return true; // myRC->Length() >0; NOT IMPLEMENTED
}

//=================================================================================================

occ::handle<Resource_Manager> ShapeProcess_Context::LoadResourceManager(const char* name)
{
  // Mutex is needed because we are initializing and changing static variables here, so
  // without mutex it leads to race condition.
  std::lock_guard<std::mutex> aLock(GetShapeProcessMutex());
  // Optimisation of loading resource file: file is load only once
  // and reloaded only if file date has changed
  static occ::handle<Resource_Manager> sRC;
  static std::time_t                   sMtime, sUMtime;
  static TCollection_AsciiString       sName;

  struct stat             buf;
  std::time_t             aMtime(0), aUMtime(0);
  TCollection_AsciiString aPath, aUserPath;
  Resource_Manager::GetResourcePath(aPath, name, false);
  Resource_Manager::GetResourcePath(aUserPath, name, true);
  if (!aPath.IsEmpty())
  {
    stat(aPath.ToCString(), &buf);
    aMtime = (std::time_t)buf.st_mtime;
  }
  if (!aUserPath.IsEmpty())
  {
    stat(aUserPath.ToCString(), &buf);
    aUMtime = (std::time_t)buf.st_mtime;
  }

  bool isFileModified = false;
  if (!sRC.IsNull())
  {
    if (sName.IsEqual(name))
    {
      if (sMtime != aMtime)
      {
        sMtime         = aMtime;
        isFileModified = true;
      }
      if (sUMtime != aUMtime)
      {
        sUMtime        = aUMtime;
        isFileModified = true;
      }
      if (isFileModified)
        sRC.Nullify();
    }
    else
      sRC.Nullify();
  }
  if (sRC.IsNull())
  {
#ifdef OCCT_DEBUG
    std::cout << "Info: ShapeProcess_Context: Reload Resource_Manager: " << sName.ToCString()
              << " -> " << name << std::endl;
#endif
    sRC = new Resource_Manager(name);
    if (!isFileModified)
    {
      sName   = name;
      sMtime  = aMtime;
      sUMtime = aUMtime;
    }
  }
  // Creating copy of sRC for thread safety of Resource_Manager variables
  // We should return copy because calling of Resource_Manager::SetResource() for one object
  // in multiple threads causes race condition
  return new Resource_Manager(*sRC);
}

//=================================================================================================

const occ::handle<Resource_Manager>& ShapeProcess_Context::ResourceManager() const
{
  return myRC;
}

//=================================================================================================

void ShapeProcess_Context::SetScope(const char* scope)
{
  if (myScope.IsNull())
    myScope = new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>;
  occ::handle<TCollection_HAsciiString> str;
  if (myScope->Length() > 0)
  {
    str = new TCollection_HAsciiString(myScope->Value(myScope->Length()));
    str->AssignCat(".");
    str->AssignCat(scope);
  }
  else
    str = new TCollection_HAsciiString(scope);
  myScope->Append(str);
}

//=================================================================================================

void ShapeProcess_Context::UnSetScope()
{
  if (!myScope.IsNull() && myScope->Length() > 0)
    myScope->Remove(myScope->Length());
}

//=================================================================================================

static occ::handle<TCollection_HAsciiString> MakeName(
  const occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>>& scope,
  const char*                                                                      param)
{
  occ::handle<TCollection_HAsciiString> str;
  if (!scope.IsNull() && scope->Length() > 0)
  {
    str = new TCollection_HAsciiString(scope->Value(scope->Length())->String());
    str->AssignCat(".");
    str->AssignCat(param);
  }
  else
    str = new TCollection_HAsciiString(param);
  return str;
}

bool ShapeProcess_Context::IsParamSet(const char* param) const
{
  return !myRC.IsNull() && myRC->Find(MakeName(myScope, param)->ToCString());
}

//=================================================================================================

bool ShapeProcess_Context::GetString(const char* param, TCollection_AsciiString& str) const
{
  if (myRC.IsNull())
    return false;
  occ::handle<TCollection_HAsciiString> pname = MakeName(myScope, param);
  if (!myRC->Find(pname->ToCString()))
  {
#ifdef OCCT_DEBUG
    std::cout << "Warning: ShapeProcess_Context::GetInteger(): Parameter " << pname->ToCString()
              << " is not defined" << std::endl;
#endif
    return false;
  }
  str = myRC->Value(pname->ToCString());
  return true;
}

//=================================================================================================

bool ShapeProcess_Context::GetReal(const char* param, double& val) const
{
  if (myRC.IsNull())
    return false;

  TCollection_AsciiString str;
  if (!GetString(param, str))
    return false;

  if (str.IsRealValue())
  {
    val = str.RealValue();
    return true;
  }

  // if not real, try to treat as alias "&param"
  str.LeftAdjust();
  if (str.Value(1) == '&')
  {
    TCollection_AsciiString ref = str.Split(1);
    ref.LeftAdjust();
    ref.RightAdjust();
    if (!myRC->Find(ref.ToCString()))
    {
#ifdef OCCT_DEBUG
      std::cout << "Warning: ShapeProcess_Context::GetInteger(): Parameter " << ref.ToCString()
                << " is not defined" << std::endl;
#endif
      return false;
    }
    str = myRC->Value(ref.ToCString());
    if (str.IsRealValue())
    {
      val = str.RealValue();
      return true;
    }
  }
#ifdef OCCT_DEBUG
  std::cout << "Warning: ShapeProcess_Context::GetInteger(): Parameter " << param
            << " is neither Real nor reference to Real";
#endif
  return false;
}

//=================================================================================================

bool ShapeProcess_Context::GetInteger(const char* param, int& val) const
{
  if (myRC.IsNull())
    return false;

  TCollection_AsciiString str;
  if (!GetString(param, str))
    return false;

  if (str.IsIntegerValue())
  {
    val = str.IntegerValue();
    return true;
  }

  // if not integer, try to treat as alias "&param"
  str.LeftAdjust();
  if (str.Value(1) == '&')
  {
    TCollection_AsciiString ref = str.Split(1);
    ref.LeftAdjust();
    ref.RightAdjust();
    if (!myRC->Find(ref.ToCString()))
    {
#ifdef OCCT_DEBUG
      std::cout << "Warning: ShapeProcess_Context::GetInteger(): Parameter " << ref.ToCString()
                << " is not defined" << std::endl;
#endif
      return false;
    }
    str = myRC->Value(ref.ToCString());
    if (str.IsIntegerValue())
    {
      val = str.IntegerValue();
      return true;
    }
  }
#ifdef OCCT_DEBUG
  std::cout << "Warning: ShapeProcess_Context::GetInteger(): Parameter " << param
            << " is neither Integer nor reference to Integer";
#endif
  return false;
}

//=================================================================================================

bool ShapeProcess_Context::GetBoolean(const char* param, bool& val) const
{
  if (myRC.IsNull())
    return false;
  try
  {
    OCC_CATCH_SIGNALS
    val = myRC->Integer(MakeName(myScope, param)->ToCString()) != 0;
    return true;
  }
  catch (Standard_Failure const& anException)
  {
#ifdef OCCT_DEBUG
    std::cout << "Warning: ShapeProcess_Context::GetInteger(): " << param << ": ";
    anException.Print(std::cout);
    std::cout << std::endl;
#endif
    (void)anException;
  }
  return false;
}

//=================================================================================================

double ShapeProcess_Context::RealVal(const char* param, const double def) const
{
  double val;
  return GetReal(param, val) ? val : def;
}

//=================================================================================================

bool ShapeProcess_Context::BooleanVal(const char* param, const bool def) const
{
  bool val;
  return GetBoolean(param, val) ? val : def;
}

//=================================================================================================

int ShapeProcess_Context::IntegerVal(const char* param, const int def) const
{
  int val;
  return GetInteger(param, val) ? val : def;
}

//=================================================================================================

const char* ShapeProcess_Context::StringVal(const char* param, const char* def) const
{
  if (myRC.IsNull())
    return def;
  try
  {
    OCC_CATCH_SIGNALS
    return myRC->Value(MakeName(myScope, param)->ToCString());
  }
  catch (Standard_Failure const& anException)
  {
#ifdef OCCT_DEBUG
    std::cout << "Warning: ShapeProcess_Context::GetInteger(): " << param << ": ";
    anException.Print(std::cout);
    std::cout << std::endl;
#endif
    (void)anException;
  }
  return def;
}

//=================================================================================================

void ShapeProcess_Context::SetMessenger(const occ::handle<Message_Messenger>& messenger)
{
  if (messenger.IsNull())
    myMessenger = Message::DefaultMessenger();
  else
    myMessenger = messenger;
}

//=================================================================================================

occ::handle<Message_Messenger> ShapeProcess_Context::Messenger() const
{
  return myMessenger;
}

//=================================================================================================

void ShapeProcess_Context::SetTraceLevel(const int tracelev)
{
  myTraceLev = tracelev;
}

//=================================================================================================

int ShapeProcess_Context::TraceLevel() const
{
  return myTraceLev;
}
