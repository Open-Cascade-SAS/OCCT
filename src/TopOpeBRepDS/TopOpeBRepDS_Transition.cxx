// Created on: 1994-05-26
// Created by: Jean Yves LEBEY
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <TopOpeBRepDS_Transition.ixx>

#include <TopOpeBRepDS.hxx>
#include <TopOpeBRepDS_define.hxx>

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
    Standard_Failure::Raise("Transition::Index() on different shapes");
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

//=======================================================================
//function : DumpB
//purpose  : 
//=======================================================================
Standard_OStream& TopOpeBRepDS_Transition::DumpB(Standard_OStream& OS) const
{
#ifdef DEB
  TopOpeBRepDS::Print(myStateBefore,OS);
  TopOpeBRepDS::Print(TopOpeBRepDS::ShapeToKind(myShapeBefore),myIndexBefore,OS);
#endif
  return OS;
}

//=======================================================================
//function : DumpA
//purpose  : 
//=======================================================================
Standard_OStream& TopOpeBRepDS_Transition::DumpA(Standard_OStream& OS) const
{
#ifdef DEB
  TopOpeBRepDS::Print(myStateAfter,OS);
  TopOpeBRepDS::Print(TopOpeBRepDS::ShapeToKind(myShapeAfter),myIndexAfter,OS);
#endif
  return OS;
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
Standard_OStream& TopOpeBRepDS_Transition::Dump(Standard_OStream& OS) const
{
#ifdef DEB
  OS<<"("; DumpB(OS); OS<<","; DumpA(OS); OS<<")";
#endif
  return OS;
}
