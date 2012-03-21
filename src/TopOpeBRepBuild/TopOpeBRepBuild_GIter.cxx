// Created on: 1996-02-13
// Created by: Jean Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
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


#include <TopOpeBRepBuild_GIter.ixx>
#include <TopOpeBRepBuild_GTopo.hxx>
#define MYGTOPO (*((TopOpeBRepBuild_GTopo*)mypG))

TopOpeBRepBuild_GIter::TopOpeBRepBuild_GIter() : myII(0),mypG(NULL)
{
}

TopOpeBRepBuild_GIter::TopOpeBRepBuild_GIter(const TopOpeBRepBuild_GTopo& G) :
myII(0),mypG(NULL)
{
  Init(G);
}

void TopOpeBRepBuild_GIter::Find()
{
  while ( myII <= 8 ) {
    Standard_Boolean b = MYGTOPO.Value(myII);
    if (b) break;
    myII++;
  }
}

void TopOpeBRepBuild_GIter::Init()
{
  myII = 0;
  Find();
}

void TopOpeBRepBuild_GIter::Init(const TopOpeBRepBuild_GTopo& G)
{
  mypG = (Standard_Address)&G;
  Init();
}

Standard_Boolean TopOpeBRepBuild_GIter::More() const 
{
  if (myII <= 8) {
    Standard_Boolean b = MYGTOPO.Value(myII);
    return b;
  }
  else 
    return Standard_False;
}

void TopOpeBRepBuild_GIter::Next()
{
  myII++;
  Find();
}

void TopOpeBRepBuild_GIter::Current(TopAbs_State& s1, TopAbs_State& s2) const 
{
  if ( !More() ) return;
  Standard_Integer i1,i2; 
  MYGTOPO.Index(myII,i1,i2);
  s1 = MYGTOPO.GState(i1);
  s2 = MYGTOPO.GState(i2);
}

void TopOpeBRepBuild_GIter::Dump(Standard_OStream& OS) const 
{
  if ( !More()) return;
  TopAbs_State s1,s2; Current(s1,s2);
  Standard_Boolean b = MYGTOPO.Value(s1,s2);
  TopOpeBRepBuild_GTopo::DumpSSB(OS,s1,s2,b); OS<<endl;
}
