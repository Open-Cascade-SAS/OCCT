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

#include <ShapeCustom_Curve.ixx>
#include <Geom_BSplineCurve.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <ShapeAnalysis_Curve.hxx>


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
#ifdef DEBUG
  cout << "Warning: ShapeCustom_Surface: Closed BSplineSurface is caused to be periodic" << endl;
#endif
  if ( ! converted ) return newCurve;
  newCurve = BSpl;
  if ( substitute ) myCurve = newCurve;
  return newCurve;
}
