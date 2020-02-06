// Created by: NW,JPB,CAL
// Copyright (c) 1991-1999 Matra Datavision
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

#include <Graphic3d_StructureManager.hxx>

#include <Graphic3d_DataStructureManager.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_Structure.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_StructureManager,Standard_Transient)

#include <Graphic3d_MapIteratorOfMapOfStructure.hxx>
#include <Graphic3d_CView.hxx>

// ========================================================================
// function : Graphic3d_StructureManager
// purpose  :
// ========================================================================
Graphic3d_StructureManager::Graphic3d_StructureManager (const Handle(Graphic3d_GraphicDriver)& theDriver)
: myViewGenId (0, 31),
  myGraphicDriver (theDriver),
  myDeviceLostFlag (Standard_False)
{
  //
}

// ========================================================================
// function : ~Graphic3d_StructureManager
// purpose  :
// ========================================================================
Graphic3d_StructureManager::~Graphic3d_StructureManager()
{
  for (Graphic3d_MapIteratorOfMapOfStructure anIt (myDisplayedStructure); anIt.More(); anIt.Next())
  {
    anIt.Value()->Remove();
  }

  myDisplayedStructure.Clear();
  myHighlightedStructure.Clear();
  myDefinedViews.Clear();
}

// ========================================================================
// function : Update
// purpose  :
// ========================================================================
void Graphic3d_StructureManager::Update (const Graphic3d_ZLayerId theLayerId) const
{
  for (Graphic3d_IndexedMapOfView::Iterator aViewIt (myDefinedViews); aViewIt.More(); aViewIt.Next())
  {
    aViewIt.Value()->Update (theLayerId);
  }
}

// ========================================================================
// function : Remove
// purpose  :
// ========================================================================
void Graphic3d_StructureManager::Remove()
{
  // clear all structures whilst views are alive for correct GPU memory management
  for (Graphic3d_MapIteratorOfMapOfStructure anIt (myDisplayedStructure); anIt.More(); anIt.Next())
  {
    anIt.Value()->Remove();
  }
  myDisplayedStructure.Clear();
  myHighlightedStructure.Clear();

  // clear list of managed views
  myDefinedViews.Clear();
}

// ========================================================================
// function : Erase
// purpose  :
// ========================================================================
void Graphic3d_StructureManager::Erase()
{
  for (Graphic3d_MapIteratorOfMapOfStructure anIt (myDisplayedStructure); anIt.More(); anIt.Next())
  {
    anIt.Key()->Erase();
  }
}

void Graphic3d_StructureManager::DisplayedStructures (Graphic3d_MapOfStructure& SG) const {

  SG.Assign(myDisplayedStructure);

  //JMBStandard_Integer Length  = myDisplayedStructure.Length ();

  //JMBfor (Standard_Integer i=1; i<=Length; i++)
  //JMB SG.Add (myDisplayedStructure.Value (i));

}

Standard_Integer Graphic3d_StructureManager::NumberOfDisplayedStructures () const {

Standard_Integer Length = myDisplayedStructure.Extent ();

        return (Length);

}

//Handle(Graphic3d_Structure) Graphic3d_StructureManager::DisplayedStructure (const Standard_Integer AnIndex) const {

//return (myDisplayedStructure.Value (AnIndex));

//}

void Graphic3d_StructureManager::HighlightedStructures (Graphic3d_MapOfStructure& SG) const {

  SG.Assign(myHighlightedStructure);

}

Handle(Graphic3d_Structure) Graphic3d_StructureManager::Identification (const Standard_Integer AId) const {

//  Standard_Integer ind=0;
  Standard_Boolean notfound     = Standard_True;

  Handle(Graphic3d_Structure) StructNull;
 
  Graphic3d_MapIteratorOfMapOfStructure it( myDisplayedStructure);
  
  Handle(Graphic3d_Structure) SGfound;

  for (; it.More() && notfound; it.Next()) {
    Handle(Graphic3d_Structure) SG = it.Key();
    if ( SG->Identification () == AId) {
      notfound  = Standard_False;
      SGfound = SG;
    }
  }


  if (notfound)
    return (StructNull);
  else
    return (SGfound);

}

const Handle(Graphic3d_GraphicDriver)& Graphic3d_StructureManager::GraphicDriver () const 
{
  return (myGraphicDriver);
}

void Graphic3d_StructureManager::RecomputeStructures()
{
  myDeviceLostFlag = Standard_False;

  // Go through all unique structures including child (connected) ones and ensure that they are computed.
  NCollection_Map<Graphic3d_Structure*> aStructNetwork;
  for (Graphic3d_MapIteratorOfMapOfStructure anIter(myDisplayedStructure); anIter.More(); anIter.Next())
  {
    Handle(Graphic3d_Structure) aStructure = anIter.Key();
    anIter.Key()->Network (anIter.Key().get(), Graphic3d_TOC_DESCENDANT, aStructNetwork);
  }

  RecomputeStructures (aStructNetwork);
}

void Graphic3d_StructureManager::RecomputeStructures (const NCollection_Map<Graphic3d_Structure*>& theStructures)
{
  for (NCollection_Map<Graphic3d_Structure*>::Iterator anIter (theStructures); anIter.More(); anIter.Next())
  {
    Graphic3d_Structure* aStruct = anIter.Key();
    aStruct->Clear();
    aStruct->Compute();
  }
}

Handle(Graphic3d_ViewAffinity) Graphic3d_StructureManager::RegisterObject (const Handle(Standard_Transient)& theObject)
{
  Handle(Graphic3d_ViewAffinity) aResult;
  if (myRegisteredObjects.Find (theObject.operator->(), aResult))
  {
    return aResult;
  }

  aResult = new Graphic3d_ViewAffinity();
  myRegisteredObjects.Bind (theObject.operator->(), aResult);
  return aResult;
}

void Graphic3d_StructureManager::UnregisterObject (const Handle(Standard_Transient)& theObject)
{
  myRegisteredObjects.UnBind (theObject.operator->());
}

Handle(Graphic3d_ViewAffinity) Graphic3d_StructureManager::ObjectAffinity (const Handle(Standard_Transient)& theObject) const
{
  Handle(Graphic3d_ViewAffinity) aResult;
  myRegisteredObjects.Find (theObject.operator->(), aResult);
  return aResult;
}

// ========================================================================
// function : Identification
// purpose  :
// ========================================================================
Standard_Integer Graphic3d_StructureManager::Identification (Graphic3d_CView* theView)
{
  if (myDefinedViews.Contains (theView))
  {
    return theView->Identification();
  }

  myDefinedViews.Add (theView);
  return myViewGenId.Next();
}

// ========================================================================
// function : UnIdentification
// purpose  :
// ========================================================================
void Graphic3d_StructureManager::UnIdentification (Graphic3d_CView* theView)
{
  if (myDefinedViews.Contains (theView))
  {
    myDefinedViews.Swap (myDefinedViews.FindIndex (theView), myDefinedViews.Size());
    myDefinedViews.RemoveLast();
    myViewGenId.Free (theView->Identification());
  }
}

// ========================================================================
// function : DefinedViews
// purpose  :
// ========================================================================
const Graphic3d_IndexedMapOfView& Graphic3d_StructureManager::DefinedViews() const
{
  return myDefinedViews;
}

// ========================================================================
// function : MaxNumOfViews
// purpose  :
// ========================================================================
Standard_Integer Graphic3d_StructureManager::MaxNumOfViews() const
{
  return myViewGenId.Upper() - myViewGenId.Lower() + 1;
}

// ========================================================================
// function : ReCompute
// purpose  :
// ========================================================================
void Graphic3d_StructureManager::ReCompute (const Handle(Graphic3d_Structure)& theStructure)
{
  if (!myDisplayedStructure.Contains (theStructure))
  {
    return;
  }

  // Recompute structure for all defined views
  for (Standard_Integer aViewIt = 1; aViewIt <= myDefinedViews.Extent(); ++aViewIt)
  {
    ReCompute (theStructure, myDefinedViews (aViewIt));
  }
}

// ========================================================================
// function : ReCompute
// purpose  :
// ========================================================================
void Graphic3d_StructureManager::ReCompute (const Handle(Graphic3d_Structure)& theStructure,
                                            const Handle(Graphic3d_DataStructureManager)& theProjector)
{
  Handle(Graphic3d_CView) aView = Handle(Graphic3d_CView)::DownCast (theProjector);

  if (aView.IsNull()
   || !myDefinedViews.Contains (aView.operator->())
   || !myDisplayedStructure.Contains (theStructure))
  {
    return;
  }

  aView->ReCompute (theStructure);
}

// ========================================================================
// function : Clear
// purpose  :
// ========================================================================
void Graphic3d_StructureManager::Clear (Graphic3d_Structure* theStructure,
                                        const Standard_Boolean theWithDestruction)
{
  for (Graphic3d_IndexedMapOfView::Iterator aViewIt (myDefinedViews); aViewIt.More(); aViewIt.Next())
  {
    aViewIt.Value()->Clear (theStructure, theWithDestruction);
  }
}

// ========================================================================
// function : Connect
// purpose  :
// ========================================================================
void Graphic3d_StructureManager::Connect (const Graphic3d_Structure* theMother,
                                          const Graphic3d_Structure* theDaughter)
{
  for (Graphic3d_IndexedMapOfView::Iterator aViewIt (myDefinedViews); aViewIt.More(); aViewIt.Next())
  {
    aViewIt.Value()->Connect (theMother, theDaughter);
  }
}

// ========================================================================
// function : Disconnect
// purpose  :
// ========================================================================
void Graphic3d_StructureManager::Disconnect (const Graphic3d_Structure* theMother,
                                             const Graphic3d_Structure* theDaughter)
{
  for (Graphic3d_IndexedMapOfView::Iterator aViewIt (myDefinedViews); aViewIt.More(); aViewIt.Next())
  {
    aViewIt.Value()->Disconnect (theMother, theDaughter);
  }
}

// ========================================================================
// function : Display
// purpose  :
// ========================================================================
void Graphic3d_StructureManager::Display (const Handle(Graphic3d_Structure)& theStructure)
{
  myDisplayedStructure.Add (theStructure);

  for (Graphic3d_IndexedMapOfView::Iterator aViewIt (myDefinedViews); aViewIt.More(); aViewIt.Next())
  {
    aViewIt.Value()->Display (theStructure);
  }
}

// ========================================================================
// function : Erase
// purpose  :
// ========================================================================
void Graphic3d_StructureManager::Erase (const Handle(Graphic3d_Structure)& theStructure)
{
  myDisplayedStructure  .Remove(theStructure);
  myHighlightedStructure.Remove (theStructure);

  // Erase structure in all defined views
  for (Graphic3d_IndexedMapOfView::Iterator aViewIt (myDefinedViews); aViewIt.More(); aViewIt.Next())
  {
    aViewIt.Value()->Erase (theStructure);
  }
}

// ========================================================================
// function : Erase
// purpose  :
// ========================================================================
void Graphic3d_StructureManager::Highlight (const Handle(Graphic3d_Structure)& theStructure)
{
  myHighlightedStructure.Add (theStructure);

  // Highlight in all defined views
  for (Graphic3d_IndexedMapOfView::Iterator aViewIt (myDefinedViews); aViewIt.More(); aViewIt.Next())
  {
    aViewIt.Value()->Highlight (theStructure);
  }
}

// ========================================================================
// function : UnHighlight
// purpose  :
// ========================================================================
void Graphic3d_StructureManager::UnHighlight (const Handle(Graphic3d_Structure)& theStructure)
{
  myHighlightedStructure.Remove (theStructure);

  // UnHighlight in all defined views
  for (Graphic3d_IndexedMapOfView::Iterator aViewIt (myDefinedViews); aViewIt.More(); aViewIt.Next())
  {
    aViewIt.Value()->UnHighlight (theStructure);
  }
}

// ========================================================================
// function : UnHighlight
// purpose  :
// ========================================================================
void Graphic3d_StructureManager::UnHighlight()
{
  for (Graphic3d_MapIteratorOfMapOfStructure anIt (myHighlightedStructure); anIt.More(); anIt.Next())
  {
    anIt.Key()->UnHighlight();
  }
}

// ========================================================================
// function : SetTransform
// purpose  :
// ========================================================================
void Graphic3d_StructureManager::SetTransform (const Handle(Graphic3d_Structure)& theStructure,
                                               const Handle(TopLoc_Datum3D)& theTrsf)
{
  for (Graphic3d_IndexedMapOfView::Iterator aViewIt (myDefinedViews); aViewIt.More(); aViewIt.Next())
  {
    aViewIt.Value()->SetTransform (theStructure, theTrsf);
  }
}

// ========================================================================
// function : ChangeDisplayPriority
// purpose  :
// ========================================================================
void Graphic3d_StructureManager::ChangeDisplayPriority (const Handle(Graphic3d_Structure)& theStructure,
                                                        const Standard_Integer theOldPriority,
                                                        const Standard_Integer theNewPriority)
{
  for (Graphic3d_IndexedMapOfView::Iterator aViewIt (myDefinedViews); aViewIt.More(); aViewIt.Next())
  {
    aViewIt.Value()->ChangePriority (theStructure, theOldPriority, theNewPriority);
  }
}

//=======================================================================
//function : ChangeZLayer
//purpose  :
//=======================================================================
void Graphic3d_StructureManager::ChangeZLayer (const Handle(Graphic3d_Structure)& theStructure,
                                               const Graphic3d_ZLayerId           theLayerId)
{
  if (!myDisplayedStructure.Contains (theStructure))
  {
    return;
  }

  for (Graphic3d_IndexedMapOfView::Iterator aViewIt (myDefinedViews); aViewIt.More(); aViewIt.Next())
  {
    aViewIt.Value()->ChangeZLayer (theStructure, theLayerId);
  }
}

// =======================================================================
// function : DumpJson
// purpose  :
// =======================================================================
void Graphic3d_StructureManager::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)

  for (Graphic3d_MapOfStructure::Iterator anIter (myDisplayedStructure); anIter.More(); anIter.Next())
  {
    const Handle(Graphic3d_Structure)& aDisplayedStructure = anIter.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, aDisplayedStructure.get())
  }
  for (Graphic3d_MapOfStructure::Iterator anIter (myHighlightedStructure); anIter.More(); anIter.Next())
  {
    const Handle(Graphic3d_Structure)& aHighlightedStructure = anIter.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, aHighlightedStructure.get())
  }
  for (Graphic3d_MapOfObject::Iterator anIter (myRegisteredObjects); anIter.More(); anIter.Next())
  {
    const Handle(Graphic3d_ViewAffinity)& aRegisteredObject = anIter.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, aRegisteredObject.get())
  }

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myGraphicDriver.get())
  for (Graphic3d_IndexedMapOfView::Iterator anIter (myDefinedViews); anIter.More(); anIter.Next())
  {
    Graphic3d_CView* aDefinedView = anIter.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, aDefinedView)
  }
  
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myDeviceLostFlag)
}
