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

// 14-Mar-96 : xab  portage hp
// pmn : 28-Jun-96 Distinction entre la continuite en U et V (bug PRO4625)
// pmn : 07-Jan-97 Centralisation des verif rational (PRO6834)
//       et ajout des InvalideCache() dans les SetWeight*(PRO6833)
// RBD : 15-10-98 ; Le cache est maintenant calcule sur [-1,1] (pro15537).
// jct : 19-01-99 ; permutation de urational et vrational dans Rational.
#define No_Standard_OutOfRange


#include <BSplCLib.hxx>
#include <BSplSLib.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_UndefinedDerivative.hxx>
#include <gp.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_BSplineSurface,Geom_BoundedSurface)

//=======================================================================
//function : CheckSurfaceData
//purpose  : Internal use only.
//=======================================================================
static void CheckSurfaceData
(const TColgp_Array2OfPnt&      SPoles,
 const TColStd_Array1OfReal&    SUKnots,
 const TColStd_Array1OfReal&    SVKnots,
 const TColStd_Array1OfInteger& SUMults,
 const TColStd_Array1OfInteger& SVMults,
 const Standard_Integer         UDegree,
 const Standard_Integer         VDegree,
 const Standard_Boolean         UPeriodic,
 const Standard_Boolean         VPeriodic)
{
  if (UDegree < 1 || UDegree > Geom_BSplineSurface::MaxDegree () || 
      VDegree < 1 || VDegree > Geom_BSplineSurface::MaxDegree ()) {
    throw Standard_ConstructionError("Geom_BSplineSurface: invalid degree");
  }
  if (SPoles.ColLength () < 2 || SPoles.RowLength () < 2) {
    throw Standard_ConstructionError("Geom_BSplineSurface: at least 2 poles required");
  }

  if (SUKnots.Length() != SUMults.Length() ||
      SVKnots.Length() != SVMults.Length()) {
    throw Standard_ConstructionError("Geom_BSplineSurface: Knot and Mult array size mismatch");
  }

  Standard_Integer i;
  for (i = SUKnots.Lower(); i < SUKnots.Upper(); i++) {
    if (SUKnots(i+1) - SUKnots(i) <= Epsilon(Abs(SUKnots(i)))) {
      throw Standard_ConstructionError("Geom_BSplineSurface: UKnots interval values too close");
    }
  }

  for (i = SVKnots.Lower(); i < SVKnots.Upper(); i++) {
    if (SVKnots(i+1) - SVKnots(i) <= Epsilon(Abs(SVKnots(i)))) {
      throw Standard_ConstructionError("Geom_BSplineSurface: VKnots interval values too close");
    }
  }
  
  if (SPoles.ColLength() != BSplCLib::NbPoles(UDegree,UPeriodic,SUMults))
    throw Standard_ConstructionError("Geom_BSplineSurface: # U Poles and degree mismatch");

  if (SPoles.RowLength() != BSplCLib::NbPoles(VDegree,VPeriodic,SVMults))
    throw Standard_ConstructionError("Geom_BSplineSurface: # V Poles and degree mismatch");
}

//=======================================================================
//function : Rational
//purpose  : Internal use only.
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
//function : Copy
//purpose  : 
//=======================================================================

Handle(Geom_Geometry) Geom_BSplineSurface::Copy () const
{
  Handle(Geom_BSplineSurface) S;
  if (urational || vrational) 
    S = new Geom_BSplineSurface (poles->Array2() , weights->Array2(), 
				 uknots->Array1(), vknots->Array1(), 
				 umults->Array1(), vmults->Array1(), 
				 udeg     , vdeg, 
				 uperiodic, vperiodic);
  else
    S = new Geom_BSplineSurface (poles->Array2(),
				 uknots->Array1(), vknots->Array1(), 
				 umults->Array1(), vmults->Array1(), 
				 udeg     , vdeg, 
				 uperiodic, vperiodic);
  return S;
}

//=======================================================================
//function : Geom_BSplineSurface
//purpose  : 
//=======================================================================

Geom_BSplineSurface::Geom_BSplineSurface
(const TColgp_Array2OfPnt&      Poles, 
 const TColStd_Array1OfReal&    UKnots, 
 const TColStd_Array1OfReal&    VKnots,
 const TColStd_Array1OfInteger& UMults, 
 const TColStd_Array1OfInteger& VMults,
 const Standard_Integer         UDegree, 
 const Standard_Integer         VDegree,
 const Standard_Boolean         UPeriodic,
 const Standard_Boolean         VPeriodic
 ) :
 urational(Standard_False),
 vrational(Standard_False),
 uperiodic(UPeriodic),
 vperiodic(VPeriodic),
 udeg(UDegree),
 vdeg(VDegree),
 maxderivinvok(0)

{

  // check
  
  CheckSurfaceData(Poles,
		   UKnots   , VKnots,
		   UMults   , VMults,
		   UDegree  , VDegree,
		   UPeriodic, VPeriodic);

  // copy arrays

  poles   = new TColgp_HArray2OfPnt(1,Poles.ColLength(),
				    1,Poles.RowLength());
  poles->ChangeArray2() = Poles;

  weights = new TColStd_HArray2OfReal (1,Poles.ColLength(),
				       1,Poles.RowLength(), 1.0);

  uknots  = new TColStd_HArray1OfReal    (1,UKnots.Length());
  uknots->ChangeArray1() = UKnots;

  umults  = new TColStd_HArray1OfInteger (1,UMults.Length());
  umults->ChangeArray1() = UMults;

  vknots  = new TColStd_HArray1OfReal    (1,VKnots.Length());
  vknots->ChangeArray1() = VKnots;

  vmults  = new TColStd_HArray1OfInteger (1,VMults.Length());
  vmults->ChangeArray1() = VMults;

  UpdateUKnots();
  UpdateVKnots();
}

//=======================================================================
//function : Geom_BSplineSurface
//purpose  : 
//=======================================================================

Geom_BSplineSurface::Geom_BSplineSurface
(const TColgp_Array2OfPnt&      Poles,
 const TColStd_Array2OfReal&    Weights,
 const TColStd_Array1OfReal&    UKnots,
 const TColStd_Array1OfReal&    VKnots,
 const TColStd_Array1OfInteger& UMults, 
 const TColStd_Array1OfInteger& VMults,
 const Standard_Integer         UDegree,
 const Standard_Integer         VDegree,
 const Standard_Boolean         UPeriodic,
 const Standard_Boolean         VPeriodic) :
 urational(Standard_False),
 vrational(Standard_False),
 uperiodic(UPeriodic),
 vperiodic(VPeriodic),
 udeg(UDegree),
 vdeg(VDegree),
 maxderivinvok(0)
{
  // check weights

  if (Weights.ColLength() != Poles.ColLength())
    throw Standard_ConstructionError("Geom_BSplineSurface: U Weights and Poles array size mismatch");

  if (Weights.RowLength() != Poles.RowLength())
    throw Standard_ConstructionError("Geom_BSplineSurface: V Weights and Poles array size mismatch");

  Standard_Integer i,j;
  for (i = Weights.LowerRow(); i <= Weights.UpperRow(); i++) {
    for (j = Weights.LowerCol(); j <= Weights.UpperCol(); j++) {
      if (Weights(i,j) <= gp::Resolution())  
        throw Standard_ConstructionError("Geom_BSplineSurface: Weights values too small");
    }
  }
  
  // check really rational
  
  Rational(Weights, urational, vrational);

  // check
  
  CheckSurfaceData(Poles,
		   UKnots   , VKnots,
		   UMults   , VMults,
		   UDegree  , VDegree,
		   UPeriodic, VPeriodic);

  // copy arrays

  poles   = new TColgp_HArray2OfPnt(1,Poles.ColLength(),
				    1,Poles.RowLength());
  poles->ChangeArray2() = Poles;

  weights = new TColStd_HArray2OfReal (1,Poles.ColLength(),
				       1,Poles.RowLength());
  weights->ChangeArray2() = Weights;

  uknots  = new TColStd_HArray1OfReal    (1,UKnots.Length());
  uknots->ChangeArray1() = UKnots;

  umults  = new TColStd_HArray1OfInteger (1,UMults.Length());
  umults->ChangeArray1() = UMults;

  vknots  = new TColStd_HArray1OfReal    (1,VKnots.Length());
  vknots->ChangeArray1() = VKnots;

  vmults  = new TColStd_HArray1OfInteger (1,VMults.Length());
  vmults->ChangeArray1() = VMults;

  UpdateUKnots();
  UpdateVKnots();
}

//=======================================================================
//function : ExchangeUV
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::ExchangeUV ()
{
  Standard_Integer LC = poles->LowerCol();
  Standard_Integer UC = poles->UpperCol();
  Standard_Integer LR = poles->LowerRow();
  Standard_Integer UR = poles->UpperRow();

  Handle(TColgp_HArray2OfPnt) npoles = new TColgp_HArray2OfPnt (LC, UC, LR, UR);
  Handle(TColStd_HArray2OfReal) nweights;
  if (!weights.IsNull())
  {
    nweights = new TColStd_HArray2OfReal (LC, UC, LR, UR);
  }

  const TColgp_Array2OfPnt& spoles = poles->Array2();
  const TColStd_Array2OfReal* sweights = !weights.IsNull() ? &weights->Array2() : NULL;
  
  TColgp_Array2OfPnt& snpoles = npoles->ChangeArray2();
  TColStd_Array2OfReal* snweights = !nweights.IsNull() ? &nweights->ChangeArray2() : NULL;
  for (Standard_Integer i = LC; i <= UC; i++)
  {
    for (Standard_Integer j = LR; j <= UR; j++)
    {
      snpoles (i, j) = spoles (j, i);
      if (snweights != NULL)
      {
        snweights->ChangeValue (i, j) = sweights->Value (j, i);
      }
    }
  }
  poles   = npoles;
  weights = nweights;

  std::swap (urational, vrational);
  std::swap (uperiodic, vperiodic);
  std::swap (udeg,   vdeg);
  std::swap (uknots, vknots);
  std::swap (umults, vmults);

  UpdateUKnots();
  UpdateVKnots();
}

//=======================================================================
//function : IncreaseDegree
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::IncreaseDegree (const Standard_Integer UDegree,
					  const Standard_Integer VDegree)
{ 
  if (UDegree != udeg) {
    if ( UDegree < udeg || UDegree > Geom_BSplineSurface::MaxDegree())
      throw Standard_ConstructionError("Geom_BSplineSurface::IncreaseDegree: bad U degree value");
    
    Standard_Integer FromK1 = FirstUKnotIndex();
    Standard_Integer ToK2   = LastUKnotIndex();

    Standard_Integer Step   = UDegree - udeg;

    Handle(TColgp_HArray2OfPnt) npoles = new
      TColgp_HArray2OfPnt( 1, poles->ColLength() + Step * (ToK2 - FromK1),
			  1, poles->RowLength());

    Standard_Integer nbknots = BSplCLib::IncreaseDegreeCountKnots
      (udeg,UDegree,uperiodic,umults->Array1());

    Handle(TColStd_HArray1OfReal) nknots = 
      new TColStd_HArray1OfReal(1,nbknots);
    
    Handle(TColStd_HArray1OfInteger) nmults = 
      new TColStd_HArray1OfInteger(1,nbknots);

    Handle(TColStd_HArray2OfReal) nweights 
      = new TColStd_HArray2OfReal(1,npoles->ColLength(),
				  1,npoles->RowLength(), 1.);

    if (urational || vrational) {
      
      BSplSLib::IncreaseDegree
	(Standard_True, udeg, UDegree, uperiodic,
	 poles->Array2(),&weights->Array2(),
	 uknots->Array1(),umults->Array1(),
	 npoles->ChangeArray2(),&nweights->ChangeArray2(),
	 nknots->ChangeArray1(),nmults->ChangeArray1());
    }
    else {

      BSplSLib::IncreaseDegree
	(Standard_True, udeg, UDegree, uperiodic,
	 poles->Array2(),BSplSLib::NoWeights(),
	 uknots->Array1(),umults->Array1(),
	 npoles->ChangeArray2(),BSplSLib::NoWeights(),
	 nknots->ChangeArray1(),nmults->ChangeArray1());
    }
    udeg    = UDegree;
    poles   = npoles;
    weights = nweights;
    uknots  = nknots;
    umults  = nmults;
    UpdateUKnots();
  }

  if (VDegree != vdeg) {
    if ( VDegree < vdeg || VDegree > Geom_BSplineSurface::MaxDegree())
      throw Standard_ConstructionError("Geom_BSplineSurface::IncreaseDegree: bad V degree value");
    
    Standard_Integer FromK1 = FirstVKnotIndex();
    Standard_Integer ToK2   = LastVKnotIndex();

    Standard_Integer Step   = VDegree - vdeg;

    Handle(TColgp_HArray2OfPnt) npoles = new
      TColgp_HArray2OfPnt( 1, poles->ColLength(),
			  1, poles->RowLength() + Step * (ToK2 - FromK1));

    Standard_Integer nbknots = BSplCLib::IncreaseDegreeCountKnots
      (vdeg,VDegree,vperiodic,vmults->Array1());

    Handle(TColStd_HArray1OfReal) nknots = 
      new TColStd_HArray1OfReal(1,nbknots);
    
    Handle(TColStd_HArray1OfInteger) nmults = 
      new TColStd_HArray1OfInteger(1,nbknots);

    Handle(TColStd_HArray2OfReal) nweights
      = new TColStd_HArray2OfReal(1,npoles->ColLength(),
				  1,npoles->RowLength(), 1.);

    if (urational || vrational) {
      
      BSplSLib::IncreaseDegree
	(Standard_False, vdeg, VDegree, vperiodic,
	 poles->Array2(),&weights->Array2(),
	 vknots->Array1(),vmults->Array1(),
	 npoles->ChangeArray2(),&nweights->ChangeArray2(),
	 nknots->ChangeArray1(),nmults->ChangeArray1());
    }
    else {

      BSplSLib::IncreaseDegree
	(Standard_False, vdeg, VDegree, vperiodic,
	 poles->Array2(),BSplSLib::NoWeights(),
	 vknots->Array1(),vmults->Array1(),
	 npoles->ChangeArray2(),BSplSLib::NoWeights(),
	 nknots->ChangeArray1(),nmults->ChangeArray1());
    }
    vdeg    = VDegree;
    poles   = npoles;
    weights = nweights;
    vknots  = nknots;
    vmults  = nmults;
    UpdateVKnots();
  }
}

//=======================================================================
//function : IncreaseUMultiplicity
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::IncreaseUMultiplicity
(const Standard_Integer UIndex, 
 const Standard_Integer M)
{
  TColStd_Array1OfReal k(1,1);
  k(1) = uknots->Value(UIndex);
  TColStd_Array1OfInteger m(1,1);
  m(1) = M - umults->Value(UIndex);
  InsertUKnots(k,m,Epsilon(1.),Standard_True);
}

//=======================================================================
//function : IncreaseUMultiplicity
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::IncreaseUMultiplicity
(const Standard_Integer FromI1, 
 const Standard_Integer ToI2,
 const Standard_Integer M)
{
  Handle(TColStd_HArray1OfReal) tk = uknots;
  TColStd_Array1OfReal k((uknots->Array1())(FromI1),FromI1,ToI2);
  TColStd_Array1OfInteger m(FromI1, ToI2);
  for (Standard_Integer i = FromI1; i <= ToI2; i++) 
    m(i) = M - umults->Value(i);
  InsertUKnots(k,m,Epsilon(1.),Standard_True);
}

//=======================================================================
//function : IncreaseVMultiplicity
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::IncreaseVMultiplicity
(const Standard_Integer VIndex, 
 const Standard_Integer M)
{
  TColStd_Array1OfReal k(1,1);
  k(1) = vknots->Value(VIndex);
  TColStd_Array1OfInteger m(1,1);
  m(1) = M - vmults->Value(VIndex);
  InsertVKnots(k,m,Epsilon(1.),Standard_True);
}

//=======================================================================
//function : IncreaseVMultiplicity
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::IncreaseVMultiplicity
(const Standard_Integer FromI1,
 const Standard_Integer ToI2,
 const Standard_Integer M)
{
  Handle(TColStd_HArray1OfReal) tk = vknots;
  TColStd_Array1OfReal k((vknots->Array1())(FromI1),FromI1,ToI2);
  TColStd_Array1OfInteger m(FromI1,ToI2);
  for (Standard_Integer i = FromI1; i <= ToI2; i++)
    m(i) = M - vmults->Value(i);
  InsertVKnots(k,m,Epsilon(1.),Standard_True);
}

//=======================================================================
//function : segment
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::segment(const Standard_Real U1,
                                  const Standard_Real U2,
                                  const Standard_Real V1,
                                  const Standard_Real V2,
                                  const Standard_Real EpsU,
                                  const Standard_Real EpsV,
                                  const Standard_Boolean SegmentInU,
                                  const Standard_Boolean SegmentInV)
{
  Standard_Real deltaU = U2 - U1;
  if (uperiodic) {
    Standard_Real aUPeriod = uknots->Last() - uknots->First();
    if (deltaU - aUPeriod > Precision::PConfusion())
      throw Standard_DomainError("Geom_BSplineSurface::Segment");
    if (deltaU > aUPeriod)
      deltaU = aUPeriod;
  }

  Standard_Real deltaV = V2 - V1;
  if (vperiodic) {
    Standard_Real aVPeriod = vknots->Last() - vknots->First();
    if (deltaV - aVPeriod > Precision::PConfusion())
      throw Standard_DomainError("Geom_BSplineSurface::Segment");
    if (deltaV > aVPeriod)
      deltaV = aVPeriod;
  }

  Standard_Real NewU1, NewU2, NewV1, NewV2;
  Standard_Real U, V;
  Standard_Integer indexU, indexV;

  indexU = 0;
  BSplCLib::LocateParameter(udeg, uknots->Array1(), umults->Array1(),
                            U1, uperiodic, uknots->Lower(), uknots->Upper(),
                            indexU, NewU1);
  indexU = 0;
  BSplCLib::LocateParameter(udeg, uknots->Array1(), umults->Array1(),
                            U2, uperiodic, uknots->Lower(), uknots->Upper(),
                            indexU, NewU2);
  if (SegmentInU) {
    // inserting the UKnots
    TColStd_Array1OfReal    UKnots(1, 2);
    TColStd_Array1OfInteger UMults(1, 2);
    UKnots(1) = Min(NewU1, NewU2);
    UKnots(2) = Max(NewU1, NewU2);
    UMults(1) = UMults(2) = udeg;

    InsertUKnots(UKnots, UMults, EpsU);
  }

  indexV = 0;
  BSplCLib::LocateParameter(vdeg, vknots->Array1(), vmults->Array1(),
                            V1, vperiodic, vknots->Lower(), vknots->Upper(),
                            indexV, NewV1);
  indexV = 0;
  BSplCLib::LocateParameter(vdeg, vknots->Array1(), vmults->Array1(),
                            V2, vperiodic, vknots->Lower(), vknots->Upper(),
                            indexV, NewV2);
  if (SegmentInV) {
    // Inserting the VKnots
    TColStd_Array1OfReal    VKnots(1, 2);
    TColStd_Array1OfInteger VMults(1, 2);

    VKnots(1) = Min(NewV1, NewV2);
    VKnots(2) = Max(NewV1, NewV2);
    VMults(1) = VMults(2) = vdeg;
    InsertVKnots(VKnots, VMults, EpsV);
  }

  if (uperiodic && SegmentInU) { // set the origine at NewU1
    Standard_Integer index = 0;
    BSplCLib::LocateParameter(udeg, uknots->Array1(), umults->Array1(),
      U1, uperiodic, uknots->Lower(), uknots->Upper(),
      index, U);
    if (Abs(uknots->Value(index + 1) - U) <= EpsU)
      index++;
    SetUOrigin(index);
    SetUNotPeriodic();
  }

  // compute index1 and index2 to set the new knots and mults 
  Standard_Integer index1U = 0, index2U = 0;
  Standard_Integer FromU1 = uknots->Lower();
  Standard_Integer ToU2 = uknots->Upper();
  BSplCLib::LocateParameter(udeg, uknots->Array1(), umults->Array1(),
                            NewU1, uperiodic, FromU1, ToU2, index1U, U);
  if (Abs(uknots->Value(index1U + 1) - U) <= EpsU)
    index1U++;
  BSplCLib::LocateParameter(udeg, uknots->Array1(), umults->Array1(),
                            NewU1 + deltaU, uperiodic, FromU1, ToU2, index2U, U);
  if (Abs(uknots->Value(index2U + 1) - U) <= EpsU || index2U == index1U)
    index2U++;

  Standard_Integer nbuknots = index2U - index1U + 1;

  Handle(TColStd_HArray1OfReal)
    nuknots = new TColStd_HArray1OfReal(1, nbuknots);
  Handle(TColStd_HArray1OfInteger)
    numults = new TColStd_HArray1OfInteger(1, nbuknots);

  Standard_Integer i, k = 1;
  for (i = index1U; i <= index2U; i++) {
    nuknots->SetValue(k, uknots->Value(i));
    numults->SetValue(k, umults->Value(i));
    k++;
  }
  if (SegmentInU) {
    numults->SetValue(1, udeg + 1);
    numults->SetValue(nbuknots, udeg + 1);
  }

  if (vperiodic&& SegmentInV) { // set the origine at NewV1
    Standard_Integer index = 0;
    BSplCLib::LocateParameter(vdeg, vknots->Array1(), vmults->Array1(),
      V1, vperiodic, vknots->Lower(), vknots->Upper(),
      index, V);
    if (Abs(vknots->Value(index + 1) - V) <= EpsV)
      index++;
    SetVOrigin(index);
    SetVNotPeriodic();
  }

  // compute index1 and index2 to set the new knots and mults 
  Standard_Integer index1V = 0, index2V = 0;
  Standard_Integer FromV1 = vknots->Lower();
  Standard_Integer ToV2 = vknots->Upper();
  BSplCLib::LocateParameter(vdeg, vknots->Array1(), vmults->Array1(),
                            NewV1, vperiodic, FromV1, ToV2, index1V, V);
  if (Abs(vknots->Value(index1V + 1) - V) <= EpsV)
    index1V++;
  BSplCLib::LocateParameter(vdeg, vknots->Array1(), vmults->Array1(),
                            NewV1 + deltaV, vperiodic, FromV1, ToV2, index2V, V);
  if (Abs(vknots->Value(index2V + 1) - V) <= EpsV || index2V == index1V)
    index2V++;

  Standard_Integer nbvknots = index2V - index1V + 1;

  Handle(TColStd_HArray1OfReal)
    nvknots = new TColStd_HArray1OfReal(1, nbvknots);
  Handle(TColStd_HArray1OfInteger)
    nvmults = new TColStd_HArray1OfInteger(1, nbvknots);

  k = 1;
  for (i = index1V; i <= index2V; i++) {
    nvknots->SetValue(k, vknots->Value(i));
    nvmults->SetValue(k, vmults->Value(i));
    k++;
  }
  if (SegmentInV) {
    nvmults->SetValue(1, vdeg + 1);
    nvmults->SetValue(nbvknots, vdeg + 1);
  }


  // compute index1 and index2 to set the new poles and weights
  Standard_Integer pindex1U
    = BSplCLib::PoleIndex(udeg, index1U, uperiodic, umults->Array1());
  Standard_Integer pindex2U
    = BSplCLib::PoleIndex(udeg, index2U, uperiodic, umults->Array1());

  pindex1U++;
  pindex2U = Min(pindex2U + 1, poles->ColLength());

  Standard_Integer nbupoles = pindex2U - pindex1U + 1;

  // compute index1 and index2 to set the new poles and weights
  Standard_Integer pindex1V
    = BSplCLib::PoleIndex(vdeg, index1V, vperiodic, vmults->Array1());
  Standard_Integer pindex2V
    = BSplCLib::PoleIndex(vdeg, index2V, vperiodic, vmults->Array1());

  pindex1V++;
  pindex2V = Min(pindex2V + 1, poles->RowLength());

  Standard_Integer nbvpoles = pindex2V - pindex1V + 1;


  Handle(TColStd_HArray2OfReal) nweights;

  Handle(TColgp_HArray2OfPnt)
    npoles = new TColgp_HArray2OfPnt(1, nbupoles, 1, nbvpoles);
  k = 1;
  Standard_Integer j, l;
  if (urational || vrational) {
    nweights = new TColStd_HArray2OfReal(1, nbupoles, 1, nbvpoles);
    for (i = pindex1U; i <= pindex2U; i++) {
      l = 1;
      for (j = pindex1V; j <= pindex2V; j++) {
        npoles->SetValue(k, l, poles->Value(i, j));
        nweights->SetValue(k, l, weights->Value(i, j));
        l++;
      }
      k++;
    }
  }
  else {
    for (i = pindex1U; i <= pindex2U; i++) {
      l = 1;
      for (j = pindex1V; j <= pindex2V; j++) {
        npoles->SetValue(k, l, poles->Value(i, j));
        l++;
      }
      k++;
    }
  }

  uknots = nuknots;
  umults = numults;
  vknots = nvknots;
  vmults = nvmults;
  poles = npoles;
  if (urational || vrational)
    weights = nweights;
  else
    weights = new TColStd_HArray2OfReal(1, poles->ColLength(),
                                        1, poles->RowLength(), 1.0);

  maxderivinvok = 0;
  UpdateUKnots();
  UpdateVKnots();
}

//=======================================================================
//function : Segment
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::Segment(const Standard_Real U1, 
                                  const Standard_Real U2,
                                  const Standard_Real V1,
                                  const Standard_Real V2,
                                  const Standard_Real theUTolerance,
                                  const Standard_Real theVTolerance)
{
  if ((U2 < U1) || (V2 < V1))
    throw Standard_DomainError("Geom_BSplineSurface::Segment");

  Standard_Real aMaxU = Max(Abs(U2), Abs(U1));
  Standard_Real EpsU = Max(Epsilon(aMaxU), theUTolerance);
  
  Standard_Real aMaxV = Max(Abs(V2), Abs(V1));
  Standard_Real EpsV = Max(Epsilon(aMaxV), theVTolerance);

  segment(U1, U2, V1, V2, EpsU, EpsV, Standard_True, Standard_True);
}

//=======================================================================
//function : CheckAndSegment
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::CheckAndSegment(const Standard_Real U1, 
                                          const Standard_Real U2,
                                          const Standard_Real V1,
                                          const Standard_Real V2,
                                          const Standard_Real theUTolerance,
                                          const Standard_Real theVTolerance)
{

  if ((U2 < U1) || (V2 < V1))
    throw Standard_DomainError("Geom_BSplineSurface::CheckAndSegment");

  Standard_Real aMaxU = Max(Abs(U2), Abs(U1));
  Standard_Real EpsU = Max(Epsilon(aMaxU), theUTolerance);
  
  Standard_Real aMaxV = Max(Abs(V2), Abs(V1));
  Standard_Real EpsV = Max(Epsilon(aMaxV), theVTolerance);

  Standard_Boolean segment_in_U = Standard_True;
  Standard_Boolean segment_in_V = Standard_True;
  segment_in_U = ( Abs(U1 - uknots->Value(uknots->Lower())) > EpsU )
                        || ( Abs(U2 - uknots->Value(uknots->Upper())) > EpsU );
  segment_in_V = ( Abs(V1 - vknots->Value(vknots->Lower())) > EpsV )
                        || ( Abs(V2 - vknots->Value(vknots->Upper())) > EpsV );

  segment(U1, U2, V1, V2, EpsU, EpsV, segment_in_U, segment_in_V);
}

//=======================================================================
//function : SetUKnot
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::SetUKnot
(const Standard_Integer UIndex,
 const Standard_Real    K      )
{
  if (UIndex < 1 || UIndex > uknots->Length())
    throw Standard_OutOfRange("Geom_BSplineSurface::SetUKnot: Index and #knots mismatch");

  Standard_Integer NewIndex = UIndex;
  Standard_Real DU = Abs(Epsilon (K));
  if (UIndex == 1) {
    if (K >= uknots->Value (2) - DU)
      throw Standard_ConstructionError("Geom_BSplineSurface::SetUKnot: K out of range");
  }
  else if (UIndex == uknots->Length()) {
    if (K <= uknots->Value (uknots->Length()-1) + DU)  {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetUKnot: K out of range");
    }
  }
  else {
    if (K <= uknots->Value (NewIndex-1) + DU || 
	K >= uknots->Value (NewIndex+1) - DU ) { 
      throw Standard_ConstructionError("Geom_BSplineSurface::SetUKnot: K out of range");
    } 
  }
  
  if (K != uknots->Value (NewIndex)) {
    uknots->SetValue (NewIndex, K);
    maxderivinvok = 0;
    UpdateUKnots();
  }
}

//=======================================================================
//function : SetUKnots
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::SetUKnots (const TColStd_Array1OfReal& UK) {

  Standard_Integer Lower = UK.Lower();
  Standard_Integer Upper = UK.Upper();
  if (Lower < 1 || Lower > uknots->Length() ||
      Upper < 1 || Upper > uknots->Length() ) {
    throw Standard_OutOfRange("Geom_BSplineSurface::SetUKnots: invalid array dimension");
  }
  if (Lower > 1) {
    if (Abs (UK (Lower) - uknots->Value (Lower-1)) <= gp::Resolution()) {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetUKnots: invalid knot value");
    }
  }
  if (Upper < uknots->Length ()) {
    if (Abs (UK (Upper) - uknots->Value (Upper+1)) <= gp::Resolution()) {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetUKnots: invalid knot value");
    }
  }
  Standard_Real K1 = UK (Lower);
  for (Standard_Integer i = Lower; i <= Upper; i++) {
    uknots->SetValue (i, UK(i));
    if (i != Lower) {
      if (Abs (UK(i) - K1) <= gp::Resolution()) {
        throw Standard_ConstructionError("Geom_BSplineSurface::SetUKnots: invalid knot value");
      }
      K1 = UK (i);
    }
  }

  maxderivinvok = 0;
  UpdateUKnots();
}

//=======================================================================
//function : SetUKnot
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::SetUKnot
(const Standard_Integer UIndex,
 const Standard_Real    K,
 const Standard_Integer M)
{
  IncreaseUMultiplicity (UIndex, M);
  SetUKnot (UIndex, K);
}

//=======================================================================
//function : SetVKnot
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::SetVKnot
(const Standard_Integer VIndex,
 const Standard_Real    K)
{
  if (VIndex < 1 || VIndex > vknots->Length())
    throw Standard_OutOfRange("Geom_BSplineSurface::SetVKnot: Index and #knots mismatch");
  Standard_Integer NewIndex = VIndex + vknots->Lower() - 1;
  Standard_Real DV = Abs(Epsilon (K));
  if (VIndex == 1) {
    if (K >=  vknots->Value (2) - DV) {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetVKnot: K out of range");
    }
  }
  else if (VIndex == vknots->Length()) {
    if (K <= vknots->Value (vknots->Length()-1) + DV)  {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetVKnot: K out of range");
    }
  }
  else {
    if (K <= vknots->Value (NewIndex-1) + DV || 
	K >= vknots->Value (NewIndex+1) - DV ) { 
      throw Standard_ConstructionError("Geom_BSplineSurface::SetVKnot: K out of range");
    } 
  }
  
  if (K != vknots->Value (NewIndex)) {
    vknots->SetValue (NewIndex, K);
    maxderivinvok = 0;
    UpdateVKnots();
  }
}

//=======================================================================
//function : SetVKnots
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::SetVKnots (const TColStd_Array1OfReal& VK) {

  Standard_Integer Lower = VK.Lower();
  Standard_Integer Upper = VK.Upper();
  if (Lower < 1 || Lower > vknots->Length() ||
      Upper < 1 || Upper > vknots->Length() ) {
    throw Standard_OutOfRange("Geom_BSplineSurface::SetVKnots: invalid array dimension");
  }
  if (Lower > 1) {
    if (Abs (VK (Lower) - vknots->Value (Lower-1)) <= gp::Resolution()) {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetVKnots: invalid knot value");
    }
  }
  if (Upper < vknots->Length ()) {
    if (Abs (VK (Upper) - vknots->Value (Upper+1)) <= gp::Resolution()) {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetVKnots: invalid knot value");
    }
  }
  Standard_Real K1 = VK (Lower);
  for (Standard_Integer i = Lower; i <= Upper; i++) {
    vknots->SetValue (i, VK(i));
    if (i != Lower) {
      if (Abs (VK(i) - K1) <= gp::Resolution()) {
        throw Standard_ConstructionError("Geom_BSplineSurface::SetVKnots: invalid knot value");
      }
      K1 = VK (i);
    }
  }

  maxderivinvok = 0;
  UpdateVKnots();
}

//=======================================================================
//function : SetVKnot
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::SetVKnot
(const Standard_Integer VIndex,
 const Standard_Real    K,
 const Standard_Integer M)
{
  IncreaseVMultiplicity (VIndex, M);
  SetVKnot (VIndex, K);
}

//=======================================================================
//function : InsertUKnot
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::InsertUKnot
(const Standard_Real    U,
 const Standard_Integer M,
 const Standard_Real    ParametricTolerance,
 const Standard_Boolean Add)
{
  TColStd_Array1OfReal k(1,1);
  k(1) = U;
  TColStd_Array1OfInteger m(1,1);
  m(1) = M;
  InsertUKnots(k,m,ParametricTolerance,Add);
}

//=======================================================================
//function : InsertVKnot
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::InsertVKnot
(const Standard_Real    V,
 const Standard_Integer M,
 const Standard_Real    ParametricTolerance,
 const Standard_Boolean Add)
{
  TColStd_Array1OfReal k(1,1);
  k(1) = V;
  TColStd_Array1OfInteger m(1,1);
  m(1) = M;
  InsertVKnots(k,m,ParametricTolerance,Add);
}

//=======================================================================
//function : IncrementUMultiplicity
//purpose  : 
//=======================================================================

void  Geom_BSplineSurface::IncrementUMultiplicity
(const Standard_Integer FromI1,
 const Standard_Integer ToI2,
 const Standard_Integer Step)
{
  Handle(TColStd_HArray1OfReal) tk = uknots;
  TColStd_Array1OfReal k( (uknots->Array1())(FromI1), FromI1, ToI2);
  TColStd_Array1OfInteger m( FromI1, ToI2) ;
  m.Init(Step);
  InsertUKnots( k, m, Epsilon(1.));
}

//=======================================================================
//function : IncrementVMultiplicity
//purpose  : 
//=======================================================================

void  Geom_BSplineSurface::IncrementVMultiplicity
(const Standard_Integer FromI1,
 const Standard_Integer ToI2,
 const Standard_Integer Step)
{
  Handle(TColStd_HArray1OfReal) tk = vknots;
  TColStd_Array1OfReal k( (vknots->Array1())(FromI1), FromI1, ToI2);

  TColStd_Array1OfInteger m( FromI1, ToI2) ;
  m.Init(Step);
  
  InsertVKnots( k, m, Epsilon(1.));
}

//=======================================================================
//function : UpdateUKnots
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::UpdateUKnots()
{

  Standard_Integer MaxKnotMult = 0;
  BSplCLib::KnotAnalysis (udeg, uperiodic,
		uknots->Array1(), 
		umults->Array1(), 
		uknotSet, MaxKnotMult);
  
  if (uknotSet == GeomAbs_Uniform && !uperiodic)  {
    ufknots = uknots;
  }
  else {
    ufknots = new TColStd_HArray1OfReal 
      (1, BSplCLib::KnotSequenceLength(umults->Array1(),udeg,uperiodic));

    BSplCLib::KnotSequence (uknots->Array1(), 
			    umults->Array1(),
			    udeg,uperiodic,
			    ufknots->ChangeArray1());
  }
  
  if (MaxKnotMult == 0)  Usmooth = GeomAbs_CN;
  else {
    switch (udeg - MaxKnotMult) {
    case 0 :   Usmooth = GeomAbs_C0;   break;
    case 1 :   Usmooth = GeomAbs_C1;   break;
    case 2 :   Usmooth = GeomAbs_C2;   break;
    case 3 :   Usmooth = GeomAbs_C3;   break;
      default :  Usmooth = GeomAbs_C3;   break;
    }
  }
}

//=======================================================================
//function : UpdateVKnots
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::UpdateVKnots()
{
  Standard_Integer MaxKnotMult = 0;
  BSplCLib::KnotAnalysis (vdeg, vperiodic,
		vknots->Array1(), 
		vmults->Array1(), 
		vknotSet, MaxKnotMult);
  
  if (vknotSet == GeomAbs_Uniform && !vperiodic)  {
    vfknots = vknots;
  }
  else {
    vfknots = new TColStd_HArray1OfReal 
      (1, BSplCLib::KnotSequenceLength(vmults->Array1(),vdeg,vperiodic));

    BSplCLib::KnotSequence (vknots->Array1(), 
			    vmults->Array1(),
			    vdeg,vperiodic,
			    vfknots->ChangeArray1());
  }
  
  if (MaxKnotMult == 0)  Vsmooth = GeomAbs_CN;
  else {
    switch (vdeg - MaxKnotMult) {
    case 0 :   Vsmooth = GeomAbs_C0;   break;
    case 1 :   Vsmooth = GeomAbs_C1;   break;
    case 2 :   Vsmooth = GeomAbs_C2;   break;
    case 3 :   Vsmooth = GeomAbs_C3;   break;
      default :  Vsmooth = GeomAbs_C3;   break;
    }
  }
}


//=======================================================================
//function : Normalizes the parameters if the curve is periodic
//purpose  : that is compute the cache so that it is valid
//=======================================================================

void Geom_BSplineSurface::PeriodicNormalization
(Standard_Real&  Uparameter, 
 Standard_Real&  Vparameter) const 
{
  Standard_Real Period, aMaxVal, aMinVal;
  
  if (uperiodic) {
    aMaxVal = ufknots->Value(ufknots->Upper() - udeg);
    aMinVal = ufknots->Value (udeg + 1);
    Standard_Real eps = Abs(Epsilon(Uparameter));
    Period =  aMaxVal - aMinVal;

    if(Period <= eps) 
      throw Standard_OutOfRange("Geom_BSplineSurface::PeriodicNormalization: Uparameter is too great number");

    Standard_Boolean isLess, isGreater;
    isLess = aMinVal - Uparameter > 0;
    isGreater = Uparameter - aMaxVal > 0;
    if (isLess || isGreater) {
      Standard_Real aDPar, aNbPer;
      aDPar = (isLess) ? (aMaxVal - Uparameter) : (aMinVal - Uparameter);
      modf(aDPar / Period, &aNbPer);
      Uparameter += aNbPer * Period;
    }
  }
  if (vperiodic) {
    aMaxVal = vfknots->Value(vfknots->Upper() - vdeg);
    aMinVal = vfknots->Value (vdeg + 1);
    Standard_Real eps = Abs(Epsilon(Vparameter));
    Period = aMaxVal - aMinVal;

    if(Period <= eps) 
      throw Standard_OutOfRange("Geom_BSplineSurface::PeriodicNormalization: Vparameter is too great number");

    Standard_Boolean isLess, isGreater;
    isLess = aMinVal - Vparameter > 0;
    isGreater = Vparameter - aMaxVal > 0;
    if (isLess || isGreater) {
      Standard_Real aDPar, aNbPer;
      aDPar = (isLess) ? (aMaxVal - Vparameter) : (aMinVal - Vparameter);
      modf(aDPar / Period, &aNbPer);
      Vparameter += aNbPer * Period;
    }
  }
}

//=======================================================================
//function : SetWeight
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::SetWeight (const Standard_Integer UIndex,
				     const Standard_Integer VIndex,
				     const Standard_Real    Weight)
{
  if (Weight <= gp::Resolution())
    throw Standard_ConstructionError("Geom_BSplineSurface::SetWeight: Weight too small");
  TColStd_Array2OfReal & Weights = weights->ChangeArray2();
  if (UIndex < 1 || UIndex > Weights.ColLength() ||
      VIndex < 1 || VIndex > Weights.RowLength() ) {
    throw Standard_OutOfRange("Geom_BSplineSurface::SetWeight: Index and #pole mismatch");
  }
  Weights (UIndex+Weights.LowerRow()-1, VIndex+Weights.LowerCol()-1) = Weight;
  Rational(Weights, urational, vrational);
}

//=======================================================================
//function : SetWeightCol
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::SetWeightCol
(const Standard_Integer       VIndex, 
 const TColStd_Array1OfReal&  CPoleWeights)
{
  TColStd_Array2OfReal & Weights = weights->ChangeArray2();   
  if (VIndex < 1 || VIndex > Weights.RowLength()) {
    throw Standard_OutOfRange("Geom_BSplineSurface::SetWeightCol: Index and #pole mismatch");
  }
  if (CPoleWeights.Lower() < 1 || 
      CPoleWeights.Lower() > Weights.ColLength() ||
      CPoleWeights.Upper() < 1 ||
      CPoleWeights.Upper() > Weights.ColLength()  ) {
    throw Standard_ConstructionError("Geom_BSplineSurface::SetWeightCol: invalid array dimension");
  }
  Standard_Integer I = CPoleWeights.Lower();
  while (I <= CPoleWeights.Upper()) {
    if (CPoleWeights(I) <= gp::Resolution()) { 
      throw Standard_ConstructionError("Geom_BSplineSurface::SetWeightCol: Weight too small");
    }
    Weights (I+Weights.LowerRow()-1, VIndex+Weights.LowerCol()-1) = 
      CPoleWeights (I);
    I++;
  }
  // Verifie si c'est rationnel
  Rational(Weights, urational, vrational);
}

//=======================================================================
//function : SetWeightRow
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::SetWeightRow
(const Standard_Integer       UIndex, 
 const TColStd_Array1OfReal&  CPoleWeights)
{
  TColStd_Array2OfReal & Weights = weights->ChangeArray2();   
  if (UIndex < 1 || UIndex > Weights.ColLength()) {
    throw Standard_OutOfRange("Geom_BSplineSurface::SetWeightRow: Index and #pole mismatch");
  }
  if (CPoleWeights.Lower() < 1 ||
      CPoleWeights.Lower() > Weights.RowLength() ||
      CPoleWeights.Upper() < 1 ||
      CPoleWeights.Upper() > Weights.RowLength()  ) {
    
    throw Standard_ConstructionError("Geom_BSplineSurface::SetWeightRow: invalid array dimension");
  }
  Standard_Integer I = CPoleWeights.Lower();

  while (I <= CPoleWeights.Upper()) {
    if (CPoleWeights(I)<=gp::Resolution()) {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetWeightRow: Weight too small");
    }
    Weights (UIndex+Weights.LowerRow()-1, I+Weights.LowerCol()-1) = 
      CPoleWeights (I);
    I++;
  }
  // Verifie si c'est rationnel
  Rational(Weights, urational, vrational);
}

//=======================================================================
//function : DumpJson
//purpose  : 
//=======================================================================
void Geom_BSplineSurface::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)
  OCCT_DUMP_BASE_CLASS (theOStream, theDepth, Geom_BoundedSurface)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, urational)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, vrational)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, uperiodic)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, vperiodic)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, uknotSet)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, vknotSet)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, Usmooth)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, Vsmooth)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, udeg)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, vdeg)

  if (!poles.IsNull())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, poles->Size())
  if (!weights.IsNull())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, weights->Size())
  if (!ufknots.IsNull())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, ufknots->Size())
  if (!vfknots.IsNull())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, vfknots->Size())

  if (!uknots.IsNull())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, uknots->Size())
  if (!vknots.IsNull())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, vknots->Size())
  if (!umults.IsNull())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, umults->Size())
  if (!vmults.IsNull())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, vmults->Size())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, umaxderivinv)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, vmaxderivinv)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, maxderivinvok)
}
