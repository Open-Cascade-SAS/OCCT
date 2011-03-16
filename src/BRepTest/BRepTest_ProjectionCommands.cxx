// File:	BRepTest_ProjectionCommands.cxx
// Created:	Tue Mar  3 15:54:08 1998
// Author:	Didier PIFFAULT
//		<dpf@motox.paris1.matra-dtv.fr>


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
  Standard_Real DX=atof(a[4]),DY=atof(a[5]),DZ=atof(a[6]);
  gp_Dir TD(DX,DY,DZ);
  BRepProj_Projection Prj(InpLine,InpShape,TD);
  Standard_Integer i = 1;
  char* temp = newname;


  if (Prj.IsDone()) {
    while (Prj.More()) {
      sprintf(newname,"%s_%d",a[1],i);
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
  Standard_Real PX=atof(a[4]),PY=atof(a[5]),PZ=atof(a[6]);
  gp_Pnt P(PX,PY,PZ);
  BRepProj_Projection Prj(InpLine,InpShape,P);
  Standard_Integer i = 1;
  char* temp = newname;

  if (Prj.IsDone()) {
    while (Prj.More()) {
      sprintf(newname,"%s_%d",a[1],i);
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

