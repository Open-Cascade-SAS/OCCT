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

// modified by mps (juillet 96 ): on utilise BRepAdaptor a la place de 
// GeomAdaptor dans Initialize et Perform.

#include <BRepExtrema_ExtFF.hxx>

#include <BRepExtrema_ExtCF.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <BRepAdaptor_HSurface.hxx>

//=======================================================================
//function : BRepExtrema_ExtFF
//purpose  : 
//=======================================================================

BRepExtrema_ExtFF::BRepExtrema_ExtFF(const TopoDS_Face& F1, const TopoDS_Face& F2)
{
 Initialize(F2);
 Perform(F1,F2);
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void BRepExtrema_ExtFF::Initialize(const TopoDS_Face& F2)
{
  BRepAdaptor_Surface Surf(F2);
  if (Surf.GetType() == GeomAbs_OtherSurface)
    return; // protect against non-geometric type (e.g. triangulation)

  myHS = new BRepAdaptor_HSurface(Surf);
  Standard_Real Tol = Min(BRep_Tool::Tolerance(F2), Precision::Confusion());
  Tol = Min(Surf.UResolution(Tol), Surf.VResolution(Tol));
  Tol = Max(Tol, Precision::PConfusion());
  Standard_Real U1, U2, V1, V2;
  BRepTools::UVBounds(F2, U1, U2, V1, V2);
  myExtSS.Initialize(myHS->Surface(), U1, U2, V1, V2, Tol);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepExtrema_ExtFF::Perform(const TopoDS_Face& F1, const TopoDS_Face& F2)
{ 
  mySqDist.Clear();
  myPointsOnS1.Clear();
  myPointsOnS2.Clear();

  BRepAdaptor_Surface Surf1(F1);
  if (myHS.IsNull() || Surf1.GetType() == GeomAbs_OtherSurface)
    return; // protect against non-geometric type (e.g. triangulation)

  Handle(BRepAdaptor_HSurface) HS1 = new BRepAdaptor_HSurface(Surf1);
  Standard_Real Tol1 = Min(BRep_Tool::Tolerance(F1), Precision::Confusion());
  Tol1 = Min(Surf1.UResolution(Tol1), Surf1.VResolution(Tol1));
  Tol1 = Max(Tol1, Precision::PConfusion());
  Standard_Real U1, U2, V1, V2;
  BRepTools::UVBounds(F1, U1, U2, V1, V2);
  myExtSS.Perform(HS1->Surface(), U1, U2, V1, V2, Tol1);

  if (!myExtSS.IsDone())
    return;

  if (myExtSS.IsParallel())
    mySqDist.Append(myExtSS.SquareDistance(1));
  else
  {
    // Exploration of points and classification
    BRepClass_FaceClassifier classifier;
    const Standard_Real Tol2 = BRep_Tool::Tolerance(F2);
    Extrema_POnSurf P1, P2;

    Standard_Integer i;
    for (i = 1; i <= myExtSS.NbExt(); i++)
    {
      myExtSS.Points(i, P1, P2);
      P1.Parameter(U1, U2);
      const gp_Pnt2d Puv1(U1, U2);
      classifier.Perform(F1, Puv1, Tol1);
      const TopAbs_State state1 = classifier.State();
      if (state1 == TopAbs_ON || state1 == TopAbs_IN)
      {
        P2.Parameter(U1, U2);
        const gp_Pnt2d Puv2(U1, U2);
        classifier.Perform(F2, Puv2, Tol2);
        const TopAbs_State state2 = classifier.State();
        if (state2 == TopAbs_ON || state2 == TopAbs_IN)
        {
          mySqDist.Append(myExtSS.SquareDistance(i));
          myPointsOnS1.Append(P1);
          myPointsOnS2.Append(P2);
        }
      }
    }
  }
}
