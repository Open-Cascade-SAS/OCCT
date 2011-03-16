// File:	TopOpeBRepBuild_GTool.cxx
// Created:	Tue Feb 13 17:40:02 1996
// Author:	Jean Yves LEBEY
//		<jyl@meteox>

#include <TopOpeBRepBuild_GTool.ixx>
#include <TopOpeBRepBuild_GIter.hxx>

//=======================================================================
//function : GFusUnsh
//purpose  : 
//=======================================================================

TopOpeBRepBuild_GTopo TopOpeBRepBuild_GTool::GFusUnsh
(const TopAbs_ShapeEnum t1, const TopAbs_ShapeEnum t2)
{
  return TopOpeBRepBuild_GTopo(0,0,0,
			       0,0,1,
			       0,1,0,
			       t1,t2,
			       TopOpeBRepDS_UNSHGEOMETRY,TopOpeBRepDS_UNSHGEOMETRY);
}

//=======================================================================
//function : GFusSame
//purpose  : 
//=======================================================================

TopOpeBRepBuild_GTopo TopOpeBRepBuild_GTool::GFusSame
(const TopAbs_ShapeEnum t1, const TopAbs_ShapeEnum t2)
{
  return TopOpeBRepBuild_GTopo(0,0,0,
			       0,1,1,
			       0,1,0,
			       t1,t2,
			       TopOpeBRepDS_SAMEORIENTED,TopOpeBRepDS_SAMEORIENTED);
}

//=======================================================================
//function : GFusDiff
//purpose  : 
//=======================================================================

TopOpeBRepBuild_GTopo TopOpeBRepBuild_GTool::GFusDiff
(const TopAbs_ShapeEnum t1, const TopAbs_ShapeEnum t2)
{
  return TopOpeBRepBuild_GTopo(0,0,0,
			       0,0,1,
			       0,1,0,
			       t1,t2,
			       TopOpeBRepDS_DIFFORIENTED,TopOpeBRepDS_SAMEORIENTED);
}

//=======================================================================
//function : GCutUnsh
//purpose  : 
//=======================================================================

TopOpeBRepBuild_GTopo TopOpeBRepBuild_GTool::GCutUnsh
(const TopAbs_ShapeEnum t1, const TopAbs_ShapeEnum t2)
{
  return TopOpeBRepBuild_GTopo(0,1,0,
			       0,0,1,
			       0,0,0,
			       t1,t2,
			       TopOpeBRepDS_UNSHGEOMETRY,TopOpeBRepDS_UNSHGEOMETRY);
}

//=======================================================================
//function : GCutSame
//purpose  : 
//=======================================================================

TopOpeBRepBuild_GTopo TopOpeBRepBuild_GTool::GCutSame
(const TopAbs_ShapeEnum t1, const TopAbs_ShapeEnum t2)
{
  return TopOpeBRepBuild_GTopo(0,1,0,
			       0,0,1,
			       0,0,0,
			       t1,t2,
			       TopOpeBRepDS_SAMEORIENTED,TopOpeBRepDS_SAMEORIENTED);
}

//=======================================================================
//function : GCutDiff
//purpose  : 
//=======================================================================

TopOpeBRepBuild_GTopo TopOpeBRepBuild_GTool::GCutDiff
(const TopAbs_ShapeEnum t1, const TopAbs_ShapeEnum t2)
{
  return TopOpeBRepBuild_GTopo(0,1,0,
			       0,1,1,
			       0,0,0,
			       t1,t2,
			       TopOpeBRepDS_DIFFORIENTED,TopOpeBRepDS_SAMEORIENTED);
}


//=======================================================================
//function : GComUnsh
//purpose  : 
//=======================================================================

TopOpeBRepBuild_GTopo TopOpeBRepBuild_GTool::GComUnsh
(const TopAbs_ShapeEnum t1, const TopAbs_ShapeEnum t2)
{
  return TopOpeBRepBuild_GTopo(0,1,0,
			       1,0,0,
			       0,0,0,
			       t1,t2,
			       TopOpeBRepDS_UNSHGEOMETRY,TopOpeBRepDS_UNSHGEOMETRY);
}

//=======================================================================
//function : GComSame
//purpose  : 
//=======================================================================

TopOpeBRepBuild_GTopo TopOpeBRepBuild_GTool::GComSame
(const TopAbs_ShapeEnum t1, const TopAbs_ShapeEnum t2)
{
  return TopOpeBRepBuild_GTopo(0,1,0,
			       1,1,0,
			       0,0,0,
			       t1,t2,
			       TopOpeBRepDS_SAMEORIENTED,TopOpeBRepDS_SAMEORIENTED);
}

//=======================================================================
//function : GComDiff
//purpose  : 
//=======================================================================

TopOpeBRepBuild_GTopo TopOpeBRepBuild_GTool::GComDiff
(const TopAbs_ShapeEnum t1, const TopAbs_ShapeEnum t2)
{
  return TopOpeBRepBuild_GTopo(0,1,0,
			       1,0,0,
			       0,0,0,
			       t1,t2,
			       TopOpeBRepDS_DIFFORIENTED,TopOpeBRepDS_SAMEORIENTED);
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_GTool::Dump(Standard_OStream& OS)
{
  TopOpeBRepBuild_GIter gi;
  TopOpeBRepBuild_GTopo g;

  g = TopOpeBRepBuild_GTool::GFusUnsh(TopAbs_FACE,TopAbs_FACE);
  g.Dump(OS); for (gi.Init(g); gi.More(); gi.Next()) gi.Dump(OS); OS<<endl;

  g = TopOpeBRepBuild_GTool::GFusSame(TopAbs_FACE,TopAbs_FACE);
  g.Dump(OS); for (gi.Init(g); gi.More(); gi.Next()) gi.Dump(OS); OS<<endl;

  g = TopOpeBRepBuild_GTool::GFusDiff(TopAbs_FACE,TopAbs_FACE);
  g.Dump(OS); for (gi.Init(g); gi.More(); gi.Next()) gi.Dump(OS); OS<<endl;
  
  g = TopOpeBRepBuild_GTool::GCutDiff(TopAbs_FACE,TopAbs_EDGE);
  g.Dump(OS); for (gi.Init(g); gi.More(); gi.Next()) gi.Dump(OS); OS<<endl;

  g = g.CopyPermuted();
  g.Dump(OS); for (gi.Init(g); gi.More(); gi.Next()) gi.Dump(OS); OS<<endl;

}
