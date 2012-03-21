// Created by: DAUTRY Philippe
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

//      	---------------

// Version:	0.0
//Version	Date		Purpose
//		0.0	Mar 11 1997	Creation



#include <TDF_DataSet.ixx>

#include <TDF_MapIteratorOfAttributeMap.hxx>
#include <TDF_ListIteratorOfLabelList.hxx>
#include <TDF_MapIteratorOfLabelMap.hxx>


//=======================================================================
//function : TDF_DataSet
//purpose  : Creates a DataSet.
//=======================================================================

TDF_DataSet::TDF_DataSet()
{}


//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void TDF_DataSet::Clear()
{
  myRootLabels.Clear();
  myLabelMap.Clear();
  myAttributeMap.Clear();
}


//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDF_DataSet::Dump(Standard_OStream& anOS) const
{
  anOS<<"\t\t=====< TDF_DataSet dump >====="<<endl;
  anOS<<"Root Labels :"<<endl<<"============="<<endl;
  for (TDF_ListIteratorOfLabelList itr1(myRootLabels);
       itr1.More(); itr1.Next()) {
    itr1.Value().EntryDump(anOS);
    anOS<<" | ";
  }
  anOS<<endl<<"Labels :"<<endl<<"========"<<endl;
  for (TDF_MapIteratorOfLabelMap itr2(myLabelMap);
       itr2.More(); itr2.Next()) {
    itr2.Key().EntryDump(anOS);
    anOS<<" | ";
  }
  anOS<<endl<<"Attributes :"<<endl<<"============"<<endl<<endl;
  for (TDF_MapIteratorOfAttributeMap itr3(myAttributeMap);
       itr3.More(); itr3.Next()) {
    itr3.Key()->Label().EntryDump(anOS);
    anOS<<" \t";
    itr3.Key()->Dump(anOS);
    anOS<<endl;
  }
  anOS<<endl;
  return anOS;
}
