// Created on: 1998-11-25
// Created by: Prestataire Xuan PHAM PHU
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

#include <BndLib_Add2dCurve.hxx>
#include <BRep_Builder.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepTool.hxx>
#include <TopOpeBRepTool_2d.hxx>
#include <TopOpeBRepTool_CORRISO.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_State.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_List.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TopoDS_Edge.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopOpeBRepTool_GEOMETRY.hxx>
#include <TopOpeBRepTool_PROJECT.hxx>
#include <TopOpeBRepTool_TOPOLOGY.hxx>
#include <TopOpeBRepTool_PURGE.hxx>
#include <TopOpeBRepTool_TOOL.hxx>
#include <NCollection_Array1.hxx>

#ifdef OCCT_DEBUG
extern bool     TopOpeBRepTool_GettraceCORRISO();
Standard_EXPORT NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> STATIC_PURGE_mapv;
Standard_EXPORT NCollection_IndexedMap<TopoDS_Shape> STATIC_PURGE_mapeds;
extern void                                          FUN_tool_trace(const int Index);
extern void                                          FUN_tool_trace(const gp_Pnt2d p2d);
#endif

static void FUN_RaiseError()
{
#ifdef OCCT_DEBUG
  //  bool trc = TopOpeBRepTool_GettraceCORRISO();
  FUN_REINIT();
//  if (trc) std::cout <<"*********failure in CORRISO***********\n";
#endif
}

static void FUN_Raise()
{
#ifdef OCCT_DEBUG
//  std::cout <<"*********failure in CORRISO***********\n";
#endif
}

#ifdef DRAW
  #include <TopOpeBRepTool_DRAW.hxx>
#endif

#define M_FORWARD(sta) (sta == TopAbs_FORWARD)
#define M_REVERSED(sta) (sta == TopAbs_REVERSED)
#define M_INTERNAL(sta) (sta == TopAbs_INTERNAL)
#define M_EXTERNAL(sta) (sta == TopAbs_EXTERNAL)

//=================================================================================================

TopOpeBRepTool_CORRISO::TopOpeBRepTool_CORRISO() = default;

//=================================================================================================

TopOpeBRepTool_CORRISO::TopOpeBRepTool_CORRISO(const TopoDS_Face& Fref)
{
  myFref = Fref;

  FUN_tool_closedS(myFref, myUclosed, myUper, myVclosed, myVper);

  const occ::handle<Geom_Surface>& SU = BRep_Tool::Surface(myFref);
  myGAS                               = GeomAdaptor_Surface(SU);
}

//=================================================================================================

const TopoDS_Face& TopOpeBRepTool_CORRISO::Fref() const
{
  return myFref;
}

//=================================================================================================

const GeomAdaptor_Surface& TopOpeBRepTool_CORRISO::GASref() const
{
  return myGAS;
}

//=================================================================================================

bool TopOpeBRepTool_CORRISO::Refclosed(const int x, double& xperiod) const
{
  if (x == 1)
  {
    xperiod = myUper;
    return myUclosed;
  }
  if (x == 2)
  {
    xperiod = myVper;
    return myVclosed;
  }
  return false;
}

//=================================================================================================

bool TopOpeBRepTool_CORRISO::Init(const TopoDS_Shape& S)
{
#ifdef DRAW
  int  ie   = 0;
  bool trc  = TopOpeBRepTool_GettraceCORRISO();
  bool INIT = true;
  if (INIT)
    FUN_REINIT();
#endif

  myERep2d.Clear();
  myEds.Clear();
  myVEds.Clear();

  if (S.IsNull())
    return false;
  myS = S;

  TopExp_Explorer ex(S, TopAbs_EDGE);
  for (; ex.More(); ex.Next())
  {
    const TopoDS_Edge& E = TopoDS::Edge(ex.Current());
#ifdef OCCT_DEBUG
    int iE = STATIC_PURGE_mapeds.Add(E);
    (void)iE; // avoid warning
  #ifdef DRAW
    if (trc)
    {
      TCollection_AsciiString aa = TCollection_AsciiString("e");
      FUN_tool_draw(aa, E, iE);
    }
  #endif
#endif

    // myEds :
    myEds.Append(E);

    // myERep2d :
    //    double f,l,tol; occ::handle<Geom2d_Curve> PC = FC2D_CurveOnSurface(E,myFref,f,l,tol);
    occ::handle<Geom2d_Curve> PC;
    double                    f, l, tol;
    bool                      hasold = FC2D_HasOldCurveOnSurface(E, myFref, PC);
    PC                               = FC2D_EditableCurveOnSurface(E, myFref, f, l, tol);
    if (!hasold)
      FC2D_AddNewCurveOnSurface(PC, E, myFref, f, l, tol);
    if (PC.IsNull())
      return false;
    TopOpeBRepTool_C2DF C2DF(PC, f, l, tol, myFref);
    myERep2d.Bind(E, C2DF);

    // myVEds :
    TopExp_Explorer exv(E, TopAbs_VERTEX);
    for (; exv.More(); exv.Next())
    {
      const TopoDS_Vertex& v = TopoDS::Vertex(exv.Current());
#ifdef OCCT_DEBUG
      int aniE = STATIC_PURGE_mapeds.Add(E);
      (void)aniE; // avoid warning
  #ifdef DRAW
      if (trc)
      {
        TCollection_AsciiString bb = TCollection_AsciiString("v");
        FUN_tool_draw(bb, v, iv);
      }
  #endif
#endif
      bool isb = myVEds.IsBound(v);
      if (isb)
        myVEds.ChangeFind(v).Append(E);
      else
      {
        NCollection_List<TopoDS_Shape> loe;
        loe.Append(E);
        myVEds.Bind(v, loe);
      }
    } // exv
  }
  return true;
}

//=================================================================================================

const TopoDS_Shape& TopOpeBRepTool_CORRISO::S() const
{
  return myS;
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& TopOpeBRepTool_CORRISO::Eds() const
{
  return myEds;
}

//=================================================================================================

bool TopOpeBRepTool_CORRISO::UVRep(const TopoDS_Edge& E, TopOpeBRepTool_C2DF& C2DF) const
{
  bool isb = myERep2d.IsBound(E);
  if (!isb)
    return false;

  C2DF = myERep2d.Find(E);
  return true;
}

//=================================================================================================

bool TopOpeBRepTool_CORRISO::SetUVRep(const TopoDS_Edge& E, const TopOpeBRepTool_C2DF& C2DF)
{
  bool isb = myERep2d.IsBound(E);
  if (!isb)
    return false;

  myERep2d.ChangeFind(E) = C2DF;
  return true;
}

//=================================================================================================

bool TopOpeBRepTool_CORRISO::Connexity(const TopoDS_Vertex&            V,
                                       NCollection_List<TopoDS_Shape>& Eds) const
{
  bool isb = myVEds.IsBound(V);
  if (!isb)
    return false;

  Eds = myVEds.Find(V);
  return true;
}

//=================================================================================================

bool TopOpeBRepTool_CORRISO::SetConnexity(const TopoDS_Vertex&                  V,
                                          const NCollection_List<TopoDS_Shape>& Eds)
{
  bool isb = myVEds.IsBound(V);
  if (!isb)
    return false;

  myVEds.ChangeFind(V) = Eds;
  return true;
}

//=================================================================================================

bool TopOpeBRepTool_CORRISO::UVClosed() const
{
#ifdef OCCT_DEBUG
  bool trc = TopOpeBRepTool_GettraceCORRISO();
  if (trc)
    std::cout << "** UVClosed" << std::endl;
#endif
  NCollection_DataMap<TopoDS_Shape, int> lfyE;
  int                                    nfybounds   = 3;
  bool                                   stopatfirst = true;
  bool foundfaulty = EdgesWithFaultyUV(myEds, nfybounds, lfyE, stopatfirst);
  return !foundfaulty;
}

//=================================================================================================

double TopOpeBRepTool_CORRISO::Tol(const int I, const double tol3d) const
{
  double tol = (I == 1) ? myGAS.UResolution(tol3d) : myGAS.VResolution(tol3d);
  return tol;
}

// static double FUN_getx(const TopoDS_Edge& E,
static double FUN_getx(const TopoDS_Edge&,
                       const TopOpeBRepTool_C2DF& c2df,
                       const bool                 uiso,
                       const double               par)
{ // prequesitory : E is uviso
  gp_Pnt2d uv = TopOpeBRepTool_TOOL::UVF(par, c2df);
  double   x  = (uiso) ? uv.Y() : uv.X();
  return x;
}

//=================================================================================================

bool TopOpeBRepTool_CORRISO::PurgeFyClosingE(const NCollection_List<TopoDS_Shape>& ClEds,
                                             NCollection_List<TopoDS_Shape>&       fyClEds) const
{
  fyClEds.Clear();
#ifdef OCCT_DEBUG
  bool trc = TopOpeBRepTool_GettraceCORRISO();
  if (trc)
    std::cout << "* PurgeFyClosingE" << std::endl;
#endif
  //  double xperiod = myUclosed ? myUper : myVper;
  double tttol  = 1.e-8;
  double tttolS = BRep_Tool::Tolerance(myFref);
  double tolu = Tol(1, tttolS), tolv = Tol(2, tttolS);
  double tttuvF = std::max(tolu, tolv);

  NCollection_IndexedMap<TopoDS_Shape>     mapcl;
  NCollection_List<TopoDS_Shape>::Iterator itce(ClEds);
  for (; itce.More(); itce.Next())
    mapcl.Add(itce.Value());

  //* one closing edge should be removed
  itce.Initialize(ClEds);
  NCollection_DataMap<TopoDS_Shape, int> fyceds;
  bool                                   found = EdgesWithFaultyUV(ClEds, 3, fyceds);
  if (!found)
    return false;

  if (fyceds.Extent() == 1)
  { // ivf == 3 : cto016G*
    NCollection_DataMap<TopoDS_Shape, int> fyeds;

    EdgesWithFaultyUV(myEds, 3, fyeds);
    int nfy = fyeds.Extent();

    NCollection_DataMap<TopoDS_Shape, int>::Iterator itm(fyceds);
    const TopoDS_Edge&                               cE = TopoDS::Edge(itm.Key());

    TopAbs_Orientation OocE   = TopAbs::Complement(cE.Orientation());
    bool               isoncE = mapcl.Contains(cE.Oriented(OocE));
    if (isoncE)
    {
      NCollection_Array1<TopoDS_Shape> vcE(1, 2);
      TopOpeBRepTool_TOOL::Vertices(cE, vcE);
      TopAbs_Orientation  ocE     = cE.Orientation();
      double              tttolcE = BRep_Tool::Tolerance(cE);
      double              tttuvcE = std::max(Tol(1, tttolcE), Tol(2, tttolcE));
      TopOpeBRepTool_C2DF cE2d;
      bool                isb = UVRep(cE, cE2d);
      if (!isb)
        return false; // NYIRAISE

      // isonOcE2d :
      // OcE (closing edge with complemented orientation):
      TopAbs_Orientation oOcE        = TopAbs::Complement(ocE);
      TopoDS_Shape       alocalShape = cE.Oriented(oOcE);
      TopoDS_Edge        OcE         = TopoDS::Edge(alocalShape);
      //      TopoDS_Edge OcE = TopoDS::Edge(cE.Oriented(oOcE));
      NCollection_Array1<TopoDS_Shape> vOcE(1, 2);
      TopOpeBRepTool_TOOL::Vertices(OcE, vOcE);
      double              tttolOcE = BRep_Tool::Tolerance(OcE);
      double              tttuvOcE = std::max(Tol(1, tttolOcE), Tol(2, tttolOcE));
      TopOpeBRepTool_C2DF OcE2d;
      bool                isOb = UVRep(OcE, OcE2d);
      if (!isOb)
        return false; // NYIRAISE

      double   parvce1 = TopOpeBRepTool_TOOL::ParE(1, cE);
      gp_Pnt2d UVvce1  = TopOpeBRepTool_TOOL::UVF(parvce1, cE2d);

      double   parvOcE2 = TopOpeBRepTool_TOOL::ParE(2, OcE);
      gp_Pnt2d UVvOcE2  = TopOpeBRepTool_TOOL::UVF(parvOcE2, OcE2d);
      double   tol      = std::max(tttuvcE, tttuvOcE);
      isoncE            = (UVvce1.Distance(UVvOcE2) < tol);
      if (isoncE && (nfy != 1))
      { // cto009L2
        return false;
      }
    }

    int ivf = itm.Value();
    if (ivf == 3)
    {
      fyClEds.Append(cE);
      return true;
    }
  }
  else if (fyceds.Extent() > 1)
  { // ivf == 1,2 : cto016E*
    // if {edges of fyceds} describe a closing edge with its first and last
    // uvbounds non connexed -> we do remove these edges
    bool     hasinit = false;
    bool     isou = false, isov = false;
    gp_Pnt2d o2d;
    gp_Dir2d d2d;
    double   xinf = 1.e7, xsup = -1.e7; // faulty inf and sup bounds
    bool     infdef = false, supdef = false;
    NCollection_DataMap<TopoDS_Shape, int>::Iterator itm(fyceds);
    for (; itm.More(); itm.Next())
    {
      const TopoDS_Edge&  cE = TopoDS::Edge(itm.Key());
      TopOpeBRepTool_C2DF c2df;
      bool                isb = UVRep(cE, c2df);
      if (!isb)
        return false; // NYIRAISE

      int      ivf = itm.Value();
      bool     isoux, isovx;
      gp_Pnt2d o2dx;
      gp_Dir2d d2dx;
      bool     uvisox = TopOpeBRepTool_TOOL::UVISO(c2df, isoux, isovx, d2dx, o2dx);
      if (!uvisox)
        return false;

      if (hasinit)
      {
        bool onsamline = (isou && isoux) || (isov && isovx);
        if (!onsamline)
          return false;
      }
      if (!hasinit)
      {
        isou    = isoux;
        isov    = isovx;
        o2d     = o2dx;
        d2d     = d2dx;
        hasinit = true;
      }
      else
      {
        bool onsamline = false;
        if (isou && isoux)
        {
          double du = o2d.X() - o2dx.X();
          onsamline = (std::abs(du) < tolu);
        }
        if (isov && isovx)
        {
          double dv = o2d.Y() - o2dx.Y();
          onsamline = (std::abs(dv) < tolv);
        }
        if (!onsamline)
          return false;
      }
      for (int i = 1; i <= 2; i++)
      {
        double pari     = TopOpeBRepTool_TOOL::ParE(i, cE);
        double xi       = FUN_getx(cE, c2df, isou, pari);
        bool   vifaulty = (ivf == i || ivf == 3);
        bool   inff     = (xi < xinf);
        bool   supl     = (xi > xsup);
        //	if (inff) xinf = (ivf == i || ivf == 3) ? xi : 1.e7;
        //	if (supl) xsup = (ivf == i || ivf == 3) ? xi : -1.e7;
        if (inff)
        {
          xinf   = xi;
          infdef = vifaulty;
        }
        if (supl)
        {
          xsup   = xi;
          supdef = vifaulty;
        }
      }
      fyClEds.Append(cE);
    } // itm
    bool toremove = infdef && supdef; // ie infx,supx are not "uv-connexed"
    if (!toremove)
      fyClEds.Clear();
  }
  if (!fyClEds.IsEmpty())
    return true; // keeping only one closing edge

  //* the 2 closing edges have they 2drep "confunded"
  itce.Initialize(ClEds);
  for (; itce.More(); itce.Next())
  {
    // cE :
    const TopoDS_Edge&               cE = TopoDS::Edge(itce.Value());
    NCollection_Array1<TopoDS_Shape> vcE(1, 2);
    TopOpeBRepTool_TOOL::Vertices(cE, vcE);
    TopAbs_Orientation ocE = cE.Orientation();

    double              tttolcE = BRep_Tool::Tolerance(cE);
    double              tttuvcE = std::max(Tol(1, tttolcE), Tol(2, tttolcE));
    TopOpeBRepTool_C2DF cE2d;
    bool                isb = UVRep(cE, cE2d);
    if (!isb)
      return false; // NYIRAISE
#ifdef OCCT_DEBUG
    int icE = STATIC_PURGE_mapeds.Add(cE);
    if (trc)
      std::cout << "? e" << icE << " :" << std::endl;
#endif

    // isonOcE2d :
    bool isonOcE2d = false;
    {
      // OcE (closing edge with complemented orientation):
      TopAbs_Orientation oOcE        = TopAbs::Complement(ocE);
      TopoDS_Shape       aLocalShape = cE.Oriented(oOcE);
      TopoDS_Edge        OcE         = TopoDS::Edge(aLocalShape);
      //      TopoDS_Edge OcE = TopoDS::Edge(cE.Oriented(oOcE));
      NCollection_Array1<TopoDS_Shape> vOcE(1, 2);
      TopOpeBRepTool_TOOL::Vertices(OcE, vOcE);
      bool hasOcE = mapcl.Contains(OcE);
      if (!hasOcE)
        continue; // closing edge appears twice
      double              tttolOcE = BRep_Tool::Tolerance(OcE);
      double              tttuvOcE = std::max(Tol(1, tttolOcE), Tol(2, tttolOcE));
      TopOpeBRepTool_C2DF OcE2d;
      bool                isOb = UVRep(OcE, OcE2d);
      if (!isOb)
        return false; // NYIRAISE

      double   parvce1 = TopOpeBRepTool_TOOL::ParE(1, cE);
      gp_Pnt2d UVvce1  = TopOpeBRepTool_TOOL::UVF(parvce1, cE2d);

      double   parvOcE2 = TopOpeBRepTool_TOOL::ParE(2, OcE);
      gp_Pnt2d UVvOcE2  = TopOpeBRepTool_TOOL::UVF(parvOcE2, OcE2d);
      double   tol      = std::max(tttuvcE, tttuvOcE);
      isonOcE2d         = (UVvce1.Distance(UVvOcE2) < tol);
    }
    if (!isonOcE2d)
    {
#ifdef OCCT_DEBUG
      if (trc)
        std::cout << "-> valid edge" << std::endl;
#endif
      continue;
    }

    int nvcEok = 0;
    for (int ivce = 1; ivce <= 2; ivce++)
    {
      // <vce> (boundary of <cE>):
      const TopoDS_Vertex&           vce = TopoDS::Vertex(vcE(ivce));
      NCollection_List<TopoDS_Shape> loe;
      isb = Connexity(vce, loe);

      if (!isb)
        return false; // NYIRAISE

      double   parvce = TopOpeBRepTool_TOOL::ParE(ivce, cE);
      gp_Pnt2d UVvce  = TopOpeBRepTool_TOOL::UVF(parvce, cE2d);
#ifdef OCCT_DEBUG
      // recall in one wire, there are 2 vertices for one non-degenerated closing edge
      int ivmapv = STATIC_PURGE_mapv.Add(vce);
      if (trc)
      {
        std::cout << " connexity for v(" << ivce << ")=v" << ivmapv;
        FUN_tool_trace(UVvce);
      }
  #ifdef DRAW
      if (trc)
      {
        TCollection_AsciiString bb("uv_");
        bb += TCollection_AsciiString(ivmapv);
        FUN_tool_draw(bb, UVvce);
      }
  #endif
#endif
      double tttolvce = BRep_Tool::Tolerance(vce);
      double tttuvvce = std::max(Tol(1, tttolvce), Tol(2, tttolvce));

      bool vceok = false;
      for (NCollection_List<TopoDS_Shape>::Iterator ite(loe); ite.More(); ite.Next())
      {
        const TopoDS_Edge& E = TopoDS::Edge(ite.Value());

#ifdef OCCT_DEBUG
        int iE = STATIC_PURGE_mapeds.Add(E);
        if (trc)
        {
          std::cout << "    : on e" << iE << std::endl;
        }
#endif
        //	if (E.IsSame(cE)) continue;
        if (mapcl.Contains(E))
          continue; // do NOT check connexity on closing edges
                    // xpu090399 cto016E1

        TopOpeBRepTool_C2DF E2d;
        bool                isB2 = UVRep(E, E2d);
        if (!isB2)
          return false; // NYIRAISE

        double tttolE = BRep_Tool::Tolerance(E);
        double tttuvE = std::max(Tol(1, tttolE), Tol(2, tttolE));

        NCollection_Array1<TopoDS_Shape> vE(1, 2);
        TopOpeBRepTool_TOOL::Vertices(E, vE);
        for (int ive = 1; ive <= 2; ive++)
        {

          const TopoDS_Vertex& ve    = TopoDS::Vertex(vE(ive));
          bool                 samev = ve.IsSame(vce);
          if (!samev)
            continue;
          double   parve = TopOpeBRepTool_TOOL::ParE(ive, E);
          gp_Pnt2d UVve  = TopOpeBRepTool_TOOL::UVF(parve, E2d);
#ifdef OCCT_DEBUG
          if (trc)
          {
            std::cout << "    ve(" << ive << ")";
            FUN_tool_trace(UVve);
          }
#endif
          if (ive == ivce)
            continue; // vertex FORWARD connexed to REVERSED one
          double tttolve = BRep_Tool::Tolerance(ve);
          double tttuvve = std::max(Tol(1, tttolve), Tol(2, tttolve));

          tttol = std::max(
            tttol,
            std::max(tttuvF, std::max(tttuvE, std::max(tttuvcE, std::max(tttuvve, tttuvvce)))));
          //	  double dd = myUclosed ? (UVve.X()-UVvce.X()) : (UVve.Y()-UVvce.Y());
          //	  bool xok = (std::abs(dd)<tttol) ||
          //(std::abs(std::abs(dd)-xperiod)<tttol); 	  if (xok) {
          double dd     = UVve.Distance(UVvce);
          bool   sameuv = (dd < tttol);
          if (myUclosed)
          {
            double xperiod = myUper;
            dd             = (UVve.X() - UVvce.X());
            sameuv         = sameuv || (std::abs(std::abs(dd) - xperiod) < tttol);
          }
          if (myVclosed)
          {
            double xperiod = myVper;
            dd             = (UVve.Y() - UVvce.Y());
            sameuv         = sameuv || (std::abs(std::abs(dd) - xperiod) < tttol);
          }
          if (sameuv)
          {
            vceok = true;
#ifdef OCCT_DEBUG
            if (trc)
            {
              std::cout << " connexity->ok" << std::endl;
            }
#endif
          }
          break;
        } // ive=1..2
        if (vceok)
          break;
      } // ite(loe)

#ifdef OCCT_DEBUG
      if (trc && !vceok)
      {
        std::cout << " connexity->KO" << std::endl;
      }
#endif
      if (vceok)
        nvcEok++;
    } // ivce=1..2

    bool isfycE = (nvcEok == 0); // each bound is not connexed to any non-closed edge

#ifdef OCCT_DEBUG
    if (trc)
    {
      if (isfycE)
        std::cout << "-> faulty edge" << std::endl;
      else
        std::cout << "-> valid edge" << std::endl;
    }
#endif
    if (isfycE)
      fyClEds.Append(cE);
  } // itce
  return (!fyClEds.IsEmpty());
}

#define SPLITEDGE (0)
#define INCREASE (1)
#define DECREASE (-1)

static int FUN_tool_recadre(const double minx,
                            const double maxx,
                            const double xfirst,
                            const double xlast,
                            const double tolx,
                            bool&        maxsup)
{
  int  recadre = 10;                                              // INIT
  bool maxinf  = (maxx < xfirst + tolx);                          // permissive
  bool mininf  = (minx < xfirst - tolx);                          //
  maxsup       = (maxx > xlast + tolx);                           //
  bool minsup  = (minx > xlast - tolx);                           // permissive
  bool maxok   = (xfirst - tolx < maxx) && (maxx < xlast + tolx); // permissive
  bool minok   = (xfirst - tolx < minx) && (minx < xlast + tolx); // permissive

  if (maxinf)
    recadre = INCREASE;
  else if (minsup)
    recadre = DECREASE;
  else if (mininf && maxok)
    recadre = SPLITEDGE;
  else if (minok && maxsup)
    recadre = SPLITEDGE;
  return recadre;
}

//=================================================================================================

int TopOpeBRepTool_CORRISO::EdgeOUTofBoundsUV(const TopoDS_Edge& E,
                                              const bool         onU,
                                              const double       tolx,
                                              double&            parspE) const
{
  int recadre = 10;
  parspE      = -1.e7; // INIT
  int isb     = myERep2d.IsBound(E);
  if (!isb)
    return false;

  const TopOpeBRepTool_C2DF&       C2DF = myERep2d.Find(E);
  double                           f, l, tol;
  const occ::handle<Geom2d_Curve>& PC = C2DF.PC(f, l, tol);

  double xfirst = onU ? myGAS.FirstUParameter() : myGAS.FirstVParameter();
  // clang-format off
  double xlast   = onU ? myGAS.LastUParameter() : myGAS.LastVParameter(); // xlast=xfirst+xperiod
  // clang-format on
  double xperiod = onU ? myUper : myVper;

  bool     isou, isov;
  gp_Pnt2d o2d;
  gp_Dir2d d2d;
  bool     iso = TopOpeBRepTool_TOOL::UVISO(PC, isou, isov, d2d, o2d);

  if (iso)
  { // 2drep(E,myFref) is an ISO
    // -------------------------
    bool inX = (onU && isou) || ((!onU) && isov);
    if (inX)
    {
      // faulty u-iso : upar out of ubound
      double xpar     = onU ? o2d.X() : o2d.Y();
      bool   toosmall = (xpar < xfirst - tolx);
      bool   tobig    = (xpar > xfirst + xperiod + tolx);

      if (toosmall)
        recadre = INCREASE;
      if (tobig)
        recadre = DECREASE;
      return recadre;
    } // inX
    bool inY = (onU && isov) || ((!onU) && isou); // inY = !inX
    if (inY)
    {
      // faulty u-iso : vpar describes (minv,maxv) out of vbounds
      // PC describes [minx,maxx] in x-space
      // recadre = INCREASE : maxx < 0.
      //           DECREASE : minx > 2PI
      //           SPLITEDGE : minx<0.<maxx || minx<2PI<maxx
      double   d2ddir  = onU ? d2d.Y() : d2d.X();
      bool     reverse = (d2ddir < 0.);
      double   xfactor = reverse ? -1. : 1.;
      double   max     = reverse ? f : l;
      double   min     = reverse ? l : f;
      gp_Pnt2d maxuv   = PC->Value(max);
      gp_Pnt2d minuv   = PC->Value(min);

      double maxx = onU ? maxuv.X() : maxuv.Y();
      double minx = onU ? minuv.X() : minuv.Y();

      bool maxsup;
      recadre = FUN_tool_recadre(minx, maxx, xfirst, xlast, tolx, maxsup);
      if (recadre == SPLITEDGE)
      {
        double xbound = maxsup ? xperiod : 0.;
        parspE        = max - xfactor * (maxx - xbound);
      }
      return recadre;
    } // inY
  } // iso
  else
  { // 2drep(E, myFref) is NOT an iso
    // ------------------------------
    Bnd_Box2d           Bn2d;
    Geom2dAdaptor_Curve GC2d(PC, f, l);
    double              tolE   = BRep_Tool::Tolerance(E);
    double              toladd = std::max(tolE, tol);
    BndLib_Add2dCurve::Add(GC2d, toladd, Bn2d);
    double umin, vmin, umax, vmax;
    Bn2d.Get(umin, vmin, umax, vmax);
    double xmin = onU ? umin : vmin;
    double xmax = onU ? umax : vmax;
    bool   maxsup;
    recadre = FUN_tool_recadre(xmin, xmax, xfirst, xlast, tolx, maxsup);
    if (recadre == SPLITEDGE)
    {
      // ================================================================
      // NYIxpu271198 : intersection PC avec xiso (x= maxsup? xperiod: 0.)
      // ================================================================
      return 10;
    }
    return recadre;
  }
  return recadre;
}

//=================================================================================================

bool TopOpeBRepTool_CORRISO::EdgesOUTofBoundsUV(const NCollection_List<TopoDS_Shape>&   EdsToCheck,
                                                const bool                              onU,
                                                const double                            tolx,
                                                NCollection_DataMap<TopoDS_Shape, int>& FyEds) const
{
  FyEds.Clear();
  NCollection_List<TopoDS_Shape>::Iterator it(EdsToCheck);
  for (; it.More(); it.Next())
  {
    const TopoDS_Edge& E       = TopoDS::Edge(it.Value());
    double             sspar   = -1.e7;
    int                recadre = EdgeOUTofBoundsUV(E, onU, tolx, sspar);
    if (recadre == SPLITEDGE)
      FUN_Raise();
    if (recadre == INCREASE)
      FyEds.Bind(E, 1);
    if (recadre == DECREASE)
      FyEds.Bind(E, -1);
  }
  return (!FyEds.IsEmpty());
}

//=================================================================================================

bool TopOpeBRepTool_CORRISO::EdgeWithFaultyUV(const TopoDS_Edge& E, int& Ivfaulty) const
{
#ifdef OCCT_DEBUG
  bool trc = TopOpeBRepTool_GettraceCORRISO();
  int  iE  = STATIC_PURGE_mapeds.Add(E);
  if (trc)
    std::cout << "? e" << iE << " :" << std::endl;
#endif
  Ivfaulty      = 0;
  double tttol  = 1.e-8;
  double tttolF = BRep_Tool::Tolerance(TopoDS::Face(myFref));
  double tttuvF = std::max(Tol(1, tttolF), Tol(2, tttolF));
  double tttolE = BRep_Tool::Tolerance(E);
  double tttuvE = std::max(Tol(1, tttolE), Tol(2, tttolE));

  TopAbs_Orientation oE = E.Orientation();
  if (M_INTERNAL(oE) || M_EXTERNAL(oE))
    return false;

  NCollection_Array1<TopoDS_Shape> vEs(1, 2);
  TopOpeBRepTool_TOOL::Vertices(E, vEs);
  bool closed = vEs(1).IsSame(vEs(2));
  if (closed)
  {
#ifdef OCCT_DEBUG
    if (trc)
    {
      std::cout << "closed -> valid edge" << std::endl;
    }
#endif
    return false; // closed edge is assumed valid
  }

  int nfyv = 0;
  for (int ivE = 1; ivE <= 2; ivE++)
  {

    // <vE> (boundary of <E>):
    const TopoDS_Vertex& vE    = TopoDS::Vertex(vEs(ivE));
    double               parvE = TopOpeBRepTool_TOOL::ParE(ivE, E);
    TopOpeBRepTool_C2DF  C2DF;
    bool                 isb = UVRep(E, C2DF);
    if (!isb)
      return false; // NYIRAISE
    gp_Pnt2d UVvE = TopOpeBRepTool_TOOL::UVF(parvE, C2DF);
#ifdef OCCT_DEBUG
    // recall in one wire, there are 2 vertices for one non-degenerated closing edge
    int ivmapv = STATIC_PURGE_mapv.Add(vE);
    if (trc)
    {
      std::cout << " connexity for v(" << ivE << ")=v" << ivmapv;
      FUN_tool_trace(UVvE);
    }
  #ifdef DRAW
    if (trc)
    {
      TCollection_AsciiString bb("uv_");
      bb += TCollection_AsciiString(ivmapv);
      FUN_tool_draw(bb, UVvE);
    }
  #endif
#endif

    double tttolvE = BRep_Tool::Tolerance(vE);
    double tttuvvE = std::max(Tol(1, tttolvE), Tol(2, tttolvE));

    bool isbound = myVEds.IsBound(vE);
    if (!isbound)
    {
      FUN_RaiseError();
      return false;
    }

    // <vEok> :
    bool                                  vEok = false;
    const NCollection_List<TopoDS_Shape>& loe  = myVEds.Find(vE);

    for (NCollection_List<TopoDS_Shape>::Iterator ite(loe); ite.More(); ite.Next())
    {
      const TopoDS_Edge& e  = TopoDS::Edge(ite.Value());
      TopAbs_Orientation oe = e.Orientation();

#ifdef OCCT_DEBUG
      int ie = STATIC_PURGE_mapeds.Add(e);
      if (trc)
      {
        std::cout << "    : on e" << ie << std::endl;
      }
#endif

      if (e.IsSame(E))
        continue;
      if (M_INTERNAL(oe) || M_EXTERNAL(oe))
        continue;

      bool isBound = myERep2d.IsBound(e);
      if (!isBound)
      {
        FUN_RaiseError();
        return false;
      }
      const TopOpeBRepTool_C2DF& aC2DF = myERep2d.Find(e);

      NCollection_Array1<TopoDS_Shape> ves(1, 2);
      TopOpeBRepTool_TOOL::Vertices(e, ves);
      for (int ive = 1; ive <= 2; ive++)
      {
        const TopoDS_Vertex& ve    = TopoDS::Vertex(ves(ive));
        bool                 samev = ve.IsSame(vE);
        if (!samev)
          continue;

        double   pare = TopOpeBRepTool_TOOL::ParE(ive, e);
        gp_Pnt2d UVve = TopOpeBRepTool_TOOL::UVF(pare, aC2DF);
#ifdef OCCT_DEBUG
        if (trc)
        {
          std::cout << "    ve(" << ive << ")";
          FUN_tool_trace(UVve);
        }
#endif
        if (ive == ivE)
          continue;

        double tttolve = BRep_Tool::Tolerance(ve);
        double tttuvve = std::max(Tol(1, tttolve), Tol(2, tttolve));

        tttol = std::max(
          tttol,
          std::max(tttuvF, std::max(tttuvE, std::max(tttuvE, std::max(tttuvve, tttuvvE)))));
        bool isequal = UVvE.IsEqual(UVve, tttol);
        if (isequal)
        {
          vEok = true;
#ifdef OCCT_DEBUG
          if (trc)
          {
            std::cout << " connexity->ok" << std::endl;
          }
#endif
          break;
        }
      } // ive
      if (vEok)
        break;
    } // ite(loe)

    if (!vEok)
    {
      nfyv++;
      Ivfaulty = ivE;
    }
#ifdef OCCT_DEBUG
    if (trc && !vEok)
    {
      std::cout << " connexity->KO" << std::endl;
    }
#endif

  } // ivE = 1..2
  if (nfyv == 2)
    Ivfaulty = 3;
#ifdef OCCT_DEBUG
  if (trc)
  {
    if (Ivfaulty == 0)
      std::cout << "-> valid edge" << std::endl;
    else
      std::cout << "-> faulty edge" << std::endl;
  }
#endif
  return (Ivfaulty != 0);
}

//=================================================================================================

bool TopOpeBRepTool_CORRISO::EdgesWithFaultyUV(const NCollection_List<TopoDS_Shape>&   EdsToCheck,
                                               const int                               nfybounds,
                                               NCollection_DataMap<TopoDS_Shape, int>& FyEds,
                                               const bool stopatfirst) const
{
  FyEds.Clear();
#ifdef OCCT_DEBUG
  int  ifault = 0;
  bool trc    = TopOpeBRepTool_GettraceCORRISO();
  if (trc)
    std::cout << std::endl << "* EdgesWithFaultyUV" << std::endl;
#endif

  // fF's checking :
  // -----------------
  // An edge is valid if the first and last vertices are valid:
  // vertex <vEchk> is valid if there is an edge with bound <ve> such that :
  //   <vEchk> and <ve> share same UV geometry
  //   <vEchk> and <ve> are of opposite orientation.
  NCollection_List<TopoDS_Shape>::Iterator itchk(EdsToCheck);
  for (; itchk.More(); itchk.Next())
  {

    const TopoDS_Edge& Echk     = TopoDS::Edge(itchk.Value());
    int                Ivfaulty = 0;
    bool               faulty   = EdgeWithFaultyUV(Echk, Ivfaulty);
    if (!faulty)
      continue;
    int nfyv = (Ivfaulty == 3) ? 2 : 1;

#ifdef OCCT_DEBUG
    ifault++;
    if (trc)
      std::cout << "e" << STATIC_PURGE_mapeds.FindIndex(Echk) << " has ifyv=" << Ivfaulty
                << std::endl;
  #ifdef DRAW
    if (trc)
    {
      TCollection_AsciiString aa("fault");
      FUN_tool_draw(aa, Echk, ifault);
    }
  #endif
#endif

    bool found = false;
    if (nfybounds == 1)
      found = (nfyv == nfybounds);
    else if (nfybounds == 2)
      found = (nfyv == nfybounds);
    else if (nfybounds == 3)
      found = (nfyv > 0);

    if (found)
    {
      FyEds.Bind(Echk, Ivfaulty);
      if (stopatfirst)
        return true;
    }
  } // itchk
  int n = FyEds.Extent(); // DEB
  return (n != 0);
}

//=================================================================================================

bool TopOpeBRepTool_CORRISO::EdgeWithFaultyUV(const NCollection_List<TopoDS_Shape>& EdsToCheck,
                                              const int                             nfybounds,
                                              TopoDS_Shape&                         fyE,
                                              int&                                  Ifaulty) const
{
  NCollection_DataMap<TopoDS_Shape, int> FyEds;
  bool found = EdgesWithFaultyUV(EdsToCheck, nfybounds, FyEds, true);
  if (!found)
    return false;

  NCollection_DataMap<TopoDS_Shape, int>::Iterator itm(FyEds);
  fyE     = itm.Key();
  Ifaulty = itm.Value();
  return true;
}

//=================================================================================================

bool TopOpeBRepTool_CORRISO::TrslUV(const bool                                    onU,
                                    const NCollection_DataMap<TopoDS_Shape, int>& FyEds)
{
  gp_Vec2d tt2d;
  if (onU)
  {
    double uper;
    Refclosed(1, uper);
    if (!uper)
      return false;
    tt2d = gp_Vec2d(uper, 0.);
  }
  else
  {
    double vper;
    Refclosed(2, vper);
    if (!vper)
      return false;
    tt2d = gp_Vec2d(0., vper);
  }
  NCollection_DataMap<TopoDS_Shape, int>::Iterator itm(FyEds);
  for (; itm.More(); itm.Next())
  {
    const TopoDS_Edge&  E = TopoDS::Edge(itm.Key());
    TopOpeBRepTool_C2DF C2DF;
    bool                isb = UVRep(E, C2DF);
    if (!isb)
      return false;

    int itt = itm.Value();
    if (itt == SPLITEDGE)
      return false;
    else if (itt == INCREASE)
      TopOpeBRepTool_TOOL::TrslUV(tt2d, C2DF);
    else if (itt == DECREASE)
      TopOpeBRepTool_TOOL::TrslUV(tt2d.Multiplied(-1.), C2DF);
    else
      return false;
    SetUVRep(E, C2DF);
  }
  return true;
}

// modif in BRep_Builder031298
/*static void FUN_tool_correctdgE(const TopoDS_Edge& E)
{
  const occ::handle<BRep_TEdge>& TE = *((occ::handle<BRep_TEdge>*) &E.TShape());
  NCollection_List<occ::handle<BRep_CurveRepresentation>>& lcr = TE->ChangeCurves();
  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr(lcr);
  while (itcr.More()) {
    occ::handle<BRep_GCurve> GC = occ::down_cast<BRep_GCurve>(itcr.Value());
    if (!GC.IsNull()) {
      if (GC->IsCurve3D()) lcr.Remove(itcr);
      else itcr.Next();
    }
  }
}*/

//=================================================================================================

bool TopOpeBRepTool_CORRISO::GetnewS(TopoDS_Face& newS) const
{
  newS.Nullify();
  if (myS.ShapeType() != TopAbs_FACE)
    return false;

  newS = TopoDS::Face(myS);
  BRep_Builder BB;

  NCollection_List<TopoDS_Shape>::Iterator it(myEds);
  for (; it.More(); it.Next())
  {
    TopoDS_Edge         E    = TopoDS::Edge(it.Value());
    TopAbs_Orientation  oriE = E.Orientation();
    TopOpeBRepTool_C2DF C2DF;
    bool                isb = UVRep(E, C2DF);
    if (!isb)
      return false;

    double                           f, l, tol;
    const occ::handle<Geom2d_Curve>& PC = C2DF.PC(f, l, tol);
    occ::handle<Geom2d_TrimmedCurve> cu = new Geom2d_TrimmedCurve(PC, f, l);

    TopoDS_Shape aLocalShape = E.Oriented(TopAbs::Complement(oriE));
    TopoDS_Edge  Err         = TopoDS::Edge(aLocalShape);
    //    TopoDS_Edge Err = TopoDS::Edge(E.Oriented(TopAbs::Complement(oriE)));
    TopOpeBRepTool_C2DF C2DFrr;
    bool                isclo = UVRep(Err, C2DFrr);

    //    bool isdgE = BRep_Tool::Degenerated(E);
    // !BUC60380 : degenerated edge has a 3d curve !!, remove it
    //    if (isdgE) {FUN_tool_correctdgE(E);}

    if (isclo)
    {
      double                           frr, lrr, tolrr;
      const occ::handle<Geom2d_Curve>& PCrr = C2DFrr.PC(frr, lrr, tolrr);
      occ::handle<Geom2d_TrimmedCurve> curr = new Geom2d_TrimmedCurve(PCrr, frr, lrr);
      if (M_FORWARD(oriE))
        BB.UpdateEdge(E, cu, curr, newS, tol);
    }
    else
      BB.UpdateEdge(E, cu, newS, tol);
  }
  return true;
}

//=================================================================================================

// bool TopOpeBRepTool_CORRISO::AddNewConnexity(const TopoDS_Vertex& V,
bool TopOpeBRepTool_CORRISO::AddNewConnexity(const TopoDS_Vertex&, const TopoDS_Edge& E)
{
  // <myERep2d> :
  bool isb = myERep2d.IsBound(E);
  if (!isb)
  {
    occ::handle<Geom2d_Curve> PC;
    double                    f, l, tol;
    bool                      hasold = FC2D_HasOldCurveOnSurface(E, myFref, PC);
    PC                               = FC2D_EditableCurveOnSurface(E, myFref, f, l, tol);
    if (!hasold)
      FC2D_AddNewCurveOnSurface(PC, E, myFref, f, l, tol);
    if (PC.IsNull())
      return false;
    TopOpeBRepTool_C2DF C2DF(PC, f, l, tol, myFref);
    myERep2d.Bind(E, C2DF);
  }

  // <myEds> :
  if (!isb)
    myEds.Append(E);

  // <myVEds> :
  TopExp_Explorer exv(E, TopAbs_VERTEX);
  for (; exv.More(); exv.Next())
  {
    const TopoDS_Vertex& v    = TopoDS::Vertex(exv.Current());
    bool                 isbb = myVEds.IsBound(v);
    if (isbb)
      myVEds.ChangeFind(v).Append(E);
    else
    {
      NCollection_List<TopoDS_Shape> loe;
      loe.Append(E);
      myVEds.Bind(v, loe);
    }
  } // exv
  return true;
}

//=================================================================================================

// bool TopOpeBRepTool_CORRISO::RemoveOldConnexity(const TopoDS_Vertex& V,
bool TopOpeBRepTool_CORRISO::RemoveOldConnexity(const TopoDS_Vertex&, const TopoDS_Edge& E)
{
  // <myERep2d> :
  bool isb = myERep2d.IsBound(E);
  if (isb)
    myERep2d.UnBind(E);

  // <myEds> :
  if (isb)
  {
    NCollection_List<TopoDS_Shape>::Iterator it(myEds);
    while (it.More())
    {
      if (it.Value().IsEqual(E))
      {
        myEds.Remove(it);
        break;
      }
      else
        it.Next();
    }
  }

  // <myVEds> :
  bool            done = false;
  TopExp_Explorer exv(E, TopAbs_VERTEX);
  for (; exv.More(); exv.Next())
  {
    const TopoDS_Vertex& v        = TopoDS::Vertex(exv.Current());
    bool                 isBoundV = myVEds.IsBound(v);
    if (!isBoundV)
      return false;
    NCollection_List<TopoDS_Shape>&          loe = myVEds.ChangeFind(v);
    NCollection_List<TopoDS_Shape>::Iterator ite(loe);
    while (ite.More())
    {
      if (ite.Value().IsEqual(E))
      {
        done = true;
        loe.Remove(ite);
        break;
      }
      else
        ite.Next();
    }
  } // exv
  return done;
}
