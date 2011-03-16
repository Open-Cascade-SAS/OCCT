// File:	TopOpeBRepDS_FaceEdgeInterference.cxx
// Created:	Fri Oct 28 17:42:34 1994
// Author:	Jean Yves LEBEY
//		<jyl@bravox>

#include <TopOpeBRepDS_FaceEdgeInterference.ixx>
#include <TopOpeBRepDS_Kind.hxx>

//=======================================================================
//function : TopOpeBRepDS_FaceEdgeInterference
//purpose  : 
//=======================================================================

TopOpeBRepDS_FaceEdgeInterference::TopOpeBRepDS_FaceEdgeInterference
  (const TopOpeBRepDS_Transition& T, 
   const Standard_Integer         S, 
   const Standard_Integer         G,
   const Standard_Boolean         GIsBound, 
   const TopOpeBRepDS_Config      C) :
  TopOpeBRepDS_ShapeShapeInterference
  (T,TopOpeBRepDS_FACE,S,TopOpeBRepDS_EDGE,G,GIsBound,C)
{
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TopOpeBRepDS_FaceEdgeInterference::Dump
(Standard_OStream& OS) const
{
#ifdef DEB
  OS<<"FEI ";
  TopOpeBRepDS_ShapeShapeInterference::Dump(OS);
#endif

  return OS;
}
