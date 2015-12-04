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


#include <Standard_OutOfRange.hxx>
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
#include <TransferBRep_ShapeListBinder.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TransferBRep_ShapeListBinder,Transfer_Binder)

TransferBRep_ShapeListBinder::TransferBRep_ShapeListBinder  ()
      {  theres = new TopTools_HSequenceOfShape();  }

    TransferBRep_ShapeListBinder::TransferBRep_ShapeListBinder
  (const Handle(TopTools_HSequenceOfShape)& list)
      {  theres = list;  }

    Standard_Boolean  TransferBRep_ShapeListBinder::IsMultiple () const
      {  return (NbShapes() > 1);  }

    Handle(Standard_Type)  TransferBRep_ShapeListBinder::ResultType () const
      {  return STANDARD_TYPE(TransferBRep_ShapeListBinder);  }

    Standard_CString  TransferBRep_ShapeListBinder::ResultTypeName () const
      {  return "list(TopoDS_Shape)";  }


    void  TransferBRep_ShapeListBinder::AddResult (const TopoDS_Shape& shape)
      {  theres->Append(shape);  }

    Handle(TopTools_HSequenceOfShape) TransferBRep_ShapeListBinder::Result
  () const
      {  return theres;  }

    void  TransferBRep_ShapeListBinder::SetResult
  (const Standard_Integer num, const TopoDS_Shape& shape)
      {  theres->SetValue(num,shape);  }

    Standard_Integer  TransferBRep_ShapeListBinder::NbShapes () const
      {  return theres->Length();  }

    const TopoDS_Shape&  TransferBRep_ShapeListBinder::Shape
  (const Standard_Integer num) const
      {  return theres->Value(num);  }

    TopAbs_ShapeEnum  TransferBRep_ShapeListBinder::ShapeType
  (const Standard_Integer num) const
      {  return theres->Value(num).ShapeType();  }

    TopoDS_Vertex  TransferBRep_ShapeListBinder::Vertex
  (const Standard_Integer num) const
      {  return TopoDS::Vertex(theres->Value(num));  }

    TopoDS_Edge    TransferBRep_ShapeListBinder::Edge
  (const Standard_Integer num) const
      {  return TopoDS::Edge(theres->Value(num));  }

    TopoDS_Wire    TransferBRep_ShapeListBinder::Wire
  (const Standard_Integer num) const
      {  return TopoDS::Wire(theres->Value(num));  }

    TopoDS_Face    TransferBRep_ShapeListBinder::Face
  (const Standard_Integer num) const
      {  return TopoDS::Face(theres->Value(num));  }

    TopoDS_Shell   TransferBRep_ShapeListBinder::Shell
  (const Standard_Integer num) const
      {  return TopoDS::Shell(theres->Value(num));  }

    TopoDS_Solid   TransferBRep_ShapeListBinder::Solid
  (const Standard_Integer num) const
      {  return TopoDS::Solid(theres->Value(num));  }

    TopoDS_CompSolid  TransferBRep_ShapeListBinder::CompSolid
  (const Standard_Integer num) const
      {  return TopoDS::CompSolid(theres->Value(num));  }

    TopoDS_Compound  TransferBRep_ShapeListBinder::Compound
  (const Standard_Integer num) const
      {  return TopoDS::Compound(theres->Value(num));  }


