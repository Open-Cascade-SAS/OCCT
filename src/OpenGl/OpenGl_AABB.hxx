// Created on: 2013-08-27
// Created by: Denis BOGOLEPOV
// Copyright (c) 2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
