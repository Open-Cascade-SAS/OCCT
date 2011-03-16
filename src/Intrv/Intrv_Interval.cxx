// File:	Intrv_Interval.cxx
// Created:	Fri Dec 13 14:56:47 1991
// Author:	Christophe MARION
//		<cma@sdsun1>

#include <Intrv_Interval.ixx>

//                   **-----------****             Other
//     ***-----*                                   Before
//     ***------------*                            JustBefore
//     ***-----------------*                       OverlappingAtStart
//     ***--------------------------*              JustEnclosingAtEnd
//     ***-------------------------------------*   Enclosing
//                  ***----*                       JustOverlappingAtStart
//                  ***-------------*              Similar
//                  ***------------------------*   JustEnclosingAtStart
//                         ***-*                   Inside
//                         ***------*              JustOverlappingAtEnd
//                         ***-----------------*   OverlappingAtEnd
//                                  ***--------*   JustAfter
//                                       ***---*   After

//=======================================================================
//function : Intrv_Interval
//purpose  : 
//=======================================================================

Intrv_Interval::Intrv_Interval 
  () : myStart(RealFirst()), myEnd  (RealLast ())
{
  myTolStart = (Standard_ShortReal)Epsilon(RealFirst());
  myTolEnd   = (Standard_ShortReal)Epsilon(RealLast ());
}

//=======================================================================
//function : Intrv_Interval
//purpose  : 
//=======================================================================

Intrv_Interval::Intrv_Interval 
  (const Standard_Real Start, const Standard_Real End) :
  myStart(Start), myEnd  (End)
{
  myTolStart = (Standard_ShortReal)Epsilon(myStart);
  myTolEnd   = (Standard_ShortReal)Epsilon(myEnd);
}

//=======================================================================
//function : Intrv_Interval
//purpose  : 
//=======================================================================

Intrv_Interval::Intrv_Interval 
(const Standard_Real Start, const Standard_ShortReal TolStart,
 const Standard_Real End  , const Standard_ShortReal TolEnd) :
myStart(Start),      myEnd  (End),
myTolStart(TolStart),myTolEnd  (TolEnd)
{
  Standard_ShortReal epsStart = (Standard_ShortReal)Epsilon(myStart);
  Standard_ShortReal epsEnd   = (Standard_ShortReal)Epsilon(myEnd);
  if (myTolStart < epsStart) myTolStart = epsStart;
  if (myTolEnd   < epsEnd  ) myTolEnd   = epsEnd;
}

//=======================================================================
//function : Position
//purpose  : 
//=======================================================================

Intrv_Position Intrv_Interval::Position (const Intrv_Interval& Other) const
{
  Standard_Real mySMin =       myStart -      myTolStart;
  Standard_Real mySMax =       myStart +      myTolStart;
  Standard_Real myEMin =       myEnd   -      myTolEnd  ;
  Standard_Real myEMax =       myEnd   +      myTolEnd  ;
  Standard_Real otSMin = Other.myStart - Other.myTolStart;
  Standard_Real otSMax = Other.myStart + Other.myTolStart;
  Standard_Real otEMin = Other.myEnd   - Other.myTolEnd  ;
  Standard_Real otEMax = Other.myEnd   + Other.myTolEnd  ;
  Intrv_Position P;
  if      ( mySMax <  otSMin ) {
    if      ( myEMax <  otSMin ) P = Intrv_Before;
    else if ( otSMax >= myEMin ) P = Intrv_JustBefore;
    else if ( myEMax <  otEMin ) P = Intrv_OverlappingAtStart;
    else if ( otEMax >= myEMin ) P = Intrv_JustEnclosingAtEnd;
    else                         P = Intrv_Enclosing;
  }
  else if ( otSMax >= mySMin ) {
    if      ( myEMax <  otEMin ) P = Intrv_JustOverlappingAtStart;
    else if ( otEMax >= myEMin ) P = Intrv_Similar;
    else                         P = Intrv_JustEnclosingAtStart;
  }
  else if ( mySMax <  otEMin ) {
    if      ( myEMax <  otEMin ) P = Intrv_Inside;
    else if ( otEMax >= myEMin ) P = Intrv_JustOverlappingAtEnd;
    else                         P = Intrv_OverlappingAtEnd;
  }
  else if ( otEMax >= mySMin )   P = Intrv_JustAfter;
  else                           P = Intrv_After;
  return P;
}

