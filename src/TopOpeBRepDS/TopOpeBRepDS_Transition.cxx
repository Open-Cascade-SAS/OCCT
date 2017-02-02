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


#include <TopOpeBRepDS.hxx>
#include <TopOpeBRepDS_define.hxx>
#include <TopOpeBRepDS_Transition.hxx>

//=======================================================================
//function : TopOpeBRepDS_Transition
//purpose  : 
//=======================================================================
TopOpeBRepDS_Transition::TopOpeBRepDS_Transition() :
myStateBefore(TopAbs_UNKNOWN),myStateAfter(TopAbs_UNKNOWN),
myShapeBefore(TopAbs_FACE),myShapeAfter(TopAbs_FACE),
myIndexBefore(0),myIndexAfter(0)
{
}

//=======================================================================
//function : TopOpeBRepDS_Transition
//purpose  : 
//=======================================================================
TopOpeBRepDS_Transition::TopOpeBRepDS_Transition(const TopAbs_State SB,const TopAbs_State SA,const TopAbs_ShapeEnum ONB,const TopAbs_ShapeEnum ONA) :
myStateBefore(SB),myStateAfter(SA),
myShapeBefore(ONB),myShapeAfter(ONA),
myIndexBefore(0),myIndexAfter(0)
{
}

//=======================================================================
//function : TopOpeBRepDS_Transition
//purpose  : 
//=======================================================================
TopOpeBRepDS_Transition::TopOpeBRepDS_Transition(const TopAbs_Orientation O) :
myShapeBefore(TopAbs_FACE),myShapeAfter(TopAbs_FACE),
myIndexBefore(0),myIndexAfter(0)
{
  Set(O);
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
void TopOpeBRepDS_Transition::Set(const TopAbs_State SB,const TopAbs_State SA,const TopAbs_ShapeEnum ONB,const TopAbs_ShapeEnum ONA)
{
  myStateBefore = SB;
  myStateAfter = SA;
  myShapeBefore = ONB;
  myShapeAfter  = ONA;
}

//=======================================================================
//function : StateBefore
//purpose  : 
//=======================================================================
void TopOpeBRepDS_Transition::StateBefore(const TopAbs_State S)
{
  myStateBefore = S;
}

//=======================================================================
//function : StateAfter
//purpose  : 
//=======================================================================
void TopOpeBRepDS_Transition::StateAfter(const TopAbs_State S)
{
  myStateAfter = S;
}

//=======================================================================
//function : ShapeBefore
//purpose  : 
//=======================================================================
void TopOpeBRepDS_Transition::ShapeBefore(const TopAbs_ShapeEnum SE)
{
  myShapeBefore = SE;
}

//=======================================================================
//function : ShapeAfter
//purpose  : 
//=======================================================================
void TopOpeBRepDS_Transition::ShapeAfter(const TopAbs_ShapeEnum SE)
{
  myShapeAfter = SE;
}

//=======================================================================
//function : Before
//purpose  : 
//=======================================================================
void TopOpeBRepDS_Transition::Before(const TopAbs_State S,const TopAbs_ShapeEnum ONB)
{
  myStateBefore = S;
  myShapeBefore = ONB;
}

//=======================================================================
//function : After
//purpose  : 
//=======================================================================
void TopOpeBRepDS_Transition::After(const TopAbs_State S,const TopAbs_ShapeEnum ONA)
{
  myStateAfter = S;
  myShapeAfter = ONA;
}

//=======================================================================
//function : Index
//purpose  : 
//=======================================================================
void TopOpeBRepDS_Transition::Index(const Standard_Integer I)
{
  myIndexBefore = myIndexAfter = I;
}

//=======================================================================
//function : IndexBefore
//purpose  : 
//=======================================================================
void TopOpeBRepDS_Transition::IndexBefore(const Standard_Integer I)
{
  myIndexBefore = I;
}

//=======================================================================
//function : IndexAfter
//purpose  : 
//=======================================================================
void TopOpeBRepDS_Transition::IndexAfter(const Standard_Integer I)
{
  myIndexAfter = I;
}

//=======================================================================
//function : Before
//purpose  : 
//=======================================================================
TopAbs_State TopOpeBRepDS_Transition::Before() const 
{
  return myStateBefore;
}

//=======================================================================
//function : ONBefore
//purpose  : 
//=======================================================================
TopAbs_ShapeEnum TopOpeBRepDS_Transition::ONBefore() const 
{
  return myShapeBefore;
}

//=======================================================================
//function : After
//purpose  : 
//=======================================================================
TopAbs_State TopOpeBRepDS_Transition::After() const 
{
  return myStateAfter;
}

//=======================================================================
//function : ONAfter
//purpose  : 
//=======================================================================
TopAbs_ShapeEnum TopOpeBRepDS_Transition::ONAfter() const 
{
  return myShapeAfter;
}

//=======================================================================
//function : ShapeBefore
//purpose  : 
//=======================================================================
TopAbs_ShapeEnum TopOpeBRepDS_Transition::ShapeBefore() const 
{
  return myShapeBefore;
}

//=======================================================================
//function : ShapeAfter
//purpose  : 
//=======================================================================
TopAbs_ShapeEnum TopOpeBRepDS_Transition::ShapeAfter() const 
{
  return myShapeAfter;
}

//=======================================================================
//function : Index
//purpose  : 
//=======================================================================
Standard_Integer TopOpeBRepDS_Transition::Index() const
{
  if ( myIndexAfter != myIndexBefore ) 
    throw Standard_Failure("Transition::Index() on different shapes");
  return myIndexBefore;
}

//=======================================================================
//function : IndexBefore
//purpose  : 
//=======================================================================
Standard_Integer TopOpeBRepDS_Transition::IndexBefore() const
{
  return myIndexBefore;
}

//=======================================================================
//function : IndexAfter
//purpose  : 
//=======================================================================
Standard_Integer TopOpeBRepDS_Transition::IndexAfter() const
{
  return myIndexAfter;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
void TopOpeBRepDS_Transition::Set(const TopAbs_Orientation O)
{
  switch (O) {
  case TopAbs_FORWARD  : 
    myStateBefore = TopAbs_OUT; myStateAfter = TopAbs_IN; break;

  case TopAbs_REVERSED : 
    myStateBefore = TopAbs_IN;  myStateAfter = TopAbs_OUT; break;

  case TopAbs_INTERNAL : 
    myStateBefore = TopAbs_IN;  myStateAfter = TopAbs_IN; break;

  case TopAbs_EXTERNAL : 
    myStateBefore = TopAbs_OUT; myStateAfter = TopAbs_OUT; break;
  }
}

//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================
TopAbs_Orientation TopOpeBRepDS_Transition::Orientation(const TopAbs_State S,const TopAbs_ShapeEnum T) const
{
  if (myStateBefore == TopAbs_ON || myStateAfter == TopAbs_ON) {
    return OrientationON(S,T);
  }
  else {
    if (myStateBefore == S) {
      if (myStateAfter == S) return TopAbs_INTERNAL;
      else                   return TopAbs_REVERSED;
    }
    else  {
      if (myStateAfter == S) return TopAbs_FORWARD;
      else                   return TopAbs_EXTERNAL;
    }
  }
}

//=======================================================================
//function : OrientationON
//purpose  : 
//=======================================================================
TopAbs_Orientation TopOpeBRepDS_Transition::OrientationON(const TopAbs_State S,const TopAbs_ShapeEnum ) const // T) const
{
  TopAbs_Orientation result=TopAbs_FORWARD;

  if      (myStateBefore == TopAbs_ON && myStateAfter == TopAbs_ON) {
#if 0
    if      ( S == TopAbs_IN )  result = TopAbs_FORWARD;
    else if ( S == TopAbs_OUT ) result = TopAbs_REVERSED;
    else if ( S == TopAbs_ON )  result = TopAbs_INTERNAL;
#endif
    if      ( S == TopAbs_IN )  result = TopAbs_INTERNAL;
    else if ( S == TopAbs_OUT ) result = TopAbs_EXTERNAL;
    else if ( S == TopAbs_ON )  result = TopAbs_INTERNAL;
  }
  else if (myStateBefore == TopAbs_ON) {
    if (myStateAfter == S) return TopAbs_FORWARD;
    else                   return TopAbs_REVERSED;
  }
  else if (myStateAfter == TopAbs_ON) {
    if (myStateBefore == S) return TopAbs_REVERSED;
    else                    return TopAbs_FORWARD;
  }

  return result;
}

//=======================================================================
//function : Complement
//purpose  : 
//=======================================================================
TopOpeBRepDS_Transition TopOpeBRepDS_Transition::Complement() const
{
  TopOpeBRepDS_Transition T;
  T.myIndexBefore = myIndexBefore;
  T.myIndexAfter = myIndexAfter;

  // xpu : 14-01-98
  if ( myStateBefore == TopAbs_UNKNOWN && myStateAfter == TopAbs_UNKNOWN ) {
    T.Set(myStateAfter,myStateBefore,myShapeAfter,myShapeBefore);
    return T;
  }
  // xpu : 14-01-98

  if ( myStateBefore == TopAbs_ON || myStateAfter == TopAbs_ON) {
    T.Set(myStateAfter,myStateBefore,myShapeAfter,myShapeBefore);
  }
  else {
    TopAbs_Orientation  o = Orientation(TopAbs_IN);
    if      ( o == TopAbs_FORWARD)          // (OUT,IN) --> (IN,OUT)
      T.Set(TopAbs_IN,TopAbs_OUT,myShapeBefore,myShapeAfter);
    else if ( o == TopAbs_REVERSED)         // (IN,OUT) --> (OUT,IN)
      T.Set(TopAbs_OUT,TopAbs_IN,myShapeBefore,myShapeAfter);
    else if ( o == TopAbs_EXTERNAL)         // (OUT,OUT) --> (IN,IN)
      T.Set(TopAbs_IN,TopAbs_IN,myShapeBefore,myShapeAfter);
    else if ( o == TopAbs_INTERNAL)         // (IN,IN) --> (OUT,OUT)
      T.Set(TopAbs_OUT,TopAbs_OUT,myShapeBefore,myShapeAfter);
  }

  return T;
}
  
//=======================================================================
//function : IsUnknown
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRepDS_Transition::IsUnknown() const
{
  return (myStateBefore == TopAbs_UNKNOWN) && (myStateAfter == TopAbs_UNKNOWN);
}

