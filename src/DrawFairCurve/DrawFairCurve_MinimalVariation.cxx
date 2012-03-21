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

#include <DrawFairCurve_MinimalVariation.ixx>

#include <FairCurve_MinimalVariation.hxx>
#include <Draw_ColorKind.hxx>

DrawFairCurve_MinimalVariation::DrawFairCurve_MinimalVariation(const Standard_Address TheMVC)
                               : DrawFairCurve_Batten(TheMVC)
                                 
{
 SetColor(Draw_jaune);
}

void DrawFairCurve_MinimalVariation::SetCurvature(const Standard_Integer Side,
						  const Standard_Real Rho)
{
  if (Side == 1) {
     ((FairCurve_MinimalVariation*)MyBatten)->SetCurvature1(Rho);
     ((FairCurve_MinimalVariation*)MyBatten)->SetConstraintOrder1(2);
   }
  else {
     ((FairCurve_MinimalVariation*)MyBatten)->SetCurvature2(Rho);
     ((FairCurve_MinimalVariation*)MyBatten)->SetConstraintOrder2(2);
   }
  Compute();
}

void DrawFairCurve_MinimalVariation::FreeCurvature(const Standard_Integer Side)
{
  if (Side == 1) {
     if ( ((FairCurve_MinimalVariation*)MyBatten)->GetConstraintOrder1()>1) 
       {
	 ((FairCurve_MinimalVariation*)MyBatten)->SetConstraintOrder1(1);
       }
   }
  else {
     if ( ((FairCurve_MinimalVariation*)MyBatten)->GetConstraintOrder2()>1) 
       {
	 ((FairCurve_MinimalVariation*)MyBatten)->SetConstraintOrder2(1);
       }
   }
  Compute();
}  


void DrawFairCurve_MinimalVariation::SetPhysicalRatio( const Standard_Real Ratio)
{
 ((FairCurve_MinimalVariation*)MyBatten)->SetPhysicalRatio(Ratio);
 Compute();
}

Standard_Real DrawFairCurve_MinimalVariation::GetCurvature(const Standard_Integer Side) const
{
 if (Side == 1) {return ((FairCurve_MinimalVariation*)MyBatten)->GetCurvature1();}
 else           {return ((FairCurve_MinimalVariation*)MyBatten)->GetCurvature2();}
}

Standard_Real DrawFairCurve_MinimalVariation::GetPhysicalRatio() const
{
  return ((FairCurve_MinimalVariation*)MyBatten)->GetPhysicalRatio();
}
