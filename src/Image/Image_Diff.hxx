// Created on: 2012-07-10
// Created by: VRO
// Copyright (c) 2012-2014 OPEN CASCADE SAS
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

#ifndef _Image_Diff_H__
#define _Image_Diff_H__

#include <Image_PixMap.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Vector.hxx>

//! This class compares two images pixel-by-pixel.
//! It uses the following methods to ignore the difference between images:
//!  - Black/White comparison. It makes the images 2-colored before the comparison.
//!  - Equality with tolerance. Colors of two pixels are considered the same if the
//!    differnce of their color is less than a tolerance.
//!  - Border filter. The algorithm ignores alone independent pixels,
//!    which are different on both images, ignores the "border effect" -
//!    the difference caused by triangles located at angle about 0 or 90 degrees to the user.
//!
//! Border filter ignores a difference in implementation of
//! anti-aliasing and other effects on boundary of a shape.
//! The triangles of a boundary zone are usually located so that their normals point aside the user
//! (about 90 degree between the normal and the direction to the user's eye).
//! Deflection of the light for such a triangle depends on implementation of the video driver.
//! In order to skip this difference the following algorithm is used:
//!  a) "Different" pixels are groupped and checked on "one-pixel width line".
//!     indeed, the pixels may represent not a line, but any curve.
//!     But the width of this curve should be not more than a pixel.
//!     This group of pixels become a candidate to be ignored because of boundary effect.
//!  b) The group of pixels is checked on belonging to a "shape".
//!     Neighbour pixels are checked from the reference image.
//!     This test confirms a fact that the group of pixels belongs to a shape and
//!     represent a boundary of the shape.
//!     In this case the whole group of pixels is ignored (considered as same).
//!     Otherwise, the group of pixels may represent a geometrical curve in the viewer 3D
//!     and should be considered as "different".
//!
//! References:
//!     1. http://pdiff.sourceforge.net/ypg01.pdf
//!     2. http://pdiff.sourceforge.net/metric.html
//!     3. http://www.cs.ucf.edu/~sumant/publications/sig99.pdf
//!     4. http://www.worldscientific.com/worldscibooks/10.1142/2641#t=toc (there is a list of articles and books in PDF format)

class Image_Diff : public Standard_Transient
{

public:

  //! An empty constructor. Init() should be called for initialization.
  Standard_EXPORT Image_Diff();

  //! Desctructor.
  Standard_EXPORT virtual ~Image_Diff();

  //! Initialize algorithm by two images.
  //! @return false if images has different or unsupported pixel format.
  Standard_EXPORT Standard_Boolean Init (const Handle(Image_PixMap)& theImageRef,
                                         const Handle(Image_PixMap)& theImageNew,
                                         const Standard_Boolean      theToBlackWhite = Standard_False);

  //! Initialize algorithm by two images (will be loaded from files).
  //! @return false if images couldn't be opened or their format is unsupported.
  Standard_EXPORT Standard_Boolean Init (const TCollection_AsciiString& theImgPathRef,
                                         const TCollection_AsciiString& theImgPathNew,
                                         const Standard_Boolean         theToBlackWhite = Standard_False);

  //! Color tolerance for equality check. Should be within range 0..1:
  //! Corresponds to a difference between white and black colors (maximum difference).
  //! By default, the tolerance is equal to 0 thus equality check will return false for any different colors.
  Standard_EXPORT void SetColorTolerance (const Standard_Real theTolerance);

  //! Color tolerance for equality check.
  Standard_EXPORT Standard_Real ColorTolerance() const;

  //! Sets taking into account (ignoring) a "border effect" on comparison of images.
  //! The border effect is caused by a border of shaded shapes in the viewer 3d.
  //! Triangles of this area are located at about 0 or 90 degrees to the user.
  //! Therefore, they deflect light differently according to implementation of a video card driver.
  //! This flag allows to detect such a "border" area and skip it from comparison of images.
  //! Filter turned OFF by default.
  Standard_EXPORT void SetBorderFilterOn (const Standard_Boolean theToIgnore);

  //! Returns a flag of taking into account (ignoring) a border effect in comparison of images.
  Standard_EXPORT Standard_Boolean IsBorderFilterOn() const;

  //! Compares two images. It returns a number of different pixels (or groups of pixels).
  //! It returns -1 if algorithm not initialized before.
  Standard_EXPORT Standard_Integer Compare();

  //! Saves a difference between two images as white pixels on black backgroud.
  Standard_EXPORT Standard_Boolean SaveDiffImage (Image_PixMap& theDiffImage) const;

  //! Saves a difference between two images as white pixels on black backgroud.
  Standard_EXPORT Standard_Boolean SaveDiffImage (const TCollection_AsciiString& theDiffPath) const;

protected:

  //! Perform border filter algorithm.
  Standard_EXPORT Standard_Integer ignoreBorderEffect();

  //! Release dynamically allocated memory.
  Standard_EXPORT void releaseGroupsOfDiffPixels();

protected:

  typedef NCollection_List<TColStd_MapOfInteger* > ListOfMapOfInteger;

  Handle(Image_PixMap)              myImageRef;           //!< reference image to compare (from)
  Handle(Image_PixMap)              myImageNew;           //!< new       image to compare (to)
  Standard_Real                     myColorTolerance;     //!< tolerance for equality check (0..1, 0 - any not equal, 1 - opposite colors)
  Standard_Boolean                  myIsBorderFilterOn;   //!< perform algorithm with border effect filter
  ListOfMapOfInteger                myGroupsOfDiffPixels;
  NCollection_Vector<Standard_Size> myDiffPixels;         //!< different pixels (position packed into integer)
  TColStd_MapOfInteger              myLinearGroups;

public:

  DEFINE_STANDARD_RTTI(Image_Diff) // Type definition

};

DEFINE_STANDARD_HANDLE(Image_Diff, Standard_Transient)

#endif // _Image_Diff_H__
