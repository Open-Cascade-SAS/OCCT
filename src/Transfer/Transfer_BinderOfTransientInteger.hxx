// Created on: 1995-02-27
// Created by: Christian CAILLET
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

#ifndef _Transfer_BinderOfTransientInteger_HeaderFile
#define _Transfer_BinderOfTransientInteger_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>


class Transfer_BinderOfTransientInteger;
DEFINE_STANDARD_HANDLE(Transfer_BinderOfTransientInteger, Transfer_SimpleBinderOfTransient)

//! This type of Binder allows to attach as result, besides a
//! Transient Object, an Integer Value, which can be an Index
//! in the Object if it defines a List, for instance
//!
//! This Binder is otherwise a kind of SimpleBinderOfTransient,
//! i.e. its basic result (for iterators, etc) is the Transient
class Transfer_BinderOfTransientInteger : public Transfer_SimpleBinderOfTransient
{

public:

  
  //! Creates an empty BinderOfTransientInteger; Default value for
  //! the integer part is zero
  Standard_EXPORT Transfer_BinderOfTransientInteger();
  
  //! Sets a value for the integer part
  Standard_EXPORT void SetInteger (const Standard_Integer value);
  
  //! Returns the value set for the integer part
  Standard_EXPORT Standard_Integer Integer() const;




  DEFINE_STANDARD_RTTIEXT(Transfer_BinderOfTransientInteger,Transfer_SimpleBinderOfTransient)

protected:




private:


  Standard_Integer theintval;


};







#endif // _Transfer_BinderOfTransientInteger_HeaderFile
