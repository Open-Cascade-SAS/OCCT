// Created on: 1994-05-26
// Created by: Jean Yves LEBEY
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _TopOpeBRepDS_Transition_HeaderFile
#define _TopOpeBRepDS_Transition_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopAbs_State.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <Standard_Integer.hxx>
#include <TopAbs_Orientation.hxx>


class TopOpeBRepDS_Transition 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRepDS_Transition();
  
  Standard_EXPORT TopOpeBRepDS_Transition(const TopAbs_State StateBefore, const TopAbs_State StateAfter, const TopAbs_ShapeEnum ShapeBefore = TopAbs_FACE, const TopAbs_ShapeEnum ShapeAfter = TopAbs_FACE);
  
  Standard_EXPORT TopOpeBRepDS_Transition(const TopAbs_Orientation O);
  
  Standard_EXPORT void Set (const TopAbs_State StateBefore, const TopAbs_State StateAfter, const TopAbs_ShapeEnum ShapeBefore = TopAbs_FACE, const TopAbs_ShapeEnum ShapeAfter = TopAbs_FACE);
  
  Standard_EXPORT void StateBefore (const TopAbs_State S);
  
  Standard_EXPORT void StateAfter (const TopAbs_State S);
  
  Standard_EXPORT void ShapeBefore (const TopAbs_ShapeEnum SE);
  
  Standard_EXPORT void ShapeAfter (const TopAbs_ShapeEnum SE);
  
  Standard_EXPORT void Before (const TopAbs_State S, const TopAbs_ShapeEnum ShapeBefore = TopAbs_FACE);
  
  Standard_EXPORT void After (const TopAbs_State S, const TopAbs_ShapeEnum ShapeAfter = TopAbs_FACE);
  
  Standard_EXPORT void Index (const Standard_Integer I);
  
  Standard_EXPORT void IndexBefore (const Standard_Integer I);
  
  Standard_EXPORT void IndexAfter (const Standard_Integer I);
  
  Standard_EXPORT TopAbs_State Before() const;
  
  Standard_EXPORT TopAbs_ShapeEnum ONBefore() const;
  
  Standard_EXPORT TopAbs_State After() const;
  
  Standard_EXPORT TopAbs_ShapeEnum ONAfter() const;
  
  Standard_EXPORT TopAbs_ShapeEnum ShapeBefore() const;
  
  Standard_EXPORT TopAbs_ShapeEnum ShapeAfter() const;
  
  Standard_EXPORT Standard_Integer Index() const;
  
  Standard_EXPORT Standard_Integer IndexBefore() const;
  
  Standard_EXPORT Standard_Integer IndexAfter() const;
  
  //! set the transition corresponding to orientation <O>
  //!
  //! O       Before  After
  //!
  //! FORWARD       OUT    IN
  //! REVERSED      IN     OUT
  //! INTERNAL      IN     IN
  //! EXTERNAL      OUT    OUT
  Standard_EXPORT void Set (const TopAbs_Orientation O);
  
  //! returns the orientation corresponding to state <S>
  //!
  //! Before and After not equal TopAbs_ON :
  //! --------------------------------------
  //! Before  After   Computed orientation
  //!
  //! S      not S   REVERSED (we leave state S)
  //! not S  S       FORWARD  (we enter state S)
  //! S      S       INTERNAL (we stay in state S)
  //! not S  not S   EXTERNAL (we stay outside state S)
  Standard_EXPORT TopAbs_Orientation Orientation (const TopAbs_State S, const TopAbs_ShapeEnum T = TopAbs_FACE) const;
  
  Standard_EXPORT TopOpeBRepDS_Transition Complement() const;
  
  //! returns True if both states are UNKNOWN
  Standard_EXPORT Standard_Boolean IsUnknown() const;





protected:





private:

  
  //! returns the orientation corresponding to state <S>
  //! (if one at least of the internal states is ON)
  Standard_EXPORT TopAbs_Orientation OrientationON (const TopAbs_State S, const TopAbs_ShapeEnum T) const;


  TopAbs_State myStateBefore;
  TopAbs_State myStateAfter;
  TopAbs_ShapeEnum myShapeBefore;
  TopAbs_ShapeEnum myShapeAfter;
  Standard_Integer myIndexBefore;
  Standard_Integer myIndexAfter;


};







#endif // _TopOpeBRepDS_Transition_HeaderFile
