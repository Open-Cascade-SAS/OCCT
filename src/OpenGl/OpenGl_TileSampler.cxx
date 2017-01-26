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
#include <TCollection_ExtendedString.hxx>

namespace
{
  //! Scale factor for estimating visual error.
  static const float THE_SCALE_FACTOR = 1.0f / 1e6f;
}

//=======================================================================
//function : OpenGl_TileSampler
//purpose  :
//=======================================================================
OpenGl_TileSampler::OpenGl_TileSampler()
: mySample (0),
  mySizeX  (0),
  mySizeY  (0),
  myTilesX (0),
  myTilesY (0)
{
  mySampler.initFaure();
}

//=======================================================================
//function : GrabVarianceMap
//purpose  :
//=======================================================================
void OpenGl_TileSampler::GrabVarianceMap (const Handle(OpenGl_Context)& theContext)
{
#if !defined(GL_ES_VERSION_2_0)
  std::vector<GLint> aRawData (NbTiles(), 0);

  theContext->core11fwd->glGetTexImage (GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_INT, &aRawData.front());
  const GLenum anErr = theContext->core11fwd->glGetError();
  if (anErr != GL_NO_ERROR)
  {
    theContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_MEDIUM,
                             "Error! Failed to fetch visual error map from the GPU");
  }
  else
  {
    for (int aTileIdx = 0, aNbTiles = NbTiles(); aTileIdx < aNbTiles; ++aTileIdx)
    {
      myVarianceMap[aTileIdx] = aRawData[aTileIdx] * THE_SCALE_FACTOR;
    }

    for (int aX = 0; aX < myTilesX; ++aX)
    {
      for (int aY = 0; aY < myTilesY; ++aY)
      {
        ChangeTile (aX, aY) *= 1.0f / TileArea (aX, aY); // average error over the tile

        if (aY > 0)
        {
          ChangeTile (aX, aY) += Tile (aX, aY - 1);
        }
      }
    }

    myMarginalMap.resize (myTilesX); // build marginal distribution
    for (int aX = 0; aX < myTilesX; ++aX)
    {
      myMarginalMap[aX] = Tile (aX, myTilesY - 1);

      if (aX > 0)
        myMarginalMap[aX] += myMarginalMap[aX - 1];
    }
  }
#else
  // glGetTexImage() is unavailable on OpenGL ES, FBO + glReadPixels() can be used instead
  (void )theContext;
#endif
}

//=======================================================================
//function : Sample
//purpose  :
//=======================================================================
void OpenGl_TileSampler::Sample (int& theOffsetX,
                                 int& theOffsetY)
{
  int aX = 0;
  int aY = 0;

  const float aKsiX = mySampler.sample (0, mySample) * myMarginalMap.back();
  for (; aX < myTilesX - 1; ++aX)
  {
    if (aKsiX <= myMarginalMap[aX])
    {
      break;
    }
  }

  const float aKsiY = mySampler.sample (1, mySample) * Tile (aX, myTilesY - 1);
  for (; aY < myTilesY - 1; ++aY)
  {
    if (aKsiY <= Tile (aX, aY))
    {
      break;
    }
  }

  theOffsetX = aX * TileSize();
  theOffsetY = aY * TileSize();

  ++mySample;
}

//=======================================================================
//function : SetSize
//purpose  :
//=======================================================================
void OpenGl_TileSampler::SetSize (const int theSizeX,
                                  const int theSizeY)
{
  if (mySizeX == theSizeX
   && mySizeY == theSizeY)
  {
    return;
  }

  mySizeX = theSizeX;
  mySizeY = theSizeY;

  myTilesX = static_cast<int> (ceilf (static_cast<float> (mySizeX) / TileSize()));
  myTilesY = static_cast<int> (ceilf (static_cast<float> (mySizeY) / TileSize()));

  myVarianceMap.resize (myTilesX * myTilesY);
}

//=======================================================================
//function : Upload
//purpose  :
//=======================================================================
void OpenGl_TileSampler::Upload (const Handle(OpenGl_Context)& theContext,
                                 const Handle(OpenGl_Texture)& theTexture,
                                 const int                     theNbTilesX,
                                 const int                     theNbTilesY,
                                 const bool                    theAdaptive)
{
  if (theTexture.IsNull())
  {
    return;
  }

  const int aNbTilesX = theAdaptive ? theNbTilesX : myTilesX;
  const int aNbTilesY = theAdaptive ? theNbTilesY : myTilesY;

  Standard_ASSERT_RAISE (aNbTilesX * aNbTilesY > 0,
    "Error! Number of sampling tiles should be positive");

  std::vector<GLint> aData (aNbTilesX * aNbTilesY * 2);

  for (int aX = 0; aX < aNbTilesX; ++aX)
  {
    for (int aY = 0; aY < aNbTilesY; ++aY)
    {
      if (!theAdaptive)
      {
        aData[(aY * aNbTilesX + aX) * 2 + 0] = aX * TileSize();
        aData[(aY * aNbTilesX + aX) * 2 + 1] = aY * TileSize();
      }
      else
      {
        Sample (aData[(aY * aNbTilesX + aX) * 2 + 0],
                aData[(aY * aNbTilesX + aX) * 2 + 1]);
      }
    }
  }

  theTexture->Bind (theContext);

  theContext->core11fwd->glTexImage2D (GL_TEXTURE_2D, 0, GL_RG32I, aNbTilesX, aNbTilesY, 0, GL_RG_INTEGER, GL_UNSIGNED_INT, &aData.front());

  if (theContext->core11fwd->glGetError() != GL_NO_ERROR)
  {
    theContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_MEDIUM,
                             "Error! Failed to upload tile offset map on the GPU");
  }
}
