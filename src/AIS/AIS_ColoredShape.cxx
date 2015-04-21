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
#include <Prs3d.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_Root.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <Standard_ErrorHandler.hxx>
#include <StdPrs_ShadedShape.hxx>
#include <StdPrs_ToolTriangulatedShape.hxx>
#include <StdPrs_WFShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>




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

    if (aDrawer->HasOwnShadingAspect())
    {
      aDrawer->ShadingAspect()->SetColor (theColor, myCurrentFacingModel);
    }
    if (aDrawer->HasOwnLineAspect())
    {
      aDrawer->LineAspect()->SetColor (theColor);
    }
    if (aDrawer->HasOwnWireAspect())
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

    if (aDrawer->HasOwnLineAspect())
    {
      aDrawer->LineAspect()->SetWidth (theLineWidth);
    }
    if (aDrawer->HasOwnWireAspect())
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
    const Handle(Prs3d_Drawer)& aDrawer = anIter.Value();
    if (aDrawer->HasOwnShadingAspect())
    {
      aDrawer->ShadingAspect()->SetTransparency (theValue, myCurrentFacingModel);
    }
  }
}

//=======================================================================
//function : SetMaterial
//purpose  :
//=======================================================================

void AIS_ColoredShape::SetMaterial (const Graphic3d_MaterialAspect& theMaterial)
{
  setMaterial (myDrawer, theMaterial, HasColor(), IsTransparent());
  //myOwnMaterial = theMaterial;
  hasOwnMaterial = Standard_True;
  LoadRecomputable (AIS_Shaded);
  for (DataMapOfShapeColor::Iterator anIter (myShapeColors); anIter.More(); anIter.Next())
  {
    const Handle(AIS_ColoredDrawer)& aDrawer = anIter.Value();
    //if (aDrawer->HasOwnMaterial()) continue;
    if (aDrawer->HasOwnShadingAspect())
    {
      setMaterial (aDrawer, theMaterial, aDrawer->HasOwnColor(), Standard_False); // aDrawer->IsTransparent()
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

  if (theMode == AIS_Shaded)
  {
    if (myDrawer->IsAutoTriangulation())
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
      // After this call if type of deflection is relative
      // computed deflection coefficient is stored as absolute.
      Standard_Boolean wasRecomputed = StdPrs_ToolTriangulatedShape::Tessellate (myshape, myDrawer);

      // Set to update wireframe presentation on triangulation.
      if (myDrawer->IsoOnTriangulation() && wasRecomputed)
      {
        SetToUpdate (AIS_WireFrame);
      }
    }
  }
  else // WireFrame mode
  {
    // After this call if type of deflection is relative
    // computed deflection coefficient is stored as absolute.
    Prs3d::GetDeflection (myshape, myDrawer);
  }

  TopoDS_Compound anOpened, aClosed;
  BRep_Builder aBuilder;
  aBuilder.MakeCompound (aClosed);
  aBuilder.MakeCompound (anOpened);
  if (theMode == AIS_Shaded && myshape.ShapeType() <= TopAbs_SOLID)
  {
    StdPrs_ShadedShape::ExploreSolids (myshape, aBuilder, aClosed, anOpened, Standard_False);
  }
  else
  {
    aBuilder.Add (anOpened, myshape);
  }

  // myShapeColors + anOpened --> array[TopAbs_ShapeEnum] of map of color-to-compound
  DataMapOfShapeCompd aDispatchedOpened [(size_t)TopAbs_SHAPE];
  dispatchColors (anOpened, myShapeColors, aDispatchedOpened);
  addShapesWithCustomProps (thePrs, aDispatchedOpened, theMode, StdPrs_Volume_Opened);

  if (theMode == AIS_Shaded)
  {
    if (isShapeEntirelyVisible())
    {
      // myShapeColors + aClosed --> array[TopAbs_ShapeEnum] of map of color-to-compound
      DataMapOfShapeCompd aDispatchedClosed [(size_t)TopAbs_SHAPE];
      dispatchColors (aClosed, myShapeColors, aDispatchedClosed);
      addShapesWithCustomProps (thePrs, aDispatchedClosed, theMode, StdPrs_Volume_Closed);
    }
    else
    {
      for (TopoDS_Iterator aSolidIter (aClosed); aSolidIter.More(); aSolidIter.Next())
      {
        DataMapOfShapeCompd aDispatchedClosed [(size_t)TopAbs_SHAPE];
        dispatchColors (aSolidIter.Value(), myShapeColors, aDispatchedClosed);
        addShapesWithCustomProps (thePrs, aDispatchedClosed, theMode,
                                  isShapeEntirelyVisible (aDispatchedClosed) ? StdPrs_Volume_Closed : StdPrs_Volume_Opened);
      }
    }
  }
}

//=======================================================================
//function : addShapesWithCustomProps
//purpose  :
//=======================================================================
void AIS_ColoredShape::addShapesWithCustomProps (const Handle(Prs3d_Presentation)& thePrs,
                                                 DataMapOfShapeCompd*              theDispatched,
                                                 const Standard_Integer            theMode,
                                                 const StdPrs_Volume               theVolume)
{
  Handle(AIS_ColoredDrawer) aCustomDrawer;
  for (size_t aShType = 0; aShType < (size_t )TopAbs_SHAPE; ++aShType)
  {
    DataMapOfShapeCompd& aKeyshapeDrawshapeMap = theDispatched[aShType];
    for (DataMapOfShapeCompd::Iterator aMapIter (aKeyshapeDrawshapeMap);
         aMapIter.More(); aMapIter.Next())
    {
      const TopoDS_Shape&    aShapeKey  = aMapIter.Key();   // key shape with detailed color or a base shape
      const TopoDS_Compound& aShapeDraw = aMapIter.Value(); // compound of subshapes with <aShType> type
      Handle(Prs3d_Drawer) aDrawer;
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

      // It is supposed that absolute deflection contains previously computed relative deflection
      // (if deflection type is relative).
      // In case of CustomDrawer it is taken from Link().
      Aspect_TypeOfDeflection aPrevType = aDrawer->TypeOfDeflection();
      aDrawer->SetTypeOfDeflection (Aspect_TOD_ABSOLUTE);

      // Draw each kind of subshapes and personal-colored shapes in a separate group
      // since it's necessary to set transparency/material for all subshapes
      // without affecting their unique colors
      if (theMode == AIS_Shaded
       && aShapeDraw.ShapeType() <= TopAbs_FACE
       && !IsInfinite())
      {
        StdPrs_ShadedShape::Add (thePrs, aShapeDraw, aDrawer, theVolume);
      }
      else
      {
        StdPrs_WFShape::Add (thePrs, aShapeDraw, aDrawer);
      }
      aDrawer->SetTypeOfDeflection (aPrevType);
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

//! Function to check if specified compound is sub-shape of another one
inline Standard_Boolean isFirstCmpContainSecondOne (const TopoDS_Shape& theFirstCmp,
                                                    const TopoDS_Shape& theSecondCmp)
{
  if (theFirstCmp.ShapeType()  != TopAbs_COMPOUND
   || theSecondCmp.ShapeType() != TopAbs_COMPOUND)
  {
    return Standard_False;
  }

  for (TopoDS_Iterator aFirstCmpIter (theFirstCmp); aFirstCmpIter.More(); aFirstCmpIter.Next())
  {
    if (aFirstCmpIter.Value().ShapeType() != TopAbs_COMPOUND)
    {
      continue;
    }
    else if (aFirstCmpIter.Value() == theSecondCmp
          || isFirstCmpContainSecondOne (aFirstCmpIter.Value(), theSecondCmp))
    {
      return Standard_True;
    }
  }
  return Standard_False;
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
  for (DataMapOfShapeColor::Iterator aKeyShapeIter (theKeyshapeColorMap);
       aKeyShapeIter.More(); aKeyShapeIter.Next())
  {
    const TopoDS_Shape& aKeyShape = aKeyShapeIter.Key();
    bindSubShapes (aSubshapeKeyshapeMap, theBaseShape, aKeyShape, aKeyShape);
  }

  // Fill the array of maps per shape type
  dispatchColors (theBaseShape, theBaseShape,
                  aSubshapeKeyshapeMap, TopAbs_SHAPE,
                  theTypeKeyshapeDrawshapeArray);
}

//=======================================================================
//function : isShapeEntirelyVisible
//purpose  :
//=======================================================================
Standard_Boolean AIS_ColoredShape::isShapeEntirelyVisible (DataMapOfShapeCompd* theDispatched) const
{
  Handle(AIS_ColoredDrawer) aCustomDrawer;
  for (size_t aShType = (size_t )TopAbs_COMPOUND; aShType <= (size_t )TopAbs_FACE; ++aShType)
  {
    const DataMapOfShapeCompd& aKeyshapeDrawshapeMap = theDispatched[aShType];
    for (DataMapOfShapeCompd::Iterator aMapIter (aKeyshapeDrawshapeMap); aMapIter.More(); aMapIter.Next())
    {
      if (myShapeColors.Find (aMapIter.Key(), aCustomDrawer)
      && !aCustomDrawer.IsNull()
      &&  aCustomDrawer->IsHidden())
      {
        return Standard_False;
      }
    }
  }
  return Standard_True;
}

//=======================================================================
//function : isShapeEntirelyVisible
//purpose  :
//=======================================================================
Standard_Boolean AIS_ColoredShape::isShapeEntirelyVisible() const
{
  for (DataMapOfShapeColor::Iterator aMapIter (myShapeColors); aMapIter.More(); aMapIter.Next())
  {
    if (aMapIter.Value()->IsHidden())
    {
      return Standard_False;
    }
  }
  return Standard_True;
}

//=======================================================================
//function : bindSubShapes
//purpose  :
//=======================================================================
void AIS_ColoredShape::bindSubShapes (DataMapOfShapeShape& theSubshapeKeyshapeMap,
                                      const TopoDS_Shape&  theBaseShape,
                                      const TopoDS_Shape&  theShapeWithColor,
                                      const TopoDS_Shape&  theColorKeyShape)
{
  TopAbs_ShapeEnum aShapeWithColorType = theShapeWithColor.ShapeType();
  if (aShapeWithColorType == TopAbs_COMPOUND)
  {
    if (isFirstCmpContainSecondOne (theBaseShape, theShapeWithColor))
    {
      if (!theSubshapeKeyshapeMap.IsBound (theShapeWithColor))
      {
        theSubshapeKeyshapeMap.Bind (theShapeWithColor, theColorKeyShape);
      }
    }
    else
    {
      for (TopoDS_Iterator aSubShapeIter (theShapeWithColor); aSubShapeIter.More(); aSubShapeIter.Next())
      {
        bindSubShapes (theSubshapeKeyshapeMap, theBaseShape, aSubShapeIter.Value(), theColorKeyShape);
      }
    }
  }
  else if (aShapeWithColorType == TopAbs_SOLID || aShapeWithColorType == TopAbs_SHELL)
  {
    for (TopExp_Explorer anExp (theShapeWithColor, TopAbs_FACE); anExp.More(); anExp.Next())
    {
      if (!theSubshapeKeyshapeMap.IsBound (anExp.Current()))
      {
        theSubshapeKeyshapeMap.Bind (anExp.Current(), theColorKeyShape);
      }
    }
  }
  else if (aShapeWithColorType == TopAbs_WIRE)
  {
    for (TopExp_Explorer anExp (theShapeWithColor, TopAbs_EDGE); anExp.More(); anExp.Next())
    {
      if (!theSubshapeKeyshapeMap.IsBound (anExp.Current()))
      {
        theSubshapeKeyshapeMap.Bind (anExp.Current(), theColorKeyShape);
      }
    }
  }
  else
  {
    // bind single face, edge and vertex
    // force rebind if required due to the color of single shape has
    // higher priority than the color of "compound" shape (wire is a
    // compound of edges, shell is a compound of faces) that contains
    // this single shape.
    theSubshapeKeyshapeMap.Bind (theShapeWithColor, theColorKeyShape);
  }
}

