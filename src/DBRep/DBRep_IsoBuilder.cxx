// Created on: 1994-03-25
// Created by: Jean Marc LACHAUME
// Copyright (c) 1994-1999 Matra Datavision
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

#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <DBRep_Face.hxx>
#include <DBRep_IsoBuilder.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dHatch_Intersector.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <HatchGen_Domain.hxx>
#include <Precision.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

// Providing consistency with intersection tolerance for the linear curves
static Standard_Real IntersectorConfusion = Precision::PConfusion();
static Standard_Real IntersectorTangency  = Precision::PConfusion();
static Standard_Real HatcherConfusion2d   = 1.e-8;
static Standard_Real HatcherConfusion3d   = 1.e-8;

//=======================================================================
// Function : DBRep_IsoBuilder
// Purpose  : Constructeur.
//=======================================================================

DBRep_IsoBuilder::DBRep_IsoBuilder(const TopoDS_Face&     TopologicalFace,
                                   const Standard_Real    Infinite,
                                   const Standard_Integer NbIsos)
    : Geom2dHatch_Hatcher(Geom2dHatch_Intersector(IntersectorConfusion, IntersectorTangency),
                          HatcherConfusion2d,
                          HatcherConfusion3d,
                          Standard_True,
                          Standard_False),
      myInfinite(Infinite),
      myUMin(0.0),
      myUMax(0.0),
      myVMin(0.0),
      myVMax(0.0),
      myUPrm(1, NbIsos),
      myUInd(1, NbIsos),
      myVPrm(1, NbIsos),
      myVInd(1, NbIsos),
      myNbDom(0)
{
  myUInd.Init(0);
  myVInd.Init(0);

  //-----------------------------------------------------------------------
  // If the Min Max bounds are infinite, there are bounded to Infinite
  // value.
  //-----------------------------------------------------------------------

  BRepTools::UVBounds(TopologicalFace, myUMin, myUMax, myVMin, myVMax);
  Standard_Boolean InfiniteUMin = Precision::IsNegativeInfinite(myUMin);
  Standard_Boolean InfiniteUMax = Precision::IsPositiveInfinite(myUMax);
  Standard_Boolean InfiniteVMin = Precision::IsNegativeInfinite(myVMin);
  Standard_Boolean InfiniteVMax = Precision::IsPositiveInfinite(myVMax);
  if (InfiniteUMin && InfiniteUMax)
  {
    myUMin = -Infinite;
    myUMax = Infinite;
  }
  else if (InfiniteUMin)
  {
    myUMin = myUMax - Infinite;
  }
  else if (InfiniteUMax)
  {
    myUMax = myUMin + Infinite;
  }
  if (InfiniteVMin && InfiniteVMax)
  {
    myVMin = -Infinite;
    myVMax = Infinite;
  }
  else if (InfiniteVMin)
  {
    myVMin = myVMax - Infinite;
  }
  else if (InfiniteVMax)
  {
    myVMax = myVMin + Infinite;
  }

  //-----------------------------------------------------------------------
  // Retrieving the edges and its p-curves for further trimming
  // and loading them into the hatcher
  //-----------------------------------------------------------------------
  DataMapOfEdgePCurve anEdgePCurveMap;

  TopExp_Explorer ExpEdges;
  for (ExpEdges.Init(TopologicalFace, TopAbs_EDGE); ExpEdges.More(); ExpEdges.Next())
  {
    const TopoDS_Edge&         TopologicalEdge = TopoDS::Edge(ExpEdges.Current());
    Standard_Real              U1, U2;
    const Handle(Geom2d_Curve) PCurve =
      BRep_Tool::CurveOnSurface(TopologicalEdge, TopologicalFace, U1, U2);

    if (PCurve.IsNull())
    {
#ifdef OCCT_DEBUG
      std::cout << "DBRep_IsoBuilder : PCurve is null\n";
#endif
      return;
    }
    else if (U1 == U2)
    {
#ifdef OCCT_DEBUG
      std::cout << "DBRep_IsoBuilder PCurve : U1==U2\n";
#endif
      return;
    }

    //-- Test if a TrimmedCurve is necessary
    if (Abs(PCurve->FirstParameter() - U1) <= Precision::PConfusion()
        && Abs(PCurve->LastParameter() - U2) <= Precision::PConfusion())
    {
      anEdgePCurveMap.Add(TopologicalEdge, PCurve);
    }
    else
    {
      if (!PCurve->IsPeriodic())
      {
        Handle(Geom2d_TrimmedCurve) TrimPCurve = Handle(Geom2d_TrimmedCurve)::DownCast(PCurve);
        if (!TrimPCurve.IsNull())
        {
          if (TrimPCurve->BasisCurve()->FirstParameter() - U1 > Precision::PConfusion()
              || TrimPCurve->BasisCurve()->FirstParameter() - U2 > Precision::PConfusion()
              || U1 - TrimPCurve->BasisCurve()->LastParameter() > Precision::PConfusion()
              || U2 - TrimPCurve->BasisCurve()->LastParameter() > Precision::PConfusion())
          {
#ifdef OCCT_DEBUG
            std::cout << "DBRep_IsoBuilder TrimPCurve : parameters out of range\n";
            std::cout << "    U1(" << U1 << "), Umin(" << PCurve->FirstParameter() << "), U2(" << U2
                      << "), Umax(" << PCurve->LastParameter() << ")\n";
#endif
            return;
          }
        }
        else
        {
          if (PCurve->FirstParameter() - U1 > Precision::PConfusion())
          {
#ifdef OCCT_DEBUG
            std::cout << "DBRep_IsoBuilder PCurve : parameters out of range\n";
            std::cout << "    U1(" << U1 << "), Umin(" << PCurve->FirstParameter() << ")\n";
#endif
            U1 = PCurve->FirstParameter();
          }
          if (PCurve->FirstParameter() - U2 > Precision::PConfusion())
          {
#ifdef OCCT_DEBUG
            std::cout << "DBRep_IsoBuilder PCurve : parameters out of range\n";
            std::cout << "    U2(" << U2 << "), Umin(" << PCurve->FirstParameter() << ")\n";
#endif
            U2 = PCurve->FirstParameter();
          }
          if (U1 - PCurve->LastParameter() > Precision::PConfusion())
          {
#ifdef OCCT_DEBUG
            std::cout << "DBRep_IsoBuilder PCurve : parameters out of range\n";
            std::cout << "    U1(" << U1 << "), Umax(" << PCurve->LastParameter() << ")\n";
#endif
            U1 = PCurve->LastParameter();
          }
          if (U2 - PCurve->LastParameter() > Precision::PConfusion())
          {
#ifdef OCCT_DEBUG
            std::cout << "DBRep_IsoBuilder PCurve : parameters out of range\n";
            std::cout << "    U2(" << U2 << "), Umax(" << PCurve->LastParameter() << ")\n";
#endif
            U2 = PCurve->LastParameter();
          }
        }
      }

      // if U1 and U2 coincide-->do nothing
      if (Abs(U1 - U2) <= Precision::PConfusion())
        continue;
      Handle(Geom2d_TrimmedCurve) TrimPCurve = new Geom2d_TrimmedCurve(PCurve, U1, U2);
      anEdgePCurveMap.Add(TopologicalEdge, TrimPCurve);
    }
  }

  // Fill the gaps between 2D curves, and trim the intersecting ones.
  FillGaps(TopologicalFace, anEdgePCurveMap);

  // Load trimmed curves to the hatcher
  Standard_Integer aNbE = anEdgePCurveMap.Extent();
  for (Standard_Integer iE = 1; iE <= aNbE; ++iE)
  {
    AddElement(Geom2dAdaptor_Curve(anEdgePCurveMap(iE)), anEdgePCurveMap.FindKey(iE).Orientation());
  }
  //-----------------------------------------------------------------------
  // Loading and trimming the hatchings.
  //-----------------------------------------------------------------------

  Standard_Integer IIso;
  Standard_Real    DeltaU    = Abs(myUMax - myUMin);
  Standard_Real    DeltaV    = Abs(myVMax - myVMin);
  Standard_Real    confusion = Min(DeltaU, DeltaV) * HatcherConfusion3d;
  Confusion3d(confusion);

  Standard_Real StepU = DeltaU / (Standard_Real)NbIsos;
  if (StepU > confusion)
  {
    Standard_Real UPrm = myUMin + StepU / 2.;
    gp_Dir2d      Dir(0., 1.);
    for (IIso = 1; IIso <= NbIsos; IIso++)
    {
      myUPrm(IIso) = UPrm;
      gp_Pnt2d            Ori(UPrm, 0.);
      Geom2dAdaptor_Curve HCur(new Geom2d_Line(Ori, Dir));
      myUInd(IIso) = AddHatching(HCur);
      UPrm += StepU;
    }
  }

  Standard_Real StepV = DeltaV / (Standard_Real)NbIsos;
  if (StepV > confusion)
  {
    Standard_Real VPrm = myVMin + StepV / 2.;
    gp_Dir2d      Dir(1., 0.);
    for (IIso = 1; IIso <= NbIsos; IIso++)
    {
      myVPrm(IIso) = VPrm;
      gp_Pnt2d            Ori(0., VPrm);
      Geom2dAdaptor_Curve HCur(new Geom2d_Line(Ori, Dir));
      myVInd(IIso) = AddHatching(HCur);
      VPrm += StepV;
    }
  }

  //-----------------------------------------------------------------------
  // Computation.
  //-----------------------------------------------------------------------

  Trim();

  myNbDom = 0;
  for (IIso = 1; IIso <= NbIsos; IIso++)
  {
    Standard_Integer Index;

    Index = myUInd(IIso);
    if (Index != 0)
    {
      if (TrimDone(Index) && !TrimFailed(Index))
      {
        ComputeDomains(Index);
        if (IsDone(Index))
          myNbDom = myNbDom + Geom2dHatch_Hatcher::NbDomains(Index);
      }
    }

    Index = myVInd(IIso);
    if (Index != 0)
    {
      if (TrimDone(Index) && !TrimFailed(Index))
      {
        ComputeDomains(Index);
        if (IsDone(Index))
          myNbDom = myNbDom + Geom2dHatch_Hatcher::NbDomains(Index);
      }
    }
  }
}

//=======================================================================
// Function : LoadIsos
// Purpose  : Loading of the isoparametric curves in the Data Structure
//            of a drawable face.
//=======================================================================

void DBRep_IsoBuilder::LoadIsos(const Handle(DBRep_Face)& Face) const
{
  Standard_Integer NumIso = 0;

  for (Standard_Integer UIso = myUPrm.Lower(); UIso <= myUPrm.Upper(); UIso++)
  {
    Standard_Integer UInd = myUInd.Value(UIso);
    if (UInd != 0)
    {
      Standard_Real UPrm = myUPrm.Value(UIso);
      if (!IsDone(UInd))
      {
        std::cout << "DBRep_IsoBuilder:: U iso of parameter: " << UPrm;
        switch (Status(UInd))
        {
          case HatchGen_NoProblem:
            std::cout << " No Problem" << std::endl;
            break;
          case HatchGen_TrimFailure:
            std::cout << " Trim Failure" << std::endl;
            break;
          case HatchGen_TransitionFailure:
            std::cout << " Transition Failure" << std::endl;
            break;
          case HatchGen_IncoherentParity:
            std::cout << " Incoherent Parity" << std::endl;
            break;
          case HatchGen_IncompatibleStates:
            std::cout << " Incompatible States" << std::endl;
            break;
        }
      }
      else
      {
        Standard_Integer NbDom = Geom2dHatch_Hatcher::NbDomains(UInd);
        for (Standard_Integer IDom = 1; IDom <= NbDom; IDom++)
        {
          const HatchGen_Domain& Dom = Domain(UInd, IDom);
          Standard_Real          V1 =
            Dom.HasFirstPoint() ? Dom.FirstPoint().Parameter() : myVMin - myInfinite;
          Standard_Real V2 =
            Dom.HasSecondPoint() ? Dom.SecondPoint().Parameter() : myVMax + myInfinite;
          NumIso++;
          Face->Iso(NumIso, GeomAbs_IsoU, UPrm, V1, V2);
        }
      }
    }
  }

  for (Standard_Integer VIso = myVPrm.Lower(); VIso <= myVPrm.Upper(); VIso++)
  {
    Standard_Integer VInd = myVInd.Value(VIso);
    if (VInd != 0)
    {
      Standard_Real VPrm = myVPrm.Value(VIso);
      if (!IsDone(VInd))
      {
        std::cout << "DBRep_IsoBuilder:: V iso of parameter: " << VPrm;
        switch (Status(VInd))
        {
          case HatchGen_NoProblem:
            std::cout << " No Problem" << std::endl;
            break;
          case HatchGen_TrimFailure:
            std::cout << " Trim Failure" << std::endl;
            break;
          case HatchGen_TransitionFailure:
            std::cout << " Transition Failure" << std::endl;
            break;
          case HatchGen_IncoherentParity:
            std::cout << " Incoherent Parity" << std::endl;
            break;
          case HatchGen_IncompatibleStates:
            std::cout << " Incompatible States" << std::endl;
            break;
        }
      }
      else
      {
        Standard_Integer NbDom = Geom2dHatch_Hatcher::NbDomains(VInd);
        for (Standard_Integer IDom = 1; IDom <= NbDom; IDom++)
        {
          const HatchGen_Domain& Dom = Domain(VInd, IDom);
          Standard_Real          U1 =
            Dom.HasFirstPoint() ? Dom.FirstPoint().Parameter() : myVMin - myInfinite;
          Standard_Real U2 =
            Dom.HasSecondPoint() ? Dom.SecondPoint().Parameter() : myVMax + myInfinite;
          NumIso++;
          Face->Iso(NumIso, GeomAbs_IsoV, VPrm, U1, U2);
        }
      }
    }
  }
}

//=================================================================================================

void DBRep_IsoBuilder::FillGaps(const TopoDS_Face& theFace, DataMapOfEdgePCurve& theEdgePCurveMap)
{

  // Get surface of the face for getting the 3D points from 2D coordinates
  // of the p-curves bounds
  BRepAdaptor_Surface aBASurf(theFace, Standard_False);

  // Analyze each wire of the face separately
  TopoDS_Iterator aItW(theFace);
  for (; aItW.More(); aItW.Next())
  {
    const TopoDS_Shape& aW = aItW.Value();
    if (aW.ShapeType() != TopAbs_WIRE)
      continue;

    // Use WireExplorer to iterate on edges of the wire
    // to get the pairs of connected edges.
    // Using WireExplorer will also allow avoiding treatment
    // of the internal wires.
    BRepTools_WireExplorer aWExp;
    aWExp.Init(TopoDS::Wire(aW), theFace, myUMin, myUMax, myVMin, myVMax);
    if (!aWExp.More())
      continue;

    // Check the number of edges in the wire, not to
    // miss the wires containing one edge only
    if (aW.NbChildren() == 0)
    {
      continue;
    }
    Standard_Boolean SingleEdge = (aW.NbChildren() == 1);

    TopoDS_Edge aPrevEdge, aCurrEdge;

    // Get first edge and its p-curve
    aCurrEdge = aWExp.Current();

    // Ensure analysis of the pair of first and last edges
    TopoDS_Edge   aFirstEdge = aCurrEdge;
    Standard_Real bStop      = Standard_False;

    // Iterate on all other edges
    while (!bStop)
    {
      // Iteration to the next edge
      aPrevEdge = aCurrEdge;
      aWExp.Next();
      // Get the current edge for analysis
      if (aWExp.More())
      {
        aCurrEdge = aWExp.Current();
      }
      else
      {
        aCurrEdge = aFirstEdge;
        bStop     = Standard_True;
      }

      if (aPrevEdge.IsEqual(aCurrEdge) && !SingleEdge)
        continue;

      // Get p-curves
      Handle(Geom2d_Curve)* pPC1 = theEdgePCurveMap.ChangeSeek(aPrevEdge);
      Handle(Geom2d_Curve)* pPC2 = theEdgePCurveMap.ChangeSeek(aCurrEdge);
      if (!pPC1 || !pPC2)
        continue;

      Handle(Geom2d_Curve)& aPrevC2d = *pPC1;
      Handle(Geom2d_Curve)& aCurrC2d = *pPC2;

      // Get p-curves parameters
      Standard_Real fp, lp, fc, lc;
      fp = aPrevC2d->FirstParameter();
      lp = aPrevC2d->LastParameter();
      fc = aCurrC2d->FirstParameter();
      lc = aCurrC2d->LastParameter();

      // Get common vertex to check if the gap between two edges is closed
      // by the tolerance value of this vertex.
      // Take into account the orientation of the edges to obtain the correct
      // parameter of the vertex on edges.

      // Get vertex on the previous edge
      TopoDS_Vertex aCVOnPrev = TopExp::LastVertex(aPrevEdge, Standard_True);
      if (aCVOnPrev.IsNull())
        continue;

      // Get parameter of the vertex on the previous edge
      Standard_Real aTPrev = BRep_Tool::Parameter(aCVOnPrev, aPrevEdge);
      if (aTPrev < fp)
        aTPrev = fp;
      else if (aTPrev > lp)
        aTPrev = lp;

      // Get vertex on the current edge
      TopoDS_Vertex aCVOnCurr = TopExp::FirstVertex(aCurrEdge, Standard_True);
      if (aCVOnCurr.IsNull() || !aCVOnPrev.IsSame(aCVOnCurr))
        continue;

      // Get parameter of the vertex on the current edge
      Standard_Real aTCurr = BRep_Tool::Parameter(aCVOnCurr, aCurrEdge);
      if (aTCurr < fc)
        aTCurr = fc;
      else if (aTCurr > lc)
        aTCurr = lc;

      // Get bounding points on the edges corresponding to the current vertex
      gp_Pnt2d aPrevP2d = aPrevC2d->Value(aTPrev), aCurrP2d = aCurrC2d->Value(aTCurr);

      // Check if the vertex covers these bounding points by its tolerance
      Standard_Real aTolV2 = BRep_Tool::Tolerance(aCVOnPrev);
      gp_Pnt        aPV    = BRep_Tool::Pnt(aCVOnPrev);
      // There is no need to check the distance if the tolerance
      // of vertex is infinite (like in the test case sewing/tol_1/R2)
      if (aTolV2 < Precision::Infinite())
      {
        aTolV2 *= aTolV2;

        // Convert bounding point on previous edge into 3D
        gp_Pnt aPrevPS = aBASurf.Value(aPrevP2d.X(), aPrevP2d.Y());

        // Check if the vertex closes the gap
        if (aPV.SquareDistance(aPrevPS) > aTolV2)
          continue;

        // Convert bounding point on current edge into 3D
        gp_Pnt aCurrPS = aBASurf.Value(aCurrP2d.X(), aCurrP2d.Y());

        // Check if the vertex closes the gap
        if (aPV.SquareDistance(aCurrPS) > aTolV2)
          continue;
      }

      // Create the segment
      gp_Vec2d      aV2d(aPrevP2d, aCurrP2d);
      Standard_Real aSegmLen = aV2d.Magnitude();
      // Do not add too small segments
      Standard_Boolean bAddSegment = (aSegmLen > Precision::PConfusion());
      // Check for periodic surfaces
      if (bAddSegment)
      {
        if (aBASurf.IsUPeriodic())
          bAddSegment = aSegmLen < aBASurf.UPeriod() / 4.;

        if (bAddSegment && aBASurf.IsVPeriodic())
          bAddSegment = aSegmLen < aBASurf.VPeriod() / 4.;
      }

      // Check that p-curves do not interfere near the vertex.
      // And, if they do interfere, avoid creation of the segment.
      if (bAddSegment && !aPrevEdge.IsEqual(aCurrEdge))
      {
        Geom2dAdaptor_Curve aPrevGC(aPrevC2d, fp, lp), aCurrGC(aCurrC2d, fc, lc);
        Geom2dInt_GInter    anInter(aPrevGC,
                                 aCurrGC,
                                 Precision::PConfusion(),
                                 Precision::PConfusion());
        if (anInter.IsDone() && !anInter.IsEmpty())
        {
          // Collect intersection points
          NCollection_List<IntRes2d_IntersectionPoint> aLPInt;
          // Get bounding points from segments
          Standard_Integer iP, aNbInt = anInter.NbSegments();
          for (iP = 1; iP <= aNbInt; ++iP)
          {
            aLPInt.Append(anInter.Segment(iP).FirstPoint());
            aLPInt.Append(anInter.Segment(iP).LastPoint());
          }
          // Get intersection points
          aNbInt = anInter.NbPoints();
          for (iP = 1; iP <= aNbInt; ++iP)
            aLPInt.Append(anInter.Point(iP));

          // Analyze the points and find the one closest to the current vertex
          Standard_Boolean bPointFound   = Standard_False;
          Standard_Real    aTPrevClosest = 0., aTCurrClosest = 0.;
          Standard_Real    aDeltaPrev = ::RealLast(), aDeltaCurr = ::RealLast();

          NCollection_List<IntRes2d_IntersectionPoint>::Iterator aItLPInt(aLPInt);
          for (; aItLPInt.More(); aItLPInt.Next())
          {
            const IntRes2d_IntersectionPoint& aPnt      = aItLPInt.Value();
            const Standard_Real               aTIntPrev = aPnt.ParamOnFirst();
            const Standard_Real               aTIntCurr = aPnt.ParamOnSecond();
            // Check if the intersection point is in range
            if (aTIntPrev < fp || aTIntPrev > lp || aTIntCurr < fc || aTIntCurr > lc)
            {
              continue;
            }

            Standard_Real aDelta1 = Abs(aTIntPrev - aTPrev);
            Standard_Real aDelta2 = Abs(aTIntCurr - aTCurr);
            if (aDelta1 < aDeltaPrev || aDelta2 < aDeltaCurr)
            {
              aTPrevClosest = aTIntPrev;
              aTCurrClosest = aTIntCurr;
              aDeltaPrev    = aDelta1;
              aDeltaCurr    = aDelta2;
              bPointFound   = Standard_True;
            }
          }

          if (bPointFound)
          {
            // Check the number of common vertices between edges.
            // If on the other end, there is also a common vertex,
            // check where the intersection point is located. It might
            // be closer to the other vertex than to the current one.
            // And here we just need to close the gap, avoiding the trimming.
            // If the common vertex is only one, do not create the segment,
            // as we have the intersection of the edges and trimmed the 2d curves.
            Standard_Integer aNbCV = 0;
            for (TopoDS_Iterator it1(aPrevEdge); it1.More(); it1.Next())
            {
              for (TopoDS_Iterator it2(aCurrEdge); it2.More(); it2.Next())
              {
                if (it1.Value().IsSame(it2.Value()))
                  ++aNbCV;
              }
            }

            // Trim PCurves only if the intersection belongs to current parameter
            Standard_Boolean bTrim = (aNbCV == 1
                                      || (Abs(aTPrev - aTPrevClosest) < (lp - fp) / 2.
                                          || Abs(aTCurr - aTCurrClosest) < (lc - fc) / 2.));

            if (bTrim)
            {
              // Check that the intersection point is covered by vertex tolerance
              gp_Pnt2d     aPInt = aPrevC2d->Value(aTPrevClosest);
              const gp_Pnt aPOnS = aBASurf.Value(aPInt.X(), aPInt.Y());
              if (aTolV2 > Precision::Infinite() || aPOnS.SquareDistance(aPV) < aTolV2)
              {
                Standard_Real f, l;

                // Trim the curves with found parameters

                // Prepare trimming parameters for previous curve
                if (Abs(fp - aTPrev) < Abs(lp - aTPrev))
                {
                  f = aTPrevClosest;
                  l = lp;
                }
                else
                {
                  f = fp;
                  l = aTPrevClosest;
                }

                // Trim previous p-curve
                if (l - f > Precision::PConfusion())
                  aPrevC2d = new Geom2d_TrimmedCurve(aPrevC2d, f, l);

                // Prepare trimming parameters for current p-curve
                if (Abs(fc - aTCurr) < Abs(lc - aTCurr))
                {
                  f = aTCurrClosest;
                  l = lc;
                }
                else
                {
                  f = fc;
                  l = aTCurrClosest;
                }

                // Trim current p-curve
                if (l - f > Precision::PConfusion())
                  aCurrC2d = new Geom2d_TrimmedCurve(aCurrC2d, f, l);

                // Do not create the segment, as we performed the trimming
                // to the intersection point.
                bAddSegment = Standard_False;
              }
            }
          }
        }
      }

      if (bAddSegment)
      {
        // Add segment to the hatcher to trim the iso-lines
        Handle(Geom2d_Line)         aLine     = new Geom2d_Line(aPrevP2d, aV2d);
        Handle(Geom2d_TrimmedCurve) aLineSegm = new Geom2d_TrimmedCurve(aLine, 0.0, aSegmLen);
        AddElement(Geom2dAdaptor_Curve(aLineSegm), TopAbs_FORWARD);
      }
    }
  }
}
