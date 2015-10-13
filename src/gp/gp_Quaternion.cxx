// Created on: 2010-05-11
// Created by: Kirill GAVRILOV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
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

// Note: implementation is based on free samples from
// http://www.gamedev.ru/code/articles/?id=4215&page=3
// and maths found in Wikipedia and elsewhere

#include <gp_Quaternion.hxx>
#include <gp_Vec.hxx>
#include <gp_Mat.hxx>

//=======================================================================
//function : IsEqual
//purpose  : 
//=======================================================================

Standard_Boolean gp_Quaternion::IsEqual (const gp_Quaternion& theOther) const
{
  if (this == &theOther)
    return Standard_True;
  return Abs (x - theOther.x) <= gp::Resolution() &&
         Abs (y - theOther.y) <= gp::Resolution() &&
         Abs (z - theOther.z) <= gp::Resolution() &&
         Abs (w - theOther.w) <= gp::Resolution();
}

//=======================================================================
//function : SetRotation
//purpose  : 
//=======================================================================

void gp_Quaternion::SetRotation (const gp_Vec& theVecFrom, const gp_Vec& theVecTo)
{
  gp_Vec aVecCross (theVecFrom.Crossed (theVecTo));
  Set (aVecCross.X(), aVecCross.Y(), aVecCross.Z(), theVecFrom.Dot (theVecTo));
  Normalize();    // if "from" or "to" not unit, normalize quat
  w += 1.0;       // reducing angle to halfangle
  if (w <= gp::Resolution())  // angle close to PI
  {
    if ((theVecFrom.Z() * theVecFrom.Z()) > (theVecFrom.X() * theVecFrom.X()))
      Set (           0.0,  theVecFrom.Z(), -theVecFrom.Y(), w); // theVecFrom * gp_Vec(1,0,0)
    else 
      Set (theVecFrom.Y(), -theVecFrom.X(),             0.0, w); // theVecFrom * gp_Vec(0,0,1)
  }
  Normalize(); 
}

//=======================================================================
//function : SetRotation
//purpose  : 
//=======================================================================

void gp_Quaternion::SetRotation (const gp_Vec& theVecFrom, const gp_Vec& theVecTo, const gp_Vec& theHelpCrossVec)
{
  gp_Vec aVecCross (theVecFrom.Crossed (theVecTo));
  Set (aVecCross.X(), aVecCross.Y(), aVecCross.Z(), theVecFrom.Dot (theVecTo));
  Normalize();    // if "from" or "to" not unit, normalize quat
  w += 1.0;       // reducing angle to halfangle
  if (w <= gp::Resolution())  // angle close to PI
  {
    gp_Vec theAxis = theVecFrom.Crossed (theHelpCrossVec);
    Set (theAxis.X(), theAxis.Y(), theAxis.Z(), w);
  }
  Normalize(); 
}

//=======================================================================
//function : SetVectorAndAngle
//purpose  : 
//=======================================================================

void gp_Quaternion::SetVectorAndAngle (const gp_Vec& theAxis, const Standard_Real theAngle)
{
  gp_Vec anAxis = theAxis.Normalized();
  Standard_Real anAngleHalf = 0.5 * theAngle;
  Standard_Real sin_a = Sin (anAngleHalf);
  Set (anAxis.X() * sin_a, anAxis.Y() * sin_a, anAxis.Z() * sin_a, Cos (anAngleHalf));
}

//=======================================================================
//function : GetVectorAndAngle
//purpose  : 
//=======================================================================

void gp_Quaternion::GetVectorAndAngle (gp_Vec& theAxis, Standard_Real& theAngle) const
{
  Standard_Real vl = Sqrt (x * x + y * y + z * z);
  if (vl > gp::Resolution())
  {
    Standard_Real ivl = 1.0 / vl;
    theAxis.SetCoord (x * ivl, y * ivl, z * ivl);
    if (w < 0.0)
    {
      theAngle = 2.0 * ATan2 (-vl, -w); // [-PI,  0]
    }
    else
    {
      theAngle = 2.0 * ATan2 ( vl,  w); // [  0, PI]
    }
  }
  else
  {
    theAxis.SetCoord (0.0, 0.0, 1.0);
    theAngle = 0.0;
  }
}

//=======================================================================
//function : SetMatrix
//purpose  : 
//=======================================================================

void gp_Quaternion::SetMatrix (const gp_Mat& theMat)
{
  Standard_Real tr = theMat (1, 1) + theMat (2, 2) + theMat(3, 3); // trace of martix
  if (tr > 0.0)
  { // if trace positive than "w" is biggest component
    Set (theMat (3, 2) - theMat (2, 3),
	 theMat (1, 3) - theMat (3, 1),
	 theMat (2, 1) - theMat (1, 2),
	 tr + 1.0);
    Scale (0.5 / Sqrt (w)); // "w" contain the "norm * 4"
  }
  else if ((theMat (1, 1) > theMat (2, 2)) && (theMat (1, 1) > theMat (3, 3)))
  { // Some of vector components is bigger
    Set (1.0 + theMat (1, 1) - theMat (2, 2) - theMat (3, 3),
	 theMat (1, 2) + theMat (2, 1),
	 theMat (1, 3) + theMat (3, 1),
	 theMat (3, 2) - theMat (2, 3));
    Scale (0.5 / Sqrt (x));
  }
  else if (theMat (2, 2) > theMat (3, 3))
  {
    Set (theMat (1, 2) + theMat (2, 1),
	 1.0 + theMat (2, 2) - theMat (1, 1) - theMat (3, 3),
	 theMat (2, 3) + theMat (3, 2),
	 theMat (1, 3) - theMat (3, 1));
    Scale (0.5 / Sqrt (y));
  }
  else
  {
    Set (theMat (1, 3) + theMat (3, 1),
	 theMat (2, 3) + theMat (3, 2),
	 1.0 + theMat (3, 3) - theMat (1, 1) - theMat (2, 2),
	 theMat (2, 1) - theMat (1, 2));
    Scale (0.5 / Sqrt (z));
  }
}

//=======================================================================
//function : GetMatrix
//purpose  : 
//=======================================================================

gp_Mat gp_Quaternion::GetMatrix () const
{
  Standard_Real wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
  Standard_Real s  = 2.0 / SquareNorm(); 
  x2 = x * s;    y2 = y * s;    z2 = z * s;
  xx = x * x2;   xy = x * y2;   xz = x * z2;
  yy = y * y2;   yz = y * z2;   zz = z * z2;
  wx = w * x2;   wy = w * y2;   wz = w * z2;

  gp_Mat aMat;

  aMat (1, 1) = 1.0 - (yy + zz);
  aMat (1, 2) = xy - wz;
  aMat (1, 3) = xz + wy;
  
  aMat (2, 1) = xy + wz;
  aMat (2, 2) = 1.0 - (xx + zz);
  aMat (2, 3) = yz - wx;

  aMat (3, 1) = xz - wy;
  aMat (3, 2) = yz + wx;
  aMat (3, 3) = 1.0 - (xx + yy);
  // 1 division    16 multiplications    15 addidtions    12 variables

  return aMat;
}

namespace { // anonymous namespace

//=======================================================================
//function : translateEulerSequence
//purpose  : 
// Code supporting conversion between quaternion and generalized 
// Euler angles (sequence of three rotations) is based on
// algorithm by Ken Shoemake, published in Graphics Gems IV, p. 222-22
// http://tog.acm.org/resources/GraphicsGems/gemsiv/euler_angle/EulerAngles.c
//=======================================================================

struct gp_EulerSequence_Parameters
{
  Standard_Integer i;           // first rotation axis
  Standard_Integer j;           // next axis of rotation
  Standard_Integer k;           // third axis
  Standard_Boolean isOdd;       // true if order of two first rotation axes is odd permutation, e.g. XZ
  Standard_Boolean isTwoAxes;   // true if third rotation is about the same axis as first 
  Standard_Boolean isExtrinsic; // true if rotations are made around fixed axes

  gp_EulerSequence_Parameters (Standard_Integer theAx1, 
                               Standard_Boolean theisOdd, 
			       Standard_Boolean theisTwoAxes,
			       Standard_Boolean theisExtrinsic)
    : i(theAx1), 
      j(1 + (theAx1 + (theisOdd ? 1 : 0)) % 3), 
      k(1 + (theAx1 + (theisOdd ? 0 : 1)) % 3), 
      isOdd(theisOdd), 
      isTwoAxes(theisTwoAxes), 
      isExtrinsic(theisExtrinsic)
  {}
};

gp_EulerSequence_Parameters translateEulerSequence (const gp_EulerSequence theSeq)
{
  typedef gp_EulerSequence_Parameters Params;
  const Standard_Boolean F = Standard_False;
  const Standard_Boolean T = Standard_True;

  switch (theSeq)
  {
  case gp_Extrinsic_XYZ: return Params (1, F, F, T);
  case gp_Extrinsic_XZY: return Params (1, T, F, T);
  case gp_Extrinsic_YZX: return Params (2, F, F, T);
  case gp_Extrinsic_YXZ: return Params (2, T, F, T);
  case gp_Extrinsic_ZXY: return Params (3, F, F, T);
  case gp_Extrinsic_ZYX: return Params (3, T, F, T);

  // Conversion of intrinsic angles is made by the same code as for extrinsic,
  // using equivalence rule: intrinsic rotation is equivalent to extrinsic
  // rotation by the same angles but with inverted order of elemental rotations.
  // Swapping of angles (Alpha <-> Gamma) is done inside conversion procedure;
  // sequence of axes is inverted by setting appropriate parameters here.
  // Note that proper Euler angles (last block below) are symmetric for sequence of axes.
  case gp_Intrinsic_XYZ: return Params (3, T, F, F);
  case gp_Intrinsic_XZY: return Params (2, F, F, F);
  case gp_Intrinsic_YZX: return Params (1, T, F, F);
  case gp_Intrinsic_YXZ: return Params (3, F, F, F);
  case gp_Intrinsic_ZXY: return Params (2, T, F, F);
  case gp_Intrinsic_ZYX: return Params (1, F, F, F);

  case gp_Extrinsic_XYX: return Params (1, F, T, T);
  case gp_Extrinsic_XZX: return Params (1, T, T, T);
  case gp_Extrinsic_YZY: return Params (2, F, T, T);
  case gp_Extrinsic_YXY: return Params (2, T, T, T);
  case gp_Extrinsic_ZXZ: return Params (3, F, T, T);
  case gp_Extrinsic_ZYZ: return Params (3, T, T, T);

  case gp_Intrinsic_XYX: return Params (1, F, T, F);
  case gp_Intrinsic_XZX: return Params (1, T, T, F);
  case gp_Intrinsic_YZY: return Params (2, F, T, F);
  case gp_Intrinsic_YXY: return Params (2, T, T, F);
  case gp_Intrinsic_ZXZ: return Params (3, F, T, F);
  case gp_Intrinsic_ZYZ: return Params (3, T, T, F);

  default:
  case gp_EulerAngles : return Params (3, F, T, F); // = Intrinsic_ZXZ
  case gp_YawPitchRoll: return Params (1, F, F, F); // = Intrinsic_ZYX
  };
}

} // anonymous namespace

//=======================================================================
//function : SetEulerAngles
//purpose  : 
//=======================================================================

void gp_Quaternion::SetEulerAngles (const gp_EulerSequence theOrder,
				    const Standard_Real theAlpha,
				    const Standard_Real theBeta,
				    const Standard_Real theGamma)
{
  gp_EulerSequence_Parameters o = translateEulerSequence (theOrder);

  Standard_Real a = theAlpha, b = theBeta, c = theGamma;
  if ( ! o.isExtrinsic )
  {
    a = theGamma;
    c = theAlpha;
  }
  if ( o.isOdd )
    b = -b;

  Standard_Real ti = 0.5 * a; 
  Standard_Real tj = 0.5 * b; 
  Standard_Real th = 0.5 * c;
  Standard_Real ci = Cos (ti);  
  Standard_Real cj = Cos (tj);  
  Standard_Real ch = Cos (th);
  Standard_Real si = Sin (ti);
  Standard_Real sj = Sin (tj);
  Standard_Real sh = Sin (th);
  Standard_Real cc = ci * ch; 
  Standard_Real cs = ci * sh; 
  Standard_Real sc = si * ch; 
  Standard_Real ss = si * sh;

  Standard_Real values[4]; // w, x, y, z
  if ( o.isTwoAxes ) 
  {
    values[o.i] = cj * (cs + sc);
    values[o.j] = sj * (cc + ss);
    values[o.k] = sj * (cs - sc);
    values[0]   = cj * (cc - ss);
  } 
  else 
  {
    values[o.i] = cj * sc - sj * cs;
    values[o.j] = cj * ss + sj * cc;
    values[o.k] = cj * cs - sj * sc;
    values[0]   = cj * cc + sj * ss;
  }
  if ( o.isOdd ) 
    values[o.j] = -values[o.j];

  x = values[1];
  y = values[2];
  z = values[3];
  w = values[0];
}

//=======================================================================
//function : GetEulerAngles
//purpose  : 
//=======================================================================

void gp_Quaternion::GetEulerAngles (const gp_EulerSequence theOrder,
				    Standard_Real& theAlpha,
				    Standard_Real& theBeta,
				    Standard_Real& theGamma) const
{
  gp_Mat M = GetMatrix();

  gp_EulerSequence_Parameters o = translateEulerSequence (theOrder);
  if ( o.isTwoAxes ) 
  {
    double sy = sqrt (M(o.i, o.j) * M(o.i, o.j) + M(o.i, o.k) * M(o.i, o.k));
    if (sy > 16 * DBL_EPSILON) 
    {
      theAlpha = ATan2 (M(o.i, o.j),  M(o.i, o.k));
      theGamma = ATan2 (M(o.j, o.i), -M(o.k, o.i));
    } 
    else 
    {
      theAlpha = ATan2 (-M(o.j, o.k), M(o.j, o.j));
      theGamma = 0.;
    }
    theBeta = ATan2 (sy, M(o.i, o.i));
  } 
  else 
  {
    double cy = sqrt (M(o.i, o.i) * M(o.i, o.i) + M(o.j, o.i) * M(o.j, o.i));
    if (cy > 16 * DBL_EPSILON) 
    {
      theAlpha = ATan2 (M(o.k, o.j), M(o.k, o.k));
      theGamma = ATan2 (M(o.j, o.i), M(o.i, o.i));
    } 
    else 
    {
      theAlpha = ATan2 (-M(o.j, o.k), M(o.j, o.j));
      theGamma = 0.;
    }
    theBeta = ATan2 (-M(o.k, o.i), cy);
  }
  if ( o.isOdd ) 
  {
    theAlpha = -theAlpha;
    theBeta  = -theBeta;
    theGamma = -theGamma;
  }
  if ( ! o.isExtrinsic ) 
  { 
    Standard_Real aFirst = theAlpha; 
    theAlpha = theGamma;
    theGamma = aFirst;
  }
}

//=======================================================================
//function : StabilizeLength
//purpose  : 
//=======================================================================

void gp_Quaternion::StabilizeLength()
{
  Standard_Real cs = Abs (x) + Abs (y) + Abs (z) + Abs (w);
  if (cs > 0.0)
  {
    x /= cs; y /= cs; z /= cs; w /= cs;
  }
  else
  {
    SetIdent();
  }
}

//=======================================================================
//function : Normalize
//purpose  : 
//=======================================================================

void gp_Quaternion::Normalize()
{
  Standard_Real aMagn = Norm();
  if (aMagn < gp::Resolution())
  {
    StabilizeLength();
    aMagn = Norm();
  }
  Scale (1.0 / aMagn);
}

//=======================================================================
//function : Normalize
//purpose  : 
//=======================================================================

Standard_Real gp_Quaternion::GetRotationAngle() const
{
  if (w < 0.0)
  {
    return 2.0 * ATan2 (-Sqrt (x * x + y * y + z * z), -w);
  }
  else
  {
    return 2.0 * ATan2 ( Sqrt (x * x + y * y + z * z),  w);
  }
}

//=======================================================================
//function : Multiply
//purpose  : 
//=======================================================================

gp_Vec gp_Quaternion::Multiply (const gp_Vec& theVec) const
{
  gp_Quaternion theQ (theVec.X() * w + theVec.Z() * y - theVec.Y() * z,
		      theVec.Y() * w + theVec.X() * z - theVec.Z() * x,
		      theVec.Z() * w + theVec.Y() * x - theVec.X() * y,
		      theVec.X() * x + theVec.Y() * y + theVec.Z() * z);
  return gp_Vec (w * theQ.x + x * theQ.w + y * theQ.z - z * theQ.y,
		 w * theQ.y + y * theQ.w + z * theQ.x - x * theQ.z,
		 w * theQ.z + z * theQ.w + x * theQ.y - y * theQ.x) * (1.0 / SquareNorm());
}
