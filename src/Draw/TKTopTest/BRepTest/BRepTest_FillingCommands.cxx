// Created on: 1996-07-10
// Created by: Joelle CHAUVET
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

// Modified:	Wed Mar  5 09:45:42 1997
//    by:	Joelle CHAUVET
//              G1134 : new command "approxplate"
// Modified:	Thu Jun 12 16:51:36 1997
//    by:	Jerome LEMONIER
//              Mise a jour suite a la modification des methodes Curves2d
//		et Sense GeomPlate_BuildPlateSurface.
// Modified:	Mon Nov  3 10:24:07 1997
//		utilisation de BRepFill_CurveConstraint

#include <GeometryTest.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Appli.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <DrawTrSurf.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepTest.hxx>
#include <DBRep.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

#include <BRepLib.hxx>
#include <BRep_Builder.hxx>
#include <GeomPlate_Surface.hxx>

#include <GeomPlate_MakeApprox.hxx>
#include <GeomPlate_PlateG0Criterion.hxx>
#include <GeomPlate_PlateG1Criterion.hxx>
#include <BRepFill_CurveConstraint.hxx>
#include <Geom_Surface.hxx>

#include <TopoDS_Wire.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>

#include <AdvApp2Var_ApproxAFunc2Var.hxx>

#include <Geom_BSplineSurface.hxx>

#include <gp_XY.hxx>
#include <NCollection_Sequence.hxx>
#include <gp_XYZ.hxx>

#include <BRepAdaptor_Curve.hxx>

#include <BRepOffsetAPI_MakeFilling.hxx>
#include <TCollection_AsciiString.hxx>

#include <BRepTest_Objects.hxx>

#include <cstdio>
#include <gp_Pnt.hxx>

// pour mes tests
#ifdef OCCT_DEBUG
  #include <OSD_Chronometer.hxx>
  #include <Geom_Line.hxx>
#endif

const int    defDegree      = 3;
const int    defNbPtsOnCur  = 10;
const int    defNbIter      = 3;
const bool   defAnisotropie = false;
const double defTol2d       = 0.00001;
const double defTol3d       = 0.0001;
const double defTolAng      = 0.01;
const double defTolCurv     = 0.1;
const int    defMaxDeg      = 8;
const int    defMaxSegments = 9;

int    Degree      = defDegree;
int    NbPtsOnCur  = defNbPtsOnCur;
int    NbIter      = defNbIter;
bool   Anisotropie = defAnisotropie;
double Tol2d       = defTol2d;
double Tol3d       = defTol3d;
double TolAng      = defTolAng;
double TolCurv     = defTolCurv;
int    MaxDeg      = defMaxDeg;
int    MaxSegments = defMaxSegments;

////////////////////////////////////////////////////////////////////////////////
//  commande plate : resultat face sur surface plate
////////////////////////////////////////////////////////////////////////////////

static int plate(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 8)
    return 1;
  int                                                            NbCurFront = Draw::Atoi(a[3]);
  occ::handle<NCollection_HArray1<occ::handle<Adaptor3d_Curve>>> Fronts =
    new NCollection_HArray1<occ::handle<Adaptor3d_Curve>>(1, NbCurFront);
  occ::handle<NCollection_HArray1<int>> Tang     = new NCollection_HArray1<int>(1, NbCurFront);
  occ::handle<NCollection_HArray1<int>> NbPtsCur = new NCollection_HArray1<int>(1, NbCurFront);
  BRep_Builder                          B;

  GeomPlate_BuildPlateSurface Henri(3, 15, 2);

  int i;
  for (i = 1; i <= NbCurFront; i++)
  {
    TopoDS_Shape aLocalEdge(DBRep::Get(a[3 * i + 1], TopAbs_EDGE));
    TopoDS_Edge  E = TopoDS::Edge(aLocalEdge);
    //    TopoDS_Edge E = TopoDS::Edge(DBRep::Get(a[3*i+1],TopAbs_EDGE));
    if (E.IsNull())
      return 1;
    TopoDS_Shape aLocalFace(DBRep::Get(a[3 * i + 2], TopAbs_FACE));
    TopoDS_Face  F = TopoDS::Face(aLocalFace);
    //    TopoDS_Face F = TopoDS::Face(DBRep::Get(a[3*i+2],TopAbs_FACE));
    if (F.IsNull())
      return 1;
    int T = Draw::Atoi(a[3 * i + 3]);
    Tang->SetValue(i, T);
    NbPtsCur->SetValue(i, Draw::Atoi(a[2]));
    occ::handle<BRepAdaptor_Surface> S = new BRepAdaptor_Surface();
    S->Initialize(F);
    occ::handle<BRepAdaptor_Curve2d> C = new BRepAdaptor_Curve2d();
    C->Initialize(E, F);
    Adaptor3d_CurveOnSurface              ConS(C, S);
    occ::handle<Adaptor3d_CurveOnSurface> HConS = new Adaptor3d_CurveOnSurface(ConS);
    Fronts->SetValue(i, HConS);
    occ::handle<GeomPlate_CurveConstraint> Cont =
      new BRepFill_CurveConstraint(HConS, Tang->Value(i), NbPtsCur->Value(i));
    Henri.Add(Cont);
  }
  occ::handle<Draw_ProgressIndicator> aProgress = new Draw_ProgressIndicator(di, 1);
  Henri.Perform(aProgress->Start());
  if (aProgress->UserBreak())
  {
    di << "Error: UserBreak\n";
    return 0;
  }

  double ErrG0 = 1.1 * Henri.G0Error();
  // std::cout<<" dist. max = "<<Henri.G0Error()<<" ; angle max = "<<Henri.G1Error()<<std::endl;
  di << " dist. max = " << Henri.G0Error() << " ; angle max = " << Henri.G1Error() << "\n";

  BRepBuilderAPI_MakeWire MW;
  for (i = 1; i <= NbCurFront; i++)
  {
    int         iInOrder = Henri.Order()->Value(i);
    TopoDS_Edge E;
    if (Henri.Sense()->Value(iInOrder) == 1)
    {
      BRepBuilderAPI_MakeEdge ME(Henri.Curves2d()->Value(iInOrder),
                                 Henri.Surface(),
                                 Fronts->Value(iInOrder)->LastParameter(),
                                 Fronts->Value(iInOrder)->FirstParameter());
      E = ME.Edge();
    }
    else
    {
      BRepBuilderAPI_MakeEdge ME(Henri.Curves2d()->Value(iInOrder),
                                 Henri.Surface(),
                                 Fronts->Value(iInOrder)->FirstParameter(),
                                 Fronts->Value(iInOrder)->LastParameter());
      E = ME.Edge();
    }
    B.UpdateVertex(TopExp::FirstVertex(E), ErrG0);
    B.UpdateVertex(TopExp::LastVertex(E), ErrG0);
    BRepLib::BuildCurve3d(E);
    char name[100];
    Sprintf(name, "Edge_%d", i);
    DBRep::Set(name, E);
    MW.Add(E);
    if (MW.IsDone() == false)
    {
      throw Standard_Failure("mkWire is over ");
    }
  }
  TopoDS_Wire W;
  W = MW.Wire();
  if (!(W.Closed()))
    throw Standard_Failure("Wire is not closed");
  BRepBuilderAPI_MakeFace MF(Henri.Surface(), W, true);
  DBRep::Set(a[1], MF.Face());
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
//  commande gplate : resultat face egale a la surface approchee
////////////////////////////////////////////////////////////////////////////////

static int gplate(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 6)
    return 1;
  int NbCurFront = Draw::Atoi(a[2]), NbPointConstraint = Draw::Atoi(a[3]);

  GeomPlate_BuildPlateSurface Henri(3, 15, 2);

  int i;
  int Conti;
  int Indice = 4;
  // Surface d'init
  TopoDS_Shape aLocalFace(DBRep::Get(a[Indice++], TopAbs_FACE));
  TopoDS_Face  SI = TopoDS::Face(aLocalFace);
  //  TopoDS_Face SI = TopoDS::Face(DBRep::Get(a[Indice++],TopAbs_FACE));
  if (SI.IsNull())
    Indice--;
  else
  {
    occ::handle<BRepAdaptor_Surface> HSI = new BRepAdaptor_Surface();
    HSI->Initialize(SI);
    Henri.LoadInitSurface(BRep_Tool::Surface(HSI->Face()));
  }
  for (i = 1; i <= NbCurFront; i++)
  {
    TopoDS_Shape aLocalShape(DBRep::Get(a[Indice++], TopAbs_EDGE));
    TopoDS_Edge  E = TopoDS::Edge(aLocalShape);
    //    TopoDS_Edge E = TopoDS::Edge(DBRep::Get(a[Indice++],TopAbs_EDGE));
    if (E.IsNull())
      return 1;
    Conti = Draw::Atoi(a[Indice++]);
    if ((Conti == 0) || (Conti == -1))
    {
      occ::handle<BRepAdaptor_Curve> C = new BRepAdaptor_Curve();
      C->Initialize(E);
      const occ::handle<Adaptor3d_Curve>&    aC   = C; // to avoid ambiguity
      occ::handle<GeomPlate_CurveConstraint> Cont = new BRepFill_CurveConstraint(aC, Conti);
      Henri.Add(Cont);
    }
    else
    {
      aLocalFace    = DBRep::Get(a[Indice++], TopAbs_FACE);
      TopoDS_Face F = TopoDS::Face(aLocalFace);
      //	TopoDS_Face F = TopoDS::Face(DBRep::Get(a[Indice++],TopAbs_FACE));
      if (F.IsNull())
        return 1;
      occ::handle<BRepAdaptor_Surface> S = new BRepAdaptor_Surface();
      S->Initialize(F);
      occ::handle<BRepAdaptor_Curve2d> C = new BRepAdaptor_Curve2d();
      C->Initialize(E, F);
      Adaptor3d_CurveOnSurface               ConS(C, S);
      occ::handle<Adaptor3d_CurveOnSurface>  HConS = new Adaptor3d_CurveOnSurface(ConS);
      occ::handle<GeomPlate_CurveConstraint> Cont  = new BRepFill_CurveConstraint(HConS, Conti);
      Henri.Add(Cont);
    }
  }

  for (i = 1; i <= NbPointConstraint; i++)
  {
    //      gp_Pnt P1,P2,P3;
    gp_Pnt P1;
    //      gp_Vec V1,V2,V3,V4,V5;

    if (DrawTrSurf::GetPoint(a[Indice], P1))
    {
      Conti                                        = 0;
      occ::handle<GeomPlate_PointConstraint> PCont = new GeomPlate_PointConstraint(P1, 0);
      Henri.Add(PCont);
      Indice++;
    }
    else
    {
      double u = Draw::Atof(a[Indice++]), v = Draw::Atof(a[Indice++]);

      Conti         = Draw::Atoi(a[Indice++]);
      aLocalFace    = DBRep::Get(a[Indice++], TopAbs_FACE);
      TopoDS_Face F = TopoDS::Face(aLocalFace);
      //	  TopoDS_Face F = TopoDS::Face(DBRep::Get(a[Indice++],TopAbs_FACE));
      if (F.IsNull())
        return 1;
      occ::handle<BRepAdaptor_Surface> HF = new BRepAdaptor_Surface();
      HF->Initialize(F);
      occ::handle<GeomPlate_PointConstraint> PCont =
        new GeomPlate_PointConstraint(u,
                                      v,
                                      BRep_Tool::Surface(HF->Face()),
                                      Conti,
                                      0.001,
                                      0.001,
                                      0.001);
      Henri.Add(PCont);
    }
  }
  occ::handle<Draw_ProgressIndicator> aProgress = new Draw_ProgressIndicator(di, 1);
  Henri.Perform(aProgress->Start());
  if (aProgress->UserBreak())
  {
    di << "Error: UserBreak\n";
    return 0;
  }
  int    nbcarreau = 9;
  int    degmax    = 8;
  double seuil;

  occ::handle<GeomPlate_Surface> gpPlate = Henri.Surface();
  NCollection_Sequence<gp_XY>    S2d;
  NCollection_Sequence<gp_XYZ>   S3d;
  S2d.Clear();
  S3d.Clear();
  Henri.Disc2dContour(4, S2d);
  Henri.Disc3dContour(4, 0, S3d);
  seuil = std::max(0.0001, 10 * Henri.G0Error());
  GeomPlate_PlateG0Criterion critere(S2d, S3d, seuil);
  GeomPlate_MakeApprox       Mapp(gpPlate, critere, 0.0001, nbcarreau, degmax);
  occ::handle<Geom_Surface>  Surf(Mapp.Surface());

  double Umin, Umax, Vmin, Vmax;

  Henri.Surface()->Bounds(Umin, Umax, Vmin, Vmax);

  BRepBuilderAPI_MakeFace MF(Surf, Umin, Umax, Vmin, Vmax, Precision::Confusion());

  DBRep::Set(a[1], MF.Face());
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
//  commande approxplate : resultat face sur surface approchee
////////////////////////////////////////////////////////////////////////////////

static int approxplate(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 9)
    return 1;
  int                                                            NbMedium   = Draw::Atoi(a[2]);
  int                                                            NbCurFront = Draw::Atoi(a[3]);
  occ::handle<NCollection_HArray1<occ::handle<Adaptor3d_Curve>>> Fronts =
    new NCollection_HArray1<occ::handle<Adaptor3d_Curve>>(1, NbCurFront);
  occ::handle<NCollection_HArray1<int>> Tang     = new NCollection_HArray1<int>(1, NbCurFront);
  occ::handle<NCollection_HArray1<int>> NbPtsCur = new NCollection_HArray1<int>(1, NbCurFront);

  GeomPlate_BuildPlateSurface Henri(3, 15, 2);

  int i;
  for (i = 1; i <= NbCurFront; i++)
  {
    TopoDS_Shape aLocalShape(DBRep::Get(a[3 * i + 1], TopAbs_EDGE));
    TopoDS_Edge  E = TopoDS::Edge(aLocalShape);
    //    TopoDS_Edge E = TopoDS::Edge(DBRep::Get(a[3*i+1],TopAbs_EDGE));
    if (E.IsNull())
      return 1;
    TopoDS_Shape aLocalFace(DBRep::Get(a[3 * i + 2], TopAbs_FACE));
    TopoDS_Face  F = TopoDS::Face(aLocalFace);
    //    TopoDS_Face F = TopoDS::Face(DBRep::Get(a[3*i+2],TopAbs_FACE));
    if (F.IsNull())
      return 1;
    int T = Draw::Atoi(a[3 * i + 3]);
    Tang->SetValue(i, T);
    NbPtsCur->SetValue(i, NbMedium);
    occ::handle<BRepAdaptor_Surface> S = new BRepAdaptor_Surface();
    S->Initialize(F);
    occ::handle<BRepAdaptor_Curve2d> C = new BRepAdaptor_Curve2d();
    C->Initialize(E, F);
    Adaptor3d_CurveOnSurface              ConS(C, S);
    occ::handle<Adaptor3d_CurveOnSurface> HConS = new Adaptor3d_CurveOnSurface(ConS);
    Fronts->SetValue(i, HConS);
    occ::handle<GeomPlate_CurveConstraint> Cont =
      new BRepFill_CurveConstraint(HConS, Tang->Value(i), NbPtsCur->Value(i));
    Henri.Add(Cont);
  }

  occ::handle<Draw_ProgressIndicator> aProgress = new Draw_ProgressIndicator(di, 1);
  Henri.Perform(aProgress->Start());
  if (aProgress->UserBreak())
  {
    di << "Error: UserBreak\n";
    return 0;
  }

  double dmax = Henri.G0Error(), anmax = Henri.G1Error();
  // std::cout<<" dist. max = "<<dmax<<" ; angle max = "<<anmax<<std::endl;
  di << " dist. max = " << dmax << " ; angle max = " << anmax << "\n";

  Tol3d                                      = Draw::Atof(a[3 * NbCurFront + 4]);
  int                              Nbmax     = Draw::Atoi(a[3 * NbCurFront + 5]);
  int                              degmax    = Draw::Atoi(a[3 * NbCurFront + 6]);
  int                              CritOrder = Draw::Atoi(a[3 * NbCurFront + 7]);
  occ::handle<GeomPlate_Surface>   surf      = Henri.Surface();
  occ::handle<Geom_BSplineSurface> support;

  if (CritOrder == -1)
  {
    GeomPlate_MakeApprox MApp(surf, Tol3d, Nbmax, degmax, dmax, -1);
    support = MApp.Surface();
  }
  else if (CritOrder >= 0)
  {
    NCollection_Sequence<gp_XY>  S2d;
    NCollection_Sequence<gp_XYZ> S3d;
    S2d.Clear();
    S3d.Clear();
    double seuil;
    if (CritOrder == 0)
    {
      Henri.Disc2dContour(4, S2d);
      Henri.Disc3dContour(4, 0, S3d);
      seuil = std::max(Tol3d, dmax * 10);
      GeomPlate_PlateG0Criterion Crit0(S2d, S3d, seuil);
      GeomPlate_MakeApprox       MApp(surf, Crit0, Tol3d, Nbmax, degmax);
      support = MApp.Surface();
    }
    else if (CritOrder == 1)
    {
      Henri.Disc2dContour(4, S2d);
      Henri.Disc3dContour(4, 1, S3d);
      seuil = std::max(Tol3d, anmax * 10);
      GeomPlate_PlateG1Criterion Crit1(S2d, S3d, seuil);
      GeomPlate_MakeApprox       MApp(surf, Crit1, Tol3d, Nbmax, degmax);
      support = MApp.Surface();
    }
  }

  BRepBuilderAPI_MakeWire MW;
  BRep_Builder            B;
  for (i = 1; i <= NbCurFront; i++)
  {
    int         iInOrder = Henri.Order()->Value(i);
    TopoDS_Edge E;
    if (Henri.Sense()->Value(iInOrder) == 1)
    {
      BRepBuilderAPI_MakeEdge ME(Henri.Curves2d()->Value(iInOrder),
                                 support,
                                 Fronts->Value(iInOrder)->LastParameter(),
                                 Fronts->Value(iInOrder)->FirstParameter());
      E = ME.Edge();
    }
    else
    {
      BRepBuilderAPI_MakeEdge ME(Henri.Curves2d()->Value(iInOrder),
                                 support,
                                 Fronts->Value(iInOrder)->FirstParameter(),
                                 Fronts->Value(iInOrder)->LastParameter());
      E = ME.Edge();
    }
    B.UpdateVertex(TopExp::FirstVertex(E), dmax);
    B.UpdateVertex(TopExp::LastVertex(E), dmax);
    BRepLib::BuildCurve3d(E);
    MW.Add(E);
    if (MW.IsDone() == false)
    {
      throw Standard_Failure("mkWire is over ");
    }
  }
  TopoDS_Wire W;
  W = MW.Wire();
  if (!(W.Closed()))
    throw Standard_Failure("Wire is not closed");
  BRepBuilderAPI_MakeFace MF(support, W, true);
  DBRep::Set(a[1], MF.Face());

  return 0;
}

static int filling(Draw_Interpretor& di, int n, const char** a)
{
#ifdef OCCT_DEBUG
  // Chronometrage
  OSD_Chronometer Chrono;
  Chrono.Reset();
  Chrono.Start();
#endif

  if (n < 7)
  {
    di.PrintHelp(a[0]);
    return 1;
  }

  int NbBounds      = Draw::Atoi(a[2]);
  int NbConstraints = Draw::Atoi(a[3]);
  int NbPoints      = Draw::Atoi(a[4]);

  BRepOffsetAPI_MakeFilling MakeFilling(Degree,
                                        NbPtsOnCur,
                                        NbIter,
                                        Anisotropie,
                                        Tol2d,
                                        Tol3d,
                                        TolAng,
                                        TolCurv,
                                        MaxDeg,
                                        MaxSegments);
  TopoDS_Face               InitFace = TopoDS::Face(DBRep::Get(a[5], TopAbs_FACE));
  if (!InitFace.IsNull())
    MakeFilling.LoadInitSurface(InitFace);

  int                            i = (InitFace.IsNull()) ? 5 : 6, k;
  TopoDS_Edge                    E;
  TopoDS_Face                    F;
  gp_Pnt                         Point;
  int                            Order;
  NCollection_List<TopoDS_Shape> ListForHistory;
  for (k = 1; k <= NbBounds; k++)
  {
    E.Nullify();
    F.Nullify();
    E = TopoDS::Edge(DBRep::Get(a[i], TopAbs_EDGE));
    if (!E.IsNull())
      i++;
    F = TopoDS::Face(DBRep::Get(a[i], TopAbs_FACE));
    if (!F.IsNull())
      i++;

    Order = Draw::Atoi(a[i++]);

    if (!E.IsNull() && !F.IsNull())
      MakeFilling.Add(E, F, (GeomAbs_Shape)Order);
    else if (E.IsNull())
    {
      if (F.IsNull())
      {
        di << "\nWrong parameters\n\n";
        return 1;
      }
      else
        MakeFilling.Add(F, (GeomAbs_Shape)Order);
    }
    else
      MakeFilling.Add(E, (GeomAbs_Shape)Order);

    // History
    if (!E.IsNull())
      ListForHistory.Append(E);
  }
  for (k = 1; k <= NbConstraints; k++)
  {
    E.Nullify();
    F.Nullify();
    E = TopoDS::Edge(DBRep::Get(a[i++], TopAbs_EDGE));
    if (E.IsNull())
    {
      di << "Wrong parameters\n";
      return 1;
    }
    F = TopoDS::Face(DBRep::Get(a[i], TopAbs_FACE));
    if (!F.IsNull())
      i++;

    Order = Draw::Atoi(a[i++]);

    if (F.IsNull())
      MakeFilling.Add(E, (GeomAbs_Shape)Order, false);
    else
      MakeFilling.Add(E, F, (GeomAbs_Shape)Order, false);
  }
  for (k = 1; k <= NbPoints; k++)
  {
    if (DrawTrSurf::GetPoint(a[i], Point))
    {
      MakeFilling.Add(Point);
      i++;
    }
    else
    {
      double U = Draw::Atof(a[i++]), V = Draw::Atof(a[i++]);
      F = TopoDS::Face(DBRep::Get(a[i++], TopAbs_FACE));
      if (F.IsNull())
      {
        di << "Wrong parameters\n";
        return 1;
      }
      Order = Draw::Atoi(a[i++]);

      MakeFilling.Add(U, V, F, (GeomAbs_Shape)Order);
    }
  }

  MakeFilling.Build();
  if (!MakeFilling.IsDone())
  {
    di << "filling failed\n";
    return 0;
  }

  double dmax = MakeFilling.G0Error(), angmax = MakeFilling.G1Error(),
         curvmax = MakeFilling.G2Error();
  di << " dist. max = " << dmax << " ; angle max = " << angmax << " ; diffcurv max = " << curvmax
     << "\n";

  TopoDS_Face ResFace = TopoDS::Face(MakeFilling.Shape());
  DBRep::Set(a[1], ResFace);

#ifdef OCCT_DEBUG
  Chrono.Stop();
  double Tps;
  Chrono.Show(Tps);
  di << "*** FIN DE FILLING ***\n";
  di << "Temps de calcul  : " << Tps << "\n";
#endif

  // History
  if (BRepTest_Objects::IsHistoryNeeded())
    BRepTest_Objects::SetHistory(ListForHistory, MakeFilling);

  return 0;
}

static int fillingparam(Draw_Interpretor& di, int n, const char** a)
{
  if (n == 1)
  {

    // std::cout << "fillingparam : options are"  <<std::endl;
    // std::cout << "-l : to list current values" << std::endl;
    // std::cout << "-i : to set default values"   << std::endl;
    // std::cout << "-r deg nbPonC nbIt anis : to set filling options" <<std::endl;
    // std::cout << "-c t2d t3d tang tcur : to set tolerances" << std::endl;
    // std::cout << "-a maxdeg maxseg : Approximation option" << std::endl;
    di << "fillingparam : options are" << "\n";
    di << "-l : to list current values\n";
    di << "-i : to set default values" << "\n";
    di << "-r deg nbPonC nbIt anis : to set filling options\n";
    di << "-c t2d t3d tang tcur : to set tolerances\n";
    di << "-a maxdeg maxseg : Approximation option\n";
  }
  else if (n > 1)
  {
    TCollection_AsciiString AS(a[1]);
    AS.LowerCase();
    const char* flag = AS.ToCString();
    if (strcmp(flag, "-l") == 0 && n == 2)
    {
      // std::cout<<std::endl;
      // std::cout<<"Degree = "<<Degree<<std::endl;
      // std::cout<<"NbPtsOnCur = "<<NbPtsOnCur<<std::endl;
      // std::cout<<"NbIter = "<<NbIter<<std::endl;
      // std::cout<<"Anisotropie = "<<Anisotropie<<std::endl<<std::endl;
      //
      // std::cout<<"Tol2d = "<<Tol2d<<std::endl;
      // std::cout<<"Tol3d = "<<Tol3d<<std::endl;
      // std::cout<<"TolAng = "<<TolAng<<std::endl;
      // std::cout<<"TolCurv = "<<TolCurv<<std::endl<<std::endl;
      //
      // std::cout<<"MaxDeg = "<<MaxDeg<<std::endl;
      // std::cout<<"MaxSegments = "<<MaxSegments<<std::endl<<std::endl;
      di << "\n";
      di << "Degree = " << Degree << "\n";
      di << "NbPtsOnCur = " << NbPtsOnCur << "\n";
      di << "NbIter = " << NbIter << "\n";
      di << "Anisotropie = " << (int)Anisotropie << "\n\n";

      di << "Tol2d = " << Tol2d << "\n";
      di << "Tol3d = " << Tol3d << "\n";
      di << "TolAng = " << TolAng << "\n";
      di << "TolCurv = " << TolCurv << "\n\n";

      di << "MaxDeg = " << MaxDeg << "\n";
      di << "MaxSegments = " << MaxSegments << "\n\n";
    }
    else if (strcmp(flag, "-i") == 0 && n == 2)
    {
      Degree      = defDegree;
      NbPtsOnCur  = defNbPtsOnCur;
      NbIter      = defNbIter;
      Anisotropie = defAnisotropie;

      Tol2d   = defTol2d;
      Tol3d   = defTol3d;
      TolAng  = defTolAng;
      TolCurv = defTolCurv;

      MaxDeg      = defMaxDeg;
      MaxSegments = defMaxSegments;
    }
    else if (strcmp(flag, "-r") == 0 && n == 6)
    {
      Degree      = Draw::Atoi(a[2]);
      NbPtsOnCur  = Draw::Atoi(a[3]);
      NbIter      = Draw::Atoi(a[4]);
      Anisotropie = Draw::Atoi(a[5]) != 0;
    }
    else if (strcmp(flag, "-c") == 0 && n == 6)
    {
      Tol2d   = Draw::Atof(a[2]);
      Tol3d   = Draw::Atof(a[3]);
      TolAng  = Draw::Atof(a[4]);
      TolCurv = Draw::Atof(a[5]);
    }
    else if (strcmp(flag, "-a") == 0 && n == 4)
    {
      MaxDeg      = Draw::Atoi(a[2]);
      MaxSegments = Draw::Atoi(a[3]);
    }
    else
    {
      // std::cout<<"Wrong parameters"<<std::endl;
      di << "Wrong parameters\n";
      return 1;
    }
  }
  return 0;
}

void BRepTest::FillingCommands(Draw_Interpretor& theCommands)
{
  static bool done = false;
  if (done)
    return;
  done = true;

  DBRep::BasicCommands(theCommands);
  GeometryTest::SurfaceCommands(theCommands);

  const char* g = "Surface filling topology commands";
  theCommands.Add("plate",
                  "plate result nbrpntoncurve nbrcurfront edge face tang (0:vif;1:tang) ...",
                  __FILE__,
                  plate,
                  g);

  theCommands.Add("gplate",
                  "gplate result nbrcurfront nbrpntconst [SurfInit] [edge 0] [edge tang "
                  "(1:G1;2:G2) surf]... [point] [u v tang (1:G1;2:G2) surf] ...",
                  __FILE__,
                  gplate,
                  g);

  theCommands.Add("approxplate",
                  "approxplate result nbrpntoncurve nbrcurfront edge face tang (0:vif;1:tang) ... "
                  "tol nmax degmax crit",
                  __FILE__,
                  approxplate,
                  g);

  theCommands.Add("filling",
                  "filling result nbB nbC nbP [SurfInit] [edge][face]order... edge[face]order... "
                  "point/u v face order...",
                  __FILE__,
                  filling,
                  g);

  theCommands.Add("fillingparam", "fillingparam : no arg give help", __FILE__, fillingparam, g);
}
