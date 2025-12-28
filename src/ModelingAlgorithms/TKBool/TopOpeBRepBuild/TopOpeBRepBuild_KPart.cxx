// Created on: 1994-08-30
// Created by: Jean Yves LEBEY
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

#include <BRepClass3d.hxx>
#include <BRepClass3d_SolidExplorer.hxx>
#include <BRepTools.hxx>
#include <gp_Pnt.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepBuild_GTool.hxx>
#include <TopOpeBRepBuild_GTopo.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepBuild_kpresu.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>
#include <TopOpeBRepDS_connex.hxx>
#include <TopOpeBRepDS_CurveIterator.hxx>
#include <TopOpeBRepDS_EXPORT.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_ShapeShapeInterference.hxx>
#include <TopOpeBRepTool.hxx>
#include <TopOpeBRepTool_GEOMETRY.hxx>
#include <TopOpeBRepTool_PROJECT.hxx>
#include <TopOpeBRepTool_TOPOLOGY.hxx>
#include <TopOpeBRepTool_SC.hxx>
#include <TopOpeBRepTool_ShapeExplorer.hxx>

#ifdef OCCT_DEBUG
extern bool TopOpeBRepBuild_GettraceKPB();
#endif

static void FUN_Raise()
{
#ifdef OCCT_DEBUG
  std::cout << "******************************ERROR" << std::endl;
  throw Standard_ProgramError("KPart.cxx");
#endif
}

#define M_REVERSED(st) (st == TopAbs_REVERSED)

Standard_EXPORT bool FUNKP_KPiskolesh(const TopOpeBRepBuild_Builder&    BU,
                                      const TopOpeBRepDS_DataStructure& BDS,
                                      const TopoDS_Shape&               Sarg,
                                      NCollection_List<TopoDS_Shape>&   lShsd,
                                      NCollection_List<TopoDS_Shape>&   lfhsd);

// modified by NIZHNY-MKK  Fri May 19 17:03:59 2000.BEGIN
enum TopOpeBRepBuild_KPart_Operation
{
  TopOpeBRepBuild_KPart_Operation_Fuse,
  TopOpeBRepBuild_KPart_Operation_Common,
  TopOpeBRepBuild_KPart_Operation_Cut12,
  TopOpeBRepBuild_KPart_Operation_Cut21
};

static void LocalKPisdisjanalyse(const TopAbs_State                     Stsol1,
                                 const TopAbs_State                     Stsol2,
                                 const TopOpeBRepBuild_KPart_Operation& theOperation,
                                 int&                                   ires,
                                 int&                                   icla1,
                                 int&                                   icla2);

static TopoDS_Solid BuildNewSolid(const TopoDS_Solid& sol1,
                                  const TopoDS_Solid& sol2,
                                  const TopAbs_State  stsol1,
                                  const TopAbs_State  stsol2,
                                  const int           ires,
                                  const int           icla1,
                                  const int           icla2,
                                  const TopAbs_State  theState1,
                                  const TopAbs_State  theState2);

static bool disjPerformFuse(
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMapOfSolid1,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMapOfSolid2,
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&       theMapOfResult);

static bool disjPerformCommon(
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMapOfSolid1,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMapOfSolid2,
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&       theMapOfResult);

static bool disjPerformCut(
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMapOfSolid1,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMapOfSolid2,
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&       theMapOfResult);

// modified by NIZHNY-MKK  Fri May 19 17:04:07 2000.END

//=================================================================================================

int TopOpeBRepBuild_Builder::FindIsKPart()
{
  KPClearMaps();

#ifdef OCCT_DEBUG
  bool TKPB = TopOpeBRepBuild_GettraceKPB();
  if (TKPB)
  {
    std::cout << std::endl << "--- IsKPart ? ---" << std::endl;
  }
#endif

  int isfafa = KPisfafa();
  // face,face SameDomain
  if (isfafa)
  {
    myIsKPart = 3;
    return KPreturn(myIsKPart);
  }

  int isdisj = KPisdisj();
  // shape,shape sans aucune interference geometrique
  if (isdisj)
  {
    myIsKPart = 2;
    return KPreturn(myIsKPart);
  }

  int iskole = KPiskole();
  // solide,solide colles par faces tangentes sans aretes tangentes
  if (iskole)
  {
    myIsKPart = 1;
    return KPreturn(myIsKPart);
  }

  int iskoletge = KPiskoletge();
  // solide,solide par faces tangentes avec aretes tangentes
  if (iskoletge)
  {
    myIsKPart = 5;
    return KPreturn(myIsKPart);
  }

  int issoso = KPissoso();
  // solide,solide quelconques
  if (issoso)
  {
    myIsKPart = 4;
    return KPreturn(myIsKPart);
  }

  myIsKPart = 0;
  return KPreturn(myIsKPart);
}

//=================================================================================================

int TopOpeBRepBuild_Builder::IsKPart() const
{
  return myIsKPart;
}

//=================================================================================================

void TopOpeBRepBuild_Builder::MergeKPart(const TopAbs_State TB1, const TopAbs_State TB2)
{
  myState1 = TB1;
  myState2 = TB2;
  MergeKPart();
}

//=================================================================================================

void TopOpeBRepBuild_Builder::MergeKPart()
{
  if (myIsKPart == 1)
  { // iskole
    MergeKPartiskole();
  }
  else if (myIsKPart == 5)
  { // iskoletge
    MergeKPartiskoletge();
  }
  else if (myIsKPart == 2)
  { // isdisj
    MergeKPartisdisj();
  }
  else if (myIsKPart == 3)
  { // isfafa
    MergeKPartisfafa();
  }
  else if (myIsKPart == 4)
  { // issoso
    MergeKPartissoso();
  }
  End();
}

static void FUN_sortplcy(const NCollection_List<TopoDS_Shape>& lof,
                         NCollection_List<TopoDS_Shape>&       lopl,
                         NCollection_List<TopoDS_Shape>&       locy)
{
  NCollection_List<TopoDS_Shape>::Iterator it(lof);
  for (; it.More(); it.Next())
  {
    const TopoDS_Face& ff    = TopoDS::Face(it.Value());
    bool               plane = FUN_tool_plane(ff);
    if (plane)
    {
      lopl.Append(ff);
    }
    bool cylinder = FUN_tool_cylinder(ff);
    if (cylinder)
    {
      locy.Append(ff);
    }
  }
}

/*static bool FUN_proj2(const TopOpeBRepBuild_Builder& BU, const
occ::handle<TopOpeBRepDS_HDataStructure>& HDS, const TopoDS_Shape& Fa2,
NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& EnewE)
{
  const TopoDS_Face& F2 = TopoDS::Face(Fa2);
  TopoDS_Wire Ow2 = BRepTools::OuterWire(TopoDS::Face(F2));
  TopExp_Explorer exe(Ow2, TopAbs_EDGE);
  for (; exe.More(); exe.Next()){
    const TopoDS_Shape& ed = exe.Current();
    bool issplit = BU.IsSplit(ed,TopAbs_ON);
    if (!issplit) return false;
    const NCollection_List<TopoDS_Shape>& speds = BU.Splits(ed,TopAbs_ON);
    NCollection_List<TopoDS_Shape> lfcF2; // faces of shapei connexed to ed
    FDSCNX_FaceEdgeConnexFaces(TopoDS::Face(F2),ed,HDS,lfcF2);
    // prequesitory : ed in {edges of Ow2}
    //                ed's faces ancestor = {F2,fofj}
    if (lfcF2.Extent() != 1) return false;
    const TopoDS_Face& fcF2 = TopoDS::Face(lfcF2.First());

    // projecting sp(ed) on faces F2 and fofj
    NCollection_List<TopoDS_Shape> newesp;
    NCollection_List<TopoDS_Shape>::Iterator itspe(speds);
    for (; itspe.More(); itspe.Next()){
      TopoDS_Edge esp = TopoDS::Edge(itspe.Value());
      bool ok = FUN_tool_pcurveonF(F2,esp);
      if (!ok) return false;
      ok     = FUN_tool_pcurveonF(fcF2,esp);
      if (!ok) return false;
//      EnewE.Add(esp,newesp);
      newesp.Append(esp);
    }
    EnewE.Bind(ed,newesp);
  }
  return true;
}*/

static void FUN_addf(const TopAbs_State  sta,
                     const TopoDS_Shape& ftoadd,
                     NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& map)
{
#ifdef OCCT_DEBUG
//  bool isadded = map.IsBound(ftoadd);
#endif
  TopoDS_Shape fori = ftoadd;
  if (sta == TopAbs_IN)
    fori.Complement();
  map.Bind(fori, fori);
}

static int FUN_comparekoletgesh(TopOpeBRepTool_ShapeClassifier& SC,
                                const TopoDS_Shape&             sh1,
                                const TopoDS_Shape&             sh2,
                                const TopoDS_Shape&             fkole1,
                                const TopoDS_Shape&)
// purpose: <sh1> and <sh2> are kpkoletge on faces <fkole1>,<fkole2>
//          with <fkole1> same oriented with <fkole2>
//          returns k=1,2 if shi is contained in shk
//          else returns 0
{
  SC.SetReference(sh2);
  TopExp_Explorer exf(sh1, TopAbs_FACE);
  for (; exf.More(); exf.Next())
  {
    const TopoDS_Face& f1 = TopoDS::Face(exf.Current());
    if (f1.IsSame(fkole1))
      continue;
    gp_Pnt pnt1;
    BRepClass3d_SolidExplorer::FindAPointInTheFace(f1, pnt1);
    SC.StateP3DReference(pnt1);
    TopAbs_State stpnt1 = SC.State();
    if (stpnt1 == TopAbs_IN)
      return 2;
    if (stpnt1 == TopAbs_OUT)
      return 1;
  }
  return 0;
}

static bool FUN_changev(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS, const TopoDS_Shape& v)
{
  bool changev = HDS->HasShape(v);
  if (!changev)
    return false;
  changev = HDS->HasSameDomain(v);
  if (!changev)
    return false;
  int rankv = HDS->DS().AncestorRank(v);
  changev   = (rankv == 2);
  return changev;
}

static bool FUN_updatev(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS,
                        TopoDS_Edge&                                    newed,
                        const TopoDS_Vertex&                            v,
                        const TopAbs_Orientation                        oriv,
                        const double                                    parv,
                        const bool                                      changev)
{
  TopOpeBRepDS_BuildTool BT;
  BRep_Builder           BB;
  if (changev)
  {
    TopoDS_Shape oov;
    bool         ok = FUN_ds_getoov(v, HDS, oov);
    if (!ok)
      return false;
    oov.Orientation(oriv);
    BB.Add(newed, oov);
    BT.Parameter(newed, oov, parv);
  }
  else
  {
    TopoDS_Shape  aLocalShape = v.Oriented(oriv);
    TopoDS_Vertex ov          = TopoDS::Vertex(aLocalShape);
    //    TopoDS_Vertex ov = TopoDS::Vertex(v.Oriented(oriv));
    BB.Add(newed, ov);
    BT.Parameter(newed, ov, parv);
  }
  return true;
}

static bool FUN_makefaces(
  const TopOpeBRepBuild_Builder&                                                              BU,
  const occ::handle<TopOpeBRepDS_HDataStructure>&                                             HDS,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& EnewE,
  const TopoDS_Shape&                                                                         f,
  const TopoDS_Shape&,
  NCollection_List<TopoDS_Shape>& newfaces)
// prequesitory : <outerwi>=<f>'s outer wire
// purpose      : <outerwi>={edow},
//                edow=(vf,vl), if(rank(vj)=2 && vj sdm vj1), replace vj by vj1
//  <f> gives <newf>

{
  TopOpeBRepDS_BuildTool BT;
  BRep_Builder           BB;
  TopoDS_Shape           aLocalShape = f.Oriented(TopAbs_FORWARD);
  TopoDS_Face            F           = TopoDS::Face(aLocalShape); // working on FORWARD face
  //  TopoDS_Face F = TopoDS::Face(f.Oriented(TopAbs_FORWARD)); // working on FORWARD face
  TopAbs_Orientation of = f.Orientation();

  // the new outer wire :
  //  -------------------
  NCollection_List<TopoDS_Shape> loe;
  // -------
  TopoDS_Wire     outerwf = BRepTools::OuterWire(F);
  TopExp_Explorer ex(outerwf, TopAbs_EDGE);
  for (; ex.More(); ex.Next())
  {
    const TopoDS_Edge& ed      = TopoDS::Edge(ex.Current());
    TopAbs_Orientation oe      = ed.Orientation();
    bool               issplit = BU.IsSplit(ed, TopAbs_ON);
    bool               isbound = EnewE.IsBound(ed);

    if (!isbound && !issplit)
    {
      // new edge
      TopoDS_Vertex vf, vl;
      TopExp::Vertices(ed, vf, vl);
      bool changevf = ::FUN_changev(HDS, vf);
      bool changevl = ::FUN_changev(HDS, vl);
      bool changee  = changevf || changevl;
      if (changee)
      {
        double ff = BRep_Tool::Parameter(vf, ed);
        double l  = BRep_Tool::Parameter(vl, ed);

        TopoDS_Edge newed;
        BT.CopyEdge(ed.Oriented(TopAbs_FORWARD), newed);
        bool ok = ::FUN_updatev(HDS, newed, vf, TopAbs_FORWARD, ff, changevf);
        if (!ok)
          return false;
        ok = ::FUN_updatev(HDS, newed, vl, TopAbs_REVERSED, l, changevl);
        if (!ok)
          return false;
        newed.Orientation(oe);
        NCollection_List<TopoDS_Shape> led;
        led.Append(newed);
        EnewE.Bind(ed, led);
        isbound = true;
      }
    }

    if (issplit)
    {
      const NCollection_List<TopoDS_Shape>& speds = BU.Splits(ed, TopAbs_ON);
      if (speds.Extent() == 0)
        return false;
      const TopoDS_Edge& spe     = TopoDS::Edge(speds.First());
      bool               sameori = FUN_tool_SameOri(ed, spe);
      TopAbs_Orientation orisp   = spe.Orientation();
      if (!sameori)
        orisp = TopAbs::Complement(orisp);

      NCollection_List<TopoDS_Shape>::Iterator it(speds);
      for (; it.More(); it.Next())
      {
        TopoDS_Edge esp = TopoDS::Edge(it.Value());
        bool        ok  = FUN_tool_pcurveonF(TopoDS::Face(f), esp);
        if (!ok)
          return false;
        TopoDS_Shape ee = esp.Oriented(orisp);
        loe.Append(ee);
      }
    }
    else if (isbound)
    {
      const NCollection_List<TopoDS_Shape>&    neweds = EnewE.ChangeFind(ed);
      NCollection_List<TopoDS_Shape>::Iterator itnes(neweds);
      for (; itnes.More(); itnes.Next())
        loe.Append(itnes.Value().Oriented(oe));
    }
    else
      loe.Append(ed);
  }

  TopoDS_Wire newW;
  //---------------
  BB.MakeWire(newW);
  for (NCollection_List<TopoDS_Shape>::Iterator itee(loe); itee.More(); itee.Next())
    BB.Add(newW, itee.Value());

  // the new face :
  // --------------
  aLocalShape      = F.EmptyCopied();
  TopoDS_Face newf = TopoDS::Face(aLocalShape);
  //  TopoDS_Face newf = TopoDS::Face(F.EmptyCopied());
  BB.Add(newf, newW);

  // other wires of <f> :
  TopExp_Explorer exw(F, TopAbs_WIRE);
  for (; exw.More(); exw.Next())
  {
    const TopoDS_Wire OOw = TopoDS::Wire(exw.Current());
    if (OOw.IsSame(outerwf))
      continue;
    BB.Add(newf, OOw);
  }

  if (M_REVERSED(of))
    newf.Orientation(TopAbs_REVERSED);

  // xpu140898 : CTS21251
  TopoDS_Face Newf = newf;
  bool        ok   = TopOpeBRepTool::CorrectONUVISO(TopoDS::Face(f), Newf);
  if (ok)
    newfaces.Append(Newf);
  else
    newfaces.Append(newf);

  return true;
}

static bool FUN_rebuildfc(
  const TopOpeBRepBuild_Builder&                  BU,
  const occ::handle<TopOpeBRepDS_HDataStructure>& HDS,
  const TopAbs_State,
  const TopoDS_Shape&                                                                         Fk,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& EnewE,
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    fcnewfc)
//              Owk = <Fk>'s outer wire
// prequesitory: Owk = {edk}, Splits(edk,Stk) = spedk
// purpose: fills up <fcnewfc> = {(fcFk,newfcFk)}
//         with {fcFk} = faces of shape k connexed to Owk
//         fcFk has edges {edk}
{
#ifdef OCCT_DEBUG
//  const TopOpeBRepDS_DataStructure& BDS = HDS->DS();
//  int rFk = BDS.AncestorRank(Fk);
#endif
  TopoDS_Wire                    Owk = BRepTools::OuterWire(TopoDS::Face(Fk));
  NCollection_List<TopoDS_Shape> eds;
  FUN_tool_shapes(Owk, TopAbs_EDGE, eds);
  NCollection_List<TopoDS_Shape>::Iterator ite(eds);

  ite.Initialize(eds);
  for (; ite.More(); ite.Next())
  {
    const TopoDS_Shape&            ed = ite.Value();
    NCollection_List<TopoDS_Shape> lfcFk; // faces of shapei connexed to ed
    FDSCNX_FaceEdgeConnexFaces(TopoDS::Face(Fk), ed, HDS, lfcFk);
    // prequesitory : ed in {edges of Owk}
    //                ed's faces ancestor = {Fk,fofj}
    if (lfcFk.Extent() != 1)
      return false;

    // purpose : Building up new topologies on connexed faces
    //           attached to outer wire's edges.
    const TopoDS_Shape&            fcFk = lfcFk.First();
    NCollection_List<TopoDS_Shape> newfcFk;
    bool                           ok = FUN_makefaces(BU, HDS, EnewE, fcFk, Owk, newfcFk);
    if (!ok)
      return false;
    fcnewfc.Add(fcFk, newfcFk);
  }
  return true;
} // FUN_rebuildfc

#ifdef OCCT_DEBUG
Standard_EXPORT void debiskoletge() {}
#endif

//=================================================================================================

void TopOpeBRepBuild_Builder::MergeKPartiskoletge()
{
#ifdef OCCT_DEBUG
  bool TKPB = TopOpeBRepBuild_GettraceKPB();
  if (TKPB)
    KPreturn(myIsKPart);
  debiskoletge();
#endif

  const TopOpeBRepDS_DataStructure& BDS = myDataStructure->DS();
  //  int ibid;

  if (myIsKPart != 5)
  {
    FUN_Raise();
    return;
  }

  GMapShapes(myShape1, myShape2);
  // NYI : on doit pouvoir faire l'economie du mapping GMapShapes(...)
  // NYI en allant chercher l'indice 1,2 retourne par GShapeRank(S)
  // NYI dans la DS. l'index est defini pour tous les shapes HasSameDomain

  NCollection_List<TopoDS_Shape>& lmergesha1 = ChangeMerged(myShape1, myState1);
  ChangeMerged(myShape2, myState2);

  NCollection_List<TopoDS_Shape> lShsd1, lShsd2; // liste de solides HasSameDomain
  NCollection_List<TopoDS_Shape> lfhsd1, lfhsd2; // liste de faces HasSameDomain
  KPiskoletgesh(myShape1, lShsd1, lfhsd1);
  KPiskoletgesh(myShape2, lShsd2, lfhsd2);

  // traitement de tous les solides NYI
  TopoDS_Shape sol1 = lShsd1.First();
  TopoDS_Shape sol2 = lShsd2.First();

  ChangeMerged(sol1, myState1);
  ChangeMerged(sol2, myState2);

  NCollection_List<TopoDS_Shape> lplhsd1, lcyhsd1;
  ::FUN_sortplcy(lfhsd1, lplhsd1, lcyhsd1);
  NCollection_List<TopoDS_Shape> lplhsd2, lcyhsd2;
  ::FUN_sortplcy(lfhsd2, lplhsd2, lcyhsd2);
  const TopoDS_Face& fac1 = TopoDS::Face(lplhsd1.First());
  const TopoDS_Face& fac2 = TopoDS::Face(lplhsd2.First());
#ifdef OCCT_DEBUG
  int iF1 =
#endif
    myDataStructure->Shape(fac1); // DEB
#ifdef OCCT_DEBUG
  int iF2 =
#endif
    myDataStructure->Shape(fac2); // DEB

#ifdef OCCT_DEBUG
  if (TKPB)
  {
    std::cout << "" << std::endl;
    std::cout << "face " << iF1 << " : ";
    std::cout << iF2 << std::endl;
  }
#endif

  TopOpeBRepDS_Config config2 = BDS.SameDomainOri(fac2);

  bool SameOriented = (config2 == TopOpeBRepDS_SAMEORIENTED);

  const TopoDS_Shape* pfGRE = nullptr;
  const TopoDS_Shape* pfSMA = nullptr;

  int rgre = 1;
  if (SameOriented)
  {
    // getting greater
    rgre = ::FUN_comparekoletgesh(myShapeClassifier, myShape1, myShape2, fac1, fac2);
    if (rgre == 0)
      rgre = FUN_tool_comparebndkole(myShape1, myShape2);
  }
  if (rgre == 0)
  {
    FUN_Raise();
    return;
  }

  // ires :
  //-------
  int rsma            = (rgre == 1) ? 2 : 1;
  pfSMA               = (rsma == 1) ? &fac1 : &fac2;
  pfGRE               = (rgre == 1) ? &fac1 : &fac2;
  TopAbs_State staSMA = (rsma == 1) ? myState1 : myState2;
  TopAbs_State staGRE = (rgre == 1) ? myState1 : myState2;

  TopoDS_Shape shaSMA = (rsma == 1) ? myShape1 : myShape2;
  TopoDS_Shape shaGRE = (rgre == 1) ? myShape1 : myShape2;

  int ires = 0;
  KPiskoletgeanalyse(config2, staSMA, staGRE, ires);

  // merge :
  //-------
  TopoDS_Shape sheSMA; // sheSMA = shell accedant facSMA
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    MfacsheSMA;
  TopExp::MapShapesAndAncestors(shaSMA, TopAbs_FACE, TopAbs_SHELL, MfacsheSMA);
  const NCollection_List<TopoDS_Shape>&    lsheSMA = MfacsheSMA.FindFromKey(*pfSMA);
  NCollection_List<TopoDS_Shape>::Iterator itlsheSMA(lsheSMA);
  sheSMA = itlsheSMA.Value();

  TopoDS_Shape sheGRE; // sheGRE = shell accedant facGRE
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    MfacsheGRE;
  TopExp::MapShapesAndAncestors(shaGRE, TopAbs_FACE, TopAbs_SHELL, MfacsheGRE);
  const NCollection_List<TopoDS_Shape>&    lsheGRE = MfacsheGRE.FindFromKey(*pfGRE);
  NCollection_List<TopoDS_Shape>::Iterator itlsheGRE(lsheGRE);
  sheGRE = itlsheGRE.Value();

  ChangeMerged(sheSMA, staSMA);
  ChangeMerged(sheGRE, staGRE);

  TopoDS_Shell newshe;
  if (ires == RESNULL)
  {
    return;
  }
  else if (ires == RESSHAPE1)
  {
    myBuildTool.MakeShell(newshe);
    newshe = TopoDS::Shell(sheSMA);
  }
  else if (ires == RESSHAPE2)
  {
    myBuildTool.MakeShell(newshe);
    newshe = TopoDS::Shell(sheGRE);
  }
  else if (ires == RESNEWSOL)
  {

    NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> addedfaces;
    // As splits of outer wire's edges have 2drep only on shape1,
    // we have to project them on the connexed faces of shape2
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
      EnewE;
    //    bool ok = ::FUN_proj2((*this),myDataStructure,fac2,EnewE);
    //    if (!ok) {FUN_Raise(); return;}

    // new topologies :
    // clang-format off
    NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> fcnewfcSMA;// faces connexed to fSMA built up with the split of outerwSMA
    NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> fcnewfcGRE;// faces connexed to fGRE built up with the split of outerwGRE
    // clang-format on
    bool ok = ::FUN_rebuildfc((*this), myDataStructure, staSMA, *pfSMA, EnewE, fcnewfcSMA);
    if (!ok)
    {
      FUN_Raise();
      return;
    }
    int nfcSMA = fcnewfcSMA.Extent();
    //    for (int i=1; i<=nfcSMA; i++) {
    int i;
    for (i = 1; i <= nfcSMA; i++)
    {
      const TopoDS_Shape&                   f     = fcnewfcSMA.FindKey(i);
      const NCollection_List<TopoDS_Shape>& newlf = fcnewfcSMA.FindFromIndex(i);

      NCollection_List<TopoDS_Shape>::Iterator it(newlf);
      for (; it.More(); it.Next())
      {
        const TopoDS_Shape& ff = it.Value();
        ::FUN_addf(staSMA, ff, addedfaces);
        ChangeMerged(f, staSMA).Append(ff);
      }
    } // fcnewfcSMA
    ok = ::FUN_rebuildfc((*this), myDataStructure, staGRE, *pfGRE, EnewE, fcnewfcGRE);
    if (!ok)
    {
      FUN_Raise();
      return;
    }
    int nfcGRE = fcnewfcGRE.Extent();
    for (i = 1; i <= nfcGRE; i++)
    {
      const TopoDS_Shape&                   f     = fcnewfcGRE.FindKey(i);
      const NCollection_List<TopoDS_Shape>& newlf = fcnewfcGRE.FindFromIndex(i);

      NCollection_List<TopoDS_Shape>::Iterator it(newlf);
      for (; it.More(); it.Next())
      {
        const TopoDS_Shape& ff = it.Value();
        ::FUN_addf(staGRE, ff, addedfaces);
        ChangeMerged(f, staGRE).Append(ff);
      }
    } // fcnewfcGRE

    // old topologies :
    NCollection_List<TopoDS_Shape> lOOfSMA; // DEB : faces of SMA non connexed to fSMA
    NCollection_List<TopoDS_Shape> lOOfGRE; // DEB : faces of GRE non connexed to fGRE
    TopExp_Explorer                exSMA(shaSMA, TopAbs_FACE);
    for (; exSMA.More(); exSMA.Next())
    {
      const TopoDS_Shape& ff = exSMA.Current();
      if (fcnewfcSMA.Contains(ff))
        continue;
      if (ff.IsSame(*pfSMA))
        continue;
      lOOfSMA.Append(ff); // DEB
      ::FUN_addf(staSMA, ff, addedfaces);
    }
    TopExp_Explorer exGRE(shaGRE, TopAbs_FACE);
    for (; exGRE.More(); exGRE.Next())
    {
      const TopoDS_Shape& ff = exGRE.Current();
      if (fcnewfcGRE.Contains(ff))
        continue;
      if (ff.IsSame(*pfGRE))
        continue;
      lOOfGRE.Append(ff); // DEB
      ::FUN_addf(staGRE, ff, addedfaces);
    }

    // newshell :
    NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator itadd(
      addedfaces);
    bool yauadd = itadd.More();
    if (yauadd)
    {
      myBuildTool.MakeShell(newshe);
      myBuildTool.Closed(newshe, true); // NYI : check exact du caractere closed du shell
    }
    for (; itadd.More(); itadd.Next())
    {
      const TopoDS_Shape& ftoadd = itadd.Key();
      myBuildTool.AddShellFace(newshe, ftoadd);
    }

  } // RESNEWSOL

  TopoDS_Solid newsol;
  if (!newshe.IsNull())
  {
    myBuildTool.MakeSolid(newsol);
    myBuildTool.AddSolidShell(newsol, newshe);
  }

  // le solide final
  if (!newsol.IsNull())
  {
    lmergesha1.Append(newsol);
  }

} // MergeKPartiskoletge

//=================================================================================================

void TopOpeBRepBuild_Builder::MergeKPartisdisj()
{
#ifdef OCCT_DEBUG
  bool TKPB = TopOpeBRepBuild_GettraceKPB();
  if (TKPB)
    KPreturn(myIsKPart);
#endif

  if (myIsKPart != 2)
    return; // isdisj

  NCollection_List<TopoDS_Shape>& lmergesha1 = ChangeMerged(myShape1, myState1);
  /*  NCollection_List<TopoDS_Shape>& lmergesha2 =*/ChangeMerged(myShape2, myState2);

  bool                         soldisj = false;
  TopOpeBRepTool_ShapeExplorer exsol1(myShape1, TopAbs_SOLID);
  bool                         hassol1 = exsol1.More();
  TopOpeBRepTool_ShapeExplorer exsol2(myShape2, TopAbs_SOLID);
  bool                         hassol2 = exsol2.More();
  soldisj                              = (hassol1 && hassol2);

  // modified by NIZHNY-MKK  Fri May 19 16:18:12 2000.BEGIN
  bool            hasnotsol1 = false;
  bool            hasnotsol2 = false;
  TopExp_Explorer anExp(myShape1, TopAbs_SHELL, TopAbs_SOLID);
  for (int i = TopAbs_SHELL; i <= TopAbs_VERTEX && !hasnotsol1 && !hasnotsol2; i++)
  {
    anExp.Init(myShape1, (TopAbs_ShapeEnum)i, TopAbs_SOLID);
    if (anExp.More())
      hasnotsol1 = true;
    anExp.Init(myShape2, (TopAbs_ShapeEnum)i, TopAbs_SOLID);
    if (anExp.More())
      hasnotsol2 = true;
  }
  soldisj = !(hasnotsol1 || hasnotsol2);
  // modified by NIZHNY-MKK  Fri May 19 16:18:16 2000.END

  TopoDS_Solid sol1;
  TopoDS_Shell outsha1;
  TopoDS_Solid sol2;
  TopoDS_Shell outsha2;

  if (soldisj)
  {
    // modified by NIZHNY-MKK  Fri May 19 16:47:19 2000.BEGIN
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMapOfSolid1, aMapOfSolid2;
    TopExp::MapShapes(myShape1, TopAbs_SOLID, aMapOfSolid1);
    TopExp::MapShapes(myShape2, TopAbs_SOLID, aMapOfSolid2);

    if (aMapOfSolid1.Extent() > 1 || aMapOfSolid2.Extent() > 1)
    {
      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMapOfResult;
      if (Opefus())
      {
        if (!disjPerformFuse(aMapOfSolid1, aMapOfSolid2, aMapOfResult))
          return;
      }
      else if (Opec12())
      {
        if (!disjPerformCut(aMapOfSolid1, aMapOfSolid2, aMapOfResult))
          return;
      }
      else if (Opec21())
      {
        if (!disjPerformCut(aMapOfSolid2, aMapOfSolid1, aMapOfResult))
          return;
      }
      else if (Opecom())
      {
        if (!disjPerformCommon(aMapOfSolid1, aMapOfSolid2, aMapOfResult))
          return;
      }
      for (int ii = 1; ii <= aMapOfResult.Extent(); ii++)
      {
        lmergesha1.Append(aMapOfResult(ii));
      }
      return;
    } // end if(aMapOfSolid1.Extent() > 1 || aMapOfSolid2.Extent() > 1)
    else
    {
      // modified by NIZHNY-MKK  Fri May 19 16:47:23 2000.END
      sol1 = TopoDS::Solid(exsol1.Current());
      ChangeMerged(sol1, myState1);
      outsha1 = BRepClass3d::OuterShell(sol1);

      sol2 = TopoDS::Solid(exsol2.Current());
      ChangeMerged(sol2, myState2);
      outsha2 = BRepClass3d::OuterShell(sol2);

      TopAbs_State stsol1 = KPclasSS(outsha1, sol2);
      TopAbs_State stsol2 = KPclasSS(outsha2, sol1);

      int ires, icla1, icla2;
      KPisdisjanalyse(stsol1, stsol2, ires, icla1, icla2);

      if (ires == RESUNDEF)
      {
        return;
      }

      else if (icla1 == SHEUNDEF || icla2 == SHEUNDEF)
      {
        return;
      }

      else if (ires == RESNULL)
      {
        return;
      }

      else if (ires == RESSHAPE12)
      {
        lmergesha1.Append(myShape1);
        lmergesha1.Append(myShape2);
        return;
      }

      else if (ires == RESSHAPE1)
      {
        lmergesha1.Append(myShape1);
        return;
      }

      else if (ires == RESSHAPE2)
      {
        lmergesha1.Append(myShape2);
        return;
      }

      else if (ires == RESNEWSHA1 || ires == RESNEWSHA2)
      {
        // modified by NIZHNY-MKK  Tue May 23 11:36:33 2000.BEGIN
        TopoDS_Solid newsol =
          BuildNewSolid(sol1, sol2, stsol1, stsol2, ires, icla1, icla2, myState1, myState2);
        // modified by NIZHNY-MKK  Tue May 23 11:36:39 2000.END
        lmergesha1.Append(newsol);
        return;
      }
      else
      {
#ifdef OCCT_DEBUG
        std::cout << "TopOpeBRepBuild_MergeKPart soldisj : ires = " << ires << std::endl;
#endif
        return;
      }
      // modified by NIZHNY-MKK  Tue May 23 11:37:15 2000
    } // end else of if(aMapOfSolid1.Extent() > 1 || aMapOfSolid2.Extent() > 1)

  } // if (soldisj)
  else
  { //

    if (Opec12())
    {
      lmergesha1.Append(myShape1);
    }
    else if (Opec21())
    {
      lmergesha1.Append(myShape2);
    }
    else if (Opecom())
    {
      lmergesha1.Clear();
    }
    else if (Opefus())
    {
      lmergesha1.Append(myShape1);
      lmergesha1.Append(myShape2);
    }
    else
      return;

  } // ( !soldisj )

  return;

} // MergeKPartisdisj

//=================================================================================================

void TopOpeBRepBuild_Builder::MergeKPartisfafa()
{
#ifdef OCCT_DEBUG
  bool TKPB = TopOpeBRepBuild_GettraceKPB();
  if (TKPB)
    KPreturn(myIsKPart);
#endif

  if (myIsKPart == 3)
  { // isfafa

    TopExp_Explorer ex;
    ex.Init(myShape1, TopAbs_FACE);
    if (!ex.More())
      return;
    TopoDS_Shape F1 = ex.Current();
    ex.Init(myShape2, TopAbs_FACE);
    if (!ex.More())
      return;
    TopoDS_Shape F2 = ex.Current();

    NCollection_List<TopoDS_Shape> LF1, LF2;
    GFindSamDom(F1, LF1, LF2);

    TopAbs_ShapeEnum      tf = TopAbs_FACE;
    TopOpeBRepBuild_GTopo G;
    if (Opec12())
      G = TopOpeBRepBuild_GTool::GCutSame(tf, tf);
    else if (Opec21())
      G = TopOpeBRepBuild_GTool::GCutSame(tf, tf).CopyPermuted();
    else if (Opecom())
      G = TopOpeBRepBuild_GTool::GComSame(tf, tf);
    else if (Opefus())
      G = TopOpeBRepBuild_GTool::GFusSame(tf, tf);
    else
      return;

    GMapShapes(myShape1, myShape2);
    GMergeFaces(LF1, LF2, G);

    if (myShape1.ShapeType() == TopAbs_COMPOUND)
    {
      NCollection_List<TopoDS_Shape>& L1 = ChangeMerged(myShape1, myState1);
      L1                                 = ChangeMerged(F1, myState1);
    }

    if (myShape2.ShapeType() == TopAbs_COMPOUND)
    {
      NCollection_List<TopoDS_Shape>& L2 = ChangeMerged(myShape2, myState2);
      L2                                 = ChangeMerged(F2, myState2);
    }
  }

} // MergeKPartisfafa

//=================================================================================================

void TopOpeBRepBuild_Builder::MergeKPartissoso()
{
#ifdef OCCT_DEBUG
  bool TKPB = TopOpeBRepBuild_GettraceKPB();
  if (TKPB)
    KPreturn(myIsKPart);
#endif

  if (myIsKPart == 4)
  { // issoso

    TopExp_Explorer ex;

    TopoDS_Shape SO1;
    if (!myShape1.IsNull())
    {
      ex.Init(myShape1, TopAbs_SOLID);
      if (!ex.More())
        return;
      SO1 = ex.Current();
    }

    TopoDS_Shape SO2;
    if (!myShape2.IsNull())
    {
      ex.Init(myShape2, TopAbs_SOLID);
      if (!ex.More())
        return;
      SO2 = ex.Current();
    }

    if (SO1.IsNull())
      return;

    NCollection_List<TopoDS_Shape> LSO1, LSO2;
    GFindSamDom(SO1, LSO1, LSO2);

    TopAbs_ShapeEnum      tf = TopAbs_FACE; // NYI TopAbs_SOLID
    TopOpeBRepBuild_GTopo G;
    if (Opec12())
      G = TopOpeBRepBuild_GTool::GCutSame(tf, tf);
    else if (Opec21())
      G = TopOpeBRepBuild_GTool::GCutSame(tf, tf).CopyPermuted();
    else if (Opecom())
      G = TopOpeBRepBuild_GTool::GComSame(tf, tf);
    else if (Opefus())
      G = TopOpeBRepBuild_GTool::GFusSame(tf, tf);
    else
      return;

    GMapShapes(myShape1, myShape2);
    GMergeSolids(LSO1, LSO2, G);

    if (!myShape1.IsNull())
    {
      if (myShape1.ShapeType() == TopAbs_COMPOUND)
      {
        NCollection_List<TopoDS_Shape>& L1 = ChangeMerged(myShape1, myState1);
        L1                                 = ChangeMerged(SO1, myState1);
      }
    }

    if (!myShape2.IsNull())
    {
      if (myShape2.ShapeType() == TopAbs_COMPOUND)
      {
        NCollection_List<TopoDS_Shape>& L2 = ChangeMerged(myShape2, myState2);
        L2                                 = ChangeMerged(SO2, myState2);
      }
    }
  }

} // MergeKPartissoso

static bool sectionedgesON(
  const occ::handle<TopOpeBRepDS_HDataStructure>&                      HDS,
  const TopoDS_Shape&                                                  outerw1,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& mape2)
// prequesitory : all edges of <outerw1> are section edges
{
  TopExp_Explorer ex1(outerw1, TopAbs_EDGE);
  for (; ex1.More(); ex1.Next())
  {
    const TopoDS_Shape&                      e1  = ex1.Current();
    NCollection_List<TopoDS_Shape>::Iterator it2 = HDS->SameDomain(e1);
    if (!it2.More())
      return false; // xpu231098 : cto904C7 : e1 !hsd
    for (; it2.More(); it2.Next())
    {
      const TopoDS_Shape& e2      = it2.Value();
      bool                isbound = mape2.Contains(e2);
      if (!isbound)
        return false;
    }
  }
  return true;
}

static bool allIonsectionedges(
  const occ::handle<TopOpeBRepDS_HDataStructure>&                      HDS,
  const TopoDS_Shape&                                                  f1,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& mape1,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& mape2)
// prequesitory : all interferences attached to <f1> are SSI
{
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it1(
    HDS->DS().ShapeInterferences(f1));
  for (; it1.More(); it1.Next())
  {
    occ::handle<TopOpeBRepDS_ShapeShapeInterference> SSI1(
      occ::down_cast<TopOpeBRepDS_ShapeShapeInterference>(it1.Value()));
    int                 G1       = SSI1->Geometry();
    bool                isgbound = SSI1->GBound();
    const TopoDS_Shape& e1       = HDS->Shape(G1);
    bool                isbound  = isgbound ? mape1.Contains(e1) : mape2.Contains(e1);
    if (!isbound)
      return false;
  }
  return true;
}

//=======================================================================
// function : KPiskoletge
// purpose  : detection faces collees tangentes sur wire exterieur
//=======================================================================

int TopOpeBRepBuild_Builder::KPiskoletge()
{
  /*#ifdef OCCT_DEBUG
    bool TKPB = TopOpeBRepBuild_GettraceKPB();
  #endif*/

  NCollection_List<TopoDS_Shape> lShsd1, lShsd2; // liste de solides HasSameDomain
  NCollection_List<TopoDS_Shape> lfhsd1, lfhsd2; // liste de faces HasSameDomain

  bool iskp1 = KPiskoletgesh(myShape1, lShsd1, lfhsd1);
  if (!iskp1)
    return 0;
  NCollection_List<TopoDS_Shape> lplhsd1, lcyhsd1;
  ::FUN_sortplcy(lfhsd1, lplhsd1, lcyhsd1);
  int nplhsd1 = lplhsd1.Extent();
  int ncyhsd1 = lcyhsd1.Extent();
  if (nplhsd1 != 1)
    return 0;
  if (ncyhsd1 > 1)
    return 0;

  bool iskp2 = KPiskoletgesh(myShape2, lShsd2, lfhsd2);
  if (!iskp2)
    return 0;
  NCollection_List<TopoDS_Shape> lplhsd2, lcyhsd2;
  ::FUN_sortplcy(lfhsd2, lplhsd2, lcyhsd2);
  int nplhsd2 = lplhsd2.Extent();
  int ncyhsd2 = lcyhsd2.Extent();
  if (nplhsd2 != 1)
    return 0;

  // Si l'un des objets est constitue de plusieurs solides on passe
  // dans le cas general.
  int nshsd1 = lShsd1.Extent();
  int nshsd2 = lShsd2.Extent();
  if (nshsd1 > 1 || nshsd2 > 1)
    return 0;

  // NYI : (nplhsd1 > 1) || (nplhsd2 > 1)
  // ------------------------------------

  const TopoDS_Face& f1 = TopoDS::Face(lplhsd1.First());
#ifdef OCCT_DEBUG
//  bool isb1 = myKPMAPf1f2.IsBound(f1); // DEB
#endif

  const TopoDS_Face& f2 = TopoDS::Face(lplhsd2.First());
#ifdef OCCT_DEBUG
//  bool isb2 = myKPMAPf1f2.IsBound(f2); // DEB
#endif

#ifdef OCCT_DEBUG
  int  iF1, iF2;
  bool tSPS1 = GtraceSPS(f1, iF1);
  bool tSPS2 = GtraceSPS(f2, iF2);
  if (tSPS1 || tSPS2)
  {
    GdumpSHA(f1, (char*)"KPiskoletge ");
    std::cout << std::endl;
    GdumpSHA(f2, (char*)"KPiskoletge ");
    std::cout << std::endl;
  }
#endif

  TopoDS_Wire outerw1 = BRepTools::OuterWire(f1);
  TopoDS_Wire outerw2 = BRepTools::OuterWire(f2);

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> mape1;
  TopExp::MapShapes(outerw1, TopAbs_EDGE, mape1);
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> mape2;
  TopExp::MapShapes(outerw2, TopAbs_EDGE, mape2);

  bool se1ONouterw2 = ::sectionedgesON(myDataStructure, outerw1, mape2);
  if (!se1ONouterw2)
    return 0;
  bool se2ONouterw1 = ::sectionedgesON(myDataStructure, outerw2, mape1);
  if (!se2ONouterw1)
    return 0;

  // NYI : <fi> interferes with faces of <Sj> on edges different from outerw's edges
  // -------=-----------------------------------------------------------------------
  bool allI1onseouterw = ::allIonsectionedges(myDataStructure, f1, mape1, mape2);
  if (!allI1onseouterw)
    return 0;
  bool allI2onseouterw = ::allIonsectionedges(myDataStructure, f2, mape2, mape1);
  if (!allI2onseouterw)
    return 0;

  // NYI : (ncyhsd1 > 1) || (ncyhsd2 > 1)
  // ------------------------------------
  // KPcycy :
  if (ncyhsd1 > 0)
  {
    bool cycy = (ncyhsd1 == 1) && (ncyhsd2 == 1);
    if (!cycy)
      return 0;

    bool isbound1 = FUN_tool_inS(outerw1, f1);
    if (!isbound1)
      return 0;
    bool isbound2 = FUN_tool_inS(outerw2, f2);
    if (!isbound2)
      return 0;
  }

  return 1;
}

//=======================================================================
// function : KPisdisj
// purpose  : detection shapes disjoints
//=======================================================================

int TopOpeBRepBuild_Builder::KPisdisj()
{
#ifdef OCCT_DEBUG
  bool TKPB = TopOpeBRepBuild_GettraceKPB();
#endif

  // myShape1 et myShape2 : aucune interference
  const TopOpeBRepDS_DataStructure& DS = myDataStructure->DS();
  //  int nsh = DS.NbShapes();
  //  if (nsh != 2) return 0;

  if (!DS.HasShape(myShape1))
    return 0;
  if (!DS.HasShape(myShape2))
    return 0;

  int isdisj1 = KPisdisjsh(myShape1);
  int isdisj2 = KPisdisjsh(myShape2);

#ifdef OCCT_DEBUG
  if (TKPB)
  {
    std::cout << "isdisj : " << isdisj1 << " " << isdisj2 << std::endl;
  }
#endif

  int isdisj = (isdisj1 && isdisj2) ? 1 : 0;
  return isdisj;
}

//=======================================================================
// function : KPisfafa
// purpose  : detection {face} / {face} toutes HasSameDomain
//=======================================================================

int TopOpeBRepBuild_Builder::KPisfafa()
{
  /*#ifdef OCCT_DEBUG
    bool TKPB = TopOpeBRepBuild_GettraceKPB();
  #endif*/

  return KPisfafash(myShape1) != 0 && KPisfafash(myShape2) != 0 ? 1 : 0;
}

//=======================================================================
// function : KPissoso
// purpose  : detection {solide} / {solide} tous HasSameDomain
//=======================================================================

int TopOpeBRepBuild_Builder::KPissoso()
{
  /*#ifdef OCCT_DEBUG
    bool TKPB = TopOpeBRepBuild_GettraceKPB();
  #endif*/

  return KPissososh(myShape1) != 0 && KPissososh(myShape2) != 0 ? 1 : 0;
}

//=================================================================================================

void TopOpeBRepBuild_Builder::KPClearMaps()
{
  myKPMAPf1f2.Clear();
}

//=======================================================================
// function : KPlhg
// purpose  : --> nb des subsshapes <T> de <S> qui sont HasGeometry()
//=======================================================================

int TopOpeBRepBuild_Builder::KPlhg(const TopoDS_Shape& S, const TopAbs_ShapeEnum T) const
{
  NCollection_List<TopoDS_Shape> L;
  int                            n = KPlhg(S, T, L);
  return n;
}

//=======================================================================
// function : KPlhg
// purpose  : --> nb +liste des subsshapes <T> de <S> qui sont HasGeometry()
//=======================================================================

int TopOpeBRepBuild_Builder::KPlhg(const TopoDS_Shape&             S,
                                   const TopAbs_ShapeEnum          T,
                                   NCollection_List<TopoDS_Shape>& L) const
{
  int n = 0;
  L.Clear();

  TopExp_Explorer ex;
  for (ex.Init(S, T); ex.More(); ex.Next())
  {
    //  for (TopExp_Explorer ex(S,T); ex.More(); ex.Next()) {
    const TopoDS_Shape& s  = ex.Current();
    bool                hg = myDataStructure->HasGeometry(s);
    if (hg)
    {
      n++;
      L.Append(s);
    }
  }

  return n;
}

//=======================================================================
// function : KPlhsd
// purpose  :
// KPlhsd --> nb des subsshapes <T> de <S> qui sont HasSameDomain()
//=======================================================================

int TopOpeBRepBuild_Builder::KPlhsd(const TopoDS_Shape& S, const TopAbs_ShapeEnum T) const
{
  NCollection_List<TopoDS_Shape> L;
  int                            n = KPlhsd(S, T, L);
  return n;
}

//=======================================================================
// function : KPlhsd
// purpose  :
// KPlhsd --> nb + liste des subsshapes <T> de <S> qui sont HasSameDomain()
//=======================================================================

int TopOpeBRepBuild_Builder::KPlhsd(const TopoDS_Shape&             S,
                                    const TopAbs_ShapeEnum          T,
                                    NCollection_List<TopoDS_Shape>& L) const
{
  int n = 0;
  L.Clear();

  TopExp_Explorer ex;
  for (ex.Init(S, T); ex.More(); ex.Next())
  {
    //  for (TopExp_Explorer ex(S,T); ex.More(); ex.Next()) {
    const TopoDS_Shape& s   = ex.Current();
    bool                hsd = myDataStructure->HasSameDomain(s);
    if (hsd)
    {
      n++;
      L.Append(s);
    }
  }

  return n;
}

//=======================================================================
// function : KPclasSS
// purpose  :
// classifie le shape S1 par rapport a S2 en evitant de prendre
// les shape exLS1 de S1 comme element de classification.
// exS1 peut etre IsNull().
// S1,S2 = SOLID | SHELL
//=======================================================================

TopAbs_State TopOpeBRepBuild_Builder::KPclasSS(const TopoDS_Shape&                   S1,
                                               const NCollection_List<TopoDS_Shape>& exLS1,
                                               const TopoDS_Shape&                   S2)
{
  TopAbs_State state = TopAbs_UNKNOWN;
  state              = myShapeClassifier.StateShapeShape(S1, exLS1, S2);

#ifdef OCCT_DEBUG
  if (TopOpeBRepBuild_GettraceKPB())
  {
    const gp_Pnt& P1 = myShapeClassifier.P3D();
    std::cout << "point P1 " << P1.X() << " " << P1.Y() << " " << P1.Z();
    std::cout << "  ";
    TopAbs::Print(state, std::cout);
    std::cout << std::endl;
  }
#endif

  return state;
}

//=======================================================================
// function : KPclasSS
// purpose  :
// classifie le shape S1 par rapport a S2 en evitant de prendre
// le shape exS1 de S1 comme element de classification.
// exS1 peut etre IsNull().
// S1,S2 = SOLID | SHELL
//=======================================================================

TopAbs_State TopOpeBRepBuild_Builder::KPclasSS(const TopoDS_Shape& S1,
                                               const TopoDS_Shape& exS1,
                                               const TopoDS_Shape& S2)
{
  TopAbs_State state = myShapeClassifier.StateShapeShape(S1, exS1, S2);

#ifdef OCCT_DEBUG
  if (TopOpeBRepBuild_GettraceKPB())
  {
    const gp_Pnt& P1 = myShapeClassifier.P3D();
    std::cout << "point P1 " << P1.X() << " " << P1.Y() << " " << P1.Z();
    std::cout << "  ";
    TopAbs::Print(state, std::cout);
    std::cout << std::endl;
  }
#endif

  return state;
}

//=======================================================================
// function : KPclasSS
// purpose  : classifie le shape S1 par rapport a S2 sans evitement de shape
// S1,S2 = SOLID | SHELL
//=======================================================================

TopAbs_State TopOpeBRepBuild_Builder::KPclasSS(const TopoDS_Shape& S1, const TopoDS_Shape& S2)
{
  TopoDS_Shape Snull;
  TopAbs_State state = KPclasSS(S1, Snull, S2);
  return state;
}

//=======================================================================
// function : KPiskoletgesh
// purpose  :
// KPiskoletgesh :
// S est il un shape traite par le cas particulier de koletge?
// si oui : retourne un solide et une liste de faces de collage
//=======================================================================

bool TopOpeBRepBuild_Builder::KPiskoletgesh(const TopoDS_Shape&             Sarg,
                                            NCollection_List<TopoDS_Shape>& lShsd,
                                            NCollection_List<TopoDS_Shape>& lfhsd) const
{
#ifdef OCCT_DEBUG
  bool TKPB = TopOpeBRepBuild_GettraceKPB();
#endif
  const TopOpeBRepDS_DataStructure& BDS      = myDataStructure->DS();
  bool                              iskolesh = FUNKP_KPiskolesh((*this), BDS, Sarg, lShsd, lfhsd);
  if (!iskolesh)
    return false;

#ifdef OCCT_DEBUG
  int nfhsd =
#endif
    KPlhsd(Sarg, TopAbs_FACE, lfhsd);
  NCollection_List<TopoDS_Shape>::Iterator it(lfhsd);
  for (; it.More(); it.Next())
  {
    const TopoDS_Face& fac        = TopoDS::Face(it.Value());
    bool               isplan     = FUN_tool_plane(fac);
    bool               iscylinder = FUN_tool_cylinder(fac);
    if (iscylinder)
      continue;
    if (!isplan)
      return false;

    TopoDS_Wire outerw = BRepTools::OuterWire(fac);
    if (outerw.IsNull())
      return false;

    TopExp_Explorer exe(outerw, TopAbs_EDGE);
    //    int ne = 0;
    for (; exe.More(); exe.Next())
    {
      const TopoDS_Edge&                    ed   = TopoDS::Edge(exe.Current());
      bool                                  isse = BDS.IsSectionEdge(ed);
      const NCollection_List<TopoDS_Shape>& sp   = (*this).Splits(ed, TopAbs_ON);
      if (sp.Extent() == 0)
        return false;
      if (!isse)
        return false;
      //      ne++;
    }
    //    if (ne > 1) return false;

#ifdef OCCT_DEBUG
    int isol = myDataStructure->Shape(Sarg);
    int ifac = myDataStructure->Shape(fac);
    if (TKPB)
    {
      std::cout << "isol " << isol << std::endl;
    }
    if (TKPB)
    {
      std::cout << "nfhsd  " << nfhsd << std::endl;
    }
    if (TKPB)
    {
      std::cout << "ifac " << ifac << std::endl;
    }
    if (TKPB)
    {
      std::cout << "isplan " << isplan << std::endl;
    }
    if (TKPB)
    {
      std::cout << "iscylinder " << iscylinder << std::endl;
    }
    if (TKPB)
    {
      std::cout << std::endl;
    }
#endif
  }

  return true;
}

//=======================================================================
// function : KPSameDomain
// purpose  : complete the lists L1,L2 with the shapes of the DS
//           having same domain :
//           L1 = shapes sharing the same domain of L2 shapes
//           L2 = shapes sharing the same domain of L1 shapes
// (L1 contains a face)
//=======================================================================

void TopOpeBRepBuild_Builder::KPSameDomain(NCollection_List<TopoDS_Shape>& L1,
                                           NCollection_List<TopoDS_Shape>& L2) const
{
  int i;
  int nl1 = L1.Extent(), nl2 = L2.Extent();

  while (nl1 > 0 || nl2 > 0)
  {

    NCollection_List<TopoDS_Shape>::Iterator it1(L1);
    for (i = 1; i <= nl1; i++)
    {
      const TopoDS_Shape& S1 = it1.Value();
#ifdef OCCT_DEBUG
//      int iS1 = myDataStructure->Shape(S1);
#endif
      NCollection_List<TopoDS_Shape>::Iterator itsd(myDataStructure->SameDomain(S1));
      for (; itsd.More(); itsd.Next())
      {
        const TopoDS_Shape& S2 = itsd.Value();
#ifdef OCCT_DEBUG
//	int iS2 = myDataStructure->Shape(S2);
#endif
        bool found = KPContains(S2, L2);
        if (!found)
        {
          L2.Prepend(S2);
          nl2++;
        }
      }
      it1.Next();
    }
    nl1 = 0;

    NCollection_List<TopoDS_Shape>::Iterator it2(L2);
    for (i = 1; i <= nl2; i++)
    {
      const TopoDS_Shape& S2 = it2.Value();
#ifdef OCCT_DEBUG
//      int iS2 = myDataStructure->Shape(S2);
#endif
      NCollection_List<TopoDS_Shape>::Iterator itsd(myDataStructure->SameDomain(S2));
      for (; itsd.More(); itsd.Next())
      {
        const TopoDS_Shape& S1 = itsd.Value();
#ifdef OCCT_DEBUG
//	int iS1 = myDataStructure->Shape(S1);
#endif
        bool found = KPContains(S1, L1);
        if (!found)
        {
          L1.Prepend(S1);
          nl1++;
        }
      }
      it2.Next();
    }
    nl2 = 0;
  }
}

//=======================================================================
// function : KPisdisjsh
// purpose  : S est il un shape traite par le cas particulier "disjoint"
//=======================================================================

int TopOpeBRepBuild_Builder::KPisdisjsh(const TopoDS_Shape& Sarg) const
{
  if (Sarg.IsNull())
    return 0;

  TopExp_Explorer ex;
  int             nhg;

  nhg = KPlhg(Sarg, TopAbs_SOLID);
  if (nhg != 0)
    return 0;

  nhg = KPlhg(Sarg, TopAbs_FACE);
  if (nhg != 0)
    return 0;

  nhg = KPlhg(Sarg, TopAbs_EDGE);
  if (nhg != 0)
    return 0;

  // un seul niveau de HasSameDomain
  int                            n1, n2;
  NCollection_List<TopoDS_Shape> lshsd;

  n1 = KPlhsd(Sarg, TopAbs_SOLID, lshsd);
  if (n1)
  {
    NCollection_List<TopoDS_Shape>::Iterator it(lshsd);
    for (; it.More(); it.Next())
    {
      const TopoDS_Shape& s = it.Value();
      n2                    = KPlhsd(s, TopAbs_FACE);
      if (n2 != 0)
        return 0;
    }
  }

  n1 = KPlhsd(Sarg, TopAbs_FACE, lshsd);
  if (n1)
  {
    NCollection_List<TopoDS_Shape>::Iterator it(lshsd);
    for (; it.More(); it.Next())
    {
      const TopoDS_Shape& s = it.Value();
      n2                    = KPlhsd(s, TopAbs_EDGE);
      if (n2 != 0)
        return 0;
    }
  }

  return 1;
}

//=======================================================================
// function : KPissososh
// purpose  : detection S = {solid} tous HasSameDomain
//=======================================================================

int TopOpeBRepBuild_Builder::KPissososh(const TopoDS_Shape& Sarg) const
{
  // que des solides volants (nb total de solides = nb de solides volants)
  int             nsol1 = 0;
  TopExp_Explorer ex1(Sarg, TopAbs_SOLID);
  for (; ex1.More(); ex1.Next())
    nsol1++;

  int             nsol2 = 0;
  TopExp_Explorer ex2(Sarg, TopAbs_SOLID, TopAbs_COMPSOLID);
  for (; ex2.More(); ex2.Next())
    nsol2++;

  if (nsol1 && (nsol1 != nsol2))
    return 0;

  // toutes les solides sont HasSameDomain()
  int nhsd = KPlhsd(Sarg, TopAbs_SOLID);
  if (nhsd != nsol1)
    return 0;

  int             n;
  TopExp_Explorer ex;

  // pas de shell volant
  n = 0;
  for (ex.Init(Sarg, TopAbs_SHELL, TopAbs_SOLID); ex.More(); ex.Next())
    n++;
  if (n)
    return 0;

  // pas de face volant
  n = 0;
  for (ex.Init(Sarg, TopAbs_FACE, TopAbs_SHELL); ex.More(); ex.Next())
    n++;
  if (n)
    return 0;

  // pas d'edge volant
  n = 0;
  for (ex.Init(Sarg, TopAbs_EDGE, TopAbs_WIRE); ex.More(); ex.Next())
    n++;
  if (n)
    return 0;

  // pas de vertex volant
  n = 0;
  for (ex.Init(Sarg, TopAbs_VERTEX, TopAbs_EDGE); ex.More(); ex.Next())
    n++;
  if (n)
    return 0;

  return 1;
}

//=======================================================================
// function : KPisfafash
// purpose  : detection S = {face} toutes HasSameDomain
//=======================================================================

int TopOpeBRepBuild_Builder::KPisfafash(const TopoDS_Shape& Sarg) const
{
  // il n'y a que des faces volantes (nb total de faces = nb de faces volantes)
  int             nfac1 = 0;
  TopExp_Explorer ex1(Sarg, TopAbs_FACE);
  for (; ex1.More(); ex1.Next())
    nfac1++;

  int             nfac2 = 0;
  TopExp_Explorer ex2(Sarg, TopAbs_FACE, TopAbs_SHELL);
  for (; ex2.More(); ex2.Next())
    nfac2++;

  if (nfac1 && (nfac1 != nfac2))
    return 0;

  // toutes les faces sont HasSameDomain()
  int nhsd = KPlhsd(Sarg, TopAbs_FACE);
  if (nhsd != nfac1)
    return 0;

  int             n;
  TopExp_Explorer ex;

  // pas de wire volant
  n = 0;
  for (ex.Init(Sarg, TopAbs_WIRE, TopAbs_FACE); ex.More(); ex.Next())
    n++;
  if (n)
    return 0;

  // pas d'edge volant
  n = 0;
  for (ex.Init(Sarg, TopAbs_EDGE, TopAbs_WIRE); ex.More(); ex.Next())
    n++;
  if (n)
    return 0;

  // pas de vertex volant
  n = 0;
  for (ex.Init(Sarg, TopAbs_VERTEX, TopAbs_EDGE); ex.More(); ex.Next())
    n++;
  if (n)
    return 0;

  return 1;
}

//=================================================================================================

void TopOpeBRepBuild_Builder::KPiskoletgeanalyse(const TopOpeBRepDS_Config config2,
                                                 const TopAbs_State        Stsol1,
                                                 const TopAbs_State        Stsol2,
                                                 int&                      ires) const
{
  // -----------------------------------------------------------------------------
  // prequesitory :  (nplhsd1 == 1) || (nplhsd2 == 1)
  // -------------   <plsdmi> has all interferences Ii = (T, G=edge of outerw1
  //                                                         ||edge of outerw2, S)
  // -----------------------------------------------------------------------------

  ires = RESUNDEF;

  bool SameOriented = (config2 == TopOpeBRepDS_SAMEORIENTED);
  bool DiffOriented = (config2 == TopOpeBRepDS_DIFFORIENTED);

  //  bool com = Opecom();
  //  bool c12 = Opec12();
  //  bool c21 = Opec21();
  //  bool fus = Opefus();

  if (DiffOriented)
  {
    if (Stsol1 == TopAbs_IN && Stsol2 == TopAbs_IN)
      //      if (com) ires = RESNULL;
      ires = RESNULL;

    if (Stsol1 == TopAbs_OUT && Stsol2 == TopAbs_IN)
    {
      //      if (c12) ires = RESSHAPE1; // rank(sol1) == 1 && rank(sol2) == 2
      //      if (c21) ires = RESSHAPE2; // rank(sol1) == 2 && rank(sol2) == 1
      ires = RESSHAPE1;
    }

    if (Stsol2 == TopAbs_OUT && Stsol1 == TopAbs_IN)
    {
      //      if (c12) ires = RESSHAPE2; // rank(sol2) == 1 && rank(sol1) == 2
      //      if (c21) ires = RESSHAPE1; // rank(sol2) == 2 && rank(sol1) == 1
      ires = RESSHAPE2;
    }

    if (Stsol1 == TopAbs_OUT && Stsol2 == TopAbs_OUT)
      //      if (fus) ires = RESNEWSOL;
      ires = RESNEWSOL;
  } // DiffOriented

  if (SameOriented)
  {
    // ==============================
    // PREQUESITORY :sol1 is IN sol2
    // ==============================

    if (Stsol1 == TopAbs_IN && Stsol2 == TopAbs_IN)
      //      if (com) ires = RESSHAPE1;
      ires = RESSHAPE1;

    if (Stsol1 == TopAbs_OUT && Stsol2 == TopAbs_IN)
    {
      //      if (c12) ires = RESNULL; // rank(sol1) == 1 && rank(sol2) == 2
      //      if (c21) ires = RESNEWSOL; // rank(sol1) == 2 && rank(sol2) == 1
      ires = RESNULL;
    }

    if (Stsol2 == TopAbs_OUT && Stsol1 == TopAbs_IN)
    {
      //      if (c12) ires = RESNULL; // rank(sol2) == 1 && rank(sol1) == 2
      //      if (c21) ires = RESNEWSOL; // rank(sol2) == 2 && rank(sol1) == 1
      ires = RESNEWSOL;
    }

    if (Stsol1 == TopAbs_OUT && Stsol2 == TopAbs_OUT)
      //      if (fus) ires = RESSHAPE2;
      ires = RESSHAPE2;
  } // SameOriented

#ifdef OCCT_DEBUG
  bool TKPB = TopOpeBRepBuild_GettraceKPB();
  if (TKPB)
    std::cout << "ires = " << ires << std::endl;
#endif
}

//=================================================================================================

void TopOpeBRepBuild_Builder::KPisdisjanalyse(const TopAbs_State Stsol1,
                                              const TopAbs_State Stsol2,
                                              int&               ires,
                                              int&               icla1,
                                              int&               icla2) const
{
  ires  = RESUNDEF;
  icla1 = icla2 = SHEUNDEF;

  if (Opefus())
  {
    if (Stsol1 == TopAbs_OUT && Stsol2 == TopAbs_OUT)
    {
      ires  = RESSHAPE12;
      icla1 = icla2 = SHEAUCU; //--
    }
    else if (Stsol1 == TopAbs_OUT && Stsol2 == TopAbs_IN)
    {
      ires  = RESNEWSHA1;
      icla1 = icla2 = SHECLASAUTR; //--
    }
    else if (Stsol1 == TopAbs_IN && Stsol2 == TopAbs_OUT)
    {
      ires  = RESNEWSHA2;
      icla1 = icla2 = SHECLASAUTR; //--
    }
  }
  else if (Opec12())
  {
    if (Stsol1 == TopAbs_OUT && Stsol2 == TopAbs_OUT)
    {
      ires  = RESSHAPE1;
      icla1 = SHEGARDTOUS;
      icla2 = SHEAUCU; //--
    }
    else if (Stsol1 == TopAbs_OUT && Stsol2 == TopAbs_IN)
    {
      ires  = RESNEWSHA1;
      icla1 = SHECLASAUTR;
      icla2 = SHEGARDCOUR; //--
    }
    else if (Stsol1 == TopAbs_IN && Stsol2 == TopAbs_OUT)
    {
      ires  = RESNULL;
      icla1 = icla2 = SHEAUCU; //--
    }
  }
  else if (Opec21())
  {
    if (Stsol1 == TopAbs_OUT && Stsol2 == TopAbs_OUT)
    {
      ires  = RESSHAPE2;
      icla1 = SHEAUCU;
      icla2 = SHEGARDTOUS; //--
    }
    else if (Stsol1 == TopAbs_OUT && Stsol2 == TopAbs_IN)
    {
      ires  = RESNULL;
      icla1 = icla2 = SHEAUCU; //--
    }
    else if (Stsol1 == TopAbs_IN && Stsol2 == TopAbs_OUT)
    {
      ires  = RESNEWSHA2;
      icla1 = SHEGARDCOUR;
      icla2 = SHECLASAUTR; //--
    }
  }
  else if (Opecom())
  {
    if (Stsol1 == TopAbs_OUT && Stsol2 == TopAbs_OUT)
    {
      ires  = RESNULL;
      icla1 = icla2 = SHEAUCU; //--
    }
    else if (Stsol1 == TopAbs_OUT && Stsol2 == TopAbs_IN)
    {
      ires  = RESNEWSHA2;
      icla1 = SHECLASAUTR;
      icla2 = SHEGARDAUTR; //--
    }
    else if (Stsol1 == TopAbs_IN && Stsol2 == TopAbs_OUT)
    {
      ires  = RESNEWSHA1;
      icla1 = SHEGARDAUTR;
      icla2 = SHECLASAUTR; //--
    }
  }

#ifdef OCCT_DEBUG
  bool TKPB = TopOpeBRepBuild_GettraceKPB();
  if (TKPB)
    std::cout << "ires = " << ires << " icla1 " << icla1 << " icla2 " << icla2 << std::endl;
#endif
}

//=======================================================================
// function : KPls (class method)
// purpose  :
// KPls --> nb des subsshapes <T> de <S>
//=======================================================================

int TopOpeBRepBuild_Builder::KPls(const TopoDS_Shape& S, const TopAbs_ShapeEnum T)
{
  NCollection_List<TopoDS_Shape> L;
  int                            n = KPls(S, T, L);
  return n;
}

//=======================================================================
// function : KPls (class method)
// purpose  :
// KPls --> nb + liste des subsshapes <T> de <S>
//=======================================================================

int TopOpeBRepBuild_Builder::KPls(const TopoDS_Shape&             S,
                                  const TopAbs_ShapeEnum          T,
                                  NCollection_List<TopoDS_Shape>& L)
{
  int n = 0;
  L.Clear();

  TopExp_Explorer ex;
  for (ex.Init(S, T); ex.More(); ex.Next())
  {
    //  for (TopExp_Explorer ex(S,T); ex.More(); ex.Next()) {
    const TopoDS_Shape& s = ex.Current();
    n++;
    L.Append(s);
  }

  return n;
}

//=======================================================================
// function : KPclassF (class method)
// purpose  :
// KPclassF : classification F1 par rapport a F2
// F1 et F2 ne partagent aucune arete
// F1 et F2 sont SameDomain
//=======================================================================

TopAbs_State TopOpeBRepBuild_Builder::KPclassF(const TopoDS_Shape& F1, const TopoDS_Shape& F2)
{
  if (F1.IsNull())
    return TopAbs_UNKNOWN;
  if (F2.IsNull())
    return TopAbs_UNKNOWN;

  TopoDS_Face F1F = TopoDS::Face(F1);
  F1F.Orientation(TopAbs_FORWARD);
  TopoDS_Face F2F = TopoDS::Face(F2);
  F2F.Orientation(TopAbs_FORWARD);

  NCollection_List<TopoDS_Shape> le1;
  int                            ne1 = KPls(F1F, TopAbs_EDGE, le1);
  if (ne1 == 0)
    return TopAbs_UNKNOWN;
  const TopoDS_Edge& e1 = TopoDS::Edge(le1.First());

  int          isamdom = 1;
  TopAbs_State St1     = TopAbs_UNKNOWN;
  St1                  = myShapeClassifier.StateShapeShape(e1, F2F, isamdom);
  return St1;
}

//=======================================================================
// function : KPclassFF (class method)
// purpose  :
// classifie F1/F2 --> etats des faces l'une par rapport a l'autre
//=======================================================================

void TopOpeBRepBuild_Builder::KPclassFF(const TopoDS_Shape& F1,
                                        const TopoDS_Shape& F2,
                                        TopAbs_State&       St1,
                                        TopAbs_State&       St2)
{
  St1 = KPclassF(F1, F2);
  St2 = KPclassF(F2, F1);

#ifdef OCCT_DEBUG
  if (TopOpeBRepBuild_GettraceKPB())
  {
    std::cout << "Stf1 ";
    TopAbs::Print(St1, std::cout);
    std::cout << " ";
    std::cout << "Stf2 ";
    TopAbs::Print(St2, std::cout);
    std::cout << std::endl;
  }
#endif
}

//=======================================================================
// function : KPiskoleFF
// purpose  :
// classifie F1/F2 --> etats des faces l'une par rapport a l'autre
// --> True si la configutration topologique correspond au cas "iskole".
//=======================================================================

bool TopOpeBRepBuild_Builder::KPiskoleFF(const TopoDS_Shape& F1,
                                         const TopoDS_Shape& F2,
                                         TopAbs_State&       St1,
                                         TopAbs_State&       St2)
{
#ifdef OCCT_DEBUG
  int  iF1;
  bool tSPS1 = GtraceSPS(F1, iF1);
  int  iF2;
  bool tSPS2 = GtraceSPS(F2, iF2);
  if (tSPS1)
  {
    GdumpSHA(F1, (char*)"KPiskoleFF ");
    std::cout << std::endl;
  }
  if (tSPS2)
  {
    GdumpSHA(F2, (char*)"KPiskoleFF ");
    std::cout << std::endl;
  }
#endif

  KPclassFF(F1, F2, St1, St2);
  bool st1ok = (St1 == TopAbs_OUT || St1 == TopAbs_IN);
  bool st2ok = (St2 == TopAbs_OUT || St2 == TopAbs_IN);

  if (!st1ok)
    return false;
  if (!st2ok)
    return false;
  bool stok = (St1 != St2);
  if (!stok)
    return false;
  return true;
}

//=======================================================================
// function : KPContains (class method)
// purpose  : returns True if S is in the list L.
//=======================================================================

bool TopOpeBRepBuild_Builder::KPContains(const TopoDS_Shape&                   S,
                                         const NCollection_List<TopoDS_Shape>& L)
{
  for (NCollection_List<TopoDS_Shape>::Iterator it(L); it.More(); it.Next())
  {
    const TopoDS_Shape& SL     = it.Value();
    bool                issame = SL.IsSame(S);
    if (issame)
      return true;
  }
  return false;
} // KPContains

//=================================================================================================

int TopOpeBRepBuild_Builder::KPreturn(const int b)
{
#ifdef OCCT_DEBUG
  if (TopOpeBRepBuild_GettraceKPB())
  {
    std::cout << "--- IsKPart " << b;
    if (b == 1)
      std::cout << " iskole";
    if (b == 2)
      std::cout << " isdisj";
    if (b == 3)
      std::cout << " isfafa";
    std::cout << " ---" << std::endl;
  }
#endif
  return b;
}

// modified by NIZHNY-MKK  Tue May 23 09:48:47 2000.BEGIN
//======================================================================================================
//  static function : LocalKPisdisjanalyse
//  purpose:
//======================================================================================================
static void LocalKPisdisjanalyse(const TopAbs_State                     Stsol1,
                                 const TopAbs_State                     Stsol2,
                                 const TopOpeBRepBuild_KPart_Operation& theOperation,
                                 int&                                   ires,
                                 int&                                   icla1,
                                 int&                                   icla2)
{
  ires  = RESUNDEF;
  icla1 = icla2 = SHEUNDEF;

  switch (theOperation)
  {
    case TopOpeBRepBuild_KPart_Operation_Fuse: {
      if (Stsol1 == TopAbs_OUT && Stsol2 == TopAbs_OUT)
      {
        ires  = RESSHAPE12;
        icla1 = icla2 = SHEAUCU; //--
      }
      else if (Stsol1 == TopAbs_OUT && Stsol2 == TopAbs_IN)
      {
        ires  = RESNEWSHA1;
        icla1 = icla2 = SHECLASAUTR; //--
      }
      else if (Stsol1 == TopAbs_IN && Stsol2 == TopAbs_OUT)
      {
        ires  = RESNEWSHA2;
        icla1 = icla2 = SHECLASAUTR; //--
      }
      break;
    }
    case TopOpeBRepBuild_KPart_Operation_Cut12: {
      if (Stsol1 == TopAbs_OUT && Stsol2 == TopAbs_OUT)
      {
        ires  = RESSHAPE1;
        icla1 = SHEGARDTOUS;
        icla2 = SHEAUCU; //--
      }
      else if (Stsol1 == TopAbs_OUT && Stsol2 == TopAbs_IN)
      {
        ires  = RESNEWSHA1;
        icla1 = SHECLASAUTR;
        icla2 = SHEGARDCOUR; //--
      }
      else if (Stsol1 == TopAbs_IN && Stsol2 == TopAbs_OUT)
      {
        ires  = RESNULL;
        icla1 = icla2 = SHEAUCU; //--
      }
      break;
    }
    case TopOpeBRepBuild_KPart_Operation_Cut21: {
      if (Stsol1 == TopAbs_OUT && Stsol2 == TopAbs_OUT)
      {
        ires  = RESSHAPE2;
        icla1 = SHEAUCU;
        icla2 = SHEGARDTOUS; //--
      }
      else if (Stsol1 == TopAbs_OUT && Stsol2 == TopAbs_IN)
      {
        ires  = RESNULL;
        icla1 = icla2 = SHEAUCU; //--
      }
      else if (Stsol1 == TopAbs_IN && Stsol2 == TopAbs_OUT)
      {
        ires  = RESNEWSHA2;
        icla1 = SHEGARDCOUR;
        icla2 = SHECLASAUTR; //--
      }
      break;
    }
    case TopOpeBRepBuild_KPart_Operation_Common: {
      if (Stsol1 == TopAbs_OUT && Stsol2 == TopAbs_OUT)
      {
        ires  = RESNULL;
        icla1 = icla2 = SHEAUCU; //--
      }
      else if (Stsol1 == TopAbs_OUT && Stsol2 == TopAbs_IN)
      {
        ires  = RESNEWSHA2;
        icla1 = SHECLASAUTR;
        icla2 = SHEGARDAUTR; //--
      }
      else if (Stsol1 == TopAbs_IN && Stsol2 == TopAbs_OUT)
      {
        ires  = RESNEWSHA1;
        icla1 = SHEGARDAUTR;
        icla2 = SHECLASAUTR; //--
      }
      break;
    }
    default: {
      std::cout << "Warning: given operation is unknown" << std::endl;
      break;
    }
  } // end switch
}

//======================================================================================================
// static function : BuildNewSolid
// purpose: Build new solid based on sol1 and sol2 according to the states
//======================================================================================================
static TopoDS_Solid BuildNewSolid(const TopoDS_Solid& sol1,
                                  const TopoDS_Solid& sol2,
                                  const TopAbs_State  stsol1,
                                  const TopAbs_State  stsol2,
                                  const int           ires,
                                  const int           icla1,
                                  const int           icla2,
                                  const TopAbs_State  theState1,
                                  const TopAbs_State  theState2)
{

  TopOpeBRepTool_ShapeClassifier                         aShapeClassifier;
  TopoDS_Shape                                           Snull;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> isdisjmap;
  TopOpeBRepDS_BuildTool                                 aBuildTool;
  TopoDS_Solid                                           sol;
  TopAbs_State                                           solstate, shastatetoadd;
  TopoDS_Shell                                           outsha;
  int                                                    icla;
  TopoDS_Solid                                           othersol;
  TopoDS_Shell                                           outsha1 = BRepClass3d::OuterShell(sol1);
  TopoDS_Shell                                           outsha2 = BRepClass3d::OuterShell(sol2);

  TopoDS_Solid newsol;
  aBuildTool.MakeSolid(newsol);
  if (ires == RESNEWSHA1)
  {
    if (!isdisjmap.Contains(outsha1))
    {
      isdisjmap.Add(outsha1);
      aBuildTool.AddSolidShell(newsol, outsha1);
    }
  }
  else if (ires == RESNEWSHA2)
  {
    if (!isdisjmap.Contains(outsha2))
    {
      isdisjmap.Add(outsha2);
      aBuildTool.AddSolidShell(newsol, outsha2);
    }
  }

  sol           = sol1;
  solstate      = stsol1;
  shastatetoadd = theState1;
  outsha        = outsha1;
  icla          = icla1;
  othersol      = sol2;

  {
    TopOpeBRepTool_ShapeExplorer exsha;
    for (exsha.Init(sol, TopAbs_SHELL); exsha.More(); exsha.Next())
    {
      const TopoDS_Shell& shacur   = TopoDS::Shell(exsha.Current());
      bool                isoutsha = shacur.IsEqual(outsha);

      bool garde = true;
      if (icla == SHEAUCU)
        garde = false;
      else if (icla == SHEGARDAUTR || icla == SHECLASAUTR)
        garde = !isoutsha;
      if (!garde)
        continue;

      bool add = false;
      if (icla == SHEGARDCOUR)
        add = true;
      else if (icla == SHEGARDAUTR)
        add = true;
      else if (icla == SHEGARDTOUS)
        add = true;
      else if (icla == SHECLASAUTR)
      {
        TopAbs_State state = aShapeClassifier.StateShapeShape(shacur, Snull, othersol);
        add                = (state == shastatetoadd);
      }
      if (add)
      {
        TopoDS_Shell shaori = shacur;
        bool         r      = (solstate == TopAbs_IN);
        if (r)
          shaori.Complement();
        if (!isdisjmap.Contains(shaori))
        {
          isdisjmap.Add(shaori);
          aBuildTool.AddSolidShell(newsol, shaori);
        }
      }
    }
  } // end block1

  sol           = sol2;
  solstate      = stsol2;
  shastatetoadd = theState2;
  outsha        = outsha2;
  icla          = icla2;
  othersol      = sol1;

  {
    TopOpeBRepTool_ShapeExplorer exsha;
    for (exsha.Init(sol, TopAbs_SHELL); exsha.More(); exsha.Next())
    {
      const TopoDS_Shell& shacur   = TopoDS::Shell(exsha.Current());
      bool                isoutsha = shacur.IsEqual(outsha);

      bool garde = true;
      if (icla == SHEAUCU)
        garde = false;
      else if (icla == SHEGARDAUTR || icla == SHECLASAUTR)
        garde = !isoutsha;
      if (!garde)
        continue;

      bool add = false;
      if (icla == SHEGARDCOUR)
        add = true;
      else if (icla == SHEGARDAUTR)
        add = true;
      else if (icla == SHEGARDTOUS)
        add = true;
      else if (icla == SHECLASAUTR)
      {
        TopAbs_State state = aShapeClassifier.StateShapeShape(shacur, Snull, othersol);
        add                = (state == shastatetoadd);
      }
      if (add)
      {
        TopoDS_Shell shaori = shacur;
        bool         r      = (solstate == TopAbs_IN);
        if (r)
          shaori.Complement();
        aBuildTool.AddSolidShell(newsol, shaori);
      }
    }
  } // end block2
  return newsol;
}

//======================================================================================================
// static function : disjPerformFuse
// purpose: is needed in case of KPart==2
// attention: theMapOfResult is cleared before computations
//======================================================================================================
static bool disjPerformFuse(
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMapOfSolid1,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMapOfSolid2,
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&       theMapOfResult)
{

  theMapOfResult.Clear();

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMapOfSolid;
  aMapOfSolid = theMapOfSolid1;
  int i       = 1;
  for (i = 1; i <= theMapOfSolid2.Extent(); i++)
  {
    aMapOfSolid.Add(theMapOfSolid2(i));
  }

  TopoDS_Solid                                           sol1;
  TopoDS_Shell                                           outsha1;
  TopoDS_Solid                                           sol2;
  TopoDS_Shell                                           outsha2;
  TopOpeBRepTool_ShapeClassifier                         aShapeClassifier;
  TopoDS_Shape                                           Snull;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMapOfUsedSolids;
  TopoDS_Solid                                           acurrentsolid;
  int aMaxNumberOfIterations = aMapOfSolid.Extent() * aMapOfSolid.Extent();

  for (i = 1; i <= aMapOfSolid.Extent(); i++)
  {
    const TopoDS_Shape& localshape1 = aMapOfSolid(i);
    if (localshape1.ShapeType() != TopAbs_SOLID)
      return false;

    sol1          = TopoDS::Solid(localshape1);
    acurrentsolid = sol1;
    if (aMapOfUsedSolids.Contains(localshape1))
      continue;

    int j = 1, acheckiterator = 0;
    while (j <= aMapOfSolid.Extent() && (acheckiterator <= aMaxNumberOfIterations))
    {
      acheckiterator++;
      if (j == i)
      {
        j++;
        continue;
      }
      const TopoDS_Shape& localshape2 = aMapOfSolid(j);
      if (localshape2.ShapeType() != TopAbs_SOLID)
        return false;

      j++; // increase iterator

      if (aMapOfUsedSolids.Contains(localshape2))
      {
        continue;
      }
      sol2    = TopoDS::Solid(localshape2);
      outsha2 = BRepClass3d::OuterShell(sol2);

      outsha1             = BRepClass3d::OuterShell(acurrentsolid);
      TopAbs_State stsol1 = aShapeClassifier.StateShapeShape(outsha1, Snull, sol2);
      TopAbs_State stsol2 = aShapeClassifier.StateShapeShape(outsha2, Snull, acurrentsolid);
      int          ires = RESUNDEF, icla1 = SHEUNDEF, icla2 = SHEUNDEF;
      LocalKPisdisjanalyse(stsol1,
                           stsol2,
                           TopOpeBRepBuild_KPart_Operation_Fuse,
                           ires,
                           icla1,
                           icla2);
      if (ires == RESUNDEF || icla1 == SHEUNDEF || icla2 == SHEUNDEF || ires == RESNULL)
      {
        std::cout << "Warning: disjPerformFuse: can not determine solid's states" << std::endl;
        continue;
      }
      if (ires == RESSHAPE12)
        continue;

      if (ires == RESNEWSHA1 || ires == RESNEWSHA2)
      {
        TopoDS_Solid newsol = BuildNewSolid(acurrentsolid,
                                            sol2,
                                            stsol1,
                                            stsol2,
                                            ires,
                                            icla1,
                                            icla2,
                                            TopAbs_OUT,
                                            TopAbs_OUT);
        j = 1; // iterate on all solids again except already used (very dengerous method)
        acurrentsolid = newsol;
        aMapOfUsedSolids.Add(localshape2);
        if (acurrentsolid.IsNull())
          return false;
      }
    } // end while(j)
    if (acheckiterator > aMaxNumberOfIterations)
    {
      std::cout << "disjPerformFuse: programming error" << std::endl;
      return false;
    }
    theMapOfResult.Add(acurrentsolid);
  } // end for(i)

  return true;
}

//======================================================================================================
// static function : disjPerformCommon
// purpose: is needed in case of KPart==2
// attention: theMapOfResult is cleared before computations
//======================================================================================================
static bool disjPerformCommon(
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMapOfSolid1,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMapOfSolid2,
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&       theMapOfResult)
{

  TopoDS_Solid                                                  sol1;
  TopoDS_Shell                                                  outsha1;
  TopoDS_Solid                                                  sol2;
  TopoDS_Shell                                                  outsha2;
  TopOpeBRepTool_ShapeClassifier                                aShapeClassifier;
  TopoDS_Shape                                                  Snull;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMapOfSeparatedSolid1,
    aMapOfSeparatedSolid2, aMapOfCommonOfCouple;
  theMapOfResult.Clear();

  disjPerformFuse(theMapOfSolid1, theMapOfSolid1, aMapOfSeparatedSolid1);
  disjPerformFuse(theMapOfSolid2, theMapOfSolid2, aMapOfSeparatedSolid2);
  //   Now common parts of all couples of solids are different
  for (int i = 1; i <= aMapOfSeparatedSolid1.Extent(); i++)
  {
    const TopoDS_Shape& localshape1 = aMapOfSeparatedSolid1(i);
    if (localshape1.ShapeType() != TopAbs_SOLID)
      return false;
    sol1    = TopoDS::Solid(localshape1);
    outsha1 = BRepClass3d::OuterShell(sol1);

    for (int j = 1; j <= aMapOfSeparatedSolid2.Extent(); j++)
    {
      const TopoDS_Shape& localshape2 = aMapOfSeparatedSolid2(j);
      if (localshape2.ShapeType() != TopAbs_SOLID)
        return false;

      sol2                = TopoDS::Solid(localshape2);
      outsha2             = BRepClass3d::OuterShell(sol2);
      TopAbs_State stsol1 = aShapeClassifier.StateShapeShape(outsha1, Snull, sol2);
      TopAbs_State stsol2 = aShapeClassifier.StateShapeShape(outsha2, Snull, sol1);
      int          ires = RESUNDEF, icla1 = SHEUNDEF, icla2 = SHEUNDEF;

      LocalKPisdisjanalyse(stsol1,
                           stsol2,
                           TopOpeBRepBuild_KPart_Operation_Common,
                           ires,
                           icla1,
                           icla2);
      if (ires == RESUNDEF || icla1 == SHEUNDEF || icla2 == SHEUNDEF)
      {
        std::cout << "Warning: disjPerformCommon: can not determine solid's states" << std::endl;
        continue;
      }
      switch (ires)
      {
        case RESNULL: {
          continue;
        }
        case RESSHAPE12: {
          aMapOfCommonOfCouple.Add(sol1);
          aMapOfCommonOfCouple.Add(sol2);
          continue;
        }
        case RESSHAPE1: {
          aMapOfCommonOfCouple.Add(sol1);
          continue;
        }
        case RESSHAPE2: {
          aMapOfCommonOfCouple.Add(sol2);
          break;
        }
        case RESNEWSHA1:
        case RESNEWSHA2: {
          TopoDS_Solid newsol =
            BuildNewSolid(sol1, sol2, stsol1, stsol2, ires, icla1, icla2, TopAbs_IN, TopAbs_IN);
          aMapOfCommonOfCouple.Add(newsol);
          break;
        }
        default:
          continue;
      } // end switch
    } // end for(j)
  } // end for(i)

  disjPerformFuse(aMapOfCommonOfCouple, aMapOfCommonOfCouple, theMapOfResult);
  return true;
}

//======================================================================================================
// static function : disjPerformCut
// purpose: is needed in case of KPart==2
// attention: theMapOfResult is cleared before computations
//======================================================================================================
static bool disjPerformCut(
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMapOfSolid1,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMapOfSolid2,
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&       theMapOfResult)
{
  TopoDS_Solid                                                  sol1;
  TopoDS_Shell                                                  outsha1;
  TopoDS_Solid                                                  sol2;
  TopoDS_Shell                                                  outsha2;
  TopOpeBRepTool_ShapeClassifier                                aShapeClassifier;
  TopoDS_Shape                                                  Snull;
  TopoDS_Solid                                                  acurrentsolid;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMapOfSeparatedSolid1,
    aMapOfSeparatedSolid2;

  theMapOfResult.Clear();

  disjPerformFuse(theMapOfSolid1, theMapOfSolid1, aMapOfSeparatedSolid1);
  disjPerformFuse(theMapOfSolid2, theMapOfSolid2, aMapOfSeparatedSolid2);

  for (int i = 1; i <= aMapOfSeparatedSolid1.Extent(); i++)
  {
    const TopoDS_Shape& localshape1 = aMapOfSeparatedSolid1(i);
    if (localshape1.ShapeType() != TopAbs_SOLID)
      return false;
    sol1          = TopoDS::Solid(localshape1);
    acurrentsolid = sol1;

    bool NullResult = false;

    for (int j = 1; j <= aMapOfSeparatedSolid2.Extent() && !NullResult; j++)
    {
      const TopoDS_Shape& localshape2 = aMapOfSeparatedSolid2(j);
      if (localshape2.ShapeType() != TopAbs_SOLID)
        return false;
      sol2                = TopoDS::Solid(localshape2);
      outsha2             = BRepClass3d::OuterShell(sol2);
      outsha1             = BRepClass3d::OuterShell(acurrentsolid);
      TopAbs_State stsol1 = aShapeClassifier.StateShapeShape(outsha1, Snull, sol2);
      TopAbs_State stsol2 = aShapeClassifier.StateShapeShape(outsha2, Snull, acurrentsolid);
      int          ires = RESUNDEF, icla1 = SHEUNDEF, icla2 = SHEUNDEF;

      LocalKPisdisjanalyse(stsol1,
                           stsol2,
                           TopOpeBRepBuild_KPart_Operation_Cut12,
                           ires,
                           icla1,
                           icla2);
      if (ires == RESUNDEF || icla1 == SHEUNDEF || icla2 == SHEUNDEF)
      {
        std::cout << "Warning: disjPerformCut: can not determine solid's states" << std::endl;
        continue;
      }
      switch (ires)
      {
        case RESNULL: {
          NullResult = true;
          break;
        }
        case RESSHAPE12: {
          NullResult = true;
          break;
        }
        case RESSHAPE1: {
          NullResult = false;
          break;
        }
        case RESSHAPE2: {
          NullResult = true;
          break;
        }
        case RESNEWSHA1:
        case RESNEWSHA2: {
          TopoDS_Solid newsol = BuildNewSolid(acurrentsolid,
                                              sol2,
                                              stsol1,
                                              stsol2,
                                              ires,
                                              icla1,
                                              icla2,
                                              TopAbs_OUT,
                                              TopAbs_IN);
          acurrentsolid       = newsol;
          break;
        }
        default:
          continue;
      } // end switch
    } // end for(j)
    if (!NullResult)
    {
      if (acurrentsolid.IsNull())
        return false;
      theMapOfResult.Add(acurrentsolid);
    }
  } // end for(i)
  return true;
}

// modified by NIZHNY-MKK  Tue May 23 09:49:03 2000.END
