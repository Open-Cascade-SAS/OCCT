// Created on: 1994-07-25
// Created by: Remi LEQUETTE
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <DrawTrSurf.hxx>
#include <GeomTools.hxx>
#include <GeomTools_SurfaceSet.hxx>
#include <GeomTools_CurveSet.hxx>
#include <GeomTools_Curve2dSet.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

// method to call with dbx


void DrawTrSurf_Set(char* name, const Handle(Standard_Transient)& G)
{
  Handle(Geom_Geometry) GG = Handle(Geom_Geometry)::DownCast(G);
  if (!GG.IsNull()) {
    DrawTrSurf::Set(name,GG);
      return;
    }
  Handle(Geom2d_Curve) GC = Handle(Geom2d_Curve)::DownCast(G);
  if (!GC.IsNull()) {
    DrawTrSurf::Set(name,GC);
      return;
    }

  cout << "*** Not a geometric object ***" << endl;
}

void DrawTrSurf_Get(const char* name, Handle(Standard_Transient)& G)
{
  Handle(Geom_Geometry) GG = DrawTrSurf::Get(name);
  cout << "Nom : " << name << endl;
  if (!GG.IsNull()) {
    G = GG;
    return;
  }

  Handle(Geom2d_Curve) GC = DrawTrSurf::GetCurve2d(name);
  if (!GC.IsNull()) {
    G = GC;
    return;
  }

  cout << "*** Not a geometric object ***" << endl;
}

void DrawTrSurf_Dump(const Handle(Standard_Transient)& G)
{
  cout << "\n\n";

  Handle(Geom_Surface) GS = Handle(Geom_Surface)::DownCast(G);
  if (!GS.IsNull()) {
    GeomTools_SurfaceSet::PrintSurface(GS,cout);
    cout << endl;
      return;
    }

  Handle(Geom_Curve) GC = Handle(Geom_Curve)::DownCast(G);
  if (!GC.IsNull()) {
    GeomTools_CurveSet::PrintCurve(GC,cout);
    cout << endl;
      return;
    }

  Handle(Geom2d_Curve) GC2d = Handle(Geom2d_Curve)::DownCast(G);
  if (!GC2d.IsNull()) {
    GeomTools_Curve2dSet::PrintCurve2d(GC2d,cout);
    cout << endl;
      return;
    }
}

void DrawTrSurf_Set(char* name, const gp_Pnt& P)
{
  cout<<"point "<<name<<" "<<P.X()<<" "<<P.Y()<<" "<<P.Z()<<endl;
  DrawTrSurf::Set(name,P);
}

void DrawTrSurf_Set(char* name, const gp_Pnt2d& P)
{
  cout<<"point "<<name<<" "<<P.X()<<" "<<P.Y()<<endl;
  DrawTrSurf::Set(name,P);
}
