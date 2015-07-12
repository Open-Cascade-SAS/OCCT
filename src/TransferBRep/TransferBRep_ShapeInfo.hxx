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

#ifndef _TransferBRep_ShapeInfo_HeaderFile
#define _TransferBRep_ShapeInfo_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Type.hxx>
#include <Standard_CString.hxx>
class TopoDS_Shape;


//! Gives informations on an object, see template DataInfo
//! This class is for Shape
class TransferBRep_ShapeInfo 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns the Type attached to an object
  //! Here, TShape (Shape has no Dynamic Type)
  Standard_EXPORT static Handle(Standard_Type) Type (const TopoDS_Shape& ent);
  
  //! Returns Type Name (string)
  //! Here, the true name of the Type of a Shape
  Standard_EXPORT static Standard_CString TypeName (const TopoDS_Shape& ent);




protected:





private:





};







#endif // _TransferBRep_ShapeInfo_HeaderFile
