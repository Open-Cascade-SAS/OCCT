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

// prevent disabling some MSVC warning messages by VTK headers
#ifdef _MSC_VER
  #pragma warning(push)
#endif
#include <vtkCommand.h>
#include <vtkObjectFactory.h>
#include <vtkRenderer.h>
#include <vtkActorCollection.h>
#ifdef _MSC_VER
  #pragma warning(pop)
#endif

vtkStandardNewMacro(IVtkTools_ShapePicker);

//=================================================================================================

IVtkTools_ShapePicker::IVtkTools_ShapePicker()
    : myRenderer(nullptr),
      myIsRectSelection(false),
      myIsPolySelection(false)
{
  myOccPickerAlgo = new IVtkOCC_ShapePickerAlgo();
}

//=================================================================================================

IVtkTools_ShapePicker::~IVtkTools_ShapePicker() = default;

//=================================================================================================

void IVtkTools_ShapePicker::SetPixelTolerance(const int theTolerance)
{
  myOccPickerAlgo->SetPixelTolerance(theTolerance);
}

//=================================================================================================

int IVtkTools_ShapePicker::PixelTolerance() const
{
  return myOccPickerAlgo->PixelTolerance();
}

//=================================================================================================

bool IVtkTools_ShapePicker::convertDisplayToWorld(vtkRenderer* theRenderer,
                                                  double       theDisplayCoord[3],
                                                  double       theWorldCoord[3])
{
  // Convert the selection point into world coordinates.
  theRenderer->SetDisplayPoint(theDisplayCoord[0], theDisplayCoord[1], theDisplayCoord[2]);
  theRenderer->DisplayToWorld();

  double aCoords[4];
  theRenderer->GetWorldPoint(aCoords);
  if (aCoords[3] == 0.0)
  {
    return false;
  }

  for (int anI = 0; anI < 3; anI++)
  {
    theWorldCoord[anI] = aCoords[anI] / aCoords[3];
  }

  return true;
}

//=================================================================================================

int IVtkTools_ShapePicker::Pick(double theX, double theY, double /*theZ*/, vtkRenderer* theRenderer)
{
  double aPos[2]    = {theX, theY};
  myIsRectSelection = false;
  myIsPolySelection = false;
  return pick(aPos, theRenderer);
}

//=================================================================================================

int IVtkTools_ShapePicker::Pick(double       theXPMin,
                                double       theYPMin,
                                double       theXPMax,
                                double       theYPMax,
                                vtkRenderer* theRenderer)
{
  double aPos[4]    = {theXPMin, theYPMin, theXPMax, theYPMax};
  myIsRectSelection = true;
  myIsPolySelection = false;
  return pick(aPos, theRenderer);
}

//=================================================================================================

int IVtkTools_ShapePicker::Pick(double       thePoly[][3],
                                const int    theNbPoints,
                                vtkRenderer* theRenderer)
{
  myIsRectSelection = false;
  myIsPolySelection = true;
  return pick((double*)thePoly, theRenderer, theNbPoints);
}

//=================================================================================================

int IVtkTools_ShapePicker::pick(double* thePos, vtkRenderer* theRenderer, const int theNbPoints)
{
  //  Initialize picking process
  Initialize();

  // Emit StartPickEvent for observer callbacks (if any)
  InvokeEvent(vtkCommand::StartPickEvent, nullptr);

  vtkSmartPointer<vtkRenderer> aRenderer;
  if (theRenderer == nullptr)
  {
    aRenderer = myRenderer; // by default use own renderer
  }
  else
  {
    aRenderer = theRenderer;
  }
  doPickImpl(thePos, aRenderer, theNbPoints);

  // Emit EndPickEvent for observer callbacks (if any)
  InvokeEvent(vtkCommand::EndPickEvent, nullptr);

  return myOccPickerAlgo->NbPicked();
}

//=================================================================================================

void IVtkTools_ShapePicker::doPickImpl(double*      thePos,
                                       vtkRenderer* theRenderer,
                                       const int    theNbPoints)
{
  // Make sure the correct renderer is used
  SetRenderer(theRenderer);

  if (myIsPolySelection)
  {
    myOccPickerAlgo->Pick((double**)thePos, theNbPoints);
  }
  else if (myIsRectSelection)
  {
    myOccPickerAlgo->Pick(thePos[0], thePos[1], thePos[2], thePos[3]);
  }
  else
  {
    myOccPickerAlgo->Pick(thePos[0], thePos[1]);
  }

  PickPosition[0] = myOccPickerAlgo->TopPickedPoint().X();
  PickPosition[1] = myOccPickerAlgo->TopPickedPoint().Y();
  PickPosition[2] = myOccPickerAlgo->TopPickedPoint().Z();
}

//=================================================================================================

void IVtkTools_ShapePicker::SetRenderer(vtkRenderer* theRenderer)
{
  if (theRenderer == myRenderer.GetPointer())
  {
    return;
    // In this case we should not do anything.
    // In the worth case we need to update picker algorithm (view er selector and projection
    // options) If any needs this , call myOccPickerAlgo->Modified();
  }

  myRenderer                 = theRenderer;
  IVtkVTK_View::Handle aView = new IVtkVTK_View(myRenderer);
  myOccPickerAlgo->SetView(aView);
}

//=================================================================================================

void IVtkTools_ShapePicker::SetAreaSelection(bool theIsOn)
{
  myIsRectSelection = theIsOn;
}

//=================================================================================================

NCollection_List<IVtk_SelectionMode> IVtkTools_ShapePicker::GetSelectionModes(
  const IVtk_IShape::Handle& theShape) const
{
  return myOccPickerAlgo->GetSelectionModes(theShape);
}

//=================================================================================================

NCollection_List<IVtk_SelectionMode> IVtkTools_ShapePicker::GetSelectionModes(
  vtkActor* theShapeActor) const
{
  NCollection_List<IVtk_SelectionMode> aRes;
  IVtk_IShape::Handle                  aShape = IVtkTools_ShapeObject::GetOccShape(theShapeActor);
  if (!aShape.IsNull())
  {
    aRes = myOccPickerAlgo->GetSelectionModes(aShape);
  }
  return aRes;
}

//=================================================================================================

void IVtkTools_ShapePicker::SetSelectionMode(const IVtk_IShape::Handle& theShape,
                                             const IVtk_SelectionMode   theMode,
                                             const bool                 theIsTurnOn) const
{
  myOccPickerAlgo->SetSelectionMode(theShape, theMode, theIsTurnOn);
}

//=================================================================================================

void IVtkTools_ShapePicker::SetSelectionMode(vtkActor*                theShapeActor,
                                             const IVtk_SelectionMode theMode,
                                             const bool               theIsTurnOn) const
{
  IVtk_IShape::Handle aShape = IVtkTools_ShapeObject::GetOccShape(theShapeActor);
  if (!aShape.IsNull())
  {
    myOccPickerAlgo->SetSelectionMode(aShape, theMode, theIsTurnOn);
  }
}

//=================================================================================================

void IVtkTools_ShapePicker::SetSelectionMode(const IVtk_SelectionMode theMode,
                                             const bool               theIsTurnOn) const
{
  if (myRenderer.GetPointer() != nullptr)
  {
    // Obtain all OccShapes displayed and activate the specified selection mode
    vtkSmartPointer<vtkActorCollection> anActors = myRenderer->GetActors();
    anActors->InitTraversal();
    vtkSmartPointer<vtkActor> anActor = anActors->GetNextActor();
    while (anActor.GetPointer() != nullptr)
    {
      if (anActor->GetPickable() && anActor->GetVisibility())
      {
        if (anActor->GetMapper())
        {
          IVtk_IShape::Handle aShape = IVtkTools_ShapeObject::GetOccShape(anActor);
          if (!aShape.IsNull())
          {
            myOccPickerAlgo->SetSelectionMode(aShape, theMode, theIsTurnOn);
          }
        }
      }
      anActor = anActors->GetNextActor();
    }
  }
}

//=================================================================================================

NCollection_List<IVtk_IdType> IVtkTools_ShapePicker::GetPickedShapesIds(bool theIsAll) const
{
  if (theIsAll || myIsRectSelection)
  {
    return myOccPickerAlgo->ShapesPicked();
  }

  NCollection_List<IVtk_IdType> aRes;
  NCollection_List<IVtk_IdType> aPicked = myOccPickerAlgo->ShapesPicked();
  if (!aPicked.IsEmpty())
  {
    aRes.Append(aPicked.First());
  }
  return aRes;
}

//=================================================================================================

void IVtkTools_ShapePicker::RemoveSelectableObject(const IVtk_IShape::Handle& theShape)
{
  myOccPickerAlgo->RemoveSelectableObject(theShape);
}

//=================================================================================================

void IVtkTools_ShapePicker::RemoveSelectableActor(vtkActor* theShapeActor)
{
  IVtk_IShape::Handle aShape = IVtkTools_ShapeObject::GetOccShape(theShapeActor);
  if (!aShape.IsNull())
  {
    RemoveSelectableObject(aShape);
  }
}

//=================================================================================================

NCollection_List<IVtk_IdType> IVtkTools_ShapePicker::GetPickedSubShapesIds(const IVtk_IdType theId,
                                                                           bool theIsAll) const
{
  NCollection_List<IVtk_IdType> aRes;
  if (theIsAll)
  {
    myOccPickerAlgo->SubShapesPicked(theId, aRes);
  }
  else
  {
    NCollection_List<IVtk_IdType> aList;
    myOccPickerAlgo->SubShapesPicked(theId, aList);
    if (!aList.IsEmpty())
    {
      aRes.Append(aList.First());
    }
  }
  return aRes;
}

//=================================================================================================

vtkSmartPointer<vtkActorCollection> IVtkTools_ShapePicker::GetPickedActors(bool theIsAll) const
{
  vtkSmartPointer<vtkActorCollection> aRes  = vtkSmartPointer<vtkActorCollection>::New();
  NCollection_List<IVtk_IdType>       anIds = GetPickedShapesIds(theIsAll);
  if (myRenderer.GetPointer() != nullptr)
  {
    // Obtain all actors whose source shape ids are within selected ids.
    vtkSmartPointer<vtkActorCollection> anActors = myRenderer->GetActors();
    anActors->InitTraversal();
    vtkSmartPointer<vtkActor> anActor = anActors->GetNextActor();
    while (anActor.GetPointer() != nullptr)
    {
      if (anActor->GetPickable() && anActor->GetVisibility())
      {
        if (anActor->GetMapper())
        {
          IVtk_IShape::Handle aShape = IVtkTools_ShapeObject::GetOccShape(anActor);
          if (!aShape.IsNull())
          {
            for (NCollection_List<IVtk_IdType>::Iterator anIt(anIds); anIt.More(); anIt.Next())
            {
              if (aShape->GetId() == anIt.Value())
              {
                aRes->AddItem(anActor);
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
