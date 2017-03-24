// Created by: Peter KURNEV
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


#include <Bnd_Box.hxx>
#include <BRepLib.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <IntTools_ShrunkRange.hxx>
#include <Precision.hxx>

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
  IntTools_ShrunkRange::IntTools_ShrunkRange ()
{
  myT1=-99;
  myT2=myT1;
  myTS1=myT1;
  myTS2=myT1;
  myIsDone=Standard_False;
  myIsSplittable=Standard_False;
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
IntTools_ShrunkRange::~IntTools_ShrunkRange () 
{
}
//=======================================================================
//function : SetData
//purpose  : 
//=======================================================================
void IntTools_ShrunkRange::SetData(const TopoDS_Edge& aE,
                                   const Standard_Real aT1,
                                   const Standard_Real aT2,
                                   const TopoDS_Vertex& aV1,
                                   const TopoDS_Vertex& aV2)
{
  myEdge=aE;
  myV1=aV1;
  myV2=aV2;
  myT1=aT1;
  myT2=aT2;
  myIsDone=Standard_False;
  myIsSplittable=Standard_False;
}
//=======================================================================
//function : SetContext
//purpose  : 
//=======================================================================
void IntTools_ShrunkRange::SetContext(const Handle(IntTools_Context)& aCtx)
{
  myCtx=aCtx;
}
//=======================================================================
//function : Context
//purpose  : 
//=======================================================================
const Handle(IntTools_Context)& IntTools_ShrunkRange::Context()const
{
  return myCtx;
}
//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================
const TopoDS_Edge& IntTools_ShrunkRange::Edge() const
{
  return myEdge;
}
//=======================================================================
//function : ShrunkRange
//purpose  : 
//=======================================================================
void IntTools_ShrunkRange::ShrunkRange(Standard_Real& aT1,
                                       Standard_Real& aT2) const
{
  aT1=myTS1;
  aT2=myTS2;
}
//=======================================================================
//function : BndBox
//purpose  : 
//=======================================================================
const Bnd_Box& IntTools_ShrunkRange::BndBox() const
{
  return myBndBox;
}
//=======================================================================
//function : SetShrunkRange
//purpose  : 
//=======================================================================
void IntTools_ShrunkRange::SetShrunkRange(const Standard_Real aT1,
                                          const Standard_Real aT2) 
{
  myTS1=aT1;
  myTS2=aT2;
  //
  BRepAdaptor_Curve aBAC(myEdge);
  BndLib_Add3dCurve::Add(aBAC, aT1, aT2, 0., myBndBox);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void IntTools_ShrunkRange::Perform()
{
  myIsDone = Standard_False;
  myIsSplittable = Standard_False;
  //
  // default tolerance - Precision::Confusion()
  Standard_Real aDTol = Precision::Confusion();
  // default parametric tolerance - Precision::PConfusion()
  Standard_Real aPDTol = Precision::PConfusion();
  //
  if (myT2 - myT1 < aPDTol) {
    return;
  }
  //
  gp_Pnt aP1 = BRep_Tool::Pnt(myV1);
  gp_Pnt aP2 = BRep_Tool::Pnt(myV2);
  Standard_Real aTolE, aTolV1, aTolV2;
  aTolE = BRep_Tool::Tolerance(myEdge);
  aTolV1 = BRep_Tool::Tolerance(myV1);
  aTolV2 = BRep_Tool::Tolerance(myV2);
  //
  if (aTolV1 < aTolE) {
    aTolV1 = aTolE;
  }
  //
  if (aTolV2 < aTolE) {
    aTolV2 = aTolE;
  }
  //
  // to have correspondence with intersection precision
  // the tolerances of vertices are increased on Precision::Confusion()
  aTolV1 += aDTol;
  aTolV2 += aDTol;

  // compute the shrunk range - part of the edge not covered
  // by the tolerance spheres of its vertices
  BRepAdaptor_Curve aBAC(myEdge);
  if (!BRepLib::FindValidRange(aBAC, aTolE, myT1, aP1, aTolV1,
                               myT2, aP2, aTolV2, myTS1, myTS2)) {
    // no valid range
    return;
  }
  if ((myTS2 - myTS1) < aPDTol) {
    // micro edge
    return;
  }
  //
  // compute the length of the edge on the shrunk range
  //
  // parametric tolerance for the edge
  // to be used in AbscissaPoint computations
  Standard_Real aPTolE = aBAC.Resolution(aTolE);
  // for the edges with big tolerance use 
  // min parametric tolerance - 1% of its range
  Standard_Real aPTolEMin = (myT2 - myT1) / 100.;
  if (aPTolE > aPTolEMin) {
    aPTolE = aPTolEMin;
  }
  Standard_Real anEdgeLength =
    GCPnts_AbscissaPoint::Length(aBAC, myTS1, myTS2, aPTolE);
  if (anEdgeLength < aDTol) {
    // micro edge
    return;
  }
  //
  myIsDone = Standard_True;
  //
  // check the shrunk range to have the length not less than
  // 2*aTolE+2*Precision::Confusion()
  // for the edge to have possibility to be split at least once:
  // 2*TolE - minimal diameter of tolerance sphere of splitting vertex
  // 2*Precision::Confusion() - minimal length of the new edges
  if (anEdgeLength > (2 * aTolE + 2 * aDTol)) {
    myIsSplittable = Standard_True;
  }
  //
  // build bounding box for the edge on the shrunk range
  BndLib_Add3dCurve::Add(aBAC, myTS1, myTS2,
      aTolE + aDTol, myBndBox);
}
