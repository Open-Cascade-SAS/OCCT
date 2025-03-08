// Created on: 2001-07-02
// Created by: Mathias BOSSHARD
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#include <AIS_TexturedShape.hxx>

#include <AIS_GraphicTool.hxx>
#include <AIS_InteractiveContext.hxx>
#include <BRepTools.hxx>
#include <gp_Pnt2d.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_StructureManager.hxx>
#include <Graphic3d_Texture2D.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <PrsMgr_PresentationManager.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Prs3d_BndBox.hxx>
#include <StdPrs_ShadedShape.hxx>
#include <StdPrs_ToolTriangulatedShape.hxx>
#include <StdPrs_WFShape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_TexturedShape, AIS_Shape)

//=================================================================================================

AIS_TexturedShape::AIS_TexturedShape(const TopoDS_Shape& theShape)
    : AIS_Shape(theShape),
      myPredefTexture(Graphic3d_NameOfTexture2D(0)),
      myToMapTexture(Standard_True),
      myModulate(Standard_True),
      myIsCustomOrigin(Standard_True),
      myToRepeat(Standard_True),
      myToScale(Standard_True),
      myToShowTriangles(Standard_False)
{
}

//=================================================================================================

void AIS_TexturedShape::SetTextureFileName(const TCollection_AsciiString& theTextureFileName)
{
  myTexturePixMap.Nullify();

  if (theTextureFileName.IsIntegerValue())
  {
    const Standard_Integer aValue = theTextureFileName.IntegerValue();
    if (aValue < Graphic3d_Texture2D::NumberOfTextures() && aValue >= 0)
    {
      myPredefTexture = Graphic3d_NameOfTexture2D(aValue);
    }
    else
    {
      Message::SendFail(TCollection_AsciiString("Error: texture with ID ") + theTextureFileName
                        + " is undefined. Texture 0 will be used instead.");
      myPredefTexture = Graphic3d_NameOfTexture2D(0);
    }
    myTextureFile = "";
  }
  else
  {
    myTextureFile   = theTextureFileName;
    myPredefTexture = Graphic3d_NOT_2D_UNKNOWN;
  }
}

//=================================================================================================

void AIS_TexturedShape::SetTexturePixMap(const Handle(Image_PixMap)& theTexturePixMap)
{
  myTextureFile   = "";
  myPredefTexture = Graphic3d_NOT_2D_UNKNOWN;
  myTexturePixMap = theTexturePixMap;
}

//=================================================================================================

void AIS_TexturedShape::SetTextureRepeat(const Standard_Boolean theToRepeat,
                                         const Standard_Real    theURepeat,
                                         const Standard_Real    theVRepeat)
{
  myToRepeat = theToRepeat;
  myUVRepeat.SetCoord(theURepeat, theVRepeat);
}

//=================================================================================================

void AIS_TexturedShape::SetTextureMapOn()
{
  myToMapTexture = Standard_True;
}

//=================================================================================================

void AIS_TexturedShape::SetTextureMapOff()
{
  myToMapTexture = Standard_False;
}

//=================================================================================================

void AIS_TexturedShape::SetTextureOrigin(const Standard_Boolean theToSetTextureOrigin,
                                         const Standard_Real    theUOrigin,
                                         const Standard_Real    theVOrigin)
{
  myIsCustomOrigin = theToSetTextureOrigin;
  myUVOrigin.SetCoord(theUOrigin, theVOrigin);
}

//=================================================================================================

void AIS_TexturedShape::SetTextureScale(const Standard_Boolean theToSetTextureScale,
                                        const Standard_Real    theScaleU,
                                        const Standard_Real    theScaleV)
{
  myToScale = theToSetTextureScale;
  myUVScale.SetCoord(theScaleU, theScaleV);
}

//=================================================================================================

void AIS_TexturedShape::ShowTriangles(const Standard_Boolean theToShowTriangles)
{
  myToShowTriangles = theToShowTriangles;
}

//=================================================================================================

void AIS_TexturedShape::EnableTextureModulate()
{
  myModulate = Standard_True;
}

//=================================================================================================

void AIS_TexturedShape::DisableTextureModulate()
{
  myModulate = Standard_False;
}

//=================================================================================================

void AIS_TexturedShape::SetColor(const Quantity_Color& theColor)
{
  AIS_Shape::SetColor(theColor);

  for (PrsMgr_Presentations::Iterator aPrsIter(Presentations()); aPrsIter.More(); aPrsIter.Next())
  {
    if (aPrsIter.Value()->Mode() == 3)
    {
      updateAttributes(aPrsIter.Value());
    }
  }
}

//=================================================================================================

void AIS_TexturedShape::UnsetColor()
{
  AIS_Shape::UnsetColor();
}

//=================================================================================================

void AIS_TexturedShape::SetMaterial(const Graphic3d_MaterialAspect& theMat)
{
  AIS_Shape::SetMaterial(theMat);
  for (PrsMgr_Presentations::Iterator aPrsIter(Presentations()); aPrsIter.More(); aPrsIter.Next())
  {
    if (aPrsIter.Value()->Mode() == 3)
    {
      updateAttributes(aPrsIter.Value());
    }
  }
}

//=================================================================================================

void AIS_TexturedShape::UnsetMaterial()
{
  AIS_Shape::UnsetMaterial();
  for (PrsMgr_Presentations::Iterator aPrsIter(Presentations()); aPrsIter.More(); aPrsIter.Next())
  {
    if (aPrsIter.Value()->Mode() == 3)
    {
      updateAttributes(aPrsIter.Value());
    }
  }
}

//=================================================================================================

void AIS_TexturedShape::UpdateAttributes()
{
  updateAttributes(Presentation());
}

//=================================================================================================

void AIS_TexturedShape::updateAttributes(const Handle(Prs3d_Presentation)& thePrs)
{
  myAspect = new Graphic3d_AspectFillArea3d(*myDrawer->ShadingAspect()->Aspect());
  if (HasPolygonOffsets())
  {
    Standard_Integer   aMode;
    Standard_ShortReal aFactor, aUnits;
    PolygonOffsets(aMode, aFactor, aUnits);
    myAspect->SetPolygonOffsets(aMode, aFactor, aUnits);
  }

  Standard_Boolean hasTexture = Standard_False;
  if (myToMapTexture)
  {
    TCollection_AsciiString aTextureDesc;
    if (!myTexturePixMap.IsNull())
    {
      myTexture    = new Graphic3d_Texture2D(myTexturePixMap);
      aTextureDesc = " (custom image)";
    }
    else if (myPredefTexture != Graphic3d_NOT_2D_UNKNOWN)
    {
      myTexture    = new Graphic3d_Texture2D(myPredefTexture);
      aTextureDesc = TCollection_AsciiString(" (predefined texture ") + myTexture->GetId() + ")";
    }
    else
    {
      myTexture    = new Graphic3d_Texture2D(myTextureFile.ToCString());
      aTextureDesc = TCollection_AsciiString(" (") + myTextureFile + ")";
    }

    if (myModulate)
    {
      myTexture->EnableModulate();
    }
    else
    {
      myTexture->DisableModulate();
    }

    if (myTexture->IsDone())
    {
      hasTexture = Standard_True;
    }
    else
    {
      Message::SendFail(TCollection_AsciiString("Error: texture can not be loaded ")
                        + aTextureDesc);
    }
  }

  myAspect->SetTextureMap(myTexture);
  if (hasTexture)
  {
    myAspect->SetTextureMapOn();
  }
  else
  {
    myAspect->SetTextureMapOff();
  }

  if (myToShowTriangles)
  {
    myAspect->SetEdgeOn();
  }
  else
  {
    myAspect->SetEdgeOff();
  }

  // Go through all groups to change fill aspect for all primitives
  for (Graphic3d_SequenceOfGroup::Iterator aGroupIt(thePrs->Groups()); aGroupIt.More();
       aGroupIt.Next())
  {
    const Handle(Graphic3d_Group)& aGroup = aGroupIt.Value();
    aGroup->SetGroupPrimitivesAspect(myAspect);
  }
}

//=================================================================================================

void AIS_TexturedShape::Compute(const Handle(PrsMgr_PresentationManager)&,
                                const Handle(Prs3d_Presentation)& thePrs,
                                const Standard_Integer            theMode)
{
  if (myshape.IsNull() || (myshape.ShapeType() == TopAbs_COMPOUND && myshape.NbChildren() == 0))
  {
    return;
  }

  if (myshape.ShapeType() >= TopAbs_WIRE && myshape.ShapeType() <= TopAbs_VERTEX)
  {
    // TopAbs_WIRE -> 7, TopAbs_EDGE -> 8, TopAbs_VERTEX -> 9 (Graphic3d_DisplayPriority_Highlight)
    const Standard_Integer aPrior = (Standard_Integer)Graphic3d_DisplayPriority_Above1
                                    + (Standard_Integer)myshape.ShapeType() - TopAbs_WIRE;
    thePrs->SetVisual(Graphic3d_TOS_ALL);
    thePrs->SetDisplayPriority((Graphic3d_DisplayPriority)aPrior);
  }

  if (IsInfinite())
  {
    thePrs->SetInfiniteState(Standard_True);
  }

  switch (theMode)
  {
    case AIS_WireFrame: {
      StdPrs_ToolTriangulatedShape::ClearOnOwnDeflectionChange(myshape, myDrawer, Standard_True);
      StdPrs_WFShape::Add(thePrs, myshape, myDrawer);
      break;
    }
    case AIS_Shaded:
    case 3: // texture mapping on triangulation
    {
      StdPrs_ToolTriangulatedShape::ClearOnOwnDeflectionChange(myshape, myDrawer, Standard_True);
      if (myshape.ShapeType() > TopAbs_FACE)
      {
        StdPrs_WFShape::Add(thePrs, myshape, myDrawer);
        break;
      }

      if (IsInfinite())
      {
        StdPrs_WFShape::Add(thePrs, myshape, myDrawer);
        break;
      }
      try
      {
        OCC_CATCH_SIGNALS
        if (theMode == AIS_Shaded)
        {
          StdPrs_ShadedShape::Add(thePrs, myshape, myDrawer);
        }
        else
        {
          StdPrs_ShadedShape::Add(thePrs,
                                  myshape,
                                  myDrawer,
                                  Standard_True,
                                  myIsCustomOrigin ? myUVOrigin : gp_Pnt2d(0.0, 0.0),
                                  myUVRepeat,
                                  myToScale ? myUVScale : gp_Pnt2d(1.0, 1.0));
          updateAttributes(thePrs);
        }
      }
      catch (Standard_Failure const&)
      {
#ifdef OCCT_DEBUG
        std::cout << "AIS_TexturedShape::Compute() in ShadingMode failed \n";
#endif
        StdPrs_WFShape::Add(thePrs, myshape, myDrawer);
      }
      break;
    }
    case 2: // Bounding box
    {
      if (IsInfinite())
      {
        StdPrs_WFShape::Add(thePrs, myshape, myDrawer);
      }
      else
      {
        Prs3d_BndBox::Add(thePrs, BoundingBox(), myDrawer);
      }
      break;
    }
  }
}
