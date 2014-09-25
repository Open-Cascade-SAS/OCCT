// Created on: 2014-08-13
// Created by: Maxim GLIBIN
// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef _AIS_PointCloud_HeaderFile
#define _AIS_PointCloud_HeaderFile

#include <AIS.hxx>
#include <AIS_InteractiveObject.hxx>
#include <Bnd_Box.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <Quantity_HArray1OfColor.hxx>
#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <Standard_Macro.hxx>
#include <TColgp_HArray1OfDir.hxx>
#include <TColgp_HArray1OfPnt.hxx>

DEFINE_STANDARD_HANDLE(AIS_PointCloud, AIS_InteractiveObject)

//! Interactive object for set of points.
//! The presentation supports two display modes:
//! - Points.
//! - Bounding box for highlighting.
//! Presentation provides selection by bouding box.
//! Selection and consequently highlighting can disabled by
//! setting default selection mode to -1. There will be no way
//! to select object from interactive view. Any calls to
//! AIS_InteractiveContext::AddOrRemoveSelected should be also prohibited,
//! to avoid programmatic highlighting (workaround is setting non-supported
//! hilight mode, e.g. 100);
class AIS_PointCloud : public AIS_InteractiveObject
{

public:

  //! Display modes supported by this Point Cloud object
  enum DisplayMode
  {
    DM_Points = 0, //!< display as normal points, default presentation
    DM_BndBox = 2  //!< display as bounding box,  default for highlighting
  };

public:

  //! Constructor.
  Standard_EXPORT AIS_PointCloud();

  //! Sets the points from array of points.
  //! Method will not copy the input data - array will be stored as handle.
  //! @param thePoints [in] the array of points
  Standard_EXPORT virtual void SetPoints (const Handle(Graphic3d_ArrayOfPoints)& thePoints);

  //! Sets the points with optional colors.
  //! The input data will be copied into internal buffer.
  //! The input arrays should have equal length, otherwise
  //! the presentation will not be computed and displayed.
  //! @param theCoords  [in] the array of coordinates
  //! @param theColors  [in] optional array of colors
  //! @param theNormals [in] optional array of normals
  Standard_EXPORT virtual void SetPoints (const Handle(TColgp_HArray1OfPnt)&     theCoords,
                                          const Handle(Quantity_HArray1OfColor)& theColors  = NULL,
                                          const Handle(TColgp_HArray1OfDir)&     theNormals = NULL);

public:

  //! Get the points array.
  //! Method might be overridden to fill in points array dynamically from application data structures.
  //! @return the array of points
  Standard_EXPORT virtual const Handle(Graphic3d_ArrayOfPoints) GetPoints() const;

  //! Get bounding box for presentation.
  Standard_EXPORT virtual Bnd_Box GetBoundingBox() const;

public:

  //! Setup custom color. Affects presentation only when no per-point color attribute has been assigned.
  Standard_EXPORT virtual void SetColor (const Quantity_NameOfColor theColor);

  //! Setup custom color. Affects presentation only when no per-point color attribute has been assigned.
  Standard_EXPORT virtual void SetColor (const Quantity_Color& theColor);

  //! Restore default color.
  Standard_EXPORT virtual void UnsetColor();

  //! Setup custom material. Affects presentation only when normals are defined.
  Standard_EXPORT virtual void SetMaterial (const Graphic3d_NameOfMaterial theMatName);

  //! Setup custom material. Affects presentation only when normals are defined.
  Standard_EXPORT virtual void SetMaterial (const Graphic3d_MaterialAspect& theMat);

  //! Restore default material.
  Standard_EXPORT virtual void UnsetMaterial();

protected:

  //! Prepare presentation for this object.
  Standard_EXPORT virtual void Compute (const Handle(PrsMgr_PresentationManager3d)& thePrsMgr,
                                        const Handle(Prs3d_Presentation)&           thePrs,
                                        const Standard_Integer                      theMode);

  //! Prepare selection for this object.
  Standard_EXPORT virtual void ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                                                 const Standard_Integer             theMode);

private:

  Handle(Graphic3d_ArrayOfPoints) myPoints;    //!< points array for presentation
  Bnd_Box                         myBndBox;    //!< bounding box for presentation

public:

  DEFINE_STANDARD_RTTI(AIS_PointCloud)

};

#endif // _AIS_PointCloud_HeaderFile
