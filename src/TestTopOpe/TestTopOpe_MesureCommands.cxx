// Created on: 1996-10-21
// Created by: Jean Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
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


#include <Draw_Appli.hxx>
#include <Draw_Interpretor.hxx>
#include <DrawTrSurf_Curve.hxx>
#include <TColStd_Array1OfAsciiString.hxx>
#include <TColStd_Array1OfTransient.hxx>
#include <TestTopOpe.hxx>
#include <TestTopOpeDraw_Array1OfDrawableMesure.hxx>
#include <TestTopOpeDraw_DrawableMesure.hxx>
#include <TestTopOpeTools_HArray1OfMesure.hxx>
#include <TestTopOpeTools_Mesure.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>

#ifdef _WIN32
Standard_IMPORT Draw_Viewer dout;
#endif


//static Standard_Boolean Bool;
static Standard_Integer NbMES = 0;
static TColStd_Array1OfAsciiString *PDel = NULL;
static TColStd_Array1OfAsciiString *PNAME = NULL;
static TestTopOpeTools_Array1OfMesure *PAMES = NULL;

//-----------------------------------------------
static Standard_Integer SetName(const Standard_Character *C, Draw_Interpretor& di)
//-----------------------------------------------
// here the number of measurement is managed
{
  if(PNAME == NULL) { 
    PNAME = new TColStd_Array1OfAsciiString(1, 100);
  }
  if(PDel == NULL) { 
    PDel = new TColStd_Array1OfAsciiString(1, 100);
  }
  if(NbMES >= 100) {di << "ATTENTION : last measurement accepted";}
  for(Standard_Integer i = 1; i <= NbMES; i++) {
    Standard_Character *Ch = (Standard_Character *)(*PNAME).Value(i).ToCString();
    if( *Ch == *C) return i;
  }
  (*PNAME).SetValue(++NbMES, *C);
  (*PDel).SetValue(NbMES, "0");
  return NbMES;
}

static void Mes_help(Draw_Interpretor& di)
{
  di<<"\n";
  di<<"mesure M : create/reset measurement M "<<"\n";
  di<<"mesure reset : reinitialize all measurements"<<"\n";
  di<<"mesure M n t : add data [n t] to M"<<"\n";
  di<<"mesure range M : sort indexes 'min max' of M"<<"\n";
  di<<"mesure minmax M : take 'xmin ymin xmax ymax' from M"<<"\n";
  di<<"mesure xy <i> M : take 'x y' = M(i)"<<"\n";
  di<<"seem M : visualize M"<<"\n";
  di<<"seemx M fx : visualize M with factor in x"<<"\n";
  di<<"seemx M fy : visualize M with factor in y"<<"\n";
  di<<"seemxy M fx fy : visualize M with factor in x,y"<<"\n";
  di<<""<<"\n";
}

// ----------------------------------------------------------------------
Standard_Integer Mes(Draw_Interpretor& di,Standard_Integer na, const char** a)
// ----------------------------------------------------------------------
{
  if (!strcmp(a[0],"mesure")) {
    if (na == 1) {
      Mes_help(di);
      return 0;
    }
    else if (na == 2) {
      if (!strcmp(a[1],"reset")) { PAMES = NULL; return 0;}
      else {
	Standard_Integer CurrentMES2 = SetName(a[1],di);
	TestTopOpeTools_Mesure *MES2;
	MES2 = new TestTopOpeTools_Mesure(a[1]);
	if(PAMES == NULL) { PAMES = new  TestTopOpeTools_Array1OfMesure(1, 100); }
	(*PAMES).SetValue(CurrentMES2, *MES2);
      }
    }
    else if (na == 3) {
      if      (!strcmp(a[1],"range")) {
	const TestTopOpeTools_Mesure& M = (*PAMES).Value(SetName(a[2],di));
	di<<"1 "<<M.NPnts();
      }
      else if (!strcmp(a[1],"minmax")) {
	const TestTopOpeTools_Mesure& M = (*PAMES).Value(SetName(a[2],di));
	Standard_Integer n=M.NPnts();
	Standard_Real xm=0,ym=0,xM=0,yM=0;
	if (n) {
	  xm=1.e100;ym=1.e100;xM=-1.e100;yM=-1.e100;
	  for (Standard_Integer i=1;i<=n;i++) {
	    Standard_Real x,y,z;gp_Pnt p=M.Pnt(i);p.Coord(x,y,z);
	    xm=Min(xm,x);xM=Max(xM,x);
	    ym=Min(ym,y);yM=Max(yM,y);
	  }
	}
	di<<xm<<" "<<ym<<" "<<xM<<" "<<yM;
      }
    }
    else if (na == 4) {
      if (!strcmp(a[1],"xy")) {
	Standard_Integer i = Draw::Atoi(a[2]); 
	const TestTopOpeTools_Mesure& M = (*PAMES).Value(SetName(a[3],di));
	Standard_Real x,y,z;gp_Pnt p=M.Pnt(i);p.Coord(x,y,z);
	di<<x<<" "<<y;
	return 0;
      }
      Standard_Integer test2 = NbMES;
      Handle(Draw_Drawable3D) D = Draw::Get(a[1],Standard_False);
      Standard_Integer CurrentMES4 = SetName(a[1],di);
      if(PAMES == NULL) { PAMES = new  TestTopOpeTools_Array1OfMesure(1, 100); } 
      if((test2 < CurrentMES4) || // if nee measurement or if
	 (D.IsNull() && ((*PDel).Value(CurrentMES4) == "1"))) // after a "dall" 
	{ 
	  TestTopOpeTools_Mesure *MES4 = (new TestTopOpeTools_Mesure(a[1]));
	  (*MES4).Add(Draw::Atoi(a[2]),Draw::Atof(a[3]));
	  if((*PDel).Value(CurrentMES4) == "1") {
	    TestTopOpeTools_Mesure MES3;
	    MES3 = (*PAMES).Value(CurrentMES4);
	    MES3.Clear();
	  }
	  (*PDel).SetValue(CurrentMES4, "0");
	  (*PAMES).SetValue(CurrentMES4, *MES4);
	} else {
	  TestTopOpeTools_Mesure& MES4 = (*PAMES).ChangeValue(CurrentMES4);
	  MES4.Add(Draw::Atoi(a[2]),Draw::Atof(a[3]));
	  (*PAMES).SetValue(CurrentMES4, MES4);
	}
      
    }
  }
  else if (!strcmp(a[0],"seem")) {
    if (na < 2) return 0;
    TestTopOpeTools_Mesure MESS2;
    Handle(Draw_Drawable3D) D;
    Handle(TestTopOpeDraw_DrawableMesure) DMES2;
    Standard_Integer test4, CurrentMESS2, i;
    if(PAMES == NULL) { 
      PAMES = new  TestTopOpeTools_Array1OfMesure(1, 100); 
    }      
    for (i=1;i<na;i++) {
      test4 = NbMES;
      CurrentMESS2 = SetName(a[i],di);
      D = Draw::Get(a[i],Standard_False);
      if(test4 < CurrentMESS2) {// if nee measurement, it is after restore
	if(D.IsNull()) { return 0; }
	DMES2 = Handle(TestTopOpeDraw_DrawableMesure)::DownCast(D);
	TestTopOpeTools_Mesure *MES;
	MES = (new TestTopOpeTools_Mesure(DMES2->Pnts()));
	MES->SetName(a[i]);
	(*PAMES).SetValue(CurrentMESS2, *MES);
      } else {
	MESS2 = (*PAMES).Value(CurrentMESS2);
	if(!D.IsNull()) {
	  DMES2 = Handle(TestTopOpeDraw_DrawableMesure)::DownCast(D);
	} else { // --> M is already in PAMES, and DMES has no name
	  DMES2 = new TestTopOpeDraw_DrawableMesure(MESS2, Draw_blanc,Draw_rose);
	}
      }
      Draw::Set(a[i],DMES2);
      (*PDel).SetValue(CurrentMESS2, "1");
    }
    dout.RepaintAll();
  }
  else if (!strcmp(a[0],"seemx") || !strcmp(a[0],"seemy")) {
    if (na < 3) return 0;
    Standard_Real dx=1.,dy=1.;
    Standard_Boolean isX = Standard_False, isY = Standard_False;
    if      (!strcmp(a[0],"seemx")) { dx = Draw::Atof(a[na-1]); isX = Standard_True; }
    else if (!strcmp(a[0],"seemy")) { dy = Draw::Atof(a[na-1]); isY = Standard_True; }
    TestTopOpeTools_Mesure MES5;
    Handle(Draw_Drawable3D) D;
    Handle(TestTopOpeDraw_DrawableMesure) DMES5;
    Standard_Integer test, i;
    if(PAMES == NULL) { PAMES = new  TestTopOpeTools_Array1OfMesure(1, 100); }      
    for(i = 1; i < na-1; i++) {
      test = NbMES;
      Standard_Integer CurrentMES5 = SetName(a[i],di);
      D = Draw::Get(a[i],Standard_False);
      if(test < CurrentMES5) {// if new measurement, it is after restore
	if(D.IsNull()) { return 0; }
	DMES5 = Handle(TestTopOpeDraw_DrawableMesure)::DownCast(D);
	TestTopOpeTools_Mesure *MES;
	MES = (new TestTopOpeTools_Mesure(DMES5->Pnts()));
	MES->SetName(a[i]);
	(*PAMES).SetValue(CurrentMES5, *MES);
      } else {
	MES5 = (*PAMES).Value(CurrentMES5);
	if(!D.IsNull()) {
	  DMES5 = Handle(TestTopOpeDraw_DrawableMesure)::DownCast(D);
	} else {
	  DMES5 = new TestTopOpeDraw_DrawableMesure(MES5, Draw_blanc,Draw_rose);
	}
      }
      Draw::Set(a[i],DMES5);
      (*PDel).SetValue(CurrentMES5, "1");
      if(isX) DMES5->SetScaleX(dx);
      if(isY) DMES5->SetScaleY(dy);
    }
  }
  else if (!strcmp(a[0],"seemxy")) {
    if (na < 4) return 0;
    TestTopOpeTools_Mesure& M = (*PAMES).ChangeValue(SetName(a[1],di));
    Standard_Real dx=Draw::Atof(a[2]),dy=Draw::Atof(a[3]);
    Handle(Draw_Drawable3D) D = Draw::Get(a[1],Standard_False);
    Handle(TestTopOpeDraw_DrawableMesure) DM;
    DM = Handle(TestTopOpeDraw_DrawableMesure)::DownCast(D);
    if (DM.IsNull()) {
      DM = new TestTopOpeDraw_DrawableMesure(M,Draw_blanc,Draw_rose);
      Draw::Set(a[1],DM);
    }
    DM->SetScaleX(dx);
    DM->SetScaleY(dy);
    dout<<DM;
  }

  return 0;
}

void TestTopOpe::MesureCommands(Draw_Interpretor& theCommands)
     //=======================================================================
{
  const char* g = "Topological Operation Mesure commands";
  theCommands.Add("mesure","mesure M [n v] : create /add a data/ to a mesure",__FILE__,Mes,g);
  theCommands.Add("seem","seem M : visualize mesure M",__FILE__,Mes,g);
  theCommands.Add("seemx","seemx M ScaleX",__FILE__,Mes,g);
  theCommands.Add("seemy","seemy M ScaleY",__FILE__,Mes,g);
  theCommands.Add("seemxy","seemxy M ScaleX ScaleY",__FILE__,Mes,g);
}
