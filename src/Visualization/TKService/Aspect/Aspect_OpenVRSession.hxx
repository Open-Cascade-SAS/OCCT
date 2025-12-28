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
  Standard_EXPORT ~Aspect_OpenVRSession() override;

  //! Return TRUE if session is opened.
  Standard_EXPORT bool IsOpen() const override;

  //! Initialize session.
  Standard_EXPORT bool Open() override;

  //! Release session.
  Standard_EXPORT void Close() override;

  //! Fetch actual poses of tracked devices.
  Standard_EXPORT bool WaitPoses() override;

  //! Return recommended viewport Width x Height for rendering into VR.
  NCollection_Vec2<int> RecommendedViewport() const override { return myRendSize; }

  //! Return transformation from eye to head.
  //! vr::GetEyeToHeadTransform() wrapper.
  Standard_EXPORT NCollection_Mat4<double> EyeToHeadTransform(
    Aspect_Eye theEye) const override;

  //! Return projection matrix.
  Standard_EXPORT NCollection_Mat4<double> ProjectionMatrix(Aspect_Eye theEye,
                                                                    double     theZNear,
                                                                    double theZFar) const override;

  //! Return TRUE.
  bool HasProjectionFrustums() const override { return true; }

  //! Receive XR events.
  Standard_EXPORT void ProcessEvents() override;

  //! Submit texture eye to XR Composer.
  //! @param[in] theTexture      texture handle
  //! @param[in] theGraphicsLib  graphics library in which texture handle is defined
  //! @param[in] theColorSpace   texture color space;
  //!                            sRGB means no color conversion by composer;
  //!                            Linear means to sRGB color conversion by composer
  //! @param[in] theEye  eye to display
  //! @return FALSE on error
  Standard_EXPORT bool SubmitEye(void*                  theTexture,
                                         Aspect_GraphicsLibrary theGraphicsLib,
                                         Aspect_ColorSpace      theColorSpace,
                                         Aspect_Eye             theEye) override;

  //! Query information.
  Standard_EXPORT TCollection_AsciiString GetString(InfoString theInfo) const override;

  //! Return index of tracked device of known role.
  Standard_EXPORT int NamedTrackedDevice(
    Aspect_XRTrackedDeviceRole theDevice) const override;

  //! Fetch data for digital input action (like button).
  Standard_EXPORT Aspect_XRDigitalActionData GetDigitalActionData(
    const occ::handle<Aspect_XRAction>& theAction) const override;

  //! Fetch data for analog input action (like axis).
  Standard_EXPORT Aspect_XRAnalogActionData GetAnalogActionData(
    const occ::handle<Aspect_XRAction>& theAction) const override;

  //! Fetch data for pose input action (like fingertip position).
  Standard_EXPORT Aspect_XRPoseActionData GetPoseActionDataForNextFrame(
    const occ::handle<Aspect_XRAction>& theAction) const override;

  //! Set tracking origin.
  Standard_EXPORT void SetTrackingOrigin(TrackingUniverseOrigin theOrigin) override;

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
  Standard_EXPORT virtual void onTrackedDeviceActivated(int theDeviceIndex);

  //! Handle tracked device deactivation.
  Standard_EXPORT virtual void onTrackedDeviceDeactivated(int theDeviceIndex);

  //! Handle tracked device update.
  Standard_EXPORT virtual void onTrackedDeviceUpdated(int theDeviceIndex);

  //! Trigger vibration.
  Standard_EXPORT void triggerHapticVibrationAction(
    const occ::handle<Aspect_XRAction>& theAction,
    const Aspect_XRHapticActionData&    theParams) override;

  //! Return model for displaying device.
  Standard_EXPORT occ::handle<Graphic3d_ArrayOfTriangles> loadRenderModel(
    int                         theDevice,
    bool                        theToApplyUnitFactor,
    occ::handle<Image_Texture>& theTexture) override;

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
