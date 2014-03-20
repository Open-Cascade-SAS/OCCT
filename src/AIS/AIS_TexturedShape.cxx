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

#include <AIS_Drawer.hxx>
#include <AIS_InteractiveContext.hxx>
#include <BRepTools.hxx>
#include <gp_Pnt2d.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_StructureManager.hxx>
#include <Graphic3d_Texture2Dmanual.hxx>
#include <Precision.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <Standard_ErrorHandler.hxx>
#include <StdPrs_ShadedShape.hxx>
#include <StdPrs_WFDeflectionShape.hxx>
#include <StdPrs_WFShape.hxx>
#include <TopExp_Explorer.hxx>

IMPLEMENT_STANDARD_HANDLE (AIS_TexturedShape, AIS_Shape)
IMPLEMENT_STANDARD_RTTIEXT(AIS_TexturedShape, AIS_Shape)

//=======================================================================
//function : AIS_TexturedShape
//purpose  :
//=======================================================================
AIS_TexturedShape::AIS_TexturedShape (const TopoDS_Shape& theShape)
: AIS_Shape         (theShape),
  myPredefTexture   (Graphic3d_NameOfTexture2D(0)),
  myToMapTexture    (Standard_True),
  myModulate        (Standard_True),
  myUVOrigin        (0.0, 0.0),
  myIsCustomOrigin  (Standard_True),
  myUVRepeat        (1.0, 1.0),
  myToRepeat        (Standard_True),
  myUVScale         (1.0, 1.0),
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
      std::cout << "Texture " << theTextureFileName << " doesn't exist\n";
      std::cout << "Using Texture 0 instead ...\n";
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
//function : UpdateAttributes
//purpose  :
//=======================================================================

void AIS_TexturedShape::UpdateAttributes()
{
  Prs3d_ShadingAspect aDummy;
  myAspect = aDummy.Aspect();
  Handle(Prs3d_Presentation) aPrs = Presentation();
  if (!myToMapTexture)
  {
    myAspect->SetTextureMapOff();
    return;
  }

  if (!myTexturePixMap.IsNull())
  {
    myTexture = new Graphic3d_Texture2Dmanual (myTexturePixMap);
  }
  else if (myPredefTexture != Graphic3d_NOT_2D_UNKNOWN)
  {
    myTexture = new Graphic3d_Texture2Dmanual (myPredefTexture);
  }
  else
  {
    myTexture = new Graphic3d_Texture2Dmanual (myTextureFile.ToCString());
  }

  myAspect->SetTextureMapOn();

  myAspect->SetTextureMap (myTexture);
  if (!myTexture->IsDone())
  {
    std::cout << "An error occurred while building texture \n";
    return;
  }

  if (myModulate)
    myTexture->EnableModulate();
  else
    myTexture->DisableModulate();

  if (myToShowTriangles)
    myAspect->SetEdgeOn();
  else
    myAspect->SetEdgeOff();

  Prs3d_Root::CurrentGroup (aPrs)->SetGroupPrimitivesAspect (myAspect);
}

//=======================================================================
//function : Compute
//purpose  :
//=======================================================================

void AIS_TexturedShape::Compute (const Handle(PrsMgr_PresentationManager3d)& /*thePrsMgr*/,
                                 const Handle(Prs3d_Presentation)&           thePrs,
                                 const Standard_Integer                      theMode)
{
  thePrs->Clear();

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
      StdPrs_WFDeflectionShape::Add (thePrs, myshape, myDrawer);
      break;
    }
    case AIS_Shaded:
    {
      Standard_Real prevangle;
      Standard_Real newangle;
      Standard_Real prevcoeff;
      Standard_Real newcoeff;

      Standard_Boolean isOwnDeviationAngle = OwnDeviationAngle(newangle,prevangle);
      Standard_Boolean isOwnDeviationCoefficient = OwnDeviationCoefficient(newcoeff,prevcoeff);
      if (((Abs (newangle - prevangle) > Precision::Angular()) && isOwnDeviationAngle) ||
          ((Abs (newcoeff - prevcoeff) > Precision::Confusion()) && isOwnDeviationCoefficient)) {
        BRepTools::Clean (myshape);
      }
      if (myshape.ShapeType() > TopAbs_FACE)
      {
        StdPrs_WFDeflectionShape::Add (thePrs, myshape, myDrawer);
        break;
      }
      myDrawer->SetShadingAspectGlobal (Standard_False);
      if (IsInfinite())
      {
        StdPrs_WFDeflectionShape::Add (thePrs, myshape, myDrawer);
        break;
      }
      try
      {
        OCC_CATCH_SIGNALS
        StdPrs_ShadedShape::Add (thePrs, myshape, myDrawer);
      }
      catch (Standard_Failure)
      {
        std::cout << "AIS_TexturedShape::Compute() in ShadingMode failed \n";
        StdPrs_WFShape::Add (thePrs, myshape, myDrawer);
      }
      break;
    }
    case 2: // Bounding box
    {
      if (IsInfinite())
      {
        StdPrs_WFDeflectionShape::Add (thePrs, myshape, myDrawer);
      }
      else
      {
        AIS_Shape::DisplayBox (thePrs, BoundingBox(), myDrawer);
      }
      break;
    }
    case 3: // texture mapping on triangulation
    {
      BRepTools::Clean  (myshape);
      BRepTools::Update (myshape);

      {
        Handle(Prs3d_ShadingAspect) aPrs3d_ShadingAspect = new Prs3d_ShadingAspect();
        myAspect = aPrs3d_ShadingAspect->Aspect();

        // Issue 23115: copy polygon offset settings passed through myDrawer
        if (HasPolygonOffsets())
        {
          Standard_Integer aMode;
          Standard_ShortReal aFactor, aUnits;
          PolygonOffsets(aMode, aFactor, aUnits);
          myAspect->SetPolygonOffsets(aMode, aFactor, aUnits);
        }
      }
      if (!myToMapTexture)
      {
        myAspect->SetTextureMapOff();
        return;
      }
      myAspect->SetTextureMapOn();

      if (!myTexturePixMap.IsNull())
      {
        myTexture = new Graphic3d_Texture2Dmanual (myTexturePixMap);
      }
      else if (myPredefTexture != Graphic3d_NOT_2D_UNKNOWN)
      {
        myTexture = new Graphic3d_Texture2Dmanual (myPredefTexture);
      }
      else
      {
        myTexture = new Graphic3d_Texture2Dmanual (myTextureFile.ToCString());
      }

      if (!myTexture->IsDone())
      {
        std::cout << "An error occurred while building texture \n";
        return;
      }

      if (myModulate)
        myTexture->EnableModulate();
      else
        myTexture->DisableModulate();

      myAspect->SetTextureMap (myTexture);
      if (myToShowTriangles)
        myAspect->SetEdgeOn();
      else
        myAspect->SetEdgeOff();

      if (myToRepeat)
        myTexture->EnableRepeat();
      else
        myTexture->DisableRepeat();

      try
      {
        OCC_CATCH_SIGNALS
        StdPrs_ShadedShape::Add (thePrs, myshape, myDrawer,
                                 Standard_True,
                                 myIsCustomOrigin ? myUVOrigin : gp_Pnt2d (0.0, 0.0),
                                 myUVRepeat,
                                 myToScale        ? myUVScale  : gp_Pnt2d (1.0, 1.0));
        // within primitive arrays - object should be in one group of primitives
        Prs3d_Root::CurrentGroup (thePrs)->SetGroupPrimitivesAspect (myAspect);
      }
      catch (Standard_Failure)
      {
        std::cout << "AIS_TexturedShape::Compute() in ShadingMode failed \n";
        StdPrs_WFShape::Add (thePrs, myshape, myDrawer);
      }
      break;
    }
  }
}
