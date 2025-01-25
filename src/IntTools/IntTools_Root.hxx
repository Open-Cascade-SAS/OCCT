// Created on: 2000-05-22
// Created by: Peter KURNEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _IntTools_Root_HeaderFile
#define _IntTools_Root_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <TopAbs_State.hxx>
#include <Standard_Boolean.hxx>

//! The class is to describe the root of
//! function of one variable  for  Edge/Edge
//! and  Edge/Surface  algorithms.
class IntTools_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT IntTools_Root();

  //! Initializes my by range of parameters
  //! and type of root
  Standard_EXPORT IntTools_Root(const Standard_Real aRoot, const Standard_Integer aType);

  //! Sets the Root's value
  Standard_EXPORT void SetRoot(const Standard_Real aRoot);

  //! Sets the Root's Type
  Standard_EXPORT void SetType(const Standard_Integer aType);

  //! Set  the  value of the state before the root
  //! (at  t=Root-dt)
  Standard_EXPORT void SetStateBefore(const TopAbs_State aState);

  //! Set  the  value of the state after the root
  //! (at  t=Root-dt)
  Standard_EXPORT void SetStateAfter(const TopAbs_State aState);

  //! Not  used  in  Edge/Edge  algorithm
  Standard_EXPORT void SetLayerHeight(const Standard_Real aHeight);

  //! Sets the  interval  from which the Root was
  //! found [t1,t2] and the  corresponding  values
  //! of  the  function  on  the  bounds f(t1), f(t2).
  Standard_EXPORT void SetInterval(const Standard_Real t1,
                                   const Standard_Real t2,
                                   const Standard_Real f1,
                                   const Standard_Real f2);

  //! Returns the Root  value
  Standard_EXPORT Standard_Real Root() const;

  //! Returns the  type  of  the  root
  //! =0  -  Simple (was  found  by  bisection  method);
  //! =2  -  Smart when f1=0, f2!=0 or  vice  versa
  //! (was  found  by  Fibbonacci method);
  //! =1  -  Pure   (pure  zero  for all t [t1,t2] );
  Standard_EXPORT Standard_Integer Type() const;

  //! Returns the state before the root
  Standard_EXPORT TopAbs_State StateBefore() const;

  //! Returns the state after the root
  Standard_EXPORT TopAbs_State StateAfter() const;

  //! Not  used  in  Edge/Edge  algorithm
  Standard_EXPORT Standard_Real LayerHeight() const;

  //! Returns the validity flag for the root,
  //! True if
  //! myStateBefore==TopAbs_OUT && myStateAfter==TopAbs_IN or
  //! myStateBefore==TopAbs_OUT && myStateAfter==TopAbs_ON or
  //! myStateBefore==TopAbs_ON  && myStateAfter==TopAbs_OUT or
  //! myStateBefore==TopAbs_IN  && myStateAfter==TopAbs_OUT  .
  //! For  other  cases it  returns  False.
  Standard_EXPORT Standard_Boolean IsValid() const;

  //! Returns the values of interval  from which the Root was
  //! found [t1,t2] and the  corresponding  values
  //! of  the  function  on  the  bounds f(t1), f(t2).
  Standard_EXPORT void Interval(Standard_Real& t1,
                                Standard_Real& t2,
                                Standard_Real& f1,
                                Standard_Real& f2) const;

protected:
private:
  Standard_Real    myRoot;
  Standard_Integer myType;
  Standard_Real    myLayerHeight;
  TopAbs_State     myStateBefore;
  TopAbs_State     myStateAfter;
  Standard_Real    myt1;
  Standard_Real    myt2;
  Standard_Real    myf1;
  Standard_Real    myf2;
};

#endif // _IntTools_Root_HeaderFile
