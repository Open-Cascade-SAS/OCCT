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

#ifndef _MoniTool_Element_HeaderFile
#define _MoniTool_Element_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <MoniTool_AttrList.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Type.hxx>
#include <Standard_CString.hxx>
class MoniTool_AttrList;


class MoniTool_Element;
DEFINE_STANDARD_HANDLE(MoniTool_Element, Standard_Transient)

//! a Element allows to map any kind of object as a Key for a Map.
//! This works by defining, for a Hash Code, that of the real Key,
//! not of the Element which acts only as an intermediate.
//! When a Map asks for the HashCode of a Element, this one returns
//! the code it has determined at creation time
class MoniTool_Element : public Standard_Transient
{

public:

  
  //! Returns the HashCode which has been stored by SetHashCode
  //! (remark that HashCode could be deferred then be defined by
  //! sub-classes, the result is the same)
  Standard_EXPORT Standard_Integer GetHashCode() const;
  
  //! Specific testof equallity : to be defined by each sub-class,
  //! must be False if Elements have not the same true Type, else
  //! their contents must be compared
  Standard_EXPORT virtual Standard_Boolean Equates (const Handle(MoniTool_Element)& other) const = 0;
  
  //! Returns the Type of the Value. By default, returns the
  //! DynamicType of <me>, but can be redefined
  Standard_EXPORT virtual Handle(Standard_Type) ValueType() const;
  
  //! Returns the name of the Type of the Value. Default is name
  //! of ValueType, unless it is for a non-handled object
  Standard_EXPORT virtual Standard_CString ValueTypeName() const;
  
  //! Returns (readonly) the Attribute List
  Standard_EXPORT const MoniTool_AttrList& ListAttr() const;
  
  //! Returns (modifiable) the Attribute List
  Standard_EXPORT MoniTool_AttrList& ChangeAttr();




  DEFINE_STANDARD_RTTIEXT(MoniTool_Element,Standard_Transient)

protected:

  
  //! Stores the HashCode which corresponds to the Value given to
  //! create the Mapper
  Standard_EXPORT void SetHashCode (const Standard_Integer code);



private:


  Standard_Integer thecode;
  MoniTool_AttrList theattrib;


};







#endif // _MoniTool_Element_HeaderFile
