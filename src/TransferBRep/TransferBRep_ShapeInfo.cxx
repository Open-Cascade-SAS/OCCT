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
