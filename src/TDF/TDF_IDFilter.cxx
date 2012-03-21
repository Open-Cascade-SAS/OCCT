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

//      	---------------------

// Version:	0.0
//Version	Date		Purpose
//		0.0	May 26 1997	Creation



#include <TDF_IDFilter.ixx>

#include <TDF_IDList.hxx>
#include <TDF_ListIteratorOfIDList.hxx>
#include <TDF_MapIteratorOfIDMap.hxx>

// To avoid too much resizing actions, et 23 est un nombre premier.
#define TDF_IDFilterMapSize 23

//=======================================================================
//function : TDF_IDFilter
//purpose  : 
//=======================================================================

TDF_IDFilter::TDF_IDFilter(const Standard_Boolean ignoreMode) :
myIgnore(ignoreMode)
{}


//=======================================================================
//function : TDF_IDFilter
//purpose  : Private, to forbid implicit or hidden accesses to
//          the copy constructor.
//=======================================================================

TDF_IDFilter::TDF_IDFilter(const TDF_IDFilter& aFilter)
{}


//=======================================================================
//function : IgnoreAll
//purpose  : 
//=======================================================================

void TDF_IDFilter::IgnoreAll(const Standard_Boolean ignore)
{
  myIgnore = ignore;
  myIDMap.Clear(); myIDMap.ReSize(TDF_IDFilterMapSize);
}


//=======================================================================
//function : Keep
//purpose  : Keeps an ID.
//=======================================================================

void TDF_IDFilter::Keep(const Standard_GUID& anID) 
{
  if (myIgnore) myIDMap.Add(anID);
  else          myIDMap.Remove(anID);
}


//=======================================================================
//function : Keep
//purpose  : Keeps a list of ID.
//=======================================================================

void TDF_IDFilter::Keep(const TDF_IDList& anIDList) 
{
  if (!anIDList.IsEmpty()) {
    TDF_ListIteratorOfIDList itr(anIDList);
    if (myIgnore) {
      Standard_Integer n = anIDList.Extent() + myIDMap.NbBuckets() + 1;
      myIDMap.ReSize(n);
      for (; itr.More(); itr.Next()) myIDMap.Add(itr.Value());
    }
    else {
      for (; itr.More(); itr.Next()) myIDMap.Remove(itr.Value());
    }
  }
}


//=======================================================================
//function : Ignore
//purpose  : Ignores an ID.
//=======================================================================

void TDF_IDFilter::Ignore(const Standard_GUID& anID) 
{
  if (myIgnore) myIDMap.Remove(anID);
  else          myIDMap.Add(anID);
}


//=======================================================================
//function : Ignore
//purpose  : Ignores a list of ID.
//=======================================================================

void TDF_IDFilter::Ignore(const TDF_IDList& anIDList)
{
  if (!anIDList.IsEmpty()) {
    TDF_ListIteratorOfIDList itr(anIDList);
    if (myIgnore) {
      for (; itr.More(); itr.Next()) myIDMap.Remove(itr.Value());
    }
    else {
      Standard_Integer n = anIDList.Extent() + myIDMap.NbBuckets() + 1;
      myIDMap.ReSize(n);
      for (; itr.More(); itr.Next()) myIDMap.Add(itr.Value());
    }
  }
}


//=======================================================================
//function : IDList
//purpose  : 
//=======================================================================

void TDF_IDFilter::IDList(TDF_IDList& anIDList) const
{
  anIDList.Clear();
  for (TDF_MapIteratorOfIDMap itr(myIDMap); itr.More(); itr.Next())
    anIDList.Append(itr.Key());
}


//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

void TDF_IDFilter::Copy(const TDF_IDFilter& fromFilter)
{
  myIgnore = fromFilter.IgnoreAll();
  TDF_IDList idl;
  fromFilter.IDList(idl);
  if (myIgnore) Keep(idl);
  else          Ignore(idl);
}


//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void TDF_IDFilter::Dump(Standard_OStream& anOS) const
{
  if (myIgnore) anOS<<"EX"; else anOS<<"IN"; anOS<<"CLUSIVE filter: ";
  if (myIgnore) anOS<<"ignores"; else anOS<<"keeps  "; anOS<<" all IDs";
  TDF_MapIteratorOfIDMap itr(myIDMap);
  if (itr.More()) {
    anOS<<" BUT:"<<endl;
    for (; itr.More(); itr.Next()) {
      const Standard_GUID& guid = itr.Key();
      guid.ShallowDump(anOS);
      anOS<<endl;
    }
  }
}
