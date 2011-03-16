// File:        Extrema_FuncExtPS.cxx
// Created:        Tue Jul 18 08:11:24 1995
// Author:        Modelistation
//                <model@metrox>

//  Modified by skv - Thu Sep 30 15:21:07 2004 OCC593


#include <Extrema_FuncExtPS.ixx>
#include <gp_Vec.hxx>
#include <gp_Pnt.hxx>
#include <Standard_TypeMismatch.hxx>
#include <Precision.hxx>
#include <GeomAbs_IsoType.hxx>

/*-----------------------------------------------------------------------------
 Fonctions permettant de rechercher une distance extremale entre un point P
et une surface S (en partant d'un point approche S(u0,v0)).
 Cette classe herite de math_FunctionSetWithDerivatives et est utilisee par
l'algorithme math_FunctionSetRoot.
 Si on note Dus et Dvs, les derivees en u et v, les 2 fonctions a annuler sont:
  { F1(u,v) = (S(u,v)-P).Dus(u,v) }
  { F2(u,v) = (S(u,v)-P).Dvs(u,v) }
 Si on note Duus, Dvvs et Duvs, les derivees secondes de S, les derivees de F1
et F2 sont egales a:
  { Duf1(u,v) = Dus(u,v).Dus(u,v) + (S(u,v)-P).Duus(u,v)
              = ||Dus(u,v)|| ** 2 + (S(u,v)-P).Duus(u,v)
  { Dvf1(u,v) = Dvs(u,v).Dus(u,v) + (S(u,v)-P).Duvs(u,v)
  { Duf2(u,v) = Dus(u,v).Dvs(u,v) + (S(u,v)-P).Duvs(u,v) = dF1v(u,v)
  { Dvf2(u,v) = Dvs(u,v).Dvs(u,v) + (S(u,v)-P).Dvvs(u,v)
              = ||Dvs(u,v)|| ** 2 + (S(u,v)-P).Dvvs(u,v)

----------------------------------------------------------------------------*/
//  modified by NIZHNY-EAP Wed Nov 21 17:33:05 2001
//  -- Dus and Dvs normalized, Df modified accordingly (BUC61043)
//=============================================================================
//------------------------------------------------------------------------------
// This method checks if isocurve is punctual (for details see Geom_OffsetSurface
// and Geom_OsculatingSurface
//-------------------------------------------------------------------------------

static Standard_Boolean IsoIsDeg  (const Adaptor3d_Surface& S,
				   const Standard_Real      Param,
				   const GeomAbs_IsoType    IT,
				   const Standard_Real      TolMin,
				   const Standard_Real      TolMax) 
{
    Standard_Real U1=0.,U2=0.,V1=0.,V2=0.,T;
    Standard_Boolean Along = Standard_True;
    U1 = S.FirstUParameter();
    U2 = S.LastUParameter();
    V1 = S.FirstVParameter();
    V2 = S.LastVParameter();
    gp_Vec D1U,D1V;
    gp_Pnt P;
    Standard_Real Step,D1NormMax;
    if (IT == GeomAbs_IsoV) 
    {
      Step = (U2 - U1)/10;
      D1NormMax=0.;
      for (T=U1;T<=U2;T=T+Step) 
      {
        S.D1(T,Param,P,D1U,D1V);
        D1NormMax=Max(D1NormMax,D1U.Magnitude());
      }

      if (D1NormMax >TolMax || D1NormMax < TolMin ) 
           Along = Standard_False;
    }
    else 
    {
      Step = (V2 - V1)/10;
      D1NormMax=0.;
      for (T=V1;T<=V2;T=T+Step) 
      {
	S.D1(Param,T,P,D1U,D1V);
        D1NormMax=Max(D1NormMax,D1V.Magnitude());
      }

      if (D1NormMax >TolMax || D1NormMax < TolMin ) 
           Along = Standard_False;


    }
    return Along;
}


Extrema_FuncExtPS::Extrema_FuncExtPS ()
{
  myPinit = Standard_False;
  mySinit = Standard_False;
  myUIsoIsDeg = Standard_False;
  myVIsoIsDeg = Standard_False;
}
//=============================================================================
Extrema_FuncExtPS::Extrema_FuncExtPS (const gp_Pnt& P,
                                      const Adaptor3d_Surface& S)
{
  myP = P;
  myS = (Adaptor3d_SurfacePtr)&S;
  myUIsoIsDeg = Standard_False;
  myVIsoIsDeg = Standard_False;
  GeomAbs_SurfaceType aSType = S.GetType();
  if(aSType == GeomAbs_BezierSurface ||
     aSType == GeomAbs_BSplineSurface) {
    Standard_Real u1, u2, v1, v2;
    Standard_Real tol = Precision::Confusion();
    u1 = S.FirstUParameter();
    u2 = S.LastUParameter();
    v1 = S.FirstVParameter();
    v2 = S.LastVParameter();
    myUIsoIsDeg = IsoIsDeg(S, u1, GeomAbs_IsoU, 0., tol) ||
                  IsoIsDeg(S, u2, GeomAbs_IsoU, 0., tol);
    myVIsoIsDeg = IsoIsDeg(S, v1, GeomAbs_IsoV, 0., tol) ||
                  IsoIsDeg(S, v2, GeomAbs_IsoV, 0., tol);
  }
  myPinit = Standard_True;
  mySinit = Standard_True;
}

//=============================================================================
void Extrema_FuncExtPS::Initialize(const Adaptor3d_Surface& S)
{
  myS = (Adaptor3d_SurfacePtr)&S;
  myUIsoIsDeg = Standard_False;
  myVIsoIsDeg = Standard_False;
  GeomAbs_SurfaceType aSType = S.GetType();
  if(aSType == GeomAbs_BezierSurface ||
     aSType == GeomAbs_BSplineSurface) {
    Standard_Real u1, u2, v1, v2;
    Standard_Real tol = Precision::Confusion();
    u1 = S.FirstUParameter();
    u2 = S.LastUParameter();
    v1 = S.FirstVParameter();
    v2 = S.LastVParameter();
    myUIsoIsDeg = IsoIsDeg(S, u1, GeomAbs_IsoU, 0., tol) ||
                  IsoIsDeg(S, u2, GeomAbs_IsoU, 0., tol);
    myVIsoIsDeg = IsoIsDeg(S, v1, GeomAbs_IsoV, 0., tol) ||
                  IsoIsDeg(S, v2, GeomAbs_IsoV, 0., tol);
  }
  mySinit = Standard_True;
  myPoint.Clear();
  mySqDist.Clear();
}

//=============================================================================

void Extrema_FuncExtPS::SetPoint(const gp_Pnt& P)
{
  myP = P;
  myPinit = Standard_True;
  myPoint.Clear();
  mySqDist.Clear();
}

//=============================================================================

//=============================================================================

Standard_Integer Extrema_FuncExtPS::NbVariables () const { return 2;}
//=============================================================================

Standard_Integer Extrema_FuncExtPS::NbEquations () const { return 2;}
//=============================================================================

Standard_Boolean Extrema_FuncExtPS::Value (const math_Vector& UV, 
                                           math_Vector& F)
{
  if (!myPinit || !mySinit) Standard_TypeMismatch::Raise();
  myU = UV(1);
  myV = UV(2);
  gp_Vec Dus, Dvs;
  myS->D1(myU,myV,myPs,Dus,Dvs);

  gp_Vec PPs (myP,myPs);
  // EAP
  if(myVIsoIsDeg)
  {
    Standard_Real DusMod = Dus.Magnitude();
    if (DusMod>gp::Resolution() && DusMod<1.)
      Dus.Multiply(1/DusMod);
  }
  if (myUIsoIsDeg) {
    Standard_Real DvsMod = Dvs.Magnitude();
    if (DvsMod>gp::Resolution() && DvsMod<1.) 
      Dvs.Multiply(1/DvsMod);
  }
    
  F(1) = PPs.Dot(Dus);
  F(2) = PPs.Dot(Dvs);

  return Standard_True;
}
//=============================================================================

Standard_Boolean Extrema_FuncExtPS::Derivatives (const math_Vector& UV, 
                                                 math_Matrix& Df)
{
  math_Vector F(1,2);
  return Values(UV,F,Df);
}
//=============================================================================

Standard_Boolean Extrema_FuncExtPS::Values (const math_Vector& UV, 
                                            math_Vector& F,
                                            math_Matrix& Df)
{
  if (!myPinit || !mySinit) Standard_TypeMismatch::Raise();
  myU = UV(1);
  myV = UV(2);
  gp_Vec Dus, Dvs, Duus, Dvvs, Duvs;
  myS->D2(myU,myV,myPs,Dus,Dvs,Duus,Dvvs,Duvs);

  gp_Vec PPs (myP,myPs);
  // EAP
//  Df(1,1) = Dus.SquareMagnitude() + PPs.Dot(Duus);
//  Df(1,2) = Dvs.Dot(Dus) + PPs.Dot(Duvs);
//  Df(2,1) = Df(1,2);
//  Df(2,2) = Dvs.SquareMagnitude() + PPs.Dot(Dvvs);

  // 25/03/02 akm : (OCC231) Further normalization of derivatives for surfaces
  //                with singularities will hence be performed only when modulus
  //                becomes less than 1.0. Thus the continuity will be kept,
  //                and normalization will be switched off 'far' from singularities.
  // 1. V iso
  Standard_Real DusMod2 = Dus.SquareMagnitude();
  if (myVIsoIsDeg)
  {
    Standard_Real DusMod = Sqrt(DusMod2);
    if (DusMod2>gp::Resolution() && DusMod2<1.)
    {
      Dus.Multiply(1/DusMod);
      Df(1,1) = DusMod2 + PPs.Dot( (Duus*DusMod - Dus*(Dus.Dot(Duus)/DusMod)) / DusMod2 );
      Df(1,2) = Dvs.Dot(Dus) + PPs.Dot( (Duvs*DusMod-Dus*(Dus.Dot(Duvs)/DusMod)) / DusMod2 );
    }
    else {
      Df(1,1) = DusMod2 + PPs.Dot(Duus);
      Df(1,2) = Dvs.Dot(Dus) + PPs.Dot(Duvs);
    }
  }
  else {
    Df(1,1) = DusMod2 + PPs.Dot(Duus);
    Df(1,2) = Dvs.Dot(Dus) + PPs.Dot(Duvs);
  }
  // 2. U iso
  Standard_Real DvsMod2 = Dvs.SquareMagnitude();
  if (myUIsoIsDeg)
  {
    Standard_Real DvsMod = Sqrt(DvsMod2);
    if (DvsMod2>gp::Resolution() && DvsMod2<1.)
    {
      Dvs.Multiply(1/DvsMod);
      Df(2,1) = Dvs.Dot(Dus) + PPs.Dot( (Duvs*DvsMod-Dvs*(Dvs.Dot(Duvs)/DvsMod)) / DvsMod2 );
      Df(2,2) = DvsMod2 + PPs.Dot( (Duus*DvsMod - Dus*(Dus.Dot(Duus)/DvsMod)) / DvsMod2 );
    }
    else {
      Df(2,1) = Dvs.Dot(Dus) + PPs.Dot(Duvs);
      Df(2,2) = DvsMod2 + PPs.Dot(Dvvs);
    }
  }
  else {
    Df(2,1) = Dvs.Dot(Dus) + PPs.Dot(Duvs);
    Df(2,2) = DvsMod2 + PPs.Dot(Dvvs);
  }

  F(1) = PPs.Dot(Dus);
  F(2) = PPs.Dot(Dvs);

  return Standard_True;
}
//=============================================================================

Standard_Integer Extrema_FuncExtPS::GetStateNumber ()
{
  if (!myPinit || !mySinit) Standard_TypeMismatch::Raise();
  mySqDist.Append(myPs.SquareDistance(myP));
  myPoint.Append(Extrema_POnSurf(myU,myV,myPs));
  return 0;
}
//=============================================================================

Standard_Integer Extrema_FuncExtPS::NbExt () const
{
  return mySqDist.Length();
}
//=============================================================================

Standard_Real Extrema_FuncExtPS::SquareDistance (const Standard_Integer N) const
{
  if (!myPinit || !mySinit) Standard_TypeMismatch::Raise();
  return mySqDist.Value(N);
}
//=============================================================================

Extrema_POnSurf Extrema_FuncExtPS::Point (const Standard_Integer N) const
{
  if (!myPinit || !mySinit) Standard_TypeMismatch::Raise();
  return myPoint.Value(N);
}
//=============================================================================

//  Modified by skv - Thu Sep 30 15:21:07 2004 OCC593 Begin
Standard_Boolean Extrema_FuncExtPS::HasDegIso() const
{
  return myUIsoIsDeg || myVIsoIsDeg;
}
//  Modified by skv - Thu Sep 30 15:21:07 2004 OCC593 End
