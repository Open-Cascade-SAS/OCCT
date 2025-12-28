// Created on: 1993-01-11
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

#ifndef _IGESDraw_View_HeaderFile
#define _IGESDraw_View_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <IGESData_ViewKindEntity.hxx>
class IGESGeom_Plane;
class IGESData_TransfEntity;
class gp_XYZ;

//! defines IGES View Entity, Type <410> Form <0>
//! in package IGESDraw
//!
//! Used to define a framework for specifying a viewing
//! orientation of an object in three dimensional model
//! space (X,Y,Z). The framework is also used to support
//! the projection of all or part of model space onto a
//! view volume.
class IGESDraw_View : public IGESData_ViewKindEntity
{

public:
  Standard_EXPORT IGESDraw_View();

  //! This method is used to set fields of the class View
  //! - aViewNum     : View number
  //! - aScale       : Scale factor
  //! - aLeftPlane   : Left   plane of view volume
  //! - aTopPlane    : Top    plane of view volume
  //! - aRightPlane  : Right  plane of view volume
  //! - aBottomPlane : Bottom plane of view volume
  //! - aBackPlane   : Back   plane of view volume
  //! - aFrontPlane  : Front  plane of view volume
  Standard_EXPORT void Init(const int        aViewNum,
                            const double           aScale,
                            const occ::handle<IGESGeom_Plane>& aLeftPlane,
                            const occ::handle<IGESGeom_Plane>& aTopPlane,
                            const occ::handle<IGESGeom_Plane>& aRightPlane,
                            const occ::handle<IGESGeom_Plane>& aBottomPlane,
                            const occ::handle<IGESGeom_Plane>& aBackPlane,
                            const occ::handle<IGESGeom_Plane>& aFrontPlane);

  //! Returns True (for a single view)
  Standard_EXPORT bool IsSingle() const override;

  //! Returns 1 (single view)
  Standard_EXPORT int NbViews() const override;

  //! For a single view, returns <me> whatever <num>
  Standard_EXPORT occ::handle<IGESData_ViewKindEntity> ViewItem(const int num) const
    override;

  //! returns integer number identifying view orientation
  Standard_EXPORT int ViewNumber() const;

  //! returns the scale factor(Default = 1.0)
  Standard_EXPORT double ScaleFactor() const;

  //! returns False if left side of view volume is not present
  Standard_EXPORT bool HasLeftPlane() const;

  //! returns the left side of view volume, or null handle
  Standard_EXPORT occ::handle<IGESGeom_Plane> LeftPlane() const;

  //! returns False if top of view volume is not present
  Standard_EXPORT bool HasTopPlane() const;

  //! returns the top of view volume, or null handle
  Standard_EXPORT occ::handle<IGESGeom_Plane> TopPlane() const;

  //! returns False if right side of view volume is not present
  Standard_EXPORT bool HasRightPlane() const;

  //! returns the right side of view volume, or null handle
  Standard_EXPORT occ::handle<IGESGeom_Plane> RightPlane() const;

  //! returns False if bottom of view volume is not present
  Standard_EXPORT bool HasBottomPlane() const;

  //! returns the bottom of view volume, or null handle
  Standard_EXPORT occ::handle<IGESGeom_Plane> BottomPlane() const;

  //! returns False if back of view volume is not present
  Standard_EXPORT bool HasBackPlane() const;

  //! returns the back of view volume, or null handle
  Standard_EXPORT occ::handle<IGESGeom_Plane> BackPlane() const;

  //! returns False if front of view volume is not present
  Standard_EXPORT bool HasFrontPlane() const;

  //! returns the front of view volume, or null handle
  Standard_EXPORT occ::handle<IGESGeom_Plane> FrontPlane() const;

  //! returns the Transformation Matrix
  Standard_EXPORT occ::handle<IGESData_TransfEntity> ViewMatrix() const;

  //! returns XYZ from the Model space to the View space by
  //! applying the View Matrix
  Standard_EXPORT gp_XYZ ModelToView(const gp_XYZ& coords) const;

  DEFINE_STANDARD_RTTIEXT(IGESDraw_View, IGESData_ViewKindEntity)

private:
  int       theViewNumber;
  double          theScaleFactor;
  occ::handle<IGESGeom_Plane> theLeftPlane;
  occ::handle<IGESGeom_Plane> theTopPlane;
  occ::handle<IGESGeom_Plane> theRightPlane;
  occ::handle<IGESGeom_Plane> theBottomPlane;
  occ::handle<IGESGeom_Plane> theBackPlane;
  occ::handle<IGESGeom_Plane> theFrontPlane;
};

#endif // _IGESDraw_View_HeaderFile
