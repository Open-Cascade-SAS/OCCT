// Created on: 1993-03-09
// Created by: JCV
// Copyright (c) 1993-1999 Matra Datavision
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

//Passage en classe persistante - 23/01/91
//Modif suite a la deuxieme revue de projet toolkit Geometry -23/01/91
// pmn : 21/10/95 ; Correction de la methode segment (PRO5853)
// pmn : 31-Dec-96; Bonne gestion des poids (bug PRO4622)
// xab : 07-Jul-97; le cache est instable en degree 21
//       a partir du degree 15 on ne l'utilise plus
// RBD : 15/10/98 ; Le cache est desormais defini sur [-1,1] (pro15537).
// pmn : 10/12/98 ; Update de la methode segment (suite a la modif de cache).

#define No_Standard_OutOfRange
#define No_Standard_DimensionError


#include <BSplCLib.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Geometry.hxx>
#include <gp.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <PLib.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DimensionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>
#include <TColStd_Array1OfInteger.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_BezierSurface,Geom_BoundedSurface)

//=======================================================================
//function : Rational
//purpose  : check rationality of an array of weights
//=======================================================================
static void Rational(const TColStd_Array2OfReal& Weights,
		     Standard_Boolean& Urational,
		     Standard_Boolean& Vrational)
{
  Standard_Integer I,J;
  J = Weights.LowerCol ();
  Vrational = Standard_False;
  while (!Vrational && J <= Weights.UpperCol()) {
    I = Weights.LowerRow();
    while (!Vrational && I <= Weights.UpperRow() - 1) {
      Vrational = (Abs(Weights (I, J) - Weights (I+1, J)) 
                   > Epsilon (Abs(Weights (I, J))));
      I++;
    }
    J++;
  }

  I = Weights.LowerRow ();
  Urational = Standard_False;
  while (!Urational && I <= Weights.UpperRow()) {
    J = Weights.LowerCol();
    while (!Urational && J <= Weights.UpperCol() - 1) {
      Urational = (Abs(Weights (I, J) - Weights (I, J+1))
                   > Epsilon (Abs(Weights (I, J))));
      J++;
    }
    I++;
  }
}

//=======================================================================
//function : AddPoleCol
//purpose  : Internal use only.
//=======================================================================

static void AddPoleCol
  (const TColgp_Array2OfPnt& Poles,
   const TColgp_Array1OfPnt& PoleCol,
   const Standard_Integer    AfterIndex,
         TColgp_Array2OfPnt& NewPoles)
{
  Standard_Integer InsertIndex = AfterIndex + NewPoles.LowerCol();
  Standard_Integer Offset = NewPoles.LowerRow() - PoleCol.Lower();
  Standard_Integer ColIndex = NewPoles.LowerCol();
  Standard_Integer RowIndex;
  while (ColIndex < InsertIndex) {
    RowIndex = NewPoles.LowerRow();
    while (RowIndex <= NewPoles.UpperRow()){
      NewPoles (RowIndex, ColIndex) =  Poles (RowIndex, ColIndex);
      RowIndex++;
    }
    ColIndex++;
  }
  RowIndex = NewPoles.LowerRow();
  while (RowIndex <= NewPoles.UpperRow()){
    NewPoles (RowIndex, ColIndex) = PoleCol (RowIndex - Offset);
    RowIndex++;
  }
  ColIndex++;
  while (ColIndex <= NewPoles.UpperCol()) {
    RowIndex = NewPoles.LowerRow();
    while (RowIndex <= NewPoles.UpperRow()){
      NewPoles (RowIndex, ColIndex) =  Poles (RowIndex, ColIndex - 1);
      RowIndex++;
    }
    ColIndex++;
  }
}

//=======================================================================
//function : AddRatPoleCol
//purpose  : Internal use only.
//=======================================================================

static void AddRatPoleCol
  (const TColgp_Array2OfPnt&   Poles,
   const TColStd_Array2OfReal& Weights,
   const TColgp_Array1OfPnt&   PoleCol,
   const TColStd_Array1OfReal& PoleWeightCol,
   const Standard_Integer      AfterIndex,
         TColgp_Array2OfPnt&   NewPoles,
         TColStd_Array2OfReal& NewWeights)
{
  Standard_Integer InsertIndex = AfterIndex + NewPoles.LowerCol();
  Standard_Integer OffsetPol   = NewPoles.LowerRow() - PoleCol.Lower();
  Standard_Integer OffsetW     = NewWeights.LowerRow() - PoleWeightCol.Lower();
  
  Standard_Integer ColIndex = NewPoles.LowerCol();
  Standard_Integer RowIndex;
  while (ColIndex < InsertIndex) {
    RowIndex = NewPoles.LowerRow();
    while (RowIndex <= NewPoles.UpperRow()){
      NewPoles (RowIndex, ColIndex) = Poles (RowIndex, ColIndex);
      NewWeights (RowIndex, ColIndex)  =  Weights (RowIndex, ColIndex);
      RowIndex++;
    }
    ColIndex++;
  }
  RowIndex = NewPoles.LowerRow();
  while (RowIndex <= NewPoles.UpperRow()){
    NewPoles (RowIndex, ColIndex) = PoleCol (RowIndex - OffsetPol);
    NewWeights (RowIndex, ColIndex) =  PoleWeightCol (RowIndex - OffsetW);
    RowIndex++;
  }
  ColIndex++;
  while (ColIndex <= NewPoles.UpperCol()) {
    RowIndex = NewPoles.LowerRow();
    while (RowIndex <= NewPoles.UpperRow()){
      NewPoles (RowIndex, ColIndex) = Poles (RowIndex, ColIndex - 1);
      RowIndex++;
      NewWeights (RowIndex, ColIndex)  = Weights (RowIndex, ColIndex - 1);
    }
    ColIndex++;
  }
}

//=======================================================================
//function : AddPoleRow
//purpose  : Internal use only.
//=======================================================================

static void AddPoleRow
  (const TColgp_Array2OfPnt& Poles,
   const TColgp_Array1OfPnt& PoleRow,
   const Standard_Integer    AfterIndex,
         TColgp_Array2OfPnt& NewPoles)
{
  Standard_Integer InsertIndex = AfterIndex + NewPoles.LowerRow();
  Standard_Integer Offset = NewPoles.LowerCol() - PoleRow.Lower();
  Standard_Integer RowIndex = NewPoles.LowerRow();
  Standard_Integer ColIndex;
  while (RowIndex < InsertIndex) {
    ColIndex = NewPoles.LowerCol();
    while (ColIndex <= NewPoles.UpperCol()){
      NewPoles (RowIndex, ColIndex) = Poles (RowIndex, ColIndex);
      ColIndex++;
    }
    RowIndex++;
  }
  ColIndex = NewPoles.LowerCol();
  while (ColIndex <= NewPoles.UpperCol()){
    NewPoles (RowIndex, ColIndex) = PoleRow (ColIndex - Offset);
    ColIndex++;
  }
  RowIndex++;
  while (RowIndex <= NewPoles.UpperRow()) {
    ColIndex = NewPoles.LowerCol();
    while (ColIndex <= NewPoles.UpperCol()){
      NewPoles (RowIndex, ColIndex) = Poles (RowIndex - 1, ColIndex);
      ColIndex++;
    }
    RowIndex++;
  }
}

//=======================================================================
//function : AddRatPoleRow
//purpose  : 
//=======================================================================

static void AddRatPoleRow
  (const TColgp_Array2OfPnt&   Poles,
   const TColStd_Array2OfReal& Weights,
   const TColgp_Array1OfPnt&   PoleRow,
   const TColStd_Array1OfReal& PoleWeightRow,
   const Standard_Integer      AfterIndex,
         TColgp_Array2OfPnt&   NewPoles,
         TColStd_Array2OfReal& NewWeights)
{
  Standard_Integer InsertIndex = AfterIndex + NewPoles.LowerRow();
  Standard_Integer OffsetPol = NewPoles.LowerCol() - PoleRow.Lower();
  Standard_Integer OffsetW = NewWeights.LowerCol() - PoleWeightRow.Lower();
  
  Standard_Integer ColIndex;
  Standard_Integer RowIndex = NewPoles.LowerRow();
  while (RowIndex < InsertIndex) {
    ColIndex = NewPoles.LowerCol();
    while (ColIndex <= NewPoles.UpperCol()){
      NewPoles (RowIndex, ColIndex) =  Poles (RowIndex, ColIndex);
      NewWeights (RowIndex, ColIndex)  =  Weights (RowIndex, ColIndex);
      ColIndex++;
    }
    RowIndex++;
  }
  ColIndex = NewPoles.LowerCol();
  while (ColIndex <= NewPoles.UpperCol()){
    NewPoles (RowIndex, ColIndex) = PoleRow (ColIndex - OffsetPol);
    NewWeights (RowIndex, ColIndex) = PoleWeightRow (ColIndex - OffsetW);
    ColIndex++;
  }
  RowIndex++;
  while (RowIndex <= NewPoles.UpperRow()) {
    ColIndex = NewPoles.LowerCol();
    while (ColIndex <= NewPoles.UpperCol()){
      NewPoles (RowIndex, ColIndex) = Poles (RowIndex - 1, ColIndex);
      NewWeights (RowIndex, ColIndex)  = Weights (RowIndex - 1, ColIndex);
      ColIndex++;
    }
    RowIndex++;
  }
}

//=======================================================================
//function : DeletePoleCol
//purpose  : 
//=======================================================================

static void DeletePoleCol
  (const TColgp_Array2OfPnt& Poles,
   const Standard_Integer    Index,
         TColgp_Array2OfPnt& NewPoles)
{
  Standard_Integer Offset = 0;
  Standard_Integer RowIndex;
  Standard_Integer ColIndex = NewPoles.LowerCol();
  while (ColIndex <= NewPoles.UpperCol()) {
    RowIndex = NewPoles.LowerRow();
    if (ColIndex == Index)  Offset = 1;
    while (RowIndex <= NewPoles.UpperRow()){
      NewPoles (RowIndex, ColIndex) = Poles (RowIndex, ColIndex + Offset);
      RowIndex++;
    }
    ColIndex++;
  }
}

//=======================================================================
//function : DeleteRatPoleCol
//purpose  : 
//=======================================================================

static void DeleteRatPoleCol
  (const TColgp_Array2OfPnt&   Poles,
   const TColStd_Array2OfReal& Weights,
   const Standard_Integer      Index,
         TColgp_Array2OfPnt&   NewPoles,
         TColStd_Array2OfReal& NewWeights)
{
  Standard_Integer Offset = 0;
  Standard_Integer RowIndex;
  Standard_Integer ColIndex = NewPoles.LowerCol();
  while (ColIndex <= NewPoles.UpperCol()) {
    RowIndex = NewPoles.LowerRow();
    if (ColIndex == Index) Offset = 1;
    while (RowIndex <= NewPoles.UpperRow()){
      NewPoles (RowIndex, ColIndex) = Poles (RowIndex, ColIndex + Offset);
      NewWeights (RowIndex, ColIndex) = Weights (RowIndex, ColIndex+Offset);
      RowIndex++;
    }
    ColIndex++;
  }
}

//=======================================================================
//function : DeletePoleRow
//purpose  : 
//=======================================================================

static void DeletePoleRow
  (const TColgp_Array2OfPnt& Poles,
   const Standard_Integer    Index,
         TColgp_Array2OfPnt& NewPoles)
{
  Standard_Integer Offset = 0;
  Standard_Integer ColIndex;
  Standard_Integer RowIndex = NewPoles.LowerRow();
  while (RowIndex <= NewPoles.UpperRow()) {
    ColIndex = NewPoles.LowerCol();
    if (RowIndex == Index)  Offset = 1;
    while (ColIndex <= NewPoles.UpperCol()){
      NewPoles (RowIndex, ColIndex) = Poles (RowIndex + Offset, ColIndex);
      ColIndex++;
    }
    RowIndex++;
  }
}

//=======================================================================
//function : DeleteRatPoleRow
//purpose  : 
//=======================================================================

static void DeleteRatPoleRow
  (const TColgp_Array2OfPnt&   Poles,
   const TColStd_Array2OfReal& Weights,
   const Standard_Integer      Index,
         TColgp_Array2OfPnt&   NewPoles,
         TColStd_Array2OfReal& NewWeights)
{
  Standard_Integer Offset = 0;
  Standard_Integer ColIndex;
  Standard_Integer RowIndex = NewPoles.LowerRow();
  while (RowIndex <= NewPoles.UpperRow()) {
    ColIndex = NewPoles.LowerCol();
    if (RowIndex == Index)  Offset = 1;
    while (ColIndex <= NewPoles.UpperCol()){
      NewPoles (RowIndex, ColIndex) =  Poles (RowIndex +  Offset, ColIndex);
      NewWeights (RowIndex, ColIndex) = Weights (RowIndex+Offset, ColIndex);
      ColIndex++;
    }
    RowIndex++;
  }
}

//=======================================================================
//function : Geom_BezierSurface
//purpose  : 
//=======================================================================

Geom_BezierSurface::Geom_BezierSurface 
(const TColgp_Array2OfPnt& SurfacePoles):
 maxderivinvok(Standard_False)
{
  Standard_Integer NbUPoles = SurfacePoles.ColLength();
  Standard_Integer NbVPoles = SurfacePoles.RowLength();
  if (NbUPoles < 2 || NbUPoles > MaxDegree()+1 ||
      NbVPoles < 2 || NbVPoles > MaxDegree()+1) {
    throw Standard_ConstructionError();
  }
  
  Handle(TColgp_HArray2OfPnt) npoles = 
    new TColgp_HArray2OfPnt   (1, NbUPoles, 1, NbVPoles);
  
  urational = 0;
  vrational = 0;

  npoles->ChangeArray2() = SurfacePoles;
  
  // Init non rational
  Init(npoles,
       Handle(TColStd_HArray2OfReal)());
}

//=======================================================================
//function : Geom_BezierSurface
//purpose  : 
//=======================================================================

Geom_BezierSurface::Geom_BezierSurface 
  (const TColgp_Array2OfPnt&   SurfacePoles,
   const TColStd_Array2OfReal& PoleWeights  ):
 maxderivinvok(Standard_False)
{
  Standard_Integer NbUPoles = SurfacePoles.ColLength();
  Standard_Integer NbVPoles = SurfacePoles.RowLength();
  if (NbUPoles < 2 || NbUPoles > MaxDegree()+1 ||
      NbVPoles < 2 || NbVPoles > MaxDegree()+1 ||
      NbVPoles != PoleWeights.RowLength()      ||
      NbUPoles != PoleWeights.ColLength()      ) {
    throw Standard_ConstructionError();
  }
  
  Standard_Integer Row = PoleWeights.LowerRow();
  Standard_Integer Col = PoleWeights.LowerCol();
  while (Col <= PoleWeights.UpperCol()) {
    Row = PoleWeights.LowerRow();
    while (Row <= PoleWeights.UpperRow()) {
      if (PoleWeights(Row, Col) <= gp::Resolution()) {
        throw Standard_ConstructionError();
      }
      Row++;
    }
    Col++;
  }
  
  Handle(TColgp_HArray2OfPnt) 
    npoles = new TColgp_HArray2OfPnt   (1, NbUPoles, 1, NbVPoles);
  npoles->ChangeArray2() = SurfacePoles;

  Standard_Integer I, J;
  urational = Standard_False;
  vrational = Standard_False;
  J = PoleWeights.LowerCol ();
  while (!vrational && J <= PoleWeights.UpperCol()) {
    I = PoleWeights.LowerRow();
    while (!vrational && I <= PoleWeights.UpperRow() - 1) {
      vrational = (Abs(PoleWeights (I, J) - PoleWeights (I+1, J)) 
                   > Epsilon (Abs(PoleWeights (I, J))));
      I++;
    }
    J++;
  }
  I = PoleWeights.LowerRow ();
  while (!urational && I <= PoleWeights.UpperRow()) {
    J = PoleWeights.LowerCol();
    while (!urational && J <= PoleWeights.UpperCol() - 1) {
      urational = (Abs(PoleWeights (I, J) - PoleWeights (I, J+1))
                   > Epsilon (Abs(PoleWeights (I, J))));
      J++;
    }
    I++;
  }


  Handle(TColStd_HArray2OfReal) nweights; 
  if (urational || vrational) {
    nweights = new TColStd_HArray2OfReal (1, NbUPoles, 1, NbVPoles);
    nweights->ChangeArray2() = PoleWeights;
  }

  // Init
  Init(npoles,nweights);
}

//=======================================================================
//function : Geom_BezierSurface
//purpose  : 
//=======================================================================

Geom_BezierSurface::Geom_BezierSurface
  (const Handle(TColgp_HArray2OfPnt)&   SurfacePoles,
   const Handle(TColStd_HArray2OfReal)& PoleWeights,
   const Standard_Boolean               IsURational,
   const Standard_Boolean               IsVRational)
:maxderivinvok(Standard_False)
{
  urational = IsURational;
  vrational = IsVRational;
  Standard_Integer NbUPoles = SurfacePoles->ColLength();
  Standard_Integer NbVPoles = SurfacePoles->RowLength();

  poles    = new TColgp_HArray2OfPnt   (1,NbUPoles,
					1,NbVPoles) ;
  poles->ChangeArray2() = SurfacePoles->Array2();

  if ( urational || vrational) {
    weights  = new TColStd_HArray2OfReal (1,NbUPoles,1,NbVPoles);
    weights->ChangeArray2() = PoleWeights->Array2();
  }
}

//=======================================================================
//function : MaxDegree
//purpose  : 
//=======================================================================

Standard_Integer Geom_BezierSurface::MaxDegree () 
{ 
  return BSplCLib::MaxDegree(); 
}

//=======================================================================
//function : ExchangeUV
//purpose  : 
//=======================================================================

void Geom_BezierSurface::ExchangeUV ()
{
  Standard_Integer LC = poles->LowerCol();
  Standard_Integer UC = poles->UpperCol();
  Standard_Integer LR = poles->LowerRow();
  Standard_Integer UR = poles->UpperRow();

  Handle(TColgp_HArray2OfPnt) npoles = 
    new TColgp_HArray2OfPnt (LC, UC, LR, UR);
  Handle(TColStd_HArray2OfReal) nweights =
    new TColStd_HArray2OfReal (LC, UC, LR, UR);

  const TColgp_Array2OfPnt   & spoles   = poles->Array2();
  const TColStd_Array2OfReal & sweights = weights->Array2();

  TColgp_Array2OfPnt&   snpoles   = npoles->ChangeArray2();
  TColStd_Array2OfReal& snweights = nweights->ChangeArray2();
 
  Standard_Integer i, j;
  for (i = LC; i <= UC; i++) {
    for (j = LR; j <= UR; j++) {
      snpoles   (i,j) = spoles   (j,i);
      snweights (i,j) = sweights (j,i);
    }
  }

  poles   = npoles;
  weights = nweights;

  Standard_Boolean temp = urational;
  urational = vrational;
  vrational = temp;
}

//=======================================================================
//function : Increase
//purpose  : 
//=======================================================================

void Geom_BezierSurface::Increase (const Standard_Integer UDeg,
				   const Standard_Integer VDeg)
{
  if (UDeg < UDegree() || UDeg > Geom_BezierSurface::MaxDegree() ||
      VDeg < VDegree() || VDeg > Geom_BezierSurface::MaxDegree() )  {
    throw Standard_ConstructionError();
  }

  Standard_Integer oldUDeg = UDegree();
  Standard_Integer oldVDeg = VDegree();
  Standard_Integer IncUDeg = UDeg - oldUDeg;
  Standard_Integer IncVDeg = VDeg - oldVDeg;
  if (IncUDeg == 0 && IncVDeg == 0) return;
  
  TColStd_Array1OfReal biduknots(1,2); biduknots(1) = 0.; biduknots(2) = 1.;
  TColStd_Array1OfInteger bidumults(1,2); bidumults.Init(UDegree() + 1);
  TColStd_Array1OfReal bidvknots(1,2); bidvknots(1) = 0.; bidvknots(2) = 1.;
  TColStd_Array1OfInteger bidvmults(1,2); bidvmults.Init(VDegree() + 1);
  Handle(TColgp_HArray2OfPnt) npoles;
  Handle(TColStd_HArray2OfReal) nweights;

  if(IncUDeg > 0){
    npoles = new TColgp_HArray2OfPnt( 1, UDeg + 1, 1, oldVDeg + 1);
    
    if ( urational || vrational) {
      nweights = new TColStd_HArray2OfReal( 1, UDeg + 1, 1, VDegree() + 1);
      
      BSplSLib::IncreaseDegree(1, oldUDeg, UDeg, 0,
			       poles->Array2(),
			       &weights->Array2(),
			       biduknots, bidumults,
			       npoles->ChangeArray2(), 
			       &nweights->ChangeArray2(),
			       biduknots, bidumults);
      weights = nweights;
    }
    else {
      BSplSLib::IncreaseDegree(1, oldUDeg, UDeg, 0,
			       poles->Array2(),
			       BSplSLib::NoWeights(),
			       biduknots, bidumults,
			       npoles->ChangeArray2(), 
			       BSplSLib::NoWeights(),
			       biduknots, bidumults);
    }
    poles   = npoles;
  }
  if(IncVDeg > 0){
    npoles = new TColgp_HArray2OfPnt( 1, UDeg + 1, 1, VDeg + 1);
    
    if ( urational || vrational) {
      nweights = new TColStd_HArray2OfReal( 1, UDeg + 1, 1, VDeg + 1);
      
      BSplSLib::IncreaseDegree(0, oldVDeg, VDeg, 0,
			       poles->Array2(),
			       &weights->Array2(),
			       bidvknots, bidvmults,
			       npoles->ChangeArray2(), 
			       &nweights->ChangeArray2(),
			       bidvknots, bidvmults);
      weights = nweights;
    }
    else {
      BSplSLib::IncreaseDegree(0, oldVDeg, VDeg, 0,
			       poles->Array2(),
			       BSplSLib::NoWeights(),
			       bidvknots, bidvmults,
			       npoles->ChangeArray2(), 
			       BSplSLib::NoWeights(),
			       bidvknots, bidvmults);
      
    }
    poles   = npoles;
  }
  Init(npoles,nweights);
}

//=======================================================================
//function : InsertPoleColAfter
//purpose  : 
//=======================================================================

void Geom_BezierSurface::InsertPoleColAfter 
  (const Standard_Integer    VIndex,
   const TColgp_Array1OfPnt& CPoles)
{
  const TColgp_Array2OfPnt & Poles = poles->Array2();
  if (VIndex < 1 || VIndex > Poles.RowLength())  throw Standard_OutOfRange();
  if (CPoles.Length() != Poles.ColLength()) {
    throw Standard_ConstructionError();
  }

  Handle(TColgp_HArray2OfPnt) npoles =
    new TColgp_HArray2OfPnt(1,poles->ColLength(),1,poles->RowLength()+1);

  Handle(TColStd_HArray2OfReal) nweights;

  if (urational || vrational) {
    nweights = 
      new TColStd_HArray2OfReal(1,poles->ColLength(),1,poles->RowLength()+1);

    TColStd_Array1OfReal CWeights(nweights->LowerRow(),nweights->UpperRow());
    CWeights.Init(1.);

    AddRatPoleCol (poles->Array2(), weights->Array2(),
		   CPoles, CWeights, VIndex,
		   npoles->ChangeArray2(), nweights->ChangeArray2());
  }
  else {
    AddPoleCol (poles->Array2(), 
		CPoles, VIndex,
		npoles->ChangeArray2());
  }
  poles   = npoles;
  weights = nweights;
}

//=======================================================================
//function : InsertPoleColAfter
//purpose  : 
//=======================================================================

void Geom_BezierSurface::InsertPoleColAfter
  (const Standard_Integer      VIndex,
   const TColgp_Array1OfPnt&   CPoles,
   const TColStd_Array1OfReal& CPoleWeights)
{
  const TColgp_Array2OfPnt & Poles = poles->Array2();
  if (VIndex < 1 || VIndex > Poles.RowLength())  throw Standard_OutOfRange();
  if (CPoles.Length() != Poles.ColLength() ||
      CPoleWeights.Length() != CPoles.Length()) {
    throw Standard_ConstructionError();
    }
  Standard_Integer Index = CPoleWeights.Lower();
  while (Index <= CPoleWeights.Upper()) {
    if (CPoleWeights (Index) <= gp::Resolution()) {
      throw Standard_ConstructionError();
      }
    Index++;
  }

  Handle(TColgp_HArray2OfPnt) npoles =
    new TColgp_HArray2OfPnt(1,poles->ColLength(),1,poles->RowLength()+1);
  
  Handle(TColStd_HArray2OfReal) nweights = 
    new TColStd_HArray2OfReal(1,poles->ColLength(),1,poles->RowLength()+1);
  
  AddRatPoleCol (poles->Array2(), weights->Array2(),
		 CPoles, CPoleWeights, VIndex,
		 npoles->ChangeArray2(), nweights->ChangeArray2());

  poles   = npoles;
  weights = nweights;

  Rational(weights->Array2(), urational, vrational);
}

//=======================================================================
//function : InsertPoleColBefore
//purpose  : 
//=======================================================================

void Geom_BezierSurface::InsertPoleColBefore (const Standard_Integer VIndex,
					      const TColgp_Array1OfPnt& CPoles)
{
  InsertPoleColAfter(VIndex - 1, CPoles);
}

//=======================================================================
//function : InsertPoleColBefore
//purpose  : 
//=======================================================================

void Geom_BezierSurface::InsertPoleColBefore
  (const Standard_Integer      VIndex,
   const TColgp_Array1OfPnt&   CPoles,
   const TColStd_Array1OfReal& CPoleWeights)
{
  InsertPoleColAfter( VIndex - 1, CPoles, CPoleWeights);
}

//=======================================================================
//function : InsertPoleRowAfter
//purpose  : 
//=======================================================================

void Geom_BezierSurface::InsertPoleRowAfter (const Standard_Integer UIndex,
					     const TColgp_Array1OfPnt& CPoles)
{
  const TColgp_Array2OfPnt & Poles = poles->Array2();
  if (UIndex < 1 || UIndex > Poles.ColLength())  throw Standard_OutOfRange();
  if (CPoles.Length() != Poles.RowLength()) {
    throw Standard_ConstructionError();
  }

  Handle(TColgp_HArray2OfPnt) npoles =
    new TColgp_HArray2OfPnt(1,poles->ColLength()+1,1,poles->RowLength());

  Handle(TColStd_HArray2OfReal) nweights;

  if (urational || vrational) {
    nweights = 
      new TColStd_HArray2OfReal(1,poles->ColLength()+1,1,poles->RowLength());

//    TColStd_Array1OfReal CWeights(nweights->LowerCol(),nweights->UpperCol(),
//				  1.0); ???????????
    TColStd_Array1OfReal CWeights(1.0,
                                  nweights->LowerCol(),nweights->UpperCol());

    AddRatPoleRow (poles->Array2(), weights->Array2(),
		   CPoles, CWeights, UIndex,
		   npoles->ChangeArray2(), nweights->ChangeArray2());
  }
  else {
    AddPoleRow (poles->Array2(), 
		CPoles, UIndex,
		npoles->ChangeArray2());
  }
  poles   = npoles;
  weights = nweights;
}

//=======================================================================
//function : InsertPoleRowAfter
//purpose  : 
//=======================================================================

void Geom_BezierSurface::InsertPoleRowAfter
  (const Standard_Integer      UIndex,
   const TColgp_Array1OfPnt&   CPoles,
   const TColStd_Array1OfReal& CPoleWeights)
{
  const TColgp_Array2OfPnt & Poles = poles->Array2();
  if (UIndex < 1 || UIndex > Poles.ColLength())  throw Standard_OutOfRange();
  if (CPoles.Length() != Poles.RowLength() ||
      CPoleWeights.Length() != CPoles.Length()) {
    throw Standard_ConstructionError();
  }
  Standard_Integer Index = CPoleWeights.Lower();
  while (Index <= CPoleWeights.Upper()) {
    if (CPoleWeights(Index) <= gp::Resolution()) {
      throw Standard_ConstructionError();
    }
    Index++;
  }

  Handle(TColgp_HArray2OfPnt) npoles =
    new TColgp_HArray2OfPnt(1,poles->ColLength()+1,1,poles->RowLength());
  
  Handle(TColStd_HArray2OfReal) nweights = 
    new TColStd_HArray2OfReal(1,poles->ColLength()+1,1,poles->RowLength());
  
  AddRatPoleCol (poles->Array2(), weights->Array2(),
		 CPoles, CPoleWeights, UIndex,
		 npoles->ChangeArray2(), nweights->ChangeArray2());

  poles   = npoles;
  weights = nweights;

  Rational(weights->Array2(), urational, vrational);
}

//=======================================================================
//function : InsertPoleRowBefore
//purpose  : 
//=======================================================================

void Geom_BezierSurface::InsertPoleRowBefore (const Standard_Integer UIndex,
					      const TColgp_Array1OfPnt& CPoles)
{
  InsertPoleRowAfter( UIndex - 1, CPoles);
}

//=======================================================================
//function : InsertPoleRowBefore
//purpose  : 
//=======================================================================

void Geom_BezierSurface::InsertPoleRowBefore
  (const Standard_Integer      UIndex, 
   const TColgp_Array1OfPnt&   CPoles,
   const TColStd_Array1OfReal& CPoleWeights)
{
  InsertPoleRowAfter( UIndex - 1, CPoles, CPoleWeights);
} 

//=======================================================================
//function : RemovePoleCol
//purpose  : 
//=======================================================================

void Geom_BezierSurface::RemovePoleCol (const Standard_Integer VIndex)
{
  const TColgp_Array2OfPnt & Poles = poles->Array2();
  if (VIndex < 1 || VIndex > Poles.RowLength())  throw Standard_OutOfRange();
  if (Poles.RowLength() <= 2)             throw Standard_ConstructionError();

  Handle(TColgp_HArray2OfPnt) npoles =
    new TColgp_HArray2OfPnt(1,poles->ColLength(),1,poles->RowLength()-1);

  Handle(TColStd_HArray2OfReal) nweights;

  if (urational || vrational) {
    nweights = 
      new TColStd_HArray2OfReal(1,poles->ColLength(),1,poles->RowLength()-1);

    DeleteRatPoleCol (poles->Array2(), weights->Array2(),
		      VIndex,
		      npoles->ChangeArray2(), nweights->ChangeArray2());
    // Mise a jour de la rationalite
    Rational(nweights->Array2(), urational, vrational);
  }
  else {
    DeletePoleCol (poles->Array2(), 
		   VIndex,
		   npoles->ChangeArray2());
  }
  poles   = npoles;
  weights = nweights;
}

//=======================================================================
//function : RemovePoleRow
//purpose  : 
//=======================================================================

void Geom_BezierSurface::RemovePoleRow (const Standard_Integer UIndex)
{
  const TColgp_Array2OfPnt & Poles = poles->Array2();
  if (UIndex < 1 || UIndex > Poles.ColLength()) throw Standard_OutOfRange();
  if (Poles.ColLength() <= 2)            throw Standard_ConstructionError();

  Handle(TColgp_HArray2OfPnt) npoles =
    new TColgp_HArray2OfPnt(1,poles->ColLength()-1,1,poles->RowLength());

  Handle(TColStd_HArray2OfReal) nweights;

  if (urational || vrational) {
    nweights = 
      new TColStd_HArray2OfReal(1,poles->ColLength()-1,1,poles->RowLength());

    DeleteRatPoleRow (poles->Array2(), weights->Array2(),
		      UIndex,
		      npoles->ChangeArray2(), nweights->ChangeArray2());

    // Mise a jour de la rationalite
    Rational(nweights->Array2(), urational, vrational);
  }
  else {
    DeletePoleRow (poles->Array2(), 
		   UIndex,
		   npoles->ChangeArray2());
  }
  poles   = npoles;
  weights = nweights;
}

//=======================================================================
//function : Segment
//purpose  : 
//=======================================================================

void Geom_BezierSurface::Segment
  (const Standard_Real U1,
   const Standard_Real U2,
   const Standard_Real V1,
   const Standard_Real V2)
{
  Standard_Boolean rat = (urational || vrational);
  Handle(TColgp_HArray2OfPnt)  Coefs;
  Handle(TColStd_HArray2OfReal) WCoefs;
  
  Standard_Integer aMinDegree = UDegree() <= VDegree() ? UDegree() : VDegree();
  Standard_Integer aMaxDegree = UDegree() >  VDegree() ? UDegree() : VDegree();
  Coefs = new TColgp_HArray2OfPnt(1, aMaxDegree + 1, 1, aMinDegree + 1);
  if (rat)
    WCoefs = new TColStd_HArray2OfReal(1, aMaxDegree + 1, 1, aMinDegree + 1);

  TColStd_Array1OfReal biduflatknots(BSplCLib::FlatBezierKnots(UDegree()), 1, 2 * (UDegree() + 1));
  TColStd_Array1OfReal bidvflatknots(BSplCLib::FlatBezierKnots(VDegree()), 1, 2 * (VDegree() + 1));

  Standard_Real uparameter_11 = 0.5;
  Standard_Real uspanlenght_11 = 0.5;
  Standard_Real vparameter_11 = 0.5;
  Standard_Real vspanlenght_11 = 0.5;

  if (urational || vrational) {
    BSplSLib::BuildCache(uparameter_11, vparameter_11,
      uspanlenght_11, vspanlenght_11, 0, 0,
      UDegree(), VDegree(), 0, 0,
      biduflatknots, bidvflatknots,
      poles->Array2(),
      &weights->Array2(),
      Coefs->ChangeArray2(),
      &WCoefs->ChangeArray2());
  }
  else {
    BSplSLib::BuildCache(uparameter_11, vparameter_11,
      uspanlenght_11, vspanlenght_11, 0, 0,
      UDegree(), VDegree(), 0, 0,
      biduflatknots, bidvflatknots,
      poles->Array2(),
      BSplSLib::NoWeights(),
      Coefs->ChangeArray2(),
      BSplSLib::NoWeights());
  }

  // Attention si udeg <= vdeg u et v sont intervertis 
  // dans les coeffs, il faut donc tout transposer.
  if(UDegree() <= VDegree()) {
    Handle(TColgp_HArray2OfPnt)  coeffs = Coefs;
    Handle(TColStd_HArray2OfReal) wcoeffs = WCoefs;
    Standard_Integer ii, jj;
    Coefs = new  (TColgp_HArray2OfPnt)(1,UDegree()+1,1,VDegree()+1);
    if (rat) {
      WCoefs = new  (TColStd_HArray2OfReal)(1,UDegree()+1,1,VDegree()+1);
    }
    for (ii=1; ii<=UDegree()+1; ii++)
      for (jj=1; jj<=VDegree()+1; jj++) {
	Coefs->SetValue(ii, jj, coeffs->Value(jj,ii));
	if (rat)  WCoefs->SetValue(ii, jj, wcoeffs->Value(jj,ii));
      }
  }

// Trim dans la base cannonique et Update des Poles et Coeffs

// PMN : tranfo sur les parametres
  Standard_Real ufirst = 2*(U1 - 0.5),
                ulast =  2*(U2 - 0.5),
                vfirst = 2*(V1 - 0.5),
                vlast  = 2*(V2 - 0.5);
  if (rat) {
    PLib::UTrimming (ufirst, ulast, Coefs->ChangeArray2(),
		     &WCoefs->ChangeArray2());
    PLib::VTrimming (vfirst, vlast, Coefs->ChangeArray2(),
		     &WCoefs->ChangeArray2());
    PLib::CoefficientsPoles(Coefs->Array2(),
			    &WCoefs->Array2(),
			    poles->ChangeArray2(),
			    &weights->ChangeArray2());
  }
  else {
    PLib::UTrimming (ufirst, ulast, Coefs->ChangeArray2(), PLib::NoWeights2());
    PLib::VTrimming (vfirst, vlast, Coefs->ChangeArray2(), PLib::NoWeights2());
    PLib::CoefficientsPoles (Coefs->Array2(), PLib::NoWeights2(),
			                       poles->ChangeArray2(), PLib::NoWeights2());
  }
}

//=======================================================================
//function : SetPole
//purpose  : 
//=======================================================================

void Geom_BezierSurface::SetPole
  (const Standard_Integer UIndex,
   const Standard_Integer VIndex,
   const gp_Pnt&          P)
{
  TColgp_Array2OfPnt & Poles = poles->ChangeArray2();
  if (UIndex < 1                 ||
      UIndex > Poles.ColLength() ||
      VIndex < 1                 ||
      VIndex > Poles.RowLength() )    throw Standard_OutOfRange();
  
  Poles (UIndex, VIndex) = P;
}

//=======================================================================
//function : SetPole
//purpose  : 
//=======================================================================

void Geom_BezierSurface::SetPole
  (const Standard_Integer UIndex,
   const Standard_Integer VIndex, 
   const gp_Pnt&          P,
   const Standard_Real    Weight)
{

  if (Weight <= gp::Resolution())     
    throw Standard_ConstructionError("Geom_BezierSurface::SetPole");
  if (UIndex < 1                 || 
      UIndex > poles->ColLength() ||
      VIndex < 1                 ||
      VIndex > poles->RowLength())    
    throw Standard_OutOfRange("Geom_BezierSurface::SetPole");

  poles->SetValue(UIndex, VIndex, P);
  
  SetWeight(UIndex, VIndex,  Weight); //L'update des coeff est fait la dedans
}

//=======================================================================
//function : SetPoleCol
//purpose  : 
//=======================================================================

void Geom_BezierSurface::SetPoleCol
  (const Standard_Integer      VIndex,
   const TColgp_Array1OfPnt&   CPoles,
   const TColStd_Array1OfReal& CPoleWeights)
{
  TColgp_Array2OfPnt & Poles = poles->ChangeArray2();
  if (VIndex < 1 || VIndex > Poles.RowLength()) throw Standard_OutOfRange();

  if (CPoles.Lower() < 1                     || 
      CPoles.Lower() > Poles.ColLength()     || 
      CPoles.Upper() < 1                     || 
      CPoles.Upper() > Poles.ColLength()     ||
      CPoleWeights.Lower() != CPoles.Lower() ||
      CPoleWeights.Upper() != CPoles.Upper()) {
    throw Standard_ConstructionError();
  }
     
  Standard_Integer I;
  for (I = CPoles.Lower();  I <= CPoles.Upper(); I++) {
    Poles (I, VIndex) = CPoles (I);
  }
  SetWeightCol(VIndex, CPoleWeights); //Avec l'update
}

//=======================================================================
//function : SetPoleCol
//purpose  : 
//=======================================================================

void Geom_BezierSurface::SetPoleCol (const Standard_Integer      VIndex,
				     const TColgp_Array1OfPnt&   CPoles)
{
  TColgp_Array2OfPnt & Poles = poles->ChangeArray2();
  if (VIndex < 1 || VIndex > Poles.RowLength())  throw Standard_OutOfRange();

  if (CPoles.Lower() < 1                 || 
      CPoles.Lower() > Poles.ColLength() || 
      CPoles.Upper() < 1                 ||
      CPoles.Upper() > Poles.ColLength()) {
    throw Standard_ConstructionError();
  }
  for (Standard_Integer I = CPoles.Lower(); I <= CPoles.Upper(); I++) {
    Poles (I, VIndex) = CPoles (I);
  }
}

//=======================================================================
//function : SetPoleRow
//purpose  : 
//=======================================================================

void Geom_BezierSurface::SetPoleRow (const Standard_Integer    UIndex,
				     const TColgp_Array1OfPnt& CPoles)
{
  TColgp_Array2OfPnt & Poles = poles->ChangeArray2();
  if (UIndex < 1 || UIndex > Poles.ColLength())  throw Standard_OutOfRange();

  if (CPoles.Lower() < 1                 ||
      CPoles.Lower() > Poles.RowLength() || 
      CPoles.Upper() < 1                 || 
      CPoles.Upper() > Poles.RowLength())  throw Standard_ConstructionError();

  for (Standard_Integer I = CPoles.Lower(); I <= CPoles.Upper(); I++) {
    Poles (UIndex, I) = CPoles (I);
  }
}

//=======================================================================
//function : SetPoleRow
//purpose  : 
//=======================================================================

void Geom_BezierSurface::SetPoleRow
  (const Standard_Integer      UIndex,
   const TColgp_Array1OfPnt&   CPoles,
   const TColStd_Array1OfReal& CPoleWeights)
{
  TColgp_Array2OfPnt & Poles = poles->ChangeArray2();
  if (UIndex < 1 || UIndex > Poles.ColLength())   throw Standard_OutOfRange();

  if (CPoles.Lower() < 1                     || 
      CPoles.Lower() > Poles.RowLength()     || 
      CPoles.Upper() < 1                     || 
      CPoles.Upper() > Poles.RowLength()     ||
      CPoleWeights.Lower() != CPoles.Lower() ||
      CPoleWeights.Upper() != CPoles.Upper()) {
    throw Standard_ConstructionError();
  }

  Standard_Integer I;

  for (I = CPoles.Lower(); I <= CPoles.Upper(); I++) {
    Poles   (UIndex, I) = CPoles (I);
  }
  
  SetWeightRow(UIndex, CPoleWeights); //Avec l'update 
}

//=======================================================================
//function : SetWeight
//purpose  : 
//=======================================================================

void Geom_BezierSurface::SetWeight (const Standard_Integer UIndex,
				    const Standard_Integer VIndex,
				    const Standard_Real    Weight)
{
   // compute new rationality
  Standard_Boolean wasrat = (urational||vrational);
  if (!wasrat) {
    // a weight of 1. does not turn to rational
    if (Abs(Weight - 1.) <= gp::Resolution())
      return;

    // set weights of 1.
    weights = new TColStd_HArray2OfReal (1, poles->ColLength(),
					 1, poles->RowLength(), 1.);
  }

  TColStd_Array2OfReal & Weights = weights->ChangeArray2();
  if (Weight <= gp::Resolution())      
    throw Standard_ConstructionError("Geom_BezierSurface::SetWeight");

  if (UIndex < 1                   || 
      UIndex > Weights.ColLength() ||
      VIndex < 1                   || 
      VIndex > Weights.RowLength())    throw Standard_OutOfRange();

  if (Abs (Weight - Weights (UIndex, VIndex)) > gp::Resolution()) {
    Weights (UIndex, VIndex) = Weight;
    Rational(Weights, urational, vrational);
  }

 // is it turning into non rational
  if (wasrat && !(urational || vrational))
    weights.Nullify();
}

//=======================================================================
//function : SetWeightCol
//purpose  : 
//=======================================================================

void Geom_BezierSurface::SetWeightCol 
  (const Standard_Integer      VIndex,
   const TColStd_Array1OfReal& CPoleWeights)
{
  Standard_Integer I;
   // compute new rationality
  Standard_Boolean wasrat = (urational||vrational);
  if (!wasrat) {   
    // set weights of 1.
    weights = new TColStd_HArray2OfReal (1, poles->ColLength(),
					 1, poles->RowLength(), 1.);
  }

  TColStd_Array2OfReal & Weights = weights->ChangeArray2();
  if (VIndex < 1 || VIndex > Weights.RowLength()) throw Standard_OutOfRange();

  if (CPoleWeights.Length() !=  Weights.ColLength())  {
    throw Standard_ConstructionError("Geom_BezierSurface::SetWeightCol");
  }

  I = CPoleWeights.Lower();
  while (I <= CPoleWeights.Upper()) {
    if (CPoleWeights(I) <= gp::Resolution()) {
      throw Standard_ConstructionError();
    }
    Weights (I, VIndex) = CPoleWeights (I);
    I++;
  }

 Rational(Weights, urational, vrational);

 // is it turning into non rational
 if (wasrat && !(urational || vrational))
   weights.Nullify();
}

//=======================================================================
//function : SetWeightRow
//purpose  : 
//=======================================================================

void Geom_BezierSurface::SetWeightRow 
  (const Standard_Integer      UIndex,
   const TColStd_Array1OfReal& CPoleWeights)
{
  Standard_Integer I;
   // compute new rationality
  Standard_Boolean wasrat = (urational||vrational);
  if (!wasrat) {    
    // set weights of 1.
    weights = new TColStd_HArray2OfReal (1, poles->ColLength(),
					 1, poles->RowLength(), 1.);
  }

  TColStd_Array2OfReal & Weights = weights->ChangeArray2();
  if (UIndex < 1 || UIndex > Weights.ColLength()) 
    throw Standard_OutOfRange("Geom_BezierSurface::SetWeightRow");
  if (CPoleWeights.Lower() < 1 ||
      CPoleWeights.Lower() > Weights.RowLength() ||
      CPoleWeights.Upper() < 1 ||
      CPoleWeights.Upper() > Weights.RowLength()  ) {
    throw Standard_ConstructionError("Geom_BezierSurface::SetWeightRow");
  }

  I = CPoleWeights.Lower();
  while (I <= CPoleWeights.Upper()) {
    if (CPoleWeights(I) <= gp::Resolution())  {
      throw Standard_ConstructionError();
    }
    Weights (UIndex, I) = CPoleWeights (I);
    I++;
  }

  Rational(Weights, urational, vrational);

 // is it turning into non rational
  if (wasrat && !(urational || vrational))
    weights.Nullify();
}

//=======================================================================
//function : UReverse
//purpose  : 
//=======================================================================

void Geom_BezierSurface::UReverse ()
{
  gp_Pnt Pol;
  Standard_Integer Row,Col;
  TColgp_Array2OfPnt & Poles = poles->ChangeArray2();
  if (urational || vrational) {
    TColStd_Array2OfReal & Weights = weights->ChangeArray2();
    Standard_Real W;
    for (Col = 1; Col <= Poles.RowLength(); Col++) {
      for (Row = 1; Row <= IntegerPart (Poles.ColLength() / 2); Row++) {
        W = Weights (Row, Col);
        Weights (Row, Col) = Weights (Poles.ColLength()- Row + 1, Col);
        Weights (Poles.ColLength() - Row + 1, Col) = W;
        Pol = Poles (Row, Col);
        Poles (Row, Col) = Poles (Poles.ColLength() - Row + 1, Col);
        Poles (Poles.ColLength() - Row + 1, Col) = Pol;
      }
    }
  }
  else {
    for (Col = 1; Col <= Poles.RowLength(); Col++) {
      for (Row = 1; Row <= IntegerPart (Poles.ColLength() / 2); Row++) {
        Pol = Poles (Row, Col);
        Poles (Row, Col) = Poles (Poles.ColLength() - Row + 1, Col);
        Poles (Poles.ColLength() - Row + 1, Col) = Pol;
      }
    }
  }
}

//=======================================================================
//function : UReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_BezierSurface::UReversedParameter
  ( const Standard_Real U) const 
{
  return ( 1. - U);
}

//=======================================================================
//function : VReverse
//purpose  : 
//=======================================================================

void Geom_BezierSurface::VReverse ()
{
  gp_Pnt Pol;
  Standard_Integer Row,Col;
  TColgp_Array2OfPnt & Poles = poles->ChangeArray2();
  if (urational || vrational) {
    TColStd_Array2OfReal & Weights = weights->ChangeArray2();
    Standard_Real W;
    for (Row = 1; Row <= Poles.ColLength(); Row++) {
      for (Col = 1; Col <= IntegerPart (Poles.RowLength()/2); Col++) {
        W = Weights (Row, Col);
        Weights (Row, Col) = Weights (Row, Poles.RowLength() - Col + 1);
        Weights (Row, Poles.RowLength() - Col + 1) = W;
        Pol = Poles (Row, Col);
        Poles (Row, Col) = Poles (Row, Poles.RowLength() - Col + 1);
        Poles (Row, Poles.RowLength() - Col + 1) = Pol;
      }
    }
  }
  else {
    for (Row = 1; Row <= Poles.ColLength(); Row++) {
      for (Col = 1; Col <= IntegerPart(Poles.RowLength()/2); Col++) {
        Pol = Poles (Row, Col);
        Poles (Row, Col)= Poles (Row, Poles.RowLength() - Col + 1);
        Poles (Row, Poles.RowLength() - Col + 1) = Pol;
      }
    }
  }
}

//=======================================================================
//function : VReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_BezierSurface::VReversedParameter
  ( const Standard_Real V) const 
{
  return ( 1. - V);
}

//=======================================================================
//function : Bounds
//purpose  : 
//=======================================================================

void Geom_BezierSurface::Bounds (Standard_Real& U1,
				 Standard_Real& U2,
				 Standard_Real& V1,
				 Standard_Real& V2) const
{
  U1 = 0.0;  
  U2 = 1.0; 
  V1 = 0.0;
  V2 = 1.0;
}

//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape Geom_BezierSurface::Continuity () const
{
  return GeomAbs_CN; 
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void Geom_BezierSurface::D0 (const Standard_Real U,
			     const Standard_Real V,
			           gp_Pnt&       P ) const
{
  Standard_Real array_u[2];
  Standard_Real array_v[2];
  Standard_Integer mult_u[2];
  Standard_Integer mult_v[2];
  TColStd_Array1OfReal biduknots(array_u[0], 1, 2); biduknots(1) = 0.; biduknots(2) = 1.;
  TColStd_Array1OfInteger bidumults(mult_u[0], 1, 2); bidumults.Init(UDegree() + 1);
  TColStd_Array1OfReal bidvknots(array_v[0], 1, 2); bidvknots(1) = 0.; bidvknots(2) = 1.;
  TColStd_Array1OfInteger bidvmults(mult_v[0], 1, 2); bidvmults.Init(VDegree() + 1);
  if (urational || vrational) {
    BSplSLib::D0(U, V, 1, 1, poles->Array2(),
      &weights->Array2(),
      biduknots, bidvknots, &bidumults, &bidvmults,
      UDegree(), VDegree(),
      urational, vrational, Standard_False, Standard_False,
      P);
  }
  else {

    BSplSLib::D0(U, V, 1, 1, poles->Array2(),
      BSplSLib::NoWeights(),
      biduknots, bidvknots, &bidumults, &bidvmults,
      UDegree(), VDegree(),
      urational, vrational, Standard_False, Standard_False,
      P);
  }
}

//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Geom_BezierSurface::D1
  (const Standard_Real U,
   const Standard_Real V,
         gp_Pnt&       P,
         gp_Vec&       D1U,
         gp_Vec&       D1V ) const
{
  Standard_Real array_u[2];
  Standard_Real array_v[2];
  Standard_Integer mult_u[2];
  Standard_Integer mult_v[2];
  TColStd_Array1OfReal biduknots(array_u[0], 1, 2); biduknots(1) = 0.; biduknots(2) = 1.;
  TColStd_Array1OfInteger bidumults(mult_u[0], 1, 2); bidumults.Init(UDegree() + 1);
  TColStd_Array1OfReal bidvknots(array_v[0], 1, 2); bidvknots(1) = 0.; bidvknots(2) = 1.;
  TColStd_Array1OfInteger bidvmults(mult_v[0], 1, 2); bidvmults.Init(VDegree() + 1);
  if (urational || vrational) {
    BSplSLib::D1(U, V, 1, 1, poles->Array2(),
      &weights->Array2(),
      biduknots, bidvknots, &bidumults, &bidvmults,
      UDegree(), VDegree(),
      urational, vrational, Standard_False, Standard_False,
      P, D1U, D1V);
  }
  else {
    BSplSLib::D1(U, V, 1, 1, poles->Array2(),
      BSplSLib::NoWeights(),
      biduknots, bidvknots, &bidumults, &bidvmults,
      UDegree(), VDegree(),
      urational, vrational, Standard_False, Standard_False,
      P, D1U, D1V);
  }
}

//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Geom_BezierSurface::D2
  (const Standard_Real U,
   const Standard_Real V,
         gp_Pnt&       P,
         gp_Vec&       D1U, gp_Vec& D1V, 
         gp_Vec&       D2U, gp_Vec& D2V, gp_Vec& D2UV ) const
{
  Standard_Real array_u[2];
  Standard_Real array_v[2];
  Standard_Integer mult_u[2];
  Standard_Integer mult_v[2];
  TColStd_Array1OfReal biduknots(array_u[0], 1, 2); biduknots(1) = 0.; biduknots(2) = 1.;
  TColStd_Array1OfInteger bidumults(mult_u[0], 1, 2); bidumults.Init(UDegree() + 1);
  TColStd_Array1OfReal bidvknots(array_v[0], 1, 2); bidvknots(1) = 0.; bidvknots(2) = 1.;
  TColStd_Array1OfInteger bidvmults(mult_v[0], 1, 2); bidvmults.Init(VDegree() + 1);
  if (urational || vrational) {
    //-- ATTENTION a l'ORDRE d'appel ds BSPLSLIB 
    BSplSLib::D2(U, V, 1, 1, poles->Array2(),
      &weights->Array2(),
      biduknots, bidvknots, &bidumults, &bidvmults,
      UDegree(), VDegree(),
      urational, vrational, Standard_False, Standard_False,
      P, D1U, D1V, D2U, D2V, D2UV);
  }
  else {
    //-- ATTENTION a l'ORDRE d'appel ds BSPLSLIB 
    BSplSLib::D2(U, V, 1, 1, poles->Array2(),
      BSplSLib::NoWeights(),
      biduknots, bidvknots, &bidumults, &bidvmults,
      UDegree(), VDegree(),
      urational, vrational, Standard_False, Standard_False,
      P, D1U, D1V, D2U, D2V, D2UV);
  }
}

//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void Geom_BezierSurface::D3
  (const Standard_Real U, const Standard_Real V,
   gp_Pnt& P,
   gp_Vec& D1U, gp_Vec& D1V, 
   gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV, 
   gp_Vec& D3U, gp_Vec& D3V, gp_Vec& D3UUV, gp_Vec& D3UVV) const
{
  TColStd_Array1OfReal biduknots(1,2); biduknots(1) = 0.; biduknots(2) = 1.;
  TColStd_Array1OfInteger bidumults(1,2); bidumults.Init(UDegree() + 1);
  TColStd_Array1OfReal bidvknots(1,2); bidvknots(1) = 0.; bidvknots(2) = 1.;
  TColStd_Array1OfInteger bidvmults(1,2); bidvmults.Init(VDegree() + 1);
  if (urational || vrational) { 
    BSplSLib::D3 (U, V, 0, 0, poles->Array2(),
		  &weights->Array2(),
		  biduknots, bidvknots, &bidumults, &bidvmults,
		  UDegree(), VDegree(), urational, vrational, 0, 0,
		  P,
		  D1U, D1V,
		  D2U, D2V, D2UV,
		  D3U, D3V, D3UUV, D3UVV);
  }
  else { 
    BSplSLib::D3 (U, V, 0, 0, poles->Array2(),
		  BSplSLib::NoWeights(),
		  biduknots, bidvknots, &bidumults, &bidvmults,
		  UDegree(), VDegree(), urational, vrational, 0, 0,
		  P,
		  D1U, D1V,
		  D2U, D2V, D2UV,
		  D3U, D3V, D3UUV, D3UVV);
  }
}

//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

gp_Vec Geom_BezierSurface::DN
  (const Standard_Real    U,
   const Standard_Real    V,
   const Standard_Integer Nu,
   const Standard_Integer Nv) const
{
  Standard_RangeError_Raise_if (Nu + Nv < 1 || Nv < 0 || Nu <0, " ");
  gp_Vec Derivative;
  TColStd_Array1OfReal biduknots(1,2); biduknots(1) = 0.; biduknots(2) = 1.;
  TColStd_Array1OfInteger bidumults(1,2); bidumults.Init(UDegree() + 1);
  TColStd_Array1OfReal bidvknots(1,2); bidvknots(1) = 0.; bidvknots(2) = 1.;
  TColStd_Array1OfInteger bidvmults(1,2); bidvmults.Init(VDegree() + 1);
  if (urational || vrational) { 
    BSplSLib::DN (U, V, Nu, Nv, 0, 0, poles->Array2(),
		  &weights->Array2(),
		  biduknots, bidvknots, &bidumults, &bidvmults,
		  UDegree(), VDegree(), urational, vrational, 0, 0,
		  Derivative);
  }
  else { 
    BSplSLib::DN (U, V, Nu, Nv, 0, 0, poles->Array2(),
		  BSplSLib::NoWeights(),
		  biduknots, bidvknots, &bidumults, &bidvmults,
		  UDegree(), VDegree(), urational, vrational, 0, 0,
		  Derivative);
  }
  return Derivative;
}

//=======================================================================
//function : NbUPoles
//purpose  : 
//=======================================================================

Standard_Integer Geom_BezierSurface::NbUPoles () const
{
  return poles->ColLength(); 
}

//=======================================================================
//function : NbVPoles
//purpose  : 
//=======================================================================

Standard_Integer Geom_BezierSurface::NbVPoles () const
{
  return poles->RowLength(); 
}

//=======================================================================
//function : Pole
//purpose  : 
//=======================================================================

const gp_Pnt& Geom_BezierSurface::Pole (const Standard_Integer UIndex,
				 const Standard_Integer VIndex) const
{
  Standard_OutOfRange_Raise_if
    (UIndex < 1 || UIndex > poles->ColLength() ||
     VIndex < 1 || VIndex > poles->RowLength(), " ");
  return poles->Value (UIndex + poles->LowerRow() - 1,
		       VIndex + poles->LowerCol() - 1);
}

//=======================================================================
//function : Poles
//purpose  : 
//=======================================================================

void Geom_BezierSurface::Poles (TColgp_Array2OfPnt& P) const
{
  Standard_DimensionError_Raise_if
    (P.RowLength() != poles->RowLength() ||
     P.ColLength() != poles->ColLength(), " ");
  P = poles->Array2();
}

//=======================================================================
//function : UDegree
//purpose  : 
//=======================================================================

Standard_Integer Geom_BezierSurface::UDegree () const
{
  return poles->ColLength() - 1; 
}

//=======================================================================
//function : UIso
//purpose  : 
//=======================================================================

Handle(Geom_Curve) Geom_BezierSurface::UIso (const Standard_Real U) const
{
  TColStd_Array1OfReal biduknots(1,2); biduknots(1) = 0.; biduknots(2) = 1.;
  TColStd_Array1OfInteger bidumults(1,2); bidumults.Init(UDegree() + 1);

  Handle(Geom_BezierCurve) UIsoCurve;
  const TColgp_Array2OfPnt & Poles = poles->Array2();
  TColgp_Array1OfPnt VCurvePoles (Poles.LowerCol() , Poles.UpperCol());
  if (urational || vrational) {
    const TColStd_Array2OfReal & Weights = weights->Array2();
    TColStd_Array1OfReal VCurveWeights 
      (Weights.LowerCol() , Weights.UpperCol());
    BSplSLib::Iso (U, 1, Poles,
		   &Weights,
		   biduknots, &bidumults,
		   UDegree(), 0, VCurvePoles, &VCurveWeights);
    if (urational)
      UIsoCurve = new Geom_BezierCurve (VCurvePoles, VCurveWeights);
    else
      UIsoCurve = new Geom_BezierCurve (VCurvePoles);
  }
  else {
    BSplSLib::Iso (U, 1, Poles,
		   BSplSLib::NoWeights(),
		   biduknots, &bidumults,
		   UDegree(), 0, VCurvePoles, PLib::NoWeights());
    UIsoCurve = new Geom_BezierCurve (VCurvePoles);
  }
  return UIsoCurve;
}

//=======================================================================
//function : VDegree
//purpose  : 
//=======================================================================

Standard_Integer Geom_BezierSurface::VDegree () const
{
  return poles->RowLength() - 1; 
}

//=======================================================================
//function : VIso
//purpose  : 
//=======================================================================

Handle(Geom_Curve) Geom_BezierSurface::VIso (const Standard_Real V) const
{
  TColStd_Array1OfReal bidvknots(1,2); bidvknots(1) = 0.; bidvknots(2) = 1.;
  TColStd_Array1OfInteger bidvmults(1,2); bidvmults.Init(VDegree() + 1);

  Handle(Geom_BezierCurve) VIsoCurve;
  const TColgp_Array2OfPnt & Poles = poles->Array2();
  TColgp_Array1OfPnt VCurvePoles (Poles.LowerRow() , Poles.UpperRow());
  if (vrational || urational) {
    const TColStd_Array2OfReal & Weights = weights->Array2();
    TColStd_Array1OfReal VCurveWeights 
      (Weights.LowerRow() , Weights.UpperRow());
    BSplSLib::Iso (V, 0, Poles,
		   &Weights,
		   bidvknots, &bidvmults,
		   VDegree(), 0, VCurvePoles, &VCurveWeights);
    if (vrational)
      VIsoCurve = new Geom_BezierCurve (VCurvePoles, VCurveWeights);
    else
      VIsoCurve = new Geom_BezierCurve (VCurvePoles);
  }
  else {
    BSplSLib::Iso (V, 0, Poles,
		   BSplSLib::NoWeights(),
		   bidvknots, &bidvmults,
		   VDegree(), 0, VCurvePoles, PLib::NoWeights());
    VIsoCurve = new Geom_BezierCurve (VCurvePoles);
  }
  return VIsoCurve;
}

//=======================================================================
//function : Weight
//purpose  : 
//=======================================================================

Standard_Real Geom_BezierSurface::Weight (const Standard_Integer UIndex,
					  const Standard_Integer VIndex) const
{
  Standard_OutOfRange_Raise_if (
           UIndex < 1 || UIndex > weights->ColLength() ||
           VIndex < 1 || VIndex > weights->RowLength(), " ");

  if (urational || vrational) 
    return weights->Value (UIndex, VIndex);
  else 
    return 1;
}

//=======================================================================
//function : Weights
//purpose  : 
//=======================================================================

void Geom_BezierSurface::Weights (TColStd_Array2OfReal& W ) const
{
  Standard_DimensionError_Raise_if (
           W.RowLength() != weights->RowLength() ||
           W.ColLength() != weights->ColLength(), " " );
  if (urational || vrational) 
    W = weights->Array2();
  else 
    W.Init(1.);
}

//=======================================================================
//function : IsCNu
//purpose  : 
//=======================================================================

Standard_Boolean Geom_BezierSurface::IsCNu (const Standard_Integer ) const
{
  return Standard_True; 
}

//=======================================================================
//function : IsCNv
//purpose  : 
//=======================================================================

Standard_Boolean Geom_BezierSurface::IsCNv (const Standard_Integer ) const
{
  return Standard_True; 
}

//=======================================================================
//function : IsURational
//purpose  : 
//=======================================================================

Standard_Boolean Geom_BezierSurface::IsURational () const
{
  return urational; 
}

//=======================================================================
//function : IsVRational
//purpose  : 
//=======================================================================

Standard_Boolean Geom_BezierSurface::IsVRational () const
{
  return vrational; 
}

//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================

void Geom_BezierSurface::Transform (const gp_Trsf& T)
{
  TColgp_Array2OfPnt & Poles = poles->ChangeArray2();

  for (Standard_Integer I = 1; I <= Poles.ColLength(); I++) {

    for (Standard_Integer J = 1; J <= Poles.RowLength(); J++) {
      Poles (I, J).Transform (T);
    }
  }
}

//=======================================================================
//function : IsUClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom_BezierSurface::IsUClosed () const
{ 
  const TColgp_Array2OfPnt & Poles = poles->Array2();
  Standard_Boolean Closed = Standard_True;
  Standard_Integer Lower  = Poles.LowerRow();
  Standard_Integer Upper  = Poles.UpperRow();
  Standard_Integer Length = Poles.RowLength();
  Standard_Integer j      = Poles.LowerCol();

  while (Closed && j <= Length) {
    Closed = (Poles (Lower,j).Distance (Poles (Upper,j)) <= Precision::Confusion());
    j++;
  }
  return Closed; 
}

//=======================================================================
//function : IsVClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom_BezierSurface::IsVClosed () const
{ 
  const TColgp_Array2OfPnt & Poles = poles->Array2();
  Standard_Boolean Closed = Standard_True;
  Standard_Integer Lower  = Poles.LowerCol();
  Standard_Integer Upper  = Poles.UpperCol();
  Standard_Integer Length = Poles.ColLength();
  Standard_Integer i      = Poles.LowerRow();
  while (Closed && i <= Length) {
    Closed = (Poles (i,Lower).Distance (Poles (i,Upper)) <= Precision::Confusion());
    i++;
  }
  return Closed; 
}

//=======================================================================
//function : IsUPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom_BezierSurface::IsUPeriodic () const
{
  return Standard_False; 
}

//=======================================================================
//function : IsVPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom_BezierSurface::IsVPeriodic () const
{
  return Standard_False; 
}

//=======================================================================
//function : Resolution
//purpose  : 
//=======================================================================

void Geom_BezierSurface::Resolution(const Standard_Real  Tolerance3D,
				    Standard_Real&       UTolerance,
				    Standard_Real&       VTolerance) 
{
  if(!maxderivinvok){
    TColStd_Array1OfReal biduknots(1,2); biduknots(1) = 0.; biduknots(2) = 1.;
    TColStd_Array1OfInteger bidumults(1,2); bidumults.Init(UDegree() + 1);
    TColStd_Array1OfReal bidvknots(1,2); bidvknots(1) = 0.; bidvknots(2) = 1.;
    TColStd_Array1OfInteger bidvmults(1,2); bidvmults.Init(VDegree() + 1);
    if(urational || vrational){
      BSplSLib::Resolution(poles->Array2(),
			   &weights->Array2(),
			   biduknots,
			   bidvknots,
			   bidumults,
			   bidvmults,
			   UDegree(),
			   VDegree(),
			   urational,
			   vrational,
			   0,
			   0,
			   1.,
			   umaxderivinv,
			   vmaxderivinv) ;
    }
    else{
      BSplSLib::Resolution(poles->Array2(),
			   BSplSLib::NoWeights(),
			   biduknots,
			   bidvknots,
			   bidumults,
			   bidvmults,
			   UDegree(),
			   VDegree(),
			   urational,
			   vrational,
			   0,
			   0,
			   1.,
			   umaxderivinv,
			   vmaxderivinv) ;
    }
    maxderivinvok = 1;
  }
  UTolerance = Tolerance3D * umaxderivinv;
  VTolerance = Tolerance3D * vmaxderivinv;
}

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Geom_Geometry) Geom_BezierSurface::Copy() const
{
  Handle(Geom_BezierSurface) S = new Geom_BezierSurface
    (poles, weights, urational, vrational);
  return S;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void Geom_BezierSurface::Init
  (const Handle(TColgp_HArray2OfPnt)&   Poles, 
   const Handle(TColStd_HArray2OfReal)& Weights)
{
  // set fields
  poles = Poles;
  if (urational || vrational)
    weights = Weights;
  else
    weights.Nullify();
}

