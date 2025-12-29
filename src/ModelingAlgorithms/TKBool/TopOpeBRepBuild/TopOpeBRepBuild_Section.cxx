// Created on: 1997-01-14
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

#include <Geom2d_TrimmedCurve.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <gp_Pnt.hxx>
#include <Standard_ProgramError.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepBuild_define.hxx>
#include <TopOpeBRepBuild_EdgeBuilder.hxx>
#include <TopOpeBRepBuild_GTool.hxx>
#include <TopOpeBRepBuild_GTopo.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepBuild_PaveSet.hxx>
#include <TopOpeBRepBuild_SolidBuilder.hxx>
#include <TopOpeBRepDS_connex.hxx>
#include <TopOpeBRepDS_CurveExplorer.hxx>
#include <TopOpeBRepDS_CurveIterator.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_ProcessInterferencesTool.hxx>
#include <TopOpeBRepDS_TKI.hxx>
#include <TopOpeBRepTool_2d.hxx>
#include <TopOpeBRepTool_GEOMETRY.hxx>
#include <TopOpeBRepTool_PROJECT.hxx>
#include <TopOpeBRepTool_TOPOLOGY.hxx>
#include <TopOpeBRepTool_ShapeExplorer.hxx>
#include <TopOpeBRepTool_TOOL.hxx>

#define MGhc3 occ::handle<Geom_Curve>
#define MGhc2 occ::handle<Geom2d_Curve>

#ifdef OCCT_DEBUG
extern bool TopOpeBRepBuild_GettraceSPS();

Standard_EXPORT void debsplitse(const int) {}

Standard_EXPORT void debsplitsemess(const int i, const TCollection_AsciiString& s = "");

Standard_EXPORT void debsplitsemess(const int i, const TCollection_AsciiString& s)
{
  std::cout << "+++ debsplitse " << s << " E" << i << std::endl;
  debsplitse(i);
}

Standard_EXPORT void debspseou(const int i)
{
  debsplitsemess(i, "OUT");
}

Standard_EXPORT void debspsein(const int i)
{
  debsplitsemess(i, "IN ");
}

Standard_EXPORT void debspseon(const int i)
{
  debsplitsemess(i, "ON ");
}

extern bool TopOpeBRepTool_GettraceC2D();
#endif

#ifdef DRAW
  #include <TopOpeBRepTool_DRAW.hxx>
#endif

// Standard_IMPORT void FUN_tool_ttranslate(const gp_Vec2d& tvector, const TopoDS_Face& fF,
// TopoDS_Edge& fyE);

#include <TopOpeBRepTool_ShapeTool.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAbs_SurfaceType.hxx>
//---------------------------------------------
static bool FUN_periodicS(const TopoDS_Shape& F)
//---------------------------------------------
{
  if (F.IsNull())
    return false;
  if (F.ShapeType() != TopAbs_FACE)
    return false;
  occ::handle<Geom_Surface> SSS = TopOpeBRepTool_ShapeTool::BASISSURFACE(TopoDS::Face(F));
  GeomAdaptor_Surface       GAS(SSS);
  GeomAbs_SurfaceType       styp     = GAS.GetType();
  bool                      periodic = false;
  if (styp == GeomAbs_Cylinder)
    periodic = true;
  if (styp == GeomAbs_Cone)
    periodic = true;
  if (styp == GeomAbs_Sphere)
    periodic = true;
  // NYI : for Torus,SurfaceOfRevolution..
  return periodic;
}

static bool FUN_periodic(const TopoDS_Face& F, bool& uper, bool& vper)
{
  const occ::handle<Geom_Surface>& su = BRep_Tool::Surface(F);
  uper                                = su->IsUPeriodic();
  vper                                = su->IsVPeriodic();
  bool per                            = (uper || vper);
  return per;
}

static bool FUN_onboundsper(const gp_Pnt2d& uv, const TopoDS_Face& F)
{
  // 2d :
  const occ::handle<Geom_Surface>& su   = BRep_Tool::Surface(F);
  bool                             uclo = su->IsUPeriodic();
  bool                             vclo = su->IsVPeriodic();
  if (!uclo && !vclo)
    return false;

  double u1, u2, v1, v2;
  su->Bounds(u1, u2, v1, v2);
  double toluv = 1.e-8 * 1.e-2; // nyinyitol
  if (uclo)
  {
    double d1  = std::abs(u1 - uv.X());
    bool   on1 = (d1 < toluv);
    double d2  = std::abs(u2 - uv.X());
    bool   on2 = (d2 < toluv);
    return (on1 || on2);
  }
  if (vclo)
  {
    double d1  = std::abs(v1 - uv.Y());
    bool   on1 = (d1 < toluv);
    double d2  = std::abs(v2 - uv.Y());
    bool   on2 = (d2 < toluv);
    return (on1 || on2);
  }
  return false;
}

static bool FUN_onboundsper(const TopoDS_Edge& E, const TopoDS_Face& F, gp_Pnt2d& uv)
// uv is found by projection pnt(E,par)
{
  // 3d :
  double f, l;
  FUN_tool_bounds(E, f, l);
  double x    = 0.45678;
  double par  = f * x + (1 - x) * l;
  double tolF = BRep_Tool::Tolerance(F) * 1.e2; // nyitol
  bool   ok   = FUN_tool_parF(E, par, F, uv, tolF);
  if (!ok)
    return false;

  bool onbp = ::FUN_onboundsper(uv, F);
  return onbp;
}

//-----------------------------------------------------------------------
static bool FUN_PinC(const gp_Pnt&                  P,
                     const occ::handle<Geom_Curve>& C,
                     const double                   pmin,
                     const double                   pmax,
                     const double                   tol)
//-----------------------------------------------------------------------
{
  bool                        PinC = false;
  GeomAPI_ProjectPointOnCurve mydm(P, C, pmin, pmax);
  bool                        dmdone = (mydm.Extrema().IsDone());
  if (dmdone)
  {
    if (mydm.NbPoints())
    {
      double d = mydm.LowerDistance();
      PinC     = (d <= tol);
    }
  }
  return PinC;
}

//-----------------------------------------------------------------------
static bool FUN_PinE(const gp_Pnt& P, const TopoDS_Edge& E)
//-----------------------------------------------------------------------
{
  bool                    PinE = false;
  double                  f, l;
  occ::handle<Geom_Curve> CE   = BRep_Tool::Curve(E, f, l);
  double                  tolE = BRep_Tool::Tolerance(E);
  PinE                         = FUN_PinC(P, CE, f, l, tolE);
  return PinE;
}

#include <BRep_Tool.hxx>
#include <TopOpeBRepDS_ShapeShapeInterference.hxx>
//-----------------------------------------------------------------------
static bool FUN_EstaEE(const TopoDS_Edge& E, const TopAbs_State sta, const TopoDS_Edge& EE)
//-----------------------------------------------------------------------
{
  bool   EinEE = false;
  double f, l;
  // modified by NIZNHY-PKV Wed Nov  3 11:40:14 1999 from

  if (BRep_Tool::Degenerated(E))
  {
    if (sta == TopAbs_IN)
      return false;
    else
      return true;
  }
  // modified by NIZNHY-PKV Wed Nov  3 11:40:19 1999 to

  occ::handle<Geom_Curve> CE = BRep_Tool::Curve(E, f, l);
  double                  t  = 0.417789; // Oi blya... ???
  double                  p  = (1 - t) * f + t * l;
  gp_Pnt                  P  = CE->Value(p);
  EinEE                      = FUN_PinE(P, EE);
  if (sta == TopAbs_IN)
    return EinEE;
  else if (sta == TopAbs_OUT)
    return !EinEE;
  else
    throw Standard_ProgramError("TopOpeBRepBuild FUN_EstaEE on invalid state");
}

//=================================================================================================

void TopOpeBRepBuild_Builder::InitSection()
{
  mySectionDone = false;
  mySection.Clear();
  mySplitSectionEdgesDone = false;
  mySplitON.Clear();
}

//-----------------------------------------------------------------------
// LE : list of edges from where is extracted LEsta = edges located <sta> / edge E
// N.B. : LEsta is expanded and NOT reinitialized
static void FUN_selLEE(NCollection_List<TopoDS_Shape>& LE,
                       const TopoDS_Edge&              E,
                       const TopAbs_State              sta,
                       NCollection_List<TopoDS_Shape>& LEsta)
{
  NCollection_List<TopoDS_Shape>::Iterator it(LE);
  while (it.More())
  {
    const TopoDS_Edge& Ecur = TopoDS::Edge(it.Value());
    bool               ok   = FUN_EstaEE(Ecur, sta, E);
    if (ok)
    {
      LEsta.Append(Ecur);
      LE.Remove(it);
    }
    else
      it.Next();
  }
}

//-----------------------------------------------------------------------
// search the existence of shape E in time as shape of a mesh cell
// (shape,listofshape) of the list loslos.
bool FUN_FindEinSLOS(const TopoDS_Shape&                                       E,
                     const NCollection_List<TopOpeBRepBuild_ShapeListOfShape>& loslos)
{
  bool f = false;
  for (NCollection_List<TopOpeBRepBuild_ShapeListOfShape>::Iterator it(loslos); it.More();
       it.Next())
  {
    const TopoDS_Shape& S      = it.Value().Shape();
    bool                issame = (S.IsSame(E));
    if (issame)
    {
      f = true;
      break;
    }
  }
  return f;
}

//=================================================================================================

void TopOpeBRepBuild_Builder::SplitSectionEdges()
{
  if (mySplitSectionEdgesDone)
    return;

  const TopOpeBRepDS_DataStructure& BDS = myDataStructure->DS();
  int                               i, n = BDS.NbSectionEdges();

  for (i = 1; i <= n; i++)
  { // 1
    const TopoDS_Edge& E = TopoDS::Edge(BDS.SectionEdge(i));
    if (E.IsNull())
      continue;
    SplitSectionEdge(E);
  } // 1

  NCollection_DataMap<TopoDS_Shape,
                      NCollection_List<TopOpeBRepBuild_ShapeListOfShape>,
                      TopTools_ShapeMapHasher>
                                                                                             MEIN;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> MEOUT;

  for (i = 1; i <= n; i++)
  { // 2
    const TopoDS_Edge& E = TopoDS::Edge(BDS.SectionEdge(i));
    if (E.IsNull())
      continue;
    int iE = myDataStructure->Shape(E);
    int rE = BDS.AncestorRank(E);

#ifdef OCCT_DEBUG
    bool tSPS = GtraceSPS(E, iE);
    if (tSPS)
      debsplitsemess(iE);
    int DEBiESD = 1;
#endif

    bool isspliton = IsSplit(E, TopAbs_ON);
    if (!isspliton)
      continue;

    const NCollection_List<TopoDS_Shape>& LESD = BDS.ShapeSameDomain(E);
    if (LESD.IsEmpty())
      continue;

    const NCollection_List<TopoDS_Shape>& LEspon = Splits(E, TopAbs_ON);
    NCollection_List<TopoDS_Shape>        LEoutLESD;
    GCopyList(LEspon, LEoutLESD);
    int iRef = BDS.SameDomainRef(E);

    // LEoutLESD = list of edges Split(E,ON) OUT of all
    // edges of the list of edges same domain of E in 3d only
    // edges LEoutLESD are attached in Split(ON) to E only.

    for (NCollection_List<TopoDS_Shape>::Iterator itLESD(LESD); itLESD.More(); itLESD.Next())
    { // 2.1
      const TopoDS_Edge&                    ESD      = TopoDS::Edge(itLESD.Value());
      int                                   iESD     = myDataStructure->Shape(ESD);
      const NCollection_List<TopoDS_Shape>& LESDspon = Splits(ESD, TopAbs_ON);

#ifdef OCCT_DEBUG
      if (tSPS)
      {
        TCollection_AsciiString str("# edge ");
        str = str + iE + " sd3d edge " + iESD;
        TCollection_AsciiString stru(str.Length(), '-');
        std::cout << std::endl;
        if (DEBiESD == 1)
          std::cout << stru << std::endl;
        DEBiESD++;
        std::cout << str << std::endl;
        debsplitsemess(iE);
      }
#endif

      // reduction of LEoutLESD = edges OUT all ESD
      NCollection_List<TopoDS_Shape> dummylos;
      FUN_selLEE(LEoutLESD, ESD, TopAbs_IN, dummylos);

      const TopoDS_Edge *                  pE1 = nullptr, *pE2 = nullptr;
      const NCollection_List<TopoDS_Shape>*plos1 = nullptr, *plos2 = nullptr;
      int                                  nLEspon   = LEspon.Extent();
      int                                  nLESDspon = LESDspon.Extent();

      if (nLEspon != 0 && nLESDspon != 0)
      {
        bool takeE = ((rE == 1 && iESD != iRef) || iE == iRef);
        pE1        = takeE ? &E : &ESD;
        pE2        = !takeE ? &E : &ESD;
        plos1      = takeE ? &LEspon : &LESDspon;
        plos2      = !takeE ? &LEspon : &LESDspon;
      }
      else if (nLEspon != 0)
      {
        pE1   = &E;
        pE2   = &ESD;
        plos1 = &LEspon;
        plos2 = &LESDspon;
      }
      else if (nLESDspon != 0)
      {
        pE1   = &ESD;
        pE2   = &E;
        plos1 = &LESDspon;
        plos2 = &LEspon;
      }

      if (pE1 == nullptr || pE2 == nullptr)
        continue;
      if (plos1 == nullptr || plos2 == nullptr)
        continue;

      const TopoDS_Edge&                    E1  = *pE1;
      const TopoDS_Edge&                    E2  = *pE2;
      const NCollection_List<TopoDS_Shape>& LE1 = *plos1;

      // selection of edges IN E2 = LEinE2
      NCollection_List<TopoDS_Shape> LE1loc;
      GCopyList(LE1, LE1loc);
      NCollection_List<TopoDS_Shape> LE1inE2;
      FUN_selLEE(LE1loc, E2, TopAbs_IN, LE1inE2);
      int nLE1inE2 = LE1inE2.Extent();

#ifdef DRAW
      if (tSPS)
      {
        std::cout << "# edges ON " << iE << " ";
        TopAbs::Print(TopAbs_IN, std::cout);
        std::cout << " / esd";
        std::cout << " " << iESD;
        std::cout << " : (" << nLE1inE2 << ")" << std::endl;
        TCollection_AsciiString str("ON");
        str = str + iE + "IN" + iESD;
        FDRAW_DINLOE("   ", LE1inE2, str, "");
      }
#endif

      // edges E1 and E2 share LE1inE2
      if (nLE1inE2 != 0)
      { // 2.2

        bool                                               E1b = MEIN.IsBound(E1);
        NCollection_List<TopOpeBRepBuild_ShapeListOfShape> thelist;
        if (!E1b)
          MEIN.Bind(E1, thelist);
        NCollection_List<TopOpeBRepBuild_ShapeListOfShape>& LE1loslos = MEIN.ChangeFind(E1);
#ifdef OCCT_DEBUG
//	int nLE1 = LE1loslos.Extent();
#endif

        bool                                               E2b = MEIN.IsBound(E2);
        NCollection_List<TopOpeBRepBuild_ShapeListOfShape> thelist2;
        if (!E2b)
          MEIN.Bind(E2, thelist2);
        NCollection_List<TopOpeBRepBuild_ShapeListOfShape>& LE2loslos = MEIN.ChangeFind(E2);
#ifdef OCCT_DEBUG
//	int nLE2 = LE2loslos.Extent();
#endif

        bool E2elemofE1 = FUN_FindEinSLOS(E2, LE1loslos);
        bool E1elemofE2 = FUN_FindEinSLOS(E1, LE2loslos);

        bool condadd = (!E2elemofE1 && !E1elemofE2);
        if (condadd)
        {
          // regularization of edges of LE1inE2 -> LR
          NCollection_List<TopoDS_Shape> LR;
          for (NCollection_List<TopoDS_Shape>::Iterator ite(LE1inE2); ite.More(); ite.Next())
          {
            const TopoDS_Edge&             e = TopoDS::Edge(ite.Value());
            NCollection_List<TopoDS_Shape> newle;
            bool                           ok = TopOpeBRepTool_TOOL::SplitE(e, newle);
            if (ok)
              LR.Append(newle);
            else
              LR.Append(e);
          }
          {
            TopOpeBRepBuild_ShapeListOfShape thelist3;
            LE1loslos.Append(thelist3);
            TopOpeBRepBuild_ShapeListOfShape& E1slos = LE1loslos.Last();
            E1slos.ChangeShape()                     = E2;
            GCopyList(LR, E1slos.ChangeList());
          }
          {
            TopOpeBRepBuild_ShapeListOfShape thelist4;
            LE2loslos.Append(thelist4);
            TopOpeBRepBuild_ShapeListOfShape& E2slos = LE2loslos.Last();
            E2slos.ChangeShape()                     = E1;
            GCopyList(LR, E2slos.ChangeList());
          }
        }
      } // 2.2
    }

#ifdef DRAW
    if (tSPS)
    {
      std::cout << std::endl << "# edges ON " << iE << " ";
      TopAbs::Print(TopAbs_OUT, std::cout);
      std::cout << " / lesd";
      for (NCollection_List<TopoDS_Shape>::Iterator it(LESD); it.More(); it.Next())
        std::cout << " " << myDataStructure->Shape(it.Value());
      int n = LEoutLESD.Extent();
      std::cout << " : (" << n << ")" << std::endl;
      TCollection_AsciiString str("ON");
      str = str + iE + "OUT";
      FDRAW_DINLOE("   ", LEoutLESD, str, "");
    }
#endif

    if (!MEOUT.IsBound(E))
    {
      NCollection_List<TopoDS_Shape> thelist5;
      MEOUT.Bind(E, thelist5);
    }
    GCopyList(LEoutLESD, MEOUT.ChangeFind(E));

  } // 2

  for (i = 1; i <= n; i++)
  { // 3
    const TopoDS_Edge& E = TopoDS::Edge(BDS.SectionEdge(i));
    if (E.IsNull())
      continue;
#ifdef OCCT_DEBUG
    int iE = myDataStructure->Shape(E);
    //    int rE = GShapeRank(E);
    bool tSPS = GtraceSPS(E, iE);
    if (tSPS)
      debsplitsemess(iE);
#endif

    bool isspliton = IsSplit(E, TopAbs_ON);
    if (!isspliton)
      continue;

    const NCollection_List<TopoDS_Shape>& LESD = BDS.ShapeSameDomain(E);
    if (LESD.IsEmpty())
      continue;

    bool isbMEOUT = MEOUT.IsBound(E);
    bool isbMEIN  = MEIN.IsBound(E);
    if (!isbMEOUT && !isbMEIN)
      continue;

    NCollection_List<TopoDS_Shape>& LEspon = ChangeSplit(E, TopAbs_ON);
    LEspon.Clear();

    if (isbMEOUT)
    {
      const NCollection_List<TopoDS_Shape>& LEOUT = MEOUT.Find(E);
#ifdef OCCT_DEBUG
//      int nOUT = LEOUT.Extent();
#endif
      GCopyList(LEOUT, LEspon);
    }

    if (isbMEIN)
    {
      const NCollection_List<TopOpeBRepBuild_ShapeListOfShape>& loslos = MEIN.Find(E);
#ifdef OCCT_DEBUG
//      int nloslos = loslos.Extent();
#endif
      for (NCollection_List<TopOpeBRepBuild_ShapeListOfShape>::Iterator it(loslos); it.More();
           it.Next())
      {
        const NCollection_List<TopoDS_Shape>& los = it.Value().List();
#ifdef OCCT_DEBUG
//	int nlos = los.Extent();
#endif
        GCopyList(los, LEspon);
      }
    }
  } // 3

  BRep_Builder BB;
  for (i = 1; i <= n; i++)
  { // 4
    const TopoDS_Edge& E = TopoDS::Edge(BDS.SectionEdge(i));
    if (E.IsNull())
      continue;
#ifdef OCCT_DEBUG
    int  idebE;
    bool tSPS = GtraceSPS(E, idebE);
    if (tSPS)
      debsplitsemess(idebE);
#endif
    const NCollection_List<TopoDS_Shape>& lesd = BDS.ShapeSameDomain(E);
    if (lesd.IsEmpty())
      continue;

    int iE = BDS.Shape(E);
#ifdef OCCT_DEBUG
//    int rE = BDS.AncestorRank(E);
#endif
    int RE = BDS.SameDomainRef(E);
    if (iE != RE)
      continue;

    NCollection_List<TopoDS_Shape>           lF;
    NCollection_List<TopoDS_Shape>::Iterator itlesd;
    for (itlesd.Initialize(lesd); itlesd.More(); itlesd.Next())
    {
      const TopoDS_Edge& esd = TopoDS::Edge(itlesd.Value());
#ifdef OCCT_DEBUG
//      int iesd = BDS.Shape(esd);
#endif
      const NCollection_List<TopoDS_Shape>& lf =
        FDSCNX_EdgeConnexitySameShape(esd, myDataStructure);
      GCopyList(lf, lF);
    }
#ifdef OCCT_DEBUG
//    int nlF = lF.Extent();
#endif

    NCollection_List<TopoDS_Shape>&          lon  = ChangeSplit(E, TopAbs_ON);
    double                                   tolE = BRep_Tool::Tolerance(E);
    NCollection_List<TopoDS_Shape>::Iterator it(lF);
    for (; it.More(); it.Next())
    {
      const TopoDS_Face& F  = TopoDS::Face(it.Value());
      int                iF = BDS.Shape(F);
      int                rF = BDS.AncestorRank(iF);

      TopoDS_Edge esdF;
      bool        besdF = false; // NYI pointer on esdF
      for (itlesd.Initialize(lesd); itlesd.More(); itlesd.Next())
      {
        const TopoDS_Edge& esd  = TopoDS::Edge(itlesd.Value());
        int                iesd = BDS.Shape(esd);
        int                resd = BDS.AncestorRank(iesd);
        if (resd == rF)
        {
          TopExp_Explorer ex;
          for (ex.Init(F, TopAbs_EDGE); ex.More(); ex.Next())
          {
            //	  for (TopExp_Explorer ex(F,TopAbs_EDGE);ex.More();ex.Next()) {
            const TopoDS_Shape& ee = ex.Current();
            bool                eq = (ee.IsEqual(esd));
            if (eq)
            {
              esdF  = esd;
              besdF = true;
              break;
            }
          }
        }
        if (besdF)
          break;
      }

      NCollection_List<TopoDS_Shape>::Iterator itlon(lon);
      for (; itlon.More(); itlon.Next())
      {
        TopoDS_Edge& eon = TopoDS::Edge(itlon.ChangeValue());
        double       f, l;
        bool         hasPC = FC2D_HasCurveOnSurface(eon, F);
        if (hasPC)
          continue;
#ifdef OCCT_DEBUG
        if (TopOpeBRepTool_GettraceC2D())
        {
          std::cout << "\n#TopOpeBRepBuild_Builder::SSE : hasPC = 0 ES" << i << " E" << idebE
                    << " sur F" << iF << std::endl;
          std::cout << "tsee s " << iF << " " << idebE << ";" << std::endl;
        }
#endif
        //	double tolpc; MGhc2 PC = FC2D_CurveOnSurface(eon,F,esdF,f,l,tolpc);
        // clang-format off
	double tolpc; MGhc2 PC = FC2D_CurveOnSurface(eon,F,esdF,f,l,tolpc,true);//xpu051198 :PRO15049
        // clang-format on
        hasPC = (!PC.IsNull());
        if (!hasPC)
          throw Standard_ProgramError("TopOpeBRepBuild_Builder::SSE null PC on F");
        double tol = std::max(tolE, tolpc);
        BB.UpdateEdge(eon, PC, F, tol);
      }
    }
  } // 4

  int nsha = BDS.NbShapes();
  for (i = 1; i <= nsha; i++)
  { // 5
    const TopoDS_Shape& FOR    = myDataStructure->Shape(i);
    bool                isface = (FOR.ShapeType() == TopAbs_FACE);
    if (!isface)
      continue;
    const TopoDS_Face& FF = TopoDS::Face(FOR);
#ifdef OCCT_DEBUG
//    int iFF = BDS.Shape(FF);
#endif
    const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI = BDS.ShapeInterferences(FF);
    int                                                             nLI = LI.Extent();
    if (nLI == 0)
      continue;
    for (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator ILI(LI); ILI.More();
         ILI.Next())
    {
      occ::handle<TopOpeBRepDS_ShapeShapeInterference> SSI(
        occ::down_cast<TopOpeBRepDS_ShapeShapeInterference>(ILI.Value()));
      if (SSI.IsNull())
        continue;
      TopOpeBRepDS_Kind GT, ST;
      int               GI, SI;
      FDS_data(SSI, GT, GI, ST, SI);
      if (ST != TopOpeBRepDS_FACE)
        continue;
      const TopOpeBRepDS_Transition& TFE  = SSI->Transition();
      TopAbs_ShapeEnum               shab = TFE.ShapeBefore(), shaa = TFE.ShapeAfter();
      if (shaa != TopAbs_FACE || shab != TopAbs_FACE)
        continue;
      const TopoDS_Face& FS = TopoDS::Face(myDataStructure->Shape(SI));
#ifdef OCCT_DEBUG
//      int iFS = myDataStructure->Shape(FS);
#endif
      bool FSisper = FUN_periodicS(FS);
      if (!FSisper)
        continue;

      const TopoDS_Edge& EG = TopoDS::Edge(myDataStructure->Shape(GI));
#ifdef OCCT_DEBUG
//      int iEG = myDataStructure->Shape(EG);
#endif
      bool isrest = myDataStructure->DS().IsSectionEdge(EG);
      if (!isrest)
        continue;
#ifdef OCCT_DEBUG
//      double tolE = BRep_Tool::Tolerance(EG);
#endif
      bool haspc = FC2D_HasCurveOnSurface(EG, FS);
      if (haspc)
        continue;
      bool hasc3d = FC2D_HasC3D(EG);
      if (!hasc3d)
        throw Standard_ProgramError("TopOpeBRepBuild_Builder::SSE EG without C3D");
      double                    pf, pl, tolpc;
      occ::handle<Geom2d_Curve> PC;
      bool                      trim3d = true;
      PC                               = FC2D_CurveOnSurface(EG, FS, pf, pl, tolpc, trim3d);
      if (PC.IsNull())
        throw Standard_ProgramError("TopOpeBRepBuild_Builder::SSE EG without PC on FS");
    }
  } // 5

  for (i = 1; i <= nsha; i++)
  { // 6
    // xpu201198, for all periodic surfaces
    const TopoDS_Shape& FOR    = myDataStructure->Shape(i);
    bool                isface = (FOR.ShapeType() == TopAbs_FACE);
    if (!isface)
      continue;
    const TopoDS_Face& FF = TopoDS::Face(FOR);
#ifdef OCCT_DEBUG
//    int iFF = BDS.Shape(FF);
#endif
    bool FFuper, FFvper;
    bool FFisper = FUN_periodic(FF, FFuper, FFvper);
    if (!FFisper)
      continue;

    const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI = BDS.ShapeInterferences(FF);
    int                                                             nLI = LI.Extent();
    if (nLI == 0)
      continue;
    for (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator ILI(LI); ILI.More();
         ILI.Next())
    {
      occ::handle<TopOpeBRepDS_ShapeShapeInterference> SSI(
        occ::down_cast<TopOpeBRepDS_ShapeShapeInterference>(ILI.Value()));
      if (SSI.IsNull())
        continue;
      TopOpeBRepDS_Kind GT, ST;
      int               GI, SI;
      FDS_data(SSI, GT, GI, ST, SI);
      if (ST != TopOpeBRepDS_FACE)
        continue;
      bool GB = SSI->GBound();
      if (GB == 1)
        continue;

      const TopoDS_Edge& EG = TopoDS::Edge(myDataStructure->Shape(GI));
#ifdef OCCT_DEBUG
//      int iEG = myDataStructure->Shape(EG);
#endif
      bool isrest = myDataStructure->DS().IsSectionEdge(EG);
      if (!isrest)
        continue;

      // xpu191198 : cto016*
      double                    pf, pl, tolpc;
      occ::handle<Geom2d_Curve> PC;
      bool                      trim3d = true;
      PC                               = FC2D_CurveOnSurface(EG, FF, pf, pl, tolpc, trim3d);
      bool     isoU, isoV;
      gp_Pnt2d o2d;
      gp_Dir2d d2d;
      bool     ISO = TopOpeBRepTool_TOOL::UVISO(PC, isoU, isoV, d2d, o2d);
      if (ISO)
      {
        NCollection_List<TopoDS_Shape>::Iterator itON(Splits(EG, TopAbs_ON));
        NCollection_List<TopoDS_Shape>           newlON;
        for (; itON.More(); itON.Next())
        {
          TopoDS_Edge eon = TopoDS::Edge(itON.Value());

          double                    pfon, plon, tolpcon;
          occ::handle<Geom2d_Curve> PCon;
          PCon = FC2D_CurveOnSurface(eon, FF, pfon, plon, tolpcon, trim3d);

          bool     isouon, isovon;
          gp_Pnt2d o2don;
          gp_Dir2d d2don;
          bool     ISOon = TopOpeBRepTool_TOOL::UVISO(PCon, isouon, isovon, d2don, o2don);
          bool     PCko  = !ISOon || ((isoU && !isouon) || (isoV && !isovon));
          if (PCko)
            throw Standard_ProgramError("TopOpeBRepBuild_Builder::splitON");

          bool test = (FFuper && isoU) || (FFvper && isoV);
          if (!test)
          {
            newlON.Append(eon);
            continue;
          }

          gp_Pnt2d uvok;
          bool     isonclo = FUN_onboundsper(eon, FF, uvok); // 3d
          if (isonclo)
          {
            newlON.Append(eon);
            continue;
          }

          bool isonclo2 = FUN_onboundsper(o2don, FF); // 2d
          if (isonclo2)
          {
            gp_Vec2d tr;
            if (isoU)
            {
              double dtr = uvok.X() - o2don.X();
              tr         = gp_Vec2d(dtr, 0.);
            }
            else
            {
              double dtr = uvok.Y() - o2don.Y();
              tr         = gp_Vec2d(0., dtr);
            }
            double mag   = tr.Magnitude();
            double toluv = 1.e-8 * 1.e2; // NYINYI
            if (mag > toluv)
              TopOpeBRepTool_TOOL::TrslUVModifE(tr, FF, eon);
          }
          newlON.Append(eon);
        } // itON
        NCollection_List<TopoDS_Shape>& nlON = ChangeSplit(EG, TopAbs_ON);
        nlON.Clear();
        nlON.Append(newlON);
      } // ISO
    } // ILI(LI)
  } // 6
  mySplitSectionEdgesDone = true;
} // SplitSectionEdges

#define TheIN (1)
#define TheON (2)
#define TheOUT (3)
int GLOBAL_issp = 0; //++

#define HASSD2d (2)
#define HASSD3d (3)
Standard_EXPORT int GLOBAL_hassd = 0; //++

//=================================================================================================

void TopOpeBRepBuild_Builder::SplitSectionEdge(const TopoDS_Shape& EA)
{
#ifdef OCCT_DEBUG
  int  iE;
  bool tSPS = GtraceSPS(EA, iE);
  if (tSPS)
    debsplitsemess(iE);
#endif

  TopOpeBRepDS_DataStructure& BDS = myDataStructure->ChangeDS();
  const TopoDS_Edge&          EOR = TopoDS::Edge(EA);
  TopoDS_Edge                 EF  = EOR;
  EF.Orientation(TopAbs_FORWARD);
  // clang-format off
  int rankEF = myDataStructure->DS().AncestorRank(EF); // GShapeRank <- GMapShapes, appele par Merge
  // clang-format on

  //  FUN_removeonGB(myDataStructure,EOR); //xpu041198

  bool hg    = myDataStructure->HasGeometry(EOR);
  bool hsd3d = FDS_HasSameDomain3d(BDS, EOR);
  bool hsd2d = FDS_HasSameDomain2d(BDS, EOR);
#ifdef OCCT_DEBUG
  bool issplit = IsSplit(EOR, TopAbs_ON);
#endif

  bool cond = (hg || hsd3d); // REST2 //( hg && (!hsd) );

  GLOBAL_hassd = 0; // initializing
  if (hsd3d)
    GLOBAL_hassd = 3; //++
  if (hsd2d)
    GLOBAL_hassd = 2; //++

  if (mySplitSectionEdgesDone)
  {
#ifdef OCCT_DEBUG
    if (tSPS)
    {
      GdumpSHA(EF, (char*)"SplitSectionEdges done : ");
      if (issplit)
        std::cout << " " << Splits(EOR, TopAbs_ON).Extent() << " edges splitON" << std::endl;
      else
        std::cout << " !IsSplit" << std::endl;
    }
#endif
    return;
  }

#ifdef OCCT_DEBUG
  if (tSPS)
    GdumpSHASTA(EF, TopAbs_ON, "--- SplitSectionEdges ");
  if (tSPS)
    std::cout << " (hg=" << hg << "||hsd3d=" << hsd3d << ")=" << cond << std::endl;
#endif

  // xpu161198 BUC60382(e3on) SE EOR has all its interferences "on bounds"
  bool          allGb1 = false;
  TopoDS_Vertex vf, vl;
  TopExp::Vertices(TopoDS::Edge(EOR), vf, vl);
  const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& loi = BDS.ShapeInterferences(EOR);
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(loi);
  for (; it.More(); it.Next())
  {
    const occ::handle<TopOpeBRepDS_Interference>& I = it.Value();
    TopOpeBRepDS_Kind                             GT, ST;
    int                                           G, S;
    FDS_data(I, GT, G, ST, S);
    if (GT == TopOpeBRepDS_POINT)
    {
      allGb1 = false;
      break;
    }
    int                  rkG = BDS.AncestorRank(G);
    const TopoDS_Vertex& v   = TopoDS::Vertex(BDS.Shape(G));
    if (rkG == rankEF)
    {
      allGb1 = (v.IsSame(vf) || v.IsSame(vl));
    }
    else
    {
      TopoDS_Shape oov;
      bool         hsdmv = FUN_ds_getoov(v, myDataStructure, oov);
      if (!hsdmv)
        allGb1 = false;
      allGb1 = (oov.IsSame(vf) || oov.IsSame(vl));
    }
    if (!allGb1)
      break;
  } // tki

  bool mke = !cond;
  mke      = mke || !hg;    // xpu271098 cto002J2(e9on)
  mke      = mke || allGb1; // xpu161198 BUC60382(e3on)
  if (mke)
  {
    MarkSplit(EOR, TopAbs_ON);
    NCollection_List<TopoDS_Shape>& LON = ChangeSplit(EOR, TopAbs_ON);
#ifdef OCCT_DEBUG
//    int non = LON.Extent(); // DEB
#endif

    TopoDS_Edge newEOR;
    FUN_ds_CopyEdge(EOR, newEOR);
    bool            hasnewEOR = false;
    BRep_Builder    BB;
    TopExp_Explorer exv(EOR, TopAbs_VERTEX);
    for (; exv.More(); exv.Next())
    {
      const TopoDS_Vertex& v     = TopoDS::Vertex(exv.Current());
      double               parv  = BRep_Tool::Parameter(v, EOR);
      int                  iv    = BDS.Shape(v);
      int                  ivref = 0;
      bool                 hsd   = false;
      if (iv != 0)
        hsd = myDataStructure->HasSameDomain(v);
      if (hsd)
        ivref = myDataStructure->SameDomainReference(v);
      bool          setref = hsd && (iv != ivref);
      TopoDS_Vertex vref   = TopoDS::Vertex(BDS.Shape(ivref));
      if (!setref)
      {
        BB.Add(newEOR, v);
        FUN_ds_Parameter(newEOR, v, parv);
        continue;
      }
      hasnewEOR = true;
      vref.Orientation(v.Orientation());
      BB.Add(newEOR, vref);
      FUN_ds_Parameter(newEOR, vref, parv);
    }
    if (hasnewEOR)
      LON.Append(newEOR);
    else
      LON.Append(EOR);
    return;
  } // mke

  NCollection_List<TopoDS_Shape> LESD1, LESD2;
  GFindSamDom(EOR, LESD1, LESD2);

#ifdef OCCT_DEBUG
  if (tSPS)
    GdumpSHASTA(EF, TopAbs_ON, "--- SplitSectionEdges ");
  if (tSPS)
  {
    std::cout << " (hg=" << hg << "||hsd3d=" << hsd3d << ")=" << cond << std::endl;
    GdumpSHA(EOR, (char*)"SplitSectionEdge");
    std::cout << std::endl;
    GdumpSAMDOM(LESD1, (char*)"LESD1 : ");
    GdumpSAMDOM(LESD2, (char*)"LESD2 : ");
  }
#endif

  {
#ifdef OCCT_DEBUG
    if (tSPS)
      debspseon(iE);
#endif
    TopOpeBRepBuild_GTopo G = TopOpeBRepBuild_GTool::GComUnsh(TopAbs_FACE, TopAbs_FACE);
    myEdgeReference         = EF;
    TopOpeBRepBuild_PaveSet PVS(EF);

    GLOBAL_issp = TheON; //++
    GFillEdgePVS(EF, myEmptyShapeList, G, PVS);
    GLOBAL_issp = 0; //++

    // Create an edge builder EDBU
    TopOpeBRepBuild_PaveClassifier VCL(EF);
    bool                           equalpar = PVS.HasEqualParameters();
    if (equalpar)
      VCL.SetFirstParameter(PVS.EqualParameters());
    TopOpeBRepBuild_EdgeBuilder EDBU;
    EDBU.InitEdgeBuilder(PVS, VCL);

    // Build the new edges LEM
    NCollection_List<TopoDS_Shape> LEM;
    GEDBUMakeEdges(EF, EDBU, LEM);

    // xpu : 18-03-98 (cto 016 B2)
    //       splits edges of LEM with internal vertices
    NCollection_List<TopoDS_Shape>           newLEM;
    NCollection_List<TopoDS_Shape>::Iterator ite(LEM);
    for (; ite.More(); ite.Next())
    {
      const TopoDS_Edge&             esp = TopoDS::Edge(ite.Value());
      NCollection_List<TopoDS_Shape> lspe;
      bool                           ok      = TopOpeBRepTool_TOOL::SplitE(esp, lspe);
      bool                           nonwesp = (!ok) || (lspe.Extent() < 2);
      if (nonwesp)
        newLEM.Append(esp);
      else
        newLEM.Append(lspe);
    }
    LEM.Clear();
    LEM.Append(newLEM);

    // connect new edges LEM as split parts (ON,SOLID)
    MarkSplit(EOR, TopAbs_ON);
    NCollection_List<TopoDS_Shape>& LON = ChangeSplit(EOR, TopAbs_ON);
    GCopyList(LEM, LON);
  }

  // modified by NIZHNY-MZV  Mon Apr 17 16:25:51 2000  NCollection_List<TopoDS_Shape> losOO;
  // modified by NIZHNY-MZV  Mon Apr 17 16:25:52 2000  if      (rankEF == 1)
  // losOO.Append(GLOBALDS_shape2); modified by NIZHNY-MZV  Mon Apr 17 16:25:53 2000  else if
  // (rankEF == 2) losOO.Append(GLOBALDS_shape1);

  {
#ifdef OCCT_DEBUG
    if (tSPS)
      debspsein(iE);
#endif
    TopOpeBRepBuild_GTopo G = TopOpeBRepBuild_GTool::GCutUnsh(TopAbs_FACE, TopAbs_FACE);
    G                       = G.CopyPermuted(); // parts (IN,OUT) 3d are constructed
    myEdgeReference         = EF;
    TopOpeBRepBuild_PaveSet PVS(EF);

    GLOBAL_issp = TheIN; //++
    GFillEdgePVS(EF, myEmptyShapeList, G, PVS);
    GLOBAL_issp = 0; //++

    // Create an edge builder EDBU
    TopOpeBRepBuild_PaveClassifier VCL(EF);
    bool                           equalpar = PVS.HasEqualParameters();
    if (equalpar)
      VCL.SetFirstParameter(PVS.EqualParameters());
    TopOpeBRepBuild_EdgeBuilder EDBU;
    EDBU.InitEdgeBuilder(PVS, VCL);

    // Build the new edges LEM
    NCollection_List<TopoDS_Shape> LEMNC;
    GEDBUMakeEdges(EF, EDBU, LEMNC);

    // modified by NIZHNY-MZV  Mon Apr 17 15:23:28 2000    NCollection_List<TopoDS_Shape> LEM;
    // modified by NIZHNY-MZV  Mon Apr 17 15:23:16 2000 GKeepShapes(EF,losOO,TopAbs_IN,LEMNC,LEM);

    // connect new edges LEM as split parts (IN,SOLID)
    MarkSplit(EOR, TopAbs_IN);
    NCollection_List<TopoDS_Shape>& LINN = ChangeSplit(EOR, TopAbs_IN);
    GCopyList(LEMNC, LINN);
  }

  {
#ifdef OCCT_DEBUG
    if (tSPS)
      debspseou(iE);
#endif
    TopOpeBRepBuild_GTopo G = TopOpeBRepBuild_GTool::GCutUnsh(TopAbs_FACE, TopAbs_FACE);
    // parts (OUT,IN) 3d are constructed
    myEdgeReference = EF;
    TopOpeBRepBuild_PaveSet PVS(EF);

    GLOBAL_issp = TheOUT; //++
    GFillEdgePVS(EF, myEmptyShapeList, G, PVS);
    GLOBAL_issp = 0; //++

    // Create an edge builder EDBU
    TopOpeBRepBuild_PaveClassifier VCL(EF);
    bool                           equalpar = PVS.HasEqualParameters();
    if (equalpar)
      VCL.SetFirstParameter(PVS.EqualParameters());
    TopOpeBRepBuild_EdgeBuilder EDBU;
    EDBU.InitEdgeBuilder(PVS, VCL);

    // Build the new edges LEM
    NCollection_List<TopoDS_Shape> LEM;
    GEDBUMakeEdges(EF, EDBU, LEM);

    // connect new edges LEM as split parts (OUT,SOLID)
    MarkSplit(EOR, TopAbs_OUT);
    NCollection_List<TopoDS_Shape>& LINN = ChangeSplit(EOR, TopAbs_OUT);
    GCopyList(LEM, LINN);
  }

  GLOBAL_hassd = 0; //++
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& TopOpeBRepBuild_Builder::Section()
{
  if (mySectionDone)
    return mySection;
  mySectionDone = true;
  SectionCurves(mySection);
  SectionEdges(mySection);
  return mySection;
}

//=================================================================================================

void TopOpeBRepBuild_Builder::Section(NCollection_List<TopoDS_Shape>& L)
{
  L = Section();
}

//=================================================================================================

void TopOpeBRepBuild_Builder::SectionCurves(NCollection_List<TopoDS_Shape>& LSE)
{
  TopOpeBRepDS_CurveExplorer cex(myDataStructure->DS());
  for (; cex.More(); cex.Next())
  {
    int                                      ic = cex.Index();
    NCollection_List<TopoDS_Shape>::Iterator itloe(NewEdges(ic));
    for (; itloe.More(); itloe.Next())
    {
      LSE.Append(itloe.Value());
    }
  }
}

//=================================================================================================

void TopOpeBRepBuild_Builder::SectionEdges(NCollection_List<TopoDS_Shape>& LSE)
{
  const TopOpeBRepDS_DataStructure& BDS = myDataStructure->DS();
  int                               i, nes = BDS.NbSectionEdges();

  int iskpart = IsKPart();
  if (iskpart == 1)
  { // iskole
    for (i = 1; i <= nes; i++)
    {
      const TopoDS_Edge& E = TopoDS::Edge(BDS.SectionEdge(i));
      // modif fbi 16-09-97: case possible if RemoveSectionEdge was called
      if (E.IsNull())
        continue;
      // end modif fbi
      LSE.Append(E);
    }
    return;
  }

  SplitSectionEdges();

  // common of edges of section

  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> MOS;

  for (i = 1; i <= nes; i++)
  {

    const TopoDS_Shape& es = BDS.SectionEdge(i);
    // modif fbi 16-09-97: case possible if RemoveSectionEdge was called
    if (es.IsNull())
      continue;
    // end modif fbi
    bool         issplitIN = IsSplit(es, TopAbs_IN);
    bool         issplitON = IsSplit(es, TopAbs_ON);
    TopAbs_State staspl    = (issplitON) ? TopAbs_ON : (issplitIN) ? TopAbs_IN : TopAbs_UNKNOWN;

#ifdef OCCT_DEBUG
    int  iii;
    bool tSPS = GtraceSPS(es, iii);
    if (tSPS)
    {
      GdumpSHA(es, (char*)"--- Section ");
      std::cout << " splitIN " << issplitIN << " " << Splits(es, TopAbs_IN).Extent() << std::endl;
      std::cout << " splitON " << issplitON << " " << Splits(es, TopAbs_ON).Extent() << std::endl;
    }
#endif

    if (staspl != TopAbs_UNKNOWN)
    {
      for (NCollection_List<TopoDS_Shape>::Iterator it(Splits(es, staspl)); it.More(); it.Next())
      {
        const TopoDS_Shape& S = it.Value();
        if (!MOS.Contains(S))
        {
          MOS.Add(S);
          LSE.Append(S);
        }
      }
    }
    else
    {
      bool hasgeom = myDataStructure->HasGeometry(es);
      bool hassame = myDataStructure->HasSameDomain(es);
      bool take    = !(hasgeom || hassame);
      if (take)
      {
        if (!MOS.Contains(es))
        {
          MOS.Add(es);
          LSE.Append(es);
        }
      }
    }
  } // for i [1..nes]
} // SectionEdges

//=================================================================================================

void TopOpeBRepBuild_Builder::FillSecEdgeAncestorMap(
  const int                                                                 aShapeRank,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&             aMapON,
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& anAncMap) const
{
  const TopOpeBRepDS_DataStructure& BDS = myDataStructure->DS();

  int i, nse = BDS.NbSectionEdges();
  for (i = 1; i <= nse; i++)
  {
    const TopoDS_Shape& es = BDS.SectionEdge(i);
    if (es.IsNull() || ShapeRank(es) != aShapeRank)
      continue;
    if (aMapON.Contains(es))
    {
      anAncMap.Bind(es, es);
      continue;
    }
    TopAbs_State states[3] = {TopAbs_IN, TopAbs_ON, TopAbs_OUT};
    for (int j = 0; j < 3; j++)
    {
      //      bool isSplit = IsSplit(es,states[j]);
      if (IsSplit(es, states[j]))
      {
        NCollection_List<TopoDS_Shape>::Iterator it(Splits(es, states[j]));
        for (; it.More(); it.Next())
        {
          const TopoDS_Shape& aS = it.Value();
          if (aMapON.Contains(aS))
            anAncMap.Bind(aS, es);
        }
      }
    }
  }
}
