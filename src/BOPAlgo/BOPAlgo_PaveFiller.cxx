// Created by: Peter KURNEV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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


#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_SectionAttribute.hxx>
#include <BOPDS_Curve.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_Iterator.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <gp_Pnt.hxx>
#include <IntTools_Context.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPAlgo_PaveFiller::BOPAlgo_PaveFiller()
:
  BOPAlgo_Algo(),
  myFuzzyValue(0.)
{
  myDS=NULL;
  myIterator=NULL;
}
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPAlgo_PaveFiller::BOPAlgo_PaveFiller
  (const Handle(NCollection_BaseAllocator)& theAllocator)
:
  BOPAlgo_Algo(theAllocator),
  myFuzzyValue(0.)
{
  myDS=NULL;
  myIterator=NULL;
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
BOPAlgo_PaveFiller::~BOPAlgo_PaveFiller()
{
  Clear();
}
//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::Clear()
{
  if (myIterator) {
    delete myIterator;
    myIterator=NULL;
  }
  if (myDS) {
    delete myDS;
    myDS=NULL;
  }
}
//=======================================================================
//function : DS
//purpose  : 
//=======================================================================
const BOPDS_DS& BOPAlgo_PaveFiller::DS()
{
  return *myDS;
}
//=======================================================================
//function : PDS
//purpose  : 
//=======================================================================
BOPDS_PDS BOPAlgo_PaveFiller::PDS()
{
  return myDS;
}
//=======================================================================
//function : Context
//purpose  : 
//=======================================================================
Handle(IntTools_Context) BOPAlgo_PaveFiller::Context()
{
  return myContext;
}
//=======================================================================
//function : SectionAttribute
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::SetSectionAttribute
  (const BOPAlgo_SectionAttribute& theSecAttr)
{
  mySectionAttribute = theSecAttr;
}
//=======================================================================
//function : SetArguments
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::SetArguments(const BOPCol_ListOfShape& theLS)
{
  myArguments=theLS;
}
//=======================================================================
//function : Arguments
//purpose  : 
//=======================================================================
const BOPCol_ListOfShape& BOPAlgo_PaveFiller::Arguments()const
{
  return myArguments;
}
//=======================================================================
//function : SetFuzzyValue
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::SetFuzzyValue(const Standard_Real theFuzz)
{
  myFuzzyValue = (theFuzz < 0.) ? 0. : theFuzz;
}
//=======================================================================
//function : FuzzyValue
//purpose  : 
//=======================================================================
Standard_Real BOPAlgo_PaveFiller::FuzzyValue() const
{
  return myFuzzyValue;
}
//=======================================================================
// function: Init
// purpose: 
//=======================================================================
void BOPAlgo_PaveFiller::Init()
{
  myErrorStatus=0;
  //
  if (!myArguments.Extent()) {
    myErrorStatus=10;
    return;
  }
  //
  // 0 Clear
  Clear();
  //
  // 1.myDS 
  myDS=new BOPDS_DS(myAllocator);
  myDS->SetArguments(myArguments);
  myDS->SetFuzzyValue(myFuzzyValue);
  myDS->Init();
  //
  // 2.myIterator 
  myIterator=new BOPDS_Iterator(myAllocator);
  myIterator->SetRunParallel(myRunParallel);
  myIterator->SetDS(myDS);
  myIterator->Prepare();
  //
  // 3 myContext
  myContext=new IntTools_Context;
  //
  myErrorStatus=0;
}
//=======================================================================
// function: Perform
// purpose: 
//=======================================================================
void BOPAlgo_PaveFiller::Perform()
{
  myErrorStatus=0;
  try { 
    OCC_CATCH_SIGNALS
    //
    PerformInternal();
  }
  //
  catch (Standard_Failure) {
    myErrorStatus=11;
  } 
  //
  myDS->SetDefaultTolerances();
}
//=======================================================================
// function: PerformInternal
// purpose: 
//=======================================================================
void BOPAlgo_PaveFiller::PerformInternal()
{
  myErrorStatus=0;
  //
  Init();
  if (myErrorStatus) {
    return; 
  }
  //
  Prepare();
  if (myErrorStatus) {
    return; 
  }
  // 00
  PerformVV();
  if (myErrorStatus) {
    return; 
  }
  // 01
  PerformVE();
  if (myErrorStatus) {
    return; 
  }
  //
  myDS->UpdatePaveBlocks();
  // 11
  PerformEE();
  if (myErrorStatus) {
    return; 
  }
  // 02
  PerformVF();
  if (myErrorStatus) {
    return; 
  }
  // 12
  PerformEF();
  if (myErrorStatus) {
    return; 
  }
  //
  MakeSplitEdges();
  if (myErrorStatus) {
    return; 
  }
  //
  // 22
  PerformFF();
  if (myErrorStatus) {
    return; 
  }
  //
  MakeBlocks();
  if (myErrorStatus) {
    return; 
  }
  //
  RefineFaceInfoOn();
  //
  MakePCurves();
  if (myErrorStatus) {
    return; 
  }
  //
  ProcessDE();
  if (myErrorStatus) {
    return; 
  }
  //
  // 03
  PerformVZ();
  if (myErrorStatus) {
    return;
  }
  // 13
  PerformEZ();
  if (myErrorStatus) {
    return;
  }
  // 23
  PerformFZ();
  if (myErrorStatus) {
    return;
  }
  // 33
  PerformZZ();
  if (myErrorStatus) {
    return;
  }
} 
