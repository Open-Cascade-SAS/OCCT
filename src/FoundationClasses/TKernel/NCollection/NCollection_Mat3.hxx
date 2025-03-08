// Copyright (c) 2020 OPEN CASCADE SAS
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

#ifndef _NCollection_Mat3_HeaderFile
#define _NCollection_Mat3_HeaderFile

#include <NCollection_Vec3.hxx>
#include <Standard_ConstructionError.hxx>

//! 3x3 Matrix class.
//! Warning, empty constructor returns an identity matrix.
template <typename Element_t>
class NCollection_Mat3
{
public:
  //! Return identity matrix.
  static NCollection_Mat3 Identity() { return NCollection_Mat3(); }

  //! Return zero matrix.
  static NCollection_Mat3 Zero()
  {
    NCollection_Mat3 aMat;
    aMat.InitZero();
    return aMat;
  }

public:
  //! Empty constructor for identity matrix.
  NCollection_Mat3() { InitIdentity(); }

  //! Conversion constructor (explicitly converts some 3x3 matrix with other element type
  //! to a new 3x3 matrix with the element type Element_t,
  //! whose elements are static_cast'ed corresponding elements of theOtherMat3 matrix)
  //! @tparam OtherElement_t the element type of the other 3x3 matrix theOtherVec3
  //! @param theOtherMat3 the 3x3 matrix that needs to be converted
  template <typename OtherElement_t>
  explicit NCollection_Mat3(const NCollection_Mat3<OtherElement_t>& theOtherMat3)
  {
    ConvertFrom(theOtherMat3);
  }

  //! Get element at the specified row and column.
  //! @param[in] theRow  the row.to address.
  //! @param[in] theCol  the column to address.
  //! @return the value of the addressed element.
  Element_t GetValue(const size_t theRow, const size_t theCol) const
  {
    return myMat[theCol * 3 + theRow];
  }

  //! Access element at the specified row and column.
  //! @param[in] theRow  the row.to access.
  //! @param[in] theCol  the column to access.
  //! @return reference on the matrix element.
  Element_t& ChangeValue(const size_t theRow, const size_t theCol)
  {
    return myMat[theCol * 3 + theRow];
  }

  //! Set value for the element specified by row and columns.
  //! @param[in] theRow    the row to change.
  //! @param[in] theCol    the column to change.
  //! @param[in] theValue  the value to set.s
  void SetValue(const size_t theRow, const size_t theCol, const Element_t theValue)
  {
    myMat[theCol * 3 + theRow] = theValue;
  }

  //! Return value.
  Element_t& operator()(const size_t theRow, const size_t theCol)
  {
    return ChangeValue(theRow, theCol);
  }

  //! Return value.
  Element_t operator()(const size_t theRow, const size_t theCol) const
  {
    return GetValue(theRow, theCol);
  }

  //! Return the row.
  NCollection_Vec3<Element_t> GetRow(const size_t theRow) const
  {
    return NCollection_Vec3<Element_t>(GetValue(theRow, 0),
                                       GetValue(theRow, 1),
                                       GetValue(theRow, 2));
  }

  //! Change first 3 row values by the passed vector.
  //! @param[in] theRow  the row to change.
  //! @param[in] theVec  the vector of values.
  void SetRow(const size_t theRow, const NCollection_Vec3<Element_t>& theVec)
  {
    SetValue(theRow, 0, theVec.x());
    SetValue(theRow, 1, theVec.y());
    SetValue(theRow, 2, theVec.z());
  }

  //! Return the column.
  NCollection_Vec3<Element_t> GetColumn(const size_t theCol) const
  {
    return NCollection_Vec3<Element_t>(GetValue(0, theCol),
                                       GetValue(1, theCol),
                                       GetValue(2, theCol));
  }

  //! Change first 3 column values by the passed vector.
  //! @param[in] theCol  the column to change.
  //! @param[in] theVec  the vector of values.
  void SetColumn(const size_t theCol, const NCollection_Vec3<Element_t>& theVec)
  {
    SetValue(0, theCol, theVec.x());
    SetValue(1, theCol, theVec.y());
    SetValue(2, theCol, theVec.z());
  }

  //! Get vector of diagonal elements.
  //! @return vector of diagonal elements.
  NCollection_Vec3<Element_t> GetDiagonal() const
  {
    return NCollection_Vec3<Element_t>(GetValue(0, 0), GetValue(1, 1), GetValue(2, 2));
  }

  //! Change first 3 elements of the diagonal matrix.
  //! @param theVec the vector of values.
  void SetDiagonal(const NCollection_Vec3<Element_t>& theVec)
  {
    SetValue(0, 0, theVec.x());
    SetValue(1, 1, theVec.y());
    SetValue(2, 2, theVec.z());
  }

  //! Initialize the zero matrix.
  void InitZero() { std::memcpy(this, MyZeroArray, sizeof(NCollection_Mat3)); }

  //! Checks the matrix for zero (without tolerance).
  bool IsZero() const { return std::memcmp(this, MyZeroArray, sizeof(NCollection_Mat3)) == 0; }

  //! Initialize the identity matrix.
  void InitIdentity() { std::memcpy(this, MyIdentityArray, sizeof(NCollection_Mat3)); }

  //! Checks the matrix for identity (without tolerance).
  bool IsIdentity() const
  {
    return std::memcmp(this, MyIdentityArray, sizeof(NCollection_Mat3)) == 0;
  }

  //! Check this matrix for equality with another matrix (without tolerance!).
  bool IsEqual(const NCollection_Mat3& theOther) const
  {
    return std::memcmp(this, &theOther, sizeof(NCollection_Mat3)) == 0;
  }

  //! Comparison operator.
  bool operator==(const NCollection_Mat3& theMat) const { return IsEqual(theMat); }

  //! Check this vector with another vector for non-equality (without tolerance!).
  bool operator!=(const NCollection_Mat3& theOther) const { return !IsEqual(theOther); }

  //! Raw access to the data (for OpenGL exchange).
  //! the data is returned in column-major order.
  const Element_t* GetData() const { return myMat; }

  Element_t* ChangeData() { return myMat; }

  //! Multiply by the vector (M * V).
  //! @param[in] theVec  the vector to multiply.
  NCollection_Vec3<Element_t> operator*(const NCollection_Vec3<Element_t>& theVec) const
  {
    return NCollection_Vec3<Element_t>(
      GetValue(0, 0) * theVec.x() + GetValue(0, 1) * theVec.y() + GetValue(0, 2) * theVec.z(),
      GetValue(1, 0) * theVec.x() + GetValue(1, 1) * theVec.y() + GetValue(1, 2) * theVec.z(),
      GetValue(2, 0) * theVec.x() + GetValue(2, 1) * theVec.y() + GetValue(2, 2) * theVec.z());
  }

  //! Compute matrix multiplication product: A * B.
  //! @param[in] theMatA  the matrix "A".
  //! @param[in] theMatB  the matrix "B".
  static NCollection_Mat3 Multiply(const NCollection_Mat3& theMatA, const NCollection_Mat3& theMatB)
  {
    NCollection_Mat3 aMatRes;

    size_t aInputElem;
    for (size_t aResElem = 0; aResElem < 9; ++aResElem)
    {
      aMatRes.myMat[aResElem] = (Element_t)0;
      for (aInputElem = 0; aInputElem < 3; ++aInputElem)
      {
        aMatRes.myMat[aResElem] +=
          theMatA.GetValue(aResElem % 3, aInputElem) * theMatB.GetValue(aInputElem, aResElem / 3);
      }
    }

    return aMatRes;
  }

  //! Compute matrix multiplication.
  //! @param[in] theMat  the matrix to multiply.
  void Multiply(const NCollection_Mat3& theMat) { *this = Multiply(*this, theMat); }

  //! Multiply by the another matrix.
  //! @param[in] theMat  the other matrix.
  NCollection_Mat3& operator*=(const NCollection_Mat3& theMat)
  {
    Multiply(theMat);
    return *this;
  }

  //! Compute matrix multiplication product.
  //! @param[in] theMat  the other matrix.
  //! @return result of multiplication.
  Standard_NODISCARD NCollection_Mat3 operator*(const NCollection_Mat3& theMat) const
  {
    return Multiplied(theMat);
  }

  //! Compute matrix multiplication product.
  //! @param[in] theMat  the other matrix.
  //! @return result of multiplication.
  Standard_NODISCARD NCollection_Mat3 Multiplied(const NCollection_Mat3& theMat) const
  {
    NCollection_Mat3 aTempMat(*this);
    aTempMat *= theMat;
    return aTempMat;
  }

  //! Compute per-component multiplication.
  //! @param[in] theFactor  the scale factor.
  void Multiply(const Element_t theFactor)
  {
    for (size_t i = 0; i < 9; ++i)
    {
      myMat[i] *= theFactor;
    }
  }

  //! Compute per-element multiplication.
  //! @param[in] theFactor  the scale factor.
  NCollection_Mat3& operator*=(const Element_t theFactor)
  {
    Multiply(theFactor);
    return *this;
  }

  //! Compute per-element multiplication.
  //! @param[in] theFactor  the scale factor.
  //! @return the result of multiplication.
  Standard_NODISCARD NCollection_Mat3 operator*(const Element_t theFactor) const
  {
    return Multiplied(theFactor);
  }

  //! Compute per-element multiplication.
  //! @param[in] theFactor  the scale factor.
  //! @return the result of multiplication.
  Standard_NODISCARD NCollection_Mat3 Multiplied(const Element_t theFactor) const
  {
    NCollection_Mat3 aTempMat(*this);
    aTempMat *= theFactor;
    return aTempMat;
  }

  //! Compute per-component division.
  //! @param[in] theFactor  the scale factor.
  void Divide(const Element_t theFactor)
  {
    for (size_t i = 0; i < 9; ++i)
    {
      myMat[i] /= theFactor;
    }
  }

  //! Per-component division.
  //! @param[in] theScalar  the scale factor.
  NCollection_Mat3& operator/=(const Element_t theScalar)
  {
    Divide(theScalar);
    return *this;
  }

  //! Divides all the coefficients of the matrix by scalar.
  Standard_NODISCARD NCollection_Mat3 Divided(const Element_t theScalar) const
  {
    NCollection_Mat3 aTempMat(*this);
    aTempMat /= theScalar;
    return aTempMat;
  }

  //! Divides all the coefficients of the matrix by scalar.
  Standard_NODISCARD NCollection_Mat3 operator/(const Element_t theScalar) const
  {
    return Divided(theScalar);
  }

  //! Per-component addition of another matrix.
  void Add(const NCollection_Mat3& theMat)
  {
    for (size_t i = 0; i < 9; ++i)
    {
      myMat[i] += theMat.myMat[i];
    }
  }

  //! Per-component addition of another matrix.
  NCollection_Mat3& operator+=(const NCollection_Mat3& theMat)
  {
    Add(theMat);
    return *this;
  }

  //! Per-component subtraction of another matrix.
  void Subtract(const NCollection_Mat3& theMat)
  {
    for (size_t i = 0; i < 9; ++i)
    {
      myMat[i] -= theMat.myMat[i];
    }
  }

  //! Per-component subtraction of another matrix.
  NCollection_Mat3& operator-=(const NCollection_Mat3& theMat)
  {
    Subtract(theMat);
    return *this;
  }

  //! Per-component addition of another matrix.
  Standard_NODISCARD NCollection_Mat3 Added(const NCollection_Mat3& theMat) const
  {
    NCollection_Mat3 aMat(*this);
    aMat += theMat;
    return aMat;
  }

  //! Per-component addition of another matrix.
  Standard_NODISCARD NCollection_Mat3 operator+(const NCollection_Mat3& theMat) const
  {
    return Added(theMat);
  }

  //! Per-component subtraction of another matrix.
  Standard_NODISCARD NCollection_Mat3 Subtracted(const NCollection_Mat3& theMat) const
  {
    NCollection_Mat3 aMat(*this);
    aMat -= theMat;
    return aMat;
  }

  //! Per-component subtraction of another matrix.
  Standard_NODISCARD NCollection_Mat3 operator-(const NCollection_Mat3& theMat) const
  {
    return Subtracted(theMat);
  }

  //! Returns matrix with all components negated.
  Standard_NODISCARD NCollection_Mat3 Negated() const
  {
    NCollection_Mat3 aMat;
    for (size_t i = 0; i < 9; ++i)
    {
      aMat.myMat[i] = -myMat[i];
    }
    return aMat;
  }

  //! Returns matrix with all components negated.
  Standard_NODISCARD NCollection_Mat3 operator-() const { return Negated(); }

  //! Transpose the matrix.
  //! @return transposed copy of the matrix.
  Standard_NODISCARD NCollection_Mat3 Transposed() const
  {
    NCollection_Mat3 aTempMat;
    aTempMat.SetRow(0, GetColumn(0));
    aTempMat.SetRow(1, GetColumn(1));
    aTempMat.SetRow(2, GetColumn(2));
    return aTempMat;
  }

  //! Transpose the matrix.
  void Transpose() { *this = Transposed(); }

  //! Return determinant of the matrix.
  Element_t Determinant() const
  {
    return (GetValue(0, 0) * GetValue(1, 1) * GetValue(2, 2)
            + GetValue(0, 1) * GetValue(1, 2) * GetValue(2, 0)
            + GetValue(0, 2) * GetValue(1, 0) * GetValue(2, 1))
           - (GetValue(0, 2) * GetValue(1, 1) * GetValue(2, 0)
              + GetValue(0, 0) * GetValue(1, 2) * GetValue(2, 1)
              + GetValue(0, 1) * GetValue(1, 0) * GetValue(2, 2));
  }

  //! Return adjoint (adjugate matrix, e.g. conjugate transpose).
  Standard_NODISCARD NCollection_Mat3 Adjoint() const
  {
    NCollection_Mat3 aMat;
    aMat.SetRow(0, NCollection_Vec3<Element_t>::Cross(GetRow(1), GetRow(2)));
    aMat.SetRow(1, NCollection_Vec3<Element_t>::Cross(GetRow(2), GetRow(0)));
    aMat.SetRow(2, NCollection_Vec3<Element_t>::Cross(GetRow(0), GetRow(1)));
    return aMat;
  }

  //! Compute inverted matrix.
  //! @param[out] theInv the inverted matrix
  //! @param[out] theDet determinant of matrix
  //! @return true if reversion success
  bool Inverted(NCollection_Mat3& theInv, Element_t& theDet) const
  {
    const NCollection_Mat3 aMat = Adjoint();
    theDet                      = aMat.GetRow(0).Dot(GetRow(0));
    if (theDet == Element_t(0))
    {
      return false;
    }

    theInv = aMat.Transposed() / theDet;
    return true;
  }

  //! Compute inverted matrix.
  //! @param[out] theInv the inverted matrix
  //! @return true if reversion success
  bool Inverted(NCollection_Mat3& theInv) const
  {
    Element_t aDet;
    return Inverted(theInv, aDet);
  }

  //! Return inverted matrix.
  NCollection_Mat3 Inverted() const
  {
    NCollection_Mat3 anInv;
    if (!Inverted(anInv))
    {
      throw Standard_ConstructionError(
        "NCollection_Mat3::Inverted() - matrix has zero determinant");
    }
    return anInv;
  }

  //! Take values from NCollection_Mat3 with a different element type with type conversion.
  template <typename Other_t>
  void ConvertFrom(const NCollection_Mat3<Other_t>& theFrom)
  {
    for (int anIdx = 0; anIdx < 9; ++anIdx)
    {
      myMat[anIdx] = static_cast<Element_t>(theFrom.myMat[anIdx]);
    }
  }

  //! Maps plain C array to matrix type.
  static NCollection_Mat3<Element_t>& Map(Element_t* theData)
  {
    return *reinterpret_cast<NCollection_Mat3<Element_t>*>(theData);
  }

  //! Maps plain C array to matrix type.
  static const NCollection_Mat3<Element_t>& Map(const Element_t* theData)
  {
    return *reinterpret_cast<const NCollection_Mat3<Element_t>*>(theData);
  }

  //! Dumps the content of me into the stream
  void DumpJson(Standard_OStream& theOStream, Standard_Integer) const
  {
    OCCT_DUMP_FIELD_VALUES_NUMERICAL(theOStream,
                                     "NCollection_Mat3",
                                     9,
                                     GetValue(0, 0),
                                     GetValue(0, 1),
                                     GetValue(0, 2),
                                     GetValue(1, 0),
                                     GetValue(1, 1),
                                     GetValue(1, 2),
                                     GetValue(2, 0),
                                     GetValue(2, 1),
                                     GetValue(2, 2))
  }

private:
  Element_t myMat[9];

private:
  static const Element_t MyZeroArray[9];
  static const Element_t MyIdentityArray[9];

  // All instantiations are friend to each other
  template <class OtherType>
  friend class NCollection_Mat3;
};

template <typename Element_t>
const Element_t NCollection_Mat3<Element_t>::MyZeroArray[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

template <typename Element_t>
const Element_t NCollection_Mat3<Element_t>::MyIdentityArray[] = {1, 0, 0, 0, 1, 0, 0, 0, 1};

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
  #include <type_traits>

static_assert(std::is_trivially_copyable<NCollection_Mat3<float>>::value,
              "NCollection_Mat3 is not is_trivially_copyable() structure!");
static_assert(std::is_standard_layout<NCollection_Mat3<float>>::value,
              "NCollection_Mat3 is not is_standard_layout() structure!");
static_assert(sizeof(NCollection_Mat3<float>) == sizeof(float) * 9,
              "NCollection_Mat3 is not packed/aligned!");
#endif

#endif // _NCollection_Mat3_HeaderFile
