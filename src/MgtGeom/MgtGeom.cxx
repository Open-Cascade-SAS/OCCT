// Created on: 1993-03-05
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

#include <MgtGeom.ixx>

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

#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_HArray2OfPnt.hxx>

#include <PColgp_HArray1OfPnt.hxx>
#include <PColgp_HArray2OfPnt.hxx>

#include <gp_Vec.hxx>
#include <gp_Circ.hxx>
#include <gp_Ax1.hxx>

//modif wok++
#include <Standard_NullObject.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>

// *******************************************************************
// The following methods are subroutines used to copy elements
// from structures to other structures, as array1 or array2.
// *******************************************************************

// *******************************************************************
// Structures from TCollection to structures from PCollection.
// *******************************************************************

//=======================================================================
//function : ArrayCopy
//purpose  : Copy the gp_Pnt
//           from an Array1 from TColgp (TCollection)
//           to an HArray1 from PColgp (PCollection)
//=======================================================================

static Handle(PColgp_HArray1OfPnt) ArrayCopy
       (const TColgp_Array1OfPnt& TArray)
{
  Standard_Integer Lower = TArray.Lower();
  Standard_Integer Upper = TArray.Upper();
  Standard_Integer Index;
  Handle(PColgp_HArray1OfPnt) PArray = new PColgp_HArray1OfPnt(Lower, Upper);
  for (Index = Lower; Index <= Upper; Index++) {
    PArray->SetValue(Index, TArray(Index));
  }
  return PArray;
}

//=======================================================================
//function : ArrayCopy
//purpose  : Copy the gp_Pnt
//           from an Array2 from TColgp (TCollection)
//           to an HArray2 from PColgp (PCollection)
//=======================================================================

static Handle(PColgp_HArray2OfPnt) ArrayCopy
       (const TColgp_Array2OfPnt& TArray)
{
  Standard_Integer LowerRow = TArray.LowerRow();
  Standard_Integer UpperRow = TArray.UpperRow();
  Standard_Integer LowerCol = TArray.LowerCol();
  Standard_Integer UpperCol = TArray.UpperCol();
  Standard_Integer IndexRow, IndexCol;
  Handle(PColgp_HArray2OfPnt) PArray =
    new PColgp_HArray2OfPnt(LowerRow, UpperRow, LowerCol, UpperCol);
  for (IndexRow = LowerRow; IndexRow <= UpperRow; IndexRow++) {
    for (IndexCol = LowerCol; IndexCol <= UpperCol; IndexCol++) {
      PArray->SetValue(IndexRow, IndexCol, TArray(IndexRow, IndexCol));
    }
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
//purpose  : Copy the Standard_Real
//           from an Array2 from TColStd (TCollection)
//           to an DoubleArray from PCollection
//=======================================================================

static Handle(PColStd_HArray2OfReal) ArrayCopy
       (const TColStd_Array2OfReal& TArray)
{
  Standard_Integer LowerRow = TArray.LowerRow();
  Standard_Integer UpperRow = TArray.UpperRow();
  Standard_Integer LowerCol = TArray.LowerCol();
  Standard_Integer UpperCol = TArray.UpperCol();
  Standard_Integer IndexRow, IndexCol;
  Handle(PColStd_HArray2OfReal) PArray =
    new PColStd_HArray2OfReal(LowerRow, UpperRow, LowerCol, UpperCol);
  for (IndexRow = LowerRow; IndexRow <= UpperRow; IndexRow++) {
    for (IndexCol = LowerCol; IndexCol <= UpperCol; IndexCol++) {
      PArray->SetValue(IndexRow, IndexCol, TArray(IndexRow, IndexCol));
    }
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
//purpose  : Copy the gp_Pnt
//           from an HArray1 from PColgp (PCollection)
//           to an Array1 from TColgp (TCollection)
//=======================================================================

static void ArrayCopy
  (const Handle(PColgp_HArray1OfPnt)& PArray,
   TColgp_Array1OfPnt& TArray)
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
//purpose  : Copy the gp_Pnt
//           from an HArray2 from PColgp (PCollection)
//           to an Array2 from TColgp (TCollection)
//=======================================================================

static void ArrayCopy
  (const Handle(PColgp_HArray2OfPnt)& PArray,
   TColgp_Array2OfPnt& TArray)
{
  Standard_Integer LowerRow = TArray.LowerRow();
  Standard_Integer UpperRow = TArray.UpperRow();
  Standard_Integer LowerCol = TArray.LowerCol();
  Standard_Integer UpperCol = TArray.UpperCol();
  Standard_Integer IndexRow, IndexCol;
  for (IndexRow = LowerRow; IndexRow <= UpperRow; IndexRow++) {
    for (IndexCol = LowerCol; IndexCol <= UpperCol; IndexCol++) {
      TArray(IndexRow, IndexCol) = PArray->Value(IndexRow, IndexCol);
    }
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
//purpose  : Copy the Standard_Real
//           from an DoubleArray from PCollection
//           to an Array2 from TColStd (TCollection)
//=======================================================================

static void ArrayCopy
  (const Handle(PColStd_HArray2OfReal)& PArray,
   TColStd_Array2OfReal& TArray)
{
  Standard_Integer LowerRow = TArray.LowerRow();
  Standard_Integer UpperRow = TArray.UpperRow();
  Standard_Integer LowerCol = TArray.LowerCol();
  Standard_Integer UpperCol = TArray.UpperCol();
  Standard_Integer IndexRow, IndexCol;
  for (IndexRow = LowerRow; IndexRow <= UpperRow; IndexRow++) {
    for (IndexCol = LowerCol; IndexCol <= UpperCol; IndexCol++) {
      TArray(IndexRow, IndexCol) = PArray->Value(IndexRow, IndexCol);
    }
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
//purpose  : Translates a PGeom_Axis1Placement to a Geom_Axis1Placement.
//=======================================================================

Handle(Geom_Axis1Placement)  MgtGeom::Translate
       (const Handle(PGeom_Axis1Placement)& PObj)
{
  return new Geom_Axis1Placement(PObj->Axis());
}


//=======================================================================
//function : Translate
//purpose  : Translates a Geom_Axis1Placement to a PGeom_Axis1Placement.
//=======================================================================

Handle(PGeom_Axis1Placement)  MgtGeom::Translate
       (const Handle(Geom_Axis1Placement)& TObj)
{
  return new PGeom_Axis1Placement(TObj->Axis());
}


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom_Axis2Placement to a Geom_Axis2Placement.
//=======================================================================

Handle(Geom_Axis2Placement)  MgtGeom::Translate
       (const Handle(PGeom_Axis2Placement)& PObj)
{
  return new Geom_Axis2Placement(PObj->Axis().Location(),
				 PObj->Axis().Direction(),
				 PObj->XDirection());
}


//=======================================================================
//function : Translate
//purpose  : Translates a Geom_Axis2Placement to a PGeom_Axis2Placement.
//=======================================================================

Handle(PGeom_Axis2Placement)  MgtGeom::Translate
       (const Handle(Geom_Axis2Placement)& TObj)
{
  return new PGeom_Axis2Placement(TObj->Axis(),
				  TObj->XDirection());
}


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom_BSplineCurve to a Geom_BSplineCurve.
//=======================================================================

Handle(Geom_BSplineCurve)  MgtGeom::Translate
       (const Handle(PGeom_BSplineCurve)& PObj)
{
  Handle(Geom_BSplineCurve)  TBSplC;

  Handle(PColgp_HArray1OfPnt) oldPoles = PObj->Poles();
  TColgp_Array1OfPnt newPoles (oldPoles->Lower(),oldPoles->Upper());
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
    TBSplC = new Geom_BSplineCurve
      (newPoles, newWeights, newKnots, newMultiplicities, PObj->SpineDegree(),PObj->Periodic());
  }
  else {
    TBSplC = new Geom_BSplineCurve
      (newPoles, newKnots, newMultiplicities, PObj->SpineDegree(),PObj->Periodic());
  }
//  if (PObj->Periodic()) TBSplC->SetPeriodic();
  return TBSplC;
}


//=======================================================================
//function : Translate
//purpose  : Translates a Geom_BSplineCurve to a PGeom_BSplineCurve.
//=======================================================================

Handle(PGeom_BSplineCurve)  MgtGeom::Translate
       (const Handle(Geom_BSplineCurve)& TObj)
{
  Standard_Integer Upper;

  Upper = TObj->NbPoles();

  TColgp_Array1OfPnt oldPoles(1, Upper);
  TObj->Poles(oldPoles);
  Handle(PColgp_HArray1OfPnt) newPPoles = ArrayCopy(oldPoles);
  
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
  
  return new PGeom_BSplineCurve(TObj->IsRational(),
				TObj->IsPeriodic(),
				TObj->Degree(),
				newPPoles,
				newPWeights,
				newPKnots,
				newPMultiplicities);
}


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom_BSplineSurface to a Geom_BSplineSurface.
//=======================================================================

Handle(Geom_BSplineSurface)  MgtGeom::Translate
       (const Handle(PGeom_BSplineSurface)& PObj)
{
  Handle(Geom_BSplineSurface)  TBSplS;

  Handle(PColgp_HArray2OfPnt) oldPoles = PObj->Poles();
  TColgp_Array2OfPnt newPoles (oldPoles->LowerRow(),oldPoles->UpperRow(),
			       oldPoles->LowerCol(),oldPoles->UpperCol());
  ArrayCopy(oldPoles,newPoles);

  Handle(PColStd_HArray1OfInteger) oldUMult = PObj->UMultiplicities();
  TColStd_Array1OfInteger
    newUMultiplicities (oldUMult->Lower(),oldUMult->Upper());
  ArrayCopy(oldUMult,newUMultiplicities);

  Handle(PColStd_HArray1OfInteger) oldVMult = PObj->VMultiplicities();
  TColStd_Array1OfInteger
    newVMultiplicities (oldVMult->Lower(),oldVMult->Upper());
  ArrayCopy(oldVMult,newVMultiplicities);

  Handle(PColStd_HArray1OfReal) oldUKnots = PObj->UKnots();
  TColStd_Array1OfReal newUKnots (oldUKnots->Lower(),oldUKnots->Upper());
  ArrayCopy(oldUKnots,newUKnots);

  Handle(PColStd_HArray1OfReal) oldVKnots = PObj->VKnots();
  TColStd_Array1OfReal newVKnots (oldVKnots->Lower(),oldVKnots->Upper());
  ArrayCopy(oldVKnots,newVKnots);

  if (!PObj->URational() && !PObj->VRational()) {
    TBSplS = new Geom_BSplineSurface
      (newPoles,
       newUKnots, newVKnots,
       newUMultiplicities,newVMultiplicities,
       PObj->USpineDegree(),
       PObj->VSpineDegree(),
       PObj->UPeriodic(),
       PObj->VPeriodic());
  }
  else {
    Handle(PColStd_HArray2OfReal) oldWeights = PObj->Weights();
    TColStd_Array2OfReal
      newWeights (oldWeights->LowerRow(),oldWeights->UpperRow(),
		  oldWeights->LowerCol(),oldWeights->UpperCol());
    ArrayCopy(oldWeights,newWeights);
    TBSplS = new Geom_BSplineSurface
      (newPoles,
       newWeights,
       newUKnots, newVKnots,
       newUMultiplicities,newVMultiplicities,
       PObj->USpineDegree(),
       PObj->VSpineDegree(),
       PObj->UPeriodic(),
       PObj->VPeriodic());
  }
//  if (PObj->UPeriodic()) TBSplS->SetUPeriodic();
//  if (PObj->VPeriodic()) TBSplS->SetVPeriodic();
  return TBSplS;
}


//=======================================================================
//function : Translate
//purpose  : Translates a Geom_BSplineSurface to a PGeom_BSplineSurface.
//=======================================================================

Handle(PGeom_BSplineSurface)  MgtGeom::Translate
       (const Handle(Geom_BSplineSurface)& TObj)
{
  Standard_Integer UpperU, UpperV;

  UpperU = TObj->NbUPoles();
  UpperV = TObj->NbVPoles();

  TColgp_Array2OfPnt oldPoles(1, UpperU, 1, UpperV);
  TObj->Poles(oldPoles);
  Handle(PColgp_HArray2OfPnt) newPPoles =
    ArrayCopy(oldPoles);
  
  Handle(PColStd_HArray2OfReal) newPWeights;
  if (TObj->IsURational() || TObj->IsVRational()) {
    TColStd_Array2OfReal oldWeights(1, UpperU, 1, UpperV);
    TObj->Weights(oldWeights);
    newPWeights = ArrayCopy(oldWeights);
  }
  
  UpperU = TObj->NbUKnots();

  TColStd_Array1OfReal oldUKnots(1, UpperU);
  TObj->UKnots(oldUKnots);
  Handle(PColStd_HArray1OfReal) newPUKnots =
    ArrayCopy(oldUKnots);
  
  TColStd_Array1OfInteger oldUMultiplicities(1, UpperU);
  TObj->UMultiplicities(oldUMultiplicities);
  Handle(PColStd_HArray1OfInteger) newPUMultiplicities =
    ArrayCopy(oldUMultiplicities);
  
  UpperV = TObj->NbVKnots();

  TColStd_Array1OfReal oldVKnots(1, UpperV);
  TObj->VKnots(oldVKnots);
  Handle(PColStd_HArray1OfReal) newPVKnots =
    ArrayCopy(oldVKnots);
  
  TColStd_Array1OfInteger oldVMultiplicities(1, UpperV);
  TObj->VMultiplicities(oldVMultiplicities);
  Handle(PColStd_HArray1OfInteger) newPVMultiplicities =
    ArrayCopy(oldVMultiplicities);
  
  return new PGeom_BSplineSurface(TObj->IsURational(),
				  TObj->IsVRational(),
				  TObj->IsUPeriodic(),
				  TObj->IsVPeriodic(),
				  TObj->UDegree(),
				  TObj->VDegree(),
				  newPPoles,
				  newPWeights,
				  newPUKnots,
				  newPVKnots,
				  newPUMultiplicities,
				  newPVMultiplicities);
}


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom_BezierCurve to a Geom_BezierCurve.
//=======================================================================

Handle(Geom_BezierCurve)  MgtGeom::Translate
       (const Handle(PGeom_BezierCurve)& PObj)
{
  Handle(Geom_BezierCurve) TBzC;

  Handle(PColgp_HArray1OfPnt) oldPoles = PObj->Poles();
  TColgp_Array1OfPnt newPoles (oldPoles->Lower(),oldPoles->Upper());
  ArrayCopy(oldPoles,newPoles);

  if (PObj->Rational()) {
    Handle(PColStd_HArray1OfReal) oldWeights = PObj->Weights();
    TColStd_Array1OfReal newWeights (oldWeights->Lower(),oldWeights->Upper());
    ArrayCopy(oldWeights,newWeights);
    TBzC = new Geom_BezierCurve(newPoles, newWeights);
  }
  else {
    TBzC = new Geom_BezierCurve(newPoles);
  }
  return TBzC;
}


//=======================================================================
//function : Translate
//purpose  : Translates a Geom_BezierCurve to a PGeom_BezierCurve.
//=======================================================================

Handle(PGeom_BezierCurve)  MgtGeom::Translate
       (const Handle(Geom_BezierCurve)& TObj)
{
  Standard_Integer Upper;

  Upper = TObj->NbPoles();

  TColgp_Array1OfPnt oldPoles(1, Upper);
  TObj->Poles(oldPoles);
  Handle(PColgp_HArray1OfPnt) newPPoles = ArrayCopy(oldPoles);
  
  Handle(PColStd_HArray1OfReal) newPWeights;
  if (TObj->IsRational()) {
    TColStd_Array1OfReal oldWeights(1, Upper);
    TObj->Weights(oldWeights);
    newPWeights = ArrayCopy(oldWeights);
  }
  
  return new PGeom_BezierCurve(newPPoles,
			       newPWeights,
			       TObj->IsRational());
}


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom_BezierSurface to a Geom_BezierSurface.
//=======================================================================

Handle(Geom_BezierSurface)  MgtGeom::Translate
       (const Handle(PGeom_BezierSurface)& PObj)
{
  Handle(Geom_BezierSurface)  TBzS;

  Handle(PColgp_HArray2OfPnt) oldPoles = PObj->Poles();
  TColgp_Array2OfPnt newPoles (oldPoles->LowerRow(),oldPoles->UpperRow(),
			       oldPoles->LowerCol(),oldPoles->UpperCol());
  ArrayCopy(oldPoles,newPoles);

  if (!PObj->URational() && !PObj->VRational()) {
    TBzS = new Geom_BezierSurface(newPoles);
  }
  else {
    Handle(PColStd_HArray2OfReal) oldWeights = PObj->Weights();
    TColStd_Array2OfReal
      newWeights (oldWeights->LowerRow(),oldWeights->UpperRow(),
		  oldWeights->LowerCol(),oldWeights->UpperCol());
    ArrayCopy(oldWeights,newWeights);

    TBzS = new Geom_BezierSurface(newPoles, newWeights);
  }
  return TBzS;
}


//=======================================================================
//function : Translate
//purpose  : Translates a Geom_BezierSurface to a PGeom_BezierSurface.
//=======================================================================

Handle(PGeom_BezierSurface)  MgtGeom::Translate(const Handle(Geom_BezierSurface)& TObj)
{
  Standard_Integer UpperU, UpperV;

  UpperU = TObj->NbUPoles();
  UpperV = TObj->NbVPoles();

  TColgp_Array2OfPnt oldPoles(1, UpperU, 1, UpperV);
  TObj->Poles(oldPoles);
  Handle(PColgp_HArray2OfPnt) newPPoles =
    ArrayCopy(oldPoles);
  
  Handle(PColStd_HArray2OfReal) newPWeights;
  if (TObj->IsURational() || TObj->IsVRational()) {
    TColStd_Array2OfReal oldWeights(1, UpperU, 1, UpperV);
    TObj->Weights(oldWeights);
    newPWeights = ArrayCopy(oldWeights);
  }
  
  return new PGeom_BezierSurface(TObj->IsURational(),
				 TObj->IsVRational(),
				 newPPoles,
				 newPWeights);
}


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom_CartesianPoint to a Geom_CartesianPoint.
//=======================================================================

Handle(Geom_CartesianPoint)  MgtGeom::Translate
       (const Handle(PGeom_CartesianPoint)& PObj)
{ return new Geom_CartesianPoint(PObj->Pnt()); }


//=======================================================================
//function : Translate
//purpose  : Translates a Geom_CartesianPoint to a PGeom_CartesianPoint.
//=======================================================================

Handle(PGeom_CartesianPoint)  MgtGeom::Translate
       (const Handle(Geom_CartesianPoint)& TObj)
{ return new PGeom_CartesianPoint(TObj->Pnt()); }


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom_Circle to a Geom_Circle.
//=======================================================================

Handle(Geom_Circle)  MgtGeom::Translate(const Handle(PGeom_Circle)& PObj)
{
  return new Geom_Circle(PObj->Position(),
			 PObj->Radius());
}


//=======================================================================
//function : Translate
//purpose  : Translates a Geom_Circle to a PGeom_Circle.
//=======================================================================

Handle(PGeom_Circle)  MgtGeom::Translate(const Handle(Geom_Circle)& TObj)
{
  gp_Circ cir = TObj->Circ();
  return new PGeom_Circle(cir.Position(),cir.Radius());
}


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom_ConicalSurface to a Geom_ConicalSurface.
//=======================================================================

Handle(Geom_ConicalSurface)  MgtGeom::Translate
       (const Handle(PGeom_ConicalSurface)& PObj)
{
  return new Geom_ConicalSurface(PObj->Position(),
				 PObj->SemiAngle(),
				 PObj->Radius());
}


//=======================================================================
//function : Translate
//purpose  : Translates a Geom_ConicalSurface to a PGeom_ConicalSurface.
//=======================================================================

Handle(PGeom_ConicalSurface)  MgtGeom::Translate
       (const Handle(Geom_ConicalSurface)& TObj)
{
  return new PGeom_ConicalSurface(TObj->Position(),
				  TObj->RefRadius(),
				  TObj->SemiAngle());
}


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom_Curve to a Geom_Curve.
//=======================================================================

Handle(Geom_Curve)  MgtGeom::Translate(const Handle(PGeom_Curve)& PObj)
{
  Handle(Standard_Type) CurveType = PObj->DynamicType();
  
  if (CurveType == STANDARD_TYPE(PGeom_Line)) {
    Handle(PGeom_Line)& TLine = 
      (Handle(PGeom_Line)&) PObj;
    return MgtGeom::Translate(TLine);
  }
  else if (CurveType ==  STANDARD_TYPE(PGeom_Circle)) {
    Handle(PGeom_Circle)& TCircle = 
      (Handle(PGeom_Circle)&) PObj;
    return MgtGeom::Translate(TCircle);
  }
  else if (CurveType ==  STANDARD_TYPE(PGeom_Ellipse)) {
    Handle(PGeom_Ellipse)& TEllipse = 
      (Handle(PGeom_Ellipse)&) PObj;
    return MgtGeom::Translate(TEllipse);
  }
  else if (CurveType ==  STANDARD_TYPE(PGeom_Hyperbola)) {
    Handle(PGeom_Hyperbola)& THyperbola = 
      (Handle(PGeom_Hyperbola)&) PObj;
    return MgtGeom::Translate(THyperbola);
  }
  else if (CurveType ==  STANDARD_TYPE(PGeom_Parabola)) {
    Handle(PGeom_Parabola)& TParabola = 
      (Handle(PGeom_Parabola)&) PObj;
    return MgtGeom::Translate(TParabola);
  }
  else if (CurveType ==  STANDARD_TYPE(PGeom_BezierCurve)) {
    Handle(PGeom_BezierCurve)& TBezierCurve = 
      (Handle(PGeom_BezierCurve)&) PObj;
    return MgtGeom::Translate(TBezierCurve);
  }
  else if (CurveType ==  STANDARD_TYPE(PGeom_BSplineCurve)) {
    Handle(PGeom_BSplineCurve)& TBSplineCurve = 
      (Handle(PGeom_BSplineCurve)&) PObj;
    return MgtGeom::Translate(TBSplineCurve);
  }
  else if (CurveType ==  STANDARD_TYPE(PGeom_TrimmedCurve)) {
    Handle(PGeom_TrimmedCurve)& TTrimmedCurve = 
      (Handle(PGeom_TrimmedCurve)&) PObj;
    return MgtGeom::Translate(TTrimmedCurve);
  }
  else if (CurveType ==  STANDARD_TYPE(PGeom_OffsetCurve)) {
    Handle(PGeom_OffsetCurve)& TOffsetCurve = 
      (Handle(PGeom_OffsetCurve)&) PObj;
    return MgtGeom::Translate(TOffsetCurve);
  }
  else {
    cout << "MgtGeom : Unknown curve type ???? : " << endl;
    Standard_NullObject::Raise("No mapping for the current Persistent Curve");
  }
// POP pour NT
  Handle(Geom_Curve) dummy;
  return dummy;
}


//=======================================================================
//function : Translate
//purpose  : Translates a Geom_Curve to a PGeom_Curve.
//=======================================================================

Handle(PGeom_Curve)  MgtGeom::Translate(const Handle(Geom_Curve)& TObj)
{
  Handle(Standard_Type) CurveType = TObj->DynamicType();

  if (CurveType == STANDARD_TYPE(Geom_Line)) {
    Handle(Geom_Line)& PLine = 
      (Handle(Geom_Line)&) TObj;
    return MgtGeom::Translate(PLine);
  }
  else if (CurveType ==  STANDARD_TYPE(Geom_Circle)) {
    Handle(Geom_Circle)& PCircle = 
      (Handle(Geom_Circle)&) TObj;
    return MgtGeom::Translate(PCircle);
  }
  else if (CurveType ==  STANDARD_TYPE(Geom_Ellipse)) {
    Handle(Geom_Ellipse)& PEllipse = 
      (Handle(Geom_Ellipse)&) TObj;
    return MgtGeom::Translate(PEllipse);
  }
  else if (CurveType ==  STANDARD_TYPE(Geom_Hyperbola)) {
    Handle(Geom_Hyperbola)& PHyperbola = 
      (Handle(Geom_Hyperbola)&) TObj;
    return MgtGeom::Translate(PHyperbola);
  }
  else if (CurveType ==  STANDARD_TYPE(Geom_Parabola)) {
    Handle(Geom_Parabola)& PParabola = 
      (Handle(Geom_Parabola)&) TObj;
    return MgtGeom::Translate(PParabola);
  }
  else if (CurveType ==  STANDARD_TYPE(Geom_BezierCurve)) {
    Handle(Geom_BezierCurve)& PBezierCurve = 
      (Handle(Geom_BezierCurve)&) TObj;
    return MgtGeom::Translate(PBezierCurve);
  }
  else if (CurveType ==  STANDARD_TYPE(Geom_BSplineCurve)) {
    Handle(Geom_BSplineCurve)& PBSplineCurve = 
      (Handle(Geom_BSplineCurve)&) TObj;
    return MgtGeom::Translate(PBSplineCurve);
  }
  else if (CurveType ==  STANDARD_TYPE(Geom_TrimmedCurve)) {
    Handle(Geom_TrimmedCurve)& PTrimmedCurve = 
      (Handle(Geom_TrimmedCurve)&) TObj;
    return MgtGeom::Translate(PTrimmedCurve);
  }
  else if (CurveType ==  STANDARD_TYPE(Geom_OffsetCurve)) {
    Handle(Geom_OffsetCurve)& POffsetCurve = 
      (Handle(Geom_OffsetCurve)&) TObj;
    return MgtGeom::Translate(POffsetCurve);
  }
  else {
    cout << "MgtGeom : Unknown curve type ????"<<endl;
    Standard_NullObject::Raise("No mapping for the current Transient Curve");

  }
// POP pour NT
  Handle(PGeom_Curve) dummy;
  return dummy;
}


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom_CylindricalSurface to a
//           Geom_CylindricalSurface.
//=======================================================================

Handle(Geom_CylindricalSurface)  MgtGeom::Translate
       (const Handle(PGeom_CylindricalSurface)& PObj)
{
  return  new Geom_CylindricalSurface(PObj->Position(),
				      PObj->Radius());
}


//=======================================================================
//function : Translate
//purpose  : Translates a Geom_CylindricalSurface to a PGeom_CylindricalSurface.
//=======================================================================

Handle(PGeom_CylindricalSurface)  MgtGeom::Translate(const Handle(Geom_CylindricalSurface)& TObj)
{
  return new PGeom_CylindricalSurface(TObj->Position(),
				      TObj->Radius());
}


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom_Direction to a Geom_Direction.
//=======================================================================

Handle(Geom_Direction)  MgtGeom::Translate
       (const Handle(PGeom_Direction)& PObj)
{
  gp_Vec vec = PObj->Vec();
  return new Geom_Direction(vec.X(), vec.Y(), vec.Z()); }


//=======================================================================
//function : Translate
//purpose  : Translates a Geom_Direction to a PGeom_Direction.
//=======================================================================

Handle(PGeom_Direction)  MgtGeom::Translate
       (const Handle(Geom_Direction)& TObj)
{ return new PGeom_Direction(TObj->Vec()); }


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom_Ellipse to a Geom_Ellipse.
//=======================================================================

Handle(Geom_Ellipse)  MgtGeom::Translate
       (const Handle(PGeom_Ellipse)& PObj)
{
  return new Geom_Ellipse(PObj->Position(),
			  PObj->MajorRadius(),
			  PObj->MinorRadius());
}


//=======================================================================
//function : Translate
//purpose  : Translates a Geom_Ellipse to a PGeom_Ellipse.
//=======================================================================

Handle(PGeom_Ellipse)  MgtGeom::Translate
       (const Handle(Geom_Ellipse)& TObj)
{
  return new PGeom_Ellipse(TObj->Position(),
			   TObj->MajorRadius(),
			   TObj->MinorRadius());
}


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom_Hyperbola to a Geom_Hyperbola.
//=======================================================================

Handle(Geom_Hyperbola)  MgtGeom::Translate
       (const Handle(PGeom_Hyperbola)& PObj)
{
  return new Geom_Hyperbola(PObj->Position(),
			    PObj->MajorRadius(),
			    PObj->MinorRadius());
}


//=======================================================================
//function : Translate
//purpose  : Translates a Geom_Hyperbola to a PGeom_Hyperbola.
//=======================================================================

Handle(PGeom_Hyperbola)  MgtGeom::Translate(const Handle(Geom_Hyperbola)& TObj)
{
  return new PGeom_Hyperbola(TObj->Position(),
			     TObj->MajorRadius(),
			     TObj->MinorRadius());
}


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom_Line to a Geom_Line.
//=======================================================================

Handle(Geom_Line)  MgtGeom::Translate(const Handle(PGeom_Line)& PObj)
{ return new Geom_Line(PObj->Position()); }


//=======================================================================
//function : Translate
//purpose  : Translates a Geom_Line to a PGeom_Line.
//=======================================================================

Handle(PGeom_Line)  MgtGeom::Translate(const Handle(Geom_Line)& TObj)
{ return new PGeom_Line(TObj->Lin().Position()); }


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom_OffsetCurve to a Geom_OffsetCurve.
//=======================================================================

Handle(Geom_OffsetCurve)  MgtGeom::Translate
       (const Handle(PGeom_OffsetCurve)& PObj)
{
  return new Geom_OffsetCurve(MgtGeom::Translate(PObj->BasisCurve()),
			      PObj->OffsetValue(),
			      PObj->OffsetDirection());
}


//=======================================================================
//function : Translate
//purpose  : Translates a Geom_OffsetCurve to a PGeom_OffsetCurve.
//=======================================================================

Handle(PGeom_OffsetCurve)  MgtGeom::Translate(const Handle(Geom_OffsetCurve)& TObj)
{
  return new PGeom_OffsetCurve(MgtGeom::Translate(TObj->BasisCurve()),
			       TObj->Offset(),
			       TObj->Direction());
}


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom_OffsetSurface to a Geom_OffsetSurface.
//=======================================================================

Handle(Geom_OffsetSurface)  MgtGeom::Translate
       (const Handle(PGeom_OffsetSurface)& PObj)
{
  return new Geom_OffsetSurface(MgtGeom::Translate(PObj->BasisSurface()),
				PObj->OffsetValue());
}


//=======================================================================
//function : Translate
//purpose  : Translates a Geom_OffsetSurface to a PGeom_OffsetSurface.
//=======================================================================

Handle(PGeom_OffsetSurface)  MgtGeom::Translate
       (const Handle(Geom_OffsetSurface)& TObj)
{
  return new PGeom_OffsetSurface(MgtGeom::Translate(TObj->BasisSurface()),
				 TObj->Offset());
}


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom_Parabola to a Geom_Parabola.
//=======================================================================

Handle(Geom_Parabola)  MgtGeom::Translate
       (const Handle(PGeom_Parabola)& PObj)
{
  return new Geom_Parabola(PObj->Position(),
			   PObj->FocalLength());
}


//=======================================================================
//function : Translate
//purpose  : Translates a Geom_Parabola to a PGeom_Parabola.
//=======================================================================

Handle(PGeom_Parabola)  MgtGeom::Translate
       (const Handle(Geom_Parabola)& TObj)
{
  return new PGeom_Parabola(TObj->Position(),
			    TObj->Focal());
}


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom_Plane to a Geom_Plane.
//=======================================================================

Handle(Geom_Plane)  MgtGeom::Translate(const Handle(PGeom_Plane)& PObj)
{  return new Geom_Plane(PObj->Position()); }


//=======================================================================
//function : Translate
//purpose  : Translates a Geom_Plane to a PGeom_Plane.
//=======================================================================

Handle(PGeom_Plane)  MgtGeom::Translate(const Handle(Geom_Plane)& TObj)
{
  return new PGeom_Plane(TObj->Pln().Position());
}

//=======================================================================
//function : Translate
//purpose  : Translates a PGeom_Point to a Geom_Point.
//         : Optimized valid until new subtypes of GeomPoint exist
//=======================================================================

Handle(Geom_Point)  MgtGeom::Translate
       (const Handle(PGeom_Point)& PObj)
{
/*
  Handle(Geom_Point) GeomP;
  Handle(Standard_Type) PointType = PObj->DynamicType();
  
  if (PointType == STANDARD_TYPE(PGeom_CartesianPoint)) {
    Handle(PGeom_CartesianPoint)& PCPoint = 
      (Handle(PGeom_CartesianPoint)&) PObj;
    GeomP = MgtGeom::Translate(PCPoint);
  }
  return GeomP;
*/

  Handle(PGeom_CartesianPoint)& PCPoint = 
    (Handle(PGeom_CartesianPoint)&) PObj;
  return new Geom_CartesianPoint(PCPoint->Pnt());
}


//=======================================================================
//function : Translate
//purpose  : Translates a Geom_Point to a PGeom_Point.
//         : Optimized valid until new subtypes of GeomPoint exist
//=======================================================================

Handle(PGeom_Point)  MgtGeom::Translate
       (const Handle(Geom_Point)& TObj)
{
/*
  Handle(PGeom_Point) PGeomP;
  Handle(Standard_Type) PointType = TObj->DynamicType();

  if (PointType == STANDARD_TYPE(Geom_CartesianPoint)) {
    Handle(Geom_CartesianPoint)& TCPoint = 
      (Handle(Geom_CartesianPoint)&) TObj;
    PGeomP = MgtGeom::Translate(TCPoint);
  }
  return PGeomP;
*/

  Handle(Geom_CartesianPoint)& TCPoint = 
    (Handle(Geom_CartesianPoint)&) TObj;

  return new PGeom_CartesianPoint(TCPoint->Pnt());
}


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom_RectangularTrimmedSurface to a
//           Geom_RectangularTrimmedSurface.
//=======================================================================

Handle(Geom_RectangularTrimmedSurface)  MgtGeom::Translate
       (const Handle(PGeom_RectangularTrimmedSurface)& PObj)
{
  return new Geom_RectangularTrimmedSurface
    (MgtGeom::Translate(PObj->BasisSurface()),
     PObj->FirstU(),
     PObj->LastU(),
     PObj->FirstV(),
     PObj->LastV(),
     Standard_True, Standard_True); // En attendant leur suppression.
}


//=======================================================================
//function : Translate
//purpose  : Translates a Geom_RectangularTrimmedSurface to a
//           PGeom_RectangularTrimmedSurface.
//=======================================================================

Handle(PGeom_RectangularTrimmedSurface)  MgtGeom::Translate
       (const Handle(Geom_RectangularTrimmedSurface)& TObj)
{
  Standard_Real FirstU, LastU, FirstV, LastV;
  TObj->Bounds(FirstU, LastU, FirstV, LastV);

  return new PGeom_RectangularTrimmedSurface
    (MgtGeom::Translate(TObj->BasisSurface()),
     FirstU, LastU,
     FirstV, LastV);
}


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom_SphericalSurface to a
//           Geom_SphericalSurface.
//=======================================================================

Handle(Geom_SphericalSurface)  MgtGeom::Translate
       (const Handle(PGeom_SphericalSurface)& PObj)
{
  return new Geom_SphericalSurface(PObj->Position(),
				   PObj->Radius());
}


//=======================================================================
//function : Translate
//purpose  : Translates a Geom_SphericalSurface to a
//           PGeom_SphericalSurface.
//=======================================================================

Handle(PGeom_SphericalSurface)  MgtGeom::Translate
       (const Handle(Geom_SphericalSurface)& TObj)
{
  return new PGeom_SphericalSurface(TObj->Position(),
				    TObj->Radius());
}


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom_Surface to a Geom_Surface.
//=======================================================================

Handle(Geom_Surface)  MgtGeom::Translate(const Handle(PGeom_Surface)& PObj)
{
  Handle(Standard_Type) SurfaceType = PObj->DynamicType();

  if (SurfaceType == STANDARD_TYPE(PGeom_Plane)) {
    Handle(PGeom_Plane)& TPlane = 
      (Handle(PGeom_Plane)&) PObj;
    return MgtGeom::Translate(TPlane);
  }
  else if (SurfaceType ==  STANDARD_TYPE(PGeom_CylindricalSurface)) {
    Handle(PGeom_CylindricalSurface)& TCylindricalSurface = 
      (Handle(PGeom_CylindricalSurface)&) PObj;
    return MgtGeom::Translate(TCylindricalSurface);
  }
  else if (SurfaceType ==  STANDARD_TYPE(PGeom_ConicalSurface)) {
    Handle(PGeom_ConicalSurface)& TConicalSurface = 
      (Handle(PGeom_ConicalSurface)&) PObj;
    return MgtGeom::Translate(TConicalSurface);
  }
  else if (SurfaceType ==  STANDARD_TYPE(PGeom_SphericalSurface)) {
    Handle(PGeom_SphericalSurface)& TSphericalSurface = 
      (Handle(PGeom_SphericalSurface)&) PObj;
    return MgtGeom::Translate(TSphericalSurface);
  }
  else if (SurfaceType ==  STANDARD_TYPE(PGeom_ToroidalSurface)) {
    Handle(PGeom_ToroidalSurface)& TToroidalSurface = 
      (Handle(PGeom_ToroidalSurface)&) PObj;
    return MgtGeom::Translate(TToroidalSurface);
  }
  else if (SurfaceType ==  STANDARD_TYPE(PGeom_SurfaceOfLinearExtrusion)) {
    Handle(PGeom_SurfaceOfLinearExtrusion)& TSurfaceOfLinearExtrusion = 
      (Handle(PGeom_SurfaceOfLinearExtrusion)&) PObj;
    return MgtGeom::Translate(TSurfaceOfLinearExtrusion);
  }
  else if (SurfaceType ==  STANDARD_TYPE(PGeom_SurfaceOfRevolution)) {
    Handle(PGeom_SurfaceOfRevolution)& TSurfaceOfRevolution = 
      (Handle(PGeom_SurfaceOfRevolution)&) PObj;
    return MgtGeom::Translate(TSurfaceOfRevolution);
  }
  else if (SurfaceType ==  STANDARD_TYPE(PGeom_BezierSurface)) {
    Handle(PGeom_BezierSurface)& TBezierSurface = 
      (Handle(PGeom_BezierSurface)&) PObj;
    return MgtGeom::Translate(TBezierSurface);
  }
  else if (SurfaceType ==  STANDARD_TYPE(PGeom_BSplineSurface)) {
    Handle(PGeom_BSplineSurface)& TBSplineSurface = 
      (Handle(PGeom_BSplineSurface)&) PObj;
    return MgtGeom::Translate(TBSplineSurface);
  }
  else if (SurfaceType ==  STANDARD_TYPE(PGeom_RectangularTrimmedSurface)) {
    Handle(PGeom_RectangularTrimmedSurface)& TRectangularTrimmedSurface = 
      (Handle(PGeom_RectangularTrimmedSurface)&) PObj;
    return MgtGeom::Translate(TRectangularTrimmedSurface);
  }
  else if (SurfaceType ==  STANDARD_TYPE(PGeom_OffsetSurface)) {
    Handle(PGeom_OffsetSurface)& TOffsetSurface = 
      (Handle(PGeom_OffsetSurface)&) PObj;
    return MgtGeom::Translate(TOffsetSurface);
  }
  else {
    cout << "MgtGeom : Unknown surface type ????"<<endl;
    Standard_NullObject::Raise("No mapping for the current Persistent Surface");

  }
// POP pour NT
  Handle(Geom_Surface) dummy;
  return dummy;
}


//=======================================================================
//function : Translate
//purpose  : Translates a Geom_Surface to a PGeom_Surface.
//=======================================================================

Handle(PGeom_Surface)  MgtGeom::Translate(const Handle(Geom_Surface)& TObj)
{
  Handle(Standard_Type) SurfaceType = TObj->DynamicType();

  if (SurfaceType == STANDARD_TYPE(Geom_Plane)) {
    Handle(Geom_Plane)& PPlane = 
      (Handle(Geom_Plane)&) TObj;
    return MgtGeom::Translate(PPlane);
  }
  else if (SurfaceType ==  STANDARD_TYPE(Geom_CylindricalSurface)) {
    Handle(Geom_CylindricalSurface)& PCylindricalSurface = 
      (Handle(Geom_CylindricalSurface)&) TObj;
    return MgtGeom::Translate(PCylindricalSurface);
  }
  else if (SurfaceType ==  STANDARD_TYPE(Geom_ConicalSurface)) {
    Handle(Geom_ConicalSurface)& PConicalSurface = 
      (Handle(Geom_ConicalSurface)&) TObj;
    return MgtGeom::Translate(PConicalSurface);
  }
  else if (SurfaceType ==  STANDARD_TYPE(Geom_SphericalSurface)) {
    Handle(Geom_SphericalSurface)& PSphericalSurface = 
      (Handle(Geom_SphericalSurface)&) TObj;
    return MgtGeom::Translate(PSphericalSurface);
  }
  else if (SurfaceType ==  STANDARD_TYPE(Geom_ToroidalSurface)) {
    Handle(Geom_ToroidalSurface)& PToroidalSurface = 
      (Handle(Geom_ToroidalSurface)&) TObj;
    return MgtGeom::Translate(PToroidalSurface);
  }
  else if (SurfaceType ==  STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)) {
    Handle(Geom_SurfaceOfLinearExtrusion)& PSurfaceOfLinearExtrusion = 
      (Handle(Geom_SurfaceOfLinearExtrusion)&) TObj;
    return MgtGeom::Translate(PSurfaceOfLinearExtrusion);
  }
  else if (SurfaceType ==  STANDARD_TYPE(Geom_SurfaceOfRevolution)) {
    Handle(Geom_SurfaceOfRevolution)& PSurfaceOfRevolution = 
      (Handle(Geom_SurfaceOfRevolution)&) TObj;
    return MgtGeom::Translate(PSurfaceOfRevolution);
  }
  else if (SurfaceType ==  STANDARD_TYPE(Geom_BezierSurface)) {
    Handle(Geom_BezierSurface)& PBezierSurface = 
      (Handle(Geom_BezierSurface)&) TObj;
    return MgtGeom::Translate(PBezierSurface);
  }
  else if (SurfaceType ==  STANDARD_TYPE(Geom_BSplineSurface)) {
    Handle(Geom_BSplineSurface)& PBSplineSurface = 
      (Handle(Geom_BSplineSurface)&) TObj;
    return MgtGeom::Translate(PBSplineSurface);
  }
  else if (SurfaceType ==  STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
    Handle(Geom_RectangularTrimmedSurface)& PRectangularTrimmedSurface = 
      (Handle(Geom_RectangularTrimmedSurface)&) TObj;
    return MgtGeom::Translate(PRectangularTrimmedSurface);
  }
  else if (SurfaceType ==  STANDARD_TYPE(Geom_OffsetSurface)) {
    Handle(Geom_OffsetSurface)& POffsetSurface = 
      (Handle(Geom_OffsetSurface)&) TObj;
    return MgtGeom::Translate(POffsetSurface);
  }
  else {
    cout << "MgtGeom : Unknown surface type ????"<<endl;
    Standard_NullObject::Raise("No mapping for the current Transient Surface");
  }
// POP pour NT
  Handle(PGeom_Surface) dummy;
  return dummy;
}


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom_SurfaceOfLinearExtrusion to a
//           Geom_SurfaceOfLinearExtrusion.
//=======================================================================

Handle(Geom_SurfaceOfLinearExtrusion)  MgtGeom::Translate
       (const Handle(PGeom_SurfaceOfLinearExtrusion)& PObj)
{
  return new Geom_SurfaceOfLinearExtrusion
    (MgtGeom::Translate(PObj->BasisCurve()),
     PObj->Direction());
}


//=======================================================================
//function : Translate
//purpose  : Translates a Geom_SurfaceOfLinearExtrusion to a
//           PGeom_SurfaceOfLinearExtrusion.
//=======================================================================

Handle(PGeom_SurfaceOfLinearExtrusion)  MgtGeom::Translate
       (const Handle(Geom_SurfaceOfLinearExtrusion)& TObj)
{
  return new PGeom_SurfaceOfLinearExtrusion
    (MgtGeom::Translate(TObj->BasisCurve()),
     TObj->Direction());
}


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom_SurfaceOfRevolution to a
//           Geom_SurfaceOfRevolution.
//=======================================================================

Handle(Geom_SurfaceOfRevolution)  MgtGeom::Translate
       (const Handle(PGeom_SurfaceOfRevolution)& PObj)
{
  gp_Ax1 axis(PObj->Location(), PObj->Direction());
  return new Geom_SurfaceOfRevolution(MgtGeom::Translate(PObj->BasisCurve()),
				      axis);

}


//=======================================================================
//function : Translate
//purpose  : Translates a Geom_SurfaceOfRevolution to a
//           PGeom_SurfaceOfRevolution.
//=======================================================================

Handle(PGeom_SurfaceOfRevolution)  MgtGeom::Translate
       (const Handle(Geom_SurfaceOfRevolution)& TObj)
{
  return new PGeom_SurfaceOfRevolution(MgtGeom::Translate(TObj->BasisCurve()),
				       TObj->Direction(),
				       TObj->Location());
}


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom_ToroidalSurface to a Geom_ToroidalSurface.
//=======================================================================

Handle(Geom_ToroidalSurface)  MgtGeom::Translate
       (const Handle(PGeom_ToroidalSurface)& PObj)
{
  return new Geom_ToroidalSurface(PObj->Position(),
				  PObj->MajorRadius(),
				  PObj->MinorRadius());
}


//=======================================================================
//function : Translate
//purpose  : Translates a Geom_ToroidalSurface to a PGeom_ToroidalSurface.
//=======================================================================

Handle(PGeom_ToroidalSurface)  MgtGeom::Translate
       (const Handle(Geom_ToroidalSurface)& TObj)
{
  return new PGeom_ToroidalSurface(TObj->Position(),
				   TObj->MajorRadius(),
				   TObj->MinorRadius());
}


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom_Transformation to a Geom_Transformation.
//=======================================================================

Handle(Geom_Transformation)  MgtGeom::Translate
       (const Handle(PGeom_Transformation)& PObj)
{ return new Geom_Transformation(PObj->Trsf()); }


//=======================================================================
//function : Translate
//purpose  : Translates a Geom_Transformation to a PGeom_Transformation.
//=======================================================================

Handle(PGeom_Transformation)  MgtGeom::Translate
       (const Handle(Geom_Transformation)& TObj)
{ return new PGeom_Transformation(TObj->Trsf()); }


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom_TrimmedCurve to a Geom_TrimmedCurve.
//=======================================================================

Handle(Geom_TrimmedCurve)  MgtGeom::Translate
       (const Handle(PGeom_TrimmedCurve)& PObj)
{
  return new Geom_TrimmedCurve(MgtGeom::Translate(PObj->BasisCurve()),
			       PObj->FirstU(),
			       PObj->LastU(),
			       Standard_True); // En attendant sa suppression.
}


//=======================================================================
//function : Translate
//purpose  : Translates a Geom_TrimmedCurve to a PGeom_TrimmedCurve.
//=======================================================================

Handle(PGeom_TrimmedCurve)  MgtGeom::Translate
       (const Handle(Geom_TrimmedCurve)& TObj)
{
  return new PGeom_TrimmedCurve(MgtGeom::Translate(TObj->BasisCurve()),
				TObj->FirstParameter(),
				TObj->LastParameter());
}


//=======================================================================
//function : Translate
//purpose  : Translates a PGeom_VectorWithMagnitude to a
//           Geom_VectorWithMagnitude.
//=======================================================================

Handle(Geom_VectorWithMagnitude)  MgtGeom::Translate
       (const Handle(PGeom_VectorWithMagnitude)& PObj)
{ return new Geom_VectorWithMagnitude(PObj->Vec()); }


//=======================================================================
//function : Translate
//purpose  : Translates a Geom_VectorWithMagnitude to a
//           PGeom_VectorWithMagnitude.
//=======================================================================

Handle(PGeom_VectorWithMagnitude)  MgtGeom::Translate
       (const Handle(Geom_VectorWithMagnitude)& TObj)
{ return new PGeom_VectorWithMagnitude(TObj->Vec()); }
