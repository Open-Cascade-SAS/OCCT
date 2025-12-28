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

#include <TopoDS_Shape.hxx>

//
static TopoDS_Shape MakeShape(const occ::handle<Geom_Surface>&);

static bool HasAncestorFaces(const BOPAlgo_PPaveFiller&,
                                         const TopoDS_Shape&,
                                         TopoDS_Shape&,
                                         TopoDS_Shape&);
static bool HasAncestorFace(const BOPAlgo_PPaveFiller&,
                                        int,
                                        const TopoDS_Shape&,
                                        TopoDS_Shape&);

//
//=================================================================================================

BRepAlgoAPI_Section::BRepAlgoAPI_Section()
    : BRepAlgoAPI_BooleanOperation()
{
  Init(false);
}

//=================================================================================================

BRepAlgoAPI_Section::BRepAlgoAPI_Section(const BOPAlgo_PaveFiller& aPF)
    : BRepAlgoAPI_BooleanOperation(aPF)
{
  Init(false);
}

//=================================================================================================

BRepAlgoAPI_Section::BRepAlgoAPI_Section(const TopoDS_Shape&    Sh1,
                                         const TopoDS_Shape&    Sh2,
                                         const bool PerformNow)
    : BRepAlgoAPI_BooleanOperation(Sh1, Sh2, BOPAlgo_SECTION)
{
  Init(PerformNow);
}

//=================================================================================================

BRepAlgoAPI_Section::BRepAlgoAPI_Section(const TopoDS_Shape&       aS1,
                                         const TopoDS_Shape&       aS2,
                                         const BOPAlgo_PaveFiller& aDSF,
                                         const bool    PerformNow)
    : BRepAlgoAPI_BooleanOperation(aS1, aS2, aDSF, BOPAlgo_SECTION)
{
  Init(PerformNow);
}

//=================================================================================================

BRepAlgoAPI_Section::BRepAlgoAPI_Section(const TopoDS_Shape&    Sh,
                                         const gp_Pln&          Pl,
                                         const bool PerformNow)
    : BRepAlgoAPI_BooleanOperation(Sh, MakeShape(new Geom_Plane(Pl)), BOPAlgo_SECTION)
{
  Init(PerformNow);
}

//=================================================================================================

BRepAlgoAPI_Section::BRepAlgoAPI_Section(const TopoDS_Shape&         Sh,
                                         const occ::handle<Geom_Surface>& Sf,
                                         const bool      PerformNow)
    : BRepAlgoAPI_BooleanOperation(Sh, MakeShape(Sf), BOPAlgo_SECTION)
{
  Init(PerformNow);
}

//=================================================================================================

BRepAlgoAPI_Section::BRepAlgoAPI_Section(const occ::handle<Geom_Surface>& Sf,
                                         const TopoDS_Shape&         Sh,
                                         const bool      PerformNow)
    : BRepAlgoAPI_BooleanOperation(MakeShape(Sf), Sh, BOPAlgo_SECTION)
{
  Init(PerformNow);
}

//=================================================================================================

BRepAlgoAPI_Section::BRepAlgoAPI_Section(const occ::handle<Geom_Surface>& Sf1,
                                         const occ::handle<Geom_Surface>& Sf2,
                                         const bool      PerformNow)
    : BRepAlgoAPI_BooleanOperation(MakeShape(Sf1), MakeShape(Sf2), BOPAlgo_SECTION)
{
  Init(PerformNow);
}

//=================================================================================================

BRepAlgoAPI_Section::~BRepAlgoAPI_Section() {}

//=================================================================================================

void BRepAlgoAPI_Section::Init(const bool bFlag)
{
  myOperation      = BOPAlgo_SECTION;
  myApprox         = false;
  myComputePCurve1 = false;
  myComputePCurve2 = false;
  //
  if (bFlag)
  {
    Build();
  }
}

//=================================================================================================

void BRepAlgoAPI_Section::Init1(const TopoDS_Shape& S1)
{
  myArguments.Clear();
  myArguments.Append(S1);
}

//=================================================================================================

void BRepAlgoAPI_Section::Init1(const gp_Pln& Pl)
{
  Init1(MakeShape(new Geom_Plane(Pl)));
}

//=================================================================================================

void BRepAlgoAPI_Section::Init1(const occ::handle<Geom_Surface>& Sf)
{
  Init1(MakeShape(Sf));
}

//=================================================================================================

void BRepAlgoAPI_Section::Init2(const TopoDS_Shape& S2)
{
  myTools.Clear();
  myTools.Append(S2);
}

//=================================================================================================

void BRepAlgoAPI_Section::Init2(const gp_Pln& Pl)
{
  Init2(MakeShape(new Geom_Plane(Pl)));
}

//=================================================================================================

void BRepAlgoAPI_Section::Init2(const occ::handle<Geom_Surface>& Sf)
{
  Init2(MakeShape(Sf));
}

//=================================================================================================

void BRepAlgoAPI_Section::Approximation(const bool B)
{
  myApprox = B;
}

//=================================================================================================

void BRepAlgoAPI_Section::ComputePCurveOn1(const bool B)
{
  myComputePCurve1 = B;
}

//=================================================================================================

void BRepAlgoAPI_Section::ComputePCurveOn2(const bool B)
{
  myComputePCurve2 = B;
}

//=================================================================================================

void BRepAlgoAPI_Section::SetAttributes()
{
  BOPAlgo_SectionAttribute theSecAttr(myApprox, myComputePCurve1, myComputePCurve2);
  myDSFiller->SetSectionAttribute(theSecAttr);
}

//=================================================================================================

void BRepAlgoAPI_Section::Build(const Message_ProgressRange& theRange)
{
  BRepAlgoAPI_BooleanOperation::Build(theRange);
}

//=================================================================================================

bool BRepAlgoAPI_Section::HasAncestorFaceOn1(const TopoDS_Shape& aE,
                                                         TopoDS_Shape&       aF) const
{
  bool bRes;
  //
  bRes = HasAncestorFace(myDSFiller, 1, aE, aF);
  return bRes;
}

//=================================================================================================

bool BRepAlgoAPI_Section::HasAncestorFaceOn2(const TopoDS_Shape& aE,
                                                         TopoDS_Shape&       aF) const
{
  bool bRes;
  //
  bRes = HasAncestorFace(myDSFiller, 2, aE, aF);
  return bRes;
}

//=================================================================================================

bool HasAncestorFace(const BOPAlgo_PPaveFiller& pPF,
                                 int           aIndex,
                                 const TopoDS_Shape&        aE,
                                 TopoDS_Shape&              aF)
{
  bool bRes;
  //
  bRes = false;
  if (aE.IsNull())
  {
    return bRes;
  }
  if (aE.ShapeType() != TopAbs_EDGE)
  {
    return bRes;
  }
  //
  TopoDS_Shape aF1, aF2;
  //
  bRes = HasAncestorFaces(pPF, aE, aF1, aF2);
  if (!bRes)
  {
    return bRes;
  }
  //
  aF = (aIndex == 1) ? aF1 : aF2;
  return bRes;
}

//=================================================================================================

bool HasAncestorFaces(const BOPAlgo_PPaveFiller& pPF,
                                  const TopoDS_Shape&        aEx,
                                  TopoDS_Shape&              aF1,
                                  TopoDS_Shape&              aF2)
{

  int                    aNbFF, i, j, nE, nF1, nF2, aNbVC;
  NCollection_List<occ::handle<BOPDS_PaveBlock>>::Iterator aItLPB;
  //
  const BOPDS_PDS&        pDS  = pPF->PDS();
  NCollection_Vector<BOPDS_InterfFF>& aFFs = pDS->InterfFF();
  //
  // section edges
  aNbFF = aFFs.Length();
  for (i = 0; i < aNbFF; ++i)
  {
    BOPDS_InterfFF& aFFi = aFFs(i);
    aFFi.Indices(nF1, nF2);
    //
    const NCollection_Vector<BOPDS_Curve>& aVC = aFFi.Curves();
    aNbVC                          = aVC.Length();
    for (j = 0; j < aNbVC; j++)
    {
      const BOPDS_Curve& aBC = aVC(j);
      //
      const NCollection_List<occ::handle<BOPDS_PaveBlock>>& aLPB = aBC.PaveBlocks();
      //
      aItLPB.Initialize(aLPB);
      for (; aItLPB.More(); aItLPB.Next())
      {
        const occ::handle<BOPDS_PaveBlock>& aPB = aItLPB.Value();
        nE                                 = aPB->Edge();
        if (nE < 0)
        {
          continue;
        }
        //
        const TopoDS_Shape aE = pDS->Shape(nE);
        if (aEx.IsSame(aE))
        {
          aF1 = pDS->Shape(nF1);
          aF2 = pDS->Shape(nF2);
          return true;
        }
      }
    }
  }
  return false;
}

//=================================================================================================

TopoDS_Shape MakeShape(const occ::handle<Geom_Surface>& S)
{
  GeomAbs_Shape c = S->Continuity();
  if (c >= GeomAbs_C2)
  {
    return BRepBuilderAPI_MakeFace(S, Precision::Confusion());
  }
  return BRepBuilderAPI_MakeShell(S);
}
