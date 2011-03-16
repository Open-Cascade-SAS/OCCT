// File:	TopOpeBRepDS_EdgeVertexInterference.cxx
// Created:	Fri Oct 28 17:42:34 1994
// Author:	Jean Yves LEBEY
//		<jyl@bravox>


#include <TopOpeBRepDS_EdgeVertexInterference.ixx>
#include <TopOpeBRepDS_Kind.hxx>

//=======================================================================
//function : TopOpeBRepDS_EdgeVertexInterference
//purpose  : 
//=======================================================================

TopOpeBRepDS_EdgeVertexInterference::TopOpeBRepDS_EdgeVertexInterference
  (const TopOpeBRepDS_Transition& T, 
   const TopOpeBRepDS_Kind        ST,
   const Standard_Integer         S, 
   const Standard_Integer         G,
   const Standard_Boolean         GIsBound, 
   const TopOpeBRepDS_Config      C,
   const Standard_Real            P) :
  TopOpeBRepDS_ShapeShapeInterference
  (T,ST,S,TopOpeBRepDS_VERTEX,G,GIsBound,C),
  myParam(P)
{
}

//=======================================================================
//function : TopOpeBRepDS_EdgeVertexInterference
//purpose  : 
//=======================================================================

TopOpeBRepDS_EdgeVertexInterference::TopOpeBRepDS_EdgeVertexInterference
  (const TopOpeBRepDS_Transition& T, 
   const Standard_Integer         S, 
   const Standard_Integer         G,
   const Standard_Boolean         GIsBound, 
   const TopOpeBRepDS_Config      C,
   const Standard_Real            P) :
  TopOpeBRepDS_ShapeShapeInterference
  (T,TopOpeBRepDS_EDGE,S,TopOpeBRepDS_VERTEX,G,GIsBound,C),
  myParam(P)
{
}


//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

Standard_Real  TopOpeBRepDS_EdgeVertexInterference::Parameter()const 
{
  return myParam;
}


//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_EdgeVertexInterference::Parameter(const Standard_Real P)
{
  myParam = P;
}


//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TopOpeBRepDS_EdgeVertexInterference::Dump
  (Standard_OStream& OS) const
{
#ifdef DEB
  OS<<"EVI"<<" ";
  TopOpeBRepDS_ShapeShapeInterference::Dump(OS);
  OS<<" "<<myParam<<" ";
#endif

  return OS;
}
