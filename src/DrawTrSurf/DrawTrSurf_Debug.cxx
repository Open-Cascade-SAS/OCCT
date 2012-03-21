// Created on: 1994-07-25
// Created by: Remi LEQUETTE
// Copyright (c) 1994-1999 Matra Datavision
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
