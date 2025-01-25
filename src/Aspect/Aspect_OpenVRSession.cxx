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

#include <Aspect_OpenVRSession.hxx>

#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Image_PixMap.hxx>
#include <Image_Texture.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <NCollection_LocalArray.hxx>
#include <OSD.hxx>
#include <OSD_Environment.hxx>
#include <OSD_File.hxx>
#include <OSD_Process.hxx>

#ifdef HAVE_OPENVR
  #include <openvr.h>

namespace
{
//! Print OpenVR compositor error.
static const char* getVRCompositorError(vr::EVRCompositorError theVRError)
{
  switch (theVRError)
  {
    case vr::VRCompositorError_None:
      return "None";
    case vr::VRCompositorError_RequestFailed:
      return "Request Failed";
    case vr::VRCompositorError_IncompatibleVersion:
      return "Incompatible Version";
    case vr::VRCompositorError_DoNotHaveFocus:
      return "Do not have focus";
    case vr::VRCompositorError_InvalidTexture:
      return "Invalid Texture";
    case vr::VRCompositorError_IsNotSceneApplication:
      return "Is not scene application";
    case vr::VRCompositorError_TextureIsOnWrongDevice:
      return "Texture is on wrong device";
    case vr::VRCompositorError_TextureUsesUnsupportedFormat:
      return "Texture uses unsupported format";
    case vr::VRCompositorError_SharedTexturesNotSupported:
      return "Shared textures not supported";
    case vr::VRCompositorError_IndexOutOfRange:
      return "Index out of range";
    case vr::VRCompositorError_AlreadySubmitted:
      return "Already submitted";
    case vr::VRCompositorError_InvalidBounds:
      return "Invalid Bounds";
    case vr::VRCompositorError_AlreadySet:
      return "Already set";
  }
  return "UNKNOWN";
}

//! Print OpenVR input error.
static const char* getVRInputError(vr::EVRInputError theVRError)
{
  switch (theVRError)
  {
    case vr::VRInputError_None:
      return "None";
    case vr::VRInputError_NameNotFound:
      return "NameNotFound";
    case vr::VRInputError_WrongType:
      return "WrongType";
    case vr::VRInputError_InvalidHandle:
      return "InvalidHandle";
    case vr::VRInputError_InvalidParam:
      return "InvalidParam";
    case vr::VRInputError_NoSteam:
      return "NoSteam:";
    case vr::VRInputError_MaxCapacityReached:
      return "MaxCapacityReached";
    case vr::VRInputError_IPCError:
      return "IPCError";
    case vr::VRInputError_NoActiveActionSet:
      return "NoActiveActionSet";
    case vr::VRInputError_InvalidDevice:
      return "InvalidDevice";
    case vr::VRInputError_InvalidSkeleton:
      return "InvalidSkeleton";
    case vr::VRInputError_InvalidBoneCount:
      return "InvalidBoneCount";
    case vr::VRInputError_InvalidCompressedData:
      return "InvalidCompressedData";
    case vr::VRInputError_NoData:
      return "NoData";
    case vr::VRInputError_BufferTooSmall:
      return "BufferTooSmall";
    case vr::VRInputError_MismatchedActionManifest:
      return "MismatchedActionManifest";
    case vr::VRInputError_MissingSkeletonData:
      return "MissingSkeletonData";
    case vr::VRInputError_InvalidBoneIndex:
      return "InvalidBoneIndex";
    case vr::VRInputError_InvalidPriority:
      return "InvalidPriority";
    case vr::VRInputError_PermissionDenied:
      return "PermissionDenied";
    case vr::VRInputError_InvalidRenderModel:
      return "InvalidRenderModel";
  }
  return "UNKNOWN";
}

//! Convert OpenVR mat4x4 into OCCT mat4x4.
static NCollection_Mat4<double> mat44vr2Occ(const vr::HmdMatrix44_t& theMat4)
{
  NCollection_Mat4<double> aMat4;
  for (int aRow = 0; aRow < 4; ++aRow)
  {
    aMat4.SetRow(aRow,
                 NCollection_Vec4<double>(theMat4.m[aRow][0],
                                          theMat4.m[aRow][1],
                                          theMat4.m[aRow][2],
                                          theMat4.m[aRow][3]));
  }
  return aMat4;
}

//! Convert OpenVR mat3x4 into OCCT gp_Trsf.
static gp_Trsf mat34vr2OccTrsf(const vr::HmdMatrix34_t& theMat4)
{
  gp_Trsf aTrsf;
  aTrsf.SetValues(theMat4.m[0][0],
                  theMat4.m[0][1],
                  theMat4.m[0][2],
                  theMat4.m[0][3],
                  theMat4.m[1][0],
                  theMat4.m[1][1],
                  theMat4.m[1][2],
                  theMat4.m[1][3],
                  theMat4.m[2][0],
                  theMat4.m[2][1],
                  theMat4.m[2][2],
                  theMat4.m[2][3]);
  return aTrsf;
}

//! Convert OpenVR tracked pose.
static Aspect_TrackedDevicePose poseVr2Occ(const vr::TrackedDevicePose_t& theVrPose,
                                           const Standard_Real            theUnitFactor)
{
  Aspect_TrackedDevicePose aPose;
  aPose.Velocity.SetCoord(theVrPose.vVelocity.v[0],
                          theVrPose.vVelocity.v[1],
                          theVrPose.vVelocity.v[2]);
  aPose.AngularVelocity.SetCoord(theVrPose.vAngularVelocity.v[0],
                                 theVrPose.vAngularVelocity.v[1],
                                 theVrPose.vAngularVelocity.v[2]);
  aPose.IsValidPose       = theVrPose.bPoseIsValid;
  aPose.IsConnectedDevice = theVrPose.bDeviceIsConnected;
  if (aPose.IsValidPose)
  {
    aPose.Orientation = mat34vr2OccTrsf(theVrPose.mDeviceToAbsoluteTracking);
    if (theUnitFactor != 1.0)
    {
      aPose.Orientation.SetTranslationPart(aPose.Orientation.TranslationPart() * theUnitFactor);
    }
  }
  return aPose;
}

//! Find location of default actions manifest file (based on CSF_OCCTResourcePath or CASROOT
//! variables).
TCollection_AsciiString defaultActionsManifestInit()
{
  const TCollection_AsciiString THE_ACTIONS_JSON = "occtvr_actions.json";
  const TCollection_AsciiString aResRoot(OSD_Environment("CSF_OCCTResourcePath").Value());
  if (!aResRoot.IsEmpty())
  {
    if (OSD_File(aResRoot + "/" + THE_ACTIONS_JSON).Exists())
    {
      return aResRoot + "/" + THE_ACTIONS_JSON;
    }
    if (OSD_File(aResRoot + "/XRResources/" + THE_ACTIONS_JSON).Exists())
    {
      return aResRoot + "/XRResources/" + THE_ACTIONS_JSON;
    }
  }
  const TCollection_AsciiString aCasRoot(OSD_Environment("CASROOT").Value());
  if (!aCasRoot.IsEmpty())
  {
    if (OSD_File(aCasRoot + "/" + THE_ACTIONS_JSON).Exists())
    {
      return aCasRoot + "/" + THE_ACTIONS_JSON;
    }
    if (OSD_File(aCasRoot + "/XRResources/" + THE_ACTIONS_JSON).Exists())
    {
      return aCasRoot + "/XRResources/" + THE_ACTIONS_JSON;
    }
    if (OSD_File(aCasRoot + "/XRResources/src/" + THE_ACTIONS_JSON).Exists())
    {
      return aCasRoot + "/XRResources/src/" + THE_ACTIONS_JSON;
    }
  }
  return OSD_Process::ExecutablePath() + "/occtvr_actions.json";
}
} // namespace
#endif

IMPLEMENT_STANDARD_RTTIEXT(Aspect_OpenVRSession, Aspect_XRSession)

struct Aspect_OpenVRSession::VRContext
{
#ifdef HAVE_OPENVR
  // clang-format off
  vr::TrackedDevicePose_t TrackedPoses[vr::k_unMaxTrackedDeviceCount]; //!< array of tracked devices poses
  // clang-format on
  vr::IVRSystem* System; //!< OpenVR session object

  //! Empty constructor.
  VRContext()
      : System(NULL)
  {
    memset(TrackedPoses, 0, sizeof(TrackedPoses));
  }

  //! IVRSystem::PollNextEvent() wrapper.
  bool PollNextEvent(vr::VREvent_t& theEvent)
  {
    return System->PollNextEvent(&theEvent, sizeof(vr::VREvent_t));
  }

  //! IVRSystem::GetControllerState() wrapper.
  bool GetControllerState(vr::VRControllerState_t& theState, vr::TrackedDeviceIndex_t theDevice)
  {
    return System->GetControllerState(theDevice, &theState, sizeof(vr::VRControllerState_t&));
  }

  //! Retrieve string property from OpenVR.
  TCollection_AsciiString getVrTrackedDeviceString(vr::TrackedDeviceIndex_t  theDevice,
                                                   vr::TrackedDeviceProperty theProperty,
                                                   vr::TrackedPropertyError* theError = NULL)
  {
    const uint32_t aBuffLen =
      System->GetStringTrackedDeviceProperty(theDevice, theProperty, NULL, 0, theError);
    if (aBuffLen == 0)
    {
      return TCollection_AsciiString();
    }

    NCollection_LocalArray<char> aBuffer(aBuffLen + 1);
    System->GetStringTrackedDeviceProperty(theDevice, theProperty, aBuffer, aBuffLen, theError);
    aBuffer[aBuffLen] = '\0';
    const TCollection_AsciiString aResult(aBuffer);
    return aResult;
  }
#endif
};

#ifdef HAVE_OPENVR
//! Image wrapping vr::RenderModel_TextureMap_t.
class Aspect_OpenVRSession::VRImagePixmap : public Image_PixMap
{
public:
  //! Empty constructor.
  VRImagePixmap()
      : myVrTexture(NULL)
  {
  }

  //! Load the texture.
  bool Load(vr::TextureID_t theTexture, const TCollection_AsciiString& theVrModelName)
  {
    vr::RenderModel_TextureMap_t* aVrTexture = NULL;
    vr::EVRRenderModelError       aVrError   = vr::VRRenderModelError_Loading;
    for (; aVrError == vr::VRRenderModelError_Loading;)
    {
      aVrError = vr::VRRenderModels()->LoadTexture_Async(theTexture, &aVrTexture);
      OSD::MilliSecSleep(1);
    }
    if (aVrError != vr::VRRenderModelError_None || aVrTexture == NULL)
    {
      Message::SendFail(TCollection_AsciiString("OpenVR, Unable to load render model texture: ")
                        + theVrModelName + " - " + int(aVrError));
      return false;
    }

    InitWrapper(Image_Format_RGBA,
                (Standard_Byte*)aVrTexture->rubTextureMapData,
                aVrTexture->unWidth,
                aVrTexture->unHeight);
    myVrTexture = aVrTexture;
    return true;
  }

  virtual ~VRImagePixmap()
  {
    if (myVrTexture != NULL)
    {
      vr::VRRenderModels()->FreeTexture(myVrTexture);
    }
  }

private:
  vr::RenderModel_TextureMap_t* myVrTexture;
};

//! Image_Texture extension using vr::VRRenderModels().
class Aspect_OpenVRSession::VRTextureSource : public Image_Texture
{
public:
  //! Main constructor.
  VRTextureSource(vr::TextureID_t theTextureId, const TCollection_AsciiString& theVrModelName)
      : Image_Texture(""),
        myVrTextureId(theTextureId),
        myVrModelName(theVrModelName)
  {
    myTextureId = TCollection_AsciiString("texturevr://map_#") + (int)theTextureId;
  }

protected:
  //! Read image.
  virtual Handle(Image_PixMap) ReadImage(const Handle(Image_SupportedFormats)&) const
    Standard_OVERRIDE
  {
    Handle(VRImagePixmap) aPixmap = new VRImagePixmap();
    if (!aPixmap->Load(myVrTextureId, myVrModelName))
    {
      return Handle(VRImagePixmap)();
    }
    return aPixmap;
  }

private:
  vr::TextureID_t         myVrTextureId;
  TCollection_AsciiString myVrModelName;
};
#endif

// =======================================================================
// function : IsHmdPresent
// purpose  :
// =======================================================================
bool Aspect_OpenVRSession::IsHmdPresent()
{
#ifdef HAVE_OPENVR
  return vr::VR_IsHmdPresent();
#else
  return false;
#endif
}

// =======================================================================
// function : defaultActionsManifest
// purpose  :
// =======================================================================
TCollection_AsciiString Aspect_OpenVRSession::defaultActionsManifest()
{
#ifdef HAVE_OPENVR
  static const TCollection_AsciiString THE_ACTIONS_JSON_FULL = defaultActionsManifestInit();
  return THE_ACTIONS_JSON_FULL;
#else
  return TCollection_AsciiString();
#endif
}

// =======================================================================
// function : Aspect_OpenVRSession
// purpose  :
// =======================================================================
Aspect_OpenVRSession::Aspect_OpenVRSession()
    : myContext(new VRContext())
{
#ifdef HAVE_OPENVR
  myActionsManifest = defaultActionsManifest();
  myTrackedPoses.Resize(0, (Standard_Integer)vr::k_unMaxTrackedDeviceCount - 1, false);
  {
    Handle(Aspect_XRActionSet) aHeadActionSet = new Aspect_XRActionSet("/actions/generic_head");
    myActionSets.Add(aHeadActionSet->Id(), aHeadActionSet);

    Handle(Aspect_XRAction) aHeadsetOn =
      new Aspect_XRAction(aHeadActionSet->Id() + "/in/headset_on_head",
                          Aspect_XRActionType_InputDigital);
    aHeadActionSet->AddAction(aHeadsetOn);
    NCollection_Array1<Handle(Aspect_XRAction)>& aGenericSet =
      myRoleActions[Aspect_XRTrackedDeviceRole_Head];
    aGenericSet[Aspect_XRGenericAction_IsHeadsetOn] = aHeadsetOn;
  }
  for (int aHand = 0; aHand < 2; ++aHand)
  {
    NCollection_Array1<Handle(Aspect_XRAction)>& aGenericSet =
      myRoleActions[aHand == 0 ? Aspect_XRTrackedDeviceRole_LeftHand
                               : Aspect_XRTrackedDeviceRole_RightHand];
    Handle(Aspect_XRActionSet) anActionSet =
      new Aspect_XRActionSet(aHand == 0 ? "/actions/generic_left" : "/actions/generic_right");
    myActionSets.Add(anActionSet->Id(), anActionSet);

    Handle(Aspect_XRAction) anAppMenuClick =
      new Aspect_XRAction(anActionSet->Id() + "/in/appmenu_click",
                          Aspect_XRActionType_InputDigital);
    anActionSet->AddAction(anAppMenuClick);
    aGenericSet[Aspect_XRGenericAction_InputAppMenu] = anAppMenuClick;

    Handle(Aspect_XRAction) aSysMenuClick =
      new Aspect_XRAction(anActionSet->Id() + "/in/sysmenu_click",
                          Aspect_XRActionType_InputDigital);
    anActionSet->AddAction(aSysMenuClick);
    aGenericSet[Aspect_XRGenericAction_InputSysMenu] = aSysMenuClick;

    Handle(Aspect_XRAction) aTriggerPull =
      new Aspect_XRAction(anActionSet->Id() + "/in/trigger_pull", Aspect_XRActionType_InputAnalog);
    anActionSet->AddAction(aTriggerPull);
    aGenericSet[Aspect_XRGenericAction_InputTriggerPull] = aTriggerPull;

    Handle(Aspect_XRAction) aTriggerClick =
      new Aspect_XRAction(anActionSet->Id() + "/in/trigger_click",
                          Aspect_XRActionType_InputDigital);
    anActionSet->AddAction(aTriggerClick);
    aGenericSet[Aspect_XRGenericAction_InputTriggerClick] = aTriggerClick;

    Handle(Aspect_XRAction) aGripClick =
      new Aspect_XRAction(anActionSet->Id() + "/in/grip_click", Aspect_XRActionType_InputDigital);
    anActionSet->AddAction(aGripClick);
    aGenericSet[Aspect_XRGenericAction_InputGripClick] = aGripClick;

    Handle(Aspect_XRAction) aPadPos =
      new Aspect_XRAction(anActionSet->Id() + "/in/trackpad_position",
                          Aspect_XRActionType_InputAnalog);
    anActionSet->AddAction(aPadPos);
    aGenericSet[Aspect_XRGenericAction_InputTrackPadPosition] = aPadPos;

    Handle(Aspect_XRAction) aPadTouch =
      new Aspect_XRAction(anActionSet->Id() + "/in/trackpad_touch",
                          Aspect_XRActionType_InputDigital);
    anActionSet->AddAction(aPadTouch);
    aGenericSet[Aspect_XRGenericAction_InputTrackPadTouch] = aPadTouch;

    Handle(Aspect_XRAction) aPadClick =
      new Aspect_XRAction(anActionSet->Id() + "/in/trackpad_click",
                          Aspect_XRActionType_InputDigital);
    anActionSet->AddAction(aPadClick);
    aGenericSet[Aspect_XRGenericAction_InputTrackPadClick] = aPadClick;

    Handle(Aspect_XRAction) aPoseBase =
      new Aspect_XRAction(anActionSet->Id() + "/in/pose_base", Aspect_XRActionType_InputPose);
    anActionSet->AddAction(aPoseBase);
    aGenericSet[Aspect_XRGenericAction_InputPoseBase] = aPoseBase;

    Handle(Aspect_XRAction) aPoseFront =
      new Aspect_XRAction(anActionSet->Id() + "/in/pose_front", Aspect_XRActionType_InputPose);
    anActionSet->AddAction(aPoseFront);
    aGenericSet[Aspect_XRGenericAction_InputPoseFront] = aPoseFront;

    Handle(Aspect_XRAction) aPoseGrip =
      new Aspect_XRAction(anActionSet->Id() + "/in/pose_handgrip", Aspect_XRActionType_InputPose);
    anActionSet->AddAction(aPoseGrip);
    aGenericSet[Aspect_XRGenericAction_InputPoseHandGrip] = aPoseGrip;

    Handle(Aspect_XRAction) aPoseTip =
      new Aspect_XRAction(anActionSet->Id() + "/in/pose_tip", Aspect_XRActionType_InputPose);
    anActionSet->AddAction(aPoseTip);
    aGenericSet[Aspect_XRGenericAction_InputPoseFingerTip] = aPoseTip;

    Handle(Aspect_XRAction) aHaptic =
      new Aspect_XRAction(anActionSet->Id() + "/out/haptic", Aspect_XRActionType_OutputHaptic);
    anActionSet->AddAction(aHaptic);
    aGenericSet[Aspect_XRGenericAction_OutputHaptic] = aHaptic;

    Handle(Aspect_XRAction) aThumbsctickPos =
      new Aspect_XRAction(anActionSet->Id() + "/in/thumbstick_position",
                          Aspect_XRActionType_InputAnalog);
    anActionSet->AddAction(aThumbsctickPos);
    aGenericSet[Aspect_XRGenericAction_InputThumbstickPosition] = aThumbsctickPos;

    Handle(Aspect_XRAction) aThumbsctickTouch =
      new Aspect_XRAction(anActionSet->Id() + "/in/thumbstick_touch",
                          Aspect_XRActionType_InputDigital);
    anActionSet->AddAction(aThumbsctickTouch);
    aGenericSet[Aspect_XRGenericAction_InputThumbstickTouch] = aThumbsctickTouch;

    Handle(Aspect_XRAction) aThumbsctickClick =
      new Aspect_XRAction(anActionSet->Id() + "/in/thumbstick_click",
                          Aspect_XRActionType_InputDigital);
    anActionSet->AddAction(aThumbsctickClick);
    aGenericSet[Aspect_XRGenericAction_InputThumbstickClick] = aThumbsctickClick;
  }
#endif
}

// =======================================================================
// function : ~Aspect_OpenVRSession
// purpose  :
// =======================================================================
Aspect_OpenVRSession::~Aspect_OpenVRSession()
{
  closeVR();
  delete myContext;
}

// =======================================================================
// function : closeVR
// purpose  :
// =======================================================================
void Aspect_OpenVRSession::closeVR()
{
#ifdef HAVE_OPENVR
  if (myContext->System != NULL)
  {
    vr::VR_Shutdown();
    myContext->System = NULL;
  }
#endif
}

// =======================================================================
// function : getVRSystem
// purpose  :
// =======================================================================
void* Aspect_OpenVRSession::getVRSystem() const
{
#ifdef HAVE_OPENVR
  return myContext->System;
#else
  return NULL;
#endif
}

// =======================================================================
// function : Close
// purpose  :
// =======================================================================
void Aspect_OpenVRSession::Close()
{
  closeVR();
}

// =======================================================================
// function : IsOpen
// purpose  :
// =======================================================================
bool Aspect_OpenVRSession::IsOpen() const
{
#ifdef HAVE_OPENVR
  return myContext->System != NULL;
#else
  return false;
#endif
}

// =======================================================================
// function : Open
// purpose  :
// =======================================================================
bool Aspect_OpenVRSession::Open()
{
#ifdef HAVE_OPENVR
  if (myContext->System != NULL)
  {
    return true;
  }

  vr::EVRInitError aVrError = vr::VRInitError_None;
  myContext->System         = vr::VR_Init(&aVrError, vr::VRApplication_Scene);
  if (aVrError != vr::VRInitError_None)
  {
    myContext->System = NULL;
    Message::SendFail(TCollection_AsciiString("OpenVR, Unable to init VR runtime: ")
                      + vr::VR_GetVRInitErrorAsEnglishDescription(aVrError));
    Close();
    return false;
  }

  /*vr::IVRRenderModels* aRenderModels = (vr::IVRRenderModels* )vr::VR_GetGenericInterface
  (vr::IVRRenderModels_Version, &aVrError); if (aRenderModels == NULL)
  {
    Message::SendFail (TCollection_AsciiString ("Unable to get render model interface: ") +
  vr::VR_GetVRInitErrorAsEnglishDescription (aVrError));;
  }*/

  NCollection_Vec2<uint32_t> aRenderSize;
  myContext->System->GetRecommendedRenderTargetSize(&aRenderSize.x(), &aRenderSize.y());
  myRendSize = NCollection_Vec2<int>(aRenderSize);
  myDispFreq = myContext->System->GetFloatTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd,
                                                                vr::Prop_DisplayFrequency_Float);
  if (myRendSize.x() <= 0 || myRendSize.y() <= 0)
  {
    Close();
    return false;
  }
  updateProjectionFrustums();
  initInput();
  return true;
#else
  Message::SendFail("Open CASCADE has been built without OpenVR support");
  return false;
#endif
}

// =======================================================================
// function : initInput
// purpose  :
// =======================================================================
bool Aspect_OpenVRSession::initInput()
{
#ifdef HAVE_OPENVR
  if (myContext->System == NULL)
  {
    return false;
  }

  if (!OSD_File(myActionsManifest).Exists())
  {
    Message::SendFail(TCollection_AsciiString("OpenVR, Unable to open actions manifest '")
                      + myActionsManifest + "'");
    return false;
  }

  vr::EVRInputError aVrError = vr::VRInput()->SetActionManifestPath(myActionsManifest.ToCString());
  if (aVrError != vr::VRInputError_None)
  {
    Message::SendFail(TCollection_AsciiString("OpenVR, Unable to load actions manifest '")
                      + myActionsManifest + "': " + getVRInputError(aVrError));
    return false;
  }

  bool hasErrors = false;
  for (Aspect_XRActionSetMap::Iterator aSetIter(myActionSets); aSetIter.More(); aSetIter.Next())
  {
    const Handle(Aspect_XRActionSet)& anActionSet = aSetIter.Value();
    for (Aspect_XRActionMap::Iterator anActionIter(anActionSet->Actions()); anActionIter.More();
         anActionIter.Next())
    {
      const Handle(Aspect_XRAction)& anAction       = anActionIter.Value();
      vr::VRActionHandle_t           anActionHandle = 0;
      aVrError = vr::VRInput()->GetActionHandle(anAction->Id().ToCString(), &anActionHandle);
      if (aVrError == vr::VRInputError_None)
      {
        anAction->SetRawHandle(anActionHandle);
      }
      else
      {
        hasErrors = true;
        Message::SendFail(TCollection_AsciiString("OpenVR, Unable to load action '")
                          + anAction->Id() + "' from '" + myActionsManifest
                          + "': " + getVRInputError(aVrError));
      }
    }

    vr::VRActionSetHandle_t anActionSetHandle = 0;
    aVrError = vr::VRInput()->GetActionSetHandle(anActionSet->Id().ToCString(), &anActionSetHandle);
    if (aVrError == vr::VRInputError_None)
    {
      anActionSet->SetRawHandle(anActionSetHandle);
    }
    else
    {
      hasErrors = true;
      Message::SendFail(TCollection_AsciiString("OpenVR, Unable to load action set '")
                        + anActionSet->Id() + "' from '" + myActionsManifest
                        + "': " + getVRInputError(aVrError));
    }
  }
  return !hasErrors;
#else
  return false;
#endif
}

// =======================================================================
// function : GetString
// purpose  :
// =======================================================================
TCollection_AsciiString Aspect_OpenVRSession::GetString(InfoString theInfo) const
{
#ifdef HAVE_OPENVR
  if (myContext->System != NULL)
  {
    vr::ETrackedDeviceProperty aVrProp = vr::Prop_Invalid;
    switch (theInfo)
    {
      case InfoString_Vendor:
        aVrProp = vr::Prop_ManufacturerName_String;
        break;
      case InfoString_Device:
        aVrProp = vr::Prop_ModelNumber_String;
        break;
      case InfoString_Tracker:
        aVrProp = vr::Prop_TrackingSystemName_String;
        break;
      case InfoString_SerialNumber:
        aVrProp = vr::Prop_SerialNumber_String;
        break;
    }
    if (aVrProp != vr::Prop_Invalid)
    {
      return myContext->getVrTrackedDeviceString(vr::k_unTrackedDeviceIndex_Hmd, aVrProp);
    }
  }
#else
  (void)theInfo;
#endif
  return TCollection_AsciiString();
}

// =======================================================================
// function : NamedTrackedDevice
// purpose  :
// =======================================================================
Standard_Integer Aspect_OpenVRSession::NamedTrackedDevice(
  Aspect_XRTrackedDeviceRole theDevice) const
{
#ifdef HAVE_OPENVR
  if (myContext->System != NULL)
  {
    vr::TrackedDeviceIndex_t aDevIndex = vr::k_unTrackedDeviceIndexInvalid;
    switch (theDevice)
    {
      case Aspect_XRTrackedDeviceRole_Head:
        aDevIndex = vr::k_unTrackedDeviceIndex_Hmd;
        break;
      case Aspect_XRTrackedDeviceRole_LeftHand:
        aDevIndex = myContext->System->GetTrackedDeviceIndexForControllerRole(
          vr::TrackedControllerRole_LeftHand);
        break;
      case Aspect_XRTrackedDeviceRole_RightHand:
        aDevIndex = myContext->System->GetTrackedDeviceIndexForControllerRole(
          vr::TrackedControllerRole_RightHand);
        break;
      case Aspect_XRTrackedDeviceRole_Other:
        break;
    }
    if (aDevIndex == vr::k_unTrackedDeviceIndexInvalid)
    {
      return -1;
    }
    return (Standard_Integer)aDevIndex;
  }
#else
  (void)theDevice;
#endif
  return -1;
}

// =======================================================================
// function : loadRenderModel
// purpose  :
// =======================================================================
Handle(Graphic3d_ArrayOfTriangles) Aspect_OpenVRSession::loadRenderModel(
  Standard_Integer       theDevice,
  Standard_Boolean       theToApplyUnitFactor,
  Handle(Image_Texture)& theTexture)
{
  if (theDevice < 0)
  {
    return Handle(Graphic3d_ArrayOfTriangles)();
  }
#ifdef HAVE_OPENVR
  if (myContext->System == NULL)
  {
    return Handle(Graphic3d_ArrayOfTriangles)();
  }

  const TCollection_AsciiString aRenderModelName =
    myContext->getVrTrackedDeviceString(theDevice, vr::Prop_RenderModelName_String);
  if (aRenderModelName.IsEmpty())
  {
    return Handle(Graphic3d_ArrayOfTriangles)();
  }

  vr::RenderModel_t*      aVrModel = NULL;
  vr::EVRRenderModelError aVrError = vr::VRRenderModelError_Loading;
  for (; aVrError == vr::VRRenderModelError_Loading;)
  {
    aVrError = vr::VRRenderModels()->LoadRenderModel_Async(aRenderModelName.ToCString(), &aVrModel);
    OSD::MilliSecSleep(1);
  }
  if (aVrError != vr::VRRenderModelError_None)
  {
    Message::SendFail(TCollection_AsciiString("OpenVR, Unable to load render model: ")
                      + aRenderModelName + " - " + int(aVrError));
    return Handle(Graphic3d_ArrayOfTriangles)();
  }

  if (aVrModel->diffuseTextureId >= 0)
  {
    theTexture = new VRTextureSource(aVrModel->diffuseTextureId, aRenderModelName);
  }

  const float                        aScale = theToApplyUnitFactor ? float(myUnitFactor) : 1.0f;
  Handle(Graphic3d_ArrayOfTriangles) aTris  = new Graphic3d_ArrayOfTriangles(
    (int)aVrModel->unVertexCount,
    (int)aVrModel->unTriangleCount * 3,
    Graphic3d_ArrayFlags_VertexNormal | Graphic3d_ArrayFlags_VertexTexel);
  for (uint32_t aVertIter = 0; aVertIter < aVrModel->unVertexCount; ++aVertIter)
  {
    const vr::RenderModel_Vertex_t& aVert = aVrModel->rVertexData[aVertIter];
    aTris->AddVertex(aVert.vPosition.v[0] * aScale,
                     aVert.vPosition.v[1] * aScale,
                     aVert.vPosition.v[2] * aScale,
                     aVert.vNormal.v[0],
                     aVert.vNormal.v[1],
                     aVert.vNormal.v[2],
                     aVert.rfTextureCoord[0],
                     aVert.rfTextureCoord[1]);
  }
  for (uint32_t aTriIter = 0; aTriIter < aVrModel->unTriangleCount; ++aTriIter)
  {
    aTris->AddEdges(aVrModel->rIndexData[aTriIter * 3 + 0] + 1,
                    aVrModel->rIndexData[aTriIter * 3 + 1] + 1,
                    aVrModel->rIndexData[aTriIter * 3 + 2] + 1);
  }

  vr::VRRenderModels()->FreeRenderModel(aVrModel);
  return aTris;
#else
  (void)theToApplyUnitFactor;
  (void)theTexture;
  return Handle(Graphic3d_ArrayOfTriangles)();
#endif
}

// =======================================================================
// function : EyeToHeadTransform
// purpose  :
// =======================================================================
NCollection_Mat4<double> Aspect_OpenVRSession::EyeToHeadTransform(Aspect_Eye theEye) const
{
#ifdef HAVE_OPENVR
  if (myContext->System != NULL)
  {
    const vr::HmdMatrix34_t aMatVr = myContext->System->GetEyeToHeadTransform(
      theEye == Aspect_Eye_Right ? vr::Eye_Right : vr::Eye_Left);
    gp_Trsf aTrsf = mat34vr2OccTrsf(aMatVr);
    if (myUnitFactor != 1.0)
    {
      aTrsf.SetTranslationPart(aTrsf.TranslationPart() * myUnitFactor);
    }
    NCollection_Mat4<double> aMat4;
    aTrsf.GetMat4(aMat4);
    return aMat4;
  }
#else
  (void)theEye;
#endif
  return NCollection_Mat4<double>();
}

// =======================================================================
// function : ProjectionMatrix
// purpose  :
// =======================================================================
NCollection_Mat4<double> Aspect_OpenVRSession::ProjectionMatrix(Aspect_Eye theEye,
                                                                double     theZNear,
                                                                double     theZFar) const
{
#ifdef HAVE_OPENVR
  if (myContext->System != NULL)
  {
    const vr::HmdMatrix44_t aMat4 = myContext->System->GetProjectionMatrix(
      theEye == Aspect_Eye_Right ? vr::Eye_Right : vr::Eye_Left,
      (float)theZNear,
      (float)theZFar);
    return mat44vr2Occ(aMat4);
  }
#else
  (void)theEye;
  (void)theZNear;
  (void)theZFar;
#endif
  return NCollection_Mat4<double>();
}

// =======================================================================
// function : updateProjectionFrustums
// purpose  :
// =======================================================================
void Aspect_OpenVRSession::updateProjectionFrustums()
{
#ifdef HAVE_OPENVR
  Aspect_FrustumLRBT<float> aFrustL, aFrustR;
  myContext->System->GetProjectionRaw(vr::Eye_Left,
                                      &aFrustL.Left,
                                      &aFrustL.Right,
                                      &aFrustL.Top,
                                      &aFrustL.Bottom);
  myContext->System->GetProjectionRaw(vr::Eye_Right,
                                      &aFrustR.Left,
                                      &aFrustR.Right,
                                      &aFrustR.Top,
                                      &aFrustR.Bottom);
  myFrustumL = Aspect_FrustumLRBT<double>(aFrustL);
  myFrustumR = Aspect_FrustumLRBT<double>(aFrustR);
  std::swap(myFrustumL.Top, myFrustumL.Bottom);
  std::swap(myFrustumR.Top, myFrustumR.Bottom);

  const NCollection_Vec2<double> aTanHalfFov(
    NCollection_Vec4<float>(-aFrustL.Left, aFrustL.Right, -aFrustR.Left, aFrustR.Right).maxComp(),
    NCollection_Vec4<float>(-aFrustL.Top, aFrustL.Bottom, -aFrustR.Top, aFrustR.Bottom).maxComp());
  myAspect      = aTanHalfFov.x() / aTanHalfFov.y();
  myFieldOfView = 2.0 * ATan(aTanHalfFov.y()) * 180.0 / M_PI;

  // Intra-ocular Distance can be changed in runtime
  // const vr::HmdMatrix34_t aLeftToHead  = myContext->System->GetEyeToHeadTransform (vr::Eye_Left);
  // const vr::HmdMatrix34_t aRightToHead = myContext->System->GetEyeToHeadTransform
  // (vr::Eye_Right); myIod = aRightToHead.m[0][3] - aLeftToHead.m[0][3];
  myIod = myContext->System->GetFloatTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd,
                                                           vr::Prop_UserIpdMeters_Float);
  myIod *= myUnitFactor;
#endif
}

// =======================================================================
// function : SetTrackingOrigin
// purpose  :
// =======================================================================
void Aspect_OpenVRSession::SetTrackingOrigin(TrackingUniverseOrigin theOrigin)
{
#ifdef HAVE_OPENVR
  if (myContext->System != NULL)
  {
    vr::ETrackingUniverseOrigin anOrigin = vr::TrackingUniverseStanding;
    switch (theOrigin)
    {
      case TrackingUniverseOrigin_Seated:
        anOrigin = vr::TrackingUniverseSeated;
        break;
      case TrackingUniverseOrigin_Standing:
        anOrigin = vr::TrackingUniverseStanding;
        break;
    }
    vr::VRCompositor()->SetTrackingSpace(anOrigin);
  }
#endif
  myTrackOrigin = theOrigin;
}

// =======================================================================
// function : WaitPoses
// purpose  :
// =======================================================================
bool Aspect_OpenVRSession::WaitPoses()
{
#ifdef HAVE_OPENVR
  if (myContext->System == NULL)
  {
    return false;
  }

  switch (vr::VRCompositor()->GetTrackingSpace())
  {
    case vr::TrackingUniverseSeated:
      myTrackOrigin = TrackingUniverseOrigin_Seated;
      break;
    case vr::TrackingUniverseRawAndUncalibrated:
    case vr::TrackingUniverseStanding:
      myTrackOrigin = TrackingUniverseOrigin_Standing;
      break;
  }

  const vr::EVRCompositorError aVRError =
    vr::VRCompositor()->WaitGetPoses(myContext->TrackedPoses,
                                     vr::k_unMaxTrackedDeviceCount,
                                     NULL,
                                     0);
  if (aVRError != vr::VRCompositorError_None)
  {
    Message::SendTrace(TCollection_AsciiString("Compositor wait poses error: ")
                       + getVRCompositorError(aVRError));
  }

  for (Standard_Integer aPoseIter = myTrackedPoses.Lower(); aPoseIter <= myTrackedPoses.Upper();
       ++aPoseIter)
  {
    const vr::TrackedDevicePose_t& aVrPose = myContext->TrackedPoses[aPoseIter];
    myTrackedPoses[aPoseIter]              = poseVr2Occ(aVrPose, myUnitFactor);
  }

  if (myContext->TrackedPoses[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid)
  {
    myHeadPose = myTrackedPoses[vr::k_unTrackedDeviceIndex_Hmd].Orientation;
    updateProjectionFrustums();
  }
  return aVRError != vr::VRCompositorError_None;
#else
  return false;
#endif
}

// =======================================================================
// function : GetDigitalActionData
// purpose  :
// =======================================================================
Aspect_XRDigitalActionData Aspect_OpenVRSession::GetDigitalActionData(
  const Handle(Aspect_XRAction)& theAction) const
{
  if (theAction.IsNull() || theAction->Type() != Aspect_XRActionType_InputDigital)
  {
    throw Standard_ProgramError(
      "Aspect_OpenVRSession::GetDigitalActionData() called for wrong action");
  }

  Aspect_XRDigitalActionData anActionData;
#ifdef HAVE_OPENVR
  if (myContext->System != NULL && theAction->RawHandle() != 0)
  {
    vr::InputDigitalActionData_t aNewData = {};
    vr::EVRInputError            anInErr =
      vr::VRInput()->GetDigitalActionData(theAction->RawHandle(),
                                          &aNewData,
                                          sizeof(aNewData),
                                          vr::k_ulInvalidInputValueHandle);
    if (anInErr != vr::VRInputError_None)
    {
      Message::SendFail(TCollection_AsciiString("Input Error on '") + theAction->Id()
                        + "': " + getVRInputError(anInErr));
      return anActionData;
    }

    anActionData.IsActive     = aNewData.bActive;
    anActionData.IsChanged    = aNewData.bChanged;
    anActionData.IsPressed    = aNewData.bState;
    anActionData.UpdateTime   = aNewData.fUpdateTime;
    anActionData.ActiveOrigin = aNewData.activeOrigin;
  }
#endif
  return anActionData;
}

// =======================================================================
// function : GetAnalogActionData
// purpose  :
// =======================================================================
Aspect_XRAnalogActionData Aspect_OpenVRSession::GetAnalogActionData(
  const Handle(Aspect_XRAction)& theAction) const
{
  if (theAction.IsNull() || theAction->Type() != Aspect_XRActionType_InputAnalog)
  {
    throw Standard_ProgramError(
      "Aspect_OpenVRSession::GetAnalogActionData() called for wrong action");
  }

  Aspect_XRAnalogActionData anActionData;
#ifdef HAVE_OPENVR
  if (myContext->System != NULL && theAction->RawHandle() != 0)
  {
    vr::InputAnalogActionData_t aNewData = {};
    vr::EVRInputError           anInErr = vr::VRInput()->GetAnalogActionData(theAction->RawHandle(),
                                                                   &aNewData,
                                                                   sizeof(aNewData),
                                                                   vr::k_ulInvalidInputValueHandle);
    if (anInErr != vr::VRInputError_None)
    {
      Message::SendFail(TCollection_AsciiString("Input Error on '") + theAction->Id()
                        + "': " + getVRInputError(anInErr));
      return anActionData;
    }

    anActionData.IsActive = aNewData.bActive;
    anActionData.VecXYZ.SetValues(aNewData.x, aNewData.y, aNewData.z);
    anActionData.DeltaXYZ.SetValues(aNewData.deltaX, aNewData.deltaY, aNewData.deltaZ);
    anActionData.UpdateTime   = aNewData.fUpdateTime;
    anActionData.ActiveOrigin = aNewData.activeOrigin;
  }
#endif
  return anActionData;
}

// =======================================================================
// function : GetPoseActionDataForNextFrame
// purpose  :
// =======================================================================
Aspect_XRPoseActionData Aspect_OpenVRSession::GetPoseActionDataForNextFrame(
  const Handle(Aspect_XRAction)& theAction) const
{
  if (theAction.IsNull() || theAction->Type() != Aspect_XRActionType_InputPose)
  {
    throw Standard_ProgramError(
      "Aspect_OpenVRSession::GetPoseActionDataForNextFrame() called for wrong action");
  }

  Aspect_XRPoseActionData anActionData;
#ifdef HAVE_OPENVR
  if (myContext->System != NULL && theAction->RawHandle() != 0)
  {
    vr::ETrackingUniverseOrigin anOrigin = vr::TrackingUniverseSeated;
    switch (myTrackOrigin)
    {
      case TrackingUniverseOrigin_Seated:
        anOrigin = vr::TrackingUniverseSeated;
        break;
      case TrackingUniverseOrigin_Standing:
        anOrigin = vr::TrackingUniverseStanding;
        break;
    }
    vr::InputPoseActionData_t aNewData = {};
    vr::EVRInputError         anInErr =
      vr::VRInput()->GetPoseActionDataForNextFrame(theAction->RawHandle(),
                                                   anOrigin,
                                                   &aNewData,
                                                   sizeof(aNewData),
                                                   vr::k_ulInvalidInputValueHandle);
    if (anInErr != vr::VRInputError_None)
    {
      Message::SendFail(TCollection_AsciiString("Input Error on '") + theAction->Id()
                        + "': " + getVRInputError(anInErr));
      return anActionData;
    }

    anActionData.Pose         = poseVr2Occ(aNewData.pose, myUnitFactor);
    anActionData.IsActive     = aNewData.bActive;
    anActionData.ActiveOrigin = aNewData.activeOrigin;
  }
#endif
  return anActionData;
}

// =======================================================================
// function : triggerHapticVibrationAction
// purpose  :
// =======================================================================
void Aspect_OpenVRSession::triggerHapticVibrationAction(const Handle(Aspect_XRAction)&   theAction,
                                                        const Aspect_XRHapticActionData& theParams)
{
  if (theAction.IsNull() || theAction->Type() != Aspect_XRActionType_OutputHaptic)
  {
    throw Standard_ProgramError(
      "Aspect_OpenVRSession::triggerHapticVibrationAction() called for wrong action");
  }

#ifdef HAVE_OPENVR
  if (myContext->System != NULL && theAction->RawHandle() != 0)
  {
    Aspect_XRHapticActionData aParams = theParams;
    if (!theParams.IsValid())
    {
      // preset for aborting
      aParams.Duration  = 0.0f;
      aParams.Frequency = 1.0f;
      aParams.Amplitude = 0.1f;
    }
    vr::EVRInputError anInErr =
      vr::VRInput()->TriggerHapticVibrationAction(theAction->RawHandle(),
                                                  aParams.Delay,
                                                  aParams.Duration,
                                                  aParams.Frequency,
                                                  aParams.Amplitude,
                                                  vr::k_ulInvalidInputValueHandle);
    if (anInErr != vr::VRInputError_None)
    {
      Message::SendFail(TCollection_AsciiString("Output Error on '") + theAction->Id()
                        + "': " + getVRInputError(anInErr));
    }
  }
#else
  (void)theParams;
#endif
}

// =======================================================================
// function : ProcessEvents
// purpose  :
// =======================================================================
void Aspect_OpenVRSession::ProcessEvents()
{
#ifdef HAVE_OPENVR
  if (myContext->System == NULL)
  {
    return;
  }

  // process OpenVR events
  vr::VREvent_t aVREvent = {};
  for (; myContext->PollNextEvent(aVREvent);)
  {
    switch (aVREvent.eventType)
    {
      case vr::VREvent_TrackedDeviceActivated:
        onTrackedDeviceActivated((int)aVREvent.trackedDeviceIndex);
        break;
      case vr::VREvent_TrackedDeviceDeactivated:
        onTrackedDeviceDeactivated((int)aVREvent.trackedDeviceIndex);
        break;
      case vr::VREvent_TrackedDeviceUpdated:
        onTrackedDeviceUpdated((int)aVREvent.trackedDeviceIndex);
        break;
    }
  }

  // process OpenVR action state
  if (myActionSets.Extent() > 0)
  {
    NCollection_LocalArray<vr::VRActiveActionSet_t, 8> anActionSets(myActionSets.Extent());
    memset(anActionSets, 0, sizeof(vr::VRActiveActionSet_t) * myActionSets.Extent());
    for (Standard_Integer aSetIter = 0; aSetIter < myActionSets.Extent(); ++aSetIter)
    {
      anActionSets[aSetIter].ulActionSet = myActionSets.FindFromIndex(aSetIter + 1)->RawHandle();
    }
    vr::VRInput()->UpdateActionState(anActionSets,
                                     sizeof(vr::VRActiveActionSet_t),
                                     myActionSets.Extent());
  }

  WaitPoses();

  for (Aspect_XRActionSetMap::Iterator aSetIter(myActionSets); aSetIter.More(); aSetIter.Next())
  {
    const Handle(Aspect_XRActionSet)& anActionSet = aSetIter.Value();
    for (Aspect_XRActionMap::Iterator anActionIter(anActionSet->Actions()); anActionIter.More();
         anActionIter.Next())
    {
      const Handle(Aspect_XRAction)& anAction = anActionIter.Value();
      if (anAction->RawHandle() == 0 || anAction->Id().IsEmpty())
      {
        continue;
      }

      switch (anAction->Type())
      {
        case Aspect_XRActionType_InputDigital: {
          GetDigitalActionData(anAction);
          break;
        }
        case Aspect_XRActionType_InputAnalog: {
          GetAnalogActionData(anAction);
          break;
        }
        case Aspect_XRActionType_InputPose: {
          GetPoseActionDataForNextFrame(anAction);
          break;
        }
        default:
          break;
      }
    }
  }

  // process OpenVR controller state using deprecated API
  // for (vr::TrackedDeviceIndex_t aDevIter = 0; aDevIter < vr::k_unMaxTrackedDeviceCount;
  // ++aDevIter) {
  //  vr::VRControllerState_t aCtrlState = {}; if (myContext->GetControllerState (aCtrlState,
  //  aDevIter)) { aCtrlState.ulButtonPressed == 0; }
  //}
#endif
}

// =======================================================================
// function : onTrackedDeviceActivated
// purpose  :
// =======================================================================
void Aspect_OpenVRSession::onTrackedDeviceActivated(Standard_Integer theDeviceIndex)
{
  Message::SendTrace(TCollection_AsciiString("OpenVR, Device ") + theDeviceIndex + " attached");
}

// =======================================================================
// function : onTrackedDeviceDeactivated
// purpose  :
// =======================================================================
void Aspect_OpenVRSession::onTrackedDeviceDeactivated(Standard_Integer theDeviceIndex)
{
  Message::SendTrace(TCollection_AsciiString("OpenVR, Device ") + theDeviceIndex + " detached");
}

// =======================================================================
// function : onTrackedDeviceUpdated
// purpose  :
// =======================================================================
void Aspect_OpenVRSession::onTrackedDeviceUpdated(Standard_Integer theDeviceIndex)
{
  Message::SendTrace(TCollection_AsciiString("OpenVR, Device ") + theDeviceIndex + " updated");
}

// =======================================================================
// function : SubmitEye
// purpose  :
// =======================================================================
bool Aspect_OpenVRSession::SubmitEye(void*                  theTexture,
                                     Aspect_GraphicsLibrary theGraphicsLib,
                                     Aspect_ColorSpace      theColorSpace,
                                     Aspect_Eye             theEye)
{
  if (theTexture == NULL)
  {
    return false;
  }
#ifdef HAVE_OPENVR
  if (myContext->System == NULL)
  {
    return false;
  }

  vr::Texture_t aVRTexture = {(void*)theTexture, vr::TextureType_OpenGL, vr::ColorSpace_Gamma};
  switch (theGraphicsLib)
  {
    case Aspect_GraphicsLibrary_OpenGL:
      aVRTexture.eType = vr::TextureType_OpenGL;
      break;
    default:
      Message::SendFail("Compositor error: unsupported Graphics API");
      return false;
  }
  switch (theColorSpace)
  {
    case Aspect_ColorSpace_sRGB:
      aVRTexture.eColorSpace = vr::ColorSpace_Gamma;
      break;
    case Aspect_ColorSpace_Linear:
      aVRTexture.eColorSpace = vr::ColorSpace_Linear;
      break;
  }

  const vr::EVRCompositorError aVRError =
    vr::VRCompositor()->Submit(theEye == Aspect_Eye_Right ? vr::Eye_Right : vr::Eye_Left,
                               &aVRTexture);
  if (aVRError != vr::VRCompositorError_None)
  {
    if (aVRError != vr::VRCompositorError_AlreadySubmitted)
    {
      Message::SendFail(TCollection_AsciiString("Compositor Error: ")
                        + getVRCompositorError(aVRError));
    }
    else
    {
      Message::SendTrace(TCollection_AsciiString("Compositor Error: ")
                         + getVRCompositorError(aVRError));
    }
    return false;
  }
  return true;
#else
  (void)theGraphicsLib;
  (void)theColorSpace;
  (void)theEye;
  return false;
#endif
}
