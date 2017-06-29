// Created on: 1997-03-04
// Created by: Prestataire Xuan PHAM PHU
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

// Modified: eap Mar 25 2002 (occ102,occ227), touch case

#include <gp_Dir.hxx>
#include <Precision.hxx>
#include <TopAbs.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_State.hxx>
#include <TopTrans_SurfaceTransition.hxx>

static Standard_Boolean STATIC_DEFINED = Standard_False;

static gp_Dir FUN_nCinsideS(const gp_Dir& tgC, const gp_Dir& ngS)
{
  // Give us a curve C on suface S, <parOnC>, a parameter
  // Purpose : compute normal vector to C, tangent to S at
  //           given point , oriented INSIDE S  
  // <tgC> : geometric tangent at point of <parOnC>
  // <ngS> : geometric normal at point of <parOnC> 
  gp_Dir XX(ngS^tgC);
  return XX;  
}

#define M_REVERSED(st) (st == TopAbs_REVERSED)
#define M_INTERNAL(st) (st == TopAbs_INTERNAL)
#define M_UNKNOWN(st) (st == TopAbs_UNKNOWN)

static Standard_Integer FUN_OO(const Standard_Integer i)
{
  if (i == 1) return 2;
  if (i == 2) return 1;
  return 0;
}

//static Standard_Real FUN_Ang(const gp_Dir& Normref,
static Standard_Real FUN_Ang(const gp_Dir& ,
                             const gp_Dir& beafter,
                             const gp_Dir& TgC,
		             const gp_Dir& Norm,
                             const TopAbs_Orientation O)
{
  gp_Dir dironF = FUN_nCinsideS(TgC,Norm);
  if (M_REVERSED(O)) dironF.Reverse();

  Standard_Real ang = beafter.AngleWithRef(dironF,TgC);
  return ang;
}

static void FUN_getSTA(const Standard_Real Ang, const Standard_Real tola,
		       Standard_Integer& i, Standard_Integer& j)
{
  Standard_Real cos = Cos(Ang);
  Standard_Real sin = Sin(Ang);
  Standard_Boolean nullcos = Abs(cos) < tola;
  Standard_Boolean nullsin = Abs(sin) < tola;
  if (nullcos) i = 0;
  else i = (cos > 0.) ? 1 : 2;
  if (nullsin) j = 0;
  else j = (sin > 0.) ? 1 : 2;
}

/*static void FUN_getSTA(const Standard_Real Ang, const Standard_Real tola,
		       const Standard_Real Curv, const Standard_Real CurvRef,
		       Standard_Integer& i, Standard_Integer& j)
{
  // Choosing UV referential (beafter,myNorm).
  // purpose : computes position boundary face relative to the reference surface
  //  notice : j==0 =>  j==1 : the boundary face is ABOVE the reference surface
  //                    j==2 : the boundary face is UNDER the reference surface
  //  - j==0 : the boundary and the reference objects are tangent-       

  FUN_getSTA(Ang,tola,i,j);
  if (j == 0) {
      Standard_Real diff = Curv - CurvRef;
      if (Abs(diff) < tola) {STATIC_DEFINED = Standard_False; return;} // nyi FUN_Raise      
      j = (diff < 0.) ? 1 : 2; 
  }
}*/
#ifndef OCCT_DEBUG
#define M_Unknown   (-100)
#else
#define M_Unknown   (-100.)
#endif
#define M_noupdate  (0)
#define M_updateREF (1)
#define M_Ointernal (10)
static Standard_Integer FUN_refnearest(const Standard_Real Angref, const TopAbs_Orientation Oriref,
			  const Standard_Real Ang, const TopAbs_Orientation Ori, const Standard_Real tola)
{
  Standard_Boolean undef = (Angref == 100.);
  if (undef) return M_updateREF;

  Standard_Real cosref = Cos(Angref), cos = Cos(Ang);
  Standard_Real dcos = Abs(cosref) - Abs(cos);
  if (Abs(dcos) < tola) {
    // Analysis for tangent cases : if two boundary faces are same sided
    // and have tangent normals, if they have opposite orientations
    // we choose INTERNAL as resulting complex transition (case EXTERNAL
    // refering to no logical case)
    if (TopAbs::Complement(Ori) == Oriref) return M_Ointernal;
    else return (Standard_Integer ) M_Unknown; // nyi FUN_RAISE
  }
  Standard_Integer updateref = (dcos > 0.)? M_noupdate : M_updateREF;
  return updateref;
}

//=======================================================================
//function : FUN_refnearest
//purpose  : 
//=======================================================================

static Standard_Integer FUN_refnearest(const Standard_Integer i,
				       const Standard_Integer j,
				       const Standard_Real CurvSref,
				       const Standard_Real Angref,
				       const TopAbs_Orientation Oriref,
				       const Standard_Real Curvref,
				       const Standard_Real Ang,
				       const TopAbs_Orientation Ori,
				       const Standard_Real Curv,
				       const Standard_Real tola,
				       Standard_Boolean &  TouchFlag) // eap Mar 25 2002 
{
  Standard_Boolean iisj = (i == j);
  Standard_Real abscos = Abs(Cos(Ang));
  Standard_Boolean i0 = (Abs(1. - abscos) < tola);
  Standard_Boolean j0 = (abscos < tola);  
  Standard_Boolean nullcurv = (Curv == 0.);
  Standard_Boolean curvpos  = (Curv > tola);
  Standard_Boolean curvneg  = (Curv < -tola);
  Standard_Boolean nullcsref = (CurvSref == 0.);

  Standard_Boolean undef = (Angref == 100.);
  if (undef) {
    if (i0) {
      if (iisj  && curvneg) return M_noupdate;
      if (!iisj && curvpos) return M_noupdate;
    } 
    if (j0) {
      if (!nullcsref && (j == 1) && iisj  && (curvpos || nullcurv)) return M_updateREF;
      if (!nullcsref && (j == 1) && !iisj && (curvneg || nullcurv)) return M_updateREF;
      
      if (iisj  && curvpos) return M_noupdate;
      if (!iisj && curvneg) return M_noupdate;
    }
    return M_updateREF;
  } // undef
  
  Standard_Real cosref = Cos(Angref), cos = Cos(Ang);
  Standard_Real dcos = Abs(cosref) - Abs(cos); Standard_Boolean samecos = Abs(dcos) < tola;
  if (samecos) {
    // Analysis for tangent cases : if two boundary faces are same sided
    // and have sma dironF.
    
    if (Abs(Curvref - Curv) < 1.e-4) {
      if (TopAbs::Complement(Ori) == Oriref) return M_Ointernal;
      else return (Standard_Integer ) M_Unknown; // nyi FUN_RAISE
    }

    Standard_Boolean noupdate = Standard_False;
    if (iisj  && (Curvref > Curv)) noupdate = Standard_True;
    if (!iisj && (Curvref < Curv)) noupdate = Standard_True;
    Standard_Integer updateref = noupdate ? M_noupdate : M_updateREF;
    if (!j0) return updateref;
    
    if (!noupdate && !nullcsref) {
      // check for (j==1) the face is ABOVE Sref
      // check for (j==2) the face is BELOW Sref
      if ((j == 2) && (Abs(Curv) < CurvSref)) updateref = M_noupdate;
      if ((j == 1) && (Abs(Curv) > CurvSref)) updateref = M_noupdate;
    }
    return updateref;
  } // samecos

  Standard_Integer updateref = (dcos > 0.)? M_noupdate : M_updateREF;
  if (Oriref != Ori) TouchFlag = Standard_True; // eap Mar 25 2002
  
  return updateref;
}

// ============================================================
//                       methods
// ============================================================

TopTrans_SurfaceTransition::TopTrans_SurfaceTransition()
: myAng(1,2,1,2),myCurv(1,2,1,2),myOri(1,2,1,2)
{
  STATIC_DEFINED = Standard_False;
}

void TopTrans_SurfaceTransition::Reset(const gp_Dir& Tgt,
				       const gp_Dir& Norm,
				       const gp_Dir& MaxD,const gp_Dir& MinD,
				       const Standard_Real MaxCurv,const Standard_Real MinCurv)
{
  STATIC_DEFINED = Standard_True;

  Standard_Real tola = Precision::Angular();
  Standard_Boolean curismax = (Abs(MaxD.Dot(myTgt)) < tola);
  Standard_Boolean curismin = (Abs(MinD.Dot(myTgt)) < tola);

  if ((Abs(MaxCurv) < tola) && (Abs(MinCurv) < tola)) {
    Reset(Tgt,Norm);
    return;
  }

  if (!curismax && !curismin) {
    // In the plane normal to <myTgt>, we see the boundary face as
    // a boundary curve.
    // NYIxpu : compute the curvature of the curve if not MaxCurv
    //          nor MinCurv.

    STATIC_DEFINED = Standard_False;
    return; 
  }
  
  if (curismax) myCurvRef = Abs(MaxCurv); 
  if (curismin) myCurvRef = Abs(MinCurv);
  if (myCurvRef < tola) myCurvRef = 0.;

  // ============================================================
  // recall : <Norm> is oriented OUTSIDE the "geometric matter" described
  //          by the surface  
  //          -  if (myCurvRef != 0.) Sref is UNDER axis (sin = 0)
  //             referential (beafter,myNorm,myTgt)  -
  // ============================================================

  // beafter oriented (before, after) the intersection on the reference surface.
  myNorm = Norm; 
  myTgt = Tgt;
  beafter = Norm^Tgt; 
  for (Standard_Integer i = 1; i <=2; i++)
    for (Standard_Integer j = 1; j <=2; j++) 
      myAng(i,j) = 100.;

  myTouchFlag = Standard_False;  // eap Mar 25 2002 
}

void TopTrans_SurfaceTransition::Reset(const gp_Dir& Tgt,
				       const gp_Dir& Norm) 
{
  STATIC_DEFINED = Standard_True;

  // beafter oriented (before, after) the intersection on the reference surface.
  myNorm = Norm; 
  myTgt = Tgt;
  beafter = Norm^Tgt; 
  for (Standard_Integer i = 1; i <=2; i++)
    for (Standard_Integer j = 1; j <=2; j++) 
      myAng(i,j) = 100.;

  myCurvRef = 0.;
  myTouchFlag = Standard_False;  // eap Mar 25 2002 
}

void TopTrans_SurfaceTransition::Compare
//(const Standard_Real Tole,
(const Standard_Real ,
 const gp_Dir& Norm,
 const gp_Dir& MaxD,const gp_Dir& MinD,
 const Standard_Real MaxCurv,const Standard_Real MinCurv,
 const TopAbs_Orientation S,
 const TopAbs_Orientation O) 
{
  if (!STATIC_DEFINED) return;

  Standard_Real Curv=0.; 
  // ------
  Standard_Real tola = Precision::Angular();
  Standard_Boolean curismax = (Abs(MaxD.Dot(myTgt)) < tola);
  Standard_Boolean curismin = (Abs(MinD.Dot(myTgt)) < tola);
  if (!curismax && !curismin) {
    // In the plane normal to <myTgt>, we see the boundary face as
    // a boundary curve.
    // NYIxpu : compute the curvature of the curve if not MaxCurv
    //          nor MinCurv.

    STATIC_DEFINED = Standard_False;
    return; 
  }  
  if (curismax) Curv = Abs(MaxCurv); 
  if (curismin) Curv = Abs(MinCurv);
  if (myCurvRef < tola) Curv = 0.;
  gp_Dir dironF = FUN_nCinsideS(myTgt,Norm);
  Standard_Real prod = (dironF^Norm).Dot(myTgt);
  if (prod < 0.) Curv = -Curv;

  Standard_Real Ang;
  // -----
  Ang = ::FUN_Ang(myNorm,beafter,myTgt,Norm,O);

  Standard_Integer i,j; 
  // -----
  // i = 0,1,2 : cos = 0,>0,<0
  // j = 0,1,2 : sin = 0,>0,<0
  ::FUN_getSTA(Ang,tola,i,j);

  // update nearest :
  // ---------------
  Standard_Integer kmax = M_INTERNAL(O) ? 2 : 1;
  for (Standard_Integer k=1; k <=kmax; k++) {
    if (k == 2) {
      // get the opposite Ang
      i = ::FUN_OO(i);
      j = ::FUN_OO(j);
    }
    Standard_Boolean i0 = (i == 0), j0 = (j == 0);
    Standard_Integer nmax = (i0 || j0) ? 2 : 1;
    for (Standard_Integer n=1; n<=nmax; n++) { 
      if (i0) i = n;
      if (j0) j = n;
  
      // if (curvref == 0.) :
//      Standard_Boolean iisj = (i == j);
//      Standard_Boolean Curvpos = (Curv > 0.);
//      if ((Curv != 0.) && i0)  {
//	if (iisj  && !Curvpos) continue;
//	if (!iisj &&  Curvpos) continue;
//      }
//      if ((Curv != 0.) && j0)  {
//	if (iisj  && Curvpos)  continue;
//	if (!iisj && !Curvpos) continue;
//      }

      Standard_Integer refn = ::FUN_refnearest(i,j,myCurvRef,myAng(i,j),myOri(i,j),myCurv(i,j),
				  Ang,/*O*/S,Curv,tola,myTouchFlag); // eap Mar 25 2002 
      if (refn == M_Unknown) {STATIC_DEFINED = Standard_False; return;}
      if (refn > 0) {
	myAng(i,j)  = Ang;
	myOri(i,j)  = (refn == M_Ointernal) ? TopAbs_INTERNAL : S;	
	myCurv(i,j) = Curv;
      }
    } // n=1..nmax
  } // k=1..kmax

}

void TopTrans_SurfaceTransition::Compare
//(const Standard_Real Tole,
(const Standard_Real ,
 const gp_Dir& Norm,
 const TopAbs_Orientation S,
 const TopAbs_Orientation O) 
{
  if (!STATIC_DEFINED) return;

  // oriented Ang(beafter,dironF), 
  // dironF normal to the curve, oriented INSIDE F, the added oriented support
  Standard_Real Ang = ::FUN_Ang(myNorm,beafter,myTgt,Norm,O);
  Standard_Real tola = Precision::Angular(); // nyi in arg
    
  // i = 0,1,2 : cos = 0,>0,<0
  // j = 0,1,2 : sin = 0,>0,<0
  Standard_Integer i,j; ::FUN_getSTA(Ang,tola,i,j);

  Standard_Integer kmax = M_INTERNAL(O) ? 2 : 1;
  for (Standard_Integer k=1; k <=kmax; k++) {
    if (k == 2) {
      // get the opposite Ang
      i = ::FUN_OO(i);
      j = ::FUN_OO(j);
    }

    Standard_Boolean i0 = (i == 0), j0 = (j == 0);
    Standard_Integer nmax = (i0 || j0) ? 2 : 1;
    for (Standard_Integer n=1; n<=nmax; n++) { 
      if (i0) i = n;
      if (j0) j = n;
      
      Standard_Integer refn = ::FUN_refnearest(myAng(i,j),myOri(i,j),
				  Ang,/*O*/S,tola);   // eap
      if (refn == M_Unknown) {STATIC_DEFINED = Standard_False; return;}
   
      if (refn > 0) {
	myAng(i,j) = Ang;
	myOri(i,j) = (refn == M_Ointernal) ? TopAbs_INTERNAL : S;	
      }
    } // n=1..nmax
  } // k=1..kmax
}

#define BEFORE (2)
#define AFTER  (1)
static TopAbs_State FUN_getstate(const TColStd_Array2OfReal& Ang,
				 const TopTrans_Array2OfOrientation& Ori,
				 const Standard_Integer iSTA,
				 const Standard_Integer iINDEX)
{	
  if (!STATIC_DEFINED) return TopAbs_UNKNOWN;
 
  Standard_Real a1 = Ang(iSTA,1), a2 = Ang(iSTA,2);
  Standard_Boolean undef1 = (a1 == 100.), undef2 = (a2 == 100.);
  Standard_Boolean undef = undef1 && undef2;
  if (undef) return TopAbs_UNKNOWN;
  
  if (undef1 || undef2) {
    Standard_Integer jok = undef1 ? 2 : 1;
    TopAbs_Orientation o = Ori(iSTA,jok);
    TopAbs_State st = (iINDEX == BEFORE) ? TopTrans_SurfaceTransition::GetBefore(o) :
      TopTrans_SurfaceTransition::GetAfter(o);
    return st;
  }
  
  TopAbs_Orientation o1 = Ori(iSTA,1), o2 = Ori(iSTA,2);
  TopAbs_State st1 = (iINDEX == BEFORE) ? TopTrans_SurfaceTransition::GetBefore(o1) : 
    TopTrans_SurfaceTransition::GetAfter(o1);
  TopAbs_State st2 = (iINDEX == BEFORE) ? TopTrans_SurfaceTransition::GetBefore(o2) : 
    TopTrans_SurfaceTransition::GetAfter(o2);
  if (st1 != st2) return TopAbs_UNKNOWN; // Incoherent data
  return st1;
}


TopAbs_State TopTrans_SurfaceTransition::StateBefore() const
{
  if (!STATIC_DEFINED) return TopAbs_UNKNOWN;

  // we take the state before of before orientations
  TopAbs_State before = ::FUN_getstate(myAng,myOri,BEFORE,BEFORE);
  if (M_UNKNOWN(before)) {
    // looking back in before for defined states
    // we take the state before of after orientations
    before = ::FUN_getstate(myAng,myOri,AFTER,BEFORE);
    // eap Mar 25 2002 
    if (myTouchFlag) {
      if (before == TopAbs_OUT) before = TopAbs_IN;
      else if (before == TopAbs_IN) before = TopAbs_OUT;
    }
  }
  return before;
}

TopAbs_State TopTrans_SurfaceTransition::StateAfter() const
{
  if (!STATIC_DEFINED) return TopAbs_UNKNOWN;

  TopAbs_State after = ::FUN_getstate(myAng,myOri,AFTER,AFTER);
  if (M_UNKNOWN(after)) {
    // looking back in before for defined states
    after = ::FUN_getstate(myAng,myOri,BEFORE,AFTER);
    // eap Mar 25 2002 
    if (myTouchFlag) {
      if (after == TopAbs_OUT) after = TopAbs_IN;
      else if (after == TopAbs_IN) after = TopAbs_OUT;
    }
  }
  return after;
}

TopAbs_State TopTrans_SurfaceTransition::GetBefore
(const TopAbs_Orientation Tran)
{
  if (!STATIC_DEFINED) return TopAbs_UNKNOWN;

  switch (Tran)
    {
    case TopAbs_FORWARD  :
    case TopAbs_EXTERNAL :
      return TopAbs_OUT;
    case TopAbs_REVERSED :
    case TopAbs_INTERNAL :
      return TopAbs_IN;
    }
  return TopAbs_OUT;
}

TopAbs_State TopTrans_SurfaceTransition::GetAfter
(const TopAbs_Orientation Tran)
{
  if (!STATIC_DEFINED) return TopAbs_UNKNOWN;

  switch (Tran)
    {
    case TopAbs_FORWARD  :
    case TopAbs_INTERNAL :
      return TopAbs_IN;
    case TopAbs_REVERSED :
    case TopAbs_EXTERNAL :
      return TopAbs_OUT;
    }
  return TopAbs_OUT;
}
