// File:	HLRAlgo_EdgeStatus.cxx
// Created:	Tue Feb 18 11:30:23 1992
// Author:	Christophe MARION
//		<cma@sdsun1>
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

