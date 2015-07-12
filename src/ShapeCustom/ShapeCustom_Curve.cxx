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


#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <ShapeAnalysis_Curve.hxx>
#include <ShapeCustom_Curve.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>

ShapeCustom_Curve::ShapeCustom_Curve()
{
}

//=======================================================================
//function : ShapeCustom_Curve
//purpose  : 
//=======================================================================

ShapeCustom_Curve::ShapeCustom_Curve (const Handle(Geom_Curve)& C)
{
  Init ( C );
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void ShapeCustom_Curve::Init (const Handle(Geom_Curve)& C) 
{
  myCurve = C;
}
  
//=======================================================================
//function : ConvertToPeriodic
//purpose  : 
//=======================================================================

Handle(Geom_Curve) ShapeCustom_Curve::ConvertToPeriodic (const Standard_Boolean substitute,
                                                         const Standard_Real preci)
{
  Handle(Geom_Curve) newCurve;
  Handle(Geom_BSplineCurve) BSpl = Handle(Geom_BSplineCurve)::DownCast(myCurve);
  if (BSpl.IsNull()) return newCurve;
  
  // PTV 13.02.02: check if curve closed with tolerance
  Standard_Boolean closed = ShapeAnalysis_Curve::IsClosed(myCurve, preci);
  
  if ( ! closed ) return newCurve;
  
  Standard_Boolean converted = Standard_False; //:p6

  if ( closed && ! BSpl->IsPeriodic() && BSpl->NbPoles() >3 ) {
    Standard_Boolean set = Standard_True;
    // if degree+1 at ends, first change it to 1 by rearranging knots
    if ( BSpl->Multiplicity(1) == BSpl->Degree() + 1 &&
         BSpl->Multiplicity(BSpl->NbKnots()) == BSpl->Degree() + 1 ) {
      Standard_Integer nbPoles = BSpl->NbPoles();
      TColgp_Array1OfPnt oldPoles(1,nbPoles);
      TColStd_Array1OfReal oldWeights(1,nbPoles);
      Standard_Integer nbKnots = BSpl->NbKnots();
      TColStd_Array1OfReal oldKnots(1,nbKnots);
      TColStd_Array1OfInteger oldMults(1,nbKnots);
      
      BSpl->Poles(oldPoles);
      BSpl->Weights(oldWeights);
      BSpl->Knots(oldKnots);
      BSpl->Multiplicities(oldMults);
      
      TColStd_Array1OfReal newKnots (1,nbKnots+2);
      TColStd_Array1OfInteger newMults(1,nbKnots+2);
      Standard_Real a = 0.5 * ( BSpl->Knot(2) - BSpl->Knot(1) + 
			        BSpl->Knot(nbKnots) - BSpl->Knot(nbKnots-1) );
      
      newKnots(1) = oldKnots(1) - a;
      newKnots(nbKnots+2) = oldKnots(nbKnots) + a;
      newMults(1) = newMults(nbKnots+2) = 1;
      for (Standard_Integer i = 2; i<=nbKnots+1; i++) {
	newKnots(i) = oldKnots(i-1);
	newMults(i) = oldMults(i-1);
      }
      newMults(2) = newMults(nbKnots+1) = BSpl->Degree();
      Handle(Geom_BSplineCurve) res = new Geom_BSplineCurve(oldPoles, oldWeights,
                                                            newKnots,newMults,
                                                            BSpl->Degree(),BSpl->IsPeriodic());
      BSpl = res;
    }
    else if ( BSpl->Multiplicity(1) > BSpl->Degree() ||
	      BSpl->Multiplicity(BSpl->NbKnots()) > BSpl->Degree() + 1 ) set = Standard_False;
    if ( set ) {
      BSpl->SetPeriodic(); // make periodic
      converted = Standard_True;
    }
  }
#ifdef OCCT_DEBUG
  cout << "Warning: ShapeCustom_Surface: Closed BSplineSurface is caused to be periodic" << endl;
#endif
  if ( ! converted ) return newCurve;
  newCurve = BSpl;
  if ( substitute ) myCurve = newCurve;
  return newCurve;
}
