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

#ifndef _HLRAlgo_EdgeStatus_HeaderFile
#define _HLRAlgo_EdgeStatus_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_ShortReal.hxx>
#include <Standard_Boolean.hxx>
#include <Intrv_Intervals.hxx>
#include <Standard_Integer.hxx>


//! This class describes the Hidden  Line status of an
//! Edge. It contains :
//!
//! The Bounds of the Edge and their tolerances
//!
//! Two flags indicating if the edge is full visible
//! or full hidden.
//!
//! The Sequence  of visible Intervals  on the Edge.
class HLRAlgo_EdgeStatus 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT HLRAlgo_EdgeStatus();
  
  //! Creates a  new  EdgeStatus.  Default visible.  The
  //! Edge is   bounded by the  interval  <Start>, <End>
  //! with the tolerances <TolStart>, <TolEnd>.
  Standard_EXPORT HLRAlgo_EdgeStatus(const Standard_Real Start, const Standard_ShortReal TolStart, const Standard_Real End, const Standard_ShortReal TolEnd);
  
  //! Initialize  an  EdgeStatus.  Default visible.  The
  //! Edge is   bounded by the  interval  <Start>, <End>
  //! with the tolerances <TolStart>, <TolEnd>.
  Standard_EXPORT void Initialize (const Standard_Real Start, const Standard_ShortReal TolStart, const Standard_Real End, const Standard_ShortReal TolEnd);

  void Bounds (Standard_Real& theStart, Standard_ShortReal& theTolStart, Standard_Real& theEnd, Standard_ShortReal& theTolEnd) const
  {
    theStart    = myStart;
    theTolStart = myTolStart;
    theEnd      = myEnd;
    theTolEnd   = myTolEnd;
  }

  Standard_EXPORT Standard_Integer NbVisiblePart() const;
  
  Standard_EXPORT void VisiblePart (const Standard_Integer Index, Standard_Real& Start, Standard_ShortReal& TolStart, Standard_Real& End, Standard_ShortReal& TolEnd) const;
  
  //! Hides  the  interval  <Start>,    <End>   with the
  //! tolerances <TolStart>,  <TolEnd>. This interval is
  //! subtracted from the visible  parts.  If the hidden
  //! part is on ( or under ) the face the flag <OnFace>
  //! is True ( or False ).  If the hidden  part is on (
  //! or  inside  ) the boundary  of  the  face the flag
  //! <OnBoundary> is True ( or False ).
  Standard_EXPORT void Hide (const Standard_Real Start, const Standard_ShortReal TolStart, const Standard_Real End, const Standard_ShortReal TolEnd, const Standard_Boolean OnFace, const Standard_Boolean OnBoundary);

  //! Hide the whole Edge.
  void HideAll()
  {
    AllVisible(Standard_False);
    AllHidden (Standard_True);
  }

  //! Show the whole Edge.
  void ShowAll()
  {
    AllVisible(Standard_True);
    AllHidden (Standard_False);
  }

  Standard_Boolean AllHidden() const { return myAllHidden; }

  void AllHidden (const Standard_Boolean B) { myAllHidden = B; }

  Standard_Boolean AllVisible() const { return myAllVisible; }

  void AllVisible (const Standard_Boolean B) { myAllVisible = B; }

private:

  Standard_Real myStart;
  Standard_Real myEnd;
  Standard_ShortReal myTolStart;
  Standard_ShortReal myTolEnd;
  Standard_Boolean myAllHidden;
  Standard_Boolean myAllVisible;
  Intrv_Intervals myVisibles;

};

#endif // _HLRAlgo_EdgeStatus_HeaderFile
