// File:	GeomFill_Generator.cxx
// Created:	Fri Feb 25 09:10:38 1994
// Author:	Bruno DUMORTIER
//		<dub@fuegox>

#include <GeomFill_Generator.ixx>

#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>

#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>

//=======================================================================
//function : GeomFill_Generator
//purpose  : 
//=======================================================================

GeomFill_Generator::GeomFill_Generator()
{
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void GeomFill_Generator::Perform(const Standard_Real PTol)
{
  // Perform the profile of the sections.
  GeomFill_Profiler::Perform( PTol);
  
  // Create the surface.
  
  Standard_Integer i,j;
  Standard_Integer NbUPoles = NbPoles();
  Standard_Integer NbVPoles = mySequence.Length();
  Standard_Integer NbUKnots = NbKnots();
  Standard_Integer NbVKnots = NbVPoles;
  Standard_Boolean isUPeriodic = IsPeriodic();
  Standard_Boolean isVPeriodic = Standard_False;
  
  TColgp_Array2OfPnt      Poles  ( 1, NbUPoles, 1, NbVPoles);
  TColStd_Array2OfReal    Weights( 1, NbUPoles, 1, NbVPoles);
  TColStd_Array1OfReal    UKnots ( 1, NbUKnots);
  TColStd_Array1OfReal    VKnots ( 1, NbVKnots);
  TColStd_Array1OfInteger UMults ( 1, NbUKnots);
  TColStd_Array1OfInteger VMults ( 1, NbVKnots);
  VMults.Init(1);
  
  VMults(1) = VMults(NbVKnots) = 2;

  KnotsAndMults( UKnots, UMults);

  TColgp_Array1OfPnt   Pole  ( 1, NbUPoles);
  TColStd_Array1OfReal Weight( 1, NbUPoles);
  for ( j = 1; j <= NbVPoles; j++) {
    Handle(Geom_BSplineCurve) Cj = 
      Handle(Geom_BSplineCurve)::DownCast(mySequence(j));
    Cj->Poles(Pole);
    Cj->Weights(Weight);
    VKnots(j) = (Standard_Real)(j-1);
    for ( i = 1; i <= NbUPoles; i++) {
      Poles(i,j)   = Pole  (i);
      Weights(i,j) = Weight(i);
    }
  } 
  
  mySurface = new Geom_BSplineSurface( Poles, Weights, UKnots, VKnots,
				       UMults, VMults, Degree(), 1, 
				       isUPeriodic, isVPeriodic);
}
