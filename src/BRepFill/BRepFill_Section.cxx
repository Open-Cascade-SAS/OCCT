// File:	BRepFill_Section.cxx
// Created:	Wed Jul 22 10:50:48 1998
// Author:	Philippe MANGIN
//		<pmn@sgi29>


#include <BRepFill_Section.ixx>

#include <TopoDS.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Edge.hxx>


BRepFill_Section::BRepFill_Section() :islaw(0), contact(0), correction(0)
{
}


BRepFill_Section::BRepFill_Section(const TopoDS_Shape& Profile,
				   const TopoDS_Vertex& V,
				   const Standard_Boolean WithContact,
				   const Standard_Boolean WithCorrection) 
                                  : vertex(V),
				    islaw(0),contact(WithContact),
				    correction(WithCorrection)
{
  if (Profile.ShapeType() == TopAbs_WIRE)
    wire = TopoDS::Wire(Profile);
  else if (Profile.ShapeType() == TopAbs_VERTEX)
    {
      TopoDS_Vertex aVertex = TopoDS::Vertex(Profile);
      BRep_Builder BB;
      
      TopoDS_Edge DegEdge;
      BB.MakeEdge( DegEdge );
      BB.Add( DegEdge, aVertex.Oriented(TopAbs_FORWARD) );
      BB.Add( DegEdge, aVertex.Oriented(TopAbs_REVERSED) );
      BB.Degenerated( DegEdge, Standard_True );
      DegEdge.Closed( Standard_True );
      
      BB.MakeWire( wire );
      BB.Add( wire, DegEdge );
      wire.Closed( Standard_True );
    }
  else
    Standard_Failure::Raise("BRepFill_Section: bad shape type of section");
}

void BRepFill_Section::Set(const Standard_Boolean IsLaw)
{
  islaw = IsLaw;
}
