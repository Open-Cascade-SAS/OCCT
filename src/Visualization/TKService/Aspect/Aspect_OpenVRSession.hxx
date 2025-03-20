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

#ifndef _Aspect_OpenVRSession_HeaderFile
#define _Aspect_OpenVRSession_HeaderFile

#include <Aspect_XRSession.hxx>

//! OpenVR wrapper implementing Aspect_XRSession interface.
class Aspect_OpenVRSession : public Aspect_XRSession
{
  DEFINE_STANDARD_RTTIEXT(Aspect_OpenVRSession, Aspect_XRSession)
public:
  //! Return TRUE if an HMD may be presented on the system (e.g. to show VR checkbox in application
  //! GUI). This is fast check, and even if it returns TRUE, opening session may fail.
  Standard_EXPORT static bool IsHmdPresent();

public:
  //! Empty constructor.
  Standard_EXPORT Aspect_OpenVRSession();

  //! Destructor.
  Standard_EXPORT virtual ~Aspect_OpenVRSession();

  //! Return TRUE if session is opened.
  Standard_EXPORT virtual bool IsOpen() const Standard_OVERRIDE;

  //! Initialize session.
  Standard_EXPORT virtual bool Open() Standard_OVERRIDE;

  //! Release session.
  Standard_EXPORT virtual void Close() Standard_OVERRIDE;

  //! Fetch actual poses of tracked devices.
  Standard_EXPORT virtual bool WaitPoses() Standard_OVERRIDE;

  //! Return recommended viewport Width x Height for rendering into VR.
  virtual NCollection_Vec2<int> RecommendedViewport() const Standard_OVERRIDE { return myRendSize; }

  //! Return transformation from eye to head.
  //! vr::GetEyeToHeadTransform() wrapper.
  Standard_EXPORT virtual NCollection_Mat4<double> EyeToHeadTransform(Aspect_Eye theEye) const
    Standard_OVERRIDE;

  //! Return projection matrix.
  Standard_EXPORT virtual NCollection_Mat4<double> ProjectionMatrix(Aspect_Eye theEye,
                                                                    double     theZNear,
                                                                    double     theZFar) const
    Standard_OVERRIDE;

  //! Return TRUE.
  virtual bool HasProjectionFrustums() const Standard_OVERRIDE { return true; }

  //! Receive XR events.
  Standard_EXPORT virtual void ProcessEvents() Standard_OVERRIDE;

  //! Submit texture eye to XR Composer.
  //! @param[in] theTexture      texture handle
  //! @param[in] theGraphicsLib  graphics library in which texture handle is defined
  //! @param[in] theColorSpace   texture color space;
  //!                            sRGB means no color conversion by composer;
  //!                            Linear means to sRGB color conversion by composer
  //! @param[in] theEye  eye to display
  //! @return FALSE on error
  Standard_EXPORT virtual bool SubmitEye(void*                  theTexture,
                                         Aspect_GraphicsLibrary theGraphicsLib,
                                         Aspect_ColorSpace      theColorSpace,
                                         Aspect_Eye             theEye) Standard_OVERRIDE;

  //! Query information.
  Standard_EXPORT virtual TCollection_AsciiString GetString(InfoString theInfo) const
    Standard_OVERRIDE;

  //! Return index of tracked device of known role.
  Standard_EXPORT virtual Standard_Integer NamedTrackedDevice(
    Aspect_XRTrackedDeviceRole theDevice) const Standard_OVERRIDE;

  //! Fetch data for digital input action (like button).
  Standard_EXPORT virtual Aspect_XRDigitalActionData GetDigitalActionData(
    const Handle(Aspect_XRAction)& theAction) const Standard_OVERRIDE;

  //! Fetch data for analog input action (like axis).
  Standard_EXPORT virtual Aspect_XRAnalogActionData GetAnalogActionData(
    const Handle(Aspect_XRAction)& theAction) const Standard_OVERRIDE;

  //! Fetch data for pose input action (like fingertip position).
  Standard_EXPORT virtual Aspect_XRPoseActionData GetPoseActionDataForNextFrame(
    const Handle(Aspect_XRAction)& theAction) const Standard_OVERRIDE;

  //! Set tracking origin.
  Standard_EXPORT virtual void SetTrackingOrigin(TrackingUniverseOrigin theOrigin)
    Standard_OVERRIDE;

protected:
  //! Find location of default actions manifest file (based on CSF_OCCTResourcePath or CASROOT
  //! variables).
  Standard_EXPORT TCollection_AsciiString defaultActionsManifest();

  //! Release OpenVR device.
  Standard_EXPORT void closeVR();

  //! Update projection frustums.
  Standard_EXPORT virtual void updateProjectionFrustums();

  //! Init VR input.
  Standard_EXPORT virtual bool initInput();

  //! Handle tracked device activation.
  Standard_EXPORT virtual void onTrackedDeviceActivated(Standard_Integer theDeviceIndex);

  //! Handle tracked device deactivation.
  Standard_EXPORT virtual void onTrackedDeviceDeactivated(Standard_Integer theDeviceIndex);

  //! Handle tracked device update.
  Standard_EXPORT virtual void onTrackedDeviceUpdated(Standard_Integer theDeviceIndex);

  //! Trigger vibration.
  Standard_EXPORT virtual void triggerHapticVibrationAction(
    const Handle(Aspect_XRAction)&   theAction,
    const Aspect_XRHapticActionData& theParams) Standard_OVERRIDE;

  //! Return model for displaying device.
  Standard_EXPORT virtual Handle(Graphic3d_ArrayOfTriangles) loadRenderModel(
    Standard_Integer       theDevice,
    Standard_Boolean       theToApplyUnitFactor,
    Handle(Image_Texture)& theTexture) Standard_OVERRIDE;

protected:
  //! Access vr::IVRSystem* - OpenVR session object.
  Standard_EXPORT void* getVRSystem() const;

private:
  //! Internal fields
  struct VRContext;
  class VRImagePixmap;
  class VRTextureSource;

protected:
  VRContext*              myContext;
  TCollection_AsciiString myActionsManifest;
};

#endif // _Aspect_OpenVRSession_HeaderFile
