// Created on: 1998-07-08
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


#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_Surface.hxx>
#include <ElCLib.hxx>
#include <Extrema_ExtCS.hxx>
#include <Extrema_POnSurf.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomFill_FunctionGuide.hxx>
#include <GeomFill_LocationGuide.hxx>
#include <GeomFill_LocationLaw.hxx>
#include <GeomFill_SectionLaw.hxx>
#include <GeomFill_SectionPlacement.hxx>
#include <GeomFill_TrihedronWithGuide.hxx>
#include <GeomFill_UniformSection.hxx>
#include <GeomLib.hxx>
#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_GTrsf.hxx>
#include <gp_Mat.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <IntCurveSurface_IntersectionPoint.hxx>
#include <math_FunctionSetRoot.hxx>
#include <math_Gauss.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomFill_LocationGuide,GeomFill_LocationLaw)

#if DRAW
static Standard_Integer Affich = 0;
#include <Approx_Curve3d.hxx>
#include <DrawTrSurf.hxx>
#include <GeomFill_TrihedronWithGuide.hxx>
#endif

//=======================================================================
//function : TraceRevol
//purpose  : Trace la surface de revolution (Debug)
//=======================================================================
#ifdef OCCT_DEBUG
static void TraceRevol(const Standard_Real t,
                       const Standard_Real s,
		       const Handle(GeomFill_TrihedronWithGuide)& Law,
		       const Handle(GeomFill_SectionLaw)& Section,
		       const Handle(Adaptor3d_HCurve)& Curve,
		       const gp_Mat& Trans)
		       
{
  gp_Vec T, N, B;
  gp_Pnt P;
  gp_Ax3 Rep(gp::Origin(), gp::DZ(), gp::DX());

  Curve->D0(t, P);
  Law->D0(t, T, N, B);

  gp_Mat M(N.XYZ(), B.XYZ(), T.XYZ());
  M *= Trans;

  gp_Dir D = M.Column(3);
  gp_Ax1 Ax(P,D); // axe pour la surface de revoltuion
      
  // calculer transfo entre triedre et Oxyz
  gp_Dir N2 = N;
  gp_Ax3 N3(P,D,N2);
  gp_Trsf Transfo;
  Transfo.SetTransformation(N3, Rep);
  
  // transformer la section
  Standard_Real f, l,e=1.e-7;
  Handle (Geom_Curve) S, C;

  if (Section->IsConstant(e)) {
    C = Section->ConstantSection();
  }
  else {
    Standard_Integer NbPoles, NbKnots, Deg;
    Section->SectionShape(NbPoles, NbKnots, Deg);
    TColStd_Array1OfInteger Mult(1,NbKnots);
    Section->Mults( Mult);
    TColStd_Array1OfReal Knots(1,NbKnots);
    Section->Knots(Knots);
    TColgp_Array1OfPnt Poles(1, NbPoles);
    TColStd_Array1OfReal Weights(1,  NbPoles);
    Section->D0(s, Poles, Weights);
    if (Section->IsRational()) 
      C = new (Geom_BSplineCurve)
	(Poles, Weights, Knots, Mult ,
	 Deg, Section->IsUPeriodic());
   else 
     C = new (Geom_BSplineCurve)
	(Poles, Knots, Mult,
	 Deg,  Section->IsUPeriodic());
    
  }

  f = C->FirstParameter();
  l = C->LastParameter();
  S = new (Geom_TrimmedCurve) (C, f, l);
  S->Transform(Transfo);
  
  // Surface de revolution
  Handle (Geom_Surface) Revol = new(Geom_SurfaceOfRevolution) (S, Ax);
  cout << "Surf Revol at parameter t = " << t << endl;

#if DRAW
  Standard_CString aName = "TheRevol" ;
  DrawTrSurf::Set(aName,Revol);
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

//==================================================================
//Function: GeomFill_LocationGuide
//Purpose : constructor
//==================================================================
 GeomFill_LocationGuide::
 GeomFill_LocationGuide (const Handle(GeomFill_TrihedronWithGuide)& Triedre)
                        : TolRes(1,3), Inf(1,3,0.), Sup(1,3,0.), 
			  X(1,3), R(1,3), myStatus(GeomFill_PipeOk)
{
  TolRes.Init(1.e-6);
  myLaw = Triedre; // loi de triedre
  mySec.Nullify(); // loi de section
  myCurve.Nullify();
  myFirstS = myLastS = -505e77;

  myNbPts = 21;  // nb points pour les calculs
  myGuide = myLaw->Guide();  // courbe guide
  if (!myGuide->IsPeriodic()) {
    Standard_Real f, l, delta;
    f = myGuide->FirstParameter();
    l = myGuide->LastParameter();
    delta = (l-f)/100;
    f-=delta;
    l+=delta;
    myGuide = myGuide->Trim(f,l,delta*1.e-7); // courbe guide
  }// if
 
  myPoles2d = new (TColgp_HArray2OfPnt2d)(1, 2, 1, myNbPts);
  rotation = Standard_False; // contact ou non
  OrigParam1 = 0; // param pour ACR quand trajectoire
  OrigParam2 = 1; // et guide pas meme sens de parcourt
  Trans.SetIdentity();
  WithTrans = Standard_False;

#if DRAW
  if (Affich) {
    Approx_Curve3d approx(myGuide, 1.e-4, 
			  GeomAbs_C1, 
			  15+myGuide->NbIntervals(GeomAbs_CN),
			  14);
    if (approx.HasResult()) {
      Standard_CString aName = "TheGuide" ;
      DrawTrSurf::Set(aName, approx.Curve());
    }
  }
#endif
}

//==================================================================
//Function: SetRotation
//Purpose : init et force la Rotation
//==================================================================
 void GeomFill_LocationGuide::SetRotation(const Standard_Real PrecAngle,
					   Standard_Real& LastAngle)
{
  if (myCurve.IsNull())
    throw Standard_ConstructionError(
          "GeomFill_LocationGuide::The path is not setted !!");

    //repere fixe
  gp_Ax3 Rep(gp::Origin(), gp::DZ(), gp::DX());
//  gp_Pnt P,P1,P2;
  gp_Pnt P;
  gp_Vec T,N,B;
  Standard_Integer ii, Deg;
  Standard_Boolean isconst, israt=Standard_False;
  Standard_Real t, v,w, OldAngle=0, Angle, DeltaG, Diff;
  Standard_Real CurAngle =  PrecAngle, a1/*, a2*/;
  gp_Pnt2d p1,p2;
  Handle(Geom_SurfaceOfRevolution) Revol; // surface de revolution
  Handle(GeomAdaptor_HSurface) Pl; // = Revol
  Handle(Geom_TrimmedCurve) S;
  IntCurveSurface_IntersectionPoint PInt; // intersection guide/Revol
  Handle(TColStd_HArray1OfInteger) Mult;
  Handle(TColStd_HArray1OfReal) Knots, Weights;
  Handle(TColgp_HArray1OfPnt)  Poles;
  

  Standard_Real U=0, UPeriod=0;  
  Standard_Real f = myCurve->FirstParameter();
  Standard_Real l = myCurve->LastParameter();
  Standard_Boolean Ok, uperiodic =  mySec->IsUPeriodic();

  DeltaG = (myGuide->LastParameter() - myGuide->FirstParameter())/5;
  Handle(Geom_Curve) mySection;
  Standard_Real Tol =1.e-9;

  Standard_Integer NbPoles, NbKnots;
  mySec->SectionShape(NbPoles, NbKnots, Deg);


  if (mySec->IsConstant(Tol)) {
    mySection = mySec->ConstantSection();
    Uf = mySection->FirstParameter();
    Ul = mySection->LastParameter();

    isconst = Standard_True;
  }
  else {
    isconst = Standard_False;
    israt =  mySec->IsRational();
    Mult = new (TColStd_HArray1OfInteger) (1,  NbKnots);
    mySec->Mults( Mult->ChangeArray1());
    Knots = new (TColStd_HArray1OfReal) (1,  NbKnots); 
    mySec->Knots(Knots->ChangeArray1());
    Poles = new (TColgp_HArray1OfPnt) (1,  NbPoles);
    Weights = new (TColStd_HArray1OfReal) (1,  NbPoles);
    Uf = Knots->Value(1);
    Ul = Knots->Value(NbKnots);
  }

   // Bornes de calculs
  Standard_Real Delta;
//  Standard_Integer bid1, bid2, NbK; 
  Delta =  myGuide->LastParameter() - myGuide->FirstParameter();
  Inf(1) =  myGuide->FirstParameter() - Delta/10;
  Sup(1) =  myGuide->LastParameter() + Delta/10; 

  Inf(2) = -M_PI;
  Sup(2) = 3*M_PI;
 
  Delta =  Ul - Uf;
  Inf(3) = Uf - Delta/10;
  Sup(3) = Ul + Delta/10;

  // JALONNEMENT
  if (uperiodic) UPeriod = Ul-Uf;

  for (ii=1; ii<=myNbPts; ii++) {
    t = Standard_Real(myNbPts - ii)*f + Standard_Real(ii - 1)*l;
    t /= (myNbPts-1); 
    myCurve->D0(t, P);
    Ok = myLaw->D0(t, T, N, B);
    if (!Ok) {
      myStatus = myLaw->ErrorStatus();
      return; //Y a rien a faire.
    }
    gp_Dir D = T;
    if (WithTrans) {
      gp_Mat M(N.XYZ(), B.XYZ(), T.XYZ());
      M *= Trans;
      D =  M.Column(3);
    }
    gp_Ax1 Ax(P,D); // axe pour la surface de revoltuion
    
    // calculer transfo entre triedre et Oxyz
    gp_Dir N2 = N;
    gp_Ax3 N3(P,D,N2);
    gp_Trsf Transfo;
    Transfo.SetTransformation(N3, Rep);
      
    // transformer la section
    if (! isconst) {
      U = myFirstS + (t-myCurve->FirstParameter())*ratio;
      mySec->D0(U, Poles->ChangeArray1(), Weights->ChangeArray1());
      if (israt)
	mySection = new (Geom_BSplineCurve) 
	  (Poles->Array1(),
	   Weights->Array1(),
	   Knots->Array1(),
	   Mult->Array1(),
	   Deg, mySec->IsUPeriodic());
      else 
	mySection = new (Geom_BSplineCurve) 
	  (Poles->Array1(),
	   Knots->Array1(),
	   Mult->Array1(),
	   Deg, mySec->IsUPeriodic());
      S = new (Geom_TrimmedCurve) (mySection, Uf, Ul);
    }
    else {
      S = new (Geom_TrimmedCurve) 
	(Handle(Geom_Curve)::DownCast(mySection->Copy()), Uf, Ul);
    }
    S->Transform(Transfo);

    // Surface de revolution
    Revol = new(Geom_SurfaceOfRevolution) (S, Ax); 
    
    GeomAdaptor_Surface GArevol(Revol);
    Extrema_ExtCS DistMini(myGuide->Curve(), GArevol,
                           Precision::Confusion(), Precision::Confusion());
    Extrema_POnCurv Pc;
    Extrema_POnSurf Ps;
    Standard_Real theU = 0., theV = 0.;
    
    if (!DistMini.IsDone() || DistMini.NbExt() == 0) {
#ifdef OCCT_DEBUG
      cout <<"LocationGuide : Pas d'intersection"<<endl;
      TraceRevol(t, U, myLaw, mySec, myCurve, Trans);
#endif 
      Standard_Boolean SOS=Standard_False;
      if (ii>1) {
        // Intersection de secour entre surf revol et guide
        // equation 
        X(1) = myPoles2d->Value(1,ii-1).Y();
        X(2) = myPoles2d->Value(2,ii-1).X();
        X(3) = myPoles2d->Value(2,ii-1).Y();
        GeomFill_FunctionGuide E (mySec, myGuide, U);
        E.SetParam(U, P, T.XYZ(), N.XYZ()); 
        // resolution   =>  angle
        math_FunctionSetRoot Result(E, TolRes);
        Result.Perform(E, X, Inf, Sup);

        if (Result.IsDone() && 
          (Result.FunctionSetErrors().Norm() < TolRes(1)*TolRes(1)) ) {
#ifdef OCCT_DEBUG
            cout << "Ratrappage Reussi !" << endl;
#endif
            SOS = Standard_True;
            math_Vector RR(1,3);
            Result.Root(RR);
            PInt.SetValues(P, RR(2), RR(3), RR(1), IntCurveSurface_Out);
            theU = PInt.U();
            theV = PInt.V();
        }
        else {
#ifdef OCCT_DEBUG
          cout << "Echec du Ratrappage !" << endl;
#endif
        }
      }
      if (!SOS) {
	myStatus = GeomFill_ImpossibleContact;
	return;
      }
    } 
    else { // on prend le point d'intersection 
      // d'angle le plus proche de P
      
      Standard_Real MinDist = RealLast();
      Standard_Integer jref = 0;
      for (Standard_Integer j = 1; j <= DistMini.NbExt(); j++)
      {
        Standard_Real aDist = DistMini.SquareDistance(j);
        if (aDist < MinDist)
        {
          MinDist = aDist;
          jref = j;
        }
      }
      MinDist = Sqrt(MinDist);
      DistMini.Points(jref, Pc, Ps);
      
      Ps.Parameter(theU, theV);
      a1 = theU;
      
      InGoodPeriod (CurAngle, 2*M_PI, a1);
    }//else
    
    // Controle de w 
    w = Pc.Parameter();
    
    if (ii>1) {
      Diff = w -  myPoles2d->Value(1, ii-1).Y();
      if (Abs(Diff) > DeltaG) {
	if (myGuide->IsPeriodic()) {
	  InGoodPeriod (myPoles2d->Value(1, ii-1).Y(),
			myGuide->Period(), w);
	  Diff =  w - myPoles2d->Value(1, ii-1).Y();
	}
      }
      
#ifdef OCCT_DEBUG
      if (Abs(Diff) > DeltaG) {
	cout << "Location :: Diff on Guide : " << 
	  Diff << endl;
      }
#endif
    }
    //Recadrage de l'angle.
    Angle = theU;
    
    if (ii > 1) {
      Diff = Angle - OldAngle;
	if (Abs(Diff) > M_PI) {
	  InGoodPeriod (OldAngle, 2*M_PI, Angle);
	  Diff = Angle - OldAngle;
	}
#ifdef OCCT_DEBUG
      if (Abs(Diff) > M_PI/4) {
	cout << "Diff d'angle trop grand !!" << endl;
      } 
#endif
    }


    //Recadrage du V
    v = theV;
    
    if (ii > 1) {
      if (uperiodic) {
	InGoodPeriod (myPoles2d->Value(2, ii-1).Y(), UPeriod, v);
      }
      Diff = v -  myPoles2d->Value(2, ii-1).Y();
#ifdef OCCT_DEBUG
      if (Abs(Diff) > (Ul-Uf)/(2+NbKnots)) {
	cout << "Diff sur section trop grand !!" << endl;
      } 
#endif
    }
    
    p1.SetCoord(t, w); // on stocke les parametres
    p2.SetCoord(Angle , v);
    CurAngle = Angle;
    myPoles2d->SetValue(1, ii, p1);
    myPoles2d->SetValue(2, ii, p2);
    OldAngle = Angle;
  }

  LastAngle = CurAngle;
  rotation = Standard_True; //C'est pret !
}


//==================================================================
//Function: Set
//Purpose : init loi de section et force la Rotation
//==================================================================
 void GeomFill_LocationGuide::Set(const Handle(GeomFill_SectionLaw)& Section,
				  const Standard_Boolean rotat,
				  const Standard_Real SFirst,
				  const Standard_Real SLast,
				  const Standard_Real PrecAngle,
				  Standard_Real& LastAngle)
{
  myStatus = GeomFill_PipeOk;
  myFirstS = SFirst;
  myLastS  = SLast;
  LastAngle = PrecAngle;
  if (myCurve.IsNull()) 
    ratio = 0.;
  else 
    ratio = (SLast-SFirst) / (myCurve->LastParameter() - 
			      myCurve->FirstParameter());
  mySec = Section; 
  
  if (rotat) SetRotation(PrecAngle, LastAngle);
  else rotation = Standard_False;
}

//==================================================================
//Function: EraseRotation
//Purpose : Supprime la Rotation
//==================================================================
 void GeomFill_LocationGuide:: EraseRotation()
{
  rotation = Standard_False;
  if  (myStatus == GeomFill_ImpossibleContact) myStatus = GeomFill_PipeOk;
}

//==================================================================
//Function: Copy
//Purpose :
//==================================================================
 Handle(GeomFill_LocationLaw) GeomFill_LocationGuide::Copy() const
{  
  Standard_Real la;
  Handle(GeomFill_TrihedronWithGuide) L;
  L = Handle(GeomFill_TrihedronWithGuide)::DownCast(myLaw->Copy());
  Handle(GeomFill_LocationGuide) copy = new 
    (GeomFill_LocationGuide) (L);
  copy->SetOrigine(OrigParam1, OrigParam2);
  copy->Set(mySec, rotation, myFirstS, myLastS,
	    myPoles2d->Value(1,1).X(), la);
  copy->SetTrsf(Trans);

  return copy;
} 


//==================================================================
//Function: SetCurve
//Purpose : Calcul des poles sur la surface d'arret (intersection 
// courbe guide / surface de revolution en myNbPts points)
//==================================================================
 void GeomFill_LocationGuide::SetCurve(const Handle(Adaptor3d_HCurve)& C) 
{
  Standard_Real LastAngle;
  myCurve = C;
  myTrimmed = C;

  if (!myCurve.IsNull()){
    myLaw->SetCurve(C); 
    myLaw->Origine(OrigParam1, OrigParam2);
    myStatus =  myLaw->ErrorStatus();
    
    if (rotation) SetRotation(myPoles2d->Value(1,1).X(), LastAngle);
  }
}

//==================================================================
//Function: GetCurve
//Purpose : return the trajectoire
//==================================================================
 const Handle(Adaptor3d_HCurve)& GeomFill_LocationGuide::GetCurve() const
{
  return myCurve;
}

//==================================================================
//Function: SetTrsf
//Purpose :
//==================================================================
 void GeomFill_LocationGuide::SetTrsf(const gp_Mat& Transfo) 
{
  Trans = Transfo;
  gp_Mat Aux;
  Aux.SetIdentity();
  Aux -= Trans;
  WithTrans = Standard_False; // Au cas ou Trans = I
  for (Standard_Integer ii=1; ii<=3 && !WithTrans ; ii++)
    for (Standard_Integer jj=1; jj<=3 && !WithTrans; jj++)
      if (Abs(Aux.Value(ii, jj)) > 1.e-14)  WithTrans = Standard_True;
}

//==================================================================
//Function: D0
//Purpose : 
//==================================================================
 Standard_Boolean GeomFill_LocationGuide::D0(const Standard_Real Param, 
					     gp_Mat& M,
					     gp_Vec& V)
{
  Standard_Boolean Ok;
  gp_Vec T,N,B;
  gp_Pnt P;

  myCurve->D0(Param, P);
  V.SetXYZ(P.XYZ()); 
  Ok = myLaw->D0(Param, T, N, B); 
  if (!Ok) {
    myStatus = myLaw->ErrorStatus();
    return Ok;
  }
  M.SetCols(N.XYZ(), B.XYZ(), T.XYZ());

  if (WithTrans) {
    M *= Trans;
  }
  
  if(rotation) {
    Standard_Real U = myFirstS + 
      (Param-myCurve->FirstParameter())*ratio;
    // initialisations germe
    InitX(Param); 
    
    Standard_Integer Iter = 100;
    gp_XYZ t,b,n;
    t = M.Column(3);
    b = M.Column(2);
    n = M.Column(1);
    
    // Intersection entre surf revol et guide
    // equation 
    GeomFill_FunctionGuide E (mySec, myGuide, U);
    E.SetParam(Param, P, t, n); 
    // resolution   =>  angle
    math_FunctionSetRoot Result(E, TolRes, Iter);
    Result.Perform(E, X, Inf, Sup);

    if (Result.IsDone()) {
      // solution
      Result.Root(R); 
      
      // rotation 
      gp_Mat Rot;
      Rot.SetRotation(t, R(2)); 	
      b *= Rot;
      n *= Rot;
      
      M.SetCols(n, b, t);
    }
    else {
#ifdef OCCT_DEBUG
      cout << "LocationGuide::D0 : No Result !"<<endl;
      TraceRevol(Param, U, myLaw, mySec, myCurve, Trans);
#endif
	myStatus = GeomFill_ImpossibleContact;
      return Standard_False;
    }
  }

  return Standard_True;
} 

//==================================================================
//Function: D0
//Purpose : calcul de l'intersection (C0) surface revol / guide
//================================================================== 
 Standard_Boolean GeomFill_LocationGuide::D0(const Standard_Real Param, 
					     gp_Mat& M,
					     gp_Vec& V,
//					     TColgp_Array1OfPnt2d& Poles2d)
					     TColgp_Array1OfPnt2d& )
{ 
  gp_Vec T, N, B;
  gp_Pnt P;
  Standard_Boolean Ok;

  myCurve->D0(Param, P);
  V.SetXYZ(P.XYZ());
  Ok = myLaw->D0(Param, T, N, B);  
  if (!Ok) {
    myStatus = myLaw->ErrorStatus();
    return Ok;
  }
  M.SetCols(N.XYZ(), B.XYZ(), T.XYZ());

  if (WithTrans) {
    M *= Trans;
  }
  
  if (rotation) {
    //initialisation du germe
    InitX(Param);    
    Standard_Integer Iter = 100;
    gp_XYZ b, n, t;
    t = M.Column(3);
    b = M.Column(2);
    n = M.Column(1);

    // equation d'intersection entre surf revol et guide => angle
    GeomFill_FunctionGuide E (mySec, myGuide, myFirstS + 
				(Param-myCurve->FirstParameter())*ratio);
    E.SetParam(Param, P, t, n);

    // resolution
    math_FunctionSetRoot Result(E, TolRes, Iter);
    Result.Perform(E, X, Inf, Sup);

    if (Result.IsDone()) {
      // solution 
      Result.Root(R);   
      
      // rotation 
      gp_Mat Rot;
      Rot.SetRotation(t, R(2)); 
      
      
      b *= Rot;
      n *= Rot;
      
      M.SetCols(n, b, t);
    }
    else {
#ifdef OCCT_DEBUG
      Standard_Real U = myFirstS + ratio*(Param-myCurve->FirstParameter());
      cout << "LocationGuide::D0 : No Result !"<<endl;
      TraceRevol(Param, U, myLaw, mySec, myCurve, Trans);
#endif
      myStatus = GeomFill_ImpossibleContact;
      return Standard_False;
    }    
  }
  
  return Standard_True;
}


//==================================================================
//Function: D1
//Purpose : calcul de l'intersection (C1) surface revol / guide
//================================================================== 
 Standard_Boolean GeomFill_LocationGuide::D1(const Standard_Real Param,
					     gp_Mat& M,
					     gp_Vec& V,
					     gp_Mat& DM,
					     gp_Vec& DV,
//					     TColgp_Array1OfPnt2d& Poles2d,
					     TColgp_Array1OfPnt2d& ,
//					     TColgp_Array1OfVec2d& DPoles2d) 
					     TColgp_Array1OfVec2d& ) 
{
//  gp_Vec T, N, B, DT, DN, DB, T0, N0, B0;
  gp_Vec T, N, B, DT, DN, DB;
//  gp_Pnt P, P0;
  gp_Pnt P;
  Standard_Boolean Ok;

  myCurve->D1(Param, P, DV);
  V.SetXYZ(P.XYZ());
  Ok = myLaw->D1(Param, T, DT, N, DN, B, DB);
  if (!Ok) {
    myStatus = myLaw->ErrorStatus();
    return Ok;
  }
  M.SetCols(N.XYZ(), B.XYZ(), T.XYZ());
  DM.SetCols(DN.XYZ() , DB.XYZ(), DT.XYZ());

  if (WithTrans) {
    M *= Trans;
    DM *= Trans;
  }

  if (rotation) {  
    return Standard_False;
 /*   
#ifdef OCCT_DEBUG
    Standard_Real U = myFirstS + ratio*(Param-myCurve->FirstParameter());
#else
    myCurve->FirstParameter() ;
#endif
      
    // initialisation du germe 
    InitX(Param);      
    
    Standard_Integer Iter = 100;
    gp_XYZ t,b,n, dt, db, dn;
    t = M.Column(3);
    b = M.Column(2);
    n = M.Column(1);
    dt = M.Column(3);
    db = M.Column(2);
    dn = M.Column(1); 
     
    // equation d'intersection surf revol / guide => angle
    GeomFill_FunctionGuide E (mySec, myGuide, myFirstS + 
			      (Param-myCurve->FirstParameter())*ratio);
    E.SetParam(Param, P, t, n);
    
    // resolution
    math_FunctionSetRoot Result(E, X, TolRes, 
				Inf, Sup, Iter); 
    
    if (Result.IsDone()) 
      {
	// solution de la fonction
	Result.Root(R);   
	
	// derivee de la fonction 
	math_Vector DEDT(1,3);  
	  E.DerivT(R, DV.XYZ(), dt, DEDT); // dE/dt => DEDT
	  
	  math_Vector DSDT (1,3,0);
	  math_Matrix DEDX (1,3,1,3,0);
	  E.Derivatives(R, DEDX);  // dE/dx au point R => DEDX
	  
	  // resolution du syst. : DEDX*DSDT = -DEDT
	  math_Gauss Ga(DEDX);
	  if (Ga.IsDone()) 
	    {
	      Ga.Solve (DEDT.Opposite(), DSDT);// resolution du syst. 
	    }//if
	  else {
#ifdef OCCT_DEBUG
	    cout << "DEDX = " << DEDX << endl;
	    cout << "DEDT = " << DEDT << endl;
#endif
	    throw Standard_ConstructionError(
	     "LocationGuide::D1 : No Result dans la derivee");
	  }
	  
	  // transformation = rotation
	  gp_Mat Rot, DRot;
	  Rot.SetRotation(t, R(2));  
	 	 
	
	 
	  M.SetCols(n*Rot, b*Rot, t);
	  
	  // transfo entre triedre (en Q) et Oxyz
	  gp_Ax3 Rep(gp::Origin(),gp::DZ(), gp::DX());
	  gp_Ax3 RepTriedre(gp::Origin(),t,n);
	  gp_Trsf Transfo3;
	  Transfo3.SetTransformation(Rep,RepTriedre);
	  // on  se place dans Oxyz
	  Transfo3.Transforms(n);
	  Transfo3.Transforms(b);	   
	  Transfo3.Transforms(dn);
	  Transfo3.Transforms(db);

	  // matrices de rotation et derivees
	  Standard_Real A = R(2);
	  Standard_Real Aprim = DSDT(2);  

#ifdef OCCT_DEBUG	  
	  gp_Mat M2 (Cos(A), -Sin(A),0,  // rotation autour de T
		     Sin(A), Cos(A),0,
		     0,0,1);	  
#endif
	 	
	  gp_Mat M2prim (-Sin(A), -Cos(A), 0, // derivee rotation autour de T
			 Cos(A), -Sin(A), 0,
			 0, 0, 0);	
	  M2prim.Multiply(Aprim);
	 
	  // transformations	 


	  dn *= Rot;
	  db *= Rot;
	  
	  n *= DRot;
	  b *= DRot;
	  
	  dn += n;
	  db += b;

	  // on repasse dans repere triedre
          gp_Trsf InvTrsf;
	  InvTrsf = Transfo3.Inverted();
	  InvTrsf.Transforms(dn);
	  InvTrsf.Transforms(db);
	
	  DM.SetCols(dn , db , dt);	  
	}//if_Result

      else {
#ifdef OCCT_DEBUG
	cout << "LocationGuide::D1 : No Result !!"<<endl;
	TraceRevol(Param, U, myLaw, mySec, myCurve, Trans);
#endif
	myStatus = GeomFill_ImpossibleContact;
	return Standard_False;
      }
*/
    }//if_rotation
  

  return Standard_True;
  
}

//==================================================================
//Function: D2
//Purpose : calcul de l'intersection (C2) surface revol / guide
//==================================================================
Standard_Boolean GeomFill_LocationGuide::D2(const Standard_Real Param,
					     gp_Mat& M,
					     gp_Vec& V,
					     gp_Mat& DM,
					     gp_Vec& DV, 
					     gp_Mat& D2M,
					     gp_Vec& D2V,
//					     TColgp_Array1OfPnt2d& Poles2d,
					     TColgp_Array1OfPnt2d& ,
//					     TColgp_Array1OfVec2d& DPoles2d,
					     TColgp_Array1OfVec2d& ,
//					     TColgp_Array1OfVec2d& D2Poles2d) 
					     TColgp_Array1OfVec2d& ) 
{
  gp_Vec T, N, B, DT, DN, DB, D2T, D2N, D2B;
//  gp_Vec T0, N0, B0, T1, N1, B1;
//  gp_Pnt P, P0, P1;
  gp_Pnt P;
  Standard_Boolean Ok;

  myCurve->D2(Param, P, DV, D2V);
  V.SetXYZ(P.XYZ());
  Ok = myLaw->D2(Param, T, DT, D2T, N, DN, D2N, B, DB, D2B);
  if (!Ok) {
    myStatus = myLaw->ErrorStatus();
    return Ok;
  }

  if (WithTrans) {
    M   *= Trans;
    DM  *= Trans;
    D2M *= Trans;
  }

  if (rotation) 
    {
      return Standard_False;
/*
    Standard_Real U = myFirstS + 
      (Param-myCurve->FirstParameter())*ratio;
      // rotation
      math_Vector X(1,3,0);
      InitX(Param,X);      
      // tolerance sur X

      TolRes.Init(1.e-6);
      // tolerance sur E
//      Standard_Real ETol = 1.e-6;
      Standard_Integer Iter = 100;
      
      
      // resoudre equation d'intersection entre surf revol et guide => angle
      GeomFill_FunctionGuide E (mySec, myGuide, myFirstS + 
				(Param-myCurve->FirstParameter())*ratio);
      E.SetParam(Param, P, T, N);
      
      // resolution
      math_FunctionSetRoot Result(E, X, TolRes, 
                                  Inf, Sup, Iter); 
      
      if (Result.IsDone()) 
	{
	  Result.Root(R);    // solution
	  
	  //gp_Pnt2d p (R(2), R(3));  // point sur la surface (angle, v)
	  //Poles2d.SetValue(1,p);
	  
	  // derivee de la fonction 
	  math_Vector DEDT(1,3,0);
	  E.DerivT(Param, Param0, R, R0, DEDT); // dE/dt => DEDT
	  math_Vector DSDT (1,3,0);
	  math_Matrix DEDX (1,3,1,3,0);
	  E.Derivatives(R, DEDX);  // dE/dx au point R => DEDX
	 
	  // resolution du syst. lin. : DEDX*DSDT = -DEDT
	  math_Gauss Ga(DEDX);
	  if (Ga.IsDone()) 
	    {
	      Ga.Solve (DEDT.Opposite(), DSDT); // resolution du syst. lin. 
	      //gp_Vec2d dp (DSDT(2), DSDT(3));    // surface
	      //DPoles2d.SetValue(1, dp);
	    }//if
	  else cout <<"LocationGuide::D2 : No Result dans la derivee premiere"<<endl;

	  // deuxieme derivee
	  GeomFill_Tensor D2EDX2(3,3,3);
	  E.Deriv2X(R, D2EDX2); // d2E/dx2
	  
	  math_Vector D2EDT2(1,3,0);
	  
	 // if(Param1 < Param && Param < Param0)
	    E.Deriv2T(Param1, Param, Param0, R1, R, R0, D2EDT2); // d2E/dt2
	 // else if (Param < Param0 && Param0 < Param1) 
	   // E.Deriv2T(Param, Param0, Param1, R, R0, R1, D2EDT2); // d2E/dt2
	 // else 
	   // E.Deriv2T(Param0, Param1, Param, R0, R1, R, D2EDT2); // d2E/dt2
	  
	  math_Matrix D2EDTDX(1,3,1,3,0);
	  E.DerivTX(Param, Param0, R, R0, D2EDTDX); // d2E/dtdx
	  
	  math_Vector D2SDT2(1,3,0); // d2s/dt2
	  math_Matrix M1(1,3,1,3,0);
	  D2EDX2.Multiply(DSDT,M1);
	  
	  // resolution du syst. lin. 
	  math_Gauss Ga1 (DEDX);
	  if (Ga1.IsDone()) 
	    {
	      Ga1.Solve ( - M1*DSDT - 2*D2EDTDX*DSDT - D2EDT2 , D2SDT2); 
	      //gp_Vec2d d2p (D2SDT2(2), D2SDT2(3));  // surface
	      //D2Poles2d.SetValue(1, d2p);
	    }//if
	  else {
           cout <<"LocationGuide::D2 : No Result dans la derivee seconde"<<endl;
	   myStatus = GeomFill_ImpossibleContact;
	   }

//------------------------------------------
// rotation
//------------------------------------------

	  gp_Trsf Tr;
	  gp_Pnt Q (0, 0 ,0);
	  gp_Ax1 Axe (Q, D);
	  Tr.SetRotation(Axe, R(2));
	
	  gp_Vec b,b2;
	  b = b2 = B;
	  gp_Vec n,n2;
	  n = n2 = N;
	  
	  B.Transform(Tr);
	  N.Transform(Tr);
	  
	  M.SetCols(N.XYZ(), B.XYZ(), T.XYZ());

//------------------------------------------  
// derivees de la rotation  	  
// A VERIFIER !!!!
//-----------------------------------------  
	  gp_Vec db,dn,db3,dn3;
	  db = db3 = DB;
	  dn = dn3 = DN;

	  gp_Vec db1,dn1,db2,dn2;

//transfo entre triedre et Oxyz
	  gp_Ax3 RepTriedre4(Q,D,B2);
	  gp_Trsf Transfo3;
	  Transfo3.SetTransformation(Rep,RepTriedre4);

//on passe dans le repere du triedre
	  n.Transform(Transfo3);
	  b.Transform(Transfo3);
	  n2.Transform(Transfo3);
	  b2.Transform(Transfo3);
	  dn.Transform(Transfo3);
	  db.Transform(Transfo3);  
	  dn3.Transform(Transfo3);
	  db3.Transform(Transfo3);  
	  D2N.Transform(Transfo3);
	  D2B.Transform(Transfo3); 
 
//matrices de rotation et derivees
	  Standard_Real A = R(2);
	  Standard_Real Aprim = DSDT(2);  
	  Standard_Real Asec = D2SDT2(2);  
	
	  gp_Mat M2 (Cos(A),-Sin(A),0,   // rotation autour de T
		     Sin(A), Cos(A),0,
		     0, 0, 1);	  
	 
	  gp_Mat M2prim (-Sin(A),-Cos(A),0,   // derivee 1ere rotation autour de T
			 Cos(A), -Sin(A),0,
			 0,0,0);	

	  gp_Mat M2sec (-Cos(A), Sin(A), 0,   // derivee 2nde rotation autour de T
			-Sin(A), -Cos(A), 0,
			0,0,0);	
	  M2sec.Multiply(Aprim*Aprim); 
	  gp_Mat M2p = M2prim.Multiplied(Asec);
	  M2sec.Add(M2p);

	  M2prim.Multiply(Aprim);

// transformation
	  gp_Trsf Rot;
	  Rot.SetValues(M2(1,1),M2(1,2),M2(1,3),0,
			M2(2,1),M2(2,2),M2(2,3),0,
			M2(3,1),M2(3,2),M2(3,3),0,
			1.e-8,1.e-8);
	  gp_Trsf DRot;
	  DRot.SetValues(M2prim(1,1),M2prim(1,2),M2prim(1,3),0,
			 M2prim(2,1),M2prim(2,2),M2prim(2,3),0,
			 M2prim(3,1),M2prim(3,2),M2prim(3,3),0,
			 1.e-8,1.e-8);

	  gp_Trsf D2Rot;
	  D2Rot.SetValues(M2sec(1,1),M2sec(1,2),M2sec(1,3),0,
			  M2sec(2,1),M2sec(2,2),M2sec(2,3),0,
			  M2sec(3,1),M2sec(3,2),M2sec(3,3),0,
			  1.e-8,1.e-8);
	  

//derivee premiere
	  dn.Transform(Rot);
	  db.Transform(Rot);
	  n.Transform(DRot);
	  b.Transform(DRot);
	  dn1 = dn + n;
	  db1 = db + b;
	  dn1.Transform(Transfo3.Inverted());
	  db1.Transform(Transfo3.Inverted());	
	
	  DM.SetCols(dn1.XYZ(), db1.XYZ(), DT.XYZ());	

//derivee seconde
	  D2N.Transform(Rot);
	  D2B.Transform(Rot);
	  dn3.Transform(DRot);
	  db3.Transform(DRot);
	  n2.Transform(D2Rot);
	  b2.Transform(D2Rot);
	  dn2 = n2 + 2*dn3 + D2N;
	  db2 = b2 + 2*db3 + D2B;
	  dn2.Transform(Transfo3.Inverted());
	  db2.Transform(Transfo3.Inverted());	

	  D2M.SetCols(dn2.XYZ(), db2.XYZ(), D2T.XYZ()); 

	}//if_result
      else {
#ifdef OCCT_DEBUG
	cout << "LocationGuide::D2 : No Result !!" <<endl;
	TraceRevol(Param, U, myLaw, mySec, myCurve, Trans);
#endif
	return Standard_False;
      }*/
    }//if_rotation

  else 
    {
      M.SetCols(N.XYZ(), B.XYZ(), T.XYZ());
      DM.SetCols(DN.XYZ(), DB.XYZ(), DT.XYZ());
      D2M.SetCols(D2N.XYZ(), D2B.XYZ(), D2T.XYZ()); 
    }

  return Standard_True;
//  return Standard_False;
}

//==================================================================
//Function : HasFirstRestriction
//Purpose :
//==================================================================
 Standard_Boolean GeomFill_LocationGuide::HasFirstRestriction() const
{ 
  return Standard_False;
}

//==================================================================
//Function : HasLastRestriction
//Purpose :
//==================================================================
 Standard_Boolean GeomFill_LocationGuide::HasLastRestriction() const
{
  return Standard_False;
}

//==================================================================
//Function : TraceNumber
//Purpose :
//==================================================================
 Standard_Integer GeomFill_LocationGuide::TraceNumber() const
{
  return 0;
}

//==================================================================
//Function : ErrorStatus
//Purpose :
//==================================================================
 GeomFill_PipeError GeomFill_LocationGuide::ErrorStatus() const
{
  return myStatus;
}

//==================================================================
//Function:NbIntervals
//Purpose :
//==================================================================
 Standard_Integer GeomFill_LocationGuide::NbIntervals
 (const GeomAbs_Shape S) const
{
  Standard_Integer Nb_Sec, Nb_Law;
  Nb_Sec  =  myTrimmed->NbIntervals(S);
  Nb_Law  =  myLaw->NbIntervals(S);

  if  (Nb_Sec==1) {
    return Nb_Law;
  }
  else if (Nb_Law==1) {
    return Nb_Sec;
  }

  TColStd_Array1OfReal IntC(1, Nb_Sec+1);
  TColStd_Array1OfReal IntL(1, Nb_Law+1);
  TColStd_SequenceOfReal    Inter;
  myTrimmed->Intervals(IntC, S);
  myLaw->Intervals(IntL, S);

  GeomLib::FuseIntervals( IntC, IntL, Inter, Precision::PConfusion()*0.99);
  return Inter.Length()-1;

}

//==================================================================
//Function:Intervals
//Purpose :
//==================================================================
 void GeomFill_LocationGuide::Intervals(TColStd_Array1OfReal& T,
					const GeomAbs_Shape S) const
{
   Standard_Integer Nb_Sec, Nb_Law;
  Nb_Sec  =  myTrimmed->NbIntervals(S);
  Nb_Law  =  myLaw->NbIntervals(S);

  if  (Nb_Sec==1) {
    myLaw->Intervals(T, S);
    return;
  }
  else if (Nb_Law==1) {
    myTrimmed->Intervals(T, S);
    return;
  }

  TColStd_Array1OfReal IntC(1, Nb_Sec+1);
  TColStd_Array1OfReal IntL(1, Nb_Law+1);
  TColStd_SequenceOfReal    Inter;
  myTrimmed->Intervals(IntC, S);
  myLaw->Intervals(IntL, S);

  GeomLib::FuseIntervals(IntC, IntL, Inter, Precision::PConfusion()*0.99);
  for (Standard_Integer ii=1; ii<=Inter.Length(); ii++)
    T(ii) = Inter(ii);
}

//==================================================================
//Function:SetInterval
//Purpose :
//==================================================================
 void GeomFill_LocationGuide::SetInterval(const Standard_Real First,
					  const Standard_Real Last) 
{
  myLaw->SetInterval(First, Last);
  myTrimmed = myCurve->Trim(First, Last, 0);
}
//==================================================================
//Function: GetInterval
//Purpose :
//==================================================================
 void GeomFill_LocationGuide::GetInterval(Standard_Real& First,
					  Standard_Real& Last) const
{
  First = myTrimmed->FirstParameter();
  Last = myTrimmed->LastParameter();
}

//==================================================================
//Function: GetDomain
//Purpose :
//==================================================================
 void GeomFill_LocationGuide::GetDomain(Standard_Real& First,
					Standard_Real& Last) const
{
  First = myCurve->FirstParameter();
  Last = myCurve->LastParameter();
}

//==================================================================
//function : SetTolerance
//purpose  : 
//==================================================================
void GeomFill_LocationGuide::SetTolerance(const Standard_Real Tol3d,
					  const Standard_Real )
{
  TolRes(1) = myGuide->Resolution(Tol3d);
  Resolution(1, Tol3d,  TolRes(2),  TolRes(3));
 
}

//==================================================================
//function : Resolution
//purpose  : A definir
//==================================================================
//void GeomFill_LocationGuide::Resolution (const Standard_Integer Index,
void GeomFill_LocationGuide::Resolution (const Standard_Integer ,
					 const Standard_Real Tol,
					 Standard_Real& TolU, 
					 Standard_Real& TolV) const			   
{
  TolU = Tol/100;
  TolV = Tol/100;
}

//==================================================================
//Function:GetMaximalNorm
//Purpose :  On suppose les triedres normes => return 1
//==================================================================
 Standard_Real GeomFill_LocationGuide::GetMaximalNorm() 
{
  return 1.;
}

//==================================================================
//Function:GetAverageLaw
//Purpose :
//==================================================================
 void GeomFill_LocationGuide::GetAverageLaw(gp_Mat& AM,
					    gp_Vec& AV) 
{
  Standard_Integer ii;
  Standard_Real U, delta;
  gp_Vec V, V1, V2, V3;
  
  myLaw->GetAverageLaw(V1, V2, V3);
  AM.SetCols(V1.XYZ(), V2.XYZ(), V3.XYZ());

  AV.SetCoord(0., 0., 0.);
  delta = (myTrimmed->LastParameter() - myTrimmed->FirstParameter())/10;
  U =  myTrimmed->FirstParameter(); 
  for (ii=0; ii<=myNbPts; ii++, U+=delta) {
    V.SetXYZ( myTrimmed->Value(U).XYZ() );
    AV += V;
  }
  AV = AV/(myNbPts+1);
}


//==================================================================
//Function : Section
//Purpose : 
//==================================================================
 Handle(Geom_Curve) GeomFill_LocationGuide::Section() const
{
  return mySec->ConstantSection();
}

//==================================================================
//Function : Guide
//Purpose : 
//==================================================================
 Handle(Adaptor3d_HCurve) GeomFill_LocationGuide::Guide() const
{
  return myGuide;
}

//==================================================================
//Function : IsRotation
//Purpose : 
//==================================================================
// Standard_Boolean GeomFill_LocationGuide::IsRotation(Standard_Real& Error)  const
 Standard_Boolean GeomFill_LocationGuide::IsRotation(Standard_Real& )  const
{
  return Standard_False;
}

//==================================================================
//Function : Rotation
//Purpose : 
//==================================================================
// void GeomFill_LocationGuide::Rotation(gp_Pnt& Centre)  const
 void GeomFill_LocationGuide::Rotation(gp_Pnt& )  const
{
  throw Standard_NotImplemented("GeomFill_LocationGuide::Rotation");
}

//==================================================================
//Function : IsTranslation
//Purpose : 
//==================================================================
// Standard_Boolean GeomFill_LocationGuide::IsTranslation(Standard_Real& Error) const
 Standard_Boolean GeomFill_LocationGuide::IsTranslation(Standard_Real& ) const
{
  return Standard_False;
}

//==================================================================
//Function : InitX
//Purpose : recherche par interpolation d'une valeur initiale
//==================================================================
void GeomFill_LocationGuide::InitX(const Standard_Real Param) const
{

  Standard_Integer Ideb = 1, Ifin =  myPoles2d->RowLength(), Idemi;
  Standard_Real Valeur, t1, t2;

  
  Valeur = myPoles2d->Value(1, Ideb).X();
  if (Param == Valeur) {
    Ifin = Ideb+1; 
  }
  
  Valeur =  myPoles2d->Value(1, Ifin).X();
  if (Param == Valeur) {
    Ideb = Ifin-1; 
  } 
  
  while ( Ideb+1 != Ifin) {
    Idemi = (Ideb+Ifin)/2;
    Valeur = myPoles2d->Value(1, Idemi).X();
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

  t1 = myPoles2d->Value(1,Ideb).X();
  t2 = myPoles2d->Value(1,Ifin).X();
  Standard_Real diff = t2-t1;

  Standard_Real W1, W2;
  W1 = myPoles2d->Value(1,Ideb).Coord(2);
  W2 =  myPoles2d->Value(1,Ifin).Coord(2);
  const gp_Pnt2d& P1 = myPoles2d->Value(2, Ideb);
  const gp_Pnt2d& P2 = myPoles2d->Value(2, Ifin);

  if (diff > 1.e-7) {
    Standard_Real b = (Param-t1) / diff,
    a = (t2-Param) / diff;
    X(1) = a * W1 + b * W2;
    X(2) = a * P1.Coord(1) + b * P2.Coord(1); // angle
    X(3) = a * P1.Coord(2) + b * P2.Coord(2); // param isov
  }
  else {
    X(1) = (W1+W2) /2;
    X(2) = (P1.Coord(1) + P2.Coord(1)) /2;
    X(3) = (P1.Coord(2) + P2.Coord(2)) /2;
  }

  if (myGuide->IsPeriodic()) {
    X(1) = ElCLib::InPeriod(X(1), myGuide->FirstParameter(), 
			          myGuide->LastParameter());
  }
  X(2) = ElCLib::InPeriod(X(2), 0, 2*M_PI);
  if (mySec->IsUPeriodic()) {
    X(3) = ElCLib::InPeriod(X(3), Uf, Ul);
  } 
}


//==================================================================
//Function : SetOrigine
//Purpose : utilise pour ACR dans le cas ou la trajectoire est multi-edges
//==================================================================
void GeomFill_LocationGuide::SetOrigine(const Standard_Real Param1,
					const Standard_Real Param2)
{
  OrigParam1 = Param1;
  OrigParam2 = Param2;
}

//==================================================================
//Function : ComputeAutomaticLaw
//Purpose :
//==================================================================
GeomFill_PipeError GeomFill_LocationGuide::ComputeAutomaticLaw(Handle(TColgp_HArray1OfPnt2d)& ParAndRad) const
{
  gp_Pnt P;
  gp_Vec T,N,B;
  Standard_Integer ii;
  Standard_Real t;

  GeomFill_PipeError theStatus = GeomFill_PipeOk;

  Standard_Real f = myCurve->FirstParameter();
  Standard_Real l = myCurve->LastParameter();

  ParAndRad = new TColgp_HArray1OfPnt2d(1, myNbPts);
  for (ii = 1; ii <= myNbPts; ii++)
  {
    t = Standard_Real(myNbPts - ii)*f + Standard_Real(ii - 1)*l;
    t /= (myNbPts-1); 
    myCurve->D0(t, P);
    Standard_Boolean Ok = myLaw->D0(t, T, N, B);
    if (!Ok)
    {
      theStatus = myLaw->ErrorStatus();
      return theStatus;
    }
    gp_Pnt PointOnGuide = myLaw->CurrentPointOnGuide();
    Standard_Real CurWidth = P.Distance(PointOnGuide);

    gp_Pnt2d aParamWithRadius(t, CurWidth);
    ParAndRad->SetValue(ii, aParamWithRadius);
  }

  return theStatus;
}
