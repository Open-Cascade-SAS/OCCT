// File:	TopOpeBRepBuild_TopOpeBRepBuild_GIter.cxx
// Created:	Tue Feb 13 17:50:18 1996
// Author:	Jean Yves LEBEY
//		<jyl@meteox>

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
