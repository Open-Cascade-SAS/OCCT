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

#include <PrsMgr_PresentationManager.ixx>

#include <Graphic3d_GraphicDriver.hxx>
#include <Prs3d_PresentationShadow.hxx>
#include <PrsMgr_PresentableObject.hxx>
#include <PrsMgr_Presentation.hxx>
#include <PrsMgr_Presentations.hxx>
#include <PrsMgr_ModedPresentation.hxx>
#include <TColStd_ListIteratorOfListOfTransient.hxx>
#include <V3d_View.hxx>
#include <Visual3d_View.hxx>
#include <Visual3d_Layer.hxx>

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
    if (!HasPresentation (thePrsObj, theMode))
    {
      AddPresentation (thePrsObj, theMode);
    }

    Handle(PrsMgr_Presentation) aPrs = Presentation (thePrsObj, theMode);

    if (aPrs.IsNull()) return;

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

  if (HasPresentation (thePrsObj, theMode))
  {
    Presentation (thePrsObj, theMode)->Erase();
    RemovePresentation (thePrsObj, theMode);
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

  if (HasPresentation (thePrsObj, theMode))
  {
    Presentation (thePrsObj, theMode)->Clear();
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

  Presentation (thePrsObj, theMode)->SetVisible (theValue);
}

// =======================================================================
// function : Highlight
// purpose  :
// =======================================================================
void PrsMgr_PresentationManager::Highlight (const Handle(PrsMgr_PresentableObject)& thePrsObj,
                                            const Standard_Integer                  theMode)
{
  for (PrsMgr_ListOfPresentableObjectsIter anIter (thePrsObj->Children()); anIter.More(); anIter.Next())
  {
    Highlight (anIter.Value(), theMode);
  }
  if (!thePrsObj->HasOwnPresentations())
  {
    return;
  }

  if (!HasPresentation (thePrsObj, theMode))
  {
    AddPresentation (thePrsObj, theMode);
  }

  if (!HasPresentation (thePrsObj, theMode)) return;

  Handle(PrsMgr_Presentation) aPrs = Presentation (thePrsObj, theMode);
  if (aPrs->MustBeUpdated())
  {
    Update (thePrsObj, theMode);
  }

  if (myImmediateModeOn > 0)
  {
    Handle(Prs3d_PresentationShadow) aShadow = new Prs3d_PresentationShadow (myStructureManager, aPrs->Presentation());
    aShadow->Highlight();
    AddToImmediateList (aShadow);
  }
  else
  {
    aPrs->Highlight();
  }
}

// =======================================================================
// function : Unhighlight
// purpose  :
// =======================================================================
void PrsMgr_PresentationManager::Unhighlight (const Handle(PrsMgr_PresentableObject)& thePrsObj,
                                              const Standard_Integer                  theMode)
{
  for (PrsMgr_ListOfPresentableObjectsIter anIter (thePrsObj->Children()); anIter.More(); anIter.Next())
  {
    Unhighlight (anIter.Value(), theMode);
  }

  if (HasPresentation (thePrsObj, theMode))
  {
    Presentation (thePrsObj, theMode)->Unhighlight();
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

  if (HasPresentation (thePrsObj, theMode))
  {
    Presentation (thePrsObj, theMode)->SetDisplayPriority (theNewPrior);
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

  return HasPresentation (thePrsObj, theMode)
       ? Presentation (thePrsObj, theMode)->DisplayPriority()
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

  return HasPresentation (thePrsObj, theMode)
      && Presentation    (thePrsObj, theMode)->IsDisplayed();
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

  return HasPresentation (thePrsObj, theMode)
      && Presentation    (thePrsObj, theMode)->IsHighlighted();
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
  if (!HasPresentation(thePrsObj, theMode))
  {
    return;
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
  if (myImmediateView.IsNull())
  {
    myImmediateList.Clear();
    return;
  }

  for (PrsMgr_ListOfPresentations::Iterator anIter (myImmediateList); anIter.More(); anIter.Next())
  {
    myImmediateView->View()->EraseImmediate (anIter.Value());
  }

  myImmediateList.Clear();
  myImmediateView.Nullify();
}

// =======================================================================
// function : EndImmediateDraw
// purpose  :
// =======================================================================
void PrsMgr_PresentationManager::EndImmediateDraw (const Handle(V3d_View)& theView)
{
  if (--myImmediateModeOn > 0)
  {
    return;
  }

  for (PrsMgr_ListOfPresentations::Iterator anIter (myImmediateList); anIter.More(); anIter.Next())
  {
    theView->View()->DisplayImmediate (anIter.Value(), Standard_True);
  }
  if (!myImmediateList.IsEmpty())
  {
    myImmediateView = theView;
  }
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
                                                                      const Standard_Integer                  theMode) const
{
  const PrsMgr_Presentations& aPrsList = thePrsObj->Presentations();
  if (aPrsList.IsEmpty())
  {
    return Handle(PrsMgr_Presentation)();
  }

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

  return Handle(PrsMgr_Presentation)();
}

// =======================================================================
// function : AddPresentation
// purpose  :
// =======================================================================
void PrsMgr_PresentationManager::AddPresentation (const Handle(PrsMgr_PresentableObject)& thePrsObj,
                                                  const Standard_Integer                  theMode)
{
  Handle(PrsMgr_Presentation) aPrs = new PrsMgr_Presentation (this, thePrsObj);
  thePrsObj->Presentations().Append (PrsMgr_ModedPresentation (aPrs, theMode));
  thePrsObj->Fill (this, aPrs, theMode);

  // set layer index accordingly to object's presentations
  const Standard_Integer aZLayerId = GetZLayer (thePrsObj);
  if (aZLayerId >= 0)
  {
    aPrs->SetZLayer (aZLayerId);
  }
  aPrs->SetUpdateStatus (Standard_False);
}

// =======================================================================
// function : RemovePresentation
// purpose  :
// =======================================================================
void PrsMgr_PresentationManager::RemovePresentation (const Handle(PrsMgr_PresentableObject)& thePrsObj,
                                                     const Standard_Integer                  theMode)
{
  PrsMgr_Presentations& aPrsList = thePrsObj->Presentations();
  for (Standard_Integer aPrsIter = 1; aPrsIter <= aPrsList.Length(); ++aPrsIter)
  {
    if (theMode == aPrsList (aPrsIter).Mode())
    //  && this    == aPrsMgr) ??
    {
      aPrsList.Remove (aPrsIter);
      break;
    }
  }
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
  PrsMgr_Presentations& aPrsList = thePrsObj->Presentations();
  for (Standard_Integer aPrsIter = 1; aPrsIter <= aPrsList.Length(); ++aPrsIter)
  {
    Handle(PrsMgr_Presentation) aPrs = aPrsList.ChangeValue (aPrsIter).Presentation();
    if (aPrs->PresentationManager() == this)
    {
      aPrs->SetZLayer (theLayerId);
    }
  }
}

// =======================================================================
// function : GetZLayer
// purpose  :
// =======================================================================
Standard_Integer PrsMgr_PresentationManager::GetZLayer (const Handle(PrsMgr_PresentableObject)& thePrsObj) const
{
  for (PrsMgr_ListOfPresentableObjectsIter anIter (thePrsObj->Children()); anIter.More(); anIter.Next())
  {
    Standard_Integer aLayer = GetZLayer (anIter.Value());
    if (aLayer != -1)
    {
      return aLayer;
    }
  }
  if (!thePrsObj->HasOwnPresentations())
  {
    return -1;
  }
  const PrsMgr_Presentations& aPrsList = thePrsObj->Presentations();
  for (Standard_Integer aPrsIter = 1; aPrsIter <= aPrsList.Length(); ++aPrsIter)
  {
    Handle(PrsMgr_Presentation) aPrs = aPrsList.Value (aPrsIter).Presentation();
    if (aPrs->PresentationManager() == this)
    {
      return aPrs->GetZLayer();
    }
  }
  return -1;
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
  if (!HasPresentation (thePrsObject, theMode))
  {
    AddPresentation (thePrsObject, theMode);
  }
  if (!HasPresentation (theOtherObject, theOtherMode))
  {
    AddPresentation (theOtherObject, theOtherMode);
  }
  Presentation (thePrsObject, theMode)->Connect (Presentation (theOtherObject, theMode));
}

// =======================================================================
// function : Transform
// purpose  :
// =======================================================================
void PrsMgr_PresentationManager::Transform (const Handle(PrsMgr_PresentableObject)& thePrsObj,
                                            const Handle(Geom_Transformation)&      theTransformation,
                                            const Standard_Integer                  theMode)
{
  Presentation (thePrsObj, theMode)->Transform (theTransformation);
}


// =======================================================================
// function : Color
// purpose  :
// =======================================================================
void PrsMgr_PresentationManager::Color (const Handle(PrsMgr_PresentableObject)& thePrsObj,
                                        const Quantity_NameOfColor              theColor,
                                        const Standard_Integer                  theMode)
{
  for (PrsMgr_ListOfPresentableObjectsIter anIter (thePrsObj->Children()); anIter.More(); anIter.Next())
  {
    Color (anIter.Value(), theColor, theMode);
  }
  if (!thePrsObj->HasOwnPresentations())
  {
    return;
  }

  if (!HasPresentation (thePrsObj, theMode))
  {
    AddPresentation (thePrsObj, theMode);
  }

  if (!HasPresentation (thePrsObj, theMode)) return;

  Handle(PrsMgr_Presentation) aPrs = Presentation (thePrsObj, theMode);
  if (aPrs->MustBeUpdated())
  {
    Update (thePrsObj, theMode);
  }

  if (myImmediateModeOn > 0)
  {
    Handle(Prs3d_PresentationShadow) aShadow = new Prs3d_PresentationShadow (myStructureManager, aPrs->Presentation());
    aShadow->Color (theColor);
    AddToImmediateList (aShadow);
  }
  else
  {
    aPrs->Color (theColor);
  }
}

// =======================================================================
// function : BoundBox
// purpose  :
// =======================================================================
void PrsMgr_PresentationManager::BoundBox (const Handle(PrsMgr_PresentableObject)& thePrsObject,
                                           const Standard_Integer                  theMode)
{
  if (!HasPresentation (thePrsObject, theMode))
  {
    AddPresentation (thePrsObject, theMode);
  }
  else if (Presentation (thePrsObject, theMode)->MustBeUpdated())
  {
    Update (thePrsObject, theMode);
  }
  Presentation (thePrsObject, theMode)->BoundBox();
}

// =======================================================================
// function : SetShadingAspect
// purpose  :
// =======================================================================
void PrsMgr_PresentationManager::SetShadingAspect (const Handle(PrsMgr_PresentableObject)& thePrsObject,
                                                   const Quantity_NameOfColor              theColor,
                                                   const Graphic3d_NameOfMaterial          theMaterial,
                                                   const Standard_Integer                  theMode)
{
  Handle(Prs3d_ShadingAspect) anAspect = new Prs3d_ShadingAspect();
  anAspect->SetColor    (theColor);
  anAspect->SetMaterial (theMaterial);
  SetShadingAspect (thePrsObject, anAspect, theMode);
}

// =======================================================================
// function : SetShadingAspect
// purpose  :
// =======================================================================
void PrsMgr_PresentationManager::SetShadingAspect (const Handle(PrsMgr_PresentableObject)& thePrsObject,
                                                   const Handle(Prs3d_ShadingAspect)&      theShadingAspect,
                                                   const Standard_Integer                  theMode)
{
  if (HasPresentation (thePrsObject, theMode))
  {
    Presentation (thePrsObject, theMode)->SetShadingAspect (theShadingAspect);
  }
}
