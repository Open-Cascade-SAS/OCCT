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

#include <MoniTool_SignShape.ixx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_HShape.hxx>


MoniTool_SignShape::MoniTool_SignShape ()    {  }

    Standard_CString  MoniTool_SignShape::Name () const
      {  return "SHAPE";  }

    TCollection_AsciiString  MoniTool_SignShape::Text
  (const Handle(Standard_Transient)& ent,
   const Handle(Standard_Transient)& context) const
{
  if (ent.IsNull()) return "";
  Handle(TopoDS_HShape) HS = Handle(TopoDS_HShape)::DownCast(ent);
  if (HS.IsNull()) return ent->DynamicType()->Name();
  TopoDS_Shape sh = HS->Shape();
  if (sh.IsNull()) return "SHAPE";
  switch (sh.ShapeType()) {
    case TopAbs_VERTEX    : return "VERTEX";
    case TopAbs_EDGE      : return "EDGE";
    case TopAbs_WIRE      : return "WIRE";
    case TopAbs_FACE      : return "FACE";
    case TopAbs_SHELL     : return "SHELL";
    case TopAbs_SOLID     : return "SOLID";
    case TopAbs_COMPSOLID : return "COMPSOLID";
    case TopAbs_COMPOUND  : return "COMPOUND";
      default : break;
  }
  return "SHAPE";
}
