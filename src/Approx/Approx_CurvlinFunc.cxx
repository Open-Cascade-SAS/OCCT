// Created on: 1998-05-12
// Created by: Roman BORISOV
// Copyright (c) 1998-1999 Matra Datavision
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


#include <Adaptor2d_HCurve2d.hxx>
#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_HCurveOnSurface.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Approx_CurvlinFunc.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <GeomLib.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TColStd_SequenceOfReal.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Approx_CurvlinFunc,Standard_Transient)

#ifdef OCCT_DEBUG_CHRONO
#include <OSD_Timer.hxx>
static OSD_Chronometer chr_uparam;
Standard_EXPORT Standard_Integer uparam_count;
Standard_EXPORT Standard_Real t_uparam;

//Standard_IMPORT extern void InitChron(OSD_Chronometer& ch);
Standard_IMPORT void InitChron(OSD_Chronometer& ch);
//Standard_IMPORT extern void ResultChron( OSD_Chronometer & ch, Standard_Real & time);
Standard_IMPORT void ResultChron( OSD_Chronometer & ch, Standard_Real & time);
#endif

static Standard_Real cubic(const Standard_Real X, const Standard_Real *Xi, const Standard_Real *Yi)
{
  Standard_Real I1, I2, I3, I21, I22, I31, Result;

  I1 = (Yi[0] - Yi[1])/(Xi[0] - Xi[1]);
  I2 = (Yi[1] - Yi[2])/(Xi[1] - Xi[2]);
  I3 = (Yi[2] - Yi[3])/(Xi[2] - Xi[3]);

  I21 = (I1 - I2)/(Xi[0] - Xi[2]);
  I22 = (I2 - I3)/(Xi[1] - Xi[3]);
  
  I31 = (I21 - I22)/(Xi[0] - Xi[3]);  

  Result = Yi[0] + (X - Xi[0])*(I1 + (X - Xi[1])*(I21 + (X - Xi[2])*I31));

  return Result;
}

//static void findfourpoints(const Standard_Real S, 
static void findfourpoints(const Standard_Real , 
			   Standard_Integer NInterval, 
			   const Handle(TColStd_HArray1OfReal)& Si, 
			   Handle(TColStd_HArray1OfReal)& Ui, 
			   const Standard_Real prevS, 
			   const Standard_Real prevU, Standard_Real *Xi, 
			   Standard_Real *Yi)
{
  Standard_Integer i, j;
  Standard_Integer NbInt = Si->Length() - 1;
  if (NbInt < 3) throw Standard_ConstructionError("Approx_CurvlinFunc::GetUParameter");

  if(NInterval < 1) NInterval = 1;
  else if(NInterval > NbInt - 2) NInterval = NbInt - 2;

  for(i = 0; i < 4; i++) {
    Xi[i] = Si->Value(NInterval - 1 + i);
    Yi[i] = Ui->Value(NInterval - 1 + i);
  }
// try to insert (S, U)  
  for(i = 0; i < 3; i++) {
    if(Xi[i] < prevS && prevS < Xi[i+1]) {
      for(j = 0; j < i; j++) {
	Xi[j] = Xi[j+1];
	Yi[j] = Yi[j+1];
      }
      Xi[i] = prevS;
      Yi[i] = prevU;
      break;
    }
  }
}

/*static Standard_Real curvature(const Standard_Real U, const Adaptor3d_Curve& C)
{
  Standard_Real k, tau, mod1, mod2, OMEGA;
  gp_Pnt P;
  gp_Vec D1, D2, D3;
  C.D3(U, P, D1, D2, D3);
  mod1 = D1.Magnitude();
  mod2 = D1.Crossed(D2).Magnitude();
  k = mod2/(mod1*mod1*mod1);
  tau = D1.Dot(D2.Crossed(D3));
  tau /= mod2*mod2;
  OMEGA = Sqrt(k*k + tau*tau);

  return OMEGA;
}
*/

Approx_CurvlinFunc::Approx_CurvlinFunc(const Handle(Adaptor3d_HCurve)& C, const Standard_Real Tol) : myC3D(C), 
                    myCase(1), 
                    myFirstS(0), 
                    myLastS(1), 
                    myTolLen(Tol),
                    myPrevS (0.0),
                    myPrevU (0.0)
{
  Init();
}

Approx_CurvlinFunc::Approx_CurvlinFunc(const Handle(Adaptor2d_HCurve2d)& C2D, const Handle(Adaptor3d_HSurface)& S, const Standard_Real Tol) : 
                    myC2D1(C2D), 
                    mySurf1(S), 
                    myCase(2), 
                    myFirstS(0), 
                    myLastS(1), 
                    myTolLen(Tol),
                    myPrevS (0.0),
                    myPrevU (0.0)
{  
  Init();
}

Approx_CurvlinFunc::Approx_CurvlinFunc(const Handle(Adaptor2d_HCurve2d)& C2D1, const Handle(Adaptor2d_HCurve2d)& C2D2, const Handle(Adaptor3d_HSurface)& S1, const Handle(Adaptor3d_HSurface)& S2, const Standard_Real Tol) : 
                    myC2D1(C2D1), 
                    myC2D2(C2D2), 
                    mySurf1(S1), 
                    mySurf2(S2), 
                    myCase(3), 
                    myFirstS(0), 
                    myLastS(1), 
                    myTolLen(Tol),
                    myPrevS (0.0),
                    myPrevU (0.0)
{  
  Init();
}

void Approx_CurvlinFunc::Init()
{
  Adaptor3d_CurveOnSurface CurOnSur;
  
  switch(myCase) {
  case 1:
    Init(myC3D->GetCurve(), mySi_1, myUi_1);
    myFirstU1 = myC3D->FirstParameter();
    myLastU1 = myC3D->LastParameter();
    myFirstU2 = myLastU2 = 0;
    break;
  case 2:
    CurOnSur.Load(myC2D1);
    CurOnSur.Load(mySurf1);
    Init(CurOnSur, mySi_1, myUi_1);
    myFirstU1 = CurOnSur.FirstParameter();
    myLastU1 = CurOnSur.LastParameter();
    myFirstU2 = myLastU2 = 0;
    break;
  case 3:
    CurOnSur.Load(myC2D1);
    CurOnSur.Load(mySurf1);
    Init(CurOnSur, mySi_1, myUi_1);
    myFirstU1 = CurOnSur.FirstParameter();
    myLastU1 = CurOnSur.LastParameter();
    CurOnSur.Load(myC2D2);
    CurOnSur.Load(mySurf2);
    Init(CurOnSur, mySi_2, myUi_2);
    myFirstU2 = CurOnSur.FirstParameter();
    myLastU2 = CurOnSur.LastParameter();
  }

  Length();
}


//=======================================================================
//function : Init
//purpose  : Init the values
//history  : 23/10/1998 PMN : Cut at curve's discontinuities
//=======================================================================
void Approx_CurvlinFunc::Init(Adaptor3d_Curve& C, Handle(TColStd_HArray1OfReal)& Si, 
			      Handle(TColStd_HArray1OfReal)& Ui) const
{
  Standard_Real Step, FirstU, LastU;
  Standard_Integer i, j, k, NbInt, NbIntC3;
  FirstU = C.FirstParameter();
  LastU  = C.LastParameter();

  NbInt = 10;
  NbIntC3 = C.NbIntervals(GeomAbs_C3);
  TColStd_Array1OfReal Disc(1, NbIntC3+1);

  if (NbIntC3 >1) {
     C.Intervals(Disc, GeomAbs_C3);
  }
  else {
    Disc(1) = FirstU;
    Disc(2) = LastU;
  }

  Ui = new TColStd_HArray1OfReal (0,NbIntC3*NbInt);
  Si = new TColStd_HArray1OfReal (0,NbIntC3*NbInt);

  Ui->SetValue(0, FirstU);
  Si->SetValue(0, 0);

  for(j = 1, i=1; j<=NbIntC3; j++) {
    Step = (Disc(j+1) - Disc(j))/NbInt;
    for(k = 1; k <= NbInt; k++, i++) {
      Ui->ChangeValue(i) = Ui->Value(i-1) + Step;
      Si->ChangeValue(i) = Si->Value(i-1) + Length(C, Ui->Value(i-1), Ui->Value(i));
    }
  }

  Standard_Real Len = Si->Value(Si->Upper());
  for(i = Si->Lower(); i<= Si->Upper(); i++)
    Si->ChangeValue(i) /= Len;

  // TODO - fields should be mutable
  const_cast<Approx_CurvlinFunc*>(this)->myPrevS = myFirstS;
  const_cast<Approx_CurvlinFunc*>(this)->myPrevU = FirstU;
}

void  Approx_CurvlinFunc::SetTol(const Standard_Real Tol)
{
  myTolLen = Tol;
}

Standard_Real Approx_CurvlinFunc::FirstParameter() const
{
  return myFirstS;
}

Standard_Real Approx_CurvlinFunc::LastParameter() const
{
  return myLastS;
}

Standard_Integer Approx_CurvlinFunc::NbIntervals(const GeomAbs_Shape S) const
{
  Adaptor3d_CurveOnSurface CurOnSur;

  switch(myCase) {
  case 1:
    return myC3D->NbIntervals(S);
  case 2:
    CurOnSur.Load(myC2D1);
    CurOnSur.Load(mySurf1);
    return CurOnSur.NbIntervals(S);
  case 3:
    Standard_Integer NbInt;
    CurOnSur.Load(myC2D1);
    CurOnSur.Load(mySurf1);
    NbInt = CurOnSur.NbIntervals(S);
    TColStd_Array1OfReal T1(1, NbInt+1);
    CurOnSur.Intervals(T1, S);
    CurOnSur.Load(myC2D2);
    CurOnSur.Load(mySurf2);
    NbInt = CurOnSur.NbIntervals(S);
    TColStd_Array1OfReal T2(1, NbInt+1);
    CurOnSur.Intervals(T2, S);

    TColStd_SequenceOfReal Fusion;
    GeomLib::FuseIntervals(T1, T2, Fusion);
    return Fusion.Length() - 1;
  }

  //POP pour WNT
  return 1;
}

void Approx_CurvlinFunc::Intervals(TColStd_Array1OfReal& T, const GeomAbs_Shape S) const
{
  Adaptor3d_CurveOnSurface CurOnSur;
  Standard_Integer i;
    
  switch(myCase) {
  case 1:
    myC3D->Intervals(T, S);
    break;
  case 2:
    CurOnSur.Load(myC2D1);
    CurOnSur.Load(mySurf1);
    CurOnSur.Intervals(T, S);
    break;
  case 3:
    Standard_Integer NbInt;
    CurOnSur.Load(myC2D1);
    CurOnSur.Load(mySurf1);
    NbInt = CurOnSur.NbIntervals(S);
    TColStd_Array1OfReal T1(1, NbInt+1);
    CurOnSur.Intervals(T1, S);
    CurOnSur.Load(myC2D2);
    CurOnSur.Load(mySurf2);
    NbInt = CurOnSur.NbIntervals(S);
    TColStd_Array1OfReal T2(1, NbInt+1);
    CurOnSur.Intervals(T2, S);

    TColStd_SequenceOfReal Fusion;
    GeomLib::FuseIntervals(T1, T2, Fusion);

    for (i = 1; i <= Fusion.Length(); i++)
      T.ChangeValue(i) = Fusion.Value(i);
  }

  for(i = 1; i <= T.Length(); i++)
    T.ChangeValue(i) = GetSParameter(T.Value(i));
}

void Approx_CurvlinFunc::Trim(const Standard_Real First, const Standard_Real Last, const Standard_Real Tol)
{
  if (First < 0 || Last >1) throw Standard_OutOfRange("Approx_CurvlinFunc::Trim");
  if ((Last - First) < Tol) return;

  Standard_Real FirstU, LastU;
  Adaptor3d_CurveOnSurface CurOnSur;
  Handle(Adaptor3d_HCurve) HCurOnSur;

  switch(myCase) {
  case 1:
    myC3D = myC3D->Trim(myFirstU1, myLastU1, Tol);
    FirstU = GetUParameter(myC3D->GetCurve(), First, 1);
    LastU = GetUParameter(myC3D->GetCurve(), Last, 1);
    myC3D = myC3D->Trim(FirstU, LastU, Tol);
    break;
  case 3:
    CurOnSur.Load(myC2D2);
    CurOnSur.Load(mySurf2);
    HCurOnSur = CurOnSur.Trim(myFirstU2, myLastU2, Tol);
    myC2D2 = ((Adaptor3d_CurveOnSurface *)(&(HCurOnSur->Curve())))->GetCurve();
    mySurf2 = ((Adaptor3d_CurveOnSurface *)(&(HCurOnSur->Curve())))->GetSurface();
    CurOnSur.Load(myC2D2);
    CurOnSur.Load(mySurf2);

    FirstU = GetUParameter(CurOnSur, First, 1);
    LastU = GetUParameter(CurOnSur, Last, 1);
    HCurOnSur = CurOnSur.Trim(FirstU, LastU, Tol);
    myC2D2 = ((Adaptor3d_CurveOnSurface *)(&(HCurOnSur->Curve())))->GetCurve();
    mySurf2 = ((Adaptor3d_CurveOnSurface *)(&(HCurOnSur->Curve())))->GetSurface();

    Standard_FALLTHROUGH
  case 2:
    CurOnSur.Load(myC2D1);
    CurOnSur.Load(mySurf1);
    HCurOnSur = CurOnSur.Trim(myFirstU1, myLastU1, Tol);
    myC2D1 = ((Adaptor3d_CurveOnSurface *)(&(HCurOnSur->Curve())))->GetCurve();
    mySurf1 = ((Adaptor3d_CurveOnSurface *)(&(HCurOnSur->Curve())))->GetSurface();
    CurOnSur.Load(myC2D1);
    CurOnSur.Load(mySurf1);

    FirstU = GetUParameter(CurOnSur, First, 1);
    LastU = GetUParameter(CurOnSur, Last, 1);
    HCurOnSur = CurOnSur.Trim(FirstU, LastU, Tol);
    myC2D1 = ((Adaptor3d_CurveOnSurface *)(&(HCurOnSur->Curve())))->GetCurve();
    mySurf1 = ((Adaptor3d_CurveOnSurface *)(&(HCurOnSur->Curve())))->GetSurface();
  }
  myFirstS = First;
  myLastS = Last;  
}

void Approx_CurvlinFunc::Length()
{
  Adaptor3d_CurveOnSurface CurOnSur;
  Standard_Real FirstU, LastU;

  switch(myCase){
  case 1:
    FirstU = myC3D->FirstParameter();
    LastU = myC3D->LastParameter();
    myLength = Length(myC3D->GetCurve(), FirstU, LastU);    
    myLength1 = myLength2 = 0;    
    break;
  case 2:
    CurOnSur.Load(myC2D1);
    CurOnSur.Load(mySurf1);
    FirstU = CurOnSur.FirstParameter();
    LastU = CurOnSur.LastParameter();
    myLength = Length(CurOnSur, FirstU, LastU);
    myLength1 = myLength2 = 0;    
    break;
  case 3:
    CurOnSur.Load(myC2D1);
    CurOnSur.Load(mySurf1);
    FirstU = CurOnSur.FirstParameter();
    LastU = CurOnSur.LastParameter();
    myLength1 = Length(CurOnSur, FirstU, LastU);
    CurOnSur.Load(myC2D2);
    CurOnSur.Load(mySurf2);
    FirstU = CurOnSur.FirstParameter();
    LastU = CurOnSur.LastParameter();
    myLength2 = Length(CurOnSur, FirstU, LastU);
    myLength = (myLength1 + myLength2)/2;
  }
}


Standard_Real Approx_CurvlinFunc::Length(Adaptor3d_Curve& C, const Standard_Real FirstU, const Standard_Real LastU) const
{
  Standard_Real Length;

  Length = GCPnts_AbscissaPoint::Length(C, FirstU, LastU, myTolLen);
  return Length;
}


Standard_Real Approx_CurvlinFunc::GetLength() const
{
  return myLength;
}

Standard_Real Approx_CurvlinFunc::GetSParameter(const Standard_Real U) const
{
  Standard_Real S=0, S1, S2;
  Adaptor3d_CurveOnSurface CurOnSur;

  switch (myCase) {
  case 1:
    S = GetSParameter(myC3D->GetCurve(), U, myLength);
    break;
  case 2:
    CurOnSur.Load(myC2D1);
    CurOnSur.Load(mySurf1);
    S = GetSParameter(CurOnSur, U, myLength);
    break;
  case 3:
    CurOnSur.Load(myC2D1);
    CurOnSur.Load(mySurf1);
    S1 = GetSParameter(CurOnSur, U, myLength1);    
    CurOnSur.Load(myC2D2);
    CurOnSur.Load(mySurf2);
    S2 = GetSParameter(CurOnSur, U, myLength2);    
    S = (S1 + S2)/2;
  }
  return S;
}



Standard_Real Approx_CurvlinFunc::GetUParameter(Adaptor3d_Curve& C, 
						const Standard_Real S, 
						const Standard_Integer NumberOfCurve) const
{
  Standard_Real deltaS, base, U, Length;
  Standard_Integer NbInt, NInterval, i;
  Handle(TColStd_HArray1OfReal) InitUArray, InitSArray;
#ifdef OCCT_DEBUG_CHRONO
  InitChron(chr_uparam);
#endif
  if(S < 0 || S > 1) throw Standard_ConstructionError("Approx_CurvlinFunc::GetUParameter");

  if(NumberOfCurve == 1) {
    InitUArray = myUi_1;
    InitSArray = mySi_1;
    if(myCase == 3)
      Length = myLength1;
    else 
      Length = myLength;
  }
  else {
    InitUArray = myUi_2;
    InitSArray = mySi_2;
    Length = myLength2;
  }

  NbInt = InitUArray->Length() - 1;

  if(S == 1) NInterval = NbInt - 1;
  else {
    for(i = 0; i < NbInt; i++) {
      if((InitSArray->Value(i) <= S && S < InitSArray->Value(i+1)))
	break;
    }
    NInterval = i;
  }
  if(S==InitSArray->Value(NInterval)) {
    return InitUArray->Value(NInterval);
  }
  if(S==InitSArray->Value(NInterval+1)) {
    return InitUArray->Value(NInterval+1);
  } 

  base = InitUArray->Value(NInterval);
  deltaS = (S - InitSArray->Value(NInterval))*Length;

// to find an initial point
  Standard_Real Xi[4], Yi[4], UGuess;
  findfourpoints(S, NInterval, InitSArray, InitUArray, myPrevS, myPrevU, Xi, Yi);
  UGuess = cubic(S , Xi, Yi);

  U = GCPnts_AbscissaPoint(C, deltaS, base, UGuess, myTolLen).Parameter();

  // TODO - fields should be mutable
  const_cast<Approx_CurvlinFunc*>(this)->myPrevS = S;
  const_cast<Approx_CurvlinFunc*>(this)->myPrevU = U;

#ifdef OCCT_DEBUG_CHRONO
  ResultChron(chr_uparam, t_uparam);
  uparam_count++;
#endif

  return U;
}

Standard_Real Approx_CurvlinFunc::GetSParameter(Adaptor3d_Curve& C, const Standard_Real U, const Standard_Real Len) const
{
  Standard_Real S, Origin;

  Origin = C.FirstParameter();
  S = myFirstS + Length(C, Origin, U)/Len;    
  return S;
}

Standard_Boolean Approx_CurvlinFunc::EvalCase1(const Standard_Real S, const Standard_Integer Order, TColStd_Array1OfReal& Result) const
{
  if(myCase != 1) throw Standard_ConstructionError("Approx_CurvlinFunc::EvalCase1");

  gp_Pnt C;
  gp_Vec dC_dU, dC_dS, d2C_dU2, d2C_dS2;
  Standard_Real U, Mag, dU_dS, d2U_dS2;
  
  U = GetUParameter(myC3D->GetCurve(), S, 1);

  switch(Order) {

  case 0: 
    myC3D->D0(U, C);

    Result(0) = C.X();
    Result(1) = C.Y();
    Result(2) = C.Z();
    break;
    
  case 1:
    myC3D->D1(U, C, dC_dU);
    Mag = dC_dU.Magnitude();
    dU_dS = myLength/Mag;
    dC_dS = dC_dU*dU_dS;

    Result(0) = dC_dS.X();
    Result(1) = dC_dS.Y();
    Result(2) = dC_dS.Z();
    break;

  case 2:
    myC3D->D2(U, C, dC_dU, d2C_dU2);
    Mag = dC_dU.Magnitude();
    dU_dS = myLength/Mag;
    d2U_dS2 = -myLength*dC_dU.Dot(d2C_dU2)*dU_dS/(Mag*Mag*Mag);
    d2C_dS2 = d2C_dU2*dU_dS*dU_dS + dC_dU*d2U_dS2;

    Result(0) = d2C_dS2.X();
    Result(1) = d2C_dS2.Y();
    Result(2) = d2C_dS2.Z();
    break;

  default: Result(0) = Result(1) = Result(2) = 0;
    return Standard_False;
  }
  return Standard_True;
}

Standard_Boolean Approx_CurvlinFunc::EvalCase2(const Standard_Real S, const Standard_Integer Order, TColStd_Array1OfReal& Result) const
{
  if(myCase != 2) throw Standard_ConstructionError("Approx_CurvlinFunc::EvalCase2");

  Standard_Boolean Done;

  Done = EvalCurOnSur(S, Order, Result, 1);

  return Done;
}

Standard_Boolean Approx_CurvlinFunc::EvalCase3(const Standard_Real S, const Standard_Integer Order, TColStd_Array1OfReal& Result)
{
  if(myCase != 3) throw Standard_ConstructionError("Approx_CurvlinFunc::EvalCase3");
  
  TColStd_Array1OfReal tmpRes1(0, 4), tmpRes2(0, 4);
  Standard_Boolean Done;

  Done = EvalCurOnSur(S, Order, tmpRes1, 1);

  Done = EvalCurOnSur(S, Order, tmpRes2, 2) && Done;

  Result(0) = tmpRes1(0);
  Result(1) = tmpRes1(1);
  Result(2) = tmpRes2(0);
  Result(3) = tmpRes2(1);
  Result(4) = 0.5*(tmpRes1(2) + tmpRes2(2));
  Result(5) = 0.5*(tmpRes1(3) + tmpRes2(3));
  Result(6) = 0.5*(tmpRes1(4) + tmpRes2(4));

  return Done;
}

Standard_Boolean Approx_CurvlinFunc::EvalCurOnSur(const Standard_Real S, const Standard_Integer Order, TColStd_Array1OfReal& Result, const  Standard_Integer NumberOfCurve) const
{
  Handle(Adaptor2d_HCurve2d) Cur2D;
  Handle(Adaptor3d_HSurface) Surf;
  Standard_Real U=0, Length=0;

  if (NumberOfCurve == 1) {
    Cur2D = myC2D1;
    Surf = mySurf1;    
    Adaptor3d_CurveOnSurface CurOnSur(myC2D1, mySurf1);
    U = GetUParameter(CurOnSur, S, 1);
    if(myCase == 3) Length = myLength1;
    else Length = myLength;
  }
  else if (NumberOfCurve == 2) {
    Cur2D = myC2D2;
    Surf = mySurf2;    
    Adaptor3d_CurveOnSurface CurOnSur(myC2D2, mySurf2);
    U = GetUParameter(CurOnSur, S, 2);
    Length = myLength2;
  }
  else 
    throw Standard_ConstructionError("Approx_CurvlinFunc::EvalCurOnSur");

  Standard_Real Mag, dU_dS, d2U_dS2, dV_dU, dW_dU, dV_dS, dW_dS, d2V_dS2, d2W_dS2, d2V_dU2, d2W_dU2;
  gp_Pnt2d C2D;
  gp_Pnt C;
  gp_Vec2d dC2D_dU, d2C2D_dU2;
  gp_Vec dC_dU, d2C_dU2, dC_dS, d2C_dS2, dS_dV, dS_dW, d2S_dV2, d2S_dW2, d2S_dVdW;

  switch(Order) {
  case 0:
    Cur2D->D0(U, C2D);
    Surf->D0(C2D.X(), C2D.Y(), C);
    
    Result(0) = C2D.X();
    Result(1) = C2D.Y();
    Result(2) = C.X();
    Result(3) = C.Y();
    Result(4) = C.Z();
    break;

  case 1:
    Cur2D->D1(U, C2D, dC2D_dU);
    dV_dU = dC2D_dU.X();
    dW_dU = dC2D_dU.Y();
    Surf->D1(C2D.X(), C2D.Y(), C, dS_dV, dS_dW);
    dC_dU = dS_dV*dV_dU + dS_dW*dW_dU;
    Mag = dC_dU.Magnitude();
    dU_dS = Length/Mag;

    dV_dS = dV_dU*dU_dS;
    dW_dS = dW_dU*dU_dS;
    dC_dS = dC_dU*dU_dS;

    Result(0) = dV_dS;
    Result(1) = dW_dS;
    Result(2) = dC_dS.X();
    Result(3) = dC_dS.Y();
    Result(4) = dC_dS.Z();    
    break;

  case 2:
    Cur2D->D2(U, C2D, dC2D_dU, d2C2D_dU2);
    dV_dU = dC2D_dU.X();
    dW_dU = dC2D_dU.Y();
    d2V_dU2 = d2C2D_dU2.X();
    d2W_dU2 = d2C2D_dU2.Y();
    Surf->D2(C2D.X(), C2D.Y(), C, dS_dV, dS_dW, d2S_dV2, d2S_dW2, d2S_dVdW);
    dC_dU = dS_dV*dV_dU + dS_dW*dW_dU;
    d2C_dU2 = (d2S_dV2*dV_dU + d2S_dVdW*dW_dU)*dV_dU + dS_dV*d2V_dU2 + 
              (d2S_dVdW*dV_dU + d2S_dW2*dW_dU)*dW_dU + dS_dW*d2W_dU2;
    Mag = dC_dU.Magnitude();
    dU_dS = Length/Mag;
    d2U_dS2 = -Length*dC_dU.Dot(d2C_dU2)*dU_dS/(Mag*Mag*Mag);
    
    dV_dS = dV_dU * dU_dS;
    dW_dS = dW_dU * dU_dS;
    d2V_dS2 = d2V_dU2*dU_dS*dU_dS + dV_dU*d2U_dS2;
    d2W_dS2 = d2W_dU2*dU_dS*dU_dS + dW_dU*d2U_dS2;
    
    d2U_dS2 = -dC_dU.Dot(d2C_dU2)*dU_dS/(Mag*Mag);
    d2C_dS2 = (d2S_dV2 * dV_dS + d2S_dVdW * dW_dS) * dV_dS + dS_dV * d2V_dS2 +
              (d2S_dW2 * dW_dS + d2S_dVdW * dV_dS) * dW_dS + dS_dW * d2W_dS2;
    
    Result(0) = d2V_dS2;
    Result(1) = d2W_dS2;
    Result(2) = d2C_dS2.X();
    Result(3) = d2C_dS2.Y();
    Result(4) = d2C_dS2.Z();      
    break;

  default: Result(0) = Result(1) = Result(2) = Result(3) = Result(4) = 0;
    return Standard_False;
  }
  return Standard_True;
}
