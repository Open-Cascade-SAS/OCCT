// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include  <TransferBRep_ShapeListBinder.ixx>
#include  <TopoDS.hxx>

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


