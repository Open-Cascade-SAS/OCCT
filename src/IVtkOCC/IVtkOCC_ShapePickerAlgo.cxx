// Created on: 2011-10-14 
// Created by: Roman KOZLOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS 
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

#include <IVtk_Types.hxx>
#include <IVtkOCC_ShapePickerAlgo.hxx>
#include <IVtkOCC_Shape.hxx>
#include <IVtkOCC_SelectableObject.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <StdSelect_BRepOwner.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IVtkOCC_ShapePickerAlgo,IVtk_IShapePickerAlgo)

// Handle implementation


//================================================================
// Function : Constructor
// Purpose  : 
//================================================================
IVtkOCC_ShapePickerAlgo::IVtkOCC_ShapePickerAlgo() :
myViewerSelector (new IVtkOCC_ViewerSelector())
{ }

//================================================================
// Function : Destructor
// Purpose  : 
//================================================================
IVtkOCC_ShapePickerAlgo::~IVtkOCC_ShapePickerAlgo()
{ }

//================================================================
// Function : SetView
// Purpose  :
//================================================================
void IVtkOCC_ShapePickerAlgo::SetView (const IVtk_IView::Handle& theView)
{
  myView = theView;
}

//================================================================
// Function : GetSelectionModes
// Purpose  :
//================================================================
IVtk_SelectionModeList IVtkOCC_ShapePickerAlgo::GetSelectionModes (
                                  const IVtk_IShape::Handle& theShape) const
{
  IVtk_SelectionModeList aRes;

  if (! theShape.IsNull())
  {
    // Get shape implementation from shape interface.
    Handle(IVtkOCC_Shape) aShapeImpl = Handle(IVtkOCC_Shape)::DownCast(theShape);

    // Get selectable object from the shape implementation.
    Handle(IVtkOCC_SelectableObject) aSelObj = 
      Handle(IVtkOCC_SelectableObject)::DownCast(aShapeImpl->GetSelectableObject());

    if (!aSelObj.IsNull())
    {
      IVtk_SelectionMode aSelMode;
      for (aSelMode = SM_Shape; aSelMode <= SM_Compound; aSelMode = (IVtk_SelectionMode)(aSelMode + 1))
      {
        if (myViewerSelector->IsActive (aSelObj, aSelMode))
        {
          aRes.Append (aSelMode);
        }
      }
    }
  }

  return aRes;
}

//================================================================
// Function : SetSelectionMode
// Purpose  :
//================================================================
void IVtkOCC_ShapePickerAlgo::SetSelectionMode (const IVtk_IShape::Handle& theShape,
                                                const IVtk_SelectionMode theMode,
                                                const bool theIsTurnOn)
{
  if (theShape.IsNull())
  {
    return;
  }

  // TODO: treatment for mode == -1 - deactivate the shape...
  // Is this really needed? The picker and all selection classes
  // are destroyed when shapes are deactivated...

  // Get shape implementation from shape interface.
  Handle(IVtkOCC_Shape) aShapeImpl = 
    Handle(IVtkOCC_Shape)::DownCast(theShape);

  // Get selectable object from the shape implementation.
  Handle(IVtkOCC_SelectableObject) aSelObj = 
    Handle(IVtkOCC_SelectableObject)::DownCast(aShapeImpl->GetSelectableObject());

  if (theIsTurnOn)
  {
    // If there is no selectable object then create a new one for this shape.
    if (aSelObj.IsNull())
    {
      aSelObj = new IVtkOCC_SelectableObject (aShapeImpl);
    }

    // If the selectable object has no selection in the given mode
    if (!aSelObj->HasSelection (theMode))
    {
      // then create a new selection in the given mode for this object (shape).
      Handle(SelectMgr_Selection) aNewSelection = new SelectMgr_Selection (theMode); 
      aSelObj->AddSelection (aNewSelection, theMode);
      myViewerSelector->AddSelectionToObject (aSelObj, aNewSelection);
    }

    // Update the selection for the given mode according to its status.
    const Handle(SelectMgr_Selection)& aSel = aSelObj->Selection (theMode);

    switch (aSel->UpdateStatus())
    {
      case SelectMgr_TOU_Full:
        // Recompute the sensitive primitives which correspond to the mode.
        myViewerSelector->RemoveSelectionOfObject (aSelObj, aSelObj->Selection (theMode));
        aSelObj->RecomputePrimitives (theMode);
        myViewerSelector->AddSelectionToObject (aSelObj, aSelObj->Selection (theMode));
        myViewerSelector->RebuildObjectsTree();
        myViewerSelector->RebuildSensitivesTree (aSelObj);
      case SelectMgr_TOU_Partial:
        {
          if (aSelObj->HasTransformation())
          {
            myViewerSelector->RebuildObjectsTree();
          }
          break;
        }
      default:
        break;
    }
    // Set status of the selection to "nothing to update".
    aSel->UpdateStatus (SelectMgr_TOU_None);

    // Activate the selection in the viewer selector.
    myViewerSelector->Activate (aSelObj->Selection (theMode));

  }
  else
  { // turn off the selection mode

    if (!aSelObj.IsNull())
    {
      if (aSelObj->HasSelection (theMode))
      {
        const Handle(SelectMgr_Selection)& aSel = aSelObj->Selection (theMode);
        myViewerSelector->Deactivate (aSel);
      }
    }
  }
}

//================================================================
// Function : SetSelectionMode
// Purpose  :
//================================================================
void IVtkOCC_ShapePickerAlgo::SetSelectionMode (const IVtk_ShapePtrList& theShapes,
                                                const IVtk_SelectionMode theMode,
                                                const bool /*theIsTurnOn*/)
{
  IVtk_IShape::Handle aShape;
  IVtk_ShapePtrList::Iterator anIt (theShapes);
  for (; anIt.More(); anIt.Next())
  {
    aShape = anIt.Value();
    SetSelectionMode (aShape, theMode);
  }
}

//================================================================
// Function : Pick
// Purpose  :
//================================================================
bool IVtkOCC_ShapePickerAlgo::Pick (const double theX, const double theY)
{
  clearPicked();

  // Calling OCCT algortihm
  myViewerSelector->Pick ((Standard_Integer)theX,
                          (Standard_Integer)theY,
                          myView);

  // Fill the results
  return processPicked();
}

//================================================================
// Function : Pick
// Purpose  :
//================================================================
bool IVtkOCC_ShapePickerAlgo::Pick (const double theXMin,
                                    const double theYMin,
                                    const double theXMax,
                                    const double theYMax)
{
  clearPicked();

  // Calling OCCT algortihm
  myViewerSelector->Pick ((Standard_Integer)theXMin,
                          (Standard_Integer)theYMin,
                          (Standard_Integer)theXMax,
                          (Standard_Integer)theYMax,
                          myView);

  // Fill the results
  return processPicked();
}

//================================================================
// Function : Pick
// Purpose  :
//================================================================
bool IVtkOCC_ShapePickerAlgo::Pick (double** thePoly,
                                    const int theNbPoints)
{
  clearPicked();

  // Calling OCCT algortihm
  myViewerSelector->Pick (thePoly, theNbPoints, myView);

  // Fill the results
  return processPicked();
}

//================================================================
// Function : ShapesPicked
// Purpose  :
//================================================================
const IVtk_ShapeIdList& IVtkOCC_ShapePickerAlgo::ShapesPicked() const
{
  return myShapesPicked;
}

//================================================================
// Function : SubShapesPicked
// Purpose  :
//================================================================
void IVtkOCC_ShapePickerAlgo::SubShapesPicked (const IVtk_IdType theId, IVtk_ShapeIdList& theShapeList) const
{
  if (mySubShapesPicked.IsBound (theId))
  {
    theShapeList = mySubShapesPicked (theId);
  }
}

//================================================================
// Function : clearPicked
// Purpose  : Internal method, resets picked data
//================================================================
void IVtkOCC_ShapePickerAlgo::clearPicked()
{
  myShapesPicked.Clear();
  mySubShapesPicked.Clear();
}

//================================================================
// Function : NbPicked
// Purpose  : Get number of picked entities.
//================================================================
int IVtkOCC_ShapePickerAlgo::NbPicked()
{
  return myShapesPicked.Extent();
}

//================================================================
// Function : processPicked
// Purpose  :
//================================================================
bool IVtkOCC_ShapePickerAlgo::processPicked()
{
  Standard_Integer aNbPicked =  myViewerSelector->NbPicked();
  Handle(StdSelect_BRepOwner) anEntityOwner;
  Handle(Message_Messenger) anOutput = Message::DefaultMessenger();

  for (Standard_Integer aDetectIt = 1; aDetectIt <= aNbPicked; aDetectIt++)
  {
    // ViewerSelector detects sensitive entities under the mouse
    // and for each entity returns its entity owner.
    // StdSelect_BRepOwner instance holds corresponding sub-shape (TopoDS_Shape)
    // and in general entity owners have a pointer to SelectableObject that can tell us
    // what is the top-level TopoDS_Shape.
    anEntityOwner = Handle(StdSelect_BRepOwner)::DownCast (myViewerSelector->Picked (aDetectIt));
    if (!anEntityOwner.IsNull())
    {
      Handle(IVtkOCC_SelectableObject) aSelectable =
        Handle(IVtkOCC_SelectableObject)::DownCast (anEntityOwner->Selectable());

      if (aSelectable.IsNull())
      {
        anOutput << "Error: EntityOwner having null SelectableObject picked!";
        continue;
      }

      Handle(IVtkOCC_Shape) aSelShape = aSelectable->GetShape();
      if (aSelShape.IsNull())
      {
        anOutput << "Error: SelectableObject with null OccShape pointer picked!";
        continue;
      }

      IVtk_IdType aTopLevelId = aSelShape->GetId();
      myShapesPicked.Append (aTopLevelId);

      // Now try to guess if it's the top-level shape itself or just a sub-shape picked
      TopoDS_Shape aTopLevelShape = aSelShape->GetShape();
      TopoDS_Shape aSubShape      = anEntityOwner->Shape();
      if (aTopLevelShape.IsNull())
      {
        anOutput << "Error: OccShape with null top-level TopoDS_Shape picked!";
        continue;
      }
      if (aSubShape.IsNull())
      {
        anOutput << "Error: EntityOwner with null TopoDS_Shape picked!";
        continue;
      }

      if (!aSubShape.IsSame (aTopLevelShape))
      {
        IVtk_IdType aSubId = aSelShape->GetSubShapeId (aSubShape);

        if (!mySubShapesPicked.IsBound (aTopLevelId))
        {
          const IVtk_ShapeIdList aList;
          mySubShapesPicked.Bind (aTopLevelId, aList);
        }
        // Order of selected sub-shapes
        mySubShapesPicked (aTopLevelId).Append (aSubId);
      }
    }
  }

  return !myShapesPicked.IsEmpty();
}

//============================================================================
//  Method: RemoveSelectableActor
// Purpose: Remove selectable object from the picker (from internal maps).
//============================================================================
void IVtkOCC_ShapePickerAlgo::RemoveSelectableObject(const IVtk_IShape::Handle& theShape)
{
  clearPicked();
  // Get shape implementation from shape interface.
  Handle(IVtkOCC_Shape) aShapeImpl =
    Handle(IVtkOCC_Shape)::DownCast(theShape);

  // Get selectable object from the shape implementation.
  Handle(IVtkOCC_SelectableObject) aSelObj =
    Handle(IVtkOCC_SelectableObject)::DownCast(aShapeImpl->GetSelectableObject());

  myViewerSelector->RemoveSelectableObject(aSelObj);
  myViewerSelector->Clear();
  aShapeImpl->SetSelectableObject(NULL);
}