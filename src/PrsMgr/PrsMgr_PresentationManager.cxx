// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <PrsMgr_PresentationManager.hxx>

#include <Geom_Transformation.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_PresentationShadow.hxx>
#include <PrsMgr_ModedPresentation.hxx>
#include <PrsMgr_PresentableObject.hxx>
#include <PrsMgr_Presentation.hxx>
#include <PrsMgr_Presentations.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_Type.hxx>
#include <TColStd_ListIteratorOfListOfTransient.hxx>
#include <V3d_View.hxx>

IMPLEMENT_STANDARD_RTTIEXT(PrsMgr_PresentationManager, Standard_Transient)

// =======================================================================
// function : PrsMgr_PresentationManager
// purpose  :
// =======================================================================
PrsMgr_PresentationManager::PrsMgr_PresentationManager (const Handle(Graphic3d_StructureManager)& theStructureManager)
: myStructureManager (theStructureManager),
  myImmediateModeOn  (0)
{
  //
}

// =======================================================================
// function : Display
// purpose  :
// =======================================================================
void PrsMgr_PresentationManager::Display (const Handle(PrsMgr_PresentableObject)& thePrsObj,
                                          const Standard_Integer                  theMode)
{
  if (thePrsObj->HasOwnPresentations())
  {
    Handle(PrsMgr_Presentation) aPrs = Presentation (thePrsObj, theMode, Standard_True);
    if (aPrs->MustBeUpdated())
    {
      Update (thePrsObj, theMode);
    }

    if (myImmediateModeOn > 0)
    {
      AddToImmediateList (aPrs->Presentation());
    }
    else
    {
      aPrs->Display();
    }
  }
  else
  {
    thePrsObj->Compute (this, Handle(Prs3d_Presentation)(), theMode);
  }

  for (PrsMgr_ListOfPresentableObjectsIter anIter (thePrsObj->Children()); anIter.More(); anIter.Next())
  {
    Display (anIter.Value(), theMode);
  }
}

// =======================================================================
// function : Erase
// purpose  :
// =======================================================================
void PrsMgr_PresentationManager::Erase (const Handle(PrsMgr_PresentableObject)& thePrsObj,
                                        const Standard_Integer                  theMode)
{
  for (PrsMgr_ListOfPresentableObjectsIter anIter (thePrsObj->Children()); anIter.More(); anIter.Next())
  {
    Erase (anIter.Value(), theMode);
  }

  PrsMgr_Presentations& aPrsList = thePrsObj->Presentations();
  for (PrsMgr_Presentations::Iterator anIt (aPrsList); anIt.More();)
  {
    const PrsMgr_ModedPresentation& aModedPrs = anIt.Value();
    if (aModedPrs.Presentation().IsNull())
    {
      anIt.Next();
      continue;
    }

    const Handle(PrsMgr_PresentationManager)& aPrsMgr = aModedPrs.Presentation()->PresentationManager();
    if ((theMode == aModedPrs.Mode() || theMode == -1)
     && (this == aPrsMgr))
    {
      aModedPrs.Presentation()->Erase();

      aPrsList.Remove (anIt);

      if (theMode != -1)
      {
        return;
      }
    }
    else
    {
      anIt.Next();
    }
  }
}

// =======================================================================
// function : Clear
// purpose  :
// =======================================================================
void PrsMgr_PresentationManager::Clear (const Handle(PrsMgr_PresentableObject)& thePrsObj,
                                        const Standard_Integer                  theMode)
{
  for (PrsMgr_ListOfPresentableObjectsIter anIter (thePrsObj->Children()); anIter.More(); anIter.Next())
  {
    Clear (anIter.Value(), theMode);
  }

  const Handle(PrsMgr_Presentation) aPrs = Presentation (thePrsObj, theMode);
  if (!aPrs.IsNull())
  {
    aPrs->Clear();
  }
}

// =======================================================================
// function : SetVisibility
// purpose  :
// =======================================================================
void PrsMgr_PresentationManager::SetVisibility (const Handle(PrsMgr_PresentableObject)& thePrsObj,
                                                const Standard_Integer theMode,
                                                const Standard_Boolean theValue)
{
  for (PrsMgr_ListOfPresentableObjectsIter anIter (thePrsObj->Children()); anIter.More(); anIter.Next())
  {
    SetVisibility (anIter.Value(), theMode, theValue);
  }
  if (!thePrsObj->HasOwnPresentations())
  {
    return;
  }

  Handle(PrsMgr_Presentation) aPrs = Presentation (thePrsObj, theMode);
  if (!aPrs.IsNull())
  {
    aPrs->SetVisible (theValue);
  }
}

// =======================================================================
// function : Unhighlight
// purpose  :
// =======================================================================
void PrsMgr_PresentationManager::Unhighlight (const Handle(PrsMgr_PresentableObject)& thePrsObj)
{
  for (PrsMgr_ListOfPresentableObjectsIter anIter (thePrsObj->Children()); anIter.More(); anIter.Next())
  {
    Unhighlight (anIter.Value());
  }

  const PrsMgr_Presentations& aPrsList = thePrsObj->Presentations();
  for (Standard_Integer aPrsIter = 1; aPrsIter <= aPrsList.Length(); ++aPrsIter)
  {
    const PrsMgr_ModedPresentation&           aModedPrs = aPrsList.Value (aPrsIter);
    const Handle(PrsMgr_Presentation)&        aPrs      = aModedPrs.Presentation();
    const Handle(PrsMgr_PresentationManager)& aPrsMgr   = aPrs->PresentationManager();
    if (this == aPrsMgr
    &&  aPrs->IsHighlighted())
    {
      aPrs->Unhighlight();
    }
  }
}

// =======================================================================
// function : SetDisplayPriority
// purpose  :
// =======================================================================
void PrsMgr_PresentationManager::SetDisplayPriority (const Handle(PrsMgr_PresentableObject)& thePrsObj,
                                                     const Standard_Integer                  theMode,
                                                     const Standard_Integer                  theNewPrior) const
{
  for (PrsMgr_ListOfPresentableObjectsIter anIter (thePrsObj->Children()); anIter.More(); anIter.Next())
  {
    SetDisplayPriority (anIter.Value(), theMode, theNewPrior);
  }

  const Handle(PrsMgr_Presentation) aPrs = Presentation (thePrsObj, theMode);
  if (!aPrs.IsNull())
  {
    aPrs->SetDisplayPriority (theNewPrior);
  }
}

// =======================================================================
// function : DisplayPriority
// purpose  :
// =======================================================================
Standard_Integer PrsMgr_PresentationManager::DisplayPriority (const Handle(PrsMgr_PresentableObject)& thePrsObj,
                                                              const Standard_Integer                  theMode) const
{
  for (PrsMgr_ListOfPresentableObjectsIter anIter (thePrsObj->Children()); anIter.More(); anIter.Next())
  {
    Standard_Integer aPriority = DisplayPriority (anIter.Value(), theMode);
    if (aPriority != 0)
    {
      return aPriority;
    }
  }

  const Handle(PrsMgr_Presentation) aPrs = Presentation (thePrsObj, theMode);
  return !aPrs.IsNull()
        ? aPrs->DisplayPriority()
        : 0;
}

// =======================================================================
// function : IsDisplayed
// purpose  :
// =======================================================================
Standard_Boolean PrsMgr_PresentationManager::IsDisplayed (const Handle(PrsMgr_PresentableObject)& thePrsObj,
                                                          const Standard_Integer                  theMode) const
{
  for (PrsMgr_ListOfPresentableObjectsIter anIter (thePrsObj->Children()); anIter.More(); anIter.Next())
  {
    if (IsDisplayed (anIter.Value(), theMode))
    {
      return Standard_True;
    }
  }

  const Handle(PrsMgr_Presentation) aPrs = Presentation (thePrsObj, theMode);
  return !aPrs.IsNull()
       && aPrs->IsDisplayed();
}

// =======================================================================
// function : IsHighlighted
// purpose  :
// =======================================================================
Standard_Boolean PrsMgr_PresentationManager::IsHighlighted (const Handle(PrsMgr_PresentableObject)& thePrsObj,
                                                            const Standard_Integer                  theMode) const
{
  for (PrsMgr_ListOfPresentableObjectsIter anIter (thePrsObj->Children()); anIter.More(); anIter.Next())
  {
    if (IsHighlighted (anIter.Value(), theMode))
    {
      return Standard_True;
    }
  }

  const Handle(PrsMgr_Presentation) aPrs = Presentation (thePrsObj, theMode);
  return !aPrs.IsNull()
       && aPrs->IsHighlighted();
}

// =======================================================================
// function : Update
// purpose  :
// =======================================================================
void PrsMgr_PresentationManager::Update (const Handle(PrsMgr_PresentableObject)& thePrsObj,
                                         const Standard_Integer                  theMode) const
{
  for (PrsMgr_ListOfPresentableObjectsIter anIter (thePrsObj->Children()); anIter.More(); anIter.Next())
  {
    Update (anIter.Value(), theMode);
  }

  Handle(PrsMgr_Presentation) aPrs = Presentation (thePrsObj, theMode);
  if (!aPrs.IsNull())
  {
    aPrs->Clear();
    thePrsObj->Fill (this, aPrs, theMode);
    aPrs->SetUpdateStatus (Standard_False);
  }
}

// =======================================================================
// function : BeginImmediateDraw
// purpose  :
// =======================================================================
void PrsMgr_PresentationManager::BeginImmediateDraw()
{
  if (++myImmediateModeOn > 1)
  {
    return;
  }

  ClearImmediateDraw();
}

// =======================================================================
// function : ClearImmediateDraw
// purpose  :
// =======================================================================
void PrsMgr_PresentationManager::ClearImmediateDraw()
{
  for (PrsMgr_ListOfPresentations::Iterator anIter (myImmediateList); anIter.More(); anIter.Next())
  {
    anIter.Value()->Erase();
  }

  for (PrsMgr_ListOfPresentations::Iterator anIter (myViewDependentImmediateList); anIter.More(); anIter.Next())
  {
    anIter.Value()->Erase();
  }

  myImmediateList.Clear();
  myViewDependentImmediateList.Clear();
}

// =======================================================================
// function : displayImmediate
// purpose  : Handles the structures from myImmediateList and its visibility
//            in all views of the viewer given by setting proper affinity
// =======================================================================
void PrsMgr_PresentationManager::displayImmediate (const Handle(V3d_Viewer)& theViewer)
{
  for (V3d_ListOfViewIterator anActiveViewIter (theViewer->ActiveViewIterator()); anActiveViewIter.More(); anActiveViewIter.Next())
  {
    const Handle(Graphic3d_CView)& aView = anActiveViewIter.Value()->View();
    for (PrsMgr_ListOfPresentations::Iterator anIter (myImmediateList); anIter.More(); anIter.Next())
    {
      const Handle(Prs3d_Presentation)& aPrs = anIter.Value();
      if (aPrs.IsNull())
        continue;

      Handle(Graphic3d_Structure) aViewDepPrs;
      Handle(Prs3d_PresentationShadow) aShadowPrs = Handle(Prs3d_PresentationShadow)::DownCast (aPrs);
      if (!aShadowPrs.IsNull() && aView->IsComputed (aShadowPrs->ParentId(), aViewDepPrs))
      {
        aShadowPrs.Nullify();
        aShadowPrs = new Prs3d_PresentationShadow (myStructureManager, 
                                                   Handle(Prs3d_Presentation)::DownCast (aViewDepPrs));
        aShadowPrs->SetZLayer (aViewDepPrs->CStructure()->ZLayer());
        aShadowPrs->SetClipPlanes (aViewDepPrs->ClipPlanes());
        aShadowPrs->CStructure()->IsForHighlight = 1;
        aShadowPrs->Highlight (aPrs->HighlightStyle());
        myViewDependentImmediateList.Append (aShadowPrs);
      }
      // handles custom highlight presentations which were defined in overridden
      // HilightOwnerWithColor method of a custom AIS objects and maintain its
      // visibility in different views on their own
      else if (aShadowPrs.IsNull())
      {
        aPrs->Display();
        continue;
      }

      if (!aShadowPrs->IsDisplayed())
      {
        aShadowPrs->CStructure()->ViewAffinity = new Graphic3d_ViewAffinity();
        aShadowPrs->CStructure()->ViewAffinity->SetVisible (Standard_False);
        aShadowPrs->Display();
      }

      Standard_Integer aViewId = aView->Identification();
      bool isParentVisible = aShadowPrs->ParentAffinity().IsNull() ?
        Standard_True : aShadowPrs->ParentAffinity()->IsVisible (aViewId);
      aShadowPrs->CStructure()->ViewAffinity->SetVisible (aViewId, isParentVisible);
    }
  }
}

// =======================================================================
// function : EndImmediateDraw
// purpose  :
// =======================================================================
void PrsMgr_PresentationManager::EndImmediateDraw (const Handle(V3d_Viewer)& theViewer)
{
  if (--myImmediateModeOn > 0)
  {
    return;
  }

  displayImmediate (theViewer);
}

// =======================================================================
// function : RedrawImmediate
// purpose  : Clears all immediate structures and redisplays with proper
//            affinity
//=======================================================================
void PrsMgr_PresentationManager::RedrawImmediate (const Handle(V3d_Viewer)& theViewer)
{
  if (myImmediateList.IsEmpty())
    return;

  // Clear previously displayed structures
  for (PrsMgr_ListOfPresentations::Iterator anIter (myImmediateList); anIter.More(); anIter.Next())
  {
    anIter.Value()->Erase();
  }
  for (PrsMgr_ListOfPresentations::Iterator anIter (myViewDependentImmediateList); anIter.More(); anIter.Next())
  {
    anIter.Value()->Erase();
  }
  myViewDependentImmediateList.Clear();

  displayImmediate (theViewer);
}

// =======================================================================
// function : AddToImmediateList
// purpose  :
//=======================================================================
void PrsMgr_PresentationManager::AddToImmediateList (const Handle(Prs3d_Presentation)& thePrs)
{
  if (myImmediateModeOn < 1)
  {
    return;
  }

  for (PrsMgr_ListOfPresentations::Iterator anIter (myImmediateList); anIter.More(); anIter.Next())
  {
    if (anIter.Value() == thePrs)
    {
      return;
    }
  }

  myImmediateList.Append (thePrs);
}

// =======================================================================
// function : HasPresentation
// purpose  :
// =======================================================================
Standard_Boolean PrsMgr_PresentationManager::HasPresentation (const Handle(PrsMgr_PresentableObject)& thePrsObj,
                                                              const Standard_Integer                  theMode) const
{
  if (!thePrsObj->HasOwnPresentations())
    return Standard_False;

  const PrsMgr_Presentations& aPrsList = thePrsObj->Presentations();
  for (Standard_Integer aPrsIter = 1; aPrsIter <= aPrsList.Length(); ++aPrsIter)
  {
    const PrsMgr_ModedPresentation&           aModedPrs = aPrsList.Value (aPrsIter);
    const Handle(PrsMgr_PresentationManager)& aPrsMgr   = aModedPrs.Presentation()->PresentationManager();
    if (theMode == aModedPrs.Mode()
     && this    == aPrsMgr)
    {
      return Standard_True;
    }
  }
  return Standard_False;
}

// =======================================================================
// function : Presentation
// purpose  :
// =======================================================================
Handle(PrsMgr_Presentation) PrsMgr_PresentationManager::Presentation (const Handle(PrsMgr_PresentableObject)& thePrsObj,
                                                                      const Standard_Integer                  theMode,
                                                                      const Standard_Boolean                  theToCreate,
                                                                      const Handle(PrsMgr_PresentableObject)& theSelObj) const
{
  const PrsMgr_Presentations& aPrsList = thePrsObj->Presentations();
  for (Standard_Integer aPrsIter = 1; aPrsIter <= aPrsList.Length(); ++aPrsIter)
  {
    const PrsMgr_ModedPresentation&           aModedPrs = aPrsList.Value (aPrsIter);
    const Handle(PrsMgr_PresentationManager)& aPrsMgr   = aModedPrs.Presentation()->PresentationManager();
    if (theMode == aModedPrs.Mode()
     && this    == aPrsMgr)
    {
      return aModedPrs.Presentation();
    }
  }

  if (!theToCreate)
  {
    return Handle(PrsMgr_Presentation)();
  }

  Handle(PrsMgr_Presentation) aPrs = new PrsMgr_Presentation (this, thePrsObj);
  aPrs->SetZLayer (thePrsObj->ZLayer());
  aPrs->Presentation()->CStructure()->ViewAffinity = myStructureManager->ObjectAffinity (!theSelObj.IsNull() ? theSelObj : thePrsObj);
  thePrsObj->Presentations().Append (PrsMgr_ModedPresentation (aPrs, theMode));
  thePrsObj->Fill (this, aPrs, theMode);

  // set layer index accordingly to object's presentations
  aPrs->SetUpdateStatus (Standard_False);
  return aPrs;
}

// =======================================================================
// function : RemovePresentation
// purpose  :
// =======================================================================
Standard_Boolean PrsMgr_PresentationManager::RemovePresentation (const Handle(PrsMgr_PresentableObject)& thePrsObj,
                                                                 const Standard_Integer                  theMode)
{
  PrsMgr_Presentations& aPrsList = thePrsObj->Presentations();
  for (Standard_Integer aPrsIter = 1; aPrsIter <= aPrsList.Length(); ++aPrsIter)
  {
    const PrsMgr_ModedPresentation&           aModedPrs = aPrsList.Value (aPrsIter);
    const Handle(PrsMgr_PresentationManager)& aPrsMgr   = aModedPrs.Presentation()->PresentationManager();
    if (theMode == aPrsList (aPrsIter).Mode()
     && this    == aPrsMgr)
    {
      aPrsList.Remove (aPrsIter);
      return Standard_True;
    }
  }
  return Standard_False;
}

// =======================================================================
// function : SetZLayer
// purpose  :
// =======================================================================
void PrsMgr_PresentationManager::SetZLayer (const Handle(PrsMgr_PresentableObject)& thePrsObj,
                                            const Standard_Integer                  theLayerId)
{
  for (PrsMgr_ListOfPresentableObjectsIter anIter (thePrsObj->Children()); anIter.More(); anIter.Next())
  {
    SetZLayer (anIter.Value(), theLayerId);
  }
  if (!thePrsObj->HasOwnPresentations())
  {
    return;
  }

  thePrsObj->SetZLayer (theLayerId);
}

// =======================================================================
// function : GetZLayer
// purpose  :
// =======================================================================
Standard_Integer PrsMgr_PresentationManager::GetZLayer (const Handle(PrsMgr_PresentableObject)& thePrsObj) const
{
  return thePrsObj->ZLayer();
}

// =======================================================================
// function : Connect
// purpose  :
// =======================================================================
void PrsMgr_PresentationManager::Connect (const Handle(PrsMgr_PresentableObject)& thePrsObject,
                                          const Handle(PrsMgr_PresentableObject)& theOtherObject,
                                          const Standard_Integer                  theMode,
                                          const Standard_Integer                  theOtherMode)
{
  Handle(PrsMgr_Presentation) aPrs      = Presentation (thePrsObject,   theMode,      Standard_True);
  Handle(PrsMgr_Presentation) aPrsOther = Presentation (theOtherObject, theOtherMode, Standard_True);
  aPrs->Connect (aPrsOther);
}

// =======================================================================
// function : Transform
// purpose  :
// =======================================================================
void PrsMgr_PresentationManager::Transform (const Handle(PrsMgr_PresentableObject)& thePrsObj,
                                            const Handle(Geom_Transformation)&      theTransformation,
                                            const Standard_Integer                  theMode)
{
  Presentation (thePrsObj, theMode)->SetTransformation (theTransformation);
}

// =======================================================================
// function : Color
// purpose  :
// =======================================================================
void PrsMgr_PresentationManager::Color (const Handle(PrsMgr_PresentableObject)& thePrsObj,
                                        const Handle(Prs3d_Drawer)& theStyle,
                                        const Standard_Integer                  theMode,
                                        const Handle(PrsMgr_PresentableObject)& theSelObj,
                                        const Standard_Integer theImmediateStructLayerId)
{
  for (PrsMgr_ListOfPresentableObjectsIter anIter (thePrsObj->Children()); anIter.More(); anIter.Next())
  {
    Color (anIter.Value(), theStyle, theMode, NULL, theImmediateStructLayerId);
  }
  if (!thePrsObj->HasOwnPresentations())
  {
    return;
  }

  Handle(PrsMgr_Presentation) aPrs = Presentation (thePrsObj, theMode, Standard_True, theSelObj);
  if (aPrs->MustBeUpdated())
  {
    Update (thePrsObj, theMode);
  }

  if (myImmediateModeOn > 0)
  {
    Handle(Prs3d_PresentationShadow) aShadow = new Prs3d_PresentationShadow (myStructureManager, aPrs->Presentation());
    aShadow->SetZLayer (theImmediateStructLayerId);
    aShadow->SetClipPlanes (aPrs->Presentation()->ClipPlanes());
    aShadow->CStructure()->IsForHighlight = 1;
    aShadow->Highlight (theStyle);
    AddToImmediateList (aShadow);
  }
  else
  {
    aPrs->Highlight (theStyle);
  }
}

namespace
{
  // =======================================================================
  // function : updatePrsTransformation
  // purpose  : Internal function that scans thePrsList for shadow presentations
  //            and applies transformation theTrsf to them in case if parent ID
  //            of shadow presentation is equal to theRefId
  // =======================================================================
  void updatePrsTransformation (const PrsMgr_ListOfPresentations& thePrsList,
                                const Standard_Integer theRefId,
                                const Handle(Geom_Transformation)& theTrsf)
  {
    for (PrsMgr_ListOfPresentations::Iterator anIter (thePrsList); anIter.More(); anIter.Next())
    {
      const Handle(Prs3d_Presentation)& aPrs = anIter.Value();
      if (aPrs.IsNull())
        continue;

      Handle(Prs3d_PresentationShadow) aShadowPrs = Handle(Prs3d_PresentationShadow)::DownCast (aPrs);
      if (aShadowPrs.IsNull() || aShadowPrs->ParentId() != theRefId)
        continue;

      aShadowPrs->CStructure()->SetTransformation (theTrsf);
    }
  }
}

// =======================================================================
// function : UpdateHighlightTrsf
// purpose  :
// =======================================================================
void PrsMgr_PresentationManager::UpdateHighlightTrsf (const Handle(V3d_Viewer)& theViewer,
                                                      const Handle(PrsMgr_PresentableObject)& theObj,
                                                      const Standard_Integer theMode,
                                                      const Handle(PrsMgr_PresentableObject)& theSelObj)
{
  if (theObj.IsNull())
    return;

  Handle(PrsMgr_Presentation) aPrs = Presentation (!theSelObj.IsNull() ? theSelObj : theObj, theMode, Standard_False);
  if (aPrs.IsNull())
  {
    return;
  }

  Handle(Geom_Transformation) aTrsf = theObj->LocalTransformationGeom();
  const Standard_Integer aParentId = aPrs->Presentation()->CStructure()->Id;
  updatePrsTransformation (myImmediateList, aParentId, aTrsf);

  if (!myViewDependentImmediateList.IsEmpty())
  {
    for (V3d_ListOfViewIterator anActiveViewIter (theViewer->ActiveViewIterator()); anActiveViewIter.More(); anActiveViewIter.Next())
    {
      const Handle(Graphic3d_CView)& aView = anActiveViewIter.Value()->View();
      Handle(Graphic3d_Structure) aViewDepParentPrs;
      if (aView->IsComputed (aParentId, aViewDepParentPrs))
      {
        updatePrsTransformation (myViewDependentImmediateList,
                                 aViewDepParentPrs->CStructure()->Id,
                                 aTrsf);
      }
    }
  }
}
