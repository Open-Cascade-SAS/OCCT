// Created on: 1992-02-18
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
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


#ifndef No_Exception
#define No_Exception
#endif
#include <HLRAlgo_EdgeStatus.ixx>

//=======================================================================
//function : HLRAlgo_EdgeStatus
//purpose  : 
//=======================================================================

HLRAlgo_EdgeStatus::HLRAlgo_EdgeStatus ()
{
}

//=======================================================================
//function : HLRAlgo_EdgeStatus
//purpose  : 
//=======================================================================

HLRAlgo_EdgeStatus::HLRAlgo_EdgeStatus (const Standard_Real Start,
					const Standard_ShortReal TolStart,
					const Standard_Real End  ,
					const Standard_ShortReal TolEnd) :
  myStart     (Start),
  myEnd       (End),
  myTolStart  (TolStart),
  myTolEnd    (TolEnd),
  myFlags     (0)
{
  ShowAll();
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void HLRAlgo_EdgeStatus::Initialize (const Standard_Real Start,
				     const Standard_ShortReal TolStart,
				     const Standard_Real End  ,
				     const Standard_ShortReal TolEnd)
{
  myStart      = Start;
  myTolStart   = TolStart;
  myEnd        = End;
  myTolEnd     = TolEnd;
  ShowAll();
}

//=======================================================================
//function : NbVisiblePart
//purpose  : 
//=======================================================================

Standard_Integer HLRAlgo_EdgeStatus::NbVisiblePart () const
{
  if      (AllHidden ()) return 0;
  else if (AllVisible()) return 1;
  else                   return myVisibles.NbIntervals();
}

//=======================================================================
//function : VisiblePart
//purpose  : 
//=======================================================================

void HLRAlgo_EdgeStatus::VisiblePart (const Standard_Integer Index,
				      Standard_Real& Start,
				      Standard_ShortReal& TolStart,
				      Standard_Real& End  ,
				      Standard_ShortReal& TolEnd  ) const
{ 
  if (AllVisible())            Bounds(Start,TolStart,End,TolEnd);
  else myVisibles.Value(Index).Bounds(Start,TolStart,End,TolEnd);
}

//=======================================================================
//function : Hide
//purpose  : 
//=======================================================================

void HLRAlgo_EdgeStatus::Hide (const Standard_Real Start     ,
			       const Standard_ShortReal TolStart,
			       const Standard_Real End       ,
			       const Standard_ShortReal TolEnd  ,
			       const Standard_Boolean OnFace ,
			       const Standard_Boolean)
{
  if (!OnFace) {
    if (AllVisible()) {
      myVisibles = Intrv_Intervals
	(Intrv_Interval(myStart,myTolStart,myEnd,myTolEnd));
      AllVisible(Standard_False);
    }
    myVisibles.Subtract(Intrv_Interval(Start,TolStart,End,TolEnd));
    if (!AllHidden()) AllHidden(myVisibles.NbIntervals() == 0);
  }
}
