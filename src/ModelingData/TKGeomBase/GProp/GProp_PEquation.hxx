// Created on: 1993-06-16
// Created by: Isabelle GRIGNON
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

#ifndef _GProp_PEquation_HeaderFile
#define _GProp_PEquation_HeaderFile

#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <NCollection_Array1.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

class gp_Pln;
class gp_Lin;

//! Analyzes a collection of 3D points to decide whether they are
//! coincident, collinear, coplanar, or span 3D space, within a given
//! tolerance.
//!
//! Uses principal-axis analysis (eigendecomposition of the inertia matrix)
//! to determine the dimensionality of the cloud. Depending on the result
//! type, the corresponding accessor (Point(), Line(), Plane() or Box())
//! returns the fitted geometric entity.
//!
//! The raw PCA results (Barycentre(), PrincipalAxis(), Extent()) are
//! always available regardless of the fitted type.
class GProp_PEquation
{
public:
  DEFINE_STANDARD_ALLOC

  //! Type of geometric entity that best fits the cloud.
  enum class Type
  {
    None,  //!< Not yet computed
    Point, //!< All points are coincident within tolerance
    Line,  //!< Points are collinear within tolerance
    Plane, //!< Points are coplanar within tolerance
    Space  //!< Points span 3D space
  };

  //! Constructs the analysis from a set of points and a tolerance.
  //! @param[in] thePnts array of points to analyze
  //! @param[in] theTol  tolerance for dimensional collapse detection
  Standard_EXPORT GProp_PEquation(const NCollection_Array1<gp_Pnt>& thePnts, double theTol);

  //! Returns the type of the fitted entity.
  Type GetType() const { return myType; }

  //! Returns true if points are coplanar within tolerance.
  bool IsPlanar() const { return myType == Type::Plane; }

  //! Returns true if points are collinear within tolerance.
  bool IsLinear() const { return myType == Type::Line; }

  //! Returns true if points are coincident within tolerance.
  bool IsPoint() const { return myType == Type::Point; }

  //! Returns true if points span 3D space.
  bool IsSpace() const { return myType == Type::Space; }

  //! Returns the mean plane.
  //! @throws Standard_NoSuchObject if !IsPlanar().
  Standard_EXPORT gp_Pln Plane() const;

  //! Returns the mean line.
  //! @throws Standard_NoSuchObject if !IsLinear().
  Standard_EXPORT gp_Lin Line() const;

  //! Returns the mean point.
  //! @throws Standard_NoSuchObject if !IsPoint().
  Standard_EXPORT gp_Pnt Point() const;

  //! Returns a bounding box aligned with the principal axes.
  //! @param[out] theP  corner of the box (minimum projection on principal axes)
  //! @param[out] theV1 first box edge vector (along first principal axis)
  //! @param[out] theV2 second box edge vector (along second principal axis)
  //! @param[out] theV3 third box edge vector (along third principal axis)
  //! @throws Standard_NoSuchObject if !IsSpace().
  Standard_EXPORT void Box(gp_Pnt& theP, gp_Vec& theV1, gp_Vec& theV2, gp_Vec& theV3) const;

  //! Returns the centre of mass of the cloud (always valid after construction).
  const gp_Pnt& Barycentre() const { return myBarycentre; }

  //! Returns the unit principal axis at @p theIndex (1, 2 or 3),
  //! ordered by eigenvalue.
  const gp_Vec& PrincipalAxis(int theIndex) const { return myAxes[theIndex - 1]; }

  //! Returns the extent (max - min projection) along principal axis
  //! @p theIndex (1, 2 or 3).
  double Extent(int theIndex) const { return myExtents[theIndex - 1]; }

private:
  Type   myType = Type::None;
  gp_Pnt myG;  //!< Type-specific point (mean point / box corner)
  gp_Vec myV1; //!< Plane normal / line direction / box edge 1
  gp_Vec myV2; //!< Box edge 2 (Space type only)
  gp_Vec myV3; //!< Box edge 3 (Space type only)

  gp_Pnt myBarycentre;   //!< Centre of mass, always valid
  gp_Vec myAxes[3];      //!< Principal axis unit vectors
  double myExtents[3]{}; //!< Extent along each principal axis
};

#endif // _GProp_PEquation_HeaderFile
