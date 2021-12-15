// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _gp_XYZ_HeaderFile
#define _gp_XYZ_HeaderFile

#include <gp.hxx>
#include <gp_Mat.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_OStream.hxx>
#include <Standard_SStream.hxx>

//! This class describes a cartesian coordinate entity in
//! 3D space {X,Y,Z}. This entity is used for algebraic
//! calculation. This entity can be transformed
//! with a "Trsf" or a  "GTrsf" from package "gp".
//! It is used in vectorial computations or for holding this type
//! of information in data structures.
class gp_XYZ 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Creates an XYZ object with zero coordinates (0,0,0)
  gp_XYZ()
  : x (0.),
    y (0.),
    z (0.)
  {}

  //! creates an XYZ with given coordinates
  gp_XYZ (const Standard_Real theX, const Standard_Real theY, const Standard_Real theZ)
  : x (theX),
    y (theY),
    z (theZ)
  {}

  //! For this XYZ object, assigns
  //! the values theX, theY and theZ to its three coordinates
  void SetCoord (const Standard_Real theX, const Standard_Real theY, const Standard_Real theZ)
  {
    x = theX;
    y = theY;
    z = theZ;
  }

  //! modifies the coordinate of range theIndex
  //! theIndex = 1 => X is modified
  //! theIndex = 2 => Y is modified
  //! theIndex = 3 => Z is modified
  //! Raises OutOfRange if theIndex != {1, 2, 3}.
  void SetCoord (const Standard_Integer theIndex, const Standard_Real theXi)
  {
    Standard_OutOfRange_Raise_if (theIndex < 1 || theIndex > 3, NULL);
    (&x)[theIndex - 1] = theXi;
  }

  //! Assigns the given value to the X coordinate
  void SetX (const Standard_Real theX) { x = theX; }

  //! Assigns the given value to the Y coordinate
  void SetY (const Standard_Real theY) { y = theY; }

  //! Assigns the given value to the Z coordinate
  void SetZ (const Standard_Real theZ) { z = theZ; }

  //! returns the coordinate of range theIndex :
  //! theIndex = 1 => X is returned
  //! theIndex = 2 => Y is returned
  //! theIndex = 3 => Z is returned
  //!
  //! Raises OutOfRange if theIndex != {1, 2, 3}.
  Standard_Real Coord (const Standard_Integer theIndex) const
  {
    Standard_OutOfRange_Raise_if (theIndex < 1 || theIndex > 3, NULL);
    return (&x)[theIndex - 1];
  }

  Standard_Real& ChangeCoord (const Standard_Integer theIndex)
  {
    Standard_OutOfRange_Raise_if (theIndex < 1 || theIndex > 3, NULL);
    return (&x)[theIndex - 1];
  }

  void Coord (Standard_Real& theX, Standard_Real& theY, Standard_Real& theZ) const
  {
    theX = x;
    theY = y;
    theZ = z;
  }

  //! Returns a const ptr to coordinates location.
  //! Is useful for algorithms, but DOES NOT PERFORM
  //! ANY CHECKS!
  const Standard_Real* GetData() const { return (&x); }

  //! Returns a ptr to coordinates location.
  //! Is useful for algorithms, but DOES NOT PERFORM
  //! ANY CHECKS!
  Standard_Real* ChangeData() { return (&x); }

  //! Returns the X coordinate
  Standard_Real X() const { return x; }

  //! Returns the Y coordinate
  Standard_Real Y() const { return y; }

  //! Returns the Z coordinate
  Standard_Real Z() const { return z; }

  //! computes Sqrt (X*X + Y*Y + Z*Z) where X, Y and Z are the three coordinates of this XYZ object.
  Standard_Real Modulus() const { return sqrt (x * x + y * y + z * z); }

  //! Computes X*X + Y*Y + Z*Z where X, Y and Z are the three coordinates of this XYZ object.
  Standard_Real SquareModulus() const { return (x * x + y * y + z * z); }

  //! Returns True if he coordinates of this XYZ object are
  //! equal to the respective coordinates Other,
  //! within the specified tolerance theTolerance. I.e.:
  //! abs(<me>.X() - theOther.X()) <= theTolerance and
  //! abs(<me>.Y() - theOther.Y()) <= theTolerance and
  //! abs(<me>.Z() - theOther.Z()) <= theTolerance.
  Standard_EXPORT Standard_Boolean IsEqual (const gp_XYZ& theOther, const Standard_Real theTolerance) const;

  //! @code
  //! <me>.X() = <me>.X() + theOther.X()
  //! <me>.Y() = <me>.Y() + theOther.Y()
  //! <me>.Z() = <me>.Z() + theOther.Z()
  //! @endcode
  void Add (const gp_XYZ& theOther)
  {
    x += theOther.x;
    y += theOther.y;
    z += theOther.z;
  }

  void operator+= (const gp_XYZ& theOther) { Add (theOther); }

  //! @code
  //! new.X() = <me>.X() + theOther.X()
  //! new.Y() = <me>.Y() + theOther.Y()
  //! new.Z() = <me>.Z() + theOther.Z()
  //! @endcode
  Standard_NODISCARD gp_XYZ Added (const gp_XYZ& theOther) const
  {
    return gp_XYZ (x + theOther.x, y + theOther.y, z + theOther.z);
  }

  Standard_NODISCARD gp_XYZ operator + (const gp_XYZ& theOther) const { return Added (theOther); }

  //! @code
  //! <me>.X() = <me>.Y() * theOther.Z() - <me>.Z() * theOther.Y()
  //! <me>.Y() = <me>.Z() * theOther.X() - <me>.X() * theOther.Z()
  //! <me>.Z() = <me>.X() * theOther.Y() - <me>.Y() * theOther.X()
  //! @endcode
  void Cross (const gp_XYZ& theOther);

  void operator^= (const gp_XYZ& theOther) { Cross (theOther); }

  //! @code
  //! new.X() = <me>.Y() * theOther.Z() - <me>.Z() * theOther.Y()
  //! new.Y() = <me>.Z() * theOther.X() - <me>.X() * theOther.Z()
  //! new.Z() = <me>.X() * theOther.Y() - <me>.Y() * theOther.X()
  //! @endcode
  Standard_NODISCARD gp_XYZ Crossed (const gp_XYZ& theOther) const
  {
    return gp_XYZ (y * theOther.z - z * theOther.y,
                   z * theOther.x - x * theOther.z,
                   x * theOther.y - y * theOther.x);
  }

  Standard_NODISCARD gp_XYZ operator ^ (const gp_XYZ& theOther) const { return Crossed (theOther); }

  //! Computes the magnitude of the cross product between <me> and
  //! theRight. Returns || <me> ^ theRight ||
  Standard_Real CrossMagnitude (const gp_XYZ& theRight) const;

  //! Computes the square magnitude of the cross product between <me> and
  //! theRight. Returns || <me> ^ theRight ||**2
  Standard_Real CrossSquareMagnitude (const gp_XYZ& theRight) const;

  //! Triple vector product
  //! Computes <me> = <me>.Cross(theCoord1.Cross(theCoord2))
  void CrossCross (const gp_XYZ& theCoord1, const gp_XYZ& theCoord2);

  //! Triple vector product
  //! computes New = <me>.Cross(theCoord1.Cross(theCoord2))
  Standard_NODISCARD gp_XYZ CrossCrossed (const gp_XYZ& theCoord1, const gp_XYZ& theCoord2) const
  {
    gp_XYZ aCoord0 = *this;
    aCoord0.CrossCross (theCoord1, theCoord2);
    return aCoord0;
  }

  //! divides <me> by a real.
  void Divide (const Standard_Real theScalar)
  {
    x /= theScalar;
    y /= theScalar;
    z /= theScalar;
  }

  void operator/= (const Standard_Real theScalar) { Divide (theScalar); }

  //! divides <me> by a real.
  Standard_NODISCARD gp_XYZ Divided (const Standard_Real theScalar) const { return gp_XYZ (x / theScalar, y / theScalar, z / theScalar); }

  Standard_NODISCARD gp_XYZ operator/ (const Standard_Real theScalar) const { return Divided (theScalar); }

  //! computes the scalar product between <me> and theOther
  Standard_Real Dot (const gp_XYZ& theOther) const { return(x * theOther.x + y * theOther.y + z * theOther.z); }

  Standard_Real operator* (const gp_XYZ& theOther) const { return Dot (theOther); }

  //! computes the triple scalar product
  Standard_Real DotCross (const gp_XYZ& theCoord1, const gp_XYZ& theCoord2) const;

  //! @code
  //! <me>.X() = <me>.X() * theScalar;
  //! <me>.Y() = <me>.Y() * theScalar;
  //! <me>.Z() = <me>.Z() * theScalar;
  //! @endcode
  void Multiply (const Standard_Real theScalar)
  {
    x *= theScalar;
    y *= theScalar;
    z *= theScalar;
  }

  void operator*= (const Standard_Real theScalar) { Multiply (theScalar); }

  //! @code
  //! <me>.X() = <me>.X() * theOther.X();
  //! <me>.Y() = <me>.Y() * theOther.Y();
  //! <me>.Z() = <me>.Z() * theOther.Z();
  //! @endcode
  void Multiply (const gp_XYZ& theOther)
  {
    x *= theOther.x;
    y *= theOther.y;
    z *= theOther.z;
  }

  void operator*= (const gp_XYZ& theOther) { Multiply (theOther); }

  //! <me> = theMatrix * <me>
  void Multiply (const gp_Mat& theMatrix);

  void operator*= (const gp_Mat& theMatrix) { Multiply (theMatrix); }

  //! @code
  //! New.X() = <me>.X() * theScalar;
  //! New.Y() = <me>.Y() * theScalar;
  //! New.Z() = <me>.Z() * theScalar;
  //! @endcode
  Standard_NODISCARD gp_XYZ Multiplied (const Standard_Real theScalar) const { return gp_XYZ (x * theScalar, y * theScalar, z * theScalar); }

  Standard_NODISCARD gp_XYZ operator* (const Standard_Real theScalar) const { return Multiplied (theScalar); }

  //! @code
  //! new.X() = <me>.X() * theOther.X();
  //! new.Y() = <me>.Y() * theOther.Y();
  //! new.Z() = <me>.Z() * theOther.Z();
  //! @endcode
  Standard_NODISCARD gp_XYZ Multiplied (const gp_XYZ& theOther) const { return gp_XYZ (x * theOther.x, y * theOther.y, z * theOther.z); }

  //! New = theMatrix * <me>
  Standard_NODISCARD gp_XYZ Multiplied (const gp_Mat& theMatrix) const
  {
    return gp_XYZ (theMatrix.Value (1, 1) * x + theMatrix.Value (1, 2) * y + theMatrix.Value (1, 3) * z,
                   theMatrix.Value (2, 1) * x + theMatrix.Value (2, 2) * y + theMatrix.Value (2, 3) * z,
                   theMatrix.Value (3, 1) * x + theMatrix.Value (3, 2) * y + theMatrix.Value (3, 3) * z);
  }

  Standard_NODISCARD gp_XYZ operator* (const gp_Mat& theMatrix) const { return Multiplied (theMatrix); }

  //! @code
  //! <me>.X() = <me>.X()/ <me>.Modulus()
  //! <me>.Y() = <me>.Y()/ <me>.Modulus()
  //! <me>.Z() = <me>.Z()/ <me>.Modulus()
  //! @endcode
  //! Raised if <me>.Modulus() <= Resolution from gp
  void Normalize();

  //! @code
  //! New.X() = <me>.X()/ <me>.Modulus()
  //! New.Y() = <me>.Y()/ <me>.Modulus()
  //! New.Z() = <me>.Z()/ <me>.Modulus()
  //! @endcode
  //! Raised if <me>.Modulus() <= Resolution from gp
  Standard_NODISCARD gp_XYZ Normalized() const
  {
    Standard_Real aD = Modulus();
    Standard_ConstructionError_Raise_if (aD <= gp::Resolution(), "gp_XYZ::Normalized() - vector has zero norm");
    return gp_XYZ(x / aD, y / aD, z / aD);
  }

  //! @code
  //! <me>.X() = -<me>.X()
  //! <me>.Y() = -<me>.Y()
  //! <me>.Z() = -<me>.Z()
  //! @endcode
  void Reverse()
  {
    x = -x;
    y = -y;
    z = -z;
  }

  //! @code
  //! New.X() = -<me>.X()
  //! New.Y() = -<me>.Y()
  //! New.Z() = -<me>.Z()
  //! @endcode
  Standard_NODISCARD gp_XYZ Reversed() const { return gp_XYZ (-x, -y, -z); }

  //! @code
  //! <me>.X() = <me>.X() - theOther.X()
  //! <me>.Y() = <me>.Y() - theOther.Y()
  //! <me>.Z() = <me>.Z() - theOther.Z()
  //! @endcode
  void Subtract (const gp_XYZ& theOther)
  {
    x -= theOther.x;
    y -= theOther.y;
    z -= theOther.z;
  }

  void operator-= (const gp_XYZ& theOther) { Subtract (theOther); }

  //! @code
  //! new.X() = <me>.X() - theOther.X()
  //! new.Y() = <me>.Y() - theOther.Y()
  //! new.Z() = <me>.Z() - theOther.Z()
  //! @endcode
  Standard_NODISCARD gp_XYZ Subtracted (const gp_XYZ& theOther) const { return gp_XYZ (x - theOther.x, y - theOther.y, z - theOther.z); }

  Standard_NODISCARD gp_XYZ operator-  (const gp_XYZ& theOther) const { return Subtracted (theOther); }

  //! <me> is set to the following linear form :
  //! @code
  //! theA1 * theXYZ1 + theA2 * theXYZ2 + theA3 * theXYZ3 + theXYZ4
  //! @endcode
  void SetLinearForm (const Standard_Real theA1, const gp_XYZ& theXYZ1,
                      const Standard_Real theA2, const gp_XYZ& theXYZ2,
                      const Standard_Real theA3, const gp_XYZ& theXYZ3,
                      const gp_XYZ& theXYZ4)
  {
    x = theA1 * theXYZ1.x + theA2 * theXYZ2.x + theA3 * theXYZ3.x + theXYZ4.x;
    y = theA1 * theXYZ1.y + theA2 * theXYZ2.y + theA3 * theXYZ3.y + theXYZ4.y;
    z = theA1 * theXYZ1.z + theA2 * theXYZ2.z + theA3 * theXYZ3.z + theXYZ4.z;
  }

  //! <me> is set to the following linear form :
  //! @code
  //! theA1 * theXYZ1 + theA2 * theXYZ2 + theA3 * theXYZ3
  //! @endcode
  void SetLinearForm (const Standard_Real theA1, const gp_XYZ& theXYZ1,
                      const Standard_Real theA2, const gp_XYZ& theXYZ2,
                      const Standard_Real theA3, const gp_XYZ& theXYZ3)
  {
    x = theA1 * theXYZ1.x + theA2 * theXYZ2.x + theA3 * theXYZ3.x;
    y = theA1 * theXYZ1.y + theA2 * theXYZ2.y + theA3 * theXYZ3.y;
    z = theA1 * theXYZ1.z + theA2 * theXYZ2.z + theA3 * theXYZ3.z;
  }

  //! <me> is set to the following linear form :
  //! @code
  //! theA1 * theXYZ1 + theA2 * theXYZ2 + theXYZ3
  //! @endcode
  void SetLinearForm (const Standard_Real theA1, const gp_XYZ& theXYZ1,
                      const Standard_Real theA2, const gp_XYZ& theXYZ2,
                      const gp_XYZ& theXYZ3)
  {
    x = theA1 * theXYZ1.x + theA2 * theXYZ2.x + theXYZ3.x;
    y = theA1 * theXYZ1.y + theA2 * theXYZ2.y + theXYZ3.y;
    z = theA1 * theXYZ1.z + theA2 * theXYZ2.z + theXYZ3.z;
  }

  //! <me> is set to the following linear form :
  //! @code
  //! theA1 * theXYZ1 + theA2 * theXYZ2
  //! @endcode
  void SetLinearForm (const Standard_Real theA1, const gp_XYZ& theXYZ1,
                      const Standard_Real theA2, const gp_XYZ& theXYZ2)
  {
    x = theA1 * theXYZ1.x + theA2 * theXYZ2.x;
    y = theA1 * theXYZ1.y + theA2 * theXYZ2.y;
    z = theA1 * theXYZ1.z + theA2 * theXYZ2.z;
  }

  //! <me> is set to the following linear form :
  //! @code
  //! theA1 * theXYZ1 + theXYZ2
  //! @endcode
  void SetLinearForm (const Standard_Real theA1, const gp_XYZ& theXYZ1,
                      const gp_XYZ& theXYZ2)
  {
    x = theA1 * theXYZ1.x + theXYZ2.x;
    y = theA1 * theXYZ1.y + theXYZ2.y;
    z = theA1 * theXYZ1.z + theXYZ2.z;
  }

  //! <me> is set to the following linear form :
  //! @code
  //! theXYZ1 + theXYZ2
  //! @endcode
  void SetLinearForm (const gp_XYZ& theXYZ1, const gp_XYZ& theXYZ2)
  {
    x = theXYZ1.x + theXYZ2.x;
    y = theXYZ1.y + theXYZ2.y;
    z = theXYZ1.z + theXYZ2.z;
  }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

  //! Inits the content of me from the stream
  Standard_EXPORT Standard_Boolean InitFromJson (const Standard_SStream& theSStream, Standard_Integer& theStreamPos);

private:

  Standard_Real x;
  Standard_Real y;
  Standard_Real z;

};

//=======================================================================
//function : Cross
// purpose :
//=======================================================================
inline void gp_XYZ::Cross (const gp_XYZ& theRight)
{
  Standard_Real aXresult = y * theRight.z - z * theRight.y;
  Standard_Real aYresult = z * theRight.x - x * theRight.z;
  z = x * theRight.y - y * theRight.x;
  x = aXresult;
  y = aYresult;
}

//=======================================================================
//function : CrossMagnitude
// purpose :
//=======================================================================
inline Standard_Real gp_XYZ::CrossMagnitude (const gp_XYZ& theRight) const
{
  Standard_Real aXresult = y * theRight.z - z * theRight.y;
  Standard_Real aYresult = z * theRight.x - x * theRight.z;
  Standard_Real aZresult = x * theRight.y - y * theRight.x;
  return sqrt (aXresult * aXresult + aYresult * aYresult + aZresult * aZresult);
}

//=======================================================================
//function : CrossSquareMagnitude
// purpose :
//=======================================================================
inline Standard_Real gp_XYZ::CrossSquareMagnitude (const gp_XYZ& theRight) const
{
  Standard_Real aXresult = y * theRight.z - z * theRight.y;
  Standard_Real aYresult = z * theRight.x - x * theRight.z;
  Standard_Real aZresult = x * theRight.y - y * theRight.x;
  return aXresult * aXresult + aYresult * aYresult + aZresult * aZresult;
}

//=======================================================================
//function : CrossCross
// purpose :
//=======================================================================
inline void gp_XYZ::CrossCross (const gp_XYZ& theCoord1, const gp_XYZ& theCoord2)
{
  Standard_Real aXresult = y * (theCoord1.x * theCoord2.y - theCoord1.y * theCoord2.x) -
                           z * (theCoord1.z * theCoord2.x - theCoord1.x * theCoord2.z);
  Standard_Real anYresult = z * (theCoord1.y * theCoord2.z - theCoord1.z * theCoord2.y) -
                            x * (theCoord1.x * theCoord2.y - theCoord1.y * theCoord2.x);
  z = x * (theCoord1.z * theCoord2.x - theCoord1.x * theCoord2.z) -
      y * (theCoord1.y * theCoord2.z - theCoord1.z * theCoord2.y);
  x = aXresult;
  y = anYresult;
}

//=======================================================================
//function : DotCross
// purpose :
//=======================================================================
inline Standard_Real gp_XYZ::DotCross (const gp_XYZ& theCoord1, const gp_XYZ& theCoord2) const
{
  Standard_Real aXresult = theCoord1.y * theCoord2.z - theCoord1.z * theCoord2.y;
  Standard_Real anYresult = theCoord1.z * theCoord2.x - theCoord1.x * theCoord2.z;
  Standard_Real aZresult = theCoord1.x * theCoord2.y - theCoord1.y * theCoord2.x;
  return (x * aXresult + y * anYresult + z * aZresult);
}

//=======================================================================
//function : Multiply
// purpose :
//=======================================================================
inline void gp_XYZ::Multiply (const gp_Mat& theMatrix)
{
  Standard_Real aXresult = theMatrix.Value (1, 1) * x + theMatrix.Value (1, 2) * y + theMatrix.Value (1, 3) * z;
  Standard_Real anYresult = theMatrix.Value (2, 1) * x + theMatrix.Value (2, 2) * y + theMatrix.Value (2, 3) * z;
  z = theMatrix.Value (3, 1) * x + theMatrix.Value (3, 2) * y + theMatrix.Value (3, 3) * z;
  x = aXresult;
  y = anYresult;
}

//=======================================================================
//function : Normalize
// purpose :
//=======================================================================
inline void gp_XYZ::Normalize()
{
  Standard_Real aD = Modulus();
  Standard_ConstructionError_Raise_if (aD <= gp::Resolution(), "gp_XYZ::Normalize() - vector has zero norm");
  x = x / aD;
  y = y / aD;
  z = z / aD;
}

//=======================================================================
//function : operator*
// purpose :
//=======================================================================
inline gp_XYZ operator* (const gp_Mat& theMatrix, const gp_XYZ& theCoord1)
{
  return theCoord1.Multiplied (theMatrix);
}

//=======================================================================
//function : operator*
// purpose :
//=======================================================================
inline gp_XYZ operator* (const Standard_Real theScalar, const gp_XYZ& theCoord1)
{
  return theCoord1.Multiplied (theScalar);
}

#endif // _gp_XYZ_HeaderFile
