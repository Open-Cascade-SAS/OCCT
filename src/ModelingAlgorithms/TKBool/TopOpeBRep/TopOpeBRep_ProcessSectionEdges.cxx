// Created on: 1997-06-12
// Created by: Jean Yves LEBEY
// Copyright (c) 1997-1999 Matra Datavision
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
#include <gp_Pnt.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_List.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRep_define.hxx>
#include <TopOpeBRep_FacesFiller.hxx>
#include <TopOpeBRep_FacesIntersector.hxx>
#include <TopOpeBRep_FFDumper.hxx>
#include <TopOpeBRep_LineInter.hxx>
#include <TopOpeBRep_PointClassifier.hxx>
#include <TopOpeBRep_VPointInter.hxx>
#include <TopOpeBRep_VPointInterClassifier.hxx>
#include <TopOpeBRep_VPointInterIterator.hxx>
#include <TopOpeBRepDS_DataStructure.hxx>
#include <TopOpeBRepTool_GEOMETRY.hxx>
#include <TopOpeBRepTool_PROJECT.hxx>
#include <TopOpeBRepTool_TOPOLOGY.hxx>
#include <TopOpeBRepTool_ShapeTool.hxx>
#include <TopOpeBRepTool_TOOL.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>

Standard_EXPORT bool FUN_EqualponR(const TopOpeBRep_LineInter&   Lrest,
                                   const TopOpeBRep_VPointInter& VP1,
                                   const TopOpeBRep_VPointInter& VP2);
Standard_EXPORT bool FUN_EqualPonR(const TopOpeBRep_LineInter&   Lrest,
                                   const TopOpeBRep_VPointInter& VP1,
                                   const TopOpeBRep_VPointInter& VP2);

//=======================================================================
// function : GetESL
// purpose  : Get list <LES> of restriction edges from the current faces
//           intersector having part IN one of the 2 faces.
//=======================================================================
void TopOpeBRep_FacesFiller::GetESL(NCollection_List<TopoDS_Shape>& LES)
{

#ifdef OCCT_DEBUG
  bool trRL = false;
#endif

  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> mapES;

  // !! : do NOT use  myFacesIntersector->Restrictions()
  // the same map is filled for all couple of faces.

  myFacesIntersector->InitLine();
  for (; myFacesIntersector->MoreLine(); myFacesIntersector->NextLine())
  {
    const TopOpeBRep_LineInter& L      = myFacesIntersector->CurrentLine();
    TopOpeBRep_TypeLineCurve    t      = L.TypeLineCurve();
    bool                        isrest = (t == TopOpeBRep_RESTRICTION);

    if (isrest)
    {
      const TopoDS_Edge& E = TopoDS::Edge(L.Arc());

#ifdef OCCT_DEBUG
      if (trRL)
      {
        TopOpeBRep_VPointInterIterator VPI;
        VPI.Init(L);
        std::cout << std::endl << "------------ Dump Rline  --------------------" << std::endl;
        for (; VPI.More(); VPI.Next())
          myHFFD->DumpVP(VPI.CurrentVP());
      }
#endif

      bool add = !mapES.Contains(E);
      if (add)
      {
        bool checkkeep = false;
        add            = KeepRLine(L, checkkeep);
      }
      if (add)
      {
        mapES.Add(E);
        LES.Append(E);
      }
    }
  } // loop on lines
}

//=================================================================================================

bool TopOpeBRep_FacesFiller::KeepRLine(const TopOpeBRep_LineInter& L, const bool checkkeep) const
{
  TopOpeBRep_TypeLineCurve t      = L.TypeLineCurve();
  bool                     isrest = (t == TopOpeBRep_RESTRICTION);
  if (!isrest)
    return false;
  const TopoDS_Edge& EL   = TopoDS::Edge(L.Arc());
  bool               isdg = BRep_Tool::Degenerated(EL);
  if (isdg)
    return false;

  // look for a vpoint with transition IN/OUT or OUT/IN
  TopOpeBRep_VPointInterIterator VPI;
  VPI.Init(L, checkkeep);

  // With LineConstructor, each RLine restricted by its vpbounds
  // has its restrictions IN or ON the two faces
  bool keeprline;
  bool isedge1 = L.ArcIsEdge(1);
  if (!VPI.More())
    return false;

  bool                          samevp = true;
  const TopOpeBRep_VPointInter& vpf    = VPI.CurrentVP();

  TopOpeBRep_VPointInter vpl;
  VPI.Init(L, checkkeep);
  if (VPI.More())
    VPI.Next();

  bool          middle = false; // xpu011098 : cto012U1
  TopoDS_Vertex vv;
  bool          closedE = TopOpeBRepTool_TOOL::ClosedE(EL, vv);
  if (closedE)
  {
    double parf, parl;
    FUN_tool_bounds(EL, parf, parl);
    for (; VPI.More(); VPI.Next())
    {
      vpl             = VPI.CurrentVP();
      double pf       = VPParamOnER(vpl, L);
      bool   middlept = (parf < pf) && (pf < parl);
      if (middlept)
      {
        middle = true;
        samevp = false;
        break;
      }
    }
    if (middle)
    {
      VPI.Init(L, checkkeep);
      for (; VPI.More(); VPI.Next())
      {
        vpl         = VPI.CurrentVP();
        bool samept = FUN_EqualPonR(L, vpf, vpl);
        if (samept)
          continue;
        else
          break;
      }
    }
  }
  if (!middle)
  {
    VPI.Init(L, checkkeep);
    if (VPI.More())
      VPI.Next();
    for (; VPI.More(); VPI.Next())
    {
      vpl    = VPI.CurrentVP();
      samevp = FUN_EqualponR(L, vpf, vpl);
      if (!samevp)
        break;
    }
  }

  if (!samevp)
  {
    // xpu151098 : cto 904 C8 : modif done tol2d > 0 => found restriction shared
    //   by circle/line
    bool samept = FUN_EqualPonR(L, vpf, vpl);
    if (samept)
    {
      TopoDS_Vertex vclo;
      bool          closedEL = TopOpeBRepTool_TOOL::ClosedE(EL, vclo);
      if (closedEL)
      {
        double tolvclo = BRep_Tool::Tolerance(vclo);
        //	double tolvclo = BRep_Tool::Tolerance(TopoDS::Vertex(vclo));
        gp_Pnt ptclo = BRep_Tool::Pnt(vclo);
        //	gp_Pnt ptclo = BRep_Tool::Pnt(TopoDS::Vertex(vclo));
        double tolf    = vpf.Tolerance();
        gp_Pnt ptf     = vpf.Value();
        double d       = ptf.Distance(ptclo);
        bool   sameclo = (d < std::max(tolvclo, tolf));
        if (!sameclo)
          return false;
      }
      else
        return false;
    }
  }

  bool out = false;
  if (samevp)
  {
    bool isper = TopOpeBRepTool_ShapeTool::BASISCURVE(EL)->IsPeriodic();

    int f, l, n;
    L.VPBounds(f, l, n);
    if (isper && n == 2)
    {
      const TopOpeBRep_VPointInter& vpf1 = L.VPoint(f);
      const TopOpeBRep_VPointInter& vpl1 = L.VPoint(l);
      int                           ioo  = (isedge1) ? 2 : 1;
      TopAbs_State                  sf = vpf1.State(ioo), sl = vpl1.State(ioo);
      bool                          bfl = true;
      // xpu120898 : when projection fails we get unknown status
      //             recall VP are same. (CTS21182,restriction edge 6)
      bool bf = (sf == TopAbs_IN || sf == TopAbs_ON);
      bool bl = (sl == TopAbs_IN || sl == TopAbs_ON);
      //      bfl = bf && bl;
      if ((sf == TopAbs_UNKNOWN) || (sl == TopAbs_UNKNOWN))
        bfl = bf || bl;
      else
        bfl = bf && bl;

      if (bfl)
      {
        out = false;
      }
      else
      {
        out = true;
      }
    }
    else
    {
      out = true;
    }
  }
  if (out)
  {
    return false;
  }

  TopAbs_State stVPbip = StBipVPonF(vpf, vpl, L, isedge1);
  keeprline            = (stVPbip == TopAbs_IN);
  keeprline            = keeprline || (stVPbip == TopAbs_ON); // REST1
#ifdef OCCT_DEBUG
//  if (trc) {
//    std::cout<<" bip("<<vpf.Index()<<","<<vpl.Index()<<") of line restriction ";
//    std::cout<<L.Index()<<" is ";TopAbs::Print(stVPbip,std::cout);
//    if (keeprline) std::cout<<" : edge restriction kept"<<std::endl;
//    else std::cout<<" : edge restriction kept not kept"<<std::endl;
//  }
#endif

  return keeprline;
}

Standard_EXPORT bool FUN_brep_sdmRE(const TopoDS_Edge& E1, const TopoDS_Edge& E2)
{ // prequesitory : E1, E2 are restriction edges of opposite rank
  //                found in the same FacesFiller
  bool              ok = false;
  BRepAdaptor_Curve BAC;
  TopoDS_Vertex     v1, v2;
  TopExp::Vertices(E1, v1, v2);
  TopoDS_Vertex v3, v4;
  TopExp::Vertices(E2, v3, v4);
  if (!ok)
  {
    BAC.Initialize(E1);
    double tol1 = BRep_Tool::Tolerance(E1);
    double tol2 = BRep_Tool::Tolerance(v3);
    double tol3 = BRep_Tool::Tolerance(v4);
    double tol4 = std::max(tol1, std::max(tol2, tol3));
    if (!ok)
    {
      const gp_Pnt& P3 = BRep_Tool::Pnt(v3);
      ok               = FUN_tool_PinC(P3, BAC, tol4);
    }
    if (!ok)
    {
      const gp_Pnt& P4 = BRep_Tool::Pnt(v4);
      ok               = FUN_tool_PinC(P4, BAC, tol4);
    }
  }
  if (!ok)
  {
    BAC.Initialize(E2);
    double tol1 = BRep_Tool::Tolerance(E2);
    double tol2 = BRep_Tool::Tolerance(v1);
    double tol3 = BRep_Tool::Tolerance(v2);
    double tol4 = std::max(tol1, std::max(tol2, tol3));
    if (!ok)
    {
      const gp_Pnt& P1 = BRep_Tool::Pnt(v1);
      ok               = FUN_tool_PinC(P1, BAC, tol4);
    }
    if (!ok)
    {
      const gp_Pnt& P2 = BRep_Tool::Pnt(v2);
      ok               = FUN_tool_PinC(P2, BAC, tol4);
    }
  }
  return ok;
}

//=================================================================================================

void TopOpeBRep_FacesFiller::ProcessSectionEdges()
{
  // recuperation des aretes d'intersection mapES
  // MSV: replace map with list to achieve predictable order of edges
  NCollection_List<TopoDS_Shape> LES;
  GetESL(LES);

  // add LES edges as section edges in the DS.
  NCollection_List<TopoDS_Shape>::Iterator itLES;
  for (itLES.Initialize(LES); itLES.More(); itLES.Next())
  {
    const TopoDS_Edge& E = TopoDS::Edge(itLES.Value());

    bool isdg = BRep_Tool::Degenerated(E); // xpu290698
    if (isdg)
      continue; // xpu290698

    myDS->AddSectionEdge(E);
    myDS->Shape(E);
    myDS->AncestorRank(E);
  }

  NCollection_List<int>           LOI;
  NCollection_List<int>::Iterator itLOI;

  // LOI = liste des rank (1 ou 2 ) des aretes de section (liste LES)
  for (itLES.Initialize(LES); itLES.More(); itLES.Next())
  {
    const TopoDS_Edge& ELES = TopoDS::Edge(itLES.Value());
    bool               is1  = false;
    bool               is2  = false;
    myFacesIntersector->InitLine();
    TopoDS_Edge ELI;
    for (; myFacesIntersector->MoreLine(); myFacesIntersector->NextLine())
    {
      TopOpeBRep_LineInter& L = myFacesIntersector->CurrentLine();
      if (L.TypeLineCurve() != TopOpeBRep_RESTRICTION)
        continue;
      ELI = TopoDS::Edge(L.Arc());
      if (ELI.IsEqual(ELES))
      {
        is1 = L.ArcIsEdge(1);
        is2 = L.ArcIsEdge(2);
        break;
      }
    }
    double toappend = true;
    if (toappend)
    {
      if (is1)
        LOI.Append(1);
      else if (is2)
        LOI.Append(2);
    }
  }

  // ajout des aretes de section dans la DS de shape,connaissant leur rank
  for (itLES.Initialize(LES), itLOI.Initialize(LOI); itLES.More() && itLOI.More();
       itLES.Next(), itLOI.Next())
  {
    const TopoDS_Shape& E1  = itLES.Value();
    int                 rE1 = itLOI.Value();
    myDS->AddShape(E1, rE1);
  }

  // determination des aretes SameDomain en 3d pur
  // mapELE(arete(1)) -> {arete(2)}
  // mapELE(arete(2)) -> {arete(1)}
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> mapELE;
  for (itLES.Initialize(LES); itLES.More(); itLES.Next())
  {
    const TopoDS_Edge& E1  = TopoDS::Edge(itLES.Value());
    int                iE1 = myDS->Shape(E1);
    int                rE1 = myDS->AncestorRank(iE1);
    if (rE1 != 1)
      continue;
    NCollection_List<TopoDS_Shape> thelist;
    mapELE.Bind(E1, thelist);

    NCollection_List<TopoDS_Shape>::Iterator itLES2;
    for (itLES2.Initialize(LES); itLES2.More(); itLES2.Next())
    {
      const TopoDS_Edge& E2  = TopoDS::Edge(itLES2.Value());
      int                iE2 = myDS->Shape(E2);
      int                rE2 = myDS->AncestorRank(iE2);
      if (rE2 == 0 || iE1 == iE2 || rE2 == rE1)
        continue;

      bool toappend = FUN_brep_sdmRE(E1, E2);
      if (toappend)
      {
        mapELE.ChangeFind(E1).Append(E2);
      }
    }
  }

  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
    Iterator itmapELE;

  for (itmapELE.Initialize(mapELE); itmapELE.More(); itmapELE.Next())
  {
    const TopoDS_Edge&                       E1     = TopoDS::Edge(itmapELE.Key());
    int                                      iE1    = myDS->Shape(E1);
    int                                      rE1    = myDS->AncestorRank(iE1);
    const TopoDS_Face&                       aFace1 = TopoDS::Face(myFacesIntersector->Face(rE1));
    bool                                     isClosing1 = BRep_Tool::IsClosed(E1, aFace1);
    NCollection_List<TopoDS_Shape>::Iterator itL(itmapELE.Value());
    for (; itL.More(); itL.Next())
    {
      const TopoDS_Edge& E2         = TopoDS::Edge(itL.Value());
      int                iE2        = myDS->Shape(E2);
      int                rE2        = myDS->AncestorRank(iE2);
      const TopoDS_Face& aFace2     = TopoDS::Face(myFacesIntersector->Face(rE2));
      bool               isClosing2 = BRep_Tool::IsClosed(E2, aFace2);
      bool               refFirst   = isClosing1 || !isClosing2;
      myDS->FillShapesSameDomain(E1,
                                 E2,
                                 TopOpeBRepDS_UNSHGEOMETRY,
                                 TopOpeBRepDS_UNSHGEOMETRY,
                                 refFirst);
    }
  }
}
