// Created on: 1999-12-20
// Created by: data exchange team
// Copyright (c) 1999 Matra Datavision
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

#ifndef _ShapeConstruct_MakeTriangulation_HeaderFile
#define _ShapeConstruct_MakeTriangulation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopoDS_Wire.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <TColgp_Array1OfPnt.hxx>

class ShapeConstruct_MakeTriangulation : public BRepBuilderAPI_MakeShape
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT ShapeConstruct_MakeTriangulation(const TColgp_Array1OfPnt& pnts,
                                                   const Standard_Real       prec = 0.0);

  Standard_EXPORT ShapeConstruct_MakeTriangulation(const TopoDS_Wire&  wire,
                                                   const Standard_Real prec = 0.0);

  Standard_EXPORT virtual void Build(
    const Message_ProgressRange& theRange = Message_ProgressRange()) Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Boolean IsDone() const Standard_OVERRIDE;

protected:
private:
  Standard_EXPORT void Triangulate(const TopoDS_Wire& wire);

  Standard_EXPORT void AddFacet(const TopoDS_Wire& wire);

  Standard_Real myPrecision;
  TopoDS_Wire   myWire;
};

#endif // _ShapeConstruct_MakeTriangulation_HeaderFile
