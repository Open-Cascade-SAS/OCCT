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

#ifndef _BVH_Properties_Header
#define _BVH_Properties_Header

#include <BVH_Box.hxx>

#include <Standard_Macro.hxx>

//! Abstract properties of geometric object.
class BVH_Properties
{
public:

  //! Releases resources of object properties.
  Standard_EXPORT virtual ~BVH_Properties() = 0;

};

//! Stores transform properties of geometric object.
template<class T, int N>
class BVH_Transform : public BVH_Properties
{
public:

  //! Type of transformation matrix.
  typedef typename BVH::MatrixType<T, N>::Type BVH_MatNt;

public:

  //! Creates new identity transformation.
  BVH_Transform();

  //! Creates new transformation with specified matrix.
  BVH_Transform (const BVH_MatNt& theTransform);

  //! Releases resources of transformation properties.
  virtual ~BVH_Transform();

  //! Returns transformation matrix.
  const BVH_MatNt& Transform() const;

  //! Sets new transformation matrix.
  void SetTransform (const BVH_MatNt& theTransform);

  //! Returns inversed transformation matrix.
  const BVH_MatNt& Inversed() const;

  //! Applies transformation matrix to bounding box.
  BVH_Box<T, N> Apply (const BVH_Box<T, N>& theBox) const;

protected:

  BVH_MatNt myTransform;         //!< Transformation matrix
  BVH_MatNt myTransformInversed; //!< Inversed transformation matrix

};

#include <BVH_Properties.lxx>

#endif // _BVH_Properties_Header
