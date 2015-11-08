// Created on: 1993-06-23
// Created by: Didier PIFFAULT
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


#include <Bnd_Box.hxx>
#include <Bnd_Box2d.hxx>
#include <ElCLib.hxx>
#include <gp_Hypr.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pln.hxx>
#include <gp_XY.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_Conic.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <IntAna_IntConicQuad.hxx>
#include <Intf_Tool.hxx>
#include <Precision.hxx>
#include <Standard_OutOfRange.hxx>

//=======================================================================
//function : Intf_Tool
//purpose  : 
//=======================================================================
Intf_Tool::Intf_Tool()
     : nbSeg(0)
{}

//=======================================================================
//function : Lin2dBox
//purpose  : 
//=======================================================================

void  Intf_Tool::Lin2dBox(const gp_Lin2d& L2d, 
			 const Bnd_Box2d& domain, 
			 Bnd_Box2d& boxLin)
{
  nbSeg=0;
  boxLin.SetVoid();
  if        (domain.IsWhole()) {
    boxLin.Set(L2d.Location(), L2d.Direction());
    boxLin.Add(L2d.Direction().Reversed());
    nbSeg=1;
    beginOnCurve[0]=-Precision::Infinite();
    endOnCurve[0]=Precision::Infinite();
    return;
  }
  else if   (domain.IsVoid())  return;

  Standard_Real xmin, xmax, ymin, ymax;
  Standard_Real Xmin=0, Xmax=0, Ymin=0, Ymax=0;
  Standard_Real parmin=-Precision::Infinite();
  Standard_Real parmax=Precision::Infinite();
  Standard_Real parcur, par1,par2;
  Standard_Boolean xToSet, yToSet;

  domain.Get(xmin,ymin,xmax,ymax);


  if      (L2d.Direction().XY().X()>0.) {
    if (domain.IsOpenXmin()) parmin=-Precision::Infinite();
    else parmin=(xmin-L2d.Location().XY().X())/L2d.Direction().XY().X();
    if (domain.IsOpenXmax()) parmax=Precision::Infinite();
    else parmax=(xmax-L2d.Location().XY().X())/L2d.Direction().XY().X();
    xToSet=Standard_True;
  }
  else if (L2d.Direction().XY().X()<0.) {
    if (domain.IsOpenXmax()) parmin=-Precision::Infinite();
    else parmin=(xmax-L2d.Location().XY().X())/L2d.Direction().XY().X();
    if (domain.IsOpenXmin()) parmax=Precision::Infinite();
    else parmax=(xmin-L2d.Location().XY().X())/L2d.Direction().XY().X();
    xToSet=Standard_True;
  }
  else { // Parallel to axis  X
    if (L2d.Location().XY().X()<xmin || xmax<L2d.Location().XY().X())
      return;
    Xmin=L2d.Location().XY().X();
    Xmax=L2d.Location().XY().X();
    xToSet=Standard_False;
  }

  if      (L2d.Direction().XY().Y()>0.) {
    if (domain.IsOpenYmin()) parcur=-Precision::Infinite();
    else parcur=(ymin-L2d.Location().XY().Y())/L2d.Direction().XY().Y();
    parmin=Max(parmin, parcur);
    if (domain.IsOpenYmax()) parcur=Precision::Infinite();
    else parcur=(ymax-L2d.Location().XY().Y())/L2d.Direction().XY().Y();
    parmax=Min(parmax, parcur);
    yToSet=Standard_True;
  }
  else if (L2d.Direction().XY().Y()<0.) {
    if (domain.IsOpenYmax()) parcur=-Precision::Infinite();
    else parcur=(ymax-L2d.Location().XY().Y())/L2d.Direction().XY().Y();
    parmin=Max(parmin, parcur);
    if (domain.IsOpenYmin()) parcur=Precision::Infinite();
    else parcur=(ymin-L2d.Location().XY().Y())/L2d.Direction().XY().Y();
    parmax=Min(parmax, parcur);
    yToSet=Standard_True;
  }
  else { // Parallel to axis  Y
    if (L2d.Location().XY().Y()<ymin || ymax<L2d.Location().XY().Y())
      return;
    Ymin=L2d.Location().XY().Y();
    Ymax=L2d.Location().XY().Y();
    yToSet=Standard_False;
  }

  nbSeg++;
  beginOnCurve[0]=parmin;
  endOnCurve[0]=parmax;

  if (xToSet) {
    par1=L2d.Location().XY().X()+parmin*L2d.Direction().XY().X();
    par2=L2d.Location().XY().X()+parmax*L2d.Direction().XY().X();
    Xmin=Min(par1, par2);
    Xmax=Max(par1, par2);
  }

  if (yToSet) {
    par1=L2d.Location().XY().Y()+parmin*L2d.Direction().XY().Y();
    par2=L2d.Location().XY().Y()+parmax*L2d.Direction().XY().Y();
    Ymin=Min(par1, par2);
    Ymax=Max(par1, par2);
  }

  boxLin.Update(Xmin, Ymin, Xmax, Ymax);
}

//=======================================================================
//function : Hypr2dBox
//purpose  : 
//=======================================================================

void  Intf_Tool::Hypr2dBox(const gp_Hypr2d& theHypr2d, 
			  const Bnd_Box2d& domain, 
			  Bnd_Box2d& boxHypr2d)
{
  nbSeg=0;
  boxHypr2d.SetVoid();
  if        (domain.IsWhole()) {
    boxHypr2d.SetWhole();
    nbSeg=1;
    beginOnCurve[0]=-Precision::Infinite();
    endOnCurve[0]=Precision::Infinite();
    return;
  }
  else if   (domain.IsVoid())  return;

  Standard_Integer nbPi=Inters2d(theHypr2d, domain);

  if (nbPi>0) {
    Standard_Real Xmin, Xmax, Ymin, Ymax;

    domain.Get(Xmax, Ymax, Xmin, Ymin);

    Standard_Integer npi;
    for (npi=0; npi<nbPi; npi++) {
      Xmin=Min(Xmin, xint[npi]);
      Xmax=Max(Xmax, xint[npi]);
      Ymin=Min(Ymin, yint[npi]);
      Ymax=Max(Ymax, yint[npi]);
    }
    boxHypr2d.Update(Xmin, Ymin, Xmax, Ymax);

    Standard_Integer npj, npk;
    Standard_Real parmin;
    for (npi=0; npi<nbPi; npi++) {
      npk=npi;
      for (npj=npi+1; npj<nbPi; npj++) 
	if (parint[npj]<parint[npk]) npk=npj;
      if (npk!=npi) {
	parmin=parint[npk];
	parint[npk]=parint[npi];
	parint[npi]=parmin;
	npj=bord[npk];
	bord[npk]=bord[npi];
	bord[npi]=npj;
      }
    }
    
    gp_Pnt2d Pn;
    gp_Vec2d Tan;
    Standard_Real sinan=0;
    Standard_Boolean out=Standard_True;

    for (npi=0; npi<nbPi; npi++) {
      ElCLib::D1(parint[npi], theHypr2d, Pn, Tan);
      switch (bord[npi]) {
      case 1 :
	sinan=gp_XY(-1.,0.)^Tan.XY();
	break;
      case 2 :
	sinan=gp_XY(0.,-1.)^Tan.XY();
	break;
      case 3 :
	sinan=gp_XY(1.,0.)^Tan.XY();
	break;
      case 4 :
	sinan=gp_XY(0.,1.)^Tan.XY();
	break;
      }
      if (Abs(sinan)>Precision::Angular()) {
	if (sinan>0.) {
	  out=Standard_False;
	  beginOnCurve[nbSeg]=parint[npi];
	  nbSeg++;
	}
	else {
	  if (out) {
	    beginOnCurve[nbSeg]=-Precision::Infinite();
	    nbSeg++;
	  }
	  endOnCurve[nbSeg-1]=parint[npi];
	  out=Standard_True;

	  Standard_Integer ipmin;
	  if(beginOnCurve[nbSeg-1] < -10.) ipmin = -10;
	  else ipmin =  (Standard_Integer)(beginOnCurve[nbSeg-1]);

	  Standard_Integer ipmax;
	  if(endOnCurve[nbSeg-1] > 10.) ipmax = 10;
	  else ipmax =  (Standard_Integer)(endOnCurve[nbSeg-1]);

	  //Standard_Integer ipmin=Max((Standard_Integer)(beginOnCurve[nbSeg-1]),
		//		     -10);
	  //Standard_Integer ipmax=Min((Standard_Integer)(endOnCurve[nbSeg-1]), 
		//		     10);
	  ipmin=ipmin*10+1;
	  ipmax=ipmax*10-1;
	  Standard_Integer ip, pas=1;
	  for (ip=ipmin; ip<=ipmax; ip+=pas) {
	    boxHypr2d.Add(ElCLib::Value(Standard_Real(ip)/10., theHypr2d));
	    if (Abs(ip)<=10) pas=1;
	    else             pas=10;
	  }
	}
      }
    }
  }
  else if (!domain.IsOut(ElCLib::Value(0., theHypr2d))) {
    boxHypr2d=domain;
    beginOnCurve[0]=-Precision::Infinite();
    endOnCurve[0]=Precision::Infinite();
    nbSeg=1;
  }
}

//=======================================================================
//function : Inters2d
//purpose  : 
//=======================================================================

Standard_Integer Intf_Tool::Inters2d(const gp_Hypr2d& theCurv,
				    const Bnd_Box2d& Domain)
{
  Standard_Integer nbpi=0;
  Standard_Integer npi;
  Standard_Real xmin, xmax, ymin, ymax;

  Domain.Get(xmin,ymin,xmax,ymax);

  if (!Domain.IsOpenYmax()) {
    gp_Lin2d L1(gp_Pnt2d(0., ymax), gp_Dir2d(-1., 0.));
    IntAna2d_AnaIntersection Inters1(theCurv, IntAna2d_Conic(L1));
    if (Inters1.IsDone()) {
      if (!Inters1.IsEmpty()) {
	for (npi=1; npi<=Inters1.NbPoints(); npi++) {
	  xint[nbpi]=Inters1.Point(npi).Value().X();
	  if (xmin < xint[nbpi] && xint[nbpi] <=xmax) {
	    yint[nbpi]=ymax;
	    parint[nbpi]=Inters1.Point(npi).ParamOnFirst();
            bord[nbpi]=1;
            nbpi++;
	  }
	}
      }
    }
  }

  if (!Domain.IsOpenXmin()) {
    gp_Lin2d L2(gp_Pnt2d(xmin, 0.), gp_Dir2d(0., -1.));
    IntAna2d_AnaIntersection Inters2(theCurv, IntAna2d_Conic(L2));
    if (Inters2.IsDone()) {
      if (!Inters2.IsEmpty()) {
	for (npi=1; npi<=Inters2.NbPoints(); npi++) {
	  yint[nbpi]=Inters2.Point(npi).Value().Y();
	  if (ymin < yint[nbpi] && yint[nbpi] <=ymax) {
	    xint[nbpi]=xmin;
	    parint[nbpi]=Inters2.Point(npi).ParamOnFirst();
            bord[nbpi]=2;
            nbpi++;
	  }
	}
      }
    }
  }

  if (!Domain.IsOpenYmin()) {
    gp_Lin2d L3(gp_Pnt2d(0., ymin), gp_Dir2d(1., 0.));
    IntAna2d_AnaIntersection Inters3(theCurv, IntAna2d_Conic(L3));
    if (Inters3.IsDone()) {
      if (!Inters3.IsEmpty()) {
	for (npi=1; npi<=Inters3.NbPoints(); npi++) {
	  xint[nbpi]=Inters3.Point(npi).Value().X();
	  if (xmin <=xint[nbpi] && xint[nbpi] <xmax) {
	    yint[nbpi]=ymin;
	    parint[nbpi]=Inters3.Point(npi).ParamOnFirst();
            bord[nbpi]=3;
            nbpi++;
	  }
	}
      }
    }
  }

  if (!Domain.IsOpenXmax()) {
    gp_Lin2d L4(gp_Pnt2d(xmax, 0.), gp_Dir2d(0., 1.));
    IntAna2d_AnaIntersection Inters4(theCurv, IntAna2d_Conic(L4));
    if (Inters4.IsDone()) {
      if (!Inters4.IsEmpty()) {
	for (npi=1; npi<=Inters4.NbPoints(); npi++) {
	  yint[nbpi]=Inters4.Point(npi).Value().Y();
	  if (ymin <= yint[nbpi] && yint[nbpi] < ymax) {
	    xint[nbpi]=xmax;
	    parint[nbpi]=Inters4.Point(npi).ParamOnFirst();
            bord[nbpi]=4;
            nbpi++;
	  }
	}
      }
    }
  }
  return nbpi;
}

//=======================================================================
//function : Parab2dBox
//purpose  : 
//=======================================================================

void  Intf_Tool::Parab2dBox(const gp_Parab2d& theParab2d, 
			   const Bnd_Box2d& domain, 
			   Bnd_Box2d& boxParab2d)
{
  nbSeg=0;
  boxParab2d.SetVoid();
  if        (domain.IsWhole()) {
    boxParab2d.SetWhole();
    nbSeg=1;
    beginOnCurve[0]=-Precision::Infinite();
    endOnCurve[0]=Precision::Infinite();
    return;
  }
  else if   (domain.IsVoid())  return;

  Standard_Integer nbPi=Inters2d(theParab2d, domain);

  if (nbPi>0) {
    Standard_Real Xmin, Xmax, Ymin, Ymax;

    domain.Get(Xmax, Ymax, Xmin, Ymin);

    Standard_Integer npi;
    for (npi=0; npi<nbPi; npi++) {
      Xmin=Min(Xmin, xint[npi]);
      Xmax=Max(Xmax, xint[npi]);
      Ymin=Min(Ymin, yint[npi]);
      Ymax=Max(Ymax, yint[npi]);
    }
    boxParab2d.Update(Xmin, Ymin, Xmax, Ymax);

    Standard_Integer npj, npk;
    Standard_Real parmin;
    for (npi=0; npi<nbPi; npi++) {
      npk=npi;
      for (npj=npi+1; npj<nbPi; npj++) 
	if (parint[npj]<parint[npk]) npk=npj;
      if (npk!=npi) {
	parmin=parint[npk];
	parint[npk]=parint[npi];
	parint[npi]=parmin;
	npj=bord[npk];
	bord[npk]=bord[npi];
	bord[npi]=npj;
      }
    }
    
    gp_Pnt2d Pn;
    gp_Vec2d Tan;
    Standard_Real sinan=0;
    Standard_Boolean out=Standard_True;

    for (npi=0; npi<nbPi; npi++) {
      ElCLib::D1(parint[npi], theParab2d, Pn, Tan);
      switch (bord[npi]) {
      case 1 :
	sinan=gp_XY(-1.,0.)^Tan.XY();
	break;
      case 2 :
	sinan=gp_XY(0.,-1.)^Tan.XY();
	break;
      case 3 :
	sinan=gp_XY(1.,0.)^Tan.XY();
	break;
      case 4 :
	sinan=gp_XY(0.,1.)^Tan.XY();
	break;
      }
      if (Abs(sinan)>Precision::Angular()) {
	if (sinan>0.) {
	  out=Standard_False;
	  beginOnCurve[nbSeg]=parint[npi];
	  nbSeg++;
	}
	else {
	  if (out) {
	    beginOnCurve[nbSeg]=-Precision::Infinite();
	    nbSeg++;
	  }
	  endOnCurve[nbSeg-1]=parint[npi];
	  out=Standard_True;

	  Standard_Integer ipmin;
	  if(beginOnCurve[nbSeg-1] < -10.) ipmin = -10;
	  else ipmin =  (Standard_Integer)(beginOnCurve[nbSeg-1]);

	  Standard_Integer ipmax;
	  if(endOnCurve[nbSeg-1] > 10.) ipmax = 10;
	  else ipmax =  (Standard_Integer)(endOnCurve[nbSeg-1]);

	  //Standard_Integer ipmin=Max((Standard_Integer)(beginOnCurve[nbSeg-1]),
		//		     -10);
	  //Standard_Integer ipmax=Min((Standard_Integer)(endOnCurve[nbSeg-1]), 
		//		     10);
	  ipmin=ipmin*10+1;
	  ipmax=ipmax*10-1;
	  Standard_Integer ip, pas=1;
	  for (ip=ipmin; ip<=ipmax; ip+=pas) {
	    boxParab2d.Add(ElCLib::Value(Standard_Real(ip)/10., theParab2d));
	    if (Abs(ip)<=10) pas=1;
	    else             pas=10;
	  }
	}
      }
    }
  }
  else if (!domain.IsOut(ElCLib::Value(0., theParab2d))) {
    boxParab2d=domain;
    beginOnCurve[0]=-Precision::Infinite();
    endOnCurve[0]=Precision::Infinite();
    nbSeg=1;
  }
}

//=======================================================================
//function : Inters2d
//purpose  : 
//=======================================================================

Standard_Integer Intf_Tool::Inters2d(const gp_Parab2d& theCurv,
				    const Bnd_Box2d& Domain)
{
  Standard_Integer nbpi=0;
  Standard_Integer npi;
  Standard_Real xmin, xmax, ymin, ymax;

  Domain.Get(xmin,ymin,xmax,ymax);

  if (!Domain.IsOpenYmax()) {
    gp_Lin2d L1(gp_Pnt2d(0., ymax), gp_Dir2d(-1., 0.));
    IntAna2d_AnaIntersection Inters1(theCurv, IntAna2d_Conic(L1));
    if (Inters1.IsDone()) {
      if (!Inters1.IsEmpty()) {
	for (npi=1; npi<=Inters1.NbPoints(); npi++) {
	  xint[nbpi]=Inters1.Point(npi).Value().X();
	  if (xmin < xint[nbpi] && xint[nbpi] <=xmax) {
	    yint[nbpi]=ymax;
	    parint[nbpi]=Inters1.Point(npi).ParamOnFirst();
            bord[nbpi]=1;
            nbpi++;
	  }
	}
      }
    }
  }

  if (!Domain.IsOpenXmin()) {
    gp_Lin2d L2(gp_Pnt2d(xmin, 0.), gp_Dir2d(0., -1.));
    IntAna2d_AnaIntersection Inters2(theCurv, IntAna2d_Conic(L2));
    if (Inters2.IsDone()) {
      if (!Inters2.IsEmpty()) {
	for (npi=1; npi<=Inters2.NbPoints(); npi++) {
	  yint[nbpi]=Inters2.Point(npi).Value().Y();
	  if (ymin < yint[nbpi] && yint[nbpi] <=ymax) {
	    xint[nbpi]=xmin;
	    parint[nbpi]=Inters2.Point(npi).ParamOnFirst();
            bord[nbpi]=2;
            nbpi++;
	  }
	}
      }
    }
  }

  if (!Domain.IsOpenYmin()) {
    gp_Lin2d L3(gp_Pnt2d(0., ymin), gp_Dir2d(1., 0.));
    IntAna2d_AnaIntersection Inters3(theCurv, IntAna2d_Conic(L3));
    if (Inters3.IsDone()) {
      if (!Inters3.IsEmpty()) {
	for (npi=1; npi<=Inters3.NbPoints(); npi++) {
	  xint[nbpi]=Inters3.Point(npi).Value().X();
	  if (xmin <=xint[nbpi] && xint[nbpi] <xmax) {
	    yint[nbpi]=ymin;
	    parint[nbpi]=Inters3.Point(npi).ParamOnFirst();
            bord[nbpi]=3;
            nbpi++;
	  }
	}
      }
    }
  }

  if (!Domain.IsOpenXmax()) {
    gp_Lin2d L4(gp_Pnt2d(xmax, 0.), gp_Dir2d(0., 1.));
    IntAna2d_AnaIntersection Inters4(theCurv, IntAna2d_Conic(L4));
    if (Inters4.IsDone()) {
      if (!Inters4.IsEmpty()) {
	for (npi=1; npi<=Inters4.NbPoints(); npi++) {
	  yint[nbpi]=Inters4.Point(npi).Value().Y();
	  if (ymin <= yint[nbpi] && yint[nbpi] < ymax) {
	    xint[nbpi]=xmax;
	    parint[nbpi]=Inters4.Point(npi).ParamOnFirst();
            bord[nbpi]=4;
            nbpi++;
	  }
	}
      }
    }
  }
  return nbpi;
}




//=======================================================================
//function : LinBox
//purpose  : 
//=======================================================================

void  Intf_Tool::LinBox(const gp_Lin& L, 
		       const Bnd_Box& domain, 
		       Bnd_Box& boxLin)
{
  nbSeg=0;
  boxLin.SetVoid();
  if        (domain.IsWhole()) {
    boxLin.Set(L.Location(), L.Direction());
    boxLin.Add(L.Direction().Reversed());
    nbSeg=1;
    beginOnCurve[0]=-Precision::Infinite();
    endOnCurve[0]=Precision::Infinite();
    return;
  }
  else if   (domain.IsVoid())  return;

  Standard_Real xmin, xmax, ymin, ymax, zmin, zmax;
  Standard_Real Xmin=0, Xmax=0, Ymin=0, Ymax=0, Zmin=0, Zmax=0;
  Standard_Real parmin=-Precision::Infinite();
  Standard_Real parmax=Precision::Infinite();
  Standard_Real parcur, par1,par2;
  Standard_Boolean xToSet, yToSet, zToSet;

  domain.Get(xmin,ymin,zmin,xmax,ymax,zmax);


  if      (L.Direction().XYZ().X()>0.) {
    if (domain.IsOpenXmin()) parmin=-Precision::Infinite();
    else parmin=(xmin-L.Location().XYZ().X())/L.Direction().XYZ().X();
    if (domain.IsOpenXmax()) parmax=Precision::Infinite();
    else parmax=(xmax-L.Location().XYZ().X())/L.Direction().XYZ().X();
    xToSet=Standard_True;
  }
  else if (L.Direction().XYZ().X()<0.) {
    if (domain.IsOpenXmax()) parmin=-Precision::Infinite();
    else parmin=(xmax-L.Location().XYZ().X())/L.Direction().XYZ().X();
    if (domain.IsOpenXmin()) parmax=Precision::Infinite();
    else parmax=(xmin-L.Location().XYZ().X())/L.Direction().XYZ().X();
    xToSet=Standard_True;
  }
  else { // Perpendiculaire a l axe  X
    if (L.Location().XYZ().X()<xmin || xmax<L.Location().XYZ().X())
      return;
    Xmin=L.Location().XYZ().X();
    Xmax=L.Location().XYZ().X();
    xToSet=Standard_False;
  }

  if      (L.Direction().XYZ().Y()>0.) {
    if (domain.IsOpenYmin()) parcur=-Precision::Infinite();
    else parcur=(ymin-L.Location().XYZ().Y())/L.Direction().XYZ().Y();
    parmin=Max(parmin, parcur);
    if (domain.IsOpenYmax()) parcur=Precision::Infinite();
    else parcur=(ymax-L.Location().XYZ().Y())/L.Direction().XYZ().Y();
    parmax=Min(parmax, parcur);
    yToSet=Standard_True;
  }
  else if (L.Direction().XYZ().Y()<0.) {
    if (domain.IsOpenYmax()) parcur=-Precision::Infinite();
    else parcur=(ymax-L.Location().XYZ().Y())/L.Direction().XYZ().Y();
    parmin=Max(parmin, parcur);
    if (domain.IsOpenYmin()) parcur=Precision::Infinite();
    else parcur=(ymin-L.Location().XYZ().Y())/L.Direction().XYZ().Y();
    parmax=Min(parmax, parcur);
    yToSet=Standard_True;
  }
  else { // Perpendiculaire a l axe  Y
    if (L.Location().XYZ().Y()<ymin || ymax<L.Location().XYZ().Y())
      return;
    Ymin=L.Location().XYZ().Y();
    Ymax=L.Location().XYZ().Y();
    yToSet=Standard_False;
  }

  if      (L.Direction().XYZ().Z()>0.) {
    if (domain.IsOpenZmin()) parcur=-Precision::Infinite();
    else parcur=(zmin-L.Location().XYZ().Z())/L.Direction().XYZ().Z();
    parmin=Max(parmin, parcur);
    if (domain.IsOpenZmax()) parcur=Precision::Infinite();
    else parcur=(zmax-L.Location().XYZ().Z())/L.Direction().XYZ().Z();
    parmax=Min(parmax, parcur);
    zToSet=Standard_True;
  }
  else if (L.Direction().XYZ().Z()<0.) {
    if (domain.IsOpenZmax()) parcur=-Precision::Infinite();
    else parcur=(zmax-L.Location().XYZ().Z())/L.Direction().XYZ().Z();
    parmin=Max(parmin, parcur);
    if (domain.IsOpenZmin()) parcur=Precision::Infinite();
    else parcur=(zmin-L.Location().XYZ().Z())/L.Direction().XYZ().Z();
    parmax=Min(parmax, parcur);
    zToSet=Standard_True;
  }
  else { // Perpendicular to axis Z
    if (L.Location().XYZ().Z()<zmin || zmax<L.Location().XYZ().Z())
      return;
    Zmin=L.Location().XYZ().Z();
    Zmax=L.Location().XYZ().Z();
    zToSet=Standard_False;
  }

  nbSeg++;
  beginOnCurve[0]=parmin;
  endOnCurve[0]=parmax;

  if (xToSet) {
    par1=L.Location().XYZ().X()+parmin*L.Direction().XYZ().X();
    par2=L.Location().XYZ().X()+parmax*L.Direction().XYZ().X();
    Xmin=Min(par1, par2);
    Xmax=Max(par1, par2);
  }

  if (yToSet) {
    par1=L.Location().XYZ().Y()+parmin*L.Direction().XYZ().Y();
    par2=L.Location().XYZ().Y()+parmax*L.Direction().XYZ().Y();
    Ymin=Min(par1, par2);
    Ymax=Max(par1, par2);
  }

  if (zToSet) {
    par1=L.Location().XYZ().Z()+parmin*L.Direction().XYZ().Z();
    par2=L.Location().XYZ().Z()+parmax*L.Direction().XYZ().Z();
    Zmin=Min(par1, par2);
    Zmax=Max(par1, par2);
  }

  boxLin.Update(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
}

//=======================================================================
//function : HyprBox
//purpose  : 
//=======================================================================
void Intf_Tool::HyprBox(const gp_Hypr& theHypr, 
			const Bnd_Box& domain, 
			Bnd_Box& boxHypr)
{
  nbSeg=0;
  boxHypr.SetVoid();
  
  if (domain.IsWhole()) {
    boxHypr.SetWhole();
    nbSeg=1;
    //beginOnCurve[0]=-Precision::Infinite();
    //endOnCurve[0]=Precision::Infinite();
    beginOnCurve[0]=-100.;
    endOnCurve[0]=100.;
    return;
  }
  else if (domain.IsVoid())  {
    return;
  }
  //
  Standard_Integer nbPi;
  //
  nbPi=Inters3d(theHypr, domain);
  if (nbPi>0) {
    Standard_Integer npi;
    Standard_Real Xmin, Ymin, Zmin, Xmax, Ymax, Zmax;
    //
    domain.Get(Xmax, Ymax, Zmax, Xmin, Ymin, Zmin);
    //
    for (npi=0; npi<nbPi; npi++) {
      Xmin=Min(Xmin, xint[npi]);
      Xmax=Max(Xmax, xint[npi]);
      Ymin=Min(Ymin, yint[npi]);
      Ymax=Max(Ymax, yint[npi]);
      Zmin=Min(Zmin, zint[npi]);
      Zmax=Max(Zmax, yint[npi]);
    }
    boxHypr.Update(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
    //
    gp_Pnt Pn;
    gp_Vec Tan;
    Standard_Real sinan=0;
    Standard_Boolean out=Standard_True;

    for (npi=0; npi<nbPi; npi++) {
      ElCLib::D1(parint[npi], theHypr, Pn, Tan);
      switch (bord[npi]) {
      case 1 : sinan=gp_XYZ( 1., 0., 0.)*Tan.XYZ(); break;
      case 2 : sinan=gp_XYZ( 0., 1., 0.)*Tan.XYZ(); break;
      case 3 : sinan=gp_XYZ( 0., 0., 1.)*Tan.XYZ(); break;
      case 4 : sinan=gp_XYZ(-1., 0., 0.)*Tan.XYZ(); break;
      case 5 : sinan=gp_XYZ( 0.,-1., 0.)*Tan.XYZ(); break;
      case 6 : sinan=gp_XYZ( 0., 0.,-1.)*Tan.XYZ(); break;
      }
      if (Abs(sinan)>Precision::Angular()) {
	if (sinan>0.) {
	  out=Standard_False;
	  beginOnCurve[nbSeg]=parint[npi];
	  //// modified by jgv, 10.11.2009 /////
	  endOnCurve[nbSeg] = 10.;
	  //////////////////////////////////////
	  nbSeg++;
	}
	else {
	  if (out) {
	    //modified by NIZNHY-PKV Fri Jul 11 13:59:10 2008f
	    beginOnCurve[nbSeg]=-10.;
	    //beginOnCurve[nbSeg]=-Precision::Infinite();
	    //modified by NIZNHY-PKV Fri Jul 11 13:59:13 2008t
	    nbSeg++;
	  }
	  endOnCurve[nbSeg-1]=parint[npi];
	  out=Standard_True;
	  //
	  //modified by NIZNHY-PKV Fri Jul 11 13:54:54 2008f
	  Standard_Real ipmin, ipmax, ip, pas;
	  //
	  ipmin=-10.;
	  if (beginOnCurve[nbSeg-1]>ipmin) {
	    ipmin=beginOnCurve[nbSeg-1];
	  }
	  ipmax=10.;
	  if (endOnCurve[nbSeg-1]<ipmax) {
	    ipmax=endOnCurve[nbSeg-1];
	  }
	  ipmin=ipmin*10.+1.;
	  ipmax=ipmax*10.-1.;
	  //
	  pas=1.;
	  for (ip=ipmin; ip<=ipmax; ip+=pas) {
	    boxHypr.Add(ElCLib::Value(ip/10., theHypr));
	    pas=10.;
	    if (fabs(ip)<=10.) {
	      pas=1.;
	    }
	  }
	  /*
	  Standard_Integer ipmin=Max((Standard_Integer)(beginOnCurve[nbSeg-1]), -10);
	  Standard_Integer ipmax=Min((Standard_Integer)(endOnCurve[nbSeg-1]),    10);
	  
	  ipmin=ipmin*10+1;
	  ipmax=ipmax*10-1;
	  Standard_Integer ip, pas=1;
	  for (ip=ipmin; ip<=ipmax; ip+=pas) {
	    boxHypr.Add(ElCLib::Value(Standard_Real(ip)/10., theHypr));
	    
	    if (Abs(ip)<=10) {
	      pas=1;
	    }
	    else {
	      pas=10;
	    }
	  }
	  */
	  //modified by NIZNHY-PKV Fri Jul 11 13:55:04 2008t
	}
      }
    }
  }//if (nbPi>0) {
  else if (!domain.IsOut(ElCLib::Value(0., theHypr))) {
    boxHypr=domain;
    //beginOnCurve[0]=-Precision::Infinite();
    //endOnCurve[0]=Precision::Infinite();
    beginOnCurve[0]=-100.;
    endOnCurve[0]=100.;
    nbSeg=1;
  }
}

//=======================================================================
//function : Inters3d
//purpose  : 
//=======================================================================

Standard_Integer Intf_Tool::Inters3d(const gp_Hypr& theCurv,
				    const Bnd_Box& Domain)
{
  Standard_Integer nbpi=0;
  Standard_Integer npi;
  Standard_Real xmin, ymin, zmin, xmax, ymax, zmax;

  Domain.Get(xmin, ymin, zmin, xmax, ymax, zmax);

  if (!Domain.IsOpenXmin()) {
    IntAna_IntConicQuad Inters1(theCurv, 
      gp_Pln(1., 0., 0., -xmin),
      Precision::Angular());
    if (Inters1.IsDone()) {
      if (!Inters1.IsInQuadric()) {
        for (npi=1; npi<=Inters1.NbPoints(); npi++) {
          yint[nbpi]=Inters1.Point(npi).Y();
          zint[nbpi]=Inters1.Point(npi).Z();
          if (ymin <=yint[nbpi] && yint[nbpi] < ymax &&
            zmin <=zint[nbpi] && zint[nbpi] < zmax) {
              xint[nbpi]=xmin;
              parint[nbpi]=Inters1.ParamOnConic(npi);
              bord[nbpi]=1;
              nbpi++;
          }
        }
      }
    }
  }

  if (!Domain.IsOpenYmin()) {
    IntAna_IntConicQuad Inters1(theCurv, 
      gp_Pln( 0., 1., 0., -ymin),
      Precision::Angular());
    if (Inters1.IsDone()) {
      if (!Inters1.IsInQuadric()) {
        for (npi=1; npi<=Inters1.NbPoints(); npi++) {
          xint[nbpi]=Inters1.Point(npi).X();
          zint[nbpi]=Inters1.Point(npi).Z();
          if (xmin < xint[nbpi] && xint[nbpi] <=xmax &&
            zmin <=zint[nbpi] && zint[nbpi] < zmax) {
              yint[nbpi]=ymin;
              parint[nbpi]=Inters1.ParamOnConic(npi);
              bord[nbpi]=2;
              nbpi++;
          }
        }
      }
    }
  }

  if (!Domain.IsOpenZmin()) {
    IntAna_IntConicQuad Inters1(theCurv, 
      gp_Pln( 0., 0., 1., -zmin),
      Precision::Angular());
    if (Inters1.IsDone()) {
      if (!Inters1.IsInQuadric()) {
        for (npi=1; npi<=Inters1.NbPoints(); npi++) {
          xint[nbpi]=Inters1.Point(npi).X();
          yint[nbpi]=Inters1.Point(npi).Y();
          if (xmin < xint[nbpi] && xint[nbpi] <=xmax &&
            ymin < yint[nbpi] && yint[nbpi] <=ymax) {
              zint[nbpi]=zmin;
              parint[nbpi]=Inters1.ParamOnConic(npi);
              bord[nbpi]=3;
              nbpi++;
          }
        }
      }
    }
  }

  if (!Domain.IsOpenXmax()) {
    IntAna_IntConicQuad Inters1(theCurv, 
      gp_Pln(-1., 0., 0., xmax),
      Precision::Angular());
    if (Inters1.IsDone()) {
      if (!Inters1.IsInQuadric()) {
        for (npi=1; npi<=Inters1.NbPoints(); npi++) {
          yint[nbpi]=Inters1.Point(npi).Y();
          zint[nbpi]=Inters1.Point(npi).Z();
          if (ymin < yint[nbpi] && yint[nbpi] <=ymax &&
            zmin < zint[nbpi] && zint[nbpi] <=zmax) {
              xint[nbpi]=xmax;
              parint[nbpi]=Inters1.ParamOnConic(npi);
              bord[nbpi]=4;
              nbpi++;
          }
        }
      }
    }
  }

  if (!Domain.IsOpenYmax()) {
    IntAna_IntConicQuad Inters1(theCurv, 
      gp_Pln( 0.,-1., 0., ymax),
      Precision::Angular());
    if (Inters1.IsDone()) {
      if (!Inters1.IsInQuadric()) {
        for (npi=1; npi<=Inters1.NbPoints(); npi++) {
          xint[nbpi]=Inters1.Point(npi).X();
          zint[nbpi]=Inters1.Point(npi).Z();
          if (xmin <=xint[nbpi] && xint[nbpi] < xmax &&
            zmin < zint[nbpi] && zint[nbpi] <=zmax) {
              yint[nbpi]=ymax;
              parint[nbpi]=Inters1.ParamOnConic(npi);
              bord[nbpi]=5;
              nbpi++;
          }
        }
      }
    }
  }

  if (!Domain.IsOpenZmax()) {
    IntAna_IntConicQuad Inters1(theCurv, 
      gp_Pln( 0., 0.,-1., zmax),
      Precision::Angular());
    if (Inters1.IsDone()) {
      if (!Inters1.IsInQuadric()) {
        for (npi=1; npi<=Inters1.NbPoints(); npi++) {
          xint[nbpi]=Inters1.Point(npi).X();
          yint[nbpi]=Inters1.Point(npi).Y();
          if (xmin <=xint[nbpi] && xint[nbpi] < xmax &&
            ymin <=yint[nbpi] && yint[nbpi] < ymax) {
              zint[nbpi]=zmax;
              parint[nbpi]=Inters1.ParamOnConic(npi);
              bord[nbpi]=6;
              nbpi++;
          }
        }
      }
    }
  }

  Standard_Integer aNbDiffPoints = nbpi;

  //Sort parint and check if parint contains several
  //matched values. If that is true they will be deleted.
  for(Standard_Integer i = nbpi - 1; i > 0 ; i--)
  {
    for(Standard_Integer j = 0; j < i; j++)
    {
      if(parint[i] <= parint[j])
      {
	std::swap (parint[i], parint[j]);
	std::swap (zint[i], zint[j]);
	std::swap (yint[i], yint[j]);
	std::swap (xint[i], xint[j]);
	std::swap (bord[i], bord[j]);
      }

      if((i < nbpi - 1) && IsEqual(parint[i], parint[i+1]))
      {
        aNbDiffPoints--;
        for(Standard_Integer k = i; k < aNbDiffPoints; k++)
        {
          parint[k] = parint[k+1];
          zint[k] = zint[k+1];
          yint[k] = yint[k+1];
          xint[k] = xint[k+1];
          bord[k] = bord[k+1];
        }
      }
    }
  }

  return aNbDiffPoints;
}


//=======================================================================
//function : ParabBox
//purpose  : 
//=======================================================================

void  Intf_Tool::ParabBox(const gp_Parab&, 
			 const Bnd_Box& domain, 
			 Bnd_Box& boxParab)
{
  nbSeg=0;
  boxParab.SetVoid();
  if        (domain.IsWhole()) {
    boxParab.SetWhole();
    nbSeg=1;
    beginOnCurve[0]=-Precision::Infinite();
    endOnCurve[0]=Precision::Infinite();
    return;
  }
  else if   (domain.IsVoid())  return;

  
}


//=======================================================================
//function : NbSegments
//purpose  : 
//=======================================================================

Standard_Integer Intf_Tool::NbSegments() const
{
  return nbSeg;
}

//=======================================================================
//function : BeginParam
//purpose  : 
//=======================================================================

Standard_Real Intf_Tool::BeginParam(const Standard_Integer SegmentNum) const
{
  Standard_OutOfRange_Raise_if(SegmentNum<1 || SegmentNum>nbSeg ,
			       "Intf_Tool::BeginParam");
  return beginOnCurve[SegmentNum-1];
}

//=======================================================================
//function : EndParam
//purpose  : 
//=======================================================================

Standard_Real Intf_Tool::EndParam(const Standard_Integer SegmentNum) const
{
  Standard_OutOfRange_Raise_if(SegmentNum<1 || SegmentNum>nbSeg ,
			       "Intf_Tool::EndParam");
  return endOnCurve[SegmentNum-1];
}

