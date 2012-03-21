// Created on: 1998-11-12
// Created by: data exchange team
// Copyright (c) 1998-1999 Matra Datavision
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

//    abv 24.05.99 S4174: methods Debug() commented

#include <ShapeUpgrade.ixx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <BSplCLib.hxx>
#include <TColStd_Array1OfInteger.hxx>

/*
// Debug state= True / False.
static Standard_Boolean Dbg=Standard_False;

void ShapeUpgrade::SetDebug(const Standard_Boolean State) 
{
  Dbg=State;
}

Standard_Boolean ShapeUpgrade::Debug() 
{
  return Dbg;
}
*/

//=======================================================================
//function : C0BSplineToSequenceOfC1BSplineCurve
//purpose  : 
//=======================================================================

 Standard_Boolean ShapeUpgrade::C0BSplineToSequenceOfC1BSplineCurve(const Handle(Geom_BSplineCurve)& BS,
	 							    Handle(TColGeom_HSequenceOfBoundedCurve)& seqBS) 
{
  if (BS.IsNull() || (BS->IsCN (1))) return Standard_False;
  
  seqBS = new TColGeom_HSequenceOfBoundedCurve;
  BS->SetNotPeriodic(); //to have equation NbPoles = NbKnots with Multiplicities - degree - 1
  
  Standard_Integer deg     = BS->Degree();
  Standard_Integer NbKnots = BS->NbKnots();
  Standard_Integer NbPoles = BS->NbPoles();
  TColgp_Array1OfPnt      Poles        (1, NbPoles);
  TColStd_Array1OfReal    Weights      (1, NbPoles);
  TColStd_Array1OfReal    Knots        (1, NbKnots);
  TColStd_Array1OfInteger Mults        (1, NbKnots);
  TColStd_Array1OfReal    KnotSequence (1, NbPoles + deg + 1);

  BS->Poles(Poles);
  if (BS->IsRational())
    BS->Weights(Weights);
  else
    Weights.Init(1.);
  BS->Knots(Knots);
  BS->Multiplicities(Mults);
  BS->KnotSequence (KnotSequence);
	
  Standard_Integer StartKnotIndex, EndKnotIndex, j;

  StartKnotIndex = BS->FirstUKnotIndex();
  for ( EndKnotIndex = StartKnotIndex + 1; EndKnotIndex <= BS->LastUKnotIndex(); EndKnotIndex++ ) {
    if ( ( Mults (EndKnotIndex) < deg ) && ( EndKnotIndex < BS->LastUKnotIndex() ) ) continue;

    Standard_Integer StartFlatIndex = BSplCLib::FlatIndex (deg, StartKnotIndex, Mults, Standard_False);
//    StartFlatIndex += Mults (StartKnotIndex) - 1;
    Standard_Integer EndFlatIndex   = BSplCLib::FlatIndex (deg, EndKnotIndex,   Mults, Standard_False);
    EndFlatIndex -= Mults (EndKnotIndex) - 1;

    TColStd_Array1OfReal    TempKnots (1, NbKnots);
    TColStd_Array1OfInteger TempMults (1, NbKnots);
    TempMults.Init (1);
    Standard_Integer TempKnotIndex = 1;
    TempKnots (TempKnotIndex) = KnotSequence (StartFlatIndex - deg);

    for ( j = StartFlatIndex - deg + 1; j <= EndFlatIndex + deg; j++ )
      if (Abs (KnotSequence (j) - KnotSequence (j - 1)) <= gp::Resolution())
	TempMults (TempKnotIndex) ++;
      else
	TempKnots (++TempKnotIndex) = KnotSequence (j);
    
    Standard_Integer TempStartIndex = 1, TempEndIndex = TempKnotIndex;
    if (TempMults (TempStartIndex) == 1)
      TempMults (++TempStartIndex) ++;
    if (TempMults (TempEndIndex) == 1)
      TempMults (--TempEndIndex) ++;

    Standard_Integer NewNbKnots = TempEndIndex - TempStartIndex + 1;
    TColStd_Array1OfInteger newMults (1, NewNbKnots);
    TColStd_Array1OfReal    newKnots (1, NewNbKnots);
    for ( j = 1; j <= NewNbKnots; j++ ) {
      newMults (j) = TempMults (j + TempStartIndex - 1);
      newKnots (j) = TempKnots (j + TempStartIndex - 1);
    }
	
    Standard_Integer NewNbPoles = BSplCLib::NbPoles(deg, Standard_False, newMults);
    TColgp_Array1OfPnt   newPoles (1, NewNbPoles);
    TColStd_Array1OfReal newWeights (1, NewNbPoles);
    Standard_Integer PoleIndex = StartFlatIndex - deg;//Index of starting pole when splitting B-Spline is an index of starting knot
    for (j = 1; j <= NewNbPoles; j++) {
      newWeights (j) = Weights (j + PoleIndex - 1);
      newPoles   (j) = Poles   (j + PoleIndex - 1);
    }
	
    Handle(Geom_BSplineCurve) newC = new Geom_BSplineCurve
      (newPoles, newWeights, newKnots, newMults,deg);
    seqBS->Append (newC);
    
    StartKnotIndex = EndKnotIndex;
  }
  return Standard_True;
}

//=======================================================================
//function : C0BSplineToSequenceOfC1BSplineCurve
//purpose  : 
//=======================================================================

static Handle(Geom_BSplineCurve) BSplineCurve2dTo3d (const Handle(Geom2d_BSplineCurve)& BS)
{
  Standard_Integer deg     = BS->Degree();
  Standard_Integer NbKnots = BS->NbKnots();
  Standard_Integer NbPoles = BS->NbPoles();
  TColgp_Array1OfPnt2d    Poles2d (1,NbPoles);
  TColStd_Array1OfReal    Weights (1,NbPoles);
  TColStd_Array1OfReal    Knots   (1,NbKnots);
  TColStd_Array1OfInteger Mults   (1,NbKnots);

  BS->Poles(Poles2d);
  if (BS->IsRational())
    BS->Weights(Weights);
  else
    Weights.Init (1);
  BS->Knots (Knots);
  BS->Multiplicities (Mults);

  TColgp_Array1OfPnt Poles3d (1,NbPoles);
  for (Standard_Integer i = 1; i <= NbPoles; i++)
    Poles3d (i) = gp_Pnt (Poles2d (i).X(), Poles2d (i).Y(), 0);
  
  Handle(Geom_BSplineCurve) BS3d = new Geom_BSplineCurve (Poles3d, Weights,
							  Knots, Mults, deg, BS->IsPeriodic());
  return BS3d;
}

static Handle(Geom2d_BSplineCurve) BSplineCurve3dTo2d (const Handle(Geom_BSplineCurve)& BS)
{
  Standard_Integer deg     = BS->Degree();
  Standard_Integer NbKnots = BS->NbKnots();
  Standard_Integer NbPoles = BS->NbPoles();
  TColgp_Array1OfPnt      Poles3d (1, NbPoles);
  TColStd_Array1OfReal    Weights (1, NbPoles);
  TColStd_Array1OfReal    Knots   (1, NbKnots);
  TColStd_Array1OfInteger Mults   (1, NbKnots);

  BS->Poles(Poles3d);
  if (BS->IsRational())
    BS->Weights(Weights);
  else
    Weights.Init (1);
  BS->Knots (Knots);
  BS->Multiplicities (Mults);

  TColgp_Array1OfPnt2d Poles2d (1,NbPoles);
  for (Standard_Integer i = 1; i <= NbPoles; i++)
    Poles2d (i) = gp_Pnt2d (Poles3d (i).X(), Poles3d (i).Y());
  
  Handle(Geom2d_BSplineCurve) BS2d = new Geom2d_BSplineCurve (Poles2d, Weights,
							      Knots, Mults, deg, BS->IsPeriodic());
  return BS2d;
}

 Standard_Boolean ShapeUpgrade::C0BSplineToSequenceOfC1BSplineCurve(const Handle(Geom2d_BSplineCurve)& BS,
								    Handle(TColGeom2d_HSequenceOfBoundedCurve)& seqBS) 
{
  if (BS.IsNull() || (BS->IsCN (1))) return Standard_False;
  
  Handle(Geom_BSplineCurve) BS3d = BSplineCurve2dTo3d (BS);
  Handle(TColGeom_HSequenceOfBoundedCurve) seqBS3d;
  Standard_Boolean result = C0BSplineToSequenceOfC1BSplineCurve (BS3d, seqBS3d); 
  if (result) {
    seqBS = new TColGeom2d_HSequenceOfBoundedCurve;
    for (Standard_Integer i = 1; i <= seqBS3d->Length(); i++)
      seqBS->Append (BSplineCurve3dTo2d (Handle(Geom_BSplineCurve)::DownCast (seqBS3d->Value (i))));
  }
  return result;
}

