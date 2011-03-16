// File:	TopOpeBRepDS_CurveIterator.cxx
// Created:	Thu Jun 17 19:13:46 1993
// Author:	Jean Yves LEBEY
//		<jyl@zerox>


#include <TopOpeBRepDS_CurveIterator.ixx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_SurfaceCurveInterference.hxx>

//=======================================================================
//function : TopOpeBRepDS_CurveIterator
//purpose  : 
//=======================================================================

TopOpeBRepDS_CurveIterator::TopOpeBRepDS_CurveIterator
  (const TopOpeBRepDS_ListOfInterference& L) :
  TopOpeBRepDS_InterferenceIterator(L)
{
  Match();
}

//=======================================================================
//function : MatchInterference
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_CurveIterator::MatchInterference
   (const Handle(TopOpeBRepDS_Interference)& I) const
{
  TopOpeBRepDS_Kind GT = I->GeometryType();
  Standard_Boolean r = ( GT == TopOpeBRepDS_CURVE );
  return r;
}


//=======================================================================
//function : Current
//purpose  : 
//=======================================================================

Standard_Integer  TopOpeBRepDS_CurveIterator::Current()const 
{
  Handle(TopOpeBRepDS_Interference) I = Value();
  Standard_Integer G = I->Geometry();
  return G;
}


//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================

TopAbs_Orientation  TopOpeBRepDS_CurveIterator::Orientation
  (const TopAbs_State S)const 
{
  Handle(TopOpeBRepDS_Interference) I = Value();
  const TopOpeBRepDS_Transition& T = I->Transition();
  TopAbs_Orientation o = T.Orientation(S);
  return o;
}


//=======================================================================
//function : PCurve
//purpose  : 
//=======================================================================

const Handle(Geom2d_Curve)&  TopOpeBRepDS_CurveIterator::PCurve()const 
{
  return 
    (*((Handle(TopOpeBRepDS_SurfaceCurveInterference)*)&Value()))->PCurve();
}

