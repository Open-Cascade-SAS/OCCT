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
