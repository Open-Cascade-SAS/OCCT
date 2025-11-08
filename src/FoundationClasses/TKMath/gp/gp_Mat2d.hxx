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

#ifndef _gp_Mat2d_HeaderFile
#define _gp_Mat2d_HeaderFile

#include <gp.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>

class gp_XY;

//! Describes a two column, two row matrix.
//! This sort of object is used in various vectorial or matrix computations.
class gp_Mat2d
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a matrix with null coefficients.
  constexpr gp_Mat2d() noexcept
      : myMat{{0.0, 0.0}, {0.0, 0.0}}
  {
  }

  //! theCol1, theCol2 are the 2 columns of the matrix.
  Standard_EXPORT gp_Mat2d(const gp_XY& theCol1, const gp_XY& theCol2);

  //! Assigns the two coordinates of theValue to the column of range
  //! theCol of this matrix
  //! Raises OutOfRange if theCol < 1 or theCol > 2.
  Standard_EXPORT void SetCol(const Standard_Integer theCol, const gp_XY& theValue);

  //! Assigns the number pairs theCol1, theCol2 to the two columns of this matrix
  Standard_EXPORT void SetCols(const gp_XY& theCol1, const gp_XY& theCol2);

  //! Modifies the main diagonal of the matrix.
  //! @code
  //! <me>.Value (1, 1) = theX1
  //! <me>.Value (2, 2) = theX2
  //! @endcode
  //! The other coefficients of the matrix are not modified.
  constexpr void SetDiagonal(const Standard_Real theX1, const Standard_Real theX2) noexcept
  {
    myMat[0][0] = theX1;
    myMat[1][1] = theX2;
  }

  //! Modifies this matrix, so that it represents the Identity matrix.
  constexpr void SetIdentity() noexcept
  {
    myMat[0][0] = myMat[1][1] = 1.0;
    myMat[0][1] = myMat[1][0] = 0.0;
  }

  //! Modifies this matrix, so that it represents a rotation. theAng is the angular
  //! value in radian of the rotation.
  void SetRotation(const Standard_Real theAng);

  //! Assigns the two coordinates of theValue to the row of index theRow of this matrix.
  //! Raises OutOfRange if theRow < 1 or theRow > 2.
  Standard_EXPORT void SetRow(const Standard_Integer theRow, const gp_XY& theValue);

  //! Assigns the number pairs theRow1, theRow2 to the two rows of this matrix.
  Standard_EXPORT void SetRows(const gp_XY& theRow1, const gp_XY& theRow2);

  //! Modifies the matrix such that it
  //! represents a scaling transformation, where theS is the scale factor:
  //! @code
  //!         | theS    0.0 |
  //! <me> =  | 0.0   theS  |
  //! @endcode
  constexpr void SetScale(const Standard_Real theS) noexcept
  {
    myMat[0][0] = myMat[1][1] = theS;
    myMat[0][1] = myMat[1][0] = 0.0;
  }

  //! Assigns <theValue> to the coefficient of row theRow, column theCol of this matrix.
  //! Raises OutOfRange if theRow < 1 or theRow > 2 or theCol < 1 or theCol > 2
  void SetValue(const Standard_Integer theRow,
                const Standard_Integer theCol,
                const Standard_Real    theValue)
  {
    Standard_OutOfRange_Raise_if(theRow < 1 || theRow > 2 || theCol < 1 || theCol > 2, " ");
    myMat[theRow - 1][theCol - 1] = theValue;
  }

  //! Returns the column of theCol index.
  //! Raises OutOfRange if theCol < 1 or theCol > 2
  Standard_EXPORT gp_XY Column(const Standard_Integer theCol) const;

  //! Computes the determinant of the matrix.
  constexpr Standard_Real Determinant() const noexcept
  {
    return myMat[0][0] * myMat[1][1] - myMat[1][0] * myMat[0][1];
  }

  //! Returns the main diagonal of the matrix.
  Standard_EXPORT gp_XY Diagonal() const;

  //! Returns the row of index theRow.
  //! Raised if theRow < 1 or theRow > 2
  Standard_EXPORT gp_XY Row(const Standard_Integer theRow) const;

  //! Returns the coefficient of range (ttheheRow, theCol)
  //! Raises OutOfRange
  //! if theRow < 1 or theRow > 2 or theCol < 1 or theCol > 2
  const Standard_Real& Value(const Standard_Integer theRow, const Standard_Integer theCol) const
  {
    Standard_OutOfRange_Raise_if(theRow < 1 || theRow > 2 || theCol < 1 || theCol > 2, " ");
    return myMat[theRow - 1][theCol - 1];
  }

  const Standard_Real& operator()(const Standard_Integer theRow,
                                  const Standard_Integer theCol) const
  {
    return Value(theRow, theCol);
  }

  //! Returns the coefficient of range (theRow, theCol)
  //! Raises OutOfRange
  //! if theRow < 1 or theRow > 2 or theCol < 1 or theCol > 2
  Standard_Real& ChangeValue(const Standard_Integer theRow, const Standard_Integer theCol)
  {
    Standard_OutOfRange_Raise_if(theRow < 1 || theRow > 2 || theCol < 1 || theCol > 2, " ");
    return myMat[theRow - 1][theCol - 1];
  }

  Standard_Real& operator()(const Standard_Integer theRow, const Standard_Integer theCol)
  {
    return ChangeValue(theRow, theCol);
  }

  //! Returns true if this matrix is singular (and therefore, cannot be inverted).
  //! The Gauss LU decomposition is used to invert the matrix
  //! so the matrix is considered as singular if the largest
  //! pivot found is lower or equal to Resolution from gp.
  constexpr Standard_Boolean IsSingular() const noexcept
  {
    Standard_Real aDet = Determinant();
    if (aDet < 0)
    {
      aDet = -aDet;
    }
    return aDet <= gp::Resolution();
  }

  constexpr void Add(const gp_Mat2d& Other) noexcept;

  constexpr void operator+=(const gp_Mat2d& theOther) noexcept { Add(theOther); }

  //! Computes the sum of this matrix and the matrix
  //! theOther.for each coefficient of the matrix :
  //! @code
  //! <me>.Coef(i,j) + <theOther>.Coef(i,j)
  //! @endcode
  //! Note:
  //! -   operator += assigns the result to this matrix, while
  //! -   operator + creates a new one.
  Standard_NODISCARD constexpr gp_Mat2d Added(const gp_Mat2d& theOther) const noexcept;

  Standard_NODISCARD constexpr gp_Mat2d operator+(const gp_Mat2d& theOther) const noexcept
  {
    return Added(theOther);
  }

  constexpr void Divide(const Standard_Real theScalar);

  constexpr void operator/=(const Standard_Real theScalar) { Divide(theScalar); }

  //! Divides all the coefficients of the matrix by a scalar.
  Standard_NODISCARD constexpr gp_Mat2d Divided(const Standard_Real theScalar) const;

  Standard_NODISCARD constexpr gp_Mat2d operator/(const Standard_Real theScalar) const
  {
    return Divided(theScalar);
  }

  Standard_EXPORT void Invert();

  //! Inverses the matrix and raises exception if the matrix
  //! is singular.
  Standard_NODISCARD gp_Mat2d Inverted() const
  {
    gp_Mat2d aNewMat = *this;
    aNewMat.Invert();
    return aNewMat;
  }

  Standard_NODISCARD constexpr gp_Mat2d Multiplied(const gp_Mat2d& theOther) const noexcept
  {
    gp_Mat2d aNewMat2d = *this;
    aNewMat2d.Multiply(theOther);
    return aNewMat2d;
  }

  Standard_NODISCARD constexpr gp_Mat2d operator*(const gp_Mat2d& theOther) const noexcept
  {
    return Multiplied(theOther);
  }

  //! Computes the product of two matrices <me> * <theOther>
  constexpr void Multiply(const gp_Mat2d& theOther) noexcept;

  //! Modifies this matrix by premultiplying it by the matrix Other
  //! <me> = theOther * <me>.
  constexpr void PreMultiply(const gp_Mat2d& theOther) noexcept;

  Standard_NODISCARD constexpr gp_Mat2d Multiplied(const Standard_Real theScalar) const noexcept;

  Standard_NODISCARD constexpr gp_Mat2d operator*(const Standard_Real theScalar) const noexcept
  {
    return Multiplied(theScalar);
  }

  //! Multiplies all the coefficients of the matrix by a scalar.
  constexpr void Multiply(const Standard_Real theScalar) noexcept;

  constexpr void operator*=(const Standard_Real theScalar) noexcept { Multiply(theScalar); }

  Standard_EXPORT void Power(const Standard_Integer theN);

  //! computes <me> = <me> * <me> * .......* <me>, theN time.
  //! if theN = 0 <me> = Identity
  //! if theN < 0 <me> = <me>.Invert() *...........* <me>.Invert().
  //! If theN < 0 an exception can be raised if the matrix is not
  //! inversible
  Standard_NODISCARD gp_Mat2d Powered(const Standard_Integer theN) const
  {
    gp_Mat2d aMat2dN = *this;
    aMat2dN.Power(theN);
    return aMat2dN;
  }

  constexpr void Subtract(const gp_Mat2d& theOther) noexcept;

  constexpr void operator-=(const gp_Mat2d& theOther) noexcept { Subtract(theOther); }

  //! Computes for each coefficient of the matrix :
  //! @code
  //! <me>.Coef(i,j) - <theOther>.Coef(i,j)
  //! @endcode
  Standard_NODISCARD constexpr gp_Mat2d Subtracted(const gp_Mat2d& theOther) const noexcept;

  Standard_NODISCARD constexpr gp_Mat2d operator-(const gp_Mat2d& theOther) const noexcept
  {
    return Subtracted(theOther);
  }

  constexpr void Transpose() noexcept;

  //! Transposes the matrix. A(j, i) -> A (i, j)
  Standard_NODISCARD constexpr gp_Mat2d Transposed() const noexcept;

  friend class gp_Trsf2d;
  friend class gp_GTrsf2d;
  friend class gp_XY;

private:
  Standard_Real myMat[2][2];
};

//=================================================================================================

inline void gp_Mat2d::SetRotation(const Standard_Real theAng)
{
  Standard_Real aSinA = sin(theAng);
  Standard_Real aCosA = cos(theAng);
  myMat[0][0] = myMat[1][1] = aCosA;
  myMat[0][1]               = -aSinA;
  myMat[1][0]               = aSinA;
}

//=================================================================================================

inline constexpr void gp_Mat2d::Add(const gp_Mat2d& theOther) noexcept
{
  myMat[0][0] += theOther.myMat[0][0];
  myMat[0][1] += theOther.myMat[0][1];
  myMat[1][0] += theOther.myMat[1][0];
  myMat[1][1] += theOther.myMat[1][1];
}

//=================================================================================================

inline constexpr gp_Mat2d gp_Mat2d::Added(const gp_Mat2d& theOther) const noexcept
{
  gp_Mat2d aNewMat2d;
  aNewMat2d.myMat[0][0] = myMat[0][0] + theOther.myMat[0][0];
  aNewMat2d.myMat[0][1] = myMat[0][1] + theOther.myMat[0][1];
  aNewMat2d.myMat[1][0] = myMat[1][0] + theOther.myMat[1][0];
  aNewMat2d.myMat[1][1] = myMat[1][1] + theOther.myMat[1][1];
  return aNewMat2d;
}

//=================================================================================================

inline constexpr void gp_Mat2d::Divide(const Standard_Real theScalar)
{
  myMat[0][0] /= theScalar;
  myMat[0][1] /= theScalar;
  myMat[1][0] /= theScalar;
  myMat[1][1] /= theScalar;
}

//=================================================================================================

inline constexpr gp_Mat2d gp_Mat2d::Divided(const Standard_Real theScalar) const
{
  gp_Mat2d aNewMat2d;
  aNewMat2d.myMat[0][0] = myMat[0][0] / theScalar;
  aNewMat2d.myMat[0][1] = myMat[0][1] / theScalar;
  aNewMat2d.myMat[1][0] = myMat[1][0] / theScalar;
  aNewMat2d.myMat[1][1] = myMat[1][1] / theScalar;
  return aNewMat2d;
}

//=================================================================================================

inline constexpr void gp_Mat2d::Multiply(const gp_Mat2d& theOther) noexcept
{
  const Standard_Real aT00 =
    myMat[0][0] * theOther.myMat[0][0] + myMat[0][1] * theOther.myMat[1][0];
  const Standard_Real aT10 =
    myMat[1][0] * theOther.myMat[0][0] + myMat[1][1] * theOther.myMat[1][0];
  myMat[0][1] = myMat[0][0] * theOther.myMat[0][1] + myMat[0][1] * theOther.myMat[1][1];
  myMat[1][1] = myMat[1][0] * theOther.myMat[0][1] + myMat[1][1] * theOther.myMat[1][1];
  myMat[0][0] = aT00;
  myMat[1][0] = aT10;
}

//=================================================================================================

inline constexpr void gp_Mat2d::PreMultiply(const gp_Mat2d& theOther) noexcept
{
  const Standard_Real aT00 =
    theOther.myMat[0][0] * myMat[0][0] + theOther.myMat[0][1] * myMat[1][0];
  myMat[1][0] = theOther.myMat[1][0] * myMat[0][0] + theOther.myMat[1][1] * myMat[1][0];
  const Standard_Real aT01 =
    theOther.myMat[0][0] * myMat[0][1] + theOther.myMat[0][1] * myMat[1][1];
  myMat[1][1] = theOther.myMat[1][0] * myMat[0][1] + theOther.myMat[1][1] * myMat[1][1];
  myMat[0][0] = aT00;
  myMat[0][1] = aT01;
}

//=================================================================================================

inline constexpr gp_Mat2d gp_Mat2d::Multiplied(const Standard_Real theScalar) const noexcept
{
  gp_Mat2d aNewMat2d;
  aNewMat2d.myMat[0][0] = myMat[0][0] * theScalar;
  aNewMat2d.myMat[0][1] = myMat[0][1] * theScalar;
  aNewMat2d.myMat[1][0] = myMat[1][0] * theScalar;
  aNewMat2d.myMat[1][1] = myMat[1][1] * theScalar;
  return aNewMat2d;
}

//=================================================================================================

inline constexpr void gp_Mat2d::Multiply(const Standard_Real theScalar) noexcept
{
  myMat[0][0] *= theScalar;
  myMat[0][1] *= theScalar;
  myMat[1][0] *= theScalar;
  myMat[1][1] *= theScalar;
}

//=================================================================================================

inline constexpr void gp_Mat2d::Subtract(const gp_Mat2d& theOther) noexcept
{
  myMat[0][0] -= theOther.myMat[0][0];
  myMat[0][1] -= theOther.myMat[0][1];
  myMat[1][0] -= theOther.myMat[1][0];
  myMat[1][1] -= theOther.myMat[1][1];
}

//=================================================================================================

inline constexpr gp_Mat2d gp_Mat2d::Subtracted(const gp_Mat2d& theOther) const noexcept
{
  gp_Mat2d aNewMat2d;
  aNewMat2d.myMat[0][0] = myMat[0][0] - theOther.myMat[0][0];
  aNewMat2d.myMat[0][1] = myMat[0][1] - theOther.myMat[0][1];
  aNewMat2d.myMat[1][0] = myMat[1][0] - theOther.myMat[1][0];
  aNewMat2d.myMat[1][1] = myMat[1][1] - theOther.myMat[1][1];
  return aNewMat2d;
}

//=================================================================================================

inline constexpr void gp_Mat2d::Transpose() noexcept
{
  const Standard_Real aTemp = myMat[0][1];
  myMat[0][1]               = myMat[1][0];
  myMat[1][0]               = aTemp;
}

//=================================================================================================

inline constexpr gp_Mat2d gp_Mat2d::Transposed() const noexcept
{
  gp_Mat2d aNewMat2d;
  aNewMat2d.myMat[1][0] = myMat[0][1];
  aNewMat2d.myMat[0][1] = myMat[1][0];
  aNewMat2d.myMat[0][0] = myMat[0][0];
  aNewMat2d.myMat[1][1] = myMat[1][1];
  return aNewMat2d;
}

//=======================================================================
// function : operator*
// purpose :
//=======================================================================
inline constexpr gp_Mat2d operator*(const Standard_Real theScalar,
                                    const gp_Mat2d&     theMat2D) noexcept
{
  return theMat2D.Multiplied(theScalar);
}

#endif // _gp_Mat2d_HeaderFile
