// Created on: 1992-01-23
// Created by: Didier PIFFAULT
// Copyright (c) 1992-1999 Matra Datavision
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


#include <gp_Dir.hxx>
#include <TopAbs.hxx>
#include <TopTrans_CurveTransition.hxx>

#define GREATER 1
#define SAME 0
#define LOWER -1

//=======================================================================
//function : TopTrans_CurveTransition
//purpose  : Empty Constructor.
//=======================================================================

TopTrans_CurveTransition::TopTrans_CurveTransition ()
{
}

//=======================================================================
//function : Reset
//purpose  : Initializer for a complex curve transition with the elements
//           of the intersecting curve.
//=======================================================================

void TopTrans_CurveTransition::Reset (const gp_Dir& Tgt,
				      const gp_Dir& Norm,
				      const Standard_Real Curv)
{
       myTgt=Tgt; myNorm=Norm; myCurv=Curv; Init=Standard_True;
}

//=======================================================================
//function : Reset
//purpose  : Initializer for a complex curve transition with the elements
//           of the intersecting straight line.
//=======================================================================

void TopTrans_CurveTransition::Reset (const gp_Dir& Tgt)
{
       myTgt=Tgt; myCurv=0.; Init=Standard_True;
}

//=======================================================================
//function : Compare
//purpose  : Compare the elements of  an interference  on  an intersected
//           curve with the interference stored in the complex Transition.
//=======================================================================

void TopTrans_CurveTransition::Compare (const Standard_Real Tole,
					const gp_Dir& T,
					const gp_Dir& N,
					const Standard_Real C,
					const TopAbs_Orientation St,
					const TopAbs_Orientation Or)
{
  // S is the transition, how the curve cross the boundary
  // O is the orientation, how the intersection is set on the boundary
  TopAbs_Orientation S = St;
  TopAbs_Orientation O = Or;

  // adjustment for INTERNAL transition
  if (S == TopAbs_INTERNAL) {
    if (T * myTgt < 0)
      S = TopAbs::Reverse(O);
    else
      S = O;
  }

  // It is the first comparaison for this complex transition 
  if (Init) {
    Init=Standard_False;
    TgtFirst =T;
    NormFirst=N;
    CurvFirst=C;
    TranFirst=S;
    TgtLast  =T;
    NormLast =N;
    CurvLast =C;
    TranLast =S;
    switch (O) {
      // Interference en fin d'arete il faut inverser la tangente 
    case TopAbs_REVERSED :
      TgtFirst.Reverse();
      TgtLast.Reverse();
      break;
    case TopAbs_INTERNAL :
      // Interference en milieu d'arete il faut inverser en fonction de la
      // position de la tangente de reference
      if (myTgt*T>0) TgtFirst.Reverse();
      else           TgtLast.Reverse();
      break;
    case TopAbs_FORWARD :
    case TopAbs_EXTERNAL :
      break;
    }
  }

  // Compare with the existent first and last transition :
  else {
    Standard_Boolean FirstSet=Standard_False;
    Standard_Real cosAngWithT=myTgt*T;
    switch (O) {
    case TopAbs_REVERSED :
      cosAngWithT= -cosAngWithT;
      break;
    case TopAbs_INTERNAL :
      if (cosAngWithT>0) cosAngWithT=-cosAngWithT;
      break;
    case TopAbs_FORWARD :
    case TopAbs_EXTERNAL :
      break;
    }
    Standard_Real cosAngWith1=myTgt*TgtFirst;
    
    switch (Compare(cosAngWithT, cosAngWith1, Tole)) {
      
    case LOWER :
      // If the angle is greater than the first the new become the first
      FirstSet=Standard_True;
      TgtFirst =T;
      switch (O) {
      case TopAbs_REVERSED :
	TgtFirst.Reverse();
	break;
      case TopAbs_INTERNAL :
	if (myTgt*T>0) TgtFirst.Reverse();
	break;
      case TopAbs_FORWARD :
      case TopAbs_EXTERNAL :
	break;
      }
      NormFirst=N;
      CurvFirst=C;
      TranFirst=S;
      break;

    case SAME :
      // If same angles we look at the Curvature
      if (IsBefore(Tole, cosAngWithT, N, C, NormFirst, CurvFirst)) {
        FirstSet=Standard_True;
	TgtFirst =T;
	switch (O) {
	case TopAbs_REVERSED :
	  TgtFirst.Reverse();
	  break;
	case TopAbs_INTERNAL :
	  if (myTgt*T>0) TgtFirst.Reverse();
	  break;
	case TopAbs_FORWARD :
	case TopAbs_EXTERNAL :
	  break;
	}
	NormFirst=N;
	CurvFirst=C;
	TranFirst=S;
      }
      break;
  
    case GREATER:
      break;
    }

    if (!FirstSet || O==TopAbs_INTERNAL) {
      // Dans les cas de tangence le premier peut etre aussi le dernier
      if (O==TopAbs_INTERNAL) cosAngWithT=-cosAngWithT;
      Standard_Real cosAngWith2=myTgt*TgtLast;
      
      switch (Compare(cosAngWithT, cosAngWith2, Tole)) {
	
      case GREATER:
	// If the angle is lower than the last the new become the last
	TgtLast  =T;
	switch (O) {
	case TopAbs_REVERSED :
	  TgtLast.Reverse();
	  break;
	case TopAbs_INTERNAL :
	  if (myTgt*T<0) TgtLast.Reverse();
	  break;
	case TopAbs_FORWARD :
	case TopAbs_EXTERNAL :
	  break;
	}
	NormLast =N;
	CurvLast =C;
	TranLast =S;
	break;
	
      case SAME:
	// If the angle is the same we look at the curvature
	if (IsBefore(Tole, cosAngWithT, NormLast, CurvLast, N, C)) {
	  TgtLast =T;
	  switch (O) {
	  case TopAbs_REVERSED :
	    TgtLast.Reverse();
	    break;
	  case TopAbs_INTERNAL :
	    if (myTgt*T<0) TgtLast.Reverse();
	    break;
	  case TopAbs_FORWARD :
	  case TopAbs_EXTERNAL :
	    break;
	  }
	  NormLast=N;
	  CurvLast=C;
	  TranLast=S;
	}
      }
    }
  }
}

//=======================================================================
//function : StateBefore
//purpose  : Give the state of the curv before the interference.
//=======================================================================

TopAbs_State TopTrans_CurveTransition::StateBefore () const
{
  if (Init) return TopAbs_UNKNOWN;
  switch (TranFirst)
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

//=======================================================================
//function : StateAfter
//purpose  : give the state of the curve after the interference.
//=======================================================================

TopAbs_State TopTrans_CurveTransition::StateAfter () const
{
  if (Init) return TopAbs_UNKNOWN;
  switch (TranLast)
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


//=======================================================================
//function : IsBefore
//purpose  : Compare the curvature of the two transition and return true
//           if T1 is before T2
//=======================================================================

Standard_Boolean TopTrans_CurveTransition::IsBefore
  (const Standard_Real    Tole,
   const Standard_Real    CosAngl,
   const gp_Dir&          N1, 
   const Standard_Real    C1,
   const gp_Dir&          N2,
   const Standard_Real    C2) const
{
  Standard_Real TN1=myTgt*N1;
  Standard_Real TN2=myTgt*N2;
  Standard_Boolean OneBefore=Standard_False;

  if (Abs(TN1)<=Tole || Abs(TN2)<=Tole) {
    // Tangent : The first is the interference which have the nearest curvature
    //           from the reference.
    if (myCurv==0) {
      // The reference is straight
      // The first is the interference which have the lowest curvature.
      if (C1<C2) OneBefore=Standard_True;
//  Modified by Sergey KHROMOV - Wed Dec 27 17:08:49 2000 Begin
      if (CosAngl>0)
	OneBefore=!OneBefore;
//  Modified by Sergey KHROMOV - Wed Dec 27 17:08:50 2000 End
    }
    else {
      // The reference is curv 
      // The first is the interference which have the nearest curvature
      // in the direction
      Standard_Real deltaC1, deltaC2;
      if (C1==0. || myCurv==0.) {
	deltaC1=C1-myCurv;
      }
      else {
	deltaC1=(C1-myCurv)*(N1*myNorm);
      }
      if (C2==0. || myCurv==0.) {
	deltaC2=C2-myCurv;
      }
      else {
	deltaC2=(C2-myCurv)*(N2*myNorm);
      }
      if (deltaC1 < deltaC2) OneBefore=Standard_True;
      if (CosAngl>0) OneBefore=!OneBefore;
    }
  }
  else if (TN1<0) {
    // Before the first interference we are in the curvature
    if (TN2>0) {
      // Before the second  interference we are out the curvature
      // The first interference is before  /* ->)( */
      OneBefore=Standard_True;
    }
    else {
      // Before the second interference we are in the curvature
      if (C1>C2) {
	// We choice the greater curvature
	// The first interference is before   /* ->)) */
	OneBefore=Standard_True;
      }
    }
  }
  else if (TN1>0) {
    // Before the first interference we are out the curvature
    if (TN2>0) {
      // Before the second interference we are out the curvature /* ->(( */
      if (C1<C2) {
	// We choice the lower curvature
	// The first interference is before 
	OneBefore=Standard_True;
      }
    }
  }
  return OneBefore;
}


//=======================================================================
//function : Compare
//purpose  : Compare two angles
//=======================================================================

Standard_Integer TopTrans_CurveTransition::Compare(const Standard_Real Ang1, 
					  const Standard_Real Ang2,
					  const Standard_Real Tole) const
{
  Standard_Integer res=SAME;
  if (Ang1 - Ang2 > Tole) res=GREATER;
  else if (Ang2 - Ang1 > Tole) res=LOWER;

  return res;
}
