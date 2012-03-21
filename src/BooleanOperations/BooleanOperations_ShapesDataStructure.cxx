// Created on: 2000-08-10
// Created by: Vincent DELOS
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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



#include <BooleanOperations_ShapesDataStructure.ixx>

#include <stdio.h>
#include <stdlib.h>

#include <gp_Pnt.hxx>
#include <Bnd_Box.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_TShape.hxx>
#include <TopoDS_TVertex.hxx>
#include <TopoDS_TEdge.hxx>
#include <TopoDS_TWire.hxx>
#include <TopoDS_TFace.hxx>
#include <TopoDS_TShell.hxx>
#include <TopoDS_TSolid.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_TCompound.hxx>
#include <TopoDS_TCompSolid.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <BRepTools_ShapeSet.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ShapeSet.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_IndexedMapOfOrientedShape.hxx>

#include <BooleanOperations_OnceExplorer.hxx>
#include <BooleanOperations_AncestorsAndSuccessors.hxx>
#include <BooleanOperations_AncestorsSeqAndSuccessorsSeq.hxx>
#include <BooleanOperations_IndexedDataMapOfShapeAncestorsSuccessors.hxx>

#include <TColStd_SequenceOfInteger.hxx>

#include <Bnd_Box.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <BOPTColStd_Dump.hxx>


#ifdef WNT
#pragma warning ( disable : 4291 )
#endif

static
  void Message(const Standard_Integer i);

const static Standard_Integer AddedValue=20;

//===========================================================================
//function : BooleanOperations_ShapesDataStructure
//purpose  : creator
//===========================================================================
  BooleanOperations_ShapesDataStructure::BooleanOperations_ShapesDataStructure():
  myListOfShapeAndInterferences(NULL),
  myNumberOfShapesOfTheObject(0),
  myNumberOfShapesOfTheTool(0),
  myNumberOfInsertedShapes(0),
  myLength(0),
  myNbEdges(0)
{
  //printf("-BOPDS_ShapesDataStructure CREATE:%x\n", (int)this);
}
//===========================================================================
//function : BooleanOperations_ShapesDataStructure
//purpose  : creator
//===========================================================================
  BooleanOperations_ShapesDataStructure::BooleanOperations_ShapesDataStructure(const TopoDS_Shape& Object,
									       const TopoDS_Shape& Tool)
:
  myListOfShapeAndInterferences(NULL),
  myNumberOfShapesOfTheObject(0),
  myNumberOfShapesOfTheTool(0),
  myNumberOfInsertedShapes(0),
  myLength(0),
  myObject(Object),
  myTool(Tool),
  myNbEdges(0)
{
  //printf(" BOPDS_ShapesDataStructure CREATE:%x\n", (int)this);
  Standard_Integer i, Average;//, aNbShapes;
  BooleanOperations_IndexedDataMapOfShapeAncestorsSuccessors IndDatMapTool;
  BooleanOperations_IndexedDataMapOfShapeAncestorsSuccessors IndDatMapObject;

  FillIndexedMapOfShapesAncestorsAndSuccessors(Object,IndDatMapObject);
  FillIndexedMapOfShapesAncestorsAndSuccessors(Tool,IndDatMapTool);
  myNumberOfShapesOfTheObject = IndDatMapObject.Extent();
  myNumberOfShapesOfTheTool = IndDatMapTool.Extent();
  Average = (myNumberOfShapesOfTheTool+myNumberOfShapesOfTheObject)/2;
  myLength = myNumberOfShapesOfTheTool+myNumberOfShapesOfTheObject+Average;
  /////
  myListOfShapeAndInterferences = (BooleanOperations_PShapeAndInterferences)
    Standard::Allocate(myLength*sizeof(BooleanOperations_ShapeAndInterferences));
  //
  // Inserting the shapes into the DS 
  // Object
  for (i=1; i<=myNumberOfShapesOfTheObject; i++){
    const BooleanOperations_AncestorsSeqAndSuccessorsSeq& theAncestorsSeqAndSuccessorsSeq =
      IndDatMapObject.FindFromIndex(i);
    const TopoDS_Shape& theShape = IndDatMapObject.FindKey(i);
    InsertShapeAndAncestorsSuccessors(theShape, theAncestorsSeqAndSuccessorsSeq, 0);
  }
  // Tool
  for (i=1;i<=myNumberOfShapesOfTheTool;i++){
    const BooleanOperations_AncestorsSeqAndSuccessorsSeq& theAncestorsSeqAndSuccessorsSeq =
      IndDatMapTool.FindFromIndex(i);
    const TopoDS_Shape& theShape = IndDatMapTool.FindKey(i);
    InsertShapeAndAncestorsSuccessors(theShape,theAncestorsSeqAndSuccessorsSeq,myNumberOfShapesOfTheObject);
  }
  //
  // Fill the myShapeIndexMapObj
  for (i=1; i<=myNumberOfShapesOfTheObject; ++i){
    const TopoDS_Shape& aS=GetShape(i);
    myShapeIndexMapObj.Add(aS, i);
  }
  //
  //
  // Fill the myShapeIndexMapObj
  Standard_Integer iFirst, iLast;
  //
  ToolRange (iFirst, iLast);  
  for (i=iFirst; i<=iLast; ++i){
    const TopoDS_Shape& aS=GetShape(i);
    myShapeIndexMapTool.Add(aS, i);
  }
  //
  iLast=myNumberOfShapesOfTheObject+myNumberOfShapesOfTheTool;
  //
  // Fill myRefEdges
  myRefEdges.Resize(iLast);

  for (i=1; i<=iLast; ++i) {
    const TopoDS_Shape& aS=Shape(i);
    myRefEdges(i)=0;
    if (aS.ShapeType()==TopAbs_EDGE) {
      myNbEdges++;
      myRefEdges(i)=myNbEdges;
    }
  }
}
//modified by NIZNHY-PKV Wed Feb  2 11:34:07 2005f
//===========================================================================
//function : ~
//purpose  : 
//===========================================================================
  BooleanOperations_ShapesDataStructure::~BooleanOperations_ShapesDataStructure()
{
  //printf(" BOPDS_ShapesDataStructure DELETE:%x\n", (int)this);
  Destroy();
}
//modified by NIZNHY-PKV Wed Feb  2 11:34:12 2005t
//===========================================================================
//function : BooleanOperations_ShapesDataStructure
//purpose  : destructor
//===========================================================================
  void BooleanOperations_ShapesDataStructure::Destroy()
{
  Standard_Integer i;

  for (i=0;i<myNumberOfInsertedShapes;i++) {
    myListOfShapeAndInterferences[i].~BooleanOperations_ShapeAndInterferences();
  }
  
  //modified by NIZNHY-PKV Wed Feb  2 12:31:28 2005f
  //printf(" ~     :%x, now:%x\n", 
	// (int)this, 
	// (int)myListOfShapeAndInterferences);
  //modified by NIZNHY-PKV Wed Feb  2 12:31:31 2005t
  //
  Standard::Free((Standard_Address&)myListOfShapeAndInterferences);
}
//===========================================================================
//function : ShapeIndexMap
//purpose  : 
//===========================================================================
  const BooleanOperations_IndexedDataMapOfShapeInteger& 
    BooleanOperations_ShapesDataStructure::ShapeIndexMap(const Standard_Integer iRank)const
{
  if (iRank == 1)
    return myShapeIndexMapObj;
  else 
    return myShapeIndexMapTool;
}
//===========================================================================
//function : ShapeIndex
//purpose  : 
//===========================================================================
  Standard_Integer BooleanOperations_ShapesDataStructure::ShapeIndex(const TopoDS_Shape& aS,
								     const Standard_Integer iRank) const
{
  Standard_Integer anIndex=0;
  const BooleanOperations_IndexedDataMapOfShapeInteger& aMap=ShapeIndexMap(iRank);
  if (aMap.Contains(aS)) {
    anIndex=aMap.FindFromKey(aS);
    return anIndex;
  }
  return anIndex;
}

//===========================================================================
//function : FillIndexedMapOfShapesAncestorsAndSuccessors
//purpose  : 
//===========================================================================
  void BooleanOperations_ShapesDataStructure::FillIndexedMapOfShapesAncestorsAndSuccessors
  (const TopoDS_Shape& Sha,
   BooleanOperations_IndexedDataMapOfShapeAncestorsSuccessors& IndDatMap) const
{
  Standard_Integer TotalNumberOfShapes = 1;
  BooleanOperations_AncestorsSeqAndSuccessorsSeq theAncestorAndSuccessor;
  IndDatMap.Add(Sha,theAncestorAndSuccessor);
  this->FindSubshapes(Sha,TotalNumberOfShapes,IndDatMap);
  //
  Standard_Integer aNumberOfShapes = IndDatMap.Extent();
  for(Standard_Integer i=1; i <= aNumberOfShapes; i++) {
    const BooleanOperations_AncestorsSeqAndSuccessorsSeq& anAncestorAndSuccessorSeq1=IndDatMap(i);
    TColStd_MapOfInteger aMapOfIndices;
    for(Standard_Integer j = 1; j <= anAncestorAndSuccessorSeq1.NumberOfSuccessors(); j++) {
      Standard_Integer aShapeIndex = anAncestorAndSuccessorSeq1.GetSuccessor(j);
      if(aMapOfIndices.Add(aShapeIndex)) {
	BooleanOperations_AncestorsSeqAndSuccessorsSeq& anAncestorAndSuccessorSeq2 = 
	  IndDatMap.ChangeFromIndex(aShapeIndex);
	anAncestorAndSuccessorSeq2.SetNewAncestor(i);
      }
    }
  }
}
//===========================================================================
//function : FindSubshapes
//purpose  : 
//===========================================================================
  void BooleanOperations_ShapesDataStructure::FindSubshapes
  (const TopoDS_Shape& Sha,
   Standard_Integer& TotalNumberOfShapes,
   BooleanOperations_IndexedDataMapOfShapeAncestorsSuccessors& IndDatMap) const
{
// 
  TopoDS_Iterator anIt(Sha, Standard_True);//Standard_False);
  Standard_Integer anIndexOfShape = IndDatMap.FindIndex(Sha);
  BooleanOperations_AncestorsSeqAndSuccessorsSeq& AncSucOfShape=IndDatMap.ChangeFromIndex(anIndexOfShape);
  for(; anIt.More(); anIt.Next()) {
    const TopoDS_Shape& aSubShape = anIt.Value();
    Standard_Integer aSubShapeIndex = 0;
    Standard_Boolean isNewSubShape = Standard_False;
    if(!IndDatMap.Contains(aSubShape)) {
      isNewSubShape = Standard_True;
      BooleanOperations_AncestorsSeqAndSuccessorsSeq anAncestorAndSuccessorSeq;
      aSubShapeIndex = IndDatMap.Add(aSubShape, anAncestorAndSuccessorSeq);
    }
    else {
      aSubShapeIndex = IndDatMap.FindIndex(aSubShape);
    }
    AncSucOfShape.SetNewSuccessor(aSubShapeIndex);
    AncSucOfShape.SetNewOrientation(aSubShape.Orientation());
    //
    if(isNewSubShape && (aSubShape.ShapeType() != TopAbs_VERTEX)) {
      FindSubshapes(aSubShape, TotalNumberOfShapes, IndDatMap);
    }
  }
  TotalNumberOfShapes = IndDatMap.Extent();
}
//===========================================================================
//function : ReInit
//purpose  : 
//===========================================================================
  void BooleanOperations_ShapesDataStructure::ReInit() 
{
  Standard_Integer i,NewLength;
  BooleanOperations_PShapeAndInterferences NewListOfShapeAndInterferences;

  NewLength = AddedValue + myLength;
  NewListOfShapeAndInterferences = (BooleanOperations_PShapeAndInterferences)
    Standard::Allocate(NewLength*sizeof(BooleanOperations_ShapeAndInterferences));
  
  //modified by NIZNHY-PKV Wed Feb  2 12:16:51 2005f
  //printf(" ReInit:%x, was:%x, now:%x\n", 
	// (int)this, 
	// (int)myListOfShapeAndInterferences,
	// (int)NewListOfShapeAndInterferences);
  //modified by NIZNHY-PKV Wed Feb  2 12:16:55 2005t
  
  for (i=0;i<myNumberOfInsertedShapes;i++)    {
    new (&NewListOfShapeAndInterferences[i])
      BooleanOperations_ShapeAndInterferences(myListOfShapeAndInterferences[i]);
    myListOfShapeAndInterferences[i].myShape.Nullify();
  }

  if (myLength>0) {
    Standard::Free((Standard_Address&) myListOfShapeAndInterferences);
  }

  myLength = NewLength;
  myListOfShapeAndInterferences = NewListOfShapeAndInterferences;
}
//===========================================================================
//function : InsertShapeAndAncestorsSuccessors
//purpose  : 
//===========================================================================
void BooleanOperations_ShapesDataStructure::InsertShapeAndAncestorsSuccessors
  (const TopoDS_Shape& S,
   const BooleanOperations_AncestorsSeqAndSuccessorsSeq& AncSuc,
   const Standard_Integer shift)
{
  if ((myNumberOfInsertedShapes<0)||(myNumberOfInsertedShapes>myLength)) {
    Message(1);
  }
  //
  Bnd_Box B;
  //
  if (myNumberOfInsertedShapes==myLength) {
    ReInit();
  }
  
  new (&(myListOfShapeAndInterferences[myNumberOfInsertedShapes].myShape)) TopoDS_Shape(S);
  // Compute and insert the bounding box of <myShape>.
  if (!S.IsNull()) {
    BRepBndLib::Add(S,B);
  }
  new (&(myListOfShapeAndInterferences[myNumberOfInsertedShapes].myBoundingBox)) 
    Bnd_Box(B);
  new (&(myListOfShapeAndInterferences[myNumberOfInsertedShapes].myAncestorsAndSuccessors)) 
    BooleanOperations_AncestorsAndSuccessors(AncSuc,shift);

  myListOfShapeAndInterferences[myNumberOfInsertedShapes].myState = BooleanOperations_UNKNOWN;
  myNumberOfInsertedShapes++;
}

//===========================================================================
//function : GetShape
//purpose  : 
//===========================================================================
  const TopoDS_Shape& BooleanOperations_ShapesDataStructure::GetShape(const Standard_Integer index) const
{
  if ((index<1)||(index>myNumberOfInsertedShapes))    {
    Message(1);
  }
  return myListOfShapeAndInterferences[index-1].GetShape();
}
//===========================================================================
//function : Shape
//purpose  : 
//===========================================================================
  const TopoDS_Shape& BooleanOperations_ShapesDataStructure::Shape(const Standard_Integer index) const
{
  return GetShape(index);
}
//===========================================================================
//function : GetShapeType
//purpose  : 
//===========================================================================
  TopAbs_ShapeEnum BooleanOperations_ShapesDataStructure::GetShapeType(const Standard_Integer index) const
{
  if ((index<1)||(index>myNumberOfInsertedShapes))    {
    Message(1);
  }
  return myListOfShapeAndInterferences[index-1].GetShapeType();
}
//===========================================================================
//function : GetBoundingBox
//purpose  : 
//===========================================================================
  const Bnd_Box& BooleanOperations_ShapesDataStructure::GetBoundingBox(const Standard_Integer index) const
{
  if ((index<1)||(index>myNumberOfInsertedShapes))    {
    Message(1);
  }
  return myListOfShapeAndInterferences[index-1].GetBoundingBox();
}
//===========================================================================
//function : GetState
//purpose  : 
//===========================================================================
  BooleanOperations_StateOfShape BooleanOperations_ShapesDataStructure::GetState(const Standard_Integer index) const
{
  if ((index<1)||(index>myNumberOfInsertedShapes))    {
    Message(1);
  }
  return myListOfShapeAndInterferences[index-1].GetState();
}
//===========================================================================
//function : SetState
//purpose  : 
//===========================================================================
  void BooleanOperations_ShapesDataStructure::SetState(const Standard_Integer index,
						       const BooleanOperations_StateOfShape theState) 
{
  if ((index<1)||(index>myNumberOfInsertedShapes)) {
    Message(1);
  }
  myListOfShapeAndInterferences[index-1].SetState(theState);
}

//===========================================================================
//function : NumberOfAncestors
//purpose  : 
//===========================================================================
  Standard_Integer 
    BooleanOperations_ShapesDataStructure::NumberOfAncestors(const Standard_Integer index) const
{
  if ((index<1)||(index>myNumberOfInsertedShapes))  {
    Message(1);
  }
  return myListOfShapeAndInterferences[index-1].NumberOfAncestors();
}
//===========================================================================
//function : NumberOfSuccessors
//purpose  : 
//===========================================================================
  Standard_Integer 
    BooleanOperations_ShapesDataStructure::NumberOfSuccessors(const Standard_Integer index) const
{
  if ((index<1)||(index>myNumberOfInsertedShapes))    {
    Message(1);
  }
  return myListOfShapeAndInterferences[index-1].NumberOfSuccessors();
}
//===========================================================================
//function : GetAncestor
//purpose  : 
//===========================================================================
 Standard_Integer 
  BooleanOperations_ShapesDataStructure::GetAncestor(const Standard_Integer index,
						       const Standard_Integer ancestorNumber) const
{
  if ((index<1)||
      (index>myNumberOfInsertedShapes)||
      (ancestorNumber<1)||
      (ancestorNumber>NumberOfAncestors(index))) {
    Message(1);
  }
  return myListOfShapeAndInterferences[index-1].GetAncestor(ancestorNumber);
}
//===========================================================================
//function : GetSuccessor
//purpose  : 
//===========================================================================
  Standard_Integer 
    BooleanOperations_ShapesDataStructure::GetSuccessor(const Standard_Integer index,
							const Standard_Integer successorNumber) const
{
  if ((index<1)||(index>myNumberOfInsertedShapes)||(successorNumber<1)||(successorNumber>NumberOfSuccessors(index)))    {
    Message(1);
  }
  if (GetShapeType(index) == TopAbs_VERTEX)  {
    Message(2);
  }
  return myListOfShapeAndInterferences[index-1].GetSuccessor(successorNumber);
}
//===========================================================================
//function : GetAncestors
//purpose  : returns the array of ancestors
//===========================================================================
 void 
   BooleanOperations_ShapesDataStructure::GetAncestors(const Standard_Integer index,
							Standard_Address& theArrayOfAncestors,
							Standard_Integer& ancestorsNumber) const
{
  if ((index<1)||(index>myNumberOfInsertedShapes))    {
    Message(1);
  }
  myListOfShapeAndInterferences[index-1].GetAncestors(theArrayOfAncestors,ancestorsNumber);
}

 
//===========================================================================
//function : GetSuccessors
//purpose  : returns the array of successors
//===========================================================================
  void 
    BooleanOperations_ShapesDataStructure::GetSuccessors(const Standard_Integer index,
							 Standard_Address& theArrayOfSuccessors,
							 Standard_Integer& successorsNumber) const
{
  if ((index<1)||(index>myNumberOfInsertedShapes))  {
    Message(1);
  }
  if (GetShapeType(index) == TopAbs_VERTEX) {
    Message(2);
  }
  myListOfShapeAndInterferences[index-1].GetSuccessors(theArrayOfSuccessors,successorsNumber);
}

//
//===========================================================================
//function : NumberOfShapesOfTheObject
//purpose  : 
//===========================================================================
  Standard_Integer BooleanOperations_ShapesDataStructure::NumberOfShapesOfTheObject()const
{
  return myNumberOfShapesOfTheObject;
}
//===========================================================================
//function : NumberOfShapesOfTheTool
//purpose  : 
//===========================================================================
  Standard_Integer BooleanOperations_ShapesDataStructure::NumberOfShapesOfTheTool()const
{
  return myNumberOfShapesOfTheTool;
}

//

//===========================================================================
//function : NumberOfInsertedShapes
//purpose  : 
//===========================================================================
  Standard_Integer BooleanOperations_ShapesDataStructure::NumberOfInsertedShapes()const
{
  return myNumberOfInsertedShapes;
}
//===========================================================================
//function : NumberOfNewShapes
//purpose  : 
//===========================================================================
  Standard_Integer BooleanOperations_ShapesDataStructure::NumberOfNewShapes()const
{
  Standard_Integer aNb;

  aNb=NumberOfSourceShapes();
  aNb=myNumberOfInsertedShapes-aNb;
  return aNb;
}

//===========================================================================
//function : NumberOfSourceShapes
//purpose  : 
//===========================================================================
  Standard_Integer BooleanOperations_ShapesDataStructure::NumberOfSourceShapes()const
{
  Standard_Integer aNb;
  aNb=myNumberOfShapesOfTheTool+myNumberOfShapesOfTheObject;
  return aNb;
}

//===========================================================================
//function : IsNewShape
//purpose  : 
//===========================================================================
  Standard_Boolean 
    BooleanOperations_ShapesDataStructure::IsNewShape(const Standard_Integer anIndex)const
{
  Standard_Boolean aFlag;
  aFlag=anIndex>NumberOfSourceShapes();
  return aFlag;
}


//===========================================================================
//function : Line
//purpose  : 
//===========================================================================
  const  BooleanOperations_ShapeAndInterferences& 
    BooleanOperations_ShapesDataStructure::Line(const Standard_Integer index) const
{
  if ((index<1)||(index>myNumberOfInsertedShapes)) {
    Message(1);
  }
  return myListOfShapeAndInterferences[index-1];
}
//===========================================================================
//function : Object
//purpose  : 
//===========================================================================
  const TopoDS_Shape& BooleanOperations_ShapesDataStructure::Object()const 
{
  return myObject;
}
//===========================================================================
//function : Tool
//purpose  : 
//===========================================================================
  const TopoDS_Shape& BooleanOperations_ShapesDataStructure::Tool()const 
{
  return myTool;
}
//===========================================================================
//function : ObjectRange
//purpose  : 
//===========================================================================
  void BooleanOperations_ShapesDataStructure::ObjectRange(Standard_Integer& iFirst,
							  Standard_Integer& iLast)const 
{
  iFirst=1;
  iLast=NumberOfShapesOfTheObject();
}

//===========================================================================
//function : ToolRange
//purpose  : 
//===========================================================================
  void BooleanOperations_ShapesDataStructure::ToolRange(Standard_Integer& iFirst,
							Standard_Integer& iLast)const 
{
  iFirst=NumberOfShapesOfTheObject()+1;
  iLast=NumberOfShapesOfTheObject()+NumberOfShapesOfTheTool();
}
//===========================================================================
//function : Rank
//purpose  : 
//===========================================================================
  Standard_Integer BooleanOperations_ShapesDataStructure::Rank(const Standard_Integer nS)const
							
{
  if (IsNewShape(nS)) {
    return 3;
  }
  
  Standard_Integer iFirst, iLast;
  
  ObjectRange(iFirst, iLast);
  if (nS >= iFirst && nS <= iLast){
    return 1;
  }
  
  ToolRange(iFirst, iLast);
  if (nS >= iFirst && nS <= iLast){
    return 2;
  }
  return 0; // ?
}

//===========================================================================
//function : RefEdge
//purpose  : 
//===========================================================================
  Standard_Integer 
    BooleanOperations_ShapesDataStructure::RefEdge(const Standard_Integer anIndex)const
{
  Standard_Integer iRefEdge;
  iRefEdge=myRefEdges(anIndex);
  return iRefEdge;
}

//===========================================================================
//function : NbEdges
//purpose  : 
//===========================================================================
  Standard_Integer BooleanOperations_ShapesDataStructure::NbEdges()const 
{
  return myNbEdges;
}


//===========================================================================
//function : Message
//purpose  : 
//===========================================================================
void Message(const Standard_Integer i)
{
  char buf[256];
  sprintf(buf, " BooleanOperations_ShapesDataStructure:: ");
  BOPTColStd_Dump::PrintMessage(buf);

  switch (i) {
  case 1:
    sprintf (buf, "index is out of range\n");
    break;
  case 2:
    sprintf (buf, "incorrect Type\n");
    break;
  default: 
    sprintf(buf, "undefined message\n");
    break;
  }
  BOPTColStd_Dump::PrintMessage(buf);
  Standard_DomainError::Raise("Message");
}

//===========================================================================
//function : Dump
//purpose  : 
//===========================================================================
  void BooleanOperations_ShapesDataStructure::Dump(Standard_OStream& S) const
{
  
  Standard_Integer i,j;
  TopAbs_ShapeEnum T;
  //ZZ gp_Pnt thePoint;
  BooleanOperations_StateOfShape St;

  S<<endl<<"BooleanOperations_ShapesDataStructure::Dump()"<<endl;
  S<<endl<<"myLength                    = "<<myLength;
  S<<endl<<"myNumberOfInsertedShapes    = "<<myNumberOfInsertedShapes;
  S<<endl<<"myNumberOfShapesOfTheTool   = "<<myNumberOfShapesOfTheTool;
  S<<endl<<"myNumberOfShapesOfTheObject = "<<myNumberOfShapesOfTheObject<<endl;

  for (i=1;i<=myNumberOfInsertedShapes;i++)
    {
      S << "---";
      if (i < 10)   cout<< " ";
      if (i < 100)  cout<< " ";
      if (i < 1000) cout<< " ";
      cout << i << " --- ";
      T = GetShape(i).ShapeType();
      switch(T)
	{
	case TopAbs_VERTEX :
	  S << "VERTEX   ";
	  break;
	  
	case TopAbs_EDGE :
	  S << "EDGE     ";
	  break;
	  
	case TopAbs_WIRE :
	  S << "WIRE     ";
	  break;
	  
	case TopAbs_FACE :
	  S << "FACE     ";
	  break;
	  
	case TopAbs_SHELL :
	  S << "SHELL    ";
	  break;
	  
	case TopAbs_SOLID :
	  S << "SOLID    ";
	  break;
	  
	case TopAbs_COMPSOLID :
	  S << "COMPSOLID";
	  break;
	  
	case TopAbs_COMPOUND :
	  S << "COMPOUND ";
	  break;
	  
	case TopAbs_SHAPE :
	  S << "SHAPE";
	  break;
	}
      St = GetState(i);
      switch(St)
	{
	case BooleanOperations_IN :
	  S << "IN          ";
	  break;
	  
	case BooleanOperations_OUT :
	  S << "OUT         ";
	  break;
	  
	case BooleanOperations_UNKNOWN :
	  S << "UNKNOWN     ";
	  break;
	  
	case BooleanOperations_ON :
	  S << "ON          ";
	  break;
	  
	case BooleanOperations_INOROUT :
	  S << "INOROUT     ";
	  break;
	  
	case BooleanOperations_INTERSECTED :
	  S << "INTERSECTED ";
	  break;

	}
      
      Standard_Real a,b,c,d,e,f;
      GetBoundingBox(i).Get(a,b,c,d,e,f);
      S << " @ " << a << " " << b << " " << c << " " << d << " " << e << " " << f << " @ " ;
      
      S << " Ancestors :" ;
      for (j=1;j<=NumberOfAncestors(i);j++)
	S << " " << GetAncestor(i,j);
      S << " Successors :" ;
      for (j=1;j<=NumberOfSuccessors(i);j++)
	S << " " << GetSuccessor(i,j);
      /*
      S << " INTERF = " ;
      for (j=1;j<=NumberOfInterferences(i);j++)
	S << " " << GetIntersectedShape(i,j) << " " << GetIntersectionResult(i,j) << " #";
      */
      S  << endl;
    }
  S  << endl;
}


//===========================================================================
//function : LightDump
//purpose  : 
//===========================================================================
  void BooleanOperations_ShapesDataStructure::LightDump(Standard_OStream& S) const
{
  Standard_Real a,b,c,d,e,f;
  Standard_Integer i;
  TopAbs_ShapeEnum T;

  S<<endl<<"BooleanOperations_ShapesDataStructure::Dump()"<<endl;
  S<<endl<<"myLength                    = "<<myLength;
  S<<endl<<"myNumberOfInsertedShapes    = "<<myNumberOfInsertedShapes;
  S<<endl<<"myNumberOfShapesOfTheTool   = "<<myNumberOfShapesOfTheTool;
  S<<endl<<"myNumberOfShapesOfTheObject = "<<myNumberOfShapesOfTheObject<<endl;

  for (i=1;i<=myNumberOfInsertedShapes;i++)
    {
      S << "---";
      if (i < 10)   cout<< " ";
      if (i < 100)  cout<< " ";
      if (i < 1000) cout<< " ";
      cout << i << " --- ";
      T = GetShape(i).ShapeType();
      switch(T)
	{
	case TopAbs_VERTEX :
	  S << "VERTEX   ";
	  break;
	  
	case TopAbs_EDGE :
	  S << "EDGE     ";
	  break;
	  
	case TopAbs_WIRE :
	  S << "WIRE     ";
	  break;
	  
	case TopAbs_FACE :
	  S << "FACE     ";
	  break;
	  
	case TopAbs_SHELL :
	  S << "SHELL    ";
	  break;
	  
	case TopAbs_SOLID :
	  S << "SOLID    ";
	  break;
	  
	case TopAbs_COMPSOLID :
	  S << "COMPSOLID";
	  break;
	  
	case TopAbs_COMPOUND :
	  S << "COMPOUND ";
	  break;
	  
	case TopAbs_SHAPE :
	  S << "SHAPE";
	  break;
	}
      GetBoundingBox(i).Get(a,b,c,d,e,f);
      S << " @ " << a << " " << b << " " << c << " " << d << " " << e << " " << f << endl;
    }
  S  << endl;
}
//===========================================================================
//function : GetOrientation
//purpose  : 
//===========================================================================
  TopAbs_Orientation 
    BooleanOperations_ShapesDataStructure::GetOrientation(const Standard_Integer index,
							  const Standard_Integer successorNumber) const
{
  if ((index<1)||(index>myNumberOfInsertedShapes)||(successorNumber<1)||(successorNumber>NumberOfSuccessors(index)))  {
    Message(1);
  }
  if (GetShapeType(index) == TopAbs_VERTEX)   {
    Message(2);
  }
  return myListOfShapeAndInterferences[index-1].GetOrientation(successorNumber);
}
//===========================================================================
//function : GetOrientations
//purpose  : returns the array of orientations
//===========================================================================
 void 
   BooleanOperations_ShapesDataStructure::GetOrientations(const Standard_Integer index,
							   Standard_Address& theArrayOfOrientations,
							   Standard_Integer& orientationsNumber) const
{
  if ((index<1)||(index>myNumberOfInsertedShapes))    {
    Message(1);
  }
  if (GetShapeType(index) == TopAbs_VERTEX)  {
    Message(2);
  }
  myListOfShapeAndInterferences[index-1].GetOrientations(theArrayOfOrientations,orientationsNumber);
}
//modified by NIZNHY-PKV Wed Feb  2 14:44:08 2005f
/*
#ifdef WNT
#pragma warning ( default : 4291 )
#endif
*/
/*
//===========================================================================
//function : InsertInterference
//purpose  : 
//===========================================================================
  void BooleanOperations_ShapesDataStructure::InsertInterference(const Standard_Integer index,
								 const BooleanOperations_InterferenceResult&)// IR) 
{
  if ((index<1)||(index>myNumberOfInsertedShapes))    {
    Message(1);
  }
//modified by NIZNHY-PKV Wed Feb  2 13:02:32 2005ft
//  myListOfShapeAndInterferences[index-1].SetInterference(IR);
}
//===========================================================================
//function : GetInterference
//purpose  : 
//===========================================================================
  const BooleanOperations_InterferenceResult& 
    BooleanOperations_ShapesDataStructure::GetInterference(const Standard_Integer index,
							   const Standard_Integer interfNumber) const
{
  
  if ((index<1)||(index>myNumberOfInsertedShapes))    {
    Message(1);
  }
  //modified by NIZNHY-PKV Wed Feb  2 13:04:12 2005f
  //return myListOfShapeAndInterferences[index-1].GetInterference(interfNumber);
  
  static BooleanOperations_InterferenceResult aIR;
  return aIR;
  //modified by NIZNHY-PKV Wed Feb  2 13:04:22 2005t
}
 
//===========================================================================
//function : GetIntersectedShape
//purpose  : 
//===========================================================================
  Standard_Integer 
    BooleanOperations_ShapesDataStructure::GetIntersectedShape(const Standard_Integer index,
							       const Standard_Integer interfNumber) const
{
  if ((index<1)||(index>myNumberOfInsertedShapes))    {
    Message(1);
  }
  //modified by NIZNHY-PKV Wed Feb  2 13:04:39 2005f
  //return myListOfShapeAndInterferences[index-1].GetIntersectedShape(interfNumber);
  return 0;
  //modified by NIZNHY-PKV Wed Feb  2 13:04:45 2005t
}
//===========================================================================
//function : GetIntersectionResult
//purpose  : 
//===========================================================================
  Standard_Integer 
    BooleanOperations_ShapesDataStructure::GetIntersectionResult (const Standard_Integer index,
								  const Standard_Integer interfNumber) const
{
  if ((index<1)||(index>myNumberOfInsertedShapes))    {
    Message(1);
  }
  //modified by NIZNHY-PKV Wed Feb  2 13:05:36 2005f
  //return myListOfShapeAndInterferences[index-1].GetIntersectionResult(interfNumber);
  return 0;
  //modified by NIZNHY-PKV Wed Feb  2 13:04:45 2005t
}
//===========================================================================
//function : NumberOfInterferences
//purpose  : 
//===========================================================================
 Standard_Integer 
    BooleanOperations_ShapesDataStructure::NumberOfInterferences(const Standard_Integer index) const
{
  if ((index<1)||(index>myNumberOfInsertedShapes))    {
    Message(1);
  }
  //modified by NIZNHY-PKV Wed Feb  2 13:06:43 2005f
  //return myListOfShapeAndInterferences[index-1].NumberOfInterferences();
  return 0;
  //modified by NIZNHY-PKV Wed Feb  2 13:04:45 2005t
}

*/
//modified by NIZNHY-PKV Wed Feb  2 14:44:11 2005t
