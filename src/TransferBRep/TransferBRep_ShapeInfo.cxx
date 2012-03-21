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

#include <TransferBRep_ShapeInfo.ixx>
#include <TopoDS_TShape.hxx>


Handle(Standard_Type)  TransferBRep_ShapeInfo::Type
  (const TopoDS_Shape& ent)
      {  return STANDARD_TYPE(TopoDS_TShape);  }

    Standard_CString  TransferBRep_ShapeInfo::TypeName
  (const TopoDS_Shape& ent)
{
  if (ent.IsNull()) return "TopoDS_Shape";
  switch (ent.ShapeType()) {
    case TopAbs_VERTEX     : return "TopoDS_Vertex";
    case TopAbs_EDGE       : return "TopoDS_Edge";
    case TopAbs_WIRE       : return "TopoDS_Wire";
    case TopAbs_FACE       : return "TopoDS_Face";
    case TopAbs_SHELL      : return "TopoDS_Shell";
    case TopAbs_SOLID      : return "TopoDS_Solid";
    case TopAbs_COMPSOLID  : return "TopoDS_CompSolid";
    case TopAbs_COMPOUND   : return "TopoDS_Compound";
    default : break;
  }
  return "TopoDS_Shape";
}
