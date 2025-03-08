// Created by: DAUTRY Philippe
// Copyright (c) 1997-1999 Matra Datavision
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

//      	-------------
// Version:	0.0
// Version	Date		Purpose
//		0.0	Sep  8 1997	Creation

#include <Standard_OutOfRange.hxx>
#include <Standard_Dump.hxx>
#include <Standard_Type.hxx>
#include <TDF_AttributeDelta.hxx>
#include <TDF_Delta.hxx>
#include <TDF_ListIteratorOfAttributeDeltaList.hxx>
#include <TDF_ListIteratorOfLabelList.hxx>
#include <TDF_MapIteratorOfLabelMap.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDF_Delta, Standard_Transient)

#ifdef OCCT_DEBUG
  #include <Standard_ConstructionError.hxx>
#endif

#undef DEB_DELTA

//=================================================================================================

TDF_Delta::TDF_Delta()
    : myBeginTime(0),
      myEndTime(0)
{
}

//=================================================================================================

void TDF_Delta::Validity(const Standard_Integer aBeginTime, const Standard_Integer anEndTime)
{
  myBeginTime = aBeginTime;
  myEndTime   = anEndTime;
}

//=================================================================================================

void TDF_Delta::AddAttributeDelta(const Handle(TDF_AttributeDelta)& anAttributeDelta)
{
  if (!anAttributeDelta.IsNull())
    myAttDeltaList.Append(anAttributeDelta);
}

//=================================================================================================

void TDF_Delta::BeforeOrAfterApply(const Standard_Boolean before) const
{
  TDF_AttributeDeltaList ADlist;
  //  for (TDF_ListIteratorOfAttributeDeltaList itr(myAttDeltaList);
  TDF_ListIteratorOfAttributeDeltaList itr(myAttDeltaList);
  for (; itr.More(); itr.Next())
    ADlist.Append(itr.Value());

  Handle(TDF_AttributeDelta) attDelta;
  Handle(TDF_Attribute)      att;

  Standard_Boolean noDeadLock = Standard_True;
  Standard_Integer nbAD       = ADlist.Extent();
  Standard_Boolean next;
  while (noDeadLock && (nbAD != 0))
  {
    itr.Initialize(ADlist);
    while (itr.More())
    {
      next     = Standard_True;
      attDelta = itr.Value();
      att      = attDelta->Attribute();
      if (before)
        next = !att->BeforeUndo(attDelta);
      else
        next = !att->AfterUndo(attDelta);

      if (next)
        itr.Next();
      else
        ADlist.Remove(itr);
    }
    noDeadLock = (nbAD > ADlist.Extent());
    nbAD       = ADlist.Extent();
  }

  if (!noDeadLock)
  {
#ifdef OCCT_DEBUG
    if (before)
      std::cout << "Before";
    else
      std::cout << "After";
    std::cout << "Undo(): dead lock between these attributes:" << std::endl;
    for (itr.Initialize(ADlist); itr.More(); itr.Next())
    {
      std::cout << "AttributeDelta type = " << itr.Value()->DynamicType()->Name();
      std::cout << "  Attribute type = " << itr.Value()->Attribute()->DynamicType()->Name()
                << std::endl;
      if (before)
        std::cout << "BeforeUndo(): dead lock." << std::endl;
      else
        std::cout << "AfterUndo(): dead lock." << std::endl;
    }
#endif
    for (itr.Initialize(ADlist); itr.More(); itr.Next())
    {
      attDelta = itr.Value();
      att      = attDelta->Attribute();
      if (before)
        att->BeforeUndo(attDelta, Standard_True);
      else
        att->AfterUndo(attDelta, Standard_True);
    }
  }
}

//=================================================================================================

void TDF_Delta::Apply()
{
  TDF_ListIteratorOfAttributeDeltaList itr;
  for (itr.Initialize(myAttDeltaList); itr.More(); itr.Next())
  {
    const Handle(TDF_AttributeDelta)& attDelta = itr.Value();
    attDelta->Apply();
  }
}

//=================================================================================================

void TDF_Delta::Labels(TDF_LabelList& aLabelList) const
{
  TDF_LabelMap labMap;
  // If <aLabelList> is not empty...
#ifdef OCCT_DEBUG_DELTA
  Standard_Boolean inList;
  if (aLabelList.Extent() > 0)
    std::cout << "Previously added as modified label(s) ";
#endif
  for (TDF_ListIteratorOfLabelList it1(aLabelList); it1.More(); it1.Next())
  {
#ifdef OCCT_DEBUG_DELTA
    const TDF_Label& lab1 = it1.Value();
    inList                = labMap.Add(lab1);
    if (!inList)
    {
      lab1.EntryDump(std::cout);
      std::cout << " | ";
    }
#endif
  }
#ifdef OCCT_DEBUG_DELTA
  std::cout << std::endl;
#endif

  // New labels to add.
#ifdef OCCT_DEBUG_DELTA
  if (myAttDeltaList.Extent() > 0)
    std::cout << "New added as modified label(s) ";
#endif
  for (TDF_ListIteratorOfAttributeDeltaList it2(myAttDeltaList); it2.More(); it2.Next())
  {
#ifdef OCCT_DEBUG_DELTA
    const TDF_Label& lab1 = it2.Value()->Label();
    inList                = labMap.Add(lab1);
    if (!inList)
    {
      lab1.EntryDump(std::cout);
      std::cout << " | ";
    }
#endif
  }
#ifdef OCCT_DEBUG_DELTA
  std::cout << std::endl;
#endif

  // Now put labels into <aLabelList>.
  aLabelList.Clear();
  for (TDF_MapIteratorOfLabelMap it3(labMap); it3.More(); it3.Next())
  {
    aLabelList.Append(it3.Key());
  }
}

//=================================================================================================

void TDF_Delta::Dump(Standard_OStream& OS) const
{
  OS << "DELTA available from time \t#" << myBeginTime << " to time \t#" << myEndTime << std::endl;
  Standard_Integer n = 0;
  //  for (TDF_ListIteratorOfAttributeDeltaList itr(myAttDeltaList);
  TDF_ListIteratorOfAttributeDeltaList itr(myAttDeltaList);
  for (; itr.More(); itr.Next())
    ++n;
  OS << "Nb Attribute Delta(s): " << n << std::endl;
  for (itr.Initialize(myAttDeltaList); itr.More(); itr.Next())
  {
    const Handle(TDF_AttributeDelta)& attDelta = itr.Value();
    OS << "| ";
    attDelta->Dump(OS);
    OS << std::endl;
  }
}

//=================================================================================================

void TDF_Delta::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myBeginTime)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myEndTime)

  for (TDF_AttributeDeltaList::Iterator anAttDeltaListIt(myAttDeltaList); anAttDeltaListIt.More();
       anAttDeltaListIt.Next())
  {
    const Handle(TDF_AttributeDelta)& anAttDeltaList = anAttDeltaListIt.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, anAttDeltaList.get())
  }

  OCCT_DUMP_FIELD_VALUE_STRING(theOStream, myName)
}
