// Created on: 2011-10-14
// Created by: Roman KOZLOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#ifndef __IVTKVTK_VIEW_H__
#define __IVTKVTK_VIEW_H__

#include <IVtk_IView.hxx>
#include <vtkSmartPointer.h>

class vtkRenderer;

//! @class IVtkVTK_View
//! @brief ICamera implementation for VTK.
//!
//! This class is used for obtaining view transformation parameters.
//! These parameters are used by selection algorithm to compute
//! projections of selectable (active) 3D shapes.
class IVtkVTK_View : public IVtk_IView
{

public:
  typedef occ::handle<IVtkVTK_View> Handle;

  Standard_EXPORT IVtkVTK_View(vtkRenderer* theRenderer);

  //! Destructor
  Standard_EXPORT ~IVtkVTK_View() override;

  //! @return true if this is a perspective view, and false otherwise.
  Standard_EXPORT bool IsPerspective() const override;

  //! @return The focal distance of the view
  Standard_EXPORT double GetDistance() const override;

  //! @return The world coordinates of the camera position
  Standard_EXPORT void GetEyePosition(double& theX,
                                              double& theY,
                                              double& theZ) const override;

  //! @return The world coordinates of the view position
  Standard_EXPORT void GetPosition(double& theX, double& theY, double& theZ) const override;

  //! @return The "view up" direction of the view
  Standard_EXPORT void GetViewUp(double& theDx,
                                         double& theDy,
                                         double& theDz) const override;

  //! @return The projection direction vector of this view
  Standard_EXPORT void GetDirectionOfProjection(double& theDx,
                                                        double& theDy,
                                                        double& theDz) const override;

  //! @return Three doubles containing scale components of the view transformation
  Standard_EXPORT void GetScale(double& theX, double& theY, double& theZ) const override;

  //! @return The current view's zoom factor (for parallel projection)
  Standard_EXPORT double GetParallelScale() const override;

  //! @return The current view angle (for perspective projection)
  Standard_EXPORT double GetViewAngle() const override;

  //! @return The location of the near and far clipping planes along the direction of projection
  Standard_EXPORT void GetClippingRange(double& theZNear, double& theZFar) const override;

  //! @return The current view the aspect ratio
  Standard_EXPORT double GetAspectRatio() const override;

  //! @return Two doubles containing the display coordinates of the view window center
  Standard_EXPORT void GetViewCenter(double& theX, double& theY) const override;

  //! Gets window size in screen coordinates in pixels
  Standard_EXPORT void GetWindowSize(int& theX, int& theY) const override;

  //! Gets camera projection and orientation matrices
  Standard_EXPORT void GetCamera(NCollection_Mat4<double>& theProj,
                                         NCollection_Mat4<double>& theOrient,
                                         bool&                     theIsOrtho) const override;

  //! Gets viewport coordinates
  Standard_EXPORT void GetViewport(double& theX,
                                           double& theY,
                                           double& theWidth,
                                           double& theHeight) const override;

  //! Converts 3D display coordinates into 3D world coordinates.
  //! @param[in]  theDisplayPnt 2d point of display coordinates
  //! @param[out]  theWorldPnt 3d point of world coordinates
  //! @return true if conversion was successful, false otherwise
  Standard_EXPORT bool DisplayToWorld(const gp_XY& theDisplayPnt,
                                              gp_XYZ&      theWorldPnt) const override;

  DEFINE_STANDARD_RTTIEXT(IVtkVTK_View, IVtk_IView)

private:
  vtkSmartPointer<vtkRenderer> myRenderer;
};

#endif // __IVTKVTK_VIEW_H__
