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

#ifndef _gp_Quaternion_HeaderFile
#define _gp_Quaternion_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <gp_EulerSequence.hxx>
#include <gp_Vec.hxx>
class gp_Vec;
class gp_Mat;


//! Represents operation of rotation in 3d space as queternion
//! and implements operations with rotations basing on
//! quaternion mathematics.
//!
//! In addition, provides methods for conversion to and from other
//! representatons of rotation (3*3 matrix, vector and
//! angle, Euler angles)
class gp_Quaternion 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates an identity quaternion
  gp_Quaternion();
  
  //! Creates quaternion directly from component values
  gp_Quaternion(const Standard_Real x, const Standard_Real y, const Standard_Real z, const Standard_Real w);
  
  //! Creates copy of another quaternion
  gp_Quaternion(const gp_Quaternion& theToCopy);
  
  //! Creates quaternion representing shortest-arc rotation
  //! operator producing vector theVecTo from vector theVecFrom.
  gp_Quaternion(const gp_Vec& theVecFrom, const gp_Vec& theVecTo);
  
  //! Creates quaternion representing shortest-arc rotation
  //! operator producing vector theVecTo from vector theVecFrom.
  //! Additional vector theHelpCrossVec defines preferred direction for
  //! rotation and is used when theVecTo and theVecFrom are directed
  //! oppositely.
  gp_Quaternion(const gp_Vec& theVecFrom, const gp_Vec& theVecTo, const gp_Vec& theHelpCrossVec);
  
  //! Creates quaternion representing rotation on angle
  //! theAngle around vector theAxis
  gp_Quaternion(const gp_Vec& theAxis, const Standard_Real theAngle);
  
  //! Creates quaternion from rotation matrix 3*3
  //! (which should be orthonormal skew-symmetric matrix)
  gp_Quaternion(const gp_Mat& theMat);
  
  //! Simple equal test without precision
  Standard_EXPORT Standard_Boolean IsEqual (const gp_Quaternion& theOther) const;
  
  //! Sets quaternion to shortest-arc rotation producing
  //! vector theVecTo from vector theVecFrom.
  //! If vectors theVecFrom and theVecTo are opposite then rotation
  //! axis is computed as theVecFrom ^ (1,0,0) or theVecFrom ^ (0,0,1).
  Standard_EXPORT void SetRotation (const gp_Vec& theVecFrom, const gp_Vec& theVecTo);
  
  //! Sets quaternion to shortest-arc rotation producing
  //! vector theVecTo from vector theVecFrom.
  //! If vectors theVecFrom and theVecTo are opposite then rotation
  //! axis is computed as theVecFrom ^ theHelpCrossVec.
  Standard_EXPORT void SetRotation (const gp_Vec& theVecFrom, const gp_Vec& theVecTo, const gp_Vec& theHelpCrossVec);
  
  //! Create a unit quaternion from Axis+Angle representation
  Standard_EXPORT void SetVectorAndAngle (const gp_Vec& theAxis, const Standard_Real theAngle);
  
  //! Convert a quaternion to Axis+Angle representation,
  //! preserve the axis direction and angle from -PI to +PI
  Standard_EXPORT void GetVectorAndAngle (gp_Vec& theAxis, Standard_Real& theAngle) const;
  
  //! Create a unit quaternion by rotation matrix
  //! matrix must contain only rotation (not scale or shear)
  //!
  //! For numerical stability we find first the greatest component of quaternion
  //! and than search others from this one
  Standard_EXPORT void SetMatrix (const gp_Mat& theMat);
  
  //! Returns rotation operation as 3*3 matrix
  Standard_EXPORT gp_Mat GetMatrix() const;
  
  //! Create a unit quaternion representing rotation defined
  //! by generalized Euler angles
  Standard_EXPORT void SetEulerAngles (const gp_EulerSequence theOrder, const Standard_Real theAlpha, const Standard_Real theBeta, const Standard_Real theGamma);
  
  //! Returns Euler angles describing current rotation
  Standard_EXPORT void GetEulerAngles (const gp_EulerSequence theOrder, Standard_Real& theAlpha, Standard_Real& theBeta, Standard_Real& theGamma) const;
  
  void Set (const Standard_Real x, const Standard_Real y, const Standard_Real z, const Standard_Real w);
  
  void Set (const gp_Quaternion& theQuaternion);
  
  Standard_Real X() const;
  
  Standard_Real Y() const;
  
  Standard_Real Z() const;
  
  Standard_Real W() const;
  
  //! Make identity quaternion (zero-rotation)
  void SetIdent();
  
  //! Reverse direction of rotation (conjugate quaternion)
  void Reverse();
  
  //! Return rotation with reversed direction (conjugated quaternion)
  gp_Quaternion Reversed() const;
  
  //! Inverts quaternion (both rotation direction and norm)
  void Invert();
  
  //! Return inversed quaternion q^-1
  gp_Quaternion Inverted() const;
  
  //! Returns square norm of quaternion
  Standard_Real SquareNorm() const;
  
  //! Returns norm of quaternion
  Standard_Real Norm() const;
  
  //! Scale all components by quaternion by theScale; note that
  //! rotation is not changed by this operation (except 0-scaling)
  void Scale (const Standard_Real theScale);
void operator *= (const Standard_Real theScale)
{
  Scale(theScale);
}
  
  //! Returns scaled quaternion
  gp_Quaternion Scaled (const Standard_Real theScale) const;
gp_Quaternion operator * (const Standard_Real theScale) const
{
  return Scaled(theScale);
}
  
  //! Stabilize quaternion length within 1 - 1/4.
  //! This operation is a lot faster than normalization
  //! and preserve length goes to 0 or infinity
  Standard_EXPORT void StabilizeLength();
  
  //! Scale quaternion that its norm goes to 1.
  //! The appearing of 0 magnitude or near is a error,
  //! so we can be sure that can divide by magnitude
  Standard_EXPORT void Normalize();
  
  //! Returns quaternion scaled so that its norm goes to 1.
  gp_Quaternion Normalized() const;
  
  //! Returns quaternion with all components negated.
  //! Note that this operation does not affect neither
  //! rotation operator defined by quaternion nor its norm.
  gp_Quaternion Negated() const;
gp_Quaternion operator -() const
{
  return Negated();
}
  
  //! Makes sum of quaternion components; result is "rotations mix"
  gp_Quaternion Added (const gp_Quaternion& theOther) const;
gp_Quaternion operator + (const gp_Quaternion& theOther) const
{
  return Added(theOther);
}
  
  //! Makes difference of quaternion components; result is "rotations mix"
  gp_Quaternion Subtracted (const gp_Quaternion& theOther) const;
gp_Quaternion operator - (const gp_Quaternion& theOther) const
{
  return Subtracted(theOther);
}
  
  //! Multiply function - work the same as Matrices multiplying.
  //! qq' = (cross(v,v') + wv' + w'v, ww' - dot(v,v'))
  //! Result is rotation combination: q' than q (here q=this, q'=theQ).
  //! Notices than:
  //! qq' != q'q;
  //! qq^-1 = q;
  gp_Quaternion Multiplied (const gp_Quaternion& theOther) const;
gp_Quaternion operator * (const gp_Quaternion& theOther) const
{
  return Multiplied(theOther);
}
  
  //! Adds componnets of other quaternion; result is "rotations mix"
  void Add (const gp_Quaternion& theOther);
void operator += (const gp_Quaternion& theOther)
{
  Add(theOther);
}
  
  //! Subtracts componnets of other quaternion; result is "rotations mix"
  void Subtract (const gp_Quaternion& theOther);
void operator -= (const gp_Quaternion& theOther)
{
  Subtract(theOther);
}
  
  //! Adds rotation by multiplication
  void Multiply (const gp_Quaternion& theOther);
void operator *= (const gp_Quaternion& theOther)
{
  Multiply(theOther);
}
  
  //! Computes inner product / scalar product / Dot
  Standard_Real Dot (const gp_Quaternion& theOther) const;
  
  //! Return rotation angle from -PI to PI
  Standard_EXPORT Standard_Real GetRotationAngle() const;
  
  //! Rotates vector by quaternion as rotation operator
  Standard_EXPORT gp_Vec Multiply (const gp_Vec& theVec) const;
gp_Vec operator * (const gp_Vec& theVec) const
{
  return Multiply(theVec);
}




protected:





private:



  Standard_Real x;
  Standard_Real y;
  Standard_Real z;
  Standard_Real w;


};


#include <gp_Quaternion.lxx>





#endif // _gp_Quaternion_HeaderFile
