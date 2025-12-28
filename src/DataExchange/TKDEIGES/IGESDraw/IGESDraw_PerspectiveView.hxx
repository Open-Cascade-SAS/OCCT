// Created on: 1993-02-03
// Created by: CKY / Contract Toubro-Larsen ( Niraj RANGWALA )
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

#ifndef _IGESDraw_PerspectiveView_HeaderFile
#define _IGESDraw_PerspectiveView_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <gp_XYZ.hxx>
#include <gp_XY.hxx>
#include <IGESData_ViewKindEntity.hxx>
class gp_Vec;
class gp_Pnt;
class gp_Pnt2d;
class IGESData_TransfEntity;

//! defines IGESPerspectiveView, Type <410> Form <1>
//! in package IGESDraw
//!
//! Supports a perspective view.
//! Any geometric projection is defined by a view plane
//! and the projectors that pass through the view plane.
//! Projectors can be visualized as rays of light that
//! form an image by passing through the viewed object
//! and striking the view plane.
//! The projectors are defined via a point called the
//! Centre-of-Projection or the eye-point.
//! A perspective view is formed by all projectors that
//! emanate from the Centre-of-Projection and pass
//! through the view plane.
class IGESDraw_PerspectiveView : public IGESData_ViewKindEntity
{

public:
  Standard_EXPORT IGESDraw_PerspectiveView();

  //! This method is used to set the fields of the class
  //! PerspectiveView
  //! - aViewNumber         : The desired view
  //! - aScaleFactor        : Scale factor
  //! - aViewNormalVector   : View plane normal vector (model space)
  //! - aViewReferencePoint : View reference point     (model space)
  //! - aCenterOfProjection : Center Of Projection     (model space)
  //! - aViewUpVector       : View up vector           (model space)
  //! - aViewPlaneDistance  : View plane distance      (model space)
  //! - aTopLeft            : Top-left point of clipping window
  //! - aBottomRight        : Bottom-right point of clipping window
  //! - aDepthClip          : Depth clipping indicator
  //! - aBackPlaneDistance  : Distance of back clipping plane
  //! - aFrontPlaneDistance : Distance of front clipping plane
  Standard_EXPORT void Init(const int     aViewNumber,
                            const double  aScaleFactor,
                            const gp_XYZ& aViewNormalVector,
                            const gp_XYZ& aViewReferencePoint,
                            const gp_XYZ& aCenterOfProjection,
                            const gp_XYZ& aViewUpVector,
                            const double  aViewPlaneDistance,
                            const gp_XY&  aTopLeft,
                            const gp_XY&  aBottomRight,
                            const int     aDepthClip,
                            const double  aBackPlaneDistance,
                            const double  aFrontPlaneDistance);

  //! Returns True (for a single view)
  Standard_EXPORT bool IsSingle() const override;

  //! Returns 1 (single view)
  Standard_EXPORT int NbViews() const override;

  //! For a single view, returns <me> whatever <num>
  Standard_EXPORT occ::handle<IGESData_ViewKindEntity> ViewItem(const int num) const override;

  //! returns the view number associated with <me>
  Standard_EXPORT int ViewNumber() const;

  //! returns the scale factor associated with <me>
  Standard_EXPORT double ScaleFactor() const;

  //! returns the View plane normal vector (model space)
  Standard_EXPORT gp_Vec ViewNormalVector() const;

  //! returns the View reference point (model space)
  Standard_EXPORT gp_Pnt ViewReferencePoint() const;

  //! returns the Center Of Projection (model space)
  Standard_EXPORT gp_Pnt CenterOfProjection() const;

  //! returns the View up vector (model space)
  Standard_EXPORT gp_Vec ViewUpVector() const;

  //! returns the View plane distance (model space)
  Standard_EXPORT double ViewPlaneDistance() const;

  //! returns the top left point of the clipping window
  Standard_EXPORT gp_Pnt2d TopLeft() const;

  //! returns the bottom right point of the clipping window
  Standard_EXPORT gp_Pnt2d BottomRight() const;

  //! returns the Depth clipping indicator
  //! 0 = No depth clipping
  //! 1 = Back clipping plane ON
  //! 2 = Front clipping plane ON
  //! 3 = Back and front clipping planes ON
  Standard_EXPORT int DepthClip() const;

  //! returns the View coordinate denoting the location of
  //! the back clipping plane
  Standard_EXPORT double BackPlaneDistance() const;

  //! returns the View coordinate denoting the location of
  //! the front clipping plane
  Standard_EXPORT double FrontPlaneDistance() const;

  //! returns the Transformation Matrix
  Standard_EXPORT occ::handle<IGESData_TransfEntity> ViewMatrix() const;

  //! returns XYX from the Model space to the View space by
  //! applying the View Matrix
  Standard_EXPORT gp_XYZ ModelToView(const gp_XYZ& coords) const;

  DEFINE_STANDARD_RTTIEXT(IGESDraw_PerspectiveView, IGESData_ViewKindEntity)

private:
  int    theViewNumber;
  double theScaleFactor;
  gp_XYZ theViewNormalVector;
  gp_XYZ theViewReferencePoint;
  gp_XYZ theCenterOfProjection;
  gp_XYZ theViewUpVector;
  double theViewPlaneDistance;
  gp_XY  theTopLeft;
  gp_XY  theBottomRight;
  int    theDepthClip;
  double theBackPlaneDistance;
  double theFrontPlaneDistance;
};

#endif // _IGESDraw_PerspectiveView_HeaderFile
