// Created on: 1993-11-18
// Created by: Isabelle GRIGNON
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

// Modified by isg, Thu Mar 17 09:21:31 1994

#include <BRep_Tool.hxx>
#include <ChFiDS_Spine.hxx>
#include <ElCLib.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <gp_Circ.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>
#include <TopExp.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ChFiDS_Spine, Standard_Transient)

//=================================================================================================

ChFiDS_Spine::ChFiDS_Spine()
    : splitdone(false),
      myMode(ChFiDS_ClassicChamfer),
      indexofcurve(0),
      myTypeOfConcavity(ChFiDS_Other),
      firstState(ChFiDS_OnSame),
      lastState(ChFiDS_OnSame),
      tolesp(Precision::Confusion()),
      firstparam(0.0),
      lastparam(0.0),
      firstprolon(false),
      lastprolon(false),
      firstistgt(false),
      lastistgt(false),
      firsttgtpar(0.0),
      lasttgtpar(0.0),
      hasfirsttgt(false),
      haslasttgt(false),
      valref(0.0),
      hasref(false),
      errorstate(ChFiDS_Ok)
{
}

//=================================================================================================

ChFiDS_Spine::ChFiDS_Spine(const double Tol)
    : splitdone(false),
      myMode(ChFiDS_ClassicChamfer),
      indexofcurve(0),
      myTypeOfConcavity(ChFiDS_Other),
      firstState(ChFiDS_OnSame),
      lastState(ChFiDS_OnSame),
      tolesp(Tol),
      firstparam(0.0),
      lastparam(0.0),
      firstprolon(false),
      lastprolon(false),
      firstistgt(false),
      lastistgt(false),
      firsttgtpar(0.0),
      lasttgtpar(0.0),
      hasfirsttgt(false),
      haslasttgt(false),
      valref(0.0),
      hasref(false),
      errorstate(ChFiDS_Ok)
{
}

//=================================================================================================

void ChFiDS_Spine::AppendElSpine(const occ::handle<ChFiDS_ElSpine>& Els)
{
  elspines.Append(Els);
}

//=================================================================================================

void ChFiDS_Spine::AppendOffsetElSpine(const occ::handle<ChFiDS_ElSpine>& Els)
{
  offset_elspines.Append(Els);
}

//=================================================================================================

occ::handle<ChFiDS_ElSpine> ChFiDS_Spine::ElSpine(const TopoDS_Edge& E) const
{
  return ElSpine(Index(E));
}

occ::handle<ChFiDS_ElSpine> ChFiDS_Spine::ElSpine(const int IE) const
{
  double wmil = 0.5 * (FirstParameter(IE) + LastParameter(IE));
  if (IsPeriodic())
    wmil = ElCLib::InPeriod(wmil, FirstParameter(), LastParameter());
  return ElSpine(wmil);
}

occ::handle<ChFiDS_ElSpine> ChFiDS_Spine::ElSpine(const double W) const
{
  if (elspines.Extent() == 1)
    return elspines.First();
  else
  {
    NCollection_List<occ::handle<ChFiDS_ElSpine>>::Iterator It(elspines);
    for (; It.More(); It.Next())
    {
      occ::handle<ChFiDS_ElSpine> cur = It.Value();
      double                      uf  = cur->FirstParameter();
      double                      ul  = cur->LastParameter();
      if (uf <= W && W <= ul)
        return cur;
    }
    return occ::handle<ChFiDS_ElSpine>();
  }
}

//=================================================================================================

NCollection_List<occ::handle<ChFiDS_ElSpine>>& ChFiDS_Spine::ChangeElSpines()
{
  return elspines;
}

//=================================================================================================

NCollection_List<occ::handle<ChFiDS_ElSpine>>& ChFiDS_Spine::ChangeOffsetElSpines()
{
  return offset_elspines;
}

//=================================================================================================

void ChFiDS_Spine::SplitDone(const bool B)
{
  splitdone = B;
}

//=================================================================================================

bool ChFiDS_Spine::SplitDone() const
{
  return splitdone;
}

//=================================================================================================

void ChFiDS_Spine::Reset(const bool AllData)
{
  splitdone = false;
  // if(AllData && !isconstant.IsNull()) isconstant->ChangeArray1().Init(0);
  elspines.Clear();
  if (AllData)
  {
    firstparam  = 0.;
    lastparam   = abscissa->Value(abscissa->Upper());
    firstprolon = lastprolon = false;
  }
}

//=================================================================================================

double ChFiDS_Spine::FirstParameter() const
{
  if (firstprolon)
    return firstparam;
  return 0.;
}

//=================================================================================================

double ChFiDS_Spine::LastParameter() const
{
  if (lastprolon)
    return lastparam;
  return abscissa->Value(abscissa->Upper());
}

//=================================================================================================

void ChFiDS_Spine::SetFirstParameter(const double Par)
{
#ifdef OCCT_DEBUG
  if (Par >= Precision::Confusion())
    std::cout << "Interior extension at the start of guideline" << std::endl;
  if (IsPeriodic())
    std::cout << "WARNING!!! Extension on periodic guideline." << std::endl;
#endif
  firstprolon = true;
  firstparam  = Par;
}

//=================================================================================================

void ChFiDS_Spine::SetLastParameter(const double Par)
{
#ifdef OCCT_DEBUG
  double lll = abscissa->Value(abscissa->Upper());
  if ((Par - lll) <= -Precision::Confusion())
    std::cout << "Interior extension at the end of guideline" << std::endl;
  if (IsPeriodic())
    std::cout << "WARNING!!! Extension on periodic guideline." << std::endl;
#endif
  lastprolon = true;
  lastparam  = Par;
}

//=================================================================================================

double ChFiDS_Spine::FirstParameter(const int IndexSpine) const
{
  if (IndexSpine == 1)
    return 0.;
  return abscissa->Value(IndexSpine - 1);
}

//=================================================================================================

double ChFiDS_Spine::LastParameter(const int IndexSpine) const
{
  return abscissa->Value(IndexSpine);
}

//=================================================================================================

double ChFiDS_Spine::Length(const int IndexSpine) const
{
  if (IndexSpine == 1)
    return abscissa->Value(IndexSpine);
  return abscissa->Value(IndexSpine) - abscissa->Value(IndexSpine - 1);
}

//=================================================================================================

bool ChFiDS_Spine::IsPeriodic() const
{
  return (firstState == ChFiDS_Closed);
}

//=================================================================================================

bool ChFiDS_Spine::IsClosed() const
{
  return (FirstVertex().IsSame(LastVertex()));
}

//=================================================================================================

TopoDS_Vertex ChFiDS_Spine::FirstVertex() const
{
  TopoDS_Edge E = TopoDS::Edge(spine.First());
  if (E.Orientation() == TopAbs_FORWARD)
    return TopExp::FirstVertex(E);
  return TopExp::LastVertex(E);
}

//=================================================================================================

TopoDS_Vertex ChFiDS_Spine::LastVertex() const
{
  TopoDS_Edge E = TopoDS::Edge(spine.Last());
  if (E.Orientation() == TopAbs_FORWARD)
    return TopExp::LastVertex(E);
  return TopExp::FirstVertex(E);
}

//=================================================================================================

double ChFiDS_Spine::Absc(const TopoDS_Vertex& V) const
{
  TopoDS_Vertex d, f;
  TopoDS_Edge   E;
  for (int i = 1; i <= spine.Length(); i++)
  {
    E = TopoDS::Edge(spine.Value(i));
    TopExp::Vertices(E, d, f);
    if (d.IsSame(V) && E.Orientation() == TopAbs_FORWARD)
    {
      return FirstParameter(i);
    }
    if (d.IsSame(V) && E.Orientation() == TopAbs_REVERSED)
    {
      return LastParameter(i);
    }
    if (f.IsSame(V) && E.Orientation() == TopAbs_FORWARD)
    {
      return LastParameter(i);
    }
    if (f.IsSame(V) && E.Orientation() == TopAbs_REVERSED)
    {
      return FirstParameter(i);
    }
  }
  return -1.;
}

//=================================================================================================

double ChFiDS_Spine::Period() const
{
  if (!IsPeriodic())
    throw Standard_Failure("Non-periodic Spine");
  return abscissa->Value(abscissa->Upper());
}

//=================================================================================================

double ChFiDS_Spine::Resolution(const double R3d) const
{
  return R3d;
}

//=================================================================================================

void ChFiDS_Spine::SetFirstTgt(const double W)
{
  if (IsPeriodic())
    throw Standard_Failure("No extension by tangent on periodic contours");
#ifdef OCCT_DEBUG
  if (W >= Precision::Confusion())
    std::cout << "Interior extension at start of the guideline" << std::endl;
#endif
  // The flag is suspended if is already positioned to avoid
  // stopping d1
  hasfirsttgt = false;
  D1(W, firstori, firsttgt);
  // and it is reset.
  hasfirsttgt = true;
  firsttgtpar = W;
}

//=================================================================================================

void ChFiDS_Spine::SetLastTgt(const double W)
{
  if (IsPeriodic())
    throw Standard_Failure("No extension by tangent periodic contours");

#ifdef OCCT_DEBUG
  double L = W - abscissa->Value(abscissa->Upper());
  if (L <= -Precision::Confusion())
    std::cout << "Interior extension at the end of guideline" << std::endl;
#endif
  // The flag is suspended if is already positioned to avoid
  // stopping d1
  haslasttgt = false;
  D1(W, lastori, lasttgt);
  // and it is reset.
  haslasttgt = true;
  lasttgtpar = W;
}

//=================================================================================================

bool ChFiDS_Spine::HasFirstTgt() const
{
  return hasfirsttgt;
}

//=================================================================================================

bool ChFiDS_Spine::HasLastTgt() const
{
  return haslasttgt;
}

//=================================================================================================

void ChFiDS_Spine::SetReference(const double W)
{
  hasref     = true;
  double lll = abscissa->Value(abscissa->Upper());
  if (IsPeriodic())
    valref = ElCLib::InPeriod(W, 0., lll);
  else
    valref = W;
}

//=================================================================================================

void ChFiDS_Spine::SetReference(const int I)
{
  hasref = true;
  if (I == 1)
    valref = abscissa->Value(1) * 0.5;
  else
    valref = (abscissa->Value(I) + abscissa->Value(I - 1)) * 0.5;
}

//=================================================================================================

int ChFiDS_Spine::Index(const double W, const bool Forward) const
{
  int    ind, len = abscissa->Length();
  double par = W, last = abscissa->Value(abscissa->Upper());
  double f = 0., l = 0., t = std::max(tolesp, Precision::Confusion());

  if (IsPeriodic() && std::abs(par) >= t && std::abs(par - last) >= t)
    par = ElCLib::InPeriod(par, 0., last);

  for (ind = 1; ind <= len; ind++)
  {
    f = l;
    l = abscissa->Value(ind);
    if (par < l || ind == len)
      break;
  }
  if (Forward && ind < len && std::abs(par - l) < t)
    ind++;
  else if (!Forward && ind > 1 && std::abs(par - f) < t)
    ind--;
  else if (Forward && IsPeriodic() && ind == len && std::abs(par - l) < t)
    ind = 1;
  else if (!Forward && IsPeriodic() && ind == 1 && std::abs(par - f) < t)
    ind = len;
  return ind;
}

//=================================================================================================

int ChFiDS_Spine::Index(const TopoDS_Edge& E) const
{
  for (int IE = 1; IE <= spine.Length(); IE++)
  {
    if (E.IsSame(spine.Value(IE)))
      return IE;
  }
  return 0;
}

//=================================================================================================

void ChFiDS_Spine::UnsetReference()
{
  hasref = false;
}

//=================================================================================================

void ChFiDS_Spine::Load()
{
  if (!abscissa.IsNull())
  {
#ifdef OCCT_DEBUG
    std::cout << "new load of CE" << std::endl;
#endif
  }
  int len   = spine.Length();
  abscissa  = new NCollection_HArray1<double>(1, len);
  double a1 = 0.;
  for (int i = 1; i <= len; i++)
  {
    myCurve.Initialize(TopoDS::Edge(spine.Value(i)));
    a1 += GCPnts_AbscissaPoint::Length(myCurve);
    abscissa->SetValue(i, a1);
  }
  indexofcurve = 1;
  myCurve.Initialize(TopoDS::Edge(spine.Value(1)));

  // Here, we should update tolesp according to curve parameter range
  // if tolesp candidate less than default initial value.
  const double umin = FirstParameter();
  const double umax = LastParameter();

  double new_tolesp = 5.0e-5 * (umax - umin);
  if (tolesp > new_tolesp)
  {
    tolesp = new_tolesp;
  }
}

//=================================================================================================

double ChFiDS_Spine::Absc(const double U)
{
  return Absc(U, indexofcurve);
}

//=================================================================================================

double ChFiDS_Spine::Absc(const double U, const int I)
{

  if (indexofcurve != I)
  {
    void* p                          = (void*)this;
    ((ChFiDS_Spine*)p)->indexofcurve = I;
    ((ChFiDS_Spine*)p)->myCurve.Initialize(TopoDS::Edge(spine.Value(I)));
  }
  double L = FirstParameter(I);
  if (spine.Value(I).Orientation() == TopAbs_REVERSED)
  {
    L += GCPnts_AbscissaPoint::Length(myCurve, U, myCurve.LastParameter());
  }
  else
  {
    L += GCPnts_AbscissaPoint::Length(myCurve, myCurve.FirstParameter(), U);
  }
  return L;
}

//=================================================================================================

void ChFiDS_Spine::Parameter(const double AbsC, double& U, const bool Oriented)
{
  int Index;
  for (Index = 1; Index < abscissa->Length(); Index++)
  {
    if (AbsC < abscissa->Value(Index))
      break;
  }
  Parameter(Index, AbsC, U, Oriented);
}

//=================================================================================================

void ChFiDS_Spine::Parameter(const int Index, const double AbsC, double& U, const bool Oriented)
{

  if (Index != indexofcurve)
  {
    void* p                          = (void*)this;
    ((ChFiDS_Spine*)p)->indexofcurve = Index;
    ((ChFiDS_Spine*)p)->myCurve.Initialize(TopoDS::Edge(spine.Value(Index)));
  }
  double             L;
  TopAbs_Orientation Or = spine.Value(Index).Orientation();
  if (Or == TopAbs_REVERSED)
  {
    L = abscissa->Value(indexofcurve) - AbsC;
  }
  else if (indexofcurve == 1)
  {
    L = AbsC;
  }
  else
  {
    L = AbsC - abscissa->Value(indexofcurve - 1);
  }
  double t    = L / Length(Index);
  double uapp = (1. - t) * myCurve.FirstParameter() + t * myCurve.LastParameter();
  //  GCPnts_AbscissaPoint GCP;
  //  GCP.Perform(myCurve,L,myCurve.FirstParameter(),uapp,BRep_Tool::Tolerance(myCurve.Edge()));
  GCPnts_AbscissaPoint GCP(myCurve, L, myCurve.FirstParameter(), uapp);
  U = GCP.Parameter();
  if (Or == TopAbs_REVERSED && Oriented)
  {
    U = (myCurve.LastParameter() + myCurve.FirstParameter()) - U;
  }
}

//=================================================================================================

void ChFiDS_Spine::Prepare(double& L, int& Ind) const
{
  double tol  = std::max(tolesp, Precision::Confusion());
  double last = abscissa->Value(abscissa->Upper());
  int    len  = abscissa->Length();
  if (IsPeriodic() && std::abs(L) >= tol && std::abs(L - last) >= tol)
    L = ElCLib::InPeriod(L, 0., last);

  if (hasfirsttgt && (L <= firsttgtpar))
  {
    if (hasref && valref >= L && std::abs(L - firsttgtpar) <= tol)
    {
      Ind = Index(L);
    }
    else
    {
      Ind = -1;
      L -= firsttgtpar;
    }
  }
  else if (L <= 0.)
  {
    Ind = 1;
  }
  else if (haslasttgt && (L >= lasttgtpar))
  {
    if (hasref && valref <= L && std::abs(L - lasttgtpar) <= tol)
    {
      Ind = Index(L);
    }
    else
    {
      Ind = len + 1;
      L -= lasttgtpar;
    }
  }
  else if (L >= last)
  {
    Ind = len;
  }
  else
  {
    for (Ind = 1; Ind < len; Ind++)
    {
      if (L < abscissa->Value(Ind))
        break;
    }
    if (hasref)
    {
      if (L >= valref && Ind != 1)
      {
        if (std::abs(L - abscissa->Value(Ind - 1)) <= Precision::Confusion())
          Ind--;
      }
      else if (L <= valref && Ind != len)
      {
        if (std::abs(L - abscissa->Value(Ind)) <= Precision::Confusion())
          Ind++;
      }
    }
  }
  if (Ind >= 1 && Ind <= len)
  {
    if (spine.Value(Ind).Orientation() == TopAbs_REVERSED)
    {
      L = abscissa->Value(Ind) - L;
    }
    else if (Ind != 1)
    {
      L -= abscissa->Value(Ind - 1);
    }
  }
}

//=================================================================================================

gp_Pnt ChFiDS_Spine::Value(const double AbsC)
{

  int    Index;
  double L = AbsC;

  Prepare(L, Index);

  if (Index == -1)
  {
    gp_Pnt Pp = firstori;
    gp_Vec Vp = firsttgt;
    Vp.Multiply(L);
    Pp.Translate(Vp);
    return Pp;
  }
  else if (Index == (abscissa->Length() + 1))
  {
    gp_Pnt Pp = lastori;
    gp_Vec Vp = lasttgt;
    Vp.Multiply(L);
    Pp.Translate(Vp);
    return Pp;
  }
  if (Index != indexofcurve)
  {
    void* p                          = (void*)this;
    ((ChFiDS_Spine*)p)->indexofcurve = Index;
    ((ChFiDS_Spine*)p)->myCurve.Initialize(TopoDS::Edge(spine.Value(Index)));
  }
  double t    = L / Length(Index);
  double uapp = (1. - t) * myCurve.FirstParameter() + t * myCurve.LastParameter();
  //  GCPnts_AbscissaPoint GCP;
  //  GCP.Perform(myCurve,L,myCurve.FirstParameter(),uapp,BRep_Tool::Tolerance(myCurve.Edge()));
  GCPnts_AbscissaPoint GCP(myCurve, L, myCurve.FirstParameter(), uapp);
  return myCurve.Value(GCP.Parameter());
}

//=================================================================================================

void ChFiDS_Spine::D0(const double AbsC, gp_Pnt& P)
{
  P = Value(AbsC);
}

//=================================================================================================

void ChFiDS_Spine::D1(const double AbsC, gp_Pnt& P, gp_Vec& V1)
{
  int    Index;
  double L = AbsC;

  Prepare(L, Index);

  if (Index == -1)
  {
    P         = firstori;
    V1        = firsttgt;
    gp_Vec Vp = firsttgt;
    Vp.Multiply(L);
    P.Translate(Vp);
  }
  else if (Index == (abscissa->Length() + 1))
  {
    P         = lastori;
    V1        = lasttgt;
    gp_Vec Vp = lasttgt;
    Vp.Multiply(L);
    P.Translate(Vp);
  }
  else
  {
    if (Index != indexofcurve)
    {
      void* p                          = (void*)this;
      ((ChFiDS_Spine*)p)->indexofcurve = Index;
      ((ChFiDS_Spine*)p)->myCurve.Initialize(TopoDS::Edge(spine.Value(Index)));
    }
    double t    = L / Length(Index);
    double uapp = (1. - t) * myCurve.FirstParameter() + t * myCurve.LastParameter();
    //    GCPnts_AbscissaPoint GCP;
    //    GCP.Perform(myCurve,L,myCurve.FirstParameter(),uapp,BRep_Tool::Tolerance(myCurve.Edge()));
    GCPnts_AbscissaPoint GCP(myCurve, L, myCurve.FirstParameter(), uapp);
    myCurve.D1(GCP.Parameter(), P, V1);
    double D1 = 1. / V1.Magnitude();
    if (spine.Value(Index).Orientation() == TopAbs_REVERSED)
      D1 = -D1;
    V1.Multiply(D1);
  }
}

//=================================================================================================

void ChFiDS_Spine::D2(const double AbsC, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2)
{

  int    Index;
  double L = AbsC;

  Prepare(L, Index);

  if (Index == -1)
  {
    P  = firstori;
    V1 = firsttgt;
    V2.SetCoord(0., 0., 0.);
    gp_Vec Vp = firsttgt;
    Vp.Multiply(L);
    P.Translate(Vp);
  }
  else if (Index == (abscissa->Length() + 1))
  {
    P  = lastori;
    V1 = lasttgt;
    V2.SetCoord(0., 0., 0.);
    gp_Vec Vp = lasttgt;
    Vp.Multiply(L);
    P.Translate(Vp);
  }
  else
  {
    if (Index != indexofcurve)
    {
      void* p                          = (void*)this;
      ((ChFiDS_Spine*)p)->indexofcurve = Index;
      ((ChFiDS_Spine*)p)->myCurve.Initialize(TopoDS::Edge(spine.Value(Index)));
    }
    double t    = L / Length(Index);
    double uapp = (1. - t) * myCurve.FirstParameter() + t * myCurve.LastParameter();
    //    GCPnts_AbscissaPoint GCP;
    //    GCP.Perform(myCurve,L,myCurve.FirstParameter(),uapp,BRep_Tool::Tolerance(myCurve.Edge()));
    GCPnts_AbscissaPoint GCP(myCurve, L, myCurve.FirstParameter(), uapp);
    myCurve.D2(GCP.Parameter(), P, V1, V2);
    double N1 = V1.SquareMagnitude();
    double D2 = -(V1.Dot(V2)) * (1. / N1) * (1. / N1);
    V2.Multiply(1. / N1);
    N1        = std::sqrt(N1);
    gp_Vec Va = V1.Multiplied(D2);
    V2.Add(Va);
    double D1 = 1. / N1;
    if (spine.Value(Index).Orientation() == TopAbs_REVERSED)
      D1 = -D1;
    V1.Multiply(D1);
  }
}

//=================================================================================================

void ChFiDS_Spine::SetCurrent(const int Index)
{
  if (Index != indexofcurve)
  {
    indexofcurve = Index;
    myCurve.Initialize(TopoDS::Edge(spine.Value(indexofcurve)));
  }
}

//=================================================================================================

const BRepAdaptor_Curve& ChFiDS_Spine::CurrentElementarySpine(const int Index)
{
  if (Index != indexofcurve)
  {
    indexofcurve = Index;
    myCurve.Initialize(TopoDS::Edge(spine.Value(indexofcurve)));
  }
  return myCurve;
}

//=================================================================================================

GeomAbs_CurveType ChFiDS_Spine::GetType() const
{
  return myCurve.GetType();
}

//=================================================================================================

gp_Lin ChFiDS_Spine::Line() const
{
  gp_Lin LL(myCurve.Line());
  if (spine.Value(indexofcurve).Orientation() == TopAbs_REVERSED)
  {
    LL.Reverse();
    LL.SetLocation(myCurve.Value(myCurve.LastParameter()));
  }
  else
  {
    LL.SetLocation(myCurve.Value(myCurve.FirstParameter()));
  }
  return LL;
}

//=================================================================================================

gp_Circ ChFiDS_Spine::Circle() const
{
  gp_Ax2 Ac = myCurve.Circle().Position();
  gp_Dir Dc(gp_Vec(Ac.Location(), myCurve.Value(myCurve.FirstParameter())));
  gp_Dir ZZ(Ac.Direction());

  if (spine.Value(indexofcurve).Orientation() == TopAbs_REVERSED)
  {
    Dc = gp_Dir(gp_Vec(Ac.Location(), myCurve.Value(myCurve.LastParameter())));
    ZZ.Reverse();
  }
  gp_Ax2 A(Ac.Location(), ZZ, Dc);
  return gp_Circ(A, myCurve.Circle().Radius());
}

//=======================================================================
// function : SetErrorStatus
// purpose  : met a jour le statut d'erreur
//=======================================================================
void ChFiDS_Spine::SetErrorStatus(const ChFiDS_ErrorStatus state)
{
  errorstate = state;
}

//=======================================================================
// function : ErrorStatus
// purpose  : renvoie le statut d'erreur concernant la spine
//=======================================================================

ChFiDS_ErrorStatus ChFiDS_Spine::ErrorStatus() const
{
  return errorstate;
}
