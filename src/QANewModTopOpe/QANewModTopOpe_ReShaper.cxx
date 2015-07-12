// Created on: 2002-02-07
// Created by: Igor FEOKTISTOV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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


#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <QANewModTopOpe_ReShaper.hxx>
#include <Standard_Type.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_HSequenceOfShape.hxx>

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
