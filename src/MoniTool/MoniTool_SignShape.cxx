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


#include <MoniTool_SignShape.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_HShape.hxx>
#include <TopoDS_Shape.hxx>

MoniTool_SignShape::MoniTool_SignShape ()    {  }

    Standard_CString  MoniTool_SignShape::Name () const
      {  return "SHAPE";  }

    TCollection_AsciiString  MoniTool_SignShape::Text
  (const Handle(Standard_Transient)& ent,
   const Handle(Standard_Transient)& /*context*/) const
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
