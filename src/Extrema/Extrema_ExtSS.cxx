// Created on: 1995-07-19
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
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


#include <Adaptor3d_Surface.hxx>
#include <ElCLib.hxx>
#include <Extrema_ExtSS.hxx>
#include <Extrema_GenExtSS.hxx>
#include <Extrema_POnSurf.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_TypeMismatch.hxx>
#include <StdFail_InfiniteSolutions.hxx>
#include <StdFail_NotDone.hxx>

Extrema_ExtSS::Extrema_ExtSS() 
{
  myDone = Standard_False;
}

Extrema_ExtSS::Extrema_ExtSS(const Adaptor3d_Surface&     S1,
			     const Adaptor3d_Surface&     S2,
			     const Standard_Real    TolS1,
			     const Standard_Real    TolS2)

{
  Initialize(S2, S2.FirstUParameter(), 
	         S2.LastUParameter(), 
	         S2.FirstVParameter(), 
	         S2.LastVParameter(), TolS2);

  Perform(S1, S1.FirstUParameter(),
	  S1.LastUParameter(), 
	  S1.FirstVParameter(), 
	  S1.LastVParameter(), TolS1);
}

Extrema_ExtSS::Extrema_ExtSS(const Adaptor3d_Surface&   S1,
			     const Adaptor3d_Surface&   S2,
			     const Standard_Real  Uinf1,	
			     const Standard_Real  Usup1,
			     const Standard_Real  Vinf1,	
			     const Standard_Real  Vsup1,
			     const Standard_Real  Uinf2,	
			     const Standard_Real  Usup2,
			     const Standard_Real  Vinf2,	
			     const Standard_Real  Vsup2,
			     const Standard_Real  TolS1,
			     const Standard_Real  TolS2)

{
  Initialize(S2, Uinf2, Usup2, Vinf2, Vsup2, TolS2);
  Perform(S1, Uinf1, Usup1, Vinf1, Vsup1, TolS1);
}


void Extrema_ExtSS::Initialize(const Adaptor3d_Surface&  S2,
			       const Standard_Real Uinf2,	
			       const Standard_Real Usup2,
			       const Standard_Real Vinf2,	
			       const Standard_Real Vsup2,
			       const Standard_Real TolS2)
{
  myS2 = (Adaptor3d_SurfacePtr)&S2;
  myIsPar = Standard_False;
  myuinf2  = Uinf2;
  myusup2  = Usup2;
  myvinf2  = Vinf2;
  myvsup2  = Vsup2;
  mytolS2  = TolS2;
  myStype  = S2.GetType();
}

				
void Extrema_ExtSS::Perform(const Adaptor3d_Surface&   S1, 	
			    const Standard_Real  Uinf1,	
			    const Standard_Real  Usup1,
			    const Standard_Real  Vinf1,	
			    const Standard_Real  Vsup1,
			    const Standard_Real  TolS1)
{
  myuinf1  = Uinf1;
  myusup1  = Usup1;
  myvinf1  = Vinf1;
  myvsup1  = Vsup1;
  mytolS1 =  TolS1;
  myPOnS1.Clear();
  myPOnS2.Clear();
  mySqDist.Clear();
  Standard_Integer i;
  GeomAbs_SurfaceType myS1type  = S1.GetType();
  const Standard_Integer NbU = 20, NbV = 20;
  
  switch(myS1type) {

    case GeomAbs_Plane: 
    {
      
      switch(myStype) {
      case GeomAbs_Plane:
	{
	  myExtElSS.Perform(S1.Plane(),myS2->Plane());
	}
	break;
      default:
	{
	  Extrema_GenExtSS Ext(S1, *myS2, NbU, NbV, mytolS1, mytolS2);
	  myDone = Ext.IsDone();
	  if (myDone) {
	    Standard_Integer NbExt = Ext.NbExt();
	    Standard_Real U1, V1,U2,V2;
	    Extrema_POnSurf PS1;
	    Extrema_POnSurf PS2;
	    for (i = 1; i <= NbExt; i++) {
	      PS1 = Ext.PointOnS1(i);
	      PS2 = Ext.PointOnS2(i);
	      PS1.Parameter(U1, V1);
	      PS2.Parameter(U2, V2);
	      if (S1.IsUPeriodic())
		U1 = ElCLib::InPeriod(U1, myuinf1, myuinf1+S1.UPeriod());
	      if (S1.IsVPeriodic())
		V1 = ElCLib::InPeriod(V1, myvinf1, myvinf1+S1.VPeriod());
	      if (myS2->IsUPeriodic())
		U2 = ElCLib::InPeriod(U2, myuinf2, myuinf2+myS2->UPeriod());
	      if (myS2->IsVPeriodic())
		V2 = ElCLib::InPeriod(V2, myvinf2, myvinf2+myS2->VPeriod());

	      if ((myuinf1-U1) <= mytolS1 && (U1-myusup1) <= mytolS1 &&
		  (myvinf1-V1) <= mytolS1 && (V1-myvsup1) <= mytolS1 &&
		  (myuinf2-U2) <= mytolS2 && (U2-myusup2) <= mytolS2 &&
		  (myvinf2-V2) <= mytolS2 && (V2-myvsup2) <= mytolS2) {
		mySqDist.Append(Ext.SquareDistance(i));
		myPOnS1.Append(Extrema_POnSurf(U1, V1, PS1.Value()));
		myPOnS2.Append(Extrema_POnSurf(U2, V2, PS2.Value()));
	      }
	    }
	  }
	  return;
	  
	}
	break;
      }
      break;
    }
  default:
    {
      Extrema_GenExtSS Ext(S1, *myS2, NbU, NbV, mytolS1, mytolS2);
      myDone = Ext.IsDone();
      if (myDone) {
	Standard_Integer NbExt = Ext.NbExt();
	Standard_Real U1, V1,U2,V2;
	Extrema_POnSurf PS1;
	Extrema_POnSurf PS2;
	for (i = 1; i <= NbExt; i++) {
	  PS1 = Ext.PointOnS1(i);
	  PS2 = Ext.PointOnS2(i);
	  PS1.Parameter(U1, V1);
	  PS2.Parameter(U2, V2);
	  if (S1.IsUPeriodic())
	    U1 = ElCLib::InPeriod(U1, myuinf1, myuinf1+S1.UPeriod());
	  if (S1.IsVPeriodic())
	    V1 = ElCLib::InPeriod(V1, myvinf1, myvinf1+S1.VPeriod());
	  if (myS2->IsUPeriodic())
	    U2 = ElCLib::InPeriod(U2, myuinf2, myuinf2+myS2->UPeriod());
	  if (myS2->IsVPeriodic())
	    V2 = ElCLib::InPeriod(V2, myvinf2, myvinf2+myS2->VPeriod());
	  
	  if ((myuinf1-U1) <= mytolS1 && (U1-myusup1) <= mytolS1 &&
	      (myvinf1-V1) <= mytolS1 && (V1-myvsup1) <= mytolS1 &&
	      (myuinf2-U2) <= mytolS2 && (U2-myusup2) <= mytolS2 &&
	      (myvinf2-V2) <= mytolS2 && (V2-myvsup2) <= mytolS2) {
	    mySqDist.Append(Ext.SquareDistance(i));
	    myPOnS1.Append(Extrema_POnSurf(U1, V1, PS1.Value()));
	    myPOnS2.Append(Extrema_POnSurf(U2, V2, PS2.Value()));
	  }
	}
      }
      return;
      
    }
    break;
  }

  myDone = myExtElSS.IsDone();
  if (myDone) {
    myIsPar = myExtElSS.IsParallel();
    if (myIsPar) {
      mySqDist.Append(myExtElSS.SquareDistance(1));
    }
    else {
      Standard_Integer NbExt = myExtElSS.NbExt();
      Standard_Real U1, V1, U2, V2;
      Extrema_POnSurf PS1;
      Extrema_POnSurf PS2;
      for (i = 1; i <= NbExt; i++) {
	myExtElSS.Points(i, PS1, PS2);
	PS1.Parameter(U1, V1);
	PS2.Parameter(U2, V2);
	if ((myuinf1-U1) <= mytolS1 && (U1-myusup1) <= mytolS1 &&
	    (myvinf1-V1) <= mytolS1 && (V1-myvsup1) <= mytolS1 &&
	    (myuinf2-U2) <= mytolS2 && (U2-myusup2) <= mytolS2 &&
	    (myvinf2-V2) <= mytolS2 && (V2-myvsup2) <= mytolS2) {
	  mySqDist.Append(myExtElSS.SquareDistance(i));
	  myPOnS1.Append(PS1);
	  myPOnS2.Append(PS2);
	}
      }
    }
  }
  
}


Standard_Boolean Extrema_ExtSS::IsDone() const
{
  return myDone;
}

Standard_Boolean Extrema_ExtSS::IsParallel() const
{
  return myIsPar;
}


Standard_Real Extrema_ExtSS::SquareDistance(const Standard_Integer N) const
{
  if(!myDone) throw StdFail_NotDone();
  if (myIsPar && N != 1) throw StdFail_InfiniteSolutions();
  if ((N < 1) || (N > mySqDist.Length())) throw Standard_OutOfRange();
  return mySqDist.Value(N);
}


Standard_Integer Extrema_ExtSS::NbExt() const
{
  if(!myDone) throw StdFail_NotDone();
  return mySqDist.Length();
}



void Extrema_ExtSS::Points(const Standard_Integer N,
			   Extrema_POnSurf&       P1,
			   Extrema_POnSurf&       P2) const
{
  if(!myDone) throw StdFail_NotDone();
  P1 = myPOnS1.Value(N);
  P2 = myPOnS2.Value(N);
}
