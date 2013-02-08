// Created by: Peter KURNEV
// Copyright (c) 2010-2012 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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


#include <BOPAlgo_PaveFiller.ixx>

#include <NCollection_BaseAllocator.hxx>

#include <BOPInt_Context.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_Iterator.hxx>

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
  BOPAlgo_PaveFiller::BOPAlgo_PaveFiller()
:
  BOPAlgo_Algo()
{
  myDS=NULL;
  myIterator=NULL;
}
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
  BOPAlgo_PaveFiller::BOPAlgo_PaveFiller(const Handle(NCollection_BaseAllocator)& theAllocator)
:
  BOPAlgo_Algo(theAllocator)
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
  myErrorStatus=2;
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
  Handle(BOPInt_Context) BOPAlgo_PaveFiller::Context()
{
  return myContext;
}
//=======================================================================
//function : SectionAttribute
//purpose  : 
//=======================================================================
  void  BOPAlgo_PaveFiller::SetSectionAttribute(const BOPAlgo_SectionAttribute& theSecAttr)
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
  myDS->Init();
  //
  // 2.myIterator 
  myIterator=new BOPDS_Iterator(myAllocator);
  myIterator->SetDS(myDS);
  myIterator->Prepare();
  //
  // 3 myContext
  myContext=new BOPInt_Context;
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
  //
  Init();
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
}
