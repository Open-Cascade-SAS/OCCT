// Created on: 1996-02-13
// Created by: Jean Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
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


#include <Standard_ProgramError.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopAbs.hxx>
#include <TopOpeBRepBuild_GTopo.hxx>
#include <TopOpeBRepDS.hxx>

//=======================================================================
//function : TopOpeBRepBuild_GTopo
//purpose  : 
//=======================================================================
TopOpeBRepBuild_GTopo::TopOpeBRepBuild_GTopo()
{
  Reset();
}

//=======================================================================
//function : TopOpeBRepBuild_GTopo
//purpose  : 
//=======================================================================

TopOpeBRepBuild_GTopo::TopOpeBRepBuild_GTopo
(const Standard_Boolean ii,const Standard_Boolean in,const Standard_Boolean io,
 const Standard_Boolean ni,const Standard_Boolean nn,const Standard_Boolean no,
 const Standard_Boolean oi,const Standard_Boolean on,const Standard_Boolean oo,
 const TopAbs_ShapeEnum t1, const TopAbs_ShapeEnum t2,
 const TopOpeBRepDS_Config C1, const TopOpeBRepDS_Config C2)
{
  Reset();
  Set (ii, in, io,
       ni, nn, no,
       oi, on, oo);
  myt1 = t1; 
  myt2 = t2;
  myConfig1 = C1;
  myConfig2 = C2;
}

//=======================================================================
//function : Reset
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_GTopo::Reset()
{
  myt1 = myt2 = TopAbs_SHAPE;
  mycases[0][0] = mycases[0][1] = mycases[0][2] =   
  mycases[1][0] = mycases[1][1] = mycases[1][2] = 
  mycases[2][0] = mycases[2][1] = mycases[2][2] = Standard_False;
  myConfig1 = myConfig2 = TopOpeBRepDS_UNSHGEOMETRY;
  myReverseForce = myReverseValue = Standard_False;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_GTopo::Set
(const Standard_Boolean ii,const Standard_Boolean in,const Standard_Boolean io,
 const Standard_Boolean ni,const Standard_Boolean nn,const Standard_Boolean no,
 const Standard_Boolean oi,const Standard_Boolean on,const Standard_Boolean oo)
{
  mycases[0][0] = ii; mycases[0][1] = in; mycases[0][2] = io;   
  mycases[1][0] = ni; mycases[1][1] = nn; mycases[1][2] = no;
  mycases[2][0] = oi; mycases[2][1] = on; mycases[2][2] = oo;
}

//=======================================================================
//function : Type
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_GTopo::Type(TopAbs_ShapeEnum& t1, 
				 TopAbs_ShapeEnum& t2) const 
{
  t1 = myt1;
  t2 = myt2;
}

//=======================================================================
//function : ChangeType
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_GTopo::ChangeType(const TopAbs_ShapeEnum t1, 
				       const TopAbs_ShapeEnum t2)
{
  myt1 = t1;
  myt2 = t2;
}

//=======================================================================
//function : Config1
//purpose  : 
//=======================================================================

TopOpeBRepDS_Config TopOpeBRepBuild_GTopo::Config1() const
{
  return myConfig1;
}

//=======================================================================
//function : Config2
//purpose  : 
//=======================================================================

TopOpeBRepDS_Config TopOpeBRepBuild_GTopo::Config2() const
{
  return myConfig2;
}

//=======================================================================
//function : ChangeConfig
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_GTopo::ChangeConfig(const TopOpeBRepDS_Config C1,
					 const TopOpeBRepDS_Config C2)
{
  myConfig1 = C1;
  myConfig2 = C2;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepBuild_GTopo::Value(const Standard_Integer i1,
					      const Standard_Integer i2) const 
{
  Standard_Boolean b = mycases[i1][i2];
  return b;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepBuild_GTopo::Value(const TopAbs_State s1,
					      const TopAbs_State s2) const  
{
  Standard_Integer i1 = GIndex(s1);
  Standard_Integer i2 = GIndex(s2);
  Standard_Boolean b = mycases[i1][i2];
  return b;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepBuild_GTopo::Value(const Standard_Integer II) const
{
  Standard_Integer i1=0,i2=0;
  Index(II,i1,i2);
  Standard_Boolean b = Value(i1,i2);
  return b;
}

//=======================================================================
//function : ChangeValue
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_GTopo::ChangeValue(const Standard_Integer i1,
					const Standard_Integer i2,
					const Standard_Boolean b)
{
  mycases[i1][i2] = b;
}

//=======================================================================
//function : ChangeValue
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_GTopo::ChangeValue(const TopAbs_State s1, 
					const TopAbs_State s2,
					const Standard_Boolean b)
{
  Standard_Integer i1 = GIndex(s1);
  Standard_Integer i2 = GIndex(s2);
  mycases[i1][i2] = b;
}

//=======================================================================
//function : GIndex
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRepBuild_GTopo::GIndex(const TopAbs_State s) const
{
  if      (s == TopAbs_IN ) return 0;
  else if (s == TopAbs_ON ) return 1;
  else if (s == TopAbs_OUT) return 2;
  else throw Standard_ProgramError("GIndex : bad input");
}

//=======================================================================
//function : GState
//purpose  : 
//=======================================================================

TopAbs_State TopOpeBRepBuild_GTopo::GState(const Standard_Integer i) const
{
  if      (i == 0) return TopAbs_IN;
  else if (i == 1) return TopAbs_ON; 
  else if (i == 2) return TopAbs_OUT;
  else throw Standard_ProgramError("GState : bad input");
}

//=======================================================================
//function : Index
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_GTopo::Index(const Standard_Integer II,
				  Standard_Integer& i1, 
				  Standard_Integer& i2) const 
{
  switch (II) {
  case 0 : i1 = 0; i2 = 0; break;
  case 1 : i1 = 0; i2 = 1; break;
  case 2 : i1 = 0; i2 = 2; break;
  case 3 : i1 = 1; i2 = 0; break;
  case 4 : i1 = 1; i2 = 1; break;
  case 5 : i1 = 1; i2 = 2; break;
  case 6 : i1 = 2; i2 = 0; break;
  case 7 : i1 = 2; i2 = 1; break;
  case 8 : i1 = 2; i2 = 2; break;
  }
}

//=======================================================================
//function : DumpVal
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_GTopo::DumpVal(Standard_OStream& OS,
				    const TopAbs_State s1,
				    const TopAbs_State s2) const 
{
  OS<<Value(s1,s2);
}

//=======================================================================
//function : DumpType
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_GTopo::DumpType(Standard_OStream& OS) const 
{
  TopAbs::Print(myt1,OS); OS<<"/"; TopAbs::Print(myt2,OS);
}

//=======================================================================
//function : DumpSSB
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_GTopo::DumpSSB(Standard_OStream& OS,
				    const TopAbs_State s1,
				    const TopAbs_State s2,
				    const Standard_Boolean b)
{
  TopAbs::Print(s1,OS); OS<<" "; TopAbs::Print(s2,OS); OS<<" : "<<b;
}

//=======================================================================
//function : Dump
//purpose  :
//=======================================================================

void TopOpeBRepBuild_GTopo::Dump(Standard_OStream& OS,
				 const Standard_Address a) const
{
  char* s = (char*)a;

  DumpType(OS);
  OS<<" "; TopOpeBRepDS::Print(myConfig1,OS);
  OS<<" "; TopOpeBRepDS::Print(myConfig2,OS);
  OS<<endl;

  if (myReverseForce) OS<<"reverse value : "<<myReverseValue<<endl;

  if(s) OS<<s;
  OS<<"\\ I N O";
  OS<<endl;

  if(s) OS<<s;
  OS<<"I ";
  DumpVal(OS,TopAbs_IN,TopAbs_IN); OS<<" ";
  DumpVal(OS,TopAbs_IN,TopAbs_ON); OS<<" ";
  DumpVal(OS,TopAbs_IN,TopAbs_OUT); OS<<endl;

  if(s) OS<<s;
  OS<<"N ";
  DumpVal(OS,TopAbs_ON,TopAbs_IN); OS<<" ";
  DumpVal(OS,TopAbs_ON,TopAbs_ON); OS<<" ";
  DumpVal(OS,TopAbs_ON,TopAbs_OUT); OS<<endl;

  if(s) OS<<s;
  OS<<"O ";
  DumpVal(OS,TopAbs_OUT,TopAbs_IN); OS<<" ";
  DumpVal(OS,TopAbs_OUT,TopAbs_ON); OS<<" ";
  DumpVal(OS,TopAbs_OUT,TopAbs_OUT); OS<<endl;
}

//=======================================================================
//function : States
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_GTopo::StatesON(TopAbs_State& s1,
				     TopAbs_State& s2) const  
{
  s1 = TopAbs_UNKNOWN;
  if      ( Value(TopAbs_ON,TopAbs_IN) ) s1 = TopAbs_IN;
  else if ( Value(TopAbs_ON,TopAbs_OUT)) s1 = TopAbs_OUT;

  s2 = TopAbs_UNKNOWN;
  if      ( Value(TopAbs_IN,TopAbs_ON) ) s2 = TopAbs_IN;
  else if ( Value(TopAbs_OUT,TopAbs_ON)) s2 = TopAbs_OUT;

  if ( s1 == TopAbs_UNKNOWN || s2 == TopAbs_UNKNOWN ) {
    throw Standard_ProgramError("Gtopo : StatesON incorrect");
  }
}

//=======================================================================
//function : IsToReverse1
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepBuild_GTopo::IsToReverse1() const 
{
  if (myReverseForce) {
    return myReverseValue;
  }
  else {
    TopAbs_State s1,s2;StatesON(s1,s2);
    Standard_Boolean IsToRev;
    if (s1 == TopAbs_IN && s2 == TopAbs_IN) IsToRev = Standard_False;
    else IsToRev = (s1 == TopAbs_IN);
    return IsToRev;
  }
//  throw Standard_ProgramError("GTopo::IsToReverse1");
//  return Standard_False; // dummy
}


//=======================================================================
//function : IsToReverse2
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepBuild_GTopo::IsToReverse2() const 
{
  if (myReverseForce) {
    return myReverseValue;
  }
  else {
    TopAbs_State s1,s2;
    StatesON(s1,s2);
    Standard_Boolean IsToRev;
    if (s1 == TopAbs_IN && s2 == TopAbs_IN) IsToRev = Standard_False;
    else IsToRev = (s2 == TopAbs_IN);
    return IsToRev;
  }
}

//=======================================================================
//function : SetReverse
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_GTopo::SetReverse(const Standard_Boolean rev)
{
  myReverseForce = Standard_True;
  myReverseValue = rev;
}

//=======================================================================
//function : Reverse
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepBuild_GTopo::Reverse() const 
{
  if (myReverseForce) return myReverseValue;
  throw Standard_ProgramError("GTopo::ReverseValue undefined");
}

//=======================================================================
//function : CopyPermuted
//purpose  : 
//=======================================================================

TopOpeBRepBuild_GTopo TopOpeBRepBuild_GTopo::CopyPermuted() const 
{
  TopOpeBRepBuild_GTopo g;

  g.ChangeType(myt2,myt1);
  g.ChangeConfig(myConfig2,myConfig1);
  Standard_Integer i,j;
  for (i=0; i<3; i++) for (j=0; j<3; j++) g.ChangeValue(j,i,Value(i,j));
  if (myReverseForce) g.SetReverse(myReverseValue);

  return g;
}
