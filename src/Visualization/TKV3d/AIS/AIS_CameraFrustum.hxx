// Created on: 2018-12-12
// Created by: Olga SURYANINOVA
// Copyright (c) 2018 OPEN CASCADE SAS
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

#ifndef _AIS_CameraFrustum_HeaderFile
#define _AIS_CameraFrustum_HeaderFile

#include <AIS_InteractiveObject.hxx>

class Graphic3d_ArrayOfSegments;
class Graphic3d_ArrayOfTriangles;

//! Presentation for drawing camera frustum.
//! Default configuration is built with filling and some transparency.
class AIS_CameraFrustum : public AIS_InteractiveObject
{
  DEFINE_STANDARD_RTTIEXT(AIS_CameraFrustum, AIS_InteractiveObject)
public:
  //! Selection modes supported by this object
  enum SelectionMode
  {
    SelectionMode_Edges  = 0, //!< detect by edges (default)
    SelectionMode_Volume = 1, //!< detect by volume
  };

public:
  //! Constructs camera frustum with default configuration.
  Standard_EXPORT AIS_CameraFrustum();

  //! Sets camera frustum.
  Standard_EXPORT void SetCameraFrustum(const occ::handle<Graphic3d_Camera>& theCamera);

  //! Setup custom color.
  Standard_EXPORT void SetColor(const Quantity_Color& theColor) override;

  //! Restore default color.
  Standard_EXPORT void UnsetColor() override;

  //! Restore transparency setting.
  Standard_EXPORT void UnsetTransparency() override;

  //! Return true if specified display mode is supported.
  Standard_EXPORT bool AcceptDisplayMode(const int theMode) const override;

protected:
  //! Computes presentation of camera frustum.
  Standard_EXPORT void Compute(const occ::handle<PrsMgr_PresentationManager>& thePrsMgr,
                               const occ::handle<Prs3d_Presentation>&         thePrs,
                               const int                                      theMode) override;

  //! Compute selection.
  Standard_EXPORT void ComputeSelection(const occ::handle<SelectMgr_Selection>& theSelection,
                                        const int                               theMode) override;

private:
  //! Fills triangles primitive array for camera frustum filling.
  void fillTriangles();

  //! Fills polylines primitive array for camera frustum borders.
  void fillBorders();

protected:
  NCollection_Array1<NCollection_Vec3<double>> myPoints; //!< Array of points
  occ::handle<Graphic3d_ArrayOfTriangles> myTriangles;   //!< Triangles for camera frustum filling
  occ::handle<Graphic3d_ArrayOfSegments>  myBorders;     //!< Segments for camera frustum borders
};

#endif // _AIS_CameraFrustum_HeaderFile
