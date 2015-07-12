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


#include <TopoDS_Shape.hxx>
#include <TopoDS_TShape.hxx>
#include <TransferBRep_ShapeInfo.hxx>

Handle(Standard_Type)  TransferBRep_ShapeInfo::Type
  (const TopoDS_Shape& /*ent*/)
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
