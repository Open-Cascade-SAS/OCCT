// Created on: 2002-02-07
// Created by: Igor FEOKTISTOV
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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

#include <QANewModTopOpe_ReShaper.ixx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopoDS_Compound.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS.hxx>

QANewModTopOpe_ReShaper::QANewModTopOpe_ReShaper(const TopoDS_Shape& TheInitialShape):
       myInitShape(TheInitialShape)
{

}
QANewModTopOpe_ReShaper::QANewModTopOpe_ReShaper(const TopoDS_Shape& TheInitialShape, 
				       const TopTools_MapOfShape& TheMap):
       myInitShape(TheInitialShape)
{
  myMap.Assign(TheMap);
}

QANewModTopOpe_ReShaper::QANewModTopOpe_ReShaper(const TopoDS_Shape& TheInitialShape,
				       const Handle(TopTools_HSequenceOfShape)& TheShapeToBeRemoved):
       myInitShape(TheInitialShape)
{

  Standard_Integer i, n = TheShapeToBeRemoved->Length();
  for(i = 1; i <= n; i++) {
    myMap.Add(TheShapeToBeRemoved->Value(i));
  }

}

void QANewModTopOpe_ReShaper::Remove(const TopoDS_Shape& TheS) 
{
  myMap.Add(TheS);
}

void QANewModTopOpe_ReShaper::Perform() 
{
  
  BRepBuilderAPI_Copy aCopier;

  if(myMap.IsEmpty()) {
    
    aCopier.Perform(myInitShape);
    myResult = aCopier.Shape();
    return;

  }

  BRep_Builder aBB;
  myResult.Nullify();
  aBB.MakeCompound(TopoDS::Compound(myResult));
  TopoDS_Iterator anIter(myInitShape);

  for(; anIter.More(); anIter.Next()) {

    const TopoDS_Shape& aS = anIter.Value();
    
    if(myMap.Contains(aS)) continue;

    if(aS.ShapeType() != TopAbs_COMPOUND) {
      aCopier.Perform(aS);
      aBB.Add(myResult, aCopier.Shape());
    }
    else {
      Handle(QANewModTopOpe_ReShaper) aR = new QANewModTopOpe_ReShaper(aS, myMap);
      aR->Perform();
      const TopoDS_Shape& aSp = aR->GetResult();
      if(aSp.ShapeType() == TopAbs_COMPOUND) {
	TopoDS_Iterator anIt(aSp);
	if(anIt.More()) {
	  aBB.Add(myResult, aSp);
	}
      }
      else {
	aBB.Add(myResult, aSp);
      }
    }

  }

  if(myResult.ShapeType() == TopAbs_COMPOUND) {
    anIter.Initialize(myResult);
    if(anIter.More()) {
      const TopoDS_Shape& aSp = anIter.Value();
      anIter.Next();
      if(!anIter.More()) myResult = aSp;
    }
  }
  
}

const TopoDS_Shape& QANewModTopOpe_ReShaper::GetResult() const
{
  return myResult;
}

void QANewModTopOpe_ReShaper::Clear() 
{
  myMap.Clear();
  myResult.Nullify();
}
