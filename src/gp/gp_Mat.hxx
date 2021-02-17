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

#ifndef _gp_Mat_HeaderFile
#define _gp_Mat_HeaderFile

#include <gp.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_OStream.hxx>
#include <Standard_ConstructionError.hxx>

class gp_XYZ;
class gp_Trsf;
class gp_GTrsf;

#define Mat00 matrix[0][0]
#define Mat01 matrix[0][1]
#define Mat02 matrix[0][2]
#define Mat10 matrix[1][0]
#define Mat11 matrix[1][1]
#define Mat12 matrix[1][2]
#define Mat20 matrix[2][0]
#define Mat21 matrix[2][1]
#define Mat22 matrix[2][2]

#define Nat00 aNewMat.matrix[0][0]
#define Nat01 aNewMat.matrix[0][1]
#define Nat02 aNewMat.matrix[0][2]
#define Nat10 aNewMat.matrix[1][0]
#define Nat11 aNewMat.matrix[1][1]
#define Nat12 aNewMat.matrix[1][2]
#define Nat20 aNewMat.matrix[2][0]
#define Nat21 aNewMat.matrix[2][1]
#define Nat22 aNewMat.matrix[2][2]

#define Oat00 theOther.matrix[0][0]
#define Oat01 theOther.matrix[0][1]
#define Oat02 theOther.matrix[0][2]
#define Oat10 theOther.matrix[1][0]
#define Oat11 theOther.matrix[1][1]
#define Oat12 theOther.matrix[1][2]
#define Oat20 theOther.matrix[2][0]
#define Oat21 theOther.matrix[2][1]
#define Oat22 theOther.matrix[2][2]

//! Describes a three column, three row matrix. This sort of
//! object is used in various vectorial or matrix computations.
class gp_Mat 
{
public:

  DEFINE_STANDARD_ALLOC

  //! creates  a matrix with null coefficients.
  gp_Mat()
  {
    Mat00 = Mat01 = Mat02 =
    Mat10 = Mat11 = Mat12 =
    Mat20 = Mat21 = Mat22 = 0.0;
  }

  gp_Mat (const Standard_Real theA11, const Standard_Real theA12, const Standard_Real theA13,
          const Standard_Real theA21, const Standard_Real theA22, const Standard_Real theA23,
          const Standard_Real theA31, const Standard_Real theA32, const Standard_Real theA33);

  //! Creates a matrix.
  //! theCol1, theCol2, theCol3 are the 3 columns of the matrix.
  Standard_EXPORT gp_Mat (const gp_XYZ& theCol1, const gp_XYZ& theCol2, const gp_XYZ& theCol3);

  //! Assigns the three coordinates of theValue to the column of index
  //! theCol of this matrix.
  //! Raises OutOfRange if theCol < 1 or theCol > 3.
  Standard_EXPORT void SetCol (const Standard_Integer theCol, const gp_XYZ& theValue);

  //! Assigns the number triples theCol1, theCol2, theCol3 to the three
  //! columns of this matrix.
  Standard_EXPORT void SetCols (const gp_XYZ& theCol1, const gp_XYZ& theCol2, const gp_XYZ& theCol3);

  //! Modifies the matrix  M so that applying it to any number
  //! triple (X, Y, Z) produces the same result as the cross
  //! product of theRef and the number triple (X, Y, Z):
  //! i.e.: M * {X,Y,Z}t = theRef.Cross({X, Y ,Z})
  //! this matrix is anti symmetric. To apply this matrix to the
  //! triplet  {XYZ} is the same as to do the cross product between the
  //! triplet theRef and the triplet {XYZ}.
  //! Note: this matrix is anti-symmetric.
  Standard_EXPORT void SetCross (const gp_XYZ& theRef);

  //! Modifies the main diagonal of the matrix.
  //! @code
  //! <me>.Value (1, 1) = theX1
  //! <me>.Value (2, 2) = theX2
  //! <me>.Value (3, 3) = theX3
  //! @endcode
  //! The other coefficients of the matrix are not modified.
  void SetDiagonal (const Standard_Real theX1, const Standard_Real theX2, const Standard_Real theX3)
  {
    Mat00 = theX1;
    Mat11 = theX2;
    Mat22 = theX3;
  }

  //! Modifies this matrix so that applying it to any number
  //! triple (X, Y, Z) produces the same result as the scalar
  //! product of theRef and the number triple (X, Y, Z):
  //! this * (X,Y,Z) = theRef.(X,Y,Z)
  //! Note: this matrix is symmetric.
  Standard_EXPORT void SetDot (const gp_XYZ& theRef);

  //! Modifies this matrix so that it represents the Identity matrix.
  void SetIdentity()
  {
    Mat00 = Mat11 = Mat22 = 1.0;
    Mat01 = Mat02 = Mat10 = Mat12 = Mat20 = Mat21 = 0.0;
  }

  //! Modifies this matrix so that it represents a rotation. theAng is the angular value in
  //! radians and the XYZ axis gives the direction of the
  //! rotation.
  //! Raises ConstructionError if XYZ.Modulus() <= Resolution()
  Standard_EXPORT void SetRotation (const gp_XYZ& theAxis, const Standard_Real theAng);

  //! Assigns the three coordinates of Value to the row of index
  //! theRow of this matrix. Raises OutOfRange if theRow < 1 or theRow > 3.
  Standard_EXPORT void SetRow (const Standard_Integer theRow, const gp_XYZ& theValue);

  //! Assigns the number triples theRow1, theRow2, theRow3 to the three
  //! rows of this matrix.
  Standard_EXPORT void SetRows (const gp_XYZ& theRow1, const gp_XYZ& theRow2, const gp_XYZ& theRow3);

  //! Modifies the matrix so that it represents
  //! a scaling transformation, where theS is the scale factor. :
  //! @code
  //!         | theS    0.0  0.0 |
  //! <me> =  | 0.0   theS   0.0 |
  //!         | 0.0  0.0   theS  |
  //! @endcode
  void SetScale (const Standard_Real theS)
  {
    Mat00 = Mat11 = Mat22 = theS;
    Mat01 = Mat02 = Mat10 = Mat12 = Mat20 = Mat21 = 0.0;
  }

  //! Assigns <theValue> to the coefficient of row theRow, column theCol of   this matrix.
  //! Raises OutOfRange if theRow < 1 or theRow > 3 or theCol < 1 or theCol > 3
  void SetValue (const Standard_Integer theRow, const Standard_Integer theCol, const Standard_Real theValue)
  {
    Standard_OutOfRange_Raise_if (theRow < 1 || theRow > 3 || theCol < 1 || theCol > 3, " ");
    matrix[theRow - 1][theCol - 1] = theValue;
  }

  //! Returns the column of theCol index.
  //! Raises OutOfRange if theCol < 1 or theCol > 3
  Standard_EXPORT gp_XYZ Column (const Standard_Integer theCol) const;

  //! Computes the determinant of the matrix.
  Standard_Real Determinant() const
  {
    return Mat00 * (Mat11 * Mat22 - Mat21 * Mat12) -
           Mat01 * (Mat10 * Mat22 - Mat20 * Mat12) +
           Mat02 * (Mat10 * Mat21 - Mat20 * Mat11);
  }

  //! Returns the main diagonal of the matrix.
  Standard_EXPORT gp_XYZ Diagonal() const;

  //! returns the row of theRow index.
  //! Raises OutOfRange if theRow < 1 or theRow > 3
  Standard_EXPORT gp_XYZ Row (const Standard_Integer theRow) const;

  //! Returns the coefficient of range (theRow, theCol)
  //! Raises OutOfRange if theRow < 1 or theRow > 3 or theCol < 1 or theCol > 3
  const Standard_Real& Value (const Standard_Integer theRow, const Standard_Integer theCol) const
  {
    Standard_OutOfRange_Raise_if (theRow < 1 || theRow > 3 || theCol < 1 || theCol > 3, " ");
    return matrix[theRow - 1][theCol - 1];
  }

  const Standard_Real& operator() (const Standard_Integer theRow, const Standard_Integer theCol) const { return Value (theRow, theCol); }

  //! Returns the coefficient of range (theRow, theCol)
  //! Raises OutOfRange if theRow < 1 or theRow > 3 or theCol < 1 or theCol > 3
  Standard_Real& ChangeValue (const Standard_Integer theRow, const Standard_Integer theCol)
  {
    Standard_OutOfRange_Raise_if (theRow < 1 || theRow > 3 || theCol < 1 || theCol > 3, " ");
    return matrix[theRow - 1][theCol - 1];
  }

  Standard_Real& operator() (const Standard_Integer theRow, const Standard_Integer theCol) { return ChangeValue (theRow, theCol); }

  //! The Gauss LU decomposition is used to invert the matrix
  //! (see Math package) so the matrix is considered as singular if
  //! the largest pivot found is lower or equal to Resolution from gp.
  Standard_Boolean IsSingular() const
  {
    // Pour etre sur que Gauss va fonctionner, il faut faire Gauss ...
    Standard_Real aVal = Determinant();
    if (aVal < 0)
    {
      aVal = -aVal;
    }
    return aVal <= gp::Resolution();
  }

  void Add (const gp_Mat& theOther);

  void operator += (const gp_Mat& theOther) { Add (theOther); }

  //! Computes the sum of this matrix and
  //! the matrix theOther for each coefficient of the matrix :
  //! <me>.Coef(i,j) + <theOther>.Coef(i,j)
  Standard_NODISCARD gp_Mat Added (const gp_Mat& theOther) const;

  Standard_NODISCARD gp_Mat operator + (const gp_Mat& theOther) const { return Added (theOther); }

  void Divide (const Standard_Real theScalar);

  void operator /= (const Standard_Real theScalar) { Divide (theScalar); }

  //! Divides all the coefficients of the matrix by Scalar
  Standard_NODISCARD gp_Mat Divided (const Standard_Real theScalar) const;

  Standard_NODISCARD gp_Mat operator / (const Standard_Real theScalar) const { return Divided (theScalar); }

  Standard_EXPORT void Invert();

  //! Inverses the matrix and raises if the matrix is singular.
  //! -   Invert assigns the result to this matrix, while
  //! -   Inverted creates a new one.
  //! Warning
  //! The Gauss LU decomposition is used to invert the matrix.
  //! Consequently, the matrix is considered as singular if the
  //! largest pivot found is less than or equal to gp::Resolution().
  //! Exceptions
  //! Standard_ConstructionError if this matrix is singular,
  //! and therefore cannot be inverted.
  Standard_NODISCARD Standard_EXPORT gp_Mat Inverted() const;

  //! Computes  the product of two matrices <me> * <Other>
  Standard_NODISCARD gp_Mat Multiplied (const gp_Mat& theOther) const
  {
    gp_Mat aNewMat = *this;
    aNewMat.Multiply (theOther);
    return aNewMat;
  }

  Standard_NODISCARD gp_Mat operator * (const gp_Mat& theOther) const { return Multiplied (theOther); }

  //! Computes the product of two matrices <me> = <Other> * <me>.
  void Multiply (const gp_Mat& theOther);

  void operator *= (const gp_Mat& theOther) { Multiply (theOther); }

  void PreMultiply (const gp_Mat& theOther);

  Standard_NODISCARD gp_Mat Multiplied (const Standard_Real theScalar) const;

  Standard_NODISCARD gp_Mat operator * (const Standard_Real theScalar) const { return Multiplied (theScalar); }

  //! Multiplies all the coefficients of the matrix by Scalar
  void Multiply (const Standard_Real theScalar);

  void operator *= (const Standard_Real theScalar) { Multiply (theScalar); }

  Standard_EXPORT void Power (const Standard_Integer N);

  //! Computes <me> = <me> * <me> * .......* <me>,   theN time.
  //! if theN = 0 <me> = Identity
  //! if theN < 0 <me> = <me>.Invert() *...........* <me>.Invert().
  //! If theN < 0 an exception will be raised if the matrix is not
  //! inversible
  Standard_NODISCARD gp_Mat Powered (const Standard_Integer theN) const
  {
    gp_Mat aMatN = *this;
    aMatN.Power (theN);
    return aMatN;
  }

  void Subtract (const gp_Mat& theOther);

  void operator -= (const gp_Mat& theOther) { Subtract (theOther); }

  //! cOmputes for each coefficient of the matrix :
  //! <me>.Coef(i,j) - <theOther>.Coef(i,j)
  Standard_NODISCARD gp_Mat Subtracted (const gp_Mat& theOther) const;

  Standard_NODISCARD gp_Mat operator - (const gp_Mat& theOther) const { return Subtracted (theOther); }

  void Transpose();

  //! Transposes the matrix. A(j, i) -> A (i, j)
  Standard_NODISCARD gp_Mat Transposed() const
  {
    gp_Mat aNewMat = *this;
    aNewMat.Transpose();
    return aNewMat;
  }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

friend class gp_XYZ;
friend class gp_Trsf;
friend class gp_GTrsf;

private:

  Standard_Real matrix[3][3];

};

//=======================================================================
//function : gp_Mat
// purpose :
//=======================================================================
inline gp_Mat::gp_Mat (const Standard_Real theA11, const Standard_Real theA12, const Standard_Real theA13,
                       const Standard_Real theA21, const Standard_Real theA22, const Standard_Real theA23,
                       const Standard_Real theA31, const Standard_Real theA32, const Standard_Real theA33)
  {
    Mat00 = theA11;
    Mat01 = theA12;
    Mat02 = theA13;
    Mat10 = theA21;
    Mat11 = theA22;
    Mat12 = theA23;
    Mat20 = theA31;
    Mat21 = theA32;
    Mat22 = theA33;
  }

//=======================================================================
//function : Add
// purpose :
//=======================================================================
inline void gp_Mat::Add (const gp_Mat& theOther)
{
  Mat00 = Mat00 + Oat00;
  Mat01 = Mat01 + Oat01;
  Mat02 = Mat02 + Oat02;
  Mat10 = Mat10 + Oat10;
  Mat11 = Mat11 + Oat11;
  Mat12 = Mat12 + Oat12;
  Mat20 = Mat20 + Oat20;
  Mat21 = Mat21 + Oat21;
  Mat22 = Mat22 + Oat22;
}

//=======================================================================
//function : Added
// purpose :
//=======================================================================
inline gp_Mat gp_Mat::Added (const gp_Mat& theOther) const
{
  gp_Mat aNewMat;
  Nat00 = Mat00 + Oat00;
  Nat01 = Mat01 + Oat01;
  Nat02 = Mat02 + Oat02;
  Nat10 = Mat10 + Oat10;
  Nat11 = Mat11 + Oat11;
  Nat12 = Mat12 + Oat12;
  Nat20 = Mat20 + Oat20;
  Nat21 = Mat21 + Oat21;
  Nat22 = Mat22 + Oat22;
  return aNewMat;
}

//=======================================================================
//function : Divide
// purpose :
//=======================================================================
inline void gp_Mat::Divide (const Standard_Real theScalar)
{
  Standard_Real aVal = theScalar;
  if (aVal < 0)
  {
    aVal = -aVal;
  }
  Standard_ConstructionError_Raise_if (aVal <= gp::Resolution(),"gp_Mat : Divide by 0");
  Standard_Real anUnSurScalar = 1.0 / theScalar;
  Mat00 *= anUnSurScalar;
  Mat01 *= anUnSurScalar;
  Mat02 *= anUnSurScalar;
  Mat10 *= anUnSurScalar;
  Mat11 *= anUnSurScalar;
  Mat12 *= anUnSurScalar;
  Mat20 *= anUnSurScalar;
  Mat21 *= anUnSurScalar;
  Mat22 *= anUnSurScalar;
}

//=======================================================================
//function : Divided
// purpose :
//=======================================================================
inline gp_Mat gp_Mat::Divided (const Standard_Real theScalar) const
{
  Standard_Real aVal = theScalar;
  if (aVal < 0)
  {
    aVal = -aVal;
  }
  Standard_ConstructionError_Raise_if (aVal <= gp::Resolution(),"gp_Mat : Divide by 0");
  gp_Mat aNewMat;
  Standard_Real anUnSurScalar = 1.0 / theScalar;
  Nat00 = Mat00 * anUnSurScalar;
  Nat01 = Mat01 * anUnSurScalar;
  Nat02 = Mat02 * anUnSurScalar;
  Nat10 = Mat10 * anUnSurScalar;
  Nat11 = Mat11 * anUnSurScalar;
  Nat12 = Mat12 * anUnSurScalar;
  Nat20 = Mat20 * anUnSurScalar;
  Nat21 = Mat21 * anUnSurScalar;
  Nat22 = Mat22 * anUnSurScalar;
  return aNewMat;
}

//=======================================================================
//function : Multiply
// purpose :
//=======================================================================
inline void gp_Mat::Multiply (const gp_Mat& theOther)
{
  Standard_Real aT00, aT01, aT02, aT10, aT11, aT12, aT20, aT21, aT22;
  aT00 = Mat00 * Oat00 + Mat01 * Oat10 + Mat02 * Oat20;
  aT01 = Mat00 * Oat01 + Mat01 * Oat11 + Mat02 * Oat21;
  aT02 = Mat00 * Oat02 + Mat01 * Oat12 + Mat02 * Oat22;
  aT10 = Mat10 * Oat00 + Mat11 * Oat10 + Mat12 * Oat20;
  aT11 = Mat10 * Oat01 + Mat11 * Oat11 + Mat12 * Oat21;
  aT12 = Mat10 * Oat02 + Mat11 * Oat12 + Mat12 * Oat22;
  aT20 = Mat20 * Oat00 + Mat21 * Oat10 + Mat22 * Oat20;
  aT21 = Mat20 * Oat01 + Mat21 * Oat11 + Mat22 * Oat21;
  aT22 = Mat20 * Oat02 + Mat21 * Oat12 + Mat22 * Oat22;
  Mat00 = aT00;
  Mat01 = aT01;
  Mat02 = aT02;
  Mat10 = aT10;
  Mat11 = aT11;
  Mat12 = aT12;
  Mat20 = aT20;
  Mat21 = aT21;
  Mat22 = aT22;
}

//=======================================================================
//function : PreMultiply
// purpose :
//=======================================================================
inline void gp_Mat::PreMultiply (const gp_Mat& theOther)
{
  Standard_Real aT00, aT01, aT02, aT10, aT11, aT12, aT20, aT21, aT22;
  aT00 = Oat00 * Mat00 + Oat01 * Mat10 + Oat02 * Mat20;
  aT01 = Oat00 * Mat01 + Oat01 * Mat11 + Oat02 * Mat21;
  aT02 = Oat00 * Mat02 + Oat01 * Mat12 + Oat02 * Mat22;
  aT10 = Oat10 * Mat00 + Oat11 * Mat10 + Oat12 * Mat20;
  aT11 = Oat10 * Mat01 + Oat11 * Mat11 + Oat12 * Mat21;
  aT12 = Oat10 * Mat02 + Oat11 * Mat12 + Oat12 * Mat22;
  aT20 = Oat20 * Mat00 + Oat21 * Mat10 + Oat22 * Mat20;
  aT21 = Oat20 * Mat01 + Oat21 * Mat11 + Oat22 * Mat21;
  aT22 = Oat20 * Mat02 + Oat21 * Mat12 + Oat22 * Mat22;
  Mat00 = aT00;
  Mat01 = aT01;
  Mat02 = aT02;
  Mat10 = aT10;
  Mat11 = aT11;
  Mat12 = aT12;
  Mat20 = aT20;
  Mat21 = aT21;
  Mat22 = aT22;
}

//=======================================================================
//function : Multiplied
// purpose :
//=======================================================================
inline gp_Mat gp_Mat::Multiplied (const Standard_Real theScalar) const
{
  gp_Mat aNewMat;
  Nat00 = theScalar * Mat00;
  Nat01 = theScalar * Mat01;
  Nat02 = theScalar * Mat02;
  Nat10 = theScalar * Mat10;
  Nat11 = theScalar * Mat11;
  Nat12 = theScalar * Mat12;
  Nat20 = theScalar * Mat20;
  Nat21 = theScalar * Mat21;
  Nat22 = theScalar * Mat22;
  return aNewMat;
}

//=======================================================================
//function : Multiply
// purpose :
//=======================================================================
inline void gp_Mat::Multiply (const Standard_Real theScalar)
{
  Mat00 *= theScalar;
  Mat01 *= theScalar;
  Mat02 *= theScalar;
  Mat10 *= theScalar;
  Mat11 *= theScalar;
  Mat12 *= theScalar;
  Mat20 *= theScalar;
  Mat21 *= theScalar;
  Mat22 *= theScalar;
}

//=======================================================================
//function : Subtract
// purpose :
//=======================================================================
inline void gp_Mat::Subtract (const gp_Mat& theOther)
{
  Mat00 -= Oat00;
  Mat01 -= Oat01;
  Mat02 -= Oat02;
  Mat10 -= Oat10;
  Mat11 -= Oat11;
  Mat12 -= Oat12;
  Mat20 -= Oat20;
  Mat21 -= Oat21;
  Mat22 -= Oat22;
}

//=======================================================================
//function : Subtracted
// purpose :
//=======================================================================
inline gp_Mat gp_Mat::Subtracted (const gp_Mat& theOther) const
{
  gp_Mat aNewMat;
  Nat00 = Mat00 - Oat00;
  Nat01 = Mat01 - Oat01;
  Nat02 = Mat02 - Oat02;
  Nat10 = Mat10 - Oat10;
  Nat11 = Mat11 - Oat11;
  Nat12 = Mat12 - Oat12;
  Nat20 = Mat20 - Oat20;
  Nat21 = Mat21 - Oat21;
  Nat22 = Mat22 - Oat22;
  return aNewMat;
}

//=======================================================================
//function : Transpose
// purpose :
//=======================================================================
// On macOS 10.13.6 with XCode 9.4.1 the compiler has a bug leading to 
// generation of invalid code when method gp_Mat::Transpose() is called 
// for a matrix which is when applied to vector; it looks like vector
// is transformed before the matrix is actually transposed; see #29978.
// To avoid this, we disable compiler optimization here.
#if defined(__APPLE__) && (__apple_build_version__ > 9020000)
__attribute__((optnone))
#endif
inline void gp_Mat::Transpose()
{
  Standard_Real aTemp;
  aTemp  = Mat01;
  Mat01  = Mat10;
  Mat10  = aTemp;
  aTemp  = Mat02;
  Mat02  = Mat20;
  Mat20  = aTemp;
  aTemp  = Mat12;
  Mat12  = Mat21;
  Mat21  = aTemp;
}

//=======================================================================
//function : operator*
// purpose :
//=======================================================================
inline gp_Mat operator* (const Standard_Real theScalar,
                         const gp_Mat& theMat3D)
{
  return theMat3D.Multiplied (theScalar);
}

#endif // _gp_Mat_HeaderFile
