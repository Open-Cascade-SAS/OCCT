// Created on: 1997-07-11
// Created by: Philippe MANGIN
// Copyright (c) 1997-1999 Matra Datavision
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
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <GeomFill.hxx>
#include <GeomFill_CircularBlendFunc.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_SequenceOfReal.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomFill_CircularBlendFunc,Approx_SweepFunction)

#if DRAW
#include <GeomAdaptor_HCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <DrawTrSurf.hxx>
static Standard_Integer NbSections = 0;
#endif

const Standard_Real TolAng = 1.e-6;

GeomAbs_Shape GeomFillNextShape(const GeomAbs_Shape S)
{
  switch (S) {
  case GeomAbs_C0 :
    return  GeomAbs_C1;
  case GeomAbs_C1 :
    return  GeomAbs_C2;
  case GeomAbs_C2 :
    return  GeomAbs_C3;
  case GeomAbs_C3 :
    return  GeomAbs_CN;
  default :
    return  GeomAbs_CN;
  }
}

void GeomFillFusInt(const TColStd_Array1OfReal& I1,
		    const TColStd_Array1OfReal& I2,
		    TColStd_SequenceOfReal& Seq)
{
  Standard_Integer ind1=1, ind2=1;
  Standard_Real    Epspar = Precision::PConfusion()*0.99;
  // en suposant que le positionement fonctionne a PConfusion()/2
  Standard_Real    v1, v2;
// Initialisations : les IND1 et IND2 pointent sur le 1er element
// de chacune des 2 tables a traiter.


//--- On remplit TABSOR en parcourant TABLE1 et TABLE2 simultanement ---
//------------------ en eliminant les occurrences multiples ------------

 while ((ind1<=I1.Upper()) && (ind2<=I2.Upper())) {
      v1 = I1(ind1);
      v2 = I2(ind2);
      if (Abs(v1-v2)<= Epspar) {
// Ici les elements de I1 et I2 conviennent .
         Seq.Append((v1+v2)/2);
	 ind1++;
         ind2++;
       }
      else if (v1 < v2) {
	// Ici l' element de I1 convient.
         Seq.Append(v1);
         ind1++;
       }
      else {
// Ici l' element de TABLE2 convient.
	 Seq.Append(v2);
	 ind2++;
       }
    }

  if (ind1>I1.Upper()) { 
//----- Ici I1 est epuise, on complete avec la fin de TABLE2 -------

    for (; ind2<=I2.Upper(); ind2++) {
      Seq.Append(I2(ind2));
    }
  }

  if (ind2>I2.Upper()) { 
//----- Ici I2 est epuise, on complete avec la fin de I1 -------

    for (; ind1<=I1.Upper(); ind1++) {
      Seq.Append(I1(ind1));
    }
  }
}


GeomFill_CircularBlendFunc::
GeomFill_CircularBlendFunc(const Handle(Adaptor3d_HCurve)& Path,
			   const Handle(Adaptor3d_HCurve)& Curve1,
			   const Handle(Adaptor3d_HCurve)& Curve2,
			   const Standard_Real Radius,
			   const Standard_Boolean Polynomial) 
                           : maxang(RealFirst()), 
			     minang(RealLast()),
			     distmin(RealLast())
{
  // Recopie des arguments
  myPath = myTPath = Path;
  myCurve1 = myTCurve1 = Curve1;
  myCurve2 = myTCurve2 = Curve2;
  myRadius =  Radius;

  // Estimations numeriques
  Discret();

  // Type de convertion ?
  if (Polynomial) myTConv=Convert_Polynomial;
  else if(maxang > 0.65*M_PI) 
    myTConv=Convert_QuasiAngular; //car c'est Continue
  else   myTConv = Convert_TgtThetaOver2; 
                  //car c'est le plus performant 

  // On en deduit la structure 
  GeomFill::GetShape(maxang, 
		     myNbPoles, myNbKnots, 
		     myDegree, myTConv); 
}

void GeomFill_CircularBlendFunc::Discret()
{
  Standard_Real TFirst =  myPath->FirstParameter();
  Standard_Real TLast =  myPath->LastParameter(), T;
  Standard_Integer ii;
  Standard_Real L1, L2, L;
  Handle(Adaptor3d_HCurve) C;
  gp_Pnt P1, P2, P3, Center;
  gp_Vec DCenter;
 
  P1 = myCurve1->Value(TFirst);
  P2 = myCurve1->Value((TFirst+TLast)/2.);
  P3 = myCurve1->Value(TLast);
  L1 = P1.Distance(P2) + P2.Distance(P3);

  P1 = myCurve2->Value(TFirst);
  P2 = myCurve2->Value((TFirst+TLast)/2.);
  P3 = myCurve2->Value(TLast);
  L2 = P1.Distance(P2) + P2.Distance(P3);

  if (L1>L2) { 
    L = L1;
    C = myCurve1;
  }
  else {
   L = L2;
   C = myCurve2;    
  }

  Standard_Real Fleche = 1.e-2 * L;
  Standard_Real Angle, Cosa, Percent;
  GCPnts_QuasiUniformDeflection Samp;
  Samp.Initialize(C->GetCurve(), Fleche);
  myBary.SetCoord(0.,0.,0.);
  gp_Vec ns1, ns2;

  if (Samp.IsDone()) {
    Percent = ((Standard_Real)1)/(2*Samp.NbPoints());
//    char name[100];
    for (ii=1; ii<=Samp.NbPoints(); ii++) {
      T = Samp.Parameter(ii); 
      myCurve1->D0(T, P1);
      myCurve2->D0(T, P2);
/*
      sprintf(name,"PNT_%d",NbSections++);
      DrawTrSurf::Set(name, P1);
      sprintf(name,"PNT_%d",NbSections++);
      DrawTrSurf::Set(name, P2);*/
      myPath->D0(T, Center);      
      ns1.SetXYZ( Center.XYZ() - P1.XYZ());
      ns2.SetXYZ( Center.XYZ() - P2.XYZ());
      ns1.Normalize();
      ns2.Normalize();
      Cosa = ns1.Dot(ns2);
      if(Cosa > 1.) {Cosa = 1.;}
      Angle = Abs(ACos(Cosa));
      if (Angle>maxang) maxang = Angle;
      if (Angle<minang) minang = Angle;
      distmin = Min( distmin, P1.Distance(P2));
      myBary.ChangeCoord() += (P1.XYZ()+P2.XYZ());
    }
  }
  else {  
    Standard_Real Delta = (TLast-TFirst)/20;
    Percent = ((Standard_Real)1)/42;
    for (ii=0, T=TFirst; ii<=20; ii++, T+=Delta) {
      myCurve1->D0(T, P1);
      myCurve2->D0(T, P2); 
      myPath->D0(T, Center); 
     
      ns1.SetXYZ( Center.XYZ() - P1.XYZ());
      ns2.SetXYZ( Center.XYZ() - P2.XYZ());
      ns1.Normalize();
      ns2.Normalize();
      Cosa = ns1.Dot(ns2);
      if(Cosa > 1.) Cosa = 1.;
      Angle = Abs(ACos(Cosa));

      if (Angle>maxang) maxang = Angle;
      if (Angle<minang) minang = Angle;
      distmin = Min( distmin, P1.Distance(P2));
      myBary.ChangeCoord() += (P1.XYZ()+P2.XYZ());
    }
  } 
  myBary.ChangeCoord() *= Percent;

  // Faut il inverser la trajectoire ?
  T = (TFirst + TLast)/2;
  myCurve1->D0(T, P1);
  myCurve2->D0(T, P2);
  myPath->D1(T, Center, DCenter);
 
  ns1.SetXYZ( Center.XYZ() - P1.XYZ());
  ns2.SetXYZ( Center.XYZ() - P2.XYZ());     

  //myreverse = (DCenter.Dot(ns1.Crossed(ns2)) < 0);
  myreverse = Standard_False;
}



Standard_Boolean GeomFill_CircularBlendFunc::D0(const Standard_Real Param,
						const Standard_Real,
						const Standard_Real,
						TColgp_Array1OfPnt& Poles,
						TColgp_Array1OfPnt2d&,
						TColStd_Array1OfReal& Weigths) 
{
  gp_Pnt P1, P2, Center;
  gp_Vec ns1, ns2, nplan;
  gp_XYZ temp;

// Positionnement
  myTPath->D0(Param, Center);
  myTCurve1->D0(Param, P1);
  myTCurve2->D0(Param, P2);
  ns1.SetXYZ( Center.XYZ() - P1.XYZ());
  ns2.SetXYZ( Center.XYZ() - P2.XYZ());
  if (!ns1.IsParallel(ns2, TolAng))  nplan = ns1.Crossed(ns2);
  else {
    myTPath->D1(Param, Center, nplan);
    if (myreverse) nplan.Reverse();
  }

// Normalisation
  ns1.Normalize();
  ns2.Normalize();
  nplan.Normalize(); 

  temp.SetLinearForm(myRadius, ns1.XYZ(),
		     myRadius, ns2.XYZ(),
		     1, P1.XYZ(),
		     P2.XYZ());
  Center.ChangeCoord() = 0.5*temp;
    
  // Section
  GeomFill::GetCircle(myTConv, 
		      ns1, ns2, nplan,
		      P1, P2, 
		      myRadius, Center,
		      Poles,  Weigths);  

#if DRAW
//  Handle(Geom_BSplineCurve) BS = 
//    new Geom_BSplineCurve(Poles,Weights,Knots,Mults,Degree);
//  sprintf(name,"SECT_%d",NbSections++);
//  DrawTrSurf::Set(name,BS);
#endif
  return Standard_True;
}


Standard_Boolean GeomFill_CircularBlendFunc::D1(const Standard_Real Param,
//						const Standard_Real First,
						const Standard_Real ,
//						const Standard_Real Last,
						const Standard_Real ,
						TColgp_Array1OfPnt& Poles,
						TColgp_Array1OfVec& DPoles,
//						TColgp_Array1OfPnt2d& Poles2d,
						TColgp_Array1OfPnt2d& ,
//						TColgp_Array1OfVec2d& DPoles2d,
						TColgp_Array1OfVec2d& ,
						TColStd_Array1OfReal& Weigths,
						TColStd_Array1OfReal& DWeigths) 
{
  gp_Pnt P1, P2, Center;
  Standard_Real invnorm1, invnorm2, invnormp;
//  gp_Vec DCenter, D2Center, nplan, dnplan, DP1, DP2;
  gp_Vec DCenter, nplan, dnplan, DP1, DP2;
//  gp_Vec ns1, ns2, Dns1, Dns2, vtmp;
  gp_Vec ns1, ns2, Dns1, Dns2;
  gp_XYZ temp;

// Positionemment
  myTPath  ->D1(Param, Center, DCenter);
  myTCurve1->D1(Param, P1, DP1);
  myTCurve2->D1(Param, P2, DP2);

  ns1.SetXYZ( Center.XYZ() - P1.XYZ());
  ns2.SetXYZ( Center.XYZ() - P2.XYZ());
  Dns1 = DCenter - DP1;
  Dns2 = DCenter - DP2;
 
  if (!ns1.IsParallel(ns2, TolAng)) {
    nplan = ns1.Crossed(ns2);
    dnplan =  Dns1.Crossed(ns2).Added( ns1.Crossed(Dns2));
  }
  else {
    myTPath->D2(Param, Center, nplan, dnplan);
    if (myreverse) {
      nplan.Reverse();
      dnplan.Reverse();
    }
  }

// Normalisation
  invnorm1 = ((Standard_Real) 1) / ns1.Magnitude();
  invnorm2 = ((Standard_Real) 1) / ns2.Magnitude(); 

  ns1 *= invnorm1;
  Dns1.SetLinearForm( -Dns1.Dot(ns1), ns1, Dns1);
  Dns1 *= invnorm1; 
      
  ns2 *= invnorm2;
  Dns2.SetLinearForm(-Dns2.Dot(ns2), ns2, Dns2);
  Dns2 *= invnorm2;

  temp.SetLinearForm(myRadius, ns1.XYZ(),
		     myRadius, ns2.XYZ(),
		     1, P1.XYZ(), P2.XYZ());       
  Center.ChangeCoord() = 0.5*temp;
  DCenter.SetLinearForm(myRadius, Dns1,
		        myRadius, Dns2,
			1, DP1, DP2);
  DCenter *= 0.5;
    
  invnormp = ((Standard_Real)1) / nplan.Magnitude();
  nplan *= invnormp;
  dnplan.SetLinearForm(-dnplan.Dot(nplan), nplan, dnplan);
  dnplan *= invnormp;
 
  GeomFill::GetCircle(myTConv, 
		      ns1, ns2, 
		      Dns1, Dns2,
		      nplan, dnplan,
		      P1, P2,
		      DP1, DP2,
		      myRadius, 0,
		      Center, DCenter,
		      Poles,  DPoles,
		      Weigths, DWeigths);
  return Standard_True;
}

Standard_Boolean GeomFill_CircularBlendFunc::D2(const Standard_Real Param,
//						const Standard_Real First,
						const Standard_Real ,
//						const Standard_Real Last,
						const Standard_Real ,
						TColgp_Array1OfPnt& Poles,
						TColgp_Array1OfVec& DPoles,
						TColgp_Array1OfVec& D2Poles,
//						TColgp_Array1OfPnt2d& Poles2d,
						TColgp_Array1OfPnt2d& ,
//						TColgp_Array1OfVec2d& DPoles2d,
						TColgp_Array1OfVec2d& ,
//						TColgp_Array1OfVec2d& D2Poles2d,
						TColgp_Array1OfVec2d& ,
						TColStd_Array1OfReal& Weigths,
						TColStd_Array1OfReal& DWeigths,
						TColStd_Array1OfReal& D2Weigths) 
{
  gp_Pnt P1, P2, Center;
  Standard_Real invnorm1, invnorm2, invnormp, sc;
  gp_Vec DCenter, D2Center, DP1, DP2, D2P1, D2P2;
  gp_Vec nplan, dnplan, d2nplan;
  gp_Vec ns1, ns2, Dns1, Dns2, D2ns1, D2ns2;
  gp_XYZ temp;

  // Positionement
  myTPath  ->D2(Param, Center, DCenter, D2Center);
  myTCurve1->D2(Param, P1, DP1, D2P1);
  myTCurve2->D2(Param, P2, DP2, D2P2);

  ns1.SetXYZ( Center.XYZ() - P1.XYZ());
  Dns1 = DCenter - DP1;
  D2ns1 =  D2Center - D2P1;  
  ns2.SetXYZ( Center.XYZ() - P2.XYZ());
  Dns2 = DCenter - DP2;
  D2ns2 =  D2Center - D2P2;

  if (!ns1.IsParallel(ns2, TolAng)) {
    nplan = ns1.Crossed(ns2);
    dnplan = Dns1.Crossed(ns2).Added( ns1.Crossed(Dns2));
    d2nplan.SetLinearForm(1, D2ns1.Crossed(ns2),
			  2, Dns1.Crossed(Dns2),
			  ns1.Crossed(D2ns2));
  }
  else {
    myTPath->D3(Param, Center, nplan, dnplan, d2nplan);
    if (myreverse) {
      nplan.Reverse();
      dnplan.Reverse();
      d2nplan.Reverse();
    }
  }

  // Normalisation
  invnorm1 = ((Standard_Real) 1) / ns1.Magnitude();
  invnorm2 = ((Standard_Real) 1) / ns2.Magnitude(); 

  ns1 *= invnorm1;
  sc = Dns1.Dot(ns1);
  D2ns1.SetLinearForm( 3*sc*sc*invnorm1 - D2ns1.Dot(ns1)
		      - invnorm1*Dns1.SquareMagnitude(), ns1, 
                      -2*sc*invnorm1 , Dns1,
                      D2ns1);
  Dns1.SetLinearForm( -Dns1.Dot(ns1), ns1, Dns1);
  D2ns1 *= invnorm1;
  Dns1 *= invnorm1;

  
     
  ns2 *= invnorm2;
  sc = Dns2.Dot(ns2);
  D2ns2.SetLinearForm( 3*sc*sc*invnorm2 - D2ns2.Dot(ns2)
		      - invnorm2*Dns2.SquareMagnitude(), ns2, 
                      -2*sc*invnorm2 , Dns2,
                      D2ns2);
  Dns2.SetLinearForm(-sc, ns2, Dns2);
  D2ns2 *= invnorm2;
  Dns2 *= invnorm2;


  temp.SetLinearForm(myRadius, ns1.XYZ(),
		     myRadius, ns2.XYZ(),
		     1, P1.XYZ(), P2.XYZ());       
  Center.ChangeCoord() = 0.5*temp;
  DCenter.SetLinearForm(myRadius, Dns1,
		        myRadius, Dns2,
			1, DP1, DP2);
  DCenter *= 0.5;
  D2Center.SetLinearForm(myRadius, D2ns1,
		         myRadius, D2ns2,
			 1, D2P1, D2P2);
  D2Center *= 0.5;
 
  invnormp = ((Standard_Real)1) / nplan.Magnitude();
  nplan *= invnormp;   
  sc = dnplan.Dot(nplan);
  d2nplan.SetLinearForm( 3*sc*sc*invnormp - d2nplan.Dot(nplan)
		      - invnormp*dnplan.SquareMagnitude(), nplan, 
                      -2*sc*invnormp , dnplan,
                      d2nplan); 
  dnplan.SetLinearForm(-sc, nplan, dnplan);
  dnplan *= invnormp;  
  d2nplan *= invnormp;

  GeomFill::GetCircle(myTConv, 
		      ns1, ns2, 
		      Dns1, Dns2,
		      D2ns1, D2ns2,
		      nplan, dnplan, d2nplan,
		      P1, P2,
		      DP1, DP2,
		      D2P1, D2P2,
		      myRadius, 0, 0,
		      Center, DCenter, D2Center,
		      Poles,  DPoles, D2Poles,
		      Weigths, DWeigths, D2Weigths);
  return Standard_True;
}

 Standard_Integer GeomFill_CircularBlendFunc::Nb2dCurves() const
{
  return 0;
}

void GeomFill_CircularBlendFunc::SectionShape(Standard_Integer& NbPoles,
					      Standard_Integer& NbKnots,
					      Standard_Integer& Degree) const
{
  NbPoles = myNbPoles;
  NbKnots = myNbKnots;
  Degree  = myDegree; 
}

 void GeomFill_CircularBlendFunc::Knots(TColStd_Array1OfReal& TKnots) const
{
  GeomFill::Knots(myTConv, TKnots);
}

void GeomFill_CircularBlendFunc::Mults(TColStd_Array1OfInteger& TMults) const
{
  GeomFill::Mults(myTConv, TMults);
}

Standard_Boolean GeomFill_CircularBlendFunc::IsRational() const
{
  return (myTConv != Convert_Polynomial);
}

Standard_Integer GeomFill_CircularBlendFunc::
NbIntervals(const GeomAbs_Shape S) const
{
 Standard_Integer NbI_Center, NbI_Cb1, NbI_Cb2, ii;
 NbI_Center =  myPath->NbIntervals(GeomFillNextShape(S));
 NbI_Cb1    =  myCurve1->NbIntervals(S);
 NbI_Cb2    =  myCurve2->NbIntervals(S);

 TColStd_Array1OfReal ICenter(1, NbI_Center+1);
 TColStd_Array1OfReal ICb1(1, NbI_Cb1+1);
 TColStd_Array1OfReal ICb2(1, NbI_Cb2+1);
 TColStd_SequenceOfReal    Inter;

 myPath->Intervals(ICenter, GeomFillNextShape(S));
 myCurve1->Intervals(ICb1, S);
 myCurve2->Intervals(ICb2, S);
/* cout << "Intervals : " << S << endl;
 cout << "-- Center-> " << endl;
 for (ii=1; ii<=ICenter.Length(); ii++) cout << " , "<< ICenter(ii);
 cout << endl; cout << endl;
 cout << "-- Cb1-> " << endl;
 for (ii=1; ii<=ICb1.Length(); ii++) cout << " , "<< ICb1(ii);
 cout << endl; cout << endl;
 cout << "-- Cb2-> " << endl;
 for (ii=1; ii<=ICb2.Length(); ii++) cout << " , "<< ICb1(ii);
 cout << endl; cout << endl;*/

 GeomFillFusInt(ICb1, ICb2, Inter);

 TColStd_Array1OfReal ICbs (1, Inter.Length());
 for (ii=1; ii<=ICbs.Length(); ii++) ICbs(ii) = Inter(ii);

 Inter.Clear();
 GeomFillFusInt(ICenter, ICbs, Inter);
 
 return Inter.Length()-1;  
}

void GeomFill_CircularBlendFunc::
Intervals(TColStd_Array1OfReal& T, const GeomAbs_Shape S) const
{
 Standard_Integer NbI_Center, NbI_Cb1, NbI_Cb2, ii;
 NbI_Center =  myPath->NbIntervals(GeomFillNextShape(S));
 NbI_Cb1    =  myCurve1->NbIntervals(S);
 NbI_Cb2    =  myCurve2->NbIntervals(S);

 TColStd_Array1OfReal ICenter(1, NbI_Center+1);
 TColStd_Array1OfReal ICb1(1, NbI_Cb1+1);
 TColStd_Array1OfReal ICb2(1, NbI_Cb2+1);
 TColStd_SequenceOfReal    Inter;

 myPath->Intervals(ICenter, GeomFillNextShape(S));
 myCurve1->Intervals(ICb1, S);
 myCurve2->Intervals(ICb2, S);

 GeomFillFusInt(ICb1, ICb2, Inter);

 TColStd_Array1OfReal ICbs (1, Inter.Length());
 for (ii=1; ii<=ICbs.Length(); ii++) ICbs(ii) = Inter(ii);

 Inter.Clear();
 GeomFillFusInt(ICenter, ICbs, Inter);

 // Recopie du resultat
 for (ii=1; ii<=Inter.Length(); ii++) T(ii) = Inter(ii);
}

 void GeomFill_CircularBlendFunc::SetInterval(const Standard_Real First,
					      const Standard_Real Last) 
{
  Standard_Real Eps = Precision::PConfusion();
  myTPath = myPath->Trim(First, Last, Eps);
  myTCurve1 = myCurve1->Trim(First, Last, Eps);
  myTCurve2 = myCurve2->Trim(First, Last, Eps); 
}


void GeomFill_CircularBlendFunc::GetTolerance(const Standard_Real BoundTol,
					      const Standard_Real SurfTol,
					      const Standard_Real AngleTol,
					      TColStd_Array1OfReal& Tol3d) const
{
 Standard_Integer low = Tol3d.Lower() , up=Tol3d.Upper();
 Standard_Real Tol;

 Tol= GeomFill::GetTolerance(myTConv, minang, 
			     myRadius,  AngleTol, SurfTol);
 Tol3d.Init(SurfTol);
 Tol3d(low+1) = Tol3d(up-1) = Min(Tol, SurfTol);
 Tol3d(low) = Tol3d(up) = Min(Tol, BoundTol);   
}


 void GeomFill_CircularBlendFunc::SetTolerance(const Standard_Real, 
					       const Standard_Real) 
{
 // y rien a faire !
}

 gp_Pnt GeomFill_CircularBlendFunc::BarycentreOfSurf() const
{
  return myBary;
}

 Standard_Real GeomFill_CircularBlendFunc::MaximalSection() const
{
  return maxang*myRadius;
}

void GeomFill_CircularBlendFunc::
GetMinimalWeight(TColStd_Array1OfReal& Weigths) const
{
  GeomFill::GetMinimalWeights(myTConv, minang, maxang, Weigths); 
}

