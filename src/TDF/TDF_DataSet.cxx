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

//      	---------------
// Version:	0.0
//Version	Date		Purpose
//		0.0	Mar 11 1997	Creation

#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_DataSet.hxx>
#include <TDF_Label.hxx>
#include <TDF_ListIteratorOfLabelList.hxx>
#include <TDF_MapIteratorOfAttributeMap.hxx>
#include <TDF_MapIteratorOfLabelMap.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDF_DataSet,Standard_Transient)

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
