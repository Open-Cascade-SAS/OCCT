// Created on: 2011-10-11 
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

#ifndef __IVTK_IVIEW_H__
#define __IVTK_IVIEW_H__

#include <IVtk_Interface.hxx>
#include <gp_XY.hxx>
#include <gp_XYZ.hxx>
#include <gp_Pnt.hxx>
#include <Graphic3d_Mat4d.hxx>

class IVtk_IView;
DEFINE_STANDARD_HANDLE( IVtk_IView, IVtk_Interface )

//! @class IVtk_IView 
//! @brief Interface for obtaining view transformation parameters.
//!
//! These parameters are used by selection algorithm to compute 
//! projections of selectable (active) 3D shapes.
class IVtk_IView : public IVtk_Interface
{

public:
  typedef Handle(IVtk_IView) Handle;

  //! Destructor
  virtual ~IVtk_IView() { }

  //! @return true if this is a perspective view, and false otherwise.
  virtual bool    IsPerspective() const = 0;

  //! @return The focal distance of the view
  virtual double  GetDistance() const = 0;

  //! @return The world coordinates of the camera position
  virtual void    GetEyePosition (double& theX, double& theY, double& theZ) const = 0;

  //! @return The world coordinates of the view position
  virtual void    GetPosition (double& theX, double& theY, double& theZ) const = 0;

  //! @return The "view up" direction of the view
  virtual void    GetViewUp (double& theDx, double& theDy, double& theDz) const = 0;

  //! @return The projection direction vector of this view
  virtual void    GetDirectionOfProjection (double& theDx, 
                                            double& theDy, 
                                            double& theDz) const = 0;

  //! @return Three doubles containing scale components of the view transformation
  virtual void    GetScale (double& theX, double& theY, double& theZ) const = 0;

  //! @return The current view's zoom factor (for parallel projection)
  virtual double  GetParallelScale() const = 0;

  //! @return The current view angle (for perspective projection)
  virtual double  GetViewAngle() const = 0;

  //! @return The location of the near and far clipping planes along the direction of projection
  virtual void    GetClippingRange (double& theZNear, double& theZFar) const = 0;

  //! @return The current view the aspect ratio
  virtual double  GetAspectRatio() const = 0;

  //! @return Two doubles containing the display coordinates of the view window center 
  virtual void    GetViewCenter (double& theX, double& theY) const = 0;

  //! Gets window size in screen coordinates in pixels
  virtual void    GetWindowSize (int& theX, int& theY) const = 0;

  //! Gets camera projection and orientation matrices
  virtual void    GetCamera (Graphic3d_Mat4d& theProj,
                             Graphic3d_Mat4d& theOrient,
                             Standard_Boolean& theIsOrtho) const = 0;

  //! Converts 3D display coordinates into 3D world coordinates.
  //! @param [in] theDisplayPnt 2d point of display coordinates
  //! @param [out] theWorldPnt 3d point of world coordinates
  //! @return true if conversion was successful, false otherwise
  virtual bool    DisplayToWorld (const gp_XY& theDisplayPnt, gp_XYZ& theWorldPnt) const = 0;

  //! Gets viewport coordinates
  virtual void    GetViewport (Standard_Real& theX,
                               Standard_Real& theY,
                               Standard_Real& theWidth,
                               Standard_Real& theHeight) const = 0;

  DEFINE_STANDARD_RTTIEXT(IVtk_IView,IVtk_Interface)
};

#endif // __IVTK_IVIEW_H__
