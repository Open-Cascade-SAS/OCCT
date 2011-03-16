// File:	BRepFill_EdgeFaceAndOrder.cxx
// Created:	Fri Oct  2 19:41:14 1998
// Author:	Julia GERASIMOVA
//		<jgv@redfox.nnov.matra-dtv.fr>


#include <BRepFill_EdgeFaceAndOrder.ixx>

BRepFill_EdgeFaceAndOrder::BRepFill_EdgeFaceAndOrder()
{
}

BRepFill_EdgeFaceAndOrder::BRepFill_EdgeFaceAndOrder( const TopoDS_Edge& anEdge,
						      const TopoDS_Face& aFace,
						      const GeomAbs_Shape anOrder )
{
  myEdge  = anEdge;
  myFace  = aFace;
  myOrder = anOrder;
}
