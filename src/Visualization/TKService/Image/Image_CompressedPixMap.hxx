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

#ifndef _Image_CompressedPixMap_HeaderFile
#define _Image_CompressedPixMap_HeaderFile

#include <Image_Format.hxx>
#include <Image_CompressedFormat.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Buffer.hxx>
#include <Standard_Type.hxx>

//! Compressed pixmap data definition.
//! It is defined independently from Image_PixMap, which defines only uncompressed formats.
class Image_CompressedPixMap : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Image_CompressedPixMap, Standard_Transient)
public:
  //! Return base (uncompressed) pixel format.
  Image_Format BaseFormat() const { return myBaseFormat; }

  //! Set base (uncompressed) pixel format.
  void SetBaseFormat(Image_Format theFormat) { myBaseFormat = theFormat; }

  //! Return compressed format.
  Image_CompressedFormat CompressedFormat() const { return myFormat; }

  //! Set compressed format.
  void SetCompressedFormat(Image_CompressedFormat theFormat) { myFormat = theFormat; }

  //! Return raw (compressed) data.
  const occ::handle<NCollection_Buffer>& FaceData() const { return myFaceData; }

  //! Set raw (compressed) data.
  void SetFaceData(const occ::handle<NCollection_Buffer>& theBuffer) { myFaceData = theBuffer; }

  //! Return Array of mipmap sizes, including base level.
  const NCollection_Array1<int>& MipMaps() const { return myMipMaps; }

  //! Return Array of mipmap sizes, including base level.
  NCollection_Array1<int>& ChangeMipMaps() { return myMipMaps; }

  //! Return TRUE if complete mip map level set (up to 1x1 resolution).
  bool IsCompleteMipMapSet() const { return myIsCompleteMips; }

  //! Set if complete mip map level set (up to 1x1 resolution).
  void SetCompleteMipMapSet(bool theIsComplete) { myIsCompleteMips = theIsComplete; }

  //! Return surface length in bytes.
  size_t FaceBytes() const { return myFaceBytes; }

  //! Set surface length in bytes.
  void SetFaceBytes(size_t theSize) { myFaceBytes = theSize; }

  //! Return surface width.
  int SizeX() const { return mySizeX; }

  //! Return surface height.
  int SizeY() const { return mySizeY; }

  //! Set surface width x height.
  void SetSize(int theSizeX, int theSizeY)
  {
    mySizeX = theSizeX;
    mySizeY = theSizeY;
  }

  //! Return TRUE if image layout is top-down (always true).
  bool IsTopDown() const { return true; }

  //! Return number of faces in the file; should be 6 for cubemap.
  int NbFaces() const { return myNbFaces; }

  //! Set number of faces in the file.
  void SetNbFaces(int theSize) { myNbFaces = theSize; }

public:
  //! Empty constructor.
  Image_CompressedPixMap()
      : myFaceBytes(0),
        myNbFaces(0),
        mySizeX(0),
        mySizeY(0),
        myBaseFormat(Image_Format_UNKNOWN),
        myFormat(Image_CompressedFormat_UNKNOWN),
        myIsCompleteMips(false)
  {
  }

protected:
  NCollection_Array1<int> myMipMaps;   //!< Array of mipmap sizes, including base level
  occ::handle<NCollection_Buffer>           myFaceData;  //!< raw compressed data
  size_t                        myFaceBytes; //!< surface length in bytes
  int                     myNbFaces;   //!< number of faces in the file
  int                     mySizeX;     //!< surface width
  int                     mySizeY;     //!< surface height
  Image_Format                         myBaseFormat; //!< base (uncompressed) pixel format
  Image_CompressedFormat               myFormat;     //!< compressed format
  // clang-format off
  bool           myIsCompleteMips;    //!< flag indicating complete mip map level set (up to 1x1 resolution)
  // clang-format on
};

#endif // _Image_CompressedPixMap_HeaderFile
