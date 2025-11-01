// Author: Ilya Khramov
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

#ifndef _Graphic3d_CubeMapPacked_HeaderFile
#define _Graphic3d_CubeMapPacked_HeaderFile

#include <Graphic3d_CubeMap.hxx>
#include <NCollection_Array1.hxx>
#include <OSD_Path.hxx>

//! Class is intended to process cubemap packed into single image plane.
class Graphic3d_CubeMapPacked : public Graphic3d_CubeMap
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_CubeMapPacked, Graphic3d_CubeMap)
public:
  //! Initialization to load cubemap from file.
  //! @theFileName - path to the cubemap image
  //! @theOrder - array containing six different indexes of cubemap sides which maps tile grid to
  //! cubemap sides
  Standard_EXPORT Graphic3d_CubeMapPacked(
    const TCollection_AsciiString&         theFileName,
    const Graphic3d_ValidatedCubeMapOrder& theOrder = Graphic3d_CubeMapOrder::Default());

  //! Initialization to set cubemap directly by PixMap.
  //! @thePixMap - origin PixMap
  //! @theOrder - array containing six different indexes of cubemap sides which maps tile grid to
  //! cubemap sides
  Standard_EXPORT Graphic3d_CubeMapPacked(
    const Handle(Image_PixMap)&            theImage,
    const Graphic3d_ValidatedCubeMapOrder& theOrder = Graphic3d_CubeMapOrder::Default());

  //! Returns current cubemap side as compressed PixMap.
  Standard_EXPORT virtual Handle(Image_CompressedPixMap) CompressedValue(
    const Handle(Image_SupportedFormats)& theSupported) Standard_OVERRIDE;

  //! Returns current cubemap side as PixMap.
  //! Resulting PixMap is memory wrapper over original image.
  //! Returns null handle if current side or whole cubemap is invalid.
  //! Origin image has to contain six quad tiles having one sizes without any gaps to be valid.
  Standard_EXPORT virtual Handle(Image_PixMap) Value(
    const Handle(Image_SupportedFormats)& theSupported) Standard_OVERRIDE;

  //! Empty destructor.
  ~Graphic3d_CubeMapPacked() {}

private:
  //! Checks whether given tiles order is valid.
  static Standard_Boolean checkOrder(const NCollection_Array1<unsigned int>& theOrder);

  //! Checks whether given pixmap is valid to contain six tiles.
  static Standard_Boolean checkImage(const Handle(Image_PixMap)& theImage,
                                     unsigned int&               theTileNumberX);

  //! Tries to load image from file and checks it after that.
  //! Does nothing in case of fail.
  void tryLoadImage(const Handle(Image_SupportedFormats)& theSupported,
                    const TCollection_AsciiString&        theFilePath);

protected:
  Graphic3d_CubeMapOrder myOrder;       //!< order mapping tile grit to cubemap sides
  unsigned int           myTileNumberX; //!< width of tile grid
};

DEFINE_STANDARD_HANDLE(Graphic3d_CubeMapPacked, Graphic3d_CubeMap)

#endif // _Graphic3d_CubeMapPacked_HeaderFile
