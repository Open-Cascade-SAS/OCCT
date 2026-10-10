// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in the OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <gtest/gtest.h>

#include <BRepAlgo_Image.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <NCollection_List.hxx>
#include <Standard_ConstructionError.hxx>
#include <TopoDS_Shape.hxx>

TEST(BRepAlgo_ImageTest, UnmappedShapeUsesExplicitFallback)
{
  BRepAlgo_Image     aImages;
  const TopoDS_Shape aShape = BRepPrimAPI_MakeBox(1.0, 2.0, 3.0).Shape();

  EXPECT_FALSE(aImages.HasImage(aShape));
  EXPECT_TRUE(aImages.FirstImage(aShape).IsSame(aShape));
  EXPECT_THROW(aImages.Image(aShape), Standard_ConstructionError);

  NCollection_List<TopoDS_Shape>        aFallback;
  const NCollection_List<TopoDS_Shape>& aImagesOrSelf = aImages.ImageOrSelf(aShape, aFallback);
  ASSERT_EQ(aImagesOrSelf.Extent(), 1);
  EXPECT_TRUE(aImagesOrSelf.First().IsSame(aShape));
  EXPECT_EQ(&aImagesOrSelf, &aFallback);
}

TEST(BRepAlgo_ImageTest, MappedShapeReturnsAllImages)
{
  BRepAlgo_Image     aImages;
  const TopoDS_Shape aShape       = BRepPrimAPI_MakeBox(1.0, 2.0, 3.0).Shape();
  const TopoDS_Shape aFirstImage  = BRepPrimAPI_MakeBox(2.0, 3.0, 4.0).Shape();
  const TopoDS_Shape aSecondImage = BRepPrimAPI_MakeBox(3.0, 4.0, 5.0).Shape();
  aImages.Bind(aShape, aFirstImage);
  aImages.Add(aShape, aSecondImage);

  const NCollection_List<TopoDS_Shape>& aImageList = aImages.Image(aShape);
  ASSERT_EQ(aImageList.Extent(), 2);
  EXPECT_TRUE(aImageList.First().IsSame(aFirstImage));
  EXPECT_TRUE(aImageList.Last().IsSame(aSecondImage));
  EXPECT_TRUE(aImages.FirstImage(aShape).IsSame(aFirstImage));

  NCollection_List<TopoDS_Shape>        aFallback;
  const NCollection_List<TopoDS_Shape>& aImagesOrSelf = aImages.ImageOrSelf(aShape, aFallback);
  EXPECT_EQ(&aImagesOrSelf, &aImageList);
  EXPECT_TRUE(aFallback.IsEmpty());
}
