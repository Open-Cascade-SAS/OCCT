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

#include <Image_PixMapTypedData.hxx>

#include <vector>

class Graphic3d_RenderingParams;

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

  //! Creates new tile sampler.
  Standard_EXPORT OpenGl_TileSampler();

  //! Size of individual tile in pixels.
  Graphic3d_Vec2i TileSize() const { return Graphic3d_Vec2i (myTileSize, myTileSize); }

  //! Scale factor for quantization of visual error (float) into signed integer.
  float VarianceScaleFactor() const { return myScaleFactor; }

  //! Returns number of tiles in X dimension.
  int NbTilesX() const { return (int)myTiles.SizeX; }

  //! Returns number of tiles in Y dimension.
  int NbTilesY() const { return (int)myTiles.SizeY; }

  //! Returns total number of tiles in viewport.
  int NbTiles() const { return int(myTiles.SizeX * myTiles.SizeY); }

  //! Returns ray-tracing viewport.
  const Graphic3d_Vec2i& ViewSize() const { return myViewSize; }

  //! Number of tiles within offsets texture.
  Graphic3d_Vec2i NbOffsetTiles (bool theAdaptive) const
  {
    return theAdaptive
         ? Graphic3d_Vec2i ((int )myOffsetsShrunk.SizeX, (int )myOffsetsShrunk.SizeY)
         : Graphic3d_Vec2i ((int )myOffsets.SizeX,       (int )myOffsets.SizeY);
  }

  //! Maximum number of tiles within offsets texture.
  Graphic3d_Vec2i NbOffsetTilesMax() const { return NbOffsetTiles (true).cwiseMax (NbOffsetTiles (false)); }

  //! Viewport for rendering using offsets texture.
  Graphic3d_Vec2i OffsetTilesViewport (bool theAdaptive) const { return NbOffsetTiles (theAdaptive) * myTileSize; }

  //! Maximum viewport for rendering using offsets texture.
  Graphic3d_Vec2i OffsetTilesViewportMax() const { return NbOffsetTilesMax() * myTileSize; }

  //! Return maximum number of samples per tile.
  int MaxTileSamples() const
  {
    int aNbSamples = 0;
    for (Standard_Size aRowIter = 0; aRowIter < myTiles.SizeY; ++aRowIter)
    {
      for (Standard_Size aColIter = 0; aColIter < myTiles.SizeX; ++aColIter)
      {
        aNbSamples = Max (aNbSamples, myTiles.Value (aRowIter, aColIter));
      }
    }
    return aNbSamples;
  }

  //! Specifies size of ray-tracing viewport and recomputes tile size.
  Standard_EXPORT void SetSize (const Graphic3d_RenderingParams& theParams,
                                const Graphic3d_Vec2i& theSize);

  //! Fetches current error estimation from the GPU and
  //! builds 2D discrete distribution for tile sampling.
  Standard_EXPORT void GrabVarianceMap (const Handle(OpenGl_Context)& theContext,
                                        const Handle(OpenGl_Texture)& theTexture);

  //! Resets (restart) tile sampler to initial state.
  void Reset() { myLastSample = 0; }

  //! Uploads tile samples to the given OpenGL texture.
  bool UploadSamples (const Handle(OpenGl_Context)& theContext,
                      const Handle(OpenGl_Texture)& theSamplesTexture,
                      const bool theAdaptive)
  {
    return upload (theContext, theSamplesTexture, Handle(OpenGl_Texture)(), theAdaptive);
  }

  //! Uploads offsets of sampled tiles to the given OpenGL texture.
  bool UploadOffsets (const Handle(OpenGl_Context)& theContext,
                      const Handle(OpenGl_Texture)& theOffsetsTexture,
                      const bool theAdaptive)
  {
    return upload (theContext, Handle(OpenGl_Texture)(), theOffsetsTexture, theAdaptive);
  }

protected:

  //! Returns number of pixels in the given tile.
  int tileArea (int theX, int theY) const
  {
    const int aSizeX = Min (myTileSize, myViewSize.x() - theX * myTileSize);
    const int aSizeY = Min (myTileSize, myViewSize.y() - theY * myTileSize);
    return aSizeX * aSizeY;
  }

  //! Samples tile location according to estimated error.
  Standard_EXPORT Graphic3d_Vec2i nextTileToSample();

  //! Uploads offsets of sampled tiles to the given OpenGL texture.
  Standard_EXPORT bool upload (const Handle(OpenGl_Context)& theContext,
                               const Handle(OpenGl_Texture)& theSamplesTexture,
                               const Handle(OpenGl_Texture)& theOffsetsTexture,
                               const bool theAdaptive);

  //! Auxiliary method for dumping 2D image map into stream (e.g. for debugging).
  Standard_EXPORT void dumpMap (std::ostream& theStream,
                                const Image_PixMapTypedData<int>& theMap,
                                const char* theTitle) const;

protected:

  Image_PixMapTypedData<unsigned int>    myTiles;         //!< number of samples per tile (initially all 1)
  Image_PixMapTypedData<unsigned int>    myTileSamples;   //!< number of samples for all pixels within the tile (initially equals to Tile area)
  Image_PixMapTypedData<float>           myVarianceMap;   //!< Estimation of visual error per tile
  Image_PixMapTypedData<int>             myVarianceRaw;   //!< Estimation of visual error per tile (raw data)
  Image_PixMapTypedData<Graphic3d_Vec2i> myOffsets;       //!< 2D array of tiles redirecting to another tile
  Image_PixMapTypedData<Graphic3d_Vec2i> myOffsetsShrunk; //!< 2D array of tiles redirecting to another tile (shrunk)
  std::vector<float>                     myMarginalMap;   //!< Marginal distribution of 2D error map
  OpenGl_HaltonSampler                   mySampler;       //!< Halton sequence generator
  unsigned int                           myLastSample;    //!< Index of generated sample
  float                                  myScaleFactor;   //!< scale factor for quantization of visual error (float) into signed integer
  int                                    myTileSize;      //!< tile size
  Graphic3d_Vec2i                        myViewSize;      //!< ray-tracing viewport

};

#endif // _OpenGl_TileSampler_H
