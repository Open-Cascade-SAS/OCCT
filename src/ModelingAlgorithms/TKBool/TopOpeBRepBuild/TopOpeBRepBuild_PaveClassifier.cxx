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

#include <BRep_Tool.hxx>
#include <ElCLib.hxx>
#include <Geom_Curve.hxx>
#include <Precision.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepBuild_Pave.hxx>
#include <TopOpeBRepBuild_PaveClassifier.hxx>

//=================================================================================================

TopOpeBRepBuild_PaveClassifier::TopOpeBRepBuild_PaveClassifier(const TopoDS_Shape& E)
    : myEdgePeriodic(false),
      mySameParameters(false),
      myClosedVertices(false)
{
  myEdge = TopoDS::Edge(E);

  if (!BRep_Tool::Degenerated(myEdge))
  {
    TopLoc_Location         loc;
    double                  f, l;
    occ::handle<Geom_Curve> C = BRep_Tool::Curve(myEdge, loc, f, l);
    if (!C.IsNull())
    {
      if (C->IsPeriodic())
      {
        TopoDS_Vertex v1, v2;
        TopExp::Vertices(myEdge, v1, v2); // v1 FORWARD, v2 REVERSED
        if (!v1.IsNull() && !v2.IsNull())
        {
          // --- the edge has vertices
          myFirst        = f;
          double fC      = C->FirstParameter();
          double lC      = C->LastParameter();
          myPeriod       = lC - fC;
          myEdgePeriodic = mySameParameters = v1.IsSame(v2);
          if (mySameParameters)
          {
            myFirst = BRep_Tool::Parameter(v1, myEdge);
          }
        }
        else
        {
          // --- the edge has no vertices
          myFirst          = f;
          myPeriod         = l - f;
          myEdgePeriodic   = true;
          mySameParameters = false;
        }
      }
    }

  } // ! degenerated
}

//=================================================================================================

TopAbs_State TopOpeBRepBuild_PaveClassifier::CompareOnNonPeriodic()
{

  TopAbs_State state = TopAbs_UNKNOWN;
  bool         lower = false;
  switch (myO2)
  {
    case TopAbs_FORWARD:
      lower = false;
      break;
    case TopAbs_REVERSED:
      lower = true;
      break;
    case TopAbs_INTERNAL:
      state = TopAbs_IN;
      break;
    case TopAbs_EXTERNAL:
      state = TopAbs_OUT;
      break;
  }

  if (state == TopAbs_UNKNOWN)
  {
    if (myP1 == myP2)
    {
      if (myO1 == myO2)
      {
        state = TopAbs_IN;
      }
      else
      {
        state = TopAbs_OUT;
      }
    }
    else if (myP1 < myP2)
    {
      if (lower)
      {
        state = TopAbs_IN;
      }
      else
      {
        state = TopAbs_OUT;
      }
    }
    else
    {
      if (lower)
      {
        state = TopAbs_OUT;
      }
      else
      {
        state = TopAbs_IN;
      }
    }
  }

  return state;
}

//=================================================================================================

double TopOpeBRepBuild_PaveClassifier::AdjustCase(const double             p1,
                                                  const TopAbs_Orientation o,
                                                  const double             first,
                                                  const double             period,
                                                  const double             tol,
                                                  int&                     cas)
{
  double p2;
  if (std::abs(p1 - first) < tol)
  { // p1 is first
    if (o == TopAbs_REVERSED)
    {
      p2  = p1 + period;
      cas = 1;
    }
    else
    {
      p2  = p1;
      cas = 2;
    }
  }
  else
  { // p1 is not on first
    double last = first + period;
    if (std::abs(p1 - last) < tol)
    { // p1 is on last
      p2  = p1;
      cas = 3;
    }
    else
    { // p1 is not on last
      p2  = ElCLib::InPeriod(p1, first, last);
      cas = 4;
    }
  }
  return p2;
}

//=================================================================================================

void TopOpeBRepBuild_PaveClassifier::AdjustOnPeriodic()
{
  if (!ToAdjustOnPeriodic())
  {
    return;
  }

  double tol = Precision::PConfusion();

  if (mySameParameters)
  {
    myP1 = AdjustCase(myP1, myO1, myFirst, myPeriod, tol, myCas1);
    myP2 = AdjustCase(myP2, myO2, myFirst, myPeriod, tol, myCas2);
  }
  else if (myO1 != myO2)
  {
    if (myO1 == TopAbs_FORWARD)
    {
      myP2 = AdjustCase(myP2, myO2, myP1, myPeriod, tol, myCas2);
    }
    if (myO2 == TopAbs_FORWARD)
    {
      myP1 = AdjustCase(myP1, myO1, myP2, myPeriod, tol, myCas1);
    }
  }

}

//=================================================================================================

bool TopOpeBRepBuild_PaveClassifier::ToAdjustOnPeriodic() const
{
  bool toadjust = ((mySameParameters) || (myO1 != myO2));
  return toadjust;
}

//=================================================================================================

TopAbs_State TopOpeBRepBuild_PaveClassifier::CompareOnPeriodic()
{
  TopAbs_State state;

  if (ToAdjustOnPeriodic())
  {
    state = CompareOnNonPeriodic();
  }
  else if (myO1 == TopAbs_FORWARD)
  {
    state  = TopAbs_OUT;
    myCas1 = myCas2 = 5;
  }
  else if (myO1 == TopAbs_REVERSED)
  {
    state  = TopAbs_OUT;
    myCas1 = myCas2 = 6;
  }
  else
  {
    state  = TopAbs_OUT;
    myCas1 = myCas2 = 7;
  }

  return state;
}

//=================================================================================================

TopAbs_State TopOpeBRepBuild_PaveClassifier::Compare(const occ::handle<TopOpeBRepBuild_Loop>& L1,
                                                     const occ::handle<TopOpeBRepBuild_Loop>& L2)
{
  const occ::handle<TopOpeBRepBuild_Pave>& PV1 = *((occ::handle<TopOpeBRepBuild_Pave>*)&(L1));
  const occ::handle<TopOpeBRepBuild_Pave>& PV2 = *((occ::handle<TopOpeBRepBuild_Pave>*)&(L2));

  myCas1 = myCas2 = 0; // debug
  myO1            = PV1->Vertex().Orientation();
  myO2            = PV2->Vertex().Orientation();
  myP1            = PV1->Parameter();
  myP2            = PV2->Parameter();

  if (myEdgePeriodic && ToAdjustOnPeriodic())
  {
    AdjustOnPeriodic();
  }

  TopAbs_State state;
  if (myEdgePeriodic)
  {
    state = CompareOnPeriodic();
  }
  else
  {
    state = CompareOnNonPeriodic();
  }

  return state;
}

//=================================================================================================

void TopOpeBRepBuild_PaveClassifier::SetFirstParameter(const double P)
{
  myFirst          = P;
  mySameParameters = true;

}

//=================================================================================================

void TopOpeBRepBuild_PaveClassifier::ClosedVertices(const bool Closed)
{
  myClosedVertices = Closed;
}
