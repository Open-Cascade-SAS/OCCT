// Created on: 1996-10-23
// Created by: Jean Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
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

#include <BRep_Tool.hxx>
#include <IntPatch_GLine.hxx>
#include <Standard_Type.hxx>
#include <TopoDS.hxx>
#include <TopOpeBRep.hxx>
#include <TopOpeBRep_FacesFiller.hxx>
#include <TopOpeBRep_FacesIntersector.hxx>
#include <TopOpeBRep_FFDumper.hxx>
#include <TopOpeBRep_FFTransitionTool.hxx>
#include <TopOpeBRep_LineInter.hxx>
#include <TopOpeBRep_VPointInter.hxx>
#include <TopOpeBRep_VPointInterIterator.hxx>
#include <TopOpeBRepDS_DataStructure.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepTool_ShapeTool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TopOpeBRep_FFDumper, Standard_Transient)

#ifdef OCCT_DEBUG
static TCollection_AsciiString PRODINP("dinp ");
#endif

//=================================================================================================

#ifndef OCCT_DEBUG
TopOpeBRep_FFDumper::TopOpeBRep_FFDumper(const TopOpeBRep_PFacesFiller&)
{
#else
TopOpeBRep_FFDumper::TopOpeBRep_FFDumper(const TopOpeBRep_PFacesFiller& PFF)
{
  Init(PFF);
#endif
}

//=================================================================================================

#ifndef OCCT_DEBUG
void TopOpeBRep_FFDumper::Init(const TopOpeBRep_PFacesFiller&)
{
  // just shut up compiler warnings
  (void)myEn1;
  (void)myEn2;
  (void)myLineIndex;
}
#else
void TopOpeBRep_FFDumper::Init(const TopOpeBRep_PFacesFiller& PFF)
{
  myPFF                     = PFF;
  const TopoDS_Face& fpff1  = myPFF->Face(1);
  const TopoDS_Face& fpff2  = myPFF->Face(2);
  Standard_Boolean   f1diff = (!myF1.IsEqual(fpff1));
  Standard_Boolean   f2diff = (!myF2.IsEqual(fpff2));
  Standard_Boolean   init   = f1diff || f2diff;
  if (init)
  {
    myF1 = myPFF->Face(1);
    myF2 = myPFF->Face(2);
    myEM1.Clear();
    myEn1 = 0;
    myEM2.Clear();
    myEn2 = 0;
    TopExp_Explorer x;
    for (x.Init(myF1, TopAbs_EDGE); x.More(); x.Next())
      myEM1.Bind(x.Current(), ++myEn1);
    for (x.Init(myF2, TopAbs_EDGE); x.More(); x.Next())
      myEM2.Bind(x.Current(), ++myEn2);
    myLineIndex = 0;
  }
}
#endif

//=================================================================================================

#ifndef OCCT_DEBUG
void TopOpeBRep_FFDumper::DumpLine(const Standard_Integer)
{
#else
void TopOpeBRep_FFDumper::DumpLine(const Standard_Integer I)
{
  const TopOpeBRep_LineInter& L = myPFF->ChangeFacesIntersector().ChangeLine(I);
  DumpLine(L);
#endif
}

//=================================================================================================

#ifndef OCCT_DEBUG
void TopOpeBRep_FFDumper::DumpLine(const TopOpeBRep_LineInter&)
{
#else
void TopOpeBRep_FFDumper::DumpLine(const TopOpeBRep_LineInter& LI)
{
  Standard_Integer il         = LI.Index();
  myLineIndex                 = il;
  Standard_Integer nl         = myPFF->ChangeFacesIntersector().NbLines();
  Standard_Boolean HasVPonR   = LI.HasVPonR();
  Standard_Boolean IsVClosed  = LI.IsVClosed();
  Standard_Boolean IsPeriodic = LI.IsPeriodic();
  Standard_Boolean isrest     = (LI.TypeLineCurve() == TopOpeBRep_RESTRICTION);

  std::cout << std::endl << "--------------------------------------------------" << std::endl;
  std::cout << "line " << il << "/" << nl << " is a ";
  LI.DumpType();
  if (isrest)
  {
    Standard_Boolean isedge1 = LI.ArcIsEdge(1);
    Standard_Boolean isedge2 = LI.ArcIsEdge(2);
    if (isedge1)
      std::cout << " of 1";
    else if (isedge2)
      std::cout << " of 2";
    else
      std::cout << "of 0(BUG)";
  }
  std::cout << std::endl;
  if (isrest)
  {
    const TopoDS_Shape& Erest    = LI.Arc();
    Standard_Boolean    FIisrest = myPFF->ChangeFacesIntersector().IsRestriction(Erest);
    std::cout << "++++ line restriction";
    if (FIisrest)
    {
      std::cout << " edge restriction";
      Standard_Integer iErest = 0;
      if (myPFF->ChangeDataStructure().HasShape(Erest))
        iErest = myPFF->ChangeDataStructure().Shape(Erest);
      std::cout << " " << iErest;
    }
    std::cout << std::endl;
  }
  if (HasVPonR)
    std::cout << "has vertex on restriction" << std::endl;
  else
    std::cout << "has no vertex on restriction" << std::endl;
  if (IsVClosed)
    std::cout << "is closed by vertices" << std::endl;
  else
    std::cout << "is not closed by vertices" << std::endl;
  if (IsPeriodic)
    std::cout << "is periodic" << std::endl;
  else
    std::cout << "is not periodic" << std::endl;

  TopOpeBRep_VPointInterIterator VPI;

  VPI.Init(LI);
  if (VPI.More())
    std::cout << std::endl;
  for (; VPI.More(); VPI.Next())
  {
    TCollection_AsciiString stol("; #draw ");
    stol = stol + VPI.CurrentVP().Tolerance() + "\n";
    LI.DumpVPoint(VPI.CurrentVPIndex(), PRODINP, stol);
  }

  VPI.Init(LI);
  if (VPI.More())
    std::cout << std::endl;
  for (; VPI.More(); VPI.Next())
  {
    const TopOpeBRep_VPointInter& VP   = VPI.CurrentVP();
    Standard_Boolean              dump = VP.Keep();
    if (dump)
    {
      DumpVP(VP);
      std::cout << std::endl;
    }
  }

  if (LI.TypeLineCurve() == TopOpeBRep_LINE)
  {
    gp_Dir D = LI.LineG()->Line().Direction();
    TopOpeBRep::Print(LI.TypeLineCurve(), std::cout);
    Standard_Real x, y, z;
    D.Coord(x, y, z);
    std::cout << " dir : " << x << " " << y << " " << z << std::endl;
  }

  LI.DumpLineTransitions(std::cout);

  std::cout << std::endl << "--------------------------------------------------" << std::endl;
#endif
}

//=================================================================================================

#ifndef OCCT_DEBUG
void TopOpeBRep_FFDumper::DumpVP(const TopOpeBRep_VPointInter&)
{
#else
void TopOpeBRep_FFDumper::DumpVP(const TopOpeBRep_VPointInter& VP)
{
  Standard_Integer il = myLineIndex;
  std::cout << "VP " << VP.Index() << " on " << VP.ShapeIndex() << " :";
  Standard_Real Cpar = VP.ParameterOnLine();
  std::cout << " on curve : " << Cpar;
  if (!VP.Keep())
    std::cout << " NOT kept";
  std::cout << std::endl;
  Standard_Boolean k = VP.Keep();
  const gp_Pnt&    P = VP.Value();
  std::cout << PRODINP << "L" << il << "P" << VP.Index();
  if (k)
    std::cout << "K";
  std::cout << " " << P.X() << " " << P.Y() << " " << P.Z();
  std::cout << "; #draw" << std::endl;

  if (VP.ShapeIndex() == 1)
    DumpVP(VP, 1);
  else if (VP.ShapeIndex() == 2)
    DumpVP(VP, 2);
  else if (VP.ShapeIndex() == 3)
  {
    DumpVP(VP, 1);
    DumpVP(VP, 2);
  }
#endif
}

//=================================================================================================

#ifndef OCCT_DEBUG
void TopOpeBRep_FFDumper::DumpVP(const TopOpeBRep_VPointInter&, const Standard_Integer)
{
#else
void TopOpeBRep_FFDumper::DumpVP(const TopOpeBRep_VPointInter& VP, const Standard_Integer ISI)
{
  const Handle(TopOpeBRepDS_HDataStructure)& HDS  = myPFF->HDataStructure();
  const TopoDS_Edge&                         E    = TopoDS::Edge(VP.Edge(ISI));
  Standard_Real                              Epar = VP.EdgeParameter(ISI);
  TopAbs_Orientation                         O    = E.Orientation();
  TopOpeBRep_FFTransitionTool::ProcessLineTransition(VP, ISI, O);
  const TopoDS_Face F      = myPFF->Face(ISI);
  Standard_Boolean  Closed = TopOpeBRepTool_ShapeTool::Closed(E, F);
  Standard_Boolean  Degen  = BRep_Tool::Degenerated(E);
  Standard_Integer  exi    = ExploreIndex(E, ISI);
  Standard_Integer  dsi    = (HDS->HasShape(E)) ? HDS->Shape(E) : 0;
  Standard_Boolean  isv    = VP.IsVertex(ISI);
  if (isv)
    std::cout << "is vertex of " << ISI << std::endl;
  if (Closed)
    std::cout << "on closing edge ";
  else
    std::cout << "on edge ";
  if (Degen)
    std::cout << " on degenerated edge ";
  TopAbs::Print(O, std::cout);
  std::cout << " (ds" << dsi << ") (ex" << exi << ") of face of " << ISI;
  std::cout << " : par : " << Epar << std::endl;
  if (Closed)
    std::cout << "on closing edge ";
  else
    std::cout << "on edge ";
  if (Degen)
    std::cout << " on degenerated edge ";
  TopAbs::Print(O, std::cout);
  std::cout << " (ds" << dsi << ") (ex" << exi << ") of face of " << ISI;
#endif
}

//=================================================================================================

#ifndef OCCT_DEBUG
Standard_Integer TopOpeBRep_FFDumper::ExploreIndex(const TopoDS_Shape&,
                                                   const Standard_Integer) const
{
  return 0;
}
#else
Standard_Integer TopOpeBRep_FFDumper::ExploreIndex(const TopoDS_Shape&    S,
                                                   const Standard_Integer ISI) const
{
  Standard_Integer r = 0;
  if (ISI == 1)
    r = myEM1.Find(S);
  if (ISI == 2)
    r = myEM2.Find(S);
  return r;
}
#endif

//=================================================================================================

#ifndef OCCT_DEBUG
void TopOpeBRep_FFDumper::DumpDSP(const TopOpeBRep_VPointInter&,
                                  const TopOpeBRepDS_Kind,
                                  const Standard_Integer,
                                  const Standard_Boolean) const
{
#else
void TopOpeBRep_FFDumper::DumpDSP(const TopOpeBRep_VPointInter& VP,
                                  const TopOpeBRepDS_Kind       GK,
                                  const Standard_Integer        G,
                                  const Standard_Boolean        newinDS) const
{
  std::cout << "VP " << VP.Index() << " on " << VP.ShapeIndex();
  if (newinDS)
  {
    if (GK == TopOpeBRepDS_VERTEX)
      std::cout << " gives new DSV";
    else if (GK == TopOpeBRepDS_POINT)
      std::cout << " gives new DSP";
    else
      std::cout << " gives new DS???";
  }
  else
  {
    if (GK == TopOpeBRepDS_VERTEX)
      std::cout << " equals new DSV";
    else if (GK == TopOpeBRepDS_POINT)
      std::cout << " equals new DSP";
    else
      std::cout << " equals new DS???";
  }
  std::cout << " " << G;

  const Handle(TopOpeBRepDS_HDataStructure)& HDS = myPFF->HDataStructure();
  Standard_Real                              tol = Precision::Confusion();
  if (GK == TopOpeBRepDS_VERTEX)
    tol = BRep_Tool::Tolerance(TopoDS::Vertex(HDS->Shape(G)));
  else if (GK == TopOpeBRepDS_POINT)
    tol = HDS->Point(G).Tolerance();
  std::cout << " tol = " << tol;
  std::cout << std::endl;
#endif
}

TopOpeBRep_PFacesFiller TopOpeBRep_FFDumper::PFacesFillerDummy() const
{
  return myPFF;
}
