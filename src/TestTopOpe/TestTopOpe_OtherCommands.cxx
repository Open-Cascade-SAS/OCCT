// Created on: 1997-01-21
// Created by: Jean Yves LEBEY
// Copyright (c) 1997-1999 Matra Datavision
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


#include <Bnd_Box.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <DBRep.hxx>
#include <Draw.hxx>
#include <Draw_Appli.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Marker3D.hxx>
#include <Draw_Segment3D.hxx>
#include <DrawTrSurf.hxx>
#include <DrawTrSurf_Point.hxx>
#include <gp.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <TestTopOpe.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>

#ifdef _WIN32
Standard_IMPORT Draw_Viewer dout;
#endif

//=======================================================================
// bounds S xmin ymin zmin zmax ymax zmax
//=======================================================================
Standard_Integer BOUNDS(Draw_Interpretor& di, Standard_Integer narg, const char** a)
{
  if (narg != 8) return 1;
  TopoDS_Shape S = DBRep::Get(a[1]);
  Standard_Real xmin,ymin,zmin,xmax,ymax,zmax;
  Bnd_Box b; BRepBndLib::Add(S,b);
  b.Enlarge(Precision::Confusion());
  b.Get(xmin,ymin,zmin,xmax,ymax,zmax);
  Draw::Set(a[2],xmin);
  Draw::Set(a[3],ymin);
  Draw::Set(a[4],zmin);
  Draw::Set(a[5],xmax);
  Draw::Set(a[6],ymax);
  Draw::Set(a[7],zmax);
  di<<"xmin:"<<xmin<<" ";
  di<<"ymin:"<<ymin<<" ";
  di<<"zmin:"<<zmin<<" ";
  di<<"xmax:"<<xmax<<" ";
  di<<"ymax:"<<ymax<<" ";
  di<<"zmax:"<<zmax<<"\n";
  return 0 ;
}

//---------------------------------------------------------------------
static Standard_Boolean PersEyeDir(const Standard_Integer ViewId,
				   gp_Pnt& E,
				   gp_Dir& D)
//---------------------------------------------------------------------
{
  gp_Trsf T;
  gp_Pnt Eye;
  gp_Dir Dirz(0,0,1);
  dout.GetTrsf(ViewId,T);
  T.Invert();
  Dirz.Transform(T);
  Standard_Boolean pers = Standard_False;
  if (!strcmp("PERS",dout.GetType(ViewId))) {
    pers = Standard_True;
    Eye.SetXYZ(dout.Focal(ViewId)*Dirz.XYZ());
  }
  E = Eye;
  D = Dirz;
  return pers;
}

//=======================================================================
// DOTVIEW dx dy dz id  ddx ddy ddz   (ddx,ddy,ddz) = (dx,dy,dz)^(<id> axis)
//=======================================================================
Standard_Integer DOTVIEW(Draw_Interpretor&, Standard_Integer narg, const char** a)
{
  if (narg < 8) return 1;

  Standard_Real dx,dy,dz;
  Draw::Get(a[1],dx); Draw::Get(a[2],dy); Draw::Get(a[3],dz);
  gp_Dir d1(dx,dy,dz);
  Standard_Real idr; Draw::Get(a[4],idr); 
  gp_Pnt p; gp_Dir d2; PersEyeDir((Standard_Integer)idr,p,d2);
  
  gp_Dir d3(d1.Crossed(d2));
  Draw::Set(a[5],d3.X());
  Draw::Set(a[6],d3.Y());
  Draw::Set(a[7],d3.Z());
  
  return 0;
}

Standard_Integer VECTEUR(Draw_Interpretor& di, Standard_Integer , const char** ) {
  di << "Pick positions with button \n";
  Standard_Integer id,X,Y,b;
  gp_Trsf T;
  gp_Pnt P1,P2,PP1,PP2;
  
  //-----------------------------------------------------------
  dout.Select(id,X,Y,b);    dout.GetTrsf(id,T);
  T.Invert();
  Standard_Real z = dout.Zoom(id);
  P1.SetCoord((Standard_Real)X /z,(Standard_Real)Y /z,0.0);
  P1.Transform(T);
  
  dout.Select(id,X,Y,b);  dout.GetTrsf(id,T);
  T.Invert();  z = dout.Zoom(id);
  
  P2.SetCoord((Standard_Real)X /z,(Standard_Real)Y /z,0.0);
  P2.Transform(T);
  Standard_Real xa,ya,za;
  if(Abs(P1.X())>Abs(P2.X())) xa = P1.X(); else xa = P2.X();
  if(Abs(P1.Y())>Abs(P2.Y())) ya = P1.Y(); else ya = P2.Y();
  if(Abs(P1.Z())>Abs(P2.Z())) za = P1.Z(); else za = P2.Z();
  P1.SetCoord(xa,ya,za);
  Handle(Draw_Marker3D) D0 = new Draw_Marker3D
    (gp_Pnt(P1.X(),P1.Y(),P1.Z()),Draw_Square,Draw_blanc,1);
  dout << D0;
  dout.Flush();
  //-----------------------------------------------------------
  dout.Select(id,X,Y,b);  
  dout.GetTrsf(id,T);
  T.Invert();
  z = dout.Zoom(id);
  PP1.SetCoord((Standard_Real)X /z,(Standard_Real)Y /z,0.0);
  PP1.Transform(T);
  dout.Select(id,X,Y,b);
  dout.GetTrsf(id,T);
  T.Invert();
  z = dout.Zoom(id);
  PP2.SetCoord((Standard_Real)X /z,(Standard_Real)Y /z,0.0);
  PP2.Transform(T);
  if(Abs(PP1.X())>Abs(PP2.X())) xa = PP1.X(); else xa = PP2.X();
  if(Abs(PP1.Y())>Abs(PP2.Y())) ya = PP1.Y(); else ya = PP2.Y();
  if(Abs(PP1.Z())>Abs(PP2.Z())) za = PP1.Z(); else za = PP2.Z();
  PP1.SetCoord(xa,ya,za);
  Handle(Draw_Segment3D) d = new Draw_Segment3D(P1, PP1, Draw_blanc);
  dout << d;
  dout.Flush();
  di<<"\n";
  di<<"cdinp P1 "<<P1.X()<<" "<<P1.Y()<<" "<<P1.Z()<<"\n";
  di<<"cdinp P2 "<<PP1.X()<<" "<<PP1.Y()<<" "<<PP1.Z()<<"\n";
  di<<"cdinp PM "<<(PP1.X()+P1.X())/2<<" "<<(PP1.Y()+P1.Y())/2<<" "<<(PP1.Z()+P1.Z())/2<<"\n";
  di<<"\n";
  di<<"ttranslate "<<PP1.X()-P1.X()<<" "<<PP1.Y()-P1.Y()<<" "<<PP1.Z()-P1.Z()<<"\n";
  
  return 0;
}
 
Standard_Integer MKSOLSHE(Draw_Interpretor&, Standard_Integer narg, const char** a) {
  // 0 = [mksol | mkshe], 1 = solide/shell a creer avec 1 = shell ou face
  // 0 = [mksol | mkshe], 1 = solide/shell a creer avec 2...narg-1 subsshapes
  if (narg < 2) return 1;
  
  Standard_Integer i;
  BRep_Builder BB;
  TopoDS_Shape res;

  Standard_Integer i1 = (narg == 2) ? 1 : 2;
  Standard_Integer i2 = (narg > 2) ? narg : 2;

  // take all the FACE args, place them in the shell <she>
  TopoDS_Shell she; BB.MakeShell(she); she.Closed(Standard_False);
  Standard_Boolean yaface = Standard_False;
  for (i = i1; i < i2; i++) {
    const TopoDS_Shape& S = DBRep::Get(a[i]);
    if (S.IsNull()) continue;
    if (S.ShapeType() == TopAbs_FACE) {
      BB.Add(she,S);
      she.Closed (BRep_Tool::IsClosed (she));
      yaface = Standard_True;
    }
  }

  // take all the SHELL args, place them in the solid <sol>
  TopoDS_Solid sol; BB.MakeSolid(sol);
  for (i = i1; i < i2; i++) {
    const TopoDS_Shape& S = DBRep::Get(a[i]);
    if (S.IsNull()) continue;
    if (S.ShapeType() == TopAbs_SHELL) {
      BB.Add(sol,S);
    }
  }

  if      (!strcmp("mksol",a[0])) {
    if (yaface) BB.Add(sol,she);
    res = sol;
  }
  else if (!strcmp("mkshe",a[0])) {
    res = she;
  }
  else {
    return 1;
  }

  DBRep::Set(a[1],res);
  return 0;
}

//=======================================================================
// Grille  xmin ymin zmin zmax ymax zmax nbx nby nbz
//=======================================================================
Standard_Integer GRILLE(Draw_Interpretor& di, Standard_Integer narg, const char** a)
{
  if (narg != 10) return 1;
  Standard_Real xmin,ymin,zmin,xmax,ymax,zmax,dx,dy;
  Standard_Integer nbx,nby,nbz;
  xmin = Draw::Atof(a[1]);
  ymin = Draw::Atof(a[2]);
  zmin = Draw::Atof(a[3]);
  xmax = Draw::Atof(a[4]);
  ymax = Draw::Atof(a[5]);
  zmax = Draw::Atof(a[6]);
  nbx  = Draw::Atoi(a[7]);
  nby  = Draw::Atoi(a[8]);
  nbz  = Draw::Atoi(a[9]);
  if(xmax<xmin || ymax<ymin || zmax<zmin || nbx<0 || nby<0 || nbz<0) 
    return 1;
  if(nbx) dx = (xmax-xmin)/nbx; else dx=0;
  if(nby) dy = (ymax-ymin)/nby; else dy=0;
  di<<"compound CE\n";
  for(Standard_Real x=xmin; x<xmax; x+=dx) { 
    for(Standard_Real y=ymin; y<ymax; y+=dx) { 
      //-- cout<<x<<" "<<y<<" "<<zmin<<"   "<<x<<" "<<y<<" "<<zmax<<"\n";
      di<<"line l "<<x<<" "<<y<<" "<<zmin<<" 0 0 1;mkedge e l 0 ";
      di<<zmax-zmin<<" ;orient e EXTERNAL; add e CE\n";
      if(dy==0) y=ymax;
    } 
    if(dx==0) x=xmax;
  }
  return 0 ;
}

Standard_Integer GETP3D(Draw_Interpretor& di, Standard_Integer /*narg*/, const char** a)
{
  TopoDS_Shape V = DBRep::Get(a[1]);
  if (V.IsNull()) {
    di<<"null shape\n";
    return 1;
  }
  gp_Pnt pV = BRep_Tool::Pnt(TopoDS::Vertex(V));
  di<<pV.X()<<" "<<pV.Y()<<" "<<pV.Z()<<"\n";
  return 0;
}
#ifdef OCCT_DEBUG
#include <Draw_Chronometer.hxx>
Standard_IMPORT Standard_Boolean Draw_Chrono;
Standard_Integer CHROBOOP(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  Standard_Real ns = 0.;
  Standard_Integer nm = 0;
  Standard_Integer nh = 0;
  Standard_Real ct = 0.;

  if ((n == 1) || (*a[1] == '0') || (*a[1] == '1')) {
    if (n == 1)
      Draw_Chrono = !Draw_Chrono;
    else
      Draw_Chrono = (*a[1] == '1');
    
    if (Draw_Chrono) di << "Chronometers activated.\n";
    else di << "Chronometers desactivated.\n";
  }
  else {
    Handle(Draw_Drawable3D) D = Draw::Get(a[1]);
    Handle(Draw_Chronometer) C;
    if (!D.IsNull()) {
      C = Handle(Draw_Chronometer)::DownCast(D);
    }
    if (C.IsNull()) {
      C = new Draw_Chronometer();
    Draw::Set(a[1],C,Standard_False);
    }
    if (n <= 2) {
      C->Timer().Reset();
    }
    else if (n <= 3) {
      if (!strcasecmp(a[2],"reset"))
	C->Timer().Reset();
      else if (!strcasecmp(a[2],"start"))
	C->Timer().Start();
      else if (!strcasecmp(a[2],"stop"))
	C->Timer().Stop();
      else if (!strcasecmp(a[2],"show"))
	C->Timer().Show();
      else {
	C->Timer().Show(ns,nm,nh,ct);
	if      (!strcasecmp(a[2],"-s")) {
	  di<<ns;
	}
	else if (!strcasecmp(a[2],"-m")) {
	  di<<nm;
	}
	else if (!strcasecmp(a[2],"-h")) {
	  di<<nh;
	}
	else if (!strcasecmp(a[2],"-c")) {
	  di<<ct;
	}
      }
    }
  }
  return 0;
}
#endif

//=======================================================================
//function : OtherCommands
//purpose  : 
//=======================================================================
void TestTopOpe::OtherCommands(Draw_Interpretor& theCommands)
{
  const char* g = "TestTopOpe OtherCommands";
  theCommands.Add("grille","grille x0 y0 z0 x1 y1 z1 nbx nby nbz",__FILE__,GRILLE,g);
  theCommands.Add("vecteur","4 Pick",__FILE__,VECTEUR,g);
  theCommands.Add("bounds","bounds S x1 y1 z1 z2 y2 z2 = (box of S)",__FILE__,BOUNDS,g);
  theCommands.Add("dotview","dotview dx dy dz id _dx1 _dx2 _dx3",__FILE__,DOTVIEW,g);
  theCommands.Add("mksol","make a solid [1] with [2] ...",__FILE__,MKSOLSHE,g);
  theCommands.Add("mkshe","make a shell [1] with [2] ...",__FILE__,MKSOLSHE,g);
  theCommands.Add("getp3d","getp3d vertex",__FILE__,GETP3D,g);
#ifdef OCCT_DEBUG
  theCommands.Add("chroboop","",__FILE__,CHROBOOP,g);
#endif
}
