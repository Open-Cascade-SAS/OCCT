// Created on: 1998-03-03
// Created by: Didier PIFFAULT
// Copyright (c) 1998-1999 Matra Datavision
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
//#ifdef WNT
#include <stdio.h>
//#endif
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

