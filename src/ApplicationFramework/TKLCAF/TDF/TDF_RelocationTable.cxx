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

//      	-----------------------
// Version:	0.0
// Version	Date		Purpose
//		0.0	Mar  7 1997	Creation

#include <Standard_Transient.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDF_RelocationTable, Standard_Transient)

//=================================================================================================

TDF_RelocationTable::TDF_RelocationTable(const bool selfRelocate)
    : mySelfRelocate(selfRelocate),
      myAfterRelocate(false)
{
}

//=================================================================================================

void TDF_RelocationTable::SelfRelocate(const bool selfRelocate)
{
  mySelfRelocate = selfRelocate;
}

//=================================================================================================

bool TDF_RelocationTable::SelfRelocate() const
{
  return mySelfRelocate;
}

//=================================================================================================

void TDF_RelocationTable::AfterRelocate(const bool afterRelocate)
{
  myAfterRelocate = afterRelocate;
}

//=================================================================================================

bool TDF_RelocationTable::AfterRelocate() const
{
  return myAfterRelocate;
}

//=======================================================================
// function : SetRelocation
// purpose  : Sets the relocation value of <aSourceLabel>
//           to <aTargetLabel>.
//=======================================================================

void TDF_RelocationTable::SetRelocation(const TDF_Label& aSourceLabel,
                                        const TDF_Label& aTargetLabel)
{
  if (!myLabelTable.IsBound(aSourceLabel))
    myLabelTable.Bind(aSourceLabel, aTargetLabel);
}

//=======================================================================
// function : HasRelocation
// purpose  : Finds the relocation value of <aSourceLabel>
//           and returns it into <aTargetLabel>.
//=======================================================================

bool TDF_RelocationTable::HasRelocation(const TDF_Label& aSourceLabel,
                                        TDF_Label&       aTargetLabel) const
{
  aTargetLabel.Nullify();
  if (myLabelTable.IsBound(aSourceLabel))
  {
    aTargetLabel = myLabelTable.Find(aSourceLabel);
    return true;
  }
  if (mySelfRelocate)
  {
    aTargetLabel = aSourceLabel;
    return !myAfterRelocate;
  }
  return false;
}

//=======================================================================
// function : SetRelocation
// purpose  : Sets the relocation value of <aSourceAttribute>
//           to <aTargetAttribute>.
//=======================================================================

void TDF_RelocationTable::SetRelocation(const occ::handle<TDF_Attribute>& aSourceAttribute,
                                        const occ::handle<TDF_Attribute>& aTargetAttribute)
{
  if (!myAttributeTable.IsBound(aSourceAttribute))
    myAttributeTable.Bind(aSourceAttribute, aTargetAttribute);
}

//=======================================================================
// function : HasRelocation
// purpose  : Finds the relocation value of <aSourceAttribute>
//           and returns it into <aTargetAttribute>.
//=======================================================================

bool TDF_RelocationTable::HasRelocation(const occ::handle<TDF_Attribute>& aSourceAttribute,
                                        occ::handle<TDF_Attribute>&       aTargetAttribute) const
{
  aTargetAttribute.Nullify();
  if (myAttributeTable.IsBound(aSourceAttribute))
  {
    aTargetAttribute = myAttributeTable.Find(aSourceAttribute);
    return true;
  }
  if (mySelfRelocate)
  {
    aTargetAttribute = aSourceAttribute;
    return !myAfterRelocate;
  }
  return false;
}

//=======================================================================
// function : SetTransientRelocation
// purpose  : Sets the relocation value of <aSourceTransient>
//           to <aTargetTransient>.
//=======================================================================

void TDF_RelocationTable::SetTransientRelocation(
  const occ::handle<Standard_Transient>& aSourceTransient,
  const occ::handle<Standard_Transient>& aTargetTransient)
{
  if (!myTransientTable.Contains(aSourceTransient))
    myTransientTable.Add(aSourceTransient, aTargetTransient);
}

//=======================================================================
// function : HasTransientRelocation
// purpose  : Finds the relocation value of <aSourceTransient>
//           and returns it into <aTargetTransient>.
//=======================================================================

bool TDF_RelocationTable::HasTransientRelocation(
  const occ::handle<Standard_Transient>& aSourceTransient,
  occ::handle<Standard_Transient>&       aTargetTransient) const
{
  aTargetTransient.Nullify();
  if (myTransientTable.Contains(aSourceTransient))
  {
    aTargetTransient = myTransientTable.FindFromKey(aSourceTransient);
    return true;
  }
  if (mySelfRelocate)
  {
    aTargetTransient = aSourceTransient;
    return !myAfterRelocate;
  }
  return false;
}

//=======================================================================
// function : Clear
// purpose  : Clears the relocation dictionary.
//=======================================================================

void TDF_RelocationTable::Clear()
{
  myLabelTable.Clear();
  myAttributeTable.Clear();
  myTransientTable.Clear();
}

//=================================================================================================

void TDF_RelocationTable::TargetLabelMap(NCollection_Map<TDF_Label>& aLabelMap) const
{
  for (NCollection_DataMap<TDF_Label, TDF_Label>::Iterator itr(myLabelTable); itr.More();
       itr.Next())
    aLabelMap.Add(itr.Value());
}

//=================================================================================================

void TDF_RelocationTable::TargetAttributeMap(
  NCollection_Map<occ::handle<TDF_Attribute>>& anAttributeMap) const
{
  for (NCollection_DataMap<occ::handle<TDF_Attribute>, occ::handle<TDF_Attribute>>::Iterator itr(
         myAttributeTable);
       itr.More();
       itr.Next())
    anAttributeMap.Add(itr.Value());
}

//=================================================================================================

NCollection_DataMap<TDF_Label, TDF_Label>& TDF_RelocationTable::LabelTable()
{
  return myLabelTable;
}

//=================================================================================================

NCollection_DataMap<occ::handle<TDF_Attribute>, occ::handle<TDF_Attribute>>& TDF_RelocationTable::
  AttributeTable()
{
  return myAttributeTable;
}

//=================================================================================================

NCollection_IndexedDataMap<occ::handle<Standard_Transient>, occ::handle<Standard_Transient>>&
  TDF_RelocationTable::TransientTable()
{
  return myTransientTable;
}

//=================================================================================================

Standard_OStream& TDF_RelocationTable::Dump(const bool        dumpLabels,
                                            const bool        dumpAttributes,
                                            const bool        dumpTransients,
                                            Standard_OStream& anOS) const
{
  anOS << "Relocation Table  ";
  if (mySelfRelocate)
    anOS << "IS";
  else
    anOS << "NOT";
  anOS << " self relocate ";
  if (myAfterRelocate)
    anOS << "WITH";
  else
    anOS << "WITHOUT";
  anOS << " after relocate" << std::endl;
  anOS << "Nb labels=" << myLabelTable.Extent();
  anOS << "  Nb attributes=" << myAttributeTable.Extent();
  anOS << "  Nb transients=" << myTransientTable.Extent() << std::endl;

  int nb = 0;
  if (dumpLabels)
  {
    anOS << "Label Table:" << std::endl;
    for (NCollection_DataMap<TDF_Label, TDF_Label>::Iterator itr(myLabelTable); itr.More();
         itr.Next())
    {
      ++nb;
      anOS << nb << " ";
      itr.Key().EntryDump(anOS);
      anOS << "<=>";
      itr.Value().EntryDump(anOS);
      anOS << "| ";
    }
    std::cout << std::endl;
  }

  nb = 0;
  if (dumpAttributes)
  {
    anOS << "Attribute Table:" << std::endl;
    for (NCollection_DataMap<occ::handle<TDF_Attribute>, occ::handle<TDF_Attribute>>::Iterator itr(
           myAttributeTable);
         itr.More();
         itr.Next())
    {
      ++nb;
      anOS << nb << " ";
      itr.Key()->Dump(anOS);
      anOS << "<=>";
      itr.Value()->Dump(anOS);
      anOS << "| ";
      anOS << std::endl;
    }
  }

  if (dumpTransients)
  {
    anOS << "Transient Table:" << myTransientTable.Extent() << " transient(s) in table."
         << std::endl;
  }

  return anOS;
}
