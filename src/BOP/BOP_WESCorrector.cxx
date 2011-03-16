// File:	BOP_WESCorrector.cxx
// Created:	Fri Apr 13 10:47:47 2001
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <BOP_WESCorrector.ixx>

#include <TopTools_IndexedMapOfOrientedShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>

#include <BOP_ConnexityBlock.hxx>
#include <BOP_ListIteratorOfListOfConnexityBlock.hxx>
#include <BOP_WireSplitter.hxx>
#include <BOPTColStd_ListOfListOfShape.hxx>
#include <BOPTColStd_ListIteratorOfListOfListOfShape.hxx>

static
  void MakeWire(const TopTools_ListOfShape& aLE, 
		TopoDS_Wire& newWire);

//=======================================================================
// function: BOP_WESCorrector::BOP_WESCorrector
// purpose: 
//=======================================================================
  BOP_WESCorrector::BOP_WESCorrector()
:
  myIsDone(Standard_False),
  myErrorStatus(1)
{}
//=======================================================================
// function: SetWES
// purpose: 
//=======================================================================
  void BOP_WESCorrector::SetWES (const BOP_WireEdgeSet& aWES)
{
  BOP_WireEdgeSet* pWES=(BOP_WireEdgeSet*) &aWES;
  myWES=pWES;
}
//=======================================================================
// function: WES
// purpose: 
//=======================================================================
  BOP_WireEdgeSet& BOP_WESCorrector::WES () 
{
  return *myWES;
}
//=======================================================================
// function: NewWES
// purpose: 
//=======================================================================
  BOP_WireEdgeSet& BOP_WESCorrector::NewWES () 
{
  return myNewWES;
}
//=======================================================================
// function: IsDone
// purpose: 
//=======================================================================
  Standard_Boolean BOP_WESCorrector::IsDone () const 
{
  return myIsDone;
}
//=======================================================================
// function: ErrorStatus
// purpose: 
//=======================================================================
  Standard_Integer BOP_WESCorrector::ErrorStatus () const 
{
  return myErrorStatus;
}

//=======================================================================
// function: Do
// purpose: 
//=======================================================================
  void BOP_WESCorrector::Do()
{
  DoConnexityBlocks();
  
  DoCorrections();
  
  myIsDone=Standard_True;
}
//=======================================================================
// function: DoConnexityBlocks
// purpose: 
//=======================================================================
  void BOP_WESCorrector::DoConnexityBlocks()
{
  Standard_Boolean EnewinM, aGoOn;
  Standard_Integer Mextent, IsRegular, aNbNeighbours, Eindex;
  TopTools_IndexedMapOfOrientedShape myOrientedShapeMap, aMap;
  //
  myWES->InitStartElements();
  for (; myWES->MoreStartElements(); myWES->NextStartElement()) {
    const TopoDS_Shape& anE = myWES->StartElement(); 
    Mextent = myOrientedShapeMap.Extent();

    Eindex  = myOrientedShapeMap.Add(anE); 

    EnewinM = (Eindex > Mextent);
    if (EnewinM) {
      //
      // make a new block starting at element Eindex
      IsRegular=Standard_True; 
      aNbNeighbours=0;
      Mextent = myOrientedShapeMap.Extent();
      //
      aMap.Clear();
      aMap.Add(anE);
      //
      aGoOn = (Eindex <= Mextent);
      while (aGoOn) {
	const TopoDS_Shape& anEE = myOrientedShapeMap(Eindex);
	aNbNeighbours = myWES->MaxNumberSubShape(anEE);
	
	IsRegular = IsRegular && (aNbNeighbours == 2);
	//
	myWES->InitNeighbours(anEE);
	for (; myWES->MoreNeighbours(); myWES->NextNeighbour()) {
	  const TopoDS_Shape& aNeignbE = myWES->Neighbour();
	  myOrientedShapeMap.Add(aNeignbE);
	  //
	  aMap.Add(aNeignbE);
	}
	
	Eindex++;
	Mextent = myOrientedShapeMap.Extent();
	aGoOn = (Eindex <= Mextent);
      } // end of while aGoOn
      
      //
      //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
      {
	Standard_Integer aNbE;

	aNbE=aMap.Extent();
	if (aNbE==2) {
	  const TopoDS_Edge& aEB1=TopoDS::Edge(aMap(1));
	  const TopoDS_Edge& aEB2=TopoDS::Edge(aMap(2));
	  if (aEB1.IsSame(aEB2)) {
	    if (!BRep_Tool::IsClosed(aEB1, myWES->Face())) { 
	      IsRegular=Standard_False;
	    }
	  }
	}
      }
      //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
      //
      BOP_ConnexityBlock aConnexityBlock;
      aConnexityBlock.SetShapes(aMap);
      aConnexityBlock.SetRegularity(IsRegular);
      myConnexityBlocks.Append(aConnexityBlock);
    } // end of if (EnewinM)
  } // end of for (; myWES->MoreStartElements();...
}

//=======================================================================
// function: DoCorrections
// purpose: 
//=======================================================================
  void BOP_WESCorrector::DoCorrections()
{
  Standard_Boolean anIsRegular, anIsNothingToDo, anIsDone;

  TopoDS_Wire aW;
  BOP_ListIteratorOfListOfConnexityBlock aCBIt;
  
  myNewWES.Initialize(myWES->Face());

  aCBIt.Initialize(myConnexityBlocks);
  for (; aCBIt.More(); aCBIt.Next()) {
    const BOP_ConnexityBlock& aCB=aCBIt.Value();
    const TopTools_ListOfShape& aListOfEdges=aCB.Shapes();

    anIsRegular=aCB.IsRegular();

    if (anIsRegular) {
      MakeWire(aListOfEdges, aW);
      myNewWES.AddShape (aW);
      continue;
    }
    //
    // Treatment of non regular Connexity Block
    const TopoDS_Face& aF=myWES->Face();
    BOP_WireSplitter aWireSplitter;
    aWireSplitter.SetFace(aF);
    aWireSplitter.DoWithListOfEdges(aListOfEdges);
    
    anIsDone=aWireSplitter.IsDone();
    anIsNothingToDo=aWireSplitter.IsNothingToDo();
    
    if (!anIsDone || anIsNothingToDo) {
      MakeWire(aListOfEdges, aW);
      myNewWES.AddShape (aW);
      continue;
    }
    //
    const BOPTColStd_ListOfListOfShape& aSSS=aWireSplitter.Shapes();
    
    BOPTColStd_ListIteratorOfListOfListOfShape aWireIt(aSSS);
    for (; aWireIt.More(); aWireIt.Next()) {
      const TopTools_ListOfShape& aListEd=aWireIt.Value();
      //
      MakeWire(aListEd, aW);
      myNewWES.AddShape (aW);
    }
  }
}

//=======================================================================
// function: MakeWire
// purpose: 
//=======================================================================
  void MakeWire(const TopTools_ListOfShape& aLE, 
		TopoDS_Wire& newWire)
{
  BRep_Builder aBB;
  aBB.MakeWire(newWire);

  TopTools_ListIteratorOfListOfShape anIt(aLE);
  for (; anIt.More(); anIt.Next()){
    const TopoDS_Edge& aE=TopoDS::Edge(anIt.Value());
    aBB.Add(newWire, aE);
  }
}
/////////////////////////////////////////////////////////////////
//
// myErrorStatus:
//
// 1 - Nothing is done because only constructor has been invoked
//
/////////////////////////////////////////////////////////////////
