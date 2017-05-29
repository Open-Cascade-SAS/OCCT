// Created by: Peter KURNEV
// Copyright (c) 2014 OPEN CASCADE SAS
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


#include <BOPAlgo_Builder.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BRepAlgoAPI_BuilderAlgo.hxx>
#include <TopoDS_Shape.hxx>

//=======================================================================
// function: 
// purpose: 
//=======================================================================
BRepAlgoAPI_BuilderAlgo::BRepAlgoAPI_BuilderAlgo()
:
  BRepAlgoAPI_Algo(),
  myEntryType(1),
  myDSFiller(NULL),
  myBuilder(NULL),
  myNonDestructive(Standard_False),
  myGlue(BOPAlgo_GlueOff)
{}
//=======================================================================
// function: 
// purpose: 
//=======================================================================
BRepAlgoAPI_BuilderAlgo::BRepAlgoAPI_BuilderAlgo
  (const BOPAlgo_PaveFiller& aPF)
:
  BRepAlgoAPI_Algo(),
  myEntryType(0),
  myBuilder(NULL),
  myNonDestructive(Standard_False),
  myGlue(BOPAlgo_GlueOff)
{
  BOPAlgo_PaveFiller* pPF=(BOPAlgo_PaveFiller*)&aPF;
  myDSFiller=pPF;
}
//=======================================================================
// function: ~
// purpose: 
//=======================================================================
BRepAlgoAPI_BuilderAlgo::~BRepAlgoAPI_BuilderAlgo()
{
  Clear();
}
//=======================================================================
//function : SetNonDestructive
//purpose  : 
//=======================================================================
void BRepAlgoAPI_BuilderAlgo::SetNonDestructive(const Standard_Boolean theFlag)
{
  myNonDestructive = theFlag;
}
//=======================================================================
//function : NonDestructive
//purpose  : 
//=======================================================================
Standard_Boolean BRepAlgoAPI_BuilderAlgo::NonDestructive() const
{
  return myNonDestructive;
}
//=======================================================================
//function : SetGlue
//purpose  : 
//=======================================================================
void BRepAlgoAPI_BuilderAlgo::SetGlue(const BOPAlgo_GlueEnum theGlue)
{
  myGlue=theGlue;
}
//=======================================================================
//function : Glue
//purpose  : 
//=======================================================================
BOPAlgo_GlueEnum BRepAlgoAPI_BuilderAlgo::Glue() const 
{
  return myGlue;
}
//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void BRepAlgoAPI_BuilderAlgo::Clear()
{
  BRepAlgoAPI_Algo::Clear();
  if (myDSFiller && myEntryType) {
    delete myDSFiller;
    myDSFiller=NULL;
  }
  if (myBuilder) {
    delete myBuilder;
    myBuilder=NULL;
  }
}
//=======================================================================
//function : SetArguments
//purpose  : 
//=======================================================================
void BRepAlgoAPI_BuilderAlgo::SetArguments
  (const TopTools_ListOfShape& theLS)
{
  myArguments=theLS;
}
//=======================================================================
//function : Arguments
//purpose  : 
//=======================================================================
const TopTools_ListOfShape& BRepAlgoAPI_BuilderAlgo::Arguments()const
{
  return myArguments;
}
//=======================================================================
//function : Build
//purpose  : 
//=======================================================================
void BRepAlgoAPI_BuilderAlgo::Build()
{
  NotDone();
  //
  Clear();
  //
  if (myEntryType) {
    if (myDSFiller) {
      delete myDSFiller;
    }
    myDSFiller=new BOPAlgo_PaveFiller(myAllocator);
    //
    myDSFiller->SetArguments(myArguments);
    //
    myDSFiller->SetRunParallel(myRunParallel);
    myDSFiller->SetProgressIndicator(myProgressIndicator);
    myDSFiller->SetFuzzyValue(myFuzzyValue);
    myDSFiller->SetNonDestructive(myNonDestructive);
    myDSFiller->SetGlue(myGlue);
    //
    myDSFiller->Perform();
    //
    GetReport()->Merge (myDSFiller->GetReport());
    if (HasErrors())
    {
      return;
    }
  }// if (myEntryType) {
  // 
  if (myBuilder) {
    delete myBuilder;
    myBuilder = NULL;
  }
  myBuilder=new BOPAlgo_Builder(myAllocator);
  //
  myBuilder->SetArguments(myArguments);
  //
  myBuilder->SetRunParallel(myRunParallel);
  myBuilder->SetProgressIndicator(myProgressIndicator);
  //
  myBuilder->PerformWithFiller(*myDSFiller);
  //
  GetReport()->Merge (myBuilder->GetReport());
  //
  Done();
  myShape=myBuilder->Shape();
}
//=======================================================================
//function : Generated
//purpose  : 
//=======================================================================
const TopTools_ListOfShape& BRepAlgoAPI_BuilderAlgo::Generated
  (const TopoDS_Shape& aS) 
{
  if (myBuilder==NULL) {
    myGenerated.Clear();
    return myGenerated;
  }
  myGenerated = myBuilder->Generated(aS);
  return myGenerated;
}
//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================
const TopTools_ListOfShape& BRepAlgoAPI_BuilderAlgo::Modified
  (const TopoDS_Shape& aS) 
{
  if (myBuilder==NULL) {
    myGenerated.Clear();
    return myGenerated;
  }
  myGenerated = myBuilder->Modified(aS);
  return myGenerated;
}
//=======================================================================
//function : IsDeleted
//purpose  : 
//=======================================================================
Standard_Boolean BRepAlgoAPI_BuilderAlgo::IsDeleted
  (const TopoDS_Shape& aS) 
{
  Standard_Boolean bDeleted = Standard_True; 
  if (myBuilder != NULL) {
    bDeleted=myBuilder->IsDeleted(aS);
  }
  return bDeleted; 
}
//=======================================================================
//function : HasModified
//purpose  : 
//=======================================================================
Standard_Boolean BRepAlgoAPI_BuilderAlgo::HasModified() const
{
  if (myBuilder==NULL) {
    return Standard_False;
  }
  return myBuilder->HasModified();
}
//=======================================================================
//function : HasGenerated
//purpose  : 
//=======================================================================
Standard_Boolean BRepAlgoAPI_BuilderAlgo::HasGenerated() const
{
  if (myBuilder==NULL) {
    return Standard_False;
  }
  return myBuilder->HasGenerated();
}
//=======================================================================
//function : HasDeleted
//purpose  : 
//=======================================================================
Standard_Boolean BRepAlgoAPI_BuilderAlgo::HasDeleted() const
{
  if (myBuilder==NULL) {
    return Standard_False;
  }
  return myBuilder->HasDeleted();
}
