// Created on: 1993-08-12
// Created by: Joelle CHAUVET
// Copyright (c) 1993-1999 Matra Datavision
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

// 09/06/97 : JPI : suppression des commandes redondantes suite a la creation de GeomliteTest
// Modified:	Mon Jan 18 11:06:46 1999
//		option -NS dans la commande tuyau pour distinguer les tuyaux
//		a section evolutive des tuyaux a N sections avec N = 2

#include <GeometryTest.hxx>
#include <DrawTrSurf.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Appli.hxx>
#include <Draw_Display.hxx>

#include <GeomAbs_SurfaceType.hxx>
#include <GeomAbs_IsoType.hxx>
#include <GeomAbs_Shape.hxx>

#include <Geom_BSplineSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomConvert.hxx>


#include <GeomAdaptor_Surface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>

#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>
#include <TColGeom_SequenceOfCurve.hxx>

#include <Precision.hxx>

#include <GeomFill.hxx>
#include <GeomFill_BSplineCurves.hxx>
#include <GeomFill_Pipe.hxx>
#include <GeomFill_SectionGenerator.hxx>
#include <GeomFill_Line.hxx>
#include <GeomFill_AppSurf.hxx>


#include <DrawTrSurf_BezierSurface.hxx>
#include <DrawTrSurf_BSplineSurface.hxx>

#ifdef _MSC_VER
#include <stdio.h>
//#define strcasecmp strcmp Already defined
#endif

#ifdef _WIN32
Standard_IMPORT Draw_Viewer dout;
#endif




//=======================================================================
//function : sweep
//purpose  : 
//=======================================================================

static Standard_Integer sweep (Draw_Interpretor& di,
			       Standard_Integer n, const char** a)
{
  GeomFill_Trihedron Option = GeomFill_IsCorrectedFrenet;
  Standard_Integer ipath =2, isection =4 , NbSeg=30, MaxDegree = 10;
  Standard_Real Tol = 1.e-4;

  if (n < 4 ) return 1;
  
  if (! strcmp(a[2],"-FX")) {
    ipath = 3;
    Option = GeomFill_IsFixed;
  }
  if (! strcmp(a[2],"-FR")) {
    ipath = 3;
    Option = GeomFill_IsFrenet;
  }
  if (! strcmp(a[2],"-CF")) {
    ipath = 3;
    Option = GeomFill_IsCorrectedFrenet;
  }
  if (! strcmp(a[2],"-CN")) {
    ipath = 3;
    isection = 7;
    Option = GeomFill_IsConstantNormal;
  }
  if (! strcmp(a[2],"-DX")) {
    ipath = 3;
    isection = 5;
    Option = GeomFill_IsDarboux;
  }

  GeomFill_Pipe Pipe;
  Pipe.GenerateParticularCase(Standard_True);
  
  if (Option == GeomFill_IsDarboux) {
    Handle(Geom2d_Curve) path = DrawTrSurf::GetCurve2d(a[ipath]);
    if ( path.IsNull()) return 1;
    
    Handle(Geom_Surface) Support = DrawTrSurf::GetSurface(a[ipath+1]);
    if ( Support.IsNull()) return 1;

    Handle(Geom_Curve) firstS = DrawTrSurf::GetCurve(a[ipath+2]);
    if ( firstS.IsNull()) return 1;
    
    Pipe.Init(path, Support, firstS);
  }
  else if (Option == GeomFill_IsConstantNormal) {
    gp_Dir D ( Draw::Atof(a[3]), Draw::Atof(a[4]), Draw::Atof(a[5]) );
    Handle(Geom_Curve) path = DrawTrSurf::GetCurve(a[6]);
    Handle(Geom_Curve) firstS = DrawTrSurf::GetCurve(a[7]);
    Pipe.Init(path, firstS, D);
  }
  else  {
    Handle(Geom_Curve) path = DrawTrSurf::GetCurve(a[ipath]);
    if ( path.IsNull()) return 1;
    
    Handle(Geom_Curve) firstS = DrawTrSurf::GetCurve(a[ipath+1]);
    if ( firstS.IsNull()) return 1;

    Pipe.Init(path, firstS, Option);
  }     

  if (n >=isection+2) { 
    MaxDegree = Draw::Atoi(a[isection+1]);
    if (n >isection+2) NbSeg = Draw::Atoi(a[isection+2]);
  }
    
  Pipe.Perform(Tol, Standard_False,  GeomAbs_C2, MaxDegree, NbSeg);

  if (!Pipe.IsDone()) {
    di << "GeomFill_Pipe cannot make a surface\n";
    return 1;
  }

  DrawTrSurf::Set(a[1], Pipe.Surface());
  return 0;
  
}   
//=======================================================================
//function : tuyau
//purpose  : 
//=======================================================================

static Standard_Integer tuyau (Draw_Interpretor& di,
                               Standard_Integer n, const char** a)
{
  if ( n < 4) return 1;

  GeomAbs_Shape Cont = GeomAbs_C2;
  GeomFill_Pipe Pipe;
  Pipe.GenerateParticularCase(Standard_True);

  Standard_Boolean Option_NS = Standard_False;
  Standard_Integer indice_path = 2, narg = n;
  if (! strcmp(a[1],"-NS")) {
    Option_NS = Standard_True;
    indice_path++;
    narg--;
  }
  Handle(Geom_Curve) path = DrawTrSurf::GetCurve(a[indice_path]);
  if ( path.IsNull()) return 1;

  Standard_Integer isect = indice_path+1;
  Handle(Geom_Curve) firstS = DrawTrSurf::GetCurve(a[isect]);
  if ( firstS.IsNull()) {
    if ( narg == 4) { 
      // tuyau a rayon constant.
      Pipe.Init(path, Draw::Atof(a[isect]));
    }
    else 
      return 1;
  }
  else {
    if ( narg == 4) {
      // tuyau a section constante.
      Pipe.Init(path,firstS, GeomFill_IsCorrectedFrenet);
    }
    else {
      if ( narg == 5 && !Option_NS) {
	// tuyau a section evolutive
	Handle(Geom_Curve) lastS = DrawTrSurf::GetCurve(a[isect+1]);
	Cont = GeomAbs_C2;
	Pipe.Init(path, firstS, lastS); 
      }
      else { 
	// tuyau a N sections, N>=2
	TColGeom_SequenceOfCurve Seq;
	Seq.Clear();
	Seq.Append(firstS);
	for (Standard_Integer i=isect+1;i<n;i++) {
	  Handle(Geom_Curve) nextS = DrawTrSurf::GetCurve(a[i]);
	  Seq.Append(nextS);
	}
	Cont = GeomAbs_C2;
	Pipe.Init(path, Seq); 
      }
    }
  }

  Pipe.Perform(1.e-4, Standard_False,  Cont);
  if (!Pipe.IsDone()) {
    di << "GeomFill_Pipe cannot make a surface\n";
    return 1;
  }
  DrawTrSurf::Set(a[indice_path-1], Pipe.Surface());

  return 0;
}

//=======================================================================
//function : ruled
//purpose  : 
//=======================================================================

static Standard_Integer ruled(Draw_Interpretor& di,
			      Standard_Integer n, const char** a)
{
  if ( n < 4) return 1;

  Handle(Geom_Curve) C1 = DrawTrSurf::GetCurve(a[2]);
  if ( C1.IsNull()) {
    di << " C1 is not a Curve ==> Command failed\n";
    return 1;
  }
  Handle(Geom_Curve) C2 = DrawTrSurf::GetCurve(a[3]);
  if ( C2.IsNull()) {
    di << " C2 is not a Curve ==> Command failed\n";
    return 1;
  }
  
  Handle(Geom_Surface) S = GeomFill::Surface(C1,C2);
  if ( S.IsNull()) return 1;

  DrawTrSurf::Set(a[1],S);
  return 0;
}

//=======================================================================
//function : appsurf
//purpose  : 
//=======================================================================

static Standard_Integer appsurf(Draw_Interpretor& di,
				Standard_Integer n, const char** a)
{
  if ( n < 4) return 1;
  
  GeomFill_SectionGenerator Section;
  Standard_Integer i;
  Handle(Geom_Curve) C;
  for ( i = 2; i < n; i++) {
    C = DrawTrSurf::GetCurve(a[i]);
    if (C.IsNull()) 
      return 1;
    Section.AddCurve(C);
  }
  
  Section.Perform(Precision::PConfusion());

  Handle(GeomFill_Line) Line = new GeomFill_Line(n-2);
  Standard_Integer NbIt = 0;
  GeomFill_AppSurf App( 3, 8, Precision::Confusion(), Precision::PConfusion(),
		        NbIt);

  App.Perform( Line, Section);

  if ( !App.IsDone()) {
    di << "    Approximation aux fraises \n";
  }

  Standard_Integer UDegree, VDegree, NbUPoles, NbVPoles, NbUKnots, NbVKnots;
  App.SurfShape(UDegree, VDegree, NbUPoles, NbVPoles, NbUKnots, NbVKnots);

  Handle(Geom_BSplineSurface) GBS = new 
    Geom_BSplineSurface(App.SurfPoles(),
			App.SurfWeights(),
			App.SurfUKnots(),
			App.SurfVKnots(),
			App.SurfUMults(),
			App.SurfVMults(),
			App.UDegree(),
			App.VDegree());

  DrawTrSurf::Set(a[1],GBS);
  return 0;
}

static Standard_Integer fillcurves(Draw_Interpretor& /*di*/,
				   Standard_Integer n, const char** a)
{
  if ( n < 6) return 1;
  
  Standard_Integer i;
  Handle(Geom_Curve) aC;
  Handle(Geom_BSplineCurve) C[4];
  for ( i = 2; i < 6; i++) {
    aC = DrawTrSurf::GetCurve(a[i]);
    if (aC.IsNull()) 
      return 1;
    C[i-2] = GeomConvert::CurveToBSplineCurve(aC, Convert_RationalC1);
  }

  Standard_Integer ist = 2;
  GeomFill_FillingStyle Style = GeomFill_CoonsStyle;
  if(n > 6) ist = Draw::Atoi(a[6]);

  if(ist == 1) Style =  GeomFill_StretchStyle;
  if(ist == 2) Style =  GeomFill_CoonsStyle;
  if(ist == 3) Style =  GeomFill_CurvedStyle;

  GeomFill_BSplineCurves aFilling(C[0], C[1], C[2], C[3], Style);
   
  const Handle(Geom_BSplineSurface)& GBS = aFilling.Surface();
  DrawTrSurf::Set(a[1],GBS);
  return 0;
}


//=======================================================================
//function : GetSurfaceContinuity
//purpose  : Returns the continuity of the given surface
//=======================================================================
static Standard_Integer GetSurfaceContinuity( Draw_Interpretor& theDI,
                                              Standard_Integer theNArg,
                                              const char** theArgv)
{
  if(theNArg != 2)
  {
    theDI << "Use: getsurfcontinuity surface\n";
    return 1;
  }

  Handle(Geom_Surface) GS1 = DrawTrSurf::GetSurface(theArgv[1]);
  if(GS1.IsNull())
  {
    theDI << "Argument is not a surface!\n";
    return 1;
  }

  char aContName[7][3] = {"C0",   //0
                          "G1",   //1
                          "C1",   //2
                          "G2",   //3
                          "C2",   //4
                          "C3",   //5
                          "CN"};  //6

  theDI << theArgv[1] << " has " << aContName[GS1->Continuity()] << " continuity.\n";

  return 0;
}


//=======================================================================
//function : SurfaceCommands
//purpose  : 
//=======================================================================


void  GeometryTest::SurfaceCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean loaded = Standard_False;
  if (loaded) return;
  loaded = Standard_True;

  DrawTrSurf::BasicCommands(theCommands);

  const char* g;

  g = "GEOMETRY surfaces creation";


  theCommands.Add("tuyau",
		  "tuyau [-NS] result Path Curve/Radius [Curve2] [Curve3] ... \n the option -NS is used only with 2 sections.\n With it, <result> is going from the first section to the last section \n Without, <result> is a pipe by evolutive section ",
		  __FILE__,
		  tuyau,g);

  theCommands.Add("partuyau",
		  "tuyau result Path Curve/Radius [Curve2]\n the parametrization of the surface in the V direction will be as the Path",
		  __FILE__,
		  tuyau,g);

  theCommands.Add("sweep", "sweep result [options] path [Surf] curve [Tol [nbsegment]]\n sweep the the curve along the path, options are \n -FX : Tangent and Normal are fixed\n -FR : Tangent and Normal are given by Frenet trihedron \n -CF : Tangente is given by Frenet, \n     the Normal is computed to minimize the torsion \n -DX : Tangent and Normal are given by Darboux trihedron \n     <path> have to be a 2d curve,\n     <Surf> have to be defined\n -CN dx dy dz : Normal is given by dx dy dz" ,
		  __FILE__,
		  sweep, g);

  theCommands.Add("ruled",
		  "ruled result C1 C2",
		  __FILE__,
		  ruled,g);

  theCommands.Add("appsurf",
		  "appsurf result C1 C2 C3 .....: \n\tCreate a surface passing through the curves",
		  __FILE__,
		  appsurf,g);
 
 theCommands.Add("fillcurves",
		  "fillcurves result C1 C2 C3 C4 [style 1/2/3]: \n\tCreate a surface filling frame of 4 curves",
		  __FILE__,
		  fillcurves,g);

 theCommands.Add("getsurfcontinuity",
		  "getsurfcontinuity surface: \n\tReturns the continuity of the given surface",
		  __FILE__,
		  GetSurfaceContinuity,g);


}




