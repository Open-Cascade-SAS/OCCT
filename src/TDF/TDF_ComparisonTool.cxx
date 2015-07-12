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
//Version	Date		Purpose
//		0.0	Sep  4 1997	Creation

#include <TDF_Attribute.hxx>
#include <TDF_AttributeDataMap.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_AttributeMap.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_ComparisonTool.hxx>
#include <TDF_DataMapIteratorOfAttributeDataMap.hxx>
#include <TDF_DataMapIteratorOfLabelDataMap.hxx>
#include <TDF_DataSet.hxx>
#include <TDF_IDFilter.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelDataMap.hxx>
#include <TDF_LabelMap.hxx>
#include <TDF_ListIteratorOfLabelList.hxx>
#include <TDF_MapIteratorOfAttributeMap.hxx>
#include <TDF_MapIteratorOfLabelMap.hxx>
#include <TDF_RelocationTable.hxx>

//=======================================================================
//function : Compare
//purpose  : Comparison method between 2 DataSets.
//=======================================================================
void TDF_ComparisonTool::Compare
(const Handle(TDF_DataSet)& aSourceDataSet,
 const Handle(TDF_DataSet)& aTargetDataSet,
 const TDF_IDFilter& aFilter,
 const Handle(TDF_RelocationTable)& aRelocationTable) 
{
  if (aSourceDataSet->IsEmpty() || aTargetDataSet->IsEmpty()) return;

  const TDF_LabelList& srcRoots = aSourceDataSet->Roots();
  TDF_ListIteratorOfLabelList srcRootItr(srcRoots);

  const TDF_LabelList& trgRoots = aTargetDataSet->Roots();
  TDF_ListIteratorOfLabelList trgRootItr;

  TDF_LabelDataMap& the2LabMap  = aRelocationTable->LabelTable();

  // Try to match source and target roots by their tag.
  for (; srcRootItr.More(); srcRootItr.Next()) {
    const TDF_Label& srcLab = srcRootItr.Value();
    for (trgRootItr.Initialize(trgRoots);trgRootItr.More();trgRootItr.Next()){
      const TDF_Label& trgLab = trgRootItr.Value();
      if (srcLab.Tag() == trgLab.Tag()) {
	the2LabMap.Bind(srcLab, trgLab);
	// Now, compare recursively!
	TDF_ComparisonTool::Compare(srcLab,trgLab,
				    aSourceDataSet, aTargetDataSet, aFilter,
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
//function : Compare
//purpose  : Internal recursive comparison method.
//=======================================================================

void TDF_ComparisonTool::Compare
(const TDF_Label& aSrcLabel,
 const TDF_Label& aTrgLabel,
 const Handle(TDF_DataSet)& aSourceDataSet,
 const Handle(TDF_DataSet)& aTargetDataSet,
 const TDF_IDFilter& aFilter,
 const Handle(TDF_RelocationTable)& aRelocationTable) 
{
  TDF_LabelDataMap&     the2LabMap  = aRelocationTable->LabelTable();
  TDF_AttributeDataMap& the2AttMap  = aRelocationTable->AttributeTable();

  Handle(TDF_Attribute) tAtt;

  // Compare source and target attributes.
  for (TDF_AttributeIterator attItr(aSrcLabel); attItr.More(); attItr.Next()){
    const Handle(TDF_Attribute) sAtt = attItr.Value();
    if (aFilter.IsKept(sAtt) && aSourceDataSet->ContainsAttribute(sAtt)) {
      if (aTrgLabel.FindAttribute(sAtt->ID(),tAtt)) {
	if (aTargetDataSet->ContainsAttribute(tAtt))
	  the2AttMap.Bind(sAtt,tAtt);
      }
    }
  }

  // Do the same for the children.
  TDF_ChildIterator childItr1, childItr2;
  for (childItr1.Initialize(aSrcLabel); childItr1.More(); childItr1.Next()){
    const TDF_Label& childSrcLab = childItr1.Value();
    if (aSourceDataSet->ContainsLabel(childSrcLab)) {
      for (childItr2.Initialize(aSrcLabel);childItr2.More();childItr2.Next()){
	const TDF_Label& childTrgLab = childItr2.Value();
	if (aTargetDataSet->ContainsLabel(childTrgLab)) {
	  if (childSrcLab.Tag() == childTrgLab.Tag()) {
	    the2LabMap.Bind(childSrcLab, childTrgLab);
	    TDF_ComparisonTool::Compare(childSrcLab,childTrgLab,
					aSourceDataSet,aTargetDataSet,aFilter,
					aRelocationTable);
	    break;
	  }
	}
      }
    }
  }
}


//=======================================================================
//function : SourceUnbound
//purpose  : 
//=======================================================================

Standard_Boolean TDF_ComparisonTool::SourceUnbound
(const Handle(TDF_DataSet)& aRefDataSet,
 const Handle(TDF_RelocationTable)& aRelocationTable,
 const TDF_IDFilter& aFilter,
 const Handle(TDF_DataSet)& aDiffDataSet,
 const Standard_Integer anOption)
{
  if (aRefDataSet->IsEmpty()) return Standard_False;
  else return Unbound(aRefDataSet, aRelocationTable, aFilter,
		      aDiffDataSet, anOption, Standard_True);
}


//=======================================================================
//function : TargetUnbound
//purpose  : 
//=======================================================================

Standard_Boolean TDF_ComparisonTool::TargetUnbound
(const Handle(TDF_DataSet)& aRefDataSet,
 const Handle(TDF_RelocationTable)& aRelocationTable,
 const TDF_IDFilter& aFilter,
 const Handle(TDF_DataSet)& aDiffDataSet,
 const Standard_Integer anOption)
{
  if (aRefDataSet->IsEmpty()) return Standard_False;
  else return Unbound(aRefDataSet, aRelocationTable, aFilter,
		      aDiffDataSet, anOption, Standard_False);
}


//=======================================================================
//function : Unbound
//purpose  : Internal function used by SourceUnbound and TargetUnbound.
//=======================================================================

Standard_Boolean TDF_ComparisonTool::Unbound
(const Handle(TDF_DataSet)& aRefDataSet,
 const Handle(TDF_RelocationTable)& aRelocationTable,
 const TDF_IDFilter& aFilter,
 const Handle(TDF_DataSet)& aDiffDataSet,
 const Standard_Integer anOption,
 const Standard_Boolean theSource)
{
  Standard_Boolean hasDiff = Standard_False;

  // Labels
  if ((anOption & 1) != 0) {
    const TDF_LabelMap&      refLabs    = aRefDataSet->Labels();
    TDF_LabelMap&            diffLabs   = aDiffDataSet->Labels();
    const TDF_LabelDataMap&  the2LabMap = aRelocationTable->LabelTable();
    TDF_LabelMap             theTLabMap;
    if (!theSource) aRelocationTable->TargetLabelMap(theTLabMap);
    for (TDF_MapIteratorOfLabelMap refLabMItr(refLabs);
	 refLabMItr.More(); refLabMItr.Next()) {
      const TDF_Label& refLab = refLabMItr.Key();
      if (!(theSource ?
	    the2LabMap.IsBound(refLab)
	    : theTLabMap.Contains(refLab)))
	diffLabs.Add(refLab);
    }
    hasDiff = (diffLabs.Extent()>0);
  }

  // Attributes
  if ((anOption & 2) != 0) {
    const TDF_AttributeMap&     refAtts    = aRefDataSet->Attributes();
    TDF_AttributeMap&           diffAtts   = aDiffDataSet->Attributes();
    const TDF_AttributeDataMap& the2AttMap =aRelocationTable->AttributeTable();
    TDF_AttributeMap            theTAttMap;
    if (!theSource) aRelocationTable->TargetAttributeMap(theTAttMap);
    for (TDF_MapIteratorOfAttributeMap refAttMItr(refAtts);
	 refAttMItr.More(); refAttMItr.Next()) {
      const Handle(TDF_Attribute)& refAtt = refAttMItr.Key();
      if (aFilter.IsKept(refAtt)) {
	if (!(theSource ?
	      the2AttMap.IsBound(refAtt)
	      : theTAttMap.Contains(refAtt)))
	  diffAtts.Add(refAtt);
      }
    }
    hasDiff = (hasDiff || diffAtts.Extent()>0);
  }

  return hasDiff;
}


//=======================================================================
//function : Cut
//purpose  : Removes the attributes contained into <aDataSet>
//=======================================================================

void TDF_ComparisonTool::Cut
(const Handle(TDF_DataSet)& aDataSet) 
{
  if (aDataSet->IsEmpty()) return;

  const TDF_AttributeMap& refAtts = aDataSet->Attributes();

  // Removes the attributes.
  TDF_MapIteratorOfAttributeMap refAttMItr(refAtts);
  for (; refAttMItr.More(); refAttMItr.Next()) {
    const Handle(TDF_Attribute)& locAtt = refAttMItr.Key();
    locAtt->Label().ForgetAttribute(locAtt);
  }
}


//=======================================================================
//function : IsSelfContained
//purpose  : Returns true if all the labels of <aDataSet> are
//          descendant of <aLabel>.
//=======================================================================

Standard_Boolean TDF_ComparisonTool::IsSelfContained
(const TDF_Label& aLabel,
 const Handle(TDF_DataSet)& aDataSet)
{
  if (!aDataSet->IsEmpty()) {
    const TDF_LabelMap& refLabs = aDataSet->Labels();
    for (TDF_MapIteratorOfLabelMap refLabMItr(refLabs);
	 refLabMItr.More();
	 refLabMItr.Next()) {
      if (!refLabMItr.Key().IsDescendant(aLabel)) return Standard_False;
    }
  }
  return Standard_True;
}
