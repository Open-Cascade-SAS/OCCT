// Created by: DAUTRY Philippe
// Copyright (c) 1998-1999 Matra Datavision
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

//      	-------------------
// Version:	0.0
//Version	Date		Purpose
//		0.0	Sep  8 1998	Creation

#include <Standard_TypeMismatch.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_AttributeMap.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_ClosureMode.hxx>
#include <TDF_ClosureTool.hxx>
#include <TDF_DataSet.hxx>
#include <TDF_IDFilter.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelMap.hxx>
#include <TDF_ListIteratorOfLabelList.hxx>
#include <TDF_MapIteratorOfAttributeMap.hxx>
#include <TDF_MapIteratorOfLabelMap.hxx>

//=======================================================================
//function : Closure
//purpose  : Builds the transitive closure whithout attribute filter.
//=======================================================================

void TDF_ClosureTool::Closure
(const Handle(TDF_DataSet)& aDataSet) 
{
  TDF_IDFilter Filter(Standard_False); // "Keep all"
  TDF_ClosureMode Mode; // All modes are set to true.
  TDF_ClosureTool::Closure(aDataSet, Filter, Mode);
}


//=======================================================================
//function : Closure
//purpose  : Builds the transitive closure with an attribute filter.
//=======================================================================

void TDF_ClosureTool::Closure
(const Handle(TDF_DataSet)& aDataSet,
 const TDF_IDFilter& aFilter,
 const TDF_ClosureMode& aMode) 
{
  TDF_LabelMap&     labMap = aDataSet->Labels();
  TDF_AttributeMap& attMap = aDataSet->Attributes();
  TDF_LabelList&    rootLst  = aDataSet->Roots();

  // Memorizes the roots for future uses.
  rootLst.Clear();
  TDF_MapIteratorOfLabelMap labMItr(labMap);
  for (; labMItr.More(); labMItr.Next()) rootLst.Append(labMItr.Key());

  // Iterates on roots.
  TDF_ListIteratorOfLabelList labLItr(rootLst);
  for (; labLItr.More(); labLItr.Next()) {
    const TDF_Label& lab = labLItr.Value();
    if (lab.HasAttribute())
      TDF_ClosureTool::LabelAttributes(lab,labMap,attMap,aFilter,aMode);
    TDF_ClosureTool::Closure(lab,labMap,attMap,aFilter,aMode); 
  }
}


//=======================================================================
//function : Closure
//purpose  : Internal closure method.
//=======================================================================

void TDF_ClosureTool::Closure
(const TDF_Label& aLabel,
 TDF_LabelMap& aLabMap,
 TDF_AttributeMap& anAttMap,
 const TDF_IDFilter& aFilter,
 const TDF_ClosureMode& aMode) 
{
  TDF_Label upLab;
  for (TDF_ChildIterator childItr(aLabel,Standard_True);
       childItr.More();childItr.Next()){
    const TDF_Label& locLab = childItr.Value();
    // On ne peut faire cette optimisation car il faudrait d'abord
    // qu'aucun label donne comme Root ne soit fils d'un autre label root!
    if (locLab.HasAttribute()) { // && aLabMap.Add(locLab)) {
      aLabMap.Add(locLab);
      upLab = locLab.Father();
      while (aLabMap.Add(upLab)) upLab = upLab.Father();
      TDF_ClosureTool::LabelAttributes(locLab,aLabMap,anAttMap,aFilter,aMode);
    }
  }
}




//=======================================================================
//function : LabelAttributes
//purpose  : Internal method: adds the attributes to <aDataSet>.
//=======================================================================

void TDF_ClosureTool::LabelAttributes
(const TDF_Label& aLabel,
 TDF_LabelMap& aLabMap,
 TDF_AttributeMap& anAttMap,
 const TDF_IDFilter& aFilter,
 const TDF_ClosureMode& aMode) 
{
  Handle(TDF_DataSet) tmpDataSet;
  Standard_Boolean BindLabel;
  TDF_MapIteratorOfAttributeMap attMItr;
  TDF_MapIteratorOfLabelMap labMItr;

  // Attributes directly attached to the label.
  for (TDF_AttributeIterator attItr(aLabel); attItr.More(); attItr.Next()) {
    const Handle(TDF_Attribute) locAtt1 = attItr.Value();
    if (aFilter.IsKept(locAtt1)) {
      if (anAttMap.Add(locAtt1)) {
	// locAtt1 not yet in the map.

	// Labels & Attributes referenced by the attribute.
	tmpDataSet = new TDF_DataSet();
	if (aMode.References()) {
	  // 1 - The referenced attributes
	  // 1.1 - A referenced attribute has a label : adds the label;
	  // 1.2 - A referenced attribute has no label : adds the attribute;
	  // 2 - Adds the referenced labels.

	  locAtt1->References(tmpDataSet);

	  // 1 - The referenced attributes
	  const TDF_AttributeMap& tmpAttMap = tmpDataSet->Attributes();
	  for (attMItr.Initialize(tmpAttMap);
	       attMItr.More(); attMItr.Next()) {
	    const Handle(TDF_Attribute)& locAtt2 = attMItr.Key();
	    BindLabel = Standard_False;
	    if (!locAtt2.IsNull()) {
	      const TDF_Label& locLab2 = locAtt2->Label();
	      BindLabel = !locLab2.IsNull();
	      if (BindLabel) {
		// 1.1 - A referenced attribute has a label.
		if (aLabMap.Add(locLab2))
		  TDF_ClosureTool::Closure(locLab2,
					   aLabMap,anAttMap,aFilter,aMode);
	      }
	      else {
		// 1.2 - A referenced attribute has no label.
		// We suppose locAtt2 has no referenced attribute itself.
		anAttMap.Add(locAtt2);
	      }
	    }
	  }

	  // 2 - Adds the referenced labels.
	  const TDF_LabelMap& tmpLabMap = tmpDataSet->Labels();
	  for (labMItr.Initialize(tmpLabMap);
	       labMItr.More(); labMItr.Next()) {
	    const TDF_Label& locLab1 = labMItr.Key();
	    if (aLabMap.Add(locLab1))
	      TDF_ClosureTool::Closure(locLab1,
				       aLabMap,anAttMap,aFilter,aMode);
	  }
	}
      }
    }
  }
}
