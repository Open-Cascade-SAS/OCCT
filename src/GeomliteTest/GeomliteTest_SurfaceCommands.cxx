// Created on: 1993-08-12
// Created by: Bruno DUMORTIER
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

#include <GeomliteTest.hxx>
#include <DrawTrSurf.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Appli.hxx>
#include <Draw_Display.hxx>

#include <GeomAbs_SurfaceType.hxx>
#include <GeomAbs_IsoType.hxx>
#include <GeomAbs_Shape.hxx>

#include <Geom_Plane.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Surface.hxx>

#include <Geom_TrimmedCurve.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>

#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_OffsetCurve.hxx>

#include <GeomAdaptor_Surface.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>

#include <TColGeom_Array2OfBezierSurface.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>

#include <ElSLib.hxx>
#include <ElCLib.hxx>
#include <Precision.hxx>
#include <Convert_CompBezierCurvesToBSplineCurve.hxx>
#include <GeomConvert.hxx>
#include <GeomConvert_BSplineCurveToBezierCurve.hxx>
#include <GeomConvert_BSplineSurfaceToBezierSurface.hxx>
#include <GeomConvert_CompBezierSurfacesToBSplineSurface.hxx>
#include <Geom2dConvert.hxx>
#include <Geom2dConvert_BSplineCurveToBezierCurve.hxx>
#include <GeomLProp_SLProps.hxx>


#include <DrawTrSurf_BezierSurface.hxx>
#include <DrawTrSurf_BSplineSurface.hxx>
#include <GeomConvert_ApproxSurface.hxx>
#include <GeomLib_Tool.hxx>
#include <TopoDS_Shape.hxx>
#include <DBRep.hxx>
#include <Geom_Curve.hxx>

#include <stdio.h>
#ifdef _WIN32
Standard_IMPORT Draw_Viewer dout;
#endif




//=======================================================================
//function : compute min max radius of curvature on a surface
//purpose  : 
//=======================================================================
static Standard_Integer surface_radius (Draw_Interpretor& di,
					Standard_Integer n, 
					const char** a)
{
  Standard_Integer report_curvature = 0 ;
  Standard_Real UParameter,VParameter,radius,tolerance = 1.0e-7 ;

  if (n <  4) return 1;
  if (n >= 6) report_curvature = 1 ;

  UParameter = Draw::Atof(a[2]);
  VParameter = Draw::Atof(a[3]);
  Handle(Geom_Surface) SurfacePtr = DrawTrSurf::GetSurface(a[1]);
  if (!SurfacePtr.IsNull()) {
    GeomLProp_SLProps myProperties(SurfacePtr,
				   UParameter,
				   VParameter,
				   2,
				   tolerance);
    if (myProperties.IsCurvatureDefined()) {
      radius = myProperties.MinCurvature();
      
      if (report_curvature) Draw::Set(a[4],radius);
      
      if (Abs(radius) > tolerance) { 
	radius = 1.0e0/ radius ;
	di << "Min Radius of Curvature : " << radius  << "\n";
      }
      else {
	di << "Min Radius of Curvature :  infinite\n";
      }
    
      radius = myProperties.MaxCurvature();
      if (report_curvature) Draw::Set(a[5],radius);
      if (Abs(radius) > tolerance)  { 
	radius = 1.0e0/ radius;
	di << "Max Radius of Curvature : " << radius  << "\n";
      }
      else
	di << "Min Radius of Curvature :  infinite\n";
    }
    else {
      di << "Curvature not defined.\n";
    }
  }
  else {
    return 1;
  }
  return 0;
}


//=======================================================================
//function : anasurface
//purpose  : 
//=======================================================================

static Standard_Integer anasurface (Draw_Interpretor& ,
				    Standard_Integer  n, 
				    const char** a)
{
  if (n < 2) return 1;
  gp_Ax3 loc;

  Standard_Integer i;

  if (n < 5) {
    loc = gp_Ax3(gp_Pnt(0,0,0),gp_Dir(0,0,1),gp_Dir(1,0,0));
    i = 2;
  }
  else if (n < 8) {
    loc = gp_Ax3(gp_Pnt(Draw::Atof(a[2]),Draw::Atof(a[3]),Draw::Atof(a[4])),
		 gp_Dir(0,0,1),gp_Dir(1,0,0));
    i = 5;
  }
  else if (n < 11) {
    loc = gp_Ax3(gp_Pnt(Draw::Atof(a[2]),Draw::Atof(a[3]),Draw::Atof(a[4])),
		 gp_Dir(Draw::Atof(a[5]),Draw::Atof(a[6]),Draw::Atof(a[7])));
    i = 8;
  }
  else if (n < 14) {
    loc = gp_Ax3(gp_Pnt(Draw::Atof(a[2]),Draw::Atof(a[3]),Draw::Atof(a[4])),
		 gp_Dir(Draw::Atof(a[5]),Draw::Atof(a[6]),Draw::Atof(a[7])),
		 gp_Dir(Draw::Atof(a[8]),Draw::Atof(a[9]),Draw::Atof(a[10])));
    i = 11;
  }
  else
    return 1;

  Handle(Geom_Geometry) result;

  if (!strcasecmp(a[0],"plane")) {
    Handle(Geom_Plane) C = new Geom_Plane(loc);
    result = C;
  }
  else {
    if (i >= n) return 1;
    Standard_Real par1 = Draw::Atof(a[i]);
    
    if (!strcasecmp(a[0],"cylinder")) {
      Handle(Geom_CylindricalSurface) C = 
	new Geom_CylindricalSurface(loc,par1);
      result = C;
    }
    
    else if (!strcasecmp(a[0],"sphere")) {
      Handle(Geom_SphericalSurface) C = 
	new Geom_SphericalSurface(loc,par1);
      result = C;
    }
    
    else {
      if (i+1 >= n) return 1;
      Standard_Real par2 = Draw::Atof(a[i+1]);
      
      if (!strcasecmp(a[0],"cone")) {
	par1 *= (M_PI / 180.0);
	Handle(Geom_ConicalSurface) C =
	  new Geom_ConicalSurface(loc,par1,par2);
	result = C;
      }
    
      else if (!strcasecmp(a[0],"torus")) {
	Handle(Geom_ToroidalSurface) C =
	  new Geom_ToroidalSurface(loc,par1,par2);
	result = C;
      }
    }    
  }

  DrawTrSurf::Set(a[1],result);
  return 0;
}


//=======================================================================
//function : polesurface
//purpose  : 
//=======================================================================

static Standard_Integer polesurface (Draw_Interpretor& , Standard_Integer n, const char** a)
{
  Standard_Integer k,j,i;


  if (n < 4) return 1;

  if (!strcasecmp(a[0],"beziersurf")) {
    
    Standard_Integer nup = Draw::Atoi(a[2]);
    Standard_Integer nvp = Draw::Atoi(a[3]);
    if (nup * nvp == 0) return 1;
    
    i = (n - 4) / (nup * nvp);
    if (i < 3 || i > 4) return 1;
    Standard_Boolean hasw = i == 4;
    
    TColgp_Array2OfPnt poles(1,nup,1,nvp);
    TColStd_Array2OfReal weights(1,nup,1,nvp);
    
    k = 4;
    for (j = 1; j <= nvp; j++) {
      for (i = 1; i <= nup; i++) {
	poles(i, j).SetCoord(Draw::Atof(a[k]),Draw::Atof(a[k+1]),Draw::Atof(a[k+2]));
	k += 3;
	if (hasw) {
	  weights(i, j) = Draw::Atof(a[k]);
	  k++;
	}
      }
    }
    
    Handle(Geom_BezierSurface) result;
    if (hasw)
      result = new Geom_BezierSurface(poles,weights);
    else
      result = new Geom_BezierSurface(poles);

    DrawTrSurf::Set(a[1],result);
  }

  else {
    Standard_Integer udeg = Draw::Atoi(a[2]);
    Standard_Integer nbuk = Draw::Atoi(a[3]);

    Standard_Boolean uper = (*a[0] == 'u') || (*(a[0]+1) == 'u');
    Standard_Boolean vper = (*a[0] == 'v') || (*(a[0]+1) == 'v');

    TColStd_Array1OfReal    uk   (1, nbuk);
    TColStd_Array1OfInteger umult(1, nbuk);
    k = 4;
    Standard_Integer SigmaU = 0;
    for (i = 1; i<=nbuk; i++) {
      uk( i) = Draw::Atof(a[k]);
      k++;
      umult( i) = Draw::Atoi(a[k]);
      SigmaU += umult(i);
      k++;
    }

    Standard_Integer vdeg = Draw::Atoi(a[k]);
    k++;
    Standard_Integer nbvk = Draw::Atoi(a[k]);
    k++;

    TColStd_Array1OfReal    vk   (1, nbvk);
    TColStd_Array1OfInteger vmult(1, nbvk);
    Standard_Integer SigmaV = 0;
    for (i = 1; i<=nbvk; i++) {
      vk( i) = Draw::Atof(a[k]);
      k++;
      vmult( i) = Draw::Atoi(a[k]);
      SigmaV += vmult(i);
      k++;
    }

    Standard_Integer nup,nvp;
    if (uper)
      nup = SigmaU - umult(nbuk);
    else
      nup = SigmaU - udeg  -1;
    if (vper)
      nvp = SigmaV - vmult(nbvk);
    else
      nvp = SigmaV - vdeg  -1;
    TColgp_Array2OfPnt   poles  (1, nup, 1, nvp);
    TColStd_Array2OfReal weights(1, nup, 1, nvp);
    
    for (j = 1; j <= nvp; j++) {
      for (i = 1; i <= nup; i++) {
	poles(i, j).SetCoord(Draw::Atof(a[k]),Draw::Atof(a[k+1]),Draw::Atof(a[k+2]));
	k += 3;
	weights(i, j) = Draw::Atof(a[k]);
	k++;
      }
    }
    
    Handle(Geom_BSplineSurface) result =
      new Geom_BSplineSurface(poles, weights,
			      uk   , vk     ,
			      umult, vmult  ,
			      udeg , vdeg   ,
			      uper , vper   );

    DrawTrSurf::Set(a[1],result);
  }
  
  return 0;
}

//=======================================================================
//function : algosurface
//purpose  : 
//=======================================================================

static Standard_Integer algosurface (Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 5) return 1;

  Handle(Geom_Curve) GC = DrawTrSurf::GetCurve(a[2]);
  if (GC.IsNull()) return 1;

  gp_Dir D;
  gp_Pnt P;

  if (!strcasecmp(a[0],"extsurf")) {
    D.SetCoord(Draw::Atof(a[3]),Draw::Atof(a[4]),Draw::Atof(a[5]));
    Handle(Geom_SurfaceOfLinearExtrusion) result =
      new Geom_SurfaceOfLinearExtrusion(GC,D);

    DrawTrSurf::Set(a[1],result);

  }
  else if (!strcasecmp(a[0],"revsurf")) {
    if (n<8) return 1;
    P.SetCoord(Draw::Atof(a[3]),Draw::Atof(a[4]),Draw::Atof(a[5]));
    D.SetCoord(Draw::Atof(a[6]),Draw::Atof(a[7]),Draw::Atof(a[8]));
    
    Handle(Geom_SurfaceOfRevolution) result =
      new Geom_SurfaceOfRevolution(GC,gp_Ax1(P,D));

    DrawTrSurf::Set(a[1],result);

  }

  return 0;

}



//=======================================================================
//function : trimming
//purpose  : 
//=======================================================================

static Standard_Integer trimming (Draw_Interpretor& , 
				  Standard_Integer n, const char** a)
{
  if (n < 3) return 1;

  Handle(Geom_Curve)   GC   = DrawTrSurf::GetCurve(a[2]);
  Handle(Geom2d_Curve) GC2d = DrawTrSurf::GetCurve2d(a[2]);
  Handle(Geom_Surface) GS   = DrawTrSurf::GetSurface(a[2]);

  if (n == 3) {
    if (!GC.IsNull()) {
      Handle(Geom_TrimmedCurve) T = Handle(Geom_TrimmedCurve)::DownCast(GC);
      if (!T.IsNull()) GC = T->BasisCurve();
      DrawTrSurf::Set(a[1],GC);
    }
    else if (!GC2d.IsNull()) {
      Handle(Geom2d_TrimmedCurve) T = Handle(Geom2d_TrimmedCurve)::DownCast(GC2d);
      if (!T.IsNull()) GC2d = T->BasisCurve();
      DrawTrSurf::Set(a[1],GC2d);
    }
    else if (!GS.IsNull()) {
      Handle(Geom_RectangularTrimmedSurface) T = Handle(Geom_RectangularTrimmedSurface)::DownCast(GS);
      if (!T.IsNull()) GS = T->BasisSurface();
      DrawTrSurf::Set(a[1],GS);
    }
    return 0;
  }
	
  if (n < 5) return 1;

  Standard_Real u1 = Draw::Atof(a[3]);
  Standard_Real u2 = Draw::Atof(a[4]);

  Handle(Geom_Geometry) result;
  Handle(Geom2d_Curve) result2d;

  if (!strcasecmp(a[0],"trim")) {
    if (!GS.IsNull()) {
      if (n<7) return 1;
      result =
	new Geom_RectangularTrimmedSurface(GS,u1,u2,Draw::Atof(a[5]),Draw::Atof(a[6]));
    }
    else if (!GC.IsNull()) {
      result = new Geom_TrimmedCurve(GC, u1, u2);
    }
    else if (!GC2d.IsNull()) {
      result2d = new Geom2d_TrimmedCurve(GC2d, u1, u2);
    }
    else
      return 1;
  }
  else {
    if (GS.IsNull()) return 1;
    result =  new Geom_RectangularTrimmedSurface(GS,u1,u2,
						 !strcasecmp(a[0],"trimu"));
  }

  if (!result.IsNull())
    DrawTrSurf::Set(a[1], result);
  else
    DrawTrSurf::Set(a[1],result2d);
  
  return 0;
}

//=======================================================================
//function : converting
//purpose  : 
//=======================================================================

static Standard_Integer converting(Draw_Interpretor& , Standard_Integer n, const char ** a)
{
  if ( n < 3) return 1;

  Convert_ParameterisationType 
    Parameterisation = Convert_TgtThetaOver2 ;
  if (strcmp(a[n-1], "qa") == 0) {
    Parameterisation = Convert_QuasiAngular ;
  }
  else if (strcmp(a[n-1], "c1") == 0) {
    Parameterisation = Convert_RationalC1 ;
  }
  else if (strcmp (a[n-1], "s1") == 0) {
    Parameterisation = Convert_TgtThetaOver2_1 ;
  } 
  else if (strcmp (a[n-1], "s2") == 0) {
    Parameterisation = Convert_TgtThetaOver2_2;
  }
  else if (strcmp (a[n-1], "s3") == 0) {
    Parameterisation = Convert_TgtThetaOver2_3 ;
  }
  else if (strcmp (a[n-1], "s4") == 0) {
    Parameterisation = Convert_TgtThetaOver2_4 ;
  }
  else if (strcmp (a[n-1], "po") == 0) {
    Parameterisation = Convert_Polynomial;
  }

  Handle(Geom_Curve) GC = DrawTrSurf::GetCurve(a[2]);
  if ( GC.IsNull()) {
    Handle(Geom_Surface) GS = DrawTrSurf::GetSurface(a[2]);
    if ( GS.IsNull()) {
      Handle(Geom2d_Curve) G2d = DrawTrSurf::GetCurve2d(a[2]);
      if ( G2d.IsNull()) {
	return 1;
      }
      else {
	G2d = Geom2dConvert::CurveToBSplineCurve(G2d,
						 Parameterisation);
	DrawTrSurf::Set(a[1], G2d);
      }
    }
    else {
      GS = GeomConvert::SurfaceToBSplineSurface( GS);
      DrawTrSurf::Set(a[1], GS);
    }
  }
  else {
    GC = GeomConvert::CurveToBSplineCurve( GC,
					  Parameterisation);
    DrawTrSurf::Set(a[1], GC);
  }

  return 0;
}


//=======================================================================
//function : tobezier
//purpose  : 
//=======================================================================

static Standard_Integer tobezier(Draw_Interpretor& di,
				 Standard_Integer n, const char** a)
{
  if ( n < 3) return 1;
  Standard_Integer i,j,NbU,NbV,NbArc;
  char* name = new char[100];
  
  Handle(Geom2d_BSplineCurve) C2d = 
    DrawTrSurf::GetBSplineCurve2d(a[2]);
  if ( C2d.IsNull()) {
    Handle(Geom_BSplineCurve) C3d = 
      DrawTrSurf::GetBSplineCurve(a[2]);
    if ( C3d.IsNull()) {
      Handle(Geom_BSplineSurface) S = 
	DrawTrSurf::GetBSplineSurface(a[2]);
      if ( S.IsNull()) return 1;
      if (n == 7) {
	Standard_Real U1, U2, V1, V2;
	U1 = Draw::Atof(a[3]);
	U2 = Draw::Atof(a[4]);
	V1 = Draw::Atof(a[5]);
	V2 = Draw::Atof(a[6]);
	GeomConvert_BSplineSurfaceToBezierSurface 
	  Conv(S, U1, U2, V1, V2, Precision::PConfusion());
	NbU = Conv.NbUPatches();
	NbV = Conv.NbVPatches();
	di << NbU << " X " << NbV << " patches in the result\n";
	for (i = 1; i <= NbU; i++) {
	  for (j = 1; j <= NbV; j++) {
	    Sprintf(name,"%s_%i_%i",a[1],i,j);
	    char *temp = name ;
	    DrawTrSurf::Set(temp,Conv.Patch(i,j));
	  }
	}
      }
      else {
	GeomConvert_BSplineSurfaceToBezierSurface Conv(S);
	NbU = Conv.NbUPatches();
	NbV = Conv.NbVPatches();
	di << NbU << " X " << NbV << " patches in the result\n";
	for (i = 1; i <= NbU; i++) {
	  for (j = 1; j <= NbV; j++) {
	    Sprintf(name,"%s_%i_%i",a[1],i,j);
	    char *temp = name ;
	    DrawTrSurf::Set(temp,Conv.Patch(i,j));
	  }
	}
      }
    }
    else {
      if (n==5) {
	Standard_Real U1, U2;
	U1 = Draw::Atof(a[3]);
	U2 = Draw::Atof(a[4]);
	GeomConvert_BSplineCurveToBezierCurve Conv(C3d, U1, U2, 
						   Precision::PConfusion());
	NbArc = Conv.NbArcs();
	di << NbArc << " arcs in the result\n";
	for (i = 1; i <= NbArc; i++) {
	  Sprintf(name,"%s_%i",a[1],i);
	  char *temp = name ;
	  DrawTrSurf::Set(temp,Conv.Arc(i));
	}
      }
      else {
	GeomConvert_BSplineCurveToBezierCurve Conv(C3d);
	NbArc = Conv.NbArcs();
	di << NbArc << " arcs in the result\n";
	for (i = 1; i <= NbArc; i++) {
	  Sprintf(name,"%s_%i",a[1],i);
	  char *temp = name ;
	  DrawTrSurf::Set(temp,Conv.Arc(i));
	}
      }
    }
  }
  else {
    if (n==5) {
      Standard_Real U1, U2;
      U1 = Draw::Atof(a[3]);
      U2 = Draw::Atof(a[4]);
      Geom2dConvert_BSplineCurveToBezierCurve Conv(C2d, U1, U2, 
						   Precision::PConfusion());
      NbArc = Conv.NbArcs();
      di << NbArc << " arcs in the result\n";
      for (i = 1; i <= NbArc; i++) {
	Sprintf(name,"%s_%i",a[1],i);
	char *temp = name ;
	DrawTrSurf::Set(temp,Conv.Arc(i));
      }
    }
    else {
      Geom2dConvert_BSplineCurveToBezierCurve Conv(C2d);
      NbArc = Conv.NbArcs();
      di << NbArc << " arcs in the result\n";
      for (i = 1; i <= NbArc; i++) {
	Sprintf(name,"%s_%i",a[1],i);
	char *temp = name ;
	DrawTrSurf::Set(temp,Conv.Arc(i));
      }
    }
  }

  return 0;
}

//=======================================================================
//function : convbz
//purpose  : 
//=======================================================================

static Standard_Integer convbz(Draw_Interpretor& di,
				 Standard_Integer n, const char** a)
{
  if ( n < 4) return 1;

  Standard_Integer ii, jj, kk=0, NbU, NbV;
  Standard_Real Tol = Precision::Confusion();
  
  NbU = Draw::Atoi(a[2]);
  Handle(Geom_Curve) aCurve (Handle(Geom_Curve)::DownCast(DrawTrSurf::Get(a[3])));
  if (aCurve.IsNull()) {
    // Cas Surfacique
    NbV = Draw::Atoi(a[3]);
    if (n<4+NbU*NbV) {
      di << "The number of bezier surface have to be " << NbU*NbV << "\n";
      return 1;
    }
    TColGeom_Array2OfBezierSurface BZ(1, NbU, 1, NbV);
    kk = 4;
    for (jj=1; jj<=NbV; jj++)
      for(ii=1;ii<=NbU; ii++) {
	BZ(ii,jj) = 
	  Handle(Geom_BezierSurface)::DownCast(DrawTrSurf::Get(a[kk]));
	if (BZ(ii,jj).IsNull()) {
	  di << "the Surface " << kk <<"is not a BezierSurface\n";
	  return 1;
	}
	kk++;
      }
    if (kk<n) Tol = Draw::Atof(a[kk]);
  
    GeomConvert_CompBezierSurfacesToBSplineSurface Conv(BZ, Tol);
    
    if (! Conv.IsDone()) {
      di << "Convert Not Done\n";
      return 1;
    }

    Handle(Geom_BSplineSurface) BSurf = 
      new Geom_BSplineSurface(Conv.Poles()->Array2(),
			      Conv.UKnots()->Array1(),
			      Conv.VKnots()->Array1(),
			      Conv.UMultiplicities()->Array1(),
			      Conv.VMultiplicities()->Array1(),
			      Conv.UDegree(),
			      Conv.VDegree());

    DrawTrSurf::Set(a[1], BSurf);
  }
  else { // cas de courbes
    Convert_CompBezierCurvesToBSplineCurve Conv;
    Handle(Geom_BezierCurve) BZ;
    for (ii=1, kk=3; ii<=NbU; ii++,kk++) {
      BZ =  Handle(Geom_BezierCurve)::DownCast(DrawTrSurf::Get(a[kk]));
      if (BZ.IsNull()) {
	  di << "the curve " << kk <<"is not a BezierCurve\n";
	  return 1;
	}
      TColgp_Array1OfPnt Poles(1, BZ->NbPoles());
      BZ->Poles(Poles);
      Conv.AddCurve(Poles);
    }

    Conv.Perform();

    TColgp_Array1OfPnt Poles(1, Conv.NbPoles());
    Conv.Poles(Poles);
    TColStd_Array1OfInteger Mults(1, Conv.NbKnots());
    TColStd_Array1OfReal  Knots(1, Conv.NbKnots());
    Conv.KnotsAndMults(Knots, Mults);
    Handle(Geom_BSplineCurve) BS = 
      new (Geom_BSplineCurve) (Poles, Knots, Mults,
			       Conv.Degree());
    DrawTrSurf::Set(a[1], BS);
  }

    return 0;
}

//=======================================================================
//function : approxsurf
//purpose  : Approximation d'une Surface par une BSpline non rationnelle
//=======================================================================


static Standard_Integer approxsurf(Draw_Interpretor& di, Standard_Integer n, const char** a)
{ 
// " Tolerance (par defaut 0.1mm) " 
  Standard_Real Tol = 1.e-4;
// " Ordres de continuites : 0, 1 ou 2 (par defaut 1)" 
  GeomAbs_Shape myUCont = GeomAbs_C1, myVCont = GeomAbs_C1;
// " Degre maximum des carreaux de Bezier 14 par defaut "
  Standard_Integer degU = 14, degV = 14;
// " Nombre max de carreaux (par defaut 10)" 
  Standard_Integer nmax = 16;
// "Code de precision par defaults"
  Standard_Integer myPrec = 1;  

  if ( n>10 || n<3) return 1;

  if (n>3) Tol = Max(Draw::Atof(a[3]),1.e-10);

  if (n==5)  return 1;
  
  if (n>5) {
    if (Draw::Atoi(a[4]) == 0) myUCont = GeomAbs_C0;
    if (Draw::Atoi(a[4]) == 2) myUCont = GeomAbs_C2;
    if (Draw::Atoi(a[5]) == 0) myVCont = GeomAbs_C0;
    if (Draw::Atoi(a[5]) == 2) myVCont = GeomAbs_C2;
  }

  if (n==7)  return 1;

  if (n>7) {
    ( degU = (Draw::Atoi(a[6])));
    ( degV = (Draw::Atoi(a[7])));
    if ((degU<1) || (degU>24)) degU = 14;
    if ((degV<1) || (degV>24)) degV = 14; 
  }  

  if (n>8) nmax = Draw::Atoi(a[8]);
  if (n>9) myPrec =  Draw::Atoi(a[9]);

  Handle(Geom_Surface) surf = DrawTrSurf::GetSurface(a[2]);
  if (surf.IsNull()) return 1;  
  GeomConvert_ApproxSurface myApprox(surf,Tol,myUCont,myVCont,degU,degV,nmax,myPrec);
  if ( myApprox.HasResult()) DrawTrSurf::Set(a[1], myApprox.Surface());
  di<<a[1]<<"\n";
  return 0;
}

//=======================================================================
//function : offseting
//purpose  : 
//=======================================================================

static Standard_Integer offseting (Draw_Interpretor& ,
				   Standard_Integer n, const char** a)
{
  if (n < 4) return 1;

  // test the Geom2d curve
  Handle(Geom2d_Curve) C2d = DrawTrSurf::GetCurve2d(a[2]);
  if (!C2d.IsNull()) {
    Handle(Geom2d_OffsetCurve) OC = new Geom2d_OffsetCurve(C2d,Draw::Atof(a[3]));
    DrawTrSurf::Set(a[1],OC);
    return 0;
  }

  Standard_Boolean yasurf = Standard_False;

  Handle(Geom_Curve) GC = DrawTrSurf::GetCurve(a[2]);
  Handle(Geom_Surface) GS;
  if (GC.IsNull()) {
    GS = DrawTrSurf::GetSurface(a[2]);
    if (GS.IsNull())
      return 1;
    yasurf = Standard_True;
  }

  Standard_Real dist = Draw::Atof(a[3]);

  Handle(Geom_Geometry) result;

  if (yasurf) {
    Handle(Geom_OffsetSurface) GO = new Geom_OffsetSurface(GS,dist);
    result = GO;
  }
  else {
    if (n < 7) return 1;
    gp_Dir D(Draw::Atof(a[4]),Draw::Atof(a[5]),Draw::Atof(a[6]));
    Handle(Geom_OffsetCurve) GT = new Geom_OffsetCurve(GC, dist, D);
    result = GT;
  }

  DrawTrSurf::Set(a[1], result);
  return 0;
}

//=======================================================================
//function : sreverse
//purpose  : 
//=======================================================================

static Standard_Integer sreverse (Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 2) return 1;

  Standard_Integer i;
  for (i = 1; i < n; i++) {

    Handle(Geom_Surface) GS = DrawTrSurf::GetSurface(a[i]);
    if (!GS.IsNull()) {
      if (*a[0] == 'u')
	GS->UReverse();
      else
	GS->VReverse();
      Draw::Repaint();
    }
  }

  return 0;
}

//=======================================================================
//function : iso
//purpose  : 
//=======================================================================

static Standard_Integer iso (Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 4) return 1;

  Handle(Geom_Curve) C;
  Standard_Real par = Draw::Atof(a[3]);
  Handle(Geom_Surface) GS = DrawTrSurf::GetSurface(a[2]);
  if (!GS.IsNull()) {
    if (*a[0] == 'u')
      C = GS->UIso(par);
    else
      C = GS->VIso(par);
    DrawTrSurf::Set(a[1],C);
  }
  
  return 0;
}


//=======================================================================
//function : value
//purpose  : 
//=======================================================================

static Standard_Integer value (Draw_Interpretor& ,
			       Standard_Integer n, const char** a)
{
  if (n < 5) return 1;

  Handle(Geom_Surface) GS = DrawTrSurf::GetSurface(a[1]);
  if (GS.IsNull()) return 1;

  Standard_Real U = Draw::Atof(a[2]);
  Standard_Real V = Draw::Atof(a[3]);

  Standard_Boolean DrawPoint = ( n%3 == 2);
  if ( DrawPoint) n--;

  gp_Pnt P;
  if (n >= 13) {
    gp_Vec DU,DV;
    if (n >= 22) {
      gp_Vec D2U,D2V,D2UV;
      GS->D2(U,V,P,DU,DV,D2U,D2V,D2UV);
      Draw::Set(a[13],D2U.X());
      Draw::Set(a[14],D2U.Y());
      Draw::Set(a[15],D2U.Z());
      Draw::Set(a[16],D2V.X());
      Draw::Set(a[17],D2V.Y());
      Draw::Set(a[18],D2V.Z());
      Draw::Set(a[19],D2UV.X());
      Draw::Set(a[20],D2UV.Y());
      Draw::Set(a[21],D2UV.Z());
    }
    else
      GS->D1(U,V,P,DU,DV);

    Draw::Set(a[7],DU.X());
    Draw::Set(a[8],DU.Y());
    Draw::Set(a[9],DU.Z());
    Draw::Set(a[10],DV.X());
    Draw::Set(a[11],DV.Y());
    Draw::Set(a[12],DV.Z());
  }
  else 
    GS->D0(U,V,P);

  if ( n > 6) {
    Draw::Set(a[4],P.X());
    Draw::Set(a[5],P.Y());
    Draw::Set(a[6],P.Z());
  }
  if ( DrawPoint) {
    DrawTrSurf::Set(a[n],P);
  }

  return 0;
}

//=======================================================================
//function : movepole
//purpose  : 
//=======================================================================

static Standard_Integer movepole (Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 6) return 1;
  Standard_Boolean BSpline = Standard_False;

  Handle(Geom_BezierSurface) GBz = DrawTrSurf::GetBezierSurface(a[1]);
  Handle(Geom_BSplineSurface) GBs;
  if (GBz.IsNull()) {
    GBs = DrawTrSurf::GetBSplineSurface(a[1]);
    if (GBs.IsNull())
    {
      return 1;
    }
    BSpline = Standard_True;
  }

  Standard_Real dx = Draw::Atof(a[n-3]);
  Standard_Real dy = Draw::Atof(a[n-2]);
  Standard_Real dz = Draw::Atof(a[n-1]);
  
  Standard_Integer nup, nvp;
  if( !BSpline) {
    nup = GBz->NbUPoles();
    nvp = GBz->NbVPoles();
  }
  else {
    nup = GBs->NbUPoles();
    nvp = GBs->NbVPoles();
  }

  Standard_Integer FirstRow=0, LastRow=0, FirstCol=0, LastCol=0;
  // Rem : Row = indice ligne.  -> variation en U. 
  //       Col = indice colonne.-> variation en V.

  if (!strcasecmp(a[0],"movep")) {
    if (n<7) return 1;
    FirstRow = Draw::Atoi(a[2]);
    FirstCol = Draw::Atoi(a[3]);
    if ( FirstRow < 1  || FirstRow > nup ||
	 FirstCol < 1  || FirstCol > nvp   ) return 1;
    LastRow = FirstRow;
    LastCol = FirstCol;
  }
  else if (!strcasecmp(a[0],"moverowp")) {
    FirstRow = Draw::Atoi(a[2]);
    if ( FirstRow < 1  || FirstRow > nup ) return 1;
    LastRow = FirstRow;
    FirstCol = 1;
    LastCol  = nvp;
  }
  else if (!strcasecmp(a[0],"movecolp")) {
    FirstCol = Draw::Atoi(a[2]);
    if ( FirstCol < 1  || FirstCol > nvp ) return 1;
    LastCol = FirstCol;
    FirstRow = 1;
    LastRow  = nup;
  }

  gp_Pnt P;

  for ( Standard_Integer i = FirstRow; i<= LastRow; i++) {
    for ( Standard_Integer j = FirstCol; j<= LastCol; j++) {
      if( !BSpline) {
	P = GBz->Pole(i,j);
	P.SetCoord(P.X()+dx, P.Y()+dy, P.Z()+dz);
	GBz->SetPole(i,j,P);
      }
      else {
	P = GBs->Pole(i,j);
	P.SetCoord(P.X()+dx, P.Y()+dy, P.Z()+dz);
	GBs->SetPole(i,j,P);
      }
    }
  }

  Draw::Repaint();

  return 0;
}


//=======================================================================
//function : movepoint
//purpose  : 
//=======================================================================

static Standard_Integer movepoint (Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 7) return 1;

  Handle(Geom_BSplineSurface) GBs = DrawTrSurf::GetBSplineSurface(a[1]);
  if (GBs.IsNull()) {
    return 1;
  }

  Standard_Real u = Draw::Atof(a[2]);
  Standard_Real v = Draw::Atof(a[3]);

  Standard_Real dx = Draw::Atof(a[4]);
  Standard_Real dy = Draw::Atof(a[5]);
  Standard_Real dz = Draw::Atof(a[6]);
  
  Standard_Integer index1u = 0;
  Standard_Integer index2u = 0;
  Standard_Integer index1v = 0;
  Standard_Integer index2v = 0;

  Standard_Integer fmodifu, lmodifu, fmodifv, lmodifv;
  if (n == 11) {
    index1u = Draw::Atoi(a[7]);
    index2u = Draw::Atoi(a[8]);
    index1v = Draw::Atoi(a[9]);
    index2v = Draw::Atoi(a[10]);
  }
  else {
    index1u = 2;
    index2u = GBs->NbUPoles()-1;
    index1v = 2;
    index2v = GBs->NbVPoles()-1;
  }

  gp_Pnt p;
  GBs->D0(u, v, p);
  p.SetCoord(p.X()+dx, p.Y()+dy, p.Z()+dz);
  GBs->MovePoint(u, v, p, index1u, index2u, index1v, index2v, fmodifu, lmodifu, fmodifv, lmodifv);
  Draw::Repaint();
  return 0;
}


//=======================================================================
//function : insertknot
//purpose  : 
//=======================================================================

static Standard_Integer insertknot (Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 3) return 1;

  Handle(Geom_BSplineSurface) GBs = DrawTrSurf::GetBSplineSurface(a[1]);

  if (GBs.IsNull()) return 1;

  Standard_Real    knot=0;
  Standard_Integer mult = 0;
  Standard_Integer index=0;
  if (  !strcasecmp(a[0],"insertuknot") ||
        !strcasecmp(a[0],"insertvknot")   ) {
    if (n<4) return 1;
    knot = Draw::Atof(a[2]);
    mult = Draw::Atoi(a[3]);
  }
  else if (  !strcasecmp(a[0],"remuknot") ||
	     !strcasecmp(a[0],"remvknot")   ) {
    index = Draw::Atoi(a[2]);
    if (n>=4) mult  = Draw::Atoi(a[3]);
  }

  Standard_Real tol = RealLast();

  if (!strcasecmp(a[0],"insertuknot")) {
    GBs->InsertUKnot(knot,mult,Precision::PConfusion());
  }
  else if (!strcasecmp(a[0],"insertvknot")) {
    GBs->InsertVKnot(knot,mult,Precision::PConfusion());
  }
  else if (!strcasecmp(a[0],"remuknot")) {
    if (n>=5) tol = Draw::Atof(a[4]);
    if (!GBs->RemoveUKnot(index,mult,tol)) 
      return 1;
  }
  else if (!strcasecmp(a[0],"remvknot")) {
    if (n>=5) tol = Draw::Atof(a[4]);
    if (!GBs->RemoveVKnot(index,mult,tol))
      return 1;
  }

  Draw::Repaint();
  return 0;
}

//=======================================================================
//function : incdegree
//purpose  : 
//=======================================================================

static Standard_Integer incdegree (Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 3) return 1;
  
  Standard_Integer NewDeg = Draw::Atoi(a[2]);
  Standard_Boolean BSpline = Standard_False;
  
  Standard_Integer UDeg=0, VDeg=0;
  
  Handle(Geom_BezierSurface) GBz = DrawTrSurf::GetBezierSurface(a[1]);
  Handle(Geom_BSplineSurface) GBs;
  
  if (GBz.IsNull()) {
    GBs = DrawTrSurf::GetBSplineSurface(a[1]);
    if (GBs.IsNull())
      return 1;
    BSpline = Standard_True;
  }
  
  Standard_Integer Degree=0;
  if ( !strcasecmp(a[0],"incudeg")) {
    UDeg = NewDeg;
    if (BSpline) {
      Degree = GBs->UDegree();
      VDeg   = GBs->VDegree();
    }
    else {
      Degree = GBz->UDegree();
      VDeg   = GBz->VDegree();
    }
  }  
  else if ( !strcasecmp(a[0],"incvdeg")) {
    VDeg = NewDeg;
    if (BSpline) {
      Degree = GBs->VDegree();
      UDeg   = GBs->UDegree();
    }
    else {
      Degree = GBz->VDegree();
      UDeg   = GBz->UDegree();
    }
  }
  
  if (Degree > NewDeg) {
    di<<"The Degree must be greater than " << Degree <<"\n";
    return 1;
  }
  
  if ( BSpline) {
    GBs->IncreaseDegree(UDeg, VDeg);
  }
  else {
    GBz->Increase(UDeg, VDeg);
  }

  Draw::Repaint();
  return 0;
}

//=======================================================================
//function : rempole
//purpose  : 
//=======================================================================

static Standard_Integer rempole (Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 3) return 1;
  
  Standard_Integer NewIndex = Draw::Atoi(a[2]);
  Standard_Boolean BSpline  = Standard_False;
  
  Handle(Geom_BezierSurface) GBz = DrawTrSurf::GetBezierSurface(a[1]);
  Handle(Geom_BSplineSurface) GBs;
  
  if (GBz.IsNull()) {
    GBs = DrawTrSurf::GetBSplineSurface(a[1]);
    if (GBs.IsNull())
      return 1;
    BSpline = Standard_True;
  }
  
  if ( !strcasecmp(a[0],"remrowpole")) {
    if ( BSpline) {
      di << " Error : Cannot remove a polerow on a BSplineSurface \n";
    }
    else {
      GBz->RemovePoleRow(NewIndex);
    }
  }
  else if ( !strcasecmp(a[0],"remcolpole")) {
    if ( BSpline) {
      di << " Error : Cannot remove a polecol on a BSplineSurface \n";
    }
    else {
      GBz->RemovePoleCol(NewIndex);
    }
  }

  Draw::Repaint();
  return 0;
}

//=======================================================================
//function : sfindp
//purpose  : 
//=======================================================================

static Standard_Integer sfindp (Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 7) return 1;
  Standard_Boolean BSpline = Standard_False;

  Handle(Geom_BezierSurface) GBz = DrawTrSurf::GetBezierSurface(a[1]);
  Handle(Geom_BSplineSurface) GBs;
  if (GBz.IsNull()) {
    GBs = DrawTrSurf::GetBSplineSurface(a[1]);
    if (GBs.IsNull())
    {
      return 1;
    }
    BSpline = Standard_True;
  }

  Standard_Integer UIndex = 0;
  Standard_Integer VIndex = 0;
  Standard_Integer view = Draw::Atoi(a[2]);
  Standard_Real x = Draw::Atof(a[3]);
  Standard_Real y = Draw::Atof(a[4]);

  Draw_Display d = dout.MakeDisplay(view);
  
  if( !BSpline) {
    Handle(DrawTrSurf_BezierSurface) DBz = 
      new DrawTrSurf_BezierSurface(GBz);
    DBz->FindPole( x, y, d, 5, UIndex,VIndex);
  }
  else {
    Handle(DrawTrSurf_BSplineSurface) DBs = 
      new DrawTrSurf_BSplineSurface(GBs);
    DBs->FindPole( x, y, d, 5, UIndex,VIndex);
  }

  Draw::Set(a[5],UIndex);
  Draw::Set(a[6],VIndex);
  
  return 0;
}


//=======================================================================
//function : ssetperiodic
//purpose  : 
//=======================================================================

static Standard_Integer ssetperiodic (Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 2) return 1;

  Standard_Integer i;

  if (!strcasecmp(a[0],"setuperiodic")) {
    for (i = 1; i < n; i++) {
      Handle(Geom_BSplineSurface) 
	GBs = DrawTrSurf::GetBSplineSurface(a[i]);
      if (!GBs.IsNull()) {
	GBs->SetUPeriodic();
	Draw::Repaint();
      }
    }
  }
  else if (!strcasecmp(a[0],"setvperiodic")){
    for (i = 1; i < n; i++) {
      Handle(Geom_BSplineSurface) 
	GBs = DrawTrSurf::GetBSplineSurface(a[i]);
      if (!GBs.IsNull()) {
	GBs->SetVPeriodic();
	Draw::Repaint();
      }
    }
  }
  else if (!strcasecmp(a[0],"setunotperiodic")){
    for (i = 1; i < n; i++) {
      Handle(Geom_BSplineSurface) 
	GBs = DrawTrSurf::GetBSplineSurface(a[i]);
      if (!GBs.IsNull()) {
	GBs->SetUNotPeriodic();
	Draw::Repaint();
      }
    }
  }
  else if (!strcasecmp(a[0],"setvnotperiodic")){
    for (i = 1; i < n; i++) {
      Handle(Geom_BSplineSurface) 
	GBs = DrawTrSurf::GetBSplineSurface(a[i]);
      if (!GBs.IsNull()) {
	GBs->SetVNotPeriodic();
	Draw::Repaint();
      }
    }
  }
  return 0;
}

//=======================================================================
//function : exchuv
//purpose  : 
//=======================================================================

static Standard_Integer exchuv (Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 2) return 1;

  Standard_Integer i;
  for (i = 1; i < n; i++) {

    Handle(Geom_BSplineSurface) GBs = DrawTrSurf::GetBSplineSurface(a[i]);
    if (!GBs.IsNull()) {
      GBs->ExchangeUV();
      Draw::Repaint();
    }
    else {
      Handle(Geom_BezierSurface) GBz = DrawTrSurf::GetBezierSurface(a[i]);
      if (!GBz.IsNull()) {
	GBz->ExchangeUV();
	Draw::Repaint();
      }
    }
  }

  return 0;
}

//=======================================================================
//function : segsur
//purpose  : 
//=======================================================================

static Standard_Integer segsur (Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 6) return 1;

  Handle(Geom_BezierSurface) GBz = DrawTrSurf::GetBezierSurface(a[1]);
  Handle(Geom_BSplineSurface) GBs;
  if (GBz.IsNull()) {
    GBs = DrawTrSurf::GetBSplineSurface(a[1]);
    if (GBs.IsNull())
      return 1;
    GBs->Segment(Draw::Atof(a[2]),Draw::Atof(a[3]),Draw::Atof(a[4]),Draw::Atof(a[5])); 
  }
  else {
    GBz->Segment(Draw::Atof(a[2]),Draw::Atof(a[3]),Draw::Atof(a[4]),Draw::Atof(a[5]));
  }

  Draw::Repaint();
  return 0;
}

static Standard_Integer compBsplSur (Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 2) 
  {
    cout<<"Invalid number of parameters"<<endl;
    return 1;
  }

  Handle(Geom_BSplineSurface) GBs1 = DrawTrSurf::GetBSplineSurface(a[1]);
  Handle(Geom_BSplineSurface) GBs2 = DrawTrSurf::GetBSplineSurface(a[2]);
  if (GBs1.IsNull() || GBs2.IsNull()) {
    cout<<"Invalid surface"<<endl;
    return 1;
  }
   
  Standard_Real aU11,aU12,aV11,aV12;
  GBs1->Bounds(aU11,aU12,aV11,aV12);
  
  Standard_Real aU21,aU22,aV21,aV22;
  GBs2->Bounds(aU21,aU22,aV21,aV22);
  
  Standard_Real aUmin = Max(aU11,aU21);
  Standard_Real aUmax = Min(aU12,aU22);
  
  Standard_Real aVmin = Max(aV11,aV21);
  Standard_Real aVmax = Min(aV12,aV22);
  
  Standard_Integer nbP = 100;
  Standard_Real aStepU = (aUmax - aUmin)/nbP;
  Standard_Real aStepV = (aVmax - aVmin)/nbP;
  Standard_Integer nbErr =0;
  Standard_Integer i =1;
  for( ; i <= nbP +1; i++)
  {
    Standard_Real aU = aUmin + aStepU*(i-1);
    Standard_Integer j =1;
    for( ; j <= nbP +1; j++)
    {
      Standard_Real aV = aVmin + aStepV*(j-1);
      gp_Pnt aP1 = GBs1->Value(aU,aV);
      gp_Pnt aP2 = GBs2->Value(aU,aV);
      Standard_Real aDist = aP1.SquareDistance(aP2);
      if(aDist > Precision::SquareConfusion())
      {
        nbErr++;
        Standard_Real aD = sqrt(aDist);
        cout<<"Surfaces differ for U,V,Dist: "<<aU<<" "<<aV<<" "<<aD<<endl;
      }
    }
  }
  
  
  Draw::Repaint();
  return 0;
}

//=======================================================================
//function : setuvorigin
//purpose  : 
//=======================================================================

static Standard_Integer setuvorigin (Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 3) return 1;

  Handle(Geom_BSplineSurface) GBs = DrawTrSurf::GetBSplineSurface(a[1]);
  if (GBs.IsNull())
    return 1;
  if ( !strcasecmp(a[0],"setuorigin")) {
    GBs->SetUOrigin(Draw::Atoi(a[2])); 
  }
  else if ( !strcasecmp(a[0],"setvorigin")) {
    GBs->SetVOrigin(Draw::Atoi(a[2])); 
  }
  else 
    return 1;

  Draw::Repaint();
  return 0;
}


//=======================================================================
//function : parameters
//purpose  : 
//=======================================================================

static Standard_Integer parameters (Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if(n == 8)
    { 
      // try to find parameters on a Surface
      Handle(Geom_Surface) S = DrawTrSurf::GetSurface(a[1]);
      if( S.IsNull() ) { di << "Unknown surface\n"; return 1; }
      gp_Pnt P(Draw::Atof(a[2]), Draw::Atof(a[3]), Draw::Atof(a[4]));
      Standard_Real Tol = Draw::Atof(a[5]), U = 0., V = 0.;
      Standard_Boolean res = GeomLib_Tool::Parameters(S,P,Tol,U,V);

      Draw::Set(a[6],U);
      Draw::Set(a[7],V);

      if( !res ) { di << "Wrong point\n"; return 1; }
    }
  else if(n == 7)
    {
      // try to find parameters on a 3d Curve
      Handle(Geom_Curve) C = DrawTrSurf::GetCurve(a[1]);
      if( C.IsNull() ) { di << "Unknown curve\n"; return 1; }
      gp_Pnt P(Draw::Atof(a[2]), Draw::Atof(a[3]), Draw::Atof(a[4]));
      Standard_Real Tol = Draw::Atof(a[5]), U = 0.;
      Standard_Boolean res = GeomLib_Tool::Parameter(C,P,Tol,U);
    
      Draw::Set(a[6],U);

    if( !res ) { di << "Wrong point\n"; return 1; }
    }
  else if(n == 6)
    {
      // try to find parameters on a 2d Curve
      Handle(Geom2d_Curve) C = DrawTrSurf::GetCurve2d(a[1]);
      if( C.IsNull() ) { di << "Unknown curve 2d\n";  return 1; }
      gp_Pnt2d P(Draw::Atof(a[2]), Draw::Atof(a[3]));
      Standard_Real Tol = Draw::Atof(a[4]), U = 0.;
      Standard_Boolean res = GeomLib_Tool::Parameter(C,P,Tol,U);
    
      Draw::Set(a[5],U);

      if( !res ) { di << "Wrong point\n"; return 1; }
    }
  else
    {
      di << "Invalid parameters!\n";
      di << "Usage:\n";
      di << "parameters Surf X Y Z Tol U V\n";
      di << "parameters Curv X Y Z Tol U\n";
      di << "parameters Curv2d X Y Tol U\n";
      return 1;
    }

  return 0;
}


//=======================================================================
//function : bounds
//purpose  : 
//=======================================================================

Standard_Integer bounds(Draw_Interpretor&, Standard_Integer n, const char** a)
{
  Standard_Real U1, U2, V1, V2;
  if ( n == 4) {  // compute on a curve or a 2d curve
    Handle(Geom_Curve) C3d = DrawTrSurf::GetCurve(a[1]);
    if ( C3d.IsNull()) { // 2dcurve
      Handle(Geom2d_Curve) C2d = DrawTrSurf::GetCurve2d(a[1]);
      if ( C2d.IsNull()) return 1;
      U1 = C2d->FirstParameter();
      U2 = C2d->LastParameter();
    }
    else { // 3dcurve
      U1 = C3d->FirstParameter();
      U2 = C3d->LastParameter();
    }
    Draw::Set(a[2],U1);
    Draw::Set(a[3],U2);
  }
  else if ( n == 6) { // compute on a Surface
    Handle(Geom_Surface) S = DrawTrSurf::GetSurface(a[1]);
    if ( S.IsNull()) return 1;
    S->Bounds(U1,U2,V1,V2);

    Draw::Set(a[2],U1);
    Draw::Set(a[3],U2);
    Draw::Set(a[4],V1);
    Draw::Set(a[5],V2);
  }

  return 0;
}

//=======================================================================
//function : SurfaceCommands
//purpose  : 
//=======================================================================


void  GeomliteTest::SurfaceCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean loaded = Standard_False;
  if (loaded) return;
  loaded = Standard_True;

  DrawTrSurf::BasicCommands(theCommands);

  const char* g;
  // analytic surfaces
  g = "GEOMETRY surfaces creation";

  theCommands.Add("plane",
		  "plane name [x y z [dx dy dz [ux uy uz]]]",
		  __FILE__,
		  anasurface,g);

  theCommands.Add("cone",
		  "cone name [x y z [dx dy dz [ux uy uz]]] semi-angle radius",
		  __FILE__,
		  anasurface,g);

  theCommands.Add("cylinder",
		  "cylinder name [x y z [dx dy dz [ux uy uz]]]  radius",
		  __FILE__,
		  anasurface,g);

  theCommands.Add("sphere",
		  "sphere name [x y z [dx dy dz [ux uy uz]]]  radius",
		  __FILE__,
		  anasurface,g);

  theCommands.Add("torus",
		  "torus name [x y z [dx dy dz [ux uy uz]]]  major minor",
		  __FILE__,
		  anasurface,g);

  theCommands.Add("beziersurf",
		  "beziersurf name nbupoles nbvpoles pole, [weight]",
		  __FILE__,
		  polesurface,g);

  theCommands.Add("bsplinesurf",
		  "bsplinesurf name udegree nbuknots  uknot, umult  vdegree nbvknots vknot, vmult pole, weight",
		  __FILE__,
		  polesurface,g);

  theCommands.Add("upbsplinesurf",
		  "bsplinesurf name udegree nbuknots  uknot, umult  vdegree nbvknots vknot, vmult pole, weight",
		  __FILE__,
		  polesurface,g);

  theCommands.Add("vpbsplinesurf",
		  "bsplinesurf name udegree nbuknots  uknot, umult  vdegree nbvknots vknot, vmult pole, weight",
		  __FILE__,
		  polesurface,g);

  theCommands.Add("uvpbsplinesurf",
		  "bsplinesurf name udegree nbuknots  uknot, umult  vdegree nbvknots vknot, vmult pole, weight",
		  __FILE__,
		  polesurface,g);

  theCommands.Add("extsurf",
		  "extsurf name curvename dx dy dz",
		  __FILE__,
		  algosurface,g);

  theCommands.Add("revsurf",
		  "revsurf name curvename x y z dx dy dz",
		  __FILE__,
		  algosurface,g);

  theCommands.Add("offset",
		  "offset name basename distance [dx dy dz]",
		  __FILE__,
		  offseting,g);

  theCommands.Add("trim",
		  "trim newname name [u1 u2 [v1 v2]], no args remove trim",
		  __FILE__,
		  trimming,g);

  theCommands.Add("trimu",
		  "trim newname name u1 u2",
		  __FILE__,
		  trimming,g);

  theCommands.Add("trimv",
		  "trim newname name v1 v2",
		  __FILE__,
		  trimming,g);

  theCommands.Add("convert",
		  "convert result c2d/c3d/surf [qa,c1,s1,s2,s3,s4,po]",
		  __FILE__,
		  converting,g);

  theCommands.Add("tobezier",
		  "tobezier result c2d/c3d/surf [ufirst, ulast / ufirst, ulast, vfirst, vlast]",
		  __FILE__,
		  tobezier,g);

  theCommands.Add("convertfrombezier",
		  "convertfrombezier result nbu [nbv] bz1 [bz2 .... bzn] [tol]",
		  __FILE__,
		  convbz,g);

  theCommands.Add("approxsurf",
                  "approxsurf name surf [Tol [CnU CnV [degU degV [nmax]]]] ",
		  __FILE__,
		  approxsurf,g);

  g = "GEOMETRY Curves and Surfaces modification";

  theCommands.Add("ureverse",
		  "ureverse name ... ",
		  __FILE__,
		  sreverse,g);

  theCommands.Add("vreverse",
		  "vreverse name ... ",
		  __FILE__,
		  sreverse,g);

  theCommands.Add("movep",
		  "movep name row col dx dy dz",
		  __FILE__,
		  movepole,g);

  theCommands.Add("moverowp",
		  "moverowp name row dx dy dz",
		  __FILE__,
		  movepole,g);

  theCommands.Add("movecolp",
		  "movecolp name col dx dy dz",
		  __FILE__,
		  movepole,g);

  theCommands.Add("movepoint",
		  "movepoint name u v dx dy dz [index1u index2u index2v index2v",
		  __FILE__,
		  movepoint,g);

  theCommands.Add("insertuknot",
		  "insertuknot name knot mult",
		  __FILE__,
		  insertknot,g);

  theCommands.Add("insertvknot",
		  "insertvknot name knot mult",
		  __FILE__,
		  insertknot,g);

  theCommands.Add("remuknot",
		  "remuknot name index [mult] [tol]",
		  __FILE__,
		  insertknot,g);
  
  theCommands.Add("remvknot",
		  "remvknot name index [mult] [tol]",
		  __FILE__,
		  insertknot,g);

  theCommands.Add("incudeg",
		  "incudeg name degree",
		  __FILE__,
		  incdegree,g);

  theCommands.Add("incvdeg",
		  "incvdeg name degree",
		  __FILE__,
		  incdegree,g);

  theCommands.Add("remrowpole",
		  "remrowpole name index",
		  __FILE__,
		  rempole,g);

  theCommands.Add("remcolpole",
		  "remcolpole name index",
		  __FILE__,
		  rempole,g);

  theCommands.Add("sfindp",
		  "sfindp name view x y Uindex Vindex",
		  __FILE__,
		  sfindp,g);

  theCommands.Add("setuperiodic",
		  "setuperiodic name ...",
		  __FILE__,
		  ssetperiodic,g);

  theCommands.Add("setvperiodic",
		  "setvperiodic name ...",
		  __FILE__,
		  ssetperiodic,g);

  theCommands.Add("setunotperiodic",
		  "setunotperiodic name ...",
		  __FILE__,
		  ssetperiodic,g);

  theCommands.Add("setvnotperiodic",
		  "setvnotperiodic name ...",
		  __FILE__,
		  ssetperiodic,g);

  theCommands.Add("exchuv",
		  "exchuv name ...",
		  __FILE__,
		  exchuv,g);

  theCommands.Add("segsur",
		  "segsur name Ufirst Ulast Vfirst Vlast",
		  __FILE__,
		  segsur , g);

  theCommands.Add("setuorigin",
		  "setuorigin name knotindex",
		  __FILE__,
		  setuvorigin , g);

  theCommands.Add("setvorigin",
		  "setvorigin name knotindex",
		  __FILE__,
		  setuvorigin , g);

  g = "GEOMETRY curves creation";


  theCommands.Add("uiso",
		  "uiso curvename surfacename u",
		  __FILE__,
		  iso,g);

  theCommands.Add("viso",
		  "viso curvename surfacename v",
		  __FILE__,
		  iso,g);


  g = "GEOMETRY curves and surfaces analysis";

  theCommands.Add("svalue",
		  "svalue surfname U V X Y Z [DUX DUY DUZ DVX DVY DVZ [D2UX D2UY D2UZ D2VX D2VY D2VZ D2UVX D2UVY D2UVZ]]",
		  __FILE__,
		  value,g);

  theCommands.Add("parameters",
		  "parameters surf/curve X Y [Z] Tol U [V] : {X Y Z} point, {U V} output parameter(s)",
		  __FILE__,
		  parameters,g);

  theCommands.Add("bounds",
		  "bounds S/C/C2d U1 U2 [V1 V2]",
		  __FILE__,
		  bounds,g);

  theCommands.Add("surface_radius",
                  "surface_radius surface Uvalue <Real> Vvalue <Real> returns min max radius of curvature",
                  __FILE__,
		  surface_radius,g);
  theCommands.Add("compBsplSur","BsplSurf1 BSplSurf2",__FILE__,compBsplSur,g);
  
  
}
