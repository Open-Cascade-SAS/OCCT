// Created on: 1993-12-15
// Created by: Christophe MARION
// Copyright (c) 1993-1999 Matra Datavision
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

#include <BRepExtrema_ExtCF.hxx>

#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <gp_Pnt2d.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_Curve.hxx>

//=================================================================================================

BRepExtrema_ExtCF::BRepExtrema_ExtCF(const TopoDS_Edge& E, const TopoDS_Face& F)
{
  Initialize(E, F);
  Perform(E, F);
}

//=================================================================================================

void BRepExtrema_ExtCF::Initialize(const TopoDS_Edge& E, const TopoDS_Face& F)
{
  BRepAdaptor_Surface Surf(F);
  if (Surf.GetType() == GeomAbs_OtherSurface || !BRep_Tool::IsGeometric(E))
    return; // protect against non-geometric type (e.g. triangulation)
  BRepAdaptor_Curve aC(E);
  myHS = new BRepAdaptor_Surface(Surf);
  Standard_Real aTolC, aTolS;
  //
  aTolS = Min(BRep_Tool::Tolerance(F), Precision::Confusion());
  aTolS = Min(Surf.UResolution(aTolS), Surf.VResolution(aTolS));
  aTolS = Max(aTolS, Precision::PConfusion());
  //
  aTolC = Min(BRep_Tool::Tolerance(E), Precision::Confusion());
  aTolC = aC.Resolution(aTolC);
  aTolC = Max(aTolC, Precision::PConfusion());
  //
  Standard_Real U1, U2, V1, V2;
  BRepTools::UVBounds(F, U1, U2, V1, V2);
  myExtCS.Initialize(*myHS, U1, U2, V1, V2, aTolC, aTolS);
}

//=================================================================================================

void BRepExtrema_ExtCF::Perform(const TopoDS_Edge& E, const TopoDS_Face& F2)
{
  mySqDist.Clear();
  myPointsOnS.Clear();
  myPointsOnC.Clear();

  if (myHS.IsNull())
    return; // protect against non-geometric type (e.g. triangulation)

  Standard_Real U1, U2;
  BRep_Tool::Range(E, U1, U2);

  BRepAdaptor_Curve         Curv(E);
  Handle(BRepAdaptor_Curve) HC = new BRepAdaptor_Curve(Curv);
  myExtCS.Perform(*HC, U1, U2);

  if (!myExtCS.IsDone())
    return;

  if (myExtCS.IsParallel())
    mySqDist.Append(myExtCS.SquareDistance(1));
  else
  {
    // Exploration of points and classification
    const Standard_Real     Tol = BRep_Tool::Tolerance(F2);
    BRepTopAdaptor_FClass2d classifier(F2, Tol);

    // If the underlying surface of the face is periodic
    // Extrema should return the point within the period,
    // so there is no point to adjust it in classifier.
    Standard_Boolean isAdjustPeriodic = Standard_False;

    Extrema_POnCurv P1;
    Extrema_POnSurf P2;

    for (Standard_Integer i = 1; i <= myExtCS.NbExt(); i++)
    {
      myExtCS.Points(i, P1, P2);
      P2.Parameter(U1, U2);
      const gp_Pnt2d     Puv(U1, U2);
      const TopAbs_State state = classifier.Perform(Puv, isAdjustPeriodic);
      if (state == TopAbs_ON || state == TopAbs_IN)
      {
        mySqDist.Append(myExtCS.SquareDistance(i));
        myPointsOnC.Append(P1);
        myPointsOnS.Append(P2);
      }
    }
  }
}
