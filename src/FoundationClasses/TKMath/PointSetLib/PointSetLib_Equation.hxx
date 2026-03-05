// Copyright (c) 2026 OPEN CASCADE SAS
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

#ifndef _PointSetLib_Equation_HeaderFile
#define _PointSetLib_Equation_HeaderFile

#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <NCollection_Array1.hxx>
#include <Standard.hxx>

class gp_Pln;
class gp_Lin;

//! Analyzes a collection of 3D points to determine if they are coincident,
//! collinear, coplanar, or span 3D space, within a given tolerance.
//!
//! Uses principal component analysis (eigendecomposition of the inertia
//! matrix) to find the principal axes and determine the dimensionality of
//! the point cloud.
//!
//! Depending on the result type, the corresponding accessor (Point(), Line(),
//! Plane() or Box()) returns the fitted geometric entity.
//!
//! The raw PCA results (barycentre, principal axes, extents) are always
//! available via Barycentre(), PrincipalAxis() and Extent() regardless
//! of the fitted type.
class PointSetLib_Equation
{
public:
  //! Type of geometric entity that best fits a point cloud.
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
  //! @param[in] theTol tolerance for dimension collapse detection
  Standard_EXPORT PointSetLib_Equation(const NCollection_Array1<gp_Pnt>& thePnts, double theTol);

  //! Returns the type of the fitted entity.
  Type GetType() const { return myType; }

  //! Returns true if points are coplanar within tolerance.
  bool IsPlanar() const { return myType == Type::Plane; }

  //! Returns true if points are collinear within tolerance.
  bool IsLinear() const { return myType == Type::Line; }

  //! Returns true if points are coincident within tolerance.
  bool IsPoint() const { return myType == Type::Point; }

  //! Returns true if points span 3D space (not coincident, collinear, or coplanar).
  bool IsSpace() const { return myType == Type::Space; }

  //! Returns the mean plane if points are coplanar.
  Standard_EXPORT gp_Pln Plane() const;

  //! Returns the mean line if points are collinear.
  Standard_EXPORT gp_Lin Line() const;

  //! Returns the mean point if points are coincident.
  Standard_EXPORT gp_Pnt Point() const;

  //! Returns the bounding box in principal axes if points span 3D space.
  //! @param[out] theP corner of the box (minimum extents along principal axes)
  //! @param[out] theV1 first box edge vector (along first principal axis)
  //! @param[out] theV2 second box edge vector (along second principal axis)
  //! @param[out] theV3 third box edge vector (along third principal axis)
  Standard_EXPORT void Box(gp_Pnt& theP, gp_Vec& theV1, gp_Vec& theV2, gp_Vec& theV3) const;

  //! Returns the barycentre (centre of mass) of the point cloud.
  //! Always valid after construction (regardless of fitted type).
  const gp_Pnt& Barycentre() const { return myBarycentre; }

  //! Returns the principal axis direction (unit vector) for the given index.
  //! @param[in] theIndex axis index (1, 2 or 3), ordered by eigenvalue
  const gp_Vec& PrincipalAxis(int theIndex) const { return myAxes[theIndex - 1]; }

  //! Returns the extent (max - min projection) along the given principal axis.
  //! @param[in] theIndex axis index (1, 2 or 3)
  double Extent(int theIndex) const { return myExtents[theIndex - 1]; }

private:
  Type   myType = Type::None;
  gp_Pnt myG;  //!< Type-specific point (barycentre / box corner)
  gp_Vec myV1; //!< Type-specific vector (plane normal / line direction / box edge)
  gp_Vec myV2; //!< Box edge (Space type only)
  gp_Vec myV3; //!< Box edge (Space type only)

  gp_Pnt myBarycentre;    //!< Centre of mass (always valid)
  gp_Vec myAxes[3];       //!< Principal axis unit vectors
  double myExtents[3] {}; //!< Extent along each principal axis
};

#endif // _PointSetLib_Equation_HeaderFile
