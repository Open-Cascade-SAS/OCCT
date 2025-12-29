// Created on: 1995-02-07
// Created by: Jacques GOUSSARD
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

#include <algorithm>

#include <Adaptor2d_Curve2d.hxx>
#include <Adaptor3d_TopolTool.hxx>
#include <ElCLib.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomInt.hxx>
#include <GeomInt_LineConstructor.hxx>
#include <GeomInt_LineTool.hxx>
#include <GeomInt_ParameterAndOrientation.hxx>
#include <NCollection_Sequence.hxx>
#include <gp_Pnt2d.hxx>
#include <IntPatch_ALine.hxx>
#include <IntPatch_GLine.hxx>
#include <IntPatch_Line.hxx>
#include <IntPatch_Point.hxx>
#include <IntPatch_WLine.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <IntSurf_Quadric.hxx>
#include <IntSurf_Transition.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <StdFail_NotDone.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_IndexedMap.hxx>
#include <TopAbs_Orientation.hxx>

static const double TwoPI = M_PI + M_PI;

//=======================================================================
// class    : GeomInt_Vertex
// purpose  : This class has been created in order to provide possibility
//            to sort IntPatch_Points by their parameter on line.
//=======================================================================
class GeomInt_Vertex
{
public:
  GeomInt_Vertex() = default;

  //! Initializes this class by IntPatch_Point
  void SetVertex(const IntPatch_Point& theOther)
  {
    myVertex               = theOther;
    const double aNewParam = ElCLib::InPeriod(theOther.ParameterOnLine(), 0.0, TwoPI);
    SetParameter(aNewParam);
  }

  //! Sets Parameter on Line
  void SetParameter(const double theParam) { myVertex.SetParameter(theParam); }

  //! Returns IntPatch_Point
  const IntPatch_Point& Getvertex() const { return myVertex; }

  //! To provide sort
  bool operator<(const GeomInt_Vertex& theOther) const
  {
    return myVertex.ParameterOnLine() < theOther.myVertex.ParameterOnLine();
  }

private:
  IntPatch_Point myVertex;
};

//------------
static void Parameters(const occ::handle<GeomAdaptor_Surface>& myHS1,
                       const gp_Pnt&                           Ptref,
                       double&                                 U1,
                       double&                                 V1);

static void Parameters(const occ::handle<GeomAdaptor_Surface>& myHS1,
                       const occ::handle<GeomAdaptor_Surface>& myHS2,
                       const gp_Pnt&                           Ptref,
                       double&                                 U1,
                       double&                                 V1,
                       double&                                 U2,
                       double&                                 V2);

static void GLinePoint(const IntPatch_IType               typl,
                       const occ::handle<IntPatch_GLine>& GLine,
                       const double                       aT,
                       gp_Pnt&                            aP);

static void AdjustPeriodic(const occ::handle<GeomAdaptor_Surface>& myHS1,
                           const occ::handle<GeomAdaptor_Surface>& myHS2,
                           double&                                 u1,
                           double&                                 v1,
                           double&                                 u2,
                           double&                                 v2);

static bool RejectMicroCircle(const occ::handle<IntPatch_GLine>& aGLine,
                              const IntPatch_IType               aType,
                              const double                       aTol3D);

static void RejectDuplicates(NCollection_Array1<GeomInt_Vertex>& theVtxArr);

//=================================================================================================

void GeomInt_LineConstructor::Perform(const occ::handle<IntPatch_Line>& L)
{
  int              i, nbvtx;
  double           firstp, lastp;
  constexpr double Tol = Precision::PConfusion() * 35.0;

  const IntPatch_IType typl = L->ArcType();
  if (typl == IntPatch_Analytic)
  {
    double                      u1, v1, u2, v2;
    occ::handle<IntPatch_ALine> ALine(occ::down_cast<IntPatch_ALine>(L));
    seqp.Clear();
    nbvtx = GeomInt_LineTool::NbVertex(L);
    for (i = 1; i < nbvtx; i++)
    {
      firstp = GeomInt_LineTool::Vertex(L, i).ParameterOnLine();
      lastp  = GeomInt_LineTool::Vertex(L, i + 1).ParameterOnLine();
      if (firstp != lastp)
      {
        const double pmid = (firstp + lastp) * 0.5;
        const gp_Pnt Pmid = ALine->Value(pmid);
        Parameters(myHS1, myHS2, Pmid, u1, v1, u2, v2);
        AdjustPeriodic(myHS1, myHS2, u1, v1, u2, v2);
        const TopAbs_State in1 = myDom1->Classify(gp_Pnt2d(u1, v1), Tol);
        if (in1 != TopAbs_OUT)
        {
          const TopAbs_State in2 = myDom2->Classify(gp_Pnt2d(u2, v2), Tol);
          if (in2 != TopAbs_OUT)
          {
            seqp.Append(firstp);
            seqp.Append(lastp);
          }
        }
      }
    }
    done = true;
    return;
  } // if(typl == IntPatch_Analytic)  {
  else if (typl == IntPatch_Walking)
  {
    double                      u1, v1, u2, v2;
    occ::handle<IntPatch_WLine> WLine(occ::down_cast<IntPatch_WLine>(L));
    seqp.Clear();
    nbvtx = GeomInt_LineTool::NbVertex(L);
    for (i = 1; i < nbvtx; i++)
    {
      firstp = GeomInt_LineTool::Vertex(L, i).ParameterOnLine();
      lastp  = GeomInt_LineTool::Vertex(L, i + 1).ParameterOnLine();
      if (firstp != lastp)
      {
        if (lastp != firstp + 1)
        {
          const int              pmid = (int)((firstp + lastp) / 2);
          const IntSurf_PntOn2S& Pmid = WLine->Point(pmid);
          Pmid.Parameters(u1, v1, u2, v2);
          AdjustPeriodic(myHS1, myHS2, u1, v1, u2, v2);
          const TopAbs_State in1 = myDom1->Classify(gp_Pnt2d(u1, v1), Tol);
          if (in1 != TopAbs_OUT)
          {
            const TopAbs_State in2 = myDom2->Classify(gp_Pnt2d(u2, v2), Tol);
            if (in2 != TopAbs_OUT)
            {
              seqp.Append(firstp);
              seqp.Append(lastp);
            }
          }
        }
        else
        {
          if (WLine->GetCreatingWay() == IntPatch_WLine::IntPatch_WLImpPrm)
          {
            // The fix #29972.
            // Implicit-Parametric intersector does not respect domain of
            // the quadric surface (it takes into account the domain of the
            // parametric surface only). It means that we cannot warrant that
            // we have a point exactly in the quadric boundary.
            // E.g. in the test cases "bugs modalg_5 bug25697_2",
            //"bugs modalg_5 bug23948", "boolean bopfuse_complex G9",
            //"boolean bopcommon_complex H7", "boolean bopcut_complex I7" etc.
            // the WLine contains 2 points and one is out of the quadric's domain.
            // In order to process these cases correctly, we classify a middle
            //(between these two) point (obtained by interpolation).

            // Other types of intersector take into account the domains of both surfaces.
            // So, they require to reject all "outboundaried" parts of WLine. As result,
            // more strict check (all two points of WLine are checksed) is
            // applied in this case.

            double                 aU21, aV21, aU22, aV22;
            const IntSurf_PntOn2S& aPfirst = WLine->Point((int)(firstp));
            const IntSurf_PntOn2S& aPlast  = WLine->Point((int)(lastp));
            aPfirst.Parameters(u1, v1, u2, v2);
            AdjustPeriodic(myHS1, myHS2, u1, v1, u2, v2);
            aPlast.Parameters(aU21, aV21, aU22, aV22);
            AdjustPeriodic(myHS1, myHS2, aU21, aV21, aU22, aV22);

            u1 = 0.5 * (u1 + aU21);
            v1 = 0.5 * (v1 + aV21);
            u2 = 0.5 * (u2 + aU22);
            v2 = 0.5 * (v2 + aV22);

            const TopAbs_State in1 = myDom1->Classify(gp_Pnt2d(u1, v1), Tol);
            if (in1 != TopAbs_OUT)
            {
              const TopAbs_State in2 = myDom2->Classify(gp_Pnt2d(u2, v2), Tol);
              if (in2 != TopAbs_OUT)
              {
                seqp.Append(firstp);
                seqp.Append(lastp);
              }
            }
          }
          else
          {
            const IntSurf_PntOn2S& Pfirst = WLine->Point((int)(firstp));
            Pfirst.Parameters(u1, v1, u2, v2);
            AdjustPeriodic(myHS1, myHS2, u1, v1, u2, v2);
            TopAbs_State in1 = myDom1->Classify(gp_Pnt2d(u1, v1), Tol);
            if (in1 != TopAbs_OUT)
            {
              TopAbs_State in2 = myDom2->Classify(gp_Pnt2d(u2, v2), Tol);
              if (in2 != TopAbs_OUT)
              {
                const IntSurf_PntOn2S& Plast = WLine->Point((int)(lastp));
                Plast.Parameters(u1, v1, u2, v2);
                AdjustPeriodic(myHS1, myHS2, u1, v1, u2, v2);
                in1 = myDom1->Classify(gp_Pnt2d(u1, v1), Tol);
                if (in1 != TopAbs_OUT)
                {
                  in2 = myDom2->Classify(gp_Pnt2d(u2, v2), Tol);
                  if (in2 != TopAbs_OUT)
                  {
                    seqp.Append(firstp);
                    seqp.Append(lastp);
                  }
                }
              }
            }
          }
        }
      }
    }
    //
    // The One resulting curve consists of 7 segments that are
    // connected between each other.
    // The aim of the block is to reject these segments and have
    // one segment instead of 7.
    //     The other reason to do that is value of TolReached3D=49.
    //     Why -? It is not known yet.
    //                                             PKV 22.Apr.2002
    //
    int aNbParts;
    //
    aNbParts = seqp.Length() / 2;
    if (aNbParts > 1)
    {
      bool                bCond;
      GeomAbs_SurfaceType aST1, aST2;
      aST1 = myHS1->GetType();
      aST2 = myHS2->GetType();
      //
      bCond = false;
      if (aST1 == GeomAbs_Plane)
      {
        if (aST2 == GeomAbs_SurfaceOfExtrusion || aST2 == GeomAbs_SurfaceOfRevolution)
        { //+zft
          bCond = !bCond;
        }
      }
      else if (aST2 == GeomAbs_Plane)
      {
        if (aST1 == GeomAbs_SurfaceOfExtrusion || aST1 == GeomAbs_SurfaceOfRevolution)
        { //+zft
          bCond = !bCond;
        }
      }
      //
      if (bCond)
      {
        int                          aNb, anIndex, aNbTmp, jx;
        NCollection_IndexedMap<int>  aMap;
        NCollection_Sequence<double> aSeqTmp;
        //
        aNb = seqp.Length();
        for (i = 1; i <= aNb; ++i)
        {
          lastp   = seqp(i);
          anIndex = (int)lastp;
          if (!aMap.Contains(anIndex))
          {
            aMap.Add(anIndex);
            aSeqTmp.Append(lastp);
          }
          else
          {
            aNbTmp = aSeqTmp.Length();
            aSeqTmp.Remove(aNbTmp);
          }
        }
        //
        seqp.Clear();
        //
        aNb = aSeqTmp.Length() / 2;
        for (i = 1; i <= aNb; ++i)
        {
          jx     = 2 * i;
          firstp = aSeqTmp(jx - 1);
          lastp  = aSeqTmp(jx);
          seqp.Append(firstp);
          seqp.Append(lastp);
        }
      } // if (bCond) {
    }
    done = true;
    return;
  } // else if(typl == IntPatch_Walking)  {
  //
  //-----------------------------------------------------------
  else if (typl != IntPatch_Restriction)
  {
    seqp.Clear();
    //
    occ::handle<IntPatch_GLine> GLine(occ::down_cast<IntPatch_GLine>(L));
    //
    if (typl == IntPatch_Circle || typl == IntPatch_Ellipse)
    {
      TreatCircle(L, Tol);
      done = true;
      return;
    }
    //----------------------------
    bool   intrvtested;
    double u1, v1, u2, v2;
    //
    nbvtx       = GeomInt_LineTool::NbVertex(L);
    intrvtested = false;
    for (i = 1; i < nbvtx; ++i)
    {
      firstp = GeomInt_LineTool::Vertex(L, i).ParameterOnLine();
      lastp  = GeomInt_LineTool::Vertex(L, i + 1).ParameterOnLine();
      if (std::abs(firstp - lastp) > Precision::PConfusion())
      {
        intrvtested       = true;
        const double pmid = (firstp + lastp) * 0.5;
        gp_Pnt       Pmid;
        GLinePoint(typl, GLine, pmid, Pmid);
        //
        Parameters(myHS1, myHS2, Pmid, u1, v1, u2, v2);
        AdjustPeriodic(myHS1, myHS2, u1, v1, u2, v2);
        const TopAbs_State in1 = myDom1->Classify(gp_Pnt2d(u1, v1), Tol);
        if (in1 != TopAbs_OUT)
        {
          const TopAbs_State in2 = myDom2->Classify(gp_Pnt2d(u2, v2), Tol);
          if (in2 != TopAbs_OUT)
          {
            seqp.Append(firstp);
            seqp.Append(lastp);
          }
        }
      }
    }
    //
    if (!intrvtested)
    {
      // Keep a priori. A point 2d on each
      // surface is required to make the decision. Will be done in the caller
      seqp.Append(GeomInt_LineTool::FirstParameter(L));
      seqp.Append(GeomInt_LineTool::LastParameter(L));
    }
    //
    done = true;
    return;
  } // else if (typl != IntPatch_Restriction)  {

  done = false;
  seqp.Clear();
  nbvtx = GeomInt_LineTool::NbVertex(L);
  if (nbvtx == 0)
  { // Keep a priori. Point 2d is required on each
    // surface to make the decision. Will be done in the caller
    seqp.Append(GeomInt_LineTool::FirstParameter(L));
    seqp.Append(GeomInt_LineTool::LastParameter(L));
    done = true;
    return;
  }

  NCollection_Sequence<GeomInt_ParameterAndOrientation> seqpss;
  TopAbs_Orientation                                    or1 = TopAbs_FORWARD, or2 = TopAbs_FORWARD;

  for (i = 1; i <= nbvtx; i++)
  {
    const IntPatch_Point& thevtx = GeomInt_LineTool::Vertex(L, i);
    const double          prm    = thevtx.ParameterOnLine();
    if (thevtx.IsOnDomS1())
    {
      switch (thevtx.TransitionLineArc1().TransitionType())
      {
        case IntSurf_In:
          or1 = TopAbs_FORWARD;
          break;
        case IntSurf_Out:
          or1 = TopAbs_REVERSED;
          break;
        case IntSurf_Touch:
          or1 = TopAbs_INTERNAL;
          break;
        case IntSurf_Undecided:
          or1 = TopAbs_INTERNAL;
          break;
      }
    }
    else
    {
      or1 = TopAbs_INTERNAL;
    }

    if (thevtx.IsOnDomS2())
    {
      switch (thevtx.TransitionLineArc2().TransitionType())
      {
        case IntSurf_In:
          or2 = TopAbs_FORWARD;
          break;
        case IntSurf_Out:
          or2 = TopAbs_REVERSED;
          break;
        case IntSurf_Touch:
          or2 = TopAbs_INTERNAL;
          break;
        case IntSurf_Undecided:
          or2 = TopAbs_INTERNAL;
          break;
      }
    }
    else
    {
      or2 = TopAbs_INTERNAL;
    }
    //
    const int nbinserted = seqpss.Length();
    bool      inserted   = false;
    for (int j = 1; j <= nbinserted; j++)
    {
      if (std::abs(prm - seqpss(j).Parameter()) <= Tol)
      {
        // accumulate
        GeomInt_ParameterAndOrientation& valj = seqpss.ChangeValue(j);
        if (or1 != TopAbs_INTERNAL)
        {
          if (valj.Orientation1() != TopAbs_INTERNAL)
          {
            if (or1 != valj.Orientation1())
            {
              valj.SetOrientation1(TopAbs_INTERNAL);
            }
          }
          else
          {
            valj.SetOrientation1(or1);
          }
        }

        if (or2 != TopAbs_INTERNAL)
        {
          if (valj.Orientation2() != TopAbs_INTERNAL)
          {
            if (or2 != valj.Orientation2())
            {
              valj.SetOrientation2(TopAbs_INTERNAL);
            }
          }
          else
          {
            valj.SetOrientation2(or2);
          }
        }
        inserted = true;
        break;
      }

      if (prm < seqpss(j).Parameter() - Tol)
      {
        // insert before position j
        seqpss.InsertBefore(j, GeomInt_ParameterAndOrientation(prm, or1, or2));
        inserted = true;
        break;
      }
    }
    if (!inserted)
    {
      seqpss.Append(GeomInt_ParameterAndOrientation(prm, or1, or2));
    }
  }

  // determine the state at the beginning of line
  bool trim   = false;
  bool dansS1 = false;
  bool dansS2 = false;

  nbvtx = seqpss.Length();
  for (i = 1; i <= nbvtx; i++)
  {
    or1 = seqpss(i).Orientation1();
    if (or1 != TopAbs_INTERNAL)
    {
      trim   = true;
      dansS1 = (or1 != TopAbs_FORWARD);
      break;
    }
  }

  if (i > nbvtx)
  {
    double U, V;
    for (i = 1; i <= GeomInt_LineTool::NbVertex(L); i++)
    {
      if (!GeomInt_LineTool::Vertex(L, i).IsOnDomS1())
      {
        GeomInt_LineTool::Vertex(L, i).ParametersOnS1(U, V);
        gp_Pnt2d PPCC(U, V);
        if (myDom1->Classify(PPCC, Tol) == TopAbs_OUT)
        {
          done = true;
          return;
        }
        break;
      }
    }
    dansS1 = true; // Keep in doubt
  }
  //
  for (i = 1; i <= nbvtx; i++)
  {
    or2 = seqpss(i).Orientation2();
    if (or2 != TopAbs_INTERNAL)
    {
      trim   = true;
      dansS2 = (or2 != TopAbs_FORWARD);
      break;
    }
  }

  if (i > nbvtx)
  {
    double U, V;
    for (i = 1; i <= GeomInt_LineTool::NbVertex(L); i++)
    {
      if (!GeomInt_LineTool::Vertex(L, i).IsOnDomS2())
      {
        GeomInt_LineTool::Vertex(L, i).ParametersOnS2(U, V);
        if (myDom2->Classify(gp_Pnt2d(U, V), Tol) == TopAbs_OUT)
        {
          done = true;
          return;
        }
        break;
      }
    }
    dansS2 = true; //  Keep in doubt
  }

  if (!trim)
  { // necessarily dansS1 == dansS2 == true
    seqp.Append(GeomInt_LineTool::FirstParameter(L));
    seqp.Append(GeomInt_LineTool::LastParameter(L));
    done = true;
    return;
  }

  // sequence seqpss is peeled to create valid ends
  // and store them in seqp(2*i+1) and seqp(2*i+2)
  double thefirst = GeomInt_LineTool::FirstParameter(L);
  double thelast  = GeomInt_LineTool::LastParameter(L);
  firstp          = thefirst;

  for (i = 1; i <= nbvtx; i++)
  {
    or1 = seqpss(i).Orientation1();
    or2 = seqpss(i).Orientation2();
    if (dansS1 && dansS2)
    {
      if (or1 == TopAbs_REVERSED)
      {
        dansS1 = false;
      }

      if (or2 == TopAbs_REVERSED)
      {
        dansS2 = false;
      }
      if (!dansS1 || !dansS2)
      {
        lastp           = seqpss(i).Parameter();
        double stofirst = std::max(firstp, thefirst);
        double stolast  = std::min(lastp, thelast);

        if (stolast > stofirst)
        {
          seqp.Append(stofirst);
          seqp.Append(stolast);
        }
        if (lastp > thelast)
        {
          break;
        }
      }
    }
    else
    {
      if (dansS1)
      {
        if (or1 == TopAbs_REVERSED)
        {
          dansS1 = false;
        }
      }
      else
      {
        if (or1 == TopAbs_FORWARD)
        {
          dansS1 = true;
        }
      }
      if (dansS2)
      {
        if (or2 == TopAbs_REVERSED)
        {
          dansS2 = false;
        }
      }
      else
      {
        if (or2 == TopAbs_FORWARD)
        {
          dansS2 = true;
        }
      }
      if (dansS1 && dansS2)
      {
        firstp = seqpss(i).Parameter();
      }
    }
  }
  //
  // finally to add
  if (dansS1 && dansS2)
  {
    lastp  = thelast;
    firstp = std::max(firstp, thefirst);
    if (lastp > firstp)
    {
      seqp.Append(firstp);
      seqp.Append(lastp);
    }
  }
  done = true;
}

//=================================================================================================

void GeomInt_LineConstructor::TreatCircle(const occ::handle<IntPatch_Line>& theLine,
                                          const double                      theTol)
{
  const IntPatch_IType              aType = theLine->ArcType();
  const occ::handle<IntPatch_GLine> aGLine(occ::down_cast<IntPatch_GLine>(theLine));
  if (RejectMicroCircle(aGLine, aType, theTol))
  {
    return;
  }
  //----------------------------------------
  const int                          aNbVtx = aGLine->NbVertex();
  NCollection_Array1<GeomInt_Vertex> aVtxArr(1, aNbVtx + 1);
  for (int i = 1; i <= aNbVtx; i++)
  {
    aVtxArr(i).SetVertex(aGLine->Vertex(i));
  }

  std::sort(aVtxArr.begin(), aVtxArr.begin() + aNbVtx);

  // Create last vertex
  const double aMinPrm = aVtxArr.First().Getvertex().ParameterOnLine() + TwoPI;
  aVtxArr.ChangeLast().SetParameter(aMinPrm);

  RejectDuplicates(aVtxArr);

  std::sort(aVtxArr.begin(), aVtxArr.end());

  double   aU1, aV1, aU2, aV2;
  gp_Pnt   aPmid;
  gp_Pnt2d aP2D;
  for (int i = aVtxArr.Lower(); i <= aVtxArr.Upper() - 1; i++)
  {
    const double aT1 = aVtxArr(i).Getvertex().ParameterOnLine();
    const double aT2 = aVtxArr(i + 1).Getvertex().ParameterOnLine();

    if (aT2 == RealLast())
      break;

    const double aTmid = (aT1 + aT2) * 0.5;
    GLinePoint(aType, aGLine, aTmid, aPmid);
    //
    Parameters(myHS1, myHS2, aPmid, aU1, aV1, aU2, aV2);
    AdjustPeriodic(myHS1, myHS2, aU1, aV1, aU2, aV2);
    //
    aP2D.SetCoord(aU1, aV1);
    TopAbs_State aState = myDom1->Classify(aP2D, theTol);
    if (aState != TopAbs_OUT)
    {
      aP2D.SetCoord(aU2, aV2);
      aState = myDom2->Classify(aP2D, theTol);
      if (aState != TopAbs_OUT)
      {
        seqp.Append(aT1);
        seqp.Append(aT2);
      }
    }
  }
}

//=================================================================================================

void AdjustPeriodic(const occ::handle<GeomAdaptor_Surface>& myHS1,
                    const occ::handle<GeomAdaptor_Surface>& myHS2,
                    double&                                 u1,
                    double&                                 v1,
                    double&                                 u2,
                    double&                                 v2)
{
  bool                      myHS1IsUPeriodic, myHS1IsVPeriodic;
  const GeomAbs_SurfaceType typs1 = myHS1->GetType();
  switch (typs1)
  {
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere: {
      myHS1IsUPeriodic = true;
      myHS1IsVPeriodic = false;
      break;
    }
    case GeomAbs_Torus: {
      myHS1IsUPeriodic = myHS1IsVPeriodic = true;
      break;
    }
    default: {
      //-- Case of periodic biparameters is processed upstream
      myHS1IsUPeriodic = myHS1IsVPeriodic = false;
      break;
    }
  }
  bool                      myHS2IsUPeriodic, myHS2IsVPeriodic;
  const GeomAbs_SurfaceType typs2 = myHS2->GetType();
  switch (typs2)
  {
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere: {
      myHS2IsUPeriodic = true;
      myHS2IsVPeriodic = false;
      break;
    }
    case GeomAbs_Torus: {
      myHS2IsUPeriodic = myHS2IsVPeriodic = true;
      break;
    }
    default: {
      //-- Case of periodic biparameters is processed upstream
      myHS2IsUPeriodic = myHS2IsVPeriodic = false;
      break;
    }
  }
  double du, dv;
  //
  if (myHS1IsUPeriodic)
  {
    const double lmf = M_PI + M_PI; //-- myHS1->UPeriod();
    const double f   = myHS1->FirstUParameter();
    const double l   = myHS1->LastUParameter();
    GeomInt::AdjustPeriodic(u1, f, l, lmf, u1, du);
  }
  if (myHS1IsVPeriodic)
  {
    const double lmf = M_PI + M_PI; //-- myHS1->VPeriod();
    const double f   = myHS1->FirstVParameter();
    const double l   = myHS1->LastVParameter();
    GeomInt::AdjustPeriodic(v1, f, l, lmf, v1, dv);
  }
  if (myHS2IsUPeriodic)
  {
    const double lmf = M_PI + M_PI; //-- myHS2->UPeriod();
    const double f   = myHS2->FirstUParameter();
    const double l   = myHS2->LastUParameter();
    GeomInt::AdjustPeriodic(u2, f, l, lmf, u2, du);
  }
  if (myHS2IsVPeriodic)
  {
    const double lmf = M_PI + M_PI; //-- myHS2->VPeriod();
    const double f   = myHS2->FirstVParameter();
    const double l   = myHS2->LastVParameter();
    GeomInt::AdjustPeriodic(v2, f, l, lmf, v2, dv);
  }
}

//=================================================================================================

void Parameters(const occ::handle<GeomAdaptor_Surface>& myHS1,
                const occ::handle<GeomAdaptor_Surface>& myHS2,
                const gp_Pnt&                           Ptref,
                double&                                 U1,
                double&                                 V1,
                double&                                 U2,
                double&                                 V2)
{
  Parameters(myHS1, Ptref, U1, V1);
  Parameters(myHS2, Ptref, U2, V2);
}

//=================================================================================================

void Parameters(const occ::handle<GeomAdaptor_Surface>& myHS1,
                const gp_Pnt&                           Ptref,
                double&                                 U1,
                double&                                 V1)
{
  IntSurf_Quadric quad1;
  //
  switch (myHS1->GetType())
  {
    case GeomAbs_Plane:
      quad1.SetValue(myHS1->Plane());
      break;
    case GeomAbs_Cylinder:
      quad1.SetValue(myHS1->Cylinder());
      break;
    case GeomAbs_Cone:
      quad1.SetValue(myHS1->Cone());
      break;
    case GeomAbs_Sphere:
      quad1.SetValue(myHS1->Sphere());
      break;
    case GeomAbs_Torus:
      quad1.SetValue(myHS1->Torus());
      break;
    default:
      throw Standard_ConstructionError("GeomInt_LineConstructor::Parameters");
  }
  quad1.Parameters(Ptref, U1, V1);
}

//=================================================================================================

void GLinePoint(const IntPatch_IType               typl,
                const occ::handle<IntPatch_GLine>& GLine,
                const double                       aT,
                gp_Pnt&                            aP)
{
  switch (typl)
  {
    case IntPatch_Lin:
      aP = ElCLib::Value(aT, GLine->Line());
      break;
    case IntPatch_Circle:
      aP = ElCLib::Value(aT, GLine->Circle());
      break;
    case IntPatch_Ellipse:
      aP = ElCLib::Value(aT, GLine->Ellipse());
      break;
    case IntPatch_Hyperbola:
      aP = ElCLib::Value(aT, GLine->Hyperbola());
      break;
    case IntPatch_Parabola:
      aP = ElCLib::Value(aT, GLine->Parabola());
      break;
    default:
      throw Standard_ConstructionError("GeomInt_LineConstructor::Parameters");
  }
}

//=================================================================================================

bool RejectMicroCircle(const occ::handle<IntPatch_GLine>& aGLine,
                       const IntPatch_IType               aType,
                       const double                       aTol3D)
{
  bool   bRet;
  double aR;
  //
  bRet = false;
  //
  if (aType == IntPatch_Circle)
  {
    aR   = aGLine->Circle().Radius();
    bRet = (aR < aTol3D);
  }
  else if (aType == IntPatch_Ellipse)
  {
    aR   = aGLine->Ellipse().MajorRadius();
    bRet = (aR < aTol3D);
  }
  return bRet;
}

//=======================================================================
// function : RejectDuplicates
// purpose  : Finds two coincident IntPatch_Points (if they exist) and
//            sets Parameter-On-Line fore one such point to DBL_MAX
//            (i.e. its use in the future is forbidden).
//
// ATTENTION!!!
//           The source array must be sorted in ascending order.
//=======================================================================
void RejectDuplicates(NCollection_Array1<GeomInt_Vertex>& theVtxArr)
{
  // About the value aTolPC=1000.*Precision::PConfusion(),
  // see IntPatch_GLine::ComputeVertexParameters(...)
  // for more details;
  constexpr double aTolPC = 1000. * Precision::PConfusion();

  // Find duplicates in a slice of the array [LowerBound, UpperBound-1].
  // If a duplicate has been found, the element with greater index will be rejected.
  for (int i = theVtxArr.Lower(); i <= theVtxArr.Upper() - 2; i++)
  {
    const IntPatch_Point& aVi   = theVtxArr(i).Getvertex();
    const double          aPrmi = aVi.ParameterOnLine();

    if (aPrmi == RealLast())
      continue;

    for (int j = i + 1; j <= theVtxArr.Upper() - 1; j++)
    {
      const IntPatch_Point& aVj   = theVtxArr(j).Getvertex();
      const double          aPrmj = aVj.ParameterOnLine();

      if (aPrmj - aPrmi < aTolPC)
      {
        theVtxArr(j).SetParameter(RealLast());
      }
      else
      {
        break;
      }
    }
  }

  // Find duplicates with the last element of the array.
  // If a duplicate has been found, the found element will be rejected.
  const double aMaxPrm = theVtxArr.Last().Getvertex().ParameterOnLine();
  for (int i = theVtxArr.Upper() - 1; i > theVtxArr.Lower(); i--)
  {
    const IntPatch_Point& aVi   = theVtxArr(i).Getvertex();
    const double          aPrmi = aVi.ParameterOnLine();

    if (aPrmi == RealLast())
      continue;

    if ((aMaxPrm - aPrmi) < aTolPC)
    {
      theVtxArr(i).SetParameter(RealLast());
    }
    else
    {
      break;
    }
  }
}
