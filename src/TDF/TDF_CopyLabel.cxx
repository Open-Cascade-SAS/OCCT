// Created on: 1999-06-24
// Created by: Sergey ZARITCHNY
// Copyright (c) 1999-1999 Matra Datavision
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


#include <TDF_AttributeIterator.hxx>
#include <TDF_AttributeMap.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_ClosureMode.hxx>
#include <TDF_ClosureTool.hxx>
#include <TDF_CopyLabel.hxx>
#include <TDF_CopyTool.hxx>
#include <TDF_Data.hxx>
#include <TDF_DataSet.hxx>
#include <TDF_IDFilter.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelMap.hxx>
#include <TDF_MapIteratorOfAttributeMap.hxx>
#include <TDF_MapIteratorOfLabelMap.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDF_Tool.hxx>

// The bug concerns the COPY operation of some attributes of a non-self-contained label.
// The attributes making the label non-self-contained are not involved by the operation.
// Therefore, these attributes shouldn't be considered by the COPY mechanism and
// the label should be considered as a self-contained.
// Correction of the bug consists of ignoring the attributes not involved by the COPY operation.
//=======================================================================
//function : TDF_CopyLabel
//purpose  : 
//=======================================================================
TDF_CopyLabel::TDF_CopyLabel() 
     :myFilter(Standard_False), myIsDone(Standard_False)
{  
  mySL.Nullify();
  myTL.Nullify();
}

//=======================================================================
//function : TDF_CopyLabel
//purpose  : 
//=======================================================================

TDF_CopyLabel::TDF_CopyLabel(const TDF_Label& aSource,const TDF_Label& aTarget )
     :myFilter(Standard_False), myIsDone(Standard_False)
{
  mySL = aSource; myTL = aTarget;
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void TDF_CopyLabel::Load(const TDF_Label& aSource, const TDF_Label& aTarget)
{
  mySL = aSource; myTL = aTarget;
}

//=======================================================================
//function : ExternalReferences
//purpose  : internal
//=======================================================================
void TDF_CopyLabel::ExternalReferences(const TDF_Label& aRefLabel, const TDF_Label& aLabel,
				 TDF_AttributeMap& aExternals, const TDF_IDFilter& aFilter,
				       Handle(TDF_DataSet)& ds)
{
//  TCollection_AsciiString entr1,entr2; //d
  for (TDF_AttributeIterator itr(aLabel); itr.More(); itr.Next()) {
    itr.Value()->References(ds);
    const TDF_AttributeMap& attMap = ds->Attributes(); //attMap
//     TDF_Tool::Entry(itr.Value()->Label(), entr1);  //d
//     cout<<"\tSource Attribute dynamic type = "<<itr.Value()->DynamicType()<<" Label = "<<entr1 <<endl;
    for (TDF_MapIteratorOfAttributeMap attMItr(attMap);attMItr.More(); attMItr.Next()) {
      Handle(TDF_Attribute) att = attMItr.Key();

//       TDF_Tool::Entry(att->Label(), entr1);  
//       cout<<"\t\tReferences attribute dynamic type = "<<att->DynamicType()<<" Label = "<<entr1 <<endl;
      if (!att.IsNull() && !att->Label().IsNull())
      {
        if (aFilter.IsKept(att) && att->Label().IsDifferent(aRefLabel) &&
	    !att->Label().IsDescendant(aRefLabel)) {
          aExternals.Add(att);
        }
      }
    }
    
//     const TDF_LabelMap& labMap = ds->Labels();
//     for (TDF_MapIteratorOfLabelMap labMItr(labMap);labMItr.More(); labMItr.Next()) {
//       TDF_Tool::Entry(labMItr.Key(), entr1);  
// 	cout<<"\t\tLABELS from DS of Attr:: Lab = "<<entr1<<endl;
//       if (!labMItr.Key().IsDescendant(aRefLabel) && labMItr.Key().IsDifferent(aRefLabel)) {
// //	aExternals.Add(itr.Value()); // ??? LabelMap of Attribute has label which don't
// 	// belongs to source hierarchy. So, what we should do ?
// 	// Add this Attribute to the aExternals or add all attributes 
// 	// from this label ?
// 	TCollection_AsciiString entr1, entr2;
// 	TDF_Tool::Entry(labMItr.Key(), entr1); 
// 	TDF_Tool::Entry(aRefLabel, entr2);
// 	cout<<"\t\t\tNot descendant label:: Lab1 = "<<entr1<<" and RefLab = "<<entr2<<endl;
//       }
//     }

    ds->Clear();
  }
}
//=======================================================================
//function : ExternalReferences
//purpose  : 
//=======================================================================

Standard_Boolean TDF_CopyLabel::ExternalReferences(const TDF_Label& L,
						   TDF_AttributeMap& aExternals,
						   const TDF_IDFilter& aFilter)
{
  Handle(TDF_DataSet) ds = new TDF_DataSet();
  ExternalReferences(L, L, aExternals, aFilter, ds);
  for (TDF_ChildIterator itr(L, Standard_True);itr.More(); itr.Next()) {
    ExternalReferences(L,itr.Value(),aExternals , aFilter, ds);
  }
  if(!aExternals.Extent())
    return Standard_False;
  else 
    return Standard_True; 
}

//=======================================================================
#ifdef OCCT_DEBUG
static void PrintEntry(const TDF_Label&       label, const Standard_Boolean allLevels)
{
  TCollection_AsciiString entry;
  TDF_Tool::Entry(label, entry);
  cout << "\tShareable attribute on the label = "<< entry << endl;
  TDF_ChildIterator it (label, allLevels);
  for (; it.More(); it.Next())
    {
      TDF_Tool::Entry(it.Value(), entry);
	cout << "\tChildLabelEntry = "<< entry << endl;
    }  
}
#endif
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void TDF_CopyLabel::Perform()
{
  myIsDone = Standard_False;
  if(mySL.Data()->Root().IsDifferent(myTL.Data()->Root()) && //TDF_Data is not the same
     !TDF_Tool::IsSelfContained(mySL, myFilter)) return;               //source label isn't self-contained

  Standard_Boolean extReferers =
    ExternalReferences(mySL, myMapOfExt, myFilter);
  
  myRT = new TDF_RelocationTable(Standard_True);
  Handle(TDF_DataSet) ds = new TDF_DataSet();
  TDF_ClosureMode mode(Standard_True); // descendant plus reference
  ds->AddLabel(mySL);  
  myRT->SetRelocation(mySL, myTL);
  TDF_ClosureTool::Closure(ds, myFilter, mode);
  if(extReferers) {
    for (TDF_MapIteratorOfAttributeMap attMItr(myMapOfExt);attMItr.More(); attMItr.Next()) {
      Handle(TDF_Attribute) att = attMItr.Key();
      myRT->SetRelocation(att, att);
#ifdef OCCT_DEBUG
      PrintEntry(att->Label(), Standard_True);
#endif
    }
  }
  
  TDF_CopyTool::Copy(ds,myRT);   
  myIsDone = Standard_True;
}


//=======================================================================
//function : RelocationTable
//purpose  : 
//=======================================================================

const Handle(TDF_RelocationTable)& TDF_CopyLabel::RelocationTable() const
{
  return  myRT;
}

//=======================================================================
//function : UseFilter
//purpose  : 
//=======================================================================

void TDF_CopyLabel::UseFilter(const TDF_IDFilter& aFilter) 
{
  myFilter = aFilter;
}
