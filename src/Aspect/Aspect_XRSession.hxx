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

#ifndef _Aspect_XRSession_HeaderFile
#define _Aspect_XRSession_HeaderFile

#include <Aspect_ColorSpace.hxx>
#include <Aspect_Eye.hxx>
#include <Aspect_FrustumLRBT.hxx>
#include <Aspect_GraphicsLibrary.hxx>
#include <Aspect_XRActionSet.hxx>
#include <Aspect_XRAnalogActionData.hxx>
#include <Aspect_XRDigitalActionData.hxx>
#include <Aspect_XRGenericAction.hxx>
#include <Aspect_XRHapticActionData.hxx>
#include <Aspect_XRPoseActionData.hxx>
#include <Aspect_XRTrackedDeviceRole.hxx>
#include <gp_Trsf.hxx>
#include <NCollection_Array1.hxx>

class Graphic3d_ArrayOfTriangles;
class Image_Texture;

//! Extended Reality (XR) Session interface.
class Aspect_XRSession : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Aspect_XRSession, Standard_Transient)
public:

  //! Identifies which style of tracking origin the application wants to use for the poses it is requesting.
  enum TrackingUniverseOrigin
  {
    TrackingUniverseOrigin_Seated,   //! poses are provided relative to the seated zero pose
    TrackingUniverseOrigin_Standing, //! poses are provided relative to the safe bounds configured by the user
  };

public:

  //! Return TRUE if session is opened.
  virtual bool IsOpen() const = 0;

  //! Initialize session.
  virtual bool Open() = 0;

  //! Release session.
  virtual void Close() = 0;

  //! Fetch actual poses of tracked devices.
  virtual bool WaitPoses() = 0;

  //! Return recommended viewport Width x Height for rendering into VR.
  virtual NCollection_Vec2<int> RecommendedViewport() const = 0;

  //! Return transformation from eye to head.
  virtual NCollection_Mat4<double> EyeToHeadTransform (Aspect_Eye theEye) const = 0;

  //! Return transformation from head to eye.
  NCollection_Mat4<double> HeadToEyeTransform (Aspect_Eye theEye) const
  {
    NCollection_Mat4<double> aMat;
    EyeToHeadTransform (theEye).Inverted (aMat);
    return aMat;
  }

  //! Return projection matrix.
  virtual NCollection_Mat4<double> ProjectionMatrix (Aspect_Eye theEye,
                                                     double theZNear,
                                                     double theZFar) const = 0;

  //! Return FALSE if projection frustums are unsupported and general 4x4 projection matrix should be fetched instead
  virtual bool HasProjectionFrustums() const = 0;

  //! Receive XR events.
  virtual void ProcessEvents() = 0;

  //! Submit texture eye to XR Composer.
  //! @param theTexture     [in] texture handle
  //! @param theGraphicsLib [in] graphics library in which texture handle is defined
  //! @param theColorSpace  [in] texture color space;
  //!                            sRGB means no color conversion by composer;
  //!                            Linear means to sRGB color conversion by composer
  //! @param theEye [in] eye to display
  //! @return FALSE on error
  virtual bool SubmitEye (void* theTexture,
                          Aspect_GraphicsLibrary theGraphicsLib,
                          Aspect_ColorSpace theColorSpace,
                          Aspect_Eye theEye) = 0;

  //! Return unit scale factor defined as scale factor for m (meters); 1.0 by default.
  Standard_Real UnitFactor() const { return myUnitFactor; }

  //! Set unit scale factor.
  void SetUnitFactor (Standard_Real theFactor) { myUnitFactor = theFactor; }

  //! Return aspect ratio.
  Standard_Real Aspect() const { return myAspect; }

  //! Return field of view.
  Standard_Real FieldOfView() const { return myFieldOfView; }

  //! Return Intra-ocular Distance (IOD); also known as Interpupillary Distance (IPD).
  //! Defined in meters by default (@sa UnitFactor()).
  Standard_Real IOD() const { return myIod; }

  //! Return display frequency or 0 if unknown.
  Standard_ShortReal DisplayFrequency() const { return myDispFreq; }

  //! Return projection frustum.
  //! @sa HasProjectionFrustums().
  const Aspect_FrustumLRBT<double>& ProjectionFrustum (Aspect_Eye theEye) const
  {
    return theEye == Aspect_Eye_Right ? myFrustumR : myFrustumL;
  }

  //! Return head orientation in right-handed system:
  //! +y is up
  //! +x is to the right
  //! -z is forward
  //! Distance unit is meters by default (@sa UnitFactor()).
  const gp_Trsf& HeadPose() const { return myHeadPose; }

  //! Return left hand orientation.
  gp_Trsf LeftHandPose()  const
  {
    const Standard_Integer aDevice = NamedTrackedDevice (Aspect_XRTrackedDeviceRole_LeftHand);
    return aDevice != -1 ? myTrackedPoses[aDevice].Orientation : gp_Trsf();
  }

  //! Return right hand orientation.
  gp_Trsf RightHandPose() const
  {
    const Standard_Integer aDevice = NamedTrackedDevice (Aspect_XRTrackedDeviceRole_RightHand);
    return aDevice != -1 ? myTrackedPoses[aDevice].Orientation : gp_Trsf();
  }

  //! Return number of tracked poses array.
  const Aspect_TrackedDevicePoseArray& TrackedPoses() const { return myTrackedPoses; }

  //! Return TRUE if device orientation is defined.
  bool HasTrackedPose (Standard_Integer theDevice) const { return myTrackedPoses[theDevice].IsValidPose; }

  //! Return index of tracked device of known role, or -1 if undefined.
  virtual Standard_Integer NamedTrackedDevice (Aspect_XRTrackedDeviceRole theDevice) const = 0;

  //! Load model for displaying device.
  //! @param theDevice  [in] device index
  //! @param theTexture [out] texture source
  //! @return model triangulation or NULL if not found
  Handle(Graphic3d_ArrayOfTriangles) LoadRenderModel (Standard_Integer theDevice,
                                                      Handle(Image_Texture)& theTexture)
  {
    return loadRenderModel (theDevice, true, theTexture);
  }

  //! Load model for displaying device.
  //! @param theDevice  [in] device index
  //! @param theToApplyUnitFactor [in] flag to apply unit scale factor
  //! @param theTexture [out] texture source
  //! @return model triangulation or NULL if not found
  Handle(Graphic3d_ArrayOfTriangles) LoadRenderModel (Standard_Integer theDevice,
                                                      Standard_Boolean theToApplyUnitFactor,
                                                      Handle(Image_Texture)& theTexture)
  {
    return loadRenderModel (theDevice, theToApplyUnitFactor, theTexture);
  }

  //! Fetch data for digital input action (like button).
  //! @param theAction [in] action of Aspect_XRActionType_InputDigital type
  virtual Aspect_XRDigitalActionData GetDigitalActionData (const Handle(Aspect_XRAction)& theAction) const = 0;

  //! Fetch data for digital input action (like axis).
  //! @param theAction [in] action of Aspect_XRActionType_InputAnalog type
  virtual Aspect_XRAnalogActionData GetAnalogActionData (const Handle(Aspect_XRAction)& theAction) const = 0;

  //! Fetch data for pose input action (like fingertip position).
  //! The returned values will match the values returned by the last call to WaitPoses().
  //! @param theAction [in] action of Aspect_XRActionType_InputPose type
  virtual Aspect_XRPoseActionData GetPoseActionDataForNextFrame (const Handle(Aspect_XRAction)& theAction) const = 0;

  //! Trigger vibration.
  Standard_EXPORT void TriggerHapticVibrationAction (const Handle(Aspect_XRAction)& theAction,
                                                     const Aspect_XRHapticActionData& theParams);

  //! Abort vibration.
  Standard_EXPORT void AbortHapticVibrationAction (const Handle(Aspect_XRAction)& theAction);

  //! Return tracking origin.
  TrackingUniverseOrigin TrackingOrigin() const { return myTrackOrigin; }

  //! Set tracking origin.
  virtual void SetTrackingOrigin (TrackingUniverseOrigin theOrigin) { myTrackOrigin = theOrigin; }

  //! Return generic action for specific hand or NULL if undefined.
  const Handle(Aspect_XRAction)& GenericAction (Aspect_XRTrackedDeviceRole theDevice,
                                                Aspect_XRGenericAction theAction) const
  {
    const NCollection_Array1<Handle(Aspect_XRAction)>& anActions = myRoleActions[theDevice];
    return anActions[theAction];
  }

public:

  //! Info string enumeration.
  enum InfoString
  {
    InfoString_Vendor,
    InfoString_Device,
    InfoString_Tracker,
    InfoString_SerialNumber,
  };

  //! Query information.
  virtual TCollection_AsciiString GetString (InfoString theInfo) const = 0;

protected:

  //! Empty constructor.
  Standard_EXPORT Aspect_XRSession();

  //! Load model for displaying device.
  //! @param theDevice  [in] device index
  //! @param theToApplyUnitFactor [in] flag to apply unit scale factor
  //! @param theTexture [out] texture source
  //! @return model triangulation or NULL if not found
  virtual Handle(Graphic3d_ArrayOfTriangles) loadRenderModel (Standard_Integer theDevice,
                                                              Standard_Boolean theToApplyUnitFactor,
                                                              Handle(Image_Texture)& theTexture) = 0;

  //! Trigger vibration.
  virtual void triggerHapticVibrationAction (const Handle(Aspect_XRAction)& theAction,
                                             const Aspect_XRHapticActionData& theParams) = 0;

protected:

  NCollection_Array1<Handle(Aspect_XRAction)>
                                  myRoleActions[Aspect_XRTrackedDeviceRole_NB]; //!< generic actions
  Aspect_XRActionSetMap           myActionSets;   //!< actions sets
  TrackingUniverseOrigin          myTrackOrigin;  //!< tracking origin
  Aspect_TrackedDevicePoseArray   myTrackedPoses; //!< array of tracked poses
  gp_Trsf                         myHeadPose;     //!< head orientation
  NCollection_Vec2<int>           myRendSize;     //!< viewport Width x Height for rendering into VR
  Aspect_FrustumLRBT<double>      myFrustumL;     //!< left  eye projection frustum
  Aspect_FrustumLRBT<double>      myFrustumR;     //!< right eye projection frustum
  Standard_Real                   myUnitFactor;   //!< unit scale factor defined as scale factor for m (meters)
  Standard_Real                   myAspect;       //!< aspect ratio
  Standard_Real                   myFieldOfView;  //!< field of view
  Standard_Real                   myIod;          //!< intra-ocular distance in meters
  Standard_ShortReal              myDispFreq;     //!< display frequency

};

#endif // _Aspect_XRSession_HeaderFile
