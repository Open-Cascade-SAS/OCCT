// Created on: 1991-06-24
// Created by: Didier PIFFAULT
// Copyright (c) -1999 Matra Datavision
// Copyright (c) 1991-1999 Matra Datavision
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


#include <Bnd_Box2d.hxx>
#include <gp_Pnt2d.hxx>
#include <Intf_InterferencePolygon2d.hxx>
#include <Intf_Polygon2d.hxx>
#include <Intf_SectionPoint.hxx>
#include <Intf_SeqOfSectionPoint.hxx>
#include <Intf_SeqOfTangentZone.hxx>
#include <Intf_TangentZone.hxx>
#include <Precision.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColStd_ListOfInteger.hxx>

// Angular precision (sinus) below that value two right segments
// are considered as having a potential zone of tangency.
namespace
{
  static const Standard_Real PRCANG = Precision::Angular();
}

//=======================================================================
//function : Intf_InterferencePolygon2d
//purpose  : constructor empty
//=======================================================================

Intf_InterferencePolygon2d::Intf_InterferencePolygon2d()
: Intf_Interference (Standard_False),
  oClos (Standard_False),
  tClos (Standard_False),
  nbso (0)
{}

//=======================================================================
//function : Intf_InterferencePolygon2d
//purpose  : Constructor of the interference beetween two Polygon.
//=======================================================================

Intf_InterferencePolygon2d::Intf_InterferencePolygon2d
  (const Intf_Polygon2d& Obje1, const Intf_Polygon2d& Obje2)
: Intf_Interference (Standard_False),
  oClos (Standard_False),
  tClos (Standard_False),
  nbso (0)
{
  if (!Obje1.Bounding().IsOut(Obje2.Bounding())) {
    Tolerance=Obje1.DeflectionOverEstimation()+
              Obje2.DeflectionOverEstimation();
    if (Tolerance==0.)
      Tolerance=Epsilon(1000.);
    nbso=Obje1.NbSegments();
    oClos=Obje1.Closed();
    tClos=Obje2.Closed();
    Interference(Obje1, Obje2);
    Clean();
  }
}


//=======================================================================
//function : Intf_InterferencePolygon2d
//purpose  : Constructor of the auto interference of a Polygon.
//=======================================================================

Intf_InterferencePolygon2d::Intf_InterferencePolygon2d
  (const Intf_Polygon2d& Obje)
: Intf_Interference (Standard_True),
  oClos (Standard_False),
  tClos (Standard_False),
  nbso (0)
{
  Tolerance=Obje.DeflectionOverEstimation()*2;
  if (Tolerance==0.)
    Tolerance=Epsilon(1000.);
  oClos=Obje.Closed();
  tClos=oClos;
  Interference(Obje);
  Clean();
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void Intf_InterferencePolygon2d::Perform
  (const Intf_Polygon2d& Obje1, const Intf_Polygon2d& Obje2)
{
  SelfInterference(Standard_False);
  if (!Obje1.Bounding().IsOut(Obje2.Bounding())) {
    Tolerance=Obje1.DeflectionOverEstimation()+
              Obje2.DeflectionOverEstimation();
    if (Tolerance==0.)
      Tolerance=Epsilon(1000.);
    nbso=Obje1.NbSegments();
    oClos=Obje1.Closed();
    tClos=Obje2.Closed();
    Interference(Obje1, Obje2);
    Clean();
  }
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void Intf_InterferencePolygon2d::Perform 
  (const Intf_Polygon2d& Obje)
{
  SelfInterference(Standard_True);
  Tolerance=Obje.DeflectionOverEstimation()*2;
  if (Tolerance==0.)
    Tolerance=Epsilon(1000.);
  oClos=Obje.Closed();
  tClos=oClos;
  Interference(Obje);
  Clean();
}

//=======================================================================
//function : Pnt2dValue
//purpose  : Give the section point of range Index in the interference.
//=======================================================================

gp_Pnt2d Intf_InterferencePolygon2d::Pnt2dValue
  (const Standard_Integer Index) const
{
  return gp_Pnt2d((mySPoins(Index)).Pnt().X(),
		  (mySPoins(Index)).Pnt().Y());
}


//=======================================================================
//function : Interference
//purpose  : 
//=======================================================================

void Intf_InterferencePolygon2d::Interference
  (const Intf_Polygon2d& Obje1,
   const Intf_Polygon2d& Obje2)
{
  Bnd_Box2d bSO;
  Bnd_Box2d bST;

  Standard_Integer iObje1, iObje2, n1 = nbso, n2 = Obje2.NbSegments();
  Standard_Real d1 = Obje1.DeflectionOverEstimation(),
    d2 = Obje2.DeflectionOverEstimation();

  gp_Pnt2d p1b, p1e, p2b, p2e;
  for (iObje1=1; iObje1<=n1; iObje1++)
  {
    bSO.SetVoid();
    Obje1.Segment(iObje1,p1b,p1e);
    bSO.Add(p1b);
    bSO.Add(p1e);
    bSO.Enlarge(d1);
    if (!Obje2.Bounding().IsOut(bSO)) {
      for (iObje2=1; iObje2<=n2; iObje2++) {
        bST.SetVoid();
        Obje2.Segment(iObje2,p2b,p2e);
        bST.Add(p2b);
        bST.Add(p2e);
        bST.Enlarge(d2);
        if (!bSO.IsOut(bST))
          Intersect(iObje1, iObje2, p1b, p1e, p2b, p2e);
      }
    }
  }
}

//=======================================================================
//function : Interference
//purpose  : 
//=======================================================================

void Intf_InterferencePolygon2d::Interference
  (const Intf_Polygon2d& Obje)
{
  Bnd_Box2d bSO;
  Bnd_Box2d bST;

  Standard_Integer iObje1, iObje2, n = Obje.NbSegments();
  Standard_Real d = Obje.DeflectionOverEstimation();

  gp_Pnt2d p1b, p1e, p2b, p2e;
  for (iObje1=1; iObje1<=n; iObje1++) {
    bSO.SetVoid();
    Obje.Segment(iObje1,p1b,p1e);
    bSO.Add(p1b);
    bSO.Add(p1e);
    bSO.Enlarge(d);
    if (!Obje.Bounding().IsOut(bSO)) {
      for (iObje2=iObje1+1;iObje2<=n;iObje2++){
        bST.SetVoid();
        Obje.Segment(iObje2,p2b,p2e);
        bST.Add(p2b);
        bST.Add(p2e);
        bST.Enlarge(d);
        if (!bSO.IsOut(bST))
          Intersect(iObje1, iObje2, p1b, p1e, p2b, p2e);
      }
    }
  }
}


//=======================================================================
//function : Clean
//purpose  : 
//=======================================================================

void Intf_InterferencePolygon2d::Clean()
{

// The zones of tangency that concerns only one couple of segments are
// conserved if the angle between the segments is less than <PRCANG> and
// if there is no real point of intersection EDGE/EDGE:
  Standard_Integer nbIt=myTZones.Length();
  Standard_Integer decal=0;
  Standard_Integer addr1, addr2;
  Intf_PIType      dim1, dim2;
  Standard_Real    par;
  Standard_Integer tsp, tsps;
  Standard_Integer lpi, ltz;
  Standard_Boolean Only1Seg=Standard_False;

#define PI1 (myTZones(ltz-decal).GetPoint(lpi))
#define PI2 (myTZones(ltz-decal).GetPoint(tsp))

  for (ltz=1; ltz<=nbIt; ltz++) {
    tsp=tsps=0;
    Standard_Real pr1mi,pr1ma,pr2mi,pr2ma,delta1,delta2;
    myTZones(ltz-decal).ParamOnFirst(pr1mi,pr1ma);
    delta1=pr1ma-pr1mi;
    myTZones(ltz-decal).ParamOnSecond(pr2mi,pr2ma);
    delta2=pr2ma-pr2mi;
    if (delta1<1. && delta2<1.) Only1Seg=Standard_True;
    if (delta1==0. || delta2==0.) Only1Seg=Standard_True;

    for (lpi=1; lpi<=myTZones(ltz-decal).NumberOfPoints(); lpi++) {
      if (PI1.Incidence()<=PRCANG) {tsp=tsps=0;break;}
      PI1.InfoFirst(dim1,addr1,par);
      PI1.InfoSecond(dim2,addr2,par);
      if (dim1==Intf_EDGE && dim2==Intf_EDGE) {
	tsps=0;
	if (tsp>0) {
	  tsp=0;
	  Only1Seg=Standard_False;
	  break;
	}
	tsp=lpi;
      }
      else if (dim1!=Intf_EXTERNAL && dim2!=Intf_EXTERNAL) {
	tsps=lpi;
      }
    }
    if (tsp>0) {
      mySPoins.Append(myTZones(ltz-decal).GetPoint(tsp));
      myTZones.Remove(ltz-decal);
      decal++;
    }
    else if (Only1Seg && tsps!=0) {
      mySPoins.Append(myTZones(ltz-decal).GetPoint(tsps));
      myTZones.Remove(ltz-decal);
      decal++;
    }
  }


// The points of intersection located in the tangency zone are
// removed from the list :
  nbIt=mySPoins.Length();
  decal=0;

  for (lpi=1; lpi<=nbIt; lpi++) {
    for (ltz=1; ltz<=myTZones.Length(); ltz++) {
      if (myTZones(ltz).RangeContains(mySPoins(lpi-decal))) {
	mySPoins.Remove(lpi-decal);
	decal++;
	break;
      }
    }
  }
}


//=======================================================================
//function : Intersect
//purpose  : 
//=======================================================================

void Intf_InterferencePolygon2d::Intersect
  (const Standard_Integer iObje1, const Standard_Integer iObje2,
   const gp_Pnt2d& BegO, const gp_Pnt2d& EndO,
   const gp_Pnt2d& BegT, const gp_Pnt2d& EndT)
{
  if(SelfIntf) { 
    if(Abs(iObje1-iObje2)<=1) return;  //-- Ajout du 15 jan 98 
  }

  Standard_Integer nbpi=0;
  Standard_Real parO[8];
  Standard_Real parT[8];
  Intf_SeqOfSectionPoint thePi;
  gp_XY segT =EndT.XY()-BegT.XY();
  gp_XY segO =EndO.XY()-BegO.XY();

// If the length of segment is zero, nothing is done
  Standard_Real lgT =Sqrt(segT*segT);
  if (lgT<=0.) return;
  Standard_Real lgO =Sqrt(segO*segO);
  if (lgO<=0.) return;

// Direction of parsing of segments
  Standard_Real sigPS=(segO*segT)>0.0 ? 1.0 : -1.0;

// Precision of calculation
  Standard_Real floatgap=Epsilon(lgO+lgT);

// Angle between two straight lines and radius of interference
  Standard_Real sinTeta=(segO.CrossMagnitude(segT)/lgO)/lgT;
  Standard_Real rayIntf=0.;
  if (sinTeta>0.) rayIntf=Tolerance/sinTeta;
	  
// Interference <begO> <segT>
  Standard_Real dbOT=((BegO.XY()-BegT.XY())^segT)/lgT;
  Standard_Real dbObT=BegO.Distance(BegT);
  Standard_Real dbOeT=BegO.Distance(EndT);
  if (Abs(dbOT)<=Tolerance) {
    if (dbObT<=Tolerance) {
      nbpi++;
      parO[nbpi]=0.;parT[nbpi]=0.;
      thePi.Append(Intf_SectionPoint(BegO,Intf_VERTEX,iObje1,0.,
				     Intf_VERTEX,iObje2,0.,sinTeta));
    }
    if (dbOeT<=Tolerance) {
      nbpi++;
      parO[nbpi]=0.;parT[nbpi]=1.;
      thePi.Append(Intf_SectionPoint(BegO,Intf_VERTEX,iObje1,0.,
				     Intf_VERTEX,iObje2+1,0.,sinTeta));
    }
    if (dbObT>Tolerance && dbOeT>Tolerance &&
	dbObT+dbOeT<=(lgT+Tolerance)) {
      nbpi++;
      parO[nbpi]=0.;parT[nbpi]=dbObT/lgT;
      thePi.Append(Intf_SectionPoint(BegO,Intf_VERTEX,iObje1,0.,
				     Intf_EDGE,iObje2,parT[nbpi],sinTeta));
    }
  }
  
// Interference <endO> <segT>
  Standard_Real deOT=((EndO.XY()-BegT.XY())^segT)/lgT;
  Standard_Real deObT=EndO.Distance(BegT);
  Standard_Real deOeT=EndO.Distance(EndT);
  if (Abs(deOT)<=Tolerance) {
    if (deObT<=Tolerance) {
      nbpi++;
      parO[nbpi]=1.;parT[nbpi]=0.;
      thePi.Append(Intf_SectionPoint(EndO,Intf_VERTEX,iObje1+1,0.,
				     Intf_VERTEX,iObje2,0.,sinTeta));
    }
    if (deOeT<=Tolerance) {
      nbpi++;
      parO[nbpi]=1.;parT[nbpi]=1.;
      thePi.Append(Intf_SectionPoint(EndO,Intf_VERTEX,iObje1+1,0.,
				     Intf_VERTEX,iObje2+1,0.,sinTeta));
    }
    if (deObT>Tolerance && deOeT>Tolerance &&
	deObT+deOeT<=(lgT+Tolerance)) {
      nbpi++;
      parO[nbpi]=1.;parT[nbpi]=deObT/lgT;
      thePi.Append(Intf_SectionPoint(EndO,Intf_VERTEX,iObje1+1,0.,
				     Intf_EDGE,iObje2,parT[nbpi],sinTeta));
    }
  }
  
// Interference <begT> <segO>
  Standard_Real dbTO=((BegT.XY()-BegO.XY())^segO)/lgO;
  if (Abs(dbTO)<=Tolerance) {
    if (dbObT>Tolerance && deObT>Tolerance &&
	dbObT+deObT<=(lgO+Tolerance)) {
      nbpi++;
      parO[nbpi]=dbObT/lgO;parT[nbpi]=0.;
      thePi.Append(Intf_SectionPoint(BegT,Intf_EDGE,iObje1,parO[nbpi],
				     Intf_VERTEX,iObje2,0.,sinTeta));
    }
  }

// Interference <endT> <segO>
  Standard_Real deTO=((EndT.XY()-BegO.XY())^segO)/lgO;
  if (Abs(deTO)<=Tolerance) {
    if (dbOeT>Tolerance && deOeT>Tolerance &&
	dbOeT+deOeT<=(lgO+Tolerance)) {
      nbpi++;
      parO[nbpi]=dbOeT/lgO;parT[nbpi]=1.;
      thePi.Append(Intf_SectionPoint(EndT,Intf_EDGE,iObje1,parO[nbpi],
				     Intf_VERTEX,iObje2+1,0.,sinTeta));
    }
  }

  Standard_Boolean edgeSP=Standard_False;
  Standard_Real parOSP=0, parTSP=0;

  if (Abs(dbOT-deOT)>floatgap && Abs(dbTO-deTO)>floatgap) {
    parOSP=dbOT/(dbOT-deOT);
    parTSP=dbTO/(dbTO-deTO);
    if (dbOT*deOT<=0. && dbTO*deTO<=0.) {
      edgeSP=Standard_True;
    }
    else if (nbpi==0) return;

// If there is no interference it is necessary to take the points segment by segment
    if (nbpi==0 && sinTeta>PRCANG) {
      nbpi++;
      parO[nbpi]=parOSP;
      parT[nbpi]=parTSP;
      thePi.Append(Intf_SectionPoint(gp_Pnt2d (BegO.X()+ (segO.X()*parOSP),
					       BegO.Y()+ (segO.Y()*parOSP)),
				     Intf_EDGE,iObje1,parOSP,
				     Intf_EDGE,iObje2,parTSP,sinTeta));
    }

// Otherwise it is required to check if there is no other
    else if (rayIntf>=Tolerance) {
      Standard_Real deltaO=rayIntf/lgO;
      Standard_Real deltaT=rayIntf/lgT;
      Standard_Real x, y;
      Standard_Real parOdeb=parOSP-deltaO;
      Standard_Real parOfin=parOSP+deltaO;
      Standard_Real parTdeb=parTSP-sigPS*deltaT;
      Standard_Real parTfin=parTSP+sigPS*deltaT;
      if (nbpi==0) {
	parO[1]=parOdeb;
	parO[2]=parOfin;
	parT[1]=parTdeb;
	parT[2]=parTfin;
	while (nbpi<2) {
	  nbpi++;
	  x=BegO.X()+ (segO.X()*parO[nbpi]);
	  y=BegO.Y()+ (segO.Y()*parO[nbpi]);
	  thePi.Append(Intf_SectionPoint(gp_Pnt2d(x, y),
					 Intf_EXTERNAL, iObje1, parO[nbpi],
					 Intf_EXTERNAL, iObje2, parT[nbpi],
					 sinTeta));
	}
      }
      else {  //nbpi>0
	if (nbpi==1) {
	  Standard_Boolean ok=Standard_True;
	  if (0.<parOdeb && parOdeb<1. && 0.<parTdeb && parTdeb<1. ) {
	    parO[nbpi+1]=parOdeb;
	    parT[nbpi+1]=parTdeb;
	  }
	  else if (0.<parOfin && parOfin<1. && 0.<parTfin && parTfin<1. ) {
	    parO[nbpi+1]= parOfin;
	    parT[nbpi+1]= parTfin;
	  }
	  else {
	    ok=Standard_False;
	  }

	  if (ok) {
	    x=BegO.X()+ (segO.X()*parO[nbpi+1]);
	    y=BegO.Y()+ (segO.Y()*parO[nbpi+1]);
	    if (thePi(1).Pnt().Distance(gp_Pnt(x, y, 0)) >= (Tolerance/4.)) {
	      nbpi++;
	      thePi.Append(Intf_SectionPoint(gp_Pnt2d(x, y),
					     Intf_EXTERNAL, iObje1, parO[nbpi],
					     Intf_EXTERNAL, iObje2, parT[nbpi],
					     sinTeta));
	    }
	  }
	}
	else { // plus d une singularite
	  Standard_Real parOmin=parO[1];
	  Standard_Real parOmax=parO[1];
	  Standard_Real parTmin=parT[1];
	  Standard_Real parTmax=parT[1];
	  for (Standard_Integer i=2; i<=nbpi; i++) {
	    parOmin=Min(parOmin, parO[i]);
	    parOmax=Max(parOmax, parO[i]);
	    parTmin=Min(parTmin, parT[i]);
	    parTmax=Max(parTmax, parT[i]);
	  }

	  Standard_Real    delta;
	  if (parOdeb<0.) {
	    delta=-parOdeb;
	    parOdeb=0.;
	    parTdeb=parTdeb+sigPS*(delta*(deltaT/deltaO));
	  }
	  if (parOfin>1.) {
	    delta=parOfin-1.;
	    parOfin=1.;
	    parTfin=parTfin-sigPS*(delta*(deltaT/deltaO));
	  }
	  if (sigPS>0.) {
	    if (parTdeb<0.) {
	      delta=-parTdeb;
	      parTdeb=0.;
	      parOdeb=parOdeb+delta*(deltaO/deltaT);
	    }
	    if (parTfin>1.) {
	      delta=parTfin-1.;
	      parTfin=1.;
	      parOfin=parOfin-delta*(deltaO/deltaT);
	    }
	  }
	  else {
	    if (parTdeb>1.) {
	      delta=parTdeb-1.;
	      parTdeb=1.;
	      parOdeb=parOdeb+delta*(deltaO/deltaT);
	    }
	    if (parTfin<0.) {
	      delta=-parTfin;
	      parTfin=0.;
	      parOfin=parOfin-delta*(deltaO/deltaT);
	    }
	  }

	  if ((parOdeb<parOmin && parOmin>0.) || 
	      (sigPS>0. && parTdeb<parTmin && parTmin>0.) || 
	      (sigPS<0. && parTdeb>parTmax && parTmax<1.)) {
	    nbpi++;
	    parO[nbpi]=Max(0., Min(1., parOdeb));
	    parT[nbpi]=Max(0., Min(1., parTdeb));
	    x=BegO.X()+ (segO.X()*parO[nbpi]);
	    y=BegO.Y()+ (segO.Y()*parO[nbpi]);
	    thePi.Append(Intf_SectionPoint(gp_Pnt2d(x, y),
					   Intf_EXTERNAL, iObje1, parO[nbpi],
					   Intf_EXTERNAL, iObje2, parT[nbpi],
					   sinTeta));
	  }

	  if ((parOfin>parOmax && parOmax<1.) || 
	      (sigPS<0. && parTfin<parTmin && parTmin>0.) || 
	      (sigPS>0. && parTfin>parTmax && parTmax<1.)) {
	    nbpi++;
	    parO[nbpi]=Min(1., Max(0., parOfin));
	    parT[nbpi]=Min(1., Max(0., parTfin));
	    x=BegO.X()+ (segO.X()*parO[nbpi]);
	    y=BegO.Y()+ (segO.Y()*parO[nbpi]);
	    thePi.Append(Intf_SectionPoint(gp_Pnt2d(x, y),
					   Intf_EXTERNAL, iObje1, parO[nbpi],
					   Intf_EXTERNAL, iObje2, parT[nbpi],
					   sinTeta));
	  }
	}
      }
    }
  }
  
  //-- lbr : The points too close to each other are suspended
  Standard_Boolean suppr;
  do { 
    suppr=Standard_False;
    for(Standard_Integer i=2; suppr==Standard_False && i<=nbpi; i++) { 
      const gp_Pnt& Pim1 = thePi(i-1).Pnt();
      const gp_Pnt& Pi   = thePi(i).Pnt();
      Standard_Real d=Pi.Distance(Pim1);
      d*=50.0;
      if(d<lgT && d<lgO) { 
	for(Standard_Integer j=i; j<nbpi; j++) { 
	  thePi(j)=thePi(j+1);
	}
	nbpi--;
	suppr=Standard_True;
      }
    }
  }
  while(suppr==Standard_True);
  
  



  
  if (nbpi==1) {
    if (edgeSP) {
      thePi(1)=Intf_SectionPoint(gp_Pnt2d (BegO.X()+ (segO.X()*parOSP),
					   BegO.Y()+ (segO.Y()*parOSP)),
				 Intf_EDGE,iObje1,parOSP,
				 Intf_EDGE,iObje2,parTSP,sinTeta);
      parO[1]=parOSP;
      parT[1]=parTSP;
    }
    if (!SelfIntf) {
      Standard_Boolean contains = Standard_False;
      for (Standard_Integer i = 1; i <= mySPoins.Length(); i++)
        if (thePi(1).IsEqual(mySPoins(i))) {
          contains = Standard_True;
          break;
        }
      if (!contains)
        mySPoins.Append(thePi(1));
    }
    else if (iObje2-iObje1!=1 && 
	     (!oClos || (iObje1!=1 && iObje2!=nbso))) {
      mySPoins.Append(thePi(1));
    }
  }

  else if (nbpi>=2) {
    Intf_TangentZone TheTZ;
    if (nbpi==2) {
      TheTZ.PolygonInsert(thePi(1));
      TheTZ.PolygonInsert(thePi(2));
    }
    else {
      Standard_Integer lpj;
      Standard_Integer lmin=1;
      Standard_Integer lmax=1;
      for (lpj=2; lpj<=nbpi; lpj++) {
	if      (parO[lpj]<parO[lmin]) lmin=lpj;
	else if (parO[lpj]>parO[lmax]) lmax=lpj;
      }
      TheTZ.PolygonInsert(thePi(lmin));
      TheTZ.PolygonInsert(thePi(lmax));

      Standard_Integer ltmin=1;
      Standard_Integer ltmax=1;
      for (lpj=2; lpj<=nbpi; lpj++) {
	if      (parT[lpj]<parT[ltmin]) ltmin=lpj;
	else if (parT[lpj]>parT[ltmax]) ltmax=lpj;
      }
      if (ltmin!=lmin && ltmin!=lmax) TheTZ.PolygonInsert(thePi(ltmin));
      if (ltmax!=lmin && ltmax!=lmax) TheTZ.PolygonInsert(thePi(ltmax));
    }
    
    if (edgeSP) TheTZ.PolygonInsert(Intf_SectionPoint
				    (gp_Pnt2d (BegO.X()+ (segO.X()*parOSP),
					       BegO.Y()+ (segO.Y()*parOSP)),
				     Intf_EDGE,iObje1,parOSP,
				     Intf_EDGE,iObje2,parTSP,sinTeta));

    Standard_Integer nbtz=myTZones.Length();
#if 0 
    Standard_Integer decaltz=0;
    for (Standard_Integer ltz=1; ltz<=nbtz; ltz++) {
      if (TheTZ.HasCommonRange(myTZones(ltz-decaltz))) {
	TheTZ.Append(myTZones(ltz-decaltz));
	myTZones.Remove(ltz-decaltz);
	decaltz++;
      }
    }
    myTZones.Append(TheTZ);
#else 
    TColStd_ListOfInteger LIndex;
    for (Standard_Integer ltz=1; ltz<=nbtz; ltz++) {
      if (TheTZ.HasCommonRange(myTZones(ltz))) {
	LIndex.Append(ltz);
      }
    }
    //------------------------------------------------------------------------
    //--   The list is parsed in ascending order by index, zone and tg
    //--
    if(LIndex.IsEmpty()) { 
      myTZones.Append(TheTZ);
    }
    else {
      Standard_Integer indexfirst = LIndex.First();
      LIndex.RemoveFirst();
      Standard_Integer decal = 0;
      myTZones(indexfirst).Append(TheTZ);
      while(!LIndex.IsEmpty()) {
	Standard_Integer index = LIndex.First();
	LIndex.RemoveFirst();
	myTZones(indexfirst).Append(myTZones(index-decal));
	myTZones.Remove(index-decal);
	decal++;
      }
    }
#endif
  }
}
