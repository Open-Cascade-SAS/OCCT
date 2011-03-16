#include <BOP_SolidSolidHistoryCollector.ixx>

#include <TopAbs.hxx>
#include <TopExp_Explorer.hxx>
#include <BooleanOperations_ShapesDataStructure.hxx>
#include <BooleanOperations_IndexedDataMapOfShapeInteger.hxx>
#include <BOPTools_SSInterference.hxx>
#include <BOPTools_InterferencePool.hxx>
#include <BOPTools_CArray1OfSSInterference.hxx>
#include <BOPTools_ListIteratorOfListOfPaveBlock.hxx>
#include <BOPTools_Curve.hxx>
#include <BOPTools_PaveBlock.hxx>
#include <BOPTools_DSFiller.hxx>
#include <BOPTools_PaveFiller.hxx>
#include <BOPTools_CommonBlock.hxx>
#include <BOPTools_ListOfCommonBlock.hxx>
#include <BOPTools_ListIteratorOfListOfCommonBlock.hxx>
#include <BOPTools_ListIteratorOfListOfPave.hxx>
#include <BOP_BuilderTools.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

// ======================================================================================================
// function: Constructor
// purpose:
// ======================================================================================================
BOP_SolidSolidHistoryCollector::BOP_SolidSolidHistoryCollector(const TopoDS_Shape&       theShape1,
							       const TopoDS_Shape&       theShape2,
							       const BOP_Operation       theOperation):
BOP_HistoryCollector(theShape1, theShape2, theOperation)
{
}

// ======================================================================================================
// function: AddNewShape
// purpose:
// warning: Treats only faces
// ======================================================================================================
void BOP_SolidSolidHistoryCollector::AddNewShape(const TopoDS_Shape&       theOldShape,
						 const TopoDS_Shape&       theNewShape,
						 const BOPTools_PDSFiller& theDSFiller) 
{
  if(theOldShape.ShapeType() != theNewShape.ShapeType())
    return;

  if(theOldShape.ShapeType() != TopAbs_FACE) {
    return;
  }
  Standard_Integer iRank = 0;
  const BooleanOperations_IndexedDataMapOfShapeInteger& aMap1 = 
    theDSFiller->DS().ShapeIndexMap(1);

  const BooleanOperations_IndexedDataMapOfShapeInteger& aMap2 = 
    theDSFiller->DS().ShapeIndexMap(2);

  if(aMap1.Contains(theOldShape))
    iRank = 1;
  else {
    if(aMap2.Contains(theOldShape))
      iRank = 2;
  }

  if(iRank == 0)
    return;

  TopTools_DataMapOfShapeListOfShape& aHistoryMap = myModifMap;

  if(aHistoryMap.IsBound(theOldShape)) {
    aHistoryMap.ChangeFind(theOldShape).Append(theNewShape);
  }
  else {
    TopTools_ListOfShape aShapeList;
    aShapeList.Append(theNewShape);
    aHistoryMap.Bind(theOldShape, aShapeList);
  }
}

// ======================================================================================================
// function: SetResult
// purpose:
// ======================================================================================================
void BOP_SolidSolidHistoryCollector::SetResult(const TopoDS_Shape&       theResult,
					       const BOPTools_PDSFiller& theDSFiller) 
{
  myResult = theResult;
  myHasDeleted = Standard_False;

  TopAbs_ShapeEnum aType = TopAbs_FACE;
  TopTools_IndexedMapOfShape aMap;
  TopExp::MapShapes(myResult, aType, aMap);

  TopExp_Explorer anExpObj(myS1, aType);

  for(; anExpObj.More(); anExpObj.Next()) {
    const TopoDS_Shape& aS = anExpObj.Current();

    if(!aMap.Contains(aS)) {

      if(!myModifMap.IsBound(aS) || myModifMap(aS).IsEmpty()) {

	if(!myGenMap.IsBound(aS) || myGenMap(aS).IsEmpty()) {
	  myHasDeleted = Standard_True;
	  break;
	}
      }
    }
  }

  if(!myHasDeleted) {
    TopExp_Explorer anExpTool(myS2, aType);

    for(; anExpTool.More(); anExpTool.Next()) {
      const TopoDS_Shape& aS = anExpTool.Current();

      if(!aMap.Contains(aS)) {
	if(!myModifMap.IsBound(aS) || myModifMap(aS).IsEmpty()) {

	  if(!myGenMap.IsBound(aS) || myGenMap(aS).IsEmpty()) {
	    myHasDeleted = Standard_True;
	    break;
	  }
	}
      }
    }
  }
}
