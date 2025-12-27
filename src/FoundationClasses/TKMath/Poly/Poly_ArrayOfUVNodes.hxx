// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef _Poly_ArrayOfUVNodes_HeaderFile
#define _Poly_ArrayOfUVNodes_HeaderFile

#include <NCollection_AliasedArray.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Vec2.hxx>
#include <Standard_Macro.hxx>

//! Defines an array of 2D nodes of single/double precision configurable at construction time.
class Poly_ArrayOfUVNodes : public NCollection_AliasedArray<>
{
public:
  //! Empty constructor of double-precision array.
  Poly_ArrayOfUVNodes()
      : NCollection_AliasedArray((int)sizeof(gp_Pnt2d))
  {
    //
  }

  //! Constructor of double-precision array.
  Poly_ArrayOfUVNodes(int theLength)
      : NCollection_AliasedArray((int)sizeof(gp_Pnt2d), theLength)
  {
    //
  }

  //! Copy constructor
  Standard_EXPORT Poly_ArrayOfUVNodes(const Poly_ArrayOfUVNodes& theOther);

  //! Constructor wrapping pre-allocated C-array of values without copying them.
  Poly_ArrayOfUVNodes(const gp_Pnt2d& theBegin, int theLength)
      : NCollection_AliasedArray(theBegin, theLength)
  {
    //
  }

  //! Constructor wrapping pre-allocated C-array of values without copying them.
  Poly_ArrayOfUVNodes(const NCollection_Vec2<float>& theBegin, int theLength)
      : NCollection_AliasedArray(theBegin, theLength)
  {
    //
  }

  //! Destructor.
  Standard_EXPORT ~Poly_ArrayOfUVNodes();

  //! Returns TRUE if array defines nodes with double precision.
  bool IsDoublePrecision() const { return myStride == (int)sizeof(gp_Pnt2d); }

  //! Sets if array should define nodes with double or single precision.
  //! Raises exception if array was already allocated.
  void SetDoublePrecision(bool theIsDouble)
  {
    if (myData != NULL)
    {
      throw Standard_ProgramError(
        "Poly_ArrayOfUVNodes::SetDoublePrecision() should be called before allocation");
    }
    myStride = int(theIsDouble ? sizeof(gp_Pnt2d) : sizeof(NCollection_Vec2<float>));
  }

  //! Copies data of theOther array to this.
  //! The arrays should have the same length,
  //! but may have different precision / number of components (data conversion will be applied in
  //! the latter case).
  Standard_EXPORT Poly_ArrayOfUVNodes& Assign(const Poly_ArrayOfUVNodes& theOther);

  //! Move assignment.
  Poly_ArrayOfUVNodes& Move(Poly_ArrayOfUVNodes& theOther)
  {
    NCollection_AliasedArray::Move(theOther);
    return *this;
  }

  //! Assignment operator; @sa Assign()
  Poly_ArrayOfUVNodes& operator=(const Poly_ArrayOfUVNodes& theOther) { return Assign(theOther); }

  //! Move constructor
  Poly_ArrayOfUVNodes(Poly_ArrayOfUVNodes&& theOther) noexcept
      : NCollection_AliasedArray(std::move(theOther))
  {
    //
  }

  //! Move assignment operator; @sa Move()
  Poly_ArrayOfUVNodes& operator=(Poly_ArrayOfUVNodes&& theOther) noexcept
  {
    return Move(theOther);
  }

public:
  //! A generalized accessor to point.
  inline gp_Pnt2d Value(int theIndex) const;

  //! A generalized setter for point.
  inline void SetValue(int theIndex, const gp_Pnt2d& theValue);

  //! operator[] - alias to Value
  gp_Pnt2d operator[](int theIndex) const { return Value(theIndex); }
};

//=================================================================================================

inline gp_Pnt2d Poly_ArrayOfUVNodes::Value(int theIndex) const
{
  if (myStride == (int)sizeof(gp_Pnt2d))
  {
    return NCollection_AliasedArray::Value<gp_Pnt2d>(theIndex);
  }
  else
  {
    const NCollection_Vec2<float>& aVec2 = NCollection_AliasedArray::Value<NCollection_Vec2<float>>(theIndex);
    return gp_Pnt2d(aVec2.x(), aVec2.y());
  }
}

//=================================================================================================

inline void Poly_ArrayOfUVNodes::SetValue(int theIndex, const gp_Pnt2d& theValue)
{
  if (myStride == (int)sizeof(gp_Pnt2d))
  {
    NCollection_AliasedArray::ChangeValue<gp_Pnt2d>(theIndex) = theValue;
  }
  else
  {
    NCollection_Vec2<float>& aVec2 = NCollection_AliasedArray::ChangeValue<NCollection_Vec2<float>>(theIndex);
    aVec2.SetValues((float)theValue.X(), (float)theValue.Y());
  }
}

#endif // _Poly_ArrayOfUVNodes_HeaderFile
