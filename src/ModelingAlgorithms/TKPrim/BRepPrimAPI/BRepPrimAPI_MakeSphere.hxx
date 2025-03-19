// Created on: 1993-07-22
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _BRepPrimAPI_MakeSphere_HeaderFile
#define _BRepPrimAPI_MakeSphere_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <BRepPrim_Sphere.hxx>
#include <BRepPrimAPI_MakeOneAxis.hxx>
class gp_Pnt;
class gp_Ax2;

//! Describes functions to build spheres or portions of spheres.
//! A MakeSphere object provides a framework for:
//! -   defining the construction of a sphere,
//! -   implementing the construction algorithm, and
//! -   consulting the result.
class BRepPrimAPI_MakeSphere : public BRepPrimAPI_MakeOneAxis
{
public:
  DEFINE_STANDARD_ALLOC

  //! Make a sphere.
  //! @param[in] R  sphere radius
  Standard_EXPORT BRepPrimAPI_MakeSphere(const Standard_Real R);

  //! Make a sphere (spherical wedge).
  //! @param[in] R      sphere radius
  //! @param[in] angle  angle between the radii lying within the bounding semidisks
  Standard_EXPORT BRepPrimAPI_MakeSphere(const Standard_Real R, const Standard_Real angle);

  //! Make a sphere (spherical segment).
  //! @param[in] R  sphere radius
  //! @param[in] angle1  first angle defining a spherical segment
  //! @param[in] angle2  second angle defining a spherical segment
  Standard_EXPORT BRepPrimAPI_MakeSphere(const Standard_Real R,
                                         const Standard_Real angle1,
                                         const Standard_Real angle2);

  //! Make a sphere (spherical segment).
  //! @param[in] R       sphere radius
  //! @param[in] angle1  first angle defining a spherical segment
  //! @param[in] angle2  second angle defining a spherical segment
  //! @param[in] angle3  angle between the radii lying within the bounding semidisks
  Standard_EXPORT BRepPrimAPI_MakeSphere(const Standard_Real R,
                                         const Standard_Real angle1,
                                         const Standard_Real angle2,
                                         const Standard_Real angle3);

  //! Make a sphere.
  //! @param[in] Center  sphere center coordinates
  //! @param[in] R       sphere radius
  Standard_EXPORT BRepPrimAPI_MakeSphere(const gp_Pnt& Center, const Standard_Real R);

  //! Make a sphere (spherical wedge).
  //! @param[in] Center  sphere center coordinates
  //! @param[in] R       sphere radius
  //! @param[in] angle   angle between the radii lying within the bounding semidisks
  Standard_EXPORT BRepPrimAPI_MakeSphere(const gp_Pnt&       Center,
                                         const Standard_Real R,
                                         const Standard_Real angle);

  //! Make a sphere (spherical segment).
  //! @param[in] Center  sphere center coordinates
  //! @param[in] R       sphere radius
  //! @param[in] angle1  first angle defining a spherical segment
  //! @param[in] angle2  second angle defining a spherical segment
  Standard_EXPORT BRepPrimAPI_MakeSphere(const gp_Pnt&       Center,
                                         const Standard_Real R,
                                         const Standard_Real angle1,
                                         const Standard_Real angle2);

  //! Make a sphere (spherical segment).
  //! @param[in] Center  sphere center coordinates
  //! @param[in] R       sphere radius
  //! @param[in] angle1  first angle defining a spherical segment
  //! @param[in] angle2  second angle defining a spherical segment
  //! @param[in] angle3  angle between the radii lying within the bounding semidisks
  Standard_EXPORT BRepPrimAPI_MakeSphere(const gp_Pnt&       Center,
                                         const Standard_Real R,
                                         const Standard_Real angle1,
                                         const Standard_Real angle2,
                                         const Standard_Real angle3);

  //! Make a sphere.
  //! @param[in] Axis  coordinate system for the construction of the sphere
  //! @param[in] R     sphere radius
  Standard_EXPORT BRepPrimAPI_MakeSphere(const gp_Ax2& Axis, const Standard_Real R);

  //! Make a sphere (spherical wedge).
  //! @param[in] Axis   coordinate system for the construction of the sphere
  //! @param[in] R      sphere radius
  //! @param[in] angle  angle between the radii lying within the bounding semidisks
  Standard_EXPORT BRepPrimAPI_MakeSphere(const gp_Ax2&       Axis,
                                         const Standard_Real R,
                                         const Standard_Real angle);

  //! Make a sphere (spherical segment).
  //! @param[in] Axis    coordinate system for the construction of the sphere
  //! @param[in] R       sphere radius
  //! @param[in] angle1  first angle defining a spherical segment
  //! @param[in] angle2  second angle defining a spherical segment
  Standard_EXPORT BRepPrimAPI_MakeSphere(const gp_Ax2&       Axis,
                                         const Standard_Real R,
                                         const Standard_Real angle1,
                                         const Standard_Real angle2);

  //! Make a sphere of radius R.
  //! For all algorithms The resulting shape is composed of
  //! -   a lateral spherical face,
  //! -   two planar faces parallel to the plane z = 0 if the
  //! sphere is truncated in the v parametric direction, or
  //! only one planar face if angle1 is equal to -p/2 or if
  //! angle2 is equal to p/2 (these faces are circles in
  //! case of a complete truncated sphere),
  //! -   and in case of a portion of sphere, two planar faces
  //! to shut the shape.(in the planes u = 0 and u = angle).
  Standard_EXPORT BRepPrimAPI_MakeSphere(const gp_Ax2&       Axis,
                                         const Standard_Real R,
                                         const Standard_Real angle1,
                                         const Standard_Real angle2,
                                         const Standard_Real angle3);

  //! Returns the algorithm.
  Standard_EXPORT Standard_Address OneAxis();

  //! Returns the algorithm.
  Standard_EXPORT BRepPrim_Sphere& Sphere();

protected:
private:
  BRepPrim_Sphere mySphere;
};

#endif // _BRepPrimAPI_MakeSphere_HeaderFile
