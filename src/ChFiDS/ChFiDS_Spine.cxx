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
#include <BRepAdaptor_Curve.hxx>
#include <ChFiDS_ErrorStatus.hxx>
#include <ChFiDS_HElSpine.hxx>
#include <ChFiDS_ListIteratorOfListOfHElSpine.hxx>
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

IMPLEMENT_STANDARD_RTTIEXT(ChFiDS_Spine,Standard_Transient)

//=======================================================================
//function : ChFiDS_Spine
//purpose  : 
//=======================================================================
ChFiDS_Spine::ChFiDS_Spine():
       splitdone(Standard_False),
       tolesp(Precision::Confusion()),
       firstprolon(Standard_False), 
       lastprolon(Standard_False),
       firstistgt(Standard_False), 
       lastistgt(Standard_False),
       hasfirsttgt(Standard_False), 
       haslasttgt(Standard_False),
       hasref(Standard_False)
{
}

ChFiDS_Spine::ChFiDS_Spine(const Standard_Real Tol):
       splitdone(Standard_False),
       tolesp(Tol),
       firstprolon(Standard_False), 
       lastprolon(Standard_False),
       firstistgt(Standard_False), 
       lastistgt(Standard_False),
       hasfirsttgt(Standard_False), 
       haslasttgt(Standard_False),
       hasref(Standard_False)
{
}

//=======================================================================
//function : AppendElSpine
//purpose  : 
//=======================================================================

void ChFiDS_Spine::AppendElSpine(const Handle(ChFiDS_HElSpine)& Els)
{
  elspines.Append(Els);
}

//=======================================================================
//function : ElSpine
//purpose  : 
//=======================================================================

Handle(ChFiDS_HElSpine) ChFiDS_Spine::ElSpine(const TopoDS_Edge& E) const 
{
  return ElSpine(Index(E));
}

Handle(ChFiDS_HElSpine) ChFiDS_Spine::ElSpine(const Standard_Integer IE) const 
{
  Standard_Real wmil = 0.5 * (FirstParameter(IE) + LastParameter(IE));
  if(IsPeriodic()) wmil = ElCLib::InPeriod(wmil,FirstParameter(),LastParameter());
  return ElSpine(wmil);
}

Handle(ChFiDS_HElSpine) ChFiDS_Spine::ElSpine(const Standard_Real W) const 
{
  if (elspines.Extent() == 1)
    return elspines.First();
  else
  {
    ChFiDS_ListIteratorOfListOfHElSpine It(elspines);
    for (; It.More(); It.Next()) {
      Handle(ChFiDS_HElSpine) cur = It.Value();
      Standard_Real uf = cur->FirstParameter();
      Standard_Real ul = cur->LastParameter();
      if(uf <= W && W <= ul) return cur;
    }  
    return Handle(ChFiDS_HElSpine)();
  }
}

//=======================================================================
//function : ChangeElSpines
//purpose  : 
//=======================================================================

ChFiDS_ListOfHElSpine& ChFiDS_Spine::ChangeElSpines() 
{
  return elspines;
}

//=======================================================================
//function : SplitDone
//purpose  : 
//=======================================================================

void ChFiDS_Spine::SplitDone(const Standard_Boolean B)
{
  splitdone = B;
}

//=======================================================================
//function : SplitDone
//purpose  : 
//=======================================================================

Standard_Boolean ChFiDS_Spine::SplitDone() const 
{
  return splitdone;
}

//=======================================================================
//function : Reset
//purpose  : 
//=======================================================================

void ChFiDS_Spine::Reset(const Standard_Boolean AllData)
{
  splitdone = Standard_False;
  //if(AllData && !isconstant.IsNull()) isconstant->ChangeArray1().Init(0);
  elspines.Clear();
  if(AllData){
    firstparam = 0.;
    lastparam = abscissa->Value(abscissa->Upper());
    firstprolon = lastprolon = Standard_False;
  }
}

//=======================================================================
//function : FirstParameter
//purpose  : 
//=======================================================================

Standard_Real  ChFiDS_Spine::FirstParameter() const
{
  if(firstprolon) return firstparam;
  return 0.;
}


//=======================================================================
//function : LastParameter
//purpose  : 
//=======================================================================

Standard_Real  ChFiDS_Spine::LastParameter() const
{
  if(lastprolon) return lastparam;
  return abscissa->Value(abscissa->Upper());
}

//=======================================================================
//function : SetFirstParameter
//purpose  : 
//=======================================================================

void ChFiDS_Spine::SetFirstParameter(const Standard_Real Par) 
{
#ifdef OCCT_DEBUG
  if(Par >= Precision::Confusion()) 
    cout<<"Interior extension at the start of guideline"<<endl;
  if(IsPeriodic())
    cout<<"WARNING!!! Extension on periodic guideline."<<endl;
#endif
  firstprolon = Standard_True;
  firstparam = Par;
}


//=======================================================================
//function : SetLastParameter
//purpose  : 
//=======================================================================

void ChFiDS_Spine::SetLastParameter(const Standard_Real Par) 
{
#ifdef OCCT_DEBUG
  Standard_Real lll = abscissa->Value(abscissa->Upper());
  if((Par - lll) <= -Precision::Confusion()) 
    cout<<"Interior extension at the end of guideline"<<endl;
  if(IsPeriodic())
    cout<<"WARNING!!! Extension on periodic guideline."<<endl;
#endif
  lastprolon = Standard_True;
  lastparam = Par;
}

//=======================================================================
//function : FirstParameter
//purpose  : 
//=======================================================================

Standard_Real  ChFiDS_Spine::FirstParameter
(const Standard_Integer IndexSpine) const 
{
  if (IndexSpine==1) return 0.;
  return abscissa->Value(IndexSpine-1);
}

//=======================================================================
//function : LastParameter
//purpose  : 
//=======================================================================

Standard_Real  ChFiDS_Spine::LastParameter
(const Standard_Integer IndexSpine) const 
{
  return abscissa->Value(IndexSpine);
}

//=======================================================================
//function : Length
//purpose  : 
//=======================================================================

Standard_Real  ChFiDS_Spine::Length
(const Standard_Integer IndexSpine) const 
{
  if (IndexSpine==1) return abscissa->Value(IndexSpine);
  return abscissa->Value(IndexSpine) - abscissa->Value(IndexSpine-1);
}

//=======================================================================
//function : IsPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean ChFiDS_Spine::IsPeriodic() const
{
  return (firstState == ChFiDS_Closed);
}


//=======================================================================
//function : IsClosed
//purpose  : 
//=======================================================================

Standard_Boolean ChFiDS_Spine::IsClosed() const
{
  return (FirstVertex().IsSame(LastVertex()));
}


//=======================================================================
//function : FirstVertex
//purpose  : 
//=======================================================================

TopoDS_Vertex ChFiDS_Spine::FirstVertex() const
{
  TopoDS_Edge E = TopoDS::Edge(spine.First());
  if(E.Orientation() == TopAbs_FORWARD) return TopExp::FirstVertex(E); 
  return TopExp::LastVertex(E); 
}


//=======================================================================
//function : LastVertex
//purpose  : 
//=======================================================================

TopoDS_Vertex ChFiDS_Spine::LastVertex() const
{
  TopoDS_Edge E = TopoDS::Edge(spine.Last());
  if(E.Orientation() == TopAbs_FORWARD) return TopExp::LastVertex(E); 
  return TopExp::FirstVertex(E); 
}


//=======================================================================
//function : Absc
//purpose  : 
//=======================================================================

Standard_Real ChFiDS_Spine::Absc(const TopoDS_Vertex& V) const
{
  TopoDS_Vertex d,f;
  TopoDS_Edge E;
  for(Standard_Integer i = 1; i<=spine.Length(); i++){
    E = TopoDS::Edge(spine.Value(i));
    TopExp::Vertices(E,d,f);
    if(d.IsSame(V) && E.Orientation() == TopAbs_FORWARD){
      return FirstParameter(i);
    }
    if(d.IsSame(V) && E.Orientation() == TopAbs_REVERSED){
      return LastParameter(i);
    }
    if(f.IsSame(V) && E.Orientation() == TopAbs_FORWARD){
      return LastParameter(i);
    }
    if(f.IsSame(V) && E.Orientation() == TopAbs_REVERSED){
      return FirstParameter(i);
    }
  }
  return -1.;
}


//=======================================================================
//function : Period
//purpose  : 
//=======================================================================

Standard_Real ChFiDS_Spine::Period() const
{
  if(!IsPeriodic()) throw Standard_Failure("Non-periodic Spine");
  return abscissa->Value(abscissa->Upper());
}


//=======================================================================
//function : Resolution
//purpose  : 
//=======================================================================

Standard_Real ChFiDS_Spine::Resolution(const Standard_Real R3d) const
{
  return R3d;
}


//=======================================================================
//function : SetFirstTgt
//purpose  : 
//=======================================================================

void  ChFiDS_Spine::SetFirstTgt(const Standard_Real W)
{
  if(IsPeriodic()) throw Standard_Failure("No extension by tangent on periodic contours");
#ifdef OCCT_DEBUG
  if(W >= Precision::Confusion()) 
    cout<<"Interior extension at start of the guideline"<<endl;
#endif
  //The flag is suspended if is already positioned to avoid  
  //stopping d1
  hasfirsttgt = Standard_False;
  D1(W,firstori,firsttgt);
  //and it is reset.
  hasfirsttgt = Standard_True;
  firsttgtpar = W;
}


//=======================================================================
//function : SetLastTgt
//purpose  : 
//=======================================================================

void  ChFiDS_Spine::SetLastTgt(const Standard_Real W)
{
  if(IsPeriodic()) throw Standard_Failure("No extension by tangent periodic contours");

#ifdef OCCT_DEBUG
  Standard_Real L = W - abscissa->Value(abscissa->Upper());
  if(L <= -Precision::Confusion()) 
    cout<<"Interior extension at the end of guideline"<<endl;
#endif
  //The flag is suspended if is already positioned to avoid  
  //stopping d1 
  haslasttgt = Standard_False;
  D1(W,lastori,lasttgt);
  //and it is reset.
  haslasttgt = Standard_True;
  lasttgtpar = W;
}


//=======================================================================
//function : HasFirstTgt
//purpose  : 
//=======================================================================

Standard_Boolean  ChFiDS_Spine::HasFirstTgt()const
{
  return hasfirsttgt;
}

//=======================================================================
//function : HasLastTgt
//purpose  : 
//=======================================================================

Standard_Boolean  ChFiDS_Spine::HasLastTgt()const
{
  return haslasttgt;
}

//=======================================================================
//function : SetReference
//purpose  : 
//=======================================================================

void  ChFiDS_Spine::SetReference(const Standard_Real W)
{
  hasref = Standard_True;
  Standard_Real lll = abscissa->Value(abscissa->Upper());
  if(IsPeriodic()) valref = ElCLib::InPeriod(W,0.,lll);
  else valref = W;
}


//=======================================================================
//function : SetReference
//purpose  : 
//=======================================================================

void  ChFiDS_Spine::SetReference(const Standard_Integer I)
{
  hasref = Standard_True;
  if(I == 1) valref = abscissa->Value(1)*0.5;
  else valref = (abscissa->Value(I) + abscissa->Value(I-1))*0.5;
}


//=======================================================================
//function : Index
//purpose  : 
//=======================================================================

Standard_Integer ChFiDS_Spine::Index(const Standard_Real W,
				     const Standard_Boolean Forward) const
{
  Standard_Integer ind, len = abscissa->Length();
  Standard_Real par = W,last = abscissa->Value(abscissa->Upper());
  Standard_Real f = 0., l = 0., t = Max(tolesp,Precision::Confusion());

  if(IsPeriodic() && Abs(par) >= t && Abs(par-last) >= t) 
    par = ElCLib::InPeriod(par,0.,last);
  
  for (ind=1; ind <= len; ind++) {
    f = l;
    l = abscissa->Value(ind);
    if (par<l || ind==len) break;
  }
  if (Forward && ind<len && Abs(par-l) < t) ind++;
  else if (!Forward && ind > 1 && Abs(par-f) < t) ind--;
  else if (Forward && IsPeriodic() && ind == len && Abs(par-l) < t) ind = 1;
  else if (!Forward && IsPeriodic() && ind == 1 && Abs(par-f) < t) ind = len;
  return ind;
}

//=======================================================================
//function : Index
//purpose  : 
//=======================================================================

Standard_Integer ChFiDS_Spine::Index (const TopoDS_Edge& E) const
{
  for(Standard_Integer IE = 1; IE <= spine.Length(); IE++){
    if(E.IsSame(spine.Value(IE))) return IE;
  }
  return 0;
}

//=======================================================================
//function : UnsetReference
//purpose  : 
//=======================================================================

void  ChFiDS_Spine::UnsetReference()
{
  hasref = Standard_False;
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void  ChFiDS_Spine::Load()
{
  if(!abscissa.IsNull()){
#ifdef OCCT_DEBUG
    cout<<"new load of CE"<<endl;
#endif
  }
  Standard_Integer len = spine.Length();
  abscissa = new TColStd_HArray1OfReal(1,len);
  Standard_Real a1 = 0.;
  for (Standard_Integer i = 1; i <= len; i++){
    myCurve.Initialize(TopoDS::Edge(spine.Value(i)));
    a1 += GCPnts_AbscissaPoint::Length(myCurve);
    abscissa->SetValue(i,a1);
  }
  indexofcurve =1;
  myCurve.Initialize(TopoDS::Edge(spine.Value(1)));
}


//=======================================================================
//function : Absc
//purpose  : 
//=======================================================================

Standard_Real ChFiDS_Spine::Absc(const Standard_Real U) 
{
  return Absc(U,indexofcurve);
}

//=======================================================================
//function : Absc
//purpose  : 
//=======================================================================

Standard_Real ChFiDS_Spine::Absc(const Standard_Real U,
				 const Standard_Integer I) 
{

  
  if(indexofcurve != I){
    void* p = (void*)this;
    ((ChFiDS_Spine*)p)->indexofcurve = I;
    ((ChFiDS_Spine*)p)->myCurve.Initialize(TopoDS::Edge(spine.Value(I)));
  }
  Standard_Real L = FirstParameter(I);
  if (spine.Value(I).Orientation() == TopAbs_REVERSED) {
    L += GCPnts_AbscissaPoint::Length(myCurve,U,myCurve.LastParameter());
  }
  else{
    L += GCPnts_AbscissaPoint::Length(myCurve,myCurve.FirstParameter(),U);
  }
  return L;
}

//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

void ChFiDS_Spine::Parameter(const Standard_Real AbsC,
			     Standard_Real& U,
			     const Standard_Boolean Oriented) 
{
  Standard_Integer Index;
  for (Index=1;Index<abscissa->Length();Index++) {
    if (AbsC<abscissa->Value(Index)) break;
  }
  Parameter(Index,AbsC,U,Oriented);
}


//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

void ChFiDS_Spine::Parameter(const Standard_Integer Index,
			     const Standard_Real AbsC,
			     Standard_Real& U,
			     const Standard_Boolean Oriented) 
{

  if (Index != indexofcurve) {
    void* p = (void*)this;
    ((ChFiDS_Spine*)p)->indexofcurve = Index;
    ((ChFiDS_Spine*)p)->myCurve.Initialize(TopoDS::Edge(spine.Value(Index)));
  }
  Standard_Real L;
  TopAbs_Orientation Or = spine.Value(Index).Orientation();
  if (Or == TopAbs_REVERSED) {
    L = abscissa->Value(indexofcurve)-AbsC; 
  }
  else if (indexofcurve==1) {
    L = AbsC;
  }  
  else {
    L = AbsC - abscissa->Value(indexofcurve-1); 
  }
  Standard_Real t = L/Length(Index);
  Standard_Real uapp = (1. - t) * myCurve.FirstParameter() + t * myCurve.LastParameter();
//  GCPnts_AbscissaPoint GCP;
//  GCP.Perform(myCurve,L,myCurve.FirstParameter(),uapp,BRep_Tool::Tolerance(myCurve.Edge()));
  GCPnts_AbscissaPoint GCP(myCurve,L,myCurve.FirstParameter(),uapp);
  U = GCP.Parameter();
  if (Or == TopAbs_REVERSED && Oriented) {
    U = (myCurve.LastParameter()+myCurve.FirstParameter()) - U;
  }
}


//=======================================================================
//function : Prepare
//purpose  : 
//=======================================================================

void  ChFiDS_Spine::Prepare(Standard_Real& L, 
			    Standard_Integer& Ind) const
{
  Standard_Real tol = Max(tolesp,Precision::Confusion());
  Standard_Real last = abscissa->Value(abscissa->Upper());
  Standard_Integer len = abscissa->Length();
  if(IsPeriodic() && Abs(L) >= tol && Abs(L-last) >= tol) 
    L = ElCLib::InPeriod(L,0.,last);

  if(hasfirsttgt && (L <= firsttgtpar)){
    if(hasref && valref >= L && Abs(L-firsttgtpar) <= tol){
      Ind = Index(L);
    }
    else{Ind = -1; L -= firsttgtpar;} 
  }
  else if(L <= 0.){Ind = 1;}
  else if(haslasttgt && (L >= lasttgtpar)){
    if(hasref && valref <= L && Abs(L-lasttgtpar) <= tol){
      Ind = Index(L); 
    }
    else{Ind = len + 1; L -= lasttgtpar;} 
  }
  else if(L >= last){Ind = len;}
  else{
    for (Ind=1;Ind < len;Ind++) {
      if (L<abscissa->Value(Ind)) break;
    }
    if(hasref){
      if (L >= valref && Ind != 1){
	if(Abs(L-abscissa->Value(Ind-1)) <= Precision::Confusion()) Ind--;
      }
      else if (L <= valref && Ind != len){
	if(Abs(L-abscissa->Value(Ind)) <= Precision::Confusion()) Ind++;
      }
    }
  }
  if(Ind >= 1 && Ind <= len){ 
    if (spine.Value(Ind).Orientation() == TopAbs_REVERSED){
      L = abscissa->Value(Ind) - L; 
    }
    else if (Ind!=1){
      L -= abscissa->Value(Ind - 1); 
    }
  }
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

gp_Pnt  ChFiDS_Spine::Value(const Standard_Real AbsC) 
{

  Standard_Integer Index;
  Standard_Real L = AbsC;

  Prepare(L,Index);

  if (Index == -1) {
    gp_Pnt Pp = firstori;
    gp_Vec Vp = firsttgt;
    Vp.Multiply(L);
    Pp.Translate(Vp);
    return Pp;
  }
  else if (Index == (abscissa->Length() + 1)) {
    gp_Pnt Pp = lastori;
    gp_Vec Vp = lasttgt;
    Vp.Multiply(L);
    Pp.Translate(Vp);
    return Pp;
  }
  if (Index != indexofcurve) {
    void* p = (void*)this;
    ((ChFiDS_Spine*)p)->indexofcurve = Index;
    ((ChFiDS_Spine*)p)->myCurve.Initialize(TopoDS::Edge(spine.Value(Index)));
  }
  Standard_Real t = L/Length(Index);
  Standard_Real uapp = (1. - t) * myCurve.FirstParameter() + t * myCurve.LastParameter();
//  GCPnts_AbscissaPoint GCP;
//  GCP.Perform(myCurve,L,myCurve.FirstParameter(),uapp,BRep_Tool::Tolerance(myCurve.Edge()));
  GCPnts_AbscissaPoint GCP(myCurve,L,myCurve.FirstParameter(),uapp);
  return myCurve.Value(GCP.Parameter());
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void  ChFiDS_Spine::D0(const Standard_Real AbsC, gp_Pnt& P) 
{
  P = Value(AbsC);
}

//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void  ChFiDS_Spine::D1(const Standard_Real AbsC, 
				   gp_Pnt& P, 
				   gp_Vec& V1) 
{
  Standard_Integer Index;
  Standard_Real L = AbsC;

  Prepare(L,Index);

  if (Index == -1) {
    P = firstori;
    V1 = firsttgt;
    gp_Vec Vp = firsttgt;
    Vp.Multiply(L);
    P.Translate(Vp);
  }
  else if (Index == (abscissa->Length() + 1)) {
    P = lastori;
    V1 = lasttgt;
    gp_Vec Vp = lasttgt;
    Vp.Multiply(L);
    P.Translate(Vp);
  }
  else {
    if (Index != indexofcurve) {
      void* p = (void*)this;
      ((ChFiDS_Spine*)p)->indexofcurve = Index;
      ((ChFiDS_Spine*)p)->myCurve.Initialize(TopoDS::Edge(spine.Value(Index)));
    }
    Standard_Real t = L/Length(Index);
    Standard_Real uapp = (1. - t) * myCurve.FirstParameter() + t * myCurve.LastParameter();
//    GCPnts_AbscissaPoint GCP;
//    GCP.Perform(myCurve,L,myCurve.FirstParameter(),uapp,BRep_Tool::Tolerance(myCurve.Edge()));
    GCPnts_AbscissaPoint GCP(myCurve,L,myCurve.FirstParameter(),uapp);
    myCurve.D1(GCP.Parameter(),P,V1);
    Standard_Real D1 = 1./V1.Magnitude();
    if (spine.Value(Index).Orientation() == TopAbs_REVERSED) D1 = -D1;
    V1.Multiply(D1);
  }
}


//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void  ChFiDS_Spine::D2(const Standard_Real AbsC, 
				   gp_Pnt& P, 
				   gp_Vec& V1, 
				   gp_Vec& V2) 
{

  Standard_Integer Index;
  Standard_Real L = AbsC;

  Prepare(L,Index);

  if (Index == -1) {
    P = firstori;
    V1 = firsttgt;
    V2.SetCoord(0.,0.,0.);
    gp_Vec Vp = firsttgt;
    Vp.Multiply(L);
    P.Translate(Vp);
  }
  else if (Index == (abscissa->Length() + 1)) {
    P = lastori;
    V1 = lasttgt;
    V2.SetCoord(0.,0.,0.);
    gp_Vec Vp = lasttgt;
    Vp.Multiply(L);
    P.Translate(Vp);
  }
  else {
    if (Index != indexofcurve) {
      void* p = (void*)this;
      ((ChFiDS_Spine*)p)->indexofcurve = Index;
      ((ChFiDS_Spine*)p)->myCurve.Initialize(TopoDS::Edge(spine.Value(Index)));
    }
    Standard_Real t = L/Length(Index);
    Standard_Real uapp = (1. - t) * myCurve.FirstParameter() + t * myCurve.LastParameter();
//    GCPnts_AbscissaPoint GCP;
//    GCP.Perform(myCurve,L,myCurve.FirstParameter(),uapp,BRep_Tool::Tolerance(myCurve.Edge()));
    GCPnts_AbscissaPoint GCP(myCurve,L,myCurve.FirstParameter(),uapp);
    myCurve.D2(GCP.Parameter(),P,V1,V2);
    Standard_Real N1 = V1.SquareMagnitude();
    Standard_Real D2 = -(V1.Dot(V2))*(1./N1)*(1./N1);
    V2.Multiply(1./N1);
    N1 = Sqrt(N1);
    gp_Vec Va = V1.Multiplied(D2);
    V2.Add(Va);
    Standard_Real D1 = 1./N1;
    if (spine.Value(Index).Orientation() == TopAbs_REVERSED) D1 = -D1;
    V1.Multiply(D1);
  }
}

//=======================================================================
//function : SetCurrent
//purpose  : 
//=======================================================================

void ChFiDS_Spine::SetCurrent(const Standard_Integer Index)
{  
  if (Index != indexofcurve)  {
    indexofcurve = Index;
    myCurve.Initialize(TopoDS::Edge(spine.Value(indexofcurve)));
  }
} 

//=======================================================================
//function : CurrentElementarySpine
//purpose  : 
//=======================================================================

const BRepAdaptor_Curve&  ChFiDS_Spine::CurrentElementarySpine
(const Standard_Integer Index) 
{
  if (Index != indexofcurve)  {
    indexofcurve = Index;
    myCurve.Initialize(TopoDS::Edge(spine.Value(indexofcurve)));
  }
  return myCurve;
}

//=======================================================================
//function : GetType
//purpose  : 
//=======================================================================

GeomAbs_CurveType ChFiDS_Spine::GetType() const
{
  return myCurve.GetType();
}

//=======================================================================
//function : Line
//purpose  : 
//=======================================================================

gp_Lin ChFiDS_Spine::Line() const
{
  gp_Lin LL(myCurve.Line());
  if (spine.Value(indexofcurve).Orientation() == TopAbs_REVERSED) {
    LL.Reverse();
    LL.SetLocation(myCurve.Value(myCurve.LastParameter()));
  }
  else {
    LL.SetLocation(myCurve.Value(myCurve.FirstParameter()));
  }
  return LL;
}


//=======================================================================
//function : Circle
//purpose  : 
//=======================================================================

gp_Circ ChFiDS_Spine::Circle() const
{
  gp_Ax2 Ac = myCurve.Circle().Position();
  gp_Dir Dc(gp_Vec(Ac.Location(),myCurve.Value(myCurve.FirstParameter())));
  gp_Dir ZZ(Ac.Direction());
  
  if (spine.Value(indexofcurve).Orientation() == TopAbs_REVERSED) {
    Dc = gp_Dir(gp_Vec(Ac.Location(),myCurve.Value(myCurve.LastParameter())));
    ZZ.Reverse();
  }
  gp_Ax2 A(Ac.Location(),ZZ,Dc);
  return gp_Circ(A,myCurve.Circle().Radius());
}
//=======================================================================
//function : SetErrorStatus
//purpose  : met a jour le statut d'erreur 
//=======================================================================
void  ChFiDS_Spine::SetErrorStatus(const ChFiDS_ErrorStatus state)
{
  errorstate=state;
}
//=======================================================================
//function : ErrorStatus
//purpose  : renvoie le statut d'erreur concernant la spine 
//=======================================================================

ChFiDS_ErrorStatus  ChFiDS_Spine::ErrorStatus()const 
{
  return errorstate;
}
