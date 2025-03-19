// Created on: 1993-11-10
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

#include <TopOpeBRep_LineInter.hxx>

#include <Adaptor2d_Curve2d.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp_Pnt.hxx>
#include <IntPatch_ALine.hxx>
#include <IntPatch_ALineToWLine.hxx>
#include <IntPatch_GLine.hxx>
#include <IntPatch_IType.hxx>
#include <IntPatch_Line.hxx>
#include <IntPatch_RLine.hxx>
#include <IntPatch_SequenceOfLine.hxx>
#include <IntPatch_WLine.hxx>
#include <Standard_ProgramError.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRep.hxx>
#include <TopOpeBRep_Bipoint.hxx>
#include <TopOpeBRep_FFTransitionTool.hxx>
#include <TopOpeBRep_VPointInter.hxx>
#include <TopOpeBRep_VPointInterIterator.hxx>
#include <TopOpeBRep_WPointInter.hxx>
#include <TopOpeBRepDS_Transition.hxx>

#include <BRepAdaptor_Surface.hxx>

#ifdef OCCT_DEBUG
extern Standard_Boolean TopOpeBRep_GetcontextALWLNBP(Standard_Integer&);
extern Standard_Boolean TopOpeBRep_GettraceCONIC();
#endif

//-----------------------------------------------------------------------
static void FUN_ALINETOWLINE(const Handle(IntPatch_ALine)&      AL,
                             const Handle(BRepAdaptor_Surface)& surf1,
                             const Handle(BRepAdaptor_Surface)& surf2,
                             IntPatch_SequenceOfLine&           theLines)
{
  Standard_Integer nbpointsmax = 200;
#ifdef OCCT_DEBUG
  Standard_Integer newnbp;
  if (TopOpeBRep_GetcontextALWLNBP(newnbp))
    nbpointsmax = newnbp;
#endif
  IntPatch_ALineToWLine AToL(surf1, surf2, nbpointsmax);

  AToL.MakeWLine(AL, theLines);
}

//=================================================================================================

void TopOpeBRep_LineInter::SetLine(const Handle(IntPatch_Line)& L,
                                   const BRepAdaptor_Surface&   S1,
                                   const BRepAdaptor_Surface&   S2)
{
  // load line according to its type
  myIL                = L;
  IntPatch_IType type = L->ArcType();
  switch (type)
  {
    case IntPatch_Analytic:
      myTypeLineCurve = TopOpeBRep_ANALYTIC;
      break;
    case IntPatch_Restriction:
      myTypeLineCurve = TopOpeBRep_RESTRICTION;
      break;
    case IntPatch_Walking:
      myTypeLineCurve = TopOpeBRep_WALKING;
      break;
    case IntPatch_Lin:
      myTypeLineCurve = TopOpeBRep_LINE;
      break;
    case IntPatch_Circle:
      myTypeLineCurve = TopOpeBRep_CIRCLE;
      break;
    case IntPatch_Ellipse:
      myTypeLineCurve = TopOpeBRep_ELLIPSE;
      break;
    case IntPatch_Parabola:
      myTypeLineCurve = TopOpeBRep_PARABOLA;
      break;
    case IntPatch_Hyperbola:
      myTypeLineCurve = TopOpeBRep_HYPERBOLA;
      break;
    default:
      myTypeLineCurve = TopOpeBRep_OTHERTYPE;
      SetOK(Standard_False);
      break;
  }

  switch (type)
  {
    case IntPatch_Analytic:
      myILA = Handle(IntPatch_ALine)::DownCast(L);
      break;
    case IntPatch_Restriction:
      myILR = Handle(IntPatch_RLine)::DownCast(L);
      break;
    case IntPatch_Walking:
      myILW = Handle(IntPatch_WLine)::DownCast(L);
      break;
    default: //"geometric" line
      myILG = Handle(IntPatch_GLine)::DownCast(L);
      break;
  }

  // transform an analytic line to a walking line
  if (myTypeLineCurve == TopOpeBRep_ANALYTIC)
  {
    IntPatch_SequenceOfLine aSLin;
    FUN_ALINETOWLINE(myILA, new BRepAdaptor_Surface(S1), new BRepAdaptor_Surface(S2), aSLin);

    if (aSLin.Length() > 0)
      myILW = Handle(IntPatch_WLine)::DownCast(aSLin.Value(1));

    myTypeLineCurve = TopOpeBRep_WALKING;
  }

  // number of points found on restriction(s)
  Standard_Integer n = 0;
  switch (myTypeLineCurve)
  {
    case TopOpeBRep_ANALYTIC:
      n = myILA->NbVertex();
      break;
    case TopOpeBRep_RESTRICTION:
      n = myILR->NbVertex();
      break;
    case TopOpeBRep_WALKING:
      n = myILW->NbVertex();
      break;
    case TopOpeBRep_LINE:
      n = myILG->NbVertex();
      break;
    case TopOpeBRep_CIRCLE:
      n = myILG->NbVertex();
      break;
    case TopOpeBRep_ELLIPSE:
      n = myILG->NbVertex();
      break;
    case TopOpeBRep_PARABOLA:
      n = myILG->NbVertex();
      break;
    case TopOpeBRep_HYPERBOLA:
      n = myILG->NbVertex();
      break;
    default:
      SetOK(Standard_False);
      break;
  }
  myNbVPoint = n;

  // prepare VPoints from intersection points
  myHAVP = new TopOpeBRep_HArray1OfVPointInter(0, n);
  for (Standard_Integer i = 1; i <= n; i++)
  {
    TopOpeBRep_VPointInter& VP = myHAVP->ChangeValue(i);
    switch (myTypeLineCurve)
    {
      case TopOpeBRep_ANALYTIC:
        VP.SetPoint(myILA->Vertex(i));
        break;
      case TopOpeBRep_RESTRICTION:
        VP.SetPoint(myILR->Vertex(i));
        break;
      case TopOpeBRep_WALKING:
        VP.SetPoint(myILW->Vertex(i));
        break;
      default:
        VP.SetPoint(myILG->Vertex(i));
        break;
    }
    VP.Index(i);
  }
}

//=================================================================================================

const TopOpeBRep_VPointInter& TopOpeBRep_LineInter::VPoint(const Standard_Integer I) const
{
  return myHAVP->Value(I);
}

//=================================================================================================

TopOpeBRep_VPointInter& TopOpeBRep_LineInter::ChangeVPoint(const Standard_Integer I)
{
  return myHAVP->ChangeValue(I);
}

//=================================================================================================

void TopOpeBRep_LineInter::SetINL()
{
  TopOpeBRep_VPointInterIterator VPI(*this);
  if (!VPI.More())
  {
    myINL = Standard_False;
    return;
  }
  const Standard_Real p0 = VPI.CurrentVP().ParameterOnLine();
  VPI.Next();
  if (!VPI.More())
  {
    myINL = Standard_True;
    return;
  }
  for (; VPI.More(); VPI.Next())
  {
    const Standard_Real p = VPI.CurrentVP().ParameterOnLine();
    if (p != p0)
    {
      myINL = Standard_False;
      return;
    }
  }
  myINL = Standard_True;
}

//=================================================================================================

void TopOpeBRep_LineInter::SetIsVClosed()
{
  if (myINL)
  {
    myIsVClosed = Standard_False;
    return;
  }

  /*Standard_Boolean newV = Standard_True;
  if (!newV) {
    if (myTypeLineCurve != TopOpeBRep_WALKING) {
      myIsVClosed = Standard_False;
      return;
    }
  }*/

  TopOpeBRep_VPointInterIterator VPI(*this);
  Standard_Integer               nV   = myNbVPoint;
  Standard_Real                  pmin = RealLast(), pmax = RealFirst();
  Standard_Integer               imin = 0, imax = 0; // index of IsOnArc VPoints
  if (nV >= 2)
  {
    for (; VPI.More(); VPI.Next())
    {
      const TopOpeBRep_VPointInter& P = VPI.CurrentVP();
      if (!P.IsInternal())
      {
        const Standard_Integer i = VPI.CurrentVPIndex();
        const Standard_Real    p = P.ParameterOnLine();
        if (p < pmin)
        {
          imin = i;
          pmin = p;
        }
        if (p > pmax)
        {
          imax = i;
          pmax = p;
        }
      }
    }
    if (imax == 0)
    { // no VPoint on restriction found
      myIsVClosed = Standard_True;
      return;
    }

    const TopOpeBRep_VPointInter& P1 = VPoint(imin);
    const TopOpeBRep_VPointInter& P2 = VPoint(imax);

    const gp_Pnt& pp1 = P1.Value();
    const gp_Pnt& pp2 = P2.Value();

    const Standard_Real tol1 = P1.Tolerance();
    const Standard_Real tol2 = P2.Tolerance();
    const Standard_Real tol  = Max(tol1, tol2);

    myIsVClosed = pp1.IsEqual(pp2, tol);
  }
  else
  {
    SetOK(Standard_False);
    myIsVClosed = Standard_False;
  }
}

//=================================================================================================

void TopOpeBRep_LineInter::SetHasVPonR()
{
  myHasVPonR = Standard_False;
  TopOpeBRep_VPointInterIterator VPI(*this);
  for (; VPI.More(); VPI.Next())
  {
    const TopOpeBRep_VPointInter& P = VPI.CurrentVP();
    if (P.IsOnDomS1() || P.IsOnDomS2())
    {
      myHasVPonR = Standard_True;
      break;
    }
  }
}

//=================================================================================================

void TopOpeBRep_LineInter::SetVPBounds()
{
  myVPF = myVPL = myVPN = 0;
  myVPBDefined          = Standard_True;

  TopOpeBRep_VPointInterIterator VPI(*this);
  Standard_Integer               f = myNbVPoint + 1, l = 0, n = 0;

  for (; VPI.More(); VPI.Next())
  {
    if (VPI.CurrentVP().Keep())
    {
      n++;
      const Standard_Integer i = VPI.CurrentVPIndex();
      if (i < f)
        f = i;
      if (i > l)
        l = i;
    }
  }

  myVPF = f;
  myVPL = l;
  myVPN = n;
}

//=================================================================================================

void TopOpeBRep_LineInter::VPBounds(Standard_Integer& f,
                                    Standard_Integer& l,
                                    Standard_Integer& n) const
{
  if (!myVPBDefined)
  {
    TopOpeBRep_LineInter* p = (TopOpeBRep_LineInter*)this; // NYI deconst
    p->SetOK(Standard_False);
    f = l = n = 0;
    return;
  }
  f = myVPF;
  l = myVPL;
  n = myVPN;
}

//=================================================================================================

Standard_Boolean TopOpeBRep_LineInter::IsPeriodic() const
{
  switch (myTypeLineCurve)
  {
    case TopOpeBRep_CIRCLE:
    case TopOpeBRep_ELLIPSE:
      return Standard_True;
    default:
      break;
  }
  return Standard_False;
}

//=================================================================================================

Standard_Real TopOpeBRep_LineInter::Period() const
{
  Standard_Real aFirst = 0.0, aLast = 0.0;
  Bounds(aFirst, aLast);
  return (aLast - aFirst);
}

//=================================================================================================

void TopOpeBRep_LineInter::Bounds(Standard_Real& theFirst, Standard_Real& theLast) const
{
  theFirst = 0.0;
  theLast  = 0.0;
  if (myILG.IsNull())
  {
    TopOpeBRep_LineInter* aPtr = const_cast<TopOpeBRep_LineInter*>(this); // NYI deconst
    aPtr->SetOK(Standard_False);
    return;
  }

  if (IsPeriodic())
  {
    theLast = Curve()->Period();
  }

  if (myILG->HasFirstPoint())
  {
    theFirst = myILG->FirstPoint().ParameterOnLine();
  }

  if (myILG->HasLastPoint())
  {
    theLast = myILG->LastPoint().ParameterOnLine();
  }
}

//=================================================================================================

Standard_Boolean TopOpeBRep_LineInter::HasVInternal()
{
  TopOpeBRep_VPointInterIterator VPI(*this);
  for (; VPI.More(); VPI.Next())
  {
    if (VPI.CurrentVP().IsInternal())
      return Standard_True;
  }
  return Standard_False;
}

//=================================================================================================

Standard_Integer TopOpeBRep_LineInter::NbWPoint() const
{
  switch (myTypeLineCurve)
  {
    case TopOpeBRep_WALKING:
      return myILW->NbPnts();
    default:
      break;
  }
  return 0;
}

//=================================================================================================

const TopOpeBRep_WPointInter& TopOpeBRep_LineInter::WPoint(const Standard_Integer IW)
{
  switch (myTypeLineCurve)
  {
    case TopOpeBRep_RESTRICTION:
      myCurrentWP.Set(myILR->Point(IW));
      break;
    case TopOpeBRep_WALKING:
      myCurrentWP.Set(myILW->Point(IW));
      break;
    default:
      break;
  }
  return myCurrentWP;
}

//=================================================================================================

Handle(Geom_Curve) TopOpeBRep_LineInter::Curve() const
{
  // Build the 3d curve
  Handle(Geom_Curve) C3D;
  switch (myTypeLineCurve)
  {
    case TopOpeBRep_LINE:
      C3D = new Geom_Line(myILG->Line());
      break;
    case TopOpeBRep_CIRCLE:
      C3D = new Geom_Circle(myILG->Circle());
      break;
    case TopOpeBRep_ELLIPSE:
      C3D = new Geom_Ellipse(myILG->Ellipse());
      break;
    case TopOpeBRep_PARABOLA:
      C3D = new Geom_Parabola(myILG->Parabola());
      break;
    case TopOpeBRep_HYPERBOLA:
      C3D = new Geom_Hyperbola(myILG->Hyperbola());
      break;
    default:
      TopOpeBRep_LineInter* p = (TopOpeBRep_LineInter*)this; // NYI deconst
      p->SetOK(Standard_False);
      break;
  }
  return C3D;
}

//=================================================================================================

Handle(Geom_Curve) TopOpeBRep_LineInter::Curve(const Standard_Real parmin,
                                               const Standard_Real parmax) const
{
  // Build the trimmed 3d curve
  Handle(Geom_Curve)        C3D  = Curve();
  Handle(Geom_TrimmedCurve) TC3D = new Geom_TrimmedCurve(C3D, parmin, parmax);
#ifdef OCCT_DEBUG
  if (TopOpeBRep_GettraceCONIC())
  {
    std::cout << "TopOpeBRep_LineInter::Curve on a ";
    TopOpeBRep::Print(myTypeLineCurve, std::cout);
    std::cout << std::endl;
    std::cout << "  ... Trimmed from " << parmin << " to " << parmax << std::endl;
  }
#endif
  return TC3D;
}

//=================================================================================================

const TopoDS_Shape& TopOpeBRep_LineInter::Arc() const
{
  if (myTypeLineCurve == TopOpeBRep_RESTRICTION)
  {
    if (myILR->IsArcOnS1())
    {
      const Handle(Adaptor2d_Curve2d)& AHC2D = myILR->ArcOnS1();
      const BRepAdaptor_Curve2d&       BC2DP = *((BRepAdaptor_Curve2d*)AHC2D.get());
      const TopoDS_Shape&              S     = BC2DP.Edge();
      return S;
    }
    else
    {
      const Handle(Adaptor2d_Curve2d)& AHC2D = myILR->ArcOnS2();
      const BRepAdaptor_Curve2d&       BC2DP = *((BRepAdaptor_Curve2d*)AHC2D.get());
      const TopoDS_Shape&              S     = BC2DP.Edge();
      return S;
    }
  }
  else
    return myNullShape;
}

//=================================================================================================

Standard_Boolean TopOpeBRep_LineInter::ArcIsEdge(const Standard_Integer Index) const
{
  if (myTypeLineCurve == TopOpeBRep_RESTRICTION)
  {
    const Standard_Boolean r = myILR->IsArcOnS1();
    return (Index == 2 ? !r : r);
  }
  return Standard_False;
}

//=================================================================================================

Standard_Boolean TopOpeBRep_LineInter::HasFirstPoint() const
{
  if (myILG.IsNull())
    throw Standard_ProgramError("TopOpeBRep_LineInter::HasFirstPoint sur line != GLine");
  return myILG->HasFirstPoint();
}

//=================================================================================================

Standard_Boolean TopOpeBRep_LineInter::HasLastPoint() const
{
  if (myILG.IsNull())
    throw Standard_ProgramError("TopOpeBRep_LineInter::HasLastPoint sur line != GLine");
  return myILG->HasLastPoint();
}

//=================================================================================================

void TopOpeBRep_LineInter::ComputeFaceFaceTransition()
{
  TopAbs_Orientation F1ori = myF1.Orientation();
  TopAbs_Orientation F2ori = myF2.Orientation();
  myLineTonF1              = TopOpeBRep_FFTransitionTool::ProcessFaceTransition(*this, 1, F2ori);
  myLineTonF2              = TopOpeBRep_FFTransitionTool::ProcessFaceTransition(*this, 2, F1ori);
}

//=================================================================================================

const TopOpeBRepDS_Transition& TopOpeBRep_LineInter::FaceFaceTransition(
  const Standard_Integer I) const
{
  if (I == 1)
    return myLineTonF1;
  if (I == 2)
    return myLineTonF2;
  throw Standard_ProgramError("TopOpeBRep_LineInter::FaceFaceTransition");
}

//=================================================================================================

void TopOpeBRep_LineInter::DumpType() const
{
#ifdef OCCT_DEBUG
  TopOpeBRep::Print(myTypeLineCurve, std::cout);
#endif
}

//=================================================================================================

void TopOpeBRep_LineInter::DumpVPoint
#ifndef OCCT_DEBUG
  (const Standard_Integer, const TCollection_AsciiString&, const TCollection_AsciiString&) const
{
#else
  (const Standard_Integer         I,
   const TCollection_AsciiString& s1,
   const TCollection_AsciiString& s2) const
{
  const TopOpeBRep_VPointInter& VP = VPoint(I);
  const gp_Pnt&                 P  = VP.Value();
  std::cout << s1;
  std::cout << "L" << Index() << "P" << VP.Index();
  if (VP.Keep())
    std::cout << "K";
  std::cout << " " << P.X() << " " << P.Y() << " " << P.Z();
  std::cout << s2;
#endif
}

//=================================================================================================

void TopOpeBRep_LineInter::DumpBipoint
#ifndef OCCT_DEBUG
  (const TopOpeBRep_Bipoint&, const TCollection_AsciiString&, const TCollection_AsciiString&) const
{
#else
  (const TopOpeBRep_Bipoint&      bip,
   const TCollection_AsciiString& s1,
   const TCollection_AsciiString& s2) const
{
  Standard_Integer i1 = bip.I1();
  Standard_Integer i2 = bip.I2();
  std::cout << s1;
  std::cout << "(" << i1 << "," << i2 << ")";
  std::cout << s2;
#endif
}

//=================================================================================================

void TopOpeBRep_LineInter::SetOK(const Standard_Boolean B)
{
  myOK = B;
}

//=================================================================================================

void TopOpeBRep_LineInter::SetTraceIndex(const Standard_Integer exF1, const Standard_Integer exF2)
{
  myexF1 = exF1;
  myexF2 = exF2;
}

//=================================================================================================

void TopOpeBRep_LineInter::GetTraceIndex(Standard_Integer& exF1, Standard_Integer& exF2) const
{
  exF1 = myexF1;
  exF2 = myexF2;
}

//=================================================================================================

Standard_OStream& TopOpeBRep_LineInter::DumpLineTransitions(Standard_OStream& OS) const
{
#ifdef OCCT_DEBUG
  OS << "transition from f1 / f2 ";
  TopAbs::Print(myF2.Orientation(), OS);
  OS << "transition from f2 / f1 ";
  TopAbs::Print(myF1.Orientation(), OS);
#endif
  return OS;
}
