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

#ifndef _Intrv_Interval_HeaderFile
#define _Intrv_Interval_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_ShortReal.hxx>
#include <Intrv_Position.hxx>

//! **-----------****             Other
//! ***---*                                   IsBefore
//! ***----------*                            IsJustBefore
//! ***---------------*                       IsOverlappingAtStart
//! ***------------------------*              IsJustEnclosingAtEnd
//! ***-----------------------------------*   IsEnclosing
//! ***----*                       IsJustOverlappingAtStart
//! ***-------------*              IsSimilar
//! ***------------------------*   IsJustEnclosingAtStart
//! ***-*                   IsInside
//! ***------*              IsJustOverlappingAtEnd
//! ***-----------------*   IsOverlappingAtEnd
//! ***--------*   IsJustAfter
//! ***---*   IsAfter
class Intrv_Interval
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT Intrv_Interval();

  Standard_EXPORT Intrv_Interval(const double Start, const double End);

  Standard_EXPORT Intrv_Interval(const double Start,
                                 const float  TolStart,
                                 const double End,
                                 const float  TolEnd);

  double Start() const;

  double End() const;

  float TolStart() const;

  float TolEnd() const;

  void Bounds(double& Start, float& TolStart, double& End, float& TolEnd) const;

  void SetStart(const double Start, const float TolStart);

  //! ****+****-------------------->      Old one
  //! ****+****------------------------>      New one to fuse
  //! <<<     <<<
  //! ****+****------------------------>      result
  void FuseAtStart(const double Start, const float TolStart);

  //! ****+****----------->      Old one
  //! <----------**+**                        Tool for cutting
  //! >>>     >>>
  //! ****+****----------->      result
  void CutAtStart(const double Start, const float TolStart);

  void SetEnd(const double End, const float TolEnd);

  //! <---------------------****+****      Old one
  //! <-----------------**+**              New one to fuse
  //! >>>     >>>
  //! <---------------------****+****      result
  void FuseAtEnd(const double End, const float TolEnd);

  //! <-----****+****                      Old one
  //! **+**------>             Tool for cutting
  //! <<<     <<<
  //! <-----****+****                      result
  void CutAtEnd(const double End, const float TolEnd);

  //! True if myStart+myTolStart > myEnd-myTolEnd
  //! or if myEnd+myTolEnd > myStart-myTolStart
  bool IsProbablyEmpty() const;

  //! True if me is Before Other
  //! **-----------****             Other
  //! ***-----*                                   Before
  //! ***------------*                            JustBefore
  //! ***-----------------*                       OverlappingAtStart
  //! ***--------------------------*              JustEnclosingAtEnd
  //! ***-------------------------------------*   Enclosing
  //! ***----*                       JustOverlappingAtStart
  //! ***-------------*              Similar
  //! ***------------------------*   JustEnclosingAtStart
  //! ***-*                   Inside
  //! ***------*              JustOverlappingAtEnd
  //! ***-----------------*   OverlappingAtEnd
  //! ***--------*   JustAfter
  //! ***---*   After
  Standard_EXPORT Intrv_Position Position(const Intrv_Interval& Other) const;

  //! True if me is Before Other
  //! ***----------------**                              me
  //! **-----------****          Other
  bool IsBefore(const Intrv_Interval& Other) const;

  //! True if me is After Other
  //! **-----------****          me
  //! ***----------------**                              Other
  bool IsAfter(const Intrv_Interval& Other) const;

  //! True if me is Inside Other
  //! **-----------****                          me
  //! ***--------------------------**                    Other
  bool IsInside(const Intrv_Interval& Other) const;

  //! True if me is Enclosing Other
  //! ***----------------------------****                  me
  //! ***------------------**                        Other
  bool IsEnclosing(const Intrv_Interval& Other) const;

  //! True if me is just Enclosing Other at start
  //! ***---------------------------****            me
  //! ***------------------**                        Other
  bool IsJustEnclosingAtStart(const Intrv_Interval& Other) const;

  //! True if me is just Enclosing Other at End
  //! ***----------------------------****                  me
  //! ***-----------------****                   Other
  bool IsJustEnclosingAtEnd(const Intrv_Interval& Other) const;

  //! True if me is just before Other
  //! ***--------****                                      me
  //! ***-----------**                        Other
  bool IsJustBefore(const Intrv_Interval& Other) const;

  //! True if me is just after Other
  //! ****-------****                         me
  //! ***-----------**                                     Other
  bool IsJustAfter(const Intrv_Interval& Other) const;

  //! True if me is overlapping Other at start
  //! ***---------------***                                me
  //! ***-----------**                        Other
  bool IsOverlappingAtStart(const Intrv_Interval& Other) const;

  //! True if me is overlapping Other at end
  //! ***-----------**                        me
  //! ***---------------***                                Other
  bool IsOverlappingAtEnd(const Intrv_Interval& Other) const;

  //! True if me is just overlapping Other at start
  //! ***-----------***                                    me
  //! ***------------------------**                        Other
  bool IsJustOverlappingAtStart(const Intrv_Interval& Other) const;

  //! True if me is just overlapping Other at end
  //! ***-----------*                         me
  //! ***------------------------**                        Other
  bool IsJustOverlappingAtEnd(const Intrv_Interval& Other) const;

  //! True if me and Other have the same bounds
  //! *----------------***                                me
  //! ***-----------------**                               Other
  bool IsSimilar(const Intrv_Interval& Other) const;

private:
  double myStart;
  double myEnd;
  float  myTolStart;
  float  myTolEnd;
};

#include <Intrv_Interval.lxx>

#endif // _Intrv_Interval_HeaderFile
