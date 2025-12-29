// Created on: 1996-06-03
// Created by: Laurent BUCHARD
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

#define OPTIMISATION 1

#include <IntCurvesFace_Intersector.hxx>

#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_HSurfaceTool.hxx>
#include <Bnd_BoundSortBox.hxx>
#include <Bnd_Box.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepTopAdaptor_TopolTool.hxx>
#include <Geom_Line.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <IntCurveSurface_HInter.hxx>
#include <IntCurveSurface_IntersectionPoint.hxx>
#include <IntCurveSurface_TheHCurveTool.hxx>
#include <IntCurveSurface_ThePolygonOfHInter.hxx>
#include <IntCurveSurface_ThePolyhedronToolOfHInter.hxx>
#include <Intf_Tool.hxx>
#include <TopoDS_Face.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IntCurvesFace_Intersector, Standard_Transient)

//
static void ComputeSamplePars(const occ::handle<Adaptor3d_Surface>&     Hsurface,
                              const int                                 nbsu,
                              const int                                 nbsv,
                              occ::handle<NCollection_HArray1<double>>& UPars,
                              occ::handle<NCollection_HArray1<double>>& VPars)
{
  int                        NbUInts = Hsurface->NbUIntervals(GeomAbs_C2);
  int                        NbVInts = Hsurface->NbVIntervals(GeomAbs_C2);
  NCollection_Array1<double> UInts(1, NbUInts + 1);
  NCollection_Array1<double> VInts(1, NbVInts + 1);
  Hsurface->UIntervals(UInts, GeomAbs_C2);
  Hsurface->VIntervals(VInts, GeomAbs_C2);
  //
  NCollection_Array1<int> NbUSubInts(1, NbUInts);
  NCollection_Array1<int> NbVSubInts(1, NbVInts);
  //
  int    i, j, ind, NbU, NbV;
  double t, dt;
  t   = UInts(NbUInts + 1) - UInts(1);
  t   = 1. / t;
  NbU = 0;
  for (i = 1; i <= NbUInts; ++i)
  {
    dt            = (UInts(i + 1) - UInts(i));
    NbUSubInts(i) = RealToInt(nbsu * dt * t) + 1;
    NbU += NbUSubInts(i);
  }
  t   = VInts(NbVInts + 1) - VInts(1);
  t   = 1. / t;
  NbV = 0;
  for (i = 1; i <= NbVInts; ++i)
  {
    dt            = (VInts(i + 1) - VInts(i));
    NbVSubInts(i) = RealToInt(nbsv * dt * t) + 1;
    NbV += NbVSubInts(i);
  }
  UPars = new NCollection_HArray1<double>(1, NbU + 1);
  VPars = new NCollection_HArray1<double>(1, NbV + 1);
  //
  ind = 1;
  for (i = 1; i <= NbUInts; ++i)
  {
    UPars->SetValue(ind++, UInts(i));
    dt = (UInts(i + 1) - UInts(i)) / NbUSubInts(i);
    t  = UInts(i);
    for (j = 1; j < NbUSubInts(i); ++j)
    {
      t += dt;
      UPars->SetValue(ind++, t);
    }
  }
  UPars->SetValue(ind, UInts(NbUInts + 1));
  //
  ind = 1;
  for (i = 1; i <= NbVInts; ++i)
  {
    VPars->SetValue(ind++, VInts(i));
    dt = (VInts(i + 1) - VInts(i)) / NbVSubInts(i);
    t  = VInts(i);
    for (j = 1; j < NbVSubInts(i); ++j)
    {
      t += dt;
      VPars->SetValue(ind++, t);
    }
  }
  VPars->SetValue(ind, VInts(NbVInts + 1));
}

//
//=================================================================================================

GeomAbs_SurfaceType IntCurvesFace_Intersector::SurfaceType() const
{
  return (Adaptor3d_HSurfaceTool::GetType(Hsurface));
}

//=================================================================================================

IntCurvesFace_Intersector::IntCurvesFace_Intersector(const TopoDS_Face& Face,
                                                     const double       aTol,
                                                     const bool         aRestr,
                                                     const bool         UseBToler)

    : Tol(aTol),
      done(false),
      myReady(false),
      nbpnt(0),
      myUseBoundTol(UseBToler),
      myIsParallel(false)
{
  BRepAdaptor_Surface surface;
  face = Face;
  surface.Initialize(Face, aRestr);
  Hsurface    = new BRepAdaptor_Surface(surface);
  myTopolTool = new BRepTopAdaptor_TopolTool(Hsurface);

  GeomAbs_SurfaceType SurfaceType = Adaptor3d_HSurfaceTool::GetType(Hsurface);
  if ((SurfaceType != GeomAbs_Plane) && (SurfaceType != GeomAbs_Cylinder)
      && (SurfaceType != GeomAbs_Cone) && (SurfaceType != GeomAbs_Sphere)
      && (SurfaceType != GeomAbs_Torus))
  {
    int    nbsu, nbsv;
    double U0, V0, U1, V1;
    U0 = Hsurface->FirstUParameter();
    U1 = Hsurface->LastUParameter();
    V0 = Hsurface->FirstVParameter();
    V1 = Hsurface->LastVParameter();
    //
    nbsu = myTopolTool->NbSamplesU();
    nbsv = myTopolTool->NbSamplesV();
    //
    double aURes = Hsurface->UResolution(1.0);
    double aVRes = Hsurface->VResolution(1.0);

    // Checking correlation between number of samples and length of the face along each axis
    const double aTresh       = 100.0;
    int          aMinSamples  = 20;
    const int    aMaxSamples  = 40;
    const int    aMaxSamples2 = aMaxSamples * aMaxSamples;
    double       dU           = (U1 - U0) / aURes;
    double       dV           = (V1 - V0) / aVRes;
    if (nbsu < aMinSamples)
      nbsu = aMinSamples;
    if (nbsv < aMinSamples)
      nbsv = aMinSamples;
    if (nbsu > aMaxSamples)
      nbsu = aMaxSamples;
    if (nbsv > aMaxSamples)
      nbsv = aMaxSamples;

    if (dU > Precision::Confusion() && dV > Precision::Confusion())
    {
      if (std::max(dU, dV) > std::min(dU, dV) * aTresh)
      {
        aMinSamples = 10;
        nbsu        = (int)(std::sqrt(dU / dV) * aMaxSamples);
        if (nbsu < aMinSamples)
          nbsu = aMinSamples;
        nbsv = aMaxSamples2 / nbsu;
        if (nbsv < aMinSamples)
        {
          nbsv = aMinSamples;
          nbsu = aMaxSamples2 / aMinSamples;
        }
      }
    }
    else
    {
      return; // surface has no extension along one of directions
    }

    int NbUOnS = Hsurface->NbUIntervals(GeomAbs_C2);
    int NbVOnS = Hsurface->NbVIntervals(GeomAbs_C2);

    if (NbUOnS > 1 || NbVOnS > 1)
    {
      occ::handle<NCollection_HArray1<double>> UPars, VPars;
      ComputeSamplePars(Hsurface, nbsu, nbsv, UPars, VPars);
      myPolyhedron.reset(new IntCurveSurface_ThePolyhedronOfHInter(Hsurface,
                                                                   UPars->ChangeArray1(),
                                                                   VPars->ChangeArray1()));
    }
    else
    {
      myPolyhedron.reset(
        new IntCurveSurface_ThePolyhedronOfHInter(Hsurface, nbsu, nbsv, U0, V0, U1, V1));
    }
  }
  myReady = true;
}

//=================================================================================================

void IntCurvesFace_Intersector::InternalCall(const IntCurveSurface_HInter& HICS,
                                             const double                  parinf,
                                             const double                  parsup)
{
  if (HICS.IsDone() && HICS.NbPoints() > 0)
  {
    // Calculate tolerance for 2d classifier
    double          mintol3d = BRep_Tool::Tolerance(face);
    double          maxtol3d = mintol3d;
    double          mintol2d = Tol, maxtol2d = Tol;
    TopExp_Explorer anExp(face, TopAbs_EDGE);
    for (; anExp.More(); anExp.Next())
    {
      double curtol = BRep_Tool::Tolerance(TopoDS::Edge(anExp.Current()));
      mintol3d      = std::min(mintol3d, curtol);
      maxtol3d      = std::max(maxtol3d, curtol);
    }
    double minres = std::max(Hsurface->UResolution(mintol3d), Hsurface->VResolution(mintol3d));
    double maxres = std::max(Hsurface->UResolution(maxtol3d), Hsurface->VResolution(maxtol3d));
    mintol2d      = std::max(minres, Tol);
    maxtol2d      = std::max(maxres, Tol);
    //
    occ::handle<BRepTopAdaptor_TopolTool> anAdditionalTool;
    for (int index = HICS.NbPoints(); index >= 1; index--)
    {
      const IntCurveSurface_IntersectionPoint& HICSPointindex = HICS.Point(index);
      gp_Pnt2d                                 Puv(HICSPointindex.U(), HICSPointindex.V());

      // TopAbs_State currentstate = myTopolTool->Classify(Puv,Tol);
      TopAbs_State currentstate = myTopolTool->Classify(Puv, !myUseBoundTol ? 0 : mintol2d);
      if (myUseBoundTol && currentstate == TopAbs_OUT && maxtol2d > mintol2d)
      {
        if (anAdditionalTool.IsNull())
        {
          anAdditionalTool = new BRepTopAdaptor_TopolTool(Hsurface);
        }
        currentstate = anAdditionalTool->Classify(Puv, maxtol2d);
        if (currentstate == TopAbs_ON)
        {
          currentstate = TopAbs_OUT;
          // Find out nearest edge and it's tolerance
          anExp.Init(face, TopAbs_EDGE);
          for (; anExp.More(); anExp.Next())
          {
            TopoDS_Edge                 anE = TopoDS::Edge(anExp.Current());
            double                      f, l;
            occ::handle<Geom_Curve>     aPC = BRep_Tool::Curve(anE, f, l);
            GeomAPI_ProjectPointOnCurve aProj(HICSPointindex.Pnt(), aPC, f, l);
            if (aProj.NbPoints() > 0)
            {
              if (aProj.LowerDistance() <= maxtol3d)
              {
                // Nearest edge is found, state is really ON
                currentstate = TopAbs_ON;
                break;
              }
            }
          }
        }
      }
      if (currentstate == TopAbs_IN || currentstate == TopAbs_ON)
      {
        double HICSW = HICSPointindex.W();
        if (HICSW >= parinf && HICSW <= parsup)
        {
          double                            U          = HICSPointindex.U();
          double                            V          = HICSPointindex.V();
          double                            W          = HICSW;
          IntCurveSurface_TransitionOnCurve transition = HICSPointindex.Transition();
          gp_Pnt                            pnt        = HICSPointindex.Pnt();
          //  Modified by skv - Wed Sep  3 16:14:10 2003 OCC578 Begin
          int anIntState = (currentstate == TopAbs_IN) ? 0 : 1;
          //  Modified by skv - Wed Sep  3 16:14:11 2003 OCC578 End

          if (transition != IntCurveSurface_Tangent && face.Orientation() == TopAbs_REVERSED)
          {
            if (transition == IntCurveSurface_In)
              transition = IntCurveSurface_Out;
            else
              transition = IntCurveSurface_In;
          }
          //----- Insertion du point
          if (nbpnt == 0)
          {
            IntCurveSurface_IntersectionPoint PPP(pnt, U, V, W, transition);
            SeqPnt.Append(PPP);
            //  Modified by skv - Wed Sep  3 16:14:10 2003 OCC578 Begin
            mySeqState.Append(anIntState);
            //  Modified by skv - Wed Sep  3 16:14:11 2003 OCC578 End
          }
          else
          {
            int i = 1;
            int b = nbpnt + 1;
            while (i <= nbpnt)
            {
              const IntCurveSurface_IntersectionPoint& Pnti = SeqPnt.Value(i);
              double                                   wi   = Pnti.W();
              if (wi >= W)
              {
                b = i;
                i = nbpnt;
              }
              i++;
            }
            IntCurveSurface_IntersectionPoint PPP(pnt, U, V, W, transition);
            //  Modified by skv - Wed Sep  3 16:14:10 2003 OCC578 Begin
            // 	    if(b>nbpnt)          { SeqPnt.Append(PPP); }
            // 	    else if(b>0)             { SeqPnt.InsertBefore(b,PPP); }
            if (b > nbpnt)
            {
              SeqPnt.Append(PPP);
              mySeqState.Append(anIntState);
            }
            else if (b > 0)
            {
              SeqPnt.InsertBefore(b, PPP);
              mySeqState.InsertBefore(b, anIntState);
            }
            //  Modified by skv - Wed Sep  3 16:14:11 2003 OCC578 End
          }

          nbpnt++;
        }
      } //-- classifier state is IN or ON
    } //-- Loop on Intersection points.
  } //-- HICS.IsDone()
  else if (HICS.IsDone())
  {
    myIsParallel = HICS.IsParallel();
  }
}

//=================================================================================================

void IntCurvesFace_Intersector::Perform(const gp_Lin& L, const double ParMin, const double ParMax)
{
  done = false;
  if (!myReady)
  {
    return;
  }
  done = true;
  SeqPnt.Clear();
  mySeqState.Clear();
  nbpnt = 0;

  IntCurveSurface_HInter         HICS;
  occ::handle<Geom_Line>         geomline = new Geom_Line(L);
  GeomAdaptor_Curve              LL(geomline);
  occ::handle<GeomAdaptor_Curve> HLL    = new GeomAdaptor_Curve(LL);
  double                         parinf = ParMin;
  double                         parsup = ParMax;
  //
  if (!myPolyhedron)
  {
    HICS.Perform(HLL, Hsurface);
  }
  else
  {
    Intf_Tool bndTool;
    Bnd_Box   boxLine;
    bndTool.LinBox(L, myPolyhedron->Bounding(), boxLine);
    if (bndTool.NbSegments() == 0)
      return;
    for (int nbseg = 1; nbseg <= bndTool.NbSegments(); nbseg++)
    {
      double pinf = bndTool.BeginParam(nbseg);
      double psup = bndTool.EndParam(nbseg);
      double pppp = 0.05 * (psup - pinf);
      pinf -= pppp;
      psup += pppp;
      if ((psup - pinf) < 1e-10)
      {
        pinf -= 1e-10;
        psup += 1e-10;
      }
      if (nbseg == 1)
      {
        parinf = pinf;
        parsup = psup;
      }
      else
      {
        if (parinf > pinf)
          parinf = pinf;
        if (parsup < psup)
          parsup = psup;
      }
    }
    if (parinf > ParMax)
    {
      return;
    }
    if (parsup < ParMin)
    {
      return;
    }
    if (parinf < ParMin)
      parinf = ParMin;
    if (parsup > ParMax)
      parsup = ParMax;
    if (parinf > (parsup - 1e-9))
      return;
    IntCurveSurface_ThePolygonOfHInter polygon(HLL, parinf, parsup, 2);
#if OPTIMISATION
    if (!myBndBounding)
    {
      myBndBounding.reset(new Bnd_BoundSortBox());
      myBndBounding->Initialize(
        IntCurveSurface_ThePolyhedronToolOfHInter::Bounding(*myPolyhedron),
        IntCurveSurface_ThePolyhedronToolOfHInter::ComponentsBounding(*myPolyhedron));
    }
    HICS.Perform(HLL, polygon, Hsurface, *myPolyhedron, *myBndBounding);
#else
    HICS.Perform(HLL, polygon, Hsurface, *myPolyhedron);
#endif
  }

  InternalCall(HICS, parinf, parsup);
}

//=================================================================================================

void IntCurvesFace_Intersector::Perform(const occ::handle<Adaptor3d_Curve>& HCu,
                                        const double                        ParMin,
                                        const double                        ParMax)
{
  done = false;
  if (!myReady)
  {
    return;
  }
  done = true;
  SeqPnt.Clear();
  //  Modified by skv - Wed Sep  3 16:14:10 2003 OCC578 Begin
  mySeqState.Clear();
  //  Modified by skv - Wed Sep  3 16:14:11 2003 OCC578 End
  nbpnt = 0;
  IntCurveSurface_HInter HICS;

  //--
  double parinf = ParMin;
  double parsup = ParMax;

  if (!myPolyhedron)
  {
    HICS.Perform(HCu, Hsurface);
  }
  else
  {
    parinf = IntCurveSurface_TheHCurveTool::FirstParameter(HCu);
    parsup = IntCurveSurface_TheHCurveTool::LastParameter(HCu);
    if (parinf < ParMin)
      parinf = ParMin;
    if (parsup > ParMax)
      parsup = ParMax;
    if (parinf > (parsup - 1e-9))
      return;
    int nbs;
    nbs = IntCurveSurface_TheHCurveTool::NbSamples(HCu, parinf, parsup);

    IntCurveSurface_ThePolygonOfHInter polygon(HCu, parinf, parsup, nbs);
#if OPTIMISATION
    if (!myBndBounding)
    {
      myBndBounding.reset(new Bnd_BoundSortBox());
      myBndBounding->Initialize(
        IntCurveSurface_ThePolyhedronToolOfHInter::Bounding(*myPolyhedron),
        IntCurveSurface_ThePolyhedronToolOfHInter::ComponentsBounding(*myPolyhedron));
    }
    HICS.Perform(HCu, polygon, Hsurface, *myPolyhedron, *myBndBounding);
#else
    HICS.Perform(HCu, polygon, Hsurface, *myPolyhedron);
#endif
  }
  InternalCall(HICS, parinf, parsup);
}

//============================================================================
Bnd_Box IntCurvesFace_Intersector::Bounding() const
{
  if (myPolyhedron)
  {
    return myPolyhedron->Bounding();
  }
  else
  {
    Bnd_Box B;
    return B;
  }
}

TopAbs_State IntCurvesFace_Intersector::ClassifyUVPoint(const gp_Pnt2d& Puv) const
{
  TopAbs_State state = myTopolTool->Classify(Puv, 1e-7);
  return state;
}

void IntCurvesFace_Intersector::SetUseBoundToler(bool UseBToler)
{
  myUseBoundTol = UseBToler;
}

bool IntCurvesFace_Intersector::GetUseBoundToler() const
{
  return myUseBoundTol;
}

IntCurvesFace_Intersector::~IntCurvesFace_Intersector() = default;
