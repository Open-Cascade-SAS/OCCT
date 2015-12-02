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

// PMN : Ajout de la commande smooth
// PMN : 11/07/97 Passage a GeomliteTest de bsmooth.

#include <Standard_Stream.hxx>

#include <GeomliteTest.hxx>
#include <DrawTrSurf.hxx>
#include <Draw.hxx>
#include <Draw_Appli.hxx>
#include <Draw_Interpretor.hxx>
#include <Precision.hxx>
#include <Draw_Marker3D.hxx>
#include <Draw_Marker2D.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <TColgp_SequenceOfPnt2d.hxx>

#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <DrawTrSurf_BSplineCurve.hxx>
#include <DrawTrSurf_BezierCurve.hxx>
#include <DrawTrSurf_BSplineCurve2d.hxx>
#include <DrawTrSurf_BezierCurve2d.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_HArray1OfVec.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfBoolean.hxx>

#include <AppParCurves_MultiBSpCurve.hxx>
#include <AppParCurves_MultiCurve.hxx>
#include <AppDef_MultiLine.hxx>
#include <AppDef_Variational.hxx>
#include <AppDef_Compute.hxx>
#include <AppParCurves_HArray1OfConstraintCouple.hxx>
#include <AppParCurves_ConstraintCouple.hxx>
#include <AppDef_HArray1OfMultiPointConstraint.hxx>
#include <AppDef_Array1OfMultiPointConstraint.hxx>
#include <math_Vector.hxx>

#ifdef _MSC_VER
#include <stdio.h>
#endif

#ifdef _WIN32
Standard_IMPORT Draw_Viewer dout;
#endif

//Draw_Color DrawTrSurf_CurveColor(const Draw_Color);

//=======================================================================
//function :  NbConstraint
//=======================================================================
static Standard_Integer  NbConstraint(const AppParCurves_Constraint C1,
				      const AppParCurves_Constraint C2)
{
   Standard_Integer N =0;
   switch (C1) {
   case  AppParCurves_PassPoint : 
     {
       N = 1;
       break;
     }
   case  AppParCurves_TangencyPoint : 
     {
       N =2;
       break;
     }
   case  AppParCurves_CurvaturePoint : 
     {
       N = 3;
       break;
     }     
   default:
       break;
   }

   switch (C2) {
   case  AppParCurves_PassPoint : 
     {
       N++;
       break;
     }
   case  AppParCurves_TangencyPoint : 
     {
       N += 2;
       break;
     }
   case  AppParCurves_CurvaturePoint : 
     {
       N += 3;
       break;
     }     
   default:
       break;
   }
   return N;
}
//=======================================================================
//function : PointsByPick
//=======================================================================
static Standard_Integer PointsByPick
       (Handle(AppDef_HArray1OfMultiPointConstraint)& MPC, Draw_Interpretor& di)
{
 Standard_Integer id,XX,YY,b, i;
 
 di << "Pick points \n";
 dout.Select(id, XX, YY, b);
 Standard_Real zoom = dout.Zoom(id);
 if (b != 1) return 0;
 if (id < 0) return 0;
 gp_Pnt P;
 gp_Pnt2d P2d;

 //Standard_Boolean newcurve;
    
 if (dout.Is3D(id)) {
   // Cas du 3D -------
   Handle(Draw_Marker3D) mark;
   TColgp_SequenceOfPnt ThePoints;
   P.SetCoord((Standard_Real)XX/zoom,(Standard_Real)YY/zoom, 0.0);
   ThePoints.Append (P);
   mark = new Draw_Marker3D(P, Draw_X, Draw_orange);
   dout << mark;
   dout.Flush();
   i = 1;
      
   while (b != 3) {
     dout.Select(id,XX,YY,b, Standard_False);
     if (b == 1) { 
       i++;
       P.SetCoord( (Standard_Real)XX/zoom,
		  (Standard_Real)YY/zoom, 0.0);
       ThePoints.Append(P);
       mark = new Draw_Marker3D(P, Draw_X, Draw_orange);
       dout << mark;
       dout.Flush();
     }
   }

   MPC = new (AppDef_HArray1OfMultiPointConstraint)(1, ThePoints.Length());
   AppDef_MultiPointConstraint mpc(1,0);
   MPC->ChangeArray1().Init(mpc);
   for (i=1; i<=ThePoints.Length(); i++) {
      AppDef_MultiPointConstraint aLocalMpc(1,0);
      aLocalMpc.SetPoint(1, ThePoints.Value(i));
      MPC->SetValue(i, aLocalMpc);
   }  
 }
 
 else {
   // Cas du 2D -------
   Handle(Draw_Marker2D) mark;
   TColgp_SequenceOfPnt2d ThePoints;
   P2d.SetCoord((Standard_Real)XX/zoom,(Standard_Real)YY/zoom);
   ThePoints.Append(P2d);
   mark = new Draw_Marker2D(P2d, Draw_X, Draw_orange);
   dout << mark;
   dout.Flush();
   i = 1;
      
   while (b != 3) {
     dout.Select(id,XX,YY,b, Standard_False);
     
     if (b == 1) { 
       i++;
       P2d.SetCoord( (Standard_Real)XX/zoom, (Standard_Real)YY/zoom );
       ThePoints.Append (P2d);
       mark = new Draw_Marker2D(P2d, Draw_X, Draw_orange);
       dout << mark;
       dout.Flush();
     }
   }

   MPC = new (AppDef_HArray1OfMultiPointConstraint)(1, ThePoints.Length());
   for (i=1; i<=ThePoints.Length(); i++) {
     AppDef_MultiPointConstraint mpc(0,1);
     mpc.SetPoint2d(1, ThePoints.Value(i));
     MPC->SetValue(i,  mpc);
   }
 }
 return id;
}

//=======================================================================
//function : PointsByFile
//=======================================================================
static void PointsByFile(Handle(AppDef_HArray1OfMultiPointConstraint)& MPC,
			 Handle(AppParCurves_HArray1OfConstraintCouple)& TABofCC,
			 ifstream& iFile,
			 Draw_Interpretor& di)
{
  Standard_Integer nbp, i, nbc;
  char c;
  Standard_Real x, y, z;

  iFile >> nbp;
  char dimen[3];
  iFile >> dimen;

  if (!strcmp(dimen,"3d")) {
    Handle(Draw_Marker3D) mark;
    MPC = new (AppDef_HArray1OfMultiPointConstraint)(1, nbp);

    for (i = 1; i <= nbp; i++) {
      iFile >> x >> y >> z;
      AppDef_MultiPointConstraint mpc(1,0);
      mpc.SetPoint(1, gp_Pnt(x, y, z));
      MPC->SetValue(i,mpc);
      mark = new Draw_Marker3D(gp_Pnt(x, y, z), Draw_X, Draw_orange);
      dout << mark;
    }
    Standard_Boolean HasConstrainte = Standard_False;
    if (iFile.get(c)) {
      if ( IsControl( (Standard_Character)c) ) {   
	if (iFile.get(c))  HasConstrainte = Standard_True;
      }
      else  HasConstrainte = Standard_True;
    }

    if (HasConstrainte) {
      Standard_Integer num, ordre;
      iFile >> nbc;
      if ((nbc < 1) || (nbc>nbp)) return; // Y a comme un probleme
      AppParCurves_Constraint  Constraint = AppParCurves_NoConstraint;
      TABofCC = new AppParCurves_HArray1OfConstraintCouple(1, nbp);
      for(i=1; i<=nbp; i++){
	AppParCurves_ConstraintCouple ACC(i,Constraint);
	TABofCC->SetValue(i,ACC);
      }
      for(i=1; i<=nbc; i++) {
	iFile >> num >> ordre;
	if ((num<1)||(num>nbp)) {
	  di << "Error on point Index in constrainte\n";
	  return;
	}
	Constraint = (AppParCurves_Constraint) (ordre+1);
	TABofCC->ChangeValue(num).SetConstraint(Constraint);
	if (Constraint >=  AppParCurves_TangencyPoint) {
	  iFile >> x >> y >> z;
	  MPC->ChangeValue(num).SetTang(1, gp_Vec(x,y,z));
	}
	if (Constraint >=  AppParCurves_CurvaturePoint) {
	  iFile >> x >> y >> z;
	  MPC->ChangeValue(num).SetCurv(1, gp_Vec(x,y,z));
	}
      }
    }
    
  }
  else if (!strcmp(dimen,"2d")) {
    Handle(Draw_Marker2D) mark;
    MPC = new (AppDef_HArray1OfMultiPointConstraint)(1, nbp);

    for (i = 1; i <= nbp; i++) {
      iFile >> x >> y;
      AppDef_MultiPointConstraint mpc(0,1);
      mpc.SetPoint2d(1, gp_Pnt2d(x, y));
      MPC->SetValue(i, mpc);
      mark = new Draw_Marker2D(gp_Pnt2d(x, y), Draw_X, Draw_orange);
      dout << mark;
    }

    Standard_Boolean HasConstrainte = Standard_False;
    if (iFile.get(c)) {
      if ( IsControl( (Standard_Character)c) )  {
	if (iFile.get(c))  HasConstrainte = Standard_True;
      }
      else  HasConstrainte = Standard_True;
    }

    if (HasConstrainte) {
      Standard_Integer num, ordre;
      iFile >> nbc;
      if ((nbc < 1) || (nbc>nbp)) return; // Y a comme un probleme
      AppParCurves_Constraint  Constraint = AppParCurves_NoConstraint;
      TABofCC = new AppParCurves_HArray1OfConstraintCouple(1, nbp);
      for(i=1; i<=nbp; i++){
	AppParCurves_ConstraintCouple ACC(i,Constraint);
	TABofCC->SetValue(i,ACC);
      }
      for(i=1; i<=nbc; i++) {
	iFile >> num >> ordre;
	if ((num<1)||(num>nbp)) {
	  di << "Error on point Index in constrainte\n";
	  return;
	}
	Constraint = (AppParCurves_Constraint) (ordre+1);
	TABofCC->ChangeValue(num).SetConstraint(Constraint);
	if (Constraint >=  AppParCurves_TangencyPoint) {
	  iFile >> x >> y;
	  MPC->ChangeValue(num).SetTang2d(1, gp_Vec2d(x,y));
	}
	if (Constraint >=  AppParCurves_CurvaturePoint) {
	  iFile >> x >> y;
	  MPC->ChangeValue(num).SetCurv2d(1, gp_Vec2d(x,y));
	}
      }
    }
  }  
}



//==================================================================================
static Standard_Integer smoothing (Draw_Interpretor& di,Standard_Integer n, const char** a)
//==================================================================================
//  Tolerance < 0 lissage "filtre"
//  Tolerance > 0 lissage avec respect de l'erreur max
//  Tolerance = 0 interpolation.
//
{
  Standard_Real Tolerance=0;

  AppParCurves_Constraint  Constraint=AppParCurves_NoConstraint;

  Handle(AppParCurves_HArray1OfConstraintCouple)TABofCC;
  TABofCC.Nullify();
  Handle(AppDef_HArray1OfMultiPointConstraint) Points;
  Standard_Integer id = 0, DegMax = -1;

  if (n == 1) {
    di <<"give a name to your curve !\n";
    return 0;
  }
  if (n == 2) {
    di <<"give a tolerance to your curve !\n";
    return 0;
  }
  if (n == 3) {
    Tolerance = Draw::Atof(a[2]);
    if (Abs(Tolerance) < Precision::Confusion()*1.e-7)  {
      Constraint = AppParCurves_PassPoint;
    }
    else {
      Constraint = AppParCurves_NoConstraint;
    } 
    // Designation Graphique ------------------------
    id = PointsByPick(Points, di);
  }
  else if (n >= 4) {
    Standard_Integer ific = 3;
    Tolerance = Draw::Atof(a[2]);
    if (Abs(Tolerance) < Precision::Confusion()*1.e-7)  {
      Constraint = AppParCurves_PassPoint;
    }
    else {
      Constraint = AppParCurves_NoConstraint;
    }

    if (! strcmp(a[3],"-D")) {
       DegMax = Draw::Atoi(a[4]);
       ific = 5;
    }
    
    if (n > ific) {
      // lecture du fichier.
    // nbpoints, 2d ou 3d, puis valeurs.
      const char* nomfic = a[ific];
      ifstream iFile(nomfic, ios::in);
      if (!iFile) { 
	di << a[ific] <<"do not exist !\n";
	return 1;
      }
      PointsByFile(Points, TABofCC, iFile, di);
    }
    else {
    // Designation Graphique
    id = PointsByPick(Points, di);
    }
  }

  AppDef_MultiLine AML(Points->Array1());

  // Compute --------------
  Standard_Integer i;
  if (Points->Value(1).NbPoints()==0){
    // Cas 2d
    Handle(TColgp_HArray1OfPnt2d) ThePoints;
    // Calcul du lissage
    Standard_Integer NbPoints = Points->Length();
    if (TABofCC.IsNull()) {
      TABofCC = new AppParCurves_HArray1OfConstraintCouple(1, NbPoints);
      for(i=1; i<=NbPoints; i++){
	AppParCurves_ConstraintCouple ACC(i,Constraint);
	TABofCC->SetValue(i,ACC);
      }
    }

    AppDef_Variational Variation(AML, 
				    1, NbPoints,
				    TABofCC);
    
    if (DegMax > 0) {
      if (DegMax < 3) Variation.SetContinuity(GeomAbs_C0);
      else if (DegMax <5) Variation.SetContinuity(GeomAbs_C1);
      Variation.SetMaxDegree(DegMax);
    }
    Variation.SetTolerance( Abs(Tolerance));
    if (Tolerance>0) { Variation.SetWithMinMax(Standard_True);}
    Variation.Approximate();

#  ifdef GEOMLITETEST_DEB
    //Variation.Dump(cout);
    Standard_SStream aSStream;
    Variation.Dump(aSStream);
    di << aSStream;
#   endif

    AppParCurves_MultiBSpCurve AnMuC = Variation.Value();

    TColgp_Array1OfPnt2d ThePoles (1,  AnMuC.NbPoles() ); 
    AnMuC.Curve(1, ThePoles);    
    Handle(Geom2d_BSplineCurve) Cvliss = new (Geom2d_BSplineCurve)
      (ThePoles,
       AnMuC.Knots(),
       AnMuC. Multiplicities(),
       AnMuC.Degree() );

    Handle(DrawTrSurf_BSplineCurve2d) 
      DC = new DrawTrSurf_BSplineCurve2d(Cvliss);
    DC->ClearPoles();
    Draw::Set(a[1], DC);
    if (id!=0) dout.RepaintView(id); 
  }
  else {
    Standard_Integer NbPoints = Points->Length();
    if (TABofCC.IsNull()) {
      TABofCC = new AppParCurves_HArray1OfConstraintCouple(1, NbPoints);
      for(i=1; i<=NbPoints; i++){
	AppParCurves_ConstraintCouple ACC(i,Constraint);
	TABofCC->SetValue(i,ACC);
      }
    } 

    AppDef_Variational Variation(AML, 
				    1,  NbPoints,
				    TABofCC);

    if (DegMax > 0) {
      if (DegMax < 3) Variation.SetContinuity(GeomAbs_C0);
      else if (DegMax <5) Variation.SetContinuity(GeomAbs_C1);
      Variation.SetMaxDegree(DegMax);
    }
    Variation.SetTolerance( Abs(Tolerance));
    if (Tolerance>0) { Variation.SetWithMinMax(Standard_True);}
    Variation.Approximate();
#     ifdef GEOMLITETEST_DEB
    //Variation.Dump(cout);
    Standard_SStream aSStream;
    Variation.Dump(aSStream);
    di << aSStream;
#     endif

    AppParCurves_MultiBSpCurve AnMuC = Variation.Value();

    TColgp_Array1OfPnt ThePoles (1,  AnMuC.NbPoles() ); 
    AnMuC.Curve(1, ThePoles);    
    Handle(Geom_BSplineCurve) Cvliss = new (Geom_BSplineCurve)
      (ThePoles,
       AnMuC.Knots(),
       AnMuC. Multiplicities(),
       AnMuC.Degree() );

    Handle(DrawTrSurf_BSplineCurve) 
      DC = new DrawTrSurf_BSplineCurve(Cvliss);
    DC->ClearPoles();
    Draw::Set(a[1], DC);
    if (id!=0) dout.RepaintView(id);
  }
  return 0;
}

//=============================================================================
static Standard_Integer smoothingbybezier (Draw_Interpretor& di,
					   Standard_Integer n, 
					   const char** a)
//============================================================================
{
  Standard_Real Tolerance=0;
  AppParCurves_Constraint Constraint = AppParCurves_NoConstraint;
  Handle(AppParCurves_HArray1OfConstraintCouple)TABofCC;
  Handle(AppDef_HArray1OfMultiPointConstraint) Points;

  Standard_Integer id = 0;
  Standard_Integer methode=0;
  Standard_Integer Degree = 8;

  if (n == 1) {
    di <<"give a name to your curve !\n";
    return 0;
  }
  if (n == 2) {
    di <<"give a tolerance to your curve !\n";
    return 0;
  }
 if (n == 3) {
    di <<"give a max degree!\n";
    return 0;
  }

  if (n == 4) {
    di <<"give an option!\n";
    return 0;
  }
  if (n >= 5) {
    Tolerance = Draw::Atof(a[2]);
    Degree = Draw::Atoi(a[3]);
    if (! strcmp(a[4],"-GR")) {
      methode = 1;
    }
    else if (! strcmp(a[4],"-PR")) {
      methode = 2;
    }
    else { methode = 3;}
    
    if (Abs(Tolerance) < Precision::Confusion()*1.e-7)  {
      Constraint = AppParCurves_PassPoint;
    }
    else {
      Constraint = AppParCurves_NoConstraint;
    } 
    if (n==5)
      // Designation Graphique ------------------------
     id = PointsByPick(Points, di);
    else {
      // lecture du fichier.
      // nbpoints, 2d ou 3d, puis valeurs.
      const char* nomfic = a[5];
      ifstream iFile(nomfic, ios::in);
      if (!iFile) { 
	di << a[6] <<"do not exist !\n";
	return 1;
      }
      PointsByFile(Points, TABofCC, iFile, di);
    }
  }
    
  AppDef_MultiLine AML(Points->Array1());  

  // Compute --------------
  Standard_Integer i;
  if (Points->Value(1).NbPoints()==0){
    // Cas 2d
    Handle(TColgp_HArray1OfPnt2d) ThePoints;
    // Calcul du lissage
    Standard_Integer NbPoints = Points->Length();
    if (TABofCC.IsNull()) {
      TABofCC = new AppParCurves_HArray1OfConstraintCouple(1, NbPoints);
      for(i=1; i<=NbPoints; i++){
	AppParCurves_ConstraintCouple ACC(i,Constraint);
	TABofCC->SetValue(i,ACC);
      }

      AppParCurves_ConstraintCouple AC1(1, AppParCurves_PassPoint);
      if (TABofCC->Value(1).Constraint()<AppParCurves_PassPoint)
	TABofCC->SetValue(1, AC1);
    
      AppParCurves_ConstraintCouple AC2(NbPoints, AppParCurves_PassPoint);
      if (TABofCC->Value(NbPoints).Constraint()<AppParCurves_PassPoint)
	TABofCC->SetValue(NbPoints, AC2);
    }

    if (methode < 3) {
      Standard_Boolean mySquare = (methode == 2);
      Standard_Integer degmin = 4;
      Standard_Integer NbIteration = 5;
      
      if (Degree < 4) degmin = Max(1, Degree -1);
      degmin = Max(degmin, NbConstraint(TABofCC->Value(1).Constraint(),  
					TABofCC->Value(NbPoints).Constraint()) );
      
      AppDef_Compute Appr(degmin, Degree,
			  Abs(Tolerance),  Abs(Tolerance),
			  NbIteration, Standard_False, 
			  Approx_ChordLength, mySquare);

     Appr.SetConstraints(TABofCC->Value(1).Constraint(), 
			  TABofCC->Value(NbPoints).Constraint());
   
      Appr.Perform (AML);
      
      if (! Appr.IsAllApproximated()) {
	di << " No result\n";
      }
      AppParCurves_MultiCurve AnMuC = Appr.Value();
      ThePoints = new (TColgp_HArray1OfPnt2d) (1,  AnMuC.NbPoles() );
      AnMuC.Curve(1, ThePoints->ChangeArray1());
      Standard_Real err, err2d;
      Appr.Error(1, err, err2d);
      di <<" Error2D is : " << err2d << "\n";
    }
    else {
      AppDef_Variational Varia(AML, 
				  1,  NbPoints,
				  TABofCC,
				  Degree, 1);
      Varia.SetTolerance(Abs(Tolerance));
      Varia.Approximate();

      if (! Varia.IsDone()) {
	di << " No result\n";
      }

      AppParCurves_MultiBSpCurve  AnMuC = Varia.Value();
      di <<" Error2D is : " << Varia.MaxError() << "\n";
      ThePoints = new (TColgp_HArray1OfPnt2d) (1,  AnMuC.NbPoles() );
      AnMuC.Curve(1, ThePoints->ChangeArray1());    
 }
    
    Handle(Geom2d_BezierCurve) Cvliss = 
      new (Geom2d_BezierCurve)(ThePoints->Array1());

    Handle(DrawTrSurf_BezierCurve2d) DC = 
      new (DrawTrSurf_BezierCurve2d) (Cvliss);
    Draw::Set(a[1], DC);
    if (id!=0) dout.RepaintView(id); 
  }
  else {
    // Cas 3d
    Handle(TColgp_HArray1OfPnt) ThePoints;
    Standard_Integer NbPoints = Points->Length();
    if (TABofCC.IsNull()) {
      TABofCC = new AppParCurves_HArray1OfConstraintCouple(1, NbPoints);
      for(i=1; i<=NbPoints; i++){
	AppParCurves_ConstraintCouple ACC(i,Constraint);
	TABofCC->SetValue(i,ACC);
      }
    

      AppParCurves_ConstraintCouple AC1(1, AppParCurves_PassPoint);
      if (TABofCC->Value(1).Constraint()<AppParCurves_PassPoint)
	TABofCC->SetValue(1, AC1);
    
      AppParCurves_ConstraintCouple AC2(NbPoints, AppParCurves_PassPoint);
      if (TABofCC->Value(NbPoints).Constraint()<AppParCurves_PassPoint)
	TABofCC->SetValue(NbPoints, AC2);
    }

    if (methode < 3) {
      Standard_Boolean mySquare = (methode == 2);
      Standard_Integer degmin = 4;
      Standard_Integer NbIteration = 5;
      if (Degree < 4) degmin = Max(1, Degree - 1);
      degmin = Max(degmin, NbConstraint(TABofCC->Value(1).Constraint(),  
					TABofCC->Value(NbPoints).Constraint()) );
      
      AppDef_Compute Appr(degmin, Degree,
			  Abs(Tolerance),  Abs(Tolerance),
			  NbIteration, Standard_False, 
			  Approx_ChordLength, mySquare);

      Appr.SetConstraints(TABofCC->Value(1).Constraint(), 
			  TABofCC->Value(NbPoints).Constraint());
   
      Appr.Perform (AML);
      
      if (! Appr.IsAllApproximated()) {
	di << " No result\n";
      }
      AppParCurves_MultiCurve AnMuC = Appr.Value();
      ThePoints = new (TColgp_HArray1OfPnt) (1,  AnMuC.NbPoles() );
      AnMuC.Curve(1, ThePoints->ChangeArray1());
      Standard_Real err, err2d;
      Appr.Error(1, err, err2d);
      di <<" Error3D is : " << err << "\n";
    }
    else {
      AppDef_Variational Varia(AML, 
				  1,  NbPoints,
				  TABofCC,
				  Degree, 1);

      Varia.SetTolerance(Abs(Tolerance));
      Varia.Approximate();
      if (! Varia.IsDone()) {
	di << " No result\n";
      }

      AppParCurves_MultiBSpCurve  AnMuC = Varia.Value();
      di <<" Error3D is : " << Varia.MaxError() << "\n";
      ThePoints = new (TColgp_HArray1OfPnt) (1,  AnMuC.NbPoles() );
      AnMuC.Curve(1, ThePoints->ChangeArray1());    
    }    

    Handle(Geom_BezierCurve) Cvliss = 
      new (Geom_BezierCurve)(ThePoints->Array1());

    Handle(DrawTrSurf_BezierCurve) 
      DC = new DrawTrSurf_BezierCurve(Cvliss);
    Draw::Set(a[1], DC);
    if (id!=0) dout.RepaintView(id);
  }
  return 0;
}

//=======================================================================
//function : ConstraintCommands
//purpose  : 
//=======================================================================


void  GeomliteTest::ApproxCommands(Draw_Interpretor& theCommands)
{

  static Standard_Boolean loaded = Standard_False;
  if (loaded) return;
  loaded = Standard_True;

  DrawTrSurf::BasicCommands(theCommands);

  const char* g;
  // constrained constructs
  g = "GEOMETRY Constraints";


  theCommands.Add("bsmooth",
		  "bsmooth cname tol [-D degree] [fic]", 
		  __FILE__,
		  smoothing, g);

  theCommands.Add("bzsmooth",
		  "bzsmooth cname tol degree option [fic]", 
		  __FILE__,
		  smoothingbybezier, g);
}

