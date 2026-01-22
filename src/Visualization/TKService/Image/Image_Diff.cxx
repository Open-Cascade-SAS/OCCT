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

#include <Image_Diff.hxx>

#include <Image_AlienPixMap.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <TColStd_PackedMapOfInteger.hxx>

#include <cstdlib>

IMPLEMENT_STANDARD_RTTIEXT(Image_Diff, Standard_Transient)

namespace
{

//! Number of neighbor pixels.
static const size_t Image_Diff_NbOfNeighborPixels = 8;

//! List of neighbor pixels (offsets).
static const int Image_Diff_NEIGHBOR_PIXELS[Image_Diff_NbOfNeighborPixels][2] =
  {{-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1}};

//! @return true if pixel is black
static bool isBlackPixel(const Image_PixMap& theData, size_t theY, size_t theX)
{
  switch (theData.Format())
  {
    case Image_Format_Gray:
    case Image_Format_Alpha: {
      return theData.Value<unsigned char>(theY, theX) == 0;
    }
    case Image_Format_RGB:
    case Image_Format_BGR:
    case Image_Format_RGB32:
    case Image_Format_BGR32:
    case Image_Format_RGBA:
    case Image_Format_BGRA: {
      const uint8_t* aColor = theData.RawValue(theY, theX);
      return aColor[0] == 0 && aColor[1] == 0 && aColor[2] == 0;
    }
    default: {
      const Quantity_ColorRGBA       aPixelRgba = theData.PixelColor((int)theY, (int)theX);
      const NCollection_Vec4<float>& aPixel     = aPixelRgba;
      return aPixel.r() == 0.0f && aPixel.g() == 0.0f && aPixel.b() == 0.0f;
    }
  }
}

} // namespace

//=================================================================================================

Image_Diff::Image_Diff()
    : myColorTolerance(0.0),
      myIsBorderFilterOn(false)
{
}

//=================================================================================================

Image_Diff::~Image_Diff()
{
  releaseGroupsOfDiffPixels();
}

//=================================================================================================

bool Image_Diff::Init(const occ::handle<Image_PixMap>& theImageRef,
                      const occ::handle<Image_PixMap>& theImageNew,
                      const bool                       theToBlackWhite)
{
  myImageRef.Nullify();
  myImageNew.Nullify();
  myDiffPixels.Clear();
  releaseGroupsOfDiffPixels();
  if (theImageRef.IsNull() || theImageNew.IsNull() || theImageRef->IsEmpty()
      || theImageNew->IsEmpty() || theImageRef->SizeX() != theImageNew->SizeX()
      || theImageRef->SizeY() != theImageNew->SizeY()
      || theImageRef->Format() != theImageNew->Format())
  {
    Message::SendFail("Error: Images have different format or dimensions");
    return false;
  }
  else if (theImageRef->SizeX() >= 0xFFFF || theImageRef->SizeY() >= 0xFFFF)
  {
    Message::SendFail("Error: Images are too large");
    return false;
  }

  myImageRef = theImageRef;
  myImageNew = theImageNew;
  if (theToBlackWhite)
  {
    Image_PixMap::ToBlackWhite(*myImageRef);
    Image_PixMap::ToBlackWhite(*myImageNew);
  }
  return true;
}

//=================================================================================================

bool Image_Diff::Init(const TCollection_AsciiString& theImgPathRef,
                      const TCollection_AsciiString& theImgPathNew,
                      const bool                     theToBlackWhite)
{
  occ::handle<Image_AlienPixMap> anImgRef = new Image_AlienPixMap();
  occ::handle<Image_AlienPixMap> anImgNew = new Image_AlienPixMap();
  if (!anImgRef->Load(theImgPathRef) || !anImgNew->Load(theImgPathNew))
  {
    Message::SendFail("Error: Failed to load image(s) file(s)");
    return false;
  }
  return Init(anImgRef, anImgNew, theToBlackWhite);
}

//=================================================================================================

int Image_Diff::Compare()
{
  // Number of different pixels (by color)
  int aNbDiffColors = 0;
  myDiffPixels.Clear();

  if (myImageRef.IsNull() || myImageNew.IsNull())
  {
    return -1;
  }

  // first check if images are exactly the same
  if (myImageNew->SizeBytes() == myImageRef->SizeBytes()
      && memcmp(myImageNew->Data(), myImageRef->Data(), myImageRef->SizeBytes()) == 0)
  {
    return 0;
  }

  switch (myImageRef->Format())
  {
    case Image_Format_Gray:
    case Image_Format_Alpha: {
      // Tolerance of comparison operation for color
      const int aDiffThreshold = int(255.0 * myColorTolerance);
      for (size_t aRow = 0; aRow < myImageRef->SizeY(); ++aRow)
      {
        for (size_t aCol = 0; aCol < myImageRef->SizeX(); ++aCol)
        {
          const int aDiff = int(myImageNew->Value<unsigned char>(aRow, aCol))
                            - int(myImageRef->Value<unsigned char>(aRow, aCol));
          if (std::abs(aDiff) > aDiffThreshold)
          {
            myDiffPixels.Append(PackXY((uint16_t)aCol, (uint16_t)aRow));
            ++aNbDiffColors;
          }
        }
      }
      break;
    }
    case Image_Format_RGB:
    case Image_Format_BGR:
    case Image_Format_RGB32:
    case Image_Format_BGR32:
    case Image_Format_RGBA:
    case Image_Format_BGRA: {
      // Tolerance of comparison operation for color
      // Maximum difference between colors (white - black) = 100%
      const int aDiffThreshold = int(255.0 * myColorTolerance);

      // we don't care about RGB/BGR/RGBA/BGRA/RGB32/BGR32 differences
      // because we just compute summ of r g b components
      for (size_t aRow = 0; aRow < myImageRef->SizeY(); ++aRow)
      {
        for (size_t aCol = 0; aCol < myImageRef->SizeX(); ++aCol)
        {
          // compute Chebyshev distance between two colors
          const uint8_t* aColorRef = myImageRef->RawValue(aRow, aCol);
          const uint8_t* aColorNew = myImageNew->RawValue(aRow, aCol);
          const int      aDiff     = NCollection_Vec3<int>(int(aColorRef[0]) - int(aColorNew[0]),
                                                  int(aColorRef[1]) - int(aColorNew[1]),
                                                  int(aColorRef[2]) - int(aColorNew[2]))
                              .cwiseAbs()
                              .maxComp();
          if (aDiff > aDiffThreshold)
          {
            myDiffPixels.Append(PackXY((uint16_t)aCol, (uint16_t)aRow));
            ++aNbDiffColors;
          }
        }
      }
      break;
    }
    default: {
      // Tolerance of comparison operation for color
      // Maximum difference between colors (white - black) = 100%
      const float aDiffThreshold = float(myColorTolerance);
      for (size_t aRow = 0; aRow < myImageRef->SizeY(); ++aRow)
      {
        for (size_t aCol = 0; aCol < myImageRef->SizeX(); ++aCol)
        {
          // compute Chebyshev distance between two colors
          const Quantity_ColorRGBA       aPixel1Rgba = myImageRef->PixelColor(int(aCol), int(aRow));
          const Quantity_ColorRGBA       aPixel2Rgba = myImageNew->PixelColor(int(aCol), int(aRow));
          const NCollection_Vec3<float>& aPixel1     = aPixel1Rgba.GetRGB();
          const NCollection_Vec3<float>& aPixel2     = aPixel2Rgba.GetRGB();
          const float                    aDiff       = (aPixel2 - aPixel1).cwiseAbs().maxComp();
          if (aDiff > aDiffThreshold)
          {
            myDiffPixels.Append(PackXY((uint16_t)aCol, (uint16_t)aRow));
            ++aNbDiffColors;
          }
        }
      }
      break;
    }
  }

  // take into account a border effect
  if (myIsBorderFilterOn && !myDiffPixels.IsEmpty())
  {
    aNbDiffColors = ignoreBorderEffect();
  }

  return aNbDiffColors;
}

//=================================================================================================

bool Image_Diff::SaveDiffImage(Image_PixMap& theDiffImage) const
{
  if (myImageRef.IsNull() || myImageNew.IsNull())
  {
    return false;
  }

  if (theDiffImage.IsEmpty() || theDiffImage.SizeX() != myImageRef->SizeX()
      || theDiffImage.SizeY() != myImageRef->SizeY())
  {
    if (!theDiffImage.InitTrash(Image_Format_Gray, myImageRef->SizeX(), myImageRef->SizeY()))
    {
      return false;
    }
  }

  const Quantity_ColorRGBA aWhiteRgba(1.0f, 1.0f, 1.0f, 1.0f);

  // initialize black image for dump
  memset(theDiffImage.ChangeData(), 0, theDiffImage.SizeBytes());
  if (myGroupsOfDiffPixels.IsEmpty())
  {
    if (myIsBorderFilterOn)
    {
      return true;
    }

    switch (theDiffImage.Format())
    {
      case Image_Format_Gray:
      case Image_Format_Alpha: {
        for (NCollection_Vector<int>::Iterator aPixelIter(myDiffPixels); aPixelIter.More();
             aPixelIter.Next())
        {
          theDiffImage.ChangeValue<unsigned char>(UnpackY(aPixelIter.Value()),
                                                  UnpackX(aPixelIter.Value())) = 255;
        }
        break;
      }
      case Image_Format_RGB:
      case Image_Format_BGR:
      case Image_Format_RGB32:
      case Image_Format_BGR32:
      case Image_Format_RGBA:
      case Image_Format_BGRA: {
        for (NCollection_Vector<int>::Iterator aPixelIter(myDiffPixels); aPixelIter.More();
             aPixelIter.Next())
        {
          memset(
            theDiffImage.ChangeRawValue(UnpackY(aPixelIter.Value()), UnpackX(aPixelIter.Value())),
            255,
            3);
        }
        break;
      }
      default: {
        for (NCollection_Vector<int>::Iterator aPixelIter(myDiffPixels); aPixelIter.More();
             aPixelIter.Next())
        {
          theDiffImage.SetPixelColor(UnpackX(aPixelIter.Value()),
                                     UnpackY(aPixelIter.Value()),
                                     aWhiteRgba);
        }
        break;
      }
    }
    return true;
  }

  int aGroupId = 1;
  for (NCollection_List<occ::handle<TColStd_HPackedMapOfInteger>>::Iterator aGrIter(
         myGroupsOfDiffPixels);
       aGrIter.More();
       aGrIter.Next(), ++aGroupId)
  {
    if (myLinearGroups.Contains(aGroupId))
    {
      continue; // skip linear groups
    }

    const occ::handle<TColStd_HPackedMapOfInteger>& aGroup = aGrIter.Value();
    switch (theDiffImage.Format())
    {
      case Image_Format_Gray:
      case Image_Format_Alpha: {
        for (TColStd_PackedMapOfInteger::Iterator aPixelIter(aGroup->Map()); aPixelIter.More();
             aPixelIter.Next())
        {
          int aDiffPixel(aPixelIter.Key());
          theDiffImage.ChangeValue<unsigned char>(UnpackY(aDiffPixel), UnpackX(aDiffPixel)) = 255;
        }
        break;
      }
      case Image_Format_RGB:
      case Image_Format_BGR:
      case Image_Format_RGB32:
      case Image_Format_BGR32:
      case Image_Format_RGBA:
      case Image_Format_BGRA: {
        for (TColStd_PackedMapOfInteger::Iterator aPixelIter(aGroup->Map()); aPixelIter.More();
             aPixelIter.Next())
        {
          int aDiffPixel(aPixelIter.Key());
          memset(theDiffImage.ChangeValue<uint8_t*>(UnpackY(aDiffPixel), UnpackX(aDiffPixel)),
                 255,
                 3);
        }
        break;
      }
      default: {
        for (TColStd_PackedMapOfInteger::Iterator aPixelIter(aGroup->Map()); aPixelIter.More();
             aPixelIter.Next())
        {
          int aDiffPixel(aPixelIter.Key());
          theDiffImage.SetPixelColor(UnpackX(aDiffPixel), UnpackY(aDiffPixel), aWhiteRgba);
        }
        break;
      }
    }
  }

  return true;
}

//=================================================================================================

bool Image_Diff::SaveDiffImage(const TCollection_AsciiString& theDiffPath) const
{
  if (myImageRef.IsNull() || myImageNew.IsNull() || theDiffPath.IsEmpty())
  {
    return false;
  }

  Image_AlienPixMap aDiff;
  if (!aDiff.InitTrash(Image_Format_Gray, myImageRef->SizeX(), myImageRef->SizeY())
      || !SaveDiffImage(aDiff))
  {
    return false;
  }

  // save image
  return aDiff.Save(theDiffPath);
}

//=================================================================================================

int Image_Diff::ignoreBorderEffect()
{
  if (myImageRef.IsNull() || myImageNew.IsNull())
  {
    return 0;
  }

  // allocate groups of different pixels
  releaseGroupsOfDiffPixels();

  // Find a different area (a set of close to each other pixels which colors differ in both images).
  // It filters alone pixels with different color.
  const int aLen1 = !myDiffPixels.IsEmpty() ? (myDiffPixels.Length() - 1) : 0;
  for (int aPixelId1 = 0; aPixelId1 < aLen1; ++aPixelId1)
  {
    int aValue1 = myDiffPixels.Value(aPixelId1);

    // Check other pixels in the list looking for a neighbour of this one
    for (int aPixelId2 = aPixelId1 + 1; aPixelId2 < myDiffPixels.Length(); ++aPixelId2)
    {
      int aValue2 = myDiffPixels.Value(aPixelId2);
      if (std::abs(int(UnpackX(aValue1)) - int(UnpackX(aValue2))) <= 1
          && std::abs(int(UnpackY(aValue1)) - int(UnpackY(aValue2))) <= 1)
      {
        // A neighbour is found. Create a new group and add both pixels.
        if (myGroupsOfDiffPixels.IsEmpty())
        {
          occ::handle<TColStd_HPackedMapOfInteger> aGroup = new TColStd_HPackedMapOfInteger();
          aGroup->ChangeMap().Add(aValue1);
          aGroup->ChangeMap().Add(aValue2);
          myGroupsOfDiffPixels.Append(aGroup);
        }
        else
        {
          // Find a group the pixels belong to.
          bool isFound = false;
          for (NCollection_List<occ::handle<TColStd_HPackedMapOfInteger>>::Iterator aGrIter(
                 myGroupsOfDiffPixels);
               aGrIter.More();
               aGrIter.Next())
          {
            const occ::handle<TColStd_HPackedMapOfInteger>& aGroup = aGrIter.ChangeValue();
            if (aGroup->Map().Contains(aValue1))
            {
              aGroup->ChangeMap().Add(aValue2);
              isFound = true;
              break;
            }
          }

          if (!isFound)
          {
            // Create a new group
            occ::handle<TColStd_HPackedMapOfInteger> aGroup = new TColStd_HPackedMapOfInteger();
            aGroup->ChangeMap().Add(aValue1);
            aGroup->ChangeMap().Add(aValue2);
            myGroupsOfDiffPixels.Append(aGroup);
          }
        }
      }
    }
  }

  // filter linear groups which represent border of a solid shape
  int aGroupId = 1;
  for (NCollection_List<occ::handle<TColStd_HPackedMapOfInteger>>::Iterator aGrIter(
         myGroupsOfDiffPixels);
       aGrIter.More();
       aGrIter.Next(), ++aGroupId)
  {
    int                                             aNeighboursNb = 0;
    bool                                            isLine        = true;
    const occ::handle<TColStd_HPackedMapOfInteger>& aGroup        = aGrIter.Value();
    if (aGroup->Map().IsEmpty())
    {
      continue;
    }

    int aDiffPixel = 0;
    for (TColStd_PackedMapOfInteger::Iterator aPixelIter(aGroup->Map()); aPixelIter.More();
         aPixelIter.Next())
    {
      aDiffPixel    = aPixelIter.Key();
      aNeighboursNb = 0;

      // pixels of a line have only 1 or 2 neighbour pixels inside the same group
      // check all neighbour pixels on presence in the group
      for (size_t aNgbrIter = 0; aNgbrIter < Image_Diff_NbOfNeighborPixels; ++aNgbrIter)
      {
        int anX = UnpackX(aDiffPixel) + Image_Diff_NEIGHBOR_PIXELS[aNgbrIter][0];
        int anY = UnpackY(aDiffPixel) + Image_Diff_NEIGHBOR_PIXELS[aNgbrIter][1];
        if (size_t(anX) < myImageRef->SizeX() // this unsigned math checks size_t(-1) at-once
            && size_t(anY) < myImageRef->SizeY()
            && aGroup->Map().Contains(PackXY((uint16_t)anX, (uint16_t)anY)))
        {
          ++aNeighboursNb;
        }
      }

      if (aNeighboursNb > 2)
      {
        isLine = false;
        break;
      }
    }

    if (isLine)
    {
      // Test a pixel of the linear group on belonging to a solid shape.
      // Consider neighbour pixels of the last pixel of the linear group in the 1st image.
      // If the pixel has greater than 1 not black neighbour pixel, it is a border of a shape.
      // Otherwise, it may be a topological edge, for example.
      aNeighboursNb = 0;
      for (size_t aNgbrIter = 0; aNgbrIter < Image_Diff_NbOfNeighborPixels; ++aNgbrIter)
      {
        int anX = UnpackX(aDiffPixel) + Image_Diff_NEIGHBOR_PIXELS[aNgbrIter][0];
        int anY = UnpackY(aDiffPixel) + Image_Diff_NEIGHBOR_PIXELS[aNgbrIter][1];
        if (size_t(anX) < myImageRef->SizeX() // this unsigned math checks size_t(-1) at-once
            && size_t(anY) < myImageRef->SizeY()
            && !isBlackPixel(*myImageRef, size_t(anY), size_t(anX)))
        {
          ++aNeighboursNb;
        }
      }

      if (aNeighboursNb > 1)
      {
        myLinearGroups.Add(aGroupId);
      }
    }
  }

  // number of different groups of pixels (except linear groups)
  int aNbDiffColors = 0;
  aGroupId          = 1;
  for (NCollection_List<occ::handle<TColStd_HPackedMapOfInteger>>::Iterator aGrIter(
         myGroupsOfDiffPixels);
       aGrIter.More();
       aGrIter.Next(), ++aGroupId)
  {
    if (!myLinearGroups.Contains(aGroupId))
    {
      ++aNbDiffColors;
    }
  }

  return aNbDiffColors;
}

//=================================================================================================

void Image_Diff::releaseGroupsOfDiffPixels()
{
  myGroupsOfDiffPixels.Clear();
  myLinearGroups.Clear();
}
