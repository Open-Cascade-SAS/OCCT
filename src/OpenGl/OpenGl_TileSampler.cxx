// Created on: 2016-06-16
// Created by: Denis BOGOLEPOV & Danila ULYANOV
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

#include <OpenGl_Context.hxx>
#include <OpenGl_TileSampler.hxx>
#include <Graphic3d_RenderingParams.hxx>

// define to debug algorithm values
// #define RAY_TRACE_PRINT_DEBUG_INFO

//=================================================================================================

OpenGl_TileSampler::OpenGl_TileSampler()
    : myLastSample(0),
      myScaleFactor(1.0f),
      myTileSize(0),
      myViewSize(0, 0)
{
  //
}

//=================================================================================================

void OpenGl_TileSampler::GrabVarianceMap(const Handle(OpenGl_Context)& theContext,
                                         const Handle(OpenGl_Texture)& theTexture)
{
  if (theTexture.IsNull())
  {
    return;
  }

  myVarianceRaw.Init(0);

  theTexture->Bind(theContext);
  theContext->core11fwd->glPixelStorei(GL_PACK_ALIGNMENT, 1);
  theContext->core11fwd->glPixelStorei(GL_PACK_ROW_LENGTH, 0);
  theContext->core11fwd->glGetTexImage(GL_TEXTURE_2D,
                                       0,
                                       GL_RED_INTEGER,
                                       GL_INT,
                                       myVarianceRaw.ChangeData());
  const GLenum anErr = theContext->core11fwd->glGetError();
  theTexture->Unbind(theContext);
  if (anErr != GL_NO_ERROR)
  {
    theContext->PushMessage(
      GL_DEBUG_SOURCE_APPLICATION,
      GL_DEBUG_TYPE_ERROR,
      0,
      GL_DEBUG_SEVERITY_MEDIUM,
      TCollection_AsciiString("Error! Failed to fetch visual error map from the GPU ")
        + OpenGl_Context::FormatGlError(anErr));
    return;
  }

  const float aFactor = 1.0f / myScaleFactor;
  for (Standard_Size aColIter = 0; aColIter < myVarianceMap.SizeX; ++aColIter)
  {
    for (Standard_Size aRowIter = 0; aRowIter < myVarianceMap.SizeY; ++aRowIter)
    {
      const int aRawValue = myVarianceRaw.Value(aRowIter, aColIter);
      Standard_RangeError_Raise_if(
        aRawValue < 0,
        "Internal Error: signed integer overflow within OpenGl_TileSampler");

      float& aTile = myVarianceMap.ChangeValue(aRowIter, aColIter);
      aTile        = aFactor * float(aRawValue);
      aTile *= 1.0f / tileArea((int)aColIter, (int)aRowIter); // average error over the tile
      if (aRowIter != 0)
      {
        aTile += myVarianceMap.Value(aRowIter - 1, aColIter);
      }
    }
  }

  // build marginal distribution
  for (Standard_Size aX = 0; aX < myVarianceMap.SizeX; ++aX)
  {
    myMarginalMap[aX] = myVarianceMap.Value(myVarianceMap.SizeY - 1, aX);
    if (aX != 0)
    {
      myMarginalMap[aX] += myMarginalMap[aX - 1];
    }
  }

#ifdef RAY_TRACE_PRINT_DEBUG_INFO
  dumpMap(std::cerr, myVarianceRaw, "OpenGl_TileSampler, Variance map");
#endif
}

//=================================================================================================

void OpenGl_TileSampler::dumpMap(std::ostream&                     theStream,
                                 const Image_PixMapTypedData<int>& theMap,
                                 const char*                       theTitle) const
{
  theStream << theTitle << " " << theMap.SizeX << "x" << theMap.SizeY << " (tile " << myTileSize
            << "x" << myTileSize << ")" << ":\n";
  for (Standard_Size aRowIter = 0; aRowIter < theMap.SizeY; ++aRowIter)
  {
    for (Standard_Size aColIter = 0; aColIter < theMap.SizeX; ++aColIter)
    {
      theStream << " [" << theMap.Value(aRowIter, aColIter) << "]";
    }
    theStream << "\n";
  }
}

//=================================================================================================

Graphic3d_Vec2i OpenGl_TileSampler::nextTileToSample()
{
  Graphic3d_Vec2i aTile(0, 0);
  const float     aKsiX = mySampler.sample(0, myLastSample) * myMarginalMap.back();
  for (; (size_t)aTile.x() < myMarginalMap.size() - 1; ++aTile.x())
  {
    if (aKsiX <= myMarginalMap[aTile.x()])
    {
      break;
    }
  }

  const float aKsiY =
    mySampler.sample(1, myLastSample) * myVarianceMap.Value(myVarianceMap.SizeY - 1, aTile.x());
  for (; (size_t)aTile.y() < myVarianceMap.SizeY - 1; ++aTile.y())
  {
    if (aKsiY <= myVarianceMap.Value(aTile.y(), aTile.x()))
    {
      break;
    }
  }

  ++myLastSample;
  return aTile;
}

//=================================================================================================

void OpenGl_TileSampler::SetSize(const Graphic3d_RenderingParams& theParams,
                                 const Graphic3d_Vec2i&           theSize)
{
  if (theSize.x() <= 0 || theSize.y() <= 0)
  {
    return;
  }

  myViewSize = theSize;

  const int aTileSize = Max(theParams.RayTracingTileSize, 1);
  const int aNbTilesX =
    Max(1, static_cast<int>(ceilf(static_cast<float>(theSize.x()) / aTileSize)));
  const int aNbTilesY =
    Max(1, static_cast<int>(ceilf(static_cast<float>(theSize.y()) / aTileSize)));
  if (myTileSize != aTileSize || (int)myTiles.SizeX != aNbTilesX || (int)myTiles.SizeY != aNbTilesY)
  {
    myTileSize    = aTileSize;
    myScaleFactor = 1.0e6f * (1024.0f / float(myTileSize * myTileSize));

    Handle(NCollection_BaseAllocator) anAlloc = NCollection_BaseAllocator::CommonBaseAllocator();
    myTiles.SetTopDown(true);
    myTiles.Init(anAlloc, aNbTilesX, aNbTilesY);
    myTiles.Init(1);

    myTileSamples.SetTopDown(true);
    myTileSamples.Init(myTiles.Allocator(), aNbTilesX, aNbTilesY);
    myTileSamples.Init(1);

    myVarianceMap.SetTopDown(true);
    myVarianceMap.Init(myTiles.Allocator(), myTiles.SizeX, myTiles.SizeY);
    myVarianceMap.Init(0.0f);

    myVarianceRaw.SetTopDown(true);
    myVarianceRaw.Init(myTiles.Allocator(), myTiles.SizeX, myTiles.SizeY);
    myVarianceRaw.Init(0);

    myOffsets.SetTopDown(true);
    myOffsets.Init(myTiles.Allocator(), myTiles.SizeX, myTiles.SizeY);
    myOffsets.Init(Graphic3d_Vec2i(-1, -1));

    myMarginalMap.resize(myTiles.SizeX);
    myMarginalMap.assign(myMarginalMap.size(), 0.0f);
  }

  // calculate a size of compact offsets texture optimal for rendering reduced number of tiles
  Standard_Integer aNbShunkTilesX = (int)myTiles.SizeX, aNbShunkTilesY = (int)myTiles.SizeY;
  if (theParams.NbRayTracingTiles > 0)
  {
    aNbShunkTilesX = 8;
    aNbShunkTilesY = 8;
    for (Standard_Integer anIdx = 0; aNbShunkTilesX * aNbShunkTilesY < theParams.NbRayTracingTiles;
         ++anIdx)
    {
      (anIdx % 2 == 0 ? aNbShunkTilesX : aNbShunkTilesY) <<= 1;
    }
  }
  if ((int)myOffsetsShrunk.SizeX != aNbShunkTilesX || (int)myOffsetsShrunk.SizeY != aNbShunkTilesY)
  {
    myOffsetsShrunk.SetTopDown(true);
    myOffsetsShrunk.Init(myTiles.Allocator(), aNbShunkTilesX, aNbShunkTilesY);
    myOffsetsShrunk.Init(Graphic3d_Vec2i(-1, -1));
  }
}

//=================================================================================================

bool OpenGl_TileSampler::upload(const Handle(OpenGl_Context)& theContext,
                                const Handle(OpenGl_Texture)& theSamplesTexture,
                                const Handle(OpenGl_Texture)& theOffsetsTexture,
                                const bool                    theAdaptive)
{
  if (myTiles.IsEmpty())
  {
    return false;
  }

  // Fill in myTiles map with a number of passes (samples) per tile.
  // By default, all tiles receive 1 sample, but basing on visual error level (myVarianceMap),
  // this amount is re-distributed from tiles having smallest error take 0 samples to tiles having
  // larger error. This redistribution is smoothed by Halton sampler.
  //
  // myOffsets map is filled as redirection of currently rendered tile to another one
  // so that tiles having smallest error level have 0 tiles redirected from,
  // while tiles with great error level might be rendered more than 1.
  // This map is used within single-pass rendering method requiring atomic float operation support
  // from hardware.
  myTiles.Init(0);
  Image_PixMapTypedData<Graphic3d_Vec2i>& anOffsets = theAdaptive ? myOffsetsShrunk : myOffsets;
  anOffsets.Init(Graphic3d_Vec2i(-1, -1));
  for (Standard_Size aRowIter = 0; aRowIter < anOffsets.SizeY; ++aRowIter)
  {
    for (Standard_Size aColIter = 0; aColIter < anOffsets.SizeX; ++aColIter)
    {
      Graphic3d_Vec2i& aRedirectTile = anOffsets.ChangeValue(aRowIter, aColIter);
      aRedirectTile =
        theAdaptive ? nextTileToSample() : Graphic3d_Vec2i((int)aColIter, (int)aRowIter);
      myTiles.ChangeValue(aRedirectTile.y(), aRedirectTile.x()) += 1;
    }
  }

#ifdef RAY_TRACE_PRINT_DEBUG_INFO
  dumpMap(std::cerr, myTiles, "OpenGl_TileSampler, Samples");
#endif

  // Fill in myTileSamples map from myTiles with an actual number of Samples per Tile as multiple of
  // Tile Area (e.g. tile that should be rendered ones will have amount of samples equal to its are
  // 4x4=16). This map is used for discarding tile fragments having <=0 of samples left within
  // multi-pass rendering.
  myTileSamples.Init(0);
  for (Standard_Size aRowIter = 0; aRowIter < myTiles.SizeY; ++aRowIter)
  {
    for (Standard_Size aColIter = 0; aColIter < myTiles.SizeX; ++aColIter)
    {
      myTileSamples.ChangeValue(aRowIter, aColIter) =
        tileArea((int)aColIter, (int)aRowIter) * myTiles.Value(aRowIter, aColIter);
    }
  }

  bool hasErrors = false;

  if (!theSamplesTexture.IsNull())
  {
    theSamplesTexture->Bind(theContext);
    theContext->core11fwd->glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    if (theContext->hasUnpackRowLength)
    {
      theContext->core11fwd->glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    }
    if (theSamplesTexture->SizeX() == (int)myTileSamples.SizeX
        && theSamplesTexture->SizeY() == (int)myTileSamples.SizeY)
    {
      theContext->core11fwd->glTexSubImage2D(GL_TEXTURE_2D,
                                             0,
                                             0,
                                             0,
                                             (int)myTileSamples.SizeX,
                                             (int)myTileSamples.SizeY,
                                             GL_RED_INTEGER,
                                             GL_INT,
                                             myTileSamples.Data());
      if (theContext->core11fwd->glGetError() != GL_NO_ERROR)
      {
        hasErrors = true;
        theContext->PushMessage(GL_DEBUG_SOURCE_APPLICATION,
                                GL_DEBUG_TYPE_ERROR,
                                0,
                                GL_DEBUG_SEVERITY_MEDIUM,
                                "Error! Failed to upload tile samples map on the GPU");
      }
    }
    else
    {
      hasErrors = true;
    }
    theSamplesTexture->Unbind(theContext);
  }

  if (!theOffsetsTexture.IsNull())
  {
    if (theOffsetsTexture->SizeX() != (int)anOffsets.SizeX
        || theOffsetsTexture->SizeY() != (int)anOffsets.SizeY || !theOffsetsTexture->IsValid())
    {
      theOffsetsTexture->Release(theContext.get());
      if (!theOffsetsTexture->Init(theContext,
                                   OpenGl_TextureFormat::FindSizedFormat(theContext, GL_RG32I),
                                   Graphic3d_Vec2i((int)anOffsets.SizeX, (int)anOffsets.SizeY),
                                   Graphic3d_TypeOfTexture_2D))
      {
        hasErrors = true;
      }
    }
    if (theOffsetsTexture->IsValid())
    {
      theOffsetsTexture->Bind(theContext);
      theContext->core11fwd->glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      if (theContext->hasUnpackRowLength)
      {
        theContext->core11fwd->glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
      }
      theContext->core11fwd->glTexSubImage2D(GL_TEXTURE_2D,
                                             0,
                                             0,
                                             0,
                                             (int)anOffsets.SizeX,
                                             (int)anOffsets.SizeY,
                                             GL_RG_INTEGER,
                                             GL_INT,
                                             anOffsets.Data());
      if (theContext->core11fwd->glGetError() != GL_NO_ERROR)
      {
        hasErrors = true;
        theContext->PushMessage(GL_DEBUG_SOURCE_APPLICATION,
                                GL_DEBUG_TYPE_ERROR,
                                0,
                                GL_DEBUG_SEVERITY_MEDIUM,
                                "Error! Failed to upload tile offset map on the GPU");
      }
      theOffsetsTexture->Unbind(theContext);
    }
  }
  return !hasErrors;
}
