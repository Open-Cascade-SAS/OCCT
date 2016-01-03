// Created on: 1995-12-21
// Created by: Jean Yves LEBEY
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _TopOpeBRepBuild_AreaBuilder_HeaderFile
#define _TopOpeBRepBuild_AreaBuilder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopOpeBRepBuild_ListOfListOfLoop.hxx>
#include <TopOpeBRepBuild_ListIteratorOfListOfListOfLoop.hxx>
#include <TopOpeBRepBuild_ListIteratorOfListOfLoop.hxx>
#include <Standard_Boolean.hxx>
#include <TopAbs_State.hxx>
#include <TopOpeBRepBuild_ListOfLoop.hxx>
#include <TopOpeBRepBuild_LoopEnum.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Address.hxx>
class TopOpeBRepBuild_LoopSet;
class TopOpeBRepBuild_LoopClassifier;
class TopOpeBRepBuild_Loop;



//! The AreaBuilder algorithm is  used  to
//! reconstruct complex  topological objects as  Faces
//! or Solids.
//! * Loop is  the  composite topological object of
//! the boundary. Wire for a Face. Shell for a Solid.
//! *  LoopSet is a  tool describing the object  to
//! build.  It gives an iteration  on Loops.  For each
//! Loop it tells if it is on the boundary or if it is
//! an interference.
//! * LoopClassifier  is an algorithm  used to test
//! if a Loop is inside  another  Loop.
//! The  result of the  reconstruction is an iteration
//! on the reconstructed areas.  An  area is described
//! by a set of Loops.
//! A AreaBuilder is built with :
//! - a LoopSet describing the object to reconstruct.
//! - a LoopClassifier providing the classification algorithm.
class TopOpeBRepBuild_AreaBuilder 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRepBuild_AreaBuilder();
  
  //! Creates a AreaBuilder to build the areas on
  //! the shapes described by <LS> using the classifier <LC>.
  Standard_EXPORT TopOpeBRepBuild_AreaBuilder(TopOpeBRepBuild_LoopSet& LS, TopOpeBRepBuild_LoopClassifier& LC, const Standard_Boolean ForceClass = Standard_False);
  
  Standard_EXPORT virtual ~TopOpeBRepBuild_AreaBuilder();
  
  //! Sets a AreaBuilder to find the areas on
  //! the shapes described by <LS> using the classifier <LC>.
  Standard_EXPORT virtual void InitAreaBuilder (TopOpeBRepBuild_LoopSet& LS, TopOpeBRepBuild_LoopClassifier& LC, const Standard_Boolean ForceClass = Standard_False);
  
  //! Initialize iteration on areas.
  Standard_EXPORT Standard_Integer InitArea();
  
  Standard_EXPORT Standard_Boolean MoreArea() const;
  
  Standard_EXPORT void NextArea();
  
  //! Initialize iteration on loops of current Area.
  Standard_EXPORT Standard_Integer InitLoop();
  
  Standard_EXPORT Standard_Boolean MoreLoop() const;
  
  Standard_EXPORT void NextLoop();
  
  //! Returns the current Loop in the current area.
  Standard_EXPORT const Handle(TopOpeBRepBuild_Loop)& Loop() const;
  
  Standard_EXPORT virtual void ADD_Loop_TO_LISTOFLoop (const Handle(TopOpeBRepBuild_Loop)& L, TopOpeBRepBuild_ListOfLoop& LOL, const Standard_Address s = NULL) const;
  
  Standard_EXPORT virtual void REM_Loop_FROM_LISTOFLoop (TopOpeBRepBuild_ListIteratorOfListOfLoop& ITLOL, TopOpeBRepBuild_ListOfLoop& LOL, const Standard_Address s = NULL) const;
  
  Standard_EXPORT virtual void ADD_LISTOFLoop_TO_LISTOFLoop (TopOpeBRepBuild_ListOfLoop& LOL1, TopOpeBRepBuild_ListOfLoop& LOL2, const Standard_Address s = NULL, const Standard_Address s1 = NULL, const Standard_Address s2 = NULL) const;




protected:

  
  Standard_EXPORT TopAbs_State CompareLoopWithListOfLoop (TopOpeBRepBuild_LoopClassifier& LC, const Handle(TopOpeBRepBuild_Loop)& L, const TopOpeBRepBuild_ListOfLoop& LOL, const TopOpeBRepBuild_LoopEnum le) const;
  
  Standard_EXPORT void Atomize (TopAbs_State& state, const TopAbs_State newstate) const;


  TopOpeBRepBuild_ListOfListOfLoop myArea;
  TopOpeBRepBuild_ListIteratorOfListOfListOfLoop myAreaIterator;
  TopOpeBRepBuild_ListIteratorOfListOfLoop myLoopIterator;
  Standard_Boolean myUNKNOWNRaise;


private:





};







#endif // _TopOpeBRepBuild_AreaBuilder_HeaderFile
