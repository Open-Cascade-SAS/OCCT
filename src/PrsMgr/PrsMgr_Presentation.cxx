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

#include <PrsMgr_Presentation.hxx>

#include <Graphic3d_DataStructureManager.hxx>
#include <Precision.hxx>
#include <Prs3d_Drawer.hxx>
#include <PrsMgr_PresentableObject.hxx>
#include <PrsMgr_PresentationManager.hxx>
#include <Quantity_Color.hxx>
#include <Graphic3d_CView.hxx>

IMPLEMENT_STANDARD_RTTIEXT(PrsMgr_Presentation, Graphic3d_Structure)

namespace
{
  enum BeforeHighlightState
  {
    State_Empty,
    State_Hidden,
    State_Visible
  };

  static BeforeHighlightState StructureState (const Graphic3d_Structure* theStructure)
  {
    return !theStructure->IsDisplayed() ?
      State_Empty : !theStructure->IsVisible() ?
        State_Hidden : State_Visible;
  }
}

//=======================================================================
//function : PrsMgr_Presentation
//purpose  :
//=======================================================================
PrsMgr_Presentation::PrsMgr_Presentation (const Handle(PrsMgr_PresentationManager3d)& thePrsMgr,
                                          const Handle(PrsMgr_PresentableObject)& thePrsObject,
                                          const Standard_Integer theMode)
: Graphic3d_Structure (thePrsMgr->StructureManager()),
  myPresentationManager  (thePrsMgr),
  myPresentableObject    (thePrsObject.get()),
  myBeforeHighlightState (State_Empty),
  myMode                 (theMode),
  myMustBeUpdated        (Standard_False)
{
  if (thePrsObject->TypeOfPresentation3d() == PrsMgr_TOP_ProjectorDependant)
  {
    SetVisual (Graphic3d_TOS_COMPUTED);
  }
  SetOwner (myPresentableObject);
  SetMutable (myPresentableObject->IsMutable());
}

//=======================================================================
//function : Display
//purpose  :
//=======================================================================
void PrsMgr_Presentation::Display()
{
  display (Standard_False);
  myBeforeHighlightState = State_Visible;
}

//=======================================================================
//function : display
//purpose  :
//=======================================================================
void PrsMgr_Presentation::display (const Standard_Boolean theIsHighlight)
{
  if (!base_type::IsDisplayed())
  {
    base_type::SetIsForHighlight (theIsHighlight); // optimization - disable frustum culling for this presentation
    base_type::Display();
  }
  else if (!base_type::IsVisible())
  {
    base_type::SetVisible (Standard_True);
  }
}

//=======================================================================
//function : Erase
//purpose  :
//=======================================================================
void PrsMgr_Presentation::Erase()
{
  if (IsDeleted())
  {
    return;
  }

  // Erase structure from structure manager
  erase();
  clear (true);
  // Disconnect other structures
  DisconnectAll (Graphic3d_TOC_DESCENDANT);
  // Clear groups and remove graphic structure
  Remove();
}

//=======================================================================
//function : Highlight
//purpose  :
//=======================================================================
void PrsMgr_Presentation::Highlight (const Handle(Prs3d_Drawer)& theStyle)
{
  if (!IsHighlighted())
  {
    myBeforeHighlightState = StructureState (this);
  }

  display (Standard_True);
  base_type::Highlight (theStyle);
}

//=======================================================================
//function : Unhighlight
//purpose  :
//=======================================================================
void PrsMgr_Presentation::Unhighlight()
{
  base_type::UnHighlight();
  switch (myBeforeHighlightState)
  {
    case State_Visible:
      return;
    case State_Hidden:
      base_type::SetVisible (Standard_False);
      break;
    case State_Empty:
      base_type::erase();
      break;
  }
}

//=======================================================================
//function : Clear
//purpose  :
//=======================================================================
void PrsMgr_Presentation::Clear (const Standard_Boolean theWithDestruction)
{
  // This modification remove the contain of the structure:
  // Consequence:
  //    1. The memory zone of the group is reused
  //    2. The speed for animation is constant
  //myPresentableObject = NULL;
  SetUpdateStatus (Standard_True);
  if (IsDeleted())
  {
    return;
  }

  clear (theWithDestruction);
  DisconnectAll (Graphic3d_TOC_DESCENDANT);
}

//=======================================================================
//function : Compute
//purpose  :
//=======================================================================
void PrsMgr_Presentation::Compute()
{
  Standard_Integer aDispMode = 0;
  for (PrsMgr_Presentations::Iterator aPrsIter (myPresentableObject->myPresentations); aPrsIter.More(); aPrsIter.Next())
  {
    const Handle(PrsMgr_Presentation)& aModedPresentation = aPrsIter.Value();
    if (aModedPresentation == this)
    {
      aDispMode = aModedPresentation->Mode();
      break;
    }
  }

  myPresentableObject->Compute (myPresentationManager, this, aDispMode);
}

//=======================================================================
//function : Compute
//purpose  :
//=======================================================================
void PrsMgr_Presentation::computeHLR (const Handle(Graphic3d_Camera)& theProjector,
                                      Handle(Graphic3d_Structure)& theStructToFill)
{
  if (theStructToFill.IsNull())
  {
    theStructToFill = new Prs3d_Presentation (myPresentationManager->StructureManager());
  }
  Handle(Graphic3d_Structure) aPrs = theStructToFill;
  theStructToFill->Clear();
  myPresentableObject->computeHLR (theProjector, Transformation(), aPrs);
}

//=======================================================================
//function : ~PrsMgr_Presentation
//purpose  :
//=======================================================================
PrsMgr_Presentation::~PrsMgr_Presentation()
{
  Erase();
}

// =======================================================================
// function : DumpJson
// purpose  :
// =======================================================================
void PrsMgr_Presentation::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)
  OCCT_DUMP_BASE_CLASS (theOStream, theDepth, Graphic3d_Structure)

  OCCT_DUMP_FIELD_VALUE_POINTER (theOStream, myPresentableObject)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myBeforeHighlightState)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myMode)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myMustBeUpdated)
}
