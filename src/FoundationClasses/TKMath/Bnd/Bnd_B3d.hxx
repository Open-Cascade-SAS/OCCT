// Created on: 1991-01-08
// Created by: Didier Piffault
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _Bnd_B3d_HeaderFile
#define _Bnd_B3d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <gp_XYZ.hxx>
class gp_XYZ;
class gp_Pnt;
class gp_Trsf;
class gp_Ax1;
class gp_Ax3;

class Bnd_B3d
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor.
  Bnd_B3d();

  //! Constructor.
  Bnd_B3d(const gp_XYZ& theCenter, const gp_XYZ& theHSize);

  //! Returns True if the box is void (non-initialized).
  Standard_Boolean IsVoid() const;

  //! Reset the box data.
  void Clear();

  //! Update the box by a point.
  Standard_EXPORT void Add(const gp_XYZ& thePnt);

  //! Update the box by a point.
  void Add(const gp_Pnt& thePnt);

  //! Update the box by another box.
  void Add(const Bnd_B3d& theBox);

  //! Query the lower corner: (Center - HSize). You must make sure that
  //! the box is NOT VOID (see IsVoid()), otherwise the method returns
  //! irrelevant result.
  gp_XYZ CornerMin() const;

  //! Query the upper corner: (Center + HSize). You must make sure that
  //! the box is NOT VOID (see IsVoid()), otherwise the method returns
  //! irrelevant result.
  gp_XYZ CornerMax() const;

  //! Query the square diagonal. If the box is VOID (see method IsVoid())
  //! then a very big real value is returned.
  Standard_Real SquareExtent() const;

  //! Extend the Box by the absolute value of theDiff.
  void Enlarge(const Standard_Real theDiff);

  //! Limit the Box by the internals of theOtherBox.
  //! Returns True if the limitation takes place, otherwise False
  //! indicating that the boxes do not intersect.
  Standard_EXPORT Standard_Boolean Limit(const Bnd_B3d& theOtherBox);

  //! Transform the bounding box with the given transformation.
  //! The resulting box will be larger if theTrsf contains rotation.
  Standard_NODISCARD Standard_EXPORT Bnd_B3d Transformed(const gp_Trsf& theTrsf) const;

  //! Check the given point for the inclusion in the Box.
  //! Returns True if the point is outside.
  Standard_Boolean IsOut(const gp_XYZ& thePnt) const;

  //! Check a sphere for the intersection with the current box.
  //! Returns True if there is no intersection between boxes. If the
  //! parameter 'IsSphereHollow' is True, then the intersection is not
  //! reported for a box that is completely inside the sphere (otherwise
  //! this method would report an intersection).
  Standard_EXPORT Standard_Boolean
    IsOut(const gp_XYZ&          theCenter,
          const Standard_Real    theRadius,
          const Standard_Boolean isSphereHollow = Standard_False) const;

  //! Check the given box for the intersection with the current box.
  //! Returns True if there is no intersection between boxes.
  Standard_Boolean IsOut(const Bnd_B3d& theOtherBox) const;

  //! Check the given box oriented by the given transformation
  //! for the intersection with the current box.
  //! Returns True if there is no intersection between boxes.
  Standard_EXPORT Standard_Boolean IsOut(const Bnd_B3d& theOtherBox, const gp_Trsf& theTrsf) const;

  //! Check the given Line for the intersection with the current box.
  //! Returns True if there is no intersection.
  //! isRay==True means intersection check with the positive half-line
  //! theOverthickness is the addition to the size of the current box
  //! (may be negative). If positive, it can be treated as the thickness
  //! of the line 'theLine' or the radius of the cylinder along 'theLine'
  Standard_EXPORT Standard_Boolean IsOut(const gp_Ax1&          theLine,
                                         const Standard_Boolean isRay            = Standard_False,
                                         const Standard_Real    theOverthickness = 0.0) const;

  //! Check the given Plane for the intersection with the current box.
  //! Returns True if there is no intersection.
  Standard_EXPORT Standard_Boolean IsOut(const gp_Ax3& thePlane) const;

  //! Check that the box 'this' is inside the given box 'theBox'. Returns
  //! True if 'this' box is fully inside 'theBox'.
  Standard_Boolean IsIn(const Bnd_B3d& theBox) const;

  //! Check that the box 'this' is inside the given box 'theBox'
  //! transformed by 'theTrsf'. Returns True if 'this' box is fully
  //! inside the transformed 'theBox'.
  Standard_EXPORT Standard_Boolean IsIn(const Bnd_B3d& theBox, const gp_Trsf& theTrsf) const;

  //! Set the Center coordinates
  void SetCenter(const gp_XYZ& theCenter);

  //! Set the HSize (half-diagonal) coordinates.
  //! All components of theHSize must be non-negative.
  void SetHSize(const gp_XYZ& theHSize);

protected:
  Standard_Real myCenter[3];
  Standard_Real myHSize[3];

private:
};

#define RealType Standard_Real
#define RealType_hxx <Standard_Real.hxx>
#define Bnd_B3x Bnd_B3d
#define Bnd_B3x_hxx <Bnd_B3d.hxx>

#include <Bnd_B3x.lxx>

#undef RealType
#undef RealType_hxx
#undef Bnd_B3x
#undef Bnd_B3x_hxx

#endif // _Bnd_B3d_HeaderFile
