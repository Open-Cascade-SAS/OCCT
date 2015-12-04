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


#include <Draw_ColorKind.hxx>
#include <DrawFairCurve_MinimalVariation.hxx>
#include <FairCurve_MinimalVariation.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DrawFairCurve_MinimalVariation,DrawFairCurve_Batten)

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
