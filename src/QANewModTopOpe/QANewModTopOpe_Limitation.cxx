// Created on: 2001-05-06
// Created by: Igor FEOKTISTOV
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
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <QANewModTopOpe_Limitation.hxx>
#include <Standard_ConstructionError.hxx>
#include <TopAbs.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

static TopoDS_Shape MakeCutTool(const TopoDS_Shape& theS2)
{
  TopoDS_Shape aRealCutTool, aSh;
  BRep_Builder aBB;

  aBB.MakeSolid(TopoDS::Solid(aRealCutTool));
  aBB.MakeShell(TopoDS::Shell(aSh));

  TopExp_Explorer anExp;

  anExp.Init(theS2,TopAbs_FACE);

  for(; anExp.More(); anExp.Next()) aBB.Add(aSh, anExp.Current());
  aBB.Add(aRealCutTool, aSh);

  return aRealCutTool;
}


QANewModTopOpe_Limitation::QANewModTopOpe_Limitation(const TopoDS_Shape& theObjectToCut,
					   const TopoDS_Shape& theCutTool,
					   const QANewModTopOpe_ModeOfLimitation theMode) : 
       myObjectToCut(theObjectToCut),
	   myCut(NULL),
	   myCommon(NULL),
       myFwdIsDone(Standard_False),
       myRevIsDone(Standard_False),
       myMode(theMode)       
{

  TopExp_Explorer anExp;
  anExp.Init(theCutTool,TopAbs_FACE);

  if(!anExp.More()) return;

  myCutTool = MakeCutTool(theCutTool);

  Cut();

}

void QANewModTopOpe_Limitation::Cut() 
{

  NotDone();

  if(myMode == QANewModTopOpe_Forward) {
    if(!myFwdIsDone) {
      myCut = new BRepAlgoAPI_Cut(myObjectToCut, myCutTool);
      if(myCut->IsDone()) {
	myResultFwd = myCut->Shape();
	myFwdIsDone = Standard_True;
      }
    }
    if(myFwdIsDone) {
      myShape = myResultFwd;
      Done();
    }
  }
  else if (myMode == QANewModTopOpe_Reversed) {
    if(!myRevIsDone) {
      myCommon = new BRepAlgoAPI_Common(myObjectToCut, myCutTool);
      if(myCommon->IsDone()) {
	myResultRvs = myCommon->Shape();
	myRevIsDone = Standard_True;
      }
    }
    if(myRevIsDone) {
      myShape = myResultRvs;
      Done();
    }
  }
  else if (myMode == QANewModTopOpe_BothParts) {
    if(!myFwdIsDone) {
      myCut = new BRepAlgoAPI_Cut(myObjectToCut, myCutTool);
      if(myCut->IsDone()) {
	myResultFwd = myCut->Shape();
	myFwdIsDone = Standard_True;
      }
    }

    if(!myRevIsDone) {
      myCommon = new BRepAlgoAPI_Common(myObjectToCut, myCutTool);
      if(myCommon->IsDone()) {
	myResultRvs = myCommon->Shape();
	myRevIsDone = Standard_True;
      }
    }

    if(myFwdIsDone && myRevIsDone) {
      myShape.Nullify();
      BRep_Builder aBB;
      aBB.MakeCompound(TopoDS::Compound(myShape));
      TopoDS_Iterator aItr;
      aItr.Initialize(myResultFwd, Standard_False, Standard_False);
      for(; aItr.More(); aItr.Next()) aBB.Add(myShape, aItr.Value());

      aItr.Initialize(myResultRvs, Standard_False, Standard_False);
      for(; aItr.More(); aItr.Next()) aBB.Add(myShape, aItr.Value());

      Done();
    }
    
  }
  else {
    Standard_ConstructionError::Raise("QANewModTopOpe_Limitation : wrong mode");
  }


}

void QANewModTopOpe_Limitation::SetMode(const QANewModTopOpe_ModeOfLimitation theMode)
{
  myMode = theMode;
}

QANewModTopOpe_ModeOfLimitation  QANewModTopOpe_Limitation::GetMode() const
{
  return myMode;
}

const TopoDS_Shape& QANewModTopOpe_Limitation::Shape1() const 
{
  return myObjectToCut;
}

const TopoDS_Shape& QANewModTopOpe_Limitation::Shape2() const 
{
  return myCutTool;
}


//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& QANewModTopOpe_Limitation::Modified(const TopoDS_Shape& S) 
{
  Check();
  myGenerated.Clear();
  if(myMode == QANewModTopOpe_Forward) {
    myGenerated = myCut->Modified(S);
  }
  else if(myMode == QANewModTopOpe_Reversed) {
    myGenerated = myCommon->Modified(S);
  }
  else {
    myGenerated = myCut->Modified(S);

    TopTools_MapOfShape aMap; // to check if shape can be added in list more then one time
    TopTools_ListIteratorOfListOfShape It(myGenerated);
    for(;It.More();It.Next()) {
      aMap.Add(It.Value());
    }
    
    It.Initialize(myCommon->Modified(S));
    for(;It.More();It.Next()) {
      if(aMap.Add(It.Value())) myGenerated.Append(It.Value());
    }
  }

  return myGenerated;
}

// ================================================================================================
// function: Generated
// purpose:
// ================================================================================================
const TopTools_ListOfShape& QANewModTopOpe_Limitation::Generated(const TopoDS_Shape& S) 
{
  Check();
  myGenerated.Clear();
  if(myMode == QANewModTopOpe_Forward) {
    myGenerated = myCut->Generated(S);
  }
  else if(myMode == QANewModTopOpe_Reversed) {
    myGenerated = myCommon->Generated(S);
  }
  else {
    myGenerated = myCut->Generated(S);

    TopTools_MapOfShape aMap; // to check if shape can be added in list more then one time
    TopTools_ListIteratorOfListOfShape It(myGenerated);
    for(;It.More();It.Next()) {
      aMap.Add(It.Value());
    }
    
    It.Initialize(myCommon->Generated(S));
    for(;It.More();It.Next()) {
      if(aMap.Add(It.Value())) myGenerated.Append(It.Value());
    }
  }

  return myGenerated;
}

// ================================================================================================
// function: HasModified
// purpose:
// ================================================================================================
Standard_Boolean QANewModTopOpe_Limitation::HasModified() const
{
  Check();
  if(myMode == QANewModTopOpe_Forward) {
    return myCut->HasModified();
  }
  else if(myMode == QANewModTopOpe_Reversed) {
    return myCommon->HasModified();
  }
  else {
    return myCut->HasModified() || myCommon->HasModified();
  }
}

// ================================================================================================
// function: HasGenerated
// purpose:
// ================================================================================================
Standard_Boolean QANewModTopOpe_Limitation::HasGenerated() const
{
  Check();
  if(myMode == QANewModTopOpe_Forward) {
    return myCut->HasGenerated();
  }
  else if(myMode == QANewModTopOpe_Reversed) {
    return myCommon->HasGenerated();
  }
  else {
    return myCut->HasGenerated() || myCommon->HasGenerated();
  }
}

// ================================================================================================
// function: HasDeleted
// purpose:
// ================================================================================================
Standard_Boolean QANewModTopOpe_Limitation::HasDeleted() const
{
  Check();
  if(myMode == QANewModTopOpe_Forward) {
    return myCut->HasDeleted();
  }
  else if(myMode == QANewModTopOpe_Reversed) {
    return myCommon->HasDeleted();
  }
  else {
    return myCut->HasDeleted() || myCommon->HasDeleted();
  }
}

Standard_Boolean QANewModTopOpe_Limitation::IsDeleted(const TopoDS_Shape& S) 
{

  Check();
  if(myMode == QANewModTopOpe_Forward) {
    return myCut->IsDeleted(S);
  }
  else if(myMode == QANewModTopOpe_Reversed) {
    return myCommon->IsDeleted(S);
  }
  else {
    return myCut->IsDeleted(S) && myCommon->IsDeleted(S);
  }

}

void QANewModTopOpe_Limitation::Delete()
{
  delete myCut;
  delete myCommon;
}
