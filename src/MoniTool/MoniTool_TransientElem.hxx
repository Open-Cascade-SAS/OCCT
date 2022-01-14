// Created on: 1994-11-04
// Created by: Christian CAILLET
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _MoniTool_TransientElem_HeaderFile
#define _MoniTool_TransientElem_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <MoniTool_Element.hxx>
#include <Standard_CString.hxx>


class MoniTool_TransientElem;
DEFINE_STANDARD_HANDLE(MoniTool_TransientElem, MoniTool_Element)

//! an TransientElem defines an Element for a specific input class
//! its definition includes the value of the Key to be mapped,
//! and the HashCoder associated to the class of the Key
//!
//! Transient from Standard defines the class to be keyed
//! MapTransientHasher from TColStd is the associated Hasher
//! DataInfo from MoniTool   is an additional class which helps to provide
//! information on the value (template : see DataInfo)
class MoniTool_TransientElem : public MoniTool_Element
{

public:

  
  //! Creates a TransientElem with a Value. This Value can then not be
  //! changed. It is used by the Hasher to compute the HashCode,
  //! which will then be stored for an immediate reading.
  Standard_EXPORT MoniTool_TransientElem(const Handle(Standard_Transient)& akey);
  
  //! Returns the contained value
  Standard_EXPORT const Handle(Standard_Transient)& Value() const;
  
  //! Specific testof equality : defined as False if <other> has
  //! not the same true Type, else contents are compared (by
  //! C++ operator ==)
  Standard_EXPORT Standard_Boolean Equates (const Handle(MoniTool_Element)& other) const Standard_OVERRIDE;
  
  //! Returns the Type of the Value. By default, returns the
  //! DynamicType of <me>, but can be redefined
  Standard_EXPORT virtual Handle(Standard_Type) ValueType() const Standard_OVERRIDE;
  
  //! Returns the name of the Type of the Value. Default is name
  //! of ValueType, unless it is for a non-handled object
  Standard_EXPORT virtual Standard_CString ValueTypeName() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(MoniTool_TransientElem,MoniTool_Element)

protected:




private:


  Handle(Standard_Transient) theval;


};







#endif // _MoniTool_TransientElem_HeaderFile
