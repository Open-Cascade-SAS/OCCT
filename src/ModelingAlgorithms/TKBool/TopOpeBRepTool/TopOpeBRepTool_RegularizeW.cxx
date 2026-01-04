// Created on: 1998-12-23
// Created by: Xuan PHAM PHU
// Copyright (c) 1998-1999 Matra Datavision
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

#include <TopOpeBRepTool.hxx>
#include <TopOpeBRepTool_CLASSI.hxx>
#include <TopOpeBRepTool_REGUW.hxx>
#include <TopOpeBRepTool_TOOL.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedMap.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>

#define SAME (-1)
#define DIFF (-2)
#define UNKNOWN (0)
#define oneINtwo (1)
#define twoINone (2)

#define M_FORWARD(sta) (sta == TopAbs_FORWARD)
#define M_REVERSED(sta) (sta == TopAbs_REVERSED)
#define M_INTERNAL(sta) (sta == TopAbs_INTERNAL)
#define M_EXTERNAL(sta) (sta == TopAbs_EXTERNAL)

#ifdef OCCT_DEBUG
extern bool TopOpeBRepTool_GettraceREGUFA();
static NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> STATIC_mapw, STATIC_mapf;

static int FUN_adds(const TopoDS_Shape& s)
{
  TopAbs_ShapeEnum        typ = s.ShapeType();
  TCollection_AsciiString aa;
  int                     is = 0;
  if (typ == TopAbs_WIRE)
  {
    aa = TCollection_AsciiString("wi");
    is = STATIC_mapw.Add(s);
  }
  if (typ == TopAbs_FACE)
  {
    aa = TCollection_AsciiString("fa");
    is = STATIC_mapf.Add(s);
  }
  return is;
}

Standard_EXPORT void FUN_tool_coutsta(const int& sta, const int& i1, const int& i2)
{
  switch (sta)
  {
    case SAME:
      std::cout << i1 << " gives SAME bnd with " << i2 << std::endl;
      break;
    case DIFF:
      std::cout << i1 << " gives  OUT bnd with " << i2 << std::endl;
      break;
    case oneINtwo:
      std::cout << i1 << " is IN " << i2 << std::endl;
      break;
    case twoINone:
      std::cout << i2 << " is IN " << i1 << std::endl;
      break;
  }
}
#endif

Standard_EXPORT void FUN_addOwlw(const TopoDS_Shape&                   Ow,
                                 const NCollection_List<TopoDS_Shape>& lw,
                                 NCollection_List<TopoDS_Shape>&       lresu);

//=================================================================================================

bool TopOpeBRepTool::RegularizeWires(
  const TopoDS_Face& theFace,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    mapoldWnewW,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    ESplits) // (e,esp); esp = splits of e
{
  if (theFace.IsNull())
    return false;
  TopoDS_Shape aLocalShape = theFace.Oriented(TopAbs_FORWARD);
  TopoDS_Face  aFace       = TopoDS::Face(aLocalShape);
  //  TopoDS_Face aFace = TopoDS::Face(theFace.Oriented(TopAbs_FORWARD));

  TopOpeBRepTool_REGUW REGUW(aFace);
  REGUW.SetOwNw(mapoldWnewW);
  REGUW.SetEsplits(ESplits);

  //  bool hasregu = false;
  TopExp_Explorer exw(aFace, TopAbs_WIRE);
  for (; exw.More(); exw.Next())
  {
    const TopoDS_Shape& W = exw.Current();
    REGUW.Init(W);
    bool ok = REGUW.MapS();
    if (!ok)
      return false;
    ok = REGUW.SplitEds();
    if (!ok)
      return false;
    ok = REGUW.REGU();
    if (!ok)
      return false;
  }

  REGUW.GetEsplits(ESplits);
  REGUW.GetOwNw(mapoldWnewW);
  return true;
}

//=================================================================================================

bool TopOpeBRepTool::Regularize(
  const TopoDS_Face&              theFace,
  NCollection_List<TopoDS_Shape>& aListOfFaces,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    ESplits)
{
  TopOpeBRepTool_REGUW REGUW(theFace);
  aListOfFaces.Clear();
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
       mapoldWnewW;
  bool regu = TopOpeBRepTool::RegularizeWires(theFace, mapoldWnewW, ESplits);
  if (regu)
  {
    regu = TopOpeBRepTool::RegularizeFace(theFace, mapoldWnewW, aListOfFaces);
  }
  return regu;
}

//**********************************************************************
//   classifying wires
//**********************************************************************

// ------------------------------------------------------------
// ------------------ classifying wires -----------------------

// function : bFgreaterbFF (F,FF)
// purpose  : returns False if <F>'s 3d bounding box smaller than <FF>'s

// function : mkBnd2d (W,F,B2d)
// purpose  : get 2d bounding box <B2d> of wire <W>'s UV
//            representation on face <F>.

// function : classiBnd2d (B,ismaller)
// purpose  : compare the two 2d bounding boxes of array <B>
//            if found, B<ismaller> is the smaller one and returns IN
//            else if boxes are disjoint, returns OUT
//            else return UNKNOWN
// function : classiBnd2d (B)
// purpose  : returns SAME,DIFF,UNKNOWN,oneINtwo or twoINone

// function : mkboundedF(W,boundedF)
// purpose  :

// function : FindAPointInTheFace(F,u,v)
// purpose  :

// function : GetFP2d(W,boundedF,p2d)
// purpose  : computes <boundedF> the bounded face built up with wire <W>
//            and <p2d> a point in <boundedF>
//

// function : classiwithp2d(wi)
// purpose  : classify wires (wi(k),k = 1,2)
//            prequesitory : wires of <wi> are not intersecting each other.

// function : ClassifW(F,mapoldWnewW,mapWlow)
// purpose  : all wires described in <mapoldWnewW> are on face <F>
//            <mapoldWnewW> = map with :
//             key = a wire of <F>
//             item = the splits of the previous wire (can be an empty list)
//            the aim is to get map <mapWlow> with :
//             key = a new face's boundary
//             item = wires dexcribing holes in the previous face
//                    (can be an empty list)
// ------------------------------------------------------------

/*static TopAbs_State FUN_tool_classiBnd2d(const NCollection_Array1<Bnd_Box2d>& B,int& ismaller,
                 const bool chklarge = true)
{
  // purpose :
  //   Getting <ismaller>, index of the smallest Bnd Box
  //   if B(i) is IN B(j): ismaller = i,
  //                       return IN.
  //   else: ismaller = 1,
  //         if B(1) and B(2) are disjoint, return OUT
  //         if B(1) and B(2) are same, return ON
  //         else return UNKNOWN.
  ismaller = 1;

  NCollection_Array2<double> UV(1,2, 1,4);
  for (int i = 1; i <= 2; i++)
    //      (Umin(i), Vmin(i), Umax(i), Vmax(i))
    B(i).Get(UV(i,1), UV(i,3), UV(i,2), UV(i,4));

#ifdef OCCT_DEBUG
  bool trc = false;
  if (trc) {
    for (int i = 1; i <= 2; i++)
      std::cout<<"B("<<i<<") = ("<<UV(i,1)<<" "<<UV(i,3)<<" "<<UV(i,2)<<"
"<<UV(i,4)<<")"<<std::endl;
  }
#endif

  bool smaller, same;
  int ii, jj;
  double tol = 1.e-6;

  bool disjoint = false;
  for (int k = 1; k <= 3; k+=2) {
    for (i = 1; i <= 2; i++) {
      ii = i; jj = (i == 1) ?  2 : 1;
      //  diff = Umin<ii> - Umax<jj> : k = 1
      //  diff = Vmin<ii> - Vmax<jj> : k = 3
      double diff = UV(ii,k) - UV(jj,k+1);
      // IMPORTANT : for split faces sharing same edge, use
      // chklarge = True.
      disjoint = chklarge ? (diff >= -tol) : (diff > 0.);
      if (disjoint) {ismaller = 1; return TopAbs_OUT;}
    }
  }

  for (i = 1; i <= 2; i++) {
    ii = i; jj = (i == 1) ? 2 : 1;
    smaller = same = true;
    for (int k = 1; k <= 3; k += 2){
      //  diff = Umin<ii> - Umin<jj> : k = 1
      //  diff = Vmin<ii> - Vmin<jj> : k = 3
      double diff = UV(ii,k) - UV(jj,k);
      smaller = chklarge ? (smaller && (diff > -tol)) : (smaller && (diff > 0.));
      same = same && (std::abs(diff) <= tol);
    }
    for (k = 2; k <= 4; k +=2){
      //  diff = Umax<ii> - Umax<jj> : k = 2
      //  diff = Vmax<ii> - Vmax<jj> : k = 4
      double diff = UV(ii,k) - UV(jj,k);
      smaller = chklarge ? (smaller && (diff < tol)) : (smaller && (diff < 0.));
      same = same && (std::abs(diff) <= tol);
    }

    if (same) return TopAbs_ON;
    if (smaller) {
      ismaller = ii;
      return TopAbs_IN;
    }
  }
  return TopAbs_UNKNOWN;
}

#define SAME     (-1)
#define DIFF     (-2)
#define UNKNOWN  ( 0)
#define oneINtwo ( 1)
#define twoINone ( 2)

Standard_EXPORT int FUN_tool_classiBnd2d(const NCollection_Array1<Bnd_Box2d>& B,
                     const bool chklarge = true)
{
  int ismaller;
  TopAbs_State sta = FUN_tool_classiBnd2d(B, ismaller, chklarge);
  int res = -10;
  switch (sta) {
  case TopAbs_IN :
    res = ismaller; break;
  case TopAbs_OUT :
    res = DIFF; break;
  case TopAbs_ON :
    res = SAME; break;
  case TopAbs_UNKNOWN :
    res = UNKNOWN; break;
  }
  return res;
}

static bool FUN_tool_mkboundedF(const TopoDS_Wire& W, TopoDS_Face& boundedF)
{
  BRepLib_MakeFace mf(W, false);
  bool done = mf.IsDone();
  if (done) boundedF = mf.Face();
  return done;
}
bool FUN_tool_FindAPointInTheFace(const TopoDS_Face& F,
                 double& u, double& v)
{
  bool ok = BRepClass3d_SolidExplorer::FindAPointInTheFace(F,u,v);
  return ok;
}
static bool FUN_tool_GetFP2d(const TopoDS_Shape& W,
                TopoDS_Shape& boundedF, gp_Pnt2d& p2d)
{
  bool ok = FUN_tool_mkboundedF(TopoDS::Wire(W), TopoDS::Face(boundedF));
  if (!ok) return false;

  double u,v; ok = FUN_tool_FindAPointInTheFace(TopoDS::Face(boundedF),u,v);
  if (!ok) return false;
  p2d = gp_Pnt2d(u,v);
  return true;
}
static int FUN_tool_classiwithp2d(const NCollection_Array1<TopoDS_Shape>& wi)
{
  int stares = UNKNOWN;
  NCollection_Array1<TopoDS_Shape> fa(1,2);
  NCollection_Array1<gp_Pnt>     p3d(1,2);
  for (int k = 1; k <= 2; k++) {
    gp_Pnt2d p2d;
    bool ok = FUN_tool_GetFP2d(wi(k), fa(k), p2d);
    if (!ok) return UNKNOWN;
    BRepAdaptor_Surface BS(TopoDS::Face(fa(k)));
    p3d(k) = BS.Value(p2d.X(),p2d.Y());
  }

  TopAbs_State sta;
  int i,j; i = j = 0;
  for (int nite = 1; nite <= 2; nite++) {
    i = nite;
    j = (i == 1) ? 2 : 1;
    TopoDS_Face f = TopoDS::Face(fa(j));
    const gp_Pnt p = p3d(i);
    double tol = BRep_Tool::Tolerance(f);
    BRepClass_FaceClassifier Fclass(f, p, tol);
    sta = Fclass.State();
    if (sta == TopAbs_IN) break;
  }
  switch (sta) {
  case TopAbs_IN :
    stares = i; break;
  case TopAbs_OUT :
    stares = DIFF; break;
  case TopAbs_ON :
  case TopAbs_UNKNOWN :
    stares = UNKNOWN; break;
  }
  return stares;
}

Standard_EXPORT bool FUN_tool_ClassifW(const TopoDS_Face& F,
                      const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>,
TopTools_ShapeMapHasher>& mapoldWnewW, NCollection_DataMap<TopoDS_Shape,
NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& mapWlow)
{
  // NYI : create maps to store Bnd_Box2d, and faces.

#ifdef OCCT_DEBUG
  bool trc = TopOpeBRepTool_GettraceREGUFA();
  if (trc) std::cout<<"** ClassifW :"<<std::endl;
  STATIC_mapw.Clear();
#endif

  // Purpose :
  // --------
  // Filling the map <mapWlow> : with (key + item) = new face,
  //  item = (newface has holes) ? list of wires IN the wire key: empty list

  // prequesitory : <mapoldWnewW> binds (non split wire of <F>, emptylos)
  //                                    (split wire of <F>, splits of the wire)

  // Mapping :
  // --------
  // Filling <oldW> : list of wires of <F>
  // Filling <mapWlow> : with (non-split old wire, emptylos),
  //                          (split of old wire, emptylos)
  NCollection_List<TopoDS_Shape> oldW;
  int noldW = mapoldWnewW.Extent();
  bool oneoldW = (noldW == 1);
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>,
TopTools_ShapeMapHasher>::Iterator ite(mapoldWnewW); NCollection_List<TopoDS_Shape> emptylos;

  // --------------
  // * noldW == 1 :
  // --------------
  if (oneoldW) {
    const TopoDS_Wire& oldwi = TopoDS::Wire(ite.Key());
    const NCollection_List<TopoDS_Shape>& low = ite.Value();
    int nw = low.Extent();
    if (nw == 0) {mapWlow.Bind(oldwi,emptylos); return true;}
    if (nw == 1) {mapWlow.Bind(low.First(),emptylos); return true;}

    // <complWoldw> = {(newwire, emptylos)}
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
complWoldw; NCollection_List<TopoDS_Shape>::Iterator itlw(low); for (; itlw.More(); itlw.Next())
complWoldw.Bind(itlw.Value(), emptylos);

    // iteration on <complWoldw> :
    int ncompl = complWoldw.Extent();
    bool go = true;
    int nite = 0, nitemax = int(ncompl*(ncompl-1)/2);
    while (go && (nite <= nitemax)){
      NCollection_Array1<Bnd_Box2d> Bnd2d(1,2);
      NCollection_Array1<TopoDS_Shape> wi(1,2);

      NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>,
TopTools_ShapeMapHasher>::Iterator itmap(complWoldw); wi(1) = itmap.Key(); if (ncompl == 1) {
    mapWlow.Bind(wi(1),itmap.Value());
    break;
      }
      FUN_tool_mkBnd2d(wi(1), F, Bnd2d(1));
      bool OUTall = false;
      bool oneINother = false;
      int sma,gre; // dummy if !oneINother

      for (; itmap.More(); itmap.Next()) {
    wi(2) = itmap.Key();
    if (wi(1).IsSame(wi(2))) continue;
    FUN_tool_mkBnd2d(wi(2), F, Bnd2d(2));

    // sta : wi(1) / wi(2)
    int sta = FUN_tool_classiBnd2d(Bnd2d);
    nite++;
    if ((sta == SAME)||(sta == UNKNOWN)) sta = FUN_tool_classiwithp2d(wi);
#ifdef OCCT_DEBUG
    if (trc) {std::cout<<"#wi :";FUN_tool_coutsta(sta,FUN_adds(wi(1)),FUN_adds(wi(2)));}
#endif
    if ((sta == SAME)||(sta == UNKNOWN)) return false;
    if ((sta == DIFF) && itmap.More()) {OUTall = true; continue;}// w(1) OUT w(2)
    sma = (sta == oneINtwo) ? 1 : 2;
    gre = (sta == oneINtwo) ? 2 : 1;
    oneINother = true;
    break;
      } // itmap

      if (oneINother) {
    // Updating map <complWoldw> with :
    //  - key = wi(gre),
    //    item += wi(sma) && item += item(wi(sma))
    //  - unbinding key = (wi(sma))
    NCollection_List<TopoDS_Shape>& lwgre = complWoldw.ChangeFind(wi(gre));
    lwgre.Append(wi(sma));
    NCollection_List<TopoDS_Shape>::Iterator itwsma(complWoldw.Find(wi(sma)));
    for (; itwsma.More(); itwsma.Next()) lwgre.Append(itwsma.Value());
    complWoldw.UnBind(wi(sma));
      }
      else if (OUTall) {mapWlow.Bind(wi(1),emptylos); complWoldw.UnBind(wi(1));}
      else return false;

      ncompl = complWoldw.Extent();
      go = (ncompl >= 1);
    }
    return true;
  } // oneoldW

  // -------------
  // * noldW > 1 :
  // -------------
  for (; ite.More(); ite.Next()){
    const TopoDS_Wire& oldwi = TopoDS::Wire(ite.Key());
    const NCollection_List<TopoDS_Shape>& low = ite.Value();
    NCollection_List<TopoDS_Shape>::Iterator itlow(low);
    if (low.IsEmpty()) mapWlow.Bind(oldwi, emptylos);
    else
      for (; itlow.More(); itlow.Next()) mapWlow.Bind(itlow.Value(), emptylos);
    oldW.Append(oldwi);
  }

  // classifying wires of <mapoldWnewW> :
  // -----------------------------------
  // <Owi>            : old wires
  // <OBnd2d>         : old wires' bounding boxes
  // <Owhassp>(k) : Owi(k) has splits

  NCollection_List<TopoDS_Shape> oldWcopy; oldWcopy.Assign(oldW);
  for (NCollection_List<TopoDS_Shape>::Iterator itoldW(oldW); itoldW.More(); itoldW.Next()) {

    NCollection_Array1<TopoDS_Shape> Owi(1,2);
    NCollection_Array1<Bnd_Box2d> OBnd2d(1,2);
    NCollection_Array1<bool> Owhassp(1,2);

    Owi(1) = itoldW.Value();
    if (oldWcopy.Extent() <1) break;
    oldWcopy.RemoveFirst();

    Owhassp(1) = !mapoldWnewW.Find(Owi(1)).IsEmpty();
    bool Owi1notkey = !mapWlow.IsBound(Owi(1));
    if (Owi1notkey && !Owhassp(1)) continue;

    FUN_tool_mkBnd2d(Owi(1), F, OBnd2d(1));

    // Classifying oldwire(i) with oldwires(j): j = i+1..nwiresofF
    int osma,ogre,osta;
    NCollection_List<TopoDS_Shape>::Iterator itoldWcopy(oldWcopy);
    for (; itoldWcopy.More(); itoldWcopy.Next()) {

      NCollection_Array1<NCollection_List<TopoDS_Shape>> newwi(1,2);
      Owi(2) = TopoDS::Wire(itoldWcopy.Value());
      bool Owi2notkey = !mapWlow.IsBound(Owi(2));
      Owhassp(2) = !mapoldWnewW.Find(Owi(2)).IsEmpty();
      if (Owi2notkey && !Owhassp(2)) continue;
      FUN_tool_mkBnd2d(Owi(2), F, OBnd2d(2));

      // <osma>, <ogre> :
      // ----------------
      // Classifying Ow<i> with Ow<j> :
      osta = FUN_tool_classiBnd2d(OBnd2d);
      if ((osta == SAME)||(osta == UNKNOWN)) osta = FUN_tool_classiwithp2d(Owi);
#ifdef OCCT_DEBUG
      if (trc) {std::cout<<"wi : "; FUN_tool_coutsta(osta,FUN_adds(Owi(1)),FUN_adds(Owi(2)));}
#endif
      if ((osta == SAME)||(osta == UNKNOWN)) return false;
      if (osta == DIFF)                    continue; // Ow(1), Ow(2) are disjoint
      // Owi<sma> is IN Owi<grea>
      osma = (osta == oneINtwo) ? 1 : 2;
      ogre = (osta == oneINtwo) ? 2 : 1;

      // Owhassp<k>  : newwi<k> = splits (Owi<k>)
      // !Owhassp<k> : newwi<k> = Owi<k>
      for (int i = 1; i <= 2; i++) {
    const TopoDS_Shape& owi = Owi(i);
    if (!Owhassp(i)) newwi(i).Append(owi);
    else             newwi(i) = mapoldWnewW.Find(owi);
      }

      //
      // classifying wires of newwi<sma> with wires of newwi<gre> :
      //
      int sta, sma, gre;
      NCollection_List<TopoDS_Shape>::Iterator itnwi(newwi(osma));
      for (; itnwi.More(); itnwi.Next()) {
    // <wi>    : new wires
    // <Bnd2d> : new wires' bounding boxes
    NCollection_Array1<TopoDS_Shape> wi(1,2);
    NCollection_Array1<Bnd_Box2d> Bnd2d(1,2);

    wi(1) = itnwi.Value(); // wi(1) in {newwi(osma)}
    bool wi1notkey = !mapWlow.IsBound(wi(1));
    if (wi1notkey) continue;

    if (!Owhassp(osma)) Bnd2d(1).Add(OBnd2d(osma));
    else               FUN_tool_mkBnd2d(wi(1), F, Bnd2d(1));

    NCollection_List<TopoDS_Shape>::Iterator itnwj(newwi(ogre));
    for (; itnwj.More(); itnwj.Next()) {

      wi(2) = itnwj.Value(); // wi(2) in {newwi(ogre)}
      bool wi2notkey = !mapWlow.IsBound(wi(2));
      if (wi2notkey) continue;

      // empty the bounding box
      Bnd_Box2d newB2d;
      if (!Owhassp(ogre)) newB2d.Add(OBnd2d(ogre));
      else                FUN_tool_mkBnd2d(wi(2), F, newB2d);
       FUN_tool_UpdateBnd2d(Bnd2d(2),newB2d);

      // Classifying wi(1)  with wi(2) :
      sta = FUN_tool_classiBnd2d(Bnd2d);
#ifdef OCCT_DEBUG
      if (trc) {std::cout<<"wi : "; FUN_tool_coutsta(sta,STATIC_mapw.FindIndex(wi(1)),
                            STATIC_mapw.FindIndex(wi(2)));}
#endif
      if ((sta == SAME)||(sta == UNKNOWN)) sta = FUN_tool_classiwithp2d(wi);
      if ((sta == SAME)||(sta == UNKNOWN)) return false;
      if (sta == DIFF)                   continue;
      // wi<sma> is IN wi<grea>
      sma = (sta == oneINtwo) ? 1 : 2;
      gre = (sta == oneINtwo) ? 2 : 1;

      // Updating map <mapWlow> with :
      //  - key = wi(gre),
      //    item += wi(sma) && item += item(wi(sma))
      //  - unbinding key = (wi(sma))
      NCollection_List<TopoDS_Shape>& lwgre = mapWlow.ChangeFind(wi(gre));
      lwgre.Append(wi(sma));
      NCollection_List<TopoDS_Shape>::Iterator itwsma(mapWlow.Find(wi(sma)));
      for (; itwsma.More(); itwsma.Next()) lwgre.Append(itwsma.Value());
      mapWlow.UnBind(wi(sma));
      break;
      // wi<sma> IN wi<gre>, wi<sma> is OUT {newwi<gre>} / wi<gre>
      // wi<sma> is classified / all newwires.
    }
      } // itnwi(newwi(sma))
    }// itoldWcopy
  } // itoldW
  return true;
}
// ------------------------------------------------------------
// -------------------- building up faces ---------------------
// ------------------------------------------------------------

Standard_EXPORT bool FUN_tool_MakeFaces(const TopoDS_Face& theFace,
                       NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>,
TopTools_ShapeMapHasher>& mapWlow, NCollection_List<TopoDS_Shape>& aListOfFaces)
{
#ifdef OCCT_DEBUG
  bool trc = TopOpeBRepTool_GettraceREGUFA();
  if (trc) std::cout<<"** MakeFaces :"<<std::endl;
#endif
  bool toreverse = M_REVERSED(theFace.Orientation());
  TopoDS_Face F = TopoDS::Face(theFace.Oriented(TopAbs_FORWARD));
  BRep_Builder BB;

  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>,
TopTools_ShapeMapHasher>::Iterator itm(mapWlow); for (; itm.More(); itm.Next()) { const TopoDS_Wire&
wi = TopoDS::Wire(itm.Key()); TopoDS_Shape FF = F.EmptyCopied(); BB.Add(FF,wi);
//    BB.MakeFace(FF); // put a TShape

    NCollection_List<TopoDS_Shape>::Iterator itlow(itm.Value());
    for (; itlow.More(); itlow.Next()) {
      const TopoDS_Wire& wwi = TopoDS::Wire(itlow.Value());
      BB.Add(FF,wwi);
    }

    if (toreverse) FF.Orientation(TopAbs_REVERSED);
    aListOfFaces.Append(FF);
  }

#ifdef OCCT_DEBUG
  if (trc) {
    std::cout<<"sp(fa"<<FUN_adds(theFace)<<")=";
    NCollection_List<TopoDS_Shape>::Iterator it(aListOfFaces);
    for (; it.More(); it.Next()) std::cout<<" fa"<<FUN_adds(it.Value());
    std::cout<<std::endl;
  }
#endif

  return true;
}*/

Standard_EXPORT bool FUN_tool_ClassifW(
  const TopoDS_Face& F,
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    mapOwNw,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    mapWlow)
{
#ifdef OCCT_DEBUG
  bool trc = TopOpeBRepTool_GettraceREGUFA();
  if (trc)
    std::cout << "** ClassifW :" << std::endl;
#endif
  double       tolF        = BRep_Tool::Tolerance(F);
  double       toluv       = TopOpeBRepTool_TOOL::TolUV(F, tolF);
  TopoDS_Shape aLocalShape = F.Oriented(TopAbs_FORWARD);
  TopoDS_Face  FFOR        = TopoDS::Face(aLocalShape);
  //  TopoDS_Face FFOR = TopoDS::Face(F.Oriented(TopAbs_FORWARD));
  TopOpeBRepTool_CLASSI CLASSI;
  CLASSI.Init2d(FFOR);

  NCollection_List<TopoDS_Shape> null;
  NCollection_List<TopoDS_Shape> oldW;
  int                            noldW = mapOwNw.Extent();
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
    Iterator itm(mapOwNw);

  // noldW = 1
  // ---------
  if (noldW == 1)
  {
    const NCollection_List<TopoDS_Shape>& low = itm.Value();
    bool                                  ok  = CLASSI.Classilist(low, mapWlow);
    return ok;
  }

  // noldW > 1
  // ---------
  NCollection_List<TopoDS_Shape> lOws;
  for (; itm.More(); itm.Next())
  {
    const TopoDS_Shape& owi = itm.Key();
    lOws.Append(owi);
    const NCollection_List<TopoDS_Shape>& low = itm.Value();
    NCollection_List<TopoDS_Shape>        lwresu;
    FUN_addOwlw(owi, low, lwresu);
    NCollection_List<TopoDS_Shape>::Iterator itw(lwresu);
    for (; itw.More(); itw.Next())
      mapWlow.Bind(itw.Value(), null);
  } // itm(mapOwNw)

  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> mapdone;
  int                                                    nOw = noldW;
  int nite = 0, nitemax = int(nOw * (nOw - 1) / 2);
  while (nite <= nitemax)
  {
    nOw = lOws.Extent();
    if (nOw == 0)
      break;

    NCollection_List<TopoDS_Shape>::Iterator itOw(lOws);
    const TopoDS_Shape&                      Ow1  = itOw.Value();
    bool                                     isb1 = mapWlow.IsBound(Ow1);
    isb1                                          = isb1 || !mapdone.Contains(Ow1);
    if (!isb1)
      continue;

    const NCollection_List<TopoDS_Shape>& lw1 = mapOwNw.Find(Ow1);

    if (nOw == 1)
    {
      // all wires of <mapWs> have been treated, except the last one
      // if (nw1 == 0) mapWlow binds already (Ow1,null);
      // else         {mapWlow binds already (w1k,null), w1k in lw1}
      break;
    } // nOw == 1

    itOw.Next();
    bool         OUTall = false;
    TopoDS_Shape Ow2;
    int          sta12 = UNKNOWN;
    for (; itOw.More(); itOw.Next())
    {
      Ow2       = itOw.Value();
      bool isb2 = mapWlow.IsBound(Ow2);
      isb2      = isb2 || !mapdone.Contains(Ow2);
      if (!isb2)
        continue;
      int stabnd2d12 = CLASSI.ClassiBnd2d(Ow1, Ow2, toluv, true);
      sta12          = CLASSI.Classip2d(Ow1, Ow2, stabnd2d12);
      if (sta12 == DIFF)
      {
        OUTall = true;
        continue;
      }
      else if ((sta12 == UNKNOWN) || (sta12 == SAME))
        return false;
      break;
    }
    if (OUTall)
    {
      // if (nw1 == 0) mapWlow binds already (Ow1,null);
      // else         {mapWlow binds already (w1k,null), w1k in lw1}
      NCollection_List<TopoDS_Shape> ldone;
      FUN_addOwlw(Ow1, lw1, ldone);
      NCollection_List<TopoDS_Shape>::Iterator itw(ldone);
      for (; itw.More(); itw.Next())
        mapdone.Add(itw.Value());
#ifdef OCCT_DEBUG
      if (trc)
        std::cout << "old wires :wi" << FUN_adds(Ow1) << " is OUT all old wires" << std::endl;
#endif
      lOws.RemoveFirst();
    } // OUTall
    else
    {
#ifdef OCCT_DEBUG
      if (trc)
      {
        std::cout << "old wires :wi -> ";
        FUN_tool_coutsta(sta12, FUN_adds(Ow1), FUN_adds(Ow2));
        std::cout << std::endl;
      }
#endif
      const NCollection_List<TopoDS_Shape>& lw2 = mapOwNw.Find(Ow2);

      NCollection_List<TopoDS_Shape> lw1r;
      FUN_addOwlw(Ow1, lw1, lw1r);
      NCollection_List<TopoDS_Shape> lw2r;
      FUN_addOwlw(Ow2, lw2, lw2r);
      NCollection_List<TopoDS_Shape> lgre, lsma;
      if (sta12 == oneINtwo)
      {
        lgre.Append(lw2r);
        lsma.Append(lw1r);
      }
      if (sta12 == twoINone)
      {
        lgre.Append(lw1r);
        lsma.Append(lw2r);
      }

      NCollection_List<TopoDS_Shape>::Iterator itsma(lsma);
      for (; itsma.More(); itsma.Next())
      {
        const TopoDS_Shape& wsma   = itsma.Value();
        bool                isbsma = mapWlow.IsBound(wsma);
        isbsma                     = isbsma || !mapdone.Contains(wsma);
        if (!isbsma)
          continue;

        NCollection_List<TopoDS_Shape>::Iterator itgre(lgre);
        for (; itgre.More(); itgre.Next())
        {
          const TopoDS_Shape& wgre   = itgre.Value();
          bool                isbgre = mapWlow.IsBound(wgre);
          isbgre                     = isbgre || !mapdone.Contains(wgre);
          if (!isbgre)
            continue;

          int stabnd2d = CLASSI.ClassiBnd2d(wsma, wgre, toluv, true);
          int sta      = CLASSI.Classip2d(wsma, wgre, stabnd2d);
#ifdef OCCT_DEBUG
          if (trc)
          {
            std::cout << " wires :wi -> ";
            FUN_tool_coutsta(sta, FUN_adds(wsma), FUN_adds(wgre));
            std::cout << std::endl;
          }
#endif

          if (sta == DIFF)
            continue;
          else if (sta == oneINtwo)
          { // wsma IN wgre
            mapWlow.ChangeFind(wgre).Append(mapWlow.ChangeFind(wsma));
            mapWlow.UnBind(wsma);
          }
          else if (sta == twoINone)
          { // wgre IN wsma
            mapWlow.ChangeFind(wsma).Append(mapWlow.ChangeFind(wgre));
            mapWlow.UnBind(wgre);
          }
          else
            return false;
        } // itgre
      } // itsma
      lOws.RemoveFirst();
    } //! OUTall
  } // nite
  return true;
}

//=================================================================================================

bool TopOpeBRepTool::RegularizeFace(
  const TopoDS_Face& theFace,
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                  mapoldWnewW,
  NCollection_List<TopoDS_Shape>& newFaces)
{
  // <mapWlow>
  // ---------
  // key = wire <w>,
  // item = if the new face has holes, the item contains wires
  //        classified IN the area described by the boundary <w>
  //        on <aFace>,
  //        else : the item is an empty list, <w> describes the
  //        whole new face.
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    mapWlow;

  // Classifying  wires :
  // -------------------
  //  bool classifok = FUN_tool_ClassifW(theFace, mapoldWnewW, mapWlow);
  TopoDS_Shape aLocalShape = theFace.Oriented(TopAbs_FORWARD);
  TopoDS_Face  aFace       = TopoDS::Face(aLocalShape);
  //  TopoDS_Face aFace = TopoDS::Face(theFace.Oriented(TopAbs_FORWARD));

  bool classifok = FUN_tool_ClassifW(aFace, mapoldWnewW, mapWlow);
  if (!classifok)
    return false;

  // <aListOfFaces>
  // -------------
  bool facesbuilt = TopOpeBRepTool_TOOL::WireToFace(theFace, mapWlow, newFaces);
  return facesbuilt;
}
