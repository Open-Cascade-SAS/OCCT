// Created on: 2013-12-20
// Created by: Denis BOGOLEPOV
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

#ifndef BVH_Object_HeaderFile
#define BVH_Object_HeaderFile

#include <BVH_Box.hxx>
#include <BVH_Properties.hxx>

//! A non-template class for using as base for BVH_Object
//! (just to have a named base class).
class BVH_ObjectTransient : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(BVH_ObjectTransient, Standard_Transient)
public:
  //! Returns properties of the geometric object.
  virtual const Handle(BVH_Properties)& Properties() const { return myProperties; }

  //! Sets properties of the geometric object.
  virtual void SetProperties(const Handle(BVH_Properties)& theProperties)
  {
    myProperties = theProperties;
  }

  //! Returns TRUE if object state should be updated.
  virtual Standard_Boolean IsDirty() const { return myIsDirty; }

  //! Marks object state as outdated (needs BVH rebuilding).
  virtual void MarkDirty() { myIsDirty = Standard_True; }

protected:
  //! Creates new abstract geometric object.
  BVH_ObjectTransient()
      : myIsDirty(Standard_False)
  {
  }

protected:
  Standard_Boolean       myIsDirty;    //!< Marks internal object state as outdated
  Handle(BVH_Properties) myProperties; //!< Generic properties assigned to the object
};

//! Abstract geometric object bounded by BVH box.
//! \tparam T Numeric data type
//! \tparam N Vector dimension
template <class T, int N>
class BVH_Object : public BVH_ObjectTransient
{
public:
  //! Creates new abstract geometric object.
  BVH_Object() {}

  //! Releases resources of geometric object.
  virtual ~BVH_Object() {}

public:
  //! Returns AABB of the geometric object.
  virtual BVH_Box<T, N> Box() const = 0;
};

#endif // _BVH_Object_Header
