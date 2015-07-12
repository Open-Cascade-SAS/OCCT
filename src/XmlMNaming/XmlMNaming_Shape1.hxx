// Created on: 2001-09-14
// Created by: Alexander GRIGORIEV
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef _XmlMNaming_Shape1_HeaderFile
#define _XmlMNaming_Shape1_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <XmlObjMgt_Element.hxx>
#include <Standard_Integer.hxx>
#include <TopAbs_Orientation.hxx>
#include <XmlObjMgt_Document.hxx>
class TopoDS_Shape;


//! The XmlMNaming_Shape1 is the Persistent view of a TopoDS_Shape.
//!
//! a  Shape1 contains :
//! - a reference to a TShape
//! - a reference to Location
//! - an Orientation.
class XmlMNaming_Shape1 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT XmlMNaming_Shape1(XmlObjMgt_Document& Doc);
  
  Standard_EXPORT XmlMNaming_Shape1(const XmlObjMgt_Element& E);
  
  //! return myElement
  Standard_EXPORT const XmlObjMgt_Element& Element() const;
  
  //! return myElement
  Standard_EXPORT XmlObjMgt_Element& Element();
  
  Standard_EXPORT Standard_Integer TShapeId() const;
  
  Standard_EXPORT Standard_Integer LocId() const;
  
  Standard_EXPORT TopAbs_Orientation Orientation() const;
  
  Standard_EXPORT void SetShape (const Standard_Integer ID, const Standard_Integer LocID, const TopAbs_Orientation Orient);
  
  Standard_EXPORT void SetVertex (const TopoDS_Shape& theVertex);




protected:





private:



  XmlObjMgt_Element myElement;
  Standard_Integer myTShapeID;
  Standard_Integer myLocID;
  TopAbs_Orientation myOrientation;


};







#endif // _XmlMNaming_Shape1_HeaderFile
