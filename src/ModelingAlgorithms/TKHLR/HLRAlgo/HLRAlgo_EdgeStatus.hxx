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

#include <Intrv_Intervals.hxx>

//! This class describes the Hidden Line status of an
//! Edge. It contains:
//!
//! The Bounds of the Edge and their tolerances
//!
//! Two flags indicating if the edge is full visible
//! or full hidden
//! TheSequenceof visible Intervals on the Edge.
class HLRAlgo_EdgeStatus
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT HLRAlgo_EdgeStatus();

  //! Creates a new EdgeStatus. Default visible. The
  //! Edge is bounded by the interval <Start>, <End>
  //! with the tolerances <TolStart>, <TolEnd>.
  Standard_EXPORT HLRAlgo_EdgeStatus(const double Start,
                                     const float  TolStart,
                                     const double End,
                                     const float  TolEnd);

  //! Initialize an EdgeStatus. Default visible. The
  //! Edge is bounded by the interval <Start>, <End>
  //! with the tolerances <TolStart>, <TolEnd>.
  Standard_EXPORT void Initialize(const double Start,
                                  const float  TolStart,
                                  const double End,
                                  const float  TolEnd);

  void Bounds(double& theStart, float& theTolStart, double& theEnd, float& theTolEnd) const
  {
    theStart    = myStart;
    theTolStart = myTolStart;
    theEnd      = myEnd;
    theTolEnd   = myTolEnd;
  }

  Standard_EXPORT int NbVisiblePart() const;

  Standard_EXPORT void VisiblePart(const int Index,
                                   double&   Start,
                                   float&    TolStart,
                                   double&   End,
                                   float&    TolEnd) const;

  //! Hides the interval <Start>, <End> with the
  //! tolerances <TolStart>, <TolEnd>. This interval is
  //! subtracted from the visible parts. If the hidden
  //! part is on (or under) the face the flag <OnFace>
  //! is True (or False). If the hidden part is on
  //! (or inside) the boundary of the face the flag
  //! <OnBoundary> is True (or False).
  Standard_EXPORT void Hide(const double Start,
                            const float  TolStart,
                            const double End,
                            const float  TolEnd,
                            const bool   OnFace,
                            const bool   OnBoundary);

  //! Hide the whole Edge.
  void HideAll()
  {
    AllVisible(false);
    AllHidden(true);
  }

  //! Show the whole Edge.
  void ShowAll()
  {
    AllVisible(true);
    AllHidden(false);
  }

  bool AllHidden() const { return myAllHidden; }

  void AllHidden(const bool B) { myAllHidden = B; }

  bool AllVisible() const { return myAllVisible; }

  void AllVisible(const bool B) { myAllVisible = B; }

private:
  double          myStart;
  double          myEnd;
  float           myTolStart;
  float           myTolEnd;
  bool            myAllHidden;
  bool            myAllVisible;
  Intrv_Intervals myVisibles;
};

#endif // _HLRAlgo_EdgeStatus_HeaderFile
