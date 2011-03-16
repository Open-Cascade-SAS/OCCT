// File:	TopOpeBRepDS_SurfaceIterator.cxx
// Created:	Tue Jun  7 18:28:35 1994
// Author:	Jean Yves LEBEY
//		<jyl@meteox>

#include <TopOpeBRepDS_Interference.hxx>

#include <TopOpeBRepDS_SurfaceIterator.ixx>

//=======================================================================
//function : TopOpeBRepDS_SurfaceIterator
//purpose  : 
//=======================================================================

TopOpeBRepDS_SurfaceIterator::TopOpeBRepDS_SurfaceIterator
  (const TopOpeBRepDS_ListOfInterference& L) :
  TopOpeBRepDS_InterferenceIterator(L)
{
  TopOpeBRepDS_InterferenceIterator::GeometryKind(TopOpeBRepDS_SURFACE);
}

//=======================================================================
//function : Current
//purpose  : 
//=======================================================================

Standard_Integer  TopOpeBRepDS_SurfaceIterator::Current()const 
{
  Handle(TopOpeBRepDS_Interference) i = Value();
  Standard_Integer g = i->Geometry();
  return g;
}


//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================

TopAbs_Orientation TopOpeBRepDS_SurfaceIterator::Orientation
  (const TopAbs_State S) const 
{
  Handle(TopOpeBRepDS_Interference) i = Value();
  const TopOpeBRepDS_Transition& t = i->Transition();
  TopAbs_Orientation o = t.Orientation(S);
  return o;
}
