// Created on: 1993-06-17
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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

#include <TopOpeBRepDS_BuildTool.hxx>

#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <ElCLib.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Conic.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <gp.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <Standard_ProgramError.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepDS_Dumper.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_Point.hxx>
#include <TopOpeBRepDS_Surface.hxx>
#include <TopOpeBRepDS_SurfaceCurveInterference.hxx>
#include <TopOpeBRepTool_GeomTool.hxx>
#include <TopOpeBRepTool_OutCurveType.hxx>
#include <TopOpeBRepTool_ShapeTool.hxx>

// includes especially needed by the static Project function
#ifdef DRAW
  #include <TopOpeBRepDS_DRAW.hxx>
#endif

Standard_EXPORT occ::handle<Geom2d_Curve> BASISCURVE2D(const occ::handle<Geom2d_Curve>& C);

bool FUN_UisoLineOnSphe(const TopoDS_Shape& F, const occ::handle<Geom2d_Curve>& PC)
{
  if (PC.IsNull())
    return false;

  occ::handle<Geom_Surface>  SSS      = TopOpeBRepTool_ShapeTool::BASISSURFACE(TopoDS::Face(F));
  occ::handle<Geom2d_Curve>  LLL      = ::BASISCURVE2D(PC);
  occ::handle<Standard_Type> TS       = SSS->DynamicType();
  occ::handle<Standard_Type> T2       = LLL->DynamicType();
  bool                       issphere = (TS == STANDARD_TYPE(Geom_SphericalSurface));
  bool                       isline2d = (T2 == STANDARD_TYPE(Geom2d_Line));
  bool                       isisoU   = false;
  if (issphere && isline2d)
  {
    occ::handle<Geom2d_Line> L = occ::down_cast<Geom2d_Line>(LLL);
    const gp_Dir2d&          d = L->Direction();
    isisoU                     = (std::abs(d.X()) < Precision::Parametric(Precision::Confusion()));
  }
  return isisoU;
}

//=================================================================================================

TopOpeBRepDS_BuildTool::TopOpeBRepDS_BuildTool()
    : myCurveTool(TopOpeBRepTool_APPROX),
      myOverWrite(true),
      myTranslate(true)
{
}

//=================================================================================================

TopOpeBRepDS_BuildTool::TopOpeBRepDS_BuildTool(const TopOpeBRepTool_OutCurveType O)
    : myCurveTool(O),
      myOverWrite(true),
      myTranslate(true)
{
}

//=================================================================================================

TopOpeBRepDS_BuildTool::TopOpeBRepDS_BuildTool(const TopOpeBRepTool_GeomTool& GT)
    : myCurveTool(GT),
      myOverWrite(true),
      myTranslate(true)
{
}

bool TopOpeBRepDS_BuildTool::OverWrite() const
{
  return myOverWrite;
}

void TopOpeBRepDS_BuildTool::OverWrite(const bool O)
{
  myOverWrite = O;
}

bool TopOpeBRepDS_BuildTool::Translate() const
{
  return myTranslate;
}

void TopOpeBRepDS_BuildTool::Translate(const bool T)
{
  myTranslate = T;
}

//=================================================================================================

const TopOpeBRepTool_GeomTool& TopOpeBRepDS_BuildTool::GetGeomTool() const
{
  const TopOpeBRepTool_GeomTool& GT = myCurveTool.GetGeomTool();
  return GT;
}

//=================================================================================================

TopOpeBRepTool_GeomTool& TopOpeBRepDS_BuildTool::ChangeGeomTool()
{
  TopOpeBRepTool_GeomTool& GT = myCurveTool.ChangeGeomTool();
  return GT;
}

//=================================================================================================

void TopOpeBRepDS_BuildTool::MakeVertex(TopoDS_Shape& V, const TopOpeBRepDS_Point& P) const
{
  myBuilder.MakeVertex(TopoDS::Vertex(V), P.Point(), P.Tolerance());
}

//=================================================================================================

void TopOpeBRepDS_BuildTool::MakeEdge(TopoDS_Shape& E, const TopOpeBRepDS_Curve& C) const
{
  // Gestion des courbes nulles pour carreaux pointus
  // RLE 28-6-94

  if (C.Curve().IsNull())
  {
    myBuilder.MakeEdge(TopoDS::Edge(E));
    myBuilder.Degenerated(TopoDS::Edge(E), true);
    return;
  }

  const occ::handle<Geom_Curve>& GC = C.Curve();
  myBuilder.MakeEdge(TopoDS::Edge(E), GC, C.Tolerance());

  bool addorigin = false;
  bool setrange  = false;

  if (addorigin)
  {
    if (GC->IsClosed())
    {
      // in case of a closed curve, insert in E a vertex located at the origin
      // of the curve C.
      TopoDS_Vertex V;
      double        first = GC->FirstParameter();
      gp_Pnt        P     = GC->Value(first);
      myBuilder.MakeVertex(V, P, C.Tolerance());
      myBuilder.Add(E, V);
      V.Reverse();
      myBuilder.Add(E, V);

      // If the curve is a degree 1 bspline set the range to 1 .. NbPoles
      occ::handle<Geom_BSplineCurve> BSC = occ::down_cast<Geom_BSplineCurve>(GC);
      if (!BSC.IsNull())
      {
        if (BSC->Degree() == 1)
        {
          myBuilder.Range(TopoDS::Edge(E), 1, BSC->NbPoles());
        }
      }
    }
  }

  if (setrange)
  {
    double first, last;
    bool   rangedef = C.Range(first, last);
    if (rangedef)
    {
      Range(E, first, last);
    }
  }
}

//=================================================================================================

void TopOpeBRepDS_BuildTool::MakeEdge(TopoDS_Shape&                     E,
                                      const TopOpeBRepDS_Curve&         C,
                                      const TopOpeBRepDS_DataStructure& BDS) const
{
  // Gestion des courbes nulles pour carreaux pointus
  // RLE 28-6-94

  TopoDS_Edge& EE = TopoDS::Edge(E);

  if (C.Curve().IsNull())
  {
    myBuilder.MakeEdge(EE);
    myBuilder.Degenerated(EE, true);

    // Creation d'une arete avec PCurve connectee a la BDS Curve
    // JYL 22-09-94
    const occ::handle<TopOpeBRepDS_Interference>&      I = C.GetSCI1();
    occ::handle<TopOpeBRepDS_SurfaceCurveInterference> SCI;
    SCI                                  = occ::down_cast<TopOpeBRepDS_SurfaceCurveInterference>(I);
    int                              iS  = SCI->Support();
    const TopOpeBRepDS_Surface&      DSS = BDS.Surface(iS);
    const occ::handle<Geom_Surface>& GS  = DSS.Surface();
    const occ::handle<Geom2d_Curve>& PC  = SCI->PCurve();
    myBuilder.UpdateEdge(EE, PC, GS, TopLoc_Location(), DSS.Tolerance());
    return;
  }
  else
  {
    const occ::handle<Geom_Curve>& GC = C.Curve();
    myBuilder.MakeEdge(EE, GC, C.Tolerance());
  }
}

//=================================================================================================

void TopOpeBRepDS_BuildTool::MakeEdge(TopoDS_Shape&                  E,
                                      const occ::handle<Geom_Curve>& C,
                                      const double                   Tol) const
{
  myBuilder.MakeEdge(TopoDS::Edge(E), C, Tol);
}

//=================================================================================================

void TopOpeBRepDS_BuildTool::MakeEdge(TopoDS_Shape& E) const
{
  myBuilder.MakeEdge(TopoDS::Edge(E));
}

//=================================================================================================

void TopOpeBRepDS_BuildTool::MakeWire(TopoDS_Shape& W) const
{
  myBuilder.MakeWire(TopoDS::Wire(W));
}

//=================================================================================================

void TopOpeBRepDS_BuildTool::MakeFace(TopoDS_Shape& F, const TopOpeBRepDS_Surface& S) const
{
  myBuilder.MakeFace(TopoDS::Face(F), S.Surface(), S.Tolerance());
}

//=================================================================================================

void TopOpeBRepDS_BuildTool::MakeShell(TopoDS_Shape& Sh) const
{
  myBuilder.MakeShell(TopoDS::Shell(Sh));
}

//=================================================================================================

void TopOpeBRepDS_BuildTool::MakeSolid(TopoDS_Shape& S) const
{
  myBuilder.MakeSolid(TopoDS::Solid(S));
}

//=================================================================================================

void TopOpeBRepDS_BuildTool::CopyEdge(const TopoDS_Shape& Ein, TopoDS_Shape& Eou) const
{

  // Splendide evolution de BRep_Curve3D::BRep_Curve3D(Geom_Curve,Location)
  // apres modification de la primitive Sphere pour parametrisation de
  // l'arete meridienne en -pi/2,+pi/2.
  // Ein est l'arete de couture complete d'une sphere complete
  // BRep_Tool::Range(Ein) --> -pi/2,+pi/2
  // BRep_Tool::Range(Ein.EmptyCopied()) --> 0,2pi
  // NYI reflexion sur la notion de Range d'une arete et de la geometrie
  // NYI sous jacente dans le cas ou, par construction, les vertex d'une
  // NYI arete on des valeurs de parametre HORS des bornes [first,last] de la
  // NYI courbe 3D support de l'arete (cas de l'arete de couture d'une sphere)
  // On redefinit desormais le range de l'arete Eou, a la place de se
  // contenter du simplissime Eou = Ein.EmptyCopied();
  // merci les amis : correction bug PRO2586

  double      f, l;
  TopoDS_Edge E1 = TopoDS::Edge(Ein);
  BRep_Tool::Range(E1, f, l);
  Eou            = Ein.EmptyCopied();
  TopoDS_Edge E2 = TopoDS::Edge(Eou);
  myBuilder.Range(E2, f, l);
}

//=================================================================================================

void TopOpeBRepDS_BuildTool::GetOrientedEdgeVertices(TopoDS_Edge&   E,
                                                     TopoDS_Vertex& Vmin,
                                                     TopoDS_Vertex& Vmax,
                                                     double&        Parmin,
                                                     double&        Parmax) const
{
  if (E.Orientation() == TopAbs_FORWARD)
    TopExp::Vertices(E, Vmin, Vmax);
  else
    TopExp::Vertices(E, Vmax, Vmin);
  if (!Vmin.IsNull() && !Vmax.IsNull())
  {
    Parmin = BRep_Tool::Parameter(Vmin, E);
    Parmax = BRep_Tool::Parameter(Vmax, E);
  }
}

//=================================================================================================

void TopOpeBRepDS_BuildTool::UpdateEdgeCurveTol
  //(const TopoDS_Face& F1,const TopoDS_Face& F2,
  (const TopoDS_Face&,
   const TopoDS_Face&,
   TopoDS_Edge&                   E,
   const occ::handle<Geom_Curve>& C3Dnew,
   // const double tol3d,
   const double,
   // const double tol2d1,
   const double,
   // const double tol2d2,
   const double,
   double& newtol,
   double& newparmin,
   double& newparmax) const

{
  if (C3Dnew.IsNull())
    return;
  BRep_Builder BB;

  // newtol = max des tolerances atteintes en 3d
  // JMB le 06 Juillet 1999
  // les valeurs tol3d et tol2d1,tol2d2 proviennent des approx. Dans la version 2.0 de CasCade,
  // elles n'etaient pas calculees et on renvoyait systematiquement les valeurs initiales (a savoir)
  // 1.E-7. Dans la version 2.1 de CasCade, ces valeurs sont desormais calculees selon un calcul
  // d'erreur dans les Approx. Malheureusement, il apparait que ce calcul d'erreur renvoit dans la
  // plupart des cas de tres grosses valeurs (parfois de l'ordre de 1.E-1). Ce qui amenait la
  // topologie a coder des tolerances enormes dans les pieces resultats rendant celles-ci
  // inexpoitables. De plus on essayait de rafiner la tolerance en appelant les UResolution sur les
  // surfaces support. sur des surfaces tres particulieres, ce UREsolution n'a plus aucun sens et
  // peut amener a des valeurs abberantes. On decide donc de laisser la tolerance de l'edge telle
  // qu'elle est afin d'avoir un comportement similaire a 2.0. Jusqu'a present on a constate que des
  // problemes avec la methode de calcul d'erreur des approx.

  newtol = 1.E-7;
  //  double r1,r2;
  //  r1 = TopOpeBRepTool_ShapeTool::Resolution3d(F1,tol2d1);
  //  r2 = TopOpeBRepTool_ShapeTool::Resolution3d(F2,tol2d2);
  //  newtol=tol3d;
  //  if (r1>newtol) newtol=r1;
  //  if (r2>newtol) newtol=r2;

  //  newtol *= 1.5;

  TopoDS_Vertex Vmin, Vmax;
  double        parmin = 0.0, parmax = 0.0;
  GetOrientedEdgeVertices(E, Vmin, Vmax, parmin, parmax);

  double tolmin = BRep_Tool::Tolerance(Vmin);
  if (newtol > tolmin)
    tolmin = newtol;
  double tolmax = BRep_Tool::Tolerance(Vmax);
  if (newtol > tolmax)
    tolmax = newtol;

  //  newparmin=C3Dnew->FirstParameter(); // -merge 04-07-97
  //  newparmax=C3Dnew->LastParameter(); // -merge 04-07-97

  // +merge 04-07-97
  occ::handle<Geom_TrimmedCurve> GTC = occ::down_cast<Geom_TrimmedCurve>(C3Dnew);
  if (GTC.IsNull())
  {
    occ::handle<Geom_BSplineCurve> GBSC = occ::down_cast<Geom_BSplineCurve>(C3Dnew);
    if (GBSC.IsNull())
    {
      newparmin = parmin;
      newparmax = parmax;
    }
    else
    {
      newparmin = C3Dnew->FirstParameter();
      newparmax = C3Dnew->LastParameter();
    }
  }
  else
  {
    newparmin = C3Dnew->FirstParameter();
    newparmax = C3Dnew->LastParameter();
  } // +merge 04-07-97

  if (Vmin.Orientation() == TopAbs_FORWARD)
  {
    BB.UpdateVertex(Vmin, newparmin, E, tolmin);
    BB.UpdateVertex(Vmax, newparmax, E, tolmax);
  }
  else
  {
    BB.UpdateVertex(Vmin, newparmax, E, tolmin);
    BB.UpdateVertex(Vmax, newparmin, E, tolmax);
  }

  //  DSBT.Curve3D(E,C3Dnew,newtol);  // -merge 04-07-97
  Curve3D(E, C3Dnew, newtol);

  // projection des vertex INTERNAL de E pour parametrage
  // sur la nouvelle courbe C3Dnew de l'arete E
  TopExp_Explorer exi(E, TopAbs_VERTEX);
  for (; exi.More(); exi.Next())
  {
    const TopoDS_Vertex& vi = TopoDS::Vertex(exi.Current());
    if (vi.Orientation() != TopAbs_INTERNAL)
      continue;
    gp_Pnt                      P     = BRep_Tool::Pnt(vi);
    double                      tolvi = TopOpeBRepTool_ShapeTool::Tolerance(vi);
    GeomAPI_ProjectPointOnCurve dm(P, C3Dnew, newparmin, newparmax);
    bool                        dmdone = dm.Extrema().IsDone();
    if (dmdone)
    {
      if (dm.NbPoints())
      {
        double newpar = dm.LowerDistanceParameter();
        BB.UpdateVertex(vi, newpar, E, tolvi);
      }
    }
  } // INTERNAL vertex
}

//=================================================================================================

void TopOpeBRepDS_BuildTool::ApproxCurves(const TopOpeBRepDS_Curve&                       C,
                                          TopoDS_Edge&                                    E,
                                          int&                                            inewC,
                                          const occ::handle<TopOpeBRepDS_HDataStructure>& HDS) const
{
  TopOpeBRepDS_Curve newC1;
  inewC                    = HDS->MakeCurve(C, newC1);
  TopOpeBRepDS_Curve& newC = HDS->ChangeCurve(inewC);

  // C1 curves have been approximated by BSplines of degree 1 :
  // compute new geometry on curves.

  const TopoDS_Face& F1 = TopoDS::Face(newC.Shape1());
  const TopoDS_Face& F2 = TopoDS::Face(newC.Shape2());

  const occ::handle<Geom_Curve>&   C3D = C.Curve();
  const occ::handle<Geom2d_Curve>& PC1 = C.Curve1();
  const occ::handle<Geom2d_Curve>& PC2 = C.Curve2();

  // Vmin,Vmax = bounding vertices of edge <E>
  // and their parameters parmin,parmax .

  TopoDS_Vertex Vmin, Vmax;
  double        parmin = 0.0, parmax = 0.0;
  GetOrientedEdgeVertices(E, Vmin, Vmax, parmin, parmax);

  occ::handle<Geom_Curve>   C3Dnew;
  occ::handle<Geom2d_Curve> PC1new;
  occ::handle<Geom2d_Curve> PC2new;
  double                    tolreached3d = 0.0, tolreached2d = 0.0;
  bool                      approxMade = myCurveTool.MakeCurves(parmin,
                                           parmax,
                                           C3D,
                                           PC1,
                                           PC2,
                                           F1,
                                           F2,
                                           C3Dnew,
                                           PC1new,
                                           PC2new,
                                           tolreached3d,
                                           tolreached2d);

  double newtol = 0.0, newparmin = 0.0, newparmax = 0.0;
  // MSV Nov 12, 2001: if approx failed than leave old curves of degree 1
  if (!approxMade)
  {
    newtol    = BRep_Tool::Tolerance(E);
    newparmin = parmin;
    newparmax = parmax;
    C3Dnew    = C3D;
    PC1new    = PC1;
    PC2new    = PC2;
  }
  else
  {
    UpdateEdgeCurveTol(F1,
                       F2,
                       E,
                       C3Dnew,
                       tolreached3d,
                       tolreached2d,
                       tolreached2d,
                       newtol,
                       newparmin,
                       newparmax);
  }

  if (!C3Dnew.IsNull())
  {
    newC.DefineCurve(C3Dnew, newtol, false);
    newC.SetRange(newparmin, newparmax);
  }

  if (!PC1new.IsNull())
    newC.Curve1(PC1new);
  if (!PC2new.IsNull())
    newC.Curve2(PC2new);
}

//=================================================================================================

bool FUN_getUV(const occ::handle<Geom_Surface>& surf,
               const occ::handle<Geom_Curve>&   C3D,
               const double                     par3d,
               double&                          u0,
               double&                          v0)
{
  gp_Pnt P3d;
  C3D->D0(par3d, P3d);
  GeomAPI_ProjectPointOnSurf pons(P3d, surf);
  if (pons.NbPoints() < 1)
    return false;
  pons.LowerDistanceParameters(u0, v0);
  return true;
}

bool FUN_reversePC(occ::handle<Geom2d_Curve> PCnew,
                   const TopoDS_Face&        F,
                   const gp_Pnt&             P3DC3D,
                   const double              par2d,
                   const double              tol)
{
  gp_Pnt2d P2D;
  PCnew->D0(par2d, P2D);
  BRepAdaptor_Surface BAS(F, false);
  gp_Pnt              P3D        = BAS.Value(P2D.X(), P2D.Y());
  bool                PCreversed = false;
  bool                sam        = P3D.IsEqual(P3DC3D, tol);
  PCreversed                     = !sam;

  if (PCreversed)
  {
    occ::handle<Geom2d_Curve> PC = ::BASISCURVE2D(PCnew);
    if (!PC.IsNull())
    {
      occ::handle<Geom2d_Line> L = occ::down_cast<Geom2d_Line>(PC);
      gp_Dir2d                 d = L->Direction();
      d.Reverse();
      L->SetDirection(d);
    }
  }
  return PCreversed;
}

bool FUN_makeUisoLineOnSphe(const TopoDS_Face&             F, // with geometry the spherical surface
                            const occ::handle<Geom_Curve>& C3D,
                            occ::handle<Geom2d_Curve>      PCnew,
                            const double                   tol3d)
{
  // p3df,p3dl : C3d first and last parameters
  double p3df = C3D->FirstParameter();
  double p3dl = C3D->LastParameter();

  // u0,v0 : C3d(par3d) UV parameters
  double                    deltainf = 0.243234, deltasup = 0.543345;
  double                    par3dinf = (1 - deltainf) * p3df + deltainf * p3dl;
  double                    par3dsup = (1 - deltasup) * p3df + deltasup * p3dl;
  double                    uinf, vinf, usup, vsup;
  occ::handle<Geom_Surface> surf = BRep_Tool::Surface(F);
  if (!FUN_getUV(surf, C3D, par3dinf, uinf, vinf))
    return false;
  if (!FUN_getUV(surf, C3D, par3dsup, usup, vsup))
    return false;
  double tol = Precision::Parametric(tol3d);
  if (std::abs(uinf - usup) > tol)
    return false;

  bool     isvgrowing = (vsup - vinf > -tol);
  gp_Dir2d vdir;
  if (isvgrowing)
    vdir = gp_Dir2d(gp_Dir2d::D::Y);
  else
    vdir = gp_Dir2d(gp_Dir2d::D::NY);

  gp_Pnt2d origin(uinf, vinf);
  origin.Translate(gp_Vec2d(vdir).Scaled(p3df - par3dinf));
  occ::handle<Geom2d_Curve> PC = ::BASISCURVE2D(PCnew);
  if (!PC.IsNull())
  {
    occ::handle<Geom2d_Line> L = occ::down_cast<Geom2d_Line>(PC);
    L->SetLin2d(gp_Lin2d(origin, vdir));
  } // (!PC.IsNull())

  return true;
}

void TopOpeBRepDS_BuildTool::ComputePCurves(const TopOpeBRepDS_Curve& C,
                                            TopoDS_Edge&              E,
                                            TopOpeBRepDS_Curve&       newC,
                                            const bool                comppc1,
                                            const bool                comppc2,
                                            const bool                compc3d) const
{
  const TopoDS_Face& F1 = TopoDS::Face(newC.Shape1());
  const TopoDS_Face& F2 = TopoDS::Face(newC.Shape2());

  const occ::handle<Geom_Curve>& C3D = C.Curve();

  // get bounding vertices Vmin,Vmax supported by the new edge <E>
  // and their corresponding parameters parmin,parmax .
  TopoDS_Vertex Vmin, Vmax;
  double        parmin = 0.0, parmax = 0.0;
  GetOrientedEdgeVertices(E, Vmin, Vmax, parmin, parmax);

  occ::handle<Geom2d_Curve> PC1new, PC2new;
  if (C3D.IsNull())
  {
    double tolreached2d1 = Precision::Confusion(), tolreached2d2 = Precision::Confusion(),
           tol = Precision::Confusion();
    if (comppc1)
      PC1new = myCurveTool.MakePCurveOnFace(F1, C3D, tolreached2d1);
    if (comppc2)
      PC2new = myCurveTool.MakePCurveOnFace(F2, C3D, tolreached2d2);

    double r1 = TopOpeBRepTool_ShapeTool::Resolution3d(F1, tolreached2d1);
    double r2 = TopOpeBRepTool_ShapeTool::Resolution3d(F2, tolreached2d2);
    tol       = std::max(tol, r1);
    tol       = std::max(tol, r2);
    newC.Tolerance(tol);

    if (!PC1new.IsNull())
      newC.Curve1(PC1new);
    if (!PC2new.IsNull())
      newC.Curve2(PC2new);

    return;
  }

  occ::handle<Geom_Curve> C3Dnew = C3D;

  if (C3D->IsPeriodic())
  {
    // ellipse on cone : periodize parmin,parmax
    double period = C3D->LastParameter() - C3D->FirstParameter();
    double f, l;
    if (Vmin.Orientation() == TopAbs_FORWARD)
    {
      f = parmin;
      l = parmax;
    }
    else
    {
      f = parmax;
      l = parmin;
    }
    parmin = f;
    parmax = l;
    ElCLib::AdjustPeriodic(f, f + period, Precision::PConfusion(), parmin, parmax);
    if (compc3d)
      C3Dnew = new Geom_TrimmedCurve(C3D, parmin, parmax);
  }

  double tolreached3d  = C.Tolerance();
  double tolreached2d1 = C.Tolerance();
  double tolreached2d2 = C.Tolerance();

  if (comppc1)
    PC1new = myCurveTool.MakePCurveOnFace(F1, C3Dnew, tolreached2d1);
  if (comppc2)
    PC2new = myCurveTool.MakePCurveOnFace(F2, C3Dnew, tolreached2d2);

  double newtol, newparmin, newparmax;
  UpdateEdgeCurveTol(F1,
                     F2,
                     E,
                     C3Dnew,
                     tolreached3d,
                     tolreached2d1,
                     tolreached2d2,
                     newtol,
                     newparmin,
                     newparmax);

  // xpu : suite merge : 07-07-97
  // xpu : 17-06-97
  // Rmq : C1.Curve<i>() ne sert plus qu'a determiner si la courbe
  //       est une isos de la sphere
  // NYI : enlever FUN_reversePC
  bool UisoLineOnSphe1 = false;
  UisoLineOnSphe1      = ::FUN_UisoLineOnSphe(F1, PC1new);
  if (UisoLineOnSphe1)
    ::FUN_makeUisoLineOnSphe(F1, C3Dnew, PC1new, newtol);

  bool UisoLineOnSphe2 = false;
  UisoLineOnSphe2      = ::FUN_UisoLineOnSphe(F2, PC2new);
  if (UisoLineOnSphe2)
    ::FUN_makeUisoLineOnSphe(F2, C3Dnew, PC2new, newtol);
  // xpu : 17-06-97
  // xpu : suite merge : 07-07-97

  if (!C3Dnew.IsNull())
  {
    newC.Curve(C3Dnew, newtol);
    newC.SetRange(newparmin, newparmax);
  }
  if (!PC1new.IsNull())
    newC.Curve1(PC1new);
  if (!PC2new.IsNull())
    newC.Curve2(PC2new);
}

//=================================================================================================

void TopOpeBRepDS_BuildTool::PutPCurves(const TopOpeBRepDS_Curve& newC,
                                        TopoDS_Edge&              E,
                                        const bool                comppc1,
                                        const bool                comppc2) const
{

  TopoDS_Face&                     F1  = *((TopoDS_Face*)(void*)&(TopoDS::Face(newC.Shape1())));
  const occ::handle<Geom2d_Curve>& PC1 = newC.Curve1();
  if (!PC1.IsNull() && comppc1)
  {
    PCurve(F1, E, PC1);
  }

  TopoDS_Face&                     F2  = *((TopoDS_Face*)(void*)&(TopoDS::Face(newC.Shape2())));
  const occ::handle<Geom2d_Curve>& PC2 = newC.Curve2();
  if (!PC2.IsNull() && comppc2)
  {
    PCurve(F2, E, PC2);
  }
}

//=================================================================================================

void TopOpeBRepDS_BuildTool::RecomputeCurves(
  const TopOpeBRepDS_Curve& C,
  // const TopoDS_Edge& oldE,
  const TopoDS_Edge&,
  TopoDS_Edge&                                    E,
  int&                                            inewC,
  const occ::handle<TopOpeBRepDS_HDataStructure>& HDS) const
{
  const TopOpeBRepTool_GeomTool& GT      = myCurveTool.GetGeomTool();
  const bool                     compc3d = GT.CompC3D();
  const bool                     comppc1 = GT.CompPC1();
  const bool                     comppc2 = GT.CompPC2();
  const bool                     comppc  = comppc1 || comppc2;
  const bool                     iswalk  = C.IsWalk();
  const bool                     approx  = Approximation();

  const occ::handle<Geom_Curve>& C3D = C.Curve();
  if (comppc1 && C.Shape1().IsNull())
    throw Standard_ProgramError("TopOpeBRepDS_BuildTool::RecomputeCurve 2");
  if (comppc2 && C.Shape2().IsNull())
    throw Standard_ProgramError("TopOpeBRepDS_BuildTool::RecomputeCurve 3");
  TopoDS_Vertex Vmin, Vmax;
  TopExp::Vertices(E, Vmin, Vmax);
  if (Vmin.IsNull())
    throw Standard_ProgramError("TopOpeBRepDS_BuildTool::RecomputeCurve 4");
  if (Vmax.IsNull())
    throw Standard_ProgramError("TopOpeBRepDS_BuildTool::RecomputeCurve 5");

  if (iswalk && approx)
  {
    if (compc3d && C3D.IsNull())
      throw Standard_ProgramError("TopOpeBRepDS_BuildTool::RecomputeCurve 1");
    ApproxCurves(C, E, inewC, HDS);
    TopOpeBRepDS_Curve& newC = HDS->ChangeCurve(inewC);
    PutPCurves(newC, E, comppc1, comppc2);
  }
  //  else if (iswalk && interpol) {
  //    InterpolCurves(C, E, inewC, comppc1, comppc2, HDS);
  //    TopOpeBRepDS_Curve& newC = HDS->ChangeCurve(inewC);
  //    PutPCurves(newC, E, comppc1, comppc2);
  //  }

  else
  {
    if (comppc)
    {
      TopOpeBRepDS_Curve newC1;
      inewC                    = HDS->MakeCurve(C, newC1);
      TopOpeBRepDS_Curve& newC = HDS->ChangeCurve(inewC);
      if (iswalk && !approx)
      {
        if (compc3d && C3D.IsNull())
          throw Standard_ProgramError("TopOpeBRepDS_BuildTool::RecomputeCurve 1");
        newC.Curve1(C.Curve1());
        newC.Curve2(C.Curve2());
      }
      else
        ComputePCurves(C, E, newC, comppc1, comppc2, compc3d);
      PutPCurves(newC, E, comppc1, comppc2);
    }
  }
}

//=================================================================================================

void TopOpeBRepDS_BuildTool::CopyFace(const TopoDS_Shape& Fin, TopoDS_Shape& Fou) const
{
  Fou = Fin.EmptyCopied();
}

//=================================================================================================

void TopOpeBRepDS_BuildTool::AddEdgeVertex(const TopoDS_Shape& Ein,
                                           TopoDS_Shape&       Eou,
                                           const TopoDS_Shape& V) const
{
  myBuilder.Add(Eou, V);
  TopoDS_Edge   e1 = TopoDS::Edge(Ein);
  TopoDS_Edge   e2 = TopoDS::Edge(Eou);
  TopoDS_Vertex v1 = TopoDS::Vertex(V);
  myBuilder.Transfert(e1, e2, v1, v1);
}

//=================================================================================================

void TopOpeBRepDS_BuildTool::AddEdgeVertex(TopoDS_Shape& E, const TopoDS_Shape& V) const
{
  myBuilder.Add(E, V);
}

//=================================================================================================

void TopOpeBRepDS_BuildTool::AddWireEdge(TopoDS_Shape& W, const TopoDS_Shape& E) const
{
  myBuilder.Add(W, E);
}

//=================================================================================================

void TopOpeBRepDS_BuildTool::AddFaceWire(TopoDS_Shape& F, const TopoDS_Shape& W) const
{
  myBuilder.Add(F, W);
}

//=================================================================================================

void TopOpeBRepDS_BuildTool::AddShellFace(TopoDS_Shape& Sh, const TopoDS_Shape& F) const
{
  myBuilder.Add(Sh, F);
}

//=================================================================================================

void TopOpeBRepDS_BuildTool::AddSolidShell(TopoDS_Shape& S, const TopoDS_Shape& Sh) const
{
  myBuilder.Add(S, Sh);
}

//=================================================================================================

void TopOpeBRepDS_BuildTool::Parameter(const TopoDS_Shape& E,
                                       const TopoDS_Shape& V,
                                       const double        P) const
{
  const TopoDS_Edge&   e = TopoDS::Edge(E);
  const TopoDS_Vertex& v = TopoDS::Vertex(V);
  double               p = P;

  // 13/07/95 :
  TopLoc_Location         loc;
  double                  f, l;
  occ::handle<Geom_Curve> C = BRep_Tool::Curve(e, loc, f, l);
  if (!C.IsNull() && C->IsPeriodic())
  {
    double per = C->Period();

    TopAbs_Orientation oV = TopAbs_FORWARD;

    TopExp_Explorer exV(e, TopAbs_VERTEX);
    for (; exV.More(); exV.Next())
    {
      const TopoDS_Vertex& vofe = TopoDS::Vertex(exV.Current());
      if (vofe.IsSame(v))
      {
        oV = vofe.Orientation();
        break;
      }
    }
    if (exV.More())
    {
      if (oV == TopAbs_REVERSED)
      {
        if (p < f)
        {
          double pp = ElCLib::InPeriod(p, f, f + per);
          p         = pp;
        }
      }
    }
  }

  myBuilder.UpdateVertex(v, p, e,
                         0); // NYI : Tol on new vertex ??
}

//=================================================================================================

void TopOpeBRepDS_BuildTool::Range(const TopoDS_Shape& E,
                                   const double        first,
                                   const double        last) const
{
  myBuilder.Range(TopoDS::Edge(E), first, last);
}

//=================================================================================================

void TopOpeBRepDS_BuildTool::UpdateEdge(const TopoDS_Shape& Ein, TopoDS_Shape& Eou) const
{
  TopLoc_Location         loc;
  double                  f1, l1;
  double                  f2, l2;
  occ::handle<Geom_Curve> Cin = BRep_Tool::Curve(TopoDS::Edge(Ein), loc, f1, l1);
  occ::handle<Geom_Curve> Cou = BRep_Tool::Curve(TopoDS::Edge(Eou), loc, f2, l2);
  if (Cin.IsNull() || Cou.IsNull())
    return;

  if (Cou->IsPeriodic())
  {
    double f2n = f2, l2n = l2;
    if (l2n <= f2n)
    {
      ElCLib::AdjustPeriodic(f1, l1, Precision::PConfusion(), f2n, l2n);
      Range(Eou, f2n, l2n);
    }
  }
}

//=======================================================================
// function : Project
// purpose  : project a vertex on a curve
//=======================================================================

static bool Project(const occ::handle<Geom_Curve>& C, const TopoDS_Vertex& V, double& p)
{
  gp_Pnt            P   = BRep_Tool::Pnt(V);
  double            tol = BRep_Tool::Tolerance(V);
  GeomAdaptor_Curve GAC(C);
  Extrema_ExtPC     extrema(P, GAC);
  if (extrema.IsDone())
  {
    int i, n = extrema.NbExt();
    for (i = 1; i <= n; i++)
    {
      if (extrema.IsMin(i))
      {
        Extrema_POnCurv EPOC = extrema.Point(i);
        if (P.Distance(EPOC.Value()) <= tol)
        {
          p = EPOC.Parameter();
          return true;
        }
      }
    }
  }
  return false;
}

//=================================================================================================

void TopOpeBRepDS_BuildTool::Parameter(const TopOpeBRepDS_Curve& C,
                                       TopoDS_Shape&             E,
                                       TopoDS_Shape&             V) const
{
  double newparam;
  Project(C.Curve(), TopoDS::Vertex(V), newparam);
  Parameter(E, V, newparam);
}

//=================================================================================================

void TopOpeBRepDS_BuildTool::Curve3D(TopoDS_Shape&                  E,
                                     const occ::handle<Geom_Curve>& C,
                                     const double                   Tol) const
{
  myBuilder.UpdateEdge(TopoDS::Edge(E), C, Tol);
}

//=================================================================================================

void TopOpeBRepDS_BuildTool::TranslateOnPeriodic(TopoDS_Shape&              F,
                                                 TopoDS_Shape&              E,
                                                 occ::handle<Geom2d_Curve>& PC) const
{
  // get range C3Df,C3Dl of 3d curve C3D of E
  TopLoc_Location L;
  double          C3Df, C3Dl;
  //  occ::handle<Geom_Curve> C3D = BRep_Tool::Curve(TopoDS::Edge(E),L,C3Df,C3Dl);
  occ::handle<Geom_Curve> C3D = BRep_Tool::Curve(TopoDS::Edge(E), C3Df, C3Dl); // 13-07-97: xpu

  double first = C3Df, last = C3Dl;
  if (C3D->IsPeriodic())
  {
    if (last < first)
      last += std::abs(first - last);
  }

  // jyl-xpu : 13-06-97 :
  // if <PC> is U isoline on sphere, a special parametrization
  // is to provide, we compute <PC> (which is a line) bounds
  // with C3D bounds.
  bool UisoLineOnSphe = FUN_UisoLineOnSphe(F, PC);
  bool newv           = true;

  double du, dv;

  gp_Pnt2d ptest;
  double   t = (first + last) * .5;
  PC->D0(t, ptest);
  double u1 = ptest.X(), u2 = u1;
  double v1 = ptest.Y(), v2 = v1;

  if (newv)
  {
    if (UisoLineOnSphe)
    {
      occ::handle<Geom_Curve>    c3d = BRep_Tool::Curve(TopoDS::Edge(E), C3Df, C3Dl);
      GeomAdaptor_Curve          GC(c3d);
      gp_Pnt                     p3dtest = GC.Value(t);
      occ::handle<Geom_Surface>  surf    = BRep_Tool::Surface(TopoDS::Face(F));
      GeomAPI_ProjectPointOnSurf pons(p3dtest, surf);
      if (!(pons.NbPoints() < 1))
        pons.LowerDistanceParameters(u2, v2);
    }
    else
      TopOpeBRepTool_ShapeTool::AdjustOnPeriodic(F, u2, v2);
  }
  if (!newv)
    TopOpeBRepTool_ShapeTool::AdjustOnPeriodic(F, u2, v2);
  du = u2 - u1, dv = v2 - v1;

  if (du != 0. || dv != 0.)
  {
    // translate curve PC of du,dv
    occ::handle<Geom2d_Curve> PCT = occ::down_cast<Geom2d_Curve>(PC->Copy());
    PCT->Translate(gp_Vec2d(du, dv));
    PC = PCT;
  }
}

// RLE - IAB 16 june 94
// should be provided by the BRep_Builder

Standard_EXPORT void TopOpeBRepDS_SetThePCurve(const BRep_Builder&              B,
                                               TopoDS_Edge&                     E,
                                               const TopoDS_Face&               F,
                                               const TopAbs_Orientation         O,
                                               const occ::handle<Geom2d_Curve>& C)
{
  // check if there is already a pcurve on non planar faces
  double                    f, l;
  occ::handle<Geom2d_Curve> OC;
  TopLoc_Location           SL;
  occ::handle<Geom_Plane>   GP = occ::down_cast<Geom_Plane>(BRep_Tool::Surface(F, SL));
  if (GP.IsNull())
    OC = BRep_Tool::CurveOnSurface(E, F, f, l);

  if (OC.IsNull())
    B.UpdateEdge(E, C, F, Precision::Confusion());
  else
  {
    bool degen = BRep_Tool::Degenerated(E);
    if (!degen)
    {
      if (O == TopAbs_REVERSED)
        B.UpdateEdge(E, OC, C, F, Precision::Confusion());
      else
        B.UpdateEdge(E, C, OC, F, Precision::Confusion());
    }
  }
}

//=================================================================================================

void TopOpeBRepDS_BuildTool::PCurve(TopoDS_Shape&                    F,
                                    TopoDS_Shape&                    E,
                                    const occ::handle<Geom2d_Curve>& PC) const
{
  if (!PC.IsNull())
  {
    TopoDS_Face               FF  = TopoDS::Face(F);
    TopoDS_Edge               EE  = TopoDS::Edge(E);
    occ::handle<Geom2d_Curve> PCT = PC;

    // pour iab, ajout de Translate
    bool tran = myTranslate;

    // xpu : 13-06-97 :
    // recompute twice the pcurve boundaries if OverWrite
    // if the pcurve <PC> is U isoline on sphere -> to avoid.
    bool UisoLineOnSphe = FUN_UisoLineOnSphe(F, PC);
    bool overwrite      = UisoLineOnSphe ? false : myOverWrite;
    // xpu : 13-06-97

    if (tran)
      TranslateOnPeriodic(F, E, PCT);

    if (overwrite)
      myBuilder.UpdateEdge(EE, PCT, FF, 0);
    else
      TopOpeBRepDS_SetThePCurve(myBuilder, EE, FF, E.Orientation(), PCT);

    // parametrage sur la nouvelle courbe 2d
    TopExp_Explorer exi(E, TopAbs_VERTEX);
    for (; exi.More(); exi.Next())
    {
      const TopoDS_Vertex& vi = TopoDS::Vertex(exi.Current());
      if (vi.Orientation() != TopAbs_INTERNAL)
        continue;
      double tolvi = TopOpeBRepTool_ShapeTool::Tolerance(vi);
      // NYI tester l'existence d'au moins
      // NYI un parametrage de vi sur EE (en 3d ou en 2d)
      // NYI --> a faire dans BRep_Tool
      double newpar = BRep_Tool::Parameter(vi, EE);
      myBuilder.UpdateVertex(vi, newpar, EE, FF, tolvi);
    } // INTERNAL vertex
  }
}

//=================================================================================================

void TopOpeBRepDS_BuildTool::PCurve(TopoDS_Shape&                    F,
                                    TopoDS_Shape&                    E,
                                    const TopOpeBRepDS_Curve&        CDS,
                                    const occ::handle<Geom2d_Curve>& PC) const
{
  if (!PC.IsNull())
  {
    TopoDS_Face FF = TopoDS::Face(F);
    TopoDS_Edge EE = TopoDS::Edge(E);

    const occ::handle<Geom2d_Curve>& PCT = PC;
    double                           CDSmin, CDSmax;
    bool                             rangedef = CDS.Range(CDSmin, CDSmax);

    TopLoc_Location         L;
    double                  Cf, Cl;
    occ::handle<Geom_Curve> C = BRep_Tool::Curve(EE, L, Cf, Cl);

    if (!C.IsNull())
    {
      bool                     deca     = (std::abs(Cf - CDSmin) > Precision::PConfusion());
      occ::handle<Geom2d_Line> line2d   = occ::down_cast<Geom2d_Line>(PCT);
      bool                     isline2d = !line2d.IsNull();
      bool                     tran     = (rangedef && deca && C->IsPeriodic() && isline2d);
      if (tran)
      {
        TopLoc_Location                 Loc;
        const occ::handle<Geom_Surface> Surf     = BRep_Tool::Surface(FF, Loc);
        bool                            isUperio = Surf->IsUPeriodic();
        bool                            isVperio = Surf->IsVPeriodic();
        gp_Dir2d                        dir2d    = line2d->Direction();
        double                          delta;
        if (isUperio && dir2d.IsParallel(gp::DX2d(), Precision::Angular()))
        {
          delta = (CDSmin - Cf) * dir2d.X();
          PCT->Translate(gp_Vec2d(delta, 0.));
        }
        else if (isVperio && dir2d.IsParallel(gp::DY2d(), Precision::Angular()))
        {
          delta = (CDSmin - Cf) * dir2d.Y();
          PCT->Translate(gp_Vec2d(0., delta));
        }
      }
    }

    TopOpeBRepDS_SetThePCurve(myBuilder, EE, FF, E.Orientation(), PCT);
  }
}

//=================================================================================================

void TopOpeBRepDS_BuildTool::Orientation(TopoDS_Shape& S, const TopAbs_Orientation O) const
{
  S.Orientation(O);
}

//=================================================================================================

TopAbs_Orientation TopOpeBRepDS_BuildTool::Orientation(const TopoDS_Shape& S) const
{
  return S.Orientation();
}

//=================================================================================================

void TopOpeBRepDS_BuildTool::Closed(TopoDS_Shape& S, const bool B) const
{
  S.Closed(B);
}

//=================================================================================================

bool TopOpeBRepDS_BuildTool::Approximation() const
{
  return myCurveTool.GetGeomTool().TypeC3D() != TopOpeBRepTool_BSPLINE1;
}

void TopOpeBRepDS_BuildTool::UpdateSurface(const TopoDS_Shape&              F,
                                           const occ::handle<Geom_Surface>& SU) const
{
  BRep_Builder    BB;
  TopLoc_Location L;
  double          tol = BRep_Tool::Tolerance(TopoDS::Face(F));
  BB.UpdateFace(TopoDS::Face(F), SU, L, tol);
}

void TopOpeBRepDS_BuildTool::UpdateSurface(const TopoDS_Shape& E,
                                           const TopoDS_Shape& oldF,
                                           const TopoDS_Shape& newF) const
{
  BRep_Builder                     BB;
  double                           f, l;
  const occ::handle<Geom2d_Curve>& PC =
    BRep_Tool::CurveOnSurface(TopoDS::Edge(E), TopoDS::Face(oldF), f, l);
  double tol = BRep_Tool::Tolerance(TopoDS::Face(oldF));
  BB.UpdateEdge(TopoDS::Edge(E), PC, TopoDS::Face(newF), tol);
}

/* // - merge 04-07-97
//=================================================================================================

void  TopOpeBRepDS_BuildTool::RecomputeCurve
(const TopOpeBRepDS_Curve& C1,
 TopoDS_Shape& E,
 TopOpeBRepDS_Curve& C2 ) const
{
  // - C1 curves have been approximated by BSplines of degree 1 :
  // or
  // - C1.Curve() is non projectable on at least one of the original
  // intersecting faces.

  const TopOpeBRepTool_GeomTool& GT = myCurveTool.GetGeomTool();
  bool compc3d = GT.CompC3D();
  bool comppc1 = GT.CompPC1();
  bool comppc2 = GT.CompPC2();

  const occ::handle<Geom_Curve>& C3D = C1.Curve();
  if (compc3d && C3D.IsNull()) throw Standard_ProgramError("TopOpeBRepDS_BuildTool::RecomputeCurve
1"); if (comppc1 && C2.Shape1().IsNull()) throw
Standard_ProgramError("TopOpeBRepDS_BuildTool::RecomputeCurve 2"); if (comppc2 &&
C2.Shape2().IsNull()) throw Standard_ProgramError("TopOpeBRepDS_BuildTool::RecomputeCurve 3");
  TopoDS_Vertex Vmin,Vmax; TopExp::Vertices(TopoDS::Edge(E),Vmin,Vmax);
  if ( Vmin.IsNull() ) throw Standard_ProgramError("TopOpeBRepDS_BuildTool::RecomputeCurve 4");
  if ( Vmax.IsNull() ) throw Standard_ProgramError("TopOpeBRepDS_BuildTool::RecomputeCurve 5");

  bool kbspl1 = false;
  occ::handle<Geom_BSplineCurve> BS = occ::down_cast<Geom_BSplineCurve>(C3D);
  if (!BS.IsNull()) kbspl1 = (BS->Degree() == 1);
  if (kbspl1) RecomputeBSpline1Curve(C1,E,C2);
  else        RecomputeCurveOnCone(C1,E,C2);
}

//=================================================================================================

void  TopOpeBRepDS_BuildTool::RecomputeBSpline1Curve
(const TopOpeBRepDS_Curve& C1,
 TopoDS_Shape& EE,
 TopOpeBRepDS_Curve& C2) const
{
  // C1 curves have been approximated by BSplines of degree 1 :
  // compute new geometry on curves.

  TopoDS_Edge& E = TopoDS::Edge(EE);

  const TopOpeBRepTool_GeomTool& GT = myCurveTool.GetGeomTool();
  TopOpeBRepTool_OutCurveType typec3d = GT.TypeC3D();
  bool compc3d =            GT.CompC3D();
  bool comppc1 =            GT.CompPC1();
  bool comppc2 =            GT.CompPC2();

  const TopoDS_Face& F1 = TopoDS::Face(C2.Shape1());
  const TopoDS_Face& F2 = TopoDS::Face(C2.Shape2());

  const occ::handle<Geom_Curve>&   C3D = C1.Curve();
  const occ::handle<Geom2d_Curve>& PC1 = C1.Curve1();
  const occ::handle<Geom2d_Curve>& PC2 = C1.Curve2();

  // Vmin,Vmax = bounding vertices of edge <E>
  // and their parameters parmin,parmax .

  TopoDS_Vertex Vmin, Vmax;
  double parmin = 0.0, parmax = 0.0;
  ::GetOrientedEdgeVertices (E, Vmin, Vmax, parmin, parmax);

  occ::handle<Geom_Curve>   C3Dnew;
  occ::handle<Geom2d_Curve> PC1new;
  occ::handle<Geom2d_Curve> PC2new;
  double tolreached3d,tolreached2d;

  if ( typec3d == TopOpeBRepTool_BSPLINE1 ) {
    if ( compc3d ) {
      C3Dnew = occ::down_cast<Geom_BSplineCurve>(C3D->Copy());
      (occ::down_cast<Geom_BSplineCurve>(C3Dnew))->Segment(parmin,parmax);
    }
    if ( comppc1 && (!PC1.IsNull()) ) {
      PC1new = occ::down_cast<Geom2d_BSplineCurve>(PC1->Copy());
      (occ::down_cast<Geom2d_BSplineCurve>(PC1new))->Segment(parmin,parmax);
    }
    if ( comppc2 && (!PC2.IsNull()) ) {
      PC2new = occ::down_cast<Geom2d_BSplineCurve>(PC2->Copy());
      (occ::down_cast<Geom2d_BSplineCurve>(PC2new))->Segment(parmin,parmax);
    }
  }

  else if ( typec3d == TopOpeBRepTool_APPROX ) {
    if (!comppc1 || !comppc2) throw Standard_NotImplemented("DSBuildToolAPPROX");
    myCurveTool.MakeCurves(parmin,parmax,
               C3D,PC1,PC2,F1,F2,
               C3Dnew,PC1new,PC2new,
               tolreached3d,tolreached2d);
  }

  else if ( typec3d == TopOpeBRepTool_INTERPOL ) {
    throw Standard_NotImplemented("DSBuildToolINTERPOL");
  }

  double newtol,newparmin,newparmax;
  ::FUN_updateEDGECURVETOL
    (*this,F1,F2,E,C3Dnew,tolreached3d,tolreached2d,tolreached2d,
     newtol,newparmin,newparmax);

  if (!C3Dnew.IsNull()) {
    C2.DefineCurve(C3Dnew,newtol,false);
    C2.SetRange(newparmin,newparmax);
  }
  if (!PC1new.IsNull()) C2.Curve1(PC1new);
  if (!PC2new.IsNull()) C2.Curve2(PC2new);
}

//=================================================================================================

void  TopOpeBRepDS_BuildTool::RecomputeCurveOnCone
  (const TopOpeBRepDS_Curve& C1,
   TopoDS_Shape&             EE,
   TopOpeBRepDS_Curve&       C2 ) const
{
  // C1 Pcurves have not been computed because C1 Curve is not projectable
  // on one at least of the intersecting faces giving C1 Curve.
  // (see TopOpeBRepTool_CurveTool::IsProjectable())

  TopoDS_Edge& E = TopoDS::Edge(EE);

  const TopOpeBRepTool_GeomTool& GT = myCurveTool.GetGeomTool();
  TopOpeBRepTool_OutCurveType typec3d = GT.TypeC3D();
  bool compc3d =            GT.CompC3D();
  bool comppc1 =            GT.CompPC1();
  bool comppc2 =            GT.CompPC2();

  const TopoDS_Face& F1 = TopoDS::Face(C2.Shape1());
  const TopoDS_Face& F2 = TopoDS::Face(C2.Shape2());

  const occ::handle<Geom_Curve>&   C3D = C1.Curve();
  const occ::handle<Geom2d_Curve>& PC1 = C1.Curve1();
  const occ::handle<Geom2d_Curve>& PC2 = C1.Curve2();

  // get bounding vertices Vmin,Vmax supported by the new edge <E>
  // and their corresponding parameters parmin,parmax .
  TopoDS_Vertex Vmin, Vmax;
  double parmin = 0.0, parmax = 0.0;
  ::GetOrientedEdgeVertices (E, Vmin, Vmax, parmin, parmax);

  if ( C3D->IsPeriodic() ) {
    // ellipse on cone : periodize parmin,parmax
    double period = C3D->LastParameter() - C3D->FirstParameter();
    double f,l;
    if (Vmin.Orientation() == TopAbs_FORWARD) { f = parmin; l = parmax; }
    else {                                      f = parmax; l = parmin; }
    parmin = f; parmax = l;
    ElCLib::AdjustPeriodic(f,f+period,Precision::PConfusion(),parmin,parmax);
  }

  occ::handle<Geom_TrimmedCurve> C3Dnew;
  occ::handle<Geom2d_Curve> PC1new;
  occ::handle<Geom2d_Curve> PC2new;
  double tolreached3d = C1.Tolerance();
  double tolreached2d1 = C1.Tolerance();
  double tolreached2d2 = C1.Tolerance();
  if (compc3d) C3Dnew = new Geom_TrimmedCurve(C3D,parmin,parmax);
  if (comppc1) PC1new = myCurveTool.MakePCurveOnFace(F1,C3Dnew,tolreached2d1);
  if (comppc2) PC2new = myCurveTool.MakePCurveOnFace(F2,C3Dnew,tolreached2d2);

#ifdef DRAW
  if (tBUTO) {FUN_draw(F1); FUN_draw(F2); FUN_draw(E);}
#endif

  double newtol,newparmin,newparmax;
  FUN_updateEDGECURVETOL
  (*this,F1,F2,E,C3Dnew,tolreached3d,tolreached2d1,tolreached2d2,
   newtol,newparmin,newparmax);

//   jyl : 16-06-97
//  double fac = 0.3798123578771;
//  double tol = newtol;
//  double par3d = (1-fac)*newparmin + (fac)*newparmax;
//  double par2d = par3d - newparmin;
//
//  gp_Pnt P3DC3D;       C3D->D0(par3d,P3DC3D);
//
//  bool UisoLineOnSphe1 = false;
//  UisoLineOnSphe1 = ::FUN_UisoLineOnSphe(F1,PC1new);
//  if (UisoLineOnSphe1) {
//    double isrev1 =
//      ::FUN_reversePC(PC1new,F1,P3DC3D,par2d,tol);
//
//
//  }
//
//  bool UisoLineOnSphe2 = false;
//  UisoLineOnSphe2 = ::FUN_UisoLineOnSphe(F2,PC2new);
//  if (UisoLineOnSphe2) {
//    double isrev2 =
//     ::FUN_reversePC(PC2new,F2,P3DC3D,par2d,tol);
//
//  }

  // xpu : 17-06-97
  // Rmq : C1.Curve<i>() ne sert plus qu'a determiner si la courbe
  //       est une isos de la sphere
  // NYI : enlever FUN_reversePC
  bool UisoLineOnSphe1 = false;
  UisoLineOnSphe1 = ::FUN_UisoLineOnSphe(F1,PC1new);
  if (UisoLineOnSphe1) {
    ::FUN_makeUisoLineOnSphe(F1,C3Dnew,PC1new,newtol);
  }
  bool UisoLineOnSphe2 = false;
  UisoLineOnSphe2 = ::FUN_UisoLineOnSphe(F2,PC2new);
  if (UisoLineOnSphe2) {
    ::FUN_makeUisoLineOnSphe(F2,C3Dnew,PC2new,newtol);
  } // xpu : 17-06-97

  if (!C3Dnew.IsNull()) C2.Curve(C3Dnew,newtol);
  if (!PC1new.IsNull()) C2.Curve1(PC1new);
  if (!PC2new.IsNull()) C2.Curve2(PC2new);
}*/ // - merge 04-07-97
