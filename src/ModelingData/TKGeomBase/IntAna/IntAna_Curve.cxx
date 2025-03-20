// Created on: 1992-06-30
// Created by: Laurent BUCHARD
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef OCCT_DEBUG
  #define No_Standard_RangeError
  #define No_Standard_OutOfRange
#endif

//----------------------------------------------------------------------
//-- Differents constructeurs sont proposes qui correspondent aux
//-- polynomes en Z :
//--    A(Sin(Theta),Cos(Theta)) Z**2
//--  + B(Sin(Theta),Cos(Theta)) Z
//--  + C(Sin(Theta),Cos(Theta))
//--
//-- Une Courbe est definie sur un domaine
//--
//-- Value retourne le point de parametre U(Theta),V(Theta)
//--       ou V est la solution du polynome A V**2 + B V + C
//--       (Selon les cas, on prend V+ ou V-)
//--
//-- D1u   calcule le vecteur tangent a la courbe
//--       et retourne le booleen Standard_False si ce calcul ne peut
//--       pas etre mene a bien.
//----------------------------------------------------------------------

#include <algorithm>

#include <ElSLib.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <IntAna_Curve.hxx>
#include <math_DirectPolynomialRoots.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>

//=================================================================================================

IntAna_Curve::IntAna_Curve()
    : Z0Cte(0.0),
      Z0Sin(0.0),
      Z0Cos(0.0),
      Z0SinSin(0.0),
      Z0CosCos(0.0),
      Z0CosSin(0.0),
      Z1Cte(0.0),
      Z1Sin(0.0),
      Z1Cos(0.0),
      Z1SinSin(0.0),
      Z1CosCos(0.0),
      Z1CosSin(0.0),
      Z2Cte(0.0),
      Z2Sin(0.0),
      Z2Cos(0.0),
      Z2SinSin(0.0),
      Z2CosCos(0.0),
      Z2CosSin(0.0),
      TwoCurves(Standard_False),
      TakeZPositive(Standard_False),
      Tolerance(0.0),
      DomainInf(0.0),
      DomainSup(0.0),
      RestrictedInf(Standard_False),
      RestrictedSup(Standard_False),
      firstbounded(Standard_False),
      lastbounded(Standard_False),
      typequadric(GeomAbs_OtherSurface),
      RCyl(0.0),
      Angle(0.0),
      myFirstParameter(0.0),
      myLastParameter(0.0)
{
}

//=======================================================================
// function : SetConeQuadValues
// purpose  : Description de l intersection Cone Quadrique
//=======================================================================
void IntAna_Curve::SetConeQuadValues(const gp_Cone&         Cone,
                                     const Standard_Real    Qxx,
                                     const Standard_Real    Qyy,
                                     const Standard_Real    Qzz,
                                     const Standard_Real    Qxy,
                                     const Standard_Real    Qxz,
                                     const Standard_Real    Qyz,
                                     const Standard_Real    Qx,
                                     const Standard_Real    Qy,
                                     const Standard_Real    Qz,
                                     const Standard_Real    Q1,
                                     const Standard_Real    TOL,
                                     const Standard_Real    DomInf,
                                     const Standard_Real    DomSup,
                                     const Standard_Boolean twocurves,
                                     const Standard_Boolean takezpositive)
{

  Ax3  = Cone.Position();
  RCyl = Cone.RefRadius();

  Angle                      = Cone.SemiAngle();
  Standard_Real UnSurTgAngle = 1.0 / (Tan(Cone.SemiAngle()));

  typequadric = GeomAbs_Cone;

  TwoCurves     = twocurves;     //-- deux  Z pour un meme parametre
  TakeZPositive = takezpositive; //-- Prendre sur la courbe le Z Positif
                                 //--   ( -B + Sqrt()) et non (-B - Sqrt())

  Z0Cte    = Q1;  //-- Attention On a    Z?Cos Cos(t)
  Z0Sin    = 0.0; //-- et Non          2 Z?Cos Cos(t) !!!
  Z0Cos    = 0.0; //-- Ce pour tous les Parametres
  Z0CosCos = 0.0; //--  ie pas de Coefficient 2
  Z0SinSin = 0.0; //--     devant les termes CS C S
  Z0CosSin = 0.0;

  Z1Cte    = 2.0 * (UnSurTgAngle)*Qz;
  Z1Sin    = Qy + Qy;
  Z1Cos    = Qx + Qx;
  Z1CosCos = 0.0;
  Z1SinSin = 0.0;
  Z1CosSin = 0.0;

  Z2Cte    = Qzz * UnSurTgAngle * UnSurTgAngle;
  Z2Sin    = (UnSurTgAngle + UnSurTgAngle) * Qyz;
  Z2Cos    = (UnSurTgAngle + UnSurTgAngle) * Qxz;
  Z2CosCos = Qxx;
  Z2SinSin = Qyy;
  Z2CosSin = Qxy;

  Tolerance = TOL;
  DomainInf = DomInf;
  DomainSup = DomSup;

  RestrictedInf = RestrictedSup = Standard_True; //-- Le Domaine est Borne
  firstbounded = lastbounded = Standard_False;

  myFirstParameter = DomainInf;
  myLastParameter  = (TwoCurves) ? DomainSup + DomainSup - DomainInf : DomainSup;
}

//=======================================================================
// function : SetCylinderQuadValues
// purpose  : Description de l intersection Cylindre Quadrique
//=======================================================================
void IntAna_Curve::SetCylinderQuadValues(const gp_Cylinder&     Cyl,
                                         const Standard_Real    Qxx,
                                         const Standard_Real    Qyy,
                                         const Standard_Real    Qzz,
                                         const Standard_Real    Qxy,
                                         const Standard_Real    Qxz,
                                         const Standard_Real    Qyz,
                                         const Standard_Real    Qx,
                                         const Standard_Real    Qy,
                                         const Standard_Real    Qz,
                                         const Standard_Real    Q1,
                                         const Standard_Real    TOL,
                                         const Standard_Real    DomInf,
                                         const Standard_Real    DomSup,
                                         const Standard_Boolean twocurves,
                                         const Standard_Boolean takezpositive)
{

  Ax3         = Cyl.Position();
  RCyl        = Cyl.Radius();
  typequadric = GeomAbs_Cylinder;

  TwoCurves              = twocurves;     //-- deux  Z pour un meme parametre
  TakeZPositive          = takezpositive; //-- Prendre sur la courbe le Z Positif
  Standard_Real RCylmul2 = RCyl + RCyl;   //--   ( -B + Sqrt())

  Z0Cte    = Q1;
  Z0Sin    = RCylmul2 * Qy;
  Z0Cos    = RCylmul2 * Qx;
  Z0CosCos = Qxx * RCyl * RCyl;
  Z0SinSin = Qyy * RCyl * RCyl;
  Z0CosSin = RCyl * RCyl * Qxy;

  Z1Cte    = Qz + Qz;
  Z1Sin    = RCylmul2 * Qyz;
  Z1Cos    = RCylmul2 * Qxz;
  Z1CosCos = 0.0;
  Z1SinSin = 0.0;
  Z1CosSin = 0.0;

  Z2Cte    = Qzz;
  Z2Sin    = 0.0;
  Z2Cos    = 0.0;
  Z2CosCos = 0.0;
  Z2SinSin = 0.0;
  Z2CosSin = 0.0;

  Tolerance = TOL;
  DomainInf = DomInf;
  DomainSup = DomSup;

  RestrictedInf = RestrictedSup = Standard_True;
  firstbounded = lastbounded = Standard_False;

  myFirstParameter = DomainInf;
  myLastParameter  = (TwoCurves) ? DomainSup + DomainSup - DomainInf : DomainSup;
}

//=================================================================================================

Standard_Boolean IntAna_Curve::IsOpen() const
{
  return (RestrictedInf && RestrictedSup);
}

//=================================================================================================

void IntAna_Curve::Domain(Standard_Real& theFirst, Standard_Real& theLast) const
{
  if (RestrictedInf && RestrictedSup)
  {
    theFirst = myFirstParameter;
    theLast  = myLastParameter;
  }
  else
  {
    throw Standard_DomainError("IntAna_Curve::Domain");
  }
}

//=================================================================================================

Standard_Boolean IntAna_Curve::IsConstant() const
{
  //-- ???  Pas facile de decider a la seule vue des Param.
  return (Standard_False);
}

//=================================================================================================

Standard_Boolean IntAna_Curve::IsFirstOpen() const
{
  return (firstbounded);
}

//=================================================================================================

Standard_Boolean IntAna_Curve::IsLastOpen() const
{
  return (lastbounded);
}

//=================================================================================================

void IntAna_Curve::SetIsFirstOpen(const Standard_Boolean Flag)
{
  firstbounded = Flag;
}

//=================================================================================================

void IntAna_Curve::SetIsLastOpen(const Standard_Boolean Flag)
{
  lastbounded = Flag;
}

//=================================================================================================

void IntAna_Curve::InternalUVValue(const Standard_Real theta,
                                   Standard_Real&      Param1,
                                   Standard_Real&      Param2,
                                   Standard_Real&      A,
                                   Standard_Real&      B,
                                   Standard_Real&      C,
                                   Standard_Real&      cost,
                                   Standard_Real&      sint,
                                   Standard_Real&      SigneSqrtDis) const
{
  const Standard_Real aRelTolp = 1.0 + Epsilon(1.0), aRelTolm = 1.0 - Epsilon(1.0);

  // Infinitesimal step of increasing curve parameter. See comment below.
  const Standard_Real aDT = 100.0 * Epsilon(DomainSup + DomainSup - DomainInf);

  Standard_Real    Theta          = theta;
  Standard_Boolean SecondSolution = Standard_False;

  if ((Theta < DomainInf * aRelTolm) || ((Theta > DomainSup * aRelTolp) && (!TwoCurves))
      || (Theta > (DomainSup + DomainSup - DomainInf) * aRelTolp))
  {
    SigneSqrtDis = 0.;
    throw Standard_DomainError("IntAna_Curve::Domain");
  }

  if (Abs(Theta - DomainSup) < aDT)
  {
    // Point of Null-discriminant.
    Theta = DomainSup;
  }
  else if (Theta > DomainSup)
  {
    Theta          = DomainSup + DomainSup - Theta;
    SecondSolution = Standard_True;
  }

  Param1 = Theta;

  if (!TwoCurves)
  {
    SecondSolution = TakeZPositive;
  }
  //
  cost                       = Cos(Theta);
  sint                       = Sin(Theta);
  const Standard_Real aSin2t = Sin(Theta + Theta);
  const Standard_Real aCos2t = Cos(Theta + Theta);

  A =
    Z2Cte + sint * (Z2Sin + sint * Z2SinSin) + cost * (Z2Cos + cost * Z2CosCos) + Z2CosSin * aSin2t;

  const Standard_Real aDA =
    cost * Z2Sin - sint * Z2Cos + aSin2t * (Z2SinSin - Z2CosCos) + aCos2t * (Z2CosSin * Z2CosSin);

  B =
    Z1Cte + sint * (Z1Sin + sint * Z1SinSin) + cost * (Z1Cos + cost * Z1CosCos) + Z1CosSin * aSin2t;

  const Standard_Real aDB =
    Z1Sin * cost - Z1Cos * sint + aSin2t * (Z1SinSin - Z1CosCos) + aCos2t * (Z1CosSin + Z1CosSin);

  C =
    Z0Cte + sint * (Z0Sin + sint * Z0SinSin) + cost * (Z0Cos + cost * Z0CosCos) + Z0CosSin * aSin2t;

  const Standard_Real aDC =
    Z0Sin * cost - Z0Cos * sint + aSin2t * (Z0SinSin - Z0CosCos) + aCos2t * (Z0CosSin + Z0CosSin);

  Standard_Real aDiscriminant = B * B - 4.0 * A * C;

  // We consider that infinitesimal dt = aDT.
  // Error of discriminant computation is equal to
  // (d(Disc)/dt)*dt, where 1st derivative d(Disc)/dt = 2*B*aDB - 4*(A*aDC + C*aDA).

  const Standard_Real aTolD = 2.0 * aDT * Abs(B * aDB - 2.0 * (A * aDC + C * aDA));

  if (aDiscriminant < aTolD)
    aDiscriminant = 0.0;

  if (Abs(A) <= Precision::PConfusion())
  {
    if (Abs(B) <= Precision::PConfusion())
    {
      Param2 = 0.0;
    }
    else
    {
      Param2 = -C / B;
    }
  }
  else
  {
    SigneSqrtDis = (SecondSolution) ? Sqrt(aDiscriminant) : -Sqrt(aDiscriminant);
    Param2       = (-B + SigneSqrtDis) / (A + A);
  }
}

//=================================================================================================

gp_Pnt IntAna_Curve::Value(const Standard_Real theta)
{
  Standard_Real A, B, C, U, V, sint, cost, SigneSqrtDis;
  //
  A            = 0.0;
  B            = 0.0;
  C            = 0.0;
  U            = 0.0;
  V            = 0.0;
  sint         = 0.0;
  cost         = 0.0;
  SigneSqrtDis = 0.0;
  InternalUVValue(theta, U, V, A, B, C, cost, sint, SigneSqrtDis);
  //-- checked the parameter U and Raises Domain Error if Error
  return (InternalValue(U, V));
}

//=================================================================================================

Standard_Boolean IntAna_Curve::D1u(const Standard_Real theta, gp_Pnt& Pt, gp_Vec& Vec)
{
  //-- Pour detecter le cas ou le calcul est impossible
  Standard_Real A, B, C, U, V, sint, cost, SigneSqrtDis;
  A    = 0.0;
  B    = 0.0;
  C    = 0.0;
  U    = 0.0;
  V    = 0.0;
  sint = 0.0;
  cost = 0.0;
  //
  InternalUVValue(theta, U, V, A, B, C, cost, sint, SigneSqrtDis);
  //
  Pt = Value(theta);
  if (Abs(A) < 1.0e-7 || Abs(SigneSqrtDis) < 1.0e-10)
    return (Standard_False);

  //-- Approximation de la derivee (mieux que le calcul mathematique!)
  Standard_Real dtheta = (DomainSup - DomainInf) * 1.0e-6;
  Standard_Real theta2 = theta + dtheta;
  if ((theta2 < DomainInf) || ((theta2 > DomainSup) && (!TwoCurves))
      || (theta2 > (DomainSup + DomainSup - DomainInf + 1.0e-14)))
  {
    dtheta = -dtheta;
    theta2 = theta + dtheta;
  }
  gp_Pnt P2 = Value(theta2);
  dtheta    = 1.0 / dtheta;
  Vec.SetCoord((P2.X() - Pt.X()) * dtheta, (P2.Y() - Pt.Y()) * dtheta, (P2.Z() - Pt.Z()) * dtheta);

  return (Standard_True);
}

//=======================================================================
// function : FindParameter
// purpose  : Projects P to the ALine. Returns the list of parameters as a results
//            of projection.
//           Sometimes aline can be self-intersected line (see bug #29807 where
//            ALine goes through the cone apex).
//=======================================================================
void IntAna_Curve::FindParameter(const gp_Pnt& theP, TColStd_ListOfReal& theParams) const
{
  const Standard_Real aPIpPI = M_PI + M_PI, anEpsAng = 1.e-8,
                      InternalPrecision =
                        1.e-8; // precision of internal algorithm of values computation
  // clang-format off
  constexpr Standard_Real aSqTolPrecision = Precision::SquareConfusion(); //for boundary points to check their coincidence with others
  // clang-format on

  Standard_Real aTheta = 0.0;
  //
  switch (typequadric)
  {
    case GeomAbs_Cylinder: {
      Standard_Real aZ;
      ElSLib::CylinderParameters(Ax3, RCyl, theP, aTheta, aZ);
    }
    break;

    case GeomAbs_Cone: {
      Standard_Real aZ;
      ElSLib::ConeParameters(Ax3, RCyl, Angle, theP, aTheta, aZ);
    }
    break;

    default:
      return;
  }
  //
  if (!firstbounded && (DomainInf > aTheta) && ((DomainInf - aTheta) <= anEpsAng))
  {
    aTheta = DomainInf;
  }
  else if (!lastbounded && (aTheta > DomainSup) && ((aTheta - DomainSup) <= anEpsAng))
  {
    aTheta = DomainSup;
  }
  //
  if (aTheta < DomainInf)
  {
    aTheta = aTheta + aPIpPI;
  }
  else if (aTheta > DomainSup)
  {
    aTheta = aTheta - aPIpPI;
  }

  const Standard_Integer aMaxPar          = 5;
  Standard_Real          aParams[aMaxPar] = {DomainInf,
                                             DomainSup,
                                             aTheta,
                                    (TwoCurves) ? DomainSup + DomainSup - aTheta : RealLast(),
                                    (TwoCurves) ? DomainSup + DomainSup - DomainInf : RealLast()};

  std::sort(aParams, aParams + aMaxPar - 1);

  for (Standard_Integer i = 0; i < aMaxPar; i++)
  {
    if (aParams[i] > myLastParameter)
      break;

    if (aParams[i] < myFirstParameter)
      continue;

    if (i && (aParams[i] - aParams[i - 1]) < Precision::PConfusion())
      continue;

    Standard_Real U = 0.0, V = 0.0, A = 0.0, B = 0.0, C = 0.0, sint = 0.0, cost = 0.0,
                  SigneSqrtDis = 0.0;
    InternalUVValue(aParams[i], U, V, A, B, C, cost, sint, SigneSqrtDis);
    const gp_Pnt aP(InternalValue(U, V));

    Standard_Real aSqTol;
    if (aParams[i] == aTheta || (TwoCurves && aParams[i] == DomainSup + DomainSup - aTheta))
      aSqTol = InternalPrecision;
    else
      aSqTol = aSqTolPrecision;

    if (aP.SquareDistance(theP) < aSqTol)
    {
      theParams.Append(aParams[i]);
    }
  }
}

//=================================================================================================

gp_Pnt IntAna_Curve::InternalValue(const Standard_Real U, const Standard_Real _V) const
{
  //-- std::cout<<" ["<<U<<","<<V<<"]";
  Standard_Real V = _V;
  if (V > 100000.0)
  {
    V = 100000.0;
  }
  if (V < -100000.0)
  {
    V = -100000.0;
  }

  switch (typequadric)
  {
    case GeomAbs_Cone: {
      //------------------------------------------------
      //-- Parametrage : X = V * Cos(U)              ---
      //--               Y = V * Sin(U)              ---
      //--               Z = (V-RCyl) / Tan(SemiAngle)--
      //------------------------------------------------
      //-- Angle Vaut Cone.SemiAngle()
      return (ElSLib::ConeValue(U, (V - RCyl) / Sin(Angle), Ax3, RCyl, Angle));
    }
    break;

    case GeomAbs_Cylinder:
      return (ElSLib::CylinderValue(U, V, Ax3, RCyl));
    case GeomAbs_Sphere:
      return (ElSLib::SphereValue(U, V, Ax3, RCyl));
    default:
      return (gp_Pnt(0.0, 0.0, 0.0));
  }
}

//
//=================================================================================================

void IntAna_Curve::SetDomain(const Standard_Real theFirst, const Standard_Real theLast)
{
  if (theLast <= theFirst)
  {
    throw Standard_DomainError("IntAna_Curve::Domain");
  }
  //
  myFirstParameter = theFirst;
  myLastParameter  = theLast;
}
