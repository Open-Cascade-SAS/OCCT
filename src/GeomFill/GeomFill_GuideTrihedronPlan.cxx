// Created on: 1998-07-02
// Created by: Stephanie HUMEAU
// Copyright (c) 1998-1999 Matra Datavision
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


#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_HCurve.hxx>
#include <ElCLib.hxx>
#include <Geom_Plane.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomFill_Frenet.hxx>
#include <GeomFill_GuideTrihedronPlan.hxx>
#include <GeomFill_PlanFunc.hxx>
#include <GeomFill_TrihedronLaw.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <IntCurveSurface_HInter.hxx>
#include <IntCurveSurface_IntersectionPoint.hxx>
#include <math_FunctionRoot.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomFill_GuideTrihedronPlan,GeomFill_TrihedronWithGuide)

//#include <gp_Trsf2d.hxx>
//#include <Bnd_Box2d.hxx>
#if DRAW
#include <DrawTrSurf.hxx>
#endif

#ifdef OCCT_DEBUG
static void TracePlan(const Handle(Geom_Surface)& /*Plan*/)
{
  cout << "Pas d'intersection Guide/Plan" << endl;	
#if DRAW
  char* Temp = "ThePlan" ;
  DrawTrSurf::Set(Temp, Plan);
//  DrawTrSurf::Set("ThePlan", Plan);
#endif 
}
#endif

//==================================================================
//Function: InGoodPeriod
//Purpose : Recadre un paramtere
//==================================================================
static void InGoodPeriod(const Standard_Real Prec,
			 const Standard_Real  Period,
                         Standard_Real& Current)
{
  Standard_Real Diff=Current-Prec;
  Standard_Integer nb = (Standard_Integer ) IntegerPart(Diff/Period);
  Current -= nb*Period;
  Diff = Current-Prec;
  if (Diff > Period/2) Current -= Period;
  else if (Diff < -Period/2) Current += Period;
}

//=======================================================================
//function : GuideTrihedronPlan
//purpose  : Constructor
//=======================================================================
GeomFill_GuideTrihedronPlan::GeomFill_GuideTrihedronPlan (const Handle(Adaptor3d_HCurve)& theGuide) :
							  X(1,1),  
							  XTol(1,1),
							  Inf(1,1), Sup(1,1),
							  myStatus(GeomFill_PipeOk)
{
  myCurve.Nullify();
  myGuide = theGuide; // guide
  myTrimG = theGuide;
  myNbPts = 20;     // nb points pour calculs
  Pole = new (TColgp_HArray2OfPnt2d)(1,1,1,myNbPts);//tab pr stocker Pprime (pt sur guide)
  frenet = new (GeomFill_Frenet)();
  XTol.Init(1.e-6);
  XTol(1) = myGuide->Resolution(1.e-6);
}

//=======================================================================
//function : Init
//purpose  : calcule myNbPts points sur la courbe guide (<=> normale)
//=======================================================================
 void GeomFill_GuideTrihedronPlan::Init()
{
  myStatus = GeomFill_PipeOk;
  gp_Pnt P;
//  Bnd_Box2d Box;
//  Box.Update(-0.1, -0.1, 0.1, 0.1); // Taille minimal
  gp_Vec Tangent,Normal,BiNormal;
  Standard_Integer ii;
  Standard_Real t, DeltaG, w = 0.;
  Standard_Real f = myCurve->FirstParameter();
  Standard_Real l = myCurve->LastParameter();
  


  Handle(Geom_Plane) Plan;
  Handle(GeomAdaptor_HSurface) Pl;
  IntCurveSurface_IntersectionPoint PInt;
  IntCurveSurface_HInter Int;
  frenet->SetCurve(myCurve);
  DeltaG = (myGuide->LastParameter() -  myGuide->FirstParameter())/2;
  
  Inf(1) = myGuide->FirstParameter() - DeltaG;
  Sup(1) = myGuide->LastParameter() + DeltaG;

  if (!myGuide->IsPeriodic()) {
    myTrimG = myGuide->Trim(myGuide->FirstParameter()- DeltaG/100, 
			    myGuide->LastParameter() + DeltaG/100, 
			    DeltaG*1.e-7);
  }
  else {
    myTrimG = myGuide; 
  }
//  Standard_Real Step = DeltaG/100;
  DeltaG /= 3;
  for (ii=1; ii<=myNbPts; ii++) 
    {
      t = Standard_Real(myNbPts - ii)*f + Standard_Real(ii - 1)*l;
      t /= (myNbPts-1);
      myCurve->D0(t, P); 
      frenet->D0(t, Tangent, Normal, BiNormal);
      Plan = new (Geom_Plane) (P, Tangent);
      Pl = new(GeomAdaptor_HSurface) (Plan);

      Int.Perform(myTrimG, Pl); // intersection plan / guide 
      if (Int.NbPoints() == 0) {
#ifdef OCCT_DEBUG
	TracePlan(Plan);
#endif
        w = (fabs(myGuide->LastParameter() -w) > fabs(myGuide->FirstParameter()-w) ? myGuide->FirstParameter() : myGuide->LastParameter());
                                                                                   
        myStatus = GeomFill_PlaneNotIntersectGuide;
	//return;
      }
      else 
	{
	  gp_Pnt Pmin;
	  PInt = Int.Point(1);
	  Pmin = PInt.Pnt();
	  Standard_Real Dmin = P.Distance(Pmin);
	  for (Standard_Integer jj=2;jj<=Int.NbPoints();jj++)
	    {
	      Pmin = Int.Point(jj).Pnt();
	      if (P.Distance(Pmin) < Dmin) 
		{
		  PInt = Int.Point(jj);
		  Dmin = P.Distance(Pmin);
		}
	    }//for_jj
          
	  w = PInt.W();
        }
      if (ii>1) {
        Standard_Real Diff = w -  Pole->Value(1, ii-1).Y();
        if (Abs(Diff) > DeltaG) {
          if (myGuide->IsPeriodic()) {
            InGoodPeriod (Pole->Value(1, ii-1).Y(),
                          myGuide->Period(), w);
            
            Diff =  w -  Pole->Value(1, ii-1).Y();
          }
        }
        
#ifdef OCCT_DEBUG
        if (Abs(Diff) > DeltaG) {
          cout << "Trihedron Plan Diff on Guide : " << 
            Diff << endl;
        }
#endif
      }
      
      gp_Pnt2d p1(t, w); // on stocke les parametres
      Pole->SetValue(1, ii, p1);
      
    }// for_ii
}

//=======================================================================
//function : SetCurve
//purpose  : calculation of trihedron
//=======================================================================
void GeomFill_GuideTrihedronPlan::SetCurve(const Handle(Adaptor3d_HCurve)& C)
{
  myCurve = C;
  if (!myCurve.IsNull()) Init();
}

//=======================================================================
//function : Guide
//purpose  : calculation of trihedron
//=======================================================================

 Handle(Adaptor3d_HCurve) GeomFill_GuideTrihedronPlan::Guide()const
{
  return myGuide;
}

//=======================================================================
//function : D0
//purpose  : calculation of trihedron
//=======================================================================
 Standard_Boolean GeomFill_GuideTrihedronPlan::D0(const Standard_Real Param,
						  gp_Vec& Tangent,
						  gp_Vec& Normal,
						  gp_Vec& BiNormal) 
{ 
  gp_Pnt P, Pprime;
//  gp_Vec To;

  myCurve->D0(Param, P); 

  frenet->D0(Param,Tangent,Normal,BiNormal);
  
  //initialisation de la recherche
  InitX(Param);
      
  Standard_Integer Iter = 50;
  
  // fonction dont il faut trouver la racine : G(W)-Pl(U,V)=0
  GeomFill_PlanFunc E(P, Tangent, myGuide);
  
  // resolution
  math_FunctionRoot Result(E, X(1), XTol(1), 
			   Inf(1), Sup(1), Iter); 
  
  if (Result.IsDone()) 
    {
      Standard_Real Res = Result.Root();
//      R = Result.Root();    // solution    
   
      Pprime = myTrimG->Value(Res); // pt sur courbe guide 
      gp_Vec n (P, Pprime); // vecteur definissant la normale du triedre 
            
      Normal = n.Normalized();
      BiNormal = Tangent.Crossed(Normal);
      BiNormal.Normalized();   
    }
  else { // Erreur...
#ifdef OCCT_DEBUG
    cout << "D0 :";
    // plan ortho a la trajectoire pour determiner Pprime
    Handle(Geom_Plane) Plan = new (Geom_Plane)(P, Tangent);
    TracePlan(Plan);
#endif
    myStatus = GeomFill_PlaneNotIntersectGuide;
    return Standard_False;
  }
  
  return Standard_True;
}

//=======================================================================
//function : D1
//purpose  : calculation of trihedron and first derivative
//=======================================================================
 Standard_Boolean GeomFill_GuideTrihedronPlan::D1(const Standard_Real Param,
						  gp_Vec& Tangent,
						  gp_Vec& DTangent,
						  gp_Vec& Normal,
						  gp_Vec& DNormal,
						  gp_Vec& BiNormal,	   
						  gp_Vec& DBiNormal) 
{ 
//  return Standard_False;
  gp_Pnt P, PG;
  gp_Vec To,TG;


  
  // triedre de frenet sur la trajectoire
  myCurve->D1(Param, P, To);
  frenet->D1(Param,Tangent,DTangent,Normal,DNormal,BiNormal,DBiNormal);

 
  // tolerance sur E
  Standard_Integer Iter = 50;

  // fonction dont il faut trouver la racine : G(W)-Pl(U,V)=0
  InitX(Param);
  GeomFill_PlanFunc E(P, Tangent, myGuide);

  // resolution
  math_FunctionRoot Result(E, X(1), XTol(1), 
			   Inf(1), Sup(1), Iter);

  if (Result.IsDone()) 
    {
      Standard_Real Res =  Result.Root();
//      R = Result.Root();    // solution  
      myTrimG->D1(Res, PG, TG);
      gp_Vec n (P, PG), dn; // vecteur definissant la normale du triedre
      Standard_Real Norm = n.Magnitude();
      if (Norm < 1.e-12) {
	Norm = 1.0;
      }
      n /=Norm;

      
      Normal = n; 
      BiNormal = Tangent.Crossed(Normal);

// derivee premiere du triedre
      Standard_Real dedx, dedt, dtg_dt;
      E.Derivative(Res, dedx);
      E.DEDT(Res, To, DTangent, dedt);
      dtg_dt = -dedt/dedx;


/*      Standard_Real h=1.e-7, e, etg, etc;
      E.Value(Res, e);
      E.Value(Res+h, etg);
      if ( Abs( (etg-e)/h - dedx) > 1.e-4) {
	cout << "err :" <<  (etg-e)/h - dedx << endl;
      }
      gp_Pnt pdbg;
      gp_Vec td, nb, bnb;
      myCurve->D0(Param+h, pdbg);      
      frenet->D0(Param+h,td, nb, bnb);

      GeomFill_PlanFunc Edeb(pdbg, td, myGuide); 
      Edeb.Value(Res, etc);
      if ( Abs( (etc-e)/h - dedt) > 1.e-4) {
	cout << "err :" <<  (etc-e)/h - dedt << endl;
      } */           

      dn.SetLinearForm(dtg_dt, TG, -1, To);
      
      DNormal.SetLinearForm(-(n*dn), n, dn);
      DNormal /= Norm;
      DBiNormal.SetLinearForm(Tangent.Crossed(DNormal),
			      DTangent.Crossed(Normal));
    }
  else {// Erreur...
#ifdef OCCT_DEBUG
    cout << "D1 :";
    // plan ortho a la trajectoire
    Handle(Geom_Plane) Plan = new (Geom_Plane)(P, Tangent);
    TracePlan(Plan);
#endif
    myStatus = GeomFill_PlaneNotIntersectGuide;
    return Standard_False;
  }
 
  return Standard_True; 
}


//=======================================================================
//function : D2
//purpose  : calculation of trihedron and derivatives
//=======================================================================
 Standard_Boolean GeomFill_GuideTrihedronPlan::D2(const Standard_Real Param,
						  gp_Vec& Tangent,
						  gp_Vec& DTangent,
						  gp_Vec& D2Tangent,
						  gp_Vec& Normal,
						  gp_Vec& DNormal,
						  gp_Vec& D2Normal,
						  gp_Vec& BiNormal,			  
						  gp_Vec& DBiNormal,		  
						  gp_Vec& D2BiNormal) 
{ 
//  gp_Pnt P, PG;
  gp_Pnt P;
//  gp_Vec To,DTo,TG,DTG;
  gp_Vec To,DTo;

  myCurve->D2(Param, P, To, DTo); 

  // triedre de Frenet sur la trajectoire
  frenet->D2(Param,Tangent,DTangent,D2Tangent,
	     Normal,DNormal,D2Normal,
	     BiNormal,DBiNormal,D2BiNormal);

/*
  // plan ortho a Tangent pour trouver la pt Pprime sur le guide
  Handle(Geom_Plane) Plan = new (Geom_Plane)(P, Tangent); 
  Handle(GeomAdaptor_HSurface) Pl= new(GeomAdaptor_HSurface)(Plan);
  

  Standard_Integer Iter = 50;
  // fonction dont il faut trouver la racine : G(W) - Pl(U,V)=0
  GeomFill_FunctionPipe E(Pl , myGuide);
  InitX(Param);
  
  // resolution
  math_FunctionSetRoot Result(E, X, XTol, 
				    Inf, Sup, Iter); 
  if (Result.IsDone()) 
    {
      math_Vector R(1,3); 
      R = Result.Root();    // solution 
      myTrimG->D2(R(1), PG, TG, DTG); 

      gp_Vec n (P, PG); // vecteur definissant la normale du triedre
      Standard_Real Norm = n.Magnitude();
      n /= Norm;      
      Normal = n.Normalized(); 
      BiNormal = Tangent.Crossed(Normal);

 

   // derivee premiere du triedre 
      Standard_Real dtp_dt;
      dtp_dt = (To*Tangent - Norm*(n*DTangent))/(Tangent*TG);
      gp_Vec dn, d2n;
      dn.SetLinearForm(dtp_dt, TG, -1,  To);
      
      DNormal.SetLinearForm(-(n*dn), n, dn);
      DNormal /= Norm;  
      DBiNormal = Tangent.Crossed(DNormal) + DTangent.Crossed(Normal);
  
    // derivee seconde du triedre
      Standard_Real d2tp_dt2;
      d2tp_dt2 = (DTo*Tangent+To*DTangent - dn*DTangent-Norm*n*D2Tangent)/(TG*Tangent)
	- (To*Tangent-Norm*n*DTangent) * (DTG*dtp_dt*Tangent+TG*DTangent)
	  / ((TG*Tangent)*(TG*Tangent));


      d2n.SetLinearForm(dtp_dt*dtp_dt, DTG, d2tp_dt2, TG, -DTo);
      dn/=Norm;
      d2n/=Norm;

      D2Normal.SetLinearForm(3*Pow(n*dn,2)- (dn.SquareMagnitude() + n*d2n), n,
			     -2*(n*dn), dn,
			     d2n);
 
      D2BiNormal.SetLinearForm(1, D2Tangent.Crossed(Normal),
			       2, DTangent.Crossed(DNormal), 
			       Tangent.Crossed(D2Normal));
    }
  else {// Erreur...
#ifdef OCCT_DEBUG
    cout << "D2 :";
    TracePlan(Plan);
#endif
    myStatus = GeomFill_PlaneNotIntersectGuide;
    return Standard_False;
   }
*/
//  return Standard_True;
 return Standard_False;
}


//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================
 Handle(GeomFill_TrihedronLaw) GeomFill_GuideTrihedronPlan::Copy() const
{
 Handle(GeomFill_GuideTrihedronPlan) copy = 
   new (GeomFill_GuideTrihedronPlan) (myGuide);
 copy->SetCurve(myCurve);
 return copy;
}

//=======================================================================
//function : ErrorStatus
//purpose  : 
//=======================================================================
 GeomFill_PipeError GeomFill_GuideTrihedronPlan::ErrorStatus() const
{
  return myStatus;
}
 

//=======================================================================
//function : NbIntervals
//purpose  : Version provisoire : Il faut tenir compte du guide
//=======================================================================
 Standard_Integer GeomFill_GuideTrihedronPlan::NbIntervals(const GeomAbs_Shape S)const
{
  Standard_Integer Nb;
  GeomAbs_Shape tmpS;
  switch (S) {
  case GeomAbs_C0: tmpS = GeomAbs_C1; break;
  case GeomAbs_C1: tmpS = GeomAbs_C2; break;
  case GeomAbs_C2: tmpS = GeomAbs_C3; break;
  default: tmpS = GeomAbs_CN;
  }  

  Nb = myCurve->NbIntervals(tmpS);
  return Nb;
}
//======================================================================
//function :Intervals
//purpose  : 
//=======================================================================
 void GeomFill_GuideTrihedronPlan::Intervals(TColStd_Array1OfReal& TT,
					    const GeomAbs_Shape S) const
{
  GeomAbs_Shape tmpS;
  switch (S) {
  case GeomAbs_C0: tmpS = GeomAbs_C1; break;
  case GeomAbs_C1: tmpS = GeomAbs_C2; break;
  case GeomAbs_C2: tmpS = GeomAbs_C3; break;
  default: tmpS = GeomAbs_CN;
  }
  myCurve->Intervals(TT, tmpS);
}

//======================================================================
//function :SetInterval
//purpose  : 
//=======================================================================
void GeomFill_GuideTrihedronPlan::SetInterval(const Standard_Real First,
					      const Standard_Real Last) 
{
  myTrimmed = myCurve->Trim(First, Last, Precision::Confusion());  
}


//=======================================================================
//function : GetAverageLaw
//purpose  : 
//=======================================================================
 void GeomFill_GuideTrihedronPlan::GetAverageLaw(gp_Vec& ATangent,
				    gp_Vec& ANormal,
				    gp_Vec& ABiNormal) 
{
  Standard_Integer ii;
  Standard_Real t, Delta = (myCurve->LastParameter() - 
			    myCurve->FirstParameter())/20.001;

  ATangent.SetCoord(0.,0.,0.);
  ANormal.SetCoord(0.,0.,0.);
  ABiNormal.SetCoord(0.,0.,0.);
  gp_Vec T, N, B;
  
  for (ii=1; ii<=20; ii++) {
    t = myCurve->FirstParameter() +(ii-1)*Delta;
    D0(t, T, N, B);
    ATangent +=T;
    ANormal  +=N;
    ABiNormal+=B;
  }
  ATangent  /= 20;
  ANormal   /= 20;
  ABiNormal /= 20; 
}

//=======================================================================
//function : IsConstant
//purpose  : 
//=======================================================================
 Standard_Boolean GeomFill_GuideTrihedronPlan::IsConstant() const
{
  if ((myCurve->GetType() == GeomAbs_Line) &&
      (myGuide->GetType() == GeomAbs_Line)) {
    Standard_Real Angle;
    Angle = myCurve->Line().Angle(myGuide->Line());
    if ((Angle<1.e-12) || ((2*M_PI-Angle)<1.e-12) )
      return Standard_True;
  }

  return Standard_False;
}

//=======================================================================
//function : IsOnlyBy3dCurve
//purpose  : 
//=======================================================================
 Standard_Boolean GeomFill_GuideTrihedronPlan::IsOnlyBy3dCurve() const
{
  return Standard_False;
}

//=======================================================================
//function : Origine
//purpose  : Nothing!!
//=======================================================================
void  GeomFill_GuideTrihedronPlan::Origine(const Standard_Real ,
					   const Standard_Real  )
{
}

//==================================================================
//Function : InitX
//Purpose : recherche par interpolation d'une valeur initiale
//==================================================================
void GeomFill_GuideTrihedronPlan::InitX(const Standard_Real Param)
{

  Standard_Integer Ideb = 1, Ifin =  Pole->RowLength(), Idemi;
  Standard_Real Valeur, t1, t2;

  
  Valeur = Pole->Value(1, Ideb).X();
  if (Param == Valeur) {
    Ifin = Ideb+1; 
  }
  
  Valeur =  Pole->Value(1, Ifin).X();
  if (Param == Valeur) {
    Ideb = Ifin-1; 
  } 
  
  while ( Ideb+1 != Ifin) {
    Idemi = (Ideb+Ifin)/2;
    Valeur = Pole->Value(1, Idemi).X();
    if (Valeur < Param) {
      Ideb = Idemi;
    }
    else { 
      if ( Valeur > Param) { Ifin = Idemi;}
      else { 
	Ideb = Idemi;		     
	Ifin = Ideb+1;
      }
    }
  }

  t1 = Pole->Value(1,Ideb).X();
  t2 = Pole->Value(1,Ifin).X();
  Standard_Real diff = t2-t1;
  if (diff > 1.e-7) {
    Standard_Real b = (Param-t1) / diff,
    a = (t2-Param) / diff;

    X(1) = Pole->Value(1,Ideb).Coord(2) * a 
      + Pole->Value(1,Ifin).Coord(2) * b; //param guide 
  }
  else {
    X(1) = (Pole->Value(1, Ideb).Coord(2) + 
	    Pole->Value(1, Ifin).Coord(2)) / 2;
  }
  if (myGuide->IsPeriodic()) {
    X(1) = ElCLib::InPeriod(X(1), myGuide->FirstParameter(), 
			          myGuide->LastParameter());
  }
}
