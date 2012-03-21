// Created on: 1998-11-20
// Created by: Robert COUBLANC
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


#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <ViewerTest.hxx>
#include <string.h>
#include <BRepTest.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <DBRep.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Appli.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
//#include <BRepAlgo_BooleanOperation.hxx>
//#include <BRepAlgo_Fuse.hxx>
//#include <BRepAlgo_Cut.hxx>
#include <BiTgte_Blend.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <FilletSurf_Builder.hxx>
#include <ChFi3d_FilletShape.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <TopTools_ListOfShape.hxx>
#include <FilletSurf_StatusType.hxx>
#include <FilletSurf_ErrorTypeStatus.hxx>
#include <TopAbs.hxx>
#include <DrawTrSurf.hxx>
#include <TopTools_HArray1OfShape.hxx>

#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_Shape.hxx>

#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif
#ifdef WNT
//#define strcasecmp strcmp Already defined in Standard
#include <stdio.h>
#endif

extern  Handle(AIS_Shape) GetAISShapeFromName(const char* name);


static Standard_Real t3d = 1.e-4;
static Standard_Real t2d = 1.e-5;
static Standard_Real ta  = 1.e-2;
static Standard_Real fl  = 1.e-3;
static Standard_Real tapp_angle = 1.e-2;
static GeomAbs_Shape blend_cont = GeomAbs_C1;

static BRepFilletAPI_MakeFillet* Rakk = 0;
#ifdef DEB
static BRepFilletAPI_MakeFillet* Rake = 0;
static char name[100];
#endif


static void printtolblend(Draw_Interpretor& di)
{
  di<<"tolerance ang : "<<ta<<"\n";
  di<<"tolerance 3d  : "<<t3d<<"\n";
  di<<"tolerance 2d  : "<<t2d<<"\n";
  di<<"fleche        : "<<fl<<"\n";

  di<<"tolblend "<<ta<<" "<<t3d<<" "<<t2d<<" "<<fl<<"\n";
}

// Unused :
#ifdef DEB
static Standard_Integer tolblend(Draw_Interpretor& di, Standard_Integer narg, const char** a)
{
  if(narg == 1){
    printtolblend(di);
    return 0;
  }
  else if(narg == 5){
    ta = atof(a[1]);
    t3d = atof(a[2]);
    t2d = atof(a[3]);
    fl = atof(a[4]);
    return 0;
  }
  return 1;
}
#endif

static Standard_Integer VBLEND(Draw_Interpretor& di, Standard_Integer narg, const char** a)
{
  if(Rakk != 0) {delete Rakk; Rakk = 0;}
  printtolblend(di);
  if (narg<5) return 1;
  
  Standard_Integer NbToPick = (narg -4)/2;
  Handle(TopTools_HArray1OfShape) arr = new TopTools_HArray1OfShape(1,NbToPick);
  if(ViewerTest::PickShapes(TopAbs_EDGE,arr)){
    for(Standard_Integer i=1;i<=NbToPick;i++){
      TopoDS_Shape PickSh = arr->Value(i);
      if(!PickSh.IsNull()){
	DBRep::Set(a[(2*i+2)],PickSh);
      }
    }
  }
  
  TopoDS_Shape V = DBRep::Get(a[2]);
  if(V.IsNull()) return 1;
  ChFi3d_FilletShape FSh = ChFi3d_Rational;
  if (narg%2 == 0) {
    if (!strcasecmp(a[narg-1], "Q")) {
      FSh = ChFi3d_QuasiAngular;
    }
    else if (!strcasecmp(a[narg-1], "P")) {
      FSh = ChFi3d_Polynomial;
    }
  }
  Rakk = new BRepFilletAPI_MakeFillet(V,FSh);
  Rakk->SetParams(ta,t3d,t2d,t3d,t2d,fl);
  Rakk->SetContinuity(blend_cont, tapp_angle);
  Standard_Real Rad;
  TopoDS_Edge E;
  Standard_Integer nbedge = 0;
  for (Standard_Integer ii = 1; ii < (narg-1)/2; ii++){
    Rad = atof(a[2*ii + 1]);
    TopoDS_Shape aLocalShape = DBRep::Get(a[(2*ii+2)],TopAbs_EDGE);
    E = TopoDS::Edge(aLocalShape);
//    E = TopoDS::Edge(DBRep::Get(a[(2*ii+2)],TopAbs_EDGE));
    if(!E.IsNull()){
      Rakk->Add(Rad,E);
      nbedge++;
    }
  }
  if(!nbedge) return 1;
  Rakk->Build();
  if(!Rakk->IsDone()) return 1;
  TopoDS_Shape res = Rakk->Shape();
  DBRep::Set(a[1],res);


  // visu resultat...
  Handle(AIS_Shape) AS = GetAISShapeFromName(a[1]);
  Handle(AIS_Shape) Start = GetAISShapeFromName(a[2]);
  Handle(AIS_InteractiveContext) Ctx = ViewerTest::GetAISContext();
  Ctx->Erase(Start,Standard_False);
  if(Ctx->IsDisplayed(AS))
    Ctx->Redisplay(AS);
  else
    Ctx->Display(AS);
  return 0;
}




void  ViewerTest::FilletCommands(Draw_Interpretor& theCommands)
{
  const char* g = "Viewer Fillet construction commands";
  theCommands.Add("vblend",
		  "vblend result object rad1 ed1 rad2 ed2 ... [R/Q/P]",__FILE__,
		  VBLEND,g);
}
