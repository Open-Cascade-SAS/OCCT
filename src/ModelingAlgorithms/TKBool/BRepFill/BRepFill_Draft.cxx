// Created on: 1998-06-08
// Created by: Stephanie HUMEAU
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

#include <BRepFill_Draft.hxx>

#include <BndLib_Add3dCurve.hxx>
#include <BndLib_AddSurface.hxx>
#include <BOPAlgo_Builder.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepFill_DraftLaw.hxx>
#include <BRepFill_ShapeLaw.hxx>
#include <BRepFill_Sweep.hxx>
#include <BRepLib_FindSurface.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BRepLib_MakeWire.hxx>
#include <BRepTools.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomFill_LocationDraft.hxx>
#include <GeomLProp_SLProps.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Mat.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <Precision.hxx>
#include <StdFail_NotDone.hxx>
#include <NCollection_Array1.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Wire.hxx>
#include <NCollection_List.hxx>

//=================================================================================================

static void ComputeTrsf(const TopoDS_Wire& W, const gp_Dir& D, Bnd_Box& Box, gp_Trsf& Tf)
{
  // Calculate approximate barycenter
  BRepTools_WireExplorer Exp(W);
  // Class BRep_Tool without fields and without Constructor :
  //  BRep_Tool BT;
  gp_XYZ Bary(0., 0., 0.);
  int    nb;

  for (nb = 0; Exp.More(); Exp.Next())
  {
    //    Bary += BT.Pnt(Exp.CurrentVertex()).XYZ();
    Bary += BRep_Tool::Pnt(Exp.CurrentVertex()).XYZ();
    nb++;
  }
  Bary /= nb;

  // Calculate the Transformation
  gp_Ax3 N(Bary, D);
  Tf.SetTransformation(N);
  BRepAdaptor_Curve AC;
  //  BndLib_Add3dCurve BC;

  // transformation to the wire
  TopoDS_Wire     TheW = W;
  TopLoc_Location Loc(Tf);
  TheW.Location(Loc);

  // Calculate the box
  Box.SetVoid();
  for (Exp.Init(TheW); Exp.More(); Exp.Next())
  {
    AC.Initialize(Exp.Current());
    //    BC.Add(AC, 0.1, Box);
    BndLib_Add3dCurve::Add(AC, 0.1, Box);
  }
}

//=================================================================================================

static double Longueur(const Bnd_Box& WBox, const Bnd_Box& SBox, gp_Dir& D, gp_Pnt& P)
{
  // face of the box most remoted from the face input in
  // the direction of skin
  double Xmin, Ymin, Zmin, Xmax, Ymax, Zmax, WZmin, WZmax, L;

  //"coord" of the box
  WBox.Get(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
  WZmin = Zmin;
  WZmax = Zmax;

  SBox.Get(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
  P.SetCoord((Xmin + Xmax) / 2, (Ymin + Ymax) / 2, Zmax);

  if (Zmax < WZmin)
  {
    // Skin in the wrong direction. Invert...
    D.Reverse();
    L = WZmax - Zmin;
    P.SetZ(Zmin);
  }
  else
  {
    L = Zmax - WZmin;
  }
  return L;
}

//=======================================================================
// function : GoodOrientation
// purpose  : Check if the law is oriented to have an exterior skin
//======================================================================
static bool GoodOrientation(const Bnd_Box&                           B,
                            const occ::handle<BRepFill_LocationLaw>& Law,
                            const gp_Dir&                            D)
{
  double f, l, r, t;
  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;

  B.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
  gp_Pnt P1(aXmin, aYmin, aZmin), P2(aXmax, aYmax, aZmax);
  gp_Vec V(P1, P2);

  Law->CurvilinearBounds(Law->NbLaw(), f, l);
  r = V.Magnitude() / l;

  int ii, Ind;
  // #ifndef OCCT_DEBUG
  int Nb = (int)(4 + (10 * r));
  // #else
  //   int Nb = 4+(10*r);
  // #endif
  r = l / Nb;

  Nb++; // Number of points

  NCollection_Array1<gp_Pnt>   Pnts(1, Nb);
  occ::handle<Adaptor3d_Curve> AC;
  gp_XYZ                       Bary(0., 0., 0.);

  for (ii = 1; ii <= Nb; ii++)
  {
    Law->Parameter((ii - 1) * r, Ind, t);
    AC = Law->Law(Ind)->GetCurve();
    AC->D0(t, Pnts(ii));
    Bary += Pnts(ii).XYZ();
  }

  Bary /= Nb;
  gp_Pnt Centre(Bary);
  gp_Vec Normal(D.XYZ());
  double Angle = 0;
  gp_Vec Ref(Centre, Pnts(1));

  for (ii = 2; ii <= Nb; ii++)
  {
    gp_Vec R(Centre, Pnts(ii));
    Angle += Ref.AngleWithRef(R, Normal);
    Ref = R;
  }

  return (Angle >= 0);
}

//=================================================================================================

BRepFill_Draft::BRepFill_Draft(const TopoDS_Shape& S, const gp_Dir& Dir, const double Angle)
{
  myLoc.Nullify();
  mySec.Nullify();
  myFaces.Nullify();
  mySections.Nullify();

  switch (S.ShapeType())
  {
    case TopAbs_WIRE: {
      myWire = TopoDS::Wire(S);
      break;
    }
    case TopAbs_FACE: {
      TopoDS_Iterator Exp(S);
      myWire = TopoDS::Wire(Exp.Value());
      break;
    }
    case TopAbs_SHELL: {
      NCollection_List<TopoDS_Shape> List;
      NCollection_IndexedDataMap<TopoDS_Shape,
                                 NCollection_List<TopoDS_Shape>,
                                 TopTools_ShapeMapHasher>
        edgemap;
      TopExp::MapShapesAndAncestors(S, TopAbs_EDGE, TopAbs_FACE, edgemap);
      int iedge, nbf;
      for (iedge = 1; iedge <= edgemap.Extent(); iedge++)
      {
        const TopoDS_Edge& theEdge = TopoDS::Edge(edgemap.FindKey(iedge));
        // skip degenerated edges
        if (!BRep_Tool::Degenerated(theEdge))
        {
          nbf = edgemap(iedge).Extent();
          if (nbf == 1)
            List.Append(theEdge);
        }
      }

      if (List.Extent() > 0)
      {
        BRepLib_MakeWire MW;
        MW.Add(List);
        BRepLib_WireError Err = MW.Error();
        if (Err == BRepLib_WireDone)
        {
          myWire = MW.Wire();
        }
        else
        {
#ifdef OCCT_DEBUG
          std::cout << "Error in MakeWire" << std::endl;
#endif
          throw Standard_ConstructionError("BRepFill_Draft");
        }
      }
      else
      {
#ifdef OCCT_DEBUG
        std::cout << "No Free Borders !" << std::endl;
#endif
        throw Standard_ConstructionError("BRepFill_Draft");
      }
      break;
    }
    default:
      throw Standard_ConstructionError("BRepFill_Draft");
  }

  // Attention to closed non declared wires !
  if (!myWire.Closed())
  {
    TopoDS_Vertex Vf, Vl;
    TopExp::Vertices(myWire, Vf, Vl);
    if (Vf.IsSame(Vl))
      myWire.Closed(true);
  }

  myAngle = std::abs(Angle);
  myDir   = Dir;
  myTop   = S;
  myDone  = false;
  myTol   = 1.e-4;
  myCont  = GeomAbs_C1;
  SetOptions();
  SetDraft();
}

//=================================================================================================

void BRepFill_Draft::SetOptions(const BRepFill_TransitionStyle Style,
                                const double                   Min,
                                const double                   Max)
{
  myStyle = Style;
  angmin  = Min;
  angmax  = Max;
}

//=================================================================================================

void BRepFill_Draft::SetDraft(const bool Internal)
{
  IsInternal = Internal;
}

//=======================================================================
// function :Perform
// purpose  : calculate a surface of skinning
//======================================================================
void BRepFill_Draft::Perform(const double LengthMax)
{
  occ::handle<Geom_Surface> S;
  S.Nullify();
  Bnd_Box WBox; //, SBox;
  gp_Trsf Trsf;

  ComputeTrsf(myWire, myDir, WBox, Trsf);
  Init(S, LengthMax, WBox);
  BuildShell(S);
  Sewing();
}

//=======================================================================
// function :Perform
// purpose  : calculate a surface of skinning
//======================================================================
void BRepFill_Draft::Perform(const occ::handle<Geom_Surface>& Surface, const bool KeepInsideSurface)
{
  Bnd_Box WBox, SBox;
  gp_Trsf Trsf;
  gp_Pnt  Pt;
  double  L;

  ComputeTrsf(myWire, myDir, WBox, Trsf);

  // box with bounds of the stop surface
  occ::handle<Geom_Surface> Surf;
  Surf = occ::down_cast<Geom_Surface>(Surface->Transformed(Trsf));
  GeomAdaptor_Surface S1(Surf);
  //  BndLib_AddSurface AS;
  //  AS.Add(S1, 0.1, SBox);
  BndLib_AddSurface::Add(S1, 0.1, SBox);

  // calculate the maximum length of the rule.
  L = Longueur(WBox, SBox, myDir, Pt);
  L /= std::abs(std::cos(myAngle));

  // Construction
  Init(Surface, L, WBox);
  BuildShell(Surface, !KeepInsideSurface);
  Sewing();
}

//================================================================
// function :Perform
// purpose  : calculate the surface of skinning, stopped by a shape
//================================================================
void BRepFill_Draft::Perform(const TopoDS_Shape& StopShape, const bool KeepOutSide)
{
  Bnd_Box WBox, SBox;
  gp_Trsf Trsf;
  gp_Pnt  Pt;
  double  L;

  ComputeTrsf(myWire, myDir, WBox, Trsf);

  // bounding box of the stop shape
  Bnd_Box BSurf; //, TheBox;
  double  Umin, Umax, Vmin, Vmax;
  //  BRepTools B;
  //  BRep_Tool BT;
  occ::handle<Geom_Surface> Surf;

  //  BndLib_AddSurface AS;

  TopExp_Explorer Ex(StopShape, TopAbs_FACE);

  SBox.SetVoid();
  while (Ex.More())
  { // parse faces of the stop shape
    //    B.UVBounds(TopoDS::Face(Ex.Current()), Umin,Umax,Vmin,Vmax);
    BRepTools::UVBounds(TopoDS::Face(Ex.Current()), Umin, Umax, Vmin, Vmax);
    Surf = occ::down_cast<Geom_Surface>(
      //     BT.Surface(TopoDS::Face(Ex.Current()))->Transformed(Trsf) );
      BRep_Tool::Surface(TopoDS::Face(Ex.Current()))->Transformed(Trsf));
    GeomAdaptor_Surface S1(Surf);
    // bounding box of the current face
    //    AS.Add(S1, Umin, Umax, Vmin, Vmax, 0.1, BSurf);
    BndLib_AddSurface::Add(S1, Umin, Umax, Vmin, Vmax, 0.1, BSurf);
    SBox.Add(BSurf); // group boxes
    Ex.Next();
  } // while_Ex

  // calculate the maximum length of the rule.
  L = Longueur(WBox, SBox, myDir, Pt);
  L /= std::abs(std::cos(myAngle));

  // surface of stop
  gp_Trsf Inv;
  Inv = Trsf.Inverted(); // inverted transformation
  Pt.Transform(Inv);     // coordinate in the absolute reference
  occ::handle<Geom_Plane> Plan = new (Geom_Plane)(Pt, myDir);
  Surf                         = new (Geom_RectangularTrimmedSurface)(Plan, -L, L, -L, L);

  // Sweeping and restriction
  Init(Plan, L * 1.01, WBox);
  BuildShell(Surf, false);
  Fuse(StopShape, KeepOutSide);
  Sewing();
}

//=================================================================================================

void BRepFill_Draft::Init(const occ::handle<Geom_Surface>&, const double Length, const Bnd_Box& Box)
{
  bool B;

  // law of positioning
  occ::handle<GeomFill_LocationDraft> Loc = new (GeomFill_LocationDraft)(myDir, myAngle);
  myLoc                                   = new (BRepFill_DraftLaw)(myWire, Loc);

  B = GoodOrientation(Box, myLoc, myDir);

  if (IsInternal ^ (!B))
  {
    myAngle = -myAngle;
    Loc->SetAngle(myAngle);
    myLoc = new (BRepFill_DraftLaw)(myWire, Loc);
  }

  myLoc->CleanLaw(angmin); // Clean small discontinuities.

  // law of section
  // generating line is straight and parallel to binormal.
  gp_Pnt P(0, 0, 0);
  gp_Vec D(0., 1., 0.);

  // Control of the orientation
  double f, l;
  myLoc->Law(1)->GetDomain(f, l);
  gp_Mat M;

  gp_Vec Bid;
  myLoc->Law(1)->D0((f + l) / 2, M, Bid);
  gp_Dir BN(M.Column(2));

  double ang = myDir.Angle(BN);
  if (ang > M_PI / 2)
    D.Reverse();
  occ::handle<Geom_Line> L = new (Geom_Line)(P, D);

  occ::handle<Geom_Curve> TC = new (Geom_TrimmedCurve)(L, 0, Length);

  BRepLib_MakeEdge ME(TC);
  TopoDS_Edge      EG = ME.Edge();

  BRepLib_MakeWire MW(EG);
  TopoDS_Wire      G = MW.Wire();

  mySec = new (BRepFill_ShapeLaw)(G, true);
}

//=======================================================================
// function : BuildShell
// purpose  : Construction of the skinning surface
//======================================================================
void BRepFill_Draft::BuildShell(const occ::handle<Geom_Surface>& Surf, const bool KeepOutSide)
{
  // construction of the surface
  BRepFill_Sweep Sweep(mySec, myLoc, true);
  Sweep.SetTolerance(myTol);
  Sweep.SetAngularControl(angmin, angmax);
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> Dummy;
  NCollection_DataMap<TopoDS_Shape,
                      occ::handle<NCollection_HArray2<TopoDS_Shape>>,
                      TopTools_ShapeMapHasher>
    Dummy2;
  NCollection_DataMap<TopoDS_Shape,
                      occ::handle<NCollection_HArray2<TopoDS_Shape>>,
                      TopTools_ShapeMapHasher>
    Dummy3;
  Sweep.Build(Dummy, Dummy2, Dummy3, myStyle, myCont);
  if (Sweep.IsDone())
  {
    myShape    = Sweep.Shape();
    myShell    = TopoDS::Shell(myShape);
    myFaces    = Sweep.SubShape();
    mySections = Sweep.Sections();
    myDone     = true;
    // Control of the orientation
    bool            out = true;
    TopExp_Explorer ex(myShell, TopAbs_FACE);
    TopoDS_Face     F;
    F = TopoDS::Face(ex.Current());
    BRepAdaptor_Surface SF(F);
    double              u, v;
    gp_Pnt              P;
    gp_Vec              V1, V2, V;
    u = SF.FirstUParameter();
    v = SF.FirstVParameter();
    SF.D1(u, v, P, V1, V2);
    V = V1.Crossed(V2);
    if (F.Orientation() == TopAbs_REVERSED)
      V.Reverse();
    if (V.Magnitude() > 1.e-10)
    {
      out = myDir.Angle(V) > M_PI / 2;
    }
    if (out == IsInternal)
    {
      myShell.Reverse();
      myShape.Reverse();
    }
  }
  else
  {
    myDone = false;
    return;
  }

  if (!Surf.IsNull())
  { // Add the face at end

    // Waiting the use of traces & restriction in BRepFill_Sweep
    // Make Fuse.
    BRepLib_MakeFace MkF;
    MkF.Init(Surf, true, Precision::Confusion());
    Fuse(MkF.Face(), KeepOutSide);
  }
}

//=======================================================================
// function : Fuse
// purpose  : Boolean operation between the skin and the
//           stop shape
//======================================================================
bool BRepFill_Draft::Fuse(const TopoDS_Shape& StopShape, const bool KeepOutSide)
{
  BRep_Builder B;
  bool         issolid = false;
  TopoDS_Solid Sol1, Sol2;
  TopAbs_State State1 = TopAbs_OUT, State2 = TopAbs_OUT;

  if (myShape.ShapeType() == TopAbs_SOLID)
  {
    Sol1    = TopoDS::Solid(myShape);
    issolid = true;
  }
  else
  {
    B.MakeSolid(Sol1);
    B.Add(Sol1, myShape); // shell => solid (for fusion)
  }

  switch (StopShape.ShapeType())
  {
    case TopAbs_COMPOUND: {
      TopoDS_Iterator It(StopShape);
      return Fuse(It.Value(), KeepOutSide);
    }
    case TopAbs_SOLID: {
      Sol2 = TopoDS::Solid(StopShape);
      break;
    }
    case TopAbs_SHELL: {
      B.MakeSolid(Sol2);
      B.Add(Sol2, StopShape); // shell => solid (for fusion)
      break;
    }

    case TopAbs_FACE: {
      TopoDS_Shell S;
      B.MakeShell(S);
      B.Add(S, StopShape);
      S.Closed(BRep_Tool::IsClosed(S));
      B.MakeSolid(Sol2);
      B.Add(Sol2, S); // shell => solid (for fusion)
      break;
    }

    default: {
      return false; // Impossible to do
    }
  }

  // Perform intersection of solids
  BOPAlgo_PaveFiller             aPF;
  NCollection_List<TopoDS_Shape> anArgs;
  anArgs.Append(Sol1);
  anArgs.Append(Sol2);
  aPF.SetArguments(anArgs);
  aPF.Perform();
  if (aPF.HasErrors())
    return false;

  BRepAlgoAPI_Section aSec(Sol1, Sol2, aPF);
  const TopoDS_Shape& aSection = aSec.Shape();

  TopExp_Explorer exp(aSection, TopAbs_EDGE);
  if (!exp.More())
    // No section edges produced
    return false;

  if (StopShape.ShapeType() != TopAbs_SOLID)
  {
    // It is required to choose the state by the geometry

    // We need to find the section edge, closest to myWire
    TopoDS_Edge                aSEMin;
    double                     Dmin = Precision::Infinite();
    BRepExtrema_DistShapeShape DistTool;
    DistTool.LoadS1(myWire);

    for (; exp.More(); exp.Next())
    {
      const TopoDS_Shape& aSE = exp.Current();
      DistTool.LoadS2(aSE);
      DistTool.Perform();
      if (DistTool.IsDone())
      {
        double D = DistTool.Value();
        if (D < Dmin)
        {
          Dmin   = D;
          aSEMin = TopoDS::Edge(aSE);
          if (Dmin < Precision::Confusion())
            break;
        }
      }
    }

    if (!aSEMin.IsNull())
    {
      // Get geometry of StopShape
      occ::handle<Geom_Surface> S;
      occ::handle<Geom2d_Curve> C2d;
      gp_Pnt2d                  P2d;
      double                    f, l;
      TopLoc_Location           L;
      BRep_Tool::CurveOnSurface(aSEMin, C2d, S, L, f, l, 2);

      // Find a normal.
      C2d->D0((f + l) / 2, P2d);
      GeomLProp_SLProps SP(S, P2d.X(), P2d.Y(), 1, 1.e-12);
      if (!SP.IsNormalDefined())
      {
        C2d->D0((3 * f + l) / 4, P2d);
        SP.SetParameters(P2d.X(), P2d.Y());
        if (!SP.IsNormalDefined())
        {
          C2d->D0((f + 3 * l) / 4, P2d);
          SP.SetParameters(P2d.X(), P2d.Y());
        }
      }

      if (SP.IsNormalDefined())
      {
        // Subtract State1
        if (myDir.Angle(SP.Normal()) < M_PI / 2)
          State1 = TopAbs_IN;
        else
          State1 = TopAbs_OUT;
      }
    }
  }

  if (!KeepOutSide)
  { // Invert State2;
    if (State2 == TopAbs_IN)
      State2 = TopAbs_OUT;
    else
      State2 = TopAbs_IN;
  }

  // Perform Boolean operation
  BOPAlgo_Builder aBuilder;
  aBuilder.AddArgument(Sol1);
  aBuilder.AddArgument(Sol2);
  aBuilder.PerformWithFiller(aPF);
  if (aBuilder.HasErrors())
    return false;

  TopoDS_Shape                   result;
  occ::handle<BRepTools_History> aHistory = new BRepTools_History;

  bool isSingleOpNeeded = true;
  // To get rid of the unnecessary parts of first solid make the cutting first
  if (State1 == TopAbs_OUT)
  {
    NCollection_List<TopoDS_Shape> aLO, aLT;
    aLO.Append(Sol1);
    aLT.Append(Sol2);
    aBuilder.BuildBOP(aLO, aLT, BOPAlgo_CUT, Message_ProgressRange());
    if (!aBuilder.HasErrors())
    {
      TopoDS_Solid    aCutMin;
      TopExp_Explorer anExpS(aBuilder.Shape(), TopAbs_SOLID);
      if (anExpS.More())
      {
        aCutMin = TopoDS::Solid(anExpS.Current());
        anExpS.Next();
        if (anExpS.More())
        {
          double                     aDMin = Precision::Infinite();
          BRepExtrema_DistShapeShape DistTool;
          DistTool.LoadS1(myWire);

          for (anExpS.ReInit(); anExpS.More(); anExpS.Next())
          {
            const TopoDS_Shape& aCut = anExpS.Current();
            DistTool.LoadS2(aCut);
            DistTool.Perform();
            if (DistTool.IsDone())
            {
              double D = DistTool.Value();
              if (D < aDMin)
              {
                aDMin   = D;
                aCutMin = TopoDS::Solid(aCut);
              }
            }
          }
        }
      }

      if (!aCutMin.IsNull())
      {
        // Save history for first argument only
        aHistory->Merge(aLO, aBuilder);

        // Perform needed operation with result of Cut
        BOPAlgo_Builder aGluer;
        aGluer.AddArgument(aCutMin);
        aGluer.AddArgument(Sol2);
        aGluer.SetGlue(BOPAlgo_GlueShift);
        aGluer.Perform();

        aLO.Clear();
        aLO.Append(aCutMin);
        aGluer.BuildBOP(aLO, State1, aLT, State2, Message_ProgressRange());

        if (!aGluer.HasErrors())
        {
          aHistory->Merge(aGluer.History());

          result = aGluer.Shape();
          anExpS.Init(result, TopAbs_SOLID);
          isSingleOpNeeded = !anExpS.More();
        }
      }
    }
  }

  if (isSingleOpNeeded)
  {
    aHistory->Clear();

    NCollection_List<TopoDS_Shape> aLO, aLT;
    aLO.Append(Sol1);
    aLT.Append(Sol2);

    aBuilder.BuildBOP(aLO, State1, aLT, State2, Message_ProgressRange());
    if (aBuilder.HasErrors())
      return false;

    aHistory->Merge(aBuilder.History());
    result = aBuilder.Shape();
  }

  if (issolid)
    myShape = result;
  else
  {
    TopExp_Explorer Exp;
    Exp.Init(result, TopAbs_SHELL);
    if (Exp.More())
      myShape = Exp.Current();
  }

  // Update the History
  int ii;
  for (ii = 1; ii <= myLoc->NbLaw(); ii++)
  {
    const NCollection_List<TopoDS_Shape>& L = aHistory->Modified(myFaces->Value(1, ii));
    if (L.Extent() > 0)
      myFaces->SetValue(1, ii, L.First());
  }
  for (ii = 1; ii <= myLoc->NbLaw() + 1; ii++)
  {
    const NCollection_List<TopoDS_Shape>& L = aHistory->Modified(mySections->Value(1, ii));
    if (L.Extent() > 0)
      mySections->SetValue(1, ii, L.First());
  }

  return true;
}

//=======================================================================
// function : Sewing
// purpose  : Assemble the skin with the above face
//======================================================================
bool BRepFill_Draft::Sewing()
{
  bool ToAss;
  bool Ok = false;
  ToAss   = (myTop.ShapeType() != TopAbs_WIRE);

  if ((!ToAss) || (!myDone))
    return false;

  // Assembly make a shell from the faces of the shape + the input shape
  occ::handle<BRepBuilderAPI_Sewing> Ass = new BRepBuilderAPI_Sewing(5 * myTol, true, true, false);
  Ass->Add(myShape);
  Ass->Add(myTop);
  ToAss = true;

  int NbCE;

  Ass->Perform();
  // Check if the assembly is real.
  NbCE = Ass->NbContigousEdges();

  if (NbCE > 0)
  {
    TopoDS_Shape res;
    res = Ass->SewedShape();
    if ((res.ShapeType() == TopAbs_SHELL) || (res.ShapeType() == TopAbs_SOLID))
    {
      myShape = res;
      Ok      = true;
    }
    else if (res.ShapeType() == TopAbs_COMPOUND)
    {
      TopoDS_Iterator It(res);
      res = It.Value();
      It.Next();
      if (!It.More())
      { // Only one part => this is correct
        myShape = res;
        Ok      = true;
      }
    }
  }

  if (Ok)
  {
    // Update the History
    int ii;
    for (ii = 1; ii <= myLoc->NbLaw(); ii++)
    {
      if (Ass->IsModified(myFaces->Value(1, ii)))
        myFaces->SetValue(1, ii, Ass->Modified(myFaces->Value(1, ii)));
    }
    for (ii = 1; ii <= myLoc->NbLaw() + 1; ii++)
    {
      if (Ass->IsModified(mySections->Value(1, ii)))
        mySections->SetValue(1, ii, Ass->Modified(mySections->Value(1, ii)));
    }

    if (myShape.Closed())
    { // Make a Solid
      TopoDS_Solid solid;
      BRep_Builder BS;
      BS.MakeSolid(solid);
      BS.Add(solid, TopoDS::Shell(myShape));

      BRepClass3d_SolidClassifier SC(solid);
      SC.PerformInfinitePoint(Precision::Confusion());
      if (SC.State() == TopAbs_IN)
      {
        BS.MakeSolid(solid);
        myShape.Reverse();
        BS.Add(solid, TopoDS::Shell(myShape));
      }
      myShape = solid;
    }
  }
#ifdef OCCT_DEBUG
  else
    std::cout << "Draft : No assembly !" << std::endl;
#endif
  return Ok;
}

//=======================================================================
// function : Generated
// purpose  : return a sub-part generated by sweeping
//======================================================================
const NCollection_List<TopoDS_Shape>& BRepFill_Draft::Generated(const TopoDS_Shape& S)
{
  myGenerated.Clear();
  TopoDS_Edge E;
  int         ii;
  E = TopoDS::Edge(S);
  if (E.IsNull())
  {
    for (ii = 0; ii <= myLoc->NbLaw(); ii++)
      if (E.IsSame(myLoc->Vertex(ii)))
      {
        myGenerated.Append(mySections->Value(1, ii + 1));
        break;
      }
  }
  else
  {
    for (ii = 1; ii <= myLoc->NbLaw(); ii++)
      if (E.IsSame(myLoc->Edge(ii)))
      {
        myGenerated.Append(myFaces->Value(1, ii));
        break;
      }
  }

  return myGenerated;
}

//=======================================================================
// function : Shape
// purpose  : return the complete shape
//======================================================================
TopoDS_Shape BRepFill_Draft::Shape() const
{
  return myShape;
}

//=====================================================================
// function : Shell
// purpose  : surface of skinning with the input face (=>shell)
//=====================================================================
TopoDS_Shell BRepFill_Draft::Shell() const
{
  return myShell;
}

//=================================================================================================

bool BRepFill_Draft::IsDone() const
{
  return myDone;
}
