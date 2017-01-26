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

#ifndef _OpenGl_TileSampler_H
#define _OpenGl_TileSampler_H

#include <OpenGl_Texture.hxx>
#include <OpenGl_HaltonSampler.hxx>

#include <vector>

//! Tool object used for sampling screen tiles according to estimated pixel variance (used in path tracing engine).
//! To improve GPU thread coherency, rendering window is split into pixel blocks or tiles. The important feature of
//! this approach is that it is possible to keep the same number of tiles for any screen resolution (e.g. 256 tiles
//! can be used for both 512 x 512 window and 1920 x 1080 window). So, a smaller number of tiles allows to increase
//! interactivity (FPS), but at the cost of higher per-frame variance ('noise'). On the contrary a larger number of
//! tiles decrease interactivity, but leads to lower per-frame variance. Note that the total time needed to produce
//! final final image is the same for both cases.
class OpenGl_TileSampler
{
public:

  //! Size of individual tile in pixels.
  static int TileSize() { return 32; }

public:

  //! Creates new tile sampler.
  Standard_EXPORT OpenGl_TileSampler();

  //! Returns width of ray-tracing viewport.
  int SizeX() const { return mySizeX; }

  //! Returns height of ray-tracing viewport.
  int SizeY() const { return mySizeY; }

  //! Returns number of tiles in X dimension.
  int NbTilesX() const { return myTilesX; }

  //! Returns number of tiles in Y dimension.
  int NbTilesY() const { return myTilesY; }

  //! Returns total number of tiles in viewport.
  int NbTiles() const { return myTilesX * myTilesY; }

  //! Specifies size of ray-tracing viewport.
  Standard_EXPORT void SetSize (const int theSizeX,
                                const int theSizeY);

  //! Returns number of pixels in the given tile.
  int TileArea (const int theX,
                const int theY) const
  {
    return Min (TileSize(), mySizeX - theX * TileSize())
         * Min (TileSize(), mySizeY - theY * TileSize());
  }

  //! Fetches current error estimation from the GPU and
  //! builds 2D discrete distribution for tile sampling.
  Standard_EXPORT void GrabVarianceMap (const Handle(OpenGl_Context)& theContext);

  //! Samples tile location according to estimated error.
  Standard_EXPORT void Sample (int& theOffsetX,
                               int& theOffsetY);

  //! Resets tile sampler to initial state.
  void Reset() { mySample = 0; }

  //! Uploads offsets of sampled tiles to the given OpenGL texture.
  Standard_EXPORT void Upload (const Handle(OpenGl_Context)& theContext,
                               const Handle(OpenGl_Texture)& theTexture,
                               const int                     theNbTilesX,
                               const int                     theNbTilesY,
                               const bool                    theAdaptive);

protected:

  //! Returns tile value (estimated error).
  float Tile (const int theX,
              const int theY) const
  {
    return myVarianceMap[theY * myTilesX + theX];
  }

  //! Returns tile value (estimated error).
  float& ChangeTile (const int theX,
                     const int theY)
  {
    return myVarianceMap[theY * myTilesX + theX];
  }

protected:

  std::vector<float>   myVarianceMap; //!< Estimation of visual error per tile
  std::vector<float>   myMarginalMap; //!< Marginal distribution of 2D error map
  OpenGl_HaltonSampler mySampler;     //!< Halton sequence generator
  int                  mySample;      //!< Index of generated sample
  int                  mySizeX;       //!< Width of ray-tracing viewport
  int                  mySizeY;       //!< Height of ray-tracing viewport
  int                  myTilesX;      //!< Number of tiles in X dimension
  int                  myTilesY;      //!< Number of tiles in Y dimension

};

#endif // _OpenGl_TileSampler_H
