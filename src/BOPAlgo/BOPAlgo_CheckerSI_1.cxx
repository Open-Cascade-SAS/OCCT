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
//
#include <BOPAlgo_CheckerSI.ixx>

#include <BOPDS_DS.hxx>
#include <BOPDS_IteratorSI.hxx>
#include <BOPInt_Context.hxx>

#include <BOPDS_Interf.hxx>
#include <TopoDS_Solid.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <TopoDS_Vertex.hxx>
#include <gp_Pnt.hxx>
#include <TopAbs_State.hxx>
#include <BRep_Tool.hxx>
#include <BOPDS_VectorOfInterfVZ.hxx>


//=======================================================================
//function : PerformVZ
//purpose  : 
//=======================================================================
void BOPAlgo_CheckerSI::PerformVZ()
{
  Standard_Boolean bJustAdd;
  Standard_Integer iSize, nV, nZ, i;
  Standard_Real aTol;
  gp_Pnt aPV;
  TopAbs_State aState;
  //
  myErrorStatus=0;
  //
  myIterator->Initialize(TopAbs_VERTEX, TopAbs_SOLID);
  iSize=myIterator->ExpectedLength();
  if (!iSize) {
    return; 
  }
  //
  BOPDS_VectorOfInterfVZ& aVZs=myDS->InterfVZ();
  aVZs.SetStartSize(iSize);
  aVZs.SetIncrement(iSize);
  aVZs.Init();
  //
  for (; myIterator->More(); myIterator->Next()) {
    myIterator->Value(nV, nZ, bJustAdd);
    if(bJustAdd) {
      continue;
    }
    //
    const BOPDS_ShapeInfo& aSIV=myDS->ShapeInfo(nV);
    const BOPDS_ShapeInfo& aSIZ=myDS->ShapeInfo(nZ);
    //
    const TopoDS_Vertex& aV=*((TopoDS_Vertex*)&aSIV.Shape()); 
    const TopoDS_Solid& aZ=*((TopoDS_Solid*)&aSIZ.Shape()); 
    //
    BRepClass3d_SolidClassifier& aSC=myContext->SolidClassifier(aZ);
    //
    aPV=BRep_Tool::Pnt(aV);
    aTol=BRep_Tool::Tolerance(aV);
    //
    aSC.Perform(aPV, aTol);
    //
    aState=aSC.State();
    if (aState==TopAbs_IN)  {
      i=aVZs.Append()-1;
      BOPDS_InterfVZ& aVZ=aVZs(i);
      aVZ.SetIndices(nV, nZ);
      //
      myDS->AddInterf(nV, nZ);
    }
  }
  //
}
//=======================================================================
//function : PerformEZ
//purpose  : 
//=======================================================================
void BOPAlgo_CheckerSI::PerformEZ()
{
  Standard_Boolean bJustAdd, bHasInterf;
  Standard_Integer iSize, nE, nZ, i;
  //
  myErrorStatus=0;
  //
  myIterator->Initialize(TopAbs_EDGE, TopAbs_SOLID);
  iSize=myIterator->ExpectedLength();
  if (!iSize) {
    return; 
  }
  //
  BOPDS_VectorOfInterfEZ& aEZs=myDS->InterfEZ();
  aEZs.SetStartSize(iSize);
  aEZs.SetIncrement(iSize);
  aEZs.Init();
  //
  for (; myIterator->More(); myIterator->Next()) {
    myIterator->Value(nE, nZ, bJustAdd);
    if(bJustAdd) {
      continue;
    }
    //
    bHasInterf=myDS->HasInterfShapeSubShapes(nE, nZ);
    if (bHasInterf) {
      continue;
    }
    //
    bHasInterf=myDS->HasInterfShapeSubShapes(nZ, nE);
    if (!bHasInterf) {
      continue;
    }
    //
    i=aEZs.Append()-1;
    BOPDS_InterfEZ& aEZ=aEZs(i);
    aEZ.SetIndices(nE, nZ);
    //
    myDS->AddInterf(nE, nZ);
  }
}
//=======================================================================
//function : PerformFZ
//purpose  : 
//=======================================================================
void BOPAlgo_CheckerSI::PerformFZ()
{
  Standard_Boolean bJustAdd, bHasInterf;
  Standard_Integer iSize, nF, nZ, i;
  //
  myErrorStatus=0;
  //
  myIterator->Initialize(TopAbs_FACE, TopAbs_SOLID);
  iSize=myIterator->ExpectedLength();
  if (!iSize) {
    return; 
  }
  //
  BOPDS_VectorOfInterfFZ& aFZs=myDS->InterfFZ();
  aFZs.SetStartSize(iSize);
  aFZs.SetIncrement(iSize);
  aFZs.Init();
  //
  for (; myIterator->More(); myIterator->Next()) {
    myIterator->Value(nF, nZ, bJustAdd);
    if(bJustAdd) {
      continue;
    }
    //
    bHasInterf=myDS->HasInterfShapeSubShapes(nF, nZ);
    if (bHasInterf) {
      continue;
    }
    //
    bHasInterf=myDS->HasInterfShapeSubShapes(nZ, nF);
    if (!bHasInterf) {
      continue;
    }
    //
    i=aFZs.Append()-1;
    BOPDS_InterfFZ& aFZ=aFZs(i);
    aFZ.SetIndices(nF, nZ);
    //
    myDS->AddInterf(nF, nZ);
  }
}
//=======================================================================
//function : PerformZZ
//purpose  : 
//=======================================================================
void BOPAlgo_CheckerSI::PerformZZ()
{
  Standard_Boolean bJustAdd, bHasInterf, bFlag;
  Standard_Integer iSize, nZ1, nZ, i;
  //
  myErrorStatus=0;
  //
  myIterator->Initialize(TopAbs_SOLID, TopAbs_SOLID);
  iSize=myIterator->ExpectedLength();
  if (!iSize) {
    return; 
  }
  //
  BOPDS_VectorOfInterfZZ& aZZs=myDS->InterfZZ();
  aZZs.SetStartSize(iSize);
  aZZs.SetIncrement(iSize);
  aZZs.Init();
  //
  bFlag=Standard_False;
  //
  for (; myIterator->More(); myIterator->Next()) {
    myIterator->Value(nZ1, nZ, bJustAdd);
    if(bJustAdd) {
      continue;
    }
    //
    bHasInterf=myDS->HasInterfShapeSubShapes(nZ, nZ1, bFlag);
    if (!bHasInterf) {
      bHasInterf=myDS->HasInterfShapeSubShapes(nZ1, nZ, bFlag);
    }
    if (!bHasInterf) {
      continue; 
    }
    //
    i=aZZs.Append()-1;
    BOPDS_InterfZZ& aZZ=aZZs(i);
    aZZ.SetIndices(nZ1, nZ);
    //
    myDS->AddInterf(nZ1, nZ);
  }
}
