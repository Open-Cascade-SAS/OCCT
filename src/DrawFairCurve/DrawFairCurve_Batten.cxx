// Created on: 1996-02-16
// Created by: Philippe MANGIN
// Copyright (c) 1996-1999 Matra Datavision
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


#include <DrawFairCurve_Batten.ixx>

#include <FairCurve_Batten.hxx>
#include <FairCurve_AnalysisCode.hxx>
#include <GeomTools_Curve2dSet.hxx>

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
