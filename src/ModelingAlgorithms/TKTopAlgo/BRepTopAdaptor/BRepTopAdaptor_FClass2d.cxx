// Created on: 1995-03-22
// Created by: Laurent BUCHARD
// Copyright (c) 1995-1999 Matra Datavision
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

#define No_Standard_OutOfRange

#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <BRepClass_FaceExplorer.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <CSLib_Class2d.hxx>
#include <ElCLib.hxx>
#include <Geom2dInt_Geom2dCurveTool.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <NCollection_LinearVector.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

#include <cmath>

#ifdef _MSC_VER
  #include <stdio.h>
#endif

namespace
{
// Increments @p theValue by @p theIncrement towards @p theDirection, ensuring that the result is
// different from @p theValue. For large values of theValue with small theIncrement the result of
// theValue + theIncrement can be equal to theValue due to the limited resolution of double
// precision. This function guarantees to return the next representable value in the direction of
// theDirection in such cases.
inline double safeIncrement(const double theValue,
                            const double theDirection,
                            const double theIncrement)
{
  const double aNextValue = theValue + theIncrement;
  return aNextValue == theValue ? std::nextafter(theValue, theDirection) : aNextValue;
}

//=================================================================================================

// Checks whether the curve is degenerated between theStartParam and theEndParam.
// The check is performed by sampling the curve at several points and measuring the distance to the
// start point. If all sampled points are closer than Precision::Confusion() to the start point, the
// curve is considered degenerated.
bool isDegenerated(const BRepAdaptor_Curve& theCurve,
                   const double             theStartParam,
                   const double             theEndParam)
{
  const double aParametricStep = (theEndParam - theStartParam) * 0.1;
  const gp_Pnt aStartPoint     = theCurve.Value(theStartParam);
  for (double aCurrParam = theStartParam; aCurrParam < theEndParam;
       aCurrParam        = safeIncrement(aCurrParam, theEndParam, aParametricStep))
  {
    const gp_Pnt aCurrentPoint = theCurve.Value(aCurrParam);
    if (aStartPoint.SquareDistance(aCurrentPoint) > Precision::Confusion())
    {
      return false;
    }
  }

  return true;
}

//=================================================================================================

struct PolygonMetrics
{
  double Area      = 0.0;
  double Perimeter = 0.0;
};

PolygonMetrics polygonMetrics(const NCollection_LinearVector<gp_Pnt2d>& thePoints)
{
  PolygonMetrics aMetrics;
  if (thePoints.Size() < 2)
  {
    return aMetrics;
  }

  const size_t aLastUnique = thePoints.Size() - 2;
  size_t       aPrevious   = aLastUnique;
  for (size_t aCurrent = 0; aCurrent <= aLastUnique; ++aCurrent)
  {
    const gp_Pnt2d& aCurrentPoint  = thePoints[aCurrent];
    const gp_Pnt2d& aPreviousPoint = thePoints[aPrevious];
    aMetrics.Area +=
      (aCurrentPoint.X() - aPreviousPoint.X()) * (aCurrentPoint.Y() + aPreviousPoint.Y()) * 0.5;
    aMetrics.Perimeter += (aCurrentPoint.XY() - aPreviousPoint.XY()).Modulus();
    aPrevious = aCurrent;
  }
  return aMetrics;
}

//=================================================================================================

bool expectedThickness(const PolygonMetrics& theMetrics, double& theThickness)
{
  if (std::isnan(theMetrics.Area) || Precision::IsInfinite(theMetrics.Area)
      || std::isnan(theMetrics.Perimeter) || Precision::IsInfinite(theMetrics.Perimeter)
      || theMetrics.Perimeter <= 0.0)
  {
    return false;
  }

  theThickness = std::max(2.0 * std::abs(theMetrics.Area) / theMetrics.Perimeter, 1.e-7);
  return !std::isnan(theThickness) && !Precision::IsInfinite(theThickness);
}
} // namespace

BRepTopAdaptor_FClass2d::BRepTopAdaptor_FClass2d(const TopoDS_Face& aFace, const double TolUV)
    : Toluv(TolUV),
      Face(aFace),
      myIsUPeriodic(false),
      myIsVPeriodic(false),
      myUPeriod(0.0),
      myVPeriod(0.0),
      U1(0.0),
      V1(0.0),
      U2(0.0),
      V2(0.0)
{
  //-- dead end on surfaces defined on more than one period

  Face.Orientation(TopAbs_FORWARD);
  BRepAdaptor_Surface aSurface(aFace, false);
  myIsUPeriodic = aSurface.IsUPeriodic();
  myIsVPeriodic = aSurface.IsVPeriodic();
  myUPeriod     = myIsUPeriodic ? aSurface.UPeriod() : 0.0;
  myVPeriod     = myIsVPeriodic ? aSurface.VPeriod() : 0.0;

  TopoDS_Edge            edge;
  TopAbs_Orientation     Or;
  BRepTools_WireExplorer WireExplorer;

  Umin = Vmin = 0.0; // RealLast();
  Umax = Vmax = -Umin;

  int              aNbE        = 0;
  constexpr double eps         = 1.e-10;
  bool             anIsBadWire = false;
  for (TopExp_Explorer aFaceExplorer(Face, TopAbs_WIRE); (aFaceExplorer.More() && !anIsBadWire);
       aFaceExplorer.Next())
  {
    int                                nbpnts = 0;
    NCollection_LinearVector<gp_Pnt2d> SeqPnt2d;
    int                                firstpoint     = 1;
    double                             FlecheU        = 0.0;
    double                             FlecheV        = 0.0;
    bool                               WireIsNotEmpty = false;
    int                                NbEdges        = 0;

    TopExp_Explorer Explorer;
    for (Explorer.Init(aFaceExplorer.Current(), TopAbs_EDGE); Explorer.More(); Explorer.Next())
    {
      NbEdges++;
    }
    aNbE = NbEdges;

    gp_Pnt Ancienpnt3d(0, 0, 0);
    bool   Ancienpnt3dinitialise = false;

    for (WireExplorer.Init(TopoDS::Wire(aFaceExplorer.Current()), Face); WireExplorer.More();
         WireExplorer.Next())
    {
      NbEdges--;
      edge = WireExplorer.Current();
      Or   = edge.Orientation();
      if (Or == TopAbs_FORWARD || Or == TopAbs_REVERSED)
      {
        double pfbid, plbid;
        if (BRep_Tool::CurveOnSurface(edge, Face, pfbid, plbid).IsNull())
        {
          return;
        }

        if (std::abs(plbid - pfbid) < 1.e-9)
        {
          continue;
        }

        bool degenerated = false;
        if (BRep_Tool::Degenerated(edge))
        {
          degenerated = true;
        }
        if (BRep_Tool::IsClosed(edge, Face))
        {
          degenerated = true;
        }
        TopoDS_Vertex Va, Vb;
        TopExp::Vertices(edge, Va, Vb);
        if (Va.IsNull() || Vb.IsNull())
        {
          degenerated = true;
        }

        const BRepAdaptor_Curve2d aCurveAdaptor2D(edge, Face);
        const BRepAdaptor_Curve   aCurveAdaptor3D(edge, Face);

        //-- Check cases when it was forgotten to code degenerated :  PRO17410 (janv 99)
        if (!degenerated)
        {
          degenerated = isDegenerated(aCurveAdaptor3D, pfbid, plbid);
        }

        //-- ----------------------------------------

        // int nbs = 1 + Geom2dInt_Geom2dCurveTool::NbSamples(C);
        int nbs = Geom2dInt_Geom2dCurveTool::NbSamples(aCurveAdaptor2D);
        //-- Attention to rational bsplines of degree 3. (ends of circles among others)
        if (nbs > 2)
        {
          nbs *= 4;
        }
        double du = (plbid - pfbid) / (double)(nbs - 1);
        double u  = 0.0;
        if (Or == TopAbs_FORWARD)
        {
          u = pfbid;
        }
        else
        {
          u  = plbid;
          du = -du;
        }

        //-- ------------------------------------------------------------
        //-- Check distance uv between the start point of the edge
        //-- and the last point registered in SeqPnt2d
        //-- Try to remote the first point of the current edge
        //-- from the last saved point
        if (firstpoint == 2)
        {
          u += du;
        }
        int Avant = nbpnts;
        for (int e = firstpoint; e <= nbs; e++)
        {
          gp_Pnt2d P2d = aCurveAdaptor2D.Value(u);
          if (P2d.X() < Umin)
          {
            Umin = P2d.X();
          }
          if (P2d.X() > Umax)
          {
            Umax = P2d.X();
          }
          if (P2d.Y() < Vmin)
          {
            Vmin = P2d.Y();
          }
          if (P2d.Y() > Vmax)
          {
            Vmax = P2d.Y();
          }

          double dist3dptcourant_ancienpnt = 1e+20; // RealLast();
          gp_Pnt P3d;
          if (!degenerated)
          {
            P3d = aCurveAdaptor3D.Value(u);
            if (nbpnts > 1 && Ancienpnt3dinitialise)
            {
              dist3dptcourant_ancienpnt = P3d.Distance(Ancienpnt3d);
            }
          }
          bool IsRealCurve3d = true; // patch
          if (dist3dptcourant_ancienpnt < Precision::Confusion())
          {
            gp_Pnt MidP3d = aCurveAdaptor3D.Value(u - du / 2.);
            if (P3d.Distance(MidP3d) < Precision::Confusion())
            {
              IsRealCurve3d = false;
            }
          }
          if (IsRealCurve3d)
          {
            if (!degenerated)
            {
              Ancienpnt3d           = P3d;
              Ancienpnt3dinitialise = true;
            }
            nbpnts++;
            SeqPnt2d.Append(P2d);
          }

          u += du;
          int ii = nbpnts;
          //-- printf("\n nbpnts:%4d  u=%7.5g   FlecheU=%7.5g  FlecheV=%7.5g  ii=%3d  Avant=%3d
          //",nbpnts,u,FlecheU,FlecheV,ii,Avant);
          // 		  if(ii>(Avant+4))
          //  Modified by Sergey KHROMOV - Fri Apr 19 09:46:12 2002 Begin
          if (ii > (Avant + 4) && SeqPnt2d[ii - 3].SquareDistance(SeqPnt2d[ii - 1]))
          //  Modified by Sergey KHROMOV - Fri Apr 19 09:46:13 2002 End
          {
            gp_Lin2d Lin(SeqPnt2d[ii - 3], gp_Dir2d(gp_Vec2d(SeqPnt2d[ii - 3], SeqPnt2d[ii - 1])));
            double   ul = ElCLib::Parameter(Lin, SeqPnt2d[ii - 2]);
            gp_Pnt2d Pp = ElCLib::Value(ul, Lin);
            double   dU = std::abs(Pp.X() - SeqPnt2d[ii - 2].X());
            double   dV = std::abs(Pp.Y() - SeqPnt2d[ii - 2].Y());
            //-- printf(" (du=%7.5g   dv=%7.5g)",dU,dV);
            if (dU > FlecheU)
            {
              FlecheU = dU;
            }
            if (dV > FlecheV)
            {
              FlecheV = dV;
            }
          }
        } // for(e=firstpoint
        if (firstpoint == 1)
        {
          firstpoint = 2;
        }
        WireIsNotEmpty = true;
      } // if(Or==FORWARD,REVERSED
    } //-- Edges -> for(Ware.Explorer

    if (NbEdges)
    { //-- on compte ++ with a normal explorer and with the Wire Explorer
      NCollection_LinearVector<gp_Pnt2d> aPoints(2, gp_Pnt2d(0.0, 0.0));
      TabClass.Append(CSLib_Class2d(aPoints.ToArray1(), FlecheU, FlecheV, Umin, Vmin, Umax, Vmax));
      anIsBadWire = true;
      TabOrien.Append(WireRole::Invalid);
    }
    else if (WireIsNotEmpty)
    {
      // double anglep=0,anglem=0;
      double aSignedArea = 0.0;

      //-------------------------------------------------------------------
      //-- ** The mode of calculation was somewhat changed
      //-- Before Oct 31 97 , the total angle of
      //-- rotation of the wire was evaluated on all angles except for the last
      //-- ** Now, exactly the angle of rotation is evaluated
      //-- If a value remote from 2PI or -2PI is found, it means that there is
      //-- an uneven number of loops

      if (nbpnts > 3)
      {
        PolygonMetrics aMetrics = polygonMetrics(SeqPnt2d);
        aSignedArea             = aMetrics.Area;
        double anExpThick       = 0.0;
        if (!expectedThickness(aMetrics, anExpThick))
        {
          anIsBadWire = true;
          TabOrien.Append(WireRole::Invalid);
          NCollection_LinearVector<gp_Pnt2d> aFallbackPoints(2, gp_Pnt2d(0.0, 0.0));
          TabClass.Append(
            CSLib_Class2d(aFallbackPoints.ToArray1(), FlecheU, FlecheV, Umin, Vmin, Umax, Vmax));
          continue;
        }
        double aDefl      = std::max(FlecheU, FlecheV);
        double aDiscrDefl = std::min(aDefl * 0.1, anExpThick * 10.);
        while (aDefl > anExpThick && aDiscrDefl > 1e-7)
        {
          // Deflection of the polygon is too much for this ratio of area and perimeter,
          // and this might lead to self-intersections.
          // Build tighter samples separately so a failed edge leaves the coarse polygon intact.
          NCollection_LinearVector<gp_Pnt2d> aRefinedPoints;
          int                                aRefinedFirstPoint = 1;
          double                             aRefinedFlecheU    = 0.0;
          double                             aRefinedFlecheV    = 0.0;
          bool                               isRefinementDone   = true;
          for (WireExplorer.Init(TopoDS::Wire(aFaceExplorer.Current()), Face); WireExplorer.More();
               WireExplorer.Next())
          {
            edge = WireExplorer.Current();
            Or   = edge.Orientation();
            if (Or == TopAbs_FORWARD || Or == TopAbs_REVERSED)
            {
              double pfbid, plbid;
              BRep_Tool::Range(edge, Face, pfbid, plbid);
              if (std::abs(plbid - pfbid) < 1.e-9)
              {
                continue;
              }
              BRepAdaptor_Curve2d           C(edge, Face);
              GCPnts_QuasiUniformDeflection aDiscr(C, aDiscrDefl);
              if (!aDiscr.IsDone())
              {
                isRefinementDone = false;
                break;
              }
              int nbp   = aDiscr.NbPoints();
              int iStep = 1, i = 1, iEnd = nbp + 1;
              if (Or == TopAbs_REVERSED)
              {
                iStep = -1;
                i     = nbp;
                iEnd  = 0;
              }
              if (aRefinedFirstPoint == 2)
              {
                i += iStep;
              }
              for (; i != iEnd; i += iStep)
              {
                aRefinedPoints.Append(C.Value(aDiscr.Parameter(i)));
              }
              if (nbp > 2)
              {
                const size_t ii = aRefinedPoints.Size();
                gp_Lin2d     Lin(aRefinedPoints[ii - 3],
                             gp_Dir2d(gp_Vec2d(aRefinedPoints[ii - 3], aRefinedPoints[ii - 1])));
                double       ul = ElCLib::Parameter(Lin, aRefinedPoints[ii - 2]);
                gp_Pnt2d     Pp = ElCLib::Value(ul, Lin);
                double       dU = std::abs(Pp.X() - aRefinedPoints[ii - 2].X());
                double       dV = std::abs(Pp.Y() - aRefinedPoints[ii - 2].Y());
                if (dU > aRefinedFlecheU)
                {
                  aRefinedFlecheU = dU;
                }
                if (dV > aRefinedFlecheV)
                {
                  aRefinedFlecheV = dV;
                }
              }
              aRefinedFirstPoint = 2;
            }
          }

          const PolygonMetrics aRefinedMetrics   = polygonMetrics(aRefinedPoints);
          double               aRefinedThickness = 0.0;
          if (!isRefinementDone || aRefinedPoints.Size() <= 3
              || !expectedThickness(aRefinedMetrics, aRefinedThickness))
          {
            break;
          }

          SeqPnt2d    = std::move(aRefinedPoints);
          nbpnts      = static_cast<int>(SeqPnt2d.Size());
          FlecheU     = aRefinedFlecheU;
          FlecheV     = aRefinedFlecheV;
          aMetrics    = aRefinedMetrics;
          aSignedArea = aMetrics.Area;
          anExpThick  = aRefinedThickness;
          aDefl       = std::max(FlecheU, FlecheV);
          aDiscrDefl  = std::min(aDiscrDefl * 0.1, anExpThick * 10.);
        }

        //-- FlecheU*=10.0;
        //-- FlecheV*=10.0;
        if (aNbE == 1 && FlecheU < eps && FlecheV < eps && std::abs(aSignedArea) < eps)
        {
          TabOrien.Append(WireRole::Outer);
        }
        else
        {
          TabOrien.Append(aSignedArea < 0.0 ? WireRole::Outer : WireRole::Inner);
        }

        if (FlecheU < Toluv)
        {
          FlecheU = Toluv;
        }
        if (FlecheV < Toluv)
        {
          FlecheV = Toluv;
        }
        TabClass.Append(
          CSLib_Class2d(SeqPnt2d.ToArray1(), FlecheU, FlecheV, Umin, Vmin, Umax, Vmax));
      } // if(nbpoints>3
      else
      {
        anIsBadWire = true;
        TabOrien.Append(WireRole::Invalid);
        TabClass.Append(
          CSLib_Class2d(SeqPnt2d.ToArray1(), FlecheU, FlecheV, Umin, Vmin, Umax, Vmax));
      }
    } // else if(WareIsNotEmpty
  } // for(FaceExplorer

  const size_t nbtabclass = TabClass.Size();

  if (nbtabclass > 0)
  {
    //-- If an error was detected on a wire: set all TabOrien to -1
    if (anIsBadWire)
    {
      TabOrien[0] = WireRole::Invalid;
    }

    if (aSurface.GetType() == GeomAbs_Cone || aSurface.GetType() == GeomAbs_Cylinder
        || aSurface.GetType() == GeomAbs_Torus || aSurface.GetType() == GeomAbs_Sphere
        || aSurface.GetType() == GeomAbs_SurfaceOfRevolution)

    {
      double uuu = M_PI + M_PI - (Umax - Umin);
      if (uuu < 0)
      {
        uuu = 0;
      }
      U1 = 0.0;      // modified by NIZHNY-OFV  Thu May 31 14:24:10 2001 ---> //Umin-uuu*0.5;
      U2 = 2 * M_PI; // modified by NIZHNY-OFV  Thu May 31 14:24:35 2001 ---> //U1+M_PI+M_PI;
    }
    else
    {
      U1 = U2 = 0.0;
    }

    if (aSurface.GetType() == GeomAbs_Torus)
    {
      double uuu = M_PI + M_PI - (Vmax - Vmin);
      if (uuu < 0)
      {
        uuu = 0;
      }
      V1 = 0.0;      // modified by NIZHNY-OFV  Thu May 31 14:24:55 2001 ---> //Vmin-uuu*0.5;
      V2 = 2 * M_PI; // modified by NIZHNY-OFV  Thu May 31 14:24:59 2001 ---> //V1+M_PI+M_PI;
    }
    else
    {
      V1 = V2 = 0.0;
    }
  }
}

//=================================================================================================

BRepTopAdaptor_FClass2d::~BRepTopAdaptor_FClass2d()
{
  Destroy();
}

//=================================================================================================

TopAbs_State BRepTopAdaptor_FClass2d::PerformInfinitePoint() const
{
  if (Umax == -RealLast() || Vmax == -RealLast() || Umin == RealLast() || Vmin == RealLast())
  {
    return (TopAbs_IN);
  }
  gp_Pnt2d P(Umin - (Umax - Umin), Vmin - (Vmax - Vmin));
  return (Perform(P, false));
}

//=================================================================================================

TopAbs_State BRepTopAdaptor_FClass2d::exactState(const gp_Pnt2d& thePoint,
                                                 const double    theTolerance) const
{
  std::lock_guard<std::mutex> aLock(myExactMutex);
  if (!myExactExplorer)
  {
    myExactExplorer = std::make_unique<BRepClass_FaceExplorer>(Face);
    myExactExplorer->SetUseBndBox(myExactExplorer->ShouldUseBndBox());
  }
  BRepClass_FaceClassifier aClassifier(*myExactExplorer, thePoint, theTolerance);
  return aClassifier.State();
}

//=================================================================================================

TopAbs_State BRepTopAdaptor_FClass2d::classify(const gp_Pnt2d&          thePoint,
                                               const double             theTolerance,
                                               const bool               theRecadreOnPeriodic,
                                               const ClassificationMode theMode) const
{
  const size_t aClassifierCount = TabClass.Size();
  if (aClassifierCount == 0)
  {
    return TopAbs_IN;
  }

  //-- U1 is the First Param and U2 in this case is U1+Period
  double aU          = thePoint.X();
  double aV          = thePoint.Y();
  double aReframedU  = aU;
  double aReframedV  = aV;
  bool   isUReframed = false;
  bool   isVReframed = false;

  if (theRecadreOnPeriodic)
  {
    if (myIsUPeriodic)
    {
      if (aReframedU < Umin)
      {
        while (aReframedU < Umin)
        {
          aReframedU += myUPeriod;
        }
      }
      else
      {
        while (aReframedU >= Umin)
        {
          aReframedU -= myUPeriod;
        }
        aReframedU += myUPeriod;
      }
    }
    if (myIsVPeriodic)
    {
      if (aReframedV < Vmin)
      {
        while (aReframedV < Vmin)
        {
          aReframedV += myVPeriod;
        }
      }
      else
      {
        while (aReframedV >= Vmin)
        {
          aReframedV -= myVPeriod;
        }
        aReframedV += myVPeriod;
      }
    }
  }

  TopAbs_State aState = TopAbs_UNKNOWN;
  for (;;)
  {
    const gp_Pnt2d aPoint(aU, aV);

    if (TabOrien[0] != WireRole::Invalid)
    {
      CSLib_Class2d::Result aPolygonResult = CSLib_Class2d::Result_Inside;
      for (size_t aWireIndex = 0; aWireIndex < aClassifierCount; ++aWireIndex)
      {
        const int aWireResult = theMode == ClassificationMode::Perform
                                  ? TabClass[aWireIndex].SiDans(aPoint)
                                  : TabClass[aWireIndex].SiDans_OnMode(aPoint, theTolerance);
        if (aWireResult == CSLib_Class2d::Result_Inside)
        {
          if (TabOrien[aWireIndex] == WireRole::Inner)
          {
            aPolygonResult = CSLib_Class2d::Result_Outside;
            break;
          }
        }
        else if (aWireResult == CSLib_Class2d::Result_Outside)
        {
          if (TabOrien[aWireIndex] == WireRole::Outer)
          {
            aPolygonResult = CSLib_Class2d::Result_Outside;
            break;
          }
        }
        else
        {
          aPolygonResult = CSLib_Class2d::Result_Uncertain;
          break;
        }
      }

      if (aPolygonResult == CSLib_Class2d::Result_Uncertain)
      {
        aState = theMode == ClassificationMode::Perform ? exactState(aPoint, std::min(Toluv, 4.0))
                                                        : TopAbs_ON;
      }
      else
      {
        aState = aPolygonResult == CSLib_Class2d::Result_Inside ? TopAbs_IN : TopAbs_OUT;
      }
    }
    else
    {
      // A malformed wire cannot be classified reliably by its polygon.
      aState = exactState(aPoint, theMode == ClassificationMode::Perform ? Toluv : theTolerance);
    }

    if (!theRecadreOnPeriodic || (!myIsUPeriodic && !myIsVPeriodic))
    {
      return aState;
    }
    if (aState == TopAbs_IN || aState == TopAbs_ON)
    {
      return aState;
    }

    if (!isUReframed)
    {
      aU          = aReframedU;
      isUReframed = true;
    }
    else if (myIsUPeriodic)
    {
      aU += myUPeriod;
    }
    if (aU > Umax || !myIsUPeriodic)
    {
      if (!isVReframed)
      {
        aV          = aReframedV;
        isVReframed = true;
      }
      else if (myIsVPeriodic)
      {
        aV += myVPeriod;
      }

      aU = aReframedU;

      if (aV > Vmax || !myIsVPeriodic)
      {
        return aState;
      }
    }
  }
}

//=================================================================================================

TopAbs_State BRepTopAdaptor_FClass2d::Perform(const gp_Pnt2d& thePoint,
                                              const bool      theRecadreOnPeriodic) const
{
  return classify(thePoint, Toluv, theRecadreOnPeriodic, ClassificationMode::Perform);
}

//=================================================================================================

TopAbs_State BRepTopAdaptor_FClass2d::TestOnRestriction(const gp_Pnt2d& thePoint,
                                                        const double    theTolerance,
                                                        const bool      theRecadreOnPeriodic) const
{
  return classify(thePoint, theTolerance, theRecadreOnPeriodic, ClassificationMode::OnRestriction);
}

//=================================================================================================

void BRepTopAdaptor_FClass2d::Destroy()
{
  TabClass.Clear(true);
  TabOrien.Clear(true);
  myExactExplorer.reset();
}
