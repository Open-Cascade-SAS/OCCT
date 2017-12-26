// Created on: 1994-02-18
// Created by: Remi LEQUETTE
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

// modified by Michael KLOKOV  Wed Mar  6 15:01:25 2002
// modified by  Eugeny MALTCHIKOV Wed Jul 04 11:13:01 2012 

#include <BRepAlgoAPI_Section.hxx>

#include <BOPAlgo_PaveFiller.hxx>

#include <BOPDS_DS.hxx>

#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeShell.hxx>

#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>

#include <gp_Pln.hxx>

#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

//
static 
  TopoDS_Shape MakeShape(const Handle(Geom_Surface)& );

static 
  Standard_Boolean HasAncestorFaces(const BOPAlgo_PPaveFiller&, 
                                    const TopoDS_Shape&,
                                    TopoDS_Shape&,
                                    TopoDS_Shape&);
static
  Standard_Boolean HasAncestorFace (const BOPAlgo_PPaveFiller& ,
                                    Standard_Integer ,
                                    const TopoDS_Shape& ,
                                    TopoDS_Shape& ); 
//
//=======================================================================
//function : BRepAlgoAPI_Section
//purpose  : 
//=======================================================================
BRepAlgoAPI_Section::BRepAlgoAPI_Section()
:
  BRepAlgoAPI_BooleanOperation()
{
  Init(Standard_False);
}
//=======================================================================
//function : BRepAlgoAPI_Section
//purpose  : 
//=======================================================================
BRepAlgoAPI_Section::BRepAlgoAPI_Section(const BOPAlgo_PaveFiller& aPF)
:
  BRepAlgoAPI_BooleanOperation(aPF)
{
  Init(Standard_False);
}
//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepAlgoAPI_Section::BRepAlgoAPI_Section(const TopoDS_Shape& Sh1,
                                         const TopoDS_Shape& Sh2,
                                         const Standard_Boolean PerformNow)
:
  BRepAlgoAPI_BooleanOperation(Sh1, 
                               Sh2, 
                               BOPAlgo_SECTION)
{
  Init(PerformNow); 
}
//=======================================================================
//function : BRepAlgoAPI_Section
//purpose  : 
//=======================================================================
BRepAlgoAPI_Section::BRepAlgoAPI_Section
  (const TopoDS_Shape&      aS1, 
   const TopoDS_Shape&      aS2,
   const BOPAlgo_PaveFiller& aDSF,
   const Standard_Boolean   PerformNow)
: 
  BRepAlgoAPI_BooleanOperation(aS1, 
                               aS2, 
                               aDSF,
                               BOPAlgo_SECTION)
{
  Init(PerformNow); 
}
//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepAlgoAPI_Section::BRepAlgoAPI_Section
  (const TopoDS_Shape&    Sh,
   const gp_Pln&          Pl,
   const Standard_Boolean PerformNow)
: 
  BRepAlgoAPI_BooleanOperation(Sh, 
                               MakeShape(new Geom_Plane(Pl)), 
                               BOPAlgo_SECTION)
{
  Init(PerformNow); 
}
//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepAlgoAPI_Section::BRepAlgoAPI_Section
  (const TopoDS_Shape&         Sh,
   const Handle(Geom_Surface)& Sf,
   const Standard_Boolean      PerformNow)
: 
  BRepAlgoAPI_BooleanOperation(Sh, 
                               MakeShape(Sf), 
                               BOPAlgo_SECTION)
{
  Init(PerformNow); 
}
//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepAlgoAPI_Section::BRepAlgoAPI_Section
  (const Handle(Geom_Surface)& Sf,
   const TopoDS_Shape&         Sh,
   const Standard_Boolean      PerformNow)
: 
  BRepAlgoAPI_BooleanOperation(MakeShape(Sf), 
                               Sh, 
                               BOPAlgo_SECTION)
{
  Init(PerformNow); 
}
//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepAlgoAPI_Section::BRepAlgoAPI_Section
  (const Handle(Geom_Surface)& Sf1,
   const Handle(Geom_Surface)& Sf2,
   const Standard_Boolean PerformNow)
: 
  BRepAlgoAPI_BooleanOperation(MakeShape(Sf1), 
                               MakeShape(Sf2), 
                               BOPAlgo_SECTION)
{
  Init(PerformNow); 
}
//=======================================================================
//function : ~BRepAlgoAPI_Section
//purpose  : 
//=======================================================================
BRepAlgoAPI_Section::~BRepAlgoAPI_Section()
{
}
//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void BRepAlgoAPI_Section::Init(const Standard_Boolean bFlag) 
{
  myOperation=BOPAlgo_SECTION;
  myApprox = Standard_False;
  myComputePCurve1 = Standard_False;
  myComputePCurve2 = Standard_False;
  // 
  if (bFlag) {
    Build();
  }
}
//=======================================================================
//function : Init1
//purpose  : 
//=======================================================================
void BRepAlgoAPI_Section::Init1(const TopoDS_Shape& S1) 
{
  myArguments.Clear();
  myArguments.Append(S1);
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
  myTools.Clear();
  myTools.Append(S2);
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
  myApprox = B;
}
//=======================================================================
//function : ComputePCurveOn1
//purpose  : 
//=======================================================================
void BRepAlgoAPI_Section::ComputePCurveOn1(const Standard_Boolean B) 
{
  myComputePCurve1 = B;
}
//=======================================================================
//function : ComputePCurveOn2
//purpose  : 
//=======================================================================
void BRepAlgoAPI_Section::ComputePCurveOn2(const Standard_Boolean B) 
{
  myComputePCurve2 = B;
}
//=======================================================================
//function : SetAttributes
//purpose  : 
//=======================================================================
void BRepAlgoAPI_Section::SetAttributes()
{
  BOPAlgo_SectionAttribute theSecAttr(myApprox,
                                      myComputePCurve1,
                                      myComputePCurve2);
  myDSFiller->SetSectionAttribute(theSecAttr);
}
//=======================================================================
//function : Build
//purpose  : 
//=======================================================================
void BRepAlgoAPI_Section::Build() 
{
  BRepAlgoAPI_BooleanOperation::Build();
}
//=======================================================================
//function : HasAncestorFaceOn1
//purpose  : 
//=======================================================================
Standard_Boolean BRepAlgoAPI_Section::HasAncestorFaceOn1
  (const TopoDS_Shape& aE, 
   TopoDS_Shape& aF) const
{
  Standard_Boolean bRes;
  //
  bRes = HasAncestorFace(myDSFiller,1 , aE, aF);
  return bRes;
}
//=======================================================================
//function : HasAncestorFaceOn2
//purpose  : 
//=======================================================================
Standard_Boolean BRepAlgoAPI_Section::HasAncestorFaceOn2
  (const TopoDS_Shape& aE,
   TopoDS_Shape& aF) const
{
  Standard_Boolean bRes;
  //
  bRes = HasAncestorFace(myDSFiller, 2, aE, aF);
  return bRes;
}
//=======================================================================
//function : HasAncestorFace
//purpose  : 
//=======================================================================
Standard_Boolean HasAncestorFace (const BOPAlgo_PPaveFiller& pPF,
                                  Standard_Integer aIndex,
                                  const TopoDS_Shape& aE,
                                  TopoDS_Shape& aF) 
{
  Standard_Boolean bRes;
  //
  bRes = Standard_False;
  if(aE.IsNull()) {
    return bRes;
  }
  if(aE.ShapeType() != TopAbs_EDGE) {
    return bRes;
  }
  //
  TopoDS_Shape aF1, aF2;
  //
  bRes=HasAncestorFaces(pPF, aE, aF1, aF2);
  if (!bRes) {
    return bRes;
  }
  //
  aF=(aIndex==1) ? aF1 : aF2;
  return bRes;
}

//=======================================================================
//function : HasAncestorFaces
//purpose  : 
//=======================================================================
Standard_Boolean HasAncestorFaces (const BOPAlgo_PPaveFiller& pPF, 
                                   const TopoDS_Shape& aEx,
                                   TopoDS_Shape& aF1,
                                   TopoDS_Shape& aF2) 
{
  
  Standard_Integer aNbFF, i, j, nE, nF1, nF2, aNbVC;
  BOPDS_ListIteratorOfListOfPaveBlock aItLPB;
  //
  const BOPDS_PDS& pDS = pPF->PDS();
  BOPDS_VectorOfInterfFF& aFFs=pDS->InterfFF();
  //
  //section edges
  aNbFF=aFFs.Length();
  for (i = 0; i<aNbFF; ++i) {
    BOPDS_InterfFF& aFFi=aFFs(i);
    aFFi.Indices(nF1, nF2);
    //
    const BOPDS_VectorOfCurve& aVC=aFFi.Curves();
    aNbVC=aVC.Length();
    for (j=0; j<aNbVC; j++) {
      const BOPDS_Curve& aBC=aVC(j);
      //
      const BOPDS_ListOfPaveBlock& aLPB = aBC.PaveBlocks();
      //
      aItLPB.Initialize(aLPB);
      for(; aItLPB.More(); aItLPB.Next()) {
        const Handle(BOPDS_PaveBlock)& aPB = aItLPB.Value();
        nE = aPB->Edge();
        if(nE < 0)  {
          continue;
        }
        //
        const TopoDS_Shape aE=pDS->Shape(nE);
        if(aEx.IsSame(aE)) {
          aF1 = pDS->Shape(nF1);
          aF2 = pDS->Shape(nF2);
          return Standard_True;
        }
      }
    }
  }
  return Standard_False;
}
//=======================================================================
//function : MakeShape
//purpose  : 
//=======================================================================
TopoDS_Shape MakeShape(const Handle(Geom_Surface)& S)
{
  GeomAbs_Shape c = S->Continuity();
  if (c >= GeomAbs_C2) {
    return BRepBuilderAPI_MakeFace(S, Precision::Confusion());
  }
  return BRepBuilderAPI_MakeShell(S);
}
