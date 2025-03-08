// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <RWMesh_ShapeIterator.hxx>

#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFPrs.hxx>

//=================================================================================================

RWMesh_ShapeIterator::RWMesh_ShapeIterator(const TDF_Label&       theLabel,
                                           const TopLoc_Location& theLocation,
                                           const TopAbs_ShapeEnum theShapeTypeFind,
                                           const TopAbs_ShapeEnum theShapeTypeAvoid,
                                           const Standard_Boolean theToMapColors,
                                           const XCAFPrs_Style&   theStyle)
    : myDefStyle(theStyle),
      myToMapColors(theToMapColors),
      myShapeType(theShapeTypeFind),
      myHasColor(false)
{
  TopoDS_Shape aShape;
  if (!XCAFDoc_ShapeTool::GetShape(theLabel, aShape) || aShape.IsNull())
  {
    return;
  }

  aShape.Location(theLocation, false);
  myIter.Init(aShape, myShapeType, theShapeTypeAvoid);

  if (theToMapColors)
  {
    dispatchStyles(theLabel, theLocation, theStyle);
    myStyles.Bind(aShape, theStyle);
  }
}

//=================================================================================================

RWMesh_ShapeIterator::RWMesh_ShapeIterator(const TopoDS_Shape&    theShape,
                                           const TopAbs_ShapeEnum theShapeTypeFind,
                                           const TopAbs_ShapeEnum theShapeTypeAvoid,
                                           const XCAFPrs_Style&   theStyle)
    : myDefStyle(theStyle),
      myToMapColors(true),
      myShapeType(theShapeTypeFind),
      myHasColor(false)
{
  if (theShape.IsNull())
  {
    return;
  }
  myIter.Init(theShape, myShapeType, theShapeTypeAvoid);
}

//=================================================================================================

void RWMesh_ShapeIterator::dispatchStyles(const TDF_Label&       theLabel,
                                          const TopLoc_Location& theLocation,
                                          const XCAFPrs_Style&   theStyle)
{
  TopLoc_Location                    aDummyLoc;
  XCAFPrs_IndexedDataMapOfShapeStyle aStyles;
  XCAFPrs::CollectStyleSettings(theLabel, aDummyLoc, aStyles);

  Standard_Integer aNbTypes[TopAbs_SHAPE] = {};
  for (Standard_Integer aTypeIter = myShapeType; aTypeIter >= TopAbs_COMPOUND; --aTypeIter)
  {
    if (aTypeIter != myShapeType && aNbTypes[aTypeIter] == 0)
    {
      continue;
    }

    for (XCAFPrs_IndexedDataMapOfShapeStyle::Iterator aStyleIter(aStyles); aStyleIter.More();
         aStyleIter.Next())
    {
      const TopoDS_Shape&    aKeyShape     = aStyleIter.Key();
      const TopAbs_ShapeEnum aKeyShapeType = aKeyShape.ShapeType();
      if (aTypeIter == myShapeType)
      {
        ++aNbTypes[aKeyShapeType];
      }
      if (aTypeIter != aKeyShapeType)
      {
        continue;
      }

      XCAFPrs_Style aCafStyle = aStyleIter.Value();
      if (!aCafStyle.IsSetColorCurv() && theStyle.IsSetColorCurv())
      {
        aCafStyle.SetColorCurv(theStyle.GetColorCurv());
      }
      if (!aCafStyle.IsSetColorSurf() && theStyle.IsSetColorSurf())
      {
        aCafStyle.SetColorSurf(theStyle.GetColorSurfRGBA());
      }
      if (aCafStyle.Material().IsNull() && !theStyle.Material().IsNull())
      {
        aCafStyle.SetMaterial(theStyle.Material());
      }

      TopoDS_Shape aKeyShapeLocated = aKeyShape.Located(theLocation);
      if (aKeyShapeType == myShapeType)
      {
        myStyles.Bind(aKeyShapeLocated, aCafStyle);
      }
      else
      {
        for (TopExp_Explorer aShapeIter(aKeyShapeLocated, myShapeType); aShapeIter.More();
             aShapeIter.Next())
        {
          if (!myStyles.IsBound(aShapeIter.Current()))
          {
            myStyles.Bind(aShapeIter.Current(), aCafStyle);
          }
        }
      }
    }
  }
}

//=================================================================================================

void RWMesh_ShapeIterator::initShape()
{
  myHasColor = false;
  if (!myToMapColors)
  {
    return;
  }
  if (!myStyles.Find(Shape(), myStyle))
  {
    myStyle = myDefStyle;
  }
  if (!myStyle.Material().IsNull())
  {
    myHasColor = true;
    myColor    = myStyle.Material()->BaseColor();
  }
  else if (myStyle.IsSetColorSurf())
  {
    myHasColor = true;
    myColor    = myStyle.GetColorSurfRGBA();
  }
  else if (myStyle.IsSetColorCurv())
  {
    myHasColor = true;
    myColor    = Quantity_ColorRGBA(myStyle.GetColorCurv());
  }
}