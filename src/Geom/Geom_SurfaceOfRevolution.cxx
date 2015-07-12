// Created on: 1993-03-10
// Created by: JCV
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


#include <BSplCLib.hxx>
#include <BSplSLib.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_UndefinedDerivative.hxx>
#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir.hxx>
#include <gp_GTrsf2d.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

#define  POLES    (poles->Array2())
#define  WEIGHTS  (weights->Array2())
#define  UKNOTS   (uknots->Array1())
#define  VKNOTS   (vknots->Array1())
#define  UFKNOTS  (ufknots->Array1())
#define  VFKNOTS  (vfknots->Array1())
#define  FMULTS   (BSplCLib::NoMults())

typedef Geom_SurfaceOfRevolution         SurfaceOfRevolution;
typedef Geom_Curve                       Curve;
typedef gp_Ax1  Ax1;
typedef gp_Ax2  Ax2;
typedef gp_Dir  Dir;
typedef gp_Lin  Lin;
typedef gp_Pnt  Pnt;
typedef gp_Trsf Trsf;
typedef gp_Vec  Vec;
typedef gp_XYZ  XYZ;

//=======================================================================
//function : LocateSide
//purpose  : This  method locates U parameter on basis BSpline curve 
//  	       and calls LocalDi  methods corresponding an  order of  
//    	       derivative and  position of the surface side contained      
//    	       the point relatively the curve knots.
//=======================================================================
static void LocateSide(const Standard_Real U,
		       const Standard_Integer Side,
		       const Handle(Geom_BSplineCurve)& BSplC,
		       const Standard_Integer NDir,
		       gp_Pnt& P,
		       gp_Vec& D1U,
		       gp_Vec& D2U,
		       gp_Vec& D3U) 
{ 
  Standard_Integer Ideb, Ifin;
  Standard_Real ParTol=Precision::PConfusion()/2;
  BSplC->Geom_BSplineCurve::LocateU(U,ParTol,Ideb,Ifin,Standard_False);   
  if(Side == 1)
    {
      if(Ideb<1) Ideb=1;
      if ((Ideb>=Ifin))  Ifin = Ideb+1;
    }else
      if(Side ==-1)
	{ 
	  if(Ifin > BSplC -> NbKnots()) Ifin=BSplC->NbKnots();
	  if ((Ideb>=Ifin))  Ideb = Ifin-1;
	}
   
  switch(NDir) {
  case 0 :  BSplC->Geom_BSplineCurve::LocalD0(U,Ideb,Ifin,P); break;
  case 1 :  BSplC->Geom_BSplineCurve::LocalD1(U,Ideb,Ifin,P,D1U);  break;
  case 2 :  BSplC->Geom_BSplineCurve::LocalD2(U,Ideb,Ifin,P,D1U,D2U); break;
  case 3 :  BSplC->Geom_BSplineCurve::LocalD3(U,Ideb,Ifin,P,D1U,D2U,D3U); break;
  }
}

//=======================================================================
//function : LocateSideN
//purpose  : This  method locates U parameter on basis BSpline curve 
//  	       and calls LocalDN  method corresponding  position of  surface side 
//    	       contained the point relatively the curve knots.  
//=======================================================================
static gp_Vec LocateSideN(const Standard_Real V,
			  const Standard_Integer Side,
			  const Handle(Geom_BSplineCurve)& BSplC,
			  const	Standard_Integer  Nv ) 
{ 
  Standard_Integer Ideb, Ifin;
  Standard_Real ParTol=Precision::PConfusion()/2;
  BSplC->Geom_BSplineCurve::LocateU(V,ParTol,Ideb,Ifin,Standard_False);   
  if(Side == 1)
    {
      if(Ideb<1) Ideb=1;
      if ((Ideb>=Ifin))  Ifin = Ideb+1;
    }else
      if(Side ==-1)
	{ 
	  if(Ifin > BSplC -> NbKnots()) Ifin=BSplC->NbKnots();
	  if ((Ideb>=Ifin))  Ideb = Ifin-1;
	}
  return BSplC->Geom_BSplineCurve::LocalDN(V,Ideb,Ifin,Nv);
}




//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Geom_Geometry) Geom_SurfaceOfRevolution::Copy () const {

  return new Geom_SurfaceOfRevolution (basisCurve, Axis());
}


//=======================================================================
//function : Geom_SurfaceOfRevolution
//purpose  : 
//=======================================================================

Geom_SurfaceOfRevolution::Geom_SurfaceOfRevolution 
  (const Handle(Geom_Curve)& C , 
   const Ax1&           A1 ) : loc (A1.Location()) {

  basisCurve = Handle(Geom_Curve)::DownCast(C->Copy());
  direction  = A1.Direction();
  smooth     = C->Continuity();
}


//=======================================================================
//function : UReverse
//purpose  : 
//=======================================================================

void Geom_SurfaceOfRevolution::UReverse () { 

  direction.Reverse(); 
}


//=======================================================================
//function : UReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_SurfaceOfRevolution::UReversedParameter (const Standard_Real U) const {

  return ( 2.*M_PI - U);
}


//=======================================================================
//function : VReverse
//purpose  : 
//=======================================================================

void Geom_SurfaceOfRevolution::VReverse () { 

  basisCurve->Reverse(); 
}


//=======================================================================
//function : VReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_SurfaceOfRevolution::VReversedParameter (const Standard_Real V) const {

  return basisCurve->ReversedParameter(V);
}


//=======================================================================
//function : Location
//purpose  : 
//=======================================================================

const gp_Pnt& Geom_SurfaceOfRevolution::Location () const { 

  return loc; 
}

//=======================================================================
//function : IsUPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom_SurfaceOfRevolution::IsUPeriodic () const {

  return Standard_True; 
}

//=======================================================================
//function : IsCNu
//purpose  : 
//=======================================================================

Standard_Boolean Geom_SurfaceOfRevolution::IsCNu (const Standard_Integer ) const  {

  return Standard_True;
}

//=======================================================================
//function : Axis
//purpose  : 
//=======================================================================

Ax1 Geom_SurfaceOfRevolution::Axis () const  { 

  return Ax1 (loc, direction); 
}

//=======================================================================
//function : IsCNv
//purpose  : 
//=======================================================================

Standard_Boolean Geom_SurfaceOfRevolution::IsCNv (const Standard_Integer N) const {

  Standard_RangeError_Raise_if (N < 0, " ");
  return basisCurve->IsCN(N);
}


//=======================================================================
//function : IsUClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom_SurfaceOfRevolution::IsUClosed () const { 

  return Standard_True; 
}

//=======================================================================
//function : IsVClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom_SurfaceOfRevolution::IsVClosed () const 
{ 
  return basisCurve->IsClosed();
}


//=======================================================================
//function : IsVPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom_SurfaceOfRevolution::IsVPeriodic () const { 

  return basisCurve->IsPeriodic(); 
}


//=======================================================================
//function : SetAxis
//purpose  : 
//=======================================================================

void Geom_SurfaceOfRevolution::SetAxis (const Ax1& A1) {

   direction = A1.Direction();
   loc = A1.Location();
}


//=======================================================================
//function : SetDirection
//purpose  : 
//=======================================================================

void Geom_SurfaceOfRevolution::SetDirection (const Dir& V) {

   direction = V;
}


//=======================================================================
//function : SetBasisCurve
//purpose  : 
//=======================================================================

void Geom_SurfaceOfRevolution::SetBasisCurve (const Handle(Geom_Curve)& C) {

   basisCurve = Handle(Geom_Curve)::DownCast(C->Copy());
   smooth     = C->Continuity();
}


//=======================================================================
//function : SetLocation
//purpose  : 
//=======================================================================

void Geom_SurfaceOfRevolution::SetLocation (const Pnt& P) {

   loc = P;
}


//=======================================================================
//function : Bounds
//purpose  : 
//=======================================================================

void Geom_SurfaceOfRevolution::Bounds ( Standard_Real& U1, 
				        Standard_Real& U2, 
				        Standard_Real& V1, 
				        Standard_Real& V2 ) const {

  U1 = 0.0; 
  U2 = 2.0 * M_PI; 
  V1 = basisCurve->FirstParameter();  
  V2 = basisCurve->LastParameter();
}


//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void Geom_SurfaceOfRevolution::D0 
  (const Standard_Real U, const Standard_Real V, Pnt& P) const {

   // C origine sur l'axe de revolution
   // Vdir vecteur unitaire definissant la direction de l'axe de revolution
   // Q(v) point de parametre V sur la courbe de revolution
   // OM (u,v) = OC + CQ * Cos(U) + (CQ.Vdir)(1-Cos(U)) * Vdir +
   //            (Vdir^CQ)* Sin(U)


   Pnt Pc = basisCurve->Value (V);                  //Q(v)
   XYZ Q  = Pc.XYZ();                               //Q
   XYZ C  = loc.XYZ();                              //C
   Q.Subtract(C);                                   //CQ
   XYZ Vdir     = direction.XYZ();                  //Vdir
   XYZ VcrossCQ = Vdir.Crossed (Q);                 //Vdir^CQ
   VcrossCQ.Multiply (Sin(U));                      //(Vdir^CQ)*Sin(U)
   XYZ VdotCQ =
     Vdir.Multiplied ((Vdir.Dot(Q))*(1.0 - Cos(U)));//(CQ.Vdir)(1-Cos(U))Vdir
   VdotCQ.Add (VcrossCQ);                           //addition des composantes
   Q.Multiply (Cos(U));
   Q.Add (VdotCQ);
   Q.Add (C);
   P.SetXYZ(Q);
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Geom_SurfaceOfRevolution::D1 
  (const Standard_Real U, const Standard_Real V, 
         Pnt& P, 
         Vec& D1U, Vec& D1V   ) const {

   // C origine sur l'axe de revolution
   // Vdir vecteur unitaire definissant la direction de l'axe de revolution
   // Q(v) point de parametre V sur la courbe de revolution
   // Q'(v) = DQ/DV
   // OM (u,v) = OC + CQ * Cos(U) + (CQ.Vdir)(1-Cos(U)) * Vdir + 
   //            (Vdir^CQ) * Sin(U)
   // D1U_M(u,v) = - CQ * Sin(U) + (CQ.Vdir)(Sin(U)) * Vdir +
   //              (Vdir^CQ) * Cos(U)
   // D1V_M(u,v) = Q' * Cos(U) + (Q'.Vdir)(1-Cos(U)) * Vdir +
   //              (Vdir^Q') * Sin(U)
	   
      Pnt Pc;
      Vec V1;
      basisCurve->D1 (V, Pc, V1);
      XYZ Q    = Pc.XYZ();                               //Q
      XYZ DQv  = V1.XYZ();                               //Q'
      XYZ C    = loc.XYZ();                              //C
      XYZ Vdir = direction.XYZ();                        //Vdir
      Q.Subtract(C);                                     //CQ
      XYZ VcrossCQ  = Vdir.Crossed (Q);                  //Vdir^CQ
      // If the point is placed on the axis of revolution then derivatives on U are undefined.
      // Manually set them to zero.
      if (VcrossCQ.SquareModulus() < Precision::SquareConfusion())
        VcrossCQ.SetCoord(0.0, 0.0, 0.0);
      XYZ VcrossDQv = Vdir.Crossed (DQv);                //(Vdir^Q')
      XYZ VdotCQ    = Vdir.Multiplied (Vdir.Dot(Q));     //(Vdir.CQ)Vdir
      XYZ VdotDQv   = Vdir.Multiplied (Vdir.Dot(DQv));   //(Vdir.Q')Vdir

      VcrossDQv.Multiply (Sin(U));
      VdotDQv.Multiply   (1.0 - Cos(U));
      VdotDQv.Add        (VcrossDQv);
      DQv.Multiply       (Cos(U));
      DQv.Add            (VdotDQv);
      D1V.SetXYZ         (DQv);

      XYZ DQu = Q.Multiplied       (-Sin(U));
      DQu.Add (VcrossCQ.Multiplied (Cos(U)));
      DQu.Add (VdotCQ.Multiplied (Sin(U)));
      D1U.SetXYZ (DQu);

      Q.Multiply (Cos(U));
      Q.Add      (C);
      VcrossCQ.Multiply (Sin(U));
      Q.Add             (VcrossCQ);   
      VdotCQ.Multiply (1.0-Cos(U));
      Q.Add           (VdotCQ);   
      P.SetXYZ (Q);      
    }

//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Geom_SurfaceOfRevolution::D2 
  (const Standard_Real   U, const Standard_Real V,
         Pnt&   P, 
         Vec& D1U, Vec& D1V, 
         Vec& D2U, Vec& D2V, Vec& D2UV ) const {


   // C origine sur l'axe de revolution
   // V vecteur unitaire definissant la direction de l'axe de revolution
   // Q(v) point de parametre V sur la courbe de revolution
   // Q'(v) = D1Q/DV
   // Q"(v) = D2Q/DV
   // OM (u,v) = OC + CQ * Cos(U) + (CQ.Vdir)(1-Cos(U)) * Vdir +
   //            (Vdir^CQ) * Sin(U)
   // D1U_M(u,v) = - CQ * Sin(U) + (CQ.Vdir)(Sin(U)) * Vdir +
   //              (Vdir^CQ) * Cos(U)
   // D1V_M(u,v) = Q' * Cos(U) + (Q'.Vdir)(1-Cos(U)) * Vdir +
   //              (Vdir^Q') * Sin(U)
   // D2U_M(u,v) = -CQ * Cos(U) + (CQ.Vdir)(Cos(U)) * Vdir + 
   //              (Vdir^CQ) * -(Sin(U))
   // D2V_M(u,v) = Q" * Cos(U) + (Q".Vdir)(1-Cos(U)) * Vdir +
   //              (Vdir^Q") * Sin(U)
   // D2UV_M(u,v)= -Q' * Sin(U) + (Q'.Vdir)(Sin(U)) * Vdir +
   //              (Vdir^Q') * Cos(U)


      Pnt Pc;
      Vec V1 , V2;
      basisCurve->D2 (V, Pc, V1, V2);
      XYZ Q     = Pc.XYZ();                                 //Q
      XYZ D1Qv  = V1.XYZ();                                 //Q'
      XYZ D2Qv  = V2.XYZ();                                 //Q"
      XYZ C     = loc.XYZ();                                //C
      XYZ Vdir  = direction.XYZ();                          //Vdir
      Q.Subtract(C);                                        //CQ
      XYZ VcrossCQ   = Vdir.Crossed (Q);                    //Vdir^CQ
      // If the point is placed on the axis of revolution then derivatives on U are undefined.
      // Manually set them to zero.
      if (VcrossCQ.SquareModulus() < Precision::SquareConfusion())
        VcrossCQ.SetCoord(0.0, 0.0, 0.0);
      XYZ VcrossD1Qv = Vdir.Crossed (D1Qv);                 //(Vdir^Q')
      XYZ VcrossD2Qv = Vdir.Crossed (D2Qv);                 //(Vdir^Q")
      XYZ VdotCQ     = Vdir.Multiplied (Vdir.Dot(Q));       //(Vdir.CQ)Vdir
      XYZ VdotD1Qv   = Vdir.Multiplied (Vdir.Dot(D1Qv));    //(Vdir.Q')Vdir
      XYZ VdotD2Qv   = Vdir.Multiplied (Vdir.Dot(D2Qv));    //(Vdir.Q")Vdir

      
      XYZ D2Quv = D1Qv.Multiplied(-Sin(U));
      D2Quv.Add (VcrossD1Qv.Multiplied (Cos(U)));      
      D2Quv.Add (VdotD1Qv.Multiplied (Sin(U)));
      D2UV.SetXYZ (D2Quv);

      D1Qv.Multiply       (Cos(U));
      VcrossD1Qv.Multiply (Sin(U));
      VdotD1Qv.Multiply   (1.0 - Cos(U));
      D1Qv.Add            (VcrossD1Qv);
      D1Qv.Add            (VdotD1Qv);
      D1V.SetXYZ          (D1Qv);

      VcrossD2Qv.Multiply (Sin(U));
      VdotD2Qv.Multiply (1.0 - Cos(U));
      VdotD2Qv.Add (VcrossD2Qv);
      D2Qv.Multiply (Cos(U));
      D2Qv.Add (VdotD2Qv);
      D2V.SetXYZ (D2Qv);

      XYZ D1Qu = Q.Multiplied (-Sin(U));
      D1Qu.Add (VcrossCQ.Multiplied (Cos(U)));
      D1Qu.Add (VdotCQ.Multiplied (Sin(U)));
      D1U.SetXYZ (D1Qu);

      Q.Multiply (Cos(U));
      VcrossCQ.Multiply (Sin(U));
      Q.Add (VcrossCQ);   
      XYZ D2Qu = Q.Multiplied(-1.0);
      D2Qu.Add (VdotCQ.Multiplied (Cos(U)));
      D2U.SetXYZ (D2Qu);
      VdotCQ.Multiply (1.0-Cos(U));
      Q.Add (VdotCQ);   
      Q.Add (C);
      P.SetXYZ (Q);      
}



//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void Geom_SurfaceOfRevolution::D3 
  (const Standard_Real U, const Standard_Real V,
         Pnt& P,
         Vec& D1U, Vec& D1V, 
         Vec& D2U, Vec& D2V, Vec& D2UV,
         Vec& D3U, Vec& D3V, Vec& D3UUV, Vec& D3UVV ) const {

   // C origine sur l'axe de revolution
   // Vdir vecteur unitaire definissant la direction de l'axe de revolution
   // Q(v) point de parametre V sur la courbe de revolution
   // Q'(v) = D1Q/DV
   // Q"(v) = D2Q/DV
   // OM (u,v) = OC + CQ * Cos(u) + (CQ.Vdir)(1-Cos(u)) * Vdir +
   //            (Vdir^CQ) * Sin(u)
   // D1U_M(u,v) = - CQ * Sin(u) + (CQ.Vdir)(Sin(u)) * Vdir +
   //              (Vdir^CQ) * Cos(u)
   // D2U_M(u,v) = -CQ * Cos(u) + (CQ.Vdir)(Cos(u)) * Vdir +
   //              (Vdir^CQ) * -Sin(u)
   // D2UV_M(u,v)= -Q' * Sin(u) + (Q'.Vdir)(Sin(u)) * Vdir +
   //              (Vdir^Q') * Cos(u)
   // D3UUV_M(u,v) = -Q' * Cos(u) + (Q'.Vdir)(Cos(u)) * Vdir +
   //                (Vdir^Q') * -Sin(u)
   // D3U_M(u,v) = CQ * Sin(u) + (CQ.Vdir)(-Sin(u)) * Vdir +
   //              (Vdir^CQ) * -Cos(u)
   // D1V_M(u,v) = Q' * Cos(u) + (Q'.Vdir)(1-Cos(u)) * Vdir + 
   //              (Vdir^Q') * Sin(u)
   // D2V_M(u,v) = Q" * Cos(u) + (Q".Vdir)(1-Cos(u)) * Vdir +
   //              (Vdir^Q") * Sin(u)
   // D3UVV_M(u,v) = -Q" * Sin(u) + (Q".Vdir)(Sin(u)) * Vdir +
   //                (Vdir^Q") * Cos(u)
   // D3V_M(u,v) = Q'''* Cos(u) + (Q'''.Vdir)(1-Cos(u)) * Vdir +
   //             (Vdir^Q''') * Sin(u)
   

      Pnt Pc;
      Vec V1 , V2, V3;
      basisCurve->D3 (V, Pc, V1, V2, V3);
      XYZ Q     = Pc.XYZ();                                 //Q
      XYZ D1Qv  = V1.XYZ();                                 //Q'
      XYZ D2Qv  = V2.XYZ();                                 //Q"
      XYZ D3Qv  = V3.XYZ();                                 //Q'''
      XYZ C     = loc.XYZ();                                //C
      XYZ Vdir  = direction.XYZ();                          //Vdir
      Q.Subtract(C);                                        //CQ
      XYZ VcrossCQ   = Vdir.Crossed (Q);                    //Vdir^CQ
      // If the point is placed on the axis of revolution then derivatives on U are undefined.
      // Manually set them to zero.
      if (VcrossCQ.SquareModulus() < Precision::SquareConfusion())
        VcrossCQ.SetCoord(0.0, 0.0, 0.0);
      XYZ VcrossD1Qv = Vdir.Crossed (D1Qv);                 //(Vdir^Q')
      XYZ VcrossD2Qv = Vdir.Crossed (D2Qv);                 //(Vdir^Q")
      XYZ VcrossD3Qv = Vdir.Crossed (D3Qv);                 //(Vdir^Q''')
      XYZ VdotCQ     = Vdir.Multiplied (Vdir.Dot(Q));       //(Vdir.CQ)Vdir
      XYZ VdotD1Qv   = Vdir.Multiplied (Vdir.Dot(D1Qv));    //(Vdir.Q')Vdir
      XYZ VdotD2Qv   = Vdir.Multiplied (Vdir.Dot(D2Qv));    //(Vdir.Q")Vdir
      XYZ VdotD3Qv   = Vdir.Multiplied (Vdir.Dot(D3Qv));    //(Vdir.Q''')Vdir

      XYZ D3Quuv = D1Qv.Multiplied (-Cos(U));
      D3Quuv.Add (VcrossD1Qv.Multiplied (-Sin(U)));      
      D3Quuv.Add (VdotD1Qv.Multiplied (Cos(U)));
      D3UUV.SetXYZ (D3Quuv);

      XYZ D2Quv = D1Qv.Multiplied (-Sin(U));
      D2Quv.Add (VcrossD1Qv.Multiplied (Cos(U)));      
      D2Quv.Add (VdotD1Qv.Multiplied (Sin(U)));
      D2UV.SetXYZ (D2Quv);

      D1Qv.Multiply (Cos(U));
      VcrossD1Qv.Multiply (Sin(U));
      VdotD1Qv.Multiply (1.0 - Cos(U));
      D1Qv.Add (VcrossD1Qv);
      D1Qv.Add (VdotD1Qv);
      D1V.SetXYZ (D1Qv);

      XYZ D3Qvvu = D2Qv.Multiplied (-Sin(U));
      D3Qvvu.Add (VcrossD2Qv.Multiplied (Cos(U)));
      D3Qvvu.Add (VdotD2Qv.Multiplied (Sin(U)));
      D3UVV.SetXYZ (D3Qvvu);
      
      VcrossD2Qv.Multiply (Sin(U));
      VdotD2Qv.Multiply (1.0 - Cos(U));
      VdotD2Qv.Add (VcrossD2Qv);
      D2Qv.Multiply (Cos(U));
      D2Qv.Add (VdotD2Qv);
      D2V.SetXYZ (D2Qv);

      VcrossD3Qv.Multiply (Sin(U));
      VdotD3Qv.Multiply (1.0 - Cos(U));
      VdotD3Qv.Add (VcrossD2Qv);
      D3Qv.Multiply (Cos(U));
      D3Qv.Add (VdotD3Qv);
      D3V.SetXYZ (D3Qv);

      XYZ D1Qu = Q.Multiplied (- Sin(U));
      D1Qu.Add (VcrossCQ.Multiplied (Cos(U)));
      XYZ D3Qu = D1Qu.Multiplied (-1.0);
      D1Qu.Add (VdotCQ.Multiplied (Sin(U)));
      D3Qu.Add (VdotCQ.Multiplied (-Sin(U)));
      D1U.SetXYZ (D1Qu);
      D3U.SetXYZ (D3Qu);

      Q.Multiply (Cos(U));
      VcrossCQ.Multiply (Sin(U));
      Q.Add (VcrossCQ);   
      XYZ D2Qu = Q.Multiplied(-1.0);
      D2Qu.Add (VdotCQ.Multiplied (Cos(U)));
      D2U.SetXYZ (D2Qu);
      VdotCQ.Multiply (1.0-Cos(U));
      Q.Add (VdotCQ);   
      Q.Add (C);
      P.SetXYZ (Q);      
}


//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

Vec Geom_SurfaceOfRevolution::DN (const Standard_Real    U , const Standard_Real    V, 
                                  const Standard_Integer Nu, const Standard_Integer Nv) const {

   Standard_RangeError_Raise_if (Nu + Nv < 1 || Nu < 0 || Nv < 0, " ");
   if (Nu == 0) {
     XYZ Vn = (basisCurve->DN (V, Nv)).XYZ();
     XYZ Vdir = direction.XYZ();
     XYZ VDot = Vdir.Multiplied (Vn.Dot (Vdir));
     VDot.Multiply (1-Cos(U));
     XYZ VCross = Vdir.Crossed (Vn);
     VCross.Multiply (Sin(U));
     Vn.Multiply (Cos(U));
     Vn.Add (VDot);
     Vn.Add (VCross);     
     return Vec (Vn);
   }
   else if (Nv == 0) {
     XYZ CQ = (basisCurve->Value (V)).XYZ() - loc.XYZ();
     XYZ Vdir = direction.XYZ();
     XYZ VDot = Vdir.Multiplied (CQ.Dot (Vdir));
     XYZ VCross = Vdir.Crossed (CQ);
     if ((Nu + 6) % 4 == 0) {
       CQ.Multiply (-Cos (U));
       VDot.Multiply (Cos(U));
       VCross.Multiply (-Sin(U));
     }
     else if ((Nu + 5) % 4 == 0) {
       CQ.Multiply (Sin (U));
       VDot.Multiply (-Sin(U));
       VCross.Multiply (-Cos(U));
     }
     else if ((Nu+3) % 4 == 0) {
       CQ.Multiply (-Sin (U));
       VDot.Multiply (+Sin(U));
       VCross.Multiply (Cos(U));
     }
     else if (Nu+4 % 4 == 0) {
       CQ.Multiply (Cos (U));
       VDot.Multiply (-Cos(U));
       VCross.Multiply (Sin(U));
     }
     CQ.Add (VDot);
     CQ.Add (VCross);
     return Vec (CQ);
   }
   else {
     XYZ Vn = (basisCurve->DN (V, Nv)).XYZ();
     XYZ Vdir = direction.XYZ();
     XYZ VDot = Vdir.Multiplied (Vn.Dot (Vdir));
     XYZ VCross = Vdir.Crossed (Vn);
     if ((Nu + 6) % 4 == 0) {
       Vn.Multiply (-Cos (U));
       VDot.Multiply (Cos(U));
       VCross.Multiply (-Sin(U));
     }
     else if ((Nu + 5) % 4 == 0) {
       Vn.Multiply (Sin (U));
       VDot.Multiply (-Sin(U));
       VCross.Multiply (-Cos(U));
     }
     else if ((Nu+3) % 4 == 0) {
       Vn.Multiply (-Sin (U));
       VDot.Multiply (+Sin(U));
       VCross.Multiply (Cos(U));
     }
     else if (Nu+4 % 4 == 0) {
       Vn.Multiply (Cos (U));
       VDot.Multiply (-Cos(U));
       VCross.Multiply (Sin(U));
     }
     Vn.Add (VDot);
     Vn.Add (VCross);     
     return Vec (Vn);
   }
}

//=======================================================================
//function : LocalD0
//purpose  : 
//=======================================================================

void Geom_SurfaceOfRevolution::LocalD0 (const Standard_Real    U,
				   const Standard_Real    V, 
				   const Standard_Integer VSide,
				         gp_Pnt&          P     )  const
{
  if((VSide !=0 ) && basisCurve->IsKind(STANDARD_TYPE(Geom_BSplineCurve))) 
    { 
      gp_Vec D1V,D2V,D3V;
      Handle( Geom_BSplineCurve) BSplC;
      BSplC= Handle(Geom_BSplineCurve)::DownCast(basisCurve);

	  LocateSide(V,VSide,BSplC,0,P,D1V,D2V,D3V);
	  XYZ Q  = P.XYZ();                               //Q
	  XYZ C  = loc.XYZ();                              //C
	  Q.Subtract(C);                                   //CQ
	  XYZ Vdir     = direction.XYZ();                  //Vdir
	  XYZ VcrossCQ = Vdir.Crossed (Q);                 //Vdir^CQ
	  VcrossCQ.Multiply (Sin(U));                      //(Vdir^CQ)*Sin(U)
	  XYZ VdotCQ =
	    Vdir.Multiplied ((Vdir.Dot(Q))*(1.0 - Cos(U)));//(CQ.Vdir)(1-Cos(U))Vdir
	  VdotCQ.Add (VcrossCQ);                           //addition des composantes
	  Q.Multiply (Cos(U));
	  Q.Add (VdotCQ);
	  Q.Add (C);
	  P.SetXYZ(Q);
    }
  else  D0(U,V,P);
}

//=======================================================================
//function : LocalD1
//purpose  : 
//=======================================================================

void Geom_SurfaceOfRevolution::LocalD1 (const Standard_Real    U, 
				   const Standard_Real    V,
				   const Standard_Integer VSide, 
				         gp_Pnt&          P,
				         gp_Vec&          D1U, 
				         gp_Vec&          D1V)     const
{
 if((VSide !=0 ) && basisCurve->IsKind(STANDARD_TYPE(Geom_BSplineCurve))) 
   {
     Handle( Geom_BSplineCurve) BSplC;
     BSplC= Handle(Geom_BSplineCurve)::DownCast(basisCurve);
         Vec D2V,D3V; 
	 Vec V1; 
	 LocateSide(V,VSide,BSplC,1,P,V1,D2V,D3V);
	 XYZ Q    = P.XYZ();                               //Q
	 XYZ DQv  = V1.XYZ();                               //Q'
	 XYZ C    = loc.XYZ();                              //C
	 XYZ Vdir = direction.XYZ();                        //Vdir
	 Q.Subtract(C);                                     //CQ
	 XYZ VcrossCQ  = Vdir.Crossed (Q);                  //Vdir^CQ
      // If the point is placed on the axis of revolution then derivatives on U are undefined.
      // Manually set them to zero.
      if (VcrossCQ.SquareModulus() < Precision::SquareConfusion())
        VcrossCQ.SetCoord(0.0, 0.0, 0.0);

	 XYZ VcrossDQv = Vdir.Crossed (DQv);                //(Vdir^Q')
	 XYZ VdotCQ    = Vdir.Multiplied (Vdir.Dot(Q));     //(Vdir.CQ)Vdir
	 XYZ VdotDQv   = Vdir.Multiplied (Vdir.Dot(DQv));   //(Vdir.Q')Vdir
	 
	 VcrossDQv.Multiply (Sin(U));
	 VdotDQv.Multiply   (1.0 - Cos(U));
	 VdotDQv.Add        (VcrossDQv);
	 DQv.Multiply       (Cos(U));
	 DQv.Add            (VdotDQv);
	 D1V.SetXYZ         (DQv);
	 
	 XYZ DQu = Q.Multiplied       (-Sin(U));
	 DQu.Add (VcrossCQ.Multiplied (Cos(U)));
	 DQu.Add (VdotCQ.Multiplied (Sin(U)));
	 D1U.SetXYZ (DQu);
	 
	 Q.Multiply (Cos(U));
	 Q.Add      (C);
	 VcrossCQ.Multiply (Sin(U));
	 Q.Add             (VcrossCQ);   
	 VdotCQ.Multiply (1.0-Cos(U));
	 Q.Add           (VdotCQ);   
	 P.SetXYZ (Q);  
   }else 
     D1(U,V,P,D1U,D1V);
}

//=======================================================================
//function : LocalD2
//purpose  : 
//=======================================================================

void Geom_SurfaceOfRevolution::LocalD2 (const Standard_Real    U,
				   const Standard_Real    V,
				   const Standard_Integer VSide,
				         gp_Pnt&          P,
				         gp_Vec&          D1U,
				         gp_Vec&          D1V,
				         gp_Vec&          D2U,
				         gp_Vec&          D2V,
				         gp_Vec&          D2UV) const
{
  if((VSide !=0 ) && basisCurve->IsKind(STANDARD_TYPE(Geom_BSplineCurve))) 
    {
      Handle( Geom_BSplineCurve) BSplC;
      BSplC= Handle(Geom_BSplineCurve)::DownCast(basisCurve);
	  Vec d3v;
	  LocateSide(V,VSide,BSplC,2,P,D1V,D2V,d3v);
	  XYZ Q     = P.XYZ();                                   //Q
	  XYZ D1Qv  = D1V.XYZ();                                 //Q'
	  XYZ D2Qv  = D2V.XYZ();                                 //Q"
	  XYZ C     = loc.XYZ();                                 //C
	  XYZ Vdir  = direction.XYZ();                           //Vdir
	  Q.Subtract(C);                                         //CQ
	  XYZ VcrossCQ   = Vdir.Crossed (Q);                     //Vdir^CQ
      // If the point is placed on the axis of revolution then derivatives on U are undefined.
      // Manually set them to zero.
      if (VcrossCQ.SquareModulus() < Precision::SquareConfusion())
        VcrossCQ.SetCoord(0.0, 0.0, 0.0);

	  XYZ VcrossD1Qv = Vdir.Crossed (D1Qv);                  //(Vdir^Q')
	  XYZ VcrossD2Qv = Vdir.Crossed (D2Qv);                  //(Vdir^Q")
	  XYZ VdotCQ     = Vdir.Multiplied (Vdir.Dot(Q));        //(Vdir.CQ)Vdir
	  XYZ VdotD1Qv   = Vdir.Multiplied (Vdir.Dot(D1Qv));     //(Vdir.Q')Vdir
	  XYZ VdotD2Qv   = Vdir.Multiplied (Vdir.Dot(D2Qv));     //(Vdir.Q")Vdir
	  
	  
	  XYZ D2Quv = D1Qv.Multiplied(-Sin(U));
	  D2Quv.Add (VcrossD1Qv.Multiplied (Cos(U)));      
	  D2Quv.Add (VdotD1Qv.Multiplied (Sin(U)));
	  D2UV.SetXYZ (D2Quv);
	  
	  D1Qv.Multiply       (Cos(U));
	  VcrossD1Qv.Multiply (Sin(U));
	  VdotD1Qv.Multiply   (1.0 - Cos(U));
	  D1Qv.Add            (VcrossD1Qv);
	  D1Qv.Add            (VdotD1Qv);
	  D1V.SetXYZ          (D1Qv);
	  
	  VcrossD2Qv.Multiply (Sin(U));
	  VdotD2Qv.Multiply (1.0 - Cos(U));
	  VdotD2Qv.Add (VcrossD2Qv);
	  D2Qv.Multiply (Cos(U));
	  D2Qv.Add (VdotD2Qv);
	  D2V.SetXYZ (D2Qv);

	  XYZ D1Qu = Q.Multiplied (-Sin(U));
	  D1Qu.Add (VcrossCQ.Multiplied (Cos(U)));
	  D1Qu.Add (VdotCQ.Multiplied (Sin(U)));
	  D1U.SetXYZ (D1Qu);
	  
	  Q.Multiply (Cos(U));
	  VcrossCQ.Multiply (Sin(U));
	  Q.Add (VcrossCQ);   
	  XYZ D2Qu = Q.Multiplied(-1.0);
	  D2Qu.Add (VdotCQ.Multiplied (Cos(U)));
	  D2U.SetXYZ (D2Qu);
	  VdotCQ.Multiply (1.0-Cos(U));
	  Q.Add (VdotCQ);   
	  Q.Add (C);
	  P.SetXYZ (Q);      
    } 
  else
    D2(U,V,P,D1U,D1V,D2U,D2V,D2UV);
}
//=======================================================================
//function : LocalD3
//purpose  : 
//=======================================================================

void Geom_SurfaceOfRevolution::LocalD3 (const Standard_Real    U, 
				   const Standard_Real    V,
				   const Standard_Integer VSide, 
				         gp_Pnt&          P,
				         gp_Vec&          D1U,
				         gp_Vec&          D1V, 
				         gp_Vec&          D2U, 
				         gp_Vec&          D2V, 
				         gp_Vec&          D2UV, 
				         gp_Vec&          D3U,
				         gp_Vec&          D3V,
				         gp_Vec&          D3UUV,
				         gp_Vec&          D3UVV) const
{
  if((VSide !=0 ) && basisCurve->IsKind(STANDARD_TYPE(Geom_BSplineCurve))) 
    {
      Handle( Geom_BSplineCurve) BSplC;
      BSplC= Handle(Geom_BSplineCurve)::DownCast(basisCurve);

	  LocateSide(V,VSide,BSplC,3,P,D1V,D2V,D3V);
	  XYZ Q     = P.XYZ();                                   //Q
	  XYZ D1Qv  = D1V.XYZ();                                 //Q'
	  XYZ D2Qv  = D2V.XYZ();                                 //Q"
	  XYZ D3Qv  = D3V.XYZ();                                 //Q'''
	  XYZ C     = loc.XYZ();                                //C
	  XYZ Vdir  = direction.XYZ();                          //Vdir
	  Q.Subtract(C);                                        //CQ
	  XYZ VcrossCQ   = Vdir.Crossed (Q);                    //Vdir^CQ
      // If the point is placed on the axis of revolution then derivatives on U are undefined.
      // Manually set them to zero.
      if (VcrossCQ.SquareModulus() < Precision::SquareConfusion())
        VcrossCQ.SetCoord(0.0, 0.0, 0.0);

	  XYZ VcrossD1Qv = Vdir.Crossed (D1Qv);                 //(Vdir^Q')
	  XYZ VcrossD2Qv = Vdir.Crossed (D2Qv);                 //(Vdir^Q")
	  XYZ VcrossD3Qv = Vdir.Crossed (D3Qv);                 //(Vdir^Q''')
	  XYZ VdotCQ     = Vdir.Multiplied (Vdir.Dot(Q));       //(Vdir.CQ)Vdir
	  XYZ VdotD1Qv   = Vdir.Multiplied (Vdir.Dot(D1Qv));    //(Vdir.Q')Vdir
	  XYZ VdotD2Qv   = Vdir.Multiplied (Vdir.Dot(D2Qv));    //(Vdir.Q")Vdir
	  XYZ VdotD3Qv   = Vdir.Multiplied (Vdir.Dot(D3Qv));    //(Vdir.Q''')Vdir

	  XYZ D3Quuv = D1Qv.Multiplied (-Cos(U));
	  D3Quuv.Add (VcrossD1Qv.Multiplied (-Sin(U)));      
	  D3Quuv.Add (VdotD1Qv.Multiplied (Cos(U)));
	  D3UUV.SetXYZ (D3Quuv);
	  
	  XYZ D2Quv = D1Qv.Multiplied (-Sin(U));
	  D2Quv.Add (VcrossD1Qv.Multiplied (Cos(U)));      
	  D2Quv.Add (VdotD1Qv.Multiplied (Sin(U)));
	  D2UV.SetXYZ (D2Quv);
	  
	  D1Qv.Multiply (Cos(U));
	  VcrossD1Qv.Multiply (Sin(U));
	  VdotD1Qv.Multiply (1.0 - Cos(U));
	  D1Qv.Add (VcrossD1Qv);
	  D1Qv.Add (VdotD1Qv);
	  D1V.SetXYZ (D1Qv);

	  XYZ D3Qvvu = D2Qv.Multiplied (-Sin(U));
	  D3Qvvu.Add (VcrossD2Qv.Multiplied (Cos(U)));
	  D3Qvvu.Add (VdotD2Qv.Multiplied (Sin(U)));
	  D3UVV.SetXYZ (D3Qvvu);
	  
	  VcrossD2Qv.Multiply (Sin(U));
	  VdotD2Qv.Multiply (1.0 - Cos(U));
	  VdotD2Qv.Add (VcrossD2Qv);
	  D2Qv.Multiply (Cos(U));
	  D2Qv.Add (VdotD2Qv);
	  D2V.SetXYZ (D2Qv);
	  
	  VcrossD3Qv.Multiply (Sin(U));
	  VdotD3Qv.Multiply (1.0 - Cos(U));
	  VdotD3Qv.Add (VcrossD2Qv);
	  D3Qv.Multiply (Cos(U));
	  D3Qv.Add (VdotD3Qv);
	  D3V.SetXYZ (D3Qv);
	  
	  XYZ D1Qu = Q.Multiplied (- Sin(U));
	  D1Qu.Add (VcrossCQ.Multiplied (Cos(U)));
	  XYZ D3Qu = D1Qu.Multiplied (-1.0);
	  D1Qu.Add (VdotCQ.Multiplied (Sin(U)));
	  D3Qu.Add (VdotCQ.Multiplied (-Sin(U)));
	  D1U.SetXYZ (D1Qu);
	  D3U.SetXYZ (D3Qu);
	  
	  Q.Multiply (Cos(U));
	  VcrossCQ.Multiply (Sin(U));
	  Q.Add (VcrossCQ);   
	  XYZ D2Qu = Q.Multiplied(-1.0);
	  D2Qu.Add (VdotCQ.Multiplied (Cos(U)));
	  D2U.SetXYZ (D2Qu);
	  VdotCQ.Multiply (1.0-Cos(U));
	  Q.Add (VdotCQ);   
	  Q.Add (C);
	  P.SetXYZ (Q); 
    }
  else
    D3(U,V,P,D1U,D1V,D2U,D2V,D2UV,D3U,D3V,D3UUV,D3UVV);
}
//=======================================================================
//function : LocalDN
//purpose  : 
//=======================================================================

gp_Vec Geom_SurfaceOfRevolution::LocalDN  (const Standard_Real    U, 
				      const Standard_Real    V,
				      const Standard_Integer VSide,
				      const Standard_Integer Nu,
				      const Standard_Integer Nv) const
{
  Standard_RangeError_Raise_if (Nu + Nv < 1 || Nu < 0 || Nv < 0, " "); 
  XYZ Vn, CQ;
  if (Nu == 0) {
    if((VSide !=0 ) && basisCurve->IsKind(STANDARD_TYPE(Geom_BSplineCurve))) 
      {
	Handle( Geom_BSplineCurve) BSplC;
	BSplC= Handle(Geom_BSplineCurve)::DownCast(basisCurve);  
	Vn = LocateSideN(V,VSide,BSplC,Nv).XYZ();
      }else
	return DN(U,V,Nu,Nv);
    XYZ Vdir = direction.XYZ();
    XYZ VDot = Vdir.Multiplied (Vn.Dot (Vdir));
    VDot.Multiply (1-Cos(U));
    XYZ VCross = Vdir.Crossed (Vn);
    VCross.Multiply (Sin(U));
    Vn.Multiply (Cos(U));
    Vn.Add (VDot);
    Vn.Add (VCross);     
    return Vec (Vn);
  }
  
  else if (Nv == 0) {
    if((VSide !=0 ) && basisCurve->IsKind(STANDARD_TYPE(Geom_BSplineCurve))) 
      {
	Handle( Geom_BSplineCurve) BSplC;
	BSplC= Handle(Geom_BSplineCurve)::DownCast(basisCurve);
	CQ = LocateSideN(V,VSide,BSplC,Nv).XYZ() - loc.XYZ();
      }else
	return DN(U,V,Nu,Nv);
    XYZ Vdir = direction.XYZ();
    XYZ VDot = Vdir.Multiplied (CQ.Dot (Vdir));
    XYZ VCross = Vdir.Crossed (CQ);
    if ((Nu + 6) % 4 == 0) {
      CQ.Multiply (-Cos (U));
      VDot.Multiply (Cos(U));
      VCross.Multiply (-Sin(U));
    }
    else if ((Nu + 5) % 4 == 0) {
      CQ.Multiply (Sin (U));
      VDot.Multiply (-Sin(U));
      VCross.Multiply (-Cos(U));
    }
    else if ((Nu+3) % 4 == 0) {
      CQ.Multiply (-Sin (U));
      VDot.Multiply (+Sin(U));
      VCross.Multiply (Cos(U));
    }
    else if (Nu+4 % 4 == 0) {
      CQ.Multiply (Cos (U));
      VDot.Multiply (-Cos(U));
      VCross.Multiply (Sin(U));
    }
    CQ.Add (VDot);
    CQ.Add (VCross);
    return Vec (CQ);
  }
  
  else {
    if((VSide !=0 ) && basisCurve->IsKind(STANDARD_TYPE(Geom_BSplineCurve))) 
      {
	Handle( Geom_BSplineCurve) BSplC;
	BSplC= Handle(Geom_BSplineCurve)::DownCast(basisCurve);
	Vn = LocateSideN(V,VSide,BSplC,Nv).XYZ();
      }else 
	return DN(U,V,Nu,Nv);
    XYZ Vdir = direction.XYZ();
    XYZ VDot = Vdir.Multiplied (Vn.Dot (Vdir));
    XYZ VCross = Vdir.Crossed (Vn);
    if ((Nu + 6) % 4 == 0) {
      Vn.Multiply (-Cos (U));
      VDot.Multiply (Cos(U));
      VCross.Multiply (-Sin(U));
    }
    else if ((Nu + 5) % 4 == 0) {
      Vn.Multiply (Sin (U));
      VDot.Multiply (-Sin(U));
      VCross.Multiply (-Cos(U));
    }
    else if ((Nu+3) % 4 == 0) {
      Vn.Multiply (-Sin (U));
      VDot.Multiply (+Sin(U));
      VCross.Multiply (Cos(U));
    }
    else if (Nu+4 % 4 == 0) {
      Vn.Multiply (Cos (U));
      VDot.Multiply (-Cos(U));
      VCross.Multiply (Sin(U));
    }
    Vn.Add (VDot);
    Vn.Add (VCross);     
    return Vec (Vn);
  }
}

//=======================================================================
//function : ReferencePlane
//purpose  : 
//=======================================================================

Ax2 Geom_SurfaceOfRevolution::ReferencePlane() const {
        
   Standard_NotImplemented::Raise ();
   return gp_Ax2();
}


//=======================================================================
//function : UIso
//purpose  : 
//=======================================================================

Handle(Geom_Curve) Geom_SurfaceOfRevolution::UIso (const Standard_Real U) const {

   Handle(Geom_Curve) C = Handle(Geom_Curve)::DownCast(basisCurve->Copy());
   Ax1 RotAxis = Ax1 (loc, direction);
   C->Rotate (RotAxis, U);
   return C;
}


//=======================================================================
//function : VIso
//purpose  : 
//=======================================================================

Handle(Geom_Curve) Geom_SurfaceOfRevolution::VIso (const Standard_Real V) const {

  Handle(Geom_Circle) Circ;
  Pnt Pc = basisCurve->Value (V);
  gp_Lin L1(loc,direction);
  Standard_Real Rad= L1.Distance(Pc);

  Ax2 Rep ;
  if ( Rad > gp::Resolution()) { 
    XYZ P  = Pc.XYZ(); 
    XYZ C;
    C.SetLinearForm((P-loc.XYZ()).Dot(direction.XYZ()), 
		    direction.XYZ(), loc.XYZ() );
    P = P-C;
    if(P.Modulus() > gp::Resolution()) {
      gp_Dir D = P.Normalized();
      Rep = gp_Ax2(C, direction, D);
    }
    else 
      Rep = gp_Ax2(C, direction);
  }
  else
    Rep = gp_Ax2(Pc, direction);

  Circ   = new Geom_Circle (Rep, Rad);
  return Circ;
}


//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================

void Geom_SurfaceOfRevolution::Transform (const Trsf& T) {

  loc.Transform (T);
  direction.Transform (T);
  basisCurve->Transform (T);
  if(T.ScaleFactor()*T.HVectorialPart().Determinant() < 0.) UReverse(); 
}

//=======================================================================
//function : TransformParameters
//purpose  : 
//=======================================================================

void Geom_SurfaceOfRevolution::TransformParameters(Standard_Real& ,
						   Standard_Real& V,
						   const gp_Trsf& T) 
const
{
  V = basisCurve->TransformedParameter(V,T);
}

//=======================================================================
//function : ParametricTransformation
//purpose  : 
//=======================================================================

gp_GTrsf2d Geom_SurfaceOfRevolution::ParametricTransformation
(const gp_Trsf& T) const
{
  gp_GTrsf2d T2;
  gp_Ax2d Axis(gp::Origin2d(),gp::DX2d());
  T2.SetAffinity(Axis, basisCurve->ParametricTransformation(T));
  return T2;
}

