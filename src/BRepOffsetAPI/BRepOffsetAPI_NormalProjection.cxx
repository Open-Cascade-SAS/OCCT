// File:	BRepOffsetAPI_OrthogonalProjection.cxx
// Created:	Mon Oct 13 16:36:58 1997
// Author:	Roman BORISOV
//		<rbv@velox.nnov.matra-dtv.fr>

#include <BRepOffsetAPI_NormalProjection.ixx>

 BRepOffsetAPI_NormalProjection::BRepOffsetAPI_NormalProjection()
{
}

 BRepOffsetAPI_NormalProjection::BRepOffsetAPI_NormalProjection(const TopoDS_Shape& S)
{
  myNormalProjector.Init(S);
}

 void BRepOffsetAPI_NormalProjection::Init(const TopoDS_Shape& S) 
{
  myNormalProjector.Init(S);
}

 void BRepOffsetAPI_NormalProjection::Add(const TopoDS_Shape& ToProj) 
{
  myNormalProjector.Add(ToProj);
}

 void BRepOffsetAPI_NormalProjection::SetParams(const Standard_Real Tol3D,const Standard_Real Tol2D,const GeomAbs_Shape InternalContinuity,const Standard_Integer MaxDegree,const Standard_Integer MaxSeg) 
{
  myNormalProjector.SetParams(Tol3D, Tol2D, InternalContinuity, MaxDegree, MaxSeg);
}

 void BRepOffsetAPI_NormalProjection::SetMaxDistance(const Standard_Real MaxDist)
{
  myNormalProjector.SetMaxDistance(MaxDist);
}

 void BRepOffsetAPI_NormalProjection::SetLimit(const Standard_Boolean FaceBounds)
{
  myNormalProjector.SetLimit(FaceBounds);
}

 void BRepOffsetAPI_NormalProjection::Compute3d(const Standard_Boolean With3d)
{
  myNormalProjector.Compute3d(With3d);
}

 void BRepOffsetAPI_NormalProjection::Build() 
{
  myNormalProjector.Build();
  myShape = myNormalProjector.Projection();
  Done();
}

 Standard_Boolean BRepOffsetAPI_NormalProjection::IsDone() const
{
  return myNormalProjector.IsDone();
}

 const TopoDS_Shape& BRepOffsetAPI_NormalProjection::Couple(const TopoDS_Edge& E) const
{
  return myNormalProjector.Couple(E);
}

 const TopTools_ListOfShape& BRepOffsetAPI_NormalProjection::Generated(const TopoDS_Shape& S)
{
  return myNormalProjector.Generated(S);
}

 const TopoDS_Shape& BRepOffsetAPI_NormalProjection::Projection() const
{
  return myNormalProjector.Projection();
}

 const TopoDS_Shape& BRepOffsetAPI_NormalProjection::Ancestor(const TopoDS_Edge& E) const
{
  return myNormalProjector.Ancestor(E);
}

//=======================================================================
//function : BuildWire
//purpose  : 
//=======================================================================
 
 Standard_Boolean BRepOffsetAPI_NormalProjection::BuildWire(TopTools_ListOfShape& ListOfWire) const
{
  return myNormalProjector.BuildWire(ListOfWire);
}
