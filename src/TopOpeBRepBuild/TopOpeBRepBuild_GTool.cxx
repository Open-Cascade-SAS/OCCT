// Created on: 1996-02-13
// Created by: Jean Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <TopOpeBRepBuild_GIter.hxx>
#include <TopOpeBRepBuild_GTool.hxx>
#include <TopOpeBRepBuild_GTopo.hxx>

//=======================================================================
//function : GFusUnsh
//purpose  : 
//=======================================================================
TopOpeBRepBuild_GTopo TopOpeBRepBuild_GTool::GFusUnsh
(const TopAbs_ShapeEnum t1, const TopAbs_ShapeEnum t2)
{
  return TopOpeBRepBuild_GTopo(false, false, false,
			       false, false, true,
			       false, true,  false,
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
  return TopOpeBRepBuild_GTopo(false, false, false,
			       false, true, true,
			       false, true, false,
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
  return TopOpeBRepBuild_GTopo(false, false, false,
			       false, false, true,
			       false, true,  false,
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
  return TopOpeBRepBuild_GTopo(false, true, false,
			       false, false, true,
			       false, false, false,
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
  return TopOpeBRepBuild_GTopo(false, true, false,
			       false, false, true,
			       false, false, false,
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
  return TopOpeBRepBuild_GTopo(false, true, false,
			       false, true,  true,
			       false, false, false,
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
  return TopOpeBRepBuild_GTopo(false, true, false,
			       true,  false, false,
			       false, false, false,
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
  return TopOpeBRepBuild_GTopo(false, true, false,
			       true,  true,  false,
			       false, false, false,
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
  return TopOpeBRepBuild_GTopo(false, true, false,
			       true,  false, false,
			       false, false, false,
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
