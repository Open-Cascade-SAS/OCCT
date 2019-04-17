// Created by: Eugeny MALTCHIKOV
// Created on: 2019-04-17
// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _Bnd_Tools_Header
#define _Bnd_Tools_Header

#include <Bnd_Box2d.hxx>
#include <Bnd_Box.hxx>
#include <BVH_Box.hxx>

//! Defines a set of static methods operating with bounding boxes
class Bnd_Tools
{
public: //! @name Bnd_Box to BVH_Box conversion

  //! Converts the given Bnd_Box2d to BVH_Box
  static BVH_Box <Standard_Real, 2> Bnd2BVH (const Bnd_Box2d& theBox)
  {
    Standard_Real aXMin, aYMin, aXMax, aYMax;
    theBox.Get (aXMin, aYMin, aXMax, aYMax);
    return BVH_Box <Standard_Real, 2> (BVH_Vec2d (aXMin, aYMin),
                                       BVH_Vec2d (aXMax, aYMax));
  }

  //! Converts the given Bnd_Box to BVH_Box
  static BVH_Box <Standard_Real, 3> Bnd2BVH (const Bnd_Box& theBox)
  {
    Standard_Real aXMin, aYMin, aZMin, aXMax, aYMax, aZMax;
    theBox.Get (aXMin, aYMin, aZMin, aXMax, aYMax, aZMax);
    return BVH_Box <Standard_Real, 3> (BVH_Vec3d (aXMin, aYMin, aZMin),
                                       BVH_Vec3d (aXMax, aYMax, aZMax));
  }
};

#endif // _Bnd_Tools_Header
