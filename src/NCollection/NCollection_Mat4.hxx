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

//! Generic matrix of 4 x 4 elements.
//! To be used in conjunction with NCollection_Vec4 entities.
//! Originally introduced for 3D space projection and orientation
//! operations.
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

  //! Empty constructor.
  //! Construct the zero matrix.
  NCollection_Mat4()
  {
    InitIdentity();
  }

  //! Get element at the specified row and column.
  //! @param theRow [in] the row.to address.
  //! @param theCol [in] the column to address.
  //! @return the value of the addressed element.
  Element_t GetValue (const size_t theRow, const size_t theCol) const
  {
    return myMat[theCol * 4 + theRow];
  }

  //! Access element at the specified row and column.
  //! @param theRow [in] the row.to access.
  //! @param theCol [in] the column to access.
  //! @return reference on the matrix element.
  Element_t& ChangeValue (const size_t theRow, const size_t theCol)
  {
    return myMat[theCol * 4 + theRow];
  }

  //! Set value for the element specified by row and columns.
  //! @param theRow   [in] the row to change.
  //! @param theCol   [in] the column to change.
  //! @param theValue [in] the value to set.s
  void SetValue (const size_t    theRow,
                 const size_t    theCol,
                 const Element_t theValue)
  {
    myMat[theCol * 4 + theRow] = theValue;
  }

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
  //! \return vector of diagonal elements.
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

  //! Initialize the identity matrix.
  void InitIdentity()
  {
    std::memcpy (this, myIdentityArray, sizeof (NCollection_Mat4));
  }

  //! Checks the matrix for identity.
  bool IsIdentity() const
  {
    return std::memcmp (this, myIdentityArray, sizeof (NCollection_Mat4)) == 0;
  }

  //! Check this matrix for equality with another matrix (without tolerance!).
  bool IsEqual (const NCollection_Mat4& theOther) const
  {
    return std::memcmp (this, &theOther, sizeof(NCollection_Mat4)) == 0;
  }

  //! Check this matrix for equality with another matrix (without tolerance!).
  bool operator== (const NCollection_Mat4& theOther)       { return IsEqual (theOther); }
  bool operator== (const NCollection_Mat4& theOther) const { return IsEqual (theOther); }

  //! Check this matrix for non-equality with another matrix (without tolerance!).
  bool operator!= (const NCollection_Mat4& theOther)       { return !IsEqual (theOther); }
  bool operator!= (const NCollection_Mat4& theOther) const { return !IsEqual (theOther); }

  //! Raw access to the data (for OpenGL exchange).
  const Element_t* GetData()    const { return myMat; }
  Element_t*       ChangeData()       { return myMat; }
  operator const   Element_t*() const { return myMat; }
  operator         Element_t*()       { return myMat; }

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
  NCollection_Mat4 Multiply (const NCollection_Mat4& theMatA,
                             const NCollection_Mat4& theMatB)
  {
    NCollection_Mat4 aMatRes;

    size_t aInputElem;
    for (size_t aResElem = 0; aResElem < 16; ++aResElem)
    {
      aMatRes[aResElem] = (Element_t )0;
      for (aInputElem = 0; aInputElem < 4; ++aInputElem)
      {
        aMatRes[aResElem] += theMatA.GetValue(aResElem % 4, aInputElem)
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
  NCollection_Mat4 operator* (const NCollection_Mat4& theMat) const
  {
    return Multiplied (theMat);
  }

  //! Compute matrix multiplication product.
  //! @param theMat [in] the other matrix.
  //! @return result of multiplication.
  NCollection_Mat4 Multiplied (const NCollection_Mat4& theMat) const
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
  NCollection_Mat4& operator*=(const Element_t theFactor)
  {
    Multiply (theFactor);
    return *this;
  }

  //! Compute per-element multiplication.
  //! @param theFactor [in] the scale factor.
  //! @return the result of multiplicaton.
  NCollection_Mat4 operator* (const Element_t theFactor) const
  {
    return Multiplied (theFactor);
  }

  //! Compute per-element multiplication.
  //! @param theFactor [in] the scale factor.
  //! @return the result of multiplicaton.
  NCollection_Mat4 Multiplied (const Element_t theFactor) const
  {
    NCollection_Mat4 aTempMat (*this);
    aTempMat *= theFactor;
    return aTempMat;
  }

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
  NCollection_Mat4 Transposed() const
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
  //! @param theOutMx [out] the inverted matrix.
  //! @return true if reversion success.
  bool Inverted (NCollection_Mat4<Element_t>& theOutMx) const
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

    Element_t aDet = m[0] * inv[ 0] +
                     m[1] * inv[ 4] +
                     m[2] * inv[ 8] +
                     m[3] * inv[12];

    if (aDet == 0)
      return false;

    aDet = (Element_t) 1. / aDet;

    for (int i = 0; i < 16; ++i)
      inv[i] *= aDet;

    return true;
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

private:

  Element_t myMat[16];

private:

  static Element_t myIdentityArray[16];

  // All instantiations are friend to each other
  template<class OtherType> friend class NCollection_Mat4;

};

template<typename Element_t>
Element_t NCollection_Mat4<Element_t>::myIdentityArray[] =
  {1, 0, 0, 0,
   0, 1, 0, 0,
   0, 0, 1, 0,
   0, 0, 0, 1};

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
  #include <type_traits>

  static_assert(std::is_trivially_copyable<NCollection_Mat4<float>>::value, "NCollection_Mat4 is not is_trivially_copyable() structure!");
  static_assert(std::is_standard_layout   <NCollection_Mat4<float>>::value, "NCollection_Mat4 is not is_standard_layout() structure!");
#endif

#endif // _NCollection_Mat4_HeaderFile
