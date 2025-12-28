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

//      	----------------------
// Version:	0.0
// Version	Date		Purpose
//		0.0	Sep  4 1997	Creation

#include <TDF_AttributeIterator.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_ComparisonTool.hxx>
#include <TDF_DataSet.hxx>
#include <TDF_IDFilter.hxx>
#include <TDF_Label.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_List.hxx>
#include <TDF_RelocationTable.hxx>

//=======================================================================
// function : Compare
// purpose  : Comparison method between 2 DataSets.
//=======================================================================
void TDF_ComparisonTool::Compare(const occ::handle<TDF_DataSet>&         aSourceDataSet,
                                 const occ::handle<TDF_DataSet>&         aTargetDataSet,
                                 const TDF_IDFilter&                     aFilter,
                                 const occ::handle<TDF_RelocationTable>& aRelocationTable)
{
  if (aSourceDataSet->IsEmpty() || aTargetDataSet->IsEmpty())
    return;

  const NCollection_List<TDF_Label>&    srcRoots = aSourceDataSet->Roots();
  NCollection_List<TDF_Label>::Iterator srcRootItr(srcRoots);

  const NCollection_List<TDF_Label>&    trgRoots = aTargetDataSet->Roots();
  NCollection_List<TDF_Label>::Iterator trgRootItr;

  NCollection_DataMap<TDF_Label, TDF_Label>& the2LabMap = aRelocationTable->LabelTable();

  // Try to match source and target roots by their tag.
  for (; srcRootItr.More(); srcRootItr.Next())
  {
    const TDF_Label& srcLab = srcRootItr.Value();
    for (trgRootItr.Initialize(trgRoots); trgRootItr.More(); trgRootItr.Next())
    {
      const TDF_Label& trgLab = trgRootItr.Value();
      if (srcLab.Tag() == trgLab.Tag())
      {
        the2LabMap.Bind(srcLab, trgLab);
        // Now, compare recursively!
        TDF_ComparisonTool::Compare(srcLab,
                                    trgLab,
                                    aSourceDataSet,
                                    aTargetDataSet,
                                    aFilter,
                                    aRelocationTable);
        break;
      }
    }
  }

  // The relocation attribute table is now ready,
  // except for the label unattached attributes,
  // because we cannot treat them.
}

//=======================================================================
// function : Compare
// purpose  : Internal recursive comparison method.
//=======================================================================

void TDF_ComparisonTool::Compare(const TDF_Label&                        aSrcLabel,
                                 const TDF_Label&                        aTrgLabel,
                                 const occ::handle<TDF_DataSet>&         aSourceDataSet,
                                 const occ::handle<TDF_DataSet>&         aTargetDataSet,
                                 const TDF_IDFilter&                     aFilter,
                                 const occ::handle<TDF_RelocationTable>& aRelocationTable)
{
  NCollection_DataMap<TDF_Label, TDF_Label>& the2LabMap = aRelocationTable->LabelTable();
  NCollection_DataMap<occ::handle<TDF_Attribute>, occ::handle<TDF_Attribute>>& the2AttMap =
    aRelocationTable->AttributeTable();

  occ::handle<TDF_Attribute> tAtt;

  // Compare source and target attributes.
  for (TDF_AttributeIterator attItr(aSrcLabel); attItr.More(); attItr.Next())
  {
    const occ::handle<TDF_Attribute> sAtt = attItr.Value();
    if (aFilter.IsKept(sAtt) && aSourceDataSet->ContainsAttribute(sAtt))
    {
      if (aTrgLabel.FindAttribute(sAtt->ID(), tAtt))
      {
        if (aTargetDataSet->ContainsAttribute(tAtt))
          the2AttMap.Bind(sAtt, tAtt);
      }
    }
  }

  // Do the same for the children.
  TDF_ChildIterator childItr1, childItr2;
  for (childItr1.Initialize(aSrcLabel); childItr1.More(); childItr1.Next())
  {
    const TDF_Label& childSrcLab = childItr1.Value();
    if (aSourceDataSet->ContainsLabel(childSrcLab))
    {
      for (childItr2.Initialize(aSrcLabel); childItr2.More(); childItr2.Next())
      {
        const TDF_Label& childTrgLab = childItr2.Value();
        if (aTargetDataSet->ContainsLabel(childTrgLab))
        {
          if (childSrcLab.Tag() == childTrgLab.Tag())
          {
            the2LabMap.Bind(childSrcLab, childTrgLab);
            TDF_ComparisonTool::Compare(childSrcLab,
                                        childTrgLab,
                                        aSourceDataSet,
                                        aTargetDataSet,
                                        aFilter,
                                        aRelocationTable);
            break;
          }
        }
      }
    }
  }
}

//=================================================================================================

bool TDF_ComparisonTool::SourceUnbound(const occ::handle<TDF_DataSet>&         aRefDataSet,
                                       const occ::handle<TDF_RelocationTable>& aRelocationTable,
                                       const TDF_IDFilter&                     aFilter,
                                       const occ::handle<TDF_DataSet>&         aDiffDataSet,
                                       const int                               anOption)
{
  if (aRefDataSet->IsEmpty())
    return false;
  else
    return Unbound(aRefDataSet, aRelocationTable, aFilter, aDiffDataSet, anOption, true);
}

//=================================================================================================

bool TDF_ComparisonTool::TargetUnbound(const occ::handle<TDF_DataSet>&         aRefDataSet,
                                       const occ::handle<TDF_RelocationTable>& aRelocationTable,
                                       const TDF_IDFilter&                     aFilter,
                                       const occ::handle<TDF_DataSet>&         aDiffDataSet,
                                       const int                               anOption)
{
  if (aRefDataSet->IsEmpty())
    return false;
  else
    return Unbound(aRefDataSet, aRelocationTable, aFilter, aDiffDataSet, anOption, false);
}

//=======================================================================
// function : Unbound
// purpose  : Internal function used by SourceUnbound and TargetUnbound.
//=======================================================================

bool TDF_ComparisonTool::Unbound(const occ::handle<TDF_DataSet>&         aRefDataSet,
                                 const occ::handle<TDF_RelocationTable>& aRelocationTable,
                                 const TDF_IDFilter&                     aFilter,
                                 const occ::handle<TDF_DataSet>&         aDiffDataSet,
                                 const int                               anOption,
                                 const bool                              theSource)
{
  bool hasDiff = false;

  // Labels
  if ((anOption & 1) != 0)
  {
    const NCollection_Map<TDF_Label>&                refLabs    = aRefDataSet->Labels();
    NCollection_Map<TDF_Label>&                      diffLabs   = aDiffDataSet->Labels();
    const NCollection_DataMap<TDF_Label, TDF_Label>& the2LabMap = aRelocationTable->LabelTable();
    NCollection_Map<TDF_Label>                       theTLabMap;
    if (!theSource)
      aRelocationTable->TargetLabelMap(theTLabMap);
    for (NCollection_Map<TDF_Label>::Iterator refLabMItr(refLabs); refLabMItr.More();
         refLabMItr.Next())
    {
      const TDF_Label& refLab = refLabMItr.Key();
      if (!(theSource ? the2LabMap.IsBound(refLab) : theTLabMap.Contains(refLab)))
        diffLabs.Add(refLab);
    }
    hasDiff = (diffLabs.Extent() > 0);
  }

  // Attributes
  if ((anOption & 2) != 0)
  {
    const NCollection_Map<occ::handle<TDF_Attribute>>& refAtts  = aRefDataSet->Attributes();
    NCollection_Map<occ::handle<TDF_Attribute>>&       diffAtts = aDiffDataSet->Attributes();
    const NCollection_DataMap<occ::handle<TDF_Attribute>, occ::handle<TDF_Attribute>>& the2AttMap =
      aRelocationTable->AttributeTable();
    NCollection_Map<occ::handle<TDF_Attribute>> theTAttMap;
    if (!theSource)
      aRelocationTable->TargetAttributeMap(theTAttMap);
    for (NCollection_Map<occ::handle<TDF_Attribute>>::Iterator refAttMItr(refAtts);
         refAttMItr.More();
         refAttMItr.Next())
    {
      const occ::handle<TDF_Attribute>& refAtt = refAttMItr.Key();
      if (aFilter.IsKept(refAtt))
      {
        if (!(theSource ? the2AttMap.IsBound(refAtt) : theTAttMap.Contains(refAtt)))
          diffAtts.Add(refAtt);
      }
    }
    hasDiff = (hasDiff || diffAtts.Extent() > 0);
  }

  return hasDiff;
}

//=======================================================================
// function : Cut
// purpose  : Removes the attributes contained into <aDataSet>
//=======================================================================

void TDF_ComparisonTool::Cut(const occ::handle<TDF_DataSet>& aDataSet)
{
  if (aDataSet->IsEmpty())
    return;

  const NCollection_Map<occ::handle<TDF_Attribute>>& refAtts = aDataSet->Attributes();

  // Removes the attributes.
  NCollection_Map<occ::handle<TDF_Attribute>>::Iterator refAttMItr(refAtts);
  for (; refAttMItr.More(); refAttMItr.Next())
  {
    const occ::handle<TDF_Attribute>& locAtt = refAttMItr.Key();
    locAtt->Label().ForgetAttribute(locAtt);
  }
}

//=======================================================================
// function : IsSelfContained
// purpose  : Returns true if all the labels of <aDataSet> are
//          descendant of <aLabel>.
//=======================================================================

bool TDF_ComparisonTool::IsSelfContained(const TDF_Label&                aLabel,
                                         const occ::handle<TDF_DataSet>& aDataSet)
{
  if (!aDataSet->IsEmpty())
  {
    const NCollection_Map<TDF_Label>& refLabs = aDataSet->Labels();
    for (NCollection_Map<TDF_Label>::Iterator refLabMItr(refLabs); refLabMItr.More();
         refLabMItr.Next())
    {
      if (!refLabMItr.Key().IsDescendant(aLabel))
        return false;
    }
  }
  return true;
}
