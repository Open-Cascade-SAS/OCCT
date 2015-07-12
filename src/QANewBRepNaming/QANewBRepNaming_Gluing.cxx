// Created on: 2001-11-26
// Created by: Sergey ZARITCHNY <szy@nnov.matra-dtv.fr>
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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
#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <QANewBRepNaming_Gluing.hxx>
#include <QANewModTopOpe_Glue.hxx>
#include <Standard_NullObject.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDF_AttributeMap.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TDF_Label.hxx>
#include <TDF_MapIteratorOfAttributeMap.hxx>
#include <TDF_MapIteratorOfLabelMap.hxx>
#include <TDF_TagSource.hxx>
#include <TDF_Tool.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Selector.hxx>
#include <TNaming_Tool.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#ifdef OCCT_DEBUG
#include <TDataStd_Name.hxx>
#endif

#define SOURCES_SUBLABEL 1

//=======================================================================
//function : QANewBRepNaming_Gluing
//purpose  : 
//=======================================================================

QANewBRepNaming_Gluing::QANewBRepNaming_Gluing() {}

//=======================================================================
//function : QANewBRepNaming_Gluing
//purpose  : 
//=======================================================================

QANewBRepNaming_Gluing::QANewBRepNaming_Gluing(const TDF_Label& ResultLabel):QANewBRepNaming_TopNaming(ResultLabel) {}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void QANewBRepNaming_Gluing::Init(const TDF_Label& ResultLabel) {  
  if(ResultLabel.IsNull()) 
    Standard_NullObject::Raise("QANewBRepNaming_Gluing::Init The Result label is Null ..."); 
  myResultLabel = ResultLabel; 
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void QANewBRepNaming_Gluing::Load(QANewModTopOpe_Glue& theMkGluing) {
  TopoDS_Shape aResShape  = theMkGluing.Shape();
  const TopoDS_Shape& anObjShape = theMkGluing.Shape1();

  if (aResShape.IsNull()) {
#ifdef OCCT_DEBUG
    cout<<"QANewBRepNaming_Gluing::Load(): The result of the Gluing operation is null"<<endl;
#endif
    return;
  }
  // If the shapes are the same - select the result and exit:
  if (IsResultChanged(theMkGluing)) {
#ifdef OCCT_DEBUG
    cout<<"QANewBRepNaming_Gluing::Load(): The object and the result of the operation are the same"<<endl;
#endif
    if (aResShape.ShapeType() == TopAbs_COMPOUND) {
      Standard_Integer aNbSubResults = 0;
      TopoDS_Iterator anItr(theMkGluing.Shape());
      for (; anItr.More(); anItr.Next()) aNbSubResults++;
      if (aNbSubResults == 1) {
	anItr.Initialize(aResShape);
	if (anItr.More()) aResShape = anItr.Value();
      }
    }    
    TNaming_Builder aBuilder(ResultLabel());
    aBuilder.Select(aResShape, anObjShape);
    return;
  }
  // Naming of the result:
//   cout<<"********** RecomputeUnique"<<endl;
  RecomputeUnique(theMkGluing);
  TopTools_DataMapOfShapeInteger aSources;

//   cout<<"********** LoadSourceShapes"<<endl;
  LoadSourceShapes(aSources);
//mpv (there is must be shared elements anyway)  if(theMkGluing.HasGenerated()) 
//   cout<<"********** LoadUniqueShapes"<<endl;
  LoadUniqueShapes(theMkGluing,aSources);
  LoadContent(theMkGluing);
  if(theMkGluing.HasModified()) {
    LoadModifiedShapes(theMkGluing);
  }
  LoadResult(theMkGluing);
}

//=======================================================================
//function : ShapeType
//purpose  : 
//=======================================================================

TopAbs_ShapeEnum QANewBRepNaming_Gluing::ShapeType(const TopoDS_Shape& theShape) const {
  TopAbs_ShapeEnum aShapeType = theShape.ShapeType();
  if (aShapeType == TopAbs_COMPOUND || aShapeType == TopAbs_COMPSOLID) {
    TopoDS_Iterator itr(theShape);
    if (itr.More()) aShapeType = itr.Value().ShapeType();
  } 
  return aShapeType;
} 

//=======================================================================
//function : GetShape
//purpose  : 
//=======================================================================

TopoDS_Shape QANewBRepNaming_Gluing::GetShape(const TopoDS_Shape& theShape) const {
  if (theShape.ShapeType() == TopAbs_COMPOUND || theShape.ShapeType() == TopAbs_COMPSOLID) {
    TopoDS_Iterator itr(theShape);
    if (itr.More()) return itr.Value();
  }
  return theShape;
}

//=======================================================================
//function : LoadResult
//purpose  : 
//=======================================================================

void QANewBRepNaming_Gluing::LoadResult(QANewModTopOpe_Glue& theMkGluing) const {
  Handle(TDF_TagSource) Tagger = TDF_TagSource::Set(ResultLabel());
  if (Tagger.IsNull()) return;
  Tagger->Set(0);
  TNaming_Builder aBuilder (ResultLabel());
  TopoDS_Shape aResult = theMkGluing.Shape();
  if (aResult.ShapeType() == TopAbs_COMPOUND) {
    Standard_Integer aNbSubResults = 0;
    TopoDS_Iterator anItr(aResult);
    for (; anItr.More(); anItr.Next()) aNbSubResults++;
    if (aNbSubResults == 1) {
      anItr.Initialize(aResult);
      if (anItr.More()) aResult = anItr.Value();
    }
  }
  if (theMkGluing.Shape1().IsNull()) aBuilder.Generated(aResult);
  else {
    aBuilder.Generated(theMkGluing.Shape1(), aResult);  // mpv:?
  }
}


//=======================================================================
//function : IsResultChanged
//purpose  : 
//=======================================================================

Standard_Boolean QANewBRepNaming_Gluing::IsResultChanged(QANewModTopOpe_Glue& theMkGluing) const {
  TopoDS_Shape aResShape = theMkGluing.Shape();
  if (theMkGluing.Shape().ShapeType() == TopAbs_COMPOUND) {
    Standard_Integer aNbSubResults = 0;
    TopoDS_Iterator anItr(theMkGluing.Shape());
    for (; anItr.More(); anItr.Next()) aNbSubResults++;
    if (aNbSubResults == 1) {
      anItr.Initialize(theMkGluing.Shape());
      if (anItr.More()) aResShape = anItr.Value();
    }
  }
  return theMkGluing.Shape1().IsSame(aResShape);
}

//=======================================================================
//function : LoadModifiedShapes
//purpose  : 
//=======================================================================

void QANewBRepNaming_Gluing::LoadModifiedShapes(QANewModTopOpe_Glue& theMkGluing) const {
  const Standard_Integer aNumTypes = 1;
  const TopAbs_ShapeEnum aTypes[] = {TopAbs_FACE/*,TopAbs_EDGE,TopAbs_VERTEX*/};
#ifdef OCCT_DEBUG
  const char aNames[][20] = {"ModifiedFaces"/*,"ModifiedEdges","ModifiedVertexes"*/};
#endif
  Standard_Integer aShapeNum;

  TDF_Label aLabel = ResultLabel().FindChild(1,Standard_True);
  Handle(TNaming_NamedShape) aNS;
  while(aLabel.FindAttribute(TNaming_NamedShape::GetID(),aNS)) {
    if (aNS->Evolution() == TNaming_MODIFY) break;
    aLabel = aLabel.Father().FindChild(aLabel.Tag()+1,Standard_True);
  }
  for(Standard_Integer a=0;a<aNumTypes;a++) { // argument-shapes types cycle
#ifdef OCCT_DEBUG
    TDataStd_Name::Set(aLabel,TCollection_ExtendedString(Standard_CString(aNames[a])));
#endif
    TNaming_Builder aBuilder(aLabel);
    // functionality from Loader
    TopExp_Explorer ShapeExplorer;
    for(aShapeNum=0;aShapeNum<2;aShapeNum++) { // argument-shapes cycle
      if (aShapeNum==0) ShapeExplorer.Init(theMkGluing.Shape1(), aTypes[a]);
      else ShapeExplorer.Init(theMkGluing.Shape2(), aTypes[a]);
      TopTools_MapOfShape View;
      for (; ShapeExplorer.More(); ShapeExplorer.Next ()) { // argument-shapes subshapes cycle
	const TopoDS_Shape& Root = ShapeExplorer.Current ();
	if (!View.Add(Root)) continue;
	const TopTools_ListOfShape& Shapes = theMkGluing.Modified(Root);
	TopTools_ListIteratorOfListOfShape ShapesIterator (Shapes);
	for (;ShapesIterator.More (); ShapesIterator.Next ()) { // argument-shapes subshapes parents cycle
	  const TopoDS_Shape& newShape = ShapesIterator.Value ();
	  if (!Root.IsSame (newShape) && !myUnique.Contains(newShape)) { // modifyed compound can't contains unique
	    aBuilder.Modify(Root,newShape);
	  }
	}
      }
    }
    if (a+1<aNumTypes) aLabel = aLabel.Father().FindChild(aLabel.Tag()+1,Standard_True);
  }
}

//=======================================================================
//function : SetContext
//purpose  : 
//=======================================================================

void QANewBRepNaming_Gluing::SetContext(const TopoDS_Shape& theObject,
				   const TopoDS_Shape& theTool) {
  TopoDS_Compound aCompound;
  BRep_Builder aB;
  aB.MakeCompound(aCompound);
  aB.Add(aCompound,theObject);
  aB.Add(aCompound,theTool);
  myContext = aCompound;
}

//=======================================================================
//function : SetLog
//purpose  : 
//=======================================================================

void QANewBRepNaming_Gluing::SetLog(const TDF_LabelMap& theLog) {
  myLog.Assign(theLog);
}

//=======================================================================
//function : AddToTheUnique
//purpose  : 
//=======================================================================
void QANewBRepNaming_Gluing::AddToTheUnique(const TopoDS_Shape& theUnique, const TopoDS_Shape& theIdentifier) {
  if (!myUnique.Contains(theUnique)) {
    TopTools_ListOfShape* aNewList = new TopTools_ListOfShape;
    aNewList->Append(theIdentifier);
    myUnique.Add(theUnique, *aNewList);
  } else {
    myUnique.ChangeFromKey(theUnique).Append(theIdentifier);
  }
}

//=======================================================================
//function : RecomputeUnique
//purpose  : 
//=======================================================================
void QANewBRepNaming_Gluing::RecomputeUnique(QANewModTopOpe_Glue& theMkGluing) {
  const Standard_Integer aNumTypes = 3;
  const TopAbs_ShapeEnum aTypes[] = {TopAbs_FACE,TopAbs_EDGE,TopAbs_VERTEX};

  // special case: edges may be divided, these edges (new and modified)
  // we try to sort in the algorithm returned lists order
  myUnique.Clear();
  Standard_Integer aNext;
  TopExp_Explorer anExp(myContext, TopAbs_EDGE);
  TopTools_ListIteratorOfListOfShape anEdgesIterator;
  for(;anExp.More();anExp.Next()) {
    for(aNext=0;aNext<2;aNext++) {
      TopoDS_Edge aFullEdge = TopoDS::Edge(anExp.Current());
      if (aNext == 0) anEdgesIterator.Initialize(theMkGluing.Generated(aFullEdge));
      else anEdgesIterator.Initialize(theMkGluing.Modified(aFullEdge));
      for (;anEdgesIterator.More();anEdgesIterator.Next())
        if (anEdgesIterator.Value().ShapeType() == TopAbs_EDGE) {
          TopoDS_Edge aDivEdge = TopoDS::Edge(anEdgesIterator.Value());
          if (!TopExp::FirstVertex(aDivEdge).IsNull()) { // divided edge must have at least one vertex
            if (TopExp::FirstVertex(aFullEdge).IsNull()) AddToTheUnique(aDivEdge,aFullEdge);
            else if (!TopExp::LastVertex(aDivEdge).IsNull()) {//if full edge have at least one vertex, subedges must have two
              if (TopExp::LastVertex(aFullEdge).IsNull()) {
                if (BRep_Tool::Pnt(TopExp::FirstVertex(aFullEdge)).
                    IsEqual(BRep_Tool::Pnt(TopExp::FirstVertex(aDivEdge)),Precision::Confusion()))
                  AddToTheUnique(aDivEdge,aFullEdge);
                else if (BRep_Tool::Pnt(TopExp::FirstVertex(aFullEdge)).
                   IsEqual(BRep_Tool::Pnt(TopExp::LastVertex(aDivEdge)),Precision::Confusion()))
                  AddToTheUnique(aDivEdge,aFullEdge);
              } else {
                Standard_Integer aCounter = 0;
                if (BRep_Tool::Pnt(TopExp::FirstVertex(aFullEdge)).
                    IsEqual(BRep_Tool::Pnt(TopExp::FirstVertex(aDivEdge)),Precision::Confusion()) ||
                    BRep_Tool::Pnt(TopExp::FirstVertex(aFullEdge)).
                    IsEqual(BRep_Tool::Pnt(TopExp::LastVertex(aDivEdge)),Precision::Confusion())) aCounter++;
                if (BRep_Tool::Pnt(TopExp::LastVertex(aFullEdge)).
                    IsEqual(BRep_Tool::Pnt(TopExp::FirstVertex(aDivEdge)),Precision::Confusion()) ||
                    BRep_Tool::Pnt(TopExp::LastVertex(aFullEdge)).
                    IsEqual(BRep_Tool::Pnt(TopExp::LastVertex(aDivEdge)),Precision::Confusion())) aCounter++;
                if (aCounter!=2) AddToTheUnique(aDivEdge,aFullEdge);
              }
            }
          }
        }
    }
  }
  aNext = 2;
  Standard_Integer aDividedIndex = myUnique.Extent();;
  // accumulate all shapes, modifyed from shape1
  while(aNext) {
    for(Standard_Integer a=0;a<aNumTypes;a++) {
      TopExp_Explorer ShapeExplorer1;
      if (aNext==2) ShapeExplorer1.Init(theMkGluing.Shape1(), aTypes[a]);
      else ShapeExplorer1.Init(theMkGluing.Shape2(), aTypes[a]);
      for (; ShapeExplorer1.More(); ShapeExplorer1.Next ()) {
	TopTools_ListIteratorOfListOfShape ShapesIterator1(theMkGluing.Generated(ShapeExplorer1.Current()));
	for (;ShapesIterator1.More (); ShapesIterator1.Next ()) {
	  if (myUnique.Contains(ShapesIterator1.Value()))
	    if (myUnique.FindIndex(ShapesIterator1.Value()) <= aDividedIndex) continue;
	  AddToTheUnique(ShapesIterator1.Value(),ShapeExplorer1.Current());
	}
      }
    }
    aNext--;
  }

#ifdef OCCT_DEBUG
  cout<<"QANewBRepNaming_Gluing::RecomputeUnique found "<<myUnique.Extent()<<" shapes"<<endl;
#endif
}

//=======================================================================
//function : LoadSourceShapes
//purpose  : 
//=======================================================================
void QANewBRepNaming_Gluing::LoadSourceShapes(TopTools_DataMapOfShapeInteger& theSources) const {
#ifdef OCCT_DEBUG
  cout<<"********** QANewBRepNaming_Gluing::LoadSourceShapes the valid map:"<<endl;
  TDF_MapIteratorOfLabelMap anIter(myLog);
  for(;anIter.More();anIter.Next()) {
    TCollection_AsciiString entry;
    TDF_Tool::Entry(anIter.Key(), entry);
    cout<<entry<<endl;
  }
#endif
  theSources.Clear();

  TDF_Label aLabel = ResultLabel().FindChild(SOURCES_SUBLABEL);
  Handle(TDF_TagSource) Tagger = TDF_TagSource::Set(aLabel);
  if (Tagger.IsNull()) return;
  Tagger->Set(0);

  // get all source shapes (in the given order)
  TopTools_MapOfShape aShapes;
  TopTools_ListOfShape aSortedShapes;
  Standard_Integer anIndex;
  for(anIndex=1;anIndex<=myUnique.Extent();anIndex++) {
    TopTools_ListIteratorOfListOfShape anIter2(myUnique.FindFromIndex(anIndex));
    for(;anIter2.More();anIter2.Next()) {
      if (!aShapes.Contains(anIter2.Value())) {
	aShapes.Add(anIter2.Value());
	aSortedShapes.Append(anIter2.Value());
      }
    }
  }
  // put all source shapes to the sources label sublabels
  TopTools_ListIteratorOfListOfShape aSortedIterator(aSortedShapes);
  for(aLabel = aLabel.NewChild();aShapes.Extent();aLabel = aLabel.Father().NewChild()) {
    Handle(TNaming_NamedShape) aNS;
    if (aLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
      // old shape resolving
      TDF_LabelMap aValid;
      aValid.Assign(myLog);
      TNaming_Selector aSel(aLabel);
      aSel.Solve(aValid);
      TopoDS_Shape aShape = aSel.NamedShape()->Get();
      if (aShapes.Contains(aShape)) {
	theSources.Bind(aShape,aLabel.Tag());
	aShapes.Remove(aShape);
      }
    } else {
      // add new shape
      for(;aSortedIterator.More();aSortedIterator.Next()) {
	TopoDS_Shape aShape = aSortedIterator.Value();;
	if (aShapes.Contains(aShape)) {
	  theSources.Bind(aShape,aLabel.Tag());
	  aShapes.Remove(aShape);
	  TNaming_Selector aSelector(aLabel);
	  if (!aSelector.Select(aShape,myContext)) {
#ifdef OCCT_DEBUG	  
	    cout<<"Can't do naming"<<endl;
#endif
	  }
	  anIndex++;
	  break;
	}
      }
    }
  }
}

//=======================================================================
//function : LoadUniqueShapes
//purpose  : 
//=======================================================================
void QANewBRepNaming_Gluing::LoadUniqueShapes(QANewModTopOpe_Glue& /*theMkGluing*/, 
                                              const TopTools_DataMapOfShapeInteger& theSources)
{
  Standard_Integer anIndex=1;
  TColStd_MapOfInteger alreadyUsed;
  while(alreadyUsed.Extent() != myUnique.Extent()) { // result sublabels cycle
    TDF_Label aLabel = ResultLabel().NewChild();
    Handle(TNaming_NamedShape) aNS;
    if (aLabel.FindAttribute(TNaming_NamedShape::GetID(),aNS)) {
      if (aNS->Evolution() == TNaming_MODIFY) continue;
      Handle(TDataStd_IntegerArray) anArray;
      if (aLabel.FindAttribute(TDataStd_IntegerArray::GetID(),anArray)) { // old unique with dependencies
	Standard_Integer a;
	for(a = anIndex;a<=myUnique.Extent();a++) {
	  if (alreadyUsed.Contains(a)) continue;
	  if (myUnique.FindFromIndex(a).Extent()!=anArray->Upper()) continue;
// 	  cout<<"a="<<a<<endl;
	  TopTools_ListIteratorOfListOfShape anIter2(myUnique.FindFromIndex(a));
	  Standard_Boolean aEq = Standard_True;
	  for(;anIter2.More() && aEq;anIter2.Next()) {
	    Standard_Integer aValue = theSources.Find(anIter2.Value());
	    Standard_Integer a1;
	    for(a1=anArray->Upper();a1>=1;a1--) if (anArray->Value(a1) == aValue) break;
	    if (a1 == 0) aEq = Standard_False;
	  }
// 	  cout<<"eq="<<aEq<<endl;
	  if (aEq) {
	    TNaming_Builder aBuilder(aLabel);
	    aBuilder.Generated(myUnique.FindKey(a));
	    alreadyUsed.Add(a);
	    break;
	  }
	}
      }
      continue;
    }
    // set new entity
    while(alreadyUsed.Contains(anIndex)) anIndex++;
//     cout<<"anIndex = "<<anIndex<<endl;
    Handle(TDataStd_IntegerArray) anArray=TDataStd_IntegerArray::Set(aLabel,1,myUnique.FindFromIndex(anIndex).Extent());
    TopTools_ListIteratorOfListOfShape anIter(myUnique.FindFromIndex(anIndex));
    Standard_Integer a;
    for(a=1;anIter.More();anIter.Next(),a++) {
      anArray->SetValue(a,theSources.Find(anIter.Value()));
    }
    TNaming_Builder aBuilder(aLabel);
    aBuilder.Generated(myUnique.FindKey(anIndex));
    alreadyUsed.Add(anIndex);
  }
}


//=======================================================================
//function : Content
//purpose  : 
//=======================================================================
TDF_Label QANewBRepNaming_Gluing::Content() const {
#ifdef OCCT_DEBUG
  const TDF_Label& ContentLabel = ResultLabel().NewChild();
  TDataStd_Name::Set(ContentLabel, "Content");
  return ContentLabel;
#else
  return ResultLabel().NewChild();
#endif
}

//=======================================================================
//function : LoadUniqueShapes
//purpose  : 
//=======================================================================
void QANewBRepNaming_Gluing::LoadContent(QANewModTopOpe_Glue& MS) const {
  if (MS.Shape().ShapeType() == TopAbs_COMPSOLID || MS.Shape().ShapeType() == TopAbs_COMPOUND) {
    TopoDS_Iterator itr(MS.Shape());
    Standard_Integer nbShapes = 0;
    while (itr.More()) {
      nbShapes++;
      itr.Next();
    }
    if (nbShapes > 1) {
      for (itr.Initialize(MS.Shape()); itr.More(); itr.Next()) {
	  TNaming_Builder bContent(Content());
	  bContent.Generated(itr.Value());      
      }
    }
  } 
}
