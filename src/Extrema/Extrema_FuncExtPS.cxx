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

Extrema_FuncExtPS::Extrema_FuncExtPS ()
{
  myPinit = Standard_False;
  mySinit = Standard_False;
}

//=============================================================================
Extrema_FuncExtPS::Extrema_FuncExtPS (const gp_Pnt& P,
                                      const Adaptor3d_Surface& S)
{
  myP = P;
  myS = (Adaptor3d_SurfacePtr)&S;
  GeomAbs_SurfaceType aSType = S.GetType();
  myPinit = Standard_True;
  mySinit = Standard_True;
}

//=============================================================================
void Extrema_FuncExtPS::Initialize(const Adaptor3d_Surface& S)
{
  myS = (Adaptor3d_SurfacePtr)&S;
  GeomAbs_SurfaceType aSType = S.GetType();
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

  Df(1,1) = Dus.SquareMagnitude() + PPs.Dot(Duus);
  Df(1,2) = Dvs.Dot(Dus)          + PPs.Dot(Duvs);
  Df(2,1) = Df(1,2);
  Df(2,2) = Dvs.SquareMagnitude() + PPs.Dot(Dvvs);

  // 3. Value
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
