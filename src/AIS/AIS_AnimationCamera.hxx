// Created by: Anastasia BORISOVA
// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _AIS_AnimationCamera_HeaderFile
#define _AIS_AnimationCamera_HeaderFile

#include <AIS_Animation.hxx>

class Graphic3d_Camera;
class V3d_View;

//! Camera animation.
class AIS_AnimationCamera : public AIS_Animation
{
  DEFINE_STANDARD_RTTIEXT(AIS_AnimationCamera, AIS_Animation)
public:

  //! Main constructor.
  Standard_EXPORT AIS_AnimationCamera (const TCollection_AsciiString& theAnimationName,
                                       const Handle(V3d_View)& theView);

  //! Return the target view.
  const Handle(V3d_View)& View() const { return myView; }

  //! Set target view.
  void SetView (const Handle(V3d_View)& theView) { myView = theView; }

  //! Return camera start position.
  const Handle(Graphic3d_Camera)& CameraStart() const { return myCamStart; }

  //! Define camera start position.
  void SetCameraStart (const Handle(Graphic3d_Camera)& theCameraStart) { myCamStart = theCameraStart; }

  //! Return camera end position.
  const Handle(Graphic3d_Camera)& CameraEnd() const { return myCamEnd; }

  //! Define camera end position.
  void SetCameraEnd (const Handle(Graphic3d_Camera)& theCameraEnd) { myCamEnd = theCameraEnd; }

protected:

  //! Update the progress.
  Standard_EXPORT virtual void update (const AIS_AnimationProgress& theProgress) Standard_OVERRIDE;

protected:

  Handle(V3d_View)         myView;        //!< view to setup camera
  Handle(Graphic3d_Camera) myCamStart;    //!< starting camera position
  Handle(Graphic3d_Camera) myCamEnd;      //!< end camera position

};

DEFINE_STANDARD_HANDLE(AIS_AnimationCamera, AIS_Animation)

#endif // _AIS_AnimationCamera_HeaderFile
