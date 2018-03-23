// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <inspector/View_Displayer.hxx>

#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_Shape.hxx>
#include <AIS_Trihedron.hxx>
#include <Prs3d_PointAspect.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <inspector/View_Viewer.hxx>
#include <inspector/View_Widget.hxx>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
View_Displayer::View_Displayer()
: myIsKeepPresentations (false), myFitAllActive (false), myDisplayMode (-1)
{
}

// =======================================================================
// function : SetContext
// purpose :
// =======================================================================
void View_Displayer::SetContext (const Handle(AIS_InteractiveContext)& theContext)
{
  NCollection_DataMap<View_PresentationType, NCollection_Shared<AIS_ListOfInteractive> > aDisplayed = myDisplayed;
  EraseAllPresentations (true);
  myContext = theContext;

  for (NCollection_DataMap<View_PresentationType, NCollection_Shared<AIS_ListOfInteractive> >::Iterator aDisplayedIt(aDisplayed);
    aDisplayedIt.More(); aDisplayedIt.Next())
  {
    View_PresentationType aType = aDisplayedIt.Key();
    for (AIS_ListIteratorOfListOfInteractive aPresentationsIt (aDisplayedIt.Value());
         aPresentationsIt.More(); aPresentationsIt.Next())
      DisplayPresentation (aPresentationsIt.Value(), aType, false);
  }
  UpdateViewer();
}

// =======================================================================
// function : SetDisplayMode
// purpose :
// =======================================================================
void View_Displayer::SetDisplayMode (const int theDisplayMode,
                                     const View_PresentationType theType,
                                     const bool theToUpdateViewer)
{
  myDisplayMode = theDisplayMode;
  if (GetContext().IsNull())
    return;

  NCollection_Shared<AIS_ListOfInteractive> aDisplayed;
  DisplayedPresentations (aDisplayed, theType);

  for (AIS_ListIteratorOfListOfInteractive aDisplayedIt (aDisplayed); aDisplayedIt.More(); aDisplayedIt.Next())
    GetContext()->SetDisplayMode (aDisplayedIt.Value(), theDisplayMode, Standard_False);

  if (theToUpdateViewer)
    UpdateViewer();
}

// =======================================================================
// function : DisplayPresentation
// purpose :
// =======================================================================
void View_Displayer::DisplayPresentation (const Handle(Standard_Transient)& thePresentation,
                                          const View_PresentationType theType,
                                          const bool theToUpdateViewer)
{
  if (GetContext().IsNull())
    return;

  NCollection_Shared<AIS_ListOfInteractive> aDisplayed;
  DisplayedPresentations (aDisplayed, theType);
  if (!myIsKeepPresentations)
    ErasePresentations (theType, false);

  Handle(AIS_InteractiveObject) aPresentation = Handle(AIS_InteractiveObject)::DownCast (thePresentation);
  if (!aPresentation.IsNull() && aPresentation->GetContext().IsNull())
  {
    // one presentation can not be shown in several contexts
    if (theType == View_PresentationType_Additional)
    {
      Quantity_Color aColor;
      if (myColorAttributes.Find (View_PresentationType_Additional, aColor))
        aPresentation->SetColor (aColor);
    }
    GetContext()->Display (aPresentation, false);
    if (myDisplayMode != -1)
      GetContext()->SetDisplayMode (aPresentation, myDisplayMode, false);
    aDisplayed.Append (aPresentation);
  }

  if (!myIsKeepPresentations || myFitAllActive)
    fitAllView();

  myDisplayed.Bind (theType, aDisplayed);

  if (theToUpdateViewer)
    UpdateViewer();
}

// =======================================================================
// function : RedisplayPresentation
// purpose :
// =======================================================================
void View_Displayer::RedisplayPresentation (const Handle(Standard_Transient)& thePresentation,
                                            const bool theToUpdateViewer)
{
  Handle(AIS_InteractiveObject) aPresentation = Handle(AIS_InteractiveObject)::DownCast (thePresentation);
  if (aPresentation.IsNull() || aPresentation->GetContext().IsNull())
    return;

  GetContext()->Redisplay (aPresentation, false);

  if (myFitAllActive)
    fitAllView();

  if (theToUpdateViewer)
    UpdateViewer();
}

// =======================================================================
// function : EraseAllPresentations
// purpose :
// =======================================================================
void View_Displayer::EraseAllPresentations (const bool theToUpdateViewer)
{
  for (NCollection_DataMap<View_PresentationType, NCollection_Shared<AIS_ListOfInteractive> >::Iterator aDisplayedIt(myDisplayed);
       aDisplayedIt.More(); aDisplayedIt.Next())
    ErasePresentations (aDisplayedIt.Key(), false);

  if (theToUpdateViewer)
    UpdateViewer();
}

// =======================================================================
// function : ErasePresentations
// purpose :
// =======================================================================
void View_Displayer::ErasePresentations (const View_PresentationType theType, const bool theToUpdateViewer)
{
  if (GetContext().IsNull())
    return;

  NCollection_Shared<AIS_ListOfInteractive> aDisplayed;
  DisplayedPresentations (aDisplayed, theType);

  for (AIS_ListIteratorOfListOfInteractive aDisplayedIt (aDisplayed); aDisplayedIt.More(); aDisplayedIt.Next())
  {
    if (aDisplayedIt.Value()->IsKind(STANDARD_TYPE (AIS_Trihedron)))
      continue;

    GetContext()->Remove (aDisplayedIt.Value(), Standard_False);
  }

  aDisplayed.Clear();
  myDisplayed.Bind (theType, aDisplayed);

  if (theToUpdateViewer)
    UpdateViewer();
}

// =======================================================================
// function : ErasePresentation
// purpose :
// =======================================================================
void View_Displayer::ErasePresentation (const Handle(Standard_Transient)& thePresentation,
                                        const View_PresentationType theType,
                                        const bool theToUpdateViewer)
{
  if (GetContext().IsNull())
    return;

  Handle(AIS_InteractiveObject) aPresentation = Handle(AIS_InteractiveObject)::DownCast (thePresentation);
  if (aPresentation.IsNull())
    return;

  GetContext()->Remove (aPresentation, Standard_False);

  NCollection_Shared<AIS_ListOfInteractive> aDisplayed;
  DisplayedPresentations (aDisplayed, theType);
  aDisplayed.Remove (aPresentation);
  myDisplayed.Bind (theType, aDisplayed);

  if (myFitAllActive)
    fitAllView();

  if (theToUpdateViewer)
    UpdateViewer();
}

// =======================================================================
// function : SetVisible
// purpose :
// =======================================================================
void View_Displayer::SetVisible (const TopoDS_Shape& theShape, const bool theState, const View_PresentationType theType)
{
  if (theShape.IsNull())
    return;

  if (theState)
    DisplayPresentation (CreatePresentation (theShape), View_PresentationType_Main, Standard_False);
  else
  {
    Handle(AIS_InteractiveObject) aPresentation = FindPresentation (theShape, theType);
    if (!aPresentation.IsNull())
      ErasePresentation (aPresentation, theType, Standard_False);
  }

  UpdateViewer();
}

// =======================================================================
// function : IsVisible
// purpose :
// =======================================================================
bool View_Displayer::IsVisible (const TopoDS_Shape& theShape, const View_PresentationType theType) const
{
  Handle(AIS_InteractiveObject) aPresentation = FindPresentation (theShape, theType);
  return !aPresentation.IsNull();
}

// =======================================================================
// function : UpdateViewer
// purpose :
// =======================================================================
void View_Displayer::UpdateViewer()
{
  if (GetContext().IsNull())
    return;

  GetContext()->UpdateCurrentViewer();
}

// =======================================================================
// function : SetAttributeColor
// purpose :
// =======================================================================
void View_Displayer::SetAttributeColor (const Quantity_Color& theColor, const View_PresentationType theType)
{
  myColorAttributes.Bind (theType, theColor);
}

// =======================================================================
// function : DisplayedPresentations
// purpose :
// =======================================================================
void View_Displayer::DisplayedPresentations (NCollection_Shared<AIS_ListOfInteractive>& thePresentations,
                                             const View_PresentationType theType) const
{
  myDisplayed.Find (theType, thePresentations);
}

// =======================================================================
// function : getView
// purpose :
// =======================================================================
Handle(V3d_View) View_Displayer::GetView() const
{
  Handle(V3d_View) aView;
  if (GetContext().IsNull())
    return aView;

  const Handle(V3d_Viewer)& aViewer = GetContext()->CurrentViewer();
  if (!aViewer.IsNull())
  {
    aViewer->InitActiveViews();
    if (aViewer->MoreActiveViews())
      aView = aViewer->ActiveView();
  }
  return aView;
}

// =======================================================================
// function : FindPresentation
// purpose :
// =======================================================================
Handle(AIS_InteractiveObject) View_Displayer::FindPresentation (const TopoDS_Shape& theShape,
                                                                const View_PresentationType theType) const
{
  if (theShape.IsNull())
    return Handle(AIS_InteractiveObject)();

  NCollection_Shared<AIS_ListOfInteractive> aDisplayed;
  DisplayedPresentations (aDisplayed, theType);

  for (AIS_ListIteratorOfListOfInteractive aDisplayedIt (aDisplayed); aDisplayedIt.More(); aDisplayedIt.Next())
  {
    Handle(AIS_Shape) aPresentation = Handle(AIS_Shape)::DownCast (aDisplayedIt.Value());
    if (aPresentation->Shape().IsEqual (theShape))
      return aPresentation;
  }

  return Handle(AIS_InteractiveObject)();
}

// =======================================================================
// function : CreatePresentation
// purpose :
// =======================================================================
Handle(Standard_Transient) View_Displayer::CreatePresentation (const TopoDS_Shape& theShape)
{
  Handle(AIS_Shape) aShape = new AIS_Shape (theShape);

  aShape->Attributes()->SetPointAspect (new Prs3d_PointAspect (Aspect_TOM_POINT, Quantity_NOC_WHITE, 1.0));

  return aShape;
}

// =======================================================================
// function : fitAllView
// purpose :
// =======================================================================
void  View_Displayer::fitAllView()
{
  Handle(V3d_View) aView = GetView();
  if (!aView.IsNull())
  {
    aView->FitAll();
    aView->Redraw();
  }
}