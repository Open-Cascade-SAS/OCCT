// Created on: 1998-12-14
// Created by: Joelle CHAUVET
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

// Modified:    Fri Jan  8 15:47:20 1999
//              enfin un calcul exact pour D1 et D2
//              le calcul par differ. finies est garde dans verifD1 et verifD2
// Modified:    Mon Jan 18 11:06:46 1999
//              mise au point de D1, D2 et IsConstant

#include <BSplCLib.hxx>
#include <Convert_ParameterisationType.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomConvert.hxx>
#include <GeomFill_AppSurf.hxx>
#include <GeomFill_Line.hxx>
#include <GeomFill_NSections.hxx>
#include <GeomFill_SectionGenerator.hxx>
#include <gp_Circ.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TColGeom_Array1OfCurve.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(GeomFill_NSections,GeomFill_SectionLaw)

#ifdef OCCT_DEBUG
# ifdef DRAW
#  include <DrawTrSurf.hxx>
#include <Geom_Curve.hxx>
# endif
static Standard_Boolean Affich = 0;
static Standard_Integer NbSurf = 0;
#endif

#ifdef OCCT_DEBUG
// verification des fonctions de derivation D1 et D2 par differences finies
Standard_Boolean verifD1(const TColgp_Array1OfPnt& P1,
			 const TColStd_Array1OfReal& W1,
			 const TColgp_Array1OfPnt& P2,
			 const TColStd_Array1OfReal& W2,
			 const TColgp_Array1OfVec& DPoles,
			 const TColStd_Array1OfReal& DWeights,
			 const Standard_Real pTol, 
			 const Standard_Real wTol,
			 const Standard_Real pas)
{
  Standard_Boolean ok = Standard_True;
  Standard_Integer ii, L =  P1.Length();
  Standard_Real dw;
  gp_Vec dP;
  for (ii=1; ii<=L; ii++) {
    dw = (W2(ii)-W1(ii)) / pas;
    if (Abs(dw-DWeights(ii))>wTol) {
      if (Affich) {
        cout<<"erreur dans la derivee 1ere du poids pour l'indice "<<ii<<endl;
        cout<<"par diff finies : "<<dw<<endl;
        cout<<"resultat obtenu : "<<DWeights(ii)<<endl;
      }
      ok = Standard_False;
    }
    dP.SetXYZ( (P2(ii).XYZ()- P1(ii).XYZ()) /pas );
    gp_Vec diff = dP - DPoles(ii);
    if (diff.Magnitude()>pTol) {
      if (Affich) {
        cout<<"erreur dans la derivee 1ere du pole pour l'indice "<<ii<<endl;
        cout<<"par diff finies : ("<<dP.X()
                              <<" "<<dP.Y()
                              <<" "<<dP.Z()<<")"<<endl;
        cout<<"resultat obtenu : ("<<DPoles(ii).X()
                              <<" "<<DPoles(ii).Y()
                              <<" "<<DPoles(ii).Z()<<")"<<endl;
      }
      ok = Standard_False;
    }
  }
  return ok;
}

Standard_Boolean verifD2(const TColgp_Array1OfVec& DP1,
                         const TColStd_Array1OfReal& DW1,
                         const TColgp_Array1OfVec& DP2,
                         const TColStd_Array1OfReal& DW2,
                         const TColgp_Array1OfVec& D2Poles,
                         const TColStd_Array1OfReal& D2Weights,
                         const Standard_Real pTol, 
                         const Standard_Real wTol,
                         const Standard_Real pas)
{
  Standard_Boolean ok = Standard_True;;
  Standard_Integer ii, L =  DP1.Length();
  Standard_Real d2w;
  gp_Vec d2P;
  for (ii=1; ii<=L; ii++) {
    Standard_Real dw1 = DW1(ii), dw2 = DW2(ii);
    d2w = (dw2-dw1) / pas;
    if (Abs(d2w-D2Weights(ii))>wTol) {
      if (Affich) {
        cout<<"erreur dans la derivee 2nde du poids pour l'indice "<<ii<<endl;
        cout<<"par diff finies : "<<d2w<<endl;
        cout<<"resultat obtenu : "<<D2Weights(ii)<<endl;
      }
      ok = Standard_False;
    }
    d2P.SetXYZ( (DP2(ii).XYZ()- DP1(ii).XYZ()) /pas );
    gp_Vec diff = d2P - D2Poles(ii);
    if (diff.Magnitude()>pTol) {
      if (Affich) {
        cout<<"erreur dans la derivee 2nde du pole pour l'indice "<<ii<<endl;
        cout<<"par diff finies : ("<<d2P.X()
                              <<" "<<d2P.Y()
                              <<" "<<d2P.Z()<<")"<<endl;
        cout<<"resultat obtenu : ("<<D2Poles(ii).X()
                              <<" "<<D2Poles(ii).Y()
                              <<" "<<D2Poles(ii).Z()<<")"<<endl;
      }
      ok = Standard_False;
    }
  }
  return ok;
}
#endif

// fonction d'evaluation des poles et des poids de mySurface pour D1 et D2
static void ResultEval(const Handle(Geom_BSplineSurface)& surf,
                       const Standard_Real V,
                       const Standard_Integer deriv,
                       TColStd_Array1OfReal& Result)
{
  Standard_Boolean rational = surf->IsVRational() ;
  Standard_Integer gap = 3;
  if ( rational ) gap++;
  Standard_Integer Cdeg = surf->VDegree(), 
  Cdim = surf->NbUPoles() * gap, 
  NbP = surf->NbVPoles();
  
  //  les noeuds plats
  Standard_Integer Ksize = NbP + Cdeg + 1;
  TColStd_Array1OfReal FKnots(1,Ksize);
  surf->VKnotSequence(FKnots);
  
  //  les poles
  Standard_Integer Psize = Cdim * NbP;
  TColStd_Array1OfReal SPoles(1,Psize);
  Standard_Integer ii, jj, ipole=1;
  for (jj=1;jj<=NbP;jj++) {
    for (ii=1;ii<=surf->NbUPoles();ii++) {
      SPoles(ipole) = surf->Pole(ii,jj).X();
      SPoles(ipole+1) = surf->Pole(ii,jj).Y();
      SPoles(ipole+2) = surf->Pole(ii,jj).Z();
      if (rational) {
	SPoles(ipole+3) = surf->Weight(ii,jj);
	SPoles(ipole) *= SPoles(ipole+3);
	SPoles(ipole+1) *= SPoles(ipole+3);
	SPoles(ipole+2) *= SPoles(ipole+3);
      }
      ipole+=gap;
    }
  }
  Standard_Real * Padr = (Standard_Real *) &SPoles(1);
  
  Standard_Boolean periodic_flag = Standard_False ;
  Standard_Integer extrap_mode[2];
  extrap_mode[0] = extrap_mode[1] = Cdeg;
  TColStd_Array1OfReal  EvalBS(1, Cdim * (deriv+1)) ; 
  Standard_Real * Eadr = (Standard_Real *) &EvalBS(1) ;
  BSplCLib::Eval(V,periodic_flag,deriv,extrap_mode[0],
		 Cdeg,FKnots,Cdim,*Padr,*Eadr);

  for (ii=1;ii<=Cdim;ii++) {
    Result(ii) = EvalBS(ii+deriv*Cdim);
  }
}


//=======================================================================
//function : GeomFill_NSections
//purpose  : 
//=======================================================================

GeomFill_NSections::GeomFill_NSections(const TColGeom_SequenceOfCurve& NC)
{
  mySections = NC;
  UFirst = 0.;
  ULast = 1.;
  VFirst = 0.;
  VLast = 1.;
  myRefSurf.Nullify();
  ComputeSurface();
}

//=======================================================================
//function : GeomFill_NSections
//purpose  : 
//=======================================================================

GeomFill_NSections::GeomFill_NSections(const TColGeom_SequenceOfCurve& NC,
				       const TColStd_SequenceOfReal& NP)
{
  mySections = NC;
  myParams = NP;
  UFirst = 0.;
  ULast = 1.;
  VFirst = 0.;
  VLast = 1.;
  myRefSurf.Nullify();
  ComputeSurface();
}

//=======================================================================
//function : GeomFill_NSections
//purpose  : 
//=======================================================================

GeomFill_NSections::GeomFill_NSections(const TColGeom_SequenceOfCurve& NC,
				       const TColStd_SequenceOfReal& NP,
				       const Standard_Real UF,
				       const Standard_Real UL,
				       const Standard_Real VF,
				       const Standard_Real VL)
{
  mySections = NC;
  myParams = NP;
  UFirst = UF;
  ULast = UL;
  VFirst = VF;
  VLast = VL;
  myRefSurf.Nullify();
  ComputeSurface();
}

//=======================================================================
//function : GeomFill_NSections
//purpose  : 
//=======================================================================

GeomFill_NSections::GeomFill_NSections(const TColGeom_SequenceOfCurve& NC,
				       const GeomFill_SequenceOfTrsf& Trsfs,
				       const TColStd_SequenceOfReal& NP,
				       const Standard_Real UF,
				       const Standard_Real UL,
				       const Standard_Real VF,
				       const Standard_Real VL,
				       const Handle(Geom_BSplineSurface)& Surf)
{
  mySections = NC;
  myTrsfs = Trsfs;
  myParams = NP;
  UFirst = UF;
  ULast = UL;
  VFirst = VF;
  VLast = VL;
  myRefSurf = Surf;
  ComputeSurface();
}

//=======================================================
// Purpose :D0
//=======================================================
 Standard_Boolean GeomFill_NSections::D0(const Standard_Real V,
					      TColgp_Array1OfPnt& Poles,
					      TColStd_Array1OfReal& Weights) 
{
  if (mySurface.IsNull()) {
    return Standard_False;
  }
  else {
    Handle(Geom_BSplineCurve) Curve 
      = Handle(Geom_BSplineCurve)::DownCast(mySurface->VIso( V, Standard_False ));
    TColgp_Array1OfPnt poles(1,mySurface->NbUPoles());
    TColStd_Array1OfReal weights(1,mySurface->NbUPoles());
    Curve->Poles(poles);
    Curve->Weights(weights);
    Standard_Integer ii, L =  Poles.Length();
    for (ii=1; ii<=L; ii++) {
      Poles(ii).SetXYZ(poles(ii).XYZ());
      Weights(ii) = weights(ii);
    }
  }
  return Standard_True;
}

//=======================================================
// Purpose :D1
//=======================================================
 Standard_Boolean GeomFill_NSections::D1(const Standard_Real V,
					      TColgp_Array1OfPnt& Poles,
					      TColgp_Array1OfVec& DPoles,
					      TColStd_Array1OfReal& Weights,
					      TColStd_Array1OfReal& DWeights) 
{
  if (mySurface.IsNull() ) return Standard_False;

  Standard_Boolean ok = D0(V,Poles,Weights);
  if (!ok) return Standard_False;

  Standard_Integer L =  Poles.Length(), derivative_request = 1;
  Standard_Boolean rational = mySurface->IsVRational() ;
  Standard_Integer gap = 3;
  if (rational) gap++;

  Standard_Integer dimResult = mySurface->NbUPoles() * gap;
  Handle(Geom_BSplineSurface) surf_deper;
  if (mySurface->IsVPeriodic()) {
    surf_deper = Handle(Geom_BSplineSurface)::DownCast(mySurface->Copy());
    surf_deper->SetVNotPeriodic();
    dimResult = surf_deper->NbUPoles() * gap;
  }
  TColStd_Array1OfReal Result(1,dimResult);
  if (mySurface->IsVPeriodic()) {
    ResultEval(surf_deper,V,derivative_request,Result);
  }
  else {
    ResultEval(mySurface,V,derivative_request,Result);
  }


  Standard_Real ww, EpsW = 10*Precision::PConfusion();
  Standard_Boolean NullWeight = Standard_False;
  if (!rational) DWeights.Init(0.);
  Standard_Integer indice = 1, ii;

  //  recopie des poles du resultat sous forme de points 3D et de poids
  for (ii=1; ii<=L && (!NullWeight) ; ii++) {
    DPoles(ii).SetX( Result(indice) );
    DPoles(ii).SetY( Result(indice+1) );
    DPoles(ii).SetZ( Result(indice+2) );
    if (rational) {
      ww = Weights(ii);
      if (ww < EpsW) {
        NullWeight = Standard_True;
      }
      else {
        DWeights(ii) = Result(indice+3);
        DPoles(ii)
          .SetXYZ( ( DPoles(ii).XYZ()-DWeights(ii)*Poles(ii).Coord() ) / ww );
      }
    }
    indice += gap;
  }
  if (NullWeight) return Standard_False;

  // verif par diff finies sous debug sauf pour les surfaces periodiques
#ifdef OCCT_DEBUG
  if (!mySurface->IsVPeriodic()) {
    Standard_Real pas = 1.e-6, wTol = 1.e-4, pTol = 1.e-3;
    Standard_Real V1,V2;
    Standard_Boolean ok1,ok2;
    TColStd_Array1OfReal W1(1,L),W2(1,L);
    TColgp_Array1OfPnt P1(1,L),P2(1,L);
    gp_Pnt nul(0.,0.,0.);
    W1.Init(0.);
    W2.Init(0.);
    P1.Init(nul);
    P2.Init(nul);
    
    V1 = V;
    V2 = V+pas;
    ok1 = D0(V1,P1,W1);
    ok2 = D0(V2,P2,W2);
    if (!ok1 || !ok2) cout<<"probleme en D0"<<endl;
    Standard_Boolean check = verifD1(P1,W1,P2,W2,DPoles,DWeights,pTol,wTol,pas);
    if (!check) cout<<"D1 incorrecte en V = "<<V<<endl;
  }
#endif
  
  return Standard_True;
}

//=======================================================
// Purpose :D2
//=======================================================
 Standard_Boolean GeomFill_NSections::D2(const Standard_Real V,
                                              TColgp_Array1OfPnt& Poles,
                                              TColgp_Array1OfVec& DPoles,
                                              TColgp_Array1OfVec& D2Poles,
                                              TColStd_Array1OfReal& Weights,
                                              TColStd_Array1OfReal& DWeights,
                                              TColStd_Array1OfReal& D2Weights) 
{ 
  if (mySurface.IsNull() ) return Standard_False;

  // pb dans BSplCLib::Eval() pour les surfaces rationnelles de degre 1
  // si l'ordre de derivation est egal a 2.
  if (mySurface->VDegree()<2) return Standard_False;

  Standard_Boolean ok = D1(V,Poles,DPoles,Weights,DWeights);
  if (!ok) return Standard_False;

  Standard_Integer L =  Poles.Length(), derivative_request = 2;
  Standard_Boolean rational = mySurface->IsVRational() ;
  Standard_Integer gap = 3;
  if (rational) gap++;
  
  Standard_Integer dimResult = mySurface->NbUPoles() * gap;
  Handle(Geom_BSplineSurface) surf_deper;
  if (mySurface->IsVPeriodic()) {
    surf_deper = Handle(Geom_BSplineSurface)::DownCast(mySurface->Copy());
    surf_deper->SetVNotPeriodic();
    dimResult = surf_deper->NbUPoles() * gap;
  }
  TColStd_Array1OfReal Result(1,dimResult);
  if (mySurface->IsVPeriodic()) {
    ResultEval(surf_deper,V,derivative_request,Result);
  }
  else {
    ResultEval(mySurface,V,derivative_request,Result);
  }


  Standard_Real ww, EpsW = 10*Precision::PConfusion();
  Standard_Boolean NullWeight = Standard_False;
  if (!rational) D2Weights.Init(0.);
  Standard_Integer indice = 1, ii;

  //  recopie des poles du resultat sous forme de points 3D et de poids
  for (ii=1; ii<=L && (!NullWeight) ; ii++) {
    D2Poles(ii).SetX( Result(indice) );
    D2Poles(ii).SetY( Result(indice+1) );
    D2Poles(ii).SetZ( Result(indice+2) );
    if (rational) {
      ww = Weights(ii);
      if (ww < EpsW) {
        NullWeight = Standard_True;
      }
      else {
        D2Weights(ii) = Result(indice+3);
        D2Poles(ii)
          .SetXYZ( ( D2Poles(ii).XYZ() - D2Weights(ii)*Poles(ii).Coord()
                    - 2*DWeights(ii)*DPoles(ii).XYZ() ) / ww );
      }
    }
    indice += gap;
  }
  if (NullWeight) return Standard_False;

  // verif par diff finies sous debug sauf pour les surfaces periodiques
#ifdef OCCT_DEBUG
  if (!mySurface->IsVPeriodic()) {
    Standard_Real V1,V2;
    Standard_Boolean ok1,ok2;
    Standard_Real pas = 1.e-6, wTol = 1.e-4, pTol = 1.e-3;
    TColStd_Array1OfReal W1(1,L),W2(1,L),DW1(1,L),DW2(1,L);
    TColgp_Array1OfPnt P1(1,L),P2(1,L);
    TColgp_Array1OfVec DP1(1,L),DP2(1,L);
    gp_Pnt nul(0.,0.,0.);
    gp_Vec Vnul(0.,0.,0.);
    W1.Init(0.);
    W2.Init(0.);
    DW1.Init(0.);
    DW2.Init(0.);
    P1.Init(nul);
    P2.Init(nul);
    DP1.Init(Vnul);
    DP2.Init(Vnul);
    
    V1 = V;
    V2 = V+pas;
    ok1 = D1(V1,P1,DP1,W1,DW1);
    ok2 = D1(V2,P2,DP2,W2,DW2);
    if (!ok1 || !ok2) cout<<"probleme en D0 ou en D1"<<endl;
    Standard_Boolean check = verifD2(DP1,DW1,DP2,DW2,D2Poles,D2Weights,pTol,wTol,pas);
    if (!check) cout<<"D2 incorrecte en V = "<<V<<endl;
  }
#endif
  
  return Standard_True;
}

//=======================================================
// Purpose :BSplineSurface()
//=======================================================
 Handle(Geom_BSplineSurface) 
     GeomFill_NSections::BSplineSurface() const
{
  return mySurface;
}


//=======================================================
// Purpose :SetSurface()
//=======================================================
 void GeomFill_NSections::SetSurface(const Handle(Geom_BSplineSurface)& RefSurf)
{
  myRefSurf = RefSurf;
}

//=======================================================
// Purpose :ComputeSurface()
//=======================================================
 void GeomFill_NSections::ComputeSurface()
{

  Handle(Geom_BSplineSurface) BS;
  if (myRefSurf.IsNull()) {

    Standard_Real myPres3d = 1.e-06;
    Standard_Integer i,j,jdeb=1,jfin=mySections.Length();
    
    if (jfin <= jdeb)
    {
      //We will not be able to create surface from single curve.
      return;
    }

    GeomFill_SectionGenerator section;
    Handle(Geom_BSplineSurface) surface;

    for (j=jdeb; j<=jfin; j++) {

        // read the j-th curve
        Handle(Geom_Curve) curv = mySections(j);
        
        // transformation to BSpline reparametrized to [UFirst,ULast]
        Handle(Geom_BSplineCurve) curvBS = Handle(Geom_BSplineCurve)::DownCast (curv);
        if (curvBS.IsNull())
        {
          curvBS = GeomConvert::CurveToBSplineCurve (curv, Convert_QuasiAngular);
        }

        TColStd_Array1OfReal BSK(1,curvBS->NbKnots());
        curvBS->Knots(BSK);
        BSplCLib::Reparametrize(UFirst,ULast,BSK);
        curvBS->SetKnots(BSK);
        
        section.AddCurve(curvBS);        
    }
    
    /*
    if (s2Point) {
      curv =  mySections(jfin+1);
      first =  curv->FirstParameter();
      last = curv->LastParameter();
      TColgp_Array1OfPnt Extremities(1,2);
      Extremities(1) = curv->Value(first);
      Extremities(2) = curv->Value(last);
      TColStd_Array1OfReal Bounds(1,2);
      Bounds(1) = UFirst;
      Bounds(2) = ULast;
      Standard_Real Deg = 1;
      TColStd_Array1OfInteger Mult(1,2);
      Mult(1) = (Standard_Integer ) Deg+1;
      Mult(2) = (Standard_Integer ) Deg+1;
      Handle(Geom_BSplineCurve) BSPoint
        = new Geom_BSplineCurve(Extremities,Bounds,Mult,(Standard_Integer ) Deg);
      section.AddCurve(BSPoint);
    }*/

    Standard_Integer Nbcurves = mySections.Length();
    Standard_Integer Nbpar = myParams.Length();
    if (Nbpar > 0)
    {
      Handle(TColStd_HArray1OfReal) HPar
        = new TColStd_HArray1OfReal(1, Nbpar);
      for (i = 1; i <= Nbpar; i++) {
        HPar->SetValue(i, myParams(i));
      }
      section.SetParam(HPar);
    }
    section.Perform(Precision::PConfusion());
    
    Handle(GeomFill_Line) line = new GeomFill_Line(Nbcurves);
    Standard_Integer nbIt = 0, degmin = 2, degmax = 6;
    Standard_Boolean knownP = Nbpar > 0;
    GeomFill_AppSurf anApprox(degmin, degmax, myPres3d, myPres3d, nbIt, knownP);
    Standard_Boolean SpApprox = Standard_True;
    anApprox.Perform(line, section, SpApprox);

    BS = 
      new Geom_BSplineSurface(anApprox.SurfPoles(), anApprox.SurfWeights(),
                              anApprox.SurfUKnots(), anApprox.SurfVKnots(),
                              anApprox.SurfUMults(), anApprox.SurfVMults(),
                              anApprox.UDegree(), anApprox.VDegree());
  }

  else {
  
    // segmentation de myRefSurf
    Standard_Real Ui1, Ui2, V0, V1;
    BS = Handle(Geom_BSplineSurface)::DownCast(myRefSurf->Copy());
    Ui1 = UFirst;
    Ui2 = ULast;
    Standard_Integer i1, i2;
    myRefSurf->LocateU( Ui1, Precision::PConfusion(), i1, i2 );
    if (Abs(Ui1 - myRefSurf->UKnot(i1)) <= Precision::PConfusion())
      Ui1 = myRefSurf->UKnot(i1);
    if (Abs(Ui1 - myRefSurf->UKnot(i2)) <= Precision::PConfusion())
      Ui1 = myRefSurf->UKnot(i2);
    myRefSurf->LocateU( Ui2, Precision::PConfusion(), i1, i2 );
    if (Abs(Ui2 - myRefSurf->UKnot(i1)) <= Precision::PConfusion())
      Ui2 = myRefSurf->UKnot(i1);
    if (Abs(Ui2 - myRefSurf->UKnot(i2)) <= Precision::PConfusion())
      Ui2 = myRefSurf->UKnot(i2);
    V0  = myRefSurf->VKnot(myRefSurf->FirstVKnotIndex());
    V1  = myRefSurf->VKnot(myRefSurf->LastVKnotIndex());
    BS->CheckAndSegment(Ui1,Ui2,V0,V1);
  }
  mySurface = BS;
  // On augmente le degre pour que le positionnement D2 soit correct 
  if (mySurface->VDegree()<2) {
    mySurface->IncreaseDegree(mySurface->UDegree(),2);
  }
#ifdef OCCT_DEBUG
  NbSurf++;
  if (Affich) {
#ifdef DRAW
    char name[256];
    sprintf(name,"NS_Surf_%d",NbSurf);
    DrawTrSurf::Set(name,BS);
    cout<<endl<<"RESULTAT de ComputeSurface : NS_Surf_"<<NbSurf<<endl<<endl;
#endif
  }
#endif
}

//=======================================================
// Purpose :SectionShape
//=======================================================
 void GeomFill_NSections::SectionShape(Standard_Integer& NbPoles,
                                            Standard_Integer& NbKnots,
                                            Standard_Integer& Degree) const
{
  if (mySurface.IsNull())
    return;
  
  NbPoles = mySurface->NbUPoles();
  NbKnots = mySurface->NbUKnots();
  Degree  = mySurface->UDegree();
}

//=======================================================
// Purpose :Knots
//=======================================================
 void GeomFill_NSections::Knots(TColStd_Array1OfReal& TKnots) const
{
  if (!mySurface.IsNull())
    mySurface->UKnots(TKnots);
}

//=======================================================
// Purpose :Mults
//=======================================================
 void GeomFill_NSections::Mults(TColStd_Array1OfInteger& TMults) const
{
  if (!mySurface.IsNull())
    mySurface->UMultiplicities(TMults);
}


//=======================================================
// Purpose :IsRational
//=======================================================
 Standard_Boolean GeomFill_NSections::IsRational() const
{
  if (!mySurface.IsNull())
    return mySurface->IsURational();

  return Standard_False;
}

//=======================================================
// Purpose :IsUPeriodic
//=======================================================
 Standard_Boolean GeomFill_NSections::IsUPeriodic() const
{
  if (!mySurface.IsNull())
    return  mySurface->IsUPeriodic();

  return Standard_False;
}

//=======================================================
// Purpose :IsVPeriodic
//=======================================================
 Standard_Boolean GeomFill_NSections::IsVPeriodic() const
{
  if (!mySurface.IsNull())
    return  mySurface->IsVPeriodic();

  return Standard_False;
}

//=======================================================
// Purpose :NbIntervals
//=======================================================
 Standard_Integer GeomFill_NSections::NbIntervals(const GeomAbs_Shape S) const
{
  if (mySurface.IsNull())
    return 0;

  GeomAdaptor_Surface AdS(mySurface);
  return AdS.NbVIntervals(S);
}


//=======================================================
// Purpose :Intervals
//=======================================================
 void GeomFill_NSections::Intervals(TColStd_Array1OfReal& T,
                                         const GeomAbs_Shape S) const
{
  if (mySurface.IsNull())
    return;

  GeomAdaptor_Surface AdS(mySurface);
  AdS.VIntervals(T,S);
}


//=======================================================
// Purpose : SetInterval
//=======================================================
// void GeomFill_NSections::SetInterval(const Standard_Real F,
 void GeomFill_NSections::SetInterval(const Standard_Real ,
//                                           const Standard_Real L) 
                                           const Standard_Real ) 
{
  // rien a faire : mySurface est supposee Cn en V
}

//=======================================================
// Purpose : GetInterval
//=======================================================
 void GeomFill_NSections::GetInterval(Standard_Real& F,
                                           Standard_Real& L) const
{
  F = VFirst;
  L = VLast;
}

//=======================================================
// Purpose : GetDomain
//=======================================================
 void GeomFill_NSections::GetDomain(Standard_Real& F,
                                         Standard_Real& L) const
{
  F = VFirst;
  L = VLast;
}

//=======================================================
// Purpose : GetTolerance
//=======================================================
 void GeomFill_NSections::GetTolerance(const Standard_Real BoundTol,
                                            const Standard_Real SurfTol,
//                                            const Standard_Real AngleTol,
                                            const Standard_Real ,
                                            TColStd_Array1OfReal& Tol3d) const
{
  Tol3d.Init(SurfTol);
  if (BoundTol<SurfTol) {
    Tol3d(Tol3d.Lower()) = BoundTol;
    Tol3d(Tol3d.Upper()) = BoundTol;
  }
}

//=======================================================
// Purpose : BarycentreOfSurf
//=======================================================
 gp_Pnt GeomFill_NSections::BarycentreOfSurf() const
{
  gp_Pnt P, Bary;
  Bary.SetCoord(0., 0., 0.);

  if (mySurface.IsNull())
    return Bary;

  Standard_Integer ii,jj;
  Standard_Real U0, U1, V0, V1;
  mySurface->Bounds(U0,U1,V0,V1);
  Standard_Real V = V0, DeltaV = ( V1 - V0 ) / 20;
  Standard_Real U = U0, DeltaU = ( U1 - U0 ) / 20;
  for(jj=0;jj<=20;jj++,V+=DeltaV) {
    for (ii=0 ; ii <=20; ii++, U+=DeltaU) {
      P = mySurface->Value(U,V);
      Bary.ChangeCoord() += P.XYZ();
    } 
  }
    
  Bary.ChangeCoord() /= (21*21);
  return Bary;
}


//=======================================================
// Purpose : MaximalSection
//=======================================================
 Standard_Real GeomFill_NSections::MaximalSection() const
{
  Standard_Real L, Lmax=0.;
  Standard_Integer ii;
  for (ii=1; ii <=mySections.Length(); ii++) {
    GeomAdaptor_Curve AC (mySections(ii));
    L = GCPnts_AbscissaPoint::Length(AC);
    if (L>Lmax) Lmax = L;
  }
  return Lmax;
}


//=======================================================
// Purpose : GetMinimalWeight
//=======================================================
void GeomFill_NSections::GetMinimalWeight(TColStd_Array1OfReal& Weights) const
{
  if (mySurface.IsNull())
    return;

  if (mySurface->IsURational()) {
    Standard_Integer NbU = mySurface->NbUPoles(),
                     NbV = mySurface->NbVPoles();
    TColStd_Array2OfReal WSurf(1,NbU,1,NbV);
    mySurface->Weights(WSurf);
    Standard_Integer i,j;
    for (i=1;i<=NbU;i++) {
      Standard_Real min = WSurf(i,1);
      for (j=2;j<=NbV;j++) {
        if (min> WSurf(i,j)) min = WSurf(i,j);
      }
      Weights.SetValue(i,min);
    }
  }
  else {
    Weights.Init(1);
  }
  
}


//=======================================================
// Purpose : IsConstant
//=======================================================
 Standard_Boolean GeomFill_NSections::IsConstant(Standard_Real& Error) const
{
  // on se limite a 2 sections
  Standard_Boolean isconst = (mySections.Length()==2);
  Standard_Real Err = 0.;

  if (isconst) {
    GeomAdaptor_Curve AC1(mySections(1));
    GeomAbs_CurveType CType = AC1.GetType();
    GeomAdaptor_Curve AC2(mySections(2));
    // les sections doivent avoir le meme type
    isconst = ( AC2.GetType() == CType);

    if (isconst) {
      if (CType == GeomAbs_Circle) {
        gp_Circ C1 = AC1.Circle();
        gp_Circ C2 = AC2.Circle();
        Standard_Real Tol = 1.e-7;
        Standard_Boolean samedir, samerad, samepos;
        samedir = (C1.Axis().IsParallel(C2.Axis(),1.e-4));
        samerad = (Abs(C1.Radius()-C2.Radius())<Tol);
        samepos = (C1.Location().Distance(C2.Location())<Tol);
        if (!samepos) {
          gp_Ax1 D(C1.Location(),gp_Vec(C1.Location(),C2.Location()));
          samepos = (C1.Axis().IsParallel(D,1.e-4));
        }
        isconst = samedir && samerad && samepos;
      }
      else if (CType == GeomAbs_Line) {
        gp_Lin L1 = AC1.Line();
        gp_Lin L2 = AC2.Line(); 
        Standard_Real Tol = 1.e-7;
        Standard_Boolean samedir, samelength, samepos;
        samedir = (L1.Direction().IsParallel(L2.Direction(),1.e-4));
        gp_Pnt P11 = AC1.Value(AC1.FirstParameter()),
               P12 = AC1.Value(AC1.LastParameter()),
               P21 = AC2.Value(AC2.FirstParameter()),
               P22 = AC2.Value(AC2.LastParameter());
        samelength = (Abs(P11.Distance(P12)-P21.Distance(P22))<Tol);
        // l'ecart entre les 2 sections ne compte pas
        samepos = ( ( P11.Distance(P21)<Tol && P12.Distance(P22)<Tol )
                     || ( P12.Distance(P21)<Tol && P11.Distance(P22)<Tol ) );
        //samepos = Standard_True;
        isconst = samedir && samelength && samepos;
      }
      else {
        isconst = Standard_False;
      }
    }

  }

  Error = Err;
  return isconst;
}


//=======================================================
// Purpose : ConstantSection
//=======================================================
 Handle(Geom_Curve) GeomFill_NSections::ConstantSection() const
{
//  Standard_Real Err;
//  if (!IsConstant(Err)) throw StdFail_NotDone("The Law is not Constant!");
  Handle(Geom_Curve) C;
  C = Handle(Geom_Curve)::DownCast( mySections(1)->Copy());
  return C;
}


//=======================================================
// Purpose : IsConicalLaw
//=======================================================
 Standard_Boolean GeomFill_NSections::IsConicalLaw(Standard_Real& Error) const
{
  Standard_Boolean isconic = (mySections.Length()==2);
  Standard_Real Err = 0.;
  if (isconic) {
    GeomAdaptor_Curve AC1(mySections(1));
    GeomAdaptor_Curve AC2(mySections(2));
    isconic = ( AC1.GetType() == GeomAbs_Circle )
                     &&  ( AC2.GetType() == GeomAbs_Circle ) ;
    if (isconic) {
      gp_Circ C1 = AC1.Circle();
      if (!myTrsfs.IsEmpty())
        C1.Transform(myTrsfs(1).Inverted());
      gp_Circ C2 = AC2.Circle();
      if (!myTrsfs.IsEmpty())
        C2.Transform(myTrsfs(2).Inverted());
      Standard_Real Tol = 1.e-7;
      //Standard_Boolean samedir, linearrad, sameaxis;
      isconic = (C1.Axis().IsParallel(C2.Axis(),1.e-4));
      //  pour 2 sections, la variation du rayon est forcement lineaire
      //linearrad = Standard_True;
      //  formule plus generale pour 3 sections au moins
      //  Standard_Real param0 = C2.Radius()*myParams(1) - C1.Radius()*myParams(2);
      //  param0 = param0 / (C2.Radius()-C1.Radius()) ;
      //  linearrad = ( Abs( C3.Radius()*myParams(1)-C1.Radius()*myParams(3)
      //                          - param0*(C3.Radius()-C1.Radius()) ) < Tol);
      if (isconic)
      {
        gp_Lin Line1(C1.Axis());
        isconic = (Line1.Distance(C2.Location()) < Tol);
        /*
        sameaxis = (C1.Location().Distance(C2.Location())<Tol);
        if (!sameaxis) {
        gp_Ax1 D(C1.Location(),gp_Vec(C1.Location(),C2.Location()));
        sameaxis = (C1.Axis().IsParallel(D,1.e-4));
        }
        isconic = samedir && linearrad && sameaxis;
        */
        if (isconic)
        {
          //// Modified by jgv, 18.02.2009 for OCC20866 ////
          Standard_Real first1 = AC1.FirstParameter(), last1 = AC1.LastParameter();
          Standard_Real first2 = AC2.FirstParameter(), last2 = AC2.LastParameter();
          isconic = (Abs(first1-first2) <= Precision::PConfusion() &&
                     Abs(last1-last2)   <= Precision::PConfusion());
          //////////////////////////////////////////////////
        }
      }
    }
  }

  Error = Err;
  return isconic;
}


//=======================================================
// Purpose : CirclSection
//=======================================================
 Handle(Geom_Curve) GeomFill_NSections::CirclSection(const Standard_Real V) const
{
  Standard_Real Err;
  if (!IsConicalLaw(Err)) throw StdFail_NotDone("The Law is not Conical!");

  GeomAdaptor_Curve AC1(mySections(1));
  GeomAdaptor_Curve AC2(mySections(mySections.Length()));
  gp_Circ C1 = AC1.Circle();
  gp_Circ C2 = AC2.Circle();

  Standard_Real p1 = myParams(1), p2 = myParams(myParams.Length());
  Standard_Real radius = ( C2.Radius() - C1.Radius() ) * (V - p1) / (p2 - p1) 
                                  + C1.Radius();

  C1.SetRadius(radius);
  Handle(Geom_Curve) C = new (Geom_Circle) (C1);

  const Standard_Real aParF = AC1.FirstParameter();
  const Standard_Real aParL = AC1.LastParameter();
  const Standard_Real aPeriod = AC1.IsPeriodic() ? AC1.Period() : 0.0;

  if ((aPeriod == 0.0) || (Abs(aParL - aParF - aPeriod) > Precision::PConfusion()))
  {
    Handle(Geom_Curve) Cbis = new Geom_TrimmedCurve(C, aParF, aParL);
    C = Cbis;
  }
  return C;
}
