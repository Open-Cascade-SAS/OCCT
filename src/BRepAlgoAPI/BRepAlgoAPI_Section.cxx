// Created on: 1994-02-18
// Created by: Remi LEQUETTE
// Copyright (c) 1994-1999 Matra Datavision
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

// modified by Michael KLOKOV  Wed Mar  6 15:01:25 2002
// modified by  Eugeny MALTCHIKOV Wed Jul 04 11:13:01 2012 

#include <BRepAlgoAPI_Section.ixx>


#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeShell.hxx>
#include <Geom_Plane.hxx>
#include <Geom2d_TrimmedCurve.hxx>

#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <BRep_Tool.hxx>

#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_BOP.hxx>
#include <BOPDS_DS.hxx>

static TopoDS_Shape MakeShape(const Handle(Geom_Surface)& S)
{
  GeomAbs_Shape c = S->Continuity();
  if (c >= GeomAbs_C2) return BRepBuilderAPI_MakeFace(S, Precision::Confusion());
  else return BRepBuilderAPI_MakeShell(S);
}

static Standard_Boolean HasAncestorFaces(const BOPAlgo_PPaveFiller& theDSFiller, 
                                         const TopoDS_Shape&      E,
                                         TopoDS_Shape&            F1,
                                         TopoDS_Shape&            F2);

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepAlgoAPI_Section::BRepAlgoAPI_Section(const TopoDS_Shape& Sh1,
                                         const TopoDS_Shape& Sh2,
                                         const Standard_Boolean PerformNow)
: BRepAlgoAPI_BooleanOperation(Sh1, Sh2, BOPAlgo_SECTION)
{
  InitParameters();
  myparameterschanged = Standard_True;

  if(myS1.IsNull() || myS2.IsNull()) {
    //     StdFail_NotDone::Raise("BRepAlgoAPI_Section : Shape NULL");
    myshapeisnull = Standard_True;
  }
  if (PerformNow) {
    Build();
  }
}

BRepAlgoAPI_Section::BRepAlgoAPI_Section(const TopoDS_Shape&      aS1, 
                                         const TopoDS_Shape&      aS2,
                                         const BOPAlgo_PaveFiller& aDSF,
                                         const Standard_Boolean   PerformNow)
: BRepAlgoAPI_BooleanOperation(aS1, aS2, aDSF, BOPAlgo_SECTION)
{
  InitParameters();
  myparameterschanged = Standard_True;

  if(myS1.IsNull() || myS2.IsNull()) {
    //     StdFail_NotDone::Raise("BRepAlgoAPI_Section : Shape NULL");
    myshapeisnull = Standard_True;
  }

  if (PerformNow) {
    Build();
  }
}


//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepAlgoAPI_Section::BRepAlgoAPI_Section(const TopoDS_Shape&    Sh,
                                         const gp_Pln&          Pl,
                                         const Standard_Boolean PerformNow)
: BRepAlgoAPI_BooleanOperation(Sh, MakeShape(new Geom_Plane(Pl)), BOPAlgo_SECTION)
{
  InitParameters();
  myparameterschanged = Standard_True;

  if(Sh.IsNull() || myS2.IsNull()) {
    //     StdFail_NotDone::Raise("BRepAlgoAPI_Section : Shape NULL");
    myshapeisnull = Standard_True;
  }
  if (PerformNow) {
    Build();
  }
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepAlgoAPI_Section::BRepAlgoAPI_Section(const TopoDS_Shape&         Sh,
                                               const Handle(Geom_Surface)& Sf,
                                               const Standard_Boolean      PerformNow)
: BRepAlgoAPI_BooleanOperation(Sh, MakeShape(Sf), BOPAlgo_SECTION)
{
  InitParameters();
  myparameterschanged = Standard_True;

  if(Sh.IsNull() || myS2.IsNull()) {
//     StdFail_NotDone::Raise("BRepAlgoAPI_Section : Shape NULL");
    myshapeisnull = Standard_True;
  }
  if (PerformNow) {
    Build();
  }
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepAlgoAPI_Section::BRepAlgoAPI_Section(const Handle(Geom_Surface)& Sf,
                                               const TopoDS_Shape&         Sh,
                                               const Standard_Boolean      PerformNow)
: BRepAlgoAPI_BooleanOperation(MakeShape(Sf), Sh, BOPAlgo_SECTION)
{
  InitParameters();
  myparameterschanged = Standard_True;

  if(myS1.IsNull() || Sh.IsNull()) {
//     StdFail_NotDone::Raise("BRepAlgoAPI_Section : Shape NULL");
    myshapeisnull = Standard_True;
  }
  if (PerformNow) {
    Build();
  }
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepAlgoAPI_Section::BRepAlgoAPI_Section(const Handle(Geom_Surface)& Sf1,
                                               const Handle(Geom_Surface)& Sf2,
                                               const Standard_Boolean      PerformNow)
: BRepAlgoAPI_BooleanOperation(MakeShape(Sf1), MakeShape(Sf2), BOPAlgo_SECTION)
{
  InitParameters();
  myparameterschanged = Standard_True;

  if(myS1.IsNull() || myS2.IsNull()) {
//     StdFail_NotDone::Raise("BRepAlgoAPI_Section : Shape NULL");
    myshapeisnull = Standard_True;
  }

  if (PerformNow) {
    Build();
  }
}

//=======================================================================
//function : Init1
//purpose  : 
//=======================================================================
void BRepAlgoAPI_Section::Init1(const TopoDS_Shape& S1) 
{
  if(!S1.IsNull()) {
    if (!S1.IsEqual(myS1)) {
      myS1 = S1;

      if(!myS2.IsNull()) {
        myshapeisnull = Standard_False;
      }
      myparameterschanged = Standard_True;
    }
  }
  else {
    if(!myS1.IsNull()) {
      myS1 = S1;
      myshapeisnull = Standard_True;
      myparameterschanged = Standard_True;
    }
  }

  if(myparameterschanged)
    NotDone();
}

//=======================================================================
//function : Init1
//purpose  : 
//=======================================================================
void BRepAlgoAPI_Section::Init1(const gp_Pln& Pl) 
{
  Init1(MakeShape(new Geom_Plane(Pl)));
}

//=======================================================================
//function : Init1
//purpose  : 
//=======================================================================
void BRepAlgoAPI_Section::Init1(const Handle(Geom_Surface)& Sf) 
{
  Init1(MakeShape(Sf));
}

//=======================================================================
//function : Init2
//purpose  : 
//=======================================================================
void BRepAlgoAPI_Section::Init2(const TopoDS_Shape& S2) 
{
  if(!S2.IsNull()) {
    if (!S2.IsEqual(myS2)) {
      myS2 = S2;

      if(!myS1.IsNull()) {
        myshapeisnull = Standard_False;
      }
      myparameterschanged = Standard_True;
    }
  }
  else {
    if(!myS2.IsNull()) {
      myS2 = S2;
      myshapeisnull = Standard_True;
      myparameterschanged = Standard_True;
    }
  }

  if(myparameterschanged) {
    NotDone();
  }
}

//=======================================================================
//function : Init2
//purpose  : 
//=======================================================================
void BRepAlgoAPI_Section::Init2(const gp_Pln& Pl) 
{
  Init2(MakeShape(new Geom_Plane(Pl)));
}

//=======================================================================
//function : Init2
//purpose  : 
//=======================================================================
void BRepAlgoAPI_Section::Init2(const Handle(Geom_Surface)& Sf) 
{
  Init2(MakeShape(Sf));
}

//=======================================================================
//function : Approximation
//purpose  : 
//=======================================================================
void BRepAlgoAPI_Section::Approximation(const Standard_Boolean B) 
{
  if(myApprox != B) {
    myApprox = B;
    myparameterschanged = Standard_True;
  }
}

//=======================================================================
//function : ComputePCurveOn1
//purpose  : 
//=======================================================================
void BRepAlgoAPI_Section::ComputePCurveOn1(const Standard_Boolean B) 
{
  if(myComputePCurve1 != B) {
    myComputePCurve1 = B;
    myparameterschanged = Standard_True;
  }
}

//=======================================================================
//function : ComputePCurveOn2
//purpose  : 
//=======================================================================
void BRepAlgoAPI_Section::ComputePCurveOn2(const Standard_Boolean B) 
{
  if(myComputePCurve2 != B) {
    myComputePCurve2 = B;
    myparameterschanged = Standard_True;
  }
}

//=======================================================================
//function : Build
//purpose  : 
//=======================================================================
void BRepAlgoAPI_Section::Build() 
{
  if(myshapeisnull) {
    myErrorStatus = 2;
    NotDone();
    return;
  }

  if(myparameterschanged) {
    Standard_Boolean bIsNewFiller = PrepareFiller();
    //
    if (myErrorStatus!=1) {
      NotDone();
      // there were errors during the preparation 
      return;
    }
    //
    if (bIsNewFiller) {
      BOPAlgo_SectionAttribute theSecAttr(myApprox, myComputePCurve1, myComputePCurve2);
      myDSFiller->SetSectionAttribute(theSecAttr);
      myDSFiller->Perform();
    }
    //
    BRepAlgoAPI_BooleanOperation::Build();
    //
    myparameterschanged = Standard_False;
  }
}

//=======================================================================
//function : HasAncestorFaceOn1
//purpose  : 
//=======================================================================
Standard_Boolean BRepAlgoAPI_Section::HasAncestorFaceOn1(const TopoDS_Shape& E, TopoDS_Shape& F) const
{
  Standard_Boolean aResult = Standard_False;
  if(E.IsNull()) {
    return aResult;
  }

  if(E.ShapeType() != TopAbs_EDGE) {
    return aResult;
  }
  TopoDS_Shape F1, F2;
  aResult = HasAncestorFaces(myDSFiller, E, F1, F2);

  if(F1.IsNull()) {
    return Standard_False;
  }
  F = F1;
  return aResult;
}

//=======================================================================
//function : HasAncestorFaceOn2
//purpose  : 
//=======================================================================
Standard_Boolean BRepAlgoAPI_Section::HasAncestorFaceOn2(const TopoDS_Shape& E,TopoDS_Shape& F) const
{
  Standard_Boolean aResult = Standard_False;
  if(E.IsNull()) {
    return aResult;
  }

  if(E.ShapeType() != TopAbs_EDGE) {
    return aResult;
  }
  TopoDS_Shape F1, F2;
  aResult = HasAncestorFaces(myDSFiller, E, F1, F2);

  if(F2.IsNull()) {
    return Standard_False;
  }
  F = F2;
  return aResult;
}

//=======================================================================
//function : InitParameters
//purpose  : 
//=======================================================================
void BRepAlgoAPI_Section::InitParameters() 
{
  myparameterschanged = Standard_False;
  myshapeisnull = Standard_False;
  myApprox = Standard_False;
  myComputePCurve1 = Standard_False;
  myComputePCurve2 = Standard_False;
}

// ------------------------------------------------------------------------
// static function : HasAncestorFaces
// purpose  : 
// ------------------------------------------------------------------------
static Standard_Boolean HasAncestorFaces(const BOPAlgo_PPaveFiller& theDSFiller, 
                                         const TopoDS_Shape&      E,
                                         TopoDS_Shape&            F1,
                                         TopoDS_Shape&            F2) {

  Standard_Integer aNb, i, j, nE, nF1, nF2, aNbCurves;
  //
  const BOPDS_PDS& pDS = theDSFiller->PDS();
  BOPDS_VectorOfInterfFF& aFFs=pDS->InterfFF();
  //
  aNb=aFFs.Extent();
  //section edges
  for (i = 0; i < aNb; i++) {
    BOPDS_InterfFF& aFFi=aFFs(i);
    aFFi.Indices(nF1, nF2);
    //
    const BOPDS_VectorOfCurve& aSeqOfCurve=aFFi.Curves();
    aNbCurves=aSeqOfCurve.Extent();
    for (j=0; j<aNbCurves; j++) {
      const BOPDS_Curve& aCurve=aSeqOfCurve(j);

      const BOPDS_ListOfPaveBlock& aSectEdges = aCurve.PaveBlocks();

      BOPDS_ListIteratorOfListOfPaveBlock anIt;
      anIt.Initialize(aSectEdges);

      for(; anIt.More(); anIt.Next()) {
        const Handle(BOPDS_PaveBlock)& aPB = anIt.Value();
        nE = aPB->Edge();
        if(nE < 0)  continue;
        //
        if(E.IsSame(pDS->Shape(nE))) {
          F1 = pDS->Shape(nF1);
          F2 = pDS->Shape(nF2);
          return Standard_True;
        }
      }
    }
  }
  return Standard_False;
}
