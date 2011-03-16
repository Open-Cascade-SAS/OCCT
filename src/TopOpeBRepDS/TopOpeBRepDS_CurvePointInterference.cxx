// File:	TopOpeBRepDS_CurvePointInterference.cxx
// Created:	Wed Jun 23 19:11:24 1993
// Author:	Jean Yves LEBEY
//		<jyl@zerox>


#include <TopOpeBRepDS_CurvePointInterference.ixx>

//=======================================================================
//function : TopOpeBRepDS_CurvePointInterference
//purpose  : 
//=======================================================================

TopOpeBRepDS_CurvePointInterference::TopOpeBRepDS_CurvePointInterference
  (const TopOpeBRepDS_Transition& T, 
   const TopOpeBRepDS_Kind ST,
   const Standard_Integer S, 
   const TopOpeBRepDS_Kind GT,
   const Standard_Integer G, 
   const Standard_Real P) :
  TopOpeBRepDS_Interference(T,ST,S,GT,G),
  myParam(P)
{
}


//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

Standard_Real  TopOpeBRepDS_CurvePointInterference::Parameter()const 
{
  return myParam;
}


//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_CurvePointInterference::Parameter(const Standard_Real P)
{
  myParam = P;
}


//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TopOpeBRepDS_CurvePointInterference::Dump
  (Standard_OStream& OS) const
{
#ifdef DEB
  TopOpeBRepDS_Kind supporttype = SupportType();
  if      (supporttype == TopOpeBRepDS_EDGE)  OS<<"EPI";
  else if (supporttype == TopOpeBRepDS_CURVE) OS<<"CPI";
  else if (supporttype == TopOpeBRepDS_FACE)  OS<<"ECPI";
  else                                        OS<<"???";
  OS<<" "; TopOpeBRepDS_Interference::Dump(OS);
  OS<<" "<<myParam; 
#endif

  return OS;
}
