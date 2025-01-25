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

//! Describes a three column, three row matrix.
//! This sort of object is used in various vectorial or matrix computations.
class gp_Mat
{
public:
  DEFINE_STANDARD_ALLOC

  //! creates  a matrix with null coefficients.
  gp_Mat()
  {
    myMat[0][0] = myMat[0][1] = myMat[0][2] = myMat[1][0] = myMat[1][1] = myMat[1][2] =
      myMat[2][0] = myMat[2][1] = myMat[2][2] = 0.0;
  }

  gp_Mat(const Standard_Real theA11,
         const Standard_Real theA12,
         const Standard_Real theA13,
         const Standard_Real theA21,
         const Standard_Real theA22,
         const Standard_Real theA23,
         const Standard_Real theA31,
         const Standard_Real theA32,
         const Standard_Real theA33);

  //! Creates a matrix.
  //! theCol1, theCol2, theCol3 are the 3 columns of the matrix.
  Standard_EXPORT gp_Mat(const gp_XYZ& theCol1, const gp_XYZ& theCol2, const gp_XYZ& theCol3);

  //! Assigns the three coordinates of theValue to the column of index
  //! theCol of this matrix.
  //! Raises OutOfRange if theCol < 1 or theCol > 3.
  Standard_EXPORT void SetCol(const Standard_Integer theCol, const gp_XYZ& theValue);

  //! Assigns the number triples theCol1, theCol2, theCol3 to the three
  //! columns of this matrix.
  Standard_EXPORT void SetCols(const gp_XYZ& theCol1, const gp_XYZ& theCol2, const gp_XYZ& theCol3);

  //! Modifies the matrix  M so that applying it to any number
  //! triple (X, Y, Z) produces the same result as the cross
  //! product of theRef and the number triple (X, Y, Z):
  //! i.e.: M * {X,Y,Z}t = theRef.Cross({X, Y ,Z})
  //! this matrix is anti symmetric. To apply this matrix to the
  //! triplet  {XYZ} is the same as to do the cross product between the
  //! triplet theRef and the triplet {XYZ}.
  //! Note: this matrix is anti-symmetric.
  Standard_EXPORT void SetCross(const gp_XYZ& theRef);

  //! Modifies the main diagonal of the matrix.
  //! @code
  //! <me>.Value (1, 1) = theX1
  //! <me>.Value (2, 2) = theX2
  //! <me>.Value (3, 3) = theX3
  //! @endcode
  //! The other coefficients of the matrix are not modified.
  void SetDiagonal(const Standard_Real theX1, const Standard_Real theX2, const Standard_Real theX3)
  {
    myMat[0][0] = theX1;
    myMat[1][1] = theX2;
    myMat[2][2] = theX3;
  }

  //! Modifies this matrix so that applying it to any number
  //! triple (X, Y, Z) produces the same result as the scalar
  //! product of theRef and the number triple (X, Y, Z):
  //! this * (X,Y,Z) = theRef.(X,Y,Z)
  //! Note: this matrix is symmetric.
  Standard_EXPORT void SetDot(const gp_XYZ& theRef);

  //! Modifies this matrix so that it represents the Identity matrix.
  void SetIdentity()
  {
    myMat[0][0] = myMat[1][1] = myMat[2][2] = 1.0;
    myMat[0][1] = myMat[0][2] = myMat[1][0] = myMat[1][2] = myMat[2][0] = myMat[2][1] = 0.0;
  }

  //! Modifies this matrix so that it represents a rotation. theAng is the angular value in
  //! radians and the XYZ axis gives the direction of the
  //! rotation.
  //! Raises ConstructionError if XYZ.Modulus() <= Resolution()
  Standard_EXPORT void SetRotation(const gp_XYZ& theAxis, const Standard_Real theAng);

  //! Assigns the three coordinates of Value to the row of index
  //! theRow of this matrix. Raises OutOfRange if theRow < 1 or theRow > 3.
  Standard_EXPORT void SetRow(const Standard_Integer theRow, const gp_XYZ& theValue);

  //! Assigns the number triples theRow1, theRow2, theRow3 to the three
  //! rows of this matrix.
  Standard_EXPORT void SetRows(const gp_XYZ& theRow1, const gp_XYZ& theRow2, const gp_XYZ& theRow3);

  //! Modifies the matrix so that it represents
  //! a scaling transformation, where theS is the scale factor. :
  //! @code
  //!         | theS    0.0  0.0 |
  //! <me> =  | 0.0   theS   0.0 |
  //!         | 0.0  0.0   theS  |
  //! @endcode
  void SetScale(const Standard_Real theS)
  {
    myMat[0][0] = myMat[1][1] = myMat[2][2] = theS;
    myMat[0][1] = myMat[0][2] = myMat[1][0] = myMat[1][2] = myMat[2][0] = myMat[2][1] = 0.0;
  }

  //! Assigns <theValue> to the coefficient of row theRow, column theCol of   this matrix.
  //! Raises OutOfRange if theRow < 1 or theRow > 3 or theCol < 1 or theCol > 3
  void SetValue(const Standard_Integer theRow,
                const Standard_Integer theCol,
                const Standard_Real    theValue)
  {
    Standard_OutOfRange_Raise_if(theRow < 1 || theRow > 3 || theCol < 1 || theCol > 3, " ");
    myMat[theRow - 1][theCol - 1] = theValue;
  }

  //! Returns the column of theCol index.
  //! Raises OutOfRange if theCol < 1 or theCol > 3
  Standard_EXPORT gp_XYZ Column(const Standard_Integer theCol) const;

  //! Computes the determinant of the matrix.
  Standard_Real Determinant() const
  {
    return myMat[0][0] * (myMat[1][1] * myMat[2][2] - myMat[2][1] * myMat[1][2])
           - myMat[0][1] * (myMat[1][0] * myMat[2][2] - myMat[2][0] * myMat[1][2])
           + myMat[0][2] * (myMat[1][0] * myMat[2][1] - myMat[2][0] * myMat[1][1]);
  }

  //! Returns the main diagonal of the matrix.
  Standard_EXPORT gp_XYZ Diagonal() const;

  //! returns the row of theRow index.
  //! Raises OutOfRange if theRow < 1 or theRow > 3
  Standard_EXPORT gp_XYZ Row(const Standard_Integer theRow) const;

  //! Returns the coefficient of range (theRow, theCol)
  //! Raises OutOfRange if theRow < 1 or theRow > 3 or theCol < 1 or theCol > 3
  const Standard_Real& Value(const Standard_Integer theRow, const Standard_Integer theCol) const
  {
    Standard_OutOfRange_Raise_if(theRow < 1 || theRow > 3 || theCol < 1 || theCol > 3, " ");
    return myMat[theRow - 1][theCol - 1];
  }

  const Standard_Real& operator()(const Standard_Integer theRow,
                                  const Standard_Integer theCol) const
  {
    return Value(theRow, theCol);
  }

  //! Returns the coefficient of range (theRow, theCol)
  //! Raises OutOfRange if theRow < 1 or theRow > 3 or theCol < 1 or theCol > 3
  Standard_Real& ChangeValue(const Standard_Integer theRow, const Standard_Integer theCol)
  {
    Standard_OutOfRange_Raise_if(theRow < 1 || theRow > 3 || theCol < 1 || theCol > 3, " ");
    return myMat[theRow - 1][theCol - 1];
  }

  Standard_Real& operator()(const Standard_Integer theRow, const Standard_Integer theCol)
  {
    return ChangeValue(theRow, theCol);
  }

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

  void Add(const gp_Mat& theOther);

  void operator+=(const gp_Mat& theOther) { Add(theOther); }

  //! Computes the sum of this matrix and
  //! the matrix theOther for each coefficient of the matrix :
  //! <me>.Coef(i,j) + <theOther>.Coef(i,j)
  Standard_NODISCARD gp_Mat Added(const gp_Mat& theOther) const;

  Standard_NODISCARD gp_Mat operator+(const gp_Mat& theOther) const { return Added(theOther); }

  void Divide(const Standard_Real theScalar);

  void operator/=(const Standard_Real theScalar) { Divide(theScalar); }

  //! Divides all the coefficients of the matrix by Scalar
  Standard_NODISCARD gp_Mat Divided(const Standard_Real theScalar) const;

  Standard_NODISCARD gp_Mat operator/(const Standard_Real theScalar) const
  {
    return Divided(theScalar);
  }

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
  Standard_NODISCARD gp_Mat Multiplied(const gp_Mat& theOther) const
  {
    gp_Mat aNewMat = *this;
    aNewMat.Multiply(theOther);
    return aNewMat;
  }

  Standard_NODISCARD gp_Mat operator*(const gp_Mat& theOther) const { return Multiplied(theOther); }

  //! Computes the product of two matrices <me> = <Other> * <me>.
  void Multiply(const gp_Mat& theOther);

  void operator*=(const gp_Mat& theOther) { Multiply(theOther); }

  void PreMultiply(const gp_Mat& theOther);

  Standard_NODISCARD gp_Mat Multiplied(const Standard_Real theScalar) const;

  Standard_NODISCARD gp_Mat operator*(const Standard_Real theScalar) const
  {
    return Multiplied(theScalar);
  }

  //! Multiplies all the coefficients of the matrix by Scalar
  void Multiply(const Standard_Real theScalar);

  void operator*=(const Standard_Real theScalar) { Multiply(theScalar); }

  Standard_EXPORT void Power(const Standard_Integer N);

  //! Computes <me> = <me> * <me> * .......* <me>,   theN time.
  //! if theN = 0 <me> = Identity
  //! if theN < 0 <me> = <me>.Invert() *...........* <me>.Invert().
  //! If theN < 0 an exception will be raised if the matrix is not
  //! inversible
  Standard_NODISCARD gp_Mat Powered(const Standard_Integer theN) const
  {
    gp_Mat aMatN = *this;
    aMatN.Power(theN);
    return aMatN;
  }

  void Subtract(const gp_Mat& theOther);

  void operator-=(const gp_Mat& theOther) { Subtract(theOther); }

  //! cOmputes for each coefficient of the matrix :
  //! <me>.Coef(i,j) - <theOther>.Coef(i,j)
  Standard_NODISCARD gp_Mat Subtracted(const gp_Mat& theOther) const;

  Standard_NODISCARD gp_Mat operator-(const gp_Mat& theOther) const { return Subtracted(theOther); }

  void Transpose();

  //! Transposes the matrix. A(j, i) -> A (i, j)
  Standard_NODISCARD gp_Mat Transposed() const
  {
    gp_Mat aNewMat = *this;
    aNewMat.Transpose();
    return aNewMat;
  }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

  friend class gp_XYZ;
  friend class gp_Trsf;
  friend class gp_GTrsf;

private:
  Standard_Real myMat[3][3];
};

//=======================================================================
// function : gp_Mat
// purpose :
//=======================================================================
inline gp_Mat::gp_Mat(const Standard_Real theA11,
                      const Standard_Real theA12,
                      const Standard_Real theA13,
                      const Standard_Real theA21,
                      const Standard_Real theA22,
                      const Standard_Real theA23,
                      const Standard_Real theA31,
                      const Standard_Real theA32,
                      const Standard_Real theA33)
{
  myMat[0][0] = theA11;
  myMat[0][1] = theA12;
  myMat[0][2] = theA13;
  myMat[1][0] = theA21;
  myMat[1][1] = theA22;
  myMat[1][2] = theA23;
  myMat[2][0] = theA31;
  myMat[2][1] = theA32;
  myMat[2][2] = theA33;
}

//=======================================================================
// function : Add
// purpose :
//=======================================================================
inline void gp_Mat::Add(const gp_Mat& theOther)
{
  myMat[0][0] += theOther.myMat[0][0];
  myMat[0][1] += theOther.myMat[0][1];
  myMat[0][2] += theOther.myMat[0][2];
  myMat[1][0] += theOther.myMat[1][0];
  myMat[1][1] += theOther.myMat[1][1];
  myMat[1][2] += theOther.myMat[1][2];
  myMat[2][0] += theOther.myMat[2][0];
  myMat[2][1] += theOther.myMat[2][1];
  myMat[2][2] += theOther.myMat[2][2];
}

//=======================================================================
// function : Added
// purpose :
//=======================================================================
inline gp_Mat gp_Mat::Added(const gp_Mat& theOther) const
{
  gp_Mat aNewMat;
  aNewMat.myMat[0][0] = myMat[0][0] + theOther.myMat[0][0];
  aNewMat.myMat[0][1] = myMat[0][1] + theOther.myMat[0][1];
  aNewMat.myMat[0][2] = myMat[0][2] + theOther.myMat[0][2];
  aNewMat.myMat[1][0] = myMat[1][0] + theOther.myMat[1][0];
  aNewMat.myMat[1][1] = myMat[1][1] + theOther.myMat[1][1];
  aNewMat.myMat[1][2] = myMat[1][2] + theOther.myMat[1][2];
  aNewMat.myMat[2][0] = myMat[2][0] + theOther.myMat[2][0];
  aNewMat.myMat[2][1] = myMat[2][1] + theOther.myMat[2][1];
  aNewMat.myMat[2][2] = myMat[2][2] + theOther.myMat[2][2];
  return aNewMat;
}

//=======================================================================
// function : Divide
// purpose :
//=======================================================================
inline void gp_Mat::Divide(const Standard_Real theScalar)
{
  Standard_Real aVal = theScalar;
  if (aVal < 0)
  {
    aVal = -aVal;
  }
  Standard_ConstructionError_Raise_if(aVal <= gp::Resolution(), "gp_Mat : Divide by 0");
  const Standard_Real anUnSurScalar = 1.0 / theScalar;
  myMat[0][0] *= anUnSurScalar;
  myMat[0][1] *= anUnSurScalar;
  myMat[0][2] *= anUnSurScalar;
  myMat[1][0] *= anUnSurScalar;
  myMat[1][1] *= anUnSurScalar;
  myMat[1][2] *= anUnSurScalar;
  myMat[2][0] *= anUnSurScalar;
  myMat[2][1] *= anUnSurScalar;
  myMat[2][2] *= anUnSurScalar;
}

//=======================================================================
// function : Divided
// purpose :
//=======================================================================
inline gp_Mat gp_Mat::Divided(const Standard_Real theScalar) const
{
  Standard_Real aVal = theScalar;
  if (aVal < 0)
  {
    aVal = -aVal;
  }
  Standard_ConstructionError_Raise_if(aVal <= gp::Resolution(), "gp_Mat : Divide by 0");
  gp_Mat              aNewMat;
  const Standard_Real anUnSurScalar = 1.0 / theScalar;
  aNewMat.myMat[0][0]               = myMat[0][0] * anUnSurScalar;
  aNewMat.myMat[0][1]               = myMat[0][1] * anUnSurScalar;
  aNewMat.myMat[0][2]               = myMat[0][2] * anUnSurScalar;
  aNewMat.myMat[1][0]               = myMat[1][0] * anUnSurScalar;
  aNewMat.myMat[1][1]               = myMat[1][1] * anUnSurScalar;
  aNewMat.myMat[1][2]               = myMat[1][2] * anUnSurScalar;
  aNewMat.myMat[2][0]               = myMat[2][0] * anUnSurScalar;
  aNewMat.myMat[2][1]               = myMat[2][1] * anUnSurScalar;
  aNewMat.myMat[2][2]               = myMat[2][2] * anUnSurScalar;
  return aNewMat;
}

//=======================================================================
// function : Multiply
// purpose :
//=======================================================================
inline void gp_Mat::Multiply(const gp_Mat& theOther)
{
  const Standard_Real aT00 = myMat[0][0] * theOther.myMat[0][0] + myMat[0][1] * theOther.myMat[1][0]
                             + myMat[0][2] * theOther.myMat[2][0];
  const Standard_Real aT01 = myMat[0][0] * theOther.myMat[0][1] + myMat[0][1] * theOther.myMat[1][1]
                             + myMat[0][2] * theOther.myMat[2][1];
  const Standard_Real aT02 = myMat[0][0] * theOther.myMat[0][2] + myMat[0][1] * theOther.myMat[1][2]
                             + myMat[0][2] * theOther.myMat[2][2];
  const Standard_Real aT10 = myMat[1][0] * theOther.myMat[0][0] + myMat[1][1] * theOther.myMat[1][0]
                             + myMat[1][2] * theOther.myMat[2][0];
  const Standard_Real aT11 = myMat[1][0] * theOther.myMat[0][1] + myMat[1][1] * theOther.myMat[1][1]
                             + myMat[1][2] * theOther.myMat[2][1];
  const Standard_Real aT12 = myMat[1][0] * theOther.myMat[0][2] + myMat[1][1] * theOther.myMat[1][2]
                             + myMat[1][2] * theOther.myMat[2][2];
  const Standard_Real aT20 = myMat[2][0] * theOther.myMat[0][0] + myMat[2][1] * theOther.myMat[1][0]
                             + myMat[2][2] * theOther.myMat[2][0];
  const Standard_Real aT21 = myMat[2][0] * theOther.myMat[0][1] + myMat[2][1] * theOther.myMat[1][1]
                             + myMat[2][2] * theOther.myMat[2][1];
  const Standard_Real aT22 = myMat[2][0] * theOther.myMat[0][2] + myMat[2][1] * theOther.myMat[1][2]
                             + myMat[2][2] * theOther.myMat[2][2];
  myMat[0][0] = aT00;
  myMat[0][1] = aT01;
  myMat[0][2] = aT02;
  myMat[1][0] = aT10;
  myMat[1][1] = aT11;
  myMat[1][2] = aT12;
  myMat[2][0] = aT20;
  myMat[2][1] = aT21;
  myMat[2][2] = aT22;
}

//=======================================================================
// function : PreMultiply
// purpose :
//=======================================================================
inline void gp_Mat::PreMultiply(const gp_Mat& theOther)
{
  const Standard_Real aT00 = theOther.myMat[0][0] * myMat[0][0] + theOther.myMat[0][1] * myMat[1][0]
                             + theOther.myMat[0][2] * myMat[2][0];
  const Standard_Real aT01 = theOther.myMat[0][0] * myMat[0][1] + theOther.myMat[0][1] * myMat[1][1]
                             + theOther.myMat[0][2] * myMat[2][1];
  const Standard_Real aT02 = theOther.myMat[0][0] * myMat[0][2] + theOther.myMat[0][1] * myMat[1][2]
                             + theOther.myMat[0][2] * myMat[2][2];
  const Standard_Real aT10 = theOther.myMat[1][0] * myMat[0][0] + theOther.myMat[1][1] * myMat[1][0]
                             + theOther.myMat[1][2] * myMat[2][0];
  const Standard_Real aT11 = theOther.myMat[1][0] * myMat[0][1] + theOther.myMat[1][1] * myMat[1][1]
                             + theOther.myMat[1][2] * myMat[2][1];
  const Standard_Real aT12 = theOther.myMat[1][0] * myMat[0][2] + theOther.myMat[1][1] * myMat[1][2]
                             + theOther.myMat[1][2] * myMat[2][2];
  const Standard_Real aT20 = theOther.myMat[2][0] * myMat[0][0] + theOther.myMat[2][1] * myMat[1][0]
                             + theOther.myMat[2][2] * myMat[2][0];
  const Standard_Real aT21 = theOther.myMat[2][0] * myMat[0][1] + theOther.myMat[2][1] * myMat[1][1]
                             + theOther.myMat[2][2] * myMat[2][1];
  const Standard_Real aT22 = theOther.myMat[2][0] * myMat[0][2] + theOther.myMat[2][1] * myMat[1][2]
                             + theOther.myMat[2][2] * myMat[2][2];
  myMat[0][0] = aT00;
  myMat[0][1] = aT01;
  myMat[0][2] = aT02;
  myMat[1][0] = aT10;
  myMat[1][1] = aT11;
  myMat[1][2] = aT12;
  myMat[2][0] = aT20;
  myMat[2][1] = aT21;
  myMat[2][2] = aT22;
}

//=======================================================================
// function : Multiplied
// purpose :
//=======================================================================
inline gp_Mat gp_Mat::Multiplied(const Standard_Real theScalar) const
{
  gp_Mat aNewMat;
  aNewMat.myMat[0][0] = theScalar * myMat[0][0];
  aNewMat.myMat[0][1] = theScalar * myMat[0][1];
  aNewMat.myMat[0][2] = theScalar * myMat[0][2];
  aNewMat.myMat[1][0] = theScalar * myMat[1][0];
  aNewMat.myMat[1][1] = theScalar * myMat[1][1];
  aNewMat.myMat[1][2] = theScalar * myMat[1][2];
  aNewMat.myMat[2][0] = theScalar * myMat[2][0];
  aNewMat.myMat[2][1] = theScalar * myMat[2][1];
  aNewMat.myMat[2][2] = theScalar * myMat[2][2];
  return aNewMat;
}

//=======================================================================
// function : Multiply
// purpose :
//=======================================================================
inline void gp_Mat::Multiply(const Standard_Real theScalar)
{
  myMat[0][0] *= theScalar;
  myMat[0][1] *= theScalar;
  myMat[0][2] *= theScalar;
  myMat[1][0] *= theScalar;
  myMat[1][1] *= theScalar;
  myMat[1][2] *= theScalar;
  myMat[2][0] *= theScalar;
  myMat[2][1] *= theScalar;
  myMat[2][2] *= theScalar;
}

//=======================================================================
// function : Subtract
// purpose :
//=======================================================================
inline void gp_Mat::Subtract(const gp_Mat& theOther)
{
  myMat[0][0] -= theOther.myMat[0][0];
  myMat[0][1] -= theOther.myMat[0][1];
  myMat[0][2] -= theOther.myMat[0][2];
  myMat[1][0] -= theOther.myMat[1][0];
  myMat[1][1] -= theOther.myMat[1][1];
  myMat[1][2] -= theOther.myMat[1][2];
  myMat[2][0] -= theOther.myMat[2][0];
  myMat[2][1] -= theOther.myMat[2][1];
  myMat[2][2] -= theOther.myMat[2][2];
}

//=======================================================================
// function : Subtracted
// purpose :
//=======================================================================
inline gp_Mat gp_Mat::Subtracted(const gp_Mat& theOther) const
{
  gp_Mat aNewMat;
  aNewMat.myMat[0][0] = myMat[0][0] - theOther.myMat[0][0];
  aNewMat.myMat[0][1] = myMat[0][1] - theOther.myMat[0][1];
  aNewMat.myMat[0][2] = myMat[0][2] - theOther.myMat[0][2];
  aNewMat.myMat[1][0] = myMat[1][0] - theOther.myMat[1][0];
  aNewMat.myMat[1][1] = myMat[1][1] - theOther.myMat[1][1];
  aNewMat.myMat[1][2] = myMat[1][2] - theOther.myMat[1][2];
  aNewMat.myMat[2][0] = myMat[2][0] - theOther.myMat[2][0];
  aNewMat.myMat[2][1] = myMat[2][1] - theOther.myMat[2][1];
  aNewMat.myMat[2][2] = myMat[2][2] - theOther.myMat[2][2];
  return aNewMat;
}

//=======================================================================
// function : Transpose
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
inline void
  gp_Mat::Transpose()
{
  Standard_Real aTemp;
  aTemp       = myMat[0][1];
  myMat[0][1] = myMat[1][0];
  myMat[1][0] = aTemp;
  aTemp       = myMat[0][2];
  myMat[0][2] = myMat[2][0];
  myMat[2][0] = aTemp;
  aTemp       = myMat[1][2];
  myMat[1][2] = myMat[2][1];
  myMat[2][1] = aTemp;
}

//=======================================================================
// function : operator*
// purpose :
//=======================================================================
inline gp_Mat operator*(const Standard_Real theScalar, const gp_Mat& theMat3D)
{
  return theMat3D.Multiplied(theScalar);
}

#endif // _gp_Mat_HeaderFile
