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
//Version	Date		Purpose
//		0.0	Mar 13 1997	Creation

#include <TCollection_AsciiString.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TDF_AttributeIndexedMap.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Data.hxx>
#include <TDF_DataSet.hxx>
#include <TDF_IDFilter.hxx>
#include <TDF_Label.hxx>
#include <TDF_ListIteratorOfLabelList.hxx>
#include <TDF_MapIteratorOfAttributeMap.hxx>
#include <TDF_MapIteratorOfLabelMap.hxx>
#include <TDF_Tool.hxx>

#define TDF_TagSeparator ':'

static void TDF_Tool_ExtendedDeepDump(Standard_OStream& anOS, 
				      const TDF_Label& aLabel, 
				      const TDF_IDFilter& aFilter,
				      TDF_AttributeIndexedMap& aMap);

static Standard_Boolean TDF_Tool_DescendantRef(const TDF_Label& aRefLabel, 
					       const TDF_Label& aLabel,
					       const TDF_IDFilter& aFilter, 
					       const Handle(TDF_DataSet)& ds);

static void TDF_Tool_OutReferers(const TDF_Label& aRefLabel, 
				 const TDF_Label& aLabel,
				 TDF_AttributeMap& atts,  
				 const TDF_IDFilter& aFilterForReferers,
				 const TDF_IDFilter& aFilterForReferences,
				 const Handle(TDF_DataSet)& ds);

static void TDF_Tool_OutReferences(const TDF_Label& aRefLabel,
				   const TDF_Label& aLabel,
				   TDF_AttributeMap& atts,  
				   const TDF_IDFilter& aFilterForReferers,
				   const TDF_IDFilter& aFilterForReferences,
				   const Handle(TDF_DataSet)& ds);

//=======================================================================
//function : NbLabels
//purpose  : Returns the numbers of labels of the tree.
//=======================================================================

Standard_Integer TDF_Tool::NbLabels(const TDF_Label& aLabel)
{
  Standard_Integer n = 1;
  for (TDF_ChildIterator itr(aLabel,Standard_True); itr.More(); itr.Next())
    ++n;
  return n;
}


//=======================================================================
//function : NbAttributes
//purpose  : Returns the number of attributes of the tree.
//=======================================================================

Standard_Integer TDF_Tool::NbAttributes(const TDF_Label& aLabel)
{
  Standard_Integer n = aLabel.NbAttributes();
  for (TDF_ChildIterator itr(aLabel,Standard_True); itr.More(); itr.Next())
    n += itr.Value().NbAttributes();
  return n;
}


//=======================================================================
//function : NbAttributes
//purpose  : Returns the number of attributes of the tree,
//           selected by an IDFilter.
//=======================================================================

Standard_Integer TDF_Tool::NbAttributes
(const TDF_Label& aLabel,
 const TDF_IDFilter& aFilter)
{
  Standard_Integer n = 0;
  TDF_AttributeIterator it2;
  for (it2.Initialize(aLabel,Standard_True);it2.More();it2.Next())
    if (aFilter.IsKept(it2.Value())) ++n;
  for (TDF_ChildIterator it1(aLabel,Standard_True); it1.More(); it1.Next())
    for (it2.Initialize(it1.Value(),Standard_True);it2.More();it2.Next())
      if (aFilter.IsKept(it2.Value())) ++n;
  return n;
}


//=======================================================================
//function : IsSelfContained
//purpose  : 
//=======================================================================

Standard_Boolean TDF_Tool::IsSelfContained(const TDF_Label& aLabel)
{
  TDF_IDFilter filter(Standard_False); // Keep all.
  return IsSelfContained(aLabel,filter);
}

//=======================================================================
//function : IsSelfContained
//purpose  : 
//=======================================================================

Standard_Boolean TDF_Tool::IsSelfContained
(const TDF_Label& aLabel,
 const TDF_IDFilter& aFilter)
{
  Handle(TDF_DataSet) ds = new TDF_DataSet();

  if (!TDF_Tool_DescendantRef(aLabel,aLabel,aFilter,ds))
    return Standard_False;

  for (TDF_ChildIterator itr(aLabel,Standard_True);
       itr.More();
       itr.Next()) {
    if (!TDF_Tool_DescendantRef(aLabel,itr.Value(),aFilter,ds))
      return Standard_False;
  }
  return Standard_True;
}


//=======================================================================
//function : TDF_Tool_DescendantRef
//purpose  : 
//=======================================================================

static Standard_Boolean TDF_Tool_DescendantRef
(const TDF_Label& aRefLabel,
 const TDF_Label& aLabel,
 const TDF_IDFilter& aFilter,
 const Handle(TDF_DataSet)& ds)
{
  for (TDF_AttributeIterator itr(aLabel); itr.More(); itr.Next()) {
    // CLE
    // const Handle(TDF_Attribute)& labAtt = itr.Value();
    Handle(TDF_Attribute) labAtt = itr.Value();
    // ENDCLE
    if (aFilter.IsKept(labAtt)) {
      labAtt->References(ds);
      // First of all, the referenced labels.
      const TDF_LabelMap& labMap = ds->Labels();

      for (TDF_MapIteratorOfLabelMap labMItr(labMap);
	   labMItr.More(); labMItr.Next()) {
	if (!labMItr.Key().IsDescendant(aRefLabel))
	  return Standard_False;
      }
      // Then the referenced attributes.
      const TDF_AttributeMap& attMap = ds->Attributes();
      for (TDF_MapIteratorOfAttributeMap attMItr(attMap);
	   attMItr.More(); attMItr.Next()) {
	// CLE
	// const Handle(TDF_Attribute)& att = attMItr.Key();
	Handle(TDF_Attribute) att = attMItr.Key();
        if (!att.IsNull() && !att->Label().IsNull())
        {
	  // ENDCLE
	  if (aFilter.IsKept(att) && !att->Label().IsDescendant(aRefLabel))
	    return Standard_False;
        }
      }
      ds->Clear();
    }
  }
  return Standard_True;
}


//=======================================================================
//function : OutReferers
//purpose  : 
//=======================================================================

void TDF_Tool::OutReferers(const TDF_Label& aLabel,
			   TDF_AttributeMap& atts)
{
  TDF_IDFilter filter(Standard_False); // Keep all.
  OutReferers(aLabel,filter,filter,atts);
}


//=======================================================================
//function : OutReferers
//purpose  : 
//=======================================================================

void TDF_Tool::OutReferers(const TDF_Label& aLabel,
			   const TDF_IDFilter& aFilterForReferers,
			   const TDF_IDFilter& aFilterForReferences,
			   TDF_AttributeMap& atts)
{
  Handle(TDF_DataSet) ds = new TDF_DataSet();
  TDF_Tool_OutReferers(aLabel,aLabel,atts,aFilterForReferers,aFilterForReferences,ds);
  for (TDF_ChildIterator itr(aLabel,Standard_True);itr.More();itr.Next()) {
    TDF_Tool_OutReferers(aLabel,itr.Value(),atts,aFilterForReferers,aFilterForReferences,ds);
  }
}


//=======================================================================
//function : TDF_Tool_OutReferers
//purpose  : 
//=======================================================================

static void TDF_Tool_OutReferers(const TDF_Label& aRefLabel,
				 const TDF_Label& aLabel,
				 TDF_AttributeMap& atts,
				 const TDF_IDFilter& aFilterForReferers,
				 const TDF_IDFilter& aFilterForReferences,
				 const Handle(TDF_DataSet)& ds)
{
  Standard_Boolean outRefFound = Standard_False;

  for (TDF_AttributeIterator itr(aLabel); itr.More(); itr.Next()) {

    if (!aFilterForReferers.IsKept(itr.Value())) continue;
    itr.Value()->References(ds);

    const TDF_AttributeMap& attMap = ds->Attributes();
    for (TDF_MapIteratorOfAttributeMap attMItr(attMap);
	 attMItr.More(); attMItr.Next()) {
      // CLE
      // const Handle(TDF_Attribute)& att = attMItr.Key();
      Handle(TDF_Attribute) att = attMItr.Key();
      // ENDCLE
      if (aFilterForReferences.IsKept(att) &&
          !att->Label().IsNull() &&
          !att->Label().IsDescendant(aRefLabel))
      {
        atts.Add(itr.Value());
        outRefFound = Standard_True;
        break;
      }
    }

    if (!outRefFound) {
      const TDF_LabelMap& labMap = ds->Labels();
      for (TDF_MapIteratorOfLabelMap labMItr(labMap);
	   labMItr.More(); labMItr.Next()) {
	if (!labMItr.Key().IsDescendant(aRefLabel)) {
	  atts.Add(itr.Value());
	  break;
	}
      }
    }

    outRefFound = Standard_False;
    ds->Clear();
  }
}

//=======================================================================
//function : OutReferences
//purpose  : 
//=======================================================================

void TDF_Tool::OutReferences(const TDF_Label& aLabel,
			     TDF_AttributeMap& atts)
{
  TDF_IDFilter filter(Standard_False); // Keep all.
  OutReferences(aLabel,filter,filter,atts);
}

//=======================================================================
//function : OutReferences
//purpose  : 
//=======================================================================

void TDF_Tool::OutReferences(const TDF_Label& aLabel,
			     const TDF_IDFilter& aFilterForReferers,
			     const TDF_IDFilter& aFilterForReferences,
			     TDF_AttributeMap& atts)
{
  Handle(TDF_DataSet) ds = new TDF_DataSet();
  TDF_Tool_OutReferences(aLabel,aLabel,atts,aFilterForReferers,aFilterForReferences,ds);
  for (TDF_ChildIterator itr(aLabel,Standard_True);itr.More();itr.Next()) {
    TDF_Tool_OutReferences(aLabel,itr.Value(),atts,aFilterForReferers,aFilterForReferences,ds);
  }
}

//=======================================================================
//function : TDF_Tool_OutReferences
//purpose  : 
//=======================================================================

static void TDF_Tool_OutReferences(const TDF_Label& aRefLabel,
				   const TDF_Label& aLabel,
				   TDF_AttributeMap& atts,
				   const TDF_IDFilter& aFilterForReferers,
				   const TDF_IDFilter& aFilterForReferences,
				   const Handle(TDF_DataSet)& ds)
{
  for (TDF_AttributeIterator itr(aLabel); itr.More(); itr.Next()) {
    if (!aFilterForReferers.IsKept(itr.Value())) continue;
    itr.Value()->References(ds);
    const TDF_AttributeMap& attMap = ds->Attributes();
    for (TDF_MapIteratorOfAttributeMap attMItr(attMap);attMItr.More();attMItr.Next()) {
      Handle(TDF_Attribute) att = attMItr.Key();
      if (aFilterForReferences.IsKept(att) &&
          !att->Label().IsNull() &&
          !att->Label().IsDescendant(aRefLabel))
      {
        atts.Add(att);
      }
    }
    const TDF_LabelMap& labMap = ds->Labels();
    for (TDF_MapIteratorOfLabelMap labMItr(labMap);labMItr.More();labMItr.Next()) {
      if (!labMItr.Key().IsDescendant(aRefLabel)) {
	TDF_AttributeIterator itra(labMItr.Key());
	for (; itra.More(); itra.Next()) {
	  if (aFilterForReferences.IsKept(itra.Value())) {
	    atts.Add(itra.Value());
	  }
	}
      }
    }
  }
  ds->Clear();
}

//=======================================================================
//function : RelocateLabel
//purpose  : 
//=======================================================================

void TDF_Tool::RelocateLabel
(const TDF_Label& aSourceLabel,
 const TDF_Label& fromRoot,
 const TDF_Label& toRoot,
 TDF_Label& aTargetLabel,
 const Standard_Boolean create)
{
  if (!aSourceLabel.IsDescendant(fromRoot)) return;
  aTargetLabel.Nullify();
  TColStd_ListOfInteger labelTags;
  TDF_Tool::TagList(aSourceLabel,labelTags);
  TColStd_ListOfInteger toTags;
  TDF_Tool::TagList(toRoot,toTags);
  for (Standard_Integer i = fromRoot.Depth(); i >= 0; --i)
    labelTags.RemoveFirst();
  labelTags.Prepend(toTags);
  TDF_Tool::Label(toRoot.Data(),labelTags,aTargetLabel,create);
}


//=======================================================================
//function : Entry
//purpose  : Returns the entry as an ascii string.
//=======================================================================

void TDF_Tool::Entry
(const TDF_Label& aLabel,
 TCollection_AsciiString& anEntry)
{
  anEntry.Clear();
  if (!aLabel.IsNull()) {
    TColStd_ListOfInteger Tags;
    TDF_Tool::TagList(aLabel, Tags);
    anEntry += TCollection_AsciiString(Tags.First());
    Tags.RemoveFirst();
    if (Tags.IsEmpty()) {
      anEntry += TDF_TagSeparator; // It must be the root label case.
    }
    else {
      while (!Tags.IsEmpty()) {
	anEntry += TDF_TagSeparator;
	anEntry += TCollection_AsciiString(Tags.First());
	Tags.RemoveFirst();
      }
    }
  }
}


//=======================================================================
//function : TagList
//purpose  : Returns the entry of a label as a list of integers.
//=======================================================================

void TDF_Tool::TagList
(const TDF_Label& aLabel,
 TColStd_ListOfInteger& aTagList)
{
  aTagList.Clear();
  if (!aLabel.IsNull()) {
    TDF_Label Label = aLabel;
    for (;;) {
      aTagList.Prepend(Label.Tag());
      if (Label.IsRoot()) break;
      Label = Label.Father();
    }
  }
}


//=======================================================================
//function : TagList
//purpose  : Returns the entry expressed as a string as a list of integers.
//=======================================================================

void TDF_Tool::TagList
(const TCollection_AsciiString& anEntry,
 TColStd_ListOfInteger& aTagList)
{
  char* cc = (char *)anEntry.ToCString();
  Standard_Integer n = 0;
  aTagList.Clear();
  while (*cc != '\0') {
    while ( *cc >= '0' && *cc <= '9') {
      n = 10*n + (*cc - '0');
      ++cc;
    }
    if (*cc == TDF_TagSeparator || *cc == '\0') {
      aTagList.Append(n);
      n = 0;
      if (*cc != '\0') ++cc;
    }
    else { // Not an entry!
      aTagList.Clear();
      break;
    }
  }
}


//=======================================================================
//function : Label
//purpose  : Returns the label expressed by <anEntry>.
//=======================================================================

void TDF_Tool::Label
(const Handle(TDF_Data)& aDF,
 const TCollection_AsciiString& anEntry,
 TDF_Label& aLabel,
 const Standard_Boolean create) 
{ TDF_Tool::Label(aDF,anEntry.ToCString(),aLabel,create); }


//=======================================================================
//function : Label
//purpose  : Returns the label expressed by <anEntry>,
//           and creates it if <create> is true.
//=======================================================================

void TDF_Tool::Label
(const Handle(TDF_Data)& aDF,
 const Standard_CString  anEntry,
 TDF_Label&              aLabel,
 const Standard_Boolean  create) 
{
  TColStd_ListOfInteger tagList;
  TDF_Tool::TagList(anEntry,tagList);
  TDF_Tool::Label(aDF,tagList,aLabel,create);
}


//=======================================================================
//function : Label
//purpose  : Returns the label expressed by <anEntry>,
//           and creates it if <create> is true.
//=======================================================================

void TDF_Tool::Label
(const Handle(TDF_Data)& aDF,
 const TColStd_ListOfInteger& aTagList,
 TDF_Label& aLabel,
 const Standard_Boolean create) 
{
  if (aTagList.Extent() == 0) {
    aLabel.Nullify();
  }
  else {
    aLabel = aDF->Root();
    if (aTagList.Extent() == 1 && aTagList.First() == 0) return;
    else {
      TColStd_ListIteratorOfListOfInteger tagItr (aTagList);
      tagItr.Next(); // Suppresses root tag.
      for (; !aLabel.IsNull() && tagItr.More(); tagItr.Next()) {
	aLabel = aLabel.FindChild(tagItr.Value(),create);
      }
    }
  }
}


//=======================================================================
//function : CountLabels
//purpose  : 
//=======================================================================

void TDF_Tool::CountLabels
(TDF_LabelList& aLabelList,
 TDF_LabelIntegerMap& aLabelMap)
{
  if (aLabelList.IsEmpty()) return;
  Standard_Boolean next = Standard_True;
  TDF_ListIteratorOfLabelList itr(aLabelList);
  while (itr.More()) {
    const TDF_Label& lab = itr.Value();
    if (aLabelMap.IsBound(lab)) {
      aLabelMap(lab) += 1;
      aLabelList.Remove(itr);
      next = Standard_False;
    }
    else {
      aLabelMap.Bind(lab,1);
      next = itr.More();
    }
    if (next && !aLabelList.IsEmpty()) itr.Next();
  }
}


//=======================================================================
//function : DeductLabels
//purpose  : 
//=======================================================================

void TDF_Tool::DeductLabels
(TDF_LabelList& aLabelList,
 TDF_LabelIntegerMap& aLabelMap)
{
  if (aLabelList.IsEmpty()) return;
  Standard_Boolean next = Standard_True;
  TDF_ListIteratorOfLabelList itr(aLabelList);
  while (itr.More()) {
    const TDF_Label& lab = itr.Value();
    if (aLabelMap.IsBound(lab)) {
      aLabelMap(lab) -= 1;
      if (aLabelMap(lab) == 0) {
	aLabelMap.UnBind(lab);
	aLabelList.Remove(itr);
	next = Standard_False;
      }
    }
    else next = itr.More();
    if (next && !aLabelList.IsEmpty()) itr.Next();
  }
}


//=======================================================================
//function : DeepDump
//purpose  : Deep dump of a DF.
//=======================================================================

void TDF_Tool::DeepDump
(Standard_OStream& anOS,
 const Handle(TDF_Data)& aDF)
{
  anOS<<aDF;
  TDF_Tool::DeepDump(anOS,aDF->Root());
}


//=======================================================================
//function : ExtendedDeepDump
//purpose  : Extended deep dump of a DF.
//=======================================================================

void TDF_Tool::ExtendedDeepDump
(Standard_OStream& anOS,
 const Handle(TDF_Data)& aDF,
 const TDF_IDFilter& aFilter)
{
  anOS<<aDF;
  TDF_Tool::ExtendedDeepDump(anOS,aDF->Root(),aFilter);
}


//=======================================================================
//function : DeepDump
//purpose  : Deep dump of a label.
//=======================================================================

void TDF_Tool::DeepDump
(Standard_OStream& anOS,
 const TDF_Label& aLabel)
{
  // Dumps the label.
  anOS<<aLabel;
  // Its children
  for (TDF_ChildIterator ChildIt(aLabel); ChildIt.More (); ChildIt.Next ()) {
    TDF_Tool::DeepDump(anOS,ChildIt.Value());
  }
}


//=======================================================================
//function : ExtendedDeepDump
//purpose  : Extended deep dump of a label.
//=======================================================================

void TDF_Tool::ExtendedDeepDump
(Standard_OStream& anOS,
 const TDF_Label& aLabel,
 const TDF_IDFilter& aFilter)
{
  TDF_AttributeIndexedMap map;
  TDF_Tool_ExtendedDeepDump(anOS,aLabel,aFilter,map);

  anOS<<map.Extent()<<" attribute"; if (map.Extent()>1) anOS<<"s";
  anOS<<" referenced by the label structure."<<endl;

  anOS<<endl<<"Extended dump of filtered attribute(s):"<<endl;
  Standard_Integer nba = 0;
  TCollection_AsciiString entry;
  Standard_Integer i ;
  for ( i = 1; i<= map.Extent(); ++i) {
    const Handle(TDF_Attribute)& att = map.FindKey(i);
    if (aFilter.IsKept(att)) {
      ++nba;
      anOS<<"# "<<i;
      if (att->Label().IsNull()) {
	anOS<<" (no label)"<<endl;
      }
      else {
	TDF_Tool::Entry(att->Label(),entry);
	anOS<<" (label: "<<entry<<")"<<endl;
      }
      att->ExtendedDump(anOS,aFilter,map); anOS<<endl;
    }
  }
  anOS<<endl<<nba<<" attribute";
  if (nba>1) anOS<<"s";
  anOS<<" dumped between "<<--i<<endl;
}


//=======================================================================
//function : ExtendedDeepDump
//purpose  : Internal method.
//=======================================================================

static void TDF_Tool_ExtendedDeepDump
(Standard_OStream& anOS,
 const TDF_Label& aLabel,
 const TDF_IDFilter& aFilter,
 TDF_AttributeIndexedMap& aMap)
{
  // Dumps the label.
  aLabel.ExtendedDump(anOS,aFilter,aMap);
  // Its children
  for (TDF_ChildIterator ChildIt(aLabel); ChildIt.More (); ChildIt.Next ()) {
    TDF_Tool_ExtendedDeepDump(anOS,ChildIt.Value(),aFilter,aMap);
  }
}
