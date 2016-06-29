// Created on: 1999-01-25
// Created by: Xuan PHAM PHU
// Copyright (c) 1999-1999 Matra Datavision
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
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepDS.hxx>
#include <TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State.hxx>
#include <TopOpeBRepDS_define.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_ListOfShapeOn1State.hxx>
#include <TopOpeBRepDS_ProcessInterferencesTool.hxx>
#include <TopOpeBRepDS_ShapeShapeInterference.hxx>
#include <TopOpeBRepDS_TOOL.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_TOOL.hxx>

#define M_REVERSED(O) (O == TopAbs_REVERSED) 

static void FDS_sortGb(const Handle(TopOpeBRepDS_HDataStructure)& HDS,const TopOpeBRepDS_ListOfInterference& LI, TopOpeBRepDS_ListOfInterference& LIGb0,TopOpeBRepDS_ListOfInterference& LIGb1,TopOpeBRepDS_ListOfInterference& LIGbsd)
{
  const TopOpeBRepDS_DataStructure& BDS = HDS->DS();
  LIGb0.Clear(); LIGb1.Clear(); LIGbsd.Clear();
  TopOpeBRepDS_ListIteratorOfListOfInterference it(LI);
  for (; it.More(); it.Next()){
    const Handle(TopOpeBRepDS_Interference)& I = it.Value();
    Handle(TopOpeBRepDS_ShapeShapeInterference) SSI = Handle(TopOpeBRepDS_ShapeShapeInterference)::DownCast(I);
    if (SSI.IsNull()) {LIGb0.Append(I);continue;}

    Standard_Boolean gb1 = SSI->GBound();
    Standard_Integer G = I->Geometry();
    Standard_Boolean hsd = HDS->HasSameDomain(BDS.Shape(G));
    if (hsd)          {LIGbsd.Append(I);continue;}

    if (gb1) LIGb1.Append(I);
    else     LIGb0.Append(I);                    
  }//it(LI)  
}

//=======================================================================
//function : EShareG
//purpose  : 
//=======================================================================

#define FORWARD  (1)
#define REVERSED (2)
#define INTERNAL (3)
#define EXTERNAL (4)
#define CLOSING  (5)
Standard_Integer TopOpeBRepDS_TOOL::EShareG(const Handle(TopOpeBRepDS_HDataStructure)& HDS,const TopoDS_Edge& E,TopTools_ListOfShape& lEsd)
{
  lEsd.Clear();
  Standard_Boolean dgE = BRep_Tool::Degenerated(E);
  if (dgE) {
    Standard_Boolean hsd = HDS->HasSameDomain(E);
    if (!hsd) return 0;
    TopTools_ListIteratorOfListOfShape itsd(HDS->SameDomain(E));
    for (; itsd.More(); itsd.Next()) lEsd.Append(itsd.Value());
    return lEsd.Extent();
  }

  const TopOpeBRepDS_DataStructure& BDS = HDS->DS();
  const TopOpeBRepDS_ListOfInterference& LI = BDS.ShapeInterferences(E);
  TopOpeBRepDS_ListOfInterference LII; FDS_copy(LI,LII); 
  TopOpeBRepDS_ListOfInterference L1d; Standard_Integer n1d = FUN_selectTRASHAinterference(LII,TopAbs_EDGE,L1d);
  if (n1d == 0) return 0;
  
  TopTools_MapOfShape mapesd;
  TopOpeBRepDS_ListOfInterference l1gb0,l1gb1,l1gbsd; FDS_sortGb(HDS,L1d,l1gb0,l1gb1,l1gbsd);
  TopOpeBRepDS_ListIteratorOfListOfInterference it0(l1gb0);
  for (; it0.More(); it0.Next()) mapesd.Add(BDS.Shape(it0.Value()->Support()));        

  TopOpeBRepDS_ListIteratorOfListOfInterference it1(l1gb1);
  for (; it1.More(); it1.Next()) mapesd.Add(BDS.Shape(it1.Value()->Support()));        

  TopOpeBRepDS_ListIteratorOfListOfInterference itsd(l1gbsd);
  for (; itsd.More(); itsd.Next()) {
    const Handle(TopOpeBRepDS_Interference)& I = itsd.Value();
    const TopoDS_Edge& Esd = TopoDS::Edge(BDS.Shape(I->Support()));
    Standard_Boolean isb = mapesd.Contains(Esd);
    if (isb) continue;

    Standard_Integer G = I->Geometry();
    const TopoDS_Vertex& vG = TopoDS::Vertex(BDS.Shape(G));
    TopoDS_Vertex vsd; Standard_Boolean ok = FUN_ds_getoov(vG,BDS,vsd);
    if (!ok) continue;
    Standard_Boolean Gb1 = Handle(TopOpeBRepDS_ShapeShapeInterference)::DownCast(I)->GBound();
    TopoDS_Vertex vE   = Gb1 ? vG : vsd;
    TopoDS_Vertex vEsd = Gb1 ? vsd : vG;

    Standard_Integer ovE; gp_Vec tgE; 
    ok = TopOpeBRepTool_TOOL::TgINSIDE(vE,E,tgE,ovE);
    if (!ok) continue;
    Standard_Integer ovEsd; gp_Vec tgEsd; 
    ok = TopOpeBRepTool_TOOL::TgINSIDE(vEsd,Esd,tgEsd,ovEsd);
    if (!ok) continue;
    Standard_Boolean inE   = (ovE == CLOSING)||(ovE == INTERNAL);
    Standard_Boolean inEsd = (ovEsd == CLOSING)||(ovEsd == INTERNAL);
    if (inE || inEsd) {mapesd.Add(Esd); continue;}
    Standard_Real dot = gp_Dir(tgE).Dot(gp_Dir(tgEsd));
    if (dot > 0.)      mapesd.Add(Esd);  
  }
  TopTools_MapIteratorOfMapOfShape itm(mapesd);
  for (; itm.More(); itm.Next()) lEsd.Append(itm.Key());
  return (lEsd.Extent());
}



//=======================================================================
//function : ShareG
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_TOOL::ShareG(const Handle(TopOpeBRepDS_HDataStructure)& HDS, const Standard_Integer i1, const Standard_Integer i2)
{
  const TopoDS_Shape& s1 = HDS->Shape(i1);
  const TopoDS_Shape& s2 = HDS->Shape(i2);

  Standard_Boolean hsd1 = HDS->HasSameDomain(s1);
  if (!hsd1) return Standard_False;
  TopTools_ListIteratorOfListOfShape it1(HDS->SameDomain(s1));
  for (; it1.More(); it1.Next()){
    Standard_Boolean same = it1.Value().IsSame(s2);
    if (!same) continue;
    return Standard_True;
  }
  return Standard_False;
}



//=======================================================================
//function : GetEsd
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_TOOL::GetEsd(const Handle(TopOpeBRepDS_HDataStructure)& HDS,
			      const TopoDS_Shape& S, const Standard_Integer ie, Standard_Integer& iesd)
{
  // recall : method ::SameDomain(s) returns an iterator on the list of shapes
  //          sdm to s (ie actually sharing geometric domain with s)
  iesd = 0;
  TopTools_MapOfShape mesdS;
  TopExp_Explorer ex(S, TopAbs_EDGE);
  for (; ex.More(); ex.Next()){
    const TopoDS_Shape& e = ex.Current();
    Standard_Boolean hs = HDS->HasShape(e);
    if (!hs) continue;
    Standard_Boolean hsd = HDS->HasSameDomain(e);
    if (!hsd) continue;
    mesdS.Add(e);
//    TopTools_ListIteratorOfListOfShape itt(HDS->SameDomain(e));
//    for (; itt.More(); itt.Next()) mesdS.Add(itt.Value());
  }

  TopTools_ListIteratorOfListOfShape it(HDS->SameDomain(HDS->Shape(ie)));
  for (; it.More(); it.Next()){
    const TopoDS_Shape& esd = it.Value();
    Standard_Boolean isb = mesdS.Contains(esd);
    if (!isb) continue;
    iesd = HDS->Shape(esd);
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : ShareSplitON
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_TOOL::ShareSplitON(const Handle(TopOpeBRepDS_HDataStructure)& HDS,
				    const TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State& MEspON,
				    const Standard_Integer i1, const Standard_Integer i2, TopoDS_Shape& spON)
{  
  spON.Nullify();
  Standard_Boolean shareg = TopOpeBRepDS_TOOL::ShareG(HDS,i1,i2);
  if (!shareg) return Standard_False;

  const TopoDS_Shape& s1 = HDS->Shape(i1);
  const TopoDS_Shape& s2 = HDS->Shape(i2);

  const TopOpeBRepDS_ListOfShapeOn1State& los1 = MEspON.Find(s1);
  Standard_Boolean issp = los1.IsSplit();
  if (!issp) return Standard_False;
  const TopTools_ListOfShape& lsp1 = los1.ListOnState();
  Standard_Integer nsp1 = lsp1.Extent();
  if (nsp1 == 0) return Standard_False;
  TopTools_MapOfShape mesp1; // map of splits on of <s1>
  TopTools_ListIteratorOfListOfShape it(lsp1);
  for (; it.More(); it.Next()) mesp1.Add(it.Value());

  const TopOpeBRepDS_ListOfShapeOn1State& los2 = MEspON.Find(s2);
  Standard_Boolean issp2 = los2.IsSplit();
  if (!issp2) return Standard_False;
  const TopTools_ListOfShape& lsp2 = los2.ListOnState();
  Standard_Integer nsp2 = lsp2.Extent();
  if (nsp2 == 0) return Standard_False;  

  it.Initialize(lsp2);
  for (; it.More(); it.Next()) {
    const TopoDS_Shape& esp = it.Value();
    Standard_Boolean isb = mesp1.Contains(esp);
    if (!isb) continue;
    spON = esp; return Standard_True;
  }
  return Standard_False;   
}



//=======================================================================
//function : GetConfig
//purpose  : returns relative geometric config
//=======================================================================
#define SAMEORIENTED (1)
#define DIFFORIENTED (2)
Standard_Boolean TopOpeBRepDS_TOOL::GetConfig(const Handle(TopOpeBRepDS_HDataStructure)& HDS,
				 const TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State& MEspON,
				 const Standard_Integer ie,const Standard_Integer iesd,
				 Standard_Integer& config)
{
  config = 0;
  Standard_Boolean shareg = TopOpeBRepDS_TOOL::ShareG(HDS,ie,iesd);
  if (!shareg) return Standard_False;

  const TopoDS_Edge& e = TopoDS::Edge(HDS->Shape(ie));     TopAbs_Orientation oe = e.Orientation();
  const TopoDS_Edge& esd = TopoDS::Edge(HDS->Shape(iesd)); TopAbs_Orientation oesd = esd.Orientation();
  TopOpeBRepDS_Config conf = HDS->SameDomainOrientation(e); Standard_Boolean unsh = (conf == TopOpeBRepDS_UNSHGEOMETRY);
  TopOpeBRepDS_Config confsd = HDS->SameDomainOrientation(esd); Standard_Boolean unshsd = (confsd == TopOpeBRepDS_UNSHGEOMETRY);
  if (!unsh && !unshsd) {
    Standard_Boolean sameori = (conf == confsd);
    if (M_REVERSED(oe))   sameori = !sameori;
    if (M_REVERSED(oesd)) sameori = !sameori;
    config = sameori ? SAMEORIENTED : DIFFORIENTED;
    return Standard_True;
  }

  TopoDS_Shape eON; shareg = TopOpeBRepDS_TOOL::ShareSplitON(HDS,MEspON,ie,iesd,eON);
  if (!shareg) return Standard_False;

  Standard_Real f,l; FUN_tool_bounds(TopoDS::Edge(eON),f,l);
  Standard_Real x = 0.45678; Standard_Real parON = (1-x)*f+x*l;
  Standard_Real tole = BRep_Tool::Tolerance(TopoDS::Edge(e));
  Standard_Real pare; Standard_Boolean ok = FUN_tool_parE(TopoDS::Edge(eON),parON, e,pare, tole);
  if (!ok) return Standard_False;
  Standard_Real tolesd = BRep_Tool::Tolerance(TopoDS::Edge(esd));
  Standard_Real paresd; ok = FUN_tool_parE(TopoDS::Edge(eON),parON, esd,paresd, tolesd);
  if (!ok) return Standard_False;
  Standard_Boolean so; ok = FUN_tool_curvesSO(e,pare,esd,paresd,so);
  if (!ok) return Standard_False;
  config = (so)? SAMEORIENTED : DIFFORIENTED;  
  return Standard_True;
}

