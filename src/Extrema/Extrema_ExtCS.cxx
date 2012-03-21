// Created on: 1995-07-18
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


//  Modified by skv - Thu Jul  7 12:29:34 2005 OCC9134

#include <Extrema_ExtCS.ixx>
#include <Extrema_GenExtCS.hxx>
#include <StdFail_NotDone.hxx>
#include <Standard_NotImplemented.hxx>
#include <StdFail_InfiniteSolutions.hxx>
#include <Precision.hxx>
#include <GeomAbs_CurveType.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pln.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Cone.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <gp_Lin.hxx>

#include <ElCLib.hxx>
#include <Extrema_ExtPElC.hxx>
#include <Bnd_Box.hxx>
#include <BndLib_AddSurface.hxx>
#include <Extrema_ExtPElS.hxx>

Extrema_ExtCS::Extrema_ExtCS() 
{
  myDone = Standard_False;
}

Extrema_ExtCS::Extrema_ExtCS(const Adaptor3d_Curve&   C,
			     const Adaptor3d_Surface& S,
			     const Standard_Real    TolC,
			     const Standard_Real    TolS)

{
  Initialize(S, S.FirstUParameter(), S.LastUParameter(), 
	     S.FirstVParameter(), S.LastVParameter(), 
	     TolC, TolS);
  Perform(C, C.FirstParameter(), C.LastParameter());
}

Extrema_ExtCS::Extrema_ExtCS(const Adaptor3d_Curve&   C,
			     const Adaptor3d_Surface& S,
			     const Standard_Real    UCinf,
			     const Standard_Real    UCsup,
			     const Standard_Real    Uinf,	
			     const Standard_Real    Usup,
			     const Standard_Real    Vinf,	
			     const Standard_Real    Vsup,
			     const Standard_Real    TolC,
			     const Standard_Real    TolS)

{
  Initialize(S, Uinf, Usup, Vinf, Vsup, TolC, TolS);
  Perform(C, UCinf, UCsup);
}


void Extrema_ExtCS::Initialize(const Adaptor3d_Surface& S,
			       const Standard_Real    Uinf,	
			       const Standard_Real    Usup,
			       const Standard_Real    Vinf,	
			       const Standard_Real    Vsup,
			       const Standard_Real    TolC,
			       const Standard_Real    TolS)
{
  myS = (Adaptor3d_SurfacePtr)&S;
  myIsPar = Standard_False;
  myuinf  = Uinf;
  myusup  = Usup;
  myvinf  = Vinf;
  myvsup  = Vsup;
  mytolC  = TolC;
  mytolS  = TolS;
  myStype  = myS->GetType();
}

				
void Extrema_ExtCS::Perform(const Adaptor3d_Curve& C,
			    const Standard_Real  Uinf,
			    const Standard_Real  Usup)
{
  myucinf = Uinf;
  myucsup = Usup;
  myPOnS.Clear();
  myPOnC.Clear();
  mySqDist.Clear();
  Standard_Integer i;
  Standard_Integer NbT, NbU, NbV;
  NbT = NbU = NbV = 10;
  GeomAbs_CurveType myCtype  = C.GetType();


  switch(myCtype) {

  case GeomAbs_Line: 
    {
      
      switch(myStype) {
      case GeomAbs_Sphere:
	myExtElCS.Perform(C.Line(), myS->Sphere());
	break;   
      case GeomAbs_Cylinder:
	myExtElCS.Perform(C.Line(), myS->Cylinder());
	break;
      case GeomAbs_Plane:
	myExtElCS.Perform(C.Line(), myS->Plane());
	if (myExtElCS.IsParallel())   break;

      case GeomAbs_Torus:
      case GeomAbs_Cone:
      case GeomAbs_BezierSurface:
      case GeomAbs_BSplineSurface:
      case GeomAbs_SurfaceOfRevolution:
      case GeomAbs_SurfaceOfExtrusion:
      case GeomAbs_OtherSurface:
	{
	  Standard_Real cfirst = myucinf, clast = myucsup;
	  Standard_Real ufirst = myS->FirstUParameter(), ulast = myS->LastUParameter(), 
	                vfirst = myS->FirstVParameter(), vlast = myS->LastVParameter();

	  if(Precision::IsInfinite(Abs(cfirst)) || Precision::IsInfinite(Abs(clast))) {

	    Bnd_Box aSurfBox;
      BndLib_AddSurface::Add(*myS, ufirst, ulast, vfirst, vlast, Precision::Confusion(), aSurfBox);
	    Standard_Real xmin, ymin, zmin, xmax, ymax, zmax;
	    aSurfBox.Get(xmin, ymin, zmin, xmax, ymax, zmax);
	    Standard_Real tmin = Precision::Infinite(), tmax = -tmin;
	    gp_Lin aLin = C.Line();
	    

	    if(!( Precision::IsInfinite(Abs(xmin)) || Precision::IsInfinite(Abs(xmax)) || 
		  Precision::IsInfinite(Abs(ymin)) || Precision::IsInfinite(Abs(ymax)) || 
		  Precision::IsInfinite(Abs(zmin)) || Precision::IsInfinite(Abs(zmax)))  ) {
	      
	      Extrema_ExtPElC anExt;
	      Extrema_POnCurv aPntOnLin;
	      Standard_Real aParOnLin;
	      Standard_Real lim = Precision::Infinite();
	      gp_Pnt aLimPntArray[8];
	      
	      aLimPntArray[0].SetCoord(xmin, ymin, zmin);
	      aLimPntArray[1].SetCoord(xmax, ymin, zmin);
	      aLimPntArray[2].SetCoord(xmin, ymax, zmin);
	      aLimPntArray[3].SetCoord(xmax, ymax, zmin);
	      aLimPntArray[4].SetCoord(xmin, ymin, zmax);
	      aLimPntArray[5].SetCoord(xmax, ymin, zmax);
	      aLimPntArray[6].SetCoord(xmin, ymax, zmax);
	      aLimPntArray[7].SetCoord(xmax, ymax, zmax);

	      for(i = 0; i <= 7; i++) {
		anExt.Perform(aLimPntArray[i], aLin, Precision::Confusion(), -lim, lim);
		aPntOnLin = anExt.Point(1);
		aParOnLin = aPntOnLin.Parameter();
		tmin = Min(aParOnLin, tmin);
		tmax = Max(aParOnLin, tmax);
	      }
	      
	    }
	    else {
	      tmin = -1.e+50;
	      tmax =  1.e+50;
	    }


	    cfirst = Max(cfirst, tmin);
	    clast  = Min(clast,  tmax);

	  }


	    
	  Extrema_GenExtCS Ext(C, *myS, NbT, NbU, NbV, cfirst, clast, ufirst, ulast,
			       vfirst, vlast, mytolC, mytolS);

	  myDone = Ext.IsDone();
	  if (myDone) {
	    Standard_Integer NbExt = Ext.NbExt();
	    Standard_Real T,U,V;
	    Extrema_POnCurv PC;
	    Extrema_POnSurf PS;
	    for (i = 1; i <= NbExt; i++) {
	      PC = Ext.PointOnCurve(i);
	      PS = Ext.PointOnSurface(i);
	      T = PC.Parameter();
	      PS.Parameter(U, V);
	      if (myS->IsUPeriodic())
		U = ElCLib::InPeriod(U, myuinf, myuinf+myS->UPeriod());
	      if (myS->IsVPeriodic())
		V = ElCLib::InPeriod(V, myvinf, myvinf+myS->VPeriod());

	      if ((myucinf-T) <= mytolC && (T-myucsup) <= mytolC &&
		  (myuinf-U) <= mytolS && (U-myusup) <= mytolS &&
		  (myvinf-V) <= mytolS && (V-myvsup) <= mytolS) {
		mySqDist.Append(Ext.SquareDistance(i));
		myPOnC.Append(PC);
		myPOnS.Append(Extrema_POnSurf(U, V, PS.Value()));

	      
	      }
	    }
	  }
	  return;
	  
	}
#ifndef DEB
      default:
#endif
	break;
      }
      break;
    }
//  Modified by skv - Thu Jul  7 12:29:34 2005 OCC9134 Begin
  case GeomAbs_Circle:
    {
      if(myStype == GeomAbs_Cylinder) {
	myExtElCS.Perform(C.Circle(), myS->Cylinder());
	break;
      }
    }
  case GeomAbs_Hyperbola: 
    {
      if(myCtype == GeomAbs_Hyperbola && myStype == GeomAbs_Plane) {
//  Modified by skv - Thu Jul  7 12:29:34 2005 OCC9134 End
	myExtElCS.Perform(C.Hyperbola(), myS->Plane());
	break;
      }
    }
  default:
    {
      Extrema_GenExtCS Ext;
      Ext.Initialize(*myS, NbU, NbV, mytolS);
      if(myCtype == GeomAbs_Hyperbola) {
	Standard_Real tmin = Max(-20., C.FirstParameter());
	Standard_Real tmax = Min(20., C.LastParameter());
	Ext.Perform(C, NbT, tmin, tmax, mytolC); // to avoid overflow
      }
      else {
	if(myCtype == GeomAbs_Circle && NbT < 13) {
	  NbT = 13;
	}
	Ext.Perform(C, NbT, mytolC);
      }
	
      myDone = Ext.IsDone();
      if (myDone) {
	Standard_Integer NbExt = Ext.NbExt();
	Standard_Real T,U,V;
	Extrema_POnCurv PC;
	Extrema_POnSurf PS;
	for (i = 1; i <= NbExt; i++) {
	  PC = Ext.PointOnCurve(i);
	  PS = Ext.PointOnSurface(i);
	  T = PC.Parameter();
	  PS.Parameter(U, V);
	  if (C.IsPeriodic())
	    T = ElCLib::InPeriod(T, myucinf, myucinf+C.Period());
	  if (myS->IsUPeriodic())
	    U = ElCLib::InPeriod(U, myuinf, myuinf+myS->UPeriod());
	  if (myS->IsVPeriodic())
	    V = ElCLib::InPeriod(V, myvinf, myvinf+myS->VPeriod());
	  
	  if ((myucinf-T) <= mytolC && (T-myucsup) <= mytolC &&
	      (myuinf-U) <= mytolS && (U-myusup) <= mytolS &&
	      (myvinf-V) <= mytolS && (V-myvsup) <= mytolS) {
	    mySqDist.Append(Ext.SquareDistance(i));
	    PC.SetValues(T, PC.Value());
	    myPOnC.Append(PC);
	    myPOnS.Append(Extrema_POnSurf(U, V, PS.Value()));
	  }
	}
      }
      return;
    }
    break;
  }
  
  myDone = myExtElCS.IsDone();
  if (myDone) {
    myIsPar = myExtElCS.IsParallel();
    if (myIsPar) {
      mySqDist.Append(myExtElCS.SquareDistance(1));
    }
    else {
      Standard_Integer NbExt = myExtElCS.NbExt();
      Standard_Real U, V;
      for (i = 1; i <= NbExt; i++) {
	Extrema_POnCurv PC;
	Extrema_POnSurf PS;
	myExtElCS.Points(i, PC, PS);
	Standard_Real Ucurve = PC.Parameter();
	PS.Parameter(U, V);

	if((myStype == GeomAbs_Sphere) || (myStype == GeomAbs_Cylinder)) {
	  U = ElCLib::InPeriod(U, myuinf, myuinf+2.*M_PI);
	}

	if ((myuinf-U) <= mytolS && (U-myusup) <= mytolS &&
	    (myvinf-V) <= mytolS && (V-myvsup) <= mytolS &&
	    (myucinf-Ucurve) <= mytolC && (Ucurve-myucsup) <= mytolC) {
	  mySqDist.Append(myExtElCS.SquareDistance(i));
	  myPOnS.Append(Extrema_POnSurf(U, V, PS.Value()));
	  myPOnC.Append(PC);
	}
      }
    }
  }
  
}


Standard_Boolean Extrema_ExtCS::IsDone() const
{
  return myDone;
}

Standard_Boolean Extrema_ExtCS::IsParallel() const
{
  return myIsPar;
}


Standard_Real Extrema_ExtCS::SquareDistance(const Standard_Integer N) const
{
  if(!myDone) StdFail_NotDone::Raise();
  if (myIsPar && N != 1) StdFail_InfiniteSolutions::Raise();
  if ((N < 1) || (N > mySqDist.Length())) Standard_OutOfRange::Raise();
  return mySqDist.Value(N);
}


Standard_Integer Extrema_ExtCS::NbExt() const
{
  if(!myDone) StdFail_NotDone::Raise();
  return mySqDist.Length();
}



void Extrema_ExtCS::Points(const Standard_Integer N,
			    Extrema_POnCurv&       P1,
			    Extrema_POnSurf&       P2) const
{
  if(!myDone) StdFail_NotDone::Raise();
  P1 = myPOnC.Value(N);
  P2 = myPOnS.Value(N);
}
