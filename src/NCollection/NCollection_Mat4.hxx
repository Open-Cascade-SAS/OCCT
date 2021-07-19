// Created on: 2013-05-30
// Created by: Anton POLETAEV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef _NCollection_Mat4_HeaderFile
#define _NCollection_Mat4_HeaderFile

#include <NCollection_Vec4.hxx>
#include <NCollection_Mat3.hxx>

//! Generic matrix of 4 x 4 elements.
//! To be used in conjunction with NCollection_Vec4 entities.
//! Originally introduced for 3D space projection and orientation operations.
//! Warning, empty constructor returns an identity matrix.
template<typename Element_t>
class NCollection_Mat4
{
public:

  //! Get number of rows.
  //! @return number of rows.
  static size_t Rows()
  {
    return 4;
  }

  //! Get number of columns.
  //! @retur number of columns.
  static size_t Cols()
  {
    return 4;
  }

  //! Return identity matrix.
  static NCollection_Mat4 Identity()
  {
    return NCollection_Mat4();
  }

  //! Return zero matrix.
  static NCollection_Mat4 Zero()
  {
    NCollection_Mat4 aMat; aMat.InitZero();
    return aMat;
  }

public:

  //! Empty constructor.
  //! Construct the identity matrix.
  NCollection_Mat4()
  {
    InitIdentity();
  }

  //! Conversion constructor (explicitly converts some 4 x 4 matrix with other element type
  //! to a new 4 x 4 matrix with the element type Element_t,
  //! whose elements are static_cast'ed corresponding elements of theOtherMat4 matrix)
  //! @tparam OtherElement_t the element type of the other 4 x 4 matrix theOtherVec4
  //! @param theOtherMat4 the 4 x 4 matrix that needs to be converted
  template <typename OtherElement_t>
  explicit NCollection_Mat4 (const NCollection_Mat4<OtherElement_t>& theOtherMat4)
  {
    ConvertFrom (theOtherMat4);
  }

  //! Get element at the specified row and column.
  //! @param theRow [in] the row to address.
  //! @param theCol [in] the column to address.
  //! @return the value of the addressed element.
  Element_t GetValue (const size_t theRow, const size_t theCol) const
  {
    return myMat[theCol * 4 + theRow];
  }

  //! Access element at the specified row and column.
  //! @param theRow [in] the row to access.
  //! @param theCol [in] the column to access.
  //! @return reference on the matrix element.
  Element_t& ChangeValue (const size_t theRow, const size_t theCol)
  {
    return myMat[theCol * 4 + theRow];
  }

  //! Set value for the element specified by row and columns.
  //! @param theRow   [in] the row to change.
  //! @param theCol   [in] the column to change.
  //! @param theValue [in] the value to set.
  void SetValue (const size_t    theRow,
                 const size_t    theCol,
                 const Element_t theValue)
  {
    myMat[theCol * 4 + theRow] = theValue;
  }

  //! Return value.
  Element_t& operator() (const size_t theRow, const size_t theCol) { return ChangeValue (theRow, theCol); }

  //! Return value.
  Element_t  operator() (const size_t theRow, const size_t theCol) const { return GetValue (theRow, theCol); }

  //! Get vector of elements for the specified row.
  //! @param theRow [in] the row to access.
  //! @return vector of elements.
  NCollection_Vec4<Element_t> GetRow (const size_t theRow) const
  {
    return NCollection_Vec4<Element_t> (GetValue (theRow, 0),
                                        GetValue (theRow, 1),
                                        GetValue (theRow, 2),
                                        GetValue (theRow, 3));
  }

  //! Change first 3 row values by the passed vector.
  //! @param theRow [in] the row to change.
  //! @param theVec [in] the vector of values.
  void SetRow (const size_t theRow, const NCollection_Vec3<Element_t>& theVec)
  {
    SetValue (theRow, 0, theVec.x());
    SetValue (theRow, 1, theVec.y());
    SetValue (theRow, 2, theVec.z());
  }

  //! Set row values by the passed 4 element vector.
  //! @param theRow [in] the row to change.
  //! @param theVec [in] the vector of values.
  void SetRow (const size_t theRow, const NCollection_Vec4<Element_t>& theVec)
  {
    SetValue (theRow, 0, theVec.x());
    SetValue (theRow, 1, theVec.y());
    SetValue (theRow, 2, theVec.z());
    SetValue (theRow, 3, theVec.w());
  }

  //! Get vector of elements for the specified column.
  //! @param theCol [in] the column to access.
  //! @return vector of elements.
  NCollection_Vec4<Element_t> GetColumn (const size_t theCol) const
  {
    return NCollection_Vec4<Element_t> (GetValue (0, theCol),
                                        GetValue (1, theCol),
                                        GetValue (2, theCol),
                                        GetValue (3, theCol));
  }

  //! Change first 3 column values by the passed vector.
  //! @param theCol [in] the column to change.
  //! @param theVec [in] the vector of values.
  void SetColumn (const size_t theCol,
                  const NCollection_Vec3<Element_t>& theVec)
  {
    SetValue (0, theCol, theVec.x());
    SetValue (1, theCol, theVec.y());
    SetValue (2, theCol, theVec.z());
  }

  //! Set column values by the passed 4 element vector.
  //! @param theCol [in] the column to change.
  //! @param theVec [in] the vector of values.
  void SetColumn (const size_t theCol,
                  const NCollection_Vec4<Element_t>& theVec)
  {
    SetValue (0, theCol, theVec.x());
    SetValue (1, theCol, theVec.y());
    SetValue (2, theCol, theVec.z());
    SetValue (3, theCol, theVec.w());
  }

  //! Get vector of diagonal elements.
  //! @return vector of diagonal elements.
  NCollection_Vec4<Element_t> GetDiagonal() const
  {
    return NCollection_Vec4<Element_t> (GetValue (0, 0),
                                        GetValue (1, 1),
                                        GetValue (2, 2),
                                        GetValue (3, 3));
  }

  //! Change first 3 elements of the diagonal matrix.
  //! @param theVec the vector of values.
  void SetDiagonal (const NCollection_Vec3<Element_t>& theVec)
  {
    SetValue (0, 0, theVec.x());
    SetValue (1, 1, theVec.y());
    SetValue (2, 2, theVec.z());
  }

  //! Set diagonal elements of the matrix by the passed vector.
  //! @param theVec [in] the vector of values.
  void SetDiagonal (const NCollection_Vec4<Element_t>& theVec)
  {
    SetValue (0, 0, theVec.x());
    SetValue (1, 1, theVec.y());
    SetValue (2, 2, theVec.z());
    SetValue (3, 3, theVec.w());
  }

  //! Return 3x3 sub-matrix.
  NCollection_Mat3<Element_t> GetMat3() const
  {
    NCollection_Mat3<Element_t> aMat;
    aMat.SetColumn (0, GetColumn (0).xyz());
    aMat.SetColumn (1, GetColumn (1).xyz());
    aMat.SetColumn (2, GetColumn (2).xyz());
    return aMat;
  }

  //! Initialize the zero matrix.
  void InitZero()
  {
    std::memcpy (this, MyZeroArray, sizeof (NCollection_Mat4));
  }

  //! Checks the matrix for zero (without tolerance).
  bool IsZero() const
  {
    return std::memcmp (this, MyZeroArray, sizeof (NCollection_Mat4)) == 0;
  }

  //! Initialize the identity matrix.
  void InitIdentity()
  {
    std::memcpy (this, MyIdentityArray, sizeof (NCollection_Mat4));
  }

  //! Checks the matrix for identity (without tolerance).
  bool IsIdentity() const
  {
    return std::memcmp (this, MyIdentityArray, sizeof (NCollection_Mat4)) == 0;
  }

  //! Check this matrix for equality with another matrix (without tolerance!).
  bool IsEqual (const NCollection_Mat4& theOther) const
  {
    return std::memcmp (this, &theOther, sizeof(NCollection_Mat4)) == 0;
  }

  //! Check this matrix for equality with another matrix (without tolerance!).
  bool operator== (const NCollection_Mat4& theOther) const { return IsEqual (theOther); }

  //! Check this matrix for non-equality with another matrix (without tolerance!).
  bool operator!= (const NCollection_Mat4& theOther) const { return !IsEqual (theOther); }

  //! Raw access to the data (for OpenGL exchange);
  //! the data is returned in column-major order.
  const Element_t* GetData()    const { return myMat; }
  Element_t*       ChangeData()       { return myMat; }

  //! Multiply by the vector (M * V).
  //! @param theVec [in] the vector to multiply.
  NCollection_Vec4<Element_t> operator* (const NCollection_Vec4<Element_t>& theVec) const
  {
    return NCollection_Vec4<Element_t> (
      GetValue (0, 0) * theVec.x() + GetValue (0, 1) * theVec.y() + GetValue (0, 2) * theVec.z() + GetValue (0, 3) * theVec.w(),
      GetValue (1, 0) * theVec.x() + GetValue (1, 1) * theVec.y() + GetValue (1, 2) * theVec.z() + GetValue (1, 3) * theVec.w(),
      GetValue (2, 0) * theVec.x() + GetValue (2, 1) * theVec.y() + GetValue (2, 2) * theVec.z() + GetValue (2, 3) * theVec.w(),
      GetValue (3, 0) * theVec.x() + GetValue (3, 1) * theVec.y() + GetValue (3, 2) * theVec.z() + GetValue (3, 3) * theVec.w());
  }

  //! Compute matrix multiplication product: A * B.
  //! @param theMatA [in] the matrix "A".
  //! @param theMatB [in] the matrix "B".
  static NCollection_Mat4 Multiply (const NCollection_Mat4& theMatA,
                                    const NCollection_Mat4& theMatB)
  {
    NCollection_Mat4 aMatRes;

    size_t aInputElem;
    for (size_t aResElem = 0; aResElem < 16; ++aResElem)
    {
      aMatRes.myMat[aResElem] = (Element_t )0;
      for (aInputElem = 0; aInputElem < 4; ++aInputElem)
      {
        aMatRes.myMat[aResElem] += theMatA.GetValue(aResElem % 4, aInputElem)
                                 * theMatB.GetValue(aInputElem, aResElem / 4);
      }
    }

    return aMatRes;
  }

  //! Compute matrix multiplication.
  //! @param theMat [in] the matrix to multiply.
  void Multiply (const NCollection_Mat4& theMat)
  {
    *this = Multiply(*this, theMat);
  }

  //! Multiply by the another matrix.
  //! @param theMat [in] the other matrix.
  NCollection_Mat4& operator*= (const NCollection_Mat4& theMat)
  {
    Multiply (theMat);
    return *this;
  }

  //! Compute matrix multiplication product.
  //! @param theMat [in] the other matrix.
  //! @return result of multiplication.
  Standard_NODISCARD NCollection_Mat4 operator* (const NCollection_Mat4& theMat) const
  {
    return Multiplied (theMat);
  }

  //! Compute matrix multiplication product.
  //! @param theMat [in] the other matrix.
  //! @return result of multiplication.
  Standard_NODISCARD NCollection_Mat4 Multiplied (const NCollection_Mat4& theMat) const
  {
    NCollection_Mat4 aTempMat (*this);
    aTempMat *= theMat;
    return aTempMat;
  }

  //! Compute per-component multiplication.
  //! @param theFactor [in] the scale factor.
  void Multiply (const Element_t theFactor)
  {
    for (size_t i = 0; i < 16; ++i)
    {
      myMat[i] *= theFactor;
    }
  }

  //! Compute per-element multiplication.
  //! @param theFactor [in] the scale factor.
  NCollection_Mat4& operator*= (const Element_t theFactor)
  {
    Multiply (theFactor);
    return *this;
  }

  //! Compute per-element multiplication.
  //! @param theFactor [in] the scale factor.
  //! @return the result of multiplication.
  Standard_NODISCARD NCollection_Mat4 operator* (const Element_t theFactor) const
  {
    return Multiplied (theFactor);
  }

  //! Compute per-element multiplication.
  //! @param theFactor [in] the scale factor.
  //! @return the result of multiplication.
  Standard_NODISCARD NCollection_Mat4 Multiplied (const Element_t theFactor) const
  {
    NCollection_Mat4 aTempMat (*this);
    aTempMat *= theFactor;
    return aTempMat;
  }

  //! Compute per-component division.
  //! @param theFactor [in] the scale factor.
  void Divide (const Element_t theFactor)
  {
    for (size_t i = 0; i < 16; ++i)
    {
      myMat[i] /= theFactor;
    }
  }

  //! Per-component division.
  //! @param theScalar [in] the scale factor.
  NCollection_Mat4& operator/= (const Element_t theScalar)
  {
    Divide (theScalar);
    return *this;
  }

  //! Divides all the coefficients of the matrix by scalar.
  Standard_NODISCARD NCollection_Mat4 Divided (const Element_t theScalar) const
  {
    NCollection_Mat4 aTempMat (*this);
    aTempMat /= theScalar;
    return aTempMat;
  }

  //! Divides all the coefficients of the matrix by scalar.
  Standard_NODISCARD NCollection_Mat4 operator/ (const Element_t theScalar) const
  {
    return Divided (theScalar);
  }

  //! Per-component addition of another matrix.
  void Add (const NCollection_Mat4& theMat)
  {
    for (size_t i = 0; i < 16; ++i)
    {
      myMat[i] += theMat.myMat[i];
    }
  }

  //! Per-component addition of another matrix.
  NCollection_Mat4& operator+= (const NCollection_Mat4& theMat)
  {
    Add (theMat);
    return *this;
  }

  //! Per-component subtraction of another matrix.
  void Subtract (const NCollection_Mat4& theMat)
  {
    for (size_t i = 0; i < 16; ++i)
    {
      myMat[i] -= theMat.myMat[i];
    }
  }

  //! Per-component subtraction of another matrix.
  NCollection_Mat4& operator-= (const NCollection_Mat4& theMat)
  {
    Subtract (theMat);
    return *this;
  }

  //! Per-component addition of another matrix.
  Standard_NODISCARD NCollection_Mat4 Added (const NCollection_Mat4& theMat) const
  {
    NCollection_Mat4 aMat (*this);
    aMat += theMat;
    return aMat;
  }

  //! Per-component addition of another matrix.
  Standard_NODISCARD NCollection_Mat4 operator+ (const NCollection_Mat4& theMat) const { return Added (theMat); }

  //! Per-component subtraction of another matrix.
  Standard_NODISCARD NCollection_Mat4 Subtracted (const NCollection_Mat4& theMat) const
  {
    NCollection_Mat4 aMat (*this);
    aMat -= theMat;
    return aMat;
  }

  //! Per-component subtraction of another matrix.
  Standard_NODISCARD NCollection_Mat4 operator- (const NCollection_Mat4& theMat) const { return Subtracted (theMat); }

  //! Returns matrix with all components negated.
  Standard_NODISCARD NCollection_Mat4 Negated() const
  {
    NCollection_Mat4 aMat;
    for (size_t i = 0; i < 16; ++i)
    {
      aMat.myMat[i] = -myMat[i];
    }
    return aMat;
  }

  //! Returns matrix with all components negated.
  Standard_NODISCARD NCollection_Mat4 operator-() const { return Negated(); }

  //! Translate the matrix on the passed vector.
  //! @param theVec [in] the translation vector.
  void Translate (const NCollection_Vec3<Element_t>& theVec)
  {
    NCollection_Mat4 aTempMat;
    aTempMat.SetColumn (3, theVec);
    this->Multiply (aTempMat);
  }

  //! Transpose the matrix.
  //! @return transposed copy of the matrix.
  Standard_NODISCARD NCollection_Mat4 Transposed() const
  {
    NCollection_Mat4 aTempMat;
    aTempMat.SetRow (0, GetColumn (0));
    aTempMat.SetRow (1, GetColumn (1));
    aTempMat.SetRow (2, GetColumn (2));
    aTempMat.SetRow (3, GetColumn (3));
    return aTempMat;
  }

  //! Transpose the matrix.
  void Transpose()
  {
    *this = Transposed();
  }

  //! Compute inverted matrix.
  //! @param theOutMx [out] the inverted matrix
  //! @param theDet   [out] determinant of matrix
  //! @return true if reversion success
  bool Inverted (NCollection_Mat4<Element_t>& theOutMx, Element_t& theDet) const
  {
    Element_t* inv = theOutMx.myMat;

    // use short-cut for better readability
    const Element_t* m = myMat;

    inv[ 0] = m[ 5] * (m[10] * m[15] - m[11] * m[14]) -
              m[ 9] * (m[ 6] * m[15] - m[ 7] * m[14]) -
              m[13] * (m[ 7] * m[10] - m[ 6] * m[11]);

    inv[ 1] = m[ 1] * (m[11] * m[14] - m[10] * m[15]) -
              m[ 9] * (m[ 3] * m[14] - m[ 2] * m[15]) -
              m[13] * (m[ 2] * m[11] - m[ 3] * m[10]);

    inv[ 2] = m[ 1] * (m[ 6] * m[15] - m[ 7] * m[14]) -
              m[ 5] * (m[ 2] * m[15] - m[ 3] * m[14]) -
              m[13] * (m[ 3] * m[ 6] - m[ 2] * m[ 7]);

    inv[ 3] = m[ 1] * (m[ 7] * m[10] - m[ 6] * m[11]) -
              m[ 5] * (m[ 3] * m[10] - m[ 2] * m[11]) -
              m[ 9] * (m[ 2] * m[ 7] - m[ 3] * m[ 6]);

    inv[ 4] = m[ 4] * (m[11] * m[14] - m[10] * m[15]) -
              m[ 8] * (m[ 7] * m[14] - m[ 6] * m[15]) -
              m[12] * (m[ 6] * m[11] - m[ 7] * m[10]);

    inv[ 5] = m[ 0] * (m[10] * m[15] - m[11] * m[14]) -
              m[ 8] * (m[ 2] * m[15] - m[ 3] * m[14]) -
              m[12] * (m[ 3] * m[10] - m[ 2] * m[11]);

    inv[ 6] = m[ 0] * (m[ 7] * m[14] - m[ 6] * m[15]) -
              m[ 4] * (m[ 3] * m[14] - m[ 2] * m[15]) -
              m[12] * (m[ 2] * m[ 7] - m[ 3] * m[ 6]);

    inv[ 7] = m[ 0] * (m[ 6] * m[11] - m[ 7] * m[10]) -
              m[ 4] * (m[ 2] * m[11] - m[ 3] * m[10]) -
              m[ 8] * (m[ 3] * m[ 6] - m[ 2] * m[ 7]);

    inv[ 8] = m[ 4] * (m[ 9] * m[15] - m[11] * m[13]) -
              m[ 8] * (m[ 5] * m[15] - m[ 7] * m[13]) -
              m[12] * (m[ 7] * m[ 9] - m[ 5] * m[11]);

    inv[ 9] = m[ 0] * (m[11] * m[13] - m[ 9] * m[15]) -
              m[ 8] * (m[ 3] * m[13] - m[ 1] * m[15]) -
              m[12] * (m[ 1] * m[11] - m[ 3] * m[ 9]);

    inv[10] = m[ 0] * (m[ 5] * m[15] - m[ 7] * m[13]) -
              m[ 4] * (m[ 1] * m[15] - m[ 3] * m[13]) -
              m[12] * (m[ 3] * m[ 5] - m[ 1] * m[ 7]);

    inv[11] = m[ 0] * (m[ 7] * m[ 9] - m[ 5] * m[11]) -
              m[ 4] * (m[ 3] * m[ 9] - m[ 1] * m[11]) -
              m[ 8] * (m[ 1] * m[ 7] - m[ 3] * m[ 5]);

    inv[12] = m[ 4] * (m[10] * m[13] - m[ 9] * m[14]) -
              m[ 8] * (m[ 6] * m[13] - m[ 5] * m[14]) -
              m[12] * (m[ 5] * m[10] - m[ 6] * m[ 9]);

    inv[13] = m[ 0] * (m[ 9] * m[14] - m[10] * m[13]) -
              m[ 8] * (m[ 1] * m[14] - m[ 2] * m[13]) -
              m[12] * (m[ 2] * m[ 9] - m[ 1] * m[10]);

    inv[14] = m[ 0] * (m[ 6] * m[13] - m[ 5] * m[14]) -
              m[ 4] * (m[ 2] * m[13] - m[ 1] * m[14]) -
              m[12] * (m[ 1] * m[ 6] - m[ 2] * m[ 5]);

    inv[15] = m[ 0] * (m[ 5] * m[10] - m[ 6] * m[ 9]) -
              m[ 4] * (m[ 1] * m[10] - m[ 2] * m[ 9]) -
              m[ 8] * (m[ 2] * m[ 5] - m[ 1] * m[ 6]);

    theDet = m[0] * inv[ 0] +
             m[1] * inv[ 4] +
             m[2] * inv[ 8] +
             m[3] * inv[12];
    if (theDet == 0)
    {
      return false;
    }

    const Element_t aDiv = (Element_t) 1. / theDet;
    for (int i = 0; i < 16; ++i)
    {
      inv[i] *= aDiv;
    }
    return true;
  }

  //! Compute inverted matrix.
  //! @param theOutMx [out] the inverted matrix
  //! @return true if reversion success
  bool Inverted (NCollection_Mat4<Element_t>& theOutMx) const
  {
    Element_t aDet;
    return Inverted (theOutMx, aDet);
  }

  //! Return inverted matrix.
  NCollection_Mat4 Inverted() const
  {
    NCollection_Mat4 anInv;
    if (!Inverted (anInv))
    {
      throw Standard_ConstructionError ("NCollection_Mat4::Inverted() - matrix has zero determinant");
    }
    return anInv;
  }

  //! Return determinant of the 3x3 sub-matrix.
  Element_t DeterminantMat3() const
  {
    return (GetValue (0, 0) * GetValue (1, 1) * GetValue (2, 2)
          + GetValue (0, 1) * GetValue (1, 2) * GetValue (2, 0)
          + GetValue (0, 2) * GetValue (1, 0) * GetValue (2, 1))
         - (GetValue (0, 2) * GetValue (1, 1) * GetValue (2, 0)
          + GetValue (0, 0) * GetValue (1, 2) * GetValue (2, 1)
          + GetValue (0, 1) * GetValue (1, 0) * GetValue (2, 2));
  }

  //! Return adjoint (adjugate matrix, e.g. conjugate transpose).
  Standard_NODISCARD NCollection_Mat4<Element_t> Adjoint() const
  {
    NCollection_Mat4<Element_t> aMat;
    aMat.SetRow (0, crossVec4 ( GetRow (1), GetRow (2), GetRow (3)));
    aMat.SetRow (1, crossVec4 (-GetRow (0), GetRow (2), GetRow (3)));
    aMat.SetRow (2, crossVec4 ( GetRow (0), GetRow (1), GetRow (3)));
    aMat.SetRow (3, crossVec4 (-GetRow (0), GetRow (1), GetRow (2)));
    return aMat;
  }

  //! Take values from NCollection_Mat4 with a different element type with type conversion.
  template <typename Other_t>
  void ConvertFrom (const NCollection_Mat4<Other_t>& theFrom)
  {
    for (int anIdx = 0; anIdx < 16; ++anIdx)
    {
      myMat[anIdx] = static_cast<Element_t> (theFrom.myMat[anIdx]);
    }
  }

  //! Take values from NCollection_Mat4 with a different element type with type conversion.
  template <typename Other_t>
  void Convert (const NCollection_Mat4<Other_t>& theFrom) { ConvertFrom (theFrom); }

  //! Maps plain C array to matrix type.
  static NCollection_Mat4<Element_t>& Map (Element_t* theData)
  {
    return *reinterpret_cast<NCollection_Mat4<Element_t>*> (theData);
  }

  //! Maps plain C array to matrix type.
  static const NCollection_Mat4<Element_t>& Map (const Element_t* theData)
  {
    return *reinterpret_cast<const NCollection_Mat4<Element_t>*> (theData);
  }

  //! Dumps the content of me into the stream
  void DumpJson (Standard_OStream& theOStream, Standard_Integer) const
  {
    OCCT_DUMP_FIELD_VALUES_NUMERICAL (theOStream, "NCollection_Mat4", 16,
      GetValue (0, 0),  GetValue (0, 1), GetValue (0, 2),  GetValue (0, 3),
      GetValue (1, 0),  GetValue (1, 1), GetValue (1, 2),  GetValue (1, 3),
      GetValue (2, 0),  GetValue (2, 1), GetValue (2, 2),  GetValue (2, 3),
      GetValue (3, 0),  GetValue (3, 1), GetValue (3, 2),  GetValue (3, 3))
  }

private:

  //! Cross-product has no direct meaning in 4D space - provided for local usage.
  static NCollection_Vec4<Element_t> crossVec4 (const NCollection_Vec4<Element_t>& theA,
                                                const NCollection_Vec4<Element_t>& theB,
                                                const NCollection_Vec4<Element_t>& theC)
  {
    const Element_t aD1 = (theB.z() * theC.w()) - (theB.w() * theC.z());
    const Element_t aD2 = (theB.y() * theC.w()) - (theB.w() * theC.y());
    const Element_t aD3 = (theB.y() * theC.z()) - (theB.z() * theC.y());
    const Element_t aD4 = (theB.x() * theC.w()) - (theB.w() * theC.x());
    const Element_t aD5 = (theB.x() * theC.z()) - (theB.z() * theC.x());
    const Element_t aD6 = (theB.x() * theC.y()) - (theB.y() * theC.x());

    NCollection_Vec4<Element_t> aVec;
    aVec.x() = -theA.y() * aD1 + theA.z() * aD2 - theA.w() * aD3;
    aVec.y() =  theA.x() * aD1 - theA.z() * aD4 + theA.w() * aD5;
    aVec.z() = -theA.x() * aD2 + theA.y() * aD4 - theA.w() * aD6;
    aVec.w() =  theA.x() * aD3 - theA.y() * aD5 + theA.z() * aD6;
    return aVec;
  }

private:

  Element_t myMat[16];

private:

  static const Element_t MyZeroArray[16];
  static const Element_t MyIdentityArray[16];

  // All instantiations are friend to each other
  template<class OtherType> friend class NCollection_Mat4;

};

template<typename Element_t>
const Element_t NCollection_Mat4<Element_t>::MyZeroArray[] =
  {0, 0, 0, 0,
   0, 0, 0, 0,
   0, 0, 0, 0,
   0, 0, 0, 0};

template<typename Element_t>
const Element_t NCollection_Mat4<Element_t>::MyIdentityArray[] =
  {1, 0, 0, 0,
   0, 1, 0, 0,
   0, 0, 1, 0,
   0, 0, 0, 1};

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
  #include <type_traits>

  static_assert(std::is_trivially_copyable<NCollection_Mat4<float>>::value, "NCollection_Mat4 is not is_trivially_copyable() structure!");
  static_assert(std::is_standard_layout   <NCollection_Mat4<float>>::value, "NCollection_Mat4 is not is_standard_layout() structure!");
  static_assert(sizeof(NCollection_Mat4<float>) == sizeof(float)*16,        "NCollection_Mat4 is not packed/aligned!");
#endif

#endif // _NCollection_Mat4_HeaderFile
