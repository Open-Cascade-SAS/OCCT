// Created on: 1993-10-15
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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


#include <BRepAlgoAPI_BooleanOperation.ixx>

#include <BOP_Builder.hxx>
#include <BOP_Section.hxx>
#include <BOP_ShellShell.hxx>
#include <BOP_SolidSolid.hxx>
#include <BOP_ShellSolid.hxx>
#include <BOP_WireWire.hxx>
#include <BOP_WireShell.hxx>
#include <BOP_WireSolid.hxx>
#include <BOPTools_DSFiller.hxx>
#include <BOPTools_Tools3D.hxx>
#include <BOP_EmptyBuilder.hxx>

#include <BOP_WireSolidHistoryCollector.hxx>
#include <BOP_ShellSolidHistoryCollector.hxx>
#include <BOP_SolidSolidHistoryCollector.hxx>
#include <BOP_SectionHistoryCollector.hxx>
#include <BRepLib_FuseEdges.hxx>
#include <TopExp.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

static Handle(BOP_HistoryCollector) MakeCollector(const TopoDS_Shape&       theShape1,
						  const TopoDS_Shape&       theShape2,
						  const BOP_Operation       theOperation);


//=======================================================================
//function : BRepAlgoAPI_BooleanOperation
//purpose  : 
//=======================================================================
  BRepAlgoAPI_BooleanOperation::BRepAlgoAPI_BooleanOperation(const TopoDS_Shape& aS1, 
							     const TopoDS_Shape& aS2,
							     const BOP_Operation anOp)
: 
  myS1(aS1),
  myS2(aS2),
  myBuilderCanWork(Standard_False),
  myOperation(anOp),
  myErrorStatus(1),
  myDSFiller(NULL),
  myBuilder(NULL),
  myEntryType(1),
  myFuseEdges(Standard_False)
{
}
//=======================================================================
//function : BRepAlgoAPI_BooleanOperation
//purpose  : 
//=======================================================================
  BRepAlgoAPI_BooleanOperation::BRepAlgoAPI_BooleanOperation(const TopoDS_Shape& aS1, 
							     const TopoDS_Shape& aS2,
							     const BOPTools_DSFiller& aDSFiller,
							     const BOP_Operation anOp)
: 
  myS1(aS1),
  myS2(aS2),
  myBuilderCanWork(Standard_False),
  myOperation(anOp),
  myErrorStatus(1),
  myDSFiller(NULL),
  myBuilder(NULL),
  myEntryType(0),
  myFuseEdges(Standard_False)
{
  if ((Standard_Address) &aDSFiller!=NULL) {
    myDSFiller=(BOPTools_PDSFiller)&aDSFiller;
  }
}
//=======================================================================
//function : Destroy
//purpose  : 
//=======================================================================
  void BRepAlgoAPI_BooleanOperation::Destroy()
{
  if (myBuilder!=NULL) {
    delete myBuilder;
    myBuilder=NULL;
  }
  if (myDSFiller!=NULL && myEntryType) {
    delete myDSFiller;
    myDSFiller=NULL;
  }
}
//=======================================================================
//function : SetOperation
//purpose  : 
//=======================================================================
  void BRepAlgoAPI_BooleanOperation::SetOperation (const BOP_Operation anOp)
{
  myOperation=anOp;
}
//=======================================================================
//function : Operation
//purpose  : 
//=======================================================================
  BOP_Operation BRepAlgoAPI_BooleanOperation::Operation ()const
{
  return myOperation;
}

//=======================================================================
//function : FuseEdges
//purpose  : 
//=======================================================================
  Standard_Boolean BRepAlgoAPI_BooleanOperation::FuseEdges ()const
{
  return myFuseEdges;
}

//=======================================================================
//function : Shape1
//purpose  : 
//=======================================================================
  const TopoDS_Shape& BRepAlgoAPI_BooleanOperation::Shape1() const 
{
  return myS1;
}

//=======================================================================
//function : Shape2
//purpose  : 
//=======================================================================
  const TopoDS_Shape& BRepAlgoAPI_BooleanOperation::Shape2() const 
{
  return myS2;
}

//=======================================================================
//function : BuilderCanWork
//purpose  : 
//=======================================================================
  Standard_Boolean BRepAlgoAPI_BooleanOperation::BuilderCanWork() const
{
  return myBuilderCanWork;
}
//=======================================================================
//function : ErrorStatus
//purpose  : 
//=======================================================================
  Standard_Integer BRepAlgoAPI_BooleanOperation::ErrorStatus()const
{
  return myErrorStatus;    
}
//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================
const TopTools_ListOfShape& BRepAlgoAPI_BooleanOperation::Modified(const TopoDS_Shape& aS) 
{
  if (myBuilder==NULL) {
    myGenerated.Clear();
    return myGenerated;
  }
  else {
    const TopTools_ListOfShape& aLM=myBuilder->Modified(aS);

    if(myFuseEdges) {
      return RefinedList(aLM);
    }

    return aLM;
  }
}

//=======================================================================
//function : IsDeleted
//purpose  : 
//=======================================================================
  Standard_Boolean BRepAlgoAPI_BooleanOperation::IsDeleted(const TopoDS_Shape& aS) 
{

//   Standard_Boolean bDeleted = Standard_True; 
//   if (myBuilder==NULL) {
//     return bDeleted; 
//   }
//   else {
//     bDeleted=myBuilder->IsDeleted(aS);
//     return bDeleted;    
//   }
  if(myHistory.IsNull()) {
    Standard_Boolean bDeleted = Standard_True; 

    if (myBuilder==NULL) {
      return bDeleted; 
    }
    else {
      bDeleted = myBuilder->IsDeleted(aS);
      return bDeleted;    
    }
  }
  return myHistory->IsDeleted(aS);
}
//=======================================================================
//function : PrepareFiller
//purpose  : 
//=======================================================================
  Standard_Boolean BRepAlgoAPI_BooleanOperation::PrepareFiller()
{
  Standard_Boolean bIsNewFiller=Standard_False;
  myErrorStatus=1;
  //
  if (myS1.IsNull() || myS2.IsNull()) {
    myErrorStatus=2;
    return bIsNewFiller;
  }
  //
  if (myOperation==BOP_UNKNOWN) {
    myErrorStatus=6;
    return bIsNewFiller;
  }
  //
  if (myDSFiller==NULL) {
    bIsNewFiller=!bIsNewFiller;

    myDSFiller=new BOPTools_DSFiller;
    //
    if (myDSFiller==NULL) {
      myErrorStatus=4;
      return bIsNewFiller;
    }
    //
    myDSFiller->SetShapes(myS1, myS2);
    if (!myDSFiller->IsDone()) {
      myErrorStatus=3;
      if (myDSFiller!=NULL) {
	delete myDSFiller;
	myDSFiller = NULL;
	return bIsNewFiller;
      }
    }
  }

  return bIsNewFiller;
}
//=======================================================================
//function : Build
//purpose  : 
//=======================================================================
  void BRepAlgoAPI_BooleanOperation::Build()
{
  Standard_Boolean bIsDone, bIsNewFiller;
  //
  myBuilderCanWork=Standard_False;
  NotDone();
  //
  bIsNewFiller=PrepareFiller();
  //
  if (myErrorStatus!=1) {
    // there was errors during the preparation 
    return;
  }
  //
  if (bIsNewFiller) {
    //Prepare the DS
    myDSFiller->Perform();
  }
  //
  if (myBuilder!=NULL) {
    delete myBuilder;
    myBuilder=NULL;
  }
  //
  const TopoDS_Shape& aS1 = myDSFiller->Shape1();
  const TopoDS_Shape& aS2 = myDSFiller->Shape2();
  //
  myShape.Nullify();
  // 
  // SECTION
  //
  if (myOperation==BOP_SECTION) {
    myBuilder=new BOP_Section;
  }
  // 
  // COMMON, FUSE, CUT12, CUT21
  //
  else if (myOperation==BOP_COMMON || myOperation==BOP_FUSE ||
	   myOperation==BOP_CUT    || myOperation==BOP_CUT21) {
    //
    // Check whether one or both of the arguments is(are) empty shape(s)
    // If yes, create BOP_EmptyBuilder object and build the result fast.
    {
      Standard_Boolean bIsEmptyShape1, bIsEmptyShape2;
      
      bIsEmptyShape1=BOPTools_Tools3D::IsEmptyShape(aS1);
      bIsEmptyShape2=BOPTools_Tools3D::IsEmptyShape(aS2);
      //
      if (bIsEmptyShape1 || bIsEmptyShape2) {
	myBuilder=new BOP_EmptyBuilder;
	//
	if (myBuilder==NULL) {
	  myErrorStatus=7;
	  return ;
	}
	//
	myBuilder->SetShapes(aS1, aS2);
	myBuilder->SetOperation (myOperation);
	myBuilder->DoWithFiller (*myDSFiller);
	
	bIsDone=myBuilder->IsDone();
	
	if (bIsDone) {
	  myErrorStatus=0;
	  myBuilderCanWork=Standard_True;
	  myShape=myBuilder->Result();
	  Done(); 
	}
	else {
	  myErrorStatus=100+myBuilder->ErrorStatus();
	  NotDone();
	}
	return;
      }
    }
    //
    TopAbs_ShapeEnum aT1, aT2;

    aT1=aS1.ShapeType();
    aT2=aS2.ShapeType();
    //
    // Shell / Shell
    if (aT1==TopAbs_SHELL && aT2==TopAbs_SHELL) {
      myBuilder=new BOP_ShellShell;
    }
    //
    // Solid / Solid
    else if (aT1==TopAbs_SOLID && aT2==TopAbs_SOLID) {
      myBuilder=new BOP_SolidSolid;
    }
    //
    // Shell / Solid
    else if ((aT1==TopAbs_SOLID && aT2==TopAbs_SHELL)
	     ||
	     (aT2==TopAbs_SOLID && aT1==TopAbs_SHELL)) {
      myBuilder=new BOP_ShellSolid;
    }
    //
    // Wire / Wire
    else if (aT1==TopAbs_WIRE && aT2==TopAbs_WIRE){
      myBuilder=new BOP_WireWire;
    }
    //
    // Wire / Shell
    else if ((aT1==TopAbs_WIRE  && aT2==TopAbs_SHELL)
	     ||
	     (aT2==TopAbs_WIRE  && aT1==TopAbs_SHELL)) {
      myBuilder=new BOP_WireShell;
    }
    //
    // Wire / Shell
    else if ((aT1==TopAbs_WIRE  && aT2==TopAbs_SOLID)
	     ||
	     (aT2==TopAbs_WIRE  && aT1==TopAbs_SOLID)) {
      myBuilder=new BOP_WireSolid;
    }
    else {
      myErrorStatus=5;
      return ;
    }
  }
  //
  if (myBuilder==NULL) {
    myErrorStatus=7;
    return ;
  }
  //
  myBuilder->SetShapes(aS1, aS2);
  myBuilder->SetOperation (myOperation);

  myHistory = MakeCollector(aS1, aS2, myOperation);
  myBuilder->SetHistoryCollector(myHistory);

  myBuilder->DoWithFiller (*myDSFiller);

  bIsDone=myBuilder->IsDone();
  
  if (bIsDone) {
    myErrorStatus=0;
    myBuilderCanWork=Standard_True;
    myShape=myBuilder->Result();
    Done(); 
  }
  else {
    myErrorStatus=100+myBuilder->ErrorStatus();
    NotDone();
  }
}

//
//=======================================================================
//function : SectionEdges
//purpose  : 
//=======================================================================
const  TopTools_ListOfShape& BRepAlgoAPI_BooleanOperation::SectionEdges()
{
  if (myBuilder==NULL) {
    myGenerated.Clear();
    return myGenerated;
  }
  else {
    const TopTools_ListOfShape& aLM=myBuilder->SectionEdges();

    if(myFuseEdges) {
      return RefinedList(aLM);
    }

    return aLM;
  }
}
//

// ================================================================================================
// function: Modified2
// purpose:
// ================================================================================================
const TopTools_ListOfShape& BRepAlgoAPI_BooleanOperation::Modified2(const TopoDS_Shape& aS) 
{
  if(myHistory.IsNull()) {
    myGenerated.Clear();
    return myGenerated;
  }

  if(myFuseEdges) {
    const TopTools_ListOfShape& aL = myHistory->Modified(aS);
    return RefinedList(aL);
  }

  return myHistory->Modified(aS);
}

// ================================================================================================
// function: Generated
// purpose:
// ================================================================================================
const TopTools_ListOfShape& BRepAlgoAPI_BooleanOperation::Generated(const TopoDS_Shape& S) 
{
  if(myHistory.IsNull()) {
    myGenerated.Clear();
    return myGenerated;
  }
 
  if(myFuseEdges) {
    const TopTools_ListOfShape& aL = myHistory->Generated(S);
    return RefinedList(aL);
  }

  return myHistory->Generated(S);
}

// ================================================================================================
// function: HasModified
// purpose:
// ================================================================================================
Standard_Boolean BRepAlgoAPI_BooleanOperation::HasModified() const
{
  if(myHistory.IsNull()) {
    return Standard_False;
  }
  return myHistory->HasModified();
}

// ================================================================================================
// function: HasGenerated
// purpose:
// ================================================================================================
Standard_Boolean BRepAlgoAPI_BooleanOperation::HasGenerated() const
{
  if(myHistory.IsNull()) {
    return Standard_False;
  }
  return myHistory->HasGenerated();
}

// ================================================================================================
// function: HasDeleted
// purpose:
// ================================================================================================
Standard_Boolean BRepAlgoAPI_BooleanOperation::HasDeleted() const
{
  if(myHistory.IsNull()) {
    return Standard_False;
  }
  return myHistory->HasDeleted();
}
//=======================================================================
//function : RefineEdges
//purpose  : 
//=======================================================================

  void BRepAlgoAPI_BooleanOperation::RefineEdges ()
{

  if(myFuseEdges) return; //Edges have been refined yet

  BRepLib_FuseEdges FE(myShape);
  FE.SetConcatBSpl(Standard_True);

  // avoid fusing old edges
  TopTools_IndexedMapOfShape mapOldEdges;
  TopExp::MapShapes (myS1, TopAbs_EDGE, mapOldEdges);
  TopExp::MapShapes (myS2, TopAbs_EDGE, mapOldEdges);
  FE.AvoidEdges (mapOldEdges);

  // Get List of edges that have been fused
  myFuseEdges = Standard_False;
  myModifFaces.Clear();
  myEdgeMap.Clear();
  TopTools_DataMapOfIntegerListOfShape aFusedEdges;

  FE.Edges(aFusedEdges);
  Standard_Integer nle = aFusedEdges.Extent();
  if (nle != 0) {
    FE.Perform();
    myShape = FE.Shape();

    TopTools_DataMapOfIntegerShape aResultEdges;

    FE.ResultEdges(aResultEdges);
    FE.Faces(myModifFaces);
    myFuseEdges = Standard_True;

    
    Standard_Integer i;
    for(i = 1; i <= nle; ++i) {
      const TopoDS_Shape& aNewE = aResultEdges(i);
      const TopTools_ListOfShape& aListOfOldEdges = aFusedEdges(i);
      TopTools_ListIteratorOfListOfShape anIter(aListOfOldEdges);
      for(; anIter.More(); anIter.Next()) {
	myEdgeMap.Bind(anIter.Value(), aNewE);
      }
    }
  }
  
}

//=======================================================================
//function : RefinedList
//purpose  : 
//=======================================================================
const TopTools_ListOfShape& 
  BRepAlgoAPI_BooleanOperation::RefinedList(const TopTools_ListOfShape& theL) 
{
  myGenerated.Clear();
  TopTools_MapOfShape aMap;

  TopTools_ListIteratorOfListOfShape anIter(theL);

  for(; anIter.More(); anIter.Next()) {
    const TopoDS_Shape& anS = anIter.Value();

    if(anS.ShapeType() == TopAbs_EDGE) {
      if(myEdgeMap.IsBound(anS)) {
	const TopoDS_Shape& aNewEdge = myEdgeMap.Find(anS);
	if(aMap.Add(aNewEdge)) {
	  myGenerated.Append(aNewEdge);
	}
      }
      else {
	myGenerated.Append(anS);
      }
    }
    else if (anS.ShapeType() == TopAbs_FACE) {
      if(myModifFaces.IsBound(anS)) {
	myGenerated.Append(myModifFaces.Find(anS));
      }
      else {
	myGenerated.Append(anS);
      }
    }
    else {
      myGenerated.Append(anS);
    }
  }

  return myGenerated;

}
// -----------------------------------------------------------------------------------------
// static function: MakeCollector
// purpose:
// -----------------------------------------------------------------------------------------
Handle(BOP_HistoryCollector) MakeCollector(const TopoDS_Shape&       theShape1,
					   const TopoDS_Shape&       theShape2,
					   const BOP_Operation       theOperation) {

  Handle(BOP_HistoryCollector) aresult;

  if(theOperation == BOP_SECTION) {
    aresult = new BOP_SectionHistoryCollector(theShape1, theShape2);
    return aresult;
  }

  TopAbs_ShapeEnum aT1, aT2;

  aT1 = theShape1.ShapeType();
  aT2 = theShape2.ShapeType();
  //
  // Shell / Shell
  if (aT1==TopAbs_SHELL && aT2==TopAbs_SHELL) {
    //     aresult = new BOP_ShellShellHistoryCollector(theShape1, theShape2, theOperation, theDSFiller);
  }
  //
  // Solid / Solid
  else if (aT1==TopAbs_SOLID && aT2==TopAbs_SOLID) {
    aresult = new BOP_SolidSolidHistoryCollector(theShape1, theShape2, theOperation);
  }
  //
  // Shell / Solid
  else if ((aT1==TopAbs_SOLID && aT2==TopAbs_SHELL)
	   ||
	   (aT2==TopAbs_SOLID && aT1==TopAbs_SHELL)) {
    aresult = new BOP_ShellSolidHistoryCollector(theShape1, theShape2, theOperation);
  }
  //
  // Wire / Wire
  else if (aT1==TopAbs_WIRE && aT2==TopAbs_WIRE){
    //     aresult = new BOP_WireWireHistoryCollector(theShape1, theShape2, theOperation, theDSFiller);
  }
  //
  // Wire / Shell
  else if ((aT1==TopAbs_WIRE  && aT2==TopAbs_SHELL)
	   ||
	   (aT2==TopAbs_WIRE  && aT1==TopAbs_SHELL)) {
    //     aresult = new BOP_WireShellHistoryCollector(theShape1, theShape2, theOperation, theDSFiller);
  }
  //
  // Wire / Shell
  else if ((aT1==TopAbs_WIRE  && aT2==TopAbs_SOLID)
	   ||
	   (aT2==TopAbs_WIRE  && aT1==TopAbs_SOLID)) {
    aresult = new BOP_WireSolidHistoryCollector(theShape1, theShape2, theOperation);
  }
  return aresult;
}

