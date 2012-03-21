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

#include <BOP_HistoryCollector.ixx>

#include <TopExp_Explorer.hxx>

BOP_HistoryCollector::BOP_HistoryCollector()
{
  myOp = BOP_UNKNOWN;
  myHasDeleted = Standard_False;
}

BOP_HistoryCollector::BOP_HistoryCollector(const TopoDS_Shape& theShape1,
					   const TopoDS_Shape& theShape2,
					   const BOP_Operation theOperation)
{
  myOp = theOperation;
  myS1 = theShape1;
  myS2 = theShape2;
  myHasDeleted = Standard_False;
}

void BOP_HistoryCollector::SetResult(const TopoDS_Shape&       theResult,
				     const BOPTools_PDSFiller& theDSFiller) 
{
  myResult = theResult;
}

const TopTools_ListOfShape& BOP_HistoryCollector::Generated(const TopoDS_Shape& S) 
{
  if(S.IsNull())
    return myEmptyList;

  if(myGenMap.IsBound(S)) {
    return myGenMap.Find(S);
  }
  return myEmptyList;
}

const TopTools_ListOfShape& BOP_HistoryCollector::Modified(const TopoDS_Shape& S) 
{
  if(S.IsNull())
    return myEmptyList;

  if(myModifMap.IsBound(S)) {
    return myModifMap.Find(S);
  }
  return myEmptyList;
}

Standard_Boolean BOP_HistoryCollector::IsDeleted(const TopoDS_Shape& S) 
{
  if(S.IsNull())
    return Standard_True;

  TopAbs_ShapeEnum aType = S.ShapeType();
  TopExp_Explorer anExp(myResult, aType);

  for(; anExp.More(); anExp.Next()) {
    if(S.IsSame(anExp.Current()))
      return Standard_False;
  }

  if(myModifMap.IsBound(S)) {
    if(!myModifMap(S).IsEmpty())
      return Standard_False;
  }

  if(myGenMap.IsBound(S)) {
    if(!myGenMap(S).IsEmpty())
      return Standard_False;
  }
  return Standard_True;
}

Standard_Boolean BOP_HistoryCollector::HasGenerated() const
{
  if(!myGenMap.IsEmpty()) {
    return Standard_True;
  }
  return Standard_False;
}

Standard_Boolean BOP_HistoryCollector::HasModified() const
{
  if(!myModifMap.IsEmpty()) {
    return Standard_True;
  }
  return Standard_False;
}

Standard_Boolean BOP_HistoryCollector::HasDeleted() const
{
  return myHasDeleted;
}
