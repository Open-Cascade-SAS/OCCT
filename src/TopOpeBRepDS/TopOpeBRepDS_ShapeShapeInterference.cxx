// File:	TopOpeBRepDS_ShapeShapeInterference.cxx
// Created:	Tue Aug 30 16:13:40 1994
// Author:	Jean Yves LEBEY
//		<jyl@meteox>

#include <TCollection_AsciiString.hxx>
#include <TopOpeBRepDS.hxx>
#include <TopOpeBRepDS_ShapeShapeInterference.ixx>

//=======================================================================
//function : TopOpeBRepDS_ShapeShapeInterference
//purpose  : 
//=======================================================================

TopOpeBRepDS_ShapeShapeInterference::TopOpeBRepDS_ShapeShapeInterference
  (const TopOpeBRepDS_Transition& T,
   const TopOpeBRepDS_Kind        ST, 
   const Standard_Integer         S, 
   const TopOpeBRepDS_Kind        GT, 
   const Standard_Integer         G, 
   const Standard_Boolean         GBound, 
   const TopOpeBRepDS_Config      C) :
   TopOpeBRepDS_Interference(T,ST,S,GT,G),
   myGBound(GBound),
   myC(C)
{
}


//=======================================================================
//function : Config
//purpose  : 
//=======================================================================

TopOpeBRepDS_Config TopOpeBRepDS_ShapeShapeInterference::Config()const 
{ return myC; }


//=======================================================================
//function : GBound
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_ShapeShapeInterference::GBound()const 
{ return myGBound; }

//=======================================================================
//function : SetGBound
//purpose  : 
//=======================================================================

void TopOpeBRepDS_ShapeShapeInterference::SetGBound(const Standard_Boolean b)
{ myGBound = b; }

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TopOpeBRepDS_ShapeShapeInterference::Dump
  (Standard_OStream& OS) const
{
#ifdef DEB
  OS<<"("; TopOpeBRepDS::Print(myC,OS); OS<<")"<<" ";
  if (myGBound) OS<<"(Gb 1) ";
  else          OS<<"(Gb 0) ";
  TopOpeBRepDS_Interference::Dump(OS);
#endif

  return OS;
}
