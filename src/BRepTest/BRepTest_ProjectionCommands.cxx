// Created on: 1998-03-03
// Created by: Didier PIFFAULT
// Copyright (c) 1998-1999 Matra Datavision
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

#include <GeometryTest.hxx>
#include <DrawTrSurf.hxx>
#include <Draw_Appli.hxx>
#include <TopoDS_Shape.hxx>
#include <DBRep.hxx>
#include <Draw_Interpretor.hxx>
#include <BRepProj_Projection.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepTest.hxx>
#include <TopoDS.hxx>
#include <stdio.h>

static Standard_Integer prj(Draw_Interpretor& di, Standard_Integer n, const char** a)
{ 
  char newname[255];
  if (n < 7) return 1;
  TopoDS_Shape InpLine =  DBRep::Get(a[2]);
  TopoDS_Shape InpShape = DBRep::Get(a[3]);
  Standard_Real DX=Draw::Atof(a[4]),DY=Draw::Atof(a[5]),DZ=Draw::Atof(a[6]);
  gp_Dir TD(DX,DY,DZ);
  BRepProj_Projection Prj(InpLine,InpShape,TD);
  Standard_Integer i = 1;
  char* temp = newname;


  if (Prj.IsDone()) {
    while (Prj.More()) {
      Sprintf(newname,"%s_%d",a[1],i);
      DBRep::Set(temp,Prj.Current());
      //cout<<newname<<" ";
      di<<newname<<" ";
      i++;
      Prj.Next();
    } 
  }

  //cout<<endl;
  di<<"\n";
  return 0;
}

static Standard_Integer cprj(Draw_Interpretor& di, Standard_Integer n, const char** a)
{ 
  char newname[255];
  if (n < 7) return 1;
  TopoDS_Shape InpLine =  DBRep::Get(a[2]);
  TopoDS_Shape InpShape = DBRep::Get(a[3]);
  Standard_Real PX=Draw::Atof(a[4]),PY=Draw::Atof(a[5]),PZ=Draw::Atof(a[6]);
  gp_Pnt P(PX,PY,PZ);
  BRepProj_Projection Prj(InpLine,InpShape,P);
  Standard_Integer i = 1;
  char* temp = newname;

  if (Prj.IsDone()) {
    while (Prj.More()) {
      Sprintf(newname,"%s_%d",a[1],i);
      DBRep::Set(temp,Prj.Current());
      //cout<<newname<<" ";
      di<<newname<<" ";
      i++;
      Prj.Next();
    }
  }

  //cout<<endl;
  di<<"\n";
  return 0;
}


/*********************************************************************************/

void  BRepTest::ProjectionCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean loaded = Standard_False;
  if (loaded) return;
  loaded = Standard_True;

  const char* g = "Projection of wire commands";

  theCommands.Add("prj","prj result w s x y z: cylindrical projection of w (wire or edge) on s (faces) along direction",
		  __FILE__,
		  prj,g);


  theCommands.Add("cprj","cprj result w s x y z: conical projection of w (wire or edge) on s (faces)",
		  __FILE__,
		  cprj,g);

}

