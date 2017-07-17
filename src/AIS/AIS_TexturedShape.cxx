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
#include <Graphic3d_Texture2Dmanual.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Precision.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_Presentation.hxx>
#include <PrsMgr_ModedPresentation.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <Standard_ErrorHandler.hxx>
#include <StdPrs_BndBox.hxx>
#include <StdPrs_ShadedShape.hxx>
#include <StdPrs_ToolTriangulatedShape.hxx>
#include <StdPrs_WFShape.hxx>
#include <TopExp_Explorer.hxx>


IMPLEMENT_STANDARD_RTTIEXT(AIS_TexturedShape,AIS_Shape)

//=======================================================================
//function : AIS_TexturedShape
//purpose  :
//=======================================================================
AIS_TexturedShape::AIS_TexturedShape (const TopoDS_Shape& theShape)
: AIS_Shape         (theShape),
  myPredefTexture   (Graphic3d_NameOfTexture2D(0)),
  myToMapTexture    (Standard_True),
  myModulate        (Standard_True),
  myIsCustomOrigin  (Standard_True),
  myToRepeat        (Standard_True),
  myToScale         (Standard_True),
  myToShowTriangles (Standard_False)
{
}

//=======================================================================
//function : SetTextureFileName
//purpose  :
//=======================================================================
void AIS_TexturedShape::SetTextureFileName (const TCollection_AsciiString& theTextureFileName)
{
  myTexturePixMap.Nullify();

  if (theTextureFileName.IsIntegerValue())
  {
    const Standard_Integer aValue = theTextureFileName.IntegerValue();
    if (aValue < Graphic3d_Texture2D::NumberOfTextures()
     && aValue >= 0)
    {
      myPredefTexture = Graphic3d_NameOfTexture2D (aValue);
    }
    else
    {
      Message::DefaultMessenger()->Send (TCollection_AsciiString ("Error: texture with ID ") + theTextureFileName
                                       + " is undefined! Texture 0 will be used instead.", Message_Fail);
      myPredefTexture = Graphic3d_NameOfTexture2D (0);
    }
    myTextureFile = "";
  }
  else
  {
    myTextureFile   = theTextureFileName;
    myPredefTexture = Graphic3d_NOT_2D_UNKNOWN;
  }
}

//=======================================================================
//function : SetTexturePixMap
//purpose  :
//=======================================================================
void AIS_TexturedShape::SetTexturePixMap (const Handle(Image_PixMap)& theTexturePixMap)
{
  myTextureFile = "";
  myPredefTexture = Graphic3d_NOT_2D_UNKNOWN;
  myTexturePixMap = theTexturePixMap;
}

//=======================================================================
//function : SetTextureRepeat
//purpose  :
//=======================================================================

void AIS_TexturedShape::SetTextureRepeat (const Standard_Boolean theToRepeat,
                                          const Standard_Real    theURepeat,
                                          const Standard_Real    theVRepeat)
{
  myToRepeat = theToRepeat;
  myUVRepeat.SetCoord (theURepeat, theVRepeat);
}

//=======================================================================
//function : SetTextureMapOn
//purpose  :
//=======================================================================

void AIS_TexturedShape::SetTextureMapOn()
{
  myToMapTexture = Standard_True;
}

//=======================================================================
//function : SetTextureMapOff
//purpose  :
//=======================================================================

void AIS_TexturedShape::SetTextureMapOff()
{
  myToMapTexture = Standard_False;
}

//=======================================================================
//function : SetTextureOrigin
//purpose  :
//=======================================================================

void AIS_TexturedShape::SetTextureOrigin (const Standard_Boolean theToSetTextureOrigin,
                                          const Standard_Real    theUOrigin,
                                          const Standard_Real    theVOrigin)
{
  myIsCustomOrigin = theToSetTextureOrigin;
  myUVOrigin.SetCoord (theUOrigin, theVOrigin);
}

//=======================================================================
//function : SetTextureScale
//purpose  :
//=======================================================================

void AIS_TexturedShape::SetTextureScale (const Standard_Boolean theToSetTextureScale,
                                         const Standard_Real    theScaleU,
                                         const Standard_Real    theScaleV)
{
  myToScale = theToSetTextureScale;
  myUVScale.SetCoord (theScaleU, theScaleV);
}

//=======================================================================
//function : ShowTriangles
//purpose  :
//=======================================================================

void AIS_TexturedShape::ShowTriangles (const Standard_Boolean theToShowTriangles)
{
  myToShowTriangles = theToShowTriangles;
}

//=======================================================================
//function : EnableTextureModulate
//purpose  :
//=======================================================================

void AIS_TexturedShape::EnableTextureModulate()
{
  myModulate = Standard_True;
}

//=======================================================================
//function : DisableTextureModulate
//purpose  :
//=======================================================================

void AIS_TexturedShape::DisableTextureModulate()
{
  myModulate = Standard_False;
}

//=======================================================================
//function : SetColor
//purpose  :
//=======================================================================

void AIS_TexturedShape::SetColor (const Quantity_Color& theColor)
{
  AIS_Shape::SetColor (theColor);

  for (Standard_Integer aPrsIt = 1; aPrsIt <= Presentations().Length(); ++aPrsIt)
  {
    const PrsMgr_ModedPresentation& aPrsModed = Presentations().Value (aPrsIt);

    if (aPrsModed.Mode() != 3)
      continue;

    updateAttributes (aPrsModed.Presentation()->Presentation());
  }
}

//=======================================================================
//function : UnsetColor
//purpose  :
//=======================================================================

void AIS_TexturedShape::UnsetColor()
{
  AIS_Shape::UnsetColor();

  for (Standard_Integer aPrsIt = 1; aPrsIt <= Presentations().Length(); ++aPrsIt)
  {
    const PrsMgr_ModedPresentation& aPrsModed = Presentations().Value (aPrsIt);

    if (aPrsModed.Mode() != 3)
      continue;
    
    Handle(Prs3d_Presentation) aPrs = aPrsModed.Presentation()->Presentation();
    Handle(Graphic3d_Group)    aGroup = Prs3d_Root::CurrentGroup (aPrs);

    Handle(Graphic3d_AspectFillArea3d) anAreaAsp = myDrawer->Link()->ShadingAspect()->Aspect();
    Handle(Graphic3d_AspectLine3d)     aLineAsp  = myDrawer->Link()->LineAspect()->Aspect();
    Quantity_Color aColor;
    AIS_GraphicTool::GetInteriorColor (myDrawer->Link(), aColor);
    anAreaAsp->SetInteriorColor (aColor);
    // Check if aspect of given type is set for the group, 
    // because setting aspect for group with no already set aspect
    // can lead to loss of presentation data
    if (aGroup->IsGroupPrimitivesAspectSet (Graphic3d_ASPECT_FILL_AREA))
    {
      aGroup->SetGroupPrimitivesAspect (anAreaAsp);
    }
    if (aGroup->IsGroupPrimitivesAspectSet (Graphic3d_ASPECT_LINE))
    {
      aGroup->SetGroupPrimitivesAspect (aLineAsp);
    }

    updateAttributes (aPrs);
  }
}

//=======================================================================
//function : SetMaterial
//purpose  : 
//=======================================================================

void AIS_TexturedShape::SetMaterial (const Graphic3d_MaterialAspect& theMat)
{
  AIS_Shape::SetMaterial (theMat);

  for (Standard_Integer aPrsIt = 1; aPrsIt <= Presentations().Length(); ++aPrsIt)
  {
    const PrsMgr_ModedPresentation& aPrsModed = Presentations().Value (aPrsIt);
    
    if (aPrsModed.Mode() != 3)
      continue;
    
    updateAttributes (aPrsModed.Presentation()->Presentation());
  }
}

//=======================================================================
//function : UnsetMaterial
//purpose  : 
//=======================================================================
void AIS_TexturedShape::UnsetMaterial()
{
  AIS_Shape::UnsetMaterial();

  for (Standard_Integer aPrsIt = 1; aPrsIt <= Presentations().Length(); ++aPrsIt)
  {
    const PrsMgr_ModedPresentation& aPrsModed = Presentations().Value (aPrsIt);

    if (aPrsModed.Mode() != 3)
      continue;

    updateAttributes (aPrsModed.Presentation()->Presentation());
  }
}

//=======================================================================
//function : UpdateAttributes
//purpose  :
//=======================================================================

void AIS_TexturedShape::UpdateAttributes()
{
  updateAttributes (Presentation());
}

//=======================================================================
//function : updateAttributes
//purpose  :
//=======================================================================

void AIS_TexturedShape::updateAttributes (const Handle(Prs3d_Presentation)& thePrs)
{
  myAspect = new Graphic3d_AspectFillArea3d (*myDrawer->ShadingAspect()->Aspect());
  if (HasPolygonOffsets())
  {
    Standard_Integer aMode;
    Standard_ShortReal aFactor, aUnits;
    PolygonOffsets (aMode, aFactor, aUnits);
    myAspect->SetPolygonOffsets (aMode, aFactor, aUnits);
  }

  Standard_Boolean hasTexture = Standard_False;
  if (myToMapTexture)
  {
    TCollection_AsciiString aTextureDesc;
    if (!myTexturePixMap.IsNull())
    {
      myTexture = new Graphic3d_Texture2Dmanual (myTexturePixMap);
      aTextureDesc = " (custom image)";
    }
    else if (myPredefTexture != Graphic3d_NOT_2D_UNKNOWN)
    {
      myTexture = new Graphic3d_Texture2Dmanual (myPredefTexture);
      aTextureDesc = TCollection_AsciiString(" (predefined texture ") + myTexture->GetId() + ")";
    }
    else
    {
      myTexture = new Graphic3d_Texture2Dmanual (myTextureFile.ToCString());
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
      Message::DefaultMessenger()->Send (TCollection_AsciiString ("Error: texture can not be loaded ") + aTextureDesc, Message_Fail);
    }
  }

  myAspect->SetTextureMap (myTexture);
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
  for (Graphic3d_SequenceOfGroup::Iterator aGroupIt (thePrs->Groups()); aGroupIt.More(); aGroupIt.Next())
  {
    const Handle(Graphic3d_Group)& aGroup = aGroupIt.Value();
    if (!aGroup->IsGroupPrimitivesAspectSet (Graphic3d_ASPECT_FILL_AREA))
    {
      continue;
    }
    aGroup->SetGroupPrimitivesAspect (myAspect);
  }
}

//=======================================================================
//function : Compute
//purpose  :
//=======================================================================

void AIS_TexturedShape::Compute (const Handle(PrsMgr_PresentationManager3d)& /*thePrsMgr*/,
                                 const Handle(Prs3d_Presentation)&           thePrs,
                                 const Standard_Integer                      theMode)
{
  if (myshape.IsNull())
  {
    return;
  }

  if (myshape.ShapeType() > TopAbs_FACE && myshape.ShapeType() < TopAbs_SHAPE)
  {
    thePrs->SetVisual (Graphic3d_TOS_ALL);
    thePrs->SetDisplayPriority (myshape.ShapeType() + 2);
  }

  if (myshape.ShapeType() == TopAbs_COMPOUND)
  {
    TopExp_Explorer anExplor (myshape, TopAbs_VERTEX);
    if (!anExplor.More())
    {
      return;
    }
  }

  if (IsInfinite())
  {
    thePrs->SetInfiniteState (Standard_True);
  }

  switch (theMode)
  {
    case AIS_WireFrame:
    {
      StdPrs_ToolTriangulatedShape::ClearOnOwnDeflectionChange (myshape, myDrawer, Standard_True);
      StdPrs_WFShape::Add (thePrs, myshape, myDrawer);
      break;
    }
    case AIS_Shaded:
    case 3: // texture mapping on triangulation
    {
      StdPrs_ToolTriangulatedShape::ClearOnOwnDeflectionChange (myshape, myDrawer, Standard_True);
      if (myshape.ShapeType() > TopAbs_FACE)
      {
        StdPrs_WFShape::Add (thePrs, myshape, myDrawer);
        break;
      }

      if (IsInfinite())
      {
        StdPrs_WFShape::Add (thePrs, myshape, myDrawer);
        break;
      }
      try
      {
        OCC_CATCH_SIGNALS
        if (theMode == AIS_Shaded)
        {
          StdPrs_ShadedShape::Add (thePrs, myshape, myDrawer);
        }
        else
        {
          StdPrs_ShadedShape::Add (thePrs, myshape, myDrawer,
                                   Standard_True,
                                   myIsCustomOrigin ? myUVOrigin : gp_Pnt2d (0.0, 0.0),
                                   myUVRepeat,
                                   myToScale        ? myUVScale  : gp_Pnt2d (1.0, 1.0));
          updateAttributes (thePrs);
        }
      }
      catch (Standard_Failure)
      {
#ifdef OCCT_DEBUG
        std::cout << "AIS_TexturedShape::Compute() in ShadingMode failed \n";
#endif
        StdPrs_WFShape::Add (thePrs, myshape, myDrawer);
      }
      break;
    }
    case 2: // Bounding box
    {
      if (IsInfinite())
      {
        StdPrs_WFShape::Add (thePrs, myshape, myDrawer);
      }
      else
      {
        StdPrs_BndBox::Add (thePrs, BoundingBox(), myDrawer);
      }
      break;
    }
  }
}
