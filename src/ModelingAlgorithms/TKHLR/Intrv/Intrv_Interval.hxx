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

  Standard_EXPORT Intrv_Interval(const Standard_Real Start, const Standard_Real End);

  Standard_EXPORT Intrv_Interval(const Standard_Real      Start,
                                 const Standard_ShortReal TolStart,
                                 const Standard_Real      End,
                                 const Standard_ShortReal TolEnd);

  Standard_Real Start() const;

  Standard_Real End() const;

  Standard_ShortReal TolStart() const;

  Standard_ShortReal TolEnd() const;

  void Bounds(Standard_Real&      Start,
              Standard_ShortReal& TolStart,
              Standard_Real&      End,
              Standard_ShortReal& TolEnd) const;

  void SetStart(const Standard_Real Start, const Standard_ShortReal TolStart);

  //! ****+****-------------------->      Old one
  //! ****+****------------------------>      New one to fuse
  //! <<<     <<<
  //! ****+****------------------------>      result
  void FuseAtStart(const Standard_Real Start, const Standard_ShortReal TolStart);

  //! ****+****----------->      Old one
  //! <----------**+**                        Tool for cutting
  //! >>>     >>>
  //! ****+****----------->      result
  void CutAtStart(const Standard_Real Start, const Standard_ShortReal TolStart);

  void SetEnd(const Standard_Real End, const Standard_ShortReal TolEnd);

  //! <---------------------****+****      Old one
  //! <-----------------**+**              New one to fuse
  //! >>>     >>>
  //! <---------------------****+****      result
  void FuseAtEnd(const Standard_Real End, const Standard_ShortReal TolEnd);

  //! <-----****+****                      Old one
  //! **+**------>             Tool for cutting
  //! <<<     <<<
  //! <-----****+****                      result
  void CutAtEnd(const Standard_Real End, const Standard_ShortReal TolEnd);

  //! True if myStart+myTolStart > myEnd-myTolEnd
  //! or if myEnd+myTolEnd > myStart-myTolStart
  Standard_Boolean IsProbablyEmpty() const;

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
  Standard_Boolean IsBefore(const Intrv_Interval& Other) const;

  //! True if me is After Other
  //! **-----------****          me
  //! ***----------------**                              Other
  Standard_Boolean IsAfter(const Intrv_Interval& Other) const;

  //! True if me is Inside Other
  //! **-----------****                          me
  //! ***--------------------------**                    Other
  Standard_Boolean IsInside(const Intrv_Interval& Other) const;

  //! True if me is Enclosing Other
  //! ***----------------------------****                  me
  //! ***------------------**                        Other
  Standard_Boolean IsEnclosing(const Intrv_Interval& Other) const;

  //! True if me is just Enclosing Other at start
  //! ***---------------------------****            me
  //! ***------------------**                        Other
  Standard_Boolean IsJustEnclosingAtStart(const Intrv_Interval& Other) const;

  //! True if me is just Enclosing Other at End
  //! ***----------------------------****                  me
  //! ***-----------------****                   Other
  Standard_Boolean IsJustEnclosingAtEnd(const Intrv_Interval& Other) const;

  //! True if me is just before Other
  //! ***--------****                                      me
  //! ***-----------**                        Other
  Standard_Boolean IsJustBefore(const Intrv_Interval& Other) const;

  //! True if me is just after Other
  //! ****-------****                         me
  //! ***-----------**                                     Other
  Standard_Boolean IsJustAfter(const Intrv_Interval& Other) const;

  //! True if me is overlapping Other at start
  //! ***---------------***                                me
  //! ***-----------**                        Other
  Standard_Boolean IsOverlappingAtStart(const Intrv_Interval& Other) const;

  //! True if me is overlapping Other at end
  //! ***-----------**                        me
  //! ***---------------***                                Other
  Standard_Boolean IsOverlappingAtEnd(const Intrv_Interval& Other) const;

  //! True if me is just overlapping Other at start
  //! ***-----------***                                    me
  //! ***------------------------**                        Other
  Standard_Boolean IsJustOverlappingAtStart(const Intrv_Interval& Other) const;

  //! True if me is just overlapping Other at end
  //! ***-----------*                         me
  //! ***------------------------**                        Other
  Standard_Boolean IsJustOverlappingAtEnd(const Intrv_Interval& Other) const;

  //! True if me and Other have the same bounds
  //! *----------------***                                me
  //! ***-----------------**                               Other
  Standard_Boolean IsSimilar(const Intrv_Interval& Other) const;

protected:
private:
  Standard_Real      myStart;
  Standard_Real      myEnd;
  Standard_ShortReal myTolStart;
  Standard_ShortReal myTolEnd;
};

#include <Intrv_Interval.lxx>

#endif // _Intrv_Interval_HeaderFile
