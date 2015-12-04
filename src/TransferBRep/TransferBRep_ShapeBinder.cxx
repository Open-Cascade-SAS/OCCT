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


#include <Standard_Type.hxx>
#include <Standard_TypeMismatch.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TransferBRep_ShapeBinder.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TransferBRep_ShapeBinder,TransferBRep_BinderOfShape)

TransferBRep_ShapeBinder::TransferBRep_ShapeBinder ()    {  }

    TransferBRep_ShapeBinder::TransferBRep_ShapeBinder (const TopoDS_Shape& shape)
    :  TransferBRep_BinderOfShape (shape)    {  }

    TopAbs_ShapeEnum  TransferBRep_ShapeBinder::ShapeType () const
{
  if (!HasResult()) return TopAbs_SHAPE;
  return  Result().ShapeType();
}

    TopoDS_Vertex  TransferBRep_ShapeBinder::Vertex() const
      {  return TopoDS::Vertex(Result());  }

    TopoDS_Edge  TransferBRep_ShapeBinder::Edge() const
      {  return TopoDS::Edge(Result());  }

    TopoDS_Wire  TransferBRep_ShapeBinder::Wire() const
      {  return TopoDS::Wire(Result());  }

    TopoDS_Face  TransferBRep_ShapeBinder::Face() const
      {  return TopoDS::Face(Result());  }

    TopoDS_Shell  TransferBRep_ShapeBinder::Shell() const
      {  return TopoDS::Shell(Result());  }

    TopoDS_Solid  TransferBRep_ShapeBinder::Solid() const
      {  return TopoDS::Solid(Result());  }

    TopoDS_CompSolid  TransferBRep_ShapeBinder::CompSolid() const
      {  return TopoDS::CompSolid(Result());  }

    TopoDS_Compound  TransferBRep_ShapeBinder::Compound() const
      {  return TopoDS::Compound(Result());  }
