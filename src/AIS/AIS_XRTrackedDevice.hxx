// Copyright (c) 2020 OPEN CASCADE SAS
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

#ifndef _AIS_XRTrackedDevice_HeaderFile
#define _AIS_XRTrackedDevice_HeaderFile

#include <AIS_InteractiveObject.hxx>
#include <Aspect_XRTrackedDeviceRole.hxx>

class Graphic3d_ArrayOfTriangles;
class Image_Texture;

//! Auxiliary textured mesh presentation of tracked XR device.
class AIS_XRTrackedDevice : public AIS_InteractiveObject
{
  DEFINE_STANDARD_RTTIEXT(AIS_XRTrackedDevice, AIS_InteractiveObject)
public:
  //! Main constructor.
  Standard_EXPORT AIS_XRTrackedDevice (const Handle(Graphic3d_ArrayOfTriangles)& theTris,
                                       const Handle(Image_Texture)& theTexture);

  //! Empty constructor.
  Standard_EXPORT AIS_XRTrackedDevice();

  //! Return device role.
  Aspect_XRTrackedDeviceRole Role() const { return myRole; }

  //! Set device role.
  void SetRole (Aspect_XRTrackedDeviceRole theRole) { myRole = theRole; }

  //! Return laser color.
  const Quantity_Color& LaserColor() const { return myLaserColor; }

  //! Set laser color.
  Standard_EXPORT void SetLaserColor (const Quantity_Color& theColor);

  //! Return laser length.
  Standard_ShortReal LaserLength() const { return myLaserLength; }

  //! Set laser length.
  Standard_EXPORT void SetLaserLength (Standard_ShortReal theLength);

  //! Return unit scale factor.
  Standard_ShortReal UnitFactor() const { return myUnitFactor; }

  //! Set unit scale factor.
  void SetUnitFactor (Standard_ShortReal theFactor) { myUnitFactor = theFactor; }

protected:

  //! Returns true for 0 mode.
  virtual Standard_Boolean AcceptDisplayMode (const Standard_Integer theMode) const Standard_OVERRIDE { return theMode == 0; }

  //! Compute presentation.
  Standard_EXPORT virtual void Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                        const Handle(Prs3d_Presentation)& thePrs,
                                        const Standard_Integer theMode) Standard_OVERRIDE;

  //! Compute selection.
  Standard_EXPORT virtual void ComputeSelection (const Handle(SelectMgr_Selection)& theSel,
                                                 const Standard_Integer theMode) Standard_OVERRIDE;

  //! Compute laser ray presentation.
  Standard_EXPORT void computeLaserRay();

private:
  //! Texture holder.
  class XRTexture;

private:

  Handle(Graphic3d_Group) myRayGroup;

  Handle(Graphic3d_ArrayOfTriangles) myTris;
  Quantity_Color             myLaserColor;
  Standard_ShortReal         myLaserLength;
  Standard_ShortReal         myUnitFactor;
  Aspect_XRTrackedDeviceRole myRole;
  Standard_Boolean           myToShowAxes;
};

#endif // _AIS_XRTrackedDevice_HeaderFile
