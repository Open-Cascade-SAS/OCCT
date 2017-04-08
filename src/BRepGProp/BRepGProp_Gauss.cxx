// Copyright (c) 2008-2015 OPEN CASCADE SAS
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

#include <math.hxx>
#include <Precision.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Standard_Assert.hxx>
#include <BRepGProp_Face.hxx>
#include <BRepGProp_Domain.hxx>
#include <BRepGProp_Gauss.hxx>

// If the following is defined the error of algorithm is calculated by static moments
#define IS_MIN_DIM

namespace
{
  // Minimal value of interval's range for computation | minimal value of "dim" | ... 
  static const Standard_Real EPS_PARAM          = 1.e-12;
  static const Standard_Real EPS_DIM            = 1.e-30;
  static const Standard_Real ERROR_ALGEBR_RATIO = 2.0 / 3.0;

  // Maximum of GaussPoints on a subinterval and maximum of subintervals
  static const Standard_Integer GPM        = math::GaussPointsMax();
  static const Standard_Integer SUBS_POWER = 32;
  static const Standard_Integer SM         = SUBS_POWER * GPM + 1;

  // Auxiliary inner functions to perform arithmetic operations.
  static Standard_Real Add(const Standard_Real theA, const Standard_Real theB)
  {
    return theA + theB;
  }

  static Standard_Real AddInf(const Standard_Real theA, const Standard_Real theB)
  {
    if (Precision::IsPositiveInfinite(theA))
    {
      if (Precision::IsNegativeInfinite(theB))
        return 0.0;
      else
        return Precision::Infinite();
    }

    if (Precision::IsPositiveInfinite(theB))
    {
      if (Precision::IsNegativeInfinite(theA))
        return 0.0;
      else
        return Precision::Infinite();
    }

    if (Precision::IsNegativeInfinite(theA))
    {
      if (Precision::IsPositiveInfinite(theB))
        return 0.0;
      else
        return -Precision::Infinite();
    }

    if (Precision::IsNegativeInfinite(theB))
    {
      if (Precision::IsPositiveInfinite(theA))
        return 0.0;
      else
        return -Precision::Infinite();
    }

    return theA + theB;
  }

  static Standard_Real Mult(const Standard_Real theA, const Standard_Real theB)
  {
    return theA * theB;
  }

  static Standard_Real MultInf(const Standard_Real theA, const Standard_Real theB)
  {
    if ((theA == 0.0) || (theB == 0.0))  //strictly zerro (without any tolerances)
      return 0.0;

    if (Precision::IsPositiveInfinite(theA))
    {
      if (theB < 0.0)
        return -Precision::Infinite();
      else
        return Precision::Infinite();
    }

    if (Precision::IsPositiveInfinite(theB))
    {
      if (theA < 0.0)
        return -Precision::Infinite();
      else
        return Precision::Infinite();
    }

    if (Precision::IsNegativeInfinite(theA))
    {
      if (theB < 0.0)
        return +Precision::Infinite();
      else
        return -Precision::Infinite();
    }

    if (Precision::IsNegativeInfinite(theB))
    {
      if (theA < 0.0)
        return +Precision::Infinite();
      else
        return -Precision::Infinite();
    }

    return theA * theB;
  }
}

//=======================================================================
//function : BRepGProp_Gauss::Inert::Inert
//purpose  : Constructor
//=======================================================================
BRepGProp_Gauss::Inertia::Inertia()
: Mass(0.0),
  Ix  (0.0),
  Iy  (0.0),
  Iz  (0.0),
  Ixx (0.0),
  Iyy (0.0),
  Izz (0.0),
  Ixy (0.0),
  Ixz (0.0),
  Iyz (0.0)
{
}

//=======================================================================
//function : Inertia::Reset
//purpose  : Zeroes all values.
//=======================================================================
void BRepGProp_Gauss::Inertia::Reset()
{
  memset(reinterpret_cast<void*>(this), 0, sizeof(BRepGProp_Gauss::Inertia));
}

//=======================================================================
//function : BRepGProp_Gauss
//purpose  : Constructor
//=======================================================================
BRepGProp_Gauss::BRepGProp_Gauss(const BRepGProp_GaussType theType)
: myType(theType)
{
  add  = (::Add );
  mult = (::Mult);
}

//=======================================================================
//function : MaxSubs
//purpose  : 
//=======================================================================
Standard_Integer BRepGProp_Gauss::MaxSubs(const Standard_Integer theN,
                                          const Standard_Integer theCoeff)
{
  return IntegerLast() / theCoeff < theN ?
         IntegerLast() : theN * theCoeff + 1;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void BRepGProp_Gauss::Init(NCollection_Handle<math_Vector>&         theOutVec,
                           const Standard_Real    theValue,
                           const Standard_Integer theFirst,
                           const Standard_Integer theLast)
{
  if(theLast - theFirst == 0)
  {
    theOutVec->Init(theValue);
  }
  else
  {
    for (Standard_Integer i = theFirst; i <= theLast; ++i)
      theOutVec->Value(i) = theValue;
  }
}

//=======================================================================
//function : InitMass
//purpose  : 
//=======================================================================
void BRepGProp_Gauss::InitMass(const Standard_Real    theValue,
                               const Standard_Integer theFirst,
                               const Standard_Integer theLast,
                               InertiaArray&          theArray)
{
  if (theArray.IsNull())
    return;

  Standard_Integer aFirst = theFirst;
  Standard_Integer aLast  = theLast;

  if (theLast - theFirst == 0)
  {
    aFirst = theArray->Lower();
    aLast  = theArray->Upper();
  }

  for (Standard_Integer i = aFirst; i <= aLast; ++i)
      theArray->ChangeValue(i).Mass = theValue;
}

//=======================================================================
//function : FillIntervalBounds
//purpose  : 
//=======================================================================
Standard_Integer BRepGProp_Gauss::FillIntervalBounds(
  const Standard_Real         theA,
  const Standard_Real         theB,
  const TColStd_Array1OfReal& theKnots,
  const Standard_Integer      theNumSubs,
  InertiaArray&               theInerts,
  NCollection_Handle<math_Vector>&              theParam1,
  NCollection_Handle<math_Vector>&              theParam2,
  NCollection_Handle<math_Vector>&              theError,
  NCollection_Handle<math_Vector>&              theCommonError)
{
  const Standard_Integer aSize =
    Max(theKnots.Upper(), MaxSubs(theKnots.Upper() - 1, theNumSubs));

  if (aSize - 1 > theParam1->Upper())
  {
    theInerts = new NCollection_Array1<Inertia>(1, aSize);
    theParam1 = new math_Vector(1, aSize);
    theParam2 = new math_Vector(1, aSize);
    theError  = new math_Vector(1, aSize, 0.0);

    if (theCommonError.IsNull() == Standard_False)
      theCommonError = new math_Vector(1, aSize, 0.0);
  }

  Standard_Integer j = 1, k = 1;
  theParam1->Value(j++) = theA;

  const Standard_Integer aLength = theKnots.Upper();
  for (Standard_Integer i = 1; i <= aLength; ++i)
  {
    const Standard_Real kn = theKnots(i);
    if (theA < kn)
    {
      if (kn < theB) 
      {
        theParam1->Value(j++) = kn;
        theParam2->Value(k++) = kn; 
      }
      else
        break;
    }
  }

  theParam2->Value(k) = theB;
  return k;
}



//=======================================================================
//function : computeVInertiaOfElementaryPart
//purpose  : 
//=======================================================================
void BRepGProp_Gauss::computeVInertiaOfElementaryPart(
  const gp_Pnt&             thePoint,
  const gp_Vec&             theNormal,
  const gp_Pnt&             theLocation,
  const Standard_Real       theWeight,
  const Standard_Real       theCoeff[],
  const Standard_Boolean    theIsByPoint,
  BRepGProp_Gauss::Inertia& theOutInertia)
{
  Standard_Real x = thePoint.X() - theLocation.X();
  Standard_Real y = thePoint.Y() - theLocation.Y();
  Standard_Real z = thePoint.Z() - theLocation.Z();

  const Standard_Real xn = theNormal.X() * theWeight;
  const Standard_Real yn = theNormal.Y() * theWeight;
  const Standard_Real zn = theNormal.Z() * theWeight;

  if (theIsByPoint)
  {
    /////////////////////                        ///////////////////////
    //    OFV code     //                        //    Initial code   //
    /////////////////////                        ///////////////////////
    // modified by APO

    Standard_Real dv = x * xn + y * yn + z * zn; //xyz  = x * y * z;
    theOutInertia.Mass += dv / 3.0;              //Ixyi += zn * xyz; 
    theOutInertia.Ix   += 0.25 * x * dv;         //Iyzi += xn * xyz;
    theOutInertia.Iy   += 0.25 * y * dv;         //Ixzi += yn * xyz; 
    theOutInertia.Iz   += 0.25 * z * dv;         //xi = x * x * x * xn / 3.0;
    x -= theCoeff[0];                            //yi = y * y * y * yn / 3.0;
    y -= theCoeff[1];                            //zi = z * z * z * zn / 3.0;
    z -= theCoeff[2];                            //Ixxi += (yi + zi);
    dv *= 0.2;                                   //Iyyi += (xi + zi);
    theOutInertia.Ixy -= x * y * dv;             //Izzi += (xi + yi);
    theOutInertia.Iyz -= y * z * dv;             //x -= Coeff[0];
    theOutInertia.Ixz -= x * z * dv;             //y -= Coeff[1];
    x *= x;                                      //z -= Coeff[2];
    y *= y;                                      //dv = x * xn + y * yn + z * zn;
    z *= z;                                      //dvi +=  dv; 
    theOutInertia.Ixx += (y + z) * dv;           //Ixi += x * dv;
    theOutInertia.Iyy += (x + z) * dv;           //Iyi += y * dv;
    theOutInertia.Izz += (x + y) * dv;           //Izi += z * dv;
  }
  else
  { // By plane
    const Standard_Real s = xn * theCoeff[0] + yn * theCoeff[1] + zn * theCoeff[2];

    Standard_Real d1  = theCoeff[0] * x + theCoeff[1] * y + theCoeff[2] * z - theCoeff[3];
    Standard_Real d2  = d1 * d1;
    Standard_Real d3  = d1 * d2 / 3.0;
    Standard_Real dv  = s  * d1;

    theOutInertia.Mass += dv;
    theOutInertia.Ix   += (x - (theCoeff[0] * d1 * 0.5)) * dv;
    theOutInertia.Iy   += (y - (theCoeff[1] * d1 * 0.5)) * dv;
    theOutInertia.Iz   += (z - (theCoeff[2] * d1 * 0.5)) * dv;

    const Standard_Real px = x - theCoeff[0] * d1;
    const Standard_Real py = y - theCoeff[1] * d1;
    const Standard_Real pz = z - theCoeff[2] * d1;

    x = px * px * d1 + px * theCoeff[0] * d2 + theCoeff[0] * theCoeff[0] * d3;
    y = py * py * d1 + py * theCoeff[1] * d2 + theCoeff[1] * theCoeff[1] * d3;
    z = pz * pz * d1 + pz * theCoeff[2] * d2 + theCoeff[2] * theCoeff[2] * d3;

    theOutInertia.Ixx += (y + z) * s;
    theOutInertia.Iyy += (x + z) * s;
    theOutInertia.Izz += (x + y) * s;

    d2 *= 0.5;
    x = (py * pz * d1) + (py * theCoeff[2] * d2) + (pz * theCoeff[1] * d2) + (theCoeff[1] * theCoeff[2] * d3);
    y = (px * pz * d1) + (pz * theCoeff[0] * d2) + (px * theCoeff[2] * d2) + (theCoeff[0] * theCoeff[2] * d3);
    z = (px * py * d1) + (px * theCoeff[1] * d2) + (py * theCoeff[0] * d2) + (theCoeff[0] * theCoeff[1] * d3);

    theOutInertia.Ixy -= z * s;
    theOutInertia.Iyz -= x * s;
    theOutInertia.Ixz -= y * s;
  }
}

//=======================================================================
//function : computeSInertiaOfElementaryPart
//purpose  : 
//=======================================================================
void BRepGProp_Gauss::computeSInertiaOfElementaryPart(
  const gp_Pnt&             thePoint,
  const gp_Vec&             theNormal,
  const gp_Pnt&             theLocation,
  const Standard_Real       theWeight,
  BRepGProp_Gauss::Inertia& theOutInertia)
{
  // ds - Jacobien (x, y, z) -> (u, v) = ||n||
  const Standard_Real ds = mult(theNormal.Magnitude(), theWeight);
  const Standard_Real x = add(thePoint.X(), -theLocation.X());
  const Standard_Real y = add(thePoint.Y(), -theLocation.Y());
  const Standard_Real z = add(thePoint.Z(), -theLocation.Z());

  theOutInertia.Mass = add(theOutInertia.Mass, ds);

  const Standard_Real XdS = mult(x, ds);
  const Standard_Real YdS = mult(y, ds);
  const Standard_Real ZdS = mult(z, ds);

  theOutInertia.Ix  = add(theOutInertia.Ix,  XdS);
  theOutInertia.Iy  = add(theOutInertia.Iy,  YdS);
  theOutInertia.Iz  = add(theOutInertia.Iz,  ZdS);
  theOutInertia.Ixy = add(theOutInertia.Ixy, mult(x, YdS));
  theOutInertia.Iyz = add(theOutInertia.Iyz, mult(y, ZdS));
  theOutInertia.Ixz = add(theOutInertia.Ixz, mult(x, ZdS));

  const Standard_Real XXdS = mult(x, XdS);
  const Standard_Real YYdS = mult(y, YdS);
  const Standard_Real ZZdS = mult(z, ZdS);

  theOutInertia.Ixx = add(theOutInertia.Ixx, add(YYdS, ZZdS));
  theOutInertia.Iyy = add(theOutInertia.Iyy, add(XXdS, ZZdS));
  theOutInertia.Izz = add(theOutInertia.Izz, add(XXdS, YYdS));
}

//=======================================================================
//function : checkBounds
//purpose  : 
//=======================================================================
void BRepGProp_Gauss::checkBounds(const Standard_Real theU1,
                                  const Standard_Real theU2,
                                  const Standard_Real theV1,
                                  const Standard_Real theV2)
{
  if (Precision::IsInfinite(theU1) || Precision::IsInfinite(theU2) ||
      Precision::IsInfinite(theV1) || Precision::IsInfinite(theV2))
  {
    add  = (::AddInf);
    mult = (::MultInf);
  }
}

//=======================================================================
//function : addAndRestoreInertia
//purpose  : 
//=======================================================================
void BRepGProp_Gauss::addAndRestoreInertia(
  const BRepGProp_Gauss::Inertia& theInInertia,
  BRepGProp_Gauss::Inertia&       theOutInertia)
{
  theOutInertia.Mass = add(theOutInertia.Mass, theInInertia.Mass);
  theOutInertia.Ix   = add(theOutInertia.Ix,   theInInertia.Ix);
  theOutInertia.Iy   = add(theOutInertia.Iy,   theInInertia.Iy);
  theOutInertia.Iz   = add(theOutInertia.Iz,   theInInertia.Iz);
  theOutInertia.Ixx  = add(theOutInertia.Ixx,  theInInertia.Ixx);
  theOutInertia.Iyy  = add(theOutInertia.Iyy,  theInInertia.Iyy);
  theOutInertia.Izz  = add(theOutInertia.Izz,  theInInertia.Izz);
  theOutInertia.Ixy  = add(theOutInertia.Ixy,  theInInertia.Ixy);
  theOutInertia.Ixz  = add(theOutInertia.Ixz,  theInInertia.Ixz);
  theOutInertia.Iyz  = add(theOutInertia.Iyz,  theInInertia.Iyz);
}

//=======================================================================
//function : multAndRestoreInertia
//purpose  : 
//=======================================================================
void BRepGProp_Gauss::multAndRestoreInertia(
  const Standard_Real       theValue,
  BRepGProp_Gauss::Inertia& theInOutInertia)
{
  theInOutInertia.Mass = mult(theInOutInertia.Mass, theValue);
  theInOutInertia.Ix   = mult(theInOutInertia.Ix,   theValue);
  theInOutInertia.Iy   = mult(theInOutInertia.Iy,   theValue);
  theInOutInertia.Iz   = mult(theInOutInertia.Iz,   theValue);
  theInOutInertia.Ixx  = mult(theInOutInertia.Ixx,  theValue);
  theInOutInertia.Iyy  = mult(theInOutInertia.Iyy,  theValue);
  theInOutInertia.Izz  = mult(theInOutInertia.Izz,  theValue);
  theInOutInertia.Ixy  = mult(theInOutInertia.Ixy,  theValue);
  theInOutInertia.Ixz  = mult(theInOutInertia.Ixz,  theValue);
  theInOutInertia.Iyz  = mult(theInOutInertia.Iyz,  theValue);
}

//=======================================================================
//function : convert
//purpose  : 
//=======================================================================
void BRepGProp_Gauss::convert(const BRepGProp_Gauss::Inertia& theInertia,
                              gp_Pnt&                         theOutGravityCenter,
                              gp_Mat&                         theOutMatrixOfInertia,
                              Standard_Real&                  theOutMass)
{
  if (Abs(theInertia.Mass) >= EPS_DIM)
  {
    const Standard_Real anInvMass = 1.0 / theInertia.Mass;
    theOutGravityCenter.SetX(theInertia.Ix * anInvMass);
    theOutGravityCenter.SetY(theInertia.Iy * anInvMass);
    theOutGravityCenter.SetZ(theInertia.Iz * anInvMass);

    theOutMass = theInertia.Mass;
  }
  else
  {
    theOutMass = 0.0;
    theOutGravityCenter.SetCoord(0.0, 0.0, 0.0);
  }

  theOutMatrixOfInertia = gp_Mat(
    gp_XYZ ( theInertia.Ixx, -theInertia.Ixy, -theInertia.Ixz),
    gp_XYZ (-theInertia.Ixy,  theInertia.Iyy, -theInertia.Iyz),
    gp_XYZ (-theInertia.Ixz, -theInertia.Iyz,  theInertia.Izz));
}

//=======================================================================
//function : convert
//purpose  : 
//=======================================================================
void BRepGProp_Gauss::convert(const BRepGProp_Gauss::Inertia& theInertia,
                              const Standard_Real             theCoeff[],
                              const Standard_Boolean          theIsByPoint,
                              gp_Pnt&                         theOutGravityCenter,
                              gp_Mat&                         theOutMatrixOfInertia,
                              Standard_Real&                  theOutMass)
{
  convert(theInertia, theOutGravityCenter, theOutMatrixOfInertia, theOutMass);
  if (Abs(theInertia.Mass) >= EPS_DIM && theIsByPoint)
  {
    const Standard_Real anInvMass = 1.0 / theInertia.Mass;
    if (theIsByPoint == Standard_True)
    {
      theOutGravityCenter.SetX(theCoeff[0] + theInertia.Ix * anInvMass);
      theOutGravityCenter.SetY(theCoeff[1] + theInertia.Iy * anInvMass);
      theOutGravityCenter.SetZ(theCoeff[2] + theInertia.Iz * anInvMass);
    }
    else
    {
      theOutGravityCenter.SetX(theInertia.Ix * anInvMass);
      theOutGravityCenter.SetY(theInertia.Iy * anInvMass);
      theOutGravityCenter.SetZ(theInertia.Iz * anInvMass);
    }

    theOutMass = theInertia.Mass;
  }
  else
  {
    theOutMass = 0.0;
    theOutGravityCenter.SetCoord(0.0, 0.0, 0.0);
  }

  theOutMatrixOfInertia = gp_Mat(
    gp_XYZ (theInertia.Ixx, theInertia.Ixy, theInertia.Ixz),
    gp_XYZ (theInertia.Ixy, theInertia.Iyy, theInertia.Iyz),
    gp_XYZ (theInertia.Ixz, theInertia.Iyz, theInertia.Izz));
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
Standard_Real BRepGProp_Gauss::Compute(
  BRepGProp_Face&        theSurface,
  BRepGProp_Domain&      theDomain,
  const gp_Pnt&          theLocation,
  const Standard_Real    theEps,
  const Standard_Real    theCoeff[],
  const Standard_Boolean theIsByPoint,
  Standard_Real&         theOutMass,
  gp_Pnt&                theOutGravityCenter,
  gp_Mat&                theOutInertia)
{
  const Standard_Boolean isErrorCalculation  =
    ( 0.0 > theEps || theEps < 0.001 ) ? Standard_True : Standard_False;
  const Standard_Boolean isVerifyComputation =
    ( 0.0 < theEps && theEps < 0.001 ) ? Standard_True : Standard_False;

  Standard_Real anEpsilon= Abs(theEps);

  BRepGProp_Gauss::Inertia anInertia;
  InertiaArray anInertiaL = new NCollection_Array1<Inertia>(1, SM);
  InertiaArray anInertiaU = new NCollection_Array1<Inertia>(1, SM);

  // Prepare Gauss points and weights
  NCollection_Handle<math_Vector> LGaussP[2];
  NCollection_Handle<math_Vector> LGaussW[2];
  NCollection_Handle<math_Vector> UGaussP[2];
  NCollection_Handle<math_Vector> UGaussW[2];

  const Standard_Integer aNbGaussPoint =
    RealToInt(Ceiling(ERROR_ALGEBR_RATIO * GPM));

  LGaussP[0] = new math_Vector(1, GPM);
  LGaussP[1] = new math_Vector(1, aNbGaussPoint);
  LGaussW[0] = new math_Vector(1, GPM);
  LGaussW[1] = new math_Vector(1, aNbGaussPoint);

  UGaussP[0] = new math_Vector(1, GPM);
  UGaussP[1] = new math_Vector(1, aNbGaussPoint);
  UGaussW[0] = new math_Vector(1, GPM);
  UGaussW[1] = new math_Vector(1, aNbGaussPoint);

  NCollection_Handle<math_Vector> L1 = new math_Vector(1, SM);
  NCollection_Handle<math_Vector> L2 = new math_Vector(1, SM);
  NCollection_Handle<math_Vector> U1 = new math_Vector(1, SM);
  NCollection_Handle<math_Vector> U2 = new math_Vector(1, SM);

  NCollection_Handle<math_Vector> ErrL  = new math_Vector(1, SM, 0.0);
  NCollection_Handle<math_Vector> ErrU  = new math_Vector(1, SM, 0.0);
  NCollection_Handle<math_Vector> ErrUL = new math_Vector(1, SM, 0.0);

  // Face parametrization in U and V direction
  Standard_Real BV1, BV2, BU1, BU2;
  theSurface.Bounds(BU1, BU2, BV1, BV2);
  checkBounds(BU1, BU2, BV1, BV2);

  //
  const Standard_Integer NumSubs = SUBS_POWER;
  const TopoDS_Face& aF = theSurface.GetFace(); 
  TopoDS_Iterator aWIter(aF);
  const Standard_Boolean isNaturalRestriction = !aWIter.More(); //theSurface.NaturalRestriction();

  Standard_Real CIx, CIy, CIz, CIxy, CIxz, CIyz;
  Standard_Real CDim[2], CIxx[2], CIyy[2], CIzz[2];

  // Boundary curve parametrization
  Standard_Real u1 = BU1, u2, l1, l2, lm, lr, l, v;

  // On the boundary curve u-v
  gp_Pnt2d Puv;
  gp_Vec2d Vuv;
  Standard_Real Dul;  // Dul = Du / Dl

  Standard_Integer iLS, iLSubEnd, iGL, iGLEnd, NbLGaussP[2], LRange[2], iL, kL, kLEnd, IL, JL;
  Standard_Integer i, iUSubEnd, NbUGaussP[2], URange[2], kU, kUEnd, IU, JU;
  Standard_Integer UMaxSubs, LMaxSubs;

  Standard_Real ErrorU, ErrorL, ErrorLMax = 0.0, Eps = 0.0, EpsL = 0.0, EpsU = 0.0;
  iGLEnd = isErrorCalculation ? 2 : 1;

  NbUGaussP[0] = theSurface.SIntOrder(anEpsilon);
  NbUGaussP[1] = RealToInt( Ceiling(ERROR_ALGEBR_RATIO * NbUGaussP[0]) );

  math::GaussPoints (NbUGaussP[0], *UGaussP[0]);
  math::GaussWeights(NbUGaussP[0], *UGaussW[0]);
  math::GaussPoints (NbUGaussP[1], *UGaussP[1]);
  math::GaussWeights(NbUGaussP[1], *UGaussW[1]);

  const Standard_Integer aNbUSubs = theSurface.SUIntSubs();
  TColStd_Array1OfReal UKnots(1, aNbUSubs + 1);
  theSurface.UKnots(UKnots);

  while (isNaturalRestriction || theDomain.More())
  {
    if (isNaturalRestriction)
    {
      NbLGaussP[0] = Min(2 * NbUGaussP[0], math::GaussPointsMax());
    }
    else
    {
      theSurface.Load(theDomain.Value());
      NbLGaussP[0] = theSurface.LIntOrder(anEpsilon);
    }

    NbLGaussP[1] = RealToInt( Ceiling(ERROR_ALGEBR_RATIO * NbLGaussP[0]) );

    math::GaussPoints (NbLGaussP[0], *LGaussP[0]);
    math::GaussWeights(NbLGaussP[0], *LGaussW[0]);
    math::GaussPoints (NbLGaussP[1], *LGaussP[1]);
    math::GaussWeights(NbLGaussP[1], *LGaussW[1]);

    const Standard_Integer aNbLSubs =
      isNaturalRestriction ? theSurface.SVIntSubs(): theSurface.LIntSubs();
    TColStd_Array1OfReal LKnots(1, aNbLSubs + 1);

    if (isNaturalRestriction)
    {
      theSurface.VKnots(LKnots);
      l1 = BV1;
      l2 = BV2;
    }
    else
    {
      theSurface.LKnots(LKnots);
      l1 = theSurface.FirstParameter();
      l2 = theSurface.LastParameter();
    }
    ErrorL = 0.0;
    kLEnd = 1; JL = 0;

    if (Abs(l2 - l1) > EPS_PARAM)
    {
      iLSubEnd = FillIntervalBounds(l1, l2, LKnots, NumSubs, anInertiaL, L1, L2, ErrL, ErrUL);
      LMaxSubs = BRepGProp_Gauss::MaxSubs(iLSubEnd);

      if (LMaxSubs > SM)
      {
        LMaxSubs = SM;
      }

      BRepGProp_Gauss::InitMass(0.0, 1, LMaxSubs, anInertiaL);
      BRepGProp_Gauss::Init(ErrL,  0.0, 1, LMaxSubs);
      BRepGProp_Gauss::Init(ErrUL, 0.0, 1, LMaxSubs);

      do // while: L
      {
        if (++JL > iLSubEnd)
        {
          LRange[0] = IL = ErrL->Max();
          LRange[1] = JL;
          L1->Value(JL) = (L1->Value(IL) + L2->Value(IL)) * 0.5;
          L2->Value(JL) = L2->Value(IL);
          L2->Value(IL) = L1->Value(JL);
        }
        else
        {
          LRange[0] = IL = JL;
        }

        if (JL == LMaxSubs || Abs(L2->Value(JL) - L1->Value(JL)) < EPS_PARAM)
        {
          if (kLEnd == 1)
          {
            anInertiaL->ChangeValue(JL).Reset();
            ErrL->Value(JL) = 0.0;
          }
          else
          {
            --JL;
            EpsL = ErrorL;
            Eps = EpsL / 0.9;
            break;
          }
        }
        else
        {
          for (kL = 0; kL < kLEnd; kL++)
          {
            iLS = LRange[kL];
            lm = 0.5 * (L2->Value(iLS) + L1->Value(iLS));
            lr = 0.5 * (L2->Value(iLS) - L1->Value(iLS));

            CIx = CIy = CIz = CIxy = CIxz = CIyz = 0.0;

            for (iGL = 0; iGL < iGLEnd; ++iGL)
            {
              CDim[iGL] = CIxx[iGL] = CIyy[iGL] = CIzz[iGL] = 0.0;

              for (iL = 1; iL <= NbLGaussP[iGL]; iL++)
              {
                l = lm + lr * LGaussP[iGL]->Value(iL);
                if (isNaturalRestriction)
                {
                  v = l;
                  u2 = BU2;
                  Dul = LGaussW[iGL]->Value(iL);
                }
                else
                {
                  theSurface.D12d (l, Puv, Vuv);
                  Dul = Vuv.Y() * LGaussW[iGL]->Value(iL);  // Dul = Du / Dl

                  if (Abs(Dul) < EPS_PARAM)
                    continue;

                  v  = Puv.Y();
                  u2 = Puv.X();

                  // Check on cause out off bounds of value current parameter
                  if (v < BV1)
                    v = BV1;
                  else if (v > BV2)
                    v = BV2;

                  if (u2 < BU1)
                    u2 = BU1;
                  else if (u2 > BU2)
                    u2 = BU2;
                }

                ErrUL->Value(iLS) = 0.0;
                kUEnd = 1;
                JU = 0;

                if (Abs(u2 - u1) < EPS_PARAM)
                  continue;

                NCollection_Handle<math_Vector> aDummy;
                iUSubEnd = FillIntervalBounds(u1, u2, UKnots, NumSubs, anInertiaU, U1, U2, ErrU, aDummy);
                UMaxSubs = BRepGProp_Gauss::MaxSubs(iUSubEnd);

                if (UMaxSubs > SM)
                  UMaxSubs = SM;

                BRepGProp_Gauss::InitMass(0.0, 1, UMaxSubs, anInertiaU);
                BRepGProp_Gauss::Init(ErrU, 0.0, 1, UMaxSubs);
                ErrorU = 0.0;

                do
                {//while: U
                  if (++JU > iUSubEnd)
                  {
                    URange[0] = IU = ErrU->Max();
                    URange[1] = JU;

                    U1->Value(JU) = (U1->Value(IU) + U2->Value(IU)) * 0.5;
                    U2->Value(JU) = U2->Value(IU);
                    U2->Value(IU) = U1->Value(JU);
                  }
                  else
                    URange[0] = IU = JU;

                  if (JU == UMaxSubs || Abs(U2->Value(JU) - U1->Value(JU)) < EPS_PARAM)
                    if (kUEnd == 1)
                    {
                      ErrU->Value(JU) = 0.0;
                      anInertiaU->ChangeValue(JU).Reset();
                    }
                    else
                    {
                      --JU;
                      EpsU = ErrorU;
                      Eps  = 10. * EpsU * Abs((u2 - u1) * Dul);
                      EpsL = 0.9 * Eps;
                      break;
                    }
                  else
                  {
                    gp_Pnt aPoint;
                    gp_Vec aNormal;

                    for (kU = 0; kU < kUEnd; ++kU)
                    {
                      BRepGProp_Gauss::Inertia aLocal[2];

                      Standard_Integer iUS = URange[kU];
                      const Standard_Integer aLength = iGLEnd - iGL;

                      const Standard_Real um = 0.5 * (U2->Value(iUS) + U1->Value(iUS));
                      const Standard_Real ur = 0.5 * (U2->Value(iUS) - U1->Value(iUS));

                      for (Standard_Integer iGU = 0; iGU < aLength; ++iGU)
                      {
                        for (Standard_Integer iU = 1; iU <= NbUGaussP[iGU]; ++iU)
                        {
                          Standard_Real w = UGaussW[iGU]->Value(iU);
                          const Standard_Real u = um + ur * UGaussP[iGU]->Value(iU);

                          theSurface.Normal(u, v, aPoint, aNormal);

                          if (myType == Vinert)
                          {
                            computeVInertiaOfElementaryPart(
                              aPoint, aNormal, theLocation, w, theCoeff, theIsByPoint, aLocal[iGU]);
                          }
                          else
                          {
                            if (iGU > 0)
                              aLocal[iGU].Mass += (w * aNormal.Magnitude());
                            else
                            {
                              computeSInertiaOfElementaryPart(
                                aPoint, aNormal, theLocation, w, aLocal[iGU]);
                            }
                          }
                        }
                      }

                      BRepGProp_Gauss::Inertia& anUI =
                        anInertiaU->ChangeValue(iUS);

                      anUI.Mass = mult(aLocal[0].Mass, ur);

                      if (myType == Vinert)
                      {
                        anUI.Ixx = mult(aLocal[0].Ixx, ur);
                        anUI.Iyy = mult(aLocal[0].Iyy, ur);
                        anUI.Izz = mult(aLocal[0].Izz, ur);
                      }

                      if (iGL > 0)
                        continue;

                      Standard_Real aDMass = Abs(aLocal[1].Mass - aLocal[0].Mass);

                      if (myType == Vinert)
                      {
                        aLocal[1].Ixx = Abs(aLocal[1].Ixx - aLocal[0].Ixx);
                        aLocal[1].Iyy = Abs(aLocal[1].Iyy - aLocal[0].Iyy);
                        aLocal[1].Izz = Abs(aLocal[1].Izz - aLocal[0].Izz);

                        anUI.Ix = mult(aLocal[0].Ix, ur);
                        anUI.Iy = mult(aLocal[0].Iy, ur);
                        anUI.Iz = mult(aLocal[0].Iz, ur);

                        anUI.Ixy = mult(aLocal[0].Ixy, ur);
                        anUI.Ixz = mult(aLocal[0].Ixz, ur);
                        anUI.Iyz = mult(aLocal[0].Iyz, ur);

                        #ifndef IS_MIN_DIM
                        aDMass = aLocal[1].Ixx + aLocal[1].Iyy + aLocal[1].Izz;
                        #endif

                        ErrU->Value(iUS) = mult(aDMass, ur);
                      }
                      else
                      {
                        anUI.Ix  = mult(aLocal[0].Ix,  ur);
                        anUI.Iy  = mult(aLocal[0].Iy,  ur);
                        anUI.Iz  = mult(aLocal[0].Iz,  ur);
                        anUI.Ixx = mult(aLocal[0].Ixx, ur);
                        anUI.Iyy = mult(aLocal[0].Iyy, ur);
                        anUI.Izz = mult(aLocal[0].Izz, ur);
                        anUI.Ixy = mult(aLocal[0].Ixy, ur);
                        anUI.Ixz = mult(aLocal[0].Ixz, ur);
                        anUI.Iyz = mult(aLocal[0].Iyz, ur);

                        ErrU->Value(iUS) = mult(aDMass, ur);
                      }
                    }
                  }

                  if (JU == iUSubEnd)
                  {
                    kUEnd = 2;
                    ErrorU = ErrU->Value(ErrU->Max());
                  }
                } while ( (ErrorU - EpsU > 0.0 && EpsU != 0.0) || kUEnd == 1 );

                for (i = 1; i <= JU; ++i)
                {
                  const BRepGProp_Gauss::Inertia& anIU =
                    anInertiaU->Value(i);

                  CDim[iGL] = add(CDim[iGL], mult(anIU.Mass, Dul));
                  CIxx[iGL] = add(CIxx[iGL], mult(anIU.Ixx,  Dul));
                  CIyy[iGL] = add(CIyy[iGL], mult(anIU.Iyy,  Dul));
                  CIzz[iGL] = add(CIzz[iGL], mult(anIU.Izz,  Dul));
                }

                if (iGL > 0)
                  continue;

                ErrUL->Value(iLS) = ErrorU * Abs((u2 - u1) * Dul);

                for (i = 1; i <= JU; ++i)
                {
                  const BRepGProp_Gauss::Inertia& anIU =
                    anInertiaU->Value(i);

                  CIx = add(CIx, mult(anIU.Ix, Dul));
                  CIy = add(CIy, mult(anIU.Iy, Dul));
                  CIz = add(CIz, mult(anIU.Iz, Dul));

                  CIxy = add(CIxy, mult(anIU.Ixy, Dul));
                  CIxz = add(CIxz, mult(anIU.Ixz, Dul));
                  CIyz = add(CIyz, mult(anIU.Iyz, Dul));
                }
              }//for: iL
            }//for: iGL

            BRepGProp_Gauss::Inertia& aLI = anInertiaL->ChangeValue(iLS);

            aLI.Mass = mult(CDim[0], lr);
            aLI.Ixx  = mult(CIxx[0], lr);
            aLI.Iyy  = mult(CIyy[0], lr);
            aLI.Izz  = mult(CIzz[0], lr);

            if (iGLEnd == 2)
            {
              Standard_Real aSubDim = Abs(CDim[1] - CDim[0]);

              if (myType == Vinert)
              {
                ErrorU = ErrUL->Value(iLS);

                CIxx[1] = Abs(CIxx[1] - CIxx[0]);
                CIyy[1] = Abs(CIyy[1] - CIyy[0]);
                CIzz[1] = Abs(CIzz[1] - CIzz[0]);

                #ifndef IS_MIN_DIM
                aSubDim = CIxx[1] + CIyy[1] + CIzz[1];
                #endif

                ErrL->Value(iLS) = add(mult(aSubDim, lr), ErrorU);
              }
              else
              {
                ErrL->Value(iLS) = add(mult(aSubDim, lr), ErrUL->Value(iLS));
              }
            }

            aLI.Ix = mult(CIx, lr);
            aLI.Iy = mult(CIy, lr);
            aLI.Iz = mult(CIz, lr);

            aLI.Ixy = mult(CIxy, lr);
            aLI.Ixz = mult(CIxz, lr);
            aLI.Iyz = mult(CIyz, lr);
          }//for: (kL)iLS
        }

        // Calculate/correct epsilon of computation by current value of dim
        // That is need for not spend time for
        if (JL == iLSubEnd)
        {
          kLEnd = 2;

          Standard_Real DDim = 0.0;
          for (i = 1; i <= JL; ++i)
            DDim += anInertiaL->Value(i).Mass;

          #ifndef IS_MIN_DIM
          {
            if (myType == Vinert)
            {
              Standard_Real DIxx = 0.0, DIyy = 0.0, DIzz = 0.0;
              for (i = 1; i <= JL; ++i)
              {
                const BRepGProp_Gauss::Inertia& aLocalL =
                  anInertiaL->Value(i);

                DIxx += aLocalL.Ixx;
                DIyy += aLocalL.Iyy;
                DIzz += aLocalL.Izz;
              }

              DDim = Abs(DIxx) + Abs(DIyy) + Abs(DIzz);
            }
          }
          #endif

          DDim = Abs(DDim * anEpsilon);

          if (DDim > Eps)
          {
            Eps  = DDim;
            EpsL = 0.9 * Eps;
          }
        }
        if (kLEnd == 2)
        {
          ErrorL = ErrL->Value(ErrL->Max());
        }
      } while ( (ErrorL - EpsL > 0.0 && isVerifyComputation) || kLEnd == 1 );

      for ( i = 1; i <= JL; i++ )
      {
        addAndRestoreInertia(anInertiaL->Value(i), anInertia);
      }

      ErrorLMax = Max(ErrorLMax, ErrorL);
    }

    if (isNaturalRestriction)
      break;

    theDomain.Next();
  }

  if (myType == Vinert)
    convert(anInertia, theCoeff, theIsByPoint, theOutGravityCenter, theOutInertia, theOutMass);
  else
    convert(anInertia, theOutGravityCenter, theOutInertia, theOutMass);

  if (iGLEnd == 2)
  {
    if (theOutMass != 0.0)
    {
      Eps = ErrorLMax / Abs(theOutMass);

      #ifndef IS_MIN_DIM
      {
        if (myType == Vinert)
          Eps = ErrorLMax / (Abs(anInertia.Ixx) +
                             Abs(anInertia.Iyy) +
                             Abs(anInertia.Izz));
      }
      #endif
    }
    else
    {
      Eps = 0.0;
    }
  }
  else
  {
    Eps = anEpsilon;
  }

  return Eps;
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
Standard_Real BRepGProp_Gauss::Compute(BRepGProp_Face&     theSurface,
                                       BRepGProp_Domain&   theDomain,
                                       const gp_Pnt&       theLocation,
                                       const Standard_Real theEps,
                                       Standard_Real&      theOutMass,
                                       gp_Pnt&             theOutGravityCenter,
                                       gp_Mat&             theOutInertia)
{
  Standard_ASSERT_RAISE(myType == Sinert, "BRepGProp_Gauss: Incorrect type");

  return Compute(theSurface,
                 theDomain,
                 theLocation,
                 theEps,
                 NULL,
                 Standard_True,
                 theOutMass,
                 theOutGravityCenter,
                 theOutInertia);
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
void BRepGProp_Gauss::Compute(BRepGProp_Face&   theSurface,
                              BRepGProp_Domain& theDomain,
                              const gp_Pnt&     theLocation,
                              Standard_Real&    theOutMass,
                              gp_Pnt&           theOutGravityCenter,
                              gp_Mat&           theOutInertia)
{
  Standard_ASSERT_RAISE(myType == Sinert, "BRepGProp_Gauss: Incorrect type");

  Standard_Real u1, u2, v1, v2;
  theSurface.Bounds (u1, u2, v1, v2);
  checkBounds(u1, u2, v1, v2);

  const Standard_Integer NbUGaussgp_Pnts =
    Min(theSurface.UIntegrationOrder(), math::GaussPointsMax());

  const Standard_Integer NbVGaussgp_Pnts =
    Min(theSurface.VIntegrationOrder(), math::GaussPointsMax());

  const Standard_Integer NbGaussgp_Pnts =
    Max(NbUGaussgp_Pnts, NbVGaussgp_Pnts);

  // Number of Gauss points for the integration on the face
  math_Vector GaussSPV (1, NbGaussgp_Pnts);
  math_Vector GaussSWV (1, NbGaussgp_Pnts);
  math::GaussPoints (NbGaussgp_Pnts, GaussSPV);
  math::GaussWeights(NbGaussgp_Pnts, GaussSWV);

  BRepGProp_Gauss::Inertia anInertia;
  while (theDomain.More())
  {
    theSurface.Load(theDomain.Value());

    Standard_Integer NbCGaussgp_Pnts =
      Min(theSurface.IntegrationOrder(), math::GaussPointsMax());

    NbCGaussgp_Pnts = Max(NbCGaussgp_Pnts, NbGaussgp_Pnts);

    math_Vector GaussCP(1, NbCGaussgp_Pnts);
    math_Vector GaussCW(1, NbCGaussgp_Pnts);
    math::GaussPoints (NbCGaussgp_Pnts, GaussCP);
    math::GaussWeights(NbCGaussgp_Pnts, GaussCW);

    
    const Standard_Real l1 = theSurface.FirstParameter();
    const Standard_Real l2 = theSurface.LastParameter ();
    const Standard_Real lm = 0.5 * (l2 + l1);
    const Standard_Real lr = 0.5 * (l2 - l1);

    BRepGProp_Gauss::Inertia aCInertia;
    for (Standard_Integer i = 1; i <= NbCGaussgp_Pnts; ++i)
    {
      const Standard_Real l = lm + lr * GaussCP(i);

      gp_Pnt2d Puv;
      gp_Vec2d Vuv;
      theSurface.D12d(l, Puv, Vuv);

      const Standard_Real v = Puv.Y();
      u2  = Puv.X();

      const Standard_Real Dul = Vuv.Y() * GaussCW(i);
      const Standard_Real um  = 0.5 * (u2 + u1);
      const Standard_Real ur  = 0.5 * (u2 - u1);

      BRepGProp_Gauss::Inertia aLocalInertia;
      for (Standard_Integer j = 1; j <= NbGaussgp_Pnts; ++j)
      {
        const Standard_Real u = add(um, mult(ur, GaussSPV(j)));
        const Standard_Real aWeight = Dul * GaussSWV(j);

        gp_Pnt aPoint;
        gp_Vec aNormal;
        theSurface.Normal (u, v, aPoint, aNormal);

        computeSInertiaOfElementaryPart(aPoint, aNormal, theLocation, aWeight, aLocalInertia);
      }

      multAndRestoreInertia(ur, aLocalInertia);
      addAndRestoreInertia (aLocalInertia, aCInertia);
    }

    multAndRestoreInertia(lr, aCInertia);
    addAndRestoreInertia (aCInertia, anInertia);

    theDomain.Next();
  }

  convert(anInertia, theOutGravityCenter, theOutInertia, theOutMass);
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
void BRepGProp_Gauss::Compute(BRepGProp_Face&         theSurface,
                              BRepGProp_Domain&       theDomain,
                              const gp_Pnt&           theLocation,
                              const Standard_Real     theCoeff[],
                              const Standard_Boolean  theIsByPoint,
                              Standard_Real&          theOutMass,
                              gp_Pnt&                 theOutGravityCenter,
                              gp_Mat&                 theOutInertia)
{
  Standard_ASSERT_RAISE(myType == Vinert, "BRepGProp_Gauss: Incorrect type");

  Standard_Real u1, v1, u2, v2;
  theSurface.Bounds (u1, u2, v1, v2);
  checkBounds(u1, u2, v1, v2);

  Standard_Real _u2 = u2;  //OCC104

  BRepGProp_Gauss::Inertia anInertia;
  while (theDomain.More())
  {
    theSurface.Load(theDomain.Value());

    const Standard_Integer aVNbCGaussgp_Pnts =
      theSurface.VIntegrationOrder();

    const Standard_Integer aNbGaussgp_Pnts =
      Min( Max(theSurface.IntegrationOrder(), aVNbCGaussgp_Pnts), math::GaussPointsMax() );

    math_Vector GaussP(1, aNbGaussgp_Pnts);
    math_Vector GaussW(1, aNbGaussgp_Pnts);
    math::GaussPoints (aNbGaussgp_Pnts, GaussP);
    math::GaussWeights(aNbGaussgp_Pnts, GaussW);

    const Standard_Real l1 = theSurface.FirstParameter();
    const Standard_Real l2 = theSurface.LastParameter();
    const Standard_Real lm = 0.5 * (l2 + l1);
    const Standard_Real lr = 0.5 * (l2 - l1);

    BRepGProp_Gauss::Inertia aCInertia;
    for (Standard_Integer i = 1; i <= aNbGaussgp_Pnts; ++i)
    {
      const Standard_Real l = lm + lr * GaussP(i);

      gp_Pnt2d Puv;
      gp_Vec2d Vuv;

      theSurface.D12d(l, Puv, Vuv);

      u2  = Puv.X();
      u2 = Min( Max(u1, u2), _u2 ); // OCC104
      const Standard_Real v = Min(Max(Puv.Y(), v1), v2);

      const Standard_Real Dul = Vuv.Y() * GaussW(i);
      const Standard_Real um  = 0.5 * (u2 + u1);
      const Standard_Real ur  = 0.5 * (u2 - u1);

      BRepGProp_Gauss::Inertia aLocalInertia;
      for (Standard_Integer j = 1; j <= aNbGaussgp_Pnts; ++j)
      {
        const Standard_Real u = um + ur * GaussP(j);
        const Standard_Real aWeight = Dul * GaussW(j);

        gp_Pnt aPoint;
        gp_Vec aNormal;

        theSurface.Normal(u, v, aPoint, aNormal);

        computeVInertiaOfElementaryPart(
          aPoint,
          aNormal,
          theLocation,
          aWeight,
          theCoeff,
          theIsByPoint,
          aLocalInertia);
      }

      multAndRestoreInertia(ur, aLocalInertia);
      addAndRestoreInertia (aLocalInertia, aCInertia);
    }

    multAndRestoreInertia(lr,        aCInertia);
    addAndRestoreInertia (aCInertia, anInertia);

    theDomain.Next();
  }

  convert(anInertia, theCoeff, theIsByPoint, theOutGravityCenter, theOutInertia, theOutMass);
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
void BRepGProp_Gauss::Compute(const BRepGProp_Face&  theSurface,
                              const gp_Pnt&          theLocation,
                              const Standard_Real    theCoeff[],
                              const Standard_Boolean theIsByPoint,
                              Standard_Real&         theOutMass,
                              gp_Pnt&                theOutGravityCenter,
                              gp_Mat&                theOutInertia)
{
  Standard_Real LowerU, UpperU, LowerV, UpperV;
  theSurface.Bounds(LowerU, UpperU, LowerV, UpperV);
  checkBounds(LowerU, UpperU, LowerV, UpperV);

  const Standard_Integer UOrder =
    Min(theSurface.UIntegrationOrder(), math::GaussPointsMax());
  const Standard_Integer VOrder =
    Min(theSurface.VIntegrationOrder(), math::GaussPointsMax());

  // Gauss points and weights
  math_Vector GaussPU(1, UOrder);
  math_Vector GaussWU(1, UOrder);
  math_Vector GaussPV(1, VOrder);
  math_Vector GaussWV(1, VOrder);

  math::GaussPoints (UOrder, GaussPU);
  math::GaussWeights(UOrder, GaussWU);
  math::GaussPoints (VOrder, GaussPV);
  math::GaussWeights(VOrder, GaussWV);

  const Standard_Real um = 0.5 * add(UpperU,  LowerU);
  const Standard_Real vm = 0.5 * add(UpperV,  LowerV);
  Standard_Real ur = 0.5 * add(UpperU, -LowerU);
  Standard_Real vr = 0.5 * add(UpperV, -LowerV);

  gp_Pnt aPoint;
  gp_Vec aNormal;

  BRepGProp_Gauss::Inertia anInertia;
  for (Standard_Integer j = 1; j <= VOrder; ++j)
  {
    BRepGProp_Gauss::Inertia anInertiaOfElementaryPart;
    const Standard_Real v = add(vm, mult(vr, GaussPV(j)));

    for (Standard_Integer i = 1; i <= UOrder; ++i)
    {
      const Standard_Real aWeight = GaussWU(i);
      const Standard_Real u = add(um, mult(ur, GaussPU (i)));
      theSurface.Normal(u, v, aPoint, aNormal);

      if (myType == Vinert)
      {
        computeVInertiaOfElementaryPart(
          aPoint,
          aNormal,
          theLocation,
          aWeight,
          theCoeff,
          theIsByPoint,
          anInertiaOfElementaryPart);
      }
      else // Sinert
      {
        computeSInertiaOfElementaryPart(
          aPoint,
          aNormal,
          theLocation,
          aWeight,
          anInertiaOfElementaryPart);
      }
    }

    multAndRestoreInertia(GaussWV(j), anInertiaOfElementaryPart);
    addAndRestoreInertia (anInertiaOfElementaryPart,  anInertia);
  }
  vr            = mult(vr, ur);
  anInertia.Ixx = mult(vr, anInertia.Ixx);
  anInertia.Iyy = mult(vr, anInertia.Iyy);
  anInertia.Izz = mult(vr, anInertia.Izz);
  anInertia.Ixy = mult(vr, anInertia.Ixy);
  anInertia.Ixz = mult(vr, anInertia.Ixz);
  anInertia.Iyz = mult(vr, anInertia.Iyz);

  if (myType == Vinert)
  {
    convert(anInertia, theCoeff, theIsByPoint, theOutGravityCenter, theOutInertia, theOutMass);
  }
  else // Sinert
  {
    convert(anInertia, theOutGravityCenter, theOutInertia, theOutMass);
  }

  theOutMass *= vr;
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
void BRepGProp_Gauss::Compute(const BRepGProp_Face& theSurface,
                              const gp_Pnt&         theLocation,
                              Standard_Real&        theOutMass,
                              gp_Pnt&               theOutGravityCenter,
                              gp_Mat&               theOutInertia)
{
  Standard_ASSERT_RAISE(myType == Sinert, "BRepGProp_Gauss: Incorrect type");

  Compute(theSurface,
          theLocation,
          NULL,
          Standard_True,
          theOutMass,
          theOutGravityCenter,
          theOutInertia);
}
