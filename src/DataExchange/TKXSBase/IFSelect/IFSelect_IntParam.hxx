// Created on: 1992-11-30
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _IFSelect_IntParam_HeaderFile
#define _IFSelect_IntParam_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_DefineAlloc.hxx>
#include <Standard_Transient.hxx>

class IFSelect_IntParam;
DEFINE_STANDARD_HANDLE(IFSelect_IntParam, Standard_Transient)

//! This class simply allows to access an Integer value through a
//! Handle, as a String can be (by using HString).
//! Hence, this value can be accessed : read and modified, without
//! passing through the specific object which detains it. Thus,
//! parameters of a Selection or a Dispatch (according its type)
//! can be controlled directly from the ShareOut which contains them
//!
//! Additionally, an IntParam can be bound to a Static.
//! Remember that for a String, binding is immediate, because the
//! string value of a Static is a HAsciiString, it then suffices
//! to get its Handle.
//! For an Integer, an IntParam can designate (by its name) a
//! Static : each time its value is required or set, the Static
//! is acknowledged
class IFSelect_IntParam : public Standard_Transient
{

public:
  //! Creates an IntParam. Initial value is set to zer
  Standard_EXPORT IFSelect_IntParam();

  //! Commands this IntParam to be bound to a Static
  //! Hence, Value will return the value if this Static if it is set
  //! Else, Value works on the locally stored value
  //! SetValue also will set the value of the Static
  //! This works only for a present static of type integer or enum
  //! Else, it is ignored
  //!
  //! If <statname> is empty, disconnects the IntParam from Static
  Standard_EXPORT void SetStaticName(const Standard_CString statname);

  //! Returns the name of static parameter to which this IntParam
  //! is bound, empty if none
  Standard_EXPORT Standard_CString StaticName() const;

  //! Reads Integer Value of the IntParam. If a StaticName is
  //! defined and the Static is set, looks in priority the value
  //! of the static
  Standard_EXPORT Standard_Integer Value() const;

  //! Sets a new Integer Value for the IntParam. If a StaticName is
  //! defined and the Static is set, also sets the value of the static
  Standard_EXPORT void SetValue(const Standard_Integer val);

  DEFINE_STANDARD_RTTIEXT(IFSelect_IntParam, Standard_Transient)

private:
  Standard_Integer        theval;
  TCollection_AsciiString thestn;
};

#endif // _IFSelect_IntParam_HeaderFile
