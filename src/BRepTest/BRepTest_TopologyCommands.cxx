// Created on: 1993-07-22
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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


#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
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
#include <BRepLProp_SLProps.hxx>
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

#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif

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

  if (n > 4) {
#ifdef WNT
    if (!strcasecmp(a[4],"-2d") || !strcasecmp(a[4], "-no2d")) {
#else 
    if (!strncasecmp(a[4],"-2d", 3) || !strcasecmp(a[4], "-no2d")) {
#endif
      if (!strcasecmp(a[4], "-2d")) {
	Sec.ComputePCurveOn1(Standard_True);
	Sec.ComputePCurveOn2(Standard_True);
      } else
	if (!strcasecmp(a[4], "-2d1")) 
	  Sec.ComputePCurveOn1(Standard_True);
	else
	  if (!strcasecmp(a[4], "-2d2")) 
	    Sec.ComputePCurveOn2(Standard_True);
	  else
	    if (strcasecmp(a[4], "-no2d"))
	      return 1;
      if(n > 5)
	if (!strcasecmp(a[5], "-a")) 
	  Sec.Approximation(TopOpeBRepTool_APPROX);
	else 
	  if (strcasecmp(a[5], "-p"))
	    return 1;
    } else {// fin a[4],"-2d"
      if (!strcasecmp(a[4], "-a") || !strcasecmp(a[4], "-p")) {
	if (!strcasecmp(a[4], "-a")) 
	  Sec.Approximation(TopOpeBRepTool_APPROX);
	if(n > 5) {
#ifdef WNT
	  if (!strcasecmp(a[5],"-2d") || !strcasecmp(a[5], "-no2d")) {
#else 
	  if (!strncasecmp(a[5],"-2d", 3) || !strcasecmp(a[5], "-no2d")) {
#endif
	    if (!strcasecmp(a[5], "-2d")) {
	      Sec.ComputePCurveOn1(Standard_True);
	      Sec.ComputePCurveOn2(Standard_True);
	    } else
	      if (!strcasecmp(a[5], "-2d1")) 
		Sec.ComputePCurveOn1(Standard_True);
	      else
		if (!strcasecmp(a[5], "-2d2")) 
		  Sec.ComputePCurveOn2(Standard_True);
		else
		  if (strcasecmp(a[5], "-no2d"))
		    return 1;
	  }
	}
      } else // fin a[4],"-a"
	return 1;
    }
  }// fin n > 4

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
      di << a[2] << " must be a face or a shell" << "\n";
      return 1;
    }
    else {
      BRepPrimAPI_MakeHalfSpace Half(TopoDS::Shell(Shell),RefPnt);
      if ( Half.IsDone()) {
	DBRep::Set(a[1],Half.Solid());
      }
      else {
	//cout << " HalfSpace NotDone" << endl;
	di << " HalfSpace NotDone" << "\n";
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
      di << " HalfSpace NotDone" << "\n";
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
