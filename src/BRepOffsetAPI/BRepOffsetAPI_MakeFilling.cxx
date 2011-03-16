// File:	BRepOffsetAPI_MakeFilling.cxx
// Created:	Wed Aug 26 12:55:20 1998
// Author:	Julia GERASIMOVA
//		<jgv@clubox.nnov.matra-dtv.fr>


#include <BRepOffsetAPI_MakeFilling.ixx>


BRepOffsetAPI_MakeFilling::BRepOffsetAPI_MakeFilling( const Standard_Integer Degree,
					  const Standard_Integer NbPtsOnCur,
					  const Standard_Integer NbIter,
					  const Standard_Boolean Anisotropie,
					  const Standard_Real Tol2d,
					  const Standard_Real Tol3d,
					  const Standard_Real TolAng,
					  const Standard_Real TolCurv,
					  const Standard_Integer MaxDeg,
					  const Standard_Integer MaxSegments ) :
myFilling( Degree, NbPtsOnCur, NbIter, Anisotropie, Tol2d, Tol3d, TolAng, TolCurv, MaxDeg, MaxSegments )
{
}

void BRepOffsetAPI_MakeFilling::SetConstrParam( const Standard_Real Tol2d,
					  const Standard_Real Tol3d,
					  const Standard_Real TolAng,
					  const Standard_Real TolCurv ) 
{
  myFilling.SetConstrParam( Tol2d, Tol3d, TolAng, TolCurv );
}

void BRepOffsetAPI_MakeFilling::SetResolParam( const Standard_Integer Degree,
					 const Standard_Integer NbPtsOnCur,
					 const Standard_Integer NbIter,
					 const Standard_Boolean Anisotropie ) 
{
  myFilling.SetResolParam( Degree, NbPtsOnCur, NbIter, Anisotropie );
}

void BRepOffsetAPI_MakeFilling::SetApproxParam( const Standard_Integer MaxDeg,
					  const Standard_Integer MaxSegments ) 
{
  myFilling.SetApproxParam( MaxDeg, MaxSegments );
}

void BRepOffsetAPI_MakeFilling::LoadInitSurface( const TopoDS_Face& Surf ) 
{
  myFilling.LoadInitSurface( Surf );
}

Standard_Integer BRepOffsetAPI_MakeFilling::Add( const TopoDS_Edge& Constr,
			       const GeomAbs_Shape Order,
			       const Standard_Boolean IsBound ) 
{
  return  myFilling.Add( Constr, Order, IsBound );
}

Standard_Integer BRepOffsetAPI_MakeFilling::Add( const TopoDS_Edge& Constr,
			       const TopoDS_Face& Support,
			       const GeomAbs_Shape Order,
			       const Standard_Boolean IsBound ) 
{
  return  myFilling.Add( Constr, Support, Order, IsBound );
}

Standard_Integer BRepOffsetAPI_MakeFilling::Add( const TopoDS_Face& Support,
			       const GeomAbs_Shape Order ) 
{
  return myFilling.Add( Support, Order );
}

Standard_Integer BRepOffsetAPI_MakeFilling::Add( const gp_Pnt& Point )
{
  return myFilling.Add( Point );
}

Standard_Integer BRepOffsetAPI_MakeFilling::Add( const Standard_Real U,
			       const Standard_Real V,
			       const TopoDS_Face& Support,
			       const GeomAbs_Shape Order )
{
  return myFilling.Add( U, V, Support, Order );
}


void BRepOffsetAPI_MakeFilling::Build() 
{
  myFilling.Build();
  myShape = myFilling.Face();
}

Standard_Boolean BRepOffsetAPI_MakeFilling::IsDone() const
{
  return myFilling.IsDone();
}

Standard_Real BRepOffsetAPI_MakeFilling::G0Error() const
{
  return myFilling.G0Error();
}

Standard_Real BRepOffsetAPI_MakeFilling::G1Error() const
{
  return myFilling.G1Error();
}

Standard_Real BRepOffsetAPI_MakeFilling::G2Error() const
{
  return myFilling.G2Error();
}

Standard_Real BRepOffsetAPI_MakeFilling::G0Error( const Standard_Integer Index )
{
  return myFilling.G0Error( Index );
}

Standard_Real BRepOffsetAPI_MakeFilling::G1Error( const Standard_Integer Index )
{
  return myFilling.G1Error( Index );
}

Standard_Real BRepOffsetAPI_MakeFilling::G2Error( const Standard_Integer Index )
{
  return myFilling.G2Error( Index );
}
