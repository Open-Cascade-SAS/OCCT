// Created on: 1996-02-16
// Created by: Philippe MANGIN
// Copyright (c) 1996-1999 Matra Datavision
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


#include <DrawFairCurve_Batten.hxx>
#include <FairCurve_AnalysisCode.hxx>
#include <FairCurve_Batten.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <GeomTools_Curve2dSet.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DrawFairCurve_Batten,DrawTrSurf_BSplineCurve2d)

DrawFairCurve_Batten::DrawFairCurve_Batten(const Standard_Address TheBatten)
                     : DrawTrSurf_BSplineCurve2d( ((FairCurve_Batten*)TheBatten)->Curve()),
                       MyBatten( TheBatten)
                      
{
  Compute();
  ShowCurvature();
}

void DrawFairCurve_Batten::Compute()
{
  FairCurve_AnalysisCode Iana;
  ((FairCurve_Batten*)MyBatten)->Compute(Iana, 50, 1.0e-2);
  curv = ((FairCurve_Batten*)MyBatten)->Curve();
}
void DrawFairCurve_Batten::SetPoint(const Standard_Integer Side, const gp_Pnt2d& Point)
{
  if (Side == 1) {
    ((FairCurve_Batten*)MyBatten)->SetP1(Point);
   }
  else {
    ((FairCurve_Batten*)MyBatten)-> SetP2(Point);
   }
  Compute();
}

void DrawFairCurve_Batten::SetAngle(const Standard_Integer Side, const Standard_Real Angle)
{
  if (Side == 1) {
     ((FairCurve_Batten*)MyBatten)->SetAngle1(Angle*M_PI/180);
     if ( ((FairCurve_Batten*)MyBatten)->GetConstraintOrder1() == 0 ) 
     {
       ((FairCurve_Batten*)MyBatten)->SetConstraintOrder1(1);
     }
   }
  else {
     ((FairCurve_Batten*)MyBatten)->SetAngle2(Angle*M_PI/180);
     if ( ((FairCurve_Batten*)MyBatten)->GetConstraintOrder2() == 0 ) 
     {
     ((FairCurve_Batten*)MyBatten)->SetConstraintOrder2(1);
     }
   }
  Compute();
}

void DrawFairCurve_Batten::SetSliding(const Standard_Real Length)
{
  ((FairCurve_Batten*)MyBatten)-> SetFreeSliding(Standard_False);
  ((FairCurve_Batten*)MyBatten)->SetSlidingFactor(Length);
  Compute();
}

void DrawFairCurve_Batten::SetHeight(const Standard_Real Height)
{
 ((FairCurve_Batten*)MyBatten)->SetHeight(Height);
 Compute();
}

void DrawFairCurve_Batten::SetSlope(const Standard_Real Slope)
{
 ((FairCurve_Batten*)MyBatten)->SetSlope(Slope);
 Compute();
}

Standard_Real DrawFairCurve_Batten::GetAngle(const Standard_Integer Side) const 
{
 if (Side == 1) return ((FairCurve_Batten*)MyBatten)->GetAngle1();
 else           return ((FairCurve_Batten*)MyBatten)->GetAngle2();
}

Standard_Real DrawFairCurve_Batten::GetSliding() const 
{
   return ((FairCurve_Batten*)MyBatten)->GetSlidingFactor();
}


void DrawFairCurve_Batten::FreeSliding()
{
  ((FairCurve_Batten*)MyBatten)->SetFreeSliding(Standard_True);
  Compute();
}

void DrawFairCurve_Batten::FreeAngle(const Standard_Integer Side)
{
 if (Side == 1) ((FairCurve_Batten*)MyBatten)->SetConstraintOrder1(0);
 else           ((FairCurve_Batten*)MyBatten)->SetConstraintOrder2(0);

 Compute();
}

void DrawFairCurve_Batten::Dump(Standard_OStream& S)const 
{
  GeomTools_Curve2dSet::PrintCurve2d(curv,S);
  ((FairCurve_Batten*)MyBatten)->Dump(S);
}
