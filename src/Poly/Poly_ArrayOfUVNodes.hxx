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
#include <gp_Vec2f.hxx>
#include <Standard_Macro.hxx>

//! Defines an array of 2D nodes of single/double precision configurable at construction time.
class Poly_ArrayOfUVNodes : public NCollection_AliasedArray<>
{
public:

  //! Empty constructor of double-precision array.
  Poly_ArrayOfUVNodes() : NCollection_AliasedArray ((Standard_Integer )sizeof(gp_Pnt2d))
  {
    //
  }

  //! Constructor of double-precision array.
  Poly_ArrayOfUVNodes (Standard_Integer theLength)
  : NCollection_AliasedArray ((Standard_Integer )sizeof(gp_Pnt2d), theLength)
  {
    //
  }

  //! Copy constructor 
  Standard_EXPORT Poly_ArrayOfUVNodes (const Poly_ArrayOfUVNodes& theOther);

  //! Constructor wrapping pre-allocated C-array of values without copying them.
  Poly_ArrayOfUVNodes (const gp_Pnt2d& theBegin,
                       Standard_Integer theLength)
  : NCollection_AliasedArray (theBegin, theLength)
  {
    //
  }

  //! Constructor wrapping pre-allocated C-array of values without copying them.
  Poly_ArrayOfUVNodes (const gp_Vec2f& theBegin,
                       Standard_Integer theLength)
  : NCollection_AliasedArray (theBegin, theLength)
  {
    //
  }

  //! Destructor.
  Standard_EXPORT ~Poly_ArrayOfUVNodes();

  //! Returns TRUE if array defines nodes with double precision.
  bool IsDoublePrecision() const { return myStride == (Standard_Integer )sizeof(gp_Pnt2d); }

  //! Sets if array should define nodes with double or single precision.
  //! Raises exception if array was already allocated.
  void SetDoublePrecision (bool theIsDouble)
  {
    if (myData != NULL) { throw Standard_ProgramError ("Poly_ArrayOfUVNodes::SetDoublePrecision() should be called before allocation"); }
    myStride = Standard_Integer(theIsDouble ? sizeof(gp_Pnt2d) : sizeof(gp_Vec2f));
  }

  //! Copies data of theOther array to this.
  //! The arrays should have the same length,
  //! but may have different precision / number of components (data conversion will be applied in the latter case).
  Standard_EXPORT Poly_ArrayOfUVNodes& Assign (const Poly_ArrayOfUVNodes& theOther);

  //! Move assignment.
  Poly_ArrayOfUVNodes& Move (Poly_ArrayOfUVNodes& theOther)
  {
    NCollection_AliasedArray::Move (theOther);
    return *this;
  }

  //! Assignment operator; @sa Assign()
  Poly_ArrayOfUVNodes& operator= (const Poly_ArrayOfUVNodes& theOther) { return Assign (theOther); }

  //! Move constructor
  Poly_ArrayOfUVNodes (Poly_ArrayOfUVNodes&& theOther) Standard_Noexcept
  : NCollection_AliasedArray (std::move (theOther))
  {
    //
  }

  //! Move assignment operator; @sa Move()
  Poly_ArrayOfUVNodes& operator= (Poly_ArrayOfUVNodes&& theOther) Standard_Noexcept
  {
    return Move (theOther);
  }

public:

  //! A generalized accessor to point.
  inline gp_Pnt2d Value (Standard_Integer theIndex) const;

  //! A generalized setter for point.
  inline void SetValue (Standard_Integer theIndex, const gp_Pnt2d& theValue);

  //! operator[] - alias to Value
  gp_Pnt2d operator[] (Standard_Integer theIndex) const { return Value (theIndex); }

};

// =======================================================================
// function : Value
// purpose  :
// =======================================================================
inline gp_Pnt2d Poly_ArrayOfUVNodes::Value (Standard_Integer theIndex) const
{
  if (myStride == (Standard_Integer )sizeof(gp_Pnt2d))
  {
    return NCollection_AliasedArray::Value<gp_Pnt2d> (theIndex);
  }
  else
  {
    const gp_Vec2f& aVec2 = NCollection_AliasedArray::Value<gp_Vec2f> (theIndex);
    return gp_Pnt2d (aVec2.x(), aVec2.y());
  }
}

// =======================================================================
// function : SetValue
// purpose  :
// =======================================================================
inline void Poly_ArrayOfUVNodes::SetValue (Standard_Integer theIndex, const gp_Pnt2d& theValue)
{
  if (myStride == (Standard_Integer )sizeof(gp_Pnt2d))
  {
    NCollection_AliasedArray::ChangeValue<gp_Pnt2d> (theIndex) = theValue;
  }
  else
  {
    gp_Vec2f& aVec2 = NCollection_AliasedArray::ChangeValue<gp_Vec2f> (theIndex);
    aVec2.SetValues ((float )theValue.X(), (float )theValue.Y());
  }
}

#endif // _Poly_ArrayOfUVNodes_HeaderFile
