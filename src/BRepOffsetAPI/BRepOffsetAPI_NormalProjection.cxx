// Created on: 1997-10-13
// Created by: Roman BORISOV
// Copyright (c) 1997-1999 Matra Datavision
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


#include <BRepOffsetAPI_NormalProjection.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <StdFail_NotDone.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>

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
