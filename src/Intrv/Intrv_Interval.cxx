// Created on: 1991-12-13
// Created by: Christophe MARION
// Copyright (c) 1991-1999 Matra Datavision
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

