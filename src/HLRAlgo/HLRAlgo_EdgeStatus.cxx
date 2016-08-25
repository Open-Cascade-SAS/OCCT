// Created on: 1992-02-18
// Created by: Christophe MARION
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

#ifndef No_Exception
#define No_Exception
#endif


#include <HLRAlgo_EdgeStatus.hxx>
#include <Standard_OutOfRange.hxx>

//=======================================================================
//function : HLRAlgo_EdgeStatus
//purpose  : 
//=======================================================================
HLRAlgo_EdgeStatus::HLRAlgo_EdgeStatus()
: myStart     (0.0),
  myEnd       (0.0),
  myTolStart  (0.0f),
  myTolEnd    (0.0f),
  myAllHidden (false),
  myAllVisible(false)
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
  myAllHidden (false),
  myAllVisible(false)
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
