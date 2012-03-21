// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#ifndef _gp_Quaternion_HeaderFile
#define _gp_Quaternion_HeaderFile

/**
 * Initial sources based on free samples from
 * http://www.gamedev.ru/code/articles/?id=4215&page=3
 */

#include <gp_Vec.hxx>
#include <gp_Mat.hxx>
#include <Precision.hxx>

//! General quaternion class (unit and nonunit),
//! designed for 3d rotations and orientation representation <br>
class gp_Quaternion
{

public:

  //! Creates an identity quaternion. <br>
  gp_Quaternion()
  : x (0.0), y (0.0), z (0.0), w (1.0) {}

  gp_Quaternion (Standard_Real x, Standard_Real y, Standard_Real z, Standard_Real w)
  : x (x), y (y), z (z), w (w) {}

  gp_Quaternion (const gp_Quaternion& theToCopy)
  : x (theToCopy.x), y (theToCopy.y), z (theToCopy.z), w (theToCopy.w) {}

  gp_Quaternion (const gp_Vec& theVecFrom, const gp_Vec& theVecTo)
  : x (0.0), y (0.0), z (0.0), w (1.0)
  {
    Init (theVecFrom, theVecTo);
  }

  gp_Quaternion (const gp_Vec& theVecFrom, const gp_Vec& theVecTo, const gp_Vec& theHelpCrossVec)
  : x (0.0), y (0.0), z (0.0), w (1.0)
  {
    Init (theVecFrom, theVecTo, theHelpCrossVec);
  }

  //! Simple equal test without precision
  Standard_Boolean IsEqual (const gp_Quaternion& theOther) const
  {
    if (this == &theOther)
    {
      return Standard_True;
    }
    return Abs (x - theOther.x) <= gp::Resolution() &&
           Abs (y - theOther.y) <= gp::Resolution() &&
           Abs (z - theOther.z) <= gp::Resolution() &&
           Abs (w - theOther.w) <= gp::Resolution();
  }

  /**
   * Shortest arc quaternion rotate one vector to another by shortest path.
   * Create rotation from -> to, for any length vectors.
   * If vectors theVecFrom and theVecTo are opposite then rotation axis computed as
   * theVecFrom * gp_Vec(1,0,0) or theVecFrom * gp_Vec(0,0,1).
   */
  void Init (const gp_Vec& theVecFrom, const gp_Vec& theVecTo)
  {
    gp_Vec aVecCross (theVecFrom.Crossed (theVecTo));
    Set (aVecCross.X(), aVecCross.Y(), aVecCross.Z(), theVecFrom.Dot (theVecTo));
    Normalize();    // if "from" or "to" not unit, normalize quat
    w += 1.0;       // reducing angle to halfangle
    if (w <= Precision::Confusion())  // angle close to PI
    {
      if ((theVecFrom.Z() * theVecFrom.Z()) > (theVecFrom.X() * theVecFrom.X()))
        Set (           0.0,  theVecFrom.Z(), -theVecFrom.Y(), w); // theVecFrom * gp_Vec(1,0,0)
      else 
        Set (theVecFrom.Y(), -theVecFrom.X(),             0.0, w); // theVecFrom * gp_Vec(0,0,1)
    }
    Normalize(); 
  }

  /**
   * Shortest arc quaternion rotate one vector to another by shortest path.
   * If vectors theVecFrom and theVecTo are opposite then rotation axis computed as
   * theVecFrom * theHelpCrossVec.
   */
  void Init (const gp_Vec& theVecFrom, const gp_Vec& theVecTo, const gp_Vec& theHelpCrossVec)
  {
    gp_Vec aVecCross (theVecFrom.Crossed (theVecTo));
    Set (aVecCross.X(), aVecCross.Y(), aVecCross.Z(), theVecFrom.Dot (theVecTo));
    Normalize();    // if "from" or "to" not unit, normalize quat
    w += 1.0;       // reducing angle to halfangle
    if (w <= Precision::Confusion())  // angle close to PI
    {
      gp_Vec theAxis = theVecFrom.Crossed (theHelpCrossVec);
      Set (theAxis.X(), theAxis.Y(), theAxis.Z(), w);
    }
    Normalize(); 
  }

  gp_Quaternion (const gp_Vec& theAxis, const Standard_Real& theAngle)
  : x (0.0), y (0.0), z (0.0), w (1.0)
  {
    InitUnit (theAxis, theAngle);
  }

  //! Create a unit quaternion from Axis+Angle representation
  void InitUnit (const gp_Vec& theAxis, const Standard_Real& theAngle)
  {
    gp_Vec anAxis = theAxis.Normalized();
    Standard_Real anAngleHalf = 0.5 * theAngle;
    Standard_Real sin_a = Sin (anAngleHalf);
    Set (anAxis.X() * sin_a, anAxis.Y() * sin_a, anAxis.Z() * sin_a, Cos (anAngleHalf));
  }

  /**
   * Convert a quaternion to Axis+Angle representation, 
   * preserve the axis direction and angle from -PI to +PI
   */
  void GetAxisAngle (gp_Vec& theAxis, Standard_Real& theAngle) const
  {
    Standard_Real vl = Sqrt (x * x + y * y + z * z);
    if (vl > Precision::Confusion())
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
      theAxis.SetCoord (0.0, 0.0, 0.0);
      theAngle = 0.0;
    }
  }

  /**
   * Create a unit quaternion by rotation matrix
   * matrix must contain only rotation (not scale or shear)
   *
   * For numerical stability we find first the greatest component of quaternion
   * and than search others from this one
   */
  void InitUnit (const gp_Mat& theMat)
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

  gp_Quaternion (const gp_Mat& theMat)
  : x (0.0), y (0.0), z (0.0), w (1.0)
  {
    Init (theMat);
  }

  /**
   * Create a nonunit quaternion from rotation matrix 
   * martix must contain only rotation (not scale or shear)
   * the result quaternion length is numerical stable 
   */
  void Init (const gp_Mat& theMat)
  {
    Standard_Real tr = theMat (1, 1) + theMat (2, 2) + theMat (3, 3); // trace of martix
    if (tr > 0.0)
    { // if trace positive than "w" is biggest component
      Set (theMat (3, 2) - theMat (2, 3),
           theMat (1, 3) - theMat (3, 1),
           theMat (2, 1) - theMat (1, 2),
           tr + 1.0);
    }
    else if ((theMat (1, 1) > theMat (2, 2)) && (theMat (1, 1) > theMat (3, 3)))
    { // Some of vector components is bigger
      Set (1.0 + theMat (1, 1) - theMat (2, 2) - theMat (3, 3),
           theMat (1, 2) + theMat (2, 1),
           theMat (1, 3) + theMat (3, 1),
           theMat (3, 2) - theMat (2, 3));
    }
    else if (theMat (2, 2) > theMat (3, 3))
    {
      Set (theMat (1, 2) + theMat (2, 1),
           1.0 + theMat (2, 2) - theMat (1, 1) - theMat (3, 3),
           theMat (2, 3) + theMat (3, 2),
           theMat (1, 3) - theMat (3, 1));
    }
    else
    {
      Set (theMat (1, 3) + theMat (3, 1),
           theMat (2, 3) + theMat (3, 2),
           1.0 + theMat (3, 3) - theMat (1, 1) - theMat (2, 2),
           theMat (2, 1) - theMat (1, 2));
    }
  }

  //! Set the rotation to matrix
  void GetRotateMatrix (gp_Mat& theMat) const
  {
    Standard_Real wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
    Standard_Real s  = 2.0 / Norm(); 
    x2 = x * s;    y2 = y * s;    z2 = z * s;
    xx = x * x2;   xy = x * y2;   xz = x * z2;
    yy = y * y2;   yz = y * z2;   zz = z * z2;
    wx = w * x2;   wy = w * y2;   wz = w * z2;

    theMat (1, 1) = 1.0 - (yy + zz);
    theMat (1, 2) = xy - wz;
    theMat (1, 3) = xz + wy;

    theMat (2, 1) = xy + wz;
    theMat (2, 2) = 1.0 - (xx + zz);
    theMat (2, 3) = yz - wx;

    theMat (3, 1) = xz - wy;
    theMat (3, 2) = yz + wx;
    theMat (3, 3) = 1.0 - (xx + yy);
    // 1 division    16 multiplications    15 addidtions    12 variables
  }

  //! Set the rotation from unit quat to matrix
  void GetRotateMatrixFromUnit (gp_Mat& theMat) const
  {
    Standard_Real wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
    x2 = x + x;    y2 = y + y;    z2 = z + z;
    xx = x * x2;   xy = x * y2;   xz = x * z2;
    yy = y * y2;   yz = y * z2;   zz = z * z2;
    wx = w * x2;   wy = w * y2;   wz = w * z2;

    theMat (1, 1) = 1.0 - (yy + zz);
    theMat (1, 2) = xy - wz;
    theMat (1, 3) = xz + wy;

    theMat (2, 1) = xy + wz;
    theMat (2, 2) = 1.0 - (xx + zz);
    theMat (2, 3) = yz - wx;

    theMat (3, 1) = xz - wy;
    theMat (3, 2) = yz + wx;
    theMat (3, 3) = 1.0 - (xx + yy);
  }

  void Set (Standard_Real x, Standard_Real y, Standard_Real z, Standard_Real w)
  {
    this->x = x; this->y = y; this->z = z; this->w = w;
  }

  void Set (const gp_Quaternion& theQuaternion)
  {
    x = theQuaternion.x; y = theQuaternion.y; z = theQuaternion.z; w = theQuaternion.w;
  }

  Standard_Real X() const
  {
    return x;
  }

  Standard_Real Y() const
  {
    return y;
  }

  Standard_Real Z() const
  {
    return z;
  }

  Standard_Real W() const
  {
    return w;
  }

  //! Make identity quaternion (zero-rotation)
  void Ident()
  {
    x = 0.0; y = 0.0; z = 0.0; w = 1.0;
  }

  //! Reverse rotation
  void Conjugate()
  {
    x = -x; y = -y; z = -z;
  }

  //! Return reversed rotation
  gp_Quaternion Conjugated() const
  {
    return gp_Quaternion (-x, -y, -z, w);
  }

  //! Rotation will not be changed by this operation (except 0-scaling)
  void Scale (Standard_Real theScale)
  {
    x *= theScale; y *= theScale; z *= theScale; w *= theScale;
  }

  gp_Quaternion Scaled (const Standard_Real theScale) const
  {
    return gp_Quaternion (x * theScale, y * theScale, z * theScale, w * theScale);
  }

  Standard_Real Norm() const
  {
    return x * x + y * y + z * z + w * w;
  }

  Standard_Real Magnitude() const
  {
    return Sqrt (Norm());
  }

  //! Invert quaternion q = q^-1
  void Invert()
  {
    Standard_Real in = 1.0 / Norm();
    Set (-x * in, -y * in, -z * in, w * in);
  }

  //! Return inversed quaternion q^-1
  gp_Quaternion Inverted() const
  {
    Standard_Real in = 1.0 / Norm();
    return gp_Quaternion (-x * in, -y * in, -z * in, w * in);
  }

  /**
   * Stabilize quaternion length within 1 - 1/4
   * This operation is a lot faster than normalization
   * and preserve length goes to 0 or infinity
   */
  void StabilizeLength()
  {
    Standard_Real cs = Abs (x) + Abs (y) + Abs (z) + Abs (w);
    if (cs > 0.0)
    {
      x /= cs; y /= cs; z /= cs; w /= cs;
    }
    else
    {
      Ident();
    }
  }

  /**
   * Scale quaternion that its norm goes to 1 
   * The appearing of 0 magnitude or near is a error,
   * so we can be sure that can divide by magnitude
   */
  void Normalize()
  {
    Standard_Real aMagn = Magnitude();
    if (aMagn < Precision::Confusion())
    {
      StabilizeLength();
      aMagn = Magnitude();
    }
    Scale (1.0 / aMagn);
  }

  gp_Quaternion Normalized() const
  {
    gp_Quaternion aNormilizedQ (*this);
    aNormilizedQ.Normalize();
    return aNormilizedQ;
  }

  gp_Quaternion operator-() const
  {
    return gp_Quaternion (-x, -y, -z, -w);
  }

  //! Result is "rotations mix"
  gp_Quaternion Added (const gp_Quaternion& theQ) const
  {
    return gp_Quaternion (x + theQ.x, y + theQ.y, z + theQ.z, w + theQ.w);
  }

  //! Result is "rotations mix"
  gp_Quaternion operator+ (const gp_Quaternion& theQ) const
  {
    return Added(theQ);
  }

  //! Result is "rotations mix"
  gp_Quaternion Subtracted (const gp_Quaternion& theQ) const
  {
    return gp_Quaternion (x - theQ.x, y - theQ.y, z - theQ.z, w - theQ.w);
  }

  //! Result is "rotations mix"
  gp_Quaternion operator- (const gp_Quaternion& theQ) const
  {
    return Subtracted (theQ);
  }

  //! Rotation will not be changed by this operation (except 0-scaling)
  gp_Quaternion operator* (const Standard_Real& theScale) const
  {
    return gp_Quaternion (x * theScale, y * theScale, z * theScale, w * theScale);
  }

  /**
   * Multiply function - work the same as Matrices multiplying.
   * qq' = (cross(v,v') + wv' + w'v, ww' - dot(v,v'))
   * Result is rotation combination: q' than q (here q=this, q'=theQ).
   * Notices than:
   *   qq' != q'q;
   *   qq^-1 = q;
   */ 
  gp_Quaternion operator* (const gp_Quaternion& theQ) const
  {
    return gp_Quaternion (w * theQ.x + x * theQ.w + y * theQ.z - z * theQ.y,
                          w * theQ.y + y * theQ.w + z * theQ.x - x * theQ.z,
                          w * theQ.z + z * theQ.w + x * theQ.y - y * theQ.x,
                          w * theQ.w - x * theQ.x - y * theQ.y - z * theQ.z);
    // 16 multiplications    12 addidtions    0 variables
  }

  //! Result is "rotations mix"
  const gp_Quaternion& Add (const gp_Quaternion& theQ)
  {
    x += theQ.x; y += theQ.y; z += theQ.z; w += theQ.w;
    return *this;
  }

  //! Result is "rotations mix"
  const gp_Quaternion& operator+= (const gp_Quaternion& theQ)
  {
    return Add (theQ);
  }

  //! Result is "rotations mix"
  const gp_Quaternion& Subtract (const gp_Quaternion& theQ)
  {
    x -= theQ.x; y -= theQ.y; z -= theQ.z; w -= theQ.w;
    return *this;
  }

  //! Result is "rotations mix"
  const gp_Quaternion& operator-= (const gp_Quaternion& theQ)
  {
    return Subtract(theQ);
  }

  const gp_Quaternion& operator*= (const gp_Quaternion& theQ)
  {
    Set ((*this) * theQ);  // have no optimization here
    return *this;
  }

  //! Inner product / scalar product / Dot
  Standard_Real Dot (const gp_Quaternion& theQ)
  {
    return x * theQ.x + y * theQ.y + z * theQ.z + w * theQ.w;
  }

  //! Return rotation angle from -PI to PI
  Standard_Real GetRotationAngle() const
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

  //! Return vector rotated by quaternion
  gp_Vec Rotated (const gp_Vec& theVec) const
  {
    gp_Quaternion theQ (theVec.X() * w + theVec.Z() * y - theVec.Y() * z,
                        theVec.Y() * w + theVec.X() * z - theVec.Z() * x,
                        theVec.Z() * w + theVec.Y() * x - theVec.X() * y,
                        theVec.X() * x + theVec.Y() * y + theVec.Z() * z);
    return gp_Vec (w * theQ.x + x * theQ.w + y * theQ.z - z * theQ.y,
                   w * theQ.y + y * theQ.w + z * theQ.x - x * theQ.z,
                   w * theQ.z + z * theQ.w + x * theQ.y - y * theQ.x) * (1.0 / Norm());
  }

private: 

Standard_Real x;
Standard_Real y;
Standard_Real z;
Standard_Real w;

};

#endif //_gp_Quaternion_HeaderFile
