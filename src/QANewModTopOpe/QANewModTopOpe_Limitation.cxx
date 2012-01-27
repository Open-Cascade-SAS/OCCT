// File:	QANewModTopOpe_Limitation.cxx
// Created:	Sat May  6 14:24:19 2001
// Author:	Igor FEOKTISTOV <ifv@nnov.matra-dtv.fr>
// Copyright:	 SAMTECH S.A. 2000

// Lastly modified by :
// +---------------------------------------------------------------------------+
// !       ifv ! Creation                                ! 6-05-2001! 3.0-00-3!
// !       skv ! Adaptation to OCC version 5.0           ! 6-05-2003! 3.0-00-2!
// +---------------------------------------------------------------------------+


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

// @@SDM: begin

// Copyright SAMTECH ..........................................Version    3.0-00
// Lastly modified by : skv                                    Date :  6-05-2003

// File history synopsis (creation,modification,correction)
// +---------------------------------------------------------------------------+
// ! Developer !              Comments                   !   Date   ! Version  !
// +-----------!-----------------------------------------!----------!----------+
// !       ifv ! Creation                                ! 6-05-2001! 3.0-00-3!
// !       skv ! Adaptation to OCC version 5.0           ! 6-05-2003! 3.0-00-2!
// !  vladimir ! adaptation to CAS 5.0                   !  07/01/03!    4.0-2!
// +---------------------------------------------------------------------------+
//
// @@SDM: end
