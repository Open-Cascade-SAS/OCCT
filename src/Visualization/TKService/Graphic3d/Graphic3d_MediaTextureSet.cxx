// Created by: Kirill GAVRILOV
// Copyright (c) 2019 OPEN CASCADE SAS
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

// activate some C99 macros like UINT64_C in "stdint.h" which used by FFmpeg
#ifndef __STDC_CONSTANT_MACROS
  #define __STDC_CONSTANT_MACROS
#endif

#include <Graphic3d_MediaTextureSet.hxx>

#include <Media_Frame.hxx>
#include <Media_PlayerContext.hxx>
#include <Graphic3d_ShaderProgram.hxx>

#ifdef HAVE_FFMPEG
  #include <Standard_WarningsDisable.hxx>
extern "C"
{
  #include <libavcodec/avcodec.h>
  #include <libavutil/imgutils.h>
};
  #include <Standard_WarningsRestore.hxx>
#endif

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_MediaTextureSet, Graphic3d_TextureSet)

//=================================================================================================

Graphic3d_MediaTextureSet::Graphic3d_MediaTextureSet()
    : Graphic3d_TextureSet(4),
      myMutex(),
      myCallbackFunction(NULL),
      myCallbackUserPtr(NULL),
      myProgress(0.0),
      myDuration(0.0),
      myFront(0),
      myToPresentFrame(false),
      myIsPlanarYUV(false),
      myIsFullRangeYUV(true)
{
  myFramePair[0] = new Media_Frame();
  myFramePair[1] = new Media_Frame();

  for (int aPlaneIter = 0; aPlaneIter < Size(); ++aPlaneIter)
  {
    occ::handle<Graphic3d_MediaTexture> aTexture = new Graphic3d_MediaTexture(myMutex, aPlaneIter);
    SetValue(Lower() + aPlaneIter, aTexture);
  }

#define EOL "\n"
  TCollection_AsciiString aSrcVert = EOL
    "out vec2 TexCoord;" EOL "void main()" EOL "{" EOL "  TexCoord = occTexCoord.st;" EOL
    " gl_Position = occProjectionMatrix * occWorldViewMatrix * occModelWorldMatrix * occVertex;" EOL
    "}";

  TCollection_AsciiString F_SHADER_YUV2RGB_MPEG =
    EOL "const float TheRangeBits = 1.0;" EOL "vec3 convertToRGB (in vec3 theYUV)" EOL "{" EOL
        "  vec3 aYUV = theYUV.rgb;" EOL "  aYUV   *= TheRangeBits;" EOL
        "  aYUV.x  = 1.1643 * (aYUV.x - 0.0625);" EOL "  aYUV.y -= 0.5;" EOL "  aYUV.z -= 0.5;" EOL
        "  vec3 aColor = vec3(0.0);" EOL "  aColor.r = aYUV.x +  1.5958 * aYUV.z;" EOL
        "  aColor.g = aYUV.x - 0.39173 * aYUV.y - 0.81290 * aYUV.z;" EOL
        "  aColor.b = aYUV.x +   2.017 * aYUV.y;" EOL "  return aColor;" EOL "}";

  TCollection_AsciiString F_SHADER_YUV2RGB_FULL =
    EOL "const float TheRangeBits = 1.0;" EOL "vec3 convertToRGB (in vec3 theYUV)" EOL "{" EOL
        "  vec3 aYUV = theYUV.rgb;" EOL "  aYUV   *= TheRangeBits;" EOL "  aYUV.x  = aYUV.x;" EOL
        "  aYUV.y -= 0.5;" EOL "  aYUV.z -= 0.5;" EOL "  vec3 aColor = vec3(0.0);" EOL
        "  aColor.r = aYUV.x + 1.402 * aYUV.z;" EOL
        "  aColor.g = aYUV.x - 0.344 * aYUV.y - 0.714 * aYUV.z;" EOL
        "  aColor.b = aYUV.x + 1.772 * aYUV.y;" EOL "  return aColor;" EOL "}";

  TCollection_AsciiString aSrcFrag = EOL
    "in vec2 TexCoord;" EOL "uniform sampler2D occSampler1;" EOL
    "uniform sampler2D occSampler2;" EOL "vec3 convertToRGB (in vec3 theYUV);" EOL "void main()" EOL
    "{" EOL "  vec3 aYUV = vec3 (occTexture2D (occSampler0, TexCoord.st).r," EOL
    "                    occTexture2D (occSampler1, TexCoord.st).r," EOL
    "                    occTexture2D (occSampler2, TexCoord.st).r);" EOL
    " occSetFragColor (vec4 (convertToRGB (aYUV), 1.0));" EOL "}";

  myShaderYUV = new Graphic3d_ShaderProgram();
  myShaderYUV->SetHeader("#version 150");
  myShaderYUV->SetNbLightsMax(0);
  myShaderYUV->SetNbClipPlanesMax(0);
  myShaderYUV->AttachShader(
    Graphic3d_ShaderObject::CreateFromSource(Graphic3d_TOS_VERTEX, aSrcVert));
  myShaderYUV->AttachShader(
    Graphic3d_ShaderObject::CreateFromSource(Graphic3d_TOS_FRAGMENT,
                                             aSrcFrag + F_SHADER_YUV2RGB_MPEG));

  myShaderYUVJ = new Graphic3d_ShaderProgram();
  myShaderYUVJ->SetHeader("#version 150");
  myShaderYUVJ->SetNbLightsMax(0);
  myShaderYUVJ->SetNbClipPlanesMax(0);
  myShaderYUVJ->AttachShader(
    Graphic3d_ShaderObject::CreateFromSource(Graphic3d_TOS_VERTEX, aSrcVert));
  myShaderYUVJ->AttachShader(
    Graphic3d_ShaderObject::CreateFromSource(Graphic3d_TOS_FRAGMENT,
                                             aSrcFrag + F_SHADER_YUV2RGB_FULL));
}

//=================================================================================================

void Graphic3d_MediaTextureSet::SetCallback(CallbackOnUpdate_t theCallbackFunction,
                                            void*              theCallbackUserPtr)
{
  myCallbackFunction = theCallbackFunction;
  myCallbackUserPtr  = theCallbackUserPtr;
}

//=================================================================================================

void Graphic3d_MediaTextureSet::Notify()
{
  if (myCallbackFunction != NULL)
  {
    myCallbackFunction(myCallbackUserPtr);
  }
}

//=================================================================================================

void Graphic3d_MediaTextureSet::OpenInput(const TCollection_AsciiString& thePath, bool theToWait)
{
  if (myPlayerCtx.IsNull())
  {
    if (thePath.IsEmpty())
    {
      myInput.Clear();
      return;
    }

    myPlayerCtx = new Media_PlayerContext(this);
  }

  myProgress = 0.0;
  myDuration = 0.0;

  myPlayerCtx->SetInput(thePath, theToWait);
  myInput = thePath;
}

//=================================================================================================

occ::handle<Media_Frame> Graphic3d_MediaTextureSet::LockFrame()
{
  {
    std::lock_guard<std::mutex> aLock(myMutex);
    if (!myToPresentFrame)
    {
      occ::handle<Media_Frame> aFrame = myFramePair[myFront == 0 ? 1 : 0];
      if (aFrame->IsLocked())
      {
        return occ::handle<Media_Frame>();
      }

      aFrame->SetLocked(true);
      return aFrame;
    }
  }

  Notify();
  return occ::handle<Media_Frame>();
}

//=================================================================================================

void Graphic3d_MediaTextureSet::ReleaseFrame(const occ::handle<Media_Frame>& theFrame)
{
  {
    std::lock_guard<std::mutex> aLock(myMutex);

    theFrame->SetLocked(false);
    myToPresentFrame = true;
  }

  if (myCallbackFunction != NULL)
  {
    myCallbackFunction(myCallbackUserPtr);
  }
}

//=================================================================================================

bool Graphic3d_MediaTextureSet::SwapFrames()
{
  if (myPlayerCtx.IsNull())
  {
    return false;
  }
  bool isPaused = false;
  myPlayerCtx->PlaybackState(isPaused, myProgress, myDuration);

  std::lock_guard<std::mutex> aLock(myMutex);
  if (!myToPresentFrame)
  {
    return false;
  }

  myToPresentFrame                       = false;
  myFront                                = myFront == 0 ? 1 : 0;
  const occ::handle<Media_Frame>& aFront = myFramePair[myFront];
  myFrameSize                            = aFront->Size();
#ifdef HAVE_FFMPEG
  myIsPlanarYUV = aFront->Format() == AV_PIX_FMT_YUV420P || aFront->Format() == AV_PIX_FMT_YUVJ420P;
#endif
  myIsFullRangeYUV = aFront->IsFullRangeYUV();
  for (int aPlaneIter = Lower(); aPlaneIter <= Upper(); ++aPlaneIter)
  {
    if (Graphic3d_MediaTexture* aTexture =
          dynamic_cast<Graphic3d_MediaTexture*>(Value(aPlaneIter).get()))
    {
      aTexture->SetFrame(aFront);
      aTexture->UpdateRevision();
    }
  }
  return true;
}
