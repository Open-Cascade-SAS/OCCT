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
