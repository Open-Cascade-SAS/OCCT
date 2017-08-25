// Created on: 1995-06-16
// Created by: Jacques GOUSSARD
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

#include <Draw_Interpretor.hxx>
#include <Draw_Appli.hxx>
#include <DrawTrSurf.hxx>

#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>

#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Cylinder.hxx>

//#include <BRepFeat_LocalOperation.hxx>
#include <BRepFeat_Builder.hxx>
#include <BRepFeat_MakeCylindricalHole.hxx>
#include <BRepFeat_SplitShape.hxx>
#include <BRepFeat_Gluer.hxx>

#include <BRepFeat.hxx>
#include <BRepFeat_MakePrism.hxx>
#include <BRepFeat_MakeRevol.hxx>
#include <BRepFeat_MakePipe.hxx>
#include <BRepFeat_MakeDPrism.hxx>
#include <BRepFeat_MakeLinearForm.hxx>
#include <BRepFeat_MakeRevolutionForm.hxx>

#include <LocOpe_FindEdges.hxx>
#include <LocOpe_FindEdgesInFace.hxx>

#include <BRepOffset_MakeOffset.hxx>
#include <BRepOffset_MakeSimpleOffset.hxx>
#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <DBRep.hxx>
#include <DBRep_DrawableShape.hxx>
#include <BRepTest.hxx>

#include <BRepFilletAPI_MakeFillet.hxx>
#include <ChFi3d_FilletShape.hxx>

#include <Precision.hxx>

#ifdef _WIN32
//#define strcasecmp _stricmp Already defined
Standard_IMPORT Draw_Viewer dout;
#endif

static BRepFeat_MakeCylindricalHole theHole;
static Standard_Boolean WithControl = Standard_True;

Standard_Boolean DownCastingEnforcing = Standard_False;

static BRepFeat_MakePrism thePrism;
static BRepFeat_MakeDPrism theDPrism;
static BRepFeat_MakeRevol theRevol;
static BRepFeat_MakePipe  thePipe;
static BRepFeat_MakeLinearForm  theLF;
static BRepFeat_MakeRevolutionForm  theRF;

static Standard_Boolean dprdef = Standard_False;
static Standard_Boolean prdef = Standard_False;
static Standard_Boolean rvdef = Standard_False;
static Standard_Boolean pidef = Standard_False;
static Standard_Boolean lfdef = Standard_False;
static Standard_Boolean rfdef = Standard_False;

static Standard_Real t3d = 1.e-4;
static Standard_Real t2d = 1.e-5;
static Standard_Real ta  = 1.e-2;
static Standard_Real fl  = 1.e-3;
static Standard_Real tapp_angle = 1.e-2;
static GeomAbs_Shape blend_cont = GeomAbs_C1;
static BRepFilletAPI_MakeFillet* Rakk = 0;



static void Print(Draw_Interpretor& di,
		  const BRepFeat_Status St)
{
  di << "  Error Status : ";
  switch (St) {
  case BRepFeat_NoError:
    di << "No error";
    break;

  case BRepFeat_InvalidPlacement:
    di << "Invalid placement";
    break;

  case BRepFeat_HoleTooLong:
    di << "Hole too long";
    break;
  }
}

static Standard_Integer Loc(Draw_Interpretor& theCommands,
			    Standard_Integer narg, const char** a)
{
  if (narg<6) return 1;
  TopoDS_Shape S = DBRep::Get(a[2]);
  TopoDS_Shape T = DBRep::Get(a[3]);

  Standard_Boolean Fuse;
  if (!strcasecmp("F",a[4])) {
    Fuse = Standard_True;
  }
  else if (!strcasecmp("C",a[4])) {
    Fuse = Standard_False;
  }
  else {
    return 1;
  }

  TopTools_ListOfShape LF;
  for (Standard_Integer i=0; i<= narg-6; i++) {
    TopoDS_Shape aLocalShape(DBRep::Get(a[i+5],TopAbs_FACE));
    LF.Append(aLocalShape);
//    LF.Append(TopoDS::Face(DBRep::Get(a[i+5],TopAbs_FACE)));
  }

  //BRepFeat_LocalOperation BLoc(S);
  //BLoc.Perform(T,LF,Fuse);
  //BLoc.BuildPartsOfTool();
  TopTools_ListOfShape parts;
  BRepFeat_Builder BLoc;
  BLoc.Init(S,T);
  BLoc.SetOperation(Fuse);
  //BRepFeat_LocalOperation BLoc;
  //BLoc.Init(S,T,Fuse);
  BLoc.Perform();
  BLoc.PartsOfTool(parts);

#if 0
  char newname[1024];
  strcpy(newname,a[1]);
  char* p = newname;
  while (*p != '\0') p++;
  *p = '_';
  p++;
  TopTools_ListIteratorOfListOfShape its(parts);
  dout.Clear();
  i = 0;
  for (; its.More(); its.Next()) {
    i++;
    Sprintf(p,"%d",i);
    DBRep::Set(newname,its.Value());
  }
  if (i >= 2) {
    dout.Flush();
    Standard_Integer qq,ww,ee,button;
    TopoDS_Shell S;
    do {
      TopoDS_Shape aLocalShape(DBRep::Get(".",TopAbs_SHELL));
      S = TopoDS::Shell(aLocalShape);
//      S = TopoDS::Shell(DBRep::Get(".",TopAbs_SHELL));
      Draw::LastPick(qq,ww,ee,button);
      if (!S.IsNull()) {
        
        switch (button) {
        case 1:
          //BLoc.RemovePart(S);
          break;
        case 2:
          BLoc.KeepPart(S);
          break;
        default:
          {}
        }
      }
      else {
        button = 3;
      }

    } while (button != 3);
  }
#endif
  BLoc.PerformResult();
  if (!BLoc.HasErrors()) {
//    dout.Clear();
    DBRep::Set(a[1],BLoc.Shape());
    dout.Flush();
    return 0;
  }
  theCommands << "Local operation not done" ;
  return 1;
}



static Standard_Integer HOLE1(Draw_Interpretor& theCommands,
			      Standard_Integer narg, const char** a)
{
  if (narg<10 || narg == 11) return 1;
  TopoDS_Shape S = DBRep::Get(a[2]);

  gp_Pnt Or(Draw::Atof(a[3]),Draw::Atof(a[4]),Draw::Atof(a[5]));
  gp_Dir Di(Draw::Atof(a[6]),Draw::Atof(a[7]),Draw::Atof(a[8]));

  Standard_Real Radius = Draw::Atof(a[9]);

  theHole.Init(S,gp_Ax1(Or,Di));

  if (narg <= 10) {
    theHole.Perform(Radius);
  }
  else {
    Standard_Real pfrom = Draw::Atof(a[10]);
    Standard_Real pto   = Draw::Atof(a[11]);
    theHole.Perform(Radius,pfrom,pto,WithControl);
  }

  theHole.Build();
  if (!theHole.HasErrors()) {
//    dout.Clear();
    DBRep::Set(a[1],theHole.Shape());
    dout.Flush();
    return 0;
  }
  theCommands << "Echec de MakeCylindricalHole";
  Print(theCommands,theHole.Status());
  return 1;
}

static Standard_Integer HOLE2(Draw_Interpretor& theCommands,
			      Standard_Integer narg, const char** a)
{
  if (narg<10) return 1;
  TopoDS_Shape S = DBRep::Get(a[2]);

  gp_Pnt Or(Draw::Atof(a[3]),Draw::Atof(a[4]),Draw::Atof(a[5]));
  gp_Dir Di(Draw::Atof(a[6]),Draw::Atof(a[7]),Draw::Atof(a[8]));

  Standard_Real Radius = Draw::Atof(a[9]);

  theHole.Init(S,gp_Ax1(Or,Di));
  theHole.PerformThruNext(Radius,WithControl);

  theHole.Build();
  if (!theHole.HasErrors()) {
//    dout.Clear();
    DBRep::Set(a[1],theHole.Shape());
    dout.Flush();
    return 0;
  }
  theCommands << "Echec de MakeCylindricalHole";
  Print(theCommands,theHole.Status());
  return 1;
}

static Standard_Integer HOLE3(Draw_Interpretor& theCommands,
			      Standard_Integer narg, const char** a)
{
  if (narg<10) return 1;
  TopoDS_Shape S = DBRep::Get(a[2]);

  gp_Pnt Or(Draw::Atof(a[3]),Draw::Atof(a[4]),Draw::Atof(a[5]));
  gp_Dir Di(Draw::Atof(a[6]),Draw::Atof(a[7]),Draw::Atof(a[8]));

  Standard_Real Radius = Draw::Atof(a[9]);

  theHole.Init(S,gp_Ax1(Or,Di));
  theHole.PerformUntilEnd(Radius,WithControl);
  theHole.Build();
  if (!theHole.HasErrors()) {
//    dout.Clear();
    DBRep::Set(a[1],theHole.Shape());
    dout.Flush();
    return 0;
  }
  theCommands << "Echec de MakeCylindricalHole";
  Print(theCommands,theHole.Status());
  return 1;
}


static Standard_Integer HOLE4(Draw_Interpretor& theCommands,
			      Standard_Integer narg, const char** a)
{
  if (narg<11) return 1;
  TopoDS_Shape S = DBRep::Get(a[2]);

  gp_Pnt Or(Draw::Atof(a[3]),Draw::Atof(a[4]),Draw::Atof(a[5]));
  gp_Dir Di(Draw::Atof(a[6]),Draw::Atof(a[7]),Draw::Atof(a[8]));

  Standard_Real Radius = Draw::Atof(a[9]);
  Standard_Real Length = Draw::Atof(a[10]);

  theHole.Init(S,gp_Ax1(Or,Di));
  theHole.PerformBlind(Radius,Length,WithControl);
  theHole.Build();
  if (!theHole.HasErrors()) {
//    dout.Clear();
    DBRep::Set(a[1],theHole.Shape());
    dout.Flush();
    return 0;
  }
  theCommands << "Echec de MakeCylindricalHole";
  Print(theCommands,theHole.Status());
  return 1;
}

static Standard_Integer CONTROL(Draw_Interpretor& theCommands,
				Standard_Integer narg, const char** a)
{
  if (narg >= 2) {
    WithControl = strcmp("0", a[1]) != 0;
  }
  if (WithControl) {
    theCommands << "Mode avec controle";
  }
  else {
    theCommands << "Mode sans controle";
  }
  return 0;
}

//=======================================================================
//function : reportOffsetState
//purpose  : Print state of offset operation by error code.
//=======================================================================
static void reportOffsetState(Draw_Interpretor& theCommands,
                              const BRepOffset_Error theErrorCode)
{
  switch(theErrorCode)
  {
  case BRepOffset_NoError:
    {
      theCommands << "OK. Offset performed succesfully.";
      break;
    }
  case BRepOffset_BadNormalsOnGeometry:
    {
      theCommands << "ERROR. Degenerated normal on input data.";
      break;
    }
  case BRepOffset_C0Geometry:
    {
      theCommands << "ERROR. C0 continuity of input data.";
      break;
    }
  case BRepOffset_NullOffset:
    {
      theCommands << "ERROR. Null offset of all faces.";
      break;
    }
  case BRepOffset_NotConnectedShell:
    {
      theCommands << "ERROR. Incorrect set of faces to remove, the remaining shell is not connected.";
      break;
    }
  default:
    {
      theCommands << "ERROR. offsetperform operation not done.";
      break;
    }
  }
}

//=======================================================================
//function : PRW
//purpose  : 
//=======================================================================

static Standard_Integer PRW(Draw_Interpretor& theCommands,
			    Standard_Integer narg, const char** a)
{
  if (narg<9) return 1;
  TopoDS_Shape S = DBRep::Get(a[3]);
  BRepFeat_MakePrism thePFace;
  gp_Vec V;
  TopoDS_Shape FFrom,FUntil;
  Standard_Integer borne;
  Standard_Boolean fuse;
  if (a[1][0] == 'f' || a[1][0] == 'F') {
    fuse = Standard_True;
  }
  else if (a[1][0] == 'c' || a[1][0] == 'C') {
    fuse = Standard_False;
  }
  else {
    return 1;
  }

  if (a[4][0] == '.' || IsAlphabetic(a[4][0])) {
    if (narg < 10) {
      return 1;
    }
    if (a[5][0] == '.' || IsAlphabetic(a[5][0])) {
      if (narg < 11) {
	return 1;
      }
      V.SetCoord(Draw::Atof(a[6]),Draw::Atof(a[7]),Draw::Atof(a[8]));
      FFrom   = DBRep::Get(a[4],TopAbs_SHAPE);
      FUntil  = DBRep::Get(a[5],TopAbs_SHAPE);
      borne = 9;
    }
    else {
      V.SetCoord(Draw::Atof(a[5]),Draw::Atof(a[6]),Draw::Atof(a[7]));
      FUntil  = DBRep::Get(a[4],TopAbs_SHAPE);
      borne = 8;
    }
  }
  else {
    V.SetCoord(Draw::Atof(a[4]),Draw::Atof(a[5]),Draw::Atof(a[6]));
    borne = 7;
  }
  Standard_Real Length = V.Magnitude();
  if (Length < Precision::Confusion()) {
    return 1;
  }

  TopoDS_Shape aLocalShape(DBRep::Get(a[borne],TopAbs_FACE));
  TopoDS_Face F  = TopoDS::Face(aLocalShape);
//  TopoDS_Face F  = TopoDS::Face(DBRep::Get(a[borne],TopAbs_FACE));
  BRepFeat_SplitShape Spls(F);
  for (Standard_Integer i = borne+1; i<narg; i++) {
    TopoDS_Wire wir;
    if (a[i][0] !='-') {
      aLocalShape = DBRep::Get(a[i],TopAbs_WIRE);
      wir = TopoDS::Wire(aLocalShape);
//      wir = TopoDS::Wire(DBRep::Get(a[i],TopAbs_WIRE));
    }
    else {
      if (a[i][1] == '\0')
	return 1;
      const char* Temp = a[i]+1;
      aLocalShape = DBRep::Get(Temp,TopAbs_WIRE);
      wir = TopoDS::Wire(aLocalShape);
//      wir = TopoDS::Wire(DBRep::Get(Temp,TopAbs_WIRE));
      wir.Reverse();
    }
    Spls.Add(wir,F);
  }
  Spls.Build();

  TopoDS_Shape ToPrism;
  const TopTools_ListOfShape& lleft = Spls.DirectLeft();
  if (lleft.Extent() == 1) {
    thePFace.Init(S,lleft.First(),F,V,fuse,Standard_True);
    ToPrism = lleft.First();
  }
  else {
    BRep_Builder B;
    TopoDS_Shell Sh;
    B.MakeShell(Sh);
    TopTools_ListIteratorOfListOfShape it;
    for (it.Initialize(lleft);it.More();it.Next()) {
      B.Add(Sh,TopoDS::Face(it.Value()));
    }
    Sh.Closed (BRep_Tool::IsClosed (Sh));
    thePFace.Init(S,Sh,F,V,fuse,Standard_True);
    ToPrism = Sh;
  }

  // Recherche des faces de glissement, si on n`a pas sketche sur une face
  // du shape de depart

//  for (TopExp_Explorer exp(S,TopAbs_FACE);exp.More();exp.Next()) {
  TopExp_Explorer exp(S,TopAbs_FACE) ;
  for ( ;exp.More();exp.Next()) {
    if (exp.Current().IsSame(F)) {
      break;
    }
  }

  if (!exp.More()) {
    LocOpe_FindEdgesInFace FEIF;
    for (exp.Init(S,TopAbs_FACE);exp.More();exp.Next()) {
      const TopoDS_Face& fac = TopoDS::Face(exp.Current());
      Handle(Geom_Surface) Su = BRep_Tool::Surface(fac);
      if (Su->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
	Su = Handle(Geom_RectangularTrimmedSurface)::
	DownCast(Su)->BasisSurface();
      }
      if (Su->DynamicType() == STANDARD_TYPE(Geom_Plane)) {
	gp_Pln pl = Handle(Geom_Plane)::DownCast(Su)->Pln();
	if (pl.Contains(gp_Lin(pl.Location(),V),
			Precision::Confusion(),
			Precision::Angular())) {
	  FEIF.Set(ToPrism,fac);
	  for (FEIF.Init();FEIF.More();FEIF.Next()) {
	    thePFace.Add(FEIF.Edge(),fac);
	  }
	}
      }
      else if (Su->DynamicType() == STANDARD_TYPE(Geom_CylindricalSurface)) {
	gp_Cylinder cy = 
	  Handle(Geom_CylindricalSurface)::DownCast(Su)->Cylinder();
	if (V.IsParallel(cy.Axis().Direction(),Precision::Angular())) {
	  FEIF.Set(ToPrism,fac);
	  for (FEIF.Init();FEIF.More();FEIF.Next()) {
	    thePFace.Add(FEIF.Edge(),fac);
	  }
	}
      }
    }
  }

  if (borne == 7) {
    thePFace.Perform(Length);
  }
  else if (borne == 8) {
    thePFace.Perform(FUntil);
  }
  else if (borne == 9) {
    if (!(FFrom.IsNull() || FUntil.IsNull())) {
      thePFace.Perform(FFrom,FUntil);
    }
    else if (FFrom.IsNull()) {
      if (!FUntil.IsNull()) {
	thePFace.PerformFromEnd(FUntil);
      }
      else {
	thePFace.PerformThruAll();
      }
    }
    else {
      // il faudrait inverser V et appeler PerfomFromEnd...
      //cout << "Not Implemented" << endl;
      theCommands << "Not Implemented\n";
    }
  }
  if (!thePFace.IsDone()) {
    theCommands << "Local operation not done";
    return 1;
  }

  DBRep::Set(a[2],thePFace);
  dout.Flush();
  return 0;
}


//=======================================================================
//function : PRF
//purpose  : 
//=======================================================================

static Standard_Integer PRF(Draw_Interpretor& theCommands,
			    Standard_Integer narg, const char** a)
{
  if (narg<8) return 1;
  TopoDS_Shape S = DBRep::Get(a[3]);
  BRepFeat_MakePrism thePFace;
  Standard_Integer borne;
  gp_Vec V;
  TopoDS_Shape FFrom,FUntil;
  Standard_Boolean fuse;
  if (a[1][0] == 'f' || a[1][0] == 'F') {
    fuse = Standard_True;
  }
  else if (a[1][0] == 'c' || a[1][0] == 'C') {
    fuse = Standard_False;
  }
  else {
    return 1;
  }


  if (a[4][0] == '.' || IsAlphabetic(a[4][0])) {
    if (narg < 9) {
      return 1;
    }
    if (a[5][0] == '.' || IsAlphabetic(a[5][0])) {
      if (narg < 10) {
	return 1;
      }
      borne = 9;
      V.SetCoord(Draw::Atof(a[6]),Draw::Atof(a[7]),Draw::Atof(a[8]));
      FFrom  = DBRep::Get(a[4],TopAbs_SHAPE);
      FUntil  = DBRep::Get(a[5],TopAbs_SHAPE);
    }
    else {
      borne = 8;
      V.SetCoord(Draw::Atof(a[5]),Draw::Atof(a[6]),Draw::Atof(a[7]));
      FUntil  = DBRep::Get(a[4],TopAbs_SHAPE);
    }
  }
  else {
    borne = 7;
    V.SetCoord(Draw::Atof(a[4]),Draw::Atof(a[5]),Draw::Atof(a[6]));
  }
  Standard_Real Length = V.Magnitude();
  if (Length < Precision::Confusion()) {
    return 1;
  }

  TopoDS_Shape ToPrism;
  if (narg == borne+1) {
    TopoDS_Shape aLocalShape(DBRep::Get(a[borne],TopAbs_FACE));
    TopoDS_Face F  = TopoDS::Face(aLocalShape);
//    TopoDS_Face F  = TopoDS::Face(DBRep::Get(a[borne],TopAbs_FACE));
    thePFace.Init(S,F,F,V,fuse,Standard_True);
    ToPrism = F;
  }
  else {
    TopoDS_Shell She;
    BRep_Builder B;
    B.MakeShell(She);
    for (Standard_Integer i=borne; i<narg; i++) {
      TopoDS_Shape aLocalShape(DBRep::Get(a[i],TopAbs_FACE));
      TopoDS_Face F  = TopoDS::Face(aLocalShape);
//      TopoDS_Face F  = TopoDS::Face(DBRep::Get(a[i],TopAbs_FACE));
      if (!F.IsNull()) {
	B.Add(She,F);
      }
    }
    She.Closed (BRep_Tool::IsClosed (She));
    thePFace.Init(S,She,TopoDS_Face(),V,fuse,Standard_False);
    ToPrism = She;
  }
  
  // Recherche des faces de glissement, on ne prisme pas une face
  // du shape de depart

//  for (TopExp_Explorer exp(ToPrism,TopAbs_FACE);exp.More();exp.Next()) {
  TopExp_Explorer exp(ToPrism,TopAbs_FACE) ;
  for ( ;exp.More();exp.Next()) {
//    for (TopExp_Explorer exp2(S,TopAbs_FACE);exp2.More();exp2.Next()) {
    TopExp_Explorer exp2(S,TopAbs_FACE) ;
    for ( ;exp2.More();exp2.Next()) {
      if (exp2.Current().IsSame(exp.Current())) {
	break;
      }
    }
    if (exp2.More()) {
      break;
    }
  }

  if (!exp.More()) {
    LocOpe_FindEdgesInFace FEIF;
    for (exp.Init(S,TopAbs_FACE);exp.More();exp.Next()) {
      const TopoDS_Face& fac = TopoDS::Face(exp.Current());
      Handle(Geom_Surface) Su = BRep_Tool::Surface(fac);
      if (Su->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
	Su = Handle(Geom_RectangularTrimmedSurface)::
        DownCast(Su)->BasisSurface();
      }
      if (Su->DynamicType() == STANDARD_TYPE(Geom_Plane)) {
	gp_Pln pl = Handle(Geom_Plane)::DownCast(Su)->Pln();
	if (pl.Contains(gp_Lin(pl.Location(),V),
			Precision::Confusion(),
			Precision::Angular())) {
	  FEIF.Set(ToPrism,fac);
	  for (FEIF.Init();FEIF.More();FEIF.Next()) {
	    thePFace.Add(FEIF.Edge(),fac);
	  }
	}
      }
      else if (Su->DynamicType() == STANDARD_TYPE(Geom_CylindricalSurface)) {
	gp_Cylinder cy = 
	  Handle(Geom_CylindricalSurface)::DownCast(Su)->Cylinder();
	if (V.IsParallel(cy.Axis().Direction(),Precision::Angular())) {
	  FEIF.Set(ToPrism,fac);
	  for (FEIF.Init();FEIF.More();FEIF.Next()) {
	    thePFace.Add(FEIF.Edge(),fac);
	  }
	}
      }
    }
  }

  if (borne == 7) {
    thePFace.Perform(Length);
  }
  else if (borne == 8) {
    thePFace.Perform(FUntil);
  }
  else if (borne == 9) {
    if (!(FFrom.IsNull() || FUntil.IsNull())) {
      thePFace.Perform(FFrom,FUntil);
    }
    else if (FFrom.IsNull()) {
      if (!FUntil.IsNull()) {
	thePFace.PerformFromEnd(FUntil);
      }
      else {
	thePFace.PerformThruAll();
      }
    }
    else { //FUntil.IsNull()
      // il faudrait inverser V et appeler PerfomFromEnd...
      //cout << "Not Implemented" << endl;
      theCommands << "Not Implemented\n";
    }
  }
  if (!thePFace.IsDone()) {
    theCommands << "Local operation not done";
    return 1;
  }

  DBRep::Set(a[2],thePFace);
  dout.Flush();
  return 0;
}



//=======================================================================
//function : SPLS
//purpose  : 
//=======================================================================

static Standard_Integer SPLS(Draw_Interpretor& ,
			     Standard_Integer narg, const char** a)
{
  Standard_Integer newnarg;

  if (narg < 3)
  {
    cout << "Invalid number of arguments. Should be : splitshape result shape [splitedges] \
            [face wire/edge/compound [wire/edge/compound ...] \
            [face wire/edge/compound [wire/edge/compound...] ...] \
            [@ edgeonshape edgeonwire [edgeonshape edgeonwire...]]" << endl;
    return 1;
  }
  TopoDS_Shape S = DBRep::Get(a[2]);
  if (S.IsNull())
  {
    cout << "Invalid input shape " << a[2]<<endl;
    return 1;
  }
  BRepFeat_SplitShape Spls(S);
  Standard_Boolean pick = Standard_False;
  TopoDS_Shape EF;
  Standard_Real u,v;
  Standard_Integer i = 3;

  for ( newnarg=3; newnarg<narg; newnarg++) {
    if (a[newnarg][0] == '@') {
      break;
    }
  }

  if (newnarg == 3 || 
      (newnarg !=narg && ((narg-newnarg)<=2 || (narg-newnarg)%2 != 1))) {
    return 1;
  }
  Standard_Boolean isSplittingEdges = Standard_False;
  TopTools_SequenceOfShape aSplitEdges;
  if (i < newnarg) {
    pick = (a[i][0] == '.');
   
    TopoDS_Shape aSh = DBRep::Get(a[i]);
    if (aSh.IsNull())
    {
      cout << "Invalid input shape " <<a[i]<<endl;
      return 1;
    }


    if (aSh.ShapeType() == TopAbs_FACE)
      EF = TopoDS::Face(aSh);
    else
    {
      if (aSh.ShapeType() == TopAbs_COMPOUND || aSh.ShapeType() == TopAbs_WIRE || aSh.ShapeType() == TopAbs_EDGE)
      {
        TopExp_Explorer aExpE(aSh, TopAbs_EDGE, TopAbs_FACE);
        for (; aExpE.More(); aExpE.Next())
          aSplitEdges.Append(aExpE.Current());

        isSplittingEdges = !aSplitEdges.IsEmpty();
      }
    }

  }
  i++;
  while (i < newnarg) {
    if (pick) {
      DBRep_DrawableShape::LastPick(EF,u,v);
    }
    if (!isSplittingEdges  && !EF.IsNull() && EF.ShapeType() == TopAbs_FACE) {
      // face wire/edge ...
     
      while (i < newnarg) {
	TopoDS_Shape W;
	Standard_Boolean rever = Standard_False;
	if (a[i][0] == '-') {
	  if (a[i][1] == '\0')
	    return 1;
	  pick = (a[i][1] == '.');
	  const char* Temp = a[i]+1;
	  W = DBRep::Get(Temp,TopAbs_SHAPE,Standard_False);
	  rever = Standard_True;
	}
	else {
	  pick = (a[i][0] == '.');
	  W = DBRep::Get(a[i],TopAbs_SHAPE,Standard_False);
	}
	if (W.IsNull()) {
	  return 1; // on n`a rien recupere
	}
	TopAbs_ShapeEnum wtyp = W.ShapeType();
	if (wtyp != TopAbs_WIRE && wtyp != TopAbs_EDGE && wtyp != TopAbs_COMPOUND && pick) {
	  DBRep_DrawableShape::LastPick(W,u,v);
	  wtyp = W.ShapeType();
	}
	if (wtyp != TopAbs_WIRE && wtyp != TopAbs_EDGE && wtyp != TopAbs_COMPOUND) {
	  EF = DBRep::Get(a[i]);
	  break;
	}
	else {
	  if (rever) {
	    W.Reverse();
	  }
	  if (wtyp == TopAbs_WIRE) {
	    Spls.Add(TopoDS::Wire(W),TopoDS::Face(EF));
	  }
	  else if (wtyp == TopAbs_EDGE) {
	    Spls.Add(TopoDS::Edge(W),TopoDS::Face(EF));
	  }
          else {
	    Spls.Add(TopoDS::Compound(W),TopoDS::Face(EF));
          }
	}
	i++;
      }
    }
    else
    {
      if (isSplittingEdges)
      {
        TopoDS_Shape aSh = DBRep::Get(a[i]);
        if (aSh.IsNull())
        {
          cout << "Invalid input shape " <<a[i]<< endl;
          return 1;
        }
        TopExp_Explorer aExpE(aSh, TopAbs_EDGE, TopAbs_FACE);
        for (; aExpE.More(); aExpE.Next())
          aSplitEdges.Append(aExpE.Current());
      }
      else
      {
         cout << "Invalid input arguments. Should be : splitshape result shape [splitedges] \
            [face wire/edge/compound [wire/edge/compound ...] \
            [face wire/edge/compound [wire/edge/compound...] ...] \
            [@ edgeonshape edgeonwire [edgeonshape edgeonwire...]]"<<endl;
        return 1;
      }
    }
    i++;
  }
  
  if (isSplittingEdges)
    Spls.Add(aSplitEdges);

  // ici, i vaut newnarg
  for (; i < narg; i += 2) {
    TopoDS_Shape Ew,Es;
    TopoDS_Shape aLocalShape(DBRep::Get(a[i],TopAbs_EDGE));
    Es = TopoDS::Edge(aLocalShape);
//    Es = TopoDS::Edge(DBRep::Get(a[i],TopAbs_EDGE));
    if (Es.IsNull()) {
      return 1;
    }
    aLocalShape = DBRep::Get(a[i+1],TopAbs_EDGE);
    Ew = TopoDS::Edge(aLocalShape);
//    Ew = TopoDS::Edge(DBRep::Get(a[i+1],TopAbs_EDGE));
    if (Ew.IsNull()) {
      cout << "Invalid input shape " <<a[i+1]<< endl;
      return 1;
    }
    Spls.Add(TopoDS::Edge(Ew),TopoDS::Edge(Es));
  }


  DBRep::Set(a[1],Spls);
  return 0;
}

//=======================================================================
//function : thickshell
//purpose  : 
//=======================================================================
Standard_Integer thickshell(Draw_Interpretor& theCommands,
  Standard_Integer n, const char** a)
{
  if ( n < 4) return 1;
  TopoDS_Shape  S  = DBRep::Get(a[2]);
  if (S.IsNull()) return 1;

  Standard_Real    Of    = Draw::Atof(a[3]);

  GeomAbs_JoinType JT= GeomAbs_Arc;
  if (n > 4)
  {
    if (!strcmp(a[4],"i"))
      JT = GeomAbs_Intersection;
    if (!strcmp(a[4],"t"))
      JT = GeomAbs_Tangent;
  }

  Standard_Boolean Inter = Standard_False; //Standard_True;
  Standard_Real    Tol = Precision::Confusion();
  if (n > 5)
    Tol = Draw::Atof(a[5]);

  BRepOffset_MakeOffset B;
  B.Initialize(S,Of,Tol,BRepOffset_Skin,Inter,0,JT,Standard_True);

  B.MakeOffsetShape();

  const BRepOffset_Error aRetCode = B.Error();
  reportOffsetState(theCommands, aRetCode);

  DBRep::Set(a[1],B.Shape());
  return 0;
}

//=======================================================================
//function : offsetshape
//purpose  : 
//=======================================================================

Standard_Integer offsetshape(Draw_Interpretor& theCommands,
                             Standard_Integer n, const char** a)
{
  if ( n < 4) return 1;
  TopoDS_Shape  S  = DBRep::Get(a[2]);
  if (S.IsNull()) return 1;

  Standard_Real    Of    = Draw::Atof(a[3]);
  Standard_Boolean Inter = (!strcmp(a[0],"offsetcompshape"));
  GeomAbs_JoinType JT= GeomAbs_Arc;
  if (!strcmp(a[0],"offsetinter"))
  {
    JT    = GeomAbs_Intersection;
    Inter = Standard_True;
  }
  
  BRepOffset_MakeOffset B;
  Standard_Integer      IB  = 4;
  Standard_Real         Tol = Precision::Confusion();
  if (n > 4)
  {
    TopoDS_Shape  SF  = DBRep::Get(a[4],TopAbs_FACE);
    if (SF.IsNull())
    {
      IB  = 5;
      Tol = Draw::Atof(a[4]);
    }
  }
  B.Initialize(S,Of,Tol,BRepOffset_Skin,Inter,0,JT);
  //------------------------------------------
  // recuperation et chargement des bouchons.
  //----------------------------------------
  Standard_Boolean YaBouchon = Standard_False;

  for (Standard_Integer i = IB ; i < n; i++)
  {
    TopoDS_Shape  SF  = DBRep::Get(a[i],TopAbs_FACE);
    if (!SF.IsNull())
    {
      YaBouchon = Standard_True;
      B.AddFace(TopoDS::Face(SF));
    }
  }

  if (!YaBouchon)  B.MakeOffsetShape();
  else             B.MakeThickSolid ();

  const BRepOffset_Error aRetCode = B.Error();
  reportOffsetState(theCommands, aRetCode);

  DBRep::Set(a[1],B.Shape());

  return 0;
}

static BRepOffset_MakeOffset TheOffset;
static Standard_Real         TheRadius;
static Standard_Boolean      theYaBouchon;
static Standard_Real         TheTolerance = Precision::Confusion();
static Standard_Boolean      TheInter     = Standard_False;
static GeomAbs_JoinType      TheJoin      = GeomAbs_Arc;
static Standard_Boolean      RemoveIntEdges = Standard_False;

Standard_Integer offsetparameter(Draw_Interpretor& di,
                                 Standard_Integer n, const char** a)
{
  if ( n == 1 ) { 
    di << " offsetparameter Tol Inter(c/p) JoinType(a/i/t) [RemoveInternalEdges(r/k)]\n";
    di << " Current Values\n";
    di << "   --> Tolerance : " << TheTolerance << "\n";
    di << "   --> TheInter  : ";
    if ( TheInter) {
      di << "Complet" ;
    } else {
      di << "Partial";
    }
    di << "\n   --> TheJoin   : ";
    
    switch (TheJoin) {
    case GeomAbs_Arc:          di << "Arc";          break;
    case GeomAbs_Intersection: di << "Intersection"; break;
    default:
      break ;
    }
    //
    di << "\n   --> Internal Edges : ";
    if (RemoveIntEdges) {
      di << "Remove";
    }
    else {
      di << "Keep";
    }
    di << "\n";
    //
    return 0;
  }

  if ( n < 4 ) return 1;
  //
  TheTolerance = Draw::Atof(a[1]);
  TheInter     = strcmp(a[2],"p") != 0;
  //
  if      ( !strcmp(a[3],"a")) TheJoin = GeomAbs_Arc;
  else if ( !strcmp(a[3],"i")) TheJoin = GeomAbs_Intersection;
  else if ( !strcmp(a[3],"t")) TheJoin = GeomAbs_Tangent;
  //
  RemoveIntEdges = (n >= 5) ? !strcmp(a[4], "r") : Standard_False;
  //
  return 0;
}

//=======================================================================
//function : offsetinit
//purpose  : 
//=======================================================================

Standard_Integer offsetload(Draw_Interpretor& ,
  Standard_Integer n, const char** a)
{
  if ( n < 2) return 1;
  TopoDS_Shape  S  = DBRep::Get(a[1]);
  if (S.IsNull()) return 1;

  Standard_Real    Of    = Draw::Atof(a[2]);
  TheRadius = Of;
//  Standard_Boolean Inter = Standard_True;
  
  TheOffset.Initialize(S,Of,TheTolerance,BRepOffset_Skin,TheInter,0,TheJoin,
                       Standard_False, RemoveIntEdges);
  //------------------------------------------
  // recuperation et chargement des bouchons.
  //----------------------------------------
  for (Standard_Integer i = 3 ; i < n; i++) {
    TopoDS_Shape  SF  = DBRep::Get(a[i],TopAbs_FACE);
    if (!SF.IsNull()) {
      TheOffset.AddFace(TopoDS::Face(SF));
    }
  }
  if (n < 4)  theYaBouchon = Standard_False; //B.MakeOffsetShape();
  else        theYaBouchon = Standard_True;  //B.MakeThickSolid ();

  return 0;
}


//=======================================================================
//function : offsetonface
//purpose  : 
//=======================================================================

Standard_Integer offsetonface(Draw_Interpretor&, Standard_Integer n, const char** a)
{
  if ( n < 3) return 1;

  for (Standard_Integer i = 1 ; i < n; i+=2) {
    TopoDS_Shape  SF  = DBRep::Get(a[i],TopAbs_FACE);
    if (!SF.IsNull()) {
      Standard_Real Of = Draw::Atof(a[i+1]);
      TheOffset.SetOffsetOnFace(TopoDS::Face(SF),Of);
    }
  }
  
  return 0;
}

//=======================================================================
//function : offsetperform
//purpose  : 
//=======================================================================

Standard_Integer offsetperform(Draw_Interpretor& theCommands,
                               Standard_Integer theNArg, const char** a)
  {
  if ( theNArg < 2) return 1;

  if (theYaBouchon)
    TheOffset.MakeThickSolid ();
  else
    TheOffset.MakeOffsetShape();

  if(TheOffset.IsDone())
    {
    DBRep::Set(a[1],TheOffset.Shape());
    }
  else
    {
      const BRepOffset_Error aRetCode = TheOffset.Error();
      reportOffsetState(theCommands, aRetCode);
    }

  return 0;
  }


//=======================================================================
//function : ROW
//purpose  : 
//=======================================================================

static Standard_Integer ROW(Draw_Interpretor& theCommands,
			    Standard_Integer narg, const char** a)
{
  if (narg<13) return 1;
  TopoDS_Shape S = DBRep::Get(a[3]);
  BRepFeat_MakeRevol theRFace;
  gp_Dir D;
  gp_Pnt Or;
  Standard_Real Angle=0;
  TopoDS_Shape FFrom,FUntil;
  Standard_Integer i,borne;
  Standard_Boolean fuse;

  if (a[1][0] == 'f' || a[1][0] == 'F') {
    fuse = Standard_True;
  }
  else if (a[1][0] == 'c' || a[1][0] == 'C') {
    fuse = Standard_False;
  }
  else {
    return 1;
  }

  FFrom   = DBRep::Get(a[4],TopAbs_SHAPE);
  if (FFrom.IsNull()) {
    Angle = Draw::Atof(a[4]);
    Angle *=M_PI/180.;
    i = 5;
  }
  else {
    FUntil = DBRep::Get(a[5],TopAbs_SHAPE);
    if (FUntil.IsNull()) {
      i = 5;
      FUntil = FFrom;
      FFrom.Nullify();
      
    }
    else {
      if (narg < 14) {
	return 1;
      }
      i = 6;
    }
  }
  borne = i+6;

  Or.SetCoord(Draw::Atof(a[i]),Draw::Atof(a[i+1]),Draw::Atof(a[i+2]));
  D.SetCoord(Draw::Atof(a[i+3]),Draw::Atof(a[i+4]),Draw::Atof(a[i+5]));
  gp_Ax1 theAxis(Or,D);

  TopoDS_Shape aLocalShape(DBRep::Get(a[borne],TopAbs_FACE));
  TopoDS_Face F  = TopoDS::Face(aLocalShape);
//  TopoDS_Face F  = TopoDS::Face(DBRep::Get(a[borne],TopAbs_FACE));
  BRepFeat_SplitShape Spls(F);
  for (i = borne+1; i<narg; i++) {
    TopoDS_Wire wir;
    if (a[i][0] !='-') {
      aLocalShape = DBRep::Get(a[i],TopAbs_WIRE);
      wir = TopoDS::Wire(aLocalShape);
//      wir = TopoDS::Wire(DBRep::Get(a[i],TopAbs_WIRE));
    }
    else {
      if (a[i][1] == '\0')
	return 1;
      const char* Temp = a[i]+1;
      aLocalShape = DBRep::Get(Temp,TopAbs_WIRE);
      wir = TopoDS::Wire(aLocalShape);
//      wir = TopoDS::Wire(DBRep::Get(Temp,TopAbs_WIRE));
      wir.Reverse();
    }
    Spls.Add(wir,F);
  }
  Spls.Build();

  TopoDS_Shape ToRotate;
  const TopTools_ListOfShape& lleft = Spls.DirectLeft();
  if (lleft.Extent() == 1) {
    theRFace.Init(S,lleft.First(),F,theAxis,fuse,Standard_True);
    ToRotate = lleft.First();
  }
  else {
    BRep_Builder B;
    TopoDS_Shell Sh;
    B.MakeShell(Sh);
    TopTools_ListIteratorOfListOfShape it;
    for (it.Initialize(lleft);it.More();it.Next()) {
      B.Add(Sh,TopoDS::Face(it.Value()));
    }
    Sh.Closed (BRep_Tool::IsClosed (Sh));
    theRFace.Init(S,Sh,F,theAxis,fuse,Standard_True);
    ToRotate = Sh;
  }

  // Recherche des faces de glissement
//  for (TopExp_Explorer exp(S,TopAbs_FACE);exp.More();exp.Next()) {
  TopExp_Explorer exp(S,TopAbs_FACE) ;
  for ( ;exp.More();exp.Next()) {
    if (exp.Current().IsSame(F)) {
      break;
    }
  }

  if (!exp.More()) {
    LocOpe_FindEdgesInFace FEIF;
    for (exp.Init(S,TopAbs_FACE);exp.More();exp.Next()) {
      const TopoDS_Face& fac = TopoDS::Face(exp.Current());
      Handle(Geom_Surface) Su = BRep_Tool::Surface(fac);
      if (Su->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
	Su = Handle(Geom_RectangularTrimmedSurface)::
	  DownCast(Su)->BasisSurface();
      }
      if (Su->DynamicType() == STANDARD_TYPE(Geom_Plane)) {
	gp_Pln pl = Handle(Geom_Plane)::DownCast(Su)->Pln();
	if (pl.Axis().IsParallel(theAxis,Precision::Angular())) {
	  FEIF.Set(ToRotate,fac);
	  for (FEIF.Init();FEIF.More();FEIF.Next()) {
	    theRFace.Add(FEIF.Edge(),fac);
	  }
	}
      }
      else if (Su->DynamicType() == STANDARD_TYPE(Geom_CylindricalSurface)) {
	gp_Cylinder cy = 
	  Handle(Geom_CylindricalSurface)::DownCast(Su)->Cylinder();
	if (cy.Axis().IsCoaxial(theAxis,
				Precision::Angular(),Precision::Confusion())) {
	  FEIF.Set(ToRotate,fac);
	  for (FEIF.Init();FEIF.More();FEIF.Next()) {
	    theRFace.Add(FEIF.Edge(),fac);
	  }
	}
      }
    }
  }

  if (borne == 11) {
    if (FUntil.IsNull()) {
      theRFace.Perform(Angle);
    }
    else {
      theRFace.Perform(FUntil);
    }
  }
  else { // borne == 12
    theRFace.Perform(FFrom,FUntil);
  }

  if (!theRFace.IsDone()) {
    theCommands << "Local operation not done";
    return 1;
  }

  DBRep::Set(a[2],theRFace);
  dout.Flush();
  return 0;
}


//=======================================================================
//function : ROF
//purpose  : 
//=======================================================================

static Standard_Integer ROF(Draw_Interpretor& theCommands,
			    Standard_Integer narg, const char** a)
{
  if (narg<12) return 1;
  TopoDS_Shape S = DBRep::Get(a[3]);
  BRepFeat_MakeRevol theRFace;
  gp_Dir D;
  gp_Pnt Or;
  Standard_Real Angle=0;
  TopoDS_Shape FFrom,FUntil;
  Standard_Integer i,borne;
  Standard_Boolean fuse;

  if (a[1][0] == 'f' || a[1][0] == 'F') {
    fuse = Standard_True;
  }
  else if (a[1][0] == 'c' || a[1][0] == 'C') {
    fuse = Standard_False;
  }
  else {
    return 1;
  }

  FFrom   = DBRep::Get(a[4],TopAbs_SHAPE);
  if (FFrom.IsNull()) {
    Angle = Draw::Atof(a[4]);
    Angle *=M_PI/180.;
    i = 5;
  }
  else {
    FUntil = DBRep::Get(a[5],TopAbs_SHAPE);
    if (FUntil.IsNull()) {
      i = 5;
      FUntil = FFrom;
      FFrom.Nullify();
      
    }
    else {
      if (narg < 13) {
	return 1;
      }
      i = 6;
    }
  }

  borne = i+6;
  Or.SetCoord(Draw::Atof(a[i]),Draw::Atof(a[i+1]),Draw::Atof(a[i+2]));
  D.SetCoord(Draw::Atof(a[i+3]),Draw::Atof(a[i+4]),Draw::Atof(a[i+5]));
  gp_Ax1 theAxis(Or,D);

  TopoDS_Shape ToRotate;
  if (narg == borne+1) {
    TopoDS_Shape aLocalShape(DBRep::Get(a[borne],TopAbs_FACE));
    TopoDS_Face F  = TopoDS::Face(aLocalShape);
//    TopoDS_Face F  = TopoDS::Face(DBRep::Get(a[borne],TopAbs_FACE));
    theRFace.Init(S,F,F,theAxis,fuse,Standard_True);
    ToRotate = F;
  }
  else {
    TopoDS_Shell She;
    BRep_Builder B;
    B.MakeShell(She);
    
    for (i=borne; i<narg; i++) {
      TopoDS_Shape aLocalShape(DBRep::Get(a[i],TopAbs_FACE));
      TopoDS_Face F  = TopoDS::Face(aLocalShape);
//      TopoDS_Face F  = TopoDS::Face(DBRep::Get(a[i],TopAbs_FACE));
      if (!F.IsNull()) {
	B.Add(She,F);
      }
    }
    She.Closed (BRep_Tool::IsClosed (She));
    theRFace.Init(S,She,TopoDS_Face(),theAxis,fuse,Standard_False);
    ToRotate = She;
  }

//  for (TopExp_Explorer exp(ToRotate,TopAbs_FACE);exp.More();exp.Next()) {
  TopExp_Explorer exp(ToRotate,TopAbs_FACE) ;
  for ( ;exp.More();exp.Next()) {
//    for (TopExp_Explorer exp2(S,TopAbs_FACE);exp2.More();exp2.Next()) {
    TopExp_Explorer exp2(S,TopAbs_FACE) ;
    for ( ;exp2.More();exp2.Next()) {
      if (exp2.Current().IsSame(exp.Current())) {
	break;
      }
    }
    if (exp2.More()) {
      break;
    }
  }

  if (!exp.More()) {
    LocOpe_FindEdgesInFace FEIF;
    for (exp.Init(S,TopAbs_FACE);exp.More();exp.Next()) {
      const TopoDS_Face& fac = TopoDS::Face(exp.Current());
      Handle(Geom_Surface) Su = BRep_Tool::Surface(fac);
      if (Su->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
	Su = Handle(Geom_RectangularTrimmedSurface)::
	  DownCast(Su)->BasisSurface();
      }
      if (Su->DynamicType() == STANDARD_TYPE(Geom_Plane)) {
	gp_Pln pl = Handle(Geom_Plane)::DownCast(Su)->Pln();
	if (pl.Axis().IsParallel(theAxis,Precision::Angular())) {
	  FEIF.Set(ToRotate,fac);
	  for (FEIF.Init();FEIF.More();FEIF.Next()) {
	    theRFace.Add(FEIF.Edge(),fac);
	  }
	}
      }
      else if (Su->DynamicType() == STANDARD_TYPE(Geom_CylindricalSurface)) {
	gp_Cylinder cy = 
	  Handle(Geom_CylindricalSurface)::DownCast(Su)->Cylinder();
	if (cy.Axis().IsCoaxial(theAxis,
				Precision::Angular(),Precision::Confusion())) {
	  FEIF.Set(ToRotate,fac);
	  for (FEIF.Init();FEIF.More();FEIF.Next()) {
	    theRFace.Add(FEIF.Edge(),fac);
	  }
	}
      }
    }
  }

  if (borne == 11) {
    if (FUntil.IsNull()) {
      theRFace.Perform(Angle);
    }
    else {
      theRFace.Perform(FUntil);
    }
  }
  else { // borne == 12
    theRFace.Perform(FFrom,FUntil);
  }

  if (!theRFace.IsDone()) {
    theCommands << "Local operation not done";
    return 1;
  }

  DBRep::Set(a[2],theRFace);
  dout.Flush();
  return 0;
}


//=======================================================================
//function : GLU
//purpose  : Commande glue
//=======================================================================

static Standard_Integer GLU(Draw_Interpretor& ,
			    Standard_Integer narg, const char** a)
{
  if (narg<6 || narg%2 != 0) return 1;
  TopoDS_Shape Sne = DBRep::Get(a[2]);
  TopoDS_Shape Sba = DBRep::Get(a[3]);
  
  Standard_Boolean pick;
  
  BRepFeat_Gluer theGl(Sne,Sba);
  TopoDS_Shape Fne,Fba;
  
  LocOpe_FindEdges fined;

  Standard_Integer i = 4;
  Standard_Boolean first = Standard_True;
  while (i<narg) {
    pick = (a[i][0] == '.');
    Fne = DBRep::Get(a[i]);
    if (Fne.IsNull()) {
      return 1;
    }
    TopAbs_ShapeEnum sht = Fne.ShapeType();
    if (pick && sht != TopAbs_FACE && sht != TopAbs_EDGE) {
      Standard_Real u,v;
      DBRep_DrawableShape::LastPick(Fne,u,v);
      sht = Fne.ShapeType();
    }
    if (first && sht != TopAbs_FACE) {
      return 1;
    }
    first = Standard_False;
    pick = (a[i+1][0] == '.');
    Fba = DBRep::Get(a[i+1]);
    if (Fba.IsNull()) {
      return 1;
    }
    if (pick && Fba.ShapeType() != sht) {
      Standard_Real u,v;
      DBRep_DrawableShape::LastPick(Fba,u,v);
    }
    if (Fba.ShapeType() != sht) {
      return 1;
    }
    if (sht == TopAbs_FACE) {
      const TopoDS_Face& f1 = TopoDS::Face(Fne);
      const TopoDS_Face& f2 = TopoDS::Face(Fba);
      theGl.Bind(f1,f2);
      fined.Set(Fne,Fba);
      for (fined.InitIterator(); fined.More(); fined.Next()) {
	theGl.Bind(fined.EdgeFrom(),fined.EdgeTo());
      }
    }
    else {
      theGl.Bind(TopoDS::Edge(Fne),TopoDS::Edge(Fba));
    }
    i +=2;
  }
  
  DBRep::Set(a[1],theGl);
  dout.Flush();
  return 0;
}

static Standard_Integer DEFIN(Draw_Interpretor& theCommands,
			      Standard_Integer narg, const char** a)
{

  if (strcasecmp(a[0],"FEATPRISM") &&
      strcasecmp(a[0],"FEATDPRISM") &&
      strcasecmp(a[0],"FEATREVOL") &&
      strcasecmp(a[0],"FEATPIPE") &&
      strcasecmp(a[0],"FEATLF")  &&
      strcasecmp(a[0],"FEATRF")) {
    return 1;
  }

  if ((!strcasecmp(a[0],"FEATPRISM") && narg !=9) ||
      (!strcasecmp(a[0],"FEATREVOL") && narg != 12) ||
      (!strcasecmp(a[0],"FEATDPRISM") && narg != 7) ||
      (!strcasecmp(a[0],"FEATPIPE") && narg != 7) ||
      (!strcasecmp(a[0],"FEATLF") && narg != 12) ||
      (!strcasecmp(a[0],"FEATRF") && narg != 14)) {
    theCommands << "invalid number of arguments";
    return 1;
  }

  TopoDS_Shape Sbase = DBRep::Get(a[1]);
  if (Sbase.IsNull()) {
    theCommands << "null basis shape";
    return 1;
  }
  Standard_Integer Ifuse  = Draw::Atoi(a[narg-2]);
  Standard_Integer Imodif = Draw::Atoi(a[narg-1]);
  
  Standard_Integer Fuse = Ifuse;
  Standard_Boolean Modify = (Imodif!=0);

  TopoDS_Shape Pbase;
  TopoDS_Face Skface;
  TopoDS_Wire W;

  Handle(Geom_Plane) P;

  BRepFeat_StatusError se;

  if (strcasecmp(a[0],"FEATLF") && strcasecmp(a[0],"FEATRF")) {
    Pbase = DBRep::Get(a[2]);
    if (Pbase.IsNull()) {
      theCommands << "null shape to transform";
      return 1;
    }
    TopoDS_Shape aLocalShape(DBRep::Get(a[3],TopAbs_FACE));
    Skface = TopoDS::Face(aLocalShape);
//    Skface = TopoDS::Face(DBRep::Get(a[3],TopAbs_FACE));
    if (Skface.IsNull()) {
      theCommands << "null face of Sketch";
      return 1;
    }
  }
  else {
    TopoDS_Shape aLocalShape(DBRep::Get(a[2], TopAbs_WIRE));
    W = TopoDS::Wire(aLocalShape);
//    W = TopoDS::Wire(DBRep::Get(a[2], TopAbs_WIRE));
    if(W.IsNull()) {
      theCommands << "null profile for rib or slot";
      return 1;
    }
    Handle(Geom_Surface) s = DrawTrSurf::GetSurface(a[3]);
    P = Handle(Geom_Plane)::DownCast(s);
    if(P.IsNull()) {
      theCommands << "null plane to transform";
      return 1;
    }
  }
  if (narg == 9 || narg == 12 || narg == 14) {
//    Standard_Real X,Y,Z,X1,Y1,Z1;
    Standard_Real X,Y,Z;
    X = Draw::Atof(a[4]);
    Y = Draw::Atof(a[5]);
    Z = Draw::Atof(a[6]);
    
    if (narg == 9) { // prism
      prdef = Standard_True;      
      thePrism.Init(Sbase,Pbase,Skface,gp_Dir(X,Y,Z),Fuse,Modify);
    }
    else if(narg == 14) {
      rfdef = Standard_True;
      gp_Pnt Or(X, Y, Z);
      X = Draw::Atof(a[7]);
      Y = Draw::Atof(a[8]);
      Z = Draw::Atof(a[9]);
      Standard_Real H1 = Draw::Atof(a[10]);
      Standard_Real H2 = Draw::Atof(a[11]);
      gp_Ax1 ax1(Or, gp_Dir(X, Y, Z));
      theRF.Init(Sbase, W, P, ax1, H1, H2, Fuse, Modify);
      if (!theRF.IsDone()) {
	se = theRF.CurrentStatusError();
	//BRepFeat::Print(se,cout) << endl;
	Standard_SStream aSStream;
	BRepFeat::Print(se,aSStream);
	theCommands << aSStream << "\n";
	return 1;
      }
    }
    else if(narg == 12 && strcasecmp(a[0],"FEATLF")) {
      rvdef = Standard_True;
      gp_Pnt Or(X,Y,Z);
      X = Draw::Atof(a[7]);
      Y = Draw::Atof(a[8]);
      Z = Draw::Atof(a[9]);
      theRevol.Init(Sbase,Pbase,Skface,gp_Ax1(Or,gp_Dir(X,Y,Z)),
		    Fuse,Modify);
    }
    else {
      lfdef = Standard_True;
      gp_Vec Direct(X,Y,Z);
      X = Draw::Atof(a[7]);
      Y = Draw::Atof(a[8]);
      Z = Draw::Atof(a[9]);
      theLF.Init(Sbase, W, P, Direct, gp_Vec(X,Y,Z), Fuse,Modify);
      if (!theLF.IsDone()) {
	se = theLF.CurrentStatusError();
	//BRepFeat::Print(se,cout) << endl;
	Standard_SStream aSStream;
	BRepFeat::Print(se,aSStream);
	theCommands << aSStream << "\n";
	return 1;
      }
    }
  }
  else if (narg == 7) {
    if (!strcasecmp(a[0],"FEATDPRISM")) {
      if (Pbase.ShapeType() != TopAbs_FACE) {
	theCommands << "Invalid DPrism base";
	return 1;
      }
      Standard_Real Angle = Draw::Atof(a[4])*M_PI/360; 
      dprdef = Standard_True;
      theDPrism.Init(Sbase,TopoDS::Face(Pbase),Skface,Angle,Fuse,Modify);
    }
    else { // FEATPIPE
      TopoDS_Shape aLocalShape(DBRep::Get(a[4],TopAbs_WIRE));
      TopoDS_Wire Spine = TopoDS::Wire(aLocalShape);
//      TopoDS_Wire Spine = TopoDS::Wire(DBRep::Get(a[4],TopAbs_WIRE));
      if (Spine.IsNull()) {
	TopoDS_Shape Edspine =DBRep::Get(a[4],TopAbs_EDGE);
	if (Edspine.IsNull()) {
	  theCommands << "null spine";
	  return 1;
	}
	BRep_Builder B;
	B.MakeWire(Spine);
	B.Add(Spine,Edspine);
      }
      pidef = Standard_True;
      thePipe.Init(Sbase,Pbase,Skface,Spine,Fuse,Modify);
    }
  }
  return 0;
}



static Standard_Integer ADD(Draw_Interpretor& ,
			    Standard_Integer narg, const char** a)
{
  Standard_Integer i ;
  if (narg < 4 || narg%2 != 0) {
    return 1;
  }
  if (!strcasecmp("PRISM",a[1])) {
    if (!prdef) {
      return 1;
    }
    for ( i=2; i<narg; i+=2) {
      TopoDS_Shape aLocalShape(DBRep::Get(a[i],TopAbs_EDGE));
      TopoDS_Edge edg = TopoDS::Edge(aLocalShape);
//      TopoDS_Edge edg = TopoDS::Edge(DBRep::Get(a[i],TopAbs_EDGE));
      if (edg.IsNull()) {
	return 1;
      }
      aLocalShape = DBRep::Get(a[i+1],TopAbs_FACE);
      TopoDS_Face fac = TopoDS::Face(aLocalShape);
//      TopoDS_Face fac = TopoDS::Face(DBRep::Get(a[i+1],TopAbs_FACE));
      if (fac.IsNull()) {
	return 1;
      }
      thePrism.Add(edg,fac);
    }
  }
  else if (!strcasecmp("REVOL",a[1])) {
    if (!rvdef) {
      return 1;
    }
    for ( i=2; i<narg; i+=2) {
      TopoDS_Shape aLocalShape(DBRep::Get(a[i],TopAbs_EDGE));
      TopoDS_Edge edg = TopoDS::Edge(aLocalShape);
//      TopoDS_Edge edg = TopoDS::Edge(DBRep::Get(a[i],TopAbs_EDGE));
      if (edg.IsNull()) {
	return 1;
      }
      aLocalShape = DBRep::Get(a[i+1],TopAbs_FACE);
      TopoDS_Face fac = TopoDS::Face(aLocalShape);
//      TopoDS_Face fac = TopoDS::Face(DBRep::Get(a[i+1],TopAbs_FACE));
      if (fac.IsNull()) {
	return 1;
      }
      theRevol.Add(edg,fac);
    }
  }
  else if (!strcasecmp("PIPE",a[1])) {
    if (!pidef) {
      return 1;
    }
    for ( i=2; i<narg; i+=2) {
      TopoDS_Shape aLocalShape(DBRep::Get(a[i],TopAbs_EDGE));
      TopoDS_Edge edg = TopoDS::Edge(aLocalShape);
//      TopoDS_Edge edg = TopoDS::Edge(DBRep::Get(a[i],TopAbs_EDGE));
      if (edg.IsNull()) {
	return 1;
      }
      aLocalShape = DBRep::Get(a[i+1],TopAbs_FACE);
      TopoDS_Face fac = TopoDS::Face(aLocalShape);
//      TopoDS_Face fac = TopoDS::Face(DBRep::Get(a[i+1],TopAbs_FACE));
      if (fac.IsNull()) {
	return 1;
      }
      thePipe.Add(edg,fac);
    }
  }
  else {
    return 1;
  }
  return 0;
}



static Standard_Integer PERF(Draw_Interpretor& theCommands,
			    Standard_Integer narg, const char** a)
{
  if (narg < 3) {
    return 1;
  }
  if (strcasecmp(a[0],"FEATPERFORM") &&
      strcasecmp(a[0],"FEATPERFORMVAL")) {
    return 1;
  }

  Standard_Integer Kas;
  if (!strcasecmp("PRISM",a[1])) {
    Kas = 1;
    if (!prdef) {
      theCommands << "prism not defined";
      return 1;
    }
  }
  else if (!strcasecmp("REVOL",a[1])) {
    Kas = 2;
    if (!rvdef) {
      theCommands << "revol not defined";
      return 1;
    }
  }
  else if (!strcasecmp("PIPE",a[1])) {
    Kas = 3;
    if (!pidef) {
      theCommands << "pipe not defined";
      return 1;
    }
    if (!strcasecmp(a[0],"FEATPERFORMVAL")) {
      theCommands << "invalid command for pipe";
      return 1;
    }
  }
  else if (!strcasecmp("DPRISM",a[1])) {
    Kas = 4;
    if (!dprdef) {
      theCommands << "dprism not defined";
      return 1;
    }
  }
  else if (!strcasecmp("LF",a[1])) {
    Kas = 5;
    if (!lfdef) {
      theCommands << "lf not defined";
      return 1;
    }
    if (!strcasecmp(a[0],"FEATPERFORMVAL")) {
      theCommands << "invalid command for lf";
      return 1;
    }
  }
  else if (!strcasecmp("RF",a[1])) {
    Kas = 6;
    if (!rfdef) {
      theCommands << "rf not defined";
      return 1;
    }
    if (!strcasecmp(a[0],"FEATPERFORMVAL")) {
      theCommands << "invalid command for rf";
      return 1;
    }
  }
  else {
    theCommands << "unknown argument : " << a[1];
    return 1;
  }

  if (!strcasecmp(a[0],"FEATPERFORMVAL")) {
    if (narg !=4 && narg != 5) {
      theCommands << "invalid number of arguments";
      return 1;
    }
    if (narg == 4) {
      Standard_Real Val = Draw::Atof(a[3]);
      if (Kas == 1) {
	thePrism.Perform(Val);
      }
      else if (Kas == 2) {
	Val *=(M_PI/180.);
	theRevol.Perform(Val);
      }
      else if (Kas == 4) {
	theDPrism.Perform(Val);
      }
      else if (Kas == 5) {
	theCommands << "invalid command for lf";
	return 1;
      }
      else if (Kas == 6) {
	theCommands << "invalid command for rf";
	return 1;
      }
    }
    else if(narg == 5) {
      Standard_Real Val = Draw::Atof(a[3]);
      TopoDS_Shape FUntil = DBRep::Get(a[4],TopAbs_SHAPE);
      if (Kas == 1) {
	thePrism.PerformUntilHeight(FUntil, Val);
      }
      else if (Kas == 2) {
	Val *=(M_PI/180.);
	theRevol.PerformUntilAngle(FUntil, Val);
      }
      else if (Kas == 4) {
	theDPrism.PerformUntilHeight(FUntil, Val);
      } 
      else {
	theCommands << "invalid command for ribs or slots";
	return 1;
      }
    }
  }
  else if (!strcasecmp(a[0],"FEATPERFORM")) {
    if (narg == 3) { // Thru all
      switch (Kas) {
      case 1:
	thePrism.PerformThruAll();
	break;
      case 2:
	theRevol.PerformThruAll();
	break;
      case 3:
	thePipe.Perform();
	break;
      case 4:
        theDPrism.PerformThruAll();
        break;
      case 5:
	theLF.Perform();
        break;
      case 6:
	theRF.Perform();
        break;
      default:

	return 1;
      }
    }
    else if (narg == 4) { // Until
      TopoDS_Shape Funtil = DBRep::Get(a[3],TopAbs_SHAPE);
      switch (Kas) {
      case 1:
	{
	  if (Funtil.IsNull()) {
	    thePrism.PerformUntilEnd();
	  }
	  else {
	    thePrism.Perform(Funtil);
	  }
	}
	break;
      case 2:
	{
	  if (!Funtil.IsNull()) {
	    theRevol.Perform(Funtil);
	  }
	  else {
	    return 1;
	  }
	}
	break;
      case 3:
	{
	  if (!Funtil.IsNull()) {
	    thePipe.Perform(Funtil);
	  }
	  else {
	    theCommands << "invalid command for ribs pipe";
	    return 1;
	  }
	}
	break;
      case 4:
        {
	  if (!Funtil.IsNull()) {
	    theDPrism.Perform(Funtil);
	  }
	  else {
	    theDPrism.PerformUntilEnd();
	  }
	}
        break;
      case 5:
        {  
	  theCommands << "invalid command for lf";
	  return 1;
	}
        break;
      case 6:
        {  
	  theCommands << "invalid command for rf";
	  return 1;
	}
        break;
      default:
	return 1;
      }
    }
    else if (narg == 5) {
      TopoDS_Shape Ffrom = DBRep::Get(a[3],TopAbs_SHAPE);
      TopoDS_Shape Funtil = DBRep::Get(a[4],TopAbs_SHAPE);
      if (Funtil.IsNull()) {
	return 1;
      }
      switch (Kas) {
      case 1:
	{
	  if (Ffrom.IsNull()) {
	    thePrism.PerformFromEnd(Funtil);
	  }
	  else {
	    thePrism.Perform(Ffrom,Funtil);
	  }
	}
	break;
      case 2:
	{
	  if (Ffrom.IsNull()) {
	    return 1;
	  }
	  theRevol.Perform(Ffrom,Funtil);
	}
	break;
      case 3:
	{
	  if (Ffrom.IsNull()) {
	    return 1;
	  }
	  thePipe.Perform(Ffrom,Funtil);
	}
	break;
      case 4:
	{
	  if (Ffrom.IsNull()) {
	    theDPrism.PerformFromEnd(Funtil);
	  }
	  else {
            theDPrism.Perform(Ffrom,Funtil);
          }
	}
	break;

      default:
	return 1;
      }
    }
  }

  BRepFeat_StatusError se;
  switch (Kas) {
  case 1:
    if (!thePrism.IsDone()) {
      se = thePrism.CurrentStatusError();
      //BRepFeat::Print(se,cout) << endl;
      Standard_SStream aSStream;
      BRepFeat::Print(se,aSStream);
      theCommands << aSStream << "\n";
      return 1;
    }
    DBRep::Set(a[2],thePrism);
    dout.Flush();
    return 0;
  case 2:
    if (!theRevol.IsDone()) {
      se = theRevol.CurrentStatusError();
      //BRepFeat::Print(se,cout) << endl;
      Standard_SStream aSStream;
      BRepFeat::Print(se,aSStream);
      theCommands << aSStream << "\n";
      return 1;
    }
    DBRep::Set(a[2],theRevol);
    dout.Flush();
    return 0;
  case 3:
    if (!thePipe.IsDone()) {
      se = thePipe.CurrentStatusError();
      //BRepFeat::Print(se,cout) << endl;
      Standard_SStream aSStream;
      BRepFeat::Print(se,aSStream);
      theCommands << aSStream << "\n";
      return 1;
    }
    DBRep::Set(a[2],thePipe);
    dout.Flush();
    return 0;
  case 4:
    if (!theDPrism.IsDone()) {
      se = theDPrism.CurrentStatusError();
      //BRepFeat::Print(se,cout) << endl;
      Standard_SStream aSStream;
      BRepFeat::Print(se,aSStream);
      theCommands << aSStream << "\n";
      return 1;
    }
    DBRep::Set(a[2],theDPrism);
    dout.Flush();
    return 0;
  case 5:
    if (!theLF.IsDone()) {
      se = theLF.CurrentStatusError();
      //BRepFeat::Print(se,cout) << endl;
      Standard_SStream aSStream;
      BRepFeat::Print(se,aSStream);
      theCommands << aSStream << "\n";
      return 1;
    }
    DBRep::Set(a[2],theLF);
    dout.Flush();
    return 0;
  case 6:
    if (!theRF.IsDone()) {
      se = theRF.CurrentStatusError();
      //BRepFeat::Print(se,cout) << endl;
      Standard_SStream aSStream;
      BRepFeat::Print(se,aSStream);
      theCommands << aSStream << "\n";
      return 1;
    }
    DBRep::Set(a[2],theRF);
    dout.Flush();
    return 0;
  default:
    return 1;
  }
}


static Standard_Integer BOSS(Draw_Interpretor& theCommands,
			     Standard_Integer narg, const char** a)
{
  if (strcasecmp(a[0],"ENDEDGES") && strcasecmp(a[0],"FILLET")
      && strcasecmp(a[0],"BOSSAGE")) {
    return 1;
  }

  if ((!strcasecmp(a[0],"ENDEDGES") && narg !=5) ||
      (!strcasecmp(a[0],"FILLET") && narg <5 &&  narg%2 != 1) ||
      (!strcasecmp(a[0],"BOSSAGE") && narg != 6)) {
    theCommands << "invalid number of arguments";
    return 1;
  }

  Standard_Integer Kas=0;
  Standard_Integer dprsig=0;
  if (!strcasecmp("ENDEDGES",a[0])) {
    Kas = 1;
    dprsig = Draw::Atoi(a[4]);
  }
  else if (!strcasecmp("FILLET",a[0])) {
    Kas = 2;
  }
  else if (!strcasecmp("BOSSAGE",a[0])) {
    Kas = 3;
    dprsig = Draw::Atoi(a[5]);
  }
 
  TopoDS_Shape theShapeTop; 
  TopoDS_Shape theShapeBottom;

  if (Kas == 1 || Kas == 3) {
    if (!strcasecmp("DPRISM",a[1])) {
      if (!dprdef) {
	theCommands << "dprism not defined";
	return 1;
      }
    }
    else {
      theCommands << "unknown argument : " << a[1];
      return 1;
    }

    theDPrism.BossEdges(dprsig);
   
    TopTools_ListOfShape theTopEdges, theLatEdges;
    theTopEdges = theDPrism.TopEdges();
    theLatEdges = theDPrism.LatEdges();

    TopTools_ListIteratorOfListOfShape it;
    BRep_Builder B;

    B.MakeCompound(TopoDS::Compound(theShapeTop));
    it.Initialize(theTopEdges);
    for(; it.More(); it.Next()) {
      TopExp_Explorer exp;
      for (exp.Init(it.Value(),TopAbs_EDGE); exp.More(); exp.Next()) {
	B.Add(theShapeTop,exp.Current());
      }
    }
    DBRep::Set(a[2],theShapeTop);
    dout.Flush();

    B.MakeCompound(TopoDS::Compound(theShapeBottom));
    it.Initialize(theLatEdges);
    for(; it.More(); it.Next()) {
      B.Add(theShapeBottom,it.Value());
    }
    DBRep::Set(a[3],theShapeBottom);
    dout.Flush();
    if (Kas == 1) return 0;
  }

  if (Kas == 2 || Kas == 3) {
    
//    Standard_Integer nrad;
    TopoDS_Shape V;
    if (Kas == 2) {
      V = DBRep::Get(a[2],TopAbs_SHAPE);
    }
    else if (Kas == 3) {
      V = theDPrism;
    }

    if(V.IsNull()) return 1;
    ChFi3d_FilletShape FSh = ChFi3d_Rational;
    Rakk = new BRepFilletAPI_MakeFillet(V,FSh);
    Rakk->SetParams(ta,t3d,t2d,t3d,t2d,fl);
    Rakk->SetContinuity(blend_cont, tapp_angle);
    Standard_Real Rad;
    TopoDS_Shape S;
    TopoDS_Edge E;
    Standard_Integer nbedge = 0;

    if (Kas == 2) {
      for (Standard_Integer ii = 1; ii < (narg-1)/2; ii++){
	Rad = Draw::Atof(a[2*ii + 1]);
	if (Rad == 0.) continue;
	S = DBRep::Get(a[(2*ii+2)],TopAbs_SHAPE);
	TopExp_Explorer exp;
	for (exp.Init(S,TopAbs_EDGE); exp.More(); exp.Next()) {
	  E = TopoDS::Edge(exp.Current());
	  if(!E.IsNull()){
	    Rakk->Add(Rad,E);
	    nbedge++;
	  }
	}
      }
    }
    else if (Kas == 3) {
      Rad = Draw::Atof(a[3]);
      if (Rad != 0.) {
	S = theShapeTop;
	TopExp_Explorer exp;
	for (exp.Init(S,TopAbs_EDGE); exp.More(); exp.Next()) {
	  E = TopoDS::Edge(exp.Current());
	  if(!E.IsNull()){
	    Rakk->Add(Rad,E);
	    nbedge++;
	  }
	} 
      }
      Rad = Draw::Atof(a[4]);
      if (Rad != 0.) {
	S = theShapeBottom;
	TopExp_Explorer exp;
	for (exp.Init(S,TopAbs_EDGE); exp.More(); exp.Next()) {
	  E = TopoDS::Edge(exp.Current());
	  if(!E.IsNull()){
	    Rakk->Add(Rad,E);
	    nbedge++;
	  }
	} 
      }
    }
    
    if(!nbedge) return 1;
    Rakk->Build();
    if(!Rakk->IsDone()) return 1;
    TopoDS_Shape res = Rakk->Shape();

    if (Kas == 2) {
      DBRep::Set(a[1],res);
    } 
    else if (Kas == 3) {
      DBRep::Set(a[2],res);
    }
    dout.Flush();
    return 0;
  }

  return 1;
}

//=============================================================================
//function : ComputeSimpleOffset
//purpose  : Computes simple offset.
//=============================================================================
static Standard_Integer ComputeSimpleOffset(Draw_Interpretor& theCommands,
                                            Standard_Integer narg, 
                                            const char** a)
{
  if (narg < 4)
  {
    theCommands << "offsetshapesimple result shape offsetvalue [solid] [tolerance=1e-7]\n";
    return 1;
  }

  // Input data.
  TopoDS_Shape aShape = DBRep::Get(a[2]);
  if (aShape.IsNull())
  {
    theCommands << "Input shape is null";
    return 0;
  }
  const Standard_Real anOffsetValue = Draw::Atof(a[3]);
  if (Abs(anOffsetValue) < gp::Resolution())
  {
    theCommands << "Null offset value";
    return 0;
  }

  Standard_Boolean makeSolid = (narg > 4 && !strcasecmp(a[4],"solid"));
  int iTolArg = (makeSolid ? 5 : 4);
  Standard_Real aTol = (narg > iTolArg ? Draw::Atof(a[iTolArg]) : Precision::Confusion());

  BRepOffset_MakeSimpleOffset aMaker(aShape, anOffsetValue);
  aMaker.SetTolerance (aTol);
  aMaker.SetBuildSolidFlag(makeSolid);
  aMaker.Perform();

  if (!aMaker.IsDone())
  {
    theCommands << "ERROR:" << aMaker.GetErrorMessage() << "\n";
    return 0;
  }

  DBRep::Set(a[1], aMaker.GetResultShape());

  return 0;
}

//=======================================================================
//function : FeatureCommands
//purpose  : 
//=======================================================================

void BRepTest::FeatureCommands (Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;

  DBRep::BasicCommands(theCommands);

  const char* g = "TOPOLOGY Feature commands";

  theCommands.Add("localope", 
		  " Performs a local top. operation : localope result shape tool F/C (fuse/cut) face [face...]",
		  __FILE__,Loc,g);

  theCommands.Add("hole",
		  " Performs a hole : hole result shape Or.X Or.Y Or.Z Dir.X Dir.Y Dir.Z Radius [Pfrom Pto]",
		  __FILE__,HOLE1,g);

  theCommands.Add("firsthole",
		  " Performs the first hole : firsthole result shape Or.X Or.Y Or.Z Dir.X Dir.Y Dir.Z Radius",
		  __FILE__,HOLE2,g);

  theCommands.Add("holend",
		  " Performs the hole til end : holend result shape Or.X Or.Y Or.Z Dir.X Dir.Y Dir.Z Radius",
		  __FILE__,HOLE3,g);

  theCommands.Add("blindhole", 
		  " Performs the blind hole : blindhole result shape Or.X Or.Y Or.Z Dir.X Dir.Y Dir.Z Radius Length",
		  __FILE__,HOLE4,g);

  theCommands.Add("holecontrol",
		  "Sets/Unsets or display controls on holes : holecontrol [0/1]",
		  __FILE__,CONTROL,g);

  theCommands.Add("wprism",
		  "Prisms wires on a face : wprism f[use]/c[ut] result shape [[FaceFrom] FaceUntil] VecX VecY VecZ  SkecthFace wire1 [wire2 ....]",
		  __FILE__,PRW,g);


  theCommands.Add("fprism", 
		  "Prisms a set of faces of a shape : fprism f[use]/c[ut] result shape [[FaceFrom] FaceUntil] VecX VecY VecZ face1 [face2...]",
		  __FILE__,PRF,g);


  theCommands.Add("wrotate",
		  "Rotates wires on a face : wrotate f[use]/c[ut] result shape Angle/[FFrom] FUntil OX OY OZ DX DY DZ SkecthFace wire1 [wire2 ....]",
		  __FILE__,ROW,g);


  theCommands.Add("frotate", 
		  "Rotates a set of faces of a shape : frotate f[use]/c[ut] result shape Angle/[FaceFrom] FaceUntil OX OY OZ DX DY DZ face1 [face2...]",
		  __FILE__,ROF,g);


  theCommands.Add("splitshape",
    "splitshape result shape [splitedges] [face wire/edge/compound [wire/edge/compound ...][face wire/edge/compound [wire/edge/compound...] ...] [@ edgeonshape edgeonwire [edgeonshape edgeonwire...]]",
    __FILE__, SPLS, g);


  theCommands.Add("thickshell",
		  "thickshell r shape offset [jointype [tol] ]",
		  __FILE__,thickshell,g);
  
  theCommands.Add("offsetshape",
		  "offsetshape r shape offset [tol] [face ...]",
		  __FILE__,offsetshape,g);
  
  theCommands.Add("offsetcompshape",
		  "offsetcompshape r shape offset [face ...]",
		  __FILE__,offsetshape,g);

  theCommands.Add("offsetparameter",
		  "offsetparameter Tol Inter(c/p) JoinType(a/i/t) [RemoveInternalEdges(r/k)]",
		  __FILE__,offsetparameter);

  theCommands.Add("offsetload",
		  "offsetload shape offset bouchon1 bouchon2 ...",
		  __FILE__,offsetload,g);

  theCommands.Add("offsetonface",
		  "offsetonface face1 offset1 face2 offset2 ...",
		  __FILE__,offsetonface,g);

  theCommands.Add("offsetperform",
		  "offsetperform result",
		  __FILE__,offsetperform,g);

  theCommands.Add("glue", 
		  "glue result shapenew shapebase facenew facebase [facenew facebase...] [edgenew edgebase [edgenew edgebase...]]",
		  __FILE__,GLU,g);


  theCommands.Add("featprism", 
		  "Defines the arguments for a prism : featprism shape element skface  Dirx Diry Dirz Fuse(0/1/2) Modify(0/1)",
		  __FILE__,DEFIN);

  theCommands.Add("featrevol", 
		  "Defines the arguments for a revol : featrevol shape element skface  Ox Oy Oz Dx Dy Dz Fuse(0/1/2) Modify(0/1)",
		  __FILE__,DEFIN);

  theCommands.Add("featpipe", 
		  "Defines the arguments for a pipe : featpipe shape element skface  spine Fuse(0/1/2) Modify(0/1)",
		  __FILE__,DEFIN);

  theCommands.Add("featdprism", 
		  "Defines the arguments for a drafted prism : featdprism shape face skface angle Fuse(0/1/2) Modify(0/1)",
		  __FILE__,DEFIN);

  theCommands.Add("featlf", 
		  "Defines the arguments for a linear rib or slot : featlf shape wire plane DirX DirY DirZ DirX DirY DirZ Fuse(0/1/2) Modify(0/1)",
		  __FILE__,DEFIN);

  theCommands.Add("featrf", 
		  "Defines the arguments for a rib or slot of revolution : featrf shape wire plane X Y Z DirX DirY DirZ Size Size Fuse(0/1/2) Modify(0/1)",
		  __FILE__,DEFIN);

  theCommands.Add("addslide", 
		  " Adds sliding elements : addslide prism/revol/pipe edge face [edge face...]",
		  __FILE__,ADD);

  theCommands.Add("featperform", 
		  " Performs the prism revol dprism linform or pipe :featperform prism/revol/pipe/dprism/lf result [[Ffrom] Funtil]",
		  __FILE__,PERF);

  theCommands.Add("featperformval", 
		  " Performs the prism revol dprism or linform with a value :featperformval prism/revol/dprism/lf result value",
		  __FILE__,PERF);

  theCommands.Add("endedges", 
		  " Return top and bottom edges of dprism :endedges dprism shapetop shapebottom First/LastShape (1/2)",
		  __FILE__,BOSS);

  theCommands.Add("fillet", 
		  " Perform fillet on compounds of edges :fillet result object rad1 comp1 rad2 comp2 ...",
		  __FILE__,BOSS);

  theCommands.Add("bossage", 
		  " Perform fillet on top and bottom edges of dprism :bossage dprism result radtop radbottom First/LastShape (1/2)",
		  __FILE__,BOSS);

  theCommands.Add("offsetshapesimple", 
                  "offsetshapesimple result shape offsetvalue [solid] [tolerance=1e-7]",
                  __FILE__, ComputeSimpleOffset);
}
