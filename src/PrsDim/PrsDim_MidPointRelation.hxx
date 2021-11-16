// Created on: 2000-10-20
// Created by: Julia DOROVSKIKH
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _PrsDim_MidPointRelation_HeaderFile
#define _PrsDim_MidPointRelation_HeaderFile

#include <PrsDim_Relation.hxx>

class Geom_Plane;
class gp_Lin;
class gp_Circ;
class gp_Elips;

DEFINE_STANDARD_HANDLE(PrsDim_MidPointRelation, PrsDim_Relation)

//! presentation of equal distance to point myMidPoint
class PrsDim_MidPointRelation : public PrsDim_Relation
{
  DEFINE_STANDARD_RTTIEXT(PrsDim_MidPointRelation, PrsDim_Relation)
public:

  Standard_EXPORT PrsDim_MidPointRelation(const TopoDS_Shape& aSymmTool, const TopoDS_Shape& FirstShape, const TopoDS_Shape& SecondShape, const Handle(Geom_Plane)& aPlane);

  virtual Standard_Boolean IsMovable() const Standard_OVERRIDE { return Standard_True; }

  void SetTool (const TopoDS_Shape& aMidPointTool) { myTool = aMidPointTool; }

  const TopoDS_Shape& GetTool() const { return myTool; }

private:

  Standard_EXPORT virtual void Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                        const Handle(Prs3d_Presentation)& thePrs,
                                        const Standard_Integer theMode) Standard_OVERRIDE;

  Standard_EXPORT virtual void ComputeSelection (const Handle(SelectMgr_Selection)& theSel,
                                                 const Standard_Integer theMode) Standard_OVERRIDE;

  Standard_EXPORT void ComputeFaceFromPnt (const Handle(Prs3d_Presentation)& aprs, const Standard_Boolean first);

  Standard_EXPORT void ComputeEdgeFromPnt (const Handle(Prs3d_Presentation)& aprs, const Standard_Boolean first);

  Standard_EXPORT void ComputeVertexFromPnt (const Handle(Prs3d_Presentation)& aprs, const Standard_Boolean first);

  Standard_EXPORT void ComputePointsOnLine (const gp_Lin& aLin, const Standard_Boolean first);

  Standard_EXPORT void ComputePointsOnLine (const gp_Pnt& pnt1, const gp_Pnt& pnt2, const Standard_Boolean first);

  Standard_EXPORT void ComputePointsOnCirc (const gp_Circ& aCirc, const gp_Pnt& pnt1, const gp_Pnt& pnt2, const Standard_Boolean first);

  //! ComputePointsOn... methods set myFAttach, myFirstPnt and myLastPnt
  //! from the following initial data: curve, end points, myMidPoint.
  //! End points (pnt1 & pnt2) and curve define the trimmed curve.
  //! If end points are equal, curve is not trimmed (line - special case).
  //!
  //! .------. pnt2
  //! /        
  //! .  circle  . myLastPnt
  //! |          |
  //! . pnt1     . myFAttach
  //! \   arc  /          . myMidPoint
  //! .______. myFirstPnt
  Standard_EXPORT void ComputePointsOnElips (const gp_Elips& anEll, const gp_Pnt& pnt1, const gp_Pnt& pnt2, const Standard_Boolean first);

private:

  TopoDS_Shape myTool;
  gp_Pnt myMidPoint;
  gp_Pnt myFAttach;
  gp_Pnt myFirstPnt1;
  gp_Pnt myFirstPnt2;
  gp_Pnt mySAttach;
  gp_Pnt mySecondPnt1;
  gp_Pnt mySecondPnt2;

};

#endif // _AIS_MidPointRelation_HeaderFile
