// Created on: 2011-10-27 
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

#include <IVtkTools_ShapePicker.hxx>
#include <IVtkTools_ShapeObject.hxx>
#include <IVtkVTK_View.hxx>
#include <vtkCommand.h>
#include <vtkObjectFactory.h>
#include <vtkRenderer.h>
#include <vtkActorCollection.h>

//! @class IVtkTools_ShapePicker
//! VTK picker implementation for OCCT shapes.
//! Can pick either whole shapes or sub-shapes.
//! The kind of selectable entities is defined by the current selection mode.
//! NOTE: For performance reasons, setRenderer() method should be called in advance,
//! before the user starts to select interactively, in order for the OCCT selection
//! algorithm to prepare its internal selection data.

vtkStandardNewMacro(IVtkTools_ShapePicker)

//============================================================================
//  Method: IVtkTools_ShapePicker
// Purpose: Constructs the picker with empty renderer and ready for point selection.
//============================================================================
IVtkTools_ShapePicker::IVtkTools_ShapePicker()
: myRenderer (NULL),
  myIsRectSelection (false)
{
  myOccPickerAlgo = new IVtkOCC_ShapePickerAlgo();
}

//============================================================================
//  Method: ~IVtkTools_ShapePicker
// Purpose: Destructor
//============================================================================
IVtkTools_ShapePicker::~IVtkTools_ShapePicker()
{
}

//============================================================================
//  Method: SetTolerance
// Purpose: Setter for tolerance of picking.
//============================================================================
void IVtkTools_ShapePicker::SetTolerance (float theTolerance )
{
  myTolerance = theTolerance;
}

//============================================================================
//  Method: GetTolerance
// Purpose: Getter for tolerance of picking.
//============================================================================
float IVtkTools_ShapePicker::GetTolerance( ) const
{
  return myTolerance;
}

//============================================================================
//  Method: convertDisplayToWorld
// Purpose: Convert display coordinates to world coordinates
//============================================================================
bool IVtkTools_ShapePicker::convertDisplayToWorld (vtkRenderer *theRenderer,
                                                   double theDisplayCoord[3],
                                                   double theWorldCoord[3])
{
  // Convert the selection point into world coordinates.
  theRenderer->SetDisplayPoint (theDisplayCoord[0], theDisplayCoord[1], theDisplayCoord[2]);
  theRenderer->DisplayToWorld();

  double aCoords[4];
  theRenderer->GetWorldPoint(aCoords);
  if (aCoords[3] == 0.0)
  {
    return false;
  }

  for (Standard_Integer anI = 0; anI < 3; anI++)
  {
    theWorldCoord[anI] = aCoords[anI] / aCoords[3];
  }
  
  return true;
}

//============================================================================
// Method:  Pick
// Purpose: Pick entities in the given point.
//============================================================================
int IVtkTools_ShapePicker::Pick (double theX, double theY, double /*theZ*/, vtkRenderer *theRenderer)
{
  double aPos[2] = {theX, theY};
  myIsRectSelection = false;
  myIsPolySelection = false;
  return pick (aPos, theRenderer);
}

//============================================================================
//  Method: pick
// Purpose: Pick entities in the given rectangle area.
//============================================================================
int IVtkTools_ShapePicker::Pick (double theXPMin, double theYPMin, double theXPMax, double theYPMax,
                                 vtkRenderer *theRenderer)
{
  double aPos[4] = {theXPMin, theYPMin, theXPMax, theYPMax};
  myIsRectSelection = true;
  myIsPolySelection = false;
  return pick (aPos, theRenderer);
}
//============================================================================
//  Method: pick
// Purpose: Pick entities in the given polygonal area.
//============================================================================
int IVtkTools_ShapePicker::Pick (double thePoly[][3], const int theNbPoints,
                                 vtkRenderer *theRenderer)
{
  myIsRectSelection = false;
  myIsPolySelection = true;
  return pick ((double*)thePoly, theRenderer, theNbPoints);
}

//============================================================================
//  Method: pick
// Purpose: Pick entities in the given point or area.
//============================================================================
int IVtkTools_ShapePicker::pick (double* thePos,
                                 vtkRenderer *theRenderer,
                                 const int theNbPoints)
{
 //  Initialize picking process
  Initialize();

  // Emit StartPickEvent for observer callbacks (if any)
  InvokeEvent(vtkCommand::StartPickEvent, NULL);

  vtkSmartPointer<vtkRenderer> aRenderer;
  if (theRenderer == NULL)
  {
    aRenderer = myRenderer; // by default use own renderer
  }
  else
  {
    aRenderer = theRenderer;
  }
  doPickImpl (thePos, aRenderer, theNbPoints);

  // Emit EndPickEvent for observer callbacks (if any)
  InvokeEvent(vtkCommand::EndPickEvent, NULL);

  return myOccPickerAlgo->NbPicked();
}

//============================================================================
//  Method: doPickImpl
// Purpose: Implementation of picking algorithm.
//============================================================================
void IVtkTools_ShapePicker::doPickImpl (double* thePos,
                                        vtkRenderer* theRenderer, 
                                        const int theNbPoints)
{
  // Make sure the correct renderer is used
  SetRenderer (theRenderer);

  if (myIsPolySelection)
  {
    myOccPickerAlgo->Pick ((double**)thePos, theNbPoints);
  }
  else if (myIsRectSelection)
  {
    myOccPickerAlgo->Pick (thePos[0], thePos[1], thePos[2], thePos[3]);
  }
  else
  {
    myOccPickerAlgo->Pick (thePos[0], thePos[1]);
  }
}

//============================================================================
//  Method: SetRenderer
// Purpose: Sets the renderer to be used by OCCT selection algorithm
//============================================================================
void IVtkTools_ShapePicker::SetRenderer (vtkRenderer* theRenderer)
{
  if (theRenderer == myRenderer.GetPointer())
  {
    return;
    // In this case we should not do anything.
    // In the worth case we need to update picker algorithm (view er selector and projection options)
    // If any needs this , call myOccPickerAlgo->Modified();
  }

  myRenderer = theRenderer;
  IVtkVTK_View::Handle aView = new IVtkVTK_View (myRenderer);
  myOccPickerAlgo->SetView (aView);
}

//============================================================================
//  Method: SetAreaSelection
// Purpose: Sets area selection on/off
//============================================================================
void IVtkTools_ShapePicker::SetAreaSelection (bool theIsOn)
{
  myIsRectSelection = theIsOn;
}

//============================================================================
//  Method: GetSelectionModes
// Purpose: Get activated selection modes for a shape.
//============================================================================
IVtk_SelectionModeList IVtkTools_ShapePicker::GetSelectionModes (
                                              const IVtk_IShape::Handle& theShape) const
{
  return myOccPickerAlgo->GetSelectionModes (theShape);
}

//============================================================================
//  Method: GetSelectionModes
// Purpose: Get activated selection modes for a shape actor.
//============================================================================
IVtk_SelectionModeList IVtkTools_ShapePicker::GetSelectionModes (
                                              vtkActor* theShapeActor) const
{
  IVtk_SelectionModeList aRes;
  IVtk_IShape::Handle aShape = IVtkTools_ShapeObject::GetOccShape (theShapeActor);
  if (!aShape.IsNull())
  {
    aRes = myOccPickerAlgo->GetSelectionModes (aShape);
  }
  return aRes;
}

//============================================================================
//  Method: SetSelectionMode
// Purpose: Turn on/off a selection mode for a shape.
//============================================================================
void IVtkTools_ShapePicker::SetSelectionMode (const IVtk_IShape::Handle& theShape,
                                              const IVtk_SelectionMode   theMode,
                                              const bool                 theIsTurnOn) const
{
  myOccPickerAlgo->SetSelectionMode (theShape, theMode, theIsTurnOn);
}

//============================================================================
//  Method: SetSelectionMode
// Purpose: Turn on/off a selection mode for a shape actor.
//============================================================================
void IVtkTools_ShapePicker::SetSelectionMode (vtkActor*                theShapeActor,
                                              const IVtk_SelectionMode theMode,
                                              const bool               theIsTurnOn) const
{
  IVtk_IShape::Handle aShape = IVtkTools_ShapeObject::GetOccShape (theShapeActor);
  if (!aShape.IsNull())
  {
    myOccPickerAlgo->SetSelectionMode (aShape, theMode, theIsTurnOn);
  }
}

//============================================================================
//  Method: SetSelectionMode
// Purpose: Sets the current selection mode for all visible shape objects.
//============================================================================
void IVtkTools_ShapePicker::SetSelectionMode (const IVtk_SelectionMode theMode, 
                                              const bool               theIsTurnOn) const
{
  if (myRenderer.GetPointer() != NULL)
  {
    // Obtain all OccShapes displayed and activate the specified selection mode
    vtkSmartPointer<vtkActorCollection> anActors = myRenderer->GetActors();
    anActors->InitTraversal();
    vtkSmartPointer<vtkActor> anActor = anActors->GetNextActor();
    while ( anActor.GetPointer() != NULL )
    {
      if (anActor->GetPickable() && anActor->GetVisibility())
      {
        if (anActor->GetMapper())
        {
          IVtk_IShape::Handle aShape = IVtkTools_ShapeObject::GetOccShape (anActor);
          if (!aShape.IsNull())
          {
            myOccPickerAlgo->SetSelectionMode (aShape, theMode, theIsTurnOn);
          }
        }
      }
      anActor = anActors->GetNextActor();
    }
  }
}

//============================================================================
//  Method: GetPickedShapesIds
// Purpose: Access to the list of top-level shapes picked.
//============================================================================
IVtk_ShapeIdList IVtkTools_ShapePicker::GetPickedShapesIds (bool theIsAll) const
{
  if (theIsAll || myIsRectSelection )
  {
    return myOccPickerAlgo->ShapesPicked();
  }

  IVtk_ShapeIdList aRes;
  IVtk_ShapeIdList aPicked = myOccPickerAlgo->ShapesPicked();
  if (!aPicked.IsEmpty())
  {
    aRes.Append (aPicked.First());
  }
  return aRes;
}

//============================================================================
//  Method: RemoveSelectableActor
// Purpose: Remove selectable object from the picker (from internal maps).
//============================================================================
void IVtkTools_ShapePicker::RemoveSelectableObject(const IVtk_IShape::Handle& theShape)
{
  myOccPickerAlgo->RemoveSelectableObject(theShape);
}

//============================================================================
//  Method: RemoveSelectableActor
// Purpose: Remove selectable object from the picker (from internal maps).
//============================================================================
void IVtkTools_ShapePicker::RemoveSelectableActor(vtkActor* theShapeActor)
{
  IVtk_IShape::Handle aShape = IVtkTools_ShapeObject::GetOccShape(theShapeActor);
  if (!aShape.IsNull())
  {
    RemoveSelectableObject(aShape);
  }
}

//============================================================================
//  Method: GetPickedSubShapesIds
// Purpose: Access to the list of sub-shapes ids picked.
//============================================================================
IVtk_ShapeIdList IVtkTools_ShapePicker::GetPickedSubShapesIds (const IVtk_IdType theId, bool theIsAll) const
{
  IVtk_ShapeIdList aRes;
  if (theIsAll)
  {
    myOccPickerAlgo->SubShapesPicked (theId, aRes);
  }
  else
  {
    IVtk_ShapeIdList aList;
    myOccPickerAlgo->SubShapesPicked (theId, aList);
    if (!aList.IsEmpty())
    {
      aRes.Append (aList.First());
    }
  }
  return aRes;
}

//============================================================================
//  Method: GetPickedActors
// Purpose: Access to the list of actors picked.
//============================================================================
vtkSmartPointer<vtkActorCollection> IVtkTools_ShapePicker::GetPickedActors (bool theIsAll) const
{
  vtkSmartPointer<vtkActorCollection> aRes = vtkSmartPointer<vtkActorCollection>::New();
  IVtk_ShapeIdList anIds = GetPickedShapesIds (theIsAll);
  if (myRenderer.GetPointer() != NULL)
  {
    // Obtain all actors whose source shape ids are within selected ids.
    vtkSmartPointer<vtkActorCollection> anActors = myRenderer->GetActors();
    anActors->InitTraversal();
    vtkSmartPointer<vtkActor> anActor = anActors->GetNextActor();
    while ( anActor.GetPointer() != NULL )
    {
      if (anActor->GetPickable() && anActor->GetVisibility())
      {
        if (anActor->GetMapper())
        {
          IVtk_IShape::Handle aShape = IVtkTools_ShapeObject::GetOccShape (anActor);
          if (!aShape.IsNull())
          {
            for (IVtk_ShapeIdList::Iterator anIt (anIds); anIt.More(); anIt.Next())
            {
              if (aShape->GetId() == anIt.Value())
              {
                aRes->AddItem (anActor);
              }
            }
          }
        }
      }
      anActor = anActors->GetNextActor();
    }
  }
  return aRes;
}
