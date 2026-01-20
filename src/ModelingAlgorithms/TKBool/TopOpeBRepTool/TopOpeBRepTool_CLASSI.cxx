// Created on: 1999-01-13
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

#include <Bnd_Box2d.hxx>
#include <NCollection_Array1.hxx>
#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <BRep_Tool.hxx>
#include <BRepClass3d_SolidExplorer.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <NCollection_Array2.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepTool_2d.hxx>
#include <TopOpeBRepTool_CLASSI.hxx>
#include <TopOpeBRepTool_GEOMETRY.hxx>
#include <TopOpeBRepTool_PROJECT.hxx>
#include <TopOpeBRepTool_TOPOLOGY.hxx>
#include <TopOpeBRepTool_face.hxx>
#include <TopOpeBRepTool_TOOL.hxx>

#define SAME (-1)
#define DIFF (-2)
#define UNKNOWN (0)
#define oneINtwo (1)
#define twoINone (2)

#define M_IN(st) (st == TopAbs_IN)
#define M_OUT(st) (st == TopAbs_OUT)

//=================================================================================================

TopOpeBRepTool_CLASSI::TopOpeBRepTool_CLASSI() {}

//=================================================================================================

void TopOpeBRepTool_CLASSI::Init2d(const TopoDS_Face& Fref)
{
  myFref = Fref;
}

//=================================================================================================

bool TopOpeBRepTool_CLASSI::HasInit2d() const
{
  return (!myFref.IsNull());
}

//=================================================================================================

bool TopOpeBRepTool_CLASSI::Add2d(const TopoDS_Shape& S)
{
  if (!HasInit2d())
    return false;

  bool isb = mymapsbox2d.Contains(S);
  if (isb)
    return true;

  Bnd_Box2d       B2d;
  TopExp_Explorer exe(S, TopAbs_EDGE);
  for (; exe.More(); exe.Next())
  {
    const TopoDS_Edge& E    = TopoDS::Edge(exe.Current());
    double      tolE = BRep_Tool::Tolerance(E);

    bool haspc = FC2D_HasCurveOnSurface(E, myFref);
    if (!haspc)
      return false;
    BRepAdaptor_Curve2d BC2d(E, myFref);
    double       tol2d = BC2d.Resolution(tolE);
    BndLib_Add2dCurve::Add(BC2d, tol2d, B2d);
  }
  mymapsbox2d.Add(S, B2d);
  return true;
}

//=================================================================================================

bool TopOpeBRepTool_CLASSI::GetBox2d(const TopoDS_Shape& S, Bnd_Box2d& B2d)
{
  bool isb = mymapsbox2d.Contains(S);
  if (!isb)
    isb = Add2d(S);
  if (!isb)
    return false;
  B2d = mymapsbox2d.FindFromKey(S);
  return true;
}

//=================================================================================================

int TopOpeBRepTool_CLASSI::ClassiBnd2d(const TopoDS_Shape&    S1,
                                                    const TopoDS_Shape&    S2,
                                                    const double    tol,
                                                    const bool chklarge)
{
  NCollection_Array1<Bnd_Box2d> B(1, 2);
  bool  isb = mymapsbox2d.Contains(S1);
  if (!isb)
    isb = Add2d(S1);
  if (!isb)
    return false;
  B(1) = mymapsbox2d.FindFromKey(S1);
  isb  = mymapsbox2d.Contains(S2);
  if (!isb)
    isb = Add2d(S2);
  if (!isb)
    return false;
  B(2) = mymapsbox2d.FindFromKey(S2);

  NCollection_Array2<double> UV(1, 2, 1, 4);
  //  for (int i = 1; i <= 2; i++)
  int i;
  for (i = 1; i <= 2; i++)
    //      (Umin(i), Vmin(i), Umax(i), Vmax(i))
    B(i).Get(UV(i, 1), UV(i, 3), UV(i, 2), UV(i, 4));

#ifdef OCCT_DEBUG
  bool trc = false;
  if (trc)
  {
    for (int j = 1; j <= 2; j++)
      std::cout << "B(" << j << ") = (" << UV(j, 1) << " " << UV(j, 3) << " " << UV(j, 2) << " "
                << UV(j, 4) << ")" << std::endl;
  }
#endif

  for (int k = 1; k <= 3; k += 2)
  {
    for (i = 1; i <= 2; i++)
    {
      int ii = i, jj = (i == 1) ? 2 : 1;
      //  diff = Umin<ii> - Umax<jj> : k = 1
      //  diff = Vmin<ii> - Vmax<jj> : k = 3
      double diff = UV(ii, k) - UV(jj, k + 1);
      // IMPORTANT : for split faces sharing same edge, use
      // chklarge = True.
      bool disjoint = chklarge ? (diff >= -tol) : (diff > 0.);
      if (disjoint)
        return DIFF;
    }
  }

  for (i = 1; i <= 2; i++)
  {
    // comparing Bnd2d(ii) with Bnd2d(jj)
    int ii = i, jj = (i == 1) ? 2 : 1;
    bool smaller = true, same = true;

    //    for (int k = 1; k <= 3; k += 2){
    int k;
    for (k = 1; k <= 3; k += 2)
    {
      //  diff = Umin<ii> - Umin<jj> : k = 1
      //  diff = Vmin<ii> - Vmin<jj> : k = 3
      double diff = UV(ii, k) - UV(jj, k);
      smaller            = chklarge ? (smaller && (diff > -tol)) : (smaller && (diff > 0.));
      same               = same && (std::abs(diff) <= tol);
    }
    for (k = 2; k <= 4; k += 2)
    {
      //  diff = Umax<ii> - Umax<jj> : k = 2
      //  diff = Vmax<ii> - Vmax<jj> : k = 4
      double diff = UV(ii, k) - UV(jj, k);
      smaller            = chklarge ? (smaller && (diff < tol)) : (smaller && (diff < 0.));
      same               = same && (std::abs(diff) <= tol);
    }

    if (same)
      return SAME;
    if (smaller)
    {
      int sta = (ii == 1) ? oneINtwo : twoINone;
      return sta;
    }
  }
  return UNKNOWN;
}

static int FUN_thegreatest(const TopoDS_Face& F1, BRepClass_FaceClassifier& class2)
// prequesitory : p2d1 IN f2, p2d2 IN f1
//                class2 for f2
// returns oneINtwo || twoINone || UNKNOWN
{
  TopExp_Explorer ex1(F1, TopAbs_EDGE);
  double   tolf1 = BRep_Tool::Tolerance(F1);
  for (; ex1.More(); ex1.Next())
  {
    const TopoDS_Edge& e1 = TopoDS::Edge(ex1.Current());
    double      f1, l1;
    FUN_tool_bounds(e1, f1, l1);
    double    x  = 0.45678;
    double    p1 = (1 - x) * f1 + x + l1;
    gp_Pnt2d         uv1;
    bool ok1 = FUN_tool_paronEF(e1, p1, F1, uv1, tolf1);
    if (!ok1)
      continue;
    TopAbs_State sta12 = class2.State();
    if (M_IN(sta12))
      return oneINtwo;
    else if (M_OUT(sta12))
      return twoINone;
  }
  return UNKNOWN;
}

//=================================================================================================

int TopOpeBRepTool_CLASSI::Classip2d(const TopoDS_Shape&    S1,
                                                  const TopoDS_Shape&    S2,
                                                  const int stabnd2d12)
{
  if (!HasInit2d())
    return UNKNOWN;
  bool bnd2dUNK = (stabnd2d12 == UNKNOWN) || (stabnd2d12 == SAME);

  // fa1,ffi1,finite1 :
  TopOpeBRepTool_face fa1;
  bool    isb1 = mymapsface.IsBound(S1);
  if (isb1)
    fa1 = mymapsface.Find(S1);
  else
  {
    bool ok1 = fa1.Init(TopoDS::Wire(S1), myFref);
    if (!ok1)
      return UNKNOWN;
    mymapsface.Bind(S1, fa1);
  }
  const TopoDS_Face& ffi1    = fa1.Ffinite();
  bool   finite1 = fa1.Finite();

  // fa2,ffi2,finite2 :
  TopOpeBRepTool_face fa2;
  bool    isb2 = mymapsface.IsBound(S2);
  if (isb2)
    fa2 = mymapsface.Find(S2);
  else
  {
    bool ok2 = fa2.Init(TopoDS::Wire(S2), myFref);
    if (!ok2)
      return UNKNOWN;
    mymapsface.Bind(S2, fa2);
  }
  const TopoDS_Face& ffi2    = fa2.Ffinite();
  bool   finite2 = fa2.Finite();

  // p2d1 :
  double    u1, v1;
  bool ok1 = BRepClass3d_SolidExplorer::FindAPointInTheFace(ffi1, u1, v1);
  if (!ok1)
    return UNKNOWN;
  gp_Pnt2d p2d1(u1, v1);

  // sta12 = status(p2d1 / ffi2) :
  // recall : ffi1 and ffi2 are built on same face
  // => ffi1(u1,v1) = ffi2(u1,v1)
  // ----------------------------
  double            tol2d2 = TopOpeBRepTool_TOOL::TolUV(ffi2, BRep_Tool::Tolerance(ffi2));
  BRepClass_FaceClassifier class2(ffi2, p2d1, tol2d2);
  TopAbs_State             sta12 = class2.State();

  // staffi12 : only if !bnd2dUNK and stabnd2d12!=DIFF
  // PREQUESITORY : the only possible situations are
  //  - ffi1 IN ffi2
  //  - ffi2 IN ffi1
  //  - ffi1 and ffi2 are distinct
  int staffi12 = UNKNOWN;

  bool try21 = false;
  if (bnd2dUNK)
  {
    try21 = true;
  }
  else if (stabnd2d12 == DIFF)
  {
    return DIFF;
  }
  else
  {
    if (sta12 == TopAbs_IN)
    { // 2 possible states : oneINtwo || twoINone
      if (stabnd2d12 == oneINtwo)
        staffi12 = oneINtwo; // Bnd2d(S1) IN Bnd2d(S2)
      else if (stabnd2d12 == twoINone)
        staffi12 = twoINone; // Bnd2d(S2) IN Bnd2d(S1)
      else
        try21 = true;
    }
    else if (sta12 == TopAbs_OUT)
    { // 2 possible states : twoINone || DIFF
      if (stabnd2d12 == twoINone)
        staffi12 = twoINone;
      //    else if (stabnd2d12 == DIFF)     staffi12 = DIFF;
      else
        try21 = true;
    }
    else
      return UNKNOWN; // NYIxpu140199
  }

  if (try21)
  {
    // p2d2 :
    double    u2, v2;
    bool ok2 = BRepClass3d_SolidExplorer::FindAPointInTheFace(ffi2, u2, v2);
    if (!ok2)
      return UNKNOWN;
    gp_Pnt2d p2d2(u2, v2);

    // sta21 :
    double            tol2d1 = TopOpeBRepTool_TOOL::TolUV(ffi1, BRep_Tool::Tolerance(ffi1));
    BRepClass_FaceClassifier class1(ffi1, p2d2, tol2d1);
    TopAbs_State             sta21 = class1.State();

    if (bnd2dUNK)
    {
      if (M_OUT(sta12) && M_OUT(sta21))
        staffi12 = DIFF;
      else if (M_IN(sta12) && M_OUT(sta21))
        staffi12 = oneINtwo;
      else if (M_OUT(sta12) && M_IN(sta21))
        staffi12 = twoINone;
      else if (M_IN(sta12) && M_IN(sta21))
        staffi12 = ::FUN_thegreatest(ffi1, class2);
    }
    else
    {
      if (sta12 == TopAbs_IN)
      { // 2 possible states : oneINtwo || twoINone
        if (sta21 == TopAbs_OUT)
          staffi12 = oneINtwo;
      }
      else if (sta12 == TopAbs_OUT)
      { // 2 possible states : twoINone || DIFF
        if (sta21 == TopAbs_OUT)
          staffi12 = DIFF;
        else if (sta21 == TopAbs_IN)
          staffi12 = twoINone;
      }
      else
        return UNKNOWN; // NYIxpu140199
    }
  } // try21

  // classif(S1,S2) knowing staffi12, finite, finite2 :
  if (staffi12 == DIFF)
  {
    if (finite1 && finite2)
      return DIFF;
    if (finite1 && !finite2)
      return twoINone;
    if (!finite1 && finite2)
      return oneINtwo;
    return oneINtwo; // !!!!!!! or twoINone
  }
  else if (staffi12 == oneINtwo)
  {
    if (!finite1 && !finite2)
      return twoINone;
    return oneINtwo;
  }
  else if (staffi12 == twoINone)
  {
    if (!finite1 && !finite2)
      return oneINtwo;
    return twoINone;
  }
  return UNKNOWN;
}

//=================================================================================================

bool TopOpeBRepTool_CLASSI::Getface(const TopoDS_Shape&  S,
                                                TopOpeBRepTool_face& fa) const
{
  bool isb = mymapsface.IsBound(S);
  if (!isb)
    return false;
  fa = mymapsface.Find(S);
  return true;
}

//=================================================================================================

Standard_EXPORT void FUN_addOwlw(const TopoDS_Shape&         Ow,
                                 const NCollection_List<TopoDS_Shape>& lw,
                                 NCollection_List<TopoDS_Shape>&       lresu)
{
  int nw = lw.Extent();
  if (nw == 0)
    lresu.Append(Ow);
  else
  {
    NCollection_List<TopoDS_Shape>::Iterator it(lw);
    for (; it.More(); it.Next())
      lresu.Append(it.Value());
  }
}

bool TopOpeBRepTool_CLASSI::Classilist(const NCollection_List<TopoDS_Shape>&         lS,
                                                   NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& mapgreasma)
{
  double        tolref = BRep_Tool::Tolerance(myFref);
  double        toluv  = TopOpeBRepTool_TOOL::TolUV(myFref, tolref); // nyixpu : cdliser??
  NCollection_List<TopoDS_Shape> null;

  NCollection_List<TopoDS_Shape> lw;
  lw.Assign(lS);
  mapgreasma.Clear();
  NCollection_List<TopoDS_Shape>::Iterator anIterW(lS);
  for (; anIterW.More(); anIterW.Next())
    mapgreasma.Bind(anIterW.Value(), null);

  int nw = lw.Extent();
  if (nw <= 1)
    return true;

  int nite = 0, nitemax = int(nw * (nw - 1) / 2);
  while (nite <= nitemax)
  {
    nw = lw.Extent();
    if (nw <= 1)
      break;

    // wi1 :
    TopoDS_Shape                       wi1;
    NCollection_List<TopoDS_Shape>::Iterator itw(lw);
    for (; itw.More(); itw.Next())
    {
      wi1                   = itw.Value();
      bool isb1 = mapgreasma.IsBound(wi1);
      if (!isb1)
        continue; // wi1 stored as smaller shape
      break;
    }

    while (itw.More())
    { // compare wi1 with all wi(k) (k>1)
      bool isb1 = mapgreasma.IsBound(wi1);
      if (!isb1)
        break;

      itw.Next();
      if (!itw.More())
        break;

      // wi2, sta12 :
      int sta12  = UNKNOWN;
      bool OUTall = false;
      TopoDS_Shape     wi2;
      for (; itw.More(); itw.Next())
      {
        wi2                   = itw.Value();
        bool isb2 = mapgreasma.IsBound(wi2);
        if (!isb2)
          continue;

        int stabnd2d12 = ClassiBnd2d(wi1, wi2, toluv, true);
        sta12                       = Classip2d(wi1, wi2, stabnd2d12);
        if (sta12 == DIFF)
        {
          OUTall = true;
          continue;
        }
        break;
      }

      // mapgreasma :
      if (sta12 == oneINtwo)
      { // greater = shape two
        NCollection_List<TopoDS_Shape>& lwgre = mapgreasma.ChangeFind(wi2);
        NCollection_List<TopoDS_Shape>  lsma;
        FUN_addOwlw(wi1, mapgreasma.Find(wi1), lsma);
        mapgreasma.UnBind(wi1);
        lwgre.Append(lsma);
      }
      else if (sta12 == twoINone)
      { // greater = shape one
        NCollection_List<TopoDS_Shape>& lwgre = mapgreasma.ChangeFind(wi1);
        NCollection_List<TopoDS_Shape>  lsma;
        FUN_addOwlw(wi2, mapgreasma.Find(wi2), lsma);
        mapgreasma.UnBind(wi2);
        lwgre.Append(lsma);
      }
      else if (OUTall)
      {
        // nothing's done
      }
      else
        return false;
    } // itw.More()

    lw.RemoveFirst();
  } // nite<=nmax
  return true;
}
