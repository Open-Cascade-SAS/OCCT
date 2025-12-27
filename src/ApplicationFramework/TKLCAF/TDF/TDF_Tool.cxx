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

//      	------------
// Version:	0.0
// Version	Date		Purpose
//		0.0	Mar 13 1997	Creation

#include <TCollection_AsciiString.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Data.hxx>
#include <TDF_DataSet.hxx>
#include <TDF_IDFilter.hxx>
#include <TDF_Label.hxx>
#include <TDF_Label.hxx>
#include <NCollection_List.hxx>
#include <Standard_Handle.hxx>
#include <TDF_Attribute.hxx>
#include <NCollection_Map.hxx>
#include <TDF_Label.hxx>
#include <NCollection_Map.hxx>
#include <TDF_Tool.hxx>

#define TDF_TagSeparator ':'

static void TDF_Tool_ExtendedDeepDump(Standard_OStream&        anOS,
                                      const TDF_Label&         aLabel,
                                      const TDF_IDFilter&      aFilter,
                                      NCollection_IndexedMap<occ::handle<TDF_Attribute>>& aMap);

static bool TDF_Tool_DescendantRef(const TDF_Label&           aRefLabel,
                                               const TDF_Label&           aLabel,
                                               const TDF_IDFilter&        aFilter,
                                               const occ::handle<TDF_DataSet>& ds);

static void TDF_Tool_OutReferers(const TDF_Label&           aRefLabel,
                                 const TDF_Label&           aLabel,
                                 NCollection_Map<occ::handle<TDF_Attribute>>&          atts,
                                 const TDF_IDFilter&        aFilterForReferers,
                                 const TDF_IDFilter&        aFilterForReferences,
                                 const occ::handle<TDF_DataSet>& ds);

static void TDF_Tool_OutReferences(const TDF_Label&           aRefLabel,
                                   const TDF_Label&           aLabel,
                                   NCollection_Map<occ::handle<TDF_Attribute>>&          atts,
                                   const TDF_IDFilter&        aFilterForReferers,
                                   const TDF_IDFilter&        aFilterForReferences,
                                   const occ::handle<TDF_DataSet>& ds);

//=======================================================================
// function : NbLabels
// purpose  : Returns the numbers of labels of the tree.
//=======================================================================

int TDF_Tool::NbLabels(const TDF_Label& aLabel)
{
  int n = 1;
  for (TDF_ChildIterator itr(aLabel, true); itr.More(); itr.Next())
    ++n;
  return n;
}

//=======================================================================
// function : NbAttributes
// purpose  : Returns the number of attributes of the tree.
//=======================================================================

int TDF_Tool::NbAttributes(const TDF_Label& aLabel)
{
  int n = aLabel.NbAttributes();
  for (TDF_ChildIterator itr(aLabel, true); itr.More(); itr.Next())
    n += itr.Value().NbAttributes();
  return n;
}

//=======================================================================
// function : NbAttributes
// purpose  : Returns the number of attributes of the tree,
//           selected by an IDFilter.
//=======================================================================

int TDF_Tool::NbAttributes(const TDF_Label& aLabel, const TDF_IDFilter& aFilter)
{
  int      n = 0;
  TDF_AttributeIterator it2;
  for (it2.Initialize(aLabel, true); it2.More(); it2.Next())
    if (aFilter.IsKept(it2.Value()))
      ++n;
  for (TDF_ChildIterator it1(aLabel, true); it1.More(); it1.Next())
    for (it2.Initialize(it1.Value(), true); it2.More(); it2.Next())
      if (aFilter.IsKept(it2.Value()))
        ++n;
  return n;
}

//=================================================================================================

bool TDF_Tool::IsSelfContained(const TDF_Label& aLabel)
{
  TDF_IDFilter filter(false); // Keep all.
  return IsSelfContained(aLabel, filter);
}

//=================================================================================================

bool TDF_Tool::IsSelfContained(const TDF_Label& aLabel, const TDF_IDFilter& aFilter)
{
  occ::handle<TDF_DataSet> ds = new TDF_DataSet();

  if (!TDF_Tool_DescendantRef(aLabel, aLabel, aFilter, ds))
    return false;

  for (TDF_ChildIterator itr(aLabel, true); itr.More(); itr.Next())
  {
    if (!TDF_Tool_DescendantRef(aLabel, itr.Value(), aFilter, ds))
      return false;
  }
  return true;
}

//=================================================================================================

static bool TDF_Tool_DescendantRef(const TDF_Label&           aRefLabel,
                                               const TDF_Label&           aLabel,
                                               const TDF_IDFilter&        aFilter,
                                               const occ::handle<TDF_DataSet>& ds)
{
  for (TDF_AttributeIterator itr(aLabel); itr.More(); itr.Next())
  {
    // CLE
    // const occ::handle<TDF_Attribute>& labAtt = itr.Value();
    occ::handle<TDF_Attribute> labAtt = itr.Value();
    // ENDCLE
    if (aFilter.IsKept(labAtt))
    {
      labAtt->References(ds);
      // First of all, the referenced labels.
      const NCollection_Map<TDF_Label>& labMap = ds->Labels();

      for (NCollection_Map<TDF_Label>::Iterator labMItr(labMap); labMItr.More(); labMItr.Next())
      {
        if (!labMItr.Key().IsDescendant(aRefLabel))
          return false;
      }
      // Then the referenced attributes.
      const NCollection_Map<occ::handle<TDF_Attribute>>& attMap = ds->Attributes();
      for (NCollection_Map<occ::handle<TDF_Attribute>>::Iterator attMItr(attMap); attMItr.More(); attMItr.Next())
      {
        // CLE
        // const occ::handle<TDF_Attribute>& att = attMItr.Key();
        const occ::handle<TDF_Attribute>& att = attMItr.Key();
        if (!att.IsNull() && !att->Label().IsNull())
        {
          // ENDCLE
          if (aFilter.IsKept(att) && !att->Label().IsDescendant(aRefLabel))
            return false;
        }
      }
      ds->Clear();
    }
  }
  return true;
}

//=================================================================================================

void TDF_Tool::OutReferers(const TDF_Label& aLabel, NCollection_Map<occ::handle<TDF_Attribute>>& atts)
{
  TDF_IDFilter filter(false); // Keep all.
  OutReferers(aLabel, filter, filter, atts);
}

//=================================================================================================

void TDF_Tool::OutReferers(const TDF_Label&    aLabel,
                           const TDF_IDFilter& aFilterForReferers,
                           const TDF_IDFilter& aFilterForReferences,
                           NCollection_Map<occ::handle<TDF_Attribute>>&   atts)
{
  occ::handle<TDF_DataSet> ds = new TDF_DataSet();
  TDF_Tool_OutReferers(aLabel, aLabel, atts, aFilterForReferers, aFilterForReferences, ds);
  for (TDF_ChildIterator itr(aLabel, true); itr.More(); itr.Next())
  {
    TDF_Tool_OutReferers(aLabel, itr.Value(), atts, aFilterForReferers, aFilterForReferences, ds);
  }
}

//=================================================================================================

static void TDF_Tool_OutReferers(const TDF_Label&           aRefLabel,
                                 const TDF_Label&           aLabel,
                                 NCollection_Map<occ::handle<TDF_Attribute>>&          atts,
                                 const TDF_IDFilter&        aFilterForReferers,
                                 const TDF_IDFilter&        aFilterForReferences,
                                 const occ::handle<TDF_DataSet>& ds)
{
  bool outRefFound = false;

  for (TDF_AttributeIterator itr(aLabel); itr.More(); itr.Next())
  {

    if (!aFilterForReferers.IsKept(itr.Value()))
      continue;
    itr.Value()->References(ds);

    const NCollection_Map<occ::handle<TDF_Attribute>>& attMap = ds->Attributes();
    for (NCollection_Map<occ::handle<TDF_Attribute>>::Iterator attMItr(attMap); attMItr.More(); attMItr.Next())
    {
      // CLE
      // const occ::handle<TDF_Attribute>& att = attMItr.Key();
      const occ::handle<TDF_Attribute>& att = attMItr.Key();
      // ENDCLE
      if (aFilterForReferences.IsKept(att) && !att->Label().IsNull()
          && !att->Label().IsDescendant(aRefLabel))
      {
        atts.Add(itr.Value());
        outRefFound = true;
        break;
      }
    }

    if (!outRefFound)
    {
      const NCollection_Map<TDF_Label>& labMap = ds->Labels();
      for (NCollection_Map<TDF_Label>::Iterator labMItr(labMap); labMItr.More(); labMItr.Next())
      {
        if (!labMItr.Key().IsDescendant(aRefLabel))
        {
          atts.Add(itr.Value());
          break;
        }
      }
    }

    outRefFound = false;
    ds->Clear();
  }
}

//=================================================================================================

void TDF_Tool::OutReferences(const TDF_Label& aLabel, NCollection_Map<occ::handle<TDF_Attribute>>& atts)
{
  TDF_IDFilter filter(false); // Keep all.
  OutReferences(aLabel, filter, filter, atts);
}

//=================================================================================================

void TDF_Tool::OutReferences(const TDF_Label&    aLabel,
                             const TDF_IDFilter& aFilterForReferers,
                             const TDF_IDFilter& aFilterForReferences,
                             NCollection_Map<occ::handle<TDF_Attribute>>&   atts)
{
  occ::handle<TDF_DataSet> ds = new TDF_DataSet();
  TDF_Tool_OutReferences(aLabel, aLabel, atts, aFilterForReferers, aFilterForReferences, ds);
  for (TDF_ChildIterator itr(aLabel, true); itr.More(); itr.Next())
  {
    TDF_Tool_OutReferences(aLabel, itr.Value(), atts, aFilterForReferers, aFilterForReferences, ds);
  }
}

//=================================================================================================

static void TDF_Tool_OutReferences(const TDF_Label&           aRefLabel,
                                   const TDF_Label&           aLabel,
                                   NCollection_Map<occ::handle<TDF_Attribute>>&          atts,
                                   const TDF_IDFilter&        aFilterForReferers,
                                   const TDF_IDFilter&        aFilterForReferences,
                                   const occ::handle<TDF_DataSet>& ds)
{
  for (TDF_AttributeIterator itr(aLabel); itr.More(); itr.Next())
  {
    if (!aFilterForReferers.IsKept(itr.Value()))
      continue;
    itr.Value()->References(ds);
    const NCollection_Map<occ::handle<TDF_Attribute>>& attMap = ds->Attributes();
    for (NCollection_Map<occ::handle<TDF_Attribute>>::Iterator attMItr(attMap); attMItr.More(); attMItr.Next())
    {
      const occ::handle<TDF_Attribute>& att = attMItr.Key();
      if (aFilterForReferences.IsKept(att) && !att->Label().IsNull()
          && !att->Label().IsDescendant(aRefLabel))
      {
        atts.Add(att);
      }
    }
    const NCollection_Map<TDF_Label>& labMap = ds->Labels();
    for (NCollection_Map<TDF_Label>::Iterator labMItr(labMap); labMItr.More(); labMItr.Next())
    {
      if (!labMItr.Key().IsDescendant(aRefLabel))
      {
        TDF_AttributeIterator itra(labMItr.Key());
        for (; itra.More(); itra.Next())
        {
          if (aFilterForReferences.IsKept(itra.Value()))
          {
            atts.Add(itra.Value());
          }
        }
      }
    }
  }
  ds->Clear();
}

//=================================================================================================

void TDF_Tool::RelocateLabel(const TDF_Label&       aSourceLabel,
                             const TDF_Label&       fromRoot,
                             const TDF_Label&       toRoot,
                             TDF_Label&             aTargetLabel,
                             const bool create)
{
  if (!aSourceLabel.IsDescendant(fromRoot))
    return;
  aTargetLabel.Nullify();
  NCollection_List<int> labelTags;
  TDF_Tool::TagList(aSourceLabel, labelTags);
  NCollection_List<int> toTags;
  TDF_Tool::TagList(toRoot, toTags);
  for (int i = fromRoot.Depth(); i >= 0; --i)
    labelTags.RemoveFirst();
  labelTags.Prepend(toTags);
  TDF_Tool::Label(toRoot.Data(), labelTags, aTargetLabel, create);
}

//=======================================================================
// function : Entry
// purpose  : Returns the entry as an ascii string.
//=======================================================================

void TDF_Tool::Entry(const TDF_Label& aLabel, TCollection_AsciiString& anEntry)
{
  if (!aLabel.IsNull())
  {
    int       aStrLen = 1; // initial "0" of a root label
    TDF_Label aLab    = aLabel;
    for (; !aLab.IsRoot(); aLab = aLab.Father())
    {
      for (int aTag = aLab.Tag(); aTag > 9; aTag /= 10)
        ++aStrLen;
      aStrLen += 2; // one digit and separator
    }

    if (aStrLen == 1)
    {
      // an exceptional case for the root label, it ends with separator
      static const TCollection_AsciiString THE_ROOT_ENTRY =
        TCollection_AsciiString('0') + TDF_TagSeparator;
      anEntry = THE_ROOT_ENTRY;
    }
    else
    {
      anEntry                  = TCollection_AsciiString(aStrLen, TDF_TagSeparator);
      char* aPtr = const_cast<char*>(anEntry.ToCString() + aStrLen - 1);
      for (aLab = aLabel; !aLab.IsRoot(); aLab = aLab.Father())
      {
        int aTag = aLab.Tag();
        for (; aTag > 9; --aPtr, aTag /= 10)
          *aPtr = char(aTag % 10) + '0';
        *aPtr = char(aTag) + '0';
        aPtr -= 2;
      }
      *aPtr = '0';
    }
  }
  else
    anEntry.Clear();
}

//=======================================================================
// function : TagList
// purpose  : Returns the entry of a label as a list of integers.
//=======================================================================

void TDF_Tool::TagList(const TDF_Label& aLabel, NCollection_List<int>& aTagList)
{
  aTagList.Clear();
  if (!aLabel.IsNull())
  {
    TDF_Label Label = aLabel;
    for (;;)
    {
      aTagList.Prepend(Label.Tag());
      if (Label.IsRoot())
        break;
      Label = Label.Father();
    }
  }
}

//=======================================================================
// function : TagList
// purpose  : Returns the entry expressed as a string as a list of integers.
//=======================================================================

void TDF_Tool::TagList(const TCollection_AsciiString& anEntry, NCollection_List<int>& aTagList)
{
  char*            cc = (char*)anEntry.ToCString();
  int n  = 0;
  aTagList.Clear();
  while (*cc != '\0')
  {
    while (*cc >= '0' && *cc <= '9')
    {
      n = 10 * n + (*cc - '0');
      ++cc;
    }
    if (*cc == TDF_TagSeparator || *cc == '\0')
    {
      aTagList.Append(n);
      n = 0;
      if (*cc != '\0')
        ++cc;
    }
    else
    { // Not an entry!
      aTagList.Clear();
      break;
    }
  }
}

//=======================================================================
// function : Label
// purpose  : Returns the label expressed by <anEntry>.
//=======================================================================

void TDF_Tool::Label(const occ::handle<TDF_Data>&        aDF,
                     const TCollection_AsciiString& anEntry,
                     TDF_Label&                     aLabel,
                     const bool         create)
{
  bool isFound = false;
  if (aDF->IsAccessByEntries())
    isFound = aDF->GetLabel(anEntry, aLabel);

  if (!isFound)
    TDF_Tool::Label(aDF, anEntry.ToCString(), aLabel, create);
}

//=======================================================================
// function : Label
// purpose  : Returns the label expressed by <anEntry>,
//           and creates it if <create> is true.
//=======================================================================

void TDF_Tool::Label(const occ::handle<TDF_Data>& aDF,
                     const char*  anEntry,
                     TDF_Label&              aLabel,
                     const bool  create)
{
  bool isFound = false;
  if (aDF->IsAccessByEntries())
    isFound = aDF->GetLabel(anEntry, aLabel);

  if (!isFound)
  {
    NCollection_List<int> tagList;
    TDF_Tool::TagList(anEntry, tagList);
    TDF_Tool::Label(aDF, tagList, aLabel, create);
  }
}

//=======================================================================
// function : Label
// purpose  : Returns the label expressed by <anEntry>,
//           and creates it if <create> is true.
//=======================================================================

void TDF_Tool::Label(const occ::handle<TDF_Data>&      aDF,
                     const NCollection_List<int>& aTagList,
                     TDF_Label&                   aLabel,
                     const bool       create)
{
  if (aTagList.Extent() == 0)
  {
    aLabel.Nullify();
  }
  else
  {
    aLabel = aDF->Root();
    if (aTagList.Extent() == 1 && aTagList.First() == 0)
      return;
    else
    {
      NCollection_List<int>::Iterator tagItr(aTagList);
      tagItr.Next(); // Suppresses root tag.
      for (; !aLabel.IsNull() && tagItr.More(); tagItr.Next())
      {
        aLabel = aLabel.FindChild(tagItr.Value(), create);
      }
    }
  }
}

//=================================================================================================

void TDF_Tool::CountLabels(NCollection_List<TDF_Label>& aLabelList, NCollection_DataMap<TDF_Label, int>& aLabelMap)
{
  if (aLabelList.IsEmpty())
    return;
  bool            next = true;
  NCollection_List<TDF_Label>::Iterator itr(aLabelList);
  while (itr.More())
  {
    const TDF_Label& lab = itr.Value();
    if (aLabelMap.IsBound(lab))
    {
      aLabelMap(lab) += 1;
      aLabelList.Remove(itr);
      next = false;
    }
    else
    {
      aLabelMap.Bind(lab, 1);
      next = itr.More();
    }
    if (next && !aLabelList.IsEmpty())
      itr.Next();
  }
}

//=================================================================================================

void TDF_Tool::DeductLabels(NCollection_List<TDF_Label>& aLabelList, NCollection_DataMap<TDF_Label, int>& aLabelMap)
{
  if (aLabelList.IsEmpty())
    return;
  bool            next = true;
  NCollection_List<TDF_Label>::Iterator itr(aLabelList);
  while (itr.More())
  {
    const TDF_Label& lab = itr.Value();
    if (aLabelMap.IsBound(lab))
    {
      aLabelMap(lab) -= 1;
      if (aLabelMap(lab) == 0)
      {
        aLabelMap.UnBind(lab);
        aLabelList.Remove(itr);
        next = false;
      }
    }
    else
      next = itr.More();
    if (next && !aLabelList.IsEmpty())
      itr.Next();
  }
}

//=================================================================================================

void TDF_Tool::DeepDump(Standard_OStream& anOS, const occ::handle<TDF_Data>& aDF)
{
  anOS << aDF;
  TDF_Tool::DeepDump(anOS, aDF->Root());
}

//=======================================================================
// function : ExtendedDeepDump
// purpose  : Extended deep dump of a DF.
//=======================================================================

void TDF_Tool::ExtendedDeepDump(Standard_OStream&       anOS,
                                const occ::handle<TDF_Data>& aDF,
                                const TDF_IDFilter&     aFilter)
{
  anOS << aDF;
  TDF_Tool::ExtendedDeepDump(anOS, aDF->Root(), aFilter);
}

//=================================================================================================

void TDF_Tool::DeepDump(Standard_OStream& anOS, const TDF_Label& aLabel)
{
  // Dumps the label.
  anOS << aLabel;
  // Its children
  for (TDF_ChildIterator ChildIt(aLabel); ChildIt.More(); ChildIt.Next())
  {
    TDF_Tool::DeepDump(anOS, ChildIt.Value());
  }
}

//=======================================================================
// function : ExtendedDeepDump
// purpose  : Extended deep dump of a label.
//=======================================================================

void TDF_Tool::ExtendedDeepDump(Standard_OStream&   anOS,
                                const TDF_Label&    aLabel,
                                const TDF_IDFilter& aFilter)
{
  NCollection_IndexedMap<occ::handle<TDF_Attribute>> map;
  TDF_Tool_ExtendedDeepDump(anOS, aLabel, aFilter, map);

  anOS << map.Extent() << " attribute";
  if (map.Extent() > 1)
    anOS << "s";
  anOS << " referenced by the label structure." << std::endl;

  anOS << std::endl << "Extended dump of filtered attribute(s):" << std::endl;
  int        nba = 0;
  TCollection_AsciiString entry;
  int        i;
  for (i = 1; i <= map.Extent(); ++i)
  {
    const occ::handle<TDF_Attribute>& att = map.FindKey(i);
    if (aFilter.IsKept(att))
    {
      ++nba;
      anOS << "# " << i;
      if (att->Label().IsNull())
      {
        anOS << " (no label)" << std::endl;
      }
      else
      {
        TDF_Tool::Entry(att->Label(), entry);
        anOS << " (label: " << entry << ")" << std::endl;
      }
      att->ExtendedDump(anOS, aFilter, map);
      anOS << std::endl;
    }
  }
  anOS << std::endl << nba << " attribute";
  if (nba > 1)
    anOS << "s";
  anOS << " dumped between " << --i << std::endl;
}

//=================================================================================================

static void TDF_Tool_ExtendedDeepDump(Standard_OStream&        anOS,
                                      const TDF_Label&         aLabel,
                                      const TDF_IDFilter&      aFilter,
                                      NCollection_IndexedMap<occ::handle<TDF_Attribute>>& aMap)
{
  // Dumps the label.
  aLabel.ExtendedDump(anOS, aFilter, aMap);
  // Its children
  for (TDF_ChildIterator ChildIt(aLabel); ChildIt.More(); ChildIt.Next())
  {
    TDF_Tool_ExtendedDeepDump(anOS, ChildIt.Value(), aFilter, aMap);
  }
}
