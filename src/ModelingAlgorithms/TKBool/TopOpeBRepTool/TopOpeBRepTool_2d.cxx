// Created on: 1998-03-23
// Created by: Jean Yves LEBEY
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

#include <gp_Vec2d.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TopOpeBRepTool_2d.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Curve.hxx>
#include <ProjLib_ProjectedCurve.hxx>
#include <Geom_Surface.hxx>
#include <TopOpeBRepTool_CurveTool.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepTool_C2DF.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <TopOpeBRepTool_tol.hxx>
#include <TopOpeBRepTool_GEOMETRY.hxx>
#include <TopOpeBRepTool_PROJECT.hxx>
#include <TopOpeBRepTool_TOPOLOGY.hxx>
#include <TopOpeBRepTool_TOOL.hxx>

#ifdef DRAW
  #include <TopOpeBRepTool_DRAW.hxx>
  #include <Geom2d_Curve.hxx>
#endif

#ifdef OCCT_DEBUG
void debc2dnull(void) {}

Standard_EXPORT bool TopOpeBRepTool_GettraceC2D();
#endif

// structure e -> C2D/F
static NCollection_DataMap<TopoDS_Shape,
                           NCollection_List<TopOpeBRepTool_C2DF>,
                           TopTools_ShapeMapHasher>* GLOBAL_pmosloc2df = nullptr;
static int                                           GLOBAL_C2D_i      = 0; // DEB

// structure ancetre
static NCollection_IndexedDataMap<TopoDS_Shape,
                                  NCollection_List<TopoDS_Shape>,
                                  TopTools_ShapeMapHasher>* GLOBAL_pidmoslosc2df = nullptr;
static TopoDS_Face*                                         GLOBAL_pFc2df        = nullptr;
static TopoDS_Shape*                                        GLOBAL_pS1c2df       = nullptr;
static TopoDS_Shape*                                        GLOBAL_pS2c2df       = nullptr;

Standard_EXPORT occ::handle<Geom2d_Curve> MakePCurve(const ProjLib_ProjectedCurve& PC);

// ------------------------------------------------------------------------------------
static const TopoDS_Face& FC2D_FancestorE(const TopoDS_Edge& E)
{
  if (GLOBAL_pmosloc2df == nullptr)
    GLOBAL_pmosloc2df = new NCollection_DataMap<TopoDS_Shape,
                                                NCollection_List<TopOpeBRepTool_C2DF>,
                                                TopTools_ShapeMapHasher>();
  int ancemp = (*GLOBAL_pidmoslosc2df).Extent();
  if (ancemp == 0)
  {
    TopExp::MapShapesAndAncestors(*GLOBAL_pS1c2df,
                                  TopAbs_EDGE,
                                  TopAbs_FACE,
                                  (*GLOBAL_pidmoslosc2df));
    TopExp::MapShapesAndAncestors(*GLOBAL_pS2c2df,
                                  TopAbs_EDGE,
                                  TopAbs_FACE,
                                  (*GLOBAL_pidmoslosc2df));
  }
  bool Eb = (*GLOBAL_pidmoslosc2df).Contains(E);
  if (!Eb)
    return *GLOBAL_pFc2df;
  const NCollection_List<TopoDS_Shape>& lf = (*GLOBAL_pidmoslosc2df).FindFromKey(E);
  if (lf.IsEmpty())
    return *GLOBAL_pFc2df;
  const TopoDS_Face& F = TopoDS::Face(lf.First());
  return F;
}

// ------------------------------------------------------------------------------------
Standard_EXPORT int FC2D_Prepare(const TopoDS_Shape& S1, const TopoDS_Shape& S2)
{
  if (GLOBAL_pmosloc2df == nullptr)
    GLOBAL_pmosloc2df = new NCollection_DataMap<TopoDS_Shape,
                                                NCollection_List<TopOpeBRepTool_C2DF>,
                                                TopTools_ShapeMapHasher>();
  GLOBAL_pmosloc2df->Clear();
  GLOBAL_C2D_i = 0;

  if (GLOBAL_pidmoslosc2df == nullptr)
    GLOBAL_pidmoslosc2df = new NCollection_IndexedDataMap<TopoDS_Shape,
                                                          NCollection_List<TopoDS_Shape>,
                                                          TopTools_ShapeMapHasher>();
  GLOBAL_pidmoslosc2df->Clear();

  if (GLOBAL_pFc2df == nullptr)
    GLOBAL_pFc2df = new TopoDS_Face();
  GLOBAL_pFc2df->Nullify();

  if (GLOBAL_pS1c2df == nullptr)
    GLOBAL_pS1c2df = new TopoDS_Shape();
  *GLOBAL_pS1c2df = S1;

  if (GLOBAL_pS2c2df == nullptr)
    GLOBAL_pS2c2df = new TopoDS_Shape();
  *GLOBAL_pS2c2df = S2;

  return 0;
}

// ------------------------------------------------------------------------------------
Standard_EXPORT bool FC2D_HasC3D(const TopoDS_Edge& E)
{
  TopLoc_Location         loc;
  double                  f3d, l3d;
  occ::handle<Geom_Curve> C3D = BRep_Tool::Curve(E, loc, f3d, l3d);
  bool                    b   = (!C3D.IsNull());
  return b;
}

// ------------------------------------------------------------------------------------
Standard_EXPORT bool FC2D_HasCurveOnSurface(const TopoDS_Edge& E, const TopoDS_Face& F)
{
  occ::handle<Geom2d_Curve> C2D;
  bool                      hasold = FC2D_HasOldCurveOnSurface(E, F, C2D);
  bool                      hasnew = FC2D_HasNewCurveOnSurface(E, F, C2D);
  bool                      b      = hasold || hasnew;
  return b;
}

// ------------------------------------------------------------------------------------
Standard_EXPORT bool FC2D_HasOldCurveOnSurface(const TopoDS_Edge&         E,
                                               const TopoDS_Face&         F,
                                               occ::handle<Geom2d_Curve>& C2D,
                                               double&                    f2d,
                                               double&                    l2d,
                                               double&                    tol)
{
  bool hasold = false;
  tol         = BRep_Tool::Tolerance(E);
  C2D         = BRep_Tool::CurveOnSurface(E, F, f2d, l2d);
  hasold      = (!C2D.IsNull());
  return hasold;
}

Standard_EXPORT bool FC2D_HasOldCurveOnSurface(const TopoDS_Edge&         E,
                                               const TopoDS_Face&         F,
                                               occ::handle<Geom2d_Curve>& C2D)
{
  double f2d, l2d, tol;
  bool   hasold = FC2D_HasOldCurveOnSurface(E, F, C2D, f2d, l2d, tol);
  return hasold;
}

// ------------------------------------------------------------------------------------
static TopOpeBRepTool_C2DF* FC2D_PNewCurveOnSurface(const TopoDS_Edge& E, const TopoDS_Face& F)
{
  TopOpeBRepTool_C2DF* pc2df = nullptr;
  if (GLOBAL_pmosloc2df == nullptr)
    return nullptr;
  bool Eisb = GLOBAL_pmosloc2df->IsBound(E);
  if (!Eisb)
    return nullptr;
  NCollection_List<TopOpeBRepTool_C2DF>::Iterator it(GLOBAL_pmosloc2df->Find(E));
  for (; it.More(); it.Next())
  {
    const TopOpeBRepTool_C2DF& c2df = it.Value();
    bool                       isf  = c2df.IsFace(F);
    if (isf)
    {
      pc2df = (TopOpeBRepTool_C2DF*)&c2df;
      break;
    }
  }
  return pc2df;
}

Standard_EXPORT bool FC2D_HasNewCurveOnSurface(const TopoDS_Edge&         E,
                                               const TopoDS_Face&         F,
                                               occ::handle<Geom2d_Curve>& C2D,
                                               double&                    f2d,
                                               double&                    l2d,
                                               double&                    tol)
{
  const TopOpeBRepTool_C2DF* pc2df  = FC2D_PNewCurveOnSurface(E, F);
  bool                       hasnew = (pc2df != nullptr);
  if (hasnew)
    C2D = pc2df->PC(f2d, l2d, tol);
  return hasnew;
}

Standard_EXPORT bool FC2D_HasNewCurveOnSurface(const TopoDS_Edge&         E,
                                               const TopoDS_Face&         F,
                                               occ::handle<Geom2d_Curve>& C2D)
{
  double f2d, l2d, tol;
  bool   b = FC2D_HasNewCurveOnSurface(E, F, C2D, f2d, l2d, tol);
  return b;
}

// ------------------------------------------------------------------------------------
int FC2D_AddNewCurveOnSurface(occ::handle<Geom2d_Curve> C2D,
                              const TopoDS_Edge&        E,
                              const TopoDS_Face&        F,
                              const double&             f2d,
                              const double&             l2d,
                              const double&             tol)
{
  if (C2D.IsNull())
    return 1;
  TopOpeBRepTool_C2DF c2df(C2D, f2d, l2d, tol, F);
  if (GLOBAL_pmosloc2df == nullptr)
    return 1;
  NCollection_List<TopOpeBRepTool_C2DF> thelist;
  GLOBAL_pmosloc2df->Bind(E, thelist);
  NCollection_List<TopOpeBRepTool_C2DF>& lc2df = GLOBAL_pmosloc2df->ChangeFind(E);
  lc2df.Append(c2df);
  return 0;
}

// ------------------------------------------------------------------------------------
static occ::handle<Geom2d_Curve> FC2D_make2d(const TopoDS_Edge& E,
                                             const TopoDS_Face& F,
                                             double&            f2d,
                                             double&            l2d,
                                             double&            tol,
                                             const bool         trim3d = false);

static occ::handle<Geom2d_Curve> FC2D_make2d(const TopoDS_Edge& E,
                                             const TopoDS_Face& F,
                                             double&            f2d,
                                             double&            l2d,
                                             double&            tol,
                                             const bool         trim3d)
{
  occ::handle<Geom2d_Curve> C2D = BRep_Tool::CurveOnSurface(E, F, f2d, l2d);
  if (!C2D.IsNull())
    return C2D;

  // pas de 2D
  double                  f3d, l3d;
  TopLoc_Location         eloc;
  occ::handle<Geom_Curve> C1     = BRep_Tool::Curve(E, eloc, f3d, l3d);
  bool                    hasC3D = (!C1.IsNull());

  if (hasC3D)
  {
    bool                    elocid = eloc.IsIdentity();
    occ::handle<Geom_Curve> C2;
    if (elocid)
      C2 = C1;
    else
      C2 = occ::down_cast<Geom_Curve>(C1->Transformed(eloc.Transformation()));
    double f = 0., l = 0.;
    if (trim3d)
    {
      f = f3d;
      l = l3d;
    }
    C2D = TopOpeBRepTool_CurveTool::MakePCurveOnFace(F, C2, tol, f, l);
    f2d = f3d;
    l2d = l3d;
    return C2D;
  }
  else
  {
    // E sans courbe 2d sur F, E sans courbe 3d
    // une face accedant a E : FE
    const TopoDS_Face& FE = FC2D_FancestorE(E);
    if (FE.IsNull())
      return C2D;
    bool                             compminmaxUV = false;
    BRepAdaptor_Surface              BAS(F, compminmaxUV);
    occ::handle<BRepAdaptor_Surface> BAHS = new BRepAdaptor_Surface(BAS);
    BRepAdaptor_Curve                AC(E, FE);
    occ::handle<BRepAdaptor_Curve>   AHC = new BRepAdaptor_Curve(AC);
    double                           tolin;
    FTOL_FaceTolerances3d(F, FE, tolin);
    ProjLib_ProjectedCurve projcurv(BAHS, AHC, tolin);
    C2D = MakePCurve(projcurv);
    double f, l;
    BRep_Tool::Range(E, f, l);
    f2d = f;
    l2d = l;
  }

#ifdef OCCT_DEBUG
  if (TopOpeBRepTool_GettraceC2D() && C2D.IsNull())
  {
    std::cout << "#FC2D_make2d1 --> PCurve IsNull" << std::endl;
  }
#endif

  return C2D;
} // make2d1

// ------------------------------------------------------------------------------------
// modified by NIZHNY-MZV  Mon Oct  4 10:37:36 1999
Standard_EXPORT occ::handle<Geom2d_Curve> FC2D_MakeCurveOnSurface(const TopoDS_Edge& E,
                                                                  const TopoDS_Face& F,
                                                                  double&            f,
                                                                  double&            l,
                                                                  double&            tol,
                                                                  const bool         trim3d)
{
#ifdef DRAW
  if (TopOpeBRepTool_GettraceC2D())
  {
    std::cout << "\n#FC2D_MakeCurveOnSurface : " << std::endl;
    TCollection_AsciiString fnam("c2df");
    GLOBAL_C2D_i++;
    fnam = fnam + GLOBAL_C2D_i;
    FDRAW_DINS("", F, fnam, "");
    TCollection_AsciiString enam("c2de");
    GLOBAL_C2D_i++;
    enam = enam + GLOBAL_C2D_i;
    FDRAW_DINE(" ", E, enam, "\n");
    std::cout.flush();
    debc2dnull();
  }
#endif

  occ::handle<Geom2d_Curve> C2D = FC2D_make2d(E, F, f, l, tol, trim3d);
  FC2D_AddNewCurveOnSurface(C2D, E, F, f, l, tol);
  return C2D;
}

// ------------------------------------------------------------------------------------
Standard_EXPORT occ::handle<Geom2d_Curve> FC2D_CurveOnSurface(const TopoDS_Edge& E,
                                                              const TopoDS_Face& F,
                                                              double&            f,
                                                              double&            l,
                                                              double&            tol,
                                                              const bool         trim3d)
{
  occ::handle<Geom2d_Curve> C2D;
  bool                      hasold = FC2D_HasOldCurveOnSurface(E, F, C2D, f, l, tol);
  if (hasold)
  {
    return C2D;
  }
  bool hasnew = FC2D_HasNewCurveOnSurface(E, F, C2D, f, l, tol);
  if (hasnew)
  {
    return C2D;
  }
  C2D = FC2D_MakeCurveOnSurface(E, F, f, l, tol, trim3d);
  return C2D;
}

// ------------------------------------------------------------------------------------
Standard_EXPORT occ::handle<Geom2d_Curve> FC2D_EditableCurveOnSurface(const TopoDS_Edge& E,
                                                                      const TopoDS_Face& F,
                                                                      double&            f,
                                                                      double&            l,
                                                                      double&            tol,
                                                                      const bool         trim3d)
{
  bool hasold = false;
  {
    occ::handle<Geom2d_Curve> C2D;
    hasold = FC2D_HasOldCurveOnSurface(E, F, C2D, f, l, tol);
    if (hasold)
    {
      occ::handle<Geom2d_Curve> copC2D = occ::down_cast<Geom2d_Curve>(C2D->Copy());
      return copC2D;
    }
  }
  bool hasnew = false;
  {
    occ::handle<Geom2d_Curve> newC2D;
    hasnew = FC2D_HasNewCurveOnSurface(E, F, newC2D, f, l, tol);
    if (hasnew)
    {
      return newC2D;
    }
  }
  occ::handle<Geom2d_Curve> makC2D = FC2D_MakeCurveOnSurface(E, F, f, l, tol, trim3d);
  return makC2D;
}

// ------------------------------------------------------------------------------------
static void FC2D_translate(occ::handle<Geom2d_Curve> C2D,
                           //                           const TopoDS_Edge& E,
                           const TopoDS_Edge&,
                           const TopoDS_Face& F,
                           const TopoDS_Edge& EF)
{
  TopLoc_Location                  sloc;
  const occ::handle<Geom_Surface>& S1      = BRep_Tool::Surface(F, sloc);
  bool                             isperio = S1->IsUPeriodic() || S1->IsVPeriodic();
  gp_Dir2d                         d2d;
  gp_Pnt2d                         O2d;
  bool                             isuiso, isviso;
  bool uviso  = TopOpeBRepTool_TOOL::UVISO(C2D, isuiso, isviso, d2d, O2d);
  bool EFnull = EF.IsNull();

  if (isperio && uviso && !EFnull)
  {
    // C2D prend comme origine dans F l'origine de la pcurve de EF dans F
    TopoDS_Face FFOR = F;
    FFOR.Orientation(TopAbs_FORWARD);
    gp_Pnt2d p1, p2;
    BRep_Tool::UVPoints(EF, FFOR, p1, p2);
    double pEF    = isuiso ? p1.X() : p1.Y();
    double pC2D   = isuiso ? O2d.X() : O2d.Y();
    double factor = pEF - pC2D;
    bool   b      = (std::abs(factor) > 1.e-6);
    if (b)
    {
      gp_Vec2d transl(1., 0.);
      if (isviso)
        transl = gp_Vec2d(0., 1.);
      transl.Multiply(factor);
      C2D->Translate(transl);
    }
  }
}

// ------------------------------------------------------------------------------------
static occ::handle<Geom2d_Curve> FC2D_make2d(const TopoDS_Edge& E,
                                             const TopoDS_Face& F,
                                             const TopoDS_Edge& EF,
                                             double&            f2d,
                                             double&            l2d,
                                             double&            tol,
                                             const bool         trim3d = false);

static occ::handle<Geom2d_Curve> FC2D_make2d(const TopoDS_Edge& E,
                                             const TopoDS_Face& F,
                                             const TopoDS_Edge& EF,
                                             double&            f2d,
                                             double&            l2d,
                                             double&            tol,
                                             const bool         trim3d)
{
  occ::handle<Geom2d_Curve> C2D = BRep_Tool::CurveOnSurface(E, F, f2d, l2d);
  if (!C2D.IsNull())
    return C2D;

  // pas de 2D
  double                  f3d, l3d;
  TopLoc_Location         eloc;
  occ::handle<Geom_Curve> C1     = BRep_Tool::Curve(E, eloc, f3d, l3d);
  bool                    hasC3D = (!C1.IsNull());

  if (hasC3D)
  {
    bool                    elocid = eloc.IsIdentity();
    occ::handle<Geom_Curve> C2;
    if (elocid)
      C2 = C1;
    else
      C2 = occ::down_cast<Geom_Curve>(C1->Transformed(eloc.Transformation()));
    double f = 0., l = 0.;
    if (trim3d)
    {
      f = f3d;
      l = l3d;
    }
    C2D = TopOpeBRepTool_CurveTool::MakePCurveOnFace(F, C2, tol, f, l);
    f2d = f3d;
    l2d = l3d;
    FC2D_translate(C2D, E, F, EF);
    return C2D;
  }
  else
  {
    // E sans courbe 2d sur F, E sans courbe 3d
    // une face accedant a E : FE
    const TopoDS_Face& FE = FC2D_FancestorE(E);
    if (FE.IsNull())
      return C2D;
    bool                             compminmaxUV = false;
    BRepAdaptor_Surface              BAS(F, compminmaxUV);
    occ::handle<BRepAdaptor_Surface> BAHS = new BRepAdaptor_Surface(BAS);
    BRepAdaptor_Curve                AC(E, FE);
    occ::handle<BRepAdaptor_Curve>   AHC = new BRepAdaptor_Curve(AC);
    double                           tolin;
    FTOL_FaceTolerances3d(F, FE, tolin);
    ProjLib_ProjectedCurve projcurv(BAHS, AHC, tolin);
    C2D = MakePCurve(projcurv);
    double f, l;
    BRep_Tool::Range(E, f, l);
    f2d = f;
    l2d = l;
    FC2D_translate(C2D, E, F, EF);
  }

#ifdef OCCT_DEBUG
  if (TopOpeBRepTool_GettraceC2D() && C2D.IsNull())
  {
    std::cout << "#FC2D_make2d2 --> PCurve IsNull" << std::endl;
  }
#endif

  return C2D;
} // make2d2

// ------------------------------------------------------------------------------------
Standard_EXPORT occ::handle<Geom2d_Curve> FC2D_CurveOnSurface(const TopoDS_Edge& E,
                                                              const TopoDS_Face& F,
                                                              const TopoDS_Edge& EF,
                                                              double&            f2d,
                                                              double&            l2d,
                                                              double&            tol,
                                                              const bool         trim3d)
{
  occ::handle<Geom2d_Curve> C2D;

  bool hasold = FC2D_HasOldCurveOnSurface(E, F, C2D, f2d, l2d, tol);
  if (hasold)
    return C2D;

  TopOpeBRepTool_C2DF* pc2df = FC2D_PNewCurveOnSurface(E, F);
  if (pc2df != nullptr)
  {
    C2D = pc2df->PC(f2d, l2d, tol);
    FC2D_translate(C2D, E, F, EF);
    pc2df->SetPC(C2D, f2d, l2d, tol);
    return C2D;
  }

#ifdef DRAW
  if (TopOpeBRepTool_GettraceC2D())
  {
    std::cout << "\n#FC2D_CurveOnSurface : " << std::endl;
    TCollection_AsciiString fnam("c2df");
    GLOBAL_C2D_i++;
    fnam = fnam + GLOBAL_C2D_i;
    FDRAW_DINS("", F, fnam, "");
    TCollection_AsciiString enam("c2de");
    GLOBAL_C2D_i++;
    enam = enam + GLOBAL_C2D_i;
    FDRAW_DINE(" ", E, enam, "\n");
    std::cout.flush();
    debc2dnull();
  }
#endif

  C2D = FC2D_make2d(E, F, EF, f2d, l2d, tol, trim3d);
  FC2D_AddNewCurveOnSurface(C2D, E, F, f2d, l2d, tol);
  return C2D;
}
