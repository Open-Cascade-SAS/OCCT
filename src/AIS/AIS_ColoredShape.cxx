// Created on: 2014-04-24
// Created by: Kirill Gavrilov
// Copyright (c) 2014 OPEN CASCADE SAS
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

#include <AIS_ColoredShape.hxx>

#include <AIS_InteractiveContext.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <gp_Pnt2d.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_StructureManager.hxx>
#include <Graphic3d_Texture2Dmanual.hxx>
#include <Precision.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_Root.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <Standard_ErrorHandler.hxx>
#include <StdPrs_ShadedShape.hxx>
#include <StdPrs_ToolShadedShape.hxx>
#include <StdPrs_WFDeflectionShape.hxx>
#include <StdPrs_WFShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>

IMPLEMENT_STANDARD_HANDLE (AIS_ColoredDrawer, AIS_Drawer)
IMPLEMENT_STANDARD_RTTIEXT(AIS_ColoredDrawer, AIS_Drawer)

IMPLEMENT_STANDARD_HANDLE (AIS_ColoredShape, AIS_Shape)
IMPLEMENT_STANDARD_RTTIEXT(AIS_ColoredShape, AIS_Shape)

//=======================================================================
//function : AIS_ColoredShape
//purpose  :
//=======================================================================
AIS_ColoredShape::AIS_ColoredShape (const TopoDS_Shape& theShape)
: AIS_Shape (theShape)
{
  // disable dedicated line aspects
  myDrawer->SetFreeBoundaryAspect  (myDrawer->LineAspect());
  myDrawer->SetUnFreeBoundaryAspect(myDrawer->LineAspect());
  myDrawer->SetSeenLineAspect      (myDrawer->LineAspect());
}

//=======================================================================
//function : AIS_ColoredShape
//purpose  :
//=======================================================================
AIS_ColoredShape::AIS_ColoredShape (const Handle(AIS_Shape)& theShape)
: AIS_Shape (theShape->Shape())
{
  // disable dedicated line aspects
  myDrawer->SetFreeBoundaryAspect  (myDrawer->LineAspect());
  myDrawer->SetUnFreeBoundaryAspect(myDrawer->LineAspect());
  myDrawer->SetSeenLineAspect      (myDrawer->LineAspect());
  if (theShape->HasMaterial())
  {
    SetMaterial (theShape->Material());
  }
  if (theShape->HasColor())
  {
    SetColor (theShape->Color());
  }
  if (theShape->HasWidth())
  {
    SetWidth (theShape->Width());
  }
  if (theShape->IsTransparent())
  {
    SetTransparency (theShape->Transparency());
  }
}

//=======================================================================
//function : CustomAspects
//purpose  :
//=======================================================================
Handle(AIS_ColoredDrawer) AIS_ColoredShape::CustomAspects (const TopoDS_Shape& theShape)
{
  Handle(AIS_ColoredDrawer) aDrawer;
  myShapeColors.Find (theShape, aDrawer);
  if (aDrawer.IsNull())
  {
    aDrawer = new AIS_ColoredDrawer (myDrawer);
    myShapeColors.Bind (theShape, aDrawer);
    LoadRecomputable (AIS_WireFrame);
    LoadRecomputable (AIS_Shaded);
  }
  return aDrawer;
}

//=======================================================================
//function : ClearCustomAspects
//purpose  :
//=======================================================================
void AIS_ColoredShape::ClearCustomAspects()
{
  if (myShapeColors.IsEmpty())
  {
    return;
  }
  myShapeColors.Clear();
  LoadRecomputable (AIS_WireFrame);
  LoadRecomputable (AIS_Shaded);
}

//=======================================================================
//function : UnsetCustomAspects
//purpose  :
//=======================================================================
void AIS_ColoredShape::UnsetCustomAspects (const TopoDS_Shape&    theShape,
                                           const Standard_Boolean theToUnregister)
{
  if (!myShapeColors.IsBound (theShape))
  {
    return;
  }

  LoadRecomputable (AIS_WireFrame);
  LoadRecomputable (AIS_Shaded);
  if (theToUnregister)
  {
    myShapeColors.UnBind (theShape);
    return;
  }

  myShapeColors.ChangeFind (theShape) = new AIS_ColoredDrawer (myDrawer);
}

//=======================================================================
//function : SetCustomColor
//purpose  :
//=======================================================================
void AIS_ColoredShape::SetCustomColor (const TopoDS_Shape&   theShape,
                                       const Quantity_Color& theColor)
{
  if (theShape.IsNull())
  {
    return;
  }

  const Handle(AIS_ColoredDrawer)& aDrawer = CustomAspects (theShape);
  setColor (aDrawer, theColor);
  aDrawer->SetOwnColor (theColor);
  LoadRecomputable (AIS_WireFrame);
  LoadRecomputable (AIS_Shaded);
}

//=======================================================================
//function : SetCustomWidth
//purpose  :
//=======================================================================
void AIS_ColoredShape::SetCustomWidth (const TopoDS_Shape& theShape,
                                       const Standard_Real theLineWidth)
{
  if (theShape.IsNull())
  {
    return;
  }

  const Handle(AIS_ColoredDrawer)& aDrawer = CustomAspects (theShape);
  setWidth (CustomAspects (theShape), theLineWidth);
  aDrawer->SetOwnWidth (theLineWidth);
  LoadRecomputable (AIS_WireFrame);
  LoadRecomputable (AIS_Shaded);
}

//=======================================================================
//function : SetColor
//purpose  :
//=======================================================================

void AIS_ColoredShape::SetColor (const Quantity_Color&  theColor)
{
  setColor (myDrawer, theColor);
  myOwnColor  = theColor;
  hasOwnColor = Standard_True;
  LoadRecomputable (AIS_WireFrame);
  LoadRecomputable (AIS_Shaded);
  for (DataMapOfShapeColor::Iterator anIter (myShapeColors); anIter.More(); anIter.Next())
  {
    const Handle(AIS_ColoredDrawer)& aDrawer = anIter.Value();
    if (aDrawer->HasOwnColor())
    {
      continue;
    }

    if (aDrawer->HasShadingAspect())
    {
      aDrawer->ShadingAspect()->SetColor (theColor, myCurrentFacingModel);
    }
    if (aDrawer->HasLineAspect())
    {
      aDrawer->LineAspect()->SetColor (theColor);
    }
    if (aDrawer->HasWireAspect())
    {
      aDrawer->WireAspect()->SetColor (theColor);
    }
  }
}

//=======================================================================
//function : SetWidth
//purpose  :
//=======================================================================

void AIS_ColoredShape::SetWidth (const Standard_Real    theLineWidth)
{
  setWidth (myDrawer, theLineWidth);
  myOwnWidth = theLineWidth;
  LoadRecomputable (AIS_WireFrame);
  LoadRecomputable (AIS_Shaded);
  for (DataMapOfShapeColor::Iterator anIter (myShapeColors); anIter.More(); anIter.Next())
  {
    const Handle(AIS_ColoredDrawer)& aDrawer = anIter.Value();
    if (aDrawer->HasOwnWidth())
    {
      continue;
    }

    if (aDrawer->HasLineAspect())
    {
      aDrawer->LineAspect()->SetWidth (theLineWidth);
    }
    if (aDrawer->HasWireAspect())
    {
      aDrawer->WireAspect()->SetWidth (theLineWidth);
    }
  }
}

//=======================================================================
//function : SetTransparency
//purpose  :
//=======================================================================

void AIS_ColoredShape::SetTransparency (const Standard_Real theValue)
{
  setTransparency (myDrawer, theValue);
  myTransparency = theValue;
  LoadRecomputable (AIS_WireFrame);
  LoadRecomputable (AIS_Shaded);
  for (DataMapOfShapeColor::Iterator anIter (myShapeColors); anIter.More(); anIter.Next())
  {
    const Handle(AIS_Drawer)& aDrawer = anIter.Value();
    if (aDrawer->HasShadingAspect())
    {
      aDrawer->ShadingAspect()->SetTransparency (theValue, myCurrentFacingModel);
    }
  }
}

//=======================================================================
//function : Compute
//purpose  :
//=======================================================================
void AIS_ColoredShape::Compute (const Handle(PrsMgr_PresentationManager3d)& ,
                                const Handle(Prs3d_Presentation)&           thePrs,
                                const Standard_Integer                      theMode)
{
  thePrs->Clear();
  if (IsInfinite())
  {
    thePrs->SetInfiniteState (Standard_True);
  }

  const Standard_Boolean isClosed = StdPrs_ToolShadedShape::IsClosed (myshape);
  if (theMode == AIS_Shaded)
  {
    // compute mesh for entire shape beforehand to ensure consistency and optimizations (parallelization)
    Standard_Real anAnglePrev, anAngleNew, aCoeffPrev, aCoeffNew;
    Standard_Boolean isOwnDeviationAngle       = OwnDeviationAngle      (anAngleNew, anAnglePrev);
    Standard_Boolean isOwnDeviationCoefficient = OwnDeviationCoefficient(aCoeffNew,  aCoeffPrev);
    if ((isOwnDeviationAngle       && Abs (anAngleNew - anAnglePrev) > Precision::Angular())
     || (isOwnDeviationCoefficient && Abs (aCoeffNew  - aCoeffPrev)  > Precision::Confusion()))
    {
      BRepTools::Clean (myshape);
    }
    StdPrs_ShadedShape::Tessellate (myshape, myDrawer);
  }

  // 1) myShapeColors + myshape --> array[TopAbs_ShapeEnum] of map of color-to-compound
  DataMapOfShapeCompd aTypeKeyshapeDrawshapeArray[(size_t )TopAbs_SHAPE];
  dispatchColors (myshape, myShapeColors, aTypeKeyshapeDrawshapeArray);

  // 2) finally add appropriate presentations (1 color -- 1 compound) according to theMode
  Handle(AIS_ColoredDrawer) aCustomDrawer;
  for (size_t aShType = 0; aShType < (size_t )TopAbs_SHAPE; ++aShType)
  {
    DataMapOfShapeCompd& aKeyshapeDrawshapeMap = aTypeKeyshapeDrawshapeArray[aShType];
    for (DataMapOfShapeCompd::Iterator aMapIter (aKeyshapeDrawshapeMap);
         aMapIter.More(); aMapIter.Next())
    {
      const TopoDS_Shape&    aShapeKey  = aMapIter.Key();   // key shape with detailed color or a base shape
      const TopoDS_Compound& aShapeDraw = aMapIter.Value(); // compound of subshapes with <aShType> type
      Handle(AIS_Drawer)     aDrawer;
      if (myShapeColors.Find (aShapeKey, aCustomDrawer))
      {
        aDrawer = aCustomDrawer;
        if (aCustomDrawer->IsHidden())
        {
          continue;
        }
      }
      else
      {
        aDrawer = myDrawer;
      }

      // Draw each kind of subshapes and personal-colored shapes in a separate group
      // since it's necessary to set transparency/material for all subshapes
      // without affecting their unique colors
      Handle(Graphic3d_Group) aCurrGroup = Prs3d_Root::NewGroup (thePrs);
      switch (theMode)
      {
        default:
        case AIS_Shaded:
        {
          if ((Standard_Integer )aShapeDraw.ShapeType() <= TopAbs_FACE
           && !IsInfinite())
          {
            StdPrs_ShadedShape::Add (thePrs, aShapeDraw, aDrawer);

            aDrawer->SetShadingAspectGlobal (Standard_False);
            Handle(Graphic3d_AspectFillArea3d) anAsp = aDrawer->ShadingAspect()->Aspect();
            isClosed ? anAsp->SuppressBackFace() : anAsp->AllowBackFace();
            aCurrGroup->SetGroupPrimitivesAspect (anAsp);
            break;
          }
          // compute wire-frame otherwise
        }
        case AIS_WireFrame:
        {
          StdPrs_WFDeflectionShape::Add (thePrs, aShapeDraw, aDrawer);
          break;
        }
      }
    }
  }
}

//=======================================================================
//function : dispatchColors
//purpose  :
//=======================================================================
Standard_Boolean AIS_ColoredShape::dispatchColors (const TopoDS_Shape&        theBaseKey,
                                                   const TopoDS_Shape&        theSubshapeToParse,
                                                   const DataMapOfShapeShape& theSubshapeKeyshapeMap,
                                                   const TopAbs_ShapeEnum     theParentType,
                                                   DataMapOfShapeCompd*       theTypeKeyshapeDrawshapeArray)
{
  TopAbs_ShapeEnum aShType = theSubshapeToParse.ShapeType();
  if (aShType == TopAbs_SHAPE)
  {
    return Standard_False;
  }

  // check own setting of current shape
  TopoDS_Shape     aKeyShape   = theBaseKey;
  Standard_Boolean isOverriden = theSubshapeKeyshapeMap.Find (theSubshapeToParse, aKeyShape);

  // iterate on sub-shapes
  BRep_Builder aBBuilder;
  TopoDS_Shape aShapeCopy = theSubshapeToParse.EmptyCopied();
  aShapeCopy.Closed (theSubshapeToParse.Closed());
  Standard_Boolean isSubOverride = Standard_False;
  Standard_Integer nbDef = 0;
  for (TopoDS_Iterator it (theSubshapeToParse); it.More(); it.Next())
  {
    if (dispatchColors (theBaseKey, it.Value(),
                        theSubshapeKeyshapeMap, aShType,
                        theTypeKeyshapeDrawshapeArray))
    {
      isSubOverride = Standard_True;
    }
    else
    {
      aBBuilder.Add (aShapeCopy, it.Value());
      ++nbDef;
    }
  }
  if (aShType == TopAbs_FACE || !isSubOverride)
  {
    aShapeCopy = theSubshapeToParse;
  }
  else if (nbDef == 0)
  {
    return isOverriden || isSubOverride; // empty compound
  }

  // if any of styles is overridden regarding to default one, add rest to map
  if (isOverriden
  || (isSubOverride && theParentType != TopAbs_WIRE  // avoid drawing edges when vertex color is overridden
                    && theParentType != TopAbs_FACE) // avoid drawing edges of the same color as face
  || (theParentType == TopAbs_SHAPE && !(isOverriden || isSubOverride))) // bind original shape to default color
  {
    TopoDS_Compound aCompound;
    DataMapOfShapeCompd& aKeyshapeDrawshapeMap = theTypeKeyshapeDrawshapeArray[(size_t )aShType];
    if (!aKeyshapeDrawshapeMap.FindFromKey (aKeyShape, aCompound))
    {
      aBBuilder.MakeCompound (aCompound);
      aKeyshapeDrawshapeMap.Add (aKeyShape, aCompound);
    }
    aBBuilder.Add (aCompound, aShapeCopy);
  }
  return isOverriden || isSubOverride;
}

//=======================================================================
//function : dispatchColors
//purpose  :
//=======================================================================
void AIS_ColoredShape::dispatchColors (const TopoDS_Shape&        theBaseShape,
                                       const DataMapOfShapeColor& theKeyshapeColorMap,
                                       DataMapOfShapeCompd*       theTypeKeyshapeDrawshapeArray)
{
  // Extract <theShapeColors> map (KeyshapeColored -> Color)
  // to subshapes map (Subshape -> KeyshapeColored).
  // This needed when colored shape is not part of <theBaseShape>
  // (but subshapes are) and actually container for subshapes.
  DataMapOfShapeShape aSubshapeKeyshapeMap;
  for (DataMapOfShapeColor::Iterator anIt (theKeyshapeColorMap);
       anIt.More(); anIt.Next())
  {
    const TopoDS_Shape&   aSh = anIt.Key();
    TopAbs_ShapeEnum    aType = aSh.ShapeType();
    TopAbs_ShapeEnum aSubType = (aType == TopAbs_SOLID || aType == TopAbs_SHELL)
                              ? TopAbs_FACE
                              : (aType == TopAbs_WIRE ? TopAbs_EDGE : TopAbs_SHAPE);
    switch (aSubType)
    {
      case TopAbs_SHAPE:
      {
        aSubshapeKeyshapeMap.Bind (aSh, aSh);
        break;
      }
      default:
      {
        for (TopExp_Explorer anExp (aSh, aSubType); anExp.More(); anExp.Next())
        {
          if (!aSubshapeKeyshapeMap.IsBound (anExp.Current()))
          {
            aSubshapeKeyshapeMap.Bind (anExp.Current(), aSh);
          }
        }
      }
    }
  }

  // Fill the array of maps per shape type
  dispatchColors (theBaseShape, theBaseShape,
                  aSubshapeKeyshapeMap, TopAbs_SHAPE,
                  theTypeKeyshapeDrawshapeArray);
}
