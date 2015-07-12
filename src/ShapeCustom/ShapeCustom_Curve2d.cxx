// Created on: 2001-12-20
// Created by: Pavel TELKOV
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

// Last modification:

#include <ElCLib.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <ShapeCustom_Curve2d.hxx>
#include <Standard_ErrorHandler.hxx>

//=======================================================================
//function : GetLine
//purpose  : static
//=======================================================================
static gp_Lin2d GetLine (const gp_Pnt2d& P1, const gp_Pnt2d& P2,
                         const Standard_Real c1, Standard_Real& cf, Standard_Real& cl)
{
  gp_Vec2d avec (P1,P2);
  gp_Dir2d adir (avec);
  gp_Lin2d alin (P1,adir);
  alin.SetLocation (ElCLib::Value (c1, alin));
  cf = ElCLib::Parameter (alin, P1);
  cl = ElCLib::Parameter (alin, P2);
  return alin;
}


//=======================================================================
//function : IsLinear
//purpose  : static
//=======================================================================

Standard_Boolean ShapeCustom_Curve2d::IsLinear(const TColgp_Array1OfPnt2d& thePoles,
                                               const Standard_Real tolerance,
                                               Standard_Real& Deviation)
{
  Standard_Integer nbPoles = thePoles.Length();
  if(nbPoles < 2)
    return Standard_False;
  
  Standard_Real dMax = 0;
  Standard_Integer iMax1=0,iMax2=0;
  
  Standard_Integer i;
  for(i = 1; i < nbPoles; i++)
    for(Standard_Integer j = i+1; j <= nbPoles; j++) {
      Standard_Real dist = thePoles(i).SquareDistance(thePoles(j));
      if(dist > dMax) {
        dMax = dist;
        iMax1 = i;
        iMax2 = j;
      }
    }
  
  Standard_Real dPreci = Precision::PConfusion()*Precision::PConfusion();
  if(dMax < dPreci)
    return Standard_False;
  
  Standard_Real tol2 = tolerance*tolerance;
  gp_Vec2d avec (thePoles(iMax1),thePoles(iMax2));
  gp_Dir2d adir (avec);
  gp_Lin2d alin (thePoles(iMax1),adir);
  
  Standard_Real aMax = 0.;
  for(i = 1; i <= nbPoles; i++) {
    Standard_Real dist = alin.SquareDistance(thePoles(i));
    if(dist > tol2)
      return Standard_False;
    if(dist > aMax)
      aMax = dist;
  }
  Deviation = sqrt(aMax);
  
  return Standard_True;
}

//=======================================================================
//function : ConvertToLine2d
//purpose  : static
//=======================================================================

Handle(Geom2d_Line) ShapeCustom_Curve2d::ConvertToLine2d (const Handle(Geom2d_Curve)& theCurve,
                                                          const Standard_Real c1,
                                                          const Standard_Real c2,
                                                          const Standard_Real theTolerance,
                                                          Standard_Real& cf,
                                                          Standard_Real& cl,
                                                          Standard_Real& theDeviation)
{
  Handle(Geom2d_Line) aLine2d;
  gp_Pnt2d P1 = theCurve->Value(c1);
  gp_Pnt2d P2 = theCurve->Value(c2);
  Standard_Real dPreci = theTolerance*theTolerance;
  if(P1.SquareDistance(P2) < dPreci)
    return aLine2d; // it is not a line
  
  Handle(Geom2d_BSplineCurve) bsc = Handle(Geom2d_BSplineCurve)::DownCast(theCurve);
  if (!bsc.IsNull()) {
    Standard_Integer nbPoles = bsc->NbPoles();
    TColgp_Array1OfPnt2d Poles(1,nbPoles);
    bsc->Poles(Poles);
    if(!ShapeCustom_Curve2d::IsLinear(Poles,theTolerance,theDeviation))
      return aLine2d;  // non
    gp_Lin2d alin = GetLine (P1,P2,c1,cf,cl);
    aLine2d = new Geom2d_Line (alin);
    return aLine2d;
  }
  
  Handle(Geom2d_BezierCurve) bzc = Handle(Geom2d_BezierCurve)::DownCast(theCurve);
  if (!bzc.IsNull()) {
    Standard_Integer nbPoles = bzc->NbPoles();
    TColgp_Array1OfPnt2d Poles(1,nbPoles);
    bzc->Poles(Poles);
    if(!ShapeCustom_Curve2d::IsLinear(Poles,theTolerance,theDeviation))
      return aLine2d;  // non
    gp_Lin2d alin = GetLine (P1,P2,c1,cf,cl);
    aLine2d = new Geom2d_Line (alin);
    return aLine2d;
  }
  
  return aLine2d;
}

//=======================================================================
//function : SimplifyBSpline2d
//purpose  : 
//=======================================================================

Standard_Boolean ShapeCustom_Curve2d::SimplifyBSpline2d (Handle(Geom2d_BSplineCurve)& theBSpline2d,
                                                         const Standard_Real theTolerance)
{
  Standard_Integer aInitNbK;
  Standard_Integer NbK = aInitNbK = theBSpline2d->NbKnots();
  // search knot to remove
  Standard_Boolean IsToRemove = Standard_True;
  Standard_Integer aKnotIndx = NbK-1;
  while (IsToRemove && NbK > 2)
  {
    Standard_Integer aMult = theBSpline2d->Multiplicity(aKnotIndx);
    Standard_Integer DegMult = theBSpline2d->Degree() - aMult;
    if ((DegMult > 1) && theBSpline2d->IsCN(DegMult))
    {
      Standard_Real U = theBSpline2d->Knot(aKnotIndx);
      gp_Vec2d aVec1 = theBSpline2d->LocalDN(U, aKnotIndx-1, aKnotIndx, DegMult);
      gp_Vec2d aVec2 = theBSpline2d->LocalDN(U, aKnotIndx, aKnotIndx+1, DegMult);
      // check the derivations are have the "same" angle
      if (aVec1.IsParallel(aVec2, Precision::Angular()))
      {
        // remove knot
        try
        {
          OCC_CATCH_SIGNALS
          theBSpline2d->RemoveKnot(aKnotIndx,
                                 aMult-1,
                                 theTolerance);
        }
        catch(Standard_Failure)
        {
        }
      }
    }
    aKnotIndx--;
    
    NbK = theBSpline2d->NbKnots();
    if (aKnotIndx == 1 || aKnotIndx == NbK)
      IsToRemove = Standard_False;
  }
  return (aInitNbK > NbK);
}
