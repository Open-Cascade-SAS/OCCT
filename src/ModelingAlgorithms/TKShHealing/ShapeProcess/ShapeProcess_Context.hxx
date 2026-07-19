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

#ifndef _ShapeProcess_Context_HeaderFile
#define _ShapeProcess_Context_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_HAsciiString.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
class Resource_Manager;
class Message_Messenger;
class TCollection_AsciiString;

//! Provides convenient interface to resource file
//! Allows to load resource file and get values of
//! attributes starting from some scope, for example
//! if scope is defined as "ToV4" and requested parameter
//! is "exec.op", value of "ToV4.exec.op" parameter from
//! the resource file will be returned
class ShapeProcess_Context : public Standard_Transient
{

public:
  //! Creates an empty tool
  Standard_EXPORT ShapeProcess_Context();

  //! Creates a new tool and initialises by name of
  //! resource file and (if specified) starting scope
  //! Calls method Init()
  Standard_EXPORT ShapeProcess_Context(const char* const file, const char* const scope = "");

  //! Initialises a tool by loading resource file and
  //! (if specified) sets starting scope
  //! Returns False if resource file not found
  Standard_EXPORT bool Init(const char* const file, const char* const scope = "");

  //! Loading Resource_Manager object if this object not
  //! equal internal static Resource_Manager object or
  //! internal static Resource_Manager object is null
  Standard_EXPORT occ::handle<Resource_Manager> LoadResourceManager(const char* const file);

  //! Returns internal Resource_Manager object
  Standard_EXPORT const occ::handle<Resource_Manager>& ResourceManager() const;

  //! Set a new (sub)scope
  Standard_EXPORT void SetScope(const char* const scope);

  //! Go out of current scope
  Standard_EXPORT void UnSetScope();

  //! Returns True if parameter is defined in the resource file
  Standard_EXPORT bool IsParamSet(const char* const param) const;

  Standard_EXPORT bool GetReal(const char* const param, double& val) const;

  Standard_EXPORT bool GetInteger(const char* const param, int& val) const;

  Standard_EXPORT bool GetBoolean(const char* const param, bool& val) const;

  //! Get value of parameter as being of specific type
  //! Returns False if parameter is not defined or has a wrong type
  Standard_EXPORT bool GetString(const char* const param, TCollection_AsciiString& val) const;

  Standard_EXPORT double RealVal(const char* const param, const double def) const;

  Standard_EXPORT int IntegerVal(const char* const param, const int def) const;

  Standard_EXPORT bool BooleanVal(const char* const param, const bool def) const;

  //! Get value of parameter as being of specific type
  //! If parameter is not defined or does not have expected
  //! type, returns default value as specified
  Standard_EXPORT const char* StringVal(const char* const param, const char* const def) const;

  //! Sets Messenger used for outputting messages.
  Standard_EXPORT void SetMessenger(const occ::handle<Message_Messenger>& messenger);

  //! Returns Messenger used for outputting messages.
  Standard_EXPORT occ::handle<Message_Messenger> Messenger() const;

  //! Sets trace level used for outputting messages
  //! - 0: no trace at all
  //! - 1: errors
  //! - 2: errors and warnings
  //! - 3: all messages
  //! Default is 1 : Errors traced
  Standard_EXPORT void SetTraceLevel(const int tracelev);

  //! Returns trace level used for outputting messages.
  Standard_EXPORT int TraceLevel() const;

  DEFINE_STANDARD_RTTIEXT(ShapeProcess_Context, Standard_Transient)

private:
  occ::handle<Resource_Manager>                                             myRC;
  occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> myScope;
  occ::handle<Message_Messenger>                                            myMessenger;
  int                                                                       myTraceLev;
};

#endif // _ShapeProcess_Context_HeaderFile
