// Created on: 1993-07-22
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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

#include <BRepTest.hxx>
#include <DBRep.hxx>
#include <Draw_Interpretor.hxx>
#include <DrawTrSurf.hxx>
#include <Geom_Plane.hxx>
#include <Draw_Appli.hxx>
#include <BRep_Builder.hxx>

#include <BRepAlgo_Fuse.hxx>
#include <BRepAlgo_Common.hxx>
#include <BRepAlgo_Cut.hxx>
#include <BRepAlgo_Section.hxx>

#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepPrimAPI_MakeHalfSpace.hxx>
#include <BRepAlgo_FaceRestrictor.hxx>
#include <BRepExtrema_ExtPF.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Wire.hxx>
#include <TopExp_Explorer.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <gp.hxx>
#include <gp_Pln.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>

//=======================================================================
// topop
//=======================================================================

static Standard_Integer topop(Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 4) return 1;

  TopoDS_Shape s1 = DBRep::Get(a[2]);
  TopoDS_Shape s2 = DBRep::Get(a[3]);

  if (s1.IsNull() || s2.IsNull()) return 1;

  TopoDS_Shape res;

  if (*a[0] == 'f')
    res = BRepAlgo_Fuse(s1,s2);
  else if (*(a[0]+1) == 'o')
    res = BRepAlgo_Common(s1,s2);
  else 
    res = BRepAlgo_Cut(s1,s2);

  DBRep::Set(a[1],res);

  return 0;
}


//=======================================================================
// section
//=======================================================================

static Standard_Integer section(Draw_Interpretor& , Standard_Integer n, const char** a)
{

  if (n < 4) return 1;

  TopoDS_Shape s1 = DBRep::Get(a[2]);
  TopoDS_Shape s2 = DBRep::Get(a[3]);
  
  if (s1.IsNull() || s2.IsNull())
    return 1;

  BRepAlgo_Section Sec(s1, s2, Standard_False);
  TopoDS_Shape res;

  for (int i=4; i < n; i++) {
    if (!strcasecmp(a[i], "-2d"))
    {
      Sec.ComputePCurveOn1(Standard_True);
      Sec.ComputePCurveOn2(Standard_True);
    } 
    else if (!strcasecmp(a[i], "-2d1")) 
    {
      Sec.ComputePCurveOn1(Standard_True);
      Sec.ComputePCurveOn2(Standard_False);
    } 
    else if (!strcasecmp(a[i], "-2d2")) 
    {
      Sec.ComputePCurveOn1(Standard_False);
      Sec.ComputePCurveOn2(Standard_True);
    } 
    else if (!strcasecmp(a[i], "-no2d"))
    {
      Sec.ComputePCurveOn1(Standard_False);
      Sec.ComputePCurveOn2(Standard_False);
    } 
    else if (!strcasecmp(a[i], "-a")) 
      Sec.Approximation(Standard_True);
    else if (strcasecmp(a[i], "-p"))
    {
      cout << "Unknown option: " << a[i] << endl;
      return 1;
    }
  }

  res = Sec.Shape();
  
  DBRep::Set(a[1],res);
  
  return 0;
}

//=======================================================================
// psection
//=======================================================================

static Standard_Integer psection(Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 4) return 1;

  TopoDS_Shape s = DBRep::Get(a[2]);
  if (s.IsNull()) return 1;

  Handle(Geom_Surface) ps = DrawTrSurf::GetSurface(a[3]);
  if (ps.IsNull()) return 1;

  Handle(Geom_Plane) pg = Handle(Geom_Plane)::DownCast(ps);
  if (pg.IsNull()) return 1;

  const gp_Pln& p = pg->Pln();

  TopoDS_Shape res = BRepAlgo_Section(s,p);

  DBRep::Set(a[1],res);

  return 0;
}

static Standard_Integer halfspace(Draw_Interpretor& di,
				  Standard_Integer n, const char** a)
{
  if (n < 6) return 1;

  // Le point indiquant le cote "matiere".
  gp_Pnt RefPnt = gp_Pnt(Draw::Atof(a[3]),Draw::Atof(a[4]),Draw::Atof(a[5]));

  TopoDS_Shape Face = DBRep::Get(a[2],TopAbs_FACE);
  if ( Face.IsNull()) {
    TopoDS_Shape Shell  = DBRep::Get(a[2],TopAbs_SHELL);
    if (Shell.IsNull()) {
      //cout << a[2] << " must be a face or a shell" << endl;
      di << a[2] << " must be a face or a shell\n";
      return 1;
    }
    else {
      BRepPrimAPI_MakeHalfSpace Half(TopoDS::Shell(Shell),RefPnt);
      if ( Half.IsDone()) {
	DBRep::Set(a[1],Half.Solid());
      }
      else {
	//cout << " HalfSpace NotDone" << endl;
	di << " HalfSpace NotDone\n";
	return 1;
      }
    }
  }
  else {
    BRepPrimAPI_MakeHalfSpace Half(TopoDS::Face(Face),RefPnt);
    if ( Half.IsDone()) {
      DBRep::Set(a[1],Half.Solid());
    }
    else {
      //cout << " HalfSpace NotDone" << endl;
      di << " HalfSpace NotDone\n";
      return 1;
    }
  }
  return 0;
}

//=======================================================================
//function : buildfaces
//purpose  : 
//=======================================================================

static Standard_Integer buildfaces(Draw_Interpretor& , Standard_Integer narg, const char** a)
{
  if (narg < 4) return 1;
  
  TopoDS_Shape InputShape(DBRep::Get( a[2] ,TopAbs_FACE));
  TopoDS_Face F = TopoDS::Face(InputShape);
//  TopoDS_Face F = TopoDS::Face(DBRep::Get(a[2],TopAbs_FACE));
  BRepAlgo_FaceRestrictor FR;
  FR.Init(F);
  
  for (Standard_Integer i = 3 ; i < narg ; i++) {
    TopoDS_Shape InputWire(DBRep::Get(a[i],TopAbs_WIRE));
    TopoDS_Wire W = TopoDS::Wire(InputWire);
//    TopoDS_Wire W = TopoDS::Wire(DBRep::Get(a[i],TopAbs_WIRE));
    FR.Add(W);
  }
  FR.Perform();
  if (!FR.IsDone()) return 1;
  
  TopoDS_Compound Res;
  BRep_Builder BB;
  BB.MakeCompound(Res);

  for (; FR.More(); FR.Next()) {
    TopoDS_Face FF = FR.Current();
    BB.Add(Res,FF);
    DBRep::Set(a[1],Res);
  }
  return 0;
}

//=======================================================================
//function : TopologyCommands
//purpose  : 
//=======================================================================

void  BRepTest::TopologyCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;

  DBRep::BasicCommands(theCommands);

  const char* g = "TOPOLOGY Topological operation commands";
  
  theCommands.Add("fuse","fuse result s1 s2",__FILE__,topop,g);
  theCommands.Add("common","common result s1 s2",__FILE__,topop,g);
  theCommands.Add("cut","cut result part tool",__FILE__,topop,g);
  theCommands.Add("section","section result s1 s2 [-no2d/-2d/-2d1/-2d2] [-p/-a]",__FILE__,section,g);
  theCommands.Add("psection","psection result s plane",__FILE__,psection,g);
  theCommands.Add("halfspace","halfspace result face/shell x y z",__FILE__,halfspace,g);
  theCommands.Add("buildfaces","buildfaces result faceReference wire1 wire2 ...",__FILE__,buildfaces,g);
}
