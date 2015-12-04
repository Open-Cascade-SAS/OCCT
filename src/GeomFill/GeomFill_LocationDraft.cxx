// Created on: 1998-04-21
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
#include <Geom_Line.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomFill_DraftTrihedron.hxx>
#include <GeomFill_FunctionDraft.hxx>
#include <GeomFill_LocationDraft.hxx>
#include <GeomFill_LocationLaw.hxx>
#include <GeomFill_Tensor.hxx>
#include <GeomFill_TrihedronLaw.hxx>
#include <gp_Dir.hxx>
#include <gp_Mat.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <IntCurveSurface_HInter.hxx>
#include <IntCurveSurface_Intersection.hxx>
#include <IntCurveSurface_IntersectionPoint.hxx>
#include <math_FunctionSetWithDerivatives.hxx>
#include <math_Gauss.hxx>
#include <math_Matrix.hxx>
#include <math_NewtonFunctionSetRoot.hxx>
#include <math_Vector.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomFill_LocationDraft,GeomFill_LocationLaw)

//==================================================================
//Function: GeomFill_LocationDraft
//Purpose : constructor
//==================================================================
GeomFill_LocationDraft::GeomFill_LocationDraft
 (const gp_Dir& Direction,
  const Standard_Real Angle)
{
  myDir = Direction; // direction de depouille
 
  myAngle = Angle; // angle de depouille (teta prime)

  mySurf.Nullify();
  myLaw = new (GeomFill_DraftTrihedron)(myDir, Angle); // triedre
  myNbPts = 41; // nb de points utilises pour les calculs
  myPoles2d = new (TColgp_HArray1OfPnt2d)(1, 2*myNbPts);
  Intersec = Standard_False; //intersection avec surface d'arret ?
  WithTrans = Standard_False;
}



//==================================================================
//Function: Copy
//Purpose :
//==================================================================
 Handle(GeomFill_LocationLaw) GeomFill_LocationDraft::Copy() const
{
  Handle(GeomFill_TrihedronLaw) law;
  law = myLaw->Copy();
  Handle(GeomFill_LocationDraft) copy = 
    new (GeomFill_LocationDraft) (myDir,myAngle);
  copy->SetCurve(myCurve);
  copy->SetStopSurf(mySurf);
  if (WithTrans) copy->SetTrsf(Trans);

  return copy;
} 

//==================================================================
//Function: SetTrsf
//Purpose :
//==================================================================
 void GeomFill_LocationDraft::SetTrsf(const gp_Mat& Transfo) 
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
//Function: SetCurve
//Purpose : Calcul des poles sur la surfaces d'arret (intersection 
// entre la generatrice et la surface en myNbPts points de la section)
//==================================================================
 void GeomFill_LocationDraft::SetCurve(const Handle(Adaptor3d_HCurve)& C) 
{
  myCurve = C;
  myTrimmed = C;
  myLaw->SetCurve(C);

  Prepare(); 
}

//==================================================================
//Function: SetStopSurf
//Purpose : 
//==================================================================
 void GeomFill_LocationDraft::SetStopSurf(const Handle(Adaptor3d_HSurface)& Surf) 
{
  mySurf = Surf;
  Prepare();
}

//==================================================================
//Function: SetAngle
//Purpose : 
//==================================================================
 void GeomFill_LocationDraft::SetAngle(const Standard_Real Angle) 
{
  myAngle = Angle;
  myLaw->SetAngle(myAngle);
  Prepare();
}

//==================================================================
//Function: Prepare
//Purpose : Poses les jalon de l'intersection : depouille / Surface 
//==================================================================
 void GeomFill_LocationDraft::Prepare() 
{
  if (mySurf.IsNull()) {
    Intersec = Standard_False;
    return;
  }

  Intersec = Standard_True;

  Standard_Integer ii,jj; 
  Standard_Real f, l, t;
  gp_Pnt P;
  gp_Vec D,T,N,B; 
  Handle(Geom_Line) L;
  IntCurveSurface_IntersectionPoint P1,P2;
  f = myCurve->FirstParameter();
  l = myCurve->LastParameter();

  for (ii=1; ii<=myNbPts; ii++) 
    {
      t = Standard_Real(myNbPts - ii)*f + Standard_Real(ii - 1)*l;
      t /= (myNbPts-1);
  
      myCurve->D0(t, P);
      myLaw->D0(t,T,N,B);
  
// Generatrice
      D = Cos(myAngle)*B + Sin(myAngle)*N; 
  
      L = new (Geom_Line) (P, D);
   
      IntCurveSurface_HInter Int; // intersection surface / generatrice
      Handle(GeomAdaptor_HCurve) AC = new (GeomAdaptor_HCurve) (L);
      Int.Perform(AC, mySurf); // calcul de l'intersection

      if (Int.NbPoints() > 0) // il y a au moins 1 intersection
	{
	  P1 = Int.Point(1);  // 1er  point d'intersection          

	  for (jj=2 ; jj<=Int.NbPoints() ; jj++) 
	    {
	      P2 = Int.Point(jj);
	      if(P1.W() > P2.W()) P1 = P2; // point le plus proche
	    }//for_jj
   
	  gp_Pnt2d p (P1.W(), t);        // point de la courbe
	  gp_Pnt2d q (P1.U(),P1.V());    // point sur la surface
	  myPoles2d->SetValue(2*ii-1,p); // point de la courbe (indice impair)
	  myPoles2d->SetValue(2*ii,q);   // point sur la surface (indice pair)
	}
      else 
	{// au moins un point ou il n'y a pas intersection
	  Intersec = Standard_False;
	}

    }//for_ii
}

//==================================================================
//Function: GetCurve
//Purpose : return the path
//==================================================================
 const Handle(Adaptor3d_HCurve)& GeomFill_LocationDraft::GetCurve() const
{
  return myCurve;
}


//==================================================================
//Function: D0
//Purpose : 
//==================================================================
 Standard_Boolean GeomFill_LocationDraft::D0(const Standard_Real Param, 
					     gp_Mat& M,
					     gp_Vec& V)
{
  Standard_Boolean Ok;
  gp_Vec T,N,B;
  gp_Pnt P;

  myTrimmed->D0(Param, P);
  V.SetXYZ(P.XYZ());
 
  Ok = myLaw->D0(Param, T, N, B);  
  if (!Ok) return Ok;
  M.SetCols(N.XYZ(), B.XYZ(), T.XYZ());

  if (WithTrans) {
    M *= Trans;
  }

  return Standard_True;
}

//==================================================================
//Function: D0
//Purpose : calcul de l'intersection (C0) sur la surface
//================================================================== 
 Standard_Boolean GeomFill_LocationDraft::D0(const Standard_Real Param, 
					     gp_Mat& M,
					     gp_Vec& V,
					     TColgp_Array1OfPnt2d& Poles2d)
{ 
  Standard_Boolean Ok;
//  gp_Vec D,T,N,B,DT,DN,DB;
  gp_Vec D,T,N,B;
  gp_Pnt P;

  myCurve->D0(Param, P);
  V.SetXYZ(P.XYZ());
  Ok = myLaw->D0(Param, T, N, B);
  if (!Ok) return Ok;
  M.SetCols(N.XYZ(), B.XYZ(), T.XYZ());

  if (WithTrans) {
    M *= Trans;
  }

  if (Intersec == Standard_True) {
    // la generatrice intersecte la surface d'arret
    // la generatrice  
    D = Cos(myAngle)*B + Sin(myAngle)*N; 
    
    Handle(Geom_Line) L = new (Geom_Line) (P, D);
    Handle(GeomAdaptor_HCurve) G = new (GeomAdaptor_HCurve) (L); 
    
    Standard_Real t1,t2,Paramt1,t2Param;
    Standard_Real U0=0,V0=0,W0=0;
    
    Standard_Integer ii = 1; 
    
    // on recherche l'intervalle auquel appartient Param
      while (ii<2*myNbPts && myPoles2d->Value(ii).Coord(2) < Param) ii=ii+2;
    
    if (ii<2*myNbPts && !IsEqual(myPoles2d->Value(ii).Coord(2),Param)) 
      {
	
	// interpolation lineaire pour initialiser le germe de la recherche
	  t1 = myPoles2d->Value(ii).Coord(2);
	  t2 = myPoles2d->Value(ii-2).Coord(2);
	  
	  Paramt1 = (Param-t1) / (t2-t1);
	  t2Param = (t2-Param) / (t2-t1);

	  W0 = myPoles2d->Value(ii-2).Coord(1)*Paramt1 
                        + myPoles2d->Value(ii).Coord(1)*t2Param;
	  U0 = myPoles2d->Value(ii-1).Coord(1)*Paramt1 
                        + myPoles2d->Value(ii+1).Coord(1)*t2Param;
	  V0 = myPoles2d->Value(ii-1).Coord(2)*Paramt1 
                        + myPoles2d->Value(ii+1).Coord(2)*t2Param;
	}//if
      // on est sur un param ou les points ont deja ete calcules
      else if (ii<2*myNbPts && IsEqual(myPoles2d->Value(ii).Coord(2) ,Param)) 
	{
	  W0 = myPoles2d->Value(ii).Coord(1);
	  U0 = myPoles2d->Value(ii+1).Coord(1);
	  V0 = myPoles2d->Value(ii+1).Coord(2); 
	}//else if

 // recherche de la solution (pt d'intersection generatrice / surface)
      // point initial
      math_Vector X(1,3);
      X(1) = W0;
      X(2) = U0;
      X(3) = V0;

      // tolerance sur X
      math_Vector XTol(1,3);
      XTol.Init(0.00001);

      // tolerance sur F
      Standard_Real FTol = 0.0000001;
      Standard_Integer Iter = 100;

      // fonction dont il faut trouver la racine : G(W)-S(U,V)=0
      GeomFill_FunctionDraft E(mySurf , G);
 
      // resolution
      math_NewtonFunctionSetRoot Result(E, XTol, FTol, Iter);
      Result.Perform(E, X);

      if (Result.IsDone()) 
      {
        math_Vector R(1,3); 
        Result.Root(R);    // solution

        gp_Pnt2d p (R(2), R(3));  // point sur la surface
        gp_Pnt2d q (R(1), Param); // point de la courbe
        Poles2d.SetValue(1,p);
        Poles2d.SetValue(2,q);
      }
      else {
        return Standard_False;
      }
  }// if_Intersec

  // la generatrice n'intersecte pas la surface d'arret
  return Standard_True;
 }

//==================================================================
//Function: D1
//Purpose : calcul de l'intersection (C1) sur la surface
//================================================================== 
 Standard_Boolean GeomFill_LocationDraft::D1(const Standard_Real Param,
					     gp_Mat& M,
					     gp_Vec& V,
					     gp_Mat& DM,
					     gp_Vec& DV,
					     TColgp_Array1OfPnt2d& Poles2d,
					     TColgp_Array1OfVec2d& DPoles2d) 
{  
  Standard_Boolean Ok;
  gp_Vec D,T,N,B,DT,DN,DB;
  gp_Pnt P;

  myCurve->D1(Param, P, DV);
  V.SetXYZ(P.XYZ());
 
  Ok = myLaw->D1(Param, T, DT, N, DN, B, DB);
  if (!Ok) return Standard_False;

  M.SetCols(N.XYZ(), B.XYZ(), T.XYZ());
  DM.SetCols(DN.XYZ(), DB.XYZ(), DT.XYZ());

  if (WithTrans) {
    M *= Trans;
    DM *= Trans;
  }

  if (Intersec == Standard_True) 
    { // la generatrice intersecte la surface d'arret
      // la generatrice  
      D = Cos(myAngle)*B + Sin(myAngle)*N; 

      Handle(Geom_Line) L = new (Geom_Line) (P, D);
      Handle(GeomAdaptor_HCurve) G = new (GeomAdaptor_HCurve) (L); 
  
      Standard_Real t1,t2,Paramt1,t2Param;
      Standard_Real U0=0,V0=0,W0=0;

      Standard_Integer ii = 1; 
  
      // recherche de la solution (pt d'intersection  generatrice / surface)

      // on recherche l'intervalle auquel appartient Param
      while (ii < 2*myNbPts && myPoles2d->Value(ii).Coord(2) < Param) ii=ii+2;


      if (ii < 2*myNbPts && !IsEqual(myPoles2d->Value(ii).Coord(2) ,Param)) 
	{
	  // interpolation lineaire pour initialiser le germe de la recherche
	  t1 = myPoles2d->Value(ii).Coord(2);
	  t2 = myPoles2d->Value(ii-2).Coord(2);

	  Paramt1 = (Param-t1) / (t2-t1);
	  t2Param = (t2-Param) / (t2-t1);

	  W0 = myPoles2d->Value(ii-2).Coord(1)*Paramt1 
                       + myPoles2d->Value(ii).Coord(1)*t2Param;
	  U0 = myPoles2d->Value(ii-1).Coord(1)*Paramt1 
                       + myPoles2d->Value(ii+1).Coord(1)*t2Param;
	  V0 = myPoles2d->Value(ii-1).Coord(2)*Paramt1 
                       + myPoles2d->Value(ii+1).Coord(2)*t2Param;
	}//if
      else if (ii<2*myNbPts && IsEqual(myPoles2d->Value(ii).Coord(2) ,Param)) 
	{
	  W0 = myPoles2d->Value(ii).Coord(1);
	  U0 = myPoles2d->Value(ii+1).Coord(1);
	  V0 = myPoles2d->Value(ii+1).Coord(2); 
	}//else if

      // germe
      math_Vector X(1,3);
      X(1) = W0;
      X(2) = U0;
      X(3) = V0;

      // tolerance sur X
      math_Vector XTol(1,3);
      XTol.Init(0.0001);

      // tolerance sur F
      Standard_Real FTol = 0.000001;
      Standard_Integer Iter = 100;


      // fonction dont il faut trouver la racine : G(W)-S(U,V)=0
      GeomFill_FunctionDraft E(mySurf,G);

 
      // resolution
      math_NewtonFunctionSetRoot Result(E, XTol, FTol, Iter);
      Result.Perform(E, X);

      if (Result.IsDone()) 
      {
        math_Vector R(1,3); 
        Result.Root(R);    // solution

        gp_Pnt2d p (R(2), R(3));  // point sur la surface
        gp_Pnt2d q (R(1), Param); // point de la courbe
        Poles2d.SetValue(1,p);
        Poles2d.SetValue(2,q);

        // derivee de la fonction par rapport a Param
        math_Vector DEDT(1,3,0);
        E.DerivT(myTrimmed, Param, R(1), DN, myAngle, DEDT); // dE/dt => DEDT

        math_Vector DSDT (1,3,0);
        math_Matrix DEDX (1,3,1,3,0);
        E.Derivatives(R, DEDX);  // dE/dx au point R => DEDX

        // resolution du syst. lin. : DEDX*DSDT = -DEDT
        math_Gauss Ga(DEDX);
        if (Ga.IsDone()) 
        {
          Ga.Solve (DEDT.Opposite(), DSDT); // resolution du syst. lin. 
          gp_Vec2d dp (DSDT(2), DSDT(3));    // surface
          gp_Vec2d dq (DSDT(1), 1);          //  courbe
          DPoles2d.SetValue(1, dp);
          DPoles2d.SetValue(2, dq);
        }//if


      }//if_Result
      else {// la generatrice n'intersecte pas la surface d'arret
        return Standard_False;
      }    
  }// if_Intersec
  return Standard_True;
}

//==================================================================
//Function: D2
//Purpose : calcul de l'intersection (C2) sur la surface
//==================================================================
 Standard_Boolean GeomFill_LocationDraft::D2(const Standard_Real Param,
					     gp_Mat& M,
					     gp_Vec& V,
					     gp_Mat& DM,
					     gp_Vec& DV, 
					     gp_Mat& D2M,
					     gp_Vec& D2V,
					     TColgp_Array1OfPnt2d& Poles2d,
					     TColgp_Array1OfVec2d& DPoles2d,
					     TColgp_Array1OfVec2d& D2Poles2d) 
{
  Standard_Boolean Ok;
  gp_Vec D,T,N,B,DT,DN,DB,D2T,D2N,D2B;
  gp_Pnt P;

  myCurve->D2(Param, P, DV, D2V);
  V.SetXYZ(P.XYZ());
 
  Ok = myLaw->D2(Param, T, DT, D2T, N, DN, D2N, B, DB, D2B);
  if (!Ok) return Ok;

  M.SetCols(N.XYZ(), B.XYZ(), T.XYZ());
  DM.SetCols(DN.XYZ(), DB.XYZ(), DT.XYZ());
  D2M.SetCols(D2N.XYZ(), D2B.XYZ(), D2T.XYZ());

  if (WithTrans) {
    M *= Trans;
    DM *= Trans;
    D2M *= Trans;
  }
  if (Intersec == Standard_True) 
    {// la generatrice intersecte la surface d'arret

      // la generatrice  
      D = Cos(myAngle) * B + Sin(myAngle) * N; 

      Handle(Geom_Line) L = new (Geom_Line) (P, D);
      Handle(GeomAdaptor_HCurve) G = new (GeomAdaptor_HCurve) (L); 
  
      Standard_Real t1,t2,Paramt1,t2Param;
      Standard_Real U0=0,V0=0,W0=0;
  
      Standard_Integer ii = 1; 

      // on recherche l'intervalle auquel appartient Param
      while (ii<2*myNbPts && myPoles2d->Value(ii).Coord(2) < Param) ii=ii+2;

      if (ii<2*myNbPts && !IsEqual(myPoles2d->Value(ii).Coord(2) ,Param)) 
	{

	  // interpolation lineaire pour initialiser le germe de la recherche
	  t1 = myPoles2d->Value(ii).Coord(2);
	  t2 = myPoles2d->Value(ii-2).Coord(2);

	  Paramt1 = (Param-t1) / (t2-t1);
	  t2Param = (t2-Param) / (t2-t1);

	  W0 = myPoles2d->Value(ii-2).Coord(1)*Paramt1 + 
	    myPoles2d->Value(ii).Coord(1)*t2Param;
	  U0 = myPoles2d->Value(ii-1).Coord(1)*Paramt1 + 
	    myPoles2d->Value(ii+1).Coord(1)*t2Param;
	  V0 = myPoles2d->Value(ii-1).Coord(2)*Paramt1 + 
	    myPoles2d->Value(ii+1).Coord(2)*t2Param;
	}//if
      else if (ii<2*myNbPts  && IsEqual(myPoles2d->Value(ii).Coord(2) ,Param)) 
	{
	  W0 = myPoles2d->Value(ii).Coord(1);
	  U0 = myPoles2d->Value(ii+1).Coord(1);
	  V0 = myPoles2d->Value(ii+1).Coord(2); 
	}//else if

// recherche de la solution (pt d'intersection generatrice / surface)
      // germe
      math_Vector X(1,3);
      X(1) = W0;
      X(2) = U0;
      X(3) = V0;

      // tolerance sur X
      math_Vector XTol(1,3);
      XTol.Init(0.0001);

      // tolerance sur F
      Standard_Real FTol = 0.000001;
      Standard_Integer Iter = 150;


      // fonction dont il faut trouver la racine : G(W)-S(U,V)=0
      GeomFill_FunctionDraft E(mySurf,G);

 
      // resolution
      math_NewtonFunctionSetRoot Result (E, XTol, FTol, Iter);
      Result.Perform(E, X);

      if (Result.IsDone()) 
	{
	  math_Vector R(1,3); 
	  Result.Root(R);    // solution
  
	  // solution
	  gp_Pnt2d p (R(2), R(3));  // point sur la surface
	  gp_Pnt2d q (R(1), Param); // point de la courbe
	  Poles2d.SetValue(1,p);
	  Poles2d.SetValue(2,q);


     // premiere derivee de la fonction
	  math_Vector DEDT(1,3,0);
	  E.DerivT(myTrimmed, Param, R(1), DN, myAngle, DEDT); // dE/dt => DEDT

	  math_Vector DSDT (1,3,0);
	  math_Matrix DEDX (1,3,1,3,0);
	  E.Derivatives(R, DEDX);  // dE/dx => DEDX
 
	  // resolution du syst. lin.
	  math_Gauss Ga (DEDX);
	  if (Ga.IsDone()) 
	    {
	      Ga.Solve (DEDT.Opposite(), DSDT); 
	      gp_Vec2d dp (DSDT(2), DSDT(3));   // surface
	      gp_Vec2d dq (DSDT(1), 1);         //  courbe
	      DPoles2d.SetValue(1, dp);
	      DPoles2d.SetValue(2, dq);
	    }//if


     // deuxieme derivee
	  GeomFill_Tensor D2EDX2(3,3,3);
	  E.Deriv2X(R, D2EDX2); // d2E/dx2

	  math_Vector D2EDT2(1,3,0);
	  E.Deriv2T(myTrimmed, Param, R(1), D2N, myAngle ,D2EDT2); // d2E/dt2

	  math_Matrix D2EDTDX(1,3,1,3,0);
	  E.DerivTX(DN, myAngle, D2EDTDX); // d2E/dtdx

	  math_Vector D2SDT2(1,3,0); // d2s/dt2
	  math_Matrix aT(1,3,1,3,0);
	  D2EDX2.Multiply(DSDT,aT);
   
	  // resolution du syst. lin. 
	  math_Gauss Ga1 (DEDX);
	  if (Ga1.IsDone()) 
	    {
	      Ga1.Solve ( -aT*DSDT - 2*D2EDTDX*DSDT - D2EDT2 , D2SDT2); 
	      gp_Vec2d d2p (D2SDT2(2), D2SDT2(3));  // surface
	      gp_Vec2d d2q (D2SDT2(1), 0);          // courbe
	      D2Poles2d.SetValue(1, d2p);
	      D2Poles2d.SetValue(2, d2q);
	    }//if
	  else {// la generatrice n'intersecte pas la surface d'arret
	    return Standard_False;
	  }
	}//if_Result
    } //if_Intersec

  return Standard_True; 
}

//==================================================================
//Function : HasFirstRestriction
//Purpose :
//==================================================================
 Standard_Boolean GeomFill_LocationDraft::HasFirstRestriction() const
{
  return Standard_False;
}

//==================================================================
//Function : HasLastRestriction
//Purpose :
//==================================================================
 Standard_Boolean GeomFill_LocationDraft::HasLastRestriction() const
{
 
  if (Intersec == Standard_True) return Standard_True;
  else return Standard_False;
}

//==================================================================
//Function : TraceNumber
//Purpose :
//==================================================================
 Standard_Integer GeomFill_LocationDraft::TraceNumber() const
{
  if (Intersec == Standard_True) return 1;
  else return 0;
}

//==================================================================
//Function:NbIntervals
//Purpose :
//==================================================================
 Standard_Integer GeomFill_LocationDraft::NbIntervals
 (const GeomAbs_Shape S) const
{
  return myLaw->NbIntervals(S);
}

//==================================================================
//Function:Intervals
//Purpose :
//==================================================================
 void GeomFill_LocationDraft::Intervals(TColStd_Array1OfReal& T,
					const GeomAbs_Shape S) const
{
  myLaw->Intervals(T, S);
}

//==================================================================
//Function:SetInterval
//Purpose :
//==================================================================
 void GeomFill_LocationDraft::SetInterval(const Standard_Real First,
					  const Standard_Real Last) 
{
  myLaw->SetInterval( First, Last);
  myTrimmed = myCurve->Trim( First, Last, 0);
}
//==================================================================
//Function: GetInterval
//Purpose :
//==================================================================
 void GeomFill_LocationDraft::GetInterval(Standard_Real& First,
					  Standard_Real& Last) const
{
  First = myTrimmed->FirstParameter();
  Last = myTrimmed->LastParameter();
}

//==================================================================
//Function: GetDomain
//Purpose :
//==================================================================
 void GeomFill_LocationDraft::GetDomain(Standard_Real& First,
					Standard_Real& Last) const
{
  First = myCurve->FirstParameter();
  Last = myCurve->LastParameter();
}

//==================================================================
//function : Resolution
//purpose  : 
//==================================================================
void GeomFill_LocationDraft::Resolution (const Standard_Integer Index,
					 const Standard_Real Tol,
					 Standard_Real& TolU, 
					 Standard_Real& TolV) const
				   
{
  if (Index==1) {
      TolU = mySurf->UResolution(Tol);
      TolV = mySurf->VResolution(Tol);
    }
  else {
      TolU = Tol;
      TolV = Tol;
    }
}

//==================================================================
//Function:GetMaximalNorm
//Purpose :  On suppose les triedres normes => return 1
//==================================================================
 Standard_Real GeomFill_LocationDraft::GetMaximalNorm() 
{
  return 1.;
}

//==================================================================
//Function:GetAverageLaw
//Purpose :
//==================================================================
 void GeomFill_LocationDraft::GetAverageLaw(gp_Mat& AM,
					    gp_Vec& AV) 
{
  Standard_Integer ii;
  Standard_Real U, delta;
  gp_Vec V1,V2,V3, V;
  
  myLaw->GetAverageLaw(V1, V2, V3);
  AM.SetCols(V1.XYZ(), V2.XYZ(), V3.XYZ());

  AV.SetCoord(0., 0., 0.);
  delta = (myTrimmed->LastParameter() - myTrimmed->FirstParameter())/10;
  U=  myTrimmed->FirstParameter(); 
  for (ii=0; ii<=10; ii++, U+=delta) {
    V.SetXYZ( myTrimmed->Value(U).XYZ() );
    AV += V;
  }
  AV /= 11;   
}

//==================================================================
//Function : IsTranslation
//Purpose : 
//==================================================================
// Standard_Boolean GeomFill_LocationDraft::IsTranslation(Standard_Real& Error) const
 Standard_Boolean GeomFill_LocationDraft::IsTranslation(Standard_Real& ) const
{
  return myLaw->IsConstant();
}

//==================================================================
//Function : IsRotation
//Purpose : 
//==================================================================
 Standard_Boolean GeomFill_LocationDraft::IsRotation(Standard_Real& Error)  const
{
  GeomAbs_CurveType Type;
  Error = 0;
  Type = myCurve->GetType();
  if (Type == GeomAbs_Circle) {
    return myLaw->IsOnlyBy3dCurve();
  }
  return Standard_False;
}

//==================================================================
//Function : Rotation
//Purpose : 
//==================================================================
 void GeomFill_LocationDraft::Rotation(gp_Pnt& Centre)  const
{
  Centre =  myCurve->Circle().Location();
}


//==================================================================
//Function : IsIntersec
//Purpose : 
//==================================================================
 Standard_Boolean GeomFill_LocationDraft::IsIntersec()  const
{
 return Intersec;
}


//==================================================================
//Function : Direction
//Purpose : 
//==================================================================
 gp_Dir GeomFill_LocationDraft::Direction()  const
{
 return myDir;
}
