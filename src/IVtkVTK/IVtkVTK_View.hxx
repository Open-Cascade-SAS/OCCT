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

class IVtkVTK_View;
DEFINE_STANDARD_HANDLE( IVtkVTK_View, IVtk_IView )

//! @class IVtkVTK_View
//! @brief ICamera implementation for VTK.
//!
//! This class is used for obtaining view transformation parameters.
//! These parameters are used by selection algorithm to compute 
//! projections of selectable (active) 3D shapes.
class IVtkVTK_View : public IVtk_IView
{

public:
  typedef Handle(IVtkVTK_View) Handle;

  Standard_EXPORT IVtkVTK_View (vtkRenderer* theRenderer);

  //! Destructor
  Standard_EXPORT virtual ~IVtkVTK_View();

  //! @return true if this is a perspective view, and false otherwise.
  Standard_EXPORT virtual bool    IsPerspective() const Standard_OVERRIDE;

  //! @return The focal distance of the view
  Standard_EXPORT virtual double  GetDistance() const Standard_OVERRIDE;

  //! @return The world coordinates of the view position
  Standard_EXPORT virtual void    GetPosition (double& theX, double& theY, double& theZ) const Standard_OVERRIDE;

  //! @return The "view up" direction of the view
  Standard_EXPORT virtual void    GetViewUp (double& theDx, double& theDy, double& theDz) const Standard_OVERRIDE;

  //! @return The projection direction vector of this view
  Standard_EXPORT virtual void    GetDirectionOfProjection (double& theDx,
                                                            double& theDy,
                                                            double& theDz) const Standard_OVERRIDE;

  //! @return Three doubles containing scale components of the view transformation
  Standard_EXPORT virtual void    GetScale (double& theX, double& theY, double& theZ) const Standard_OVERRIDE;

  //! @return The current view's zoom factor (for parallel projection)
  Standard_EXPORT virtual double  GetParallelScale() const Standard_OVERRIDE;

  //! @return The current view angle (for perspective projection)
  Standard_EXPORT virtual double  GetViewAngle() const Standard_OVERRIDE;

  //! @return Two doubles containing the display coordinates of the view window center 
  Standard_EXPORT virtual void    GetViewCenter (double& theX, double& theY) const Standard_OVERRIDE;

  //! Gets window size in screen coordinates in pixels
  Standard_EXPORT virtual void    GetWindowSize (int& theX, int& theY) const Standard_OVERRIDE;

  //! Gets camera projection and orientation matrices
  Standard_EXPORT virtual void    GetCamera (Graphic3d_Mat4d& theProj,
                                             Graphic3d_Mat4d& theOrient,
                                             Standard_Boolean& theIsOrtho) const Standard_OVERRIDE;

  //! Gets viewport coordinates
  Standard_EXPORT virtual void   GetViewport (Standard_Real& theX,
                                              Standard_Real& theY,
                                              Standard_Real& theWidth,
                                              Standard_Real& theHeight) const Standard_OVERRIDE;

  //! Converts 3D display coordinates into 3D world coordinates.
  //! @param [in] theDisplayPnt 2d point of display coordinates
  //! @param [out] theWorldPnt 3d point of world coordinates
  //! @return true if conversion was successful, false otherwise
  Standard_EXPORT virtual bool DisplayToWorld (const gp_XY& theDisplayPnt, gp_XYZ& theWorldPnt) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IVtkVTK_View,IVtk_IView)

private:
  vtkSmartPointer<vtkRenderer> myRenderer;
};

#endif // __IVTKVTK_VIEW_H__
