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

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_StructureManager, Standard_Transient)

#include <NCollection_Map.hxx>
#include <Graphic3d_CView.hxx>
class Graphic3d_Structure;

//=================================================================================================

Graphic3d_StructureManager::Graphic3d_StructureManager(
  const occ::handle<Graphic3d_GraphicDriver>& theDriver)
    : myViewGenId(0, 31),
      myGraphicDriver(theDriver),
      myDeviceLostFlag(false)
{
  //
}

//=================================================================================================

Graphic3d_StructureManager::~Graphic3d_StructureManager()
{
  for (NCollection_Map<occ::handle<Graphic3d_Structure>>::Iterator anIt(myDisplayedStructure);
       anIt.More();
       anIt.Next())
  {
    anIt.Value()->Remove();
  }

  myDisplayedStructure.Clear();
  myHighlightedStructure.Clear();
  myDefinedViews.Clear();
}

//=================================================================================================

void Graphic3d_StructureManager::Update(const Graphic3d_ZLayerId theLayerId) const
{
  for (NCollection_IndexedMap<Graphic3d_CView*>::Iterator aViewIt(myDefinedViews); aViewIt.More();
       aViewIt.Next())
  {
    aViewIt.Value()->Update(theLayerId);
  }
}

//=================================================================================================

void Graphic3d_StructureManager::Remove()
{
  // clear all structures whilst views are alive for correct GPU memory management
  for (NCollection_Map<occ::handle<Graphic3d_Structure>>::Iterator anIt(myDisplayedStructure);
       anIt.More();
       anIt.Next())
  {
    anIt.Value()->Remove();
  }
  myDisplayedStructure.Clear();
  myHighlightedStructure.Clear();

  // clear list of managed views
  myDefinedViews.Clear();
}

//=================================================================================================

void Graphic3d_StructureManager::Erase()
{
  for (NCollection_Map<occ::handle<Graphic3d_Structure>>::Iterator anIt(myDisplayedStructure);
       anIt.More();
       anIt.Next())
  {
    anIt.Key()->Erase();
  }
}

void Graphic3d_StructureManager::DisplayedStructures(
  NCollection_Map<occ::handle<Graphic3d_Structure>>& SG) const
{

  SG.Assign(myDisplayedStructure);

  // JMBStandard_Integer Length  = myDisplayedStructure.Length ();

  // JMBfor (int i=1; i<=Length; i++)
  // JMB SG.Add (myDisplayedStructure.Value (i));
}

int Graphic3d_StructureManager::NumberOfDisplayedStructures() const
{

  int Length = myDisplayedStructure.Extent();

  return (Length);
}

// occ::handle<Graphic3d_Structure> Graphic3d_StructureManager::DisplayedStructure (const
// int AnIndex) const {

// return (myDisplayedStructure.Value (AnIndex));

//}

void Graphic3d_StructureManager::HighlightedStructures(
  NCollection_Map<occ::handle<Graphic3d_Structure>>& SG) const
{

  SG.Assign(myHighlightedStructure);
}

occ::handle<Graphic3d_Structure> Graphic3d_StructureManager::Identification(const int AId) const
{

  //  int ind=0;
  bool notfound = true;

  occ::handle<Graphic3d_Structure> StructNull;

  NCollection_Map<occ::handle<Graphic3d_Structure>>::Iterator it(myDisplayedStructure);

  occ::handle<Graphic3d_Structure> SGfound;

  for (; it.More() && notfound; it.Next())
  {
    const occ::handle<Graphic3d_Structure>& SG = it.Key();
    if (SG->Identification() == AId)
    {
      notfound = false;
      SGfound  = SG;
    }
  }

  if (notfound)
    return (StructNull);
  else
    return (SGfound);
}

const occ::handle<Graphic3d_GraphicDriver>& Graphic3d_StructureManager::GraphicDriver() const
{
  return (myGraphicDriver);
}

void Graphic3d_StructureManager::RecomputeStructures()
{
  myDeviceLostFlag = false;

  // Go through all unique structures including child (connected) ones and ensure that they are
  // computed.
  NCollection_Map<Graphic3d_Structure*> aStructNetwork;
  for (NCollection_Map<occ::handle<Graphic3d_Structure>>::Iterator anIter(myDisplayedStructure);
       anIter.More();
       anIter.Next())
  {
    anIter.Key()->Network(anIter.Key().get(), Graphic3d_TOC_DESCENDANT, aStructNetwork);
  }

  RecomputeStructures(aStructNetwork);
}

void Graphic3d_StructureManager::RecomputeStructures(
  const NCollection_Map<Graphic3d_Structure*>& theStructures)
{
  for (NCollection_Map<Graphic3d_Structure*>::Iterator anIter(theStructures); anIter.More();
       anIter.Next())
  {
    Graphic3d_Structure* aStruct = anIter.Key();
    aStruct->Clear();
    aStruct->Compute();
  }
}

//=================================================================================================

void Graphic3d_StructureManager::RegisterObject(
  const occ::handle<Standard_Transient>&     theObject,
  const occ::handle<Graphic3d_ViewAffinity>& theAffinity)
{
  occ::handle<Graphic3d_ViewAffinity> aResult;
  if (myRegisteredObjects.Find(theObject.operator->(), aResult) && aResult == theAffinity)
  {
    return;
  }

  myRegisteredObjects.Bind(theObject.operator->(), theAffinity);
}

//=================================================================================================

void Graphic3d_StructureManager::UnregisterObject(const occ::handle<Standard_Transient>& theObject)
{
  myRegisteredObjects.UnBind(theObject.operator->());
}

//=================================================================================================

const occ::handle<Graphic3d_ViewAffinity>& Graphic3d_StructureManager::ObjectAffinity(
  const occ::handle<Standard_Transient>& theObject) const
{
  const occ::handle<Graphic3d_ViewAffinity>* aResult =
    myRegisteredObjects.Seek(theObject.operator->());
  if (aResult == nullptr)
  {
    static const occ::handle<Graphic3d_ViewAffinity> aDummy;
    return aDummy;
  }
  return *aResult;
}

//=================================================================================================

int Graphic3d_StructureManager::Identification(Graphic3d_CView* theView)
{
  if (myDefinedViews.Contains(theView))
  {
    return theView->Identification();
  }

  myDefinedViews.Add(theView);
  return myViewGenId.Next();
}

//=================================================================================================

void Graphic3d_StructureManager::UnIdentification(Graphic3d_CView* theView)
{
  if (myDefinedViews.Contains(theView))
  {
    myDefinedViews.Swap(myDefinedViews.FindIndex(theView), myDefinedViews.Size());
    myDefinedViews.RemoveLast();
    myViewGenId.Free(theView->Identification());
  }
}

//=================================================================================================

const NCollection_IndexedMap<Graphic3d_CView*>& Graphic3d_StructureManager::DefinedViews() const
{
  return myDefinedViews;
}

//=================================================================================================

int Graphic3d_StructureManager::MaxNumOfViews() const
{
  return myViewGenId.Upper() - myViewGenId.Lower() + 1;
}

//=================================================================================================

void Graphic3d_StructureManager::ReCompute(const occ::handle<Graphic3d_Structure>& theStructure)
{
  if (!myDisplayedStructure.Contains(theStructure))
  {
    return;
  }

  // Recompute structure for all defined views
  for (int aViewIt = 1; aViewIt <= myDefinedViews.Extent(); ++aViewIt)
  {
    ReCompute(theStructure, myDefinedViews(aViewIt));
  }
}

//=================================================================================================

void Graphic3d_StructureManager::ReCompute(
  const occ::handle<Graphic3d_Structure>&            theStructure,
  const occ::handle<Graphic3d_DataStructureManager>& theProjector)
{
  occ::handle<Graphic3d_CView> aView = occ::down_cast<Graphic3d_CView>(theProjector);

  if (aView.IsNull() || !myDefinedViews.Contains(aView.operator->())
      || !myDisplayedStructure.Contains(theStructure))
  {
    return;
  }

  aView->ReCompute(theStructure);
}

//=================================================================================================

void Graphic3d_StructureManager::Clear(Graphic3d_Structure* theStructure,
                                       const bool           theWithDestruction)
{
  for (NCollection_IndexedMap<Graphic3d_CView*>::Iterator aViewIt(myDefinedViews); aViewIt.More();
       aViewIt.Next())
  {
    aViewIt.Value()->Clear(theStructure, theWithDestruction);
  }
}

//=================================================================================================

void Graphic3d_StructureManager::Connect(const Graphic3d_Structure* theMother,
                                         const Graphic3d_Structure* theDaughter)
{
  for (NCollection_IndexedMap<Graphic3d_CView*>::Iterator aViewIt(myDefinedViews); aViewIt.More();
       aViewIt.Next())
  {
    aViewIt.Value()->Connect(theMother, theDaughter);
  }
}

//=================================================================================================

void Graphic3d_StructureManager::Disconnect(const Graphic3d_Structure* theMother,
                                            const Graphic3d_Structure* theDaughter)
{
  for (NCollection_IndexedMap<Graphic3d_CView*>::Iterator aViewIt(myDefinedViews); aViewIt.More();
       aViewIt.Next())
  {
    aViewIt.Value()->Disconnect(theMother, theDaughter);
  }
}

//=================================================================================================

void Graphic3d_StructureManager::Display(const occ::handle<Graphic3d_Structure>& theStructure)
{
  myDisplayedStructure.Add(theStructure);

  for (NCollection_IndexedMap<Graphic3d_CView*>::Iterator aViewIt(myDefinedViews); aViewIt.More();
       aViewIt.Next())
  {
    aViewIt.Value()->Display(theStructure);
  }
}

//=================================================================================================

void Graphic3d_StructureManager::Erase(const occ::handle<Graphic3d_Structure>& theStructure)
{
  myDisplayedStructure.Remove(theStructure);
  myHighlightedStructure.Remove(theStructure);

  // Erase structure in all defined views
  for (NCollection_IndexedMap<Graphic3d_CView*>::Iterator aViewIt(myDefinedViews); aViewIt.More();
       aViewIt.Next())
  {
    aViewIt.Value()->Erase(theStructure);
  }
}

//=================================================================================================

void Graphic3d_StructureManager::Highlight(const occ::handle<Graphic3d_Structure>& theStructure)
{
  myHighlightedStructure.Add(theStructure);

  // Highlight in all defined views
  for (NCollection_IndexedMap<Graphic3d_CView*>::Iterator aViewIt(myDefinedViews); aViewIt.More();
       aViewIt.Next())
  {
    aViewIt.Value()->Highlight(theStructure);
  }
}

//=================================================================================================

void Graphic3d_StructureManager::UnHighlight(const occ::handle<Graphic3d_Structure>& theStructure)
{
  myHighlightedStructure.Remove(theStructure);

  // UnHighlight in all defined views
  for (NCollection_IndexedMap<Graphic3d_CView*>::Iterator aViewIt(myDefinedViews); aViewIt.More();
       aViewIt.Next())
  {
    aViewIt.Value()->UnHighlight(theStructure);
  }
}

//=================================================================================================

void Graphic3d_StructureManager::UnHighlight()
{
  for (NCollection_Map<occ::handle<Graphic3d_Structure>>::Iterator anIt(myHighlightedStructure);
       anIt.More();
       anIt.Next())
  {
    anIt.Key()->UnHighlight();
  }
}

//=================================================================================================

void Graphic3d_StructureManager::SetTransform(const occ::handle<Graphic3d_Structure>& theStructure,
                                              const occ::handle<TopLoc_Datum3D>&      theTrsf)
{
  for (NCollection_IndexedMap<Graphic3d_CView*>::Iterator aViewIt(myDefinedViews); aViewIt.More();
       aViewIt.Next())
  {
    aViewIt.Value()->SetTransform(theStructure, theTrsf);
  }
}

//=================================================================================================

void Graphic3d_StructureManager::ChangeDisplayPriority(
  const occ::handle<Graphic3d_Structure>& theStructure,
  const Graphic3d_DisplayPriority         theOldPriority,
  const Graphic3d_DisplayPriority         theNewPriority)
{
  for (NCollection_IndexedMap<Graphic3d_CView*>::Iterator aViewIt(myDefinedViews); aViewIt.More();
       aViewIt.Next())
  {
    aViewIt.Value()->ChangePriority(theStructure, theOldPriority, theNewPriority);
  }
}

//=================================================================================================

void Graphic3d_StructureManager::ChangeZLayer(const occ::handle<Graphic3d_Structure>& theStructure,
                                              const Graphic3d_ZLayerId                theLayerId)
{
  if (!myDisplayedStructure.Contains(theStructure))
  {
    return;
  }

  for (NCollection_IndexedMap<Graphic3d_CView*>::Iterator aViewIt(myDefinedViews); aViewIt.More();
       aViewIt.Next())
  {
    aViewIt.Value()->ChangeZLayer(theStructure, theLayerId);
  }
}

//=================================================================================================

void Graphic3d_StructureManager::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  for (NCollection_Map<occ::handle<Graphic3d_Structure>>::Iterator anIter(myDisplayedStructure);
       anIter.More();
       anIter.Next())
  {
    const occ::handle<Graphic3d_Structure>& aDisplayedStructure = anIter.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, aDisplayedStructure.get())
  }
  for (NCollection_Map<occ::handle<Graphic3d_Structure>>::Iterator anIter(myHighlightedStructure);
       anIter.More();
       anIter.Next())
  {
    const occ::handle<Graphic3d_Structure>& aHighlightedStructure = anIter.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, aHighlightedStructure.get())
  }
  for (NCollection_DataMap<const Standard_Transient*, occ::handle<Graphic3d_ViewAffinity>>::Iterator
         anIter(myRegisteredObjects);
       anIter.More();
       anIter.Next())
  {
    const occ::handle<Graphic3d_ViewAffinity>& aRegisteredObject = anIter.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, aRegisteredObject.get())
  }

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myGraphicDriver.get())
  for (NCollection_IndexedMap<Graphic3d_CView*>::Iterator anIter(myDefinedViews); anIter.More();
       anIter.Next())
  {
    Graphic3d_CView* aDefinedView = anIter.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, aDefinedView)
  }

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myDeviceLostFlag)
}
