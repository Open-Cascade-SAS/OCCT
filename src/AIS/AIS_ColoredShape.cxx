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
#include <BRepMesh_IncrementalMesh.hxx>
#include <gp_Pnt2d.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
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
#include <StdSelect_BRepSelectionTool.hxx>
#include <StdPrs_ShadedShape.hxx>
#include <StdPrs_ToolTriangulatedShape.hxx>
#include <StdPrs_WFShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_ColoredShape,AIS_Shape)
IMPLEMENT_STANDARD_RTTIEXT(AIS_ColoredDrawer,Prs3d_Drawer)

namespace
{
  //! Collect all sub-compounds into map.
  static void collectSubCompounds (TopTools_MapOfShape& theMap,
                                   const TopoDS_Shape&  theShape)
  {
    for (TopoDS_Iterator aChildIter (theShape); aChildIter.More(); aChildIter.Next())
    {
      const TopoDS_Shape& aShape = aChildIter.Value();
      if (aShape.ShapeType() == TopAbs_COMPOUND
       && theMap.Add (aShape))
      {
        collectSubCompounds (theMap, aShape);
      }
    }
  }
}

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
    Quantity_Color aColor;
    theShape->Color (aColor);
    SetColor (aColor);
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
  myDrawer->SetColor (theColor);
  hasOwnColor = Standard_True;
  LoadRecomputable (AIS_WireFrame);
  LoadRecomputable (AIS_Shaded);
  for (AIS_DataMapOfShapeDrawer::Iterator anIter (myShapeColors); anIter.More(); anIter.Next())
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
  for (AIS_DataMapOfShapeDrawer::Iterator anIter (myShapeColors); anIter.More(); anIter.Next())
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
  myDrawer->SetTransparency ((Standard_ShortReal )theValue);
  LoadRecomputable (AIS_WireFrame);
  LoadRecomputable (AIS_Shaded);
  for (AIS_DataMapOfShapeDrawer::Iterator anIter (myShapeColors); anIter.More(); anIter.Next())
  {
    const Handle(Prs3d_Drawer)& aDrawer = anIter.Value();
    if (aDrawer->HasOwnShadingAspect())
    {
      aDrawer->ShadingAspect()->SetTransparency (theValue, myCurrentFacingModel);
    }
  }
}

//=======================================================================
//function : UnsetTransparency
//purpose  :
//=======================================================================
void AIS_ColoredShape::UnsetTransparency()
{
  myDrawer->SetTransparency (0.0f);
  if (myDrawer->HasOwnShadingAspect())
  {
    myDrawer->ShadingAspect()->SetTransparency (0.0, myCurrentFacingModel);
    if (!HasColor()
     && !HasMaterial()
     && !myDrawer->ShadingAspect()->Aspect()->ToMapTexture())
    {
      myDrawer->SetShadingAspect (Handle(Prs3d_ShadingAspect)());
    }
  }

  for (AIS_DataMapOfShapeDrawer::Iterator anIter (myShapeColors); anIter.More(); anIter.Next())
  {
    const Handle(Prs3d_Drawer)& aDrawer = anIter.Value();
    if (aDrawer->HasOwnShadingAspect())
    {
      aDrawer->ShadingAspect()->SetTransparency (0.0, myCurrentFacingModel);
    }
  }
  SynchronizeAspects();
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
  for (AIS_DataMapOfShapeDrawer::Iterator anIter (myShapeColors); anIter.More(); anIter.Next())
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
  if (myshape.IsNull())
  {
    return;
  }

  if (IsInfinite())
  {
    thePrs->SetInfiniteState (Standard_True);
  }

  if (theMode == AIS_Shaded)
  {
    if (myDrawer->IsAutoTriangulation())
    {
      // compute mesh for entire shape beforehand to ensure consistency and optimizations (parallelization)
      StdPrs_ToolTriangulatedShape::ClearOnOwnDeflectionChange (myshape, myDrawer, Standard_True);

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
    StdPrs_ToolTriangulatedShape::ClearOnOwnDeflectionChange (myshape, myDrawer, Standard_True);

    // After this call if type of deflection is relative
    // computed deflection coefficient is stored as absolute.
    Prs3d::GetDeflection (myshape, myDrawer);
  }

  // Extract myShapeColors map (KeyshapeColored -> Color) to subshapes map (Subshape -> Color).
  // This needed when colored shape is not part of BaseShape (but subshapes are) and actually container for subshapes.
  AIS_DataMapOfShapeDrawer aSubshapeDrawerMap;
  fillSubshapeDrawerMap (aSubshapeDrawerMap);

  Handle(AIS_ColoredDrawer) aBaseDrawer;
  myShapeColors.Find (myshape, aBaseDrawer);

  // myShapeColors + anOpened --> array[TopAbs_ShapeEnum] of map of color-to-compound
  DataMapOfDrawerCompd aDispatchedOpened[(size_t)TopAbs_SHAPE];
  DataMapOfDrawerCompd aDispatchedClosed;
  dispatchColors (aBaseDrawer, myshape,
                  aSubshapeDrawerMap, TopAbs_COMPOUND, Standard_False,
                  aDispatchedOpened, theMode == AIS_Shaded ? aDispatchedClosed : aDispatchedOpened[TopAbs_FACE]);
  addShapesWithCustomProps (thePrs, aDispatchedOpened, aDispatchedClosed, theMode);
}

//=======================================================================
//function : fillSubshapeDrawerMap
//purpose  :
//=======================================================================
void AIS_ColoredShape::fillSubshapeDrawerMap (AIS_DataMapOfShapeDrawer& theSubshapeDrawerMap) const
{
  // unroll compounds specified for grouping sub-shapes with the same style
  // (e.g. the compounds that are not a part of the main shape)
  TopTools_MapOfShape aMapOfOwnCompounds;
  if (myshape.ShapeType() == TopAbs_COMPOUND)
  {
    aMapOfOwnCompounds.Add (myshape);
    collectSubCompounds (aMapOfOwnCompounds, myshape);
  }
  for (AIS_DataMapOfShapeDrawer::Iterator aKeyShapeIter (myShapeColors);
        aKeyShapeIter.More(); aKeyShapeIter.Next())
  {
    const TopoDS_Shape& aKeyShape = aKeyShapeIter.Key();
    if (aKeyShape.ShapeType() != TopAbs_COMPOUND
     || aMapOfOwnCompounds.Contains (aKeyShape))
    {
      continue;
    }

    for (TopoDS_Iterator aChildIter (aKeyShape); aChildIter.More(); aChildIter.Next())
    {
      const TopoDS_Shape& aShape = aChildIter.Value();
      if (!myShapeColors.IsBound (aShape))
      {
        bindSubShapes (theSubshapeDrawerMap, aShape, aKeyShapeIter.Value());
      }
    }
  }

  // assign other sub-shapes with styles
  for (AIS_DataMapOfShapeDrawer::Iterator aKeyShapeIter (myShapeColors);
        aKeyShapeIter.More(); aKeyShapeIter.Next())
  {
    const TopoDS_Shape& aKeyShape = aKeyShapeIter.Key();
    if (myshape == aKeyShape
    || (aKeyShape.ShapeType() == TopAbs_COMPOUND
    && !aMapOfOwnCompounds.Contains (aKeyShape)))
    {
      continue;
    }

    bindSubShapes (theSubshapeDrawerMap, aKeyShape, aKeyShapeIter.Value());
  }
}

//=======================================================================
//function : ComputeSelection
//purpose  :
//=======================================================================
void AIS_ColoredShape::ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                                         const Standard_Integer theMode)
{
  if (myshape.IsNull())
  {
    return;
  }
  else if (isShapeEntirelyVisible())
  {
    base_type::ComputeSelection (theSelection, theMode);
    return;
  }

  const TopAbs_ShapeEnum aTypOfSel   = AIS_Shape::SelectionType (theMode);
  const Standard_Real    aDeflection = Prs3d::GetDeflection (myshape, myDrawer);
  const Standard_Real    aDeviationAngle = myDrawer->HLRAngle();
  const Standard_Integer aPriority   = StdSelect_BRepSelectionTool::GetStandardPriority (myshape, aTypOfSel);
  if (myDrawer->IsAutoTriangulation()
  && !BRepTools::Triangulation (myshape, Precision::Infinite()))
  {
    BRepMesh_IncrementalMesh aMesher (myshape, aDeflection, Standard_False, aDeviationAngle);
  }

  AIS_DataMapOfShapeDrawer aSubshapeDrawerMap;
  fillSubshapeDrawerMap (aSubshapeDrawerMap);

  Handle(StdSelect_BRepOwner) aBrepOwner = new StdSelect_BRepOwner (myshape, aPriority);
  if (aTypOfSel == TopAbs_SHAPE)
  {
    aBrepOwner = new StdSelect_BRepOwner (myshape, aPriority);
  }

  Handle(AIS_ColoredDrawer) aBaseDrawer;
  myShapeColors.Find (myshape, aBaseDrawer);
  computeSubshapeSelection (aBaseDrawer, aSubshapeDrawerMap, myshape, aBrepOwner, theSelection,
                            aTypOfSel, aPriority, aDeflection, aDeviationAngle);

  Handle(SelectMgr_SelectableObject) aThis (this);
  for (theSelection->Init(); theSelection->More(); theSelection->Next())
  {
    Handle(SelectMgr_EntityOwner) anOwner = Handle(SelectMgr_EntityOwner)::DownCast (theSelection->Sensitive()->BaseSensitive()->OwnerId());
    anOwner->Set (aThis);
  }

  StdSelect_BRepSelectionTool::PreBuildBVH (theSelection);
}

//=======================================================================
//function : computeSubshapeSelection
//purpose  :
//=======================================================================
void AIS_ColoredShape::computeSubshapeSelection (const Handle(AIS_ColoredDrawer)& theParentDrawer,
                                                 const AIS_DataMapOfShapeDrawer& theShapeDrawerMap,
                                                 const TopoDS_Shape& theShape,
                                                 const Handle(StdSelect_BRepOwner)& theOwner,
                                                 const Handle(SelectMgr_Selection)& theSelection,
                                                 const TopAbs_ShapeEnum theTypOfSel,
                                                 const Standard_Integer thePriority,
                                                 const Standard_Real theDeflection,
                                                 const Standard_Real theDeflAngle)
{
  Handle(AIS_ColoredDrawer) aDrawer = theParentDrawer;
  theShapeDrawerMap.Find (theShape, aDrawer);
  if (!aDrawer.IsNull()
    && aDrawer->IsHidden())
  {
    return;
  }

  const Standard_Integer aNbPOnEdge = 9;
  const Standard_Real    aMaximalParameter = 500.0;
  if (theTypOfSel == TopAbs_SHAPE
   && theShape.ShapeType() >= TopAbs_FACE)
  {
    StdSelect_BRepSelectionTool::ComputeSensitive (theShape, theOwner, theSelection,
                                                   theDeflection, theDeflAngle, aNbPOnEdge, aMaximalParameter, myDrawer->IsAutoTriangulation());
    return;
  }
  else if (theShape.ShapeType() == theTypOfSel)
  {
    const Standard_Boolean isComesFromDecomposition = !theShape.IsEqual (myshape);
    Handle(StdSelect_BRepOwner) aBrepOwner = new StdSelect_BRepOwner (theShape, thePriority, isComesFromDecomposition);
    StdSelect_BRepSelectionTool::ComputeSensitive (theShape, aBrepOwner, theSelection,
                                                   theDeflection, theDeflAngle, aNbPOnEdge, aMaximalParameter, myDrawer->IsAutoTriangulation());
    return;
  }

  for (TopoDS_Iterator aSubShapeIter (theShape); aSubShapeIter.More(); aSubShapeIter.Next())
  {
    const TopoDS_Shape& aSubShape = aSubShapeIter.Value();
    computeSubshapeSelection (aDrawer, theShapeDrawerMap, aSubShape,
                              theOwner, theSelection, theTypOfSel, thePriority,
                              theDeflection, theDeflAngle);
  }
}

//=======================================================================
//function : addShapesWithCustomProps
//purpose  :
//=======================================================================
void AIS_ColoredShape::addShapesWithCustomProps (const Handle(Prs3d_Presentation)& thePrs,
                                                 const DataMapOfDrawerCompd* theDrawerOpenedShapePerType,
                                                 const DataMapOfDrawerCompd& theDrawerClosedFaces,
                                                 const Standard_Integer theMode)
{
  Handle(Graphic3d_Group) anOpenGroup, aClosedGroup;
  for (size_t aShType = 0; aShType <= (size_t )TopAbs_SHAPE; ++aShType)
  {
    const Standard_Boolean isClosed = aShType == TopAbs_SHAPE;
    Handle(Graphic3d_Group)& aShadedGroup = isClosed ? aClosedGroup : anOpenGroup;
    const DataMapOfDrawerCompd& aDrawerShapeMap = isClosed
                                                ? theDrawerClosedFaces
                                                : theDrawerOpenedShapePerType[aShType];
    for (DataMapOfDrawerCompd::Iterator aMapIter (aDrawerShapeMap);
         aMapIter.More(); aMapIter.Next())
    {
      const Handle(AIS_ColoredDrawer)& aCustomDrawer = aMapIter.Key();
      const TopoDS_Compound& aShapeDraw = aMapIter.Value(); // compound of subshapes with <aShType> type
      Handle(Prs3d_Drawer) aDrawer;
      if (!aCustomDrawer.IsNull())
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
        // add wireframe presentation for isolated edges and vertices
        StdPrs_ShadedShape::AddWireframeForFreeElements (thePrs, aShapeDraw, aDrawer);

        // add special wireframe presentation for faces without triangulation
        StdPrs_ShadedShape::AddWireframeForFacesWithoutTriangles (thePrs, aShapeDraw, aDrawer);

        Handle(Graphic3d_ArrayOfTriangles) aTriangles = StdPrs_ShadedShape::FillTriangles (aShapeDraw,
                                                                                           aDrawer->ShadingAspect()->Aspect()->ToMapTexture()
                                                                                       && !aDrawer->ShadingAspect()->Aspect()->TextureMap().IsNull(),
                                                                                           myUVOrigin, myUVRepeat, myUVScale);
        if (!aTriangles.IsNull())
        {
          if (aShadedGroup.IsNull())
          {
            aShadedGroup = Prs3d_Root::NewGroup (thePrs);
            aShadedGroup->SetClosed (isClosed);
          }
          aShadedGroup->SetPrimitivesAspect (aDrawer->ShadingAspect()->Aspect());
          aShadedGroup->AddPrimitiveArray (aTriangles);
        }

        if (aDrawer->FaceBoundaryDraw())
        {
          Handle(Graphic3d_ArrayOfSegments) aBndSegments = StdPrs_ShadedShape::FillFaceBoundaries (aShapeDraw);
          if (!aBndSegments.IsNull())
          {
            if (aShadedGroup.IsNull())
            {
              aShadedGroup = Prs3d_Root::NewGroup (thePrs);
              aShadedGroup->SetClosed (isClosed);
            }

            Handle(Graphic3d_AspectLine3d) aBoundaryAspect = aDrawer->FaceBoundaryAspect()->Aspect();
            aShadedGroup->SetPrimitivesAspect (aBoundaryAspect);
            aShadedGroup->AddPrimitiveArray (aBndSegments);
          }
        }
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
Standard_Boolean AIS_ColoredShape::dispatchColors (const Handle(AIS_ColoredDrawer)& theParentDrawer,
                                                   const TopoDS_Shape& theShapeToParse,
                                                   const AIS_DataMapOfShapeDrawer& theShapeDrawerMap,
                                                   const TopAbs_ShapeEnum theParentType,
                                                   const Standard_Boolean theIsParentClosed,
                                                   DataMapOfDrawerCompd* theDrawerOpenedShapePerType,
                                                   DataMapOfDrawerCompd& theDrawerClosedFaces)
{
  const TopAbs_ShapeEnum aShapeType = theShapeToParse.ShapeType();
  if (aShapeType == TopAbs_SHAPE)
  {
    return Standard_False;
  }

  // check own setting of current shape
  Handle(AIS_ColoredDrawer) aDrawer = theParentDrawer;
  const Standard_Boolean isOverriden = theShapeDrawerMap.Find (theShapeToParse, aDrawer);
  if (isOverriden
   && aDrawer->IsHidden())
  {
    return Standard_True;
  }

  // handle compounds, solids and shells
  Standard_Boolean isSubOverride = Standard_False;
  if (aShapeType <= TopAbs_SHELL)
  {
    // detect parts of closed solids
    Standard_Boolean isClosedShell = theParentType == TopAbs_SOLID
                                  && aShapeType == TopAbs_SHELL
                                  && BRep_Tool::IsClosed (theShapeToParse)
                                  && StdPrs_ToolTriangulatedShape::IsTriangulated (theShapeToParse);
    if (isClosedShell)
    {
      for (TopoDS_Iterator aFaceIter (theShapeToParse); aFaceIter.More(); aFaceIter.Next())
      {
        const TopoDS_Shape& aFace = aFaceIter.Value();
        Handle(AIS_ColoredDrawer) aFaceDrawer;
        if (aFace.ShapeType() == TopAbs_FACE
         && theShapeDrawerMap.Find (aFace, aFaceDrawer)
         && aFaceDrawer->IsHidden())
        {
          isClosedShell = Standard_False;
          break;
        }
      }
    }

    for (TopoDS_Iterator aSubShapeIter (theShapeToParse); aSubShapeIter.More(); aSubShapeIter.Next())
    {
      const TopoDS_Shape& aSubShape = aSubShapeIter.Value();
      if (dispatchColors (aDrawer, aSubShape,
                          theShapeDrawerMap, aShapeType,
                          isClosedShell,
                          theDrawerOpenedShapePerType,
                          theDrawerClosedFaces))
      {
        isSubOverride = Standard_True;
      }
    }
    return isOverriden || isSubOverride;
  }

  // iterate on sub-shapes
  BRep_Builder aBBuilder;
  TopoDS_Shape aShapeCopy = theShapeToParse.EmptyCopied();
  aShapeCopy.Closed (theShapeToParse.Closed());
  Standard_Integer nbDef = 0;
  for (TopoDS_Iterator aSubShapeIter (theShapeToParse); aSubShapeIter.More(); aSubShapeIter.Next())
  {
    const TopoDS_Shape& aSubShape = aSubShapeIter.Value();
    if (dispatchColors (aDrawer, aSubShape,
                        theShapeDrawerMap, aShapeType,
                        theIsParentClosed,
                        theDrawerOpenedShapePerType,
                        theDrawerClosedFaces))
    {
      isSubOverride = Standard_True;
    }
    else
    {
      aBBuilder.Add (aShapeCopy, aSubShape);
      ++nbDef;
    }
  }
  if (aShapeType == TopAbs_FACE || !isSubOverride)
  {
    aShapeCopy = theShapeToParse;
  }
  else if (nbDef == 0)
  {
    return isOverriden || isSubOverride; // empty compound
  }

  // if any of styles is overridden regarding to default one, add rest to map
  if (isOverriden
  || (isSubOverride && theParentType != TopAbs_WIRE  // avoid drawing edges when vertex color is overridden
                    && theParentType != TopAbs_FACE) // avoid drawing edges of the same color as face
  || (theParentType <= TopAbs_SHELL && !(isOverriden || isSubOverride))) // bind original shape to default color
  {
    TopoDS_Compound aCompound;
    DataMapOfDrawerCompd& aDrawerShapeMap = theIsParentClosed
                                         && aShapeType == TopAbs_FACE
                                          ? theDrawerClosedFaces
                                          : theDrawerOpenedShapePerType[(size_t)aShapeType];
    if (!aDrawerShapeMap.FindFromKey (aDrawer, aCompound))
    {
      aBBuilder.MakeCompound (aCompound);
      aDrawerShapeMap.Add (aDrawer, aCompound);
    }
    aBBuilder.Add (aCompound, aShapeCopy);
  }
  return isOverriden || isSubOverride;
}

//=======================================================================
//function : isShapeEntirelyVisible
//purpose  :
//=======================================================================
Standard_Boolean AIS_ColoredShape::isShapeEntirelyVisible() const
{
  for (AIS_DataMapOfShapeDrawer::Iterator aMapIter (myShapeColors); aMapIter.More(); aMapIter.Next())
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
void AIS_ColoredShape::bindSubShapes (AIS_DataMapOfShapeDrawer& theShapeDrawerMap,
                                      const TopoDS_Shape& theKeyShape,
                                      const Handle(AIS_ColoredDrawer)& theDrawer) const
{
  TopAbs_ShapeEnum aShapeWithColorType = theKeyShape.ShapeType();
  if (aShapeWithColorType == TopAbs_COMPOUND)
  {
    theShapeDrawerMap.Bind (theKeyShape, theDrawer);
  }
  else if (aShapeWithColorType == TopAbs_SOLID || aShapeWithColorType == TopAbs_SHELL)
  {
    for (TopExp_Explorer anExp (theKeyShape, TopAbs_FACE); anExp.More(); anExp.Next())
    {
      if (!theShapeDrawerMap.IsBound (anExp.Current()))
      {
        theShapeDrawerMap.Bind (anExp.Current(), theDrawer);
      }
    }
  }
  else if (aShapeWithColorType == TopAbs_WIRE)
  {
    for (TopExp_Explorer anExp (theKeyShape, TopAbs_EDGE); anExp.More(); anExp.Next())
    {
      if (!theShapeDrawerMap.IsBound (anExp.Current()))
      {
        theShapeDrawerMap.Bind (anExp.Current(), theDrawer);
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
    theShapeDrawerMap.Bind (theKeyShape, theDrawer);
  }
}
