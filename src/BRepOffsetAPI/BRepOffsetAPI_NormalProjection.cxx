// Created on: 1997-10-13
// Created by: Roman BORISOV
// Copyright (c) 1997-1999 Matra Datavision
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
