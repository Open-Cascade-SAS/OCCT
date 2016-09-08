// Created on: 1995-04-05
// Created by: Christophe MARION
// Copyright (c) 1995-1999 Matra Datavision
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


#include <DBRep.hxx>
#include <Draw.hxx>
#include <Draw_Appli.hxx>
#include <gp_Ax3.hxx>
#include <HLRAlgo_Projector.hxx>
#include <HLRAppli_ReflectLines.hxx>
#include <HLRBRep_Algo.hxx>
#include <HLRBRep_HLRToShape.hxx>
#include <HLRTest.hxx>
#include <HLRTest_OutLiner.hxx>
#include <HLRTest_Projector.hxx>
#include <HLRTopoBRep_OutLiner.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <BRep_Builder.hxx>

static Handle(HLRBRep_Algo) hider;
#ifdef _WIN32
Standard_IMPORT Draw_Viewer dout;
#endif

#include <BRepTopAdaptor_MapOfShapeTool.hxx>

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
void HLRTest::Set (const Standard_CString Name,
		   const HLRAlgo_Projector& P)
{
  Draw::Set(Name,new HLRTest_Projector(P));
}

//=======================================================================
//function : GetProjector
//purpose  : 
//=======================================================================
Standard_Boolean HLRTest::GetProjector (Standard_CString& Name,
					HLRAlgo_Projector& P)
{
  Handle(HLRTest_Projector) HP = 
    Handle(HLRTest_Projector)::DownCast(Draw::Get(Name));
  if (HP.IsNull()) return Standard_False;
  P = HP->Projector();
  return Standard_True;
  
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void HLRTest::Set (const Standard_CString Name,
		   const TopoDS_Shape& S)
{
  Draw::Set(Name,new HLRTest_OutLiner(S));
}

//=======================================================================
//function : GetOutLiner
//purpose  : 
//=======================================================================
Handle(HLRTopoBRep_OutLiner) HLRTest::GetOutLiner (Standard_CString& Name)
{
  Handle(Draw_Drawable3D) D = Draw::Get(Name);
  Handle(HLRTest_OutLiner) HS = Handle(HLRTest_OutLiner)::DownCast(D);
  if (!HS.IsNull()) return HS->OutLiner();
  Handle(HLRTopoBRep_OutLiner) HO;
  return HO;
}

//=======================================================================
//function : hprj
//purpose  : 
//=======================================================================

static Standard_Integer
hprj (Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 2) return 1;
  //
  gp_Ax2 anAx2 = gp::XOY();
  if (n == 11)
  {
    Standard_Real x = Draw::Atof(a[2]);
    Standard_Real y = Draw::Atof(a[3]);
    Standard_Real z = Draw::Atof(a[4]);

    Standard_Real dx = Draw::Atof(a[5]);
    Standard_Real dy = Draw::Atof(a[6]);
    Standard_Real dz = Draw::Atof(a[7]);

    Standard_Real dx1 = Draw::Atof(a[8]);
    Standard_Real dy1 = Draw::Atof(a[9]);
    Standard_Real dz1 = Draw::Atof(a[10]);

    gp_Pnt anOrigin (x, y, z);
    gp_Dir aNormal  (dx, dy, dz);
    gp_Dir aDX      (dx1, dy1, dz1);
    anAx2 = gp_Ax2(anOrigin, aNormal, aDX);
  }
  
  HLRAlgo_Projector P(anAx2);
  HLRTest::Set(a[1],P);
  return 0;
}

//=======================================================================
//function : hout
//purpose  : 
//=======================================================================

static Standard_Integer
hout (Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 2) return 1;
  const char *name = a[2];
  TopoDS_Shape S = DBRep::Get(name);
  if (S.IsNull()) {
    di << name << " is not a shape.\n";
    return 1;
  }
  HLRTest::Set(a[1],S);
  return 0;
}

//=======================================================================
//function : hfil
//purpose  : 
//=======================================================================

static Standard_Integer
hfil (Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  Standard_Integer nbIso = 0;
  if (n < 3) return 1;
  if (n > 3) nbIso = Draw::Atoi(a[3]);
  const char *name1 = a[1];
  Handle(HLRTopoBRep_OutLiner) HS = HLRTest::GetOutLiner(name1);
  if (HS.IsNull()) {
    di << name1 << " is not an OutLiner.\n";
    return 1;
  }
  const char *name2 = a[2];
  HLRAlgo_Projector P;
  if (!HLRTest::GetProjector(name2,P)) {
    di << name2 << " is not a projector.\n";
    return 1;
  }
  BRepTopAdaptor_MapOfShapeTool MST;
  HS->Fill(P,MST,nbIso);
  return 0;
}

//=======================================================================
//function : sori
//purpose  : 
//=======================================================================

static Standard_Integer
sori (Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 3) return 1;
  const char *name1 = a[1];
  const char *name2 = a[2];
  Handle(HLRTopoBRep_OutLiner) HS = HLRTest::GetOutLiner(name2);
  if (HS.IsNull()) {
    di << name2 << " is not an OutLiner.\n";
    return 1;
  }
  DBRep::Set(name1,HS->OriginalShape());
  return 0;
}

//=======================================================================
//function : sout
//purpose  : 
//=======================================================================

static Standard_Integer
sout (Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 3) return 1;
  const char *name1 = a[1];
  const char *name2 = a[2];
  Handle(HLRTopoBRep_OutLiner) HS = HLRTest::GetOutLiner(name2);
  if (HS.IsNull()) {
    di << name2 << " is not an OutLiner.\n";
    return 1;
  }
  if (HS->OutLinedShape().IsNull()) {
    di << name2 << " has no OutLinedShape.\n";
    return 1;
  }
  DBRep::Set(name1,HS->OutLinedShape());
  return 0;
}

//=======================================================================
//function : hloa
//purpose  : 
//=======================================================================

static Standard_Integer
hloa (Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 2) return 1;
  const char *name1 = a[1];
  Handle(HLRTopoBRep_OutLiner) HS = HLRTest::GetOutLiner(name1);
  if (HS.IsNull()) {
    di << name1 << " is not an OutLiner.\n";
    return 1;
  }
  hider->Load(HS);
  return 0;
}

//=======================================================================
//function : hrem
//purpose  : 
//=======================================================================

static Standard_Integer
hrem (Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n > 1) {
    const char *name = a[1];
    Standard_Integer index;
    Handle(HLRTopoBRep_OutLiner) HS = HLRTest::GetOutLiner(name);
    if (HS.IsNull()) {
      TopoDS_Shape S = DBRep::Get(name);
      if (S.IsNull()) {
	di << name << " is not an OutLiner and not a shape.\n";
	return 1;
      }
      else {
	index = hider->Index(S);
	if (index == 0) {
	  di << name << " not loaded shape.\n";
	  return 1;
	}
      }
    }
    else {
      index = hider->Index(HS->OriginalShape());
      if (index == 0) {
	di << name << " not loaded outliner.\n";
	return 1;
      }
    }
    hider->Remove(index);
    di << name << " removed\n";
  }
  else {
    while (hider->NbShapes() > 0) {
      hider->Remove(1);
    }
    di << " all shapes removed\n";
  }
  return 0;
}

//=======================================================================
//function : sprj
//purpose  : 
//=======================================================================

static Standard_Integer
sprj (Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 2) return 1;
  const char *name = a[1];
  HLRAlgo_Projector P;
  if (!HLRTest::GetProjector(name,P)) {
    di << name << " is not a projector.\n";
    return 1;
  }
  hider->Projector(P);
  return 0;
}

//=======================================================================
//function : upda
//purpose  : 
//=======================================================================

static Standard_Integer
upda (Draw_Interpretor& , Standard_Integer, const char**)
{
  hider->Update();
  return 0;
}

//=======================================================================
//function : hide
//purpose  : 
//=======================================================================

static Standard_Integer
hide (Draw_Interpretor& , Standard_Integer, const char**)
{
  hider->Hide();
  return 0;
}

//=======================================================================
//function : show
//purpose  : 
//=======================================================================

static Standard_Integer
show (Draw_Interpretor& , Standard_Integer, const char**)
{
  hider->ShowAll();
  return 0;
}

//=======================================================================
//function : hdbg
//purpose  : 
//=======================================================================

static Standard_Integer
hdbg (Draw_Interpretor& di, Standard_Integer, const char**)
{
  hider->Debug(!hider->Debug());
  if (hider->Debug())
    di << "debug\n";
  else
    di << "no debug\n";
  return 0;
}

//=======================================================================
//function : hnul
//purpose  : 
//=======================================================================

static Standard_Integer
hnul (Draw_Interpretor& , Standard_Integer, const char**)
{
  hider->OutLinedShapeNullify();
  return 0;
}

//=======================================================================
//function : hres
//purpose  : 
//=======================================================================

static Standard_Integer
hres (Draw_Interpretor& , Standard_Integer n, const char** a)
{
  TopoDS_Shape S,V,V1,VN,VO,VI,H,H1,HN,HO,HI;
  if (n > 1) {
    const char *name = a[1];
    S = DBRep::Get(name);
  }
  HLRBRep_HLRToShape HS(hider);

  if (S.IsNull()) {
    V  = HS.VCompound();
    V1 = HS.Rg1LineVCompound();
    VN = HS.RgNLineVCompound();
    VO = HS.OutLineVCompound();
    VI = HS.IsoLineVCompound();
    H  = HS.HCompound();
    H1 = HS.Rg1LineHCompound();
    HN = HS.RgNLineHCompound();
    HO = HS.OutLineHCompound();
    HI = HS.IsoLineHCompound();
  }
  else {
    V  = HS.VCompound(S);
    V1 = HS.Rg1LineVCompound(S);
    VN = HS.RgNLineVCompound(S);
    VO = HS.OutLineVCompound(S);
    VI = HS.IsoLineVCompound(S);
    H  = HS.HCompound(S);
    H1 = HS.Rg1LineHCompound(S);
    HN = HS.RgNLineHCompound(S);
    HO = HS.OutLineHCompound(S);
    HI = HS.IsoLineHCompound(S);
  }
  if (!V .IsNull()) DBRep::Set("vl",V);
  if (!V1.IsNull()) DBRep::Set("v1l",V1);
  if (!VN.IsNull()) DBRep::Set("vnl",VN);
  if (!VO.IsNull()) DBRep::Set("vol",VO);
  if (!VI.IsNull()) DBRep::Set("vil",VI);
  if (!H .IsNull()) DBRep::Set("hl",H);
  if (!H1.IsNull()) DBRep::Set("h1l",H1);
  if (!HN.IsNull()) DBRep::Set("hnl",HN);
  if (!HO.IsNull()) DBRep::Set("hol",HO);
  if (!HI.IsNull()) DBRep::Set("hil",HI);
  return 0;
}

//=======================================================================
//function : reflectlines
//purpose  : 
//=======================================================================

static Standard_Integer reflectlines(Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 6)
    return 1;

  TopoDS_Shape aShape =  DBRep::Get(a[2]);
  if (aShape.IsNull())
    return 1;

  Standard_Real anAISViewProjX = atof(a[3]);
  Standard_Real anAISViewProjY = atof(a[4]);
  Standard_Real anAISViewProjZ = atof(a[5]);
  
  gp_Pnt anOrigin(0.,0.,0.);
  gp_Dir aNormal(anAISViewProjX, anAISViewProjY, anAISViewProjZ);
  gp_Ax2 theAxes(anOrigin, aNormal);
  gp_Dir aDX = theAxes.XDirection();

  HLRAppli_ReflectLines Reflector(aShape);

  Reflector.SetAxes(aNormal.X(), aNormal.Y(), aNormal.Z(),
                    anOrigin.X(), anOrigin.Y(), anOrigin.Z(),
                    aDX.X(), aDX.Y(), aDX.Z());

  Reflector.Perform();

  TopoDS_Shape Result = Reflector.GetResult();
  DBRep::Set(a[1], Result);

  return 0;
}

//=======================================================================
//function : hlrin3d
//purpose  : 
//=======================================================================

static Standard_Integer hlrin3d(Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 6)
    return 1;

  TopoDS_Shape aShape =  DBRep::Get(a[2]);
  if (aShape.IsNull())
    return 1;

  Standard_Real anAISViewProjX = atof(a[3]);
  Standard_Real anAISViewProjY = atof(a[4]);
  Standard_Real anAISViewProjZ = atof(a[5]);
  
  gp_Pnt anOrigin(0.,0.,0.);
  gp_Dir aNormal(anAISViewProjX, anAISViewProjY, anAISViewProjZ);
  gp_Ax2 theAxes(anOrigin, aNormal);
  gp_Dir aDX = theAxes.XDirection();

  HLRAppli_ReflectLines Reflector(aShape);

  Reflector.SetAxes(aNormal.X(), aNormal.Y(), aNormal.Z(),
                    anOrigin.X(), anOrigin.Y(), anOrigin.Z(),
                    aDX.X(), aDX.Y(), aDX.Z());

  Reflector.Perform();

  TopoDS_Compound Result;
  BRep_Builder BB;
  BB.MakeCompound(Result);
  
  TopoDS_Shape SharpEdges = Reflector.GetCompoundOf3dEdges(HLRBRep_Sharp, Standard_True, Standard_True);
  if (!SharpEdges.IsNull())
    BB.Add(Result, SharpEdges);
  TopoDS_Shape OutLines = Reflector.GetCompoundOf3dEdges(HLRBRep_OutLine, Standard_True, Standard_True);
  if (!OutLines.IsNull())
    BB.Add(Result, OutLines);
  TopoDS_Shape SmoothEdges = Reflector.GetCompoundOf3dEdges(HLRBRep_Rg1Line, Standard_True, Standard_True);
  if (!SmoothEdges.IsNull())
    BB.Add(Result, SmoothEdges);
  
  DBRep::Set(a[1], Result);

  return 0;
}

//=======================================================================
//function : hlrin2d
//purpose  : 
//=======================================================================

static Standard_Integer hlrin2d(Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 9)
    return 1;

  TopoDS_Shape aShape =  DBRep::Get(a[2]);
  if (aShape.IsNull())
    return 1;

  Standard_Real anAISViewProjX = atof(a[3]);
  Standard_Real anAISViewProjY = atof(a[4]);
  Standard_Real anAISViewProjZ = atof(a[5]);

  Standard_Real Eye_X = atof(a[6]);
  Standard_Real Eye_Y = atof(a[7]);
  Standard_Real Eye_Z = atof(a[8]);


  
  gp_Pnt anOrigin(0.,0.,0.);
  gp_Dir aNormal(anAISViewProjX, anAISViewProjY, anAISViewProjZ);
  gp_Dir aDX(Eye_X, Eye_Y, Eye_Z);
  
  HLRAppli_ReflectLines Reflector(aShape);

  Reflector.SetAxes(aNormal.X(), aNormal.Y(), aNormal.Z(),
                    anOrigin.X(), anOrigin.Y(), anOrigin.Z(),
                    aDX.X(), aDX.Y(), aDX.Z());

  Reflector.Perform();

  TopoDS_Compound Result;
  BRep_Builder BB;
  BB.MakeCompound(Result);
  
  TopoDS_Shape SharpEdges = Reflector.GetCompoundOf3dEdges(HLRBRep_Sharp, Standard_True, Standard_False);
  if (!SharpEdges.IsNull())
    BB.Add(Result, SharpEdges);
  TopoDS_Shape OutLines = Reflector.GetCompoundOf3dEdges(HLRBRep_OutLine, Standard_True, Standard_False);
  if (!OutLines.IsNull())
    BB.Add(Result, OutLines);
  TopoDS_Shape SmoothEdges = Reflector.GetCompoundOf3dEdges(HLRBRep_Rg1Line, Standard_True, Standard_False);
  if (!SmoothEdges.IsNull())
    BB.Add(Result, SmoothEdges);
  
  DBRep::Set(a[1], Result);

  return 0;
}

//=======================================================================
//function : Commands
//purpose  : 
//=======================================================================

void HLRTest::Commands (Draw_Interpretor& theCommands)
{
  const char* g = "ADVALGOS HLR Commands";

  theCommands.Add("hprj"     ,"hprj name [view-id = 1]"     ,__FILE__,hprj,g);
  theCommands.Add("houtl"    ,"houtl name shape"            ,__FILE__,hout,g);
  theCommands.Add("hfill"    ,"hfill name proj [nbIso]"     ,__FILE__,hfil,g);
  theCommands.Add("hsin"     ,"hsin name outliner"          ,__FILE__,sori,g);
  theCommands.Add("hsout"    ,"hsout name outliner"         ,__FILE__,sout,g);
  theCommands.Add("hload"    ,"hload outliner"              ,__FILE__,hloa,g);
  theCommands.Add("hremove"  ,"hremove [name]"              ,__FILE__,hrem,g);
  theCommands.Add("hsetprj"  ,"hsetprj [name]"              ,__FILE__,sprj,g);
  theCommands.Add("hupdate"  ,"hupdate"                     ,__FILE__,upda,g);
  theCommands.Add("hhide"    ,"hhide"                       ,__FILE__,hide,g);
  theCommands.Add("hshowall" ,"hshowall"                    ,__FILE__,show,g);
  theCommands.Add("hdebug"   ,"hdebug"                      ,__FILE__,hdbg,g);
  theCommands.Add("hnullify" ,"hnullify"                    ,__FILE__,hnul,g);
  theCommands.Add("hres2d"   ,"hres2d"                      ,__FILE__,hres,g);

  theCommands.Add("reflectlines",
                  "reflectlines res shape proj_X proj_Y proj_Z",
                  __FILE__, reflectlines, g);
  
  theCommands.Add("hlrin3d",
                  "hlrin3d res shape proj_X proj_Y proj_Z",
                  __FILE__, hlrin3d, g);
  
  theCommands.Add("hlrin2d",
                  "hlrin2d res shape proj_X proj_Y proj_Z eye_x eye_y eye_z",
                  __FILE__, hlrin2d, g);
  
  hider = new HLRBRep_Algo();
}

//=======================================================================
//function : save and restore projector
//purpose  : 
//=======================================================================

static Standard_Boolean stest(const Handle(Draw_Drawable3D)& d) 
{
  return d->IsInstance(STANDARD_TYPE(HLRTest_Projector));
}

//=======================================================================
//function : ssave
//purpose  : 
//=======================================================================

static void ssave (const Handle(Draw_Drawable3D)&d, ostream& OS)
{
  Handle(HLRTest_Projector) HP =
    Handle(HLRTest_Projector)::DownCast(d);

  const HLRAlgo_Projector& P = HP->Projector();
  OS << (P.Perspective() ? "1" : "0") << "\n";
  if (P.Perspective())
    OS << P.Focus() << "\n";
  
  gp_Trsf T = P.Transformation();
  gp_XYZ V = T.TranslationPart();
  gp_Mat M = T.VectorialPart();

  OS << M(1,1) << " ";
  OS << M(1,2) << " ";
  OS << M(1,3) << " ";
  OS << V.Coord(1) << " ";
  OS << "\n";
  OS << M(2,1) << " ";
  OS << M(2,2) << " ";
  OS << M(2,3) << " ";
  OS << V.Coord(2) << " ";
  OS << "\n";
  OS << M(3,1) << " ";
  OS << M(3,2) << " ";
  OS << M(3,3) << " ";
  OS << V.Coord(3) << " ";
  OS << "\n";

}

//=======================================================================
//function : srestore
//purpose  : 
//=======================================================================

static Handle(Draw_Drawable3D) srestore (istream& IS)
{
  Standard_Boolean pers;
  IS >> pers;
  Standard_Real focus = 1;
  if (pers) IS >> focus;
  
  gp_Trsf T;
  Standard_Real V1[3],V2[3],V3[3];
  Standard_Real V[3];

  IS >> V1[0] >> V1[1] >> V1[2] >> V[0];
  IS >> V2[0] >> V2[1] >> V2[2] >> V[1];
  IS >> V3[0] >> V3[1] >> V3[2] >> V[2];

  gp_Dir D1(V1[0],V1[1],V1[2]);
  gp_Dir D2(V2[0],V2[1],V2[2]);
  gp_Dir D3(V3[0],V3[1],V3[2]);
  gp_Ax3 axes(gp_Pnt(0,0,0),D3,D1);
  D3.Cross(D1);
  if (D3.Dot(D2) < 0) axes.YReverse();
  T.SetTransformation(axes);

  T.SetTranslationPart(gp_Vec(V[0],V[1],V[2]));

  HLRAlgo_Projector P(T,pers,focus);
  Handle(HLRTest_Projector) HP = new HLRTest_Projector(P);
  return HP;
}

//=======================================================================
//function : ssr
//purpose  : 
//=======================================================================

static Draw_SaveAndRestore ssr("HLRTest_Projector",stest,ssave,srestore);

