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

#include <OpenGl_AspectsSprite.hxx>

#include <OpenGl_Context.hxx>
#include <OpenGl_PointSprite.hxx>
#include <OpenGl_TextureSet.hxx>

#include <Image_PixMap.hxx>
#include <Graphic3d_MarkerImage.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

//=================================================================================================

void OpenGl_AspectsSprite::Release(OpenGl_Context* theCtx)
{
  myIsSpriteReady = false;
  if (mySprite.IsNull())
  {
    return;
  }

  if (theCtx != NULL)
  {
    if (mySprite->ResourceId().IsEmpty())
    {
      theCtx->DelayedRelease(mySprite);
      theCtx->DelayedRelease(mySpriteA);
    }
    else
    {
      {
        const TCollection_AsciiString aSpriteKey = mySprite->ResourceId();
        mySprite.Nullify(); // we need nullify all handles before ReleaseResource() call
        theCtx->ReleaseResource(aSpriteKey, true);
      }
      if (!mySpriteA.IsNull())
      {
        const TCollection_AsciiString aSpriteKeyA = mySpriteA->ResourceId();
        mySpriteA.Nullify();
        theCtx->ReleaseResource(aSpriteKeyA, true);
      }
    }
  }
  mySprite.Nullify();
  mySpriteA.Nullify();
}

//=================================================================================================

bool OpenGl_AspectsSprite::HasPointSprite(const occ::handle<OpenGl_Context>&    theCtx,
                                          const occ::handle<Graphic3d_Aspects>& theAspects)
{
  const occ::handle<OpenGl_PointSprite>& aSprite = Sprite(theCtx, theAspects, false);
  return !aSprite.IsNull() && !aSprite->IsDisplayList();
}

//=================================================================================================

bool OpenGl_AspectsSprite::IsDisplayListSprite(const occ::handle<OpenGl_Context>&    theCtx,
                                               const occ::handle<Graphic3d_Aspects>& theAspects)
{
  if (theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES)
  {
    return false;
  }

  const occ::handle<OpenGl_PointSprite>& aSprite = Sprite(theCtx, theAspects, false);
  return !aSprite.IsNull() && aSprite->IsDisplayList();
}

//=================================================================================================

void OpenGl_AspectsSprite::UpdateRediness(const occ::handle<Graphic3d_Aspects>& theAspect)
{
  // update sprite resource bindings
  TCollection_AsciiString aSpriteKeyNew, aSpriteAKeyNew;
  spriteKeys(theAspect->MarkerImage(),
             theAspect->MarkerType(),
             theAspect->MarkerScale(),
             theAspect->ColorRGBA(),
             aSpriteKeyNew,
             aSpriteAKeyNew);
  const TCollection_AsciiString& aSpriteKeyOld =
    !mySprite.IsNull() ? mySprite->ResourceId() : TCollection_AsciiString::EmptyString();
  const TCollection_AsciiString& aSpriteAKeyOld =
    !mySpriteA.IsNull() ? mySpriteA->ResourceId() : TCollection_AsciiString::EmptyString();
  if (aSpriteKeyNew.IsEmpty() || aSpriteKeyOld != aSpriteKeyNew || aSpriteAKeyNew.IsEmpty()
      || aSpriteAKeyOld != aSpriteAKeyNew)
  {
    myIsSpriteReady = false;
    myMarkerSize    = theAspect->MarkerScale();
  }
}

//=================================================================================================

const occ::handle<OpenGl_PointSprite>& OpenGl_AspectsSprite::Sprite(
  const occ::handle<OpenGl_Context>&    theCtx,
  const occ::handle<Graphic3d_Aspects>& theAspects,
  bool                                  theIsAlphaSprite)
{
  if (!myIsSpriteReady)
  {
    build(theCtx,
          theAspects->MarkerImage(),
          theAspects->MarkerType(),
          theAspects->MarkerScale(),
          theAspects->ColorRGBA(),
          myMarkerSize);
    myIsSpriteReady = true;
  }
  return theIsAlphaSprite && !mySpriteA.IsNull() && mySpriteA->IsValid() ? mySpriteA : mySprite;
}

//=================================================================================================

void OpenGl_AspectsSprite::build(const occ::handle<OpenGl_Context>&        theCtx,
                                 const occ::handle<Graphic3d_MarkerImage>& theMarkerImage,
                                 Aspect_TypeOfMarker                       theType,
                                 float                                     theScale,
                                 const NCollection_Vec4<float>&            theColor,
                                 float&                                    theMarkerSize)
{
  // generate key for shared resource
  TCollection_AsciiString aNewKey, aNewKeyA;
  spriteKeys(theMarkerImage, theType, theScale, theColor, aNewKey, aNewKeyA);

  const TCollection_AsciiString& aSpriteKeyOld =
    !mySprite.IsNull() ? mySprite->ResourceId() : TCollection_AsciiString::EmptyString();
  const TCollection_AsciiString& aSpriteAKeyOld =
    !mySpriteA.IsNull() ? mySpriteA->ResourceId() : TCollection_AsciiString::EmptyString();

  // release old shared resources
  const bool aNewResource = aNewKey.IsEmpty() || aSpriteKeyOld != aNewKey;
  if (aNewResource)
  {
    if (!mySprite.IsNull())
    {
      if (mySprite->ResourceId().IsEmpty())
      {
        theCtx->DelayedRelease(mySprite);
        mySprite.Nullify();
      }
      else
      {
        const TCollection_AsciiString anOldKey = mySprite->ResourceId();
        mySprite.Nullify(); // we need nullify all handles before ReleaseResource() call
        theCtx->ReleaseResource(anOldKey, true);
      }
    }
  }
  if (aNewKeyA.IsEmpty() || aSpriteAKeyOld != aNewKeyA)
  {
    if (!mySpriteA.IsNull())
    {
      if (mySpriteA->ResourceId().IsEmpty())
      {
        theCtx->DelayedRelease(mySpriteA);
        mySpriteA.Nullify();
      }
      else
      {
        const TCollection_AsciiString anOldKey = mySpriteA->ResourceId();
        mySpriteA.Nullify(); // we need nullify all handles before ReleaseResource() call
        theCtx->ReleaseResource(anOldKey, true);
      }
    }
  }

  if (!aNewResource)
  {
    const OpenGl_PointSprite* aSprite = dynamic_cast<OpenGl_PointSprite*>(mySprite.get());
    if (!aSprite->IsDisplayList())
    {
      theMarkerSize = float(std::max(aSprite->SizeX(), aSprite->SizeY()));
    }
    return;
  }
  if (theType == Aspect_TOM_POINT || theType == Aspect_TOM_EMPTY
      || (theType == Aspect_TOM_USERDEFINED && theMarkerImage.IsNull()))
  {
    // nothing to do - just simple point
    return;
  }

  occ::handle<OpenGl_PointSprite>& aSprite  = mySprite;
  occ::handle<OpenGl_PointSprite>& aSpriteA = mySpriteA;
  if (!aNewKey.IsEmpty())
  {
    // clang-format off
    theCtx->GetResource<occ::handle<OpenGl_PointSprite>> (aNewKeyA, aSpriteA); // alpha sprite could be shared
    // clang-format on
    theCtx->GetResource<occ::handle<OpenGl_PointSprite>>(aNewKey, aSprite);
  }

  const bool hadAlreadyRGBA  = !aSprite.IsNull();
  const bool hadAlreadyAlpha = !aSpriteA.IsNull();
  if (hadAlreadyRGBA && hadAlreadyAlpha)
  {
    // reuse shared resource
    if (!aSprite->IsDisplayList())
    {
      theMarkerSize = float(std::max(aSprite->SizeX(), aSprite->SizeY()));
    }
    return;
  }

  if (!hadAlreadyAlpha)
  {
    aSpriteA = new OpenGl_PointSprite(aNewKeyA);
  }
  if (!hadAlreadyRGBA)
  {
    aSprite = new OpenGl_PointSprite(aNewKey);
  }
  if (!aNewKey.IsEmpty())
  {
    if (!hadAlreadyRGBA)
    {
      theCtx->ShareResource(aNewKey, aSprite);
    }
    if (!hadAlreadyAlpha)
    {
      theCtx->ShareResource(aNewKeyA, aSpriteA);
    }
  }

  occ::handle<Graphic3d_MarkerImage> aNewMarkerImage =
    theType == Aspect_TOM_USERDEFINED && !theMarkerImage.IsNull()
      ? theMarkerImage
      : Graphic3d_MarkerImage::StandardMarker(theType, theScale, theColor);
  if (aNewMarkerImage.IsNull())
  {
    return;
  }

  if (theCtx->core20fwd != NULL && (!theCtx->caps->pntSpritesDisable || theCtx->core11ffp == NULL))
  {
    // Creating texture resource for using it with point sprites
    occ::handle<Image_PixMap> anImage = aNewMarkerImage->GetImage();
    theMarkerSize                     = std::max((float)anImage->Width(), (float)anImage->Height());

    if (!hadAlreadyRGBA)
    {
      aSprite->Init(theCtx, *anImage, Graphic3d_TypeOfTexture_2D, true);
    }
    if (!hadAlreadyAlpha)
    {
      if (occ::handle<Image_PixMap> anImageA = aSprite->GetFormat() != GL_ALPHA
                                                 ? aNewMarkerImage->GetImageAlpha()
                                                 : occ::handle<Image_PixMap>())
      {
        aSpriteA->Init(theCtx, *anImageA, Graphic3d_TypeOfTexture_2D, true);
      }
    }
  }
  else if (theCtx->core11ffp != NULL)
  {
    // Creating list with bitmap for using it in compatibility mode
    GLuint aBitmapList = theCtx->core11ffp->glGenLists(1);
    aSprite->SetDisplayList(theCtx, aBitmapList);

    occ::handle<Image_PixMap> anImage =
      aNewMarkerImage->IsColoredImage() ? aNewMarkerImage->GetImage() : occ::handle<Image_PixMap>();
    const OpenGl_TextureFormat aFormat =
      !anImage.IsNull() ? OpenGl_TextureFormat::FindFormat(theCtx, anImage->Format(), true)
                        : OpenGl_TextureFormat();
    if (aFormat.IsValid())
    {
      if (anImage->IsTopDown())
      {
        occ::handle<Image_PixMap> anImageCopy = new Image_PixMap();
        anImageCopy->InitCopy(*anImage);
        Image_PixMap::FlipY(*anImageCopy);
        anImage = anImageCopy;
      }
      const GLint anAligment = std::min((GLint)anImage->MaxRowAligmentBytes(), 8);
      theCtx->core11fwd->glPixelStorei(GL_UNPACK_ALIGNMENT, anAligment);

      const GLint anExtraBytes = GLint(anImage->RowExtraBytes());
      const GLint aPixelsWidth = GLint(anImage->SizeRowBytes() / anImage->SizePixelBytes());
      const GLint aRowLength   = (anExtraBytes >= anAligment) ? aPixelsWidth : 0;
      theCtx->core11fwd->glPixelStorei(GL_UNPACK_ROW_LENGTH, aRowLength);

      theCtx->core11ffp->glNewList(aBitmapList, GL_COMPILE);
      const int aWidth = (int)anImage->Width(), aHeight = (int)anImage->Height();
      // clang-format off
      theCtx->core11ffp->glBitmap (0, 0, 0, 0, GLfloat(-0.5f * aWidth), GLfloat(-0.5f * aHeight), NULL); // make offsets that will be added to the current raster position
      // clang-format on
      theCtx->core11ffp->glDrawPixels(GLsizei(anImage->Width()),
                                      GLsizei(anImage->Height()),
                                      aFormat.PixelFormat(),
                                      aFormat.DataType(),
                                      anImage->Data());
      theCtx->core11ffp->glEndList();
      theCtx->core11fwd->glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      theCtx->core11fwd->glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    }

    if (!aFormat.IsValid() || !hadAlreadyAlpha)
    {
      if (aFormat.IsValid())
      {
        aBitmapList = theCtx->core11ffp->glGenLists(1);
        aSpriteA->SetDisplayList(theCtx, aBitmapList);
      }

      int aWidth = 0, aHeight = 0;
      aNewMarkerImage->GetTextureSize(aWidth, aHeight);
      if (occ::handle<NCollection_HArray1<uint8_t>> aBitMap = aNewMarkerImage->GetBitMapArray())
      {
        theCtx->core11ffp->glNewList(aBitmapList, GL_COMPILE);
        theCtx->core11ffp->glBitmap((GLsizei)aWidth,
                                    (GLsizei)aHeight,
                                    (GLfloat)(0.5f * aWidth),
                                    (GLfloat)(0.5f * aHeight),
                                    0.f,
                                    0.f,
                                    (const GLubyte*)&aBitMap->First());
        theCtx->core11ffp->glEndList();
      }
    }
  }
}

//=================================================================================================

void OpenGl_AspectsSprite::spriteKeys(const occ::handle<Graphic3d_MarkerImage>& theMarkerImage,
                                      Aspect_TypeOfMarker                       theType,
                                      float                                     theScale,
                                      const NCollection_Vec4<float>&            theColor,
                                      TCollection_AsciiString&                  theKey,
                                      TCollection_AsciiString&                  theKeyA)
{
  // generate key for shared resource
  if (theType == Aspect_TOM_USERDEFINED)
  {
    if (!theMarkerImage.IsNull())
    {
      theKey  = theMarkerImage->GetImageId();
      theKeyA = theMarkerImage->GetImageAlphaId();
    }
  }
  else if (theType != Aspect_TOM_POINT && theType != Aspect_TOM_EMPTY)
  {
    // predefined markers are defined with 0.5 step
    const int aScale = int(theScale * 10.0f + 0.5f);
    theKey           = TCollection_AsciiString("OpenGl_AspectMarker") + theType + "_" + aScale;
    theKeyA          = theKey + "A";
    if (theType == Aspect_TOM_BALL)
    {
      unsigned int aColor[3] = {(unsigned int)(255.0f * theColor.r()),
                                (unsigned int)(255.0f * theColor.g()),
                                (unsigned int)(255.0f * theColor.b())};
      char         aBytes[8];
      Sprintf(aBytes, "%02X%02X%02X", aColor[0], aColor[1], aColor[2]);
      theKey += aBytes;
    }
  }
}
