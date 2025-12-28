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

//                   **-----------****             Other
//     ***-----*                                   IsBefore
//     ***------------*                            IsJustBefore
//     ***-----------------*                       IsOverlappingAtStart
//     ***--------------------------*              IsJustEnclosingAtEnd
//     ***-------------------------------------*   IsEnclosing
//                  ***----*                       IsJustOverlappingAtStart
//                  ***-------------*              IsSimilar
//                  ***------------------------*   IsJustEnclosingAtStart
//                         ***-*                   IsInside
//                         ***------*              IsJustOverlappingAtEnd
//                         ***-----------------*   IsOverlappingAtEnd
//                                  ***--------*   IsJustAfter
//                                       ***---*   IsAfter

//=================================================================================================

inline double Intrv_Interval::Start() const
{
  return myStart;
}

//=================================================================================================

inline double Intrv_Interval::End() const
{
  return myEnd;
}

//=================================================================================================

inline float Intrv_Interval::TolStart() const
{
  return myTolStart;
}

//=================================================================================================

inline float Intrv_Interval::TolEnd() const
{
  return myTolEnd;
}

//=================================================================================================

inline void Intrv_Interval::Bounds(double& Start, float& TolStart, double& End, float& TolEnd) const
{
  Start    = myStart;
  TolStart = myTolStart;
  End      = myEnd;
  TolEnd   = myTolEnd;
}

//=================================================================================================

inline void Intrv_Interval::SetStart(const double Start, const float TolStart)
{
  myStart    = Start;
  myTolStart = TolStart;
}

//=======================================================================
// function : FuseAtStart
//
//                 ****+****-------------------->      Old one
//             ****+****------------------------>      New one to fuse
//             <<<     <<<
//             ****+****------------------------>      result
//
//=======================================================================

inline void Intrv_Interval::FuseAtStart(const double Start, const float TolStart)
{
  if (myStart != RealFirst())
  {
    double a   = std::min(myStart - myTolStart, Start - TolStart);
    double b   = std::min(myStart + myTolStart, Start + TolStart);
    myStart    = (a + b) / 2;
    myTolStart = (float)(b - a) / 2;
  }
}

//=======================================================================
// function : CutAtStart
//
//                          ****+****----------->      Old one
//             <----------**+**                        Tool for cutting
//                        >>>     >>>
//                          ****+****----------->      result
//
//=======================================================================

inline void Intrv_Interval::CutAtStart(const double Start, const float TolStart)
{
  if (myStart != RealFirst())
  {
    double a   = std::max(myStart - myTolStart, Start - TolStart);
    double b   = std::max(myStart + myTolStart, Start + TolStart);
    myStart    = (a + b) / 2;
    myTolStart = (float)(b - a) / 2;
  }
}

//=================================================================================================

inline void Intrv_Interval::SetEnd(const double End, const float TolEnd)
{
  myEnd    = End;
  myTolEnd = TolEnd;
}

//=======================================================================
// function : FuseAtEnd
//
//             <---------------------****+****      Old one
//             <-----------------**+**              New one to fuse
//                                 >>>     >>>
//             <---------------------****+****      result
//
//=======================================================================

inline void Intrv_Interval::FuseAtEnd(const double End, const float TolEnd)
{
  if (myEnd != RealLast())
  {
    double a = std::max(myEnd - myTolEnd, End - TolEnd);
    double b = std::max(myEnd + myTolEnd, End + TolEnd);
    myEnd    = (a + b) / 2;
    myTolEnd = (float)(b - a) / 2;
  }
}

//=======================================================================
// function : CutAtEnd
//
//             <-----****+****                      Old one
//                         **+**------>             Tool for cutting
//                   <<<     <<<
//             <-----****+****                      result
//
//=======================================================================

inline void Intrv_Interval::CutAtEnd(const double End, const float TolEnd)
{
  if (myEnd != RealLast())
  {
    double a = std::min(myEnd - myTolEnd, End - TolEnd);
    double b = std::min(myEnd + myTolEnd, End + TolEnd);
    myEnd    = (a + b) / 2;
    myTolEnd = (float)(b - a) / 2;
  }
}

//=================================================================================================

inline bool AreFused(const double c1, const float t1, const double c2, const float t2)
{
  return t1 + t2 >= std::abs(c1 - c2);
}

//=================================================================================================

inline bool Intrv_Interval::IsProbablyEmpty() const
{
  return AreFused(myStart, myTolStart, myEnd, myTolEnd);
}

//=======================================================================
//                   **-----------****             Other
//     ***-----*                                   IsBefore
//=======================================================================

inline bool Intrv_Interval::IsBefore(const Intrv_Interval& Other) const
{
  return myTolEnd + Other.myTolStart < Other.myStart - myEnd;
}

//=======================================================================
//                   **-----------****             Other
//                                       ***---*   IsAfter
//=======================================================================

inline bool Intrv_Interval::IsAfter(const Intrv_Interval& Other) const
{
  return myTolStart + Other.myTolEnd < myStart - Other.myEnd;
}

//=======================================================================
//                   **-----------****             Other
//                         ***-*                   IsInside
//=======================================================================

inline bool Intrv_Interval::IsInside(const Intrv_Interval& Other) const
{
  return myTolStart + Other.myTolStart < myStart - Other.myStart
         && myTolEnd + Other.myTolEnd < Other.myEnd - myEnd;
}

//=======================================================================
//                   **-----------****             Other
//     ***-------------------------------------*   IsEnclosing
//=======================================================================

inline bool Intrv_Interval::IsEnclosing(const Intrv_Interval& Other) const
{
  return myTolStart + Other.myTolStart < Other.myStart - myStart
         && myTolEnd + Other.myTolEnd < myEnd - Other.myEnd;
}

//=======================================================================
//                   **-----------****             Other
//                  ***------------------------*   IsJustEnclosingAtStart
//=======================================================================

inline bool Intrv_Interval::IsJustEnclosingAtStart(const Intrv_Interval& Other) const
{
  return AreFused(myStart, myTolStart, Other.myStart, Other.myTolStart)
         && myTolEnd + Other.myTolEnd < myEnd - Other.myEnd;
}

//=======================================================================
//                   **-----------****             Other
//     ***--------------------------*              IsJustEnclosingAtEnd
//=======================================================================

inline bool Intrv_Interval::IsJustEnclosingAtEnd(const Intrv_Interval& Other) const
{
  return myTolStart + Other.myTolStart < Other.myStart - myStart
         && AreFused(Other.myEnd, Other.myTolEnd, myEnd, myTolEnd);
}

//=======================================================================
//                   **-----------****             Other
//     ***------------*                            IsJustBefore
//=======================================================================

inline bool Intrv_Interval::IsJustBefore(const Intrv_Interval& Other) const
{
  return AreFused(myEnd, myTolEnd, Other.myStart, Other.myTolStart);
}

//=======================================================================
//                   **-----------****             Other
//                                  ***--------*   IsJustAfter
//=======================================================================

inline bool Intrv_Interval::IsJustAfter(const Intrv_Interval& Other) const
{
  return AreFused(Other.myEnd, Other.myTolEnd, myStart, myTolStart);
}

//=======================================================================
//                   **-----------****             Other
//     ***-----------------*                       IsOverlappingAtStart
//=======================================================================

inline bool Intrv_Interval::IsOverlappingAtStart(const Intrv_Interval& Other) const
{
  return myTolStart + Other.myTolStart < Other.myStart - myStart
         && myTolEnd + Other.myTolStart < myEnd - Other.myStart
         && myTolEnd + Other.myTolEnd < Other.myEnd - myEnd;
}

//=======================================================================
//                   **-----------****             Other
//                         ***-----------------*   IsOverlappingAtEnd
//=======================================================================

inline bool Intrv_Interval::IsOverlappingAtEnd(const Intrv_Interval& Other) const
{
  return myTolStart + Other.myTolStart < myStart - Other.myStart
         && myTolStart + Other.myTolEnd < Other.myEnd - myStart
         && myTolEnd + Other.myTolEnd < myEnd - Other.myEnd;
}

//=======================================================================
//                   **-----------****             Other
//                  ***----*                       IsJustOverlappingAtStart
//=======================================================================

inline bool Intrv_Interval::IsJustOverlappingAtStart(const Intrv_Interval& Other) const
{
  return AreFused(myStart, myTolStart, Other.myStart, Other.myTolStart)
         && myTolEnd + Other.myTolEnd < Other.myEnd - myEnd;
}

//=======================================================================
//                   **-----------****             Other
//                         ***------*              IsJustOverlappingAtEnd
//=======================================================================

inline bool Intrv_Interval::IsJustOverlappingAtEnd(const Intrv_Interval& Other) const
{
  return myTolStart + Other.myTolStart < myStart - Other.myStart
         && AreFused(Other.myEnd, Other.myTolEnd, myEnd, myTolEnd);
}

//=======================================================================
//                   **-----------****             Other
//                  ***-------------*              IsSimilar
//=======================================================================

inline bool Intrv_Interval::IsSimilar(const Intrv_Interval& Other) const
{
  bool b1, b2;
  b1 = AreFused(myStart, myTolStart, Other.myStart, Other.myTolStart);
  b2 = AreFused(myEnd, myTolEnd, Other.myEnd, Other.myTolEnd);
  return b1 && b2;
}
