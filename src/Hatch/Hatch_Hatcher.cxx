// Created on: 1992-08-19
// Created by: Modelistation
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


#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <Hatch_Hatcher.hxx>
#include <Hatch_Line.hxx>
#include <Hatch_Parameter.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <Standard_OutOfRange.hxx>

//=======================================================================
//function : Hatch_Hatcher
//purpose  : 
//=======================================================================
Hatch_Hatcher::Hatch_Hatcher(const Standard_Real Tol,
			     const Standard_Boolean Oriented) :
       myToler(Tol),
       myOrient(Oriented)
{
}

//=======================================================================
//function : AddLine
//purpose  : 
//=======================================================================

void  Hatch_Hatcher::AddLine(const gp_Lin2d& L, const Hatch_LineForm T)
{
  Hatch_Line HL(L,T);
  myLines.Append(HL);
}

//=======================================================================
//function : AddLine
//purpose  : 
//=======================================================================

void  Hatch_Hatcher::AddLine(const gp_Dir2d& D, 
			     const Standard_Real Dist)
{
  Standard_Real X = D.X();
  Standard_Real Y = D.Y();
  gp_Pnt2d O(-Y * Dist, X * Dist);
  gp_Lin2d L(O,D);
  AddLine(L,Hatch_ANYLINE);
}

//=======================================================================
//function : AddXLine
//purpose  : 
//=======================================================================

void  Hatch_Hatcher::AddXLine(const Standard_Real X)
{
  gp_Pnt2d O(X,0);
  gp_Dir2d D(0,1);
  gp_Lin2d L(O,D);
  AddLine(L,Hatch_XLINE);
}

//=======================================================================
//function : AddYLine
//purpose  : 
//=======================================================================

void  Hatch_Hatcher::AddYLine(const Standard_Real Y)
{
  gp_Pnt2d O(0,Y);
  gp_Dir2d D(1,0);
  gp_Lin2d L(O,D);
  AddLine(L,Hatch_YLINE);
}

//=======================================================================
//function : Trim
//purpose  : 
//=======================================================================

void  Hatch_Hatcher::Trim
  (const gp_Lin2d& L,
   const Standard_Integer Index)
{
  Trim(L,RealFirst(),RealLast(),Index);
}

//=======================================================================
//function : Trim
//purpose  : 
//=======================================================================

void  Hatch_Hatcher::Trim
  (const gp_Lin2d& L,
   const Standard_Real Start,
   const Standard_Real End,
   const Standard_Integer Index)
{
  IntAna2d_IntPoint        Pinter;
  IntAna2d_AnaIntersection Inters;
  Standard_Integer         iLine;
  for (iLine = 1; iLine <= myLines.Length(); iLine++) {
    Inters.Perform(myLines(iLine).myLin,L);
    if (Inters.IsDone()) {
      if (!Inters.IdenticalElements() && !Inters.ParallelElements()) {
	// we have got something
	Pinter  = Inters.Point(1);
	Standard_Real linePar = Pinter.ParamOnSecond();
	if (linePar -   Start < - myToler) continue;
	if (linePar -   End   >   myToler) continue;
	Standard_Real norm = L.Direction() ^ myLines(iLine).myLin.Direction();
	if (linePar -   Start <   myToler) {
	  // on the limit of the trimming segment
	  // accept if the other extremity is on the left
	  if (norm < 0) continue;
	}
	if (linePar -   End   >  -myToler) {
	  // on the limit of the trimming segment
	  // accept if the other extremity is on the left
	  if (norm > 0) continue;
	}
	// insert the parameter
	myLines(iLine).AddIntersection (Pinter.ParamOnFirst(),
					norm > 0,
					Index,
					Pinter.ParamOnSecond(),
					myToler);
      }
    }
  }
}

//=======================================================================
//function : Trim
//purpose  : 
//=======================================================================

void  Hatch_Hatcher::Trim
  (const gp_Pnt2d& P1, 
   const gp_Pnt2d& P2,
   const Standard_Integer Index)
{
  gp_Vec2d V(P1,P2);
  if (Abs(V.X()) > .9 * RealLast())
    V.Multiply(1/V.X());
  else if (Abs(V.Y()) > .9 * RealLast())
    V.Multiply(1/V.Y());
  if (V.Magnitude() > myToler) {
    gp_Dir2d D(V);
    gp_Lin2d L(P1,D);
    Trim(L,0,P1.Distance(P2),Index);
  }
}

//=======================================================================
//function : NbIntervals
//purpose  : 
//=======================================================================

Standard_Integer Hatch_Hatcher::NbIntervals() const
{
  Standard_Integer i, nb = 0;
  for (i = 1; i <= myLines.Length(); i++)
    nb += NbIntervals(i);
  return nb;
}

//=======================================================================
//function : NbLines
//purpose  : 
//=======================================================================

Standard_Integer Hatch_Hatcher::NbLines() const
{
  return myLines.Length();
}

//=======================================================================
//function : Line
//purpose  : 
//=======================================================================

const gp_Lin2d& Hatch_Hatcher::Line(const Standard_Integer I) const
{
  return myLines(I).myLin;
}

//=======================================================================
//function : LineForm
//purpose  : 
//=======================================================================

Hatch_LineForm Hatch_Hatcher::LineForm(const Standard_Integer I) const
{
  return myLines(I).myForm;
}

//=======================================================================
//function : Coordinate
//purpose  : 
//=======================================================================

Standard_Real Hatch_Hatcher::Coordinate(const Standard_Integer I) const
{
  switch (myLines(I).myForm) {
    
  case Hatch_XLINE :
    return myLines(I).myLin.Location().X();

  case Hatch_YLINE :
    return myLines(I).myLin.Location().Y();

  case Hatch_ANYLINE :
    throw Standard_OutOfRange("Hatcher : not an X or Y line");
  }

  return 0.;
}

//=======================================================================
//function : NbIntervals
//purpose  : 
//=======================================================================

Standard_Integer Hatch_Hatcher::NbIntervals(const Standard_Integer I) const
{
  Standard_Integer l = myLines(I).myInters.Length();
  if (l == 0)
    l = myOrient ? 1 : 0;
  else {
    l = l/2;
    if (myOrient) if (!myLines(I).myInters(1).myStart) l++;
  }
  return l;
}

//=======================================================================
//function : Start
//purpose  : 
//=======================================================================

Standard_Real Hatch_Hatcher::Start(const Standard_Integer I,
				   const Standard_Integer J) const
{
  if (myLines(I).myInters.IsEmpty()) {
    if (J != 1 || !myOrient) throw Standard_OutOfRange();
    return RealFirst();
  }
  else {
    Standard_Integer jj = 2*J - 1;
    if (!myLines(I).myInters(1).myStart && myOrient) jj--;
    if (jj == 0) return RealFirst();
    return myLines(I).myInters(jj).myPar1;
  }
}

//=======================================================================
//function : StartIndex
//purpose  : 
//=======================================================================

void Hatch_Hatcher::StartIndex
  (const Standard_Integer I,
   const Standard_Integer J,
   Standard_Integer& Index,
   Standard_Real& Par2) const
{
  if (myLines(I).myInters.IsEmpty()) {
    if (J != 1) throw Standard_OutOfRange();
    Index = 0;
    Par2  = 0;
  }
  else {
    Standard_Integer jj = 2*J - 1;
    if (!myLines(I).myInters(1).myStart && myOrient) jj--;
    if (jj == 0) {
      Index = 0;
      Par2  = 0;
    }
    else {
      Index = myLines(I).myInters(jj).myIndex;
      Par2  = myLines(I).myInters(jj).myPar2;
    }
  }
}

//=======================================================================
//function : End
//purpose  : 
//=======================================================================

Standard_Real Hatch_Hatcher::End(const Standard_Integer I,
				 const Standard_Integer J) const
{
  if (myLines(I).myInters.IsEmpty()) {
    if (J != 1 || !myOrient) throw Standard_OutOfRange();
    return RealLast();
  }
  else {
    Standard_Integer jj = 2*J;
    if (!myLines(I).myInters(1).myStart && myOrient) jj--;
    if (jj > myLines(I).myInters.Length()) return RealLast();
    return myLines(I).myInters(jj).myPar1;
  }
}

//=======================================================================
//function : EndIndex
//purpose  : 
//=======================================================================

void Hatch_Hatcher::EndIndex
  (const Standard_Integer I,
   const Standard_Integer J,
   Standard_Integer& Index,
   Standard_Real& Par2) const
{
  if (myLines(I).myInters.IsEmpty()) {
    if (J != 1) throw Standard_OutOfRange();
    Index = 0;
    Par2  = 0;
  }
  else {
    Standard_Integer jj = 2*J;
    if (!myLines(I).myInters(1).myStart && myOrient) jj--;
    if (jj > myLines(I).myInters.Length()) {
      Index = 0;
      Par2  = 0;
    }
    else {
      Index = myLines(I).myInters(jj).myIndex;
      Par2  = myLines(I).myInters(jj).myPar2;
    }
  }
}
