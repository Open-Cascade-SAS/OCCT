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
  Standard_EXPORT void SetCameraFrustum (const Handle(Graphic3d_Camera)& theCamera);

  //! Setup custom color.
  Standard_EXPORT virtual void SetColor (const Quantity_Color& theColor) Standard_OVERRIDE;

  //! Restore default color.
  Standard_EXPORT virtual void UnsetColor() Standard_OVERRIDE;

  //! Restore transparency setting.
  Standard_EXPORT virtual void UnsetTransparency() Standard_OVERRIDE;

  //! Return true if specified display mode is supported.
  Standard_EXPORT virtual Standard_Boolean AcceptDisplayMode (const Standard_Integer theMode) const Standard_OVERRIDE;

protected:

  //! Computes presentation of camera frustum.
  Standard_EXPORT virtual void Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                        const Handle(Prs3d_Presentation)& thePrs,
                                        const Standard_Integer theMode) Standard_OVERRIDE;

  //! Compute selection.
  Standard_EXPORT virtual void ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                                                 const Standard_Integer             theMode) Standard_OVERRIDE;

private:

  //! Fills triangles primitive array for camera frustum filling.
  void fillTriangles();

  //! Fills polylines primitive array for camera frustum borders.
  void fillBorders();

protected:

  NCollection_Array1<Graphic3d_Vec3d> myPoints;    //!< Array of points
  Handle(Graphic3d_ArrayOfTriangles)  myTriangles; //!< Triangles for camera frustum filling
  Handle(Graphic3d_ArrayOfSegments)   myBorders;   //!< Segments for camera frustum borders

};

#endif // _AIS_CameraFrustum_HeaderFile
