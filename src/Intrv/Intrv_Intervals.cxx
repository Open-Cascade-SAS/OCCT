// Created on: 1991-12-13
// Created by: Christophe MARION
// Copyright (c) 1991-1999 Matra Datavision
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


#include <Intrv_Interval.hxx>
#include <Intrv_Intervals.hxx>
#include <Standard_OutOfRange.hxx>

//                   **---------****             Other
//     ***-----*                                 IsBefore
//     ***------------*                          IsJustBefore
//     ***-----------------*                     IsOverlappingAtStart
//     ***------------------------*              IsJustEnclosingAtEnd
//     ***-----------------------------------*   IsEnclosing
//                  ***----*                     IsJustOverlappingAtStart
//                  ***-----------*              IsSimilar
//                  ***----------------------*   IsJustEnclosingAtStart
//                       ***-*                   IsInside
//                       ***------*              IsJustOverlappingAtEnd
//                       ***-----------------*   IsOverlappingAtEnd
//                                ***--------*   IsJustAfter
//                                     ***---*   IsAfter
//=======================================================================
//function : Intrv_Intervals
//purpose  :
//=======================================================================
Intrv_Intervals::Intrv_Intervals ()
{}

//=======================================================================
//function : Intrv_Intervals
//purpose  :
//=======================================================================

Intrv_Intervals::Intrv_Intervals (const Intrv_Interval& Int)
{ myInter.Append(Int); }

//=======================================================================
//function : Intrv_Intervals
//purpose  :
//=======================================================================

Intrv_Intervals::Intrv_Intervals (const Intrv_Intervals& Int)
{ myInter = Int.myInter; }

//=======================================================================
//function : Intersect
//purpose  :
//=======================================================================

void Intrv_Intervals::Intersect (const Intrv_Interval& Tool)
{
  Intrv_Intervals Inter(Tool);
  Intersect(Inter);
}

//=======================================================================
//function : Intersect
//purpose  :
//=======================================================================

void Intrv_Intervals::Intersect (const Intrv_Intervals& Tool)
{
  Intrv_Intervals XUni(*this);
  XUni.XUnite(Tool);
  Unite(Tool);
  Subtract(XUni);
}

//=======================================================================
//function : Subtract
//purpose  :
//=======================================================================

void Intrv_Intervals::Subtract (const Intrv_Interval& Tool)
{
  Standard_Integer index = 1;

  while (index <= myInter.Length()) {

    switch (Tool.Position (myInter(index))) {

    case Intrv_Before :
      index = myInter.Length();                       // sortir
      break;

    case Intrv_JustBefore :
      myInter(index).CutAtStart
	(Tool.End  (),Tool.TolEnd  ());               // modifier le debut
      index = myInter.Length();                       // sortir
      break;

    case Intrv_OverlappingAtStart :
    case Intrv_JustOverlappingAtStart :
      myInter(index).SetStart
	(Tool.End  (),Tool.TolEnd  ());               // garder la fin
      index = myInter.Length();                       // sortir
      break;

    case Intrv_JustEnclosingAtEnd :
    case Intrv_Enclosing :
    case Intrv_Similar :
    case Intrv_JustEnclosingAtStart :
      myInter.Remove(index);                          // detruire et
      index--;                                        // continuer
      break;

    case Intrv_Inside :
      myInter.InsertAfter(index,myInter(index));
      myInter(index  ).SetEnd
	(Tool.Start(),Tool.TolStart());               // garder le debut
      myInter(index+1).SetStart
	(Tool.End  (),Tool.TolEnd  ());               // garder la fin
      index = myInter.Length();                       // sortir
      break;

    case Intrv_JustOverlappingAtEnd :
    case Intrv_OverlappingAtEnd :
      myInter(index).SetEnd
	(Tool.Start(),Tool.TolStart());               // garder le debut
      break;                                          // continuer

    case Intrv_JustAfter :
      myInter(index).CutAtEnd
	(Tool.Start(),Tool.TolStart());               // modifier la fin
      break;                                          // continuer

    case Intrv_After :
      break;                                          // continuer

    }
    index++;
  }
}

//=======================================================================
//function : Subtract
//purpose  :
//=======================================================================

void Intrv_Intervals::Subtract (const Intrv_Intervals& Tool)
{
  Standard_Integer index;
  for (index = 1; index <= Tool.myInter.Length(); index++)
    Subtract (Tool.myInter(index));
}

//=======================================================================
//function : Unite
//purpose  :
//=======================================================================

void Intrv_Intervals::Unite (const Intrv_Interval& Tool)
{
  Standard_Boolean Inserted = Standard_False;
  Intrv_Interval Tins(Tool);
  Standard_Integer index = 1;

  while (index <= myInter.Length()) {

    switch (Tins.Position (myInter(index))) {
	
    case Intrv_Before :
      Inserted = Standard_True;
      myInter.InsertBefore(index,Tins);               // inserer avant et
      index = myInter.Length();                       // sortir
      break;

    case Intrv_JustBefore :
    case Intrv_OverlappingAtStart :
      Inserted = Standard_True;
      myInter(index).SetStart
	(Tins.Start(),Tins.TolStart());               // changer le debut
      index = myInter.Length();                       // sortir
      break;

    case Intrv_Similar :
      Tins.FuseAtStart(myInter(index).Start(),
		       myInter(index).TolStart());    // modifier le debut
    case Intrv_JustEnclosingAtEnd :
      Tins.FuseAtEnd  (myInter(index).End  (),
		       myInter(index).TolEnd  ());    // modifier la fin
    case Intrv_Enclosing :
      myInter.Remove(index);                          // detruire et
      index--;                                        // continuer
      break;

    case Intrv_JustOverlappingAtEnd :
      Tins.SetStart   (myInter(index).Start(),
		       myInter(index).TolStart());    // changer le debut
      Tins.FuseAtEnd  (myInter(index).End  (),
		       myInter(index).TolEnd  ());    // modifier la fin
      myInter.Remove(index);                          // detruire et
      index--;                                        // continuer
      break;

    case Intrv_JustOverlappingAtStart :
      Inserted = Standard_True;
      myInter(index).FuseAtStart
	(Tins.Start(),Tins.TolStart());               // modifier le debut
      index = myInter.Length();                       // sortir
      break;

    case Intrv_JustEnclosingAtStart :
      Tins.FuseAtStart(myInter(index).Start(),
		       myInter(index).TolStart());    // modifier le debut
      myInter.Remove(index);                          // detruire et
      index--;                                        // continuer
      break;

    case Intrv_Inside :
      Inserted = Standard_True;
      index = myInter.Length();                       // sortir
      break;

    case Intrv_OverlappingAtEnd :
    case Intrv_JustAfter :
      Tins.SetStart(myInter(index).Start(),
		    myInter(index).TolStart());       // changer le debut
      myInter.Remove(index);                          // detruire et
      index--;                                        // continuer
      break;

    case Intrv_After :
      break;                                          // continuer

    }
    index++;
  }
  if ( !Inserted ) myInter.Append (Tins);
}

//=======================================================================
//function : Unite
//purpose  :
//=======================================================================

void Intrv_Intervals::Unite (const Intrv_Intervals& Tool)
{
  Standard_Integer index;
  for (index = 1; index<=Tool.myInter.Length(); index++)
    Unite (Tool.myInter(index));
}

//=======================================================================
//function : XUnite
//purpose  :
//=======================================================================

void Intrv_Intervals::XUnite (const Intrv_Interval& Tool)
{
  Intrv_Intervals Inter(Tool);
  XUnite(Inter);
}

//=======================================================================
//function : XUnite
//purpose  :
//=======================================================================

void Intrv_Intervals::XUnite (const Intrv_Intervals& Tool)
{
  Intrv_Intervals Sub2(Tool);
  Sub2.Subtract(*this);
  Subtract(Tool);
  Unite(Sub2);
}

