// Created on: 2013-08-27
// Created by: Denis BOGOLEPOV
// Copyright (c) 2013 OPEN CASCADE SAS
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

#ifndef _OpenGl_AABB_Header
#define _OpenGl_AABB_Header

#include <OpenGl_RaytraceTypes.hxx>


//! Axis aligned bounding box (AABB).
class OpenGl_AABB
{
public:

  //! Creates default (invalid) bounding volume.
  OpenGl_AABB();

  //! Creates bounding volume of given point.
  OpenGl_AABB (const OpenGl_RTVec4f& thePoint);

  //! Creates copy of another bounding volume.
  OpenGl_AABB (const OpenGl_AABB& theVolume);

  //! Creates bounding volume from min and max points.
  OpenGl_AABB (const OpenGl_RTVec4f& theMinPoint,
               const OpenGl_RTVec4f& theMaxPoint);

  //! Is object represents uninitialized volume?
  bool IsVoid() const { return !myIsValid; }

  //! Appends new point to the volume.
  void Add (const OpenGl_RTVec4f& theVector);
  //! Combines the volume with another volume.
  void Combine (const OpenGl_AABB& theVolume);

  //! Returns new volume created by appending a point to current volume.
  OpenGl_AABB Added (const OpenGl_RTVec4f& thePoint) const;
  //! Returns new volume created by combining with specified volume.
  OpenGl_AABB Combined (const OpenGl_AABB& theVolume) const;

  //! Clears bounding volume (makes object invalid).
  void Clear();

  //! Evaluates surface area of bounding volume.
  float Area() const;

  //! Return diagonal of bounding volume.
  OpenGl_RTVec4f Size() const;

  //! Returns minimum point of bounding volume.
  const OpenGl_RTVec4f& CornerMin() const { return myMinPoint; }
  //! Returns maximum point of bounding volume.
  const OpenGl_RTVec4f& CornerMax() const { return myMaxPoint; }

  //! Returns minimum point of bounding volume.
  OpenGl_RTVec4f& CornerMin() { return myMinPoint; }
  //! Returns maximum point of bounding volume.
  OpenGl_RTVec4f& CornerMax() { return myMaxPoint; }

private:

  //! Minimum point of bounding volume.
  OpenGl_RTVec4f myMinPoint;      
  //! Maximum point of bounding volume.
  OpenGl_RTVec4f myMaxPoint;

  //! Is bounding volume valid (up to date)?
  bool myIsValid;
};

#endif
