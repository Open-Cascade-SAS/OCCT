// File:	BOPTools_InterferencePool.cxx
// Created:	Fri Jan 26 12:17:16 2001
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <BOPTools_InterferencePool.ixx>

#include <BRep_Tool.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

#include <IntTools_Tools.hxx>
#include <IntTools_EdgeEdge.hxx>
#include <IntTools_SequenceOfCommonPrts.hxx>
#include <IntTools_CommonPrt.hxx>

#include <BOPTools_VEInterference.hxx>
#include <BOPTools_VVInterference.hxx>
#include <BOPTools_VSInterference.hxx>
#include <BOPTools_EEInterference.hxx>
#include <BOPTools_SSInterference.hxx>
#include <BOPTools_InterferenceLine.hxx>
#include <IntTools_EdgeFace.hxx>
#include <BOPTools_ESInterference.hxx>


//=======================================================================
//function : BOPTools_InterferencePool
//purpose  : 
//=======================================================================
  BOPTools_InterferencePool::BOPTools_InterferencePool()
{
  myDS=NULL;
  myNbSourceShapes=0;
}
//=======================================================================
//function : BOPTools_InterferencePool
//purpose  : 
//=======================================================================
  BOPTools_InterferencePool::BOPTools_InterferencePool (const BooleanOperations_ShapesDataStructure& aDS) 
{
  SetDS(aDS);
}
//=======================================================================
//function : SetDS
//purpose  : 
//=======================================================================
  void BOPTools_InterferencePool::SetDS(const BooleanOperations_ShapesDataStructure& aDS) 
{
  void* p=(void*) &aDS;
  myDS=(BooleanOperations_ShapesDataStructure*) p;
  myNbSourceShapes= myDS->NumberOfShapesOfTheObject()+myDS->NumberOfShapesOfTheTool();
  myInterferenceTable.Resize (myNbSourceShapes);
}
//=======================================================================
//function : DS
//purpose  : 
//=======================================================================
  BooleanOperations_PShapesDataStructure BOPTools_InterferencePool::DS() const
{
  return myDS;
}
//=======================================================================
//function : HasInterference
//purpose  : 
//=======================================================================
  Standard_Boolean BOPTools_InterferencePool::HasInterference(const Standard_Integer anInd1)const
{
  const BOPTools_InterferenceLine& aWhatLine=myInterferenceTable(anInd1);
  Standard_Boolean bFlag=aWhatLine.HasInterference();
  return bFlag;
}
//=======================================================================
//function : IsComputed
//purpose  : 
//=======================================================================
  Standard_Boolean BOPTools_InterferencePool::IsComputed(const Standard_Integer anInd1,
							 const Standard_Integer anInd2) const
{
  BooleanOperations_KindOfInterference theType;

  theType=InterferenceType(anInd1, anInd2);

  if (theType==BooleanOperations_UnknownInterference) {
    return Standard_False;
  }

  const BOPTools_InterferenceLine& aWhatLine=myInterferenceTable(anInd1);
  Standard_Boolean aFlag=aWhatLine.IsComputed(anInd2, theType);
  return aFlag;
}


//===========================================================================
//function : AddInterference
//purpose  : 
//===========================================================================
  void BOPTools_InterferencePool::AddInterference (const Standard_Integer theWhat,
						   const Standard_Integer theWith,
						   const BooleanOperations_KindOfInterference theType,
						   const Standard_Integer theIndexOfInterference)
{
  BOPTools_InterferenceLine& aWhatLine=myInterferenceTable(theWhat);
  aWhatLine.AddInterference(theWith, theType, theIndexOfInterference);

  BOPTools_InterferenceLine& aWithLine=myInterferenceTable(theWith);
  aWithLine.AddInterference(theWhat, theType, theIndexOfInterference);
}

//=======================================================================
//function : InterferenceType
//purpose  : 
//=======================================================================
  BooleanOperations_KindOfInterference 
    BOPTools_InterferencePool::InterferenceType(const Standard_Integer theWhat,
						const Standard_Integer theWith) const
{
  BooleanOperations_KindOfInterference theType;
  Standard_Integer aWhat, aWith;
  TopAbs_ShapeEnum aType1, aType2;

  aWhat=theWhat;
  aWith=theWith;
  SortTypes(aWhat, aWith);

  aType1= myDS->GetShapeType(aWhat),
  aType2= myDS->GetShapeType(aWith);
  
  if (aType1==TopAbs_VERTEX && aType2==TopAbs_VERTEX) {
    theType=BooleanOperations_VertexVertex;
  }
  else if  (aType1==TopAbs_VERTEX && aType2==TopAbs_EDGE) {
    theType=BooleanOperations_VertexEdge;
  }
  else if  (aType1==TopAbs_VERTEX && aType2==TopAbs_FACE) {
    theType=BooleanOperations_VertexSurface;
  }
  else if  (aType1==TopAbs_EDGE && aType2==TopAbs_EDGE) {
    theType=BooleanOperations_EdgeEdge;
  }
  else if  (aType1==TopAbs_EDGE && aType2==TopAbs_FACE) {
    theType=BooleanOperations_EdgeSurface;
  }
  else if  (aType1==TopAbs_FACE && aType2==TopAbs_FACE) {
    theType=BooleanOperations_SurfaceSurface;
  }
  else {
    theType=BooleanOperations_UnknownInterference;
  }

  return theType;
}

//=======================================================================
//function : SortTypes
//purpose  : 
//=======================================================================
  void BOPTools_InterferencePool::SortTypes(Standard_Integer& theWhat,
					    Standard_Integer& theWith) const
{ 
  Standard_Boolean aReverseFlag=Standard_True;

  TopAbs_ShapeEnum aType1= myDS->GetShapeType(theWhat),
                   aType2= myDS->GetShapeType(theWith);
  
  if (aType1==aType2)
    return;
  
  if (aType1==TopAbs_EDGE && aType2==TopAbs_FACE){
    aReverseFlag=Standard_False;
  }

  if (aType1==TopAbs_VERTEX && 
      (aType2==TopAbs_FACE || aType2==TopAbs_EDGE)) {
    aReverseFlag=Standard_False;
  }
  
  Standard_Integer aWhat, aWith;
  aWhat=(aReverseFlag) ? theWith : theWhat;
  aWith=(aReverseFlag) ? theWhat : theWith;
  
  theWhat=aWhat;
  theWith=aWith;
}

//===========================================================================
//function : InterferenceTable
//purpose  : 
//===========================================================================
const BOPTools_CArray1OfInterferenceLine&
   BOPTools_InterferencePool::InterferenceTable()const
{
  return myInterferenceTable;
}

//===========================================================================
//function : SSInterferences
//purpose  : 
//===========================================================================
  BOPTools_CArray1OfSSInterference&  BOPTools_InterferencePool::SSInterferences()
{
  return mySSInterferences;
}
//===========================================================================
//function : ESInterferences
//purpose  : 
//===========================================================================
  BOPTools_CArray1OfESInterference&  BOPTools_InterferencePool::ESInterferences()
{
  return myESInterferences;
}
//===========================================================================
//function : VSInterferences
//purpose  : 
//===========================================================================
  BOPTools_CArray1OfVSInterference&  BOPTools_InterferencePool::VSInterferences()
{
  return myVSInterferences;
}
//===========================================================================
//function : EEInterferences
//purpose  : 
//===========================================================================
  BOPTools_CArray1OfEEInterference&  BOPTools_InterferencePool::EEInterferences()
{
  return myEEInterferences;
}
//===========================================================================
//function : VEInterferences
//purpose  : 
//===========================================================================
  BOPTools_CArray1OfVEInterference&  BOPTools_InterferencePool::VEInterferences()
{
  return myVEInterferences;
}

//===========================================================================
//function : VVInterferences
//purpose  : 
//===========================================================================
  BOPTools_CArray1OfVVInterference&  BOPTools_InterferencePool::VVInterferences()
{
  return myVVInterferences;
}
////////////////////////

//===========================================================================
//function : SSInterfs
//purpose  : 
//===========================================================================
  const BOPTools_CArray1OfSSInterference&  BOPTools_InterferencePool::SSInterfs()const
{
  return mySSInterferences;
}
//===========================================================================
//function : ESInterfs
//purpose  : 
//===========================================================================
  const BOPTools_CArray1OfESInterference&  BOPTools_InterferencePool::ESInterfs()const
{
  return myESInterferences;
}
//===========================================================================
//function : VSInterfs
//purpose  : 
//===========================================================================
  const BOPTools_CArray1OfVSInterference&  BOPTools_InterferencePool::VSInterfs()const
{
  return myVSInterferences;
}
//===========================================================================
//function : EEInterfs
//purpose  : 
//===========================================================================
  const BOPTools_CArray1OfEEInterference&  BOPTools_InterferencePool::EEInterfs()const
{
  return myEEInterferences;
}
//===========================================================================
//function : VEInterfs
//purpose  : 
//===========================================================================
  const BOPTools_CArray1OfVEInterference&  BOPTools_InterferencePool::VEInterfs()const
{
  return myVEInterferences;
}

//===========================================================================
//function : VVInterfs
//purpose  : 
//===========================================================================
  const BOPTools_CArray1OfVVInterference&  BOPTools_InterferencePool::VVInterfs()const
{
  return myVVInterferences;
}
//===========================================================================
//function : GetInterference
//purpose  : 
//===========================================================================
BOPTools_PShapeShapeInterference
  BOPTools_InterferencePool::GetInterference(const Standard_Integer anIndex,
					     const BooleanOperations_KindOfInterference aType)const
{
  Standard_Integer aNb;
  BOPTools_PShapeShapeInterference pI=NULL;

  switch (aType) {
    //
    case BooleanOperations_SurfaceSurface:
      aNb=mySSInterferences.Extent();
      if (anIndex > 0 && anIndex <= aNb) {
	pI=(BOPTools_PShapeShapeInterference)&mySSInterferences(anIndex);
      }
      break;
    //
    case BooleanOperations_EdgeSurface:
      aNb=myESInterferences.Extent();
      if (anIndex > 0 && anIndex <= aNb) {
	pI=(BOPTools_PShapeShapeInterference)&myESInterferences(anIndex);
      }
      break;
    //
    case BooleanOperations_VertexSurface:
      aNb=myVSInterferences.Extent();
      if (anIndex > 0 && anIndex <= aNb) {
	pI=(BOPTools_PShapeShapeInterference)&myVSInterferences(anIndex);
      }
      break;
    //
    case BooleanOperations_EdgeEdge:
      aNb=myEEInterferences.Extent();
      if (anIndex > 0 && anIndex <= aNb) {
	pI=(BOPTools_PShapeShapeInterference)&myEEInterferences(anIndex);
      }
      break;
    //  
    case BooleanOperations_VertexEdge:
      aNb=myVEInterferences.Extent();
      if (anIndex > 0 && anIndex <= aNb) {
	pI=(BOPTools_PShapeShapeInterference)&myVEInterferences(anIndex);
      }
      break;
    //
    case BooleanOperations_VertexVertex:
      aNb=myVVInterferences.Extent();
      if (anIndex > 0 && anIndex <= aNb) {
	pI=(BOPTools_PShapeShapeInterference)&myVVInterferences(anIndex);
      }
      break;
    //
    case BooleanOperations_UnknownInterference:
    default:
      break;
  }
  return pI;
}

