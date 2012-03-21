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

#include <BRepAlgoAPI_Section.ixx>


#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeShell.hxx>
#include <Geom_Plane.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <BOP_Section.hxx>
#include <BOPTools_SSIntersectionAttribute.hxx>

#include <BOPTools_SplitShapesPool.hxx>
#include <BOPTools_InterferencePool.hxx>

#include <BOPTools_PaveBlock.hxx>
#include <BOPTools_PaveFiller.hxx>
#include <BooleanOperations_ShapesDataStructure.hxx>
#include <BOPTools_ListOfPaveBlock.hxx>
#include <BOPTools_ListIteratorOfListOfPaveBlock.hxx>
#include <BOPTools_SSInterference.hxx>
#include <BOPTools_Curve.hxx>
#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <BRep_Tool.hxx>

#include <BOP_SectionHistoryCollector.hxx>

static TopoDS_Shape MakeShape(const Handle(Geom_Surface)& S)
{
  GeomAbs_Shape c = S->Continuity();
  if (c >= GeomAbs_C2) return BRepBuilderAPI_MakeFace(S, Precision::Confusion());
  else return BRepBuilderAPI_MakeShell(S);
}

static Standard_Boolean HasAncestorFaces(const BOPTools_DSFiller& theDSFiller, 
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
: BRepAlgoAPI_BooleanOperation(Sh1, Sh2, BOP_SECTION)
{
  InitParameters();
  myparameterschanged = Standard_True;

  if(myS1.IsNull() || myS2.IsNull()) {
    //     StdFail_NotDone::Raise("BRepAlgoAPI_Section : Shape NULL");
    myshapeisnull = Standard_True;
  }
  if (PerformNow)
    Build();
}

BRepAlgoAPI_Section::BRepAlgoAPI_Section(const TopoDS_Shape&      aS1, 
					 const TopoDS_Shape&      aS2,
					 const BOPTools_DSFiller& aDSF,
					 const Standard_Boolean   PerformNow)
: BRepAlgoAPI_BooleanOperation(aS1, aS2, aDSF, BOP_SECTION)
{
  InitParameters();
  myparameterschanged = Standard_True;

  if(myS1.IsNull() || myS2.IsNull()) {
    //     StdFail_NotDone::Raise("BRepAlgoAPI_Section : Shape NULL");
    myshapeisnull = Standard_True;
  }

  if(PerformNow) {
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
: BRepAlgoAPI_BooleanOperation(Sh, MakeShape(new Geom_Plane(Pl)), BOP_SECTION)
{
  InitParameters();
  myparameterschanged = Standard_True;

  if(Sh.IsNull() || myS2.IsNull()) {
    //     StdFail_NotDone::Raise("BRepAlgoAPI_Section : Shape NULL");
    myshapeisnull = Standard_True;
  }
  if (PerformNow)
    Build();
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepAlgoAPI_Section::BRepAlgoAPI_Section(const TopoDS_Shape&         Sh,
					       const Handle(Geom_Surface)& Sf,
					       const Standard_Boolean      PerformNow)
: BRepAlgoAPI_BooleanOperation(Sh, MakeShape(Sf), BOP_SECTION)
{
  InitParameters();
  myparameterschanged = Standard_True;

  if(Sh.IsNull() || myS2.IsNull()) {
//     StdFail_NotDone::Raise("BRepAlgoAPI_Section : Shape NULL");
    myshapeisnull = Standard_True;
  }
  if (PerformNow)
    Build();
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepAlgoAPI_Section::BRepAlgoAPI_Section(const Handle(Geom_Surface)& Sf,
					       const TopoDS_Shape&         Sh,
					       const Standard_Boolean      PerformNow)
: BRepAlgoAPI_BooleanOperation(MakeShape(Sf), Sh, BOP_SECTION)
{
  InitParameters();
  myparameterschanged = Standard_True;

  if(myS1.IsNull() || Sh.IsNull()) {
//     StdFail_NotDone::Raise("BRepAlgoAPI_Section : Shape NULL");
    myshapeisnull = Standard_True;
  }
  if (PerformNow)
    Build();
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepAlgoAPI_Section::BRepAlgoAPI_Section(const Handle(Geom_Surface)& Sf1,
					       const Handle(Geom_Surface)& Sf2,
					       const Standard_Boolean      PerformNow)
: BRepAlgoAPI_BooleanOperation(MakeShape(Sf1), MakeShape(Sf2), BOP_SECTION)
{
  InitParameters();
  myparameterschanged = Standard_True;

  if(myS1.IsNull() || myS2.IsNull()) {
//     StdFail_NotDone::Raise("BRepAlgoAPI_Section : Shape NULL");
    myshapeisnull = Standard_True;
  }

  if (PerformNow)
    Build();
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

    myBuilderCanWork = Standard_False;

    Standard_Boolean bIsNewFiller = PrepareFiller();
    //
    if (myErrorStatus!=1) {
      NotDone();
      // there were errors during the preparation 
      return;
    }
    //
    if (bIsNewFiller) {
      //Prepare the DS
      BOPTools_SSIntersectionAttribute aSectionAttribute(myApprox, myComputePCurve1, myComputePCurve2);
      myDSFiller->Perform(aSectionAttribute);
    }
    BOP_Section* aSectionAlgo = new BOP_Section();
    aSectionAlgo->SetShapes(myS1, myS2);

    myHistory = new BOP_SectionHistoryCollector(myS1, myS2);
    aSectionAlgo->SetHistoryCollector(myHistory);

    aSectionAlgo->DoWithFiller(*myDSFiller);

    myBuilder = aSectionAlgo;

    if(aSectionAlgo->IsDone()) {
      myErrorStatus=0;
      myBuilderCanWork=Standard_True;
      myShape = aSectionAlgo->Result();
      Done();
    }
    else {
      myErrorStatus = 100 + aSectionAlgo->ErrorStatus();
      NotDone();
    }    
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
  aResult = HasAncestorFaces(*myDSFiller, E, F1, F2);

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
  aResult = HasAncestorFaces(*myDSFiller, E, F1, F2);

  if(F2.IsNull()) {
    return Standard_False;
  }
  F = F2;
  return aResult;
}

//=======================================================================
//function : PCurveOn1
//purpose  : 
//=======================================================================
Handle(Geom2d_Curve) BRepAlgoAPI_Section::PCurveOn1(const TopoDS_Shape& E) const
{
  Handle(Geom2d_Curve) aResult;
  
  if(myComputePCurve1) {
    TopoDS_Shape aShape;

    if(HasAncestorFaceOn1(E, aShape)) {
      const TopoDS_Edge& anEdge = TopoDS::Edge(E);
      const TopoDS_Face& aFace  = TopoDS::Face(aShape);
      Standard_Real f, l;
      aResult = BRep_Tool::CurveOnSurface(anEdge, aFace, f, l);

      if(!aResult->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve))) {
	aResult = new Geom2d_TrimmedCurve(aResult, f, l);
      }
    }
  }
  return aResult;
}

//=======================================================================
//function : PCurveOn2
//purpose  : 
//=======================================================================
Handle(Geom2d_Curve) BRepAlgoAPI_Section::PCurveOn2(const TopoDS_Shape& E) const
{
  Handle(Geom2d_Curve) aResult;
  
  if(myComputePCurve2) {
    TopoDS_Shape aShape;

    if(HasAncestorFaceOn2(E, aShape)) {
      const TopoDS_Edge& anEdge = TopoDS::Edge(E);
      const TopoDS_Face& aFace  = TopoDS::Face(aShape);
      Standard_Real f, l;
      aResult = BRep_Tool::CurveOnSurface(anEdge, aFace, f, l);
      
      if(!aResult->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve))) {
	aResult = new Geom2d_TrimmedCurve(aResult, f, l);
      }
    }
  }
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
static Standard_Boolean HasAncestorFaces(const BOPTools_DSFiller& theDSFiller, 
					 const TopoDS_Shape&      E,
					 TopoDS_Shape&            F1,
					 TopoDS_Shape&            F2) {

  BOPTools_PaveFiller* aPaveFiller = (BOPTools_PaveFiller*) &theDSFiller.PaveFiller();
  const BOPTools_CArray1OfSSInterference& aFFs = aPaveFiller->InterfPool()->SSInterferences();
  const BooleanOperations_ShapesDataStructure& aDS = theDSFiller.DS();
  Standard_Integer aNb = aFFs.Extent();
  Standard_Integer i = 0;

  for (i = 1; i <= aNb; i++) {
    BOPTools_SSInterference* aFFi = (BOPTools_SSInterference*) &aFFs(i);
    const BOPTools_SequenceOfCurves& aSeqOfCurve = aFFi->Curves();
    
    for(Standard_Integer j = 1; j <= aSeqOfCurve.Length(); j++) {
      const BOPTools_Curve& aCurve = aSeqOfCurve.Value(j);
      BOPTools_ListIteratorOfListOfPaveBlock anIt(aCurve.NewPaveBlocks());

      for(; anIt.More(); anIt.Next()) {
	const BOPTools_PaveBlock& aPB = anIt.Value();
	Standard_Integer anIndex = aPB.Edge();

	if(anIndex <= 0)
	  continue;

	if(E.IsSame(aDS.Shape(anIndex))) {
	  for(Standard_Integer fIt = 0; fIt < 2; fIt++) {
	    anIndex = (fIt == 0) ? aFFi->Index1() : aFFi->Index2();
	    
	    if(anIndex > 0) {
	      if (fIt == 0)
		F1 = aDS.Shape(anIndex);
	      else
		F2 = aDS.Shape(anIndex);
	    }
	  }
	  return Standard_True;
	}
      }
    }

    BOPTools_ListIteratorOfListOfPaveBlock anIt(aFFi->PaveBlocks());
    
    for(; anIt.More(); anIt.Next()) {
      const BOPTools_PaveBlock& aPB = anIt.Value();
      Standard_Integer anIndex = aPB.Edge();

      if(anIndex <= 0)
	continue;
      
      if(E.IsSame(aDS.Shape(anIndex))) {
	for(Standard_Integer fIt = 0; fIt < 2; fIt++) {
	  anIndex = (fIt == 0) ? aFFi->Index1() : aFFi->Index2();
	  
	  if(anIndex > 0) {
	    if (fIt == 0)
	      F1 = aDS.Shape(anIndex);
	    else
	      F2 = aDS.Shape(anIndex);
	  }
	}
	return Standard_True;
      }
    }
  }
  return Standard_False;
}
