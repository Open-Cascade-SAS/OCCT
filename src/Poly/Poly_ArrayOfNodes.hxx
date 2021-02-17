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

#ifndef _Poly_ArrayOfNodes_HeaderFile
#define _Poly_ArrayOfNodes_HeaderFile

#include <NCollection_AliasedArray.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec3f.hxx>

//! Defines an array of 3D nodes of single/double precision configurable at construction time.
class Poly_ArrayOfNodes : public NCollection_AliasedArray<>
{
public:

  //! Empty constructor of double-precision array.
  Poly_ArrayOfNodes() : NCollection_AliasedArray ((Standard_Integer )sizeof(gp_Pnt))
  {
    //
  }

  //! Constructor of double-precision array.
  Poly_ArrayOfNodes (Standard_Integer theLength)
  : NCollection_AliasedArray ((Standard_Integer )sizeof(gp_Pnt), theLength)
  {
    //
  }

  //! Copy constructor 
  Standard_EXPORT Poly_ArrayOfNodes (const Poly_ArrayOfNodes& theOther);

  //! Constructor wrapping pre-allocated C-array of values without copying them.
  Poly_ArrayOfNodes (const gp_Pnt& theBegin,
                     Standard_Integer theLength)
  : NCollection_AliasedArray (theBegin, theLength)
  {
    //
  }

  //! Constructor wrapping pre-allocated C-array of values without copying them.
  Poly_ArrayOfNodes (const gp_Vec3f& theBegin,
                     Standard_Integer theLength)
  : NCollection_AliasedArray (theBegin, theLength)
  {
    //
  }

  //! Destructor.
  Standard_EXPORT ~Poly_ArrayOfNodes();

  //! Returns TRUE if array defines nodes with double precision.
  bool IsDoublePrecision() const { return myStride == (Standard_Integer )sizeof(gp_Pnt); }

  //! Sets if array should define nodes with double or single precision.
  //! Raises exception if array was already allocated.
  void SetDoublePrecision (bool theIsDouble)
  {
    if (myData != NULL) { throw Standard_ProgramError ("Poly_ArrayOfNodes::SetDoublePrecision() should be called before allocation"); }
    myStride = Standard_Integer(theIsDouble ? sizeof(gp_Pnt) : sizeof(gp_Vec3f));
  }

  //! Copies data of theOther array to this.
  //! The arrays should have the same length,
  //! but may have different precision / number of components (data conversion will be applied in the latter case).
  Standard_EXPORT Poly_ArrayOfNodes& Assign (const Poly_ArrayOfNodes& theOther);

  //! Move assignment.
  Poly_ArrayOfNodes& Move (Poly_ArrayOfNodes& theOther)
  {
    NCollection_AliasedArray::Move (theOther);
    return *this;
  }

  //! Assignment operator; @sa Assign()
  Poly_ArrayOfNodes& operator= (const Poly_ArrayOfNodes& theOther) { return Assign (theOther); }

#ifndef OCCT_NO_RVALUE_REFERENCE
  //! Move constructor
  Poly_ArrayOfNodes (Poly_ArrayOfNodes&& theOther)
  : NCollection_AliasedArray (std::move (theOther))
  {
    //
  }

  //! Move assignment operator; @sa Move()
  Poly_ArrayOfNodes& operator= (Poly_ArrayOfNodes&& theOther)
  {
    return Move (theOther);
  }
#endif

public:

  //! A generalized accessor to point.
  inline gp_Pnt Value (Standard_Integer theIndex) const;

  //! A generalized setter for point.
  inline void SetValue (Standard_Integer theIndex, const gp_Pnt& theValue);

  //! operator[] - alias to Value
  gp_Pnt operator[] (Standard_Integer theIndex) const { return Value (theIndex); }

};

// =======================================================================
// function : Value
// purpose  :
// =======================================================================
inline gp_Pnt Poly_ArrayOfNodes::Value (Standard_Integer theIndex) const
{
  if (myStride == (Standard_Integer )sizeof(gp_Pnt))
  {
    return NCollection_AliasedArray::Value<gp_Pnt> (theIndex);
  }
  else
  {
    const gp_Vec3f& aVec3 = NCollection_AliasedArray::Value<gp_Vec3f> (theIndex);
    return gp_Pnt (aVec3.x(), aVec3.y(), aVec3.z());
  }
}

// =======================================================================
// function : SetValue
// purpose  :
// =======================================================================
inline void Poly_ArrayOfNodes::SetValue (Standard_Integer theIndex, const gp_Pnt& theValue)
{
  if (myStride == (Standard_Integer )sizeof(gp_Pnt))
  {
    NCollection_AliasedArray::ChangeValue<gp_Pnt> (theIndex) = theValue;
  }
  else
  {
    gp_Vec3f& aVec3 = NCollection_AliasedArray::ChangeValue<gp_Vec3f> (theIndex);
    aVec3.SetValues ((float )theValue.X(), (float )theValue.Y(), (float )theValue.Z());
  }
}

#endif // _Poly_ArrayOfNodes_HeaderFile
