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
