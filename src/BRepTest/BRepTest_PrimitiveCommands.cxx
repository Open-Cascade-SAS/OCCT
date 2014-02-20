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
#include <Draw_Appli.hxx>
#include <DrawTrSurf.hxx>
#include <TopoDS_Solid.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeWedge.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepPrimAPI_MakeTorus.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pln.hxx>


//=======================================================================
// box
//=======================================================================

static Standard_Integer box(Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 5) return 1;
  Standard_Real dx = Draw::Atof(a[n-3]);
  Standard_Real dy = Draw::Atof(a[n-2]);
  Standard_Real dz = Draw::Atof(a[n-1]);

  TopoDS_Solid S;

  if (n > 5) {
    if (n < 8) return 1;
    Standard_Real x = Draw::Atof(a[2]);
    Standard_Real y = Draw::Atof(a[3]);
    Standard_Real z = Draw::Atof(a[4]);
    S = BRepPrimAPI_MakeBox(gp_Pnt(x,y,z),dx,dy,dz);
  }
  else {
    S = BRepPrimAPI_MakeBox(dx,dy,dz);
  }

  DBRep::Set(a[1],S);
  return 0;
}

//=======================================================================
// wedge
//=======================================================================

static Standard_Integer wedge(Draw_Interpretor& , Standard_Integer n, const char** a)
{
  TopoDS_Solid S;

//  Standard_Integer i = 0;
  if ( n == 15 || n == 18) {
    gp_Pnt LocalP(Draw::Atof(a[2]),Draw::Atof(a[3]),Draw::Atof(a[4]));
    gp_Dir LocalN(Draw::Atof(a[5]),Draw::Atof(a[6]),Draw::Atof(a[7]));
    gp_Dir LocalVx(Draw::Atof(a[8]),Draw::Atof(a[9]),Draw::Atof(a[10]));
    gp_Ax2 Axis(LocalP,LocalN,LocalVx);
//    gp_Ax2 Axis(gp_Pnt(Draw::Atof(a[2]),Draw::Atof(a[3]),Draw::Atof(a[4])),
//		gp_Dir(Draw::Atof(a[5]),Draw::Atof(a[6]),Draw::Atof(a[7])),
//		gp_Dir(Draw::Atof(a[8]),Draw::Atof(a[9]),Draw::Atof(a[10])));
    if ( n == 15) {
      S = BRepPrimAPI_MakeWedge(Axis,
			    Draw::Atof(a[11]),Draw::Atof(a[12]),Draw::Atof(a[13]),Draw::Atof(a[14]));
    }
    else {
      S = BRepPrimAPI_MakeWedge(Axis,
			    Draw::Atof(a[11]),Draw::Atof(a[12]),Draw::Atof(a[13]),
			    Draw::Atof(a[14]),Draw::Atof(a[15]),Draw::Atof(a[16]),Draw::Atof(a[17]));
    }
  }
  else if (n == 6) {
    S = BRepPrimAPI_MakeWedge(Draw::Atof(a[2]),Draw::Atof(a[3]),Draw::Atof(a[4]),Draw::Atof(a[5]));
  }
  else if (n == 9){
    S = BRepPrimAPI_MakeWedge(Draw::Atof(a[2]),Draw::Atof(a[3]),Draw::Atof(a[4]),
			  Draw::Atof(a[5]),Draw::Atof(a[6]),Draw::Atof(a[7]),Draw::Atof(a[8]));
  }
  else
    return 1;

  DBRep::Set(a[1],S);
  return 0;
}

//=======================================================================
// cylinder
//=======================================================================

static Standard_Integer cylinder(Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 3) return 1;
  TopoDS_Solid S;
  Handle(Geom_Plane) P =
    Handle(Geom_Plane)::DownCast(DrawTrSurf::Get(a[2]));

  if (n == 4) {
      S = BRepPrimAPI_MakeCylinder(Draw::Atof(a[2]),Draw::Atof(a[3]));
  }
  else if (n == 5) {
    if (P.IsNull())
      S = BRepPrimAPI_MakeCylinder(Draw::Atof(a[2]),Draw::Atof(a[3]),Draw::Atof(a[4]) * (M_PI / 180.0));
    else
      S = BRepPrimAPI_MakeCylinder(P->Pln().Position().Ax2(),Draw::Atof(a[3]),Draw::Atof(a[4]));
  }
  else if (n == 6) {
    if (P.IsNull())
      return 1;
    else
      S = BRepPrimAPI_MakeCylinder(P->Pln().Position().Ax2(),Draw::Atof(a[3]),Draw::Atof(a[4]),Draw::Atof(a[5]) * (M_PI / 180.0));
  }
  else
    return 1;

  DBRep::Set(a[1],S);
  return 0;
}

//=======================================================================
// cone
//=======================================================================

static Standard_Integer cone(Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 3) return 1;
  TopoDS_Solid S;

  Handle(Geom_Plane) P =
    Handle(Geom_Plane)::DownCast(DrawTrSurf::Get(a[2]));

  if (n == 5) {
    S = BRepPrimAPI_MakeCone(Draw::Atof(a[2]),Draw::Atof(a[3]),Draw::Atof(a[4]));
  }
  else if (n == 6) {
    if (P.IsNull())
      S = BRepPrimAPI_MakeCone(Draw::Atof(a[2]),Draw::Atof(a[3]),Draw::Atof(a[4]),Draw::Atof(a[5]) * (M_PI / 180.0));
    else
      S = BRepPrimAPI_MakeCone(P->Pln().Position().Ax2(),Draw::Atof(a[3]),Draw::Atof(a[4]),Draw::Atof(a[5]));
  }
  else if (n == 7) {
    S = BRepPrimAPI_MakeCone(P->Pln().Position().Ax2(),Draw::Atof(a[3]),Draw::Atof(a[4]),Draw::Atof(a[5]),Draw::Atof(a[6]) * (M_PI / 180.0));
  }
  else
    return 1;

  DBRep::Set(a[1],S);
  return 0;
}

//=======================================================================
// sphere
//=======================================================================

static Standard_Integer sphere(Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 3) return 1;
  TopoDS_Solid S;

  Handle(Geom_Plane) P =
    Handle(Geom_Plane)::DownCast(DrawTrSurf::Get(a[2]));

  if (n == 3) {
    S = BRepPrimAPI_MakeSphere(Draw::Atof(a[2]));
  }
  else if (n == 4) {
    if (P.IsNull())
      S = BRepPrimAPI_MakeSphere(Draw::Atof(a[2]),Draw::Atof(a[3]) * (M_PI / 180.0));
    else
      S = BRepPrimAPI_MakeSphere(P->Pln().Position().Ax2(),Draw::Atof(a[3]));
  }
  else if (n == 5) {
    if (P.IsNull())
      S = BRepPrimAPI_MakeSphere(Draw::Atof(a[2]),Draw::Atof(a[3]) * (M_PI / 180.0),Draw::Atof(a[4]) * (M_PI / 180.0));
    else
      S = BRepPrimAPI_MakeSphere(P->Pln().Position().Ax2(),Draw::Atof(a[3]),Draw::Atof(a[4]) * (M_PI / 180.0));
  }
  else if (n == 6) {
    if (P.IsNull())
      S = BRepPrimAPI_MakeSphere(Draw::Atof(a[2]),Draw::Atof(a[3]) * (M_PI / 180.0),Draw::Atof(a[4]) * (M_PI / 180.0),Draw::Atof(a[5]) * (M_PI / 180.0));
    else
      S = BRepPrimAPI_MakeSphere(P->Pln().Position().Ax2(),Draw::Atof(a[3]),Draw::Atof(a[4]) * (M_PI / 180.0),Draw::Atof(a[5]) * (M_PI / 180.0));
  }
  else if (n == 7) {
    S = BRepPrimAPI_MakeSphere(P->Pln().Position().Ax2(),Draw::Atof(a[3]),Draw::Atof(a[4]) * (M_PI / 180.0),Draw::Atof(a[5]) * (M_PI / 180.0),Draw::Atof(a[6]) * (M_PI / 180.0));
  }
  else
    return 1;

  DBRep::Set(a[1],S);
  return 0;
}

//=======================================================================
// torus
//=======================================================================

static Standard_Integer torus(Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 3) return 1;
  TopoDS_Solid S;

  Handle(Geom_Plane) P =
    Handle(Geom_Plane)::DownCast(DrawTrSurf::Get(a[2]));

  if (n == 4) {
    S = BRepPrimAPI_MakeTorus(Draw::Atof(a[2]),Draw::Atof(a[3]));
  }
  else if (n == 5) {
    if (P.IsNull())
      S = BRepPrimAPI_MakeTorus(Draw::Atof(a[2]),Draw::Atof(a[3]),Draw::Atof(a[4]) * (M_PI / 180.0));
    else
      S = BRepPrimAPI_MakeTorus(P->Pln().Position().Ax2(),Draw::Atof(a[3]),Draw::Atof(a[4]));
  }
  else if (n == 6) {
    if (P.IsNull())
      S = BRepPrimAPI_MakeTorus(Draw::Atof(a[2]),Draw::Atof(a[3]),
			    Draw::Atof(a[4]) * (M_PI / 180.0),Draw::Atof(a[5]) * (M_PI / 180.0));
    else
      S = BRepPrimAPI_MakeTorus(P->Pln().Position().Ax2(),
			    Draw::Atof(a[3]),Draw::Atof(a[4]),Draw::Atof(a[5]) * (M_PI / 180.0));
  }
  else if (n == 7) {
    if (P.IsNull())
      S = BRepPrimAPI_MakeTorus(Draw::Atof(a[2]),Draw::Atof(a[3]),
			    Draw::Atof(a[4]) * (M_PI / 180.0),Draw::Atof(a[5]) * (M_PI / 180.0),Draw::Atof(a[6]) * (M_PI / 180.0));
    else
      S = BRepPrimAPI_MakeTorus(P->Pln().Position().Ax2(),Draw::Atof(a[3]),
			    Draw::Atof(a[4]),Draw::Atof(a[5]) * (M_PI / 180.0),Draw::Atof(a[6]) * (M_PI / 180.0));
  }
  else if (n == 8) {
    S = BRepPrimAPI_MakeTorus(P->Pln().Position().Ax2(),Draw::Atof(a[3]),Draw::Atof(a[4]),
			  Draw::Atof(a[5]) * (M_PI / 180.0),Draw::Atof(a[6]) * (M_PI / 180.0),Draw::Atof(a[7]) * (M_PI / 180.0));
  }
  else
    return 1;

  DBRep::Set(a[1],S);
  return 0;
}



//=======================================================================
//function : PrimitiveCommands
//purpose  : 
//=======================================================================

void  BRepTest::PrimitiveCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;

  DBRep::BasicCommands(theCommands);

  const char* g = "Primitive building commands";
    
  theCommands.Add("box","box name [x1 y1 z1] dx dy dz",__FILE__,box,g);
  theCommands.Add("wedge","wedge name [Ox Oy Oz Zx Zy Zz Xx Xy Xz] dx dy dz ltx / xmin zmin xmax zmax",__FILE__,wedge,g);
  
  theCommands.Add("pcylinder","pcylinder name [plane(ax2)] R H [angle]",__FILE__,cylinder,g);
  theCommands.Add("pcone",    "pcone name [plane(ax2)] R1 R2 H [angle]",__FILE__,cone,g);
  theCommands.Add("psphere",  "psphere name [plane(ax2)] R [angle1 angle2] [angle]",__FILE__,sphere,g);
  theCommands.Add("ptorus",   "ptorus name [plane(ax2)] R1 R2 [angle1 angle2] [angle]",__FILE__,torus,g);
}


