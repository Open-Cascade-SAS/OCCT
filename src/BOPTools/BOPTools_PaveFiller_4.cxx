// Created on: 2002-02-22
// Created by: Peter KURNEV
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


#include <BOPTools_PaveFiller.ixx>
#include <TColStd_IndexedMapOfInteger.hxx>

#include <TopoDS_Shape.hxx>

#include <BooleanOperations_ShapesDataStructure.hxx>

#include <BooleanOperations_OnceExplorer.hxx>
#include <BooleanOperations_KindOfInterference.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <BOPTools_ListIteratorOfListOfInterference.hxx>
#include <BOPTools_CArray1OfInterferenceLine.hxx>
#include <BOPTools_InterferenceLine.hxx>
#include <BOPTools_ListOfInterference.hxx>
#include <BOPTools_Interference.hxx>
#include <BOPTools_PShapeShapeInterference.hxx>



static
  void GetFullFaceMap(const Standard_Integer nF2,
		      const BOPTools_InterferencePool& myIntrPool,
		      TColStd_IndexedMapOfInteger& aMF2);

static
  void GetInterferedVertices(const Standard_Integer nF1,
			     const TColStd_IndexedMapOfInteger& aMF2,
			     const BOPTools_InterferencePool& myIntrPool,
			     TColStd_IndexedMapOfInteger& aMV);

//=======================================================================
// function: StickVertices
// purpose: 
//=======================================================================
  void BOPTools_PaveFiller::StickVertices(const Standard_Integer nF1,
					  const Standard_Integer nF2,
					  TColStd_IndexedMapOfInteger& aMV)
{
  Standard_Integer i, aNb, iEntity;
  TColStd_IndexedMapOfInteger aMF1, aMF2;
  //
  GetFullFaceMap(nF1, *myIntrPool, aMF1);
  GetFullFaceMap(nF2, *myIntrPool, aMF2);
  //
  aNb=aMF1.Extent();
  for (i=1; i<=aNb; ++i) {
    iEntity=aMF1(i);
    GetInterferedVertices(iEntity, aMF2, *myIntrPool, aMV);
  }
  //
  aNb=aMF2.Extent();
  for (i=1; i<=aNb; ++i) {
    iEntity=aMF2(i);
    GetInterferedVertices(iEntity, aMF1, *myIntrPool, aMV);
  }
}

//=======================================================================
// function: GetFullFaceMap
// purpose: 
//=======================================================================
void GetFullFaceMap(const Standard_Integer nF2,
		    const BOPTools_InterferencePool& anIntrPool,
		    TColStd_IndexedMapOfInteger& aMF2)
{
  Standard_Integer nEF2, nVF2;
  //
  aMF2.Add(nF2);
  BooleanOperations_PShapesDataStructure myDS=anIntrPool.DS();
  BooleanOperations_OnceExplorer aExp(*myDS);
  aExp.Init(nF2, TopAbs_EDGE);
  for (; aExp.More(); aExp.Next()) {
    nEF2=aExp.Current();
    aMF2.Add(nEF2);
  }
  //
  aExp.Init(nF2, TopAbs_VERTEX);
  for (; aExp.More(); aExp.Next()) {
    nVF2=aExp.Current();
    aMF2.Add(nVF2);
  }
}
//=======================================================================
// function: GetInterferedVertices
// purpose: 
//=======================================================================
void GetInterferedVertices(const Standard_Integer nF1,
			   const TColStd_IndexedMapOfInteger& aMF2,
			   const BOPTools_InterferencePool& anIntrPool,
			   TColStd_IndexedMapOfInteger& aMV)
{
  Standard_Integer aIndex, aNewShape, aWith;
  BooleanOperations_KindOfInterference aType;
  TopAbs_ShapeEnum aShapeType;
  BOPTools_ListIteratorOfListOfInterference anIt;
  BOPTools_PShapeShapeInterference pI;
  //
  BooleanOperations_PShapesDataStructure myDS=anIntrPool.DS();
  const BOPTools_CArray1OfInterferenceLine& aITable= anIntrPool.InterferenceTable();
  const BOPTools_InterferenceLine& aILineF1=aITable(nF1);
  const BOPTools_ListOfInterference& aLIF1=aILineF1.RealList();
  
  //
  anIt.Initialize(aLIF1);
  for (; anIt.More(); anIt.Next()) {
    const BOPTools_Interference& anInterference=anIt.Value();
    aWith=anInterference.With();
    if (aMF2.Contains(aWith)) {
      aIndex=anInterference.Index();
      aType= anInterference.Type();
      pI=anIntrPool.GetInterference(aIndex, aType);
      if (pI!=NULL) {
	//aNewShape=anIntrPool.NewShape(aIndex, aType);
	aNewShape=pI->NewShape();
	if (aNewShape) {
	  const TopoDS_Shape& aS=myDS->Shape(aNewShape);
	  aShapeType=aS.ShapeType();
	  if (aShapeType==TopAbs_VERTEX) {
	    aMV.Add(aNewShape);
	  }
	}
      }
    }
  }
}





