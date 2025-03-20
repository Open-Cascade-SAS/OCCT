// Copyright (c) 2021 OPEN CASCADE SAS
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

#include <PCDM_ReaderFilter.hxx>

IMPLEMENT_STANDARD_RTTIEXT(PCDM_ReaderFilter, Standard_Transient)

PCDM_ReaderFilter::PCDM_ReaderFilter(const Handle(Standard_Type)& theSkipped)
    : myAppend(AppendMode_Forbid)
{
  mySkip.Add(theSkipped->Name());
}

PCDM_ReaderFilter::PCDM_ReaderFilter(const TCollection_AsciiString& theEntryToRead)
    : myAppend(AppendMode_Forbid)
{
  mySubTrees.Append(theEntryToRead);
}

PCDM_ReaderFilter::PCDM_ReaderFilter(const AppendMode theAppend)
    : myAppend(theAppend)
{
}

void PCDM_ReaderFilter::Clear()
{
  mySkip.Clear();
  myRead.Clear();
  mySubTrees.Clear();
}

PCDM_ReaderFilter::~PCDM_ReaderFilter()
{
  ClearTree();
}

Standard_Boolean PCDM_ReaderFilter::IsPassed(const Handle(Standard_Type)& theAttributeID) const
{
  return IsPassedAttr(theAttributeID->Name());
}

Standard_Boolean PCDM_ReaderFilter::IsPassedAttr(
  const TCollection_AsciiString& theAttributeType) const
{
  return myRead.IsEmpty() ? !mySkip.Contains(theAttributeType) : myRead.Contains(theAttributeType);
}

Standard_Boolean PCDM_ReaderFilter::IsPassed(const TCollection_AsciiString& theEntry) const
{
  if (mySubTrees.IsEmpty())
    return true;
  for (NCollection_List<TCollection_AsciiString>::Iterator anEntry(mySubTrees); anEntry.More();
       anEntry.Next())
  {
    if (theEntry.StartsWith(anEntry.Value()))
    {
      if (theEntry.Length() > anEntry.Value().Length()
          && theEntry.Value(anEntry.Value().Length() + 1)
               != ':') // case when theEntry="0:10" should not match "0:1"
        continue;
      return true;
    }
  }
  return false;
}

Standard_Boolean PCDM_ReaderFilter::IsSubPassed(const TCollection_AsciiString& theEntry) const
{
  if (mySubTrees.IsEmpty() || theEntry.Length() == 2) // root is always passed if any sub is defined
    return true;
  for (NCollection_List<TCollection_AsciiString>::Iterator anEntry(mySubTrees); anEntry.More();
       anEntry.Next())
  {
    if (theEntry.Length() < anEntry.Value().Length()
        && anEntry.Value().Value(theEntry.Length() + 1) == ':'
        && // case when theEntry="0:1" should not match "0:10"
        anEntry.Value().StartsWith(theEntry))
      return true;
  }
  return false;
}

Standard_Boolean PCDM_ReaderFilter::IsPartTree()
{
  return !(mySubTrees.IsEmpty() || (mySubTrees.Size() == 1 && mySubTrees.First().Length() < 3));
}

void PCDM_ReaderFilter::StartIteration()
{
  myCurrent      = &myTree;
  myCurrentDepth = 0;
  ClearTree();
  myTree.Bind(-1, NULL);
  if (mySubTrees.IsEmpty())
    return;
  // create an iteration-tree by the mySubTrees entries
  for (NCollection_List<TCollection_AsciiString>::Iterator aTreeIter(mySubTrees); aTreeIter.More();
       aTreeIter.Next())
  {
    TagTree*                aMap = &myTree;
    TCollection_AsciiString aTagStr, anEntry = aTreeIter.Value();
    for (Standard_Integer aTagIndex = 2; !anEntry.IsEmpty(); ++aTagIndex) // skip the root tag
    {
      aTagStr = anEntry.Token(":", aTagIndex);
      if (aTagStr.IsEmpty())
        break;
      Standard_Integer aTag = aTagStr.IntegerValue();
      if (aMap->IsBound(aTag))
      {
        aMap = (TagTree*)aMap->Find(aTag);
      }
      else
      {
        TagTree* aNewMap = new TagTree;
        aNewMap->Bind(-1, aMap); // to be able to iterate up, keep father map in the child
        aMap->Bind(aTag, aNewMap);
        aMap = aNewMap;
      }
    }
    aMap->Bind(-2, NULL); // identifier that this node is in subtrees definition
  }
}

void PCDM_ReaderFilter::Up()
{
  if (myCurrentDepth == 0)
    myCurrent = (TagTree*)myCurrent->Find(-1);
  else
    myCurrentDepth--;
}

void PCDM_ReaderFilter::Down(const int& theTag)
{
  if (myCurrentDepth == 0)
  {
    if (myCurrent->IsBound(theTag))
      myCurrent = (TagTree*)myCurrent->Find(theTag);
    else
      ++myCurrentDepth;
  }
  else
    ++myCurrentDepth;
}

Standard_Boolean PCDM_ReaderFilter::IsPassed() const
{
  return myCurrent->IsBound(-2);
}

Standard_Boolean PCDM_ReaderFilter::IsSubPassed() const
{
  return myCurrentDepth == 0;
}

void PCDM_ReaderFilter::ClearSubTree(const Standard_Address theMap)
{
  if (theMap)
  {
    TagTree* aMap = (TagTree*)theMap;
    for (TagTree::Iterator aTagIter(*aMap); aTagIter.More(); aTagIter.Next())
      if (aTagIter.Key() != -1)
        ClearSubTree(aTagIter.Value());
    delete aMap;
  }
}

void PCDM_ReaderFilter::ClearTree()
{
  for (TagTree::Iterator aTagIter(myTree); aTagIter.More(); aTagIter.Next())
    if (aTagIter.Key() != -1)
      ClearSubTree(aTagIter.Value());
  myTree.Clear();
}
