// Created on: 1998-07-22
// Created by: Philippe MANGIN
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _BRepFill_Section_HeaderFile
#define _BRepFill_Section_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Wire.hxx>
#include <TopoDS_Vertex.hxx>
#include <Standard_Boolean.hxx>


//! To store section definition
class BRepFill_Section 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepFill_Section();
  
  Standard_EXPORT BRepFill_Section(const TopoDS_Shape& Profile, const TopoDS_Vertex& V, const Standard_Boolean WithContact, const Standard_Boolean WithCorrection);
  
  Standard_EXPORT void Set (const Standard_Boolean IsLaw);
  
    const TopoDS_Shape& OriginalShape() const;
  
    const TopoDS_Wire& Wire() const;
  
    const TopoDS_Vertex& Vertex() const;
  
  Standard_EXPORT TopoDS_Shape ModifiedShape(const TopoDS_Shape& theShape) const;
  
    Standard_Boolean IsLaw() const;
  
    Standard_Boolean IsPunctual() const;
  
    Standard_Boolean WithContact() const;
  
    Standard_Boolean WithCorrection() const;




protected:





private:



  TopoDS_Shape myOriginalShape;
  TopoDS_Wire wire;
  TopoDS_Vertex vertex;
  Standard_Boolean islaw;
  Standard_Boolean ispunctual;
  Standard_Boolean contact;
  Standard_Boolean correction;

};


#include <BRepFill_Section.lxx>





#endif // _BRepFill_Section_HeaderFile
