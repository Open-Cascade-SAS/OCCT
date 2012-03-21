// Created on: 2001-05-06
// Created by: Igor FEOKTISTOV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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

#include <QANewModTopOpe_Limitation.ixx>

#include <BRep_Builder.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopAbs.hxx>
#include <TopTools_ListOfShape.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <Standard_ConstructionError.hxx>
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
       myFwdIsDone(Standard_False),
       myRevIsDone(Standard_False),
       myMode(theMode),
       myCut(NULL),
       myCommon(NULL)
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
// function: Modified2
// purpose:
// ================================================================================================
const TopTools_ListOfShape& QANewModTopOpe_Limitation::Modified2(const TopoDS_Shape& aS) 
{
  Check();
  myGenerated.Clear();
  if(myMode == QANewModTopOpe_Forward) {
    myGenerated = myCut->Modified2(aS);
  }
  else if(myMode == QANewModTopOpe_Reversed) {
    myGenerated = myCommon->Modified2(aS);
  }
  else {
    myGenerated = myCut->Modified2(aS);

    TopTools_MapOfShape aMap; // to check if shape can be added in list more then one time
    TopTools_ListIteratorOfListOfShape It(myGenerated);
    for(;It.More();It.Next()) {
      aMap.Add(It.Value());
    }
    
    It.Initialize(myCommon->Modified2(aS));
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
