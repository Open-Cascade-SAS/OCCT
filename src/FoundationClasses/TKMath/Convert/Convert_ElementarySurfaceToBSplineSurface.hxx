// Created on: 1991-10-10
// Created by: Jean Claude VAUTHIER
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

#ifndef _Convert_ElementarySurfaceToBSplineSurface_HeaderFile
#define _Convert_ElementarySurfaceToBSplineSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Pnt.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class gp_Pnt;

//! Root class for algorithms which convert an elementary
//! surface (cylinder, cone, sphere or torus) into a BSpline
//! surface (CylinderToBSplineSurface, ConeToBSplineSurface,
//! SphereToBSplineSurface, TorusToBSplineSurface).
//! These algorithms all work on elementary surfaces from
//! the gp package and compute all the data needed to
//! construct a BSpline surface equivalent to the cylinder,
//! cone, sphere or torus. This data consists of the following:
//! -   degrees in the u and v parametric directions,
//! -   periodic characteristics in the u and v parametric directions,
//! -   a poles table with associated weights,
//! -   a knots table (for the u and v parametric directions)
//! with associated multiplicities.
//! The abstract class
//! ElementarySurfaceToBSplineSurface provides a
//! framework for storing and consulting this computed data.
//! This data may then be used to construct a
//! Geom_BSplineSurface surface, for example.
//! All those classes define algorithms to convert an
//! ElementarySurface into a B-spline surface.
//! This abstract class implements the methods to get
//! the geometric representation of the B-spline surface.
//! The B-spline representation is computed at the creation
//! time in the sub classes.
//! The B-spline surface is defined with its degree in the
//! parametric U and V directions, its control points (Poles),
//! its weights, its knots and their multiplicity.
//! KeyWords :
//! Convert, ElementarySurface, BSplineSurface.
class Convert_ElementarySurfaceToBSplineSurface
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT int UDegree() const;

  //! Returns the degree for the u or v parametric direction of
  //! the BSpline surface whose data is computed in this framework.
  Standard_EXPORT int VDegree() const;

  Standard_EXPORT int NbUPoles() const;

  //! Returns the number of poles for the u or v parametric
  //! direction of the BSpline surface whose data is computed in this framework.
  Standard_EXPORT int NbVPoles() const;

  Standard_EXPORT int NbUKnots() const;

  //! Returns the number of knots for the u or v parametric
  //! direction of the BSpline surface whose data is computed in this framework .
  Standard_EXPORT int NbVKnots() const;

  Standard_EXPORT bool IsUPeriodic() const;

  //! Returns true if the BSpline surface whose data is computed
  //! in this framework is periodic in the u or v parametric direction.
  Standard_EXPORT bool IsVPeriodic() const;

  //! Returns the pole of index (UIndex,VIndex) to the poles
  //! table of the BSpline surface whose data is computed in this framework.
  //! Exceptions
  //! Standard_OutOfRange if, for the BSpline surface whose
  //! data is computed in this framework:
  //! -   UIndex is outside the bounds of the poles table in the u
  //! parametric direction, or
  //! -   VIndex is outside the bounds of the poles table in the v
  //! parametric direction.
  Standard_EXPORT gp_Pnt Pole(const int UIndex, const int VIndex) const;

  //! Returns the weight of the pole of index (UIndex,VIndex) to
  //! the poles table of the BSpline surface whose data is computed in this framework.
  //! Exceptions
  //! Standard_OutOfRange if, for the BSpline surface whose
  //! data is computed in this framework:
  //! -   UIndex is outside the bounds of the poles table in the u
  //! parametric direction, or
  //! -   VIndex is outside the bounds of the poles table in the v
  //! parametric direction.
  Standard_EXPORT double Weight(const int UIndex, const int VIndex) const;

  //! Returns the U-knot of range UIndex.
  //! Raised if UIndex < 1 or UIndex > NbUKnots.
  Standard_EXPORT double UKnot(const int UIndex) const;

  //! Returns the V-knot of range VIndex.
  //! Raised if VIndex < 1 or VIndex > NbVKnots.
  Standard_EXPORT double VKnot(const int UIndex) const;

  //! Returns the multiplicity of the U-knot of range UIndex.
  //! Raised if UIndex < 1 or UIndex > NbUKnots.
  Standard_EXPORT int UMultiplicity(const int UIndex) const;

  //! Returns the multiplicity of the V-knot of range VIndex.
  //! Raised if VIndex < 1 or VIndex > NbVKnots.
  Standard_EXPORT int VMultiplicity(const int VIndex) const;

protected:
  Standard_EXPORT Convert_ElementarySurfaceToBSplineSurface(const int NumberOfUPoles,
                                                            const int NumberOfVPoles,
                                                            const int NumberOfUKnots,
                                                            const int NumberOfVKnots,
                                                            const int UDegree,
                                                            const int VDegree);

  NCollection_Array2<gp_Pnt> poles;
  NCollection_Array2<double> weights;
  NCollection_Array1<double> uknots;
  NCollection_Array1<int>    umults;
  NCollection_Array1<double> vknots;
  NCollection_Array1<int>    vmults;
  int                        udegree;
  int                        vdegree;
  int                        nbUPoles;
  int                        nbVPoles;
  int                        nbUKnots;
  int                        nbVKnots;
  bool                       isuperiodic;
  bool                       isvperiodic;
};

#endif // _Convert_ElementarySurfaceToBSplineSurface_HeaderFile
