// Created on: 1993-04-14
// Created by: Philippe DAUTRY
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <MgtGeom2d.ixx>

#include <Standard_Type.hxx>

#include <PColStd_HArray1OfInteger.hxx>
#include <PColStd_HArray2OfInteger.hxx>
#include <PColStd_HArray1OfReal.hxx>
#include <PColStd_HArray2OfReal.hxx>

#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array2OfInteger.hxx>
#include <TColStd_HArray1OfInteger.hxx>

#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>

#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array2OfPnt2d.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <TColgp_HArray2OfPnt2d.hxx>

#include <PColgp_HArray1OfPnt2d.hxx>
#include <PColgp_HArray2OfPnt2d.hxx>

#include <gp_Vec.hxx>
#include <gp_Ax1.hxx>

//modif wok++
#include <gp_Circ2d.hxx>
#include <Standard_NullObject.hxx>
#include <gp_Lin2d.hxx>

// *******************************************************************
// The following methods are subroutines used to copy elements
// from structures to other structures, as array1 or array2.
// *******************************************************************

// *******************************************************************
// Structures from TCollection to structures from PCollection.
// *******************************************************************

//=======================================================================
//function : ArrayCopy
//purpose  : Copy the gp_Pnt2d
//           from an Array1 from TColgp (TCollection)
//           to an HArray1 from PColgp (PCollection)
//=======================================================================

static Handle(PColgp_HArray1OfPnt2d) ArrayCopy
       (const TColgp_Array1OfPnt2d& TArray)
{
  Standard_Integer Lower = TArray.Lower();
  Standard_Integer Upper = TArray.Upper();
  Standard_Integer Index;
  Handle(PColgp_HArray1OfPnt2d) PArray = 
    new PColgp_HArray1OfPnt2d(Lower, Upper);
  for (Index = Lower; Index <= Upper; Index++) {
    PArray->SetValue(Index, TArray(Index));
  }
  return PArray;
}

//=======================================================================
//function : ArrayCopy
//purpose  : Copy the Standard_Real
//           from an Array1 from TColStd (TCollection)
//           to an SingleArray from PCollection
//=======================================================================

static Handle(PColStd_HArray1OfReal) ArrayCopy
       (const TColStd_Array1OfReal& TArray)
{
  Standard_Integer Lower = TArray.Lower();
  Standard_Integer Upper = TArray.Upper();
  Standard_Integer Index;
  Handle(PColStd_HArray1OfReal) PArray =
    new PColStd_HArray1OfReal(Lower, Upper);
  for (Index = Lower; Index <= Upper; Index++) {
    PArray->SetValue(Index, TArray(Index));
  }
  return PArray;
}

//=======================================================================
//function : ArrayCopy
//purpose  : Copy the Standard_Integer
//           from an Array1 from TColStd (TCollection)
//           to an SingleArray from PCollection
//=======================================================================

static Handle(PColStd_HArray1OfInteger) ArrayCopy
       (const TColStd_Array1OfInteger& TArray)
{
  Standard_Integer Lower = TArray.Lower();
  Standard_Integer Upper = TArray.Upper();
  Standard_Integer Index;
  Handle(PColStd_HArray1OfInteger) PArray =
    new PColStd_HArray1OfInteger(Lower, Upper);
  for (Index = Lower; Index <= Upper; Index++) {
    PArray->SetValue(Index, TArray(Index));
  }
  return PArray;
}

// *******************************************************************
// Structures from PCollection to structures from TCollection.
// *******************************************************************

//=======================================================================
//function : ArrayCopy
//purpose  : Copy the gp_Pnt2d
//           from an HArray1 from PColgp (PCollection)
//           to an Array1 from TColgp (TCollection)
//=======================================================================

static void ArrayCopy
  (const Handle(PColgp_HArray1OfPnt2d)& PArray,
   TColgp_Array1OfPnt2d& TArray)
{
  Standard_Integer Lower = PArray->Lower();
  Standard_Integer Upper = PArray->Upper();
  Standard_Integer Index;
  for (Index = Lower; Index <= Upper; Index++) {
    TArray(Index) = PArray->Value(Index);
  }
}

//=======================================================================
//function : ArrayCopy
//purpose  : Copy the Standard_Real
//           from an SingleArray from PCollection
//           to an Array1 from TColStd (TCollection)
//=======================================================================

static void ArrayCopy
  (const Handle(PColStd_HArray1OfReal)& PArray,
   TColStd_Array1OfReal& TArray)
{
  Standard_Integer Lower = PArray->Lower();
  Standard_Integer Upper = PArray->Upper();
  Standard_Integer Index;
  for (Index = Lower; Index <= Upper; Index++) {
    TArray(Index) = PArray->Value(Index);
  }
}

//=======================================================================
//function : ArrayCopy
//purpose  : Copy the Standard_Integer
//           from an SingleArray from PCollection
//           to an Array1 from TColStd (TCollection)
//=======================================================================

static void ArrayCopy
  (const Handle(PColStd_HArray1OfInteger)& PArray,
   TColStd_Array1OfInteger& TArray)
{
  Standard_Integer Lower = PArray->Lower();
  Standard_Integer Upper = PArray->Upper();
  Standard_Integer Index;
  for (Index = Lower; Index <= Upper; Index++) {
    TArray(Index) = PArray->Value(Index);
  }
}



// *******************************************************************
// Here is the implementation of the package methods.
// *******************************************************************


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom2d_AxisPlacement to a Geom2d_AxisPlacement.
//=======================================================================

Handle(Geom2d_AxisPlacement)  MgtGeom2d::Translate
       (const Handle(PGeom2d_AxisPlacement)& PObj)
{
  return new Geom2d_AxisPlacement(PObj->Axis());
}


//=======================================================================
//function : Translate
//purpose  : Translates a Geom2d_AxisPlacement to a PGeom2d_AxisPlacement.
//=======================================================================

Handle(PGeom2d_AxisPlacement)  MgtGeom2d::Translate
       (const Handle(Geom2d_AxisPlacement)& TObj)
{
  return new PGeom2d_AxisPlacement(TObj->Ax2d());
}


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom2d_BSplineCurve to a Geom2d_BSplineCurve.
//=======================================================================

Handle(Geom2d_BSplineCurve)  MgtGeom2d::Translate
       (const Handle(PGeom2d_BSplineCurve)& PObj)
{
  Handle(Geom2d_BSplineCurve)  TBSplC;

  Handle(PColgp_HArray1OfPnt2d) oldPoles = PObj->Poles();
  TColgp_Array1OfPnt2d newPoles (oldPoles->Lower(),oldPoles->Upper());
  ArrayCopy(oldPoles,newPoles);

  Handle(PColStd_HArray1OfInteger) oldMult = PObj->Multiplicities();
  TColStd_Array1OfInteger newMultiplicities(oldMult->Lower(),oldMult->Upper());
  ArrayCopy(oldMult,newMultiplicities);

  Handle(PColStd_HArray1OfReal) oldKnots = PObj->Knots();
  TColStd_Array1OfReal newKnots (oldKnots->Lower(),oldKnots->Upper());
  ArrayCopy(oldKnots,newKnots);

  if (PObj->Rational()) {
    Handle(PColStd_HArray1OfReal) oldWeights = PObj->Weights();
    TColStd_Array1OfReal newWeights (oldWeights->Lower(),oldWeights->Upper());
    ArrayCopy(oldWeights,newWeights);
    // modified by NIZHNY-MKK  Fri Oct 17 11:20:32 2003.BEGIN
    TBSplC = new Geom2d_BSplineCurve
      //       (newPoles, newWeights, newKnots, newMultiplicities, PObj->SpineDegree());
      (newPoles, newWeights, newKnots, newMultiplicities, 
       PObj->SpineDegree(), PObj->Periodic());
    // modified by NIZHNY-MKK  Fri Oct 17 11:20:38 2003.END
  }
  else {
    // modified by NIZHNY-MKK  Thu Oct 16 18:19:22 2003.BEGIN
    TBSplC = new Geom2d_BSplineCurve
      //       (newPoles, newKnots, newMultiplicities, PObj->SpineDegree());
      (newPoles, newKnots, newMultiplicities, 
       PObj->SpineDegree(), PObj->Periodic());
    // modified by NIZHNY-MKK  Fri Oct 17 11:20:52 2003.END
  }
  // modified by NIZHNY-MKK  Thu Oct 16 18:19:12 2003
  //   if (PObj->Periodic()) TBSplC->SetPeriodic();
  return TBSplC;
}


//=======================================================================
//function : Translate
//purpose  : Translates a Geom2d_BSplineCurve to a PGeom2d_BSplineCurve.
//=======================================================================

Handle(PGeom2d_BSplineCurve)  MgtGeom2d::Translate
       (const Handle(Geom2d_BSplineCurve)& TObj)
{
  Standard_Integer Upper;

  Upper = TObj->NbPoles();

  TColgp_Array1OfPnt2d oldPoles(1, Upper);
  TObj->Poles(oldPoles);
  Handle(PColgp_HArray1OfPnt2d) newPPoles = ArrayCopy(oldPoles);
  
  Handle(PColStd_HArray1OfReal) newPWeights;
  if (TObj->IsRational()) {
    TColStd_Array1OfReal oldWeights(1, Upper);
    TObj->Weights(oldWeights);
    newPWeights = ArrayCopy(oldWeights);
  }
  
  Upper = TObj->NbKnots();

  TColStd_Array1OfReal oldKnots(1, Upper);
  TObj->Knots(oldKnots);
  Handle(PColStd_HArray1OfReal) newPKnots = ArrayCopy(oldKnots);

  TColStd_Array1OfInteger oldMultiplicities(1, Upper);
  TObj->Multiplicities(oldMultiplicities);
  Handle(PColStd_HArray1OfInteger) newPMultiplicities =
    ArrayCopy(oldMultiplicities);

  
  Handle(PGeom2d_BSplineCurve) PBSplC = 
    new PGeom2d_BSplineCurve(TObj->IsRational(),
			     TObj->IsPeriodic(),
			     TObj->Degree(),
			     newPPoles,
			     newPWeights,
			     newPKnots,
			     newPMultiplicities);
  return PBSplC;
}


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom2d_BezierCurve to a Geom2d_BezierCurve.
//=======================================================================

Handle(Geom2d_BezierCurve)  MgtGeom2d::Translate
       (const Handle(PGeom2d_BezierCurve)& PObj)
{
  Handle(Geom2d_BezierCurve) TBzC;

  Handle(PColgp_HArray1OfPnt2d) oldPoles = PObj->Poles();
  TColgp_Array1OfPnt2d newPoles (oldPoles->Lower(),oldPoles->Upper());
  ArrayCopy(oldPoles,newPoles);

  if (PObj->Rational()) {
    Handle(PColStd_HArray1OfReal) oldWeights = PObj->Weights();
    TColStd_Array1OfReal newWeights (oldWeights->Lower(),oldWeights->Upper());
    ArrayCopy(oldWeights,newWeights);
    TBzC = new Geom2d_BezierCurve(newPoles, newWeights);
  }
  else {
    TBzC = new Geom2d_BezierCurve(newPoles);
  }
  return TBzC;
}


//=======================================================================
//function : Translate
//purpose  : Translates a Geom2d_BezierCurve to a PGeom2d_BezierCurve.
//=======================================================================

Handle(PGeom2d_BezierCurve)  MgtGeom2d::Translate
       (const Handle(Geom2d_BezierCurve)& TObj)
{
  Standard_Integer Upper;

  Upper = TObj->NbPoles();

  TColgp_Array1OfPnt2d oldPoles(1, Upper);
  TObj->Poles(oldPoles);
  Handle(PColgp_HArray1OfPnt2d) newPPoles = ArrayCopy(oldPoles);
  
  Handle(PColStd_HArray1OfReal) newPWeights;
  if (TObj->IsRational()) {
    TColStd_Array1OfReal oldWeights(1, Upper);
    TObj->Weights(oldWeights);
    newPWeights = ArrayCopy(oldWeights);
  }
  
  Handle(PGeom2d_BezierCurve) PBzC = new PGeom2d_BezierCurve
    (newPPoles,
     newPWeights,
     TObj->IsRational());
  return PBzC;
}


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom2d_CartesianPoint to a Geom2d_CartesianPoint.
//=======================================================================

Handle(Geom2d_CartesianPoint)  MgtGeom2d::Translate
       (const Handle(PGeom2d_CartesianPoint)& PObj)
{ return new Geom2d_CartesianPoint(PObj->Pnt()); }


//=======================================================================
//function : Translate
//purpose  : Translates a Geom2d_CartesianPoint to a PGeom2d_CartesianPoint.
//=======================================================================

Handle(PGeom2d_CartesianPoint)  MgtGeom2d::Translate
       (const Handle(Geom2d_CartesianPoint)& TObj)
{ return new PGeom2d_CartesianPoint(TObj->Pnt2d()); }


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom2d_Circle to a Geom2d_Circle.
//=======================================================================

Handle(Geom2d_Circle)  MgtGeom2d::Translate(const Handle(PGeom2d_Circle)& PObj)
{
  return new Geom2d_Circle(PObj->Position(),
			   PObj->Radius());
}


//=======================================================================
//function : Translate
//purpose  : Translates a Geom2d_Circle to a PGeom2d_Circle.
//=======================================================================

Handle(PGeom2d_Circle)  MgtGeom2d::Translate(const Handle(Geom2d_Circle)& TObj)
{
  return new PGeom2d_Circle(TObj->Circ2d().Position(),TObj->Circ2d().Radius());
}


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom2d_Curve to a Geom2d_Curve.
//=======================================================================

Handle(Geom2d_Curve)  MgtGeom2d::Translate(const Handle(PGeom2d_Curve)& PObj)
{
  Handle(Standard_Type) CurveType = PObj->DynamicType();

  if (CurveType == STANDARD_TYPE(PGeom2d_Line)) {
    Handle(PGeom2d_Line)& TLine = 
      (Handle(PGeom2d_Line)&) PObj;
    return MgtGeom2d::Translate(TLine);
  }
  else if (CurveType ==  STANDARD_TYPE(PGeom2d_Circle)) {
    Handle(PGeom2d_Circle)& TCircle = 
      (Handle(PGeom2d_Circle)&) PObj;
    return MgtGeom2d::Translate(TCircle);
  }
  else if (CurveType ==  STANDARD_TYPE(PGeom2d_Ellipse)) {
    Handle(PGeom2d_Ellipse)& TEllipse = 
      (Handle(PGeom2d_Ellipse)&) PObj;
    return MgtGeom2d::Translate(TEllipse);
  }
  else if (CurveType ==  STANDARD_TYPE(PGeom2d_Hyperbola)) {
    Handle(PGeom2d_Hyperbola)& THyperbola = 
      (Handle(PGeom2d_Hyperbola)&) PObj;
    return MgtGeom2d::Translate(THyperbola);
  }
  else if (CurveType ==  STANDARD_TYPE(PGeom2d_Parabola)) {
    Handle(PGeom2d_Parabola)& TParabola = 
      (Handle(PGeom2d_Parabola)&) PObj;
    return MgtGeom2d::Translate(TParabola);
  }
  else if (CurveType ==  STANDARD_TYPE(PGeom2d_BezierCurve)) {
    Handle(PGeom2d_BezierCurve)& TBezierCurve = 
      (Handle(PGeom2d_BezierCurve)&) PObj;
    return MgtGeom2d::Translate(TBezierCurve);
  }
  else if (CurveType ==  STANDARD_TYPE(PGeom2d_BSplineCurve)) {
    Handle(PGeom2d_BSplineCurve)& TBSplineCurve = 
      (Handle(PGeom2d_BSplineCurve)&) PObj;
    return MgtGeom2d::Translate(TBSplineCurve);
  }
  else if (CurveType ==  STANDARD_TYPE(PGeom2d_TrimmedCurve)) {
    Handle(PGeom2d_TrimmedCurve)& TTrimmedCurve = 
      (Handle(PGeom2d_TrimmedCurve)&) PObj;
    return MgtGeom2d::Translate(TTrimmedCurve);
  }
  else if (CurveType ==  STANDARD_TYPE(PGeom2d_OffsetCurve)) {
    Handle(PGeom2d_OffsetCurve)& TOffsetCurve = 
      (Handle(PGeom2d_OffsetCurve)&) PObj;
    return MgtGeom2d::Translate(TOffsetCurve);
  }
  else {
    cout << "Unknown Geom2d curve type ???? " << endl;
    Standard_NullObject::Raise("No mapping for the current Persistent Curve");
  }
// POP pour NT
  Handle(Geom2d_Curve) dummy;
  return dummy;
}


//=======================================================================
//function : Translate
//purpose  : Translates a Geom2d_Curve to a PGeom2d_Curve.
//=======================================================================

Handle(PGeom2d_Curve)  MgtGeom2d::Translate(const Handle(Geom2d_Curve)& TObj)
{
  Handle(Standard_Type) CurveType = TObj->DynamicType();

  if (CurveType == STANDARD_TYPE(Geom2d_Line)) {
    Handle(Geom2d_Line)& PLine = 
      (Handle(Geom2d_Line)&) TObj;
    return MgtGeom2d::Translate(PLine);
  }
  else if (CurveType ==  STANDARD_TYPE(Geom2d_Circle)) {
    Handle(Geom2d_Circle)& PCircle = 
      (Handle(Geom2d_Circle)&) TObj;
    return MgtGeom2d::Translate(PCircle);
  }
  else if (CurveType ==  STANDARD_TYPE(Geom2d_Ellipse)) {
    Handle(Geom2d_Ellipse)& PEllipse = 
      (Handle(Geom2d_Ellipse)&) TObj;
    return MgtGeom2d::Translate(PEllipse);
  }
  else if (CurveType ==  STANDARD_TYPE(Geom2d_Hyperbola)) {
    Handle(Geom2d_Hyperbola)& PHyperbola = 
      (Handle(Geom2d_Hyperbola)&) TObj;
    return MgtGeom2d::Translate(PHyperbola);
  }
  else if (CurveType ==  STANDARD_TYPE(Geom2d_Parabola)) {
    Handle(Geom2d_Parabola)& PParabola = 
      (Handle(Geom2d_Parabola)&) TObj;
    return MgtGeom2d::Translate(PParabola);
  }
  else if (CurveType ==  STANDARD_TYPE(Geom2d_BezierCurve)) {
    Handle(Geom2d_BezierCurve)& PBezierCurve = 
      (Handle(Geom2d_BezierCurve)&) TObj;
    return MgtGeom2d::Translate(PBezierCurve);
  }
  else if (CurveType ==  STANDARD_TYPE(Geom2d_BSplineCurve)) {
    Handle(Geom2d_BSplineCurve)& PBSplineCurve = 
      (Handle(Geom2d_BSplineCurve)&) TObj;
    return MgtGeom2d::Translate(PBSplineCurve);
  }
  else if (CurveType ==  STANDARD_TYPE(Geom2d_TrimmedCurve)) {
    Handle(Geom2d_TrimmedCurve)& PTrimmedCurve = 
      (Handle(Geom2d_TrimmedCurve)&) TObj;
    return MgtGeom2d::Translate(PTrimmedCurve);
  }
  else if (CurveType ==  STANDARD_TYPE(Geom2d_OffsetCurve)) {
    Handle(Geom2d_OffsetCurve)& POffsetCurve = 
      (Handle(Geom2d_OffsetCurve)&) TObj;
    return MgtGeom2d::Translate(POffsetCurve);
  }
  else {
    cout << "Unknown Geom2d curve type ???? " << endl;
    Standard_NullObject::Raise("No mapping for the current Transient Curve");
  }
// POP pour NT
  Handle(PGeom2d_Curve) dummy;
  return dummy;
}


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom2d_Direction to a Geom2d_Direction.
//=======================================================================

Handle(Geom2d_Direction)  MgtGeom2d::Translate
       (const Handle(PGeom2d_Direction)& PObj)
{
  gp_Vec2d vec = PObj->Vec();
  return new Geom2d_Direction(vec.X(), vec.Y()); 
}


//=======================================================================
//function : Translate
//purpose  : Translates a Geom2d_Direction to a PGeom2d_Direction.
//=======================================================================

Handle(PGeom2d_Direction)  MgtGeom2d::Translate
(const Handle(Geom2d_Direction)& TObj)
{ return new PGeom2d_Direction(TObj->Vec2d()); }


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom2d_Ellipse to a Geom2d_Ellipse.
//=======================================================================

Handle(Geom2d_Ellipse)  MgtGeom2d::Translate
(const Handle(PGeom2d_Ellipse)& PObj)
{
  return new Geom2d_Ellipse(PObj->Position(),
			    PObj->MajorRadius(),
			    PObj->MinorRadius());
}


//=======================================================================
//function : Translate
//purpose  : Translates a Geom2d_Ellipse to a PGeom2d_Ellipse.
//=======================================================================

Handle(PGeom2d_Ellipse)  MgtGeom2d::Translate
(const Handle(Geom2d_Ellipse)& TObj)
{
  return new PGeom2d_Ellipse(TObj->Position(),
			     TObj->MajorRadius(),
			     TObj->MinorRadius());
}


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom2d_Hyperbola to a Geom2d_Hyperbola.
//=======================================================================

Handle(Geom2d_Hyperbola)  MgtGeom2d::Translate
(const Handle(PGeom2d_Hyperbola)& PObj)
{
  return new Geom2d_Hyperbola(PObj->Position(),
			      PObj->MajorRadius(),
			      PObj->MinorRadius());
}


//=======================================================================
//function : Translate
//purpose  : Translates a Geom2d_Hyperbola to a PGeom2d_Hyperbola.
//=======================================================================

Handle(PGeom2d_Hyperbola)  MgtGeom2d::Translate
(const Handle(Geom2d_Hyperbola)& TObj)
{
  return new PGeom2d_Hyperbola(TObj->Position(),
			       TObj->MajorRadius(),
			       TObj->MinorRadius());
}


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom2d_Line to a Geom2d_Line.
//=======================================================================

Handle(Geom2d_Line)  MgtGeom2d::Translate(const Handle(PGeom2d_Line)& PObj)
{ return new Geom2d_Line(PObj->Position()); }


//=======================================================================
//function : Translate
//purpose  : Translates a Geom2d_Line to a PGeom2d_Line.
//=======================================================================

Handle(PGeom2d_Line)  MgtGeom2d::Translate(const Handle(Geom2d_Line)& TObj)
{
  return new PGeom2d_Line(TObj->Lin2d().Position());
}

//=======================================================================
//function : Translate
//purpose  : Translates a PGeom2d_OffsetCurve to a Geom2d_OffsetCurve.
//=======================================================================

Handle(Geom2d_OffsetCurve)  MgtGeom2d::Translate
(const Handle(PGeom2d_OffsetCurve)& PObj)
{
  return new Geom2d_OffsetCurve(MgtGeom2d::Translate(PObj->BasisCurve()),
				PObj->OffsetValue());
}


//=======================================================================
//function : Translate
//purpose  : Translates a Geom2d_OffsetCurve to a PGeom2d_OffsetCurve.
//=======================================================================

Handle(PGeom2d_OffsetCurve)  MgtGeom2d::Translate
(const Handle(Geom2d_OffsetCurve)& TObj)
{
  return new PGeom2d_OffsetCurve(MgtGeom2d::Translate(TObj->BasisCurve()),
				 TObj->Offset());
}


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom2d_Parabola to a Geom2d_Parabola.
//=======================================================================

Handle(Geom2d_Parabola)  MgtGeom2d::Translate
(const Handle(PGeom2d_Parabola)& PObj)
{
  return new Geom2d_Parabola(PObj->Position(),
			     PObj->FocalLength());
}


//=======================================================================
//function : Translate
//purpose  : Translates a Geom2d_Parabola to a PGeom2d_Parabola.
//=======================================================================

Handle(PGeom2d_Parabola)  MgtGeom2d::Translate
(const Handle(Geom2d_Parabola)& TObj)
{
  return new PGeom2d_Parabola(TObj->Position(),
			      TObj->Focal());
}

//=======================================================================
//function : Translate
//purpose  : Translates a PGeom2d_Point to a Geom2d_Point.
//           Optimized valid until new subtypes of Geom2dPoint exist
//=======================================================================

Handle(Geom2d_Point)  MgtGeom2d::Translate
(const Handle(PGeom2d_Point)& PObj)
{
/*
  Handle(Geom2d_Point) Geom2dP;
  Handle(Standard_Type) PointType = PObj->DynamicType();

  if (PointType == STANDARD_TYPE(PGeom2d_CartesianPoint)) {
    Handle(PGeom2d_CartesianPoint) PCPoint = 
      Handle(PGeom2d_CartesianPoint)::DownCast(PObj);
    Geom2dP = MgtGeom2d::Translate(PCPoint);
  }
  return Geom2dP;
*/
  Handle(PGeom2d_CartesianPoint)& PCPoint = 
    (Handle(PGeom2d_CartesianPoint)&) PObj;
  return new Geom2d_CartesianPoint(PCPoint->Pnt());
}


//=======================================================================
//function : Translate
//purpose  : Translates a Geom2d_Point to a PGeom2d_Point.
//           Optimized valid until new subtypes of Geom2dPoint exist
//=======================================================================

Handle(PGeom2d_Point)  MgtGeom2d::Translate
(const Handle(Geom2d_Point)& TObj)
{
/*
  Handle(PGeom2d_Point) PGeom2dP;
  Handle(Standard_Type) PointType = TObj->DynamicType();

  if (PointType == STANDARD_TYPE(Geom2d_CartesianPoint)) {
    Handle(Geom2d_CartesianPoint) TCPoint = 
      Handle(Geom2d_CartesianPoint)::DownCast(TObj);
    PGeom2dP = MgtGeom2d::Translate(TCPoint);
  }
  return PGeom2dP;
*/
  Handle(Geom2d_CartesianPoint)& TCPoint = 
    (Handle(Geom2d_CartesianPoint)&) TObj;
  return new PGeom2d_CartesianPoint(TCPoint->Pnt2d());
}



//=======================================================================
//function : Translate
//purpose  : Translates a PGeom2d_Transformation to a Geom2d_Transformation.
//=======================================================================

Handle(Geom2d_Transformation)  MgtGeom2d::Translate
(const Handle(PGeom2d_Transformation)& PObj)
{ return new Geom2d_Transformation(PObj->Trsf()); }


//=======================================================================
//function : Translate
//purpose  : Translates a Geom2d_Transformation to a PGeom2d_Transformation.
//=======================================================================

Handle(PGeom2d_Transformation)  MgtGeom2d::Translate
(const Handle(Geom2d_Transformation)& TObj)
{ return new PGeom2d_Transformation(TObj->Trsf2d()); }


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom2d_TrimmedCurve to a Geom2d_TrimmedCurve.
//=======================================================================

Handle(Geom2d_TrimmedCurve)  MgtGeom2d::Translate
(const Handle(PGeom2d_TrimmedCurve)& PObj)
{
  return new Geom2d_TrimmedCurve(MgtGeom2d::Translate(PObj->BasisCurve()),
				 PObj->FirstU(),
				 PObj->LastU(),
				 Standard_True); // Warning : will desappear
}


//=======================================================================
//function : Translate
//purpose  : Translates a Geom2d_TrimmedCurve to a PGeom2d_TrimmedCurve.
//=======================================================================

Handle(PGeom2d_TrimmedCurve)  MgtGeom2d::Translate
(const Handle(Geom2d_TrimmedCurve)& TObj)
{
  return new PGeom2d_TrimmedCurve(MgtGeom2d::Translate(TObj->BasisCurve()),
				  TObj->FirstParameter(),
				  TObj->LastParameter());
}


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom2d_VectorWithMagnitude to a
//           Geom2d_VectorWithMagnitude.
//=======================================================================

Handle(Geom2d_VectorWithMagnitude)  MgtGeom2d::Translate
(const Handle(PGeom2d_VectorWithMagnitude)& PObj)
{ return new Geom2d_VectorWithMagnitude(PObj->Vec()); }


//=======================================================================
//function : Translate
//purpose  : Translates a Geom2d_VectorWithMagnitude to a
//           PGeom2d_VectorWithMagnitude.
//=======================================================================

Handle(PGeom2d_VectorWithMagnitude)  MgtGeom2d::Translate
(const Handle(Geom2d_VectorWithMagnitude)& TObj)
{ return new PGeom2d_VectorWithMagnitude(TObj->Vec2d()); }
