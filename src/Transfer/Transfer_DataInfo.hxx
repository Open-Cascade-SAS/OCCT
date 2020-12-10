// Created on: 1996-09-04
// Created by: Christian CAILLET
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _Transfer_DataInfo_HeaderFile
#define _Transfer_DataInfo_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Type.hxx>
#include <Standard_CString.hxx>
class Standard_Transient;


//! Gives information on an object
//! Used as template to instantiate Mapper and SimpleBinder
//! This class is for Transient
class Transfer_DataInfo 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns the Type attached to an object
  //! Here, the Dynamic Type of a Transient. Null Type if unknown
  Standard_EXPORT static Handle(Standard_Type) Type (const Handle(Standard_Transient)& ent);
  
  //! Returns Type Name (string)
  //! Allows to name type of non-handled objects
  Standard_EXPORT static Standard_CString TypeName (const Handle(Standard_Transient)& ent);




protected:





private:





};







#endif // _Transfer_DataInfo_HeaderFile
