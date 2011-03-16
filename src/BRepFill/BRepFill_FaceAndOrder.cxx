// File:	BRepFill_FaceAndOrder.cxx
// Created:	Thu Sep 10 18:04:58 1998
// Author:	Julia GERASIMOVA
//		<jgv@redfox.nnov.matra-dtv.fr>


#include <BRepFill_FaceAndOrder.ixx>

BRepFill_FaceAndOrder::BRepFill_FaceAndOrder()
{
}

BRepFill_FaceAndOrder::BRepFill_FaceAndOrder( const TopoDS_Face& aFace,
					      const GeomAbs_Shape anOrder )
{
  myFace  = aFace;
  myOrder = anOrder;
}
