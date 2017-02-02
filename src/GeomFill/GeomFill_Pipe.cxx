// Created on: 1994-04-13
// Created by: Joelle CHAUVET
// Copyright (c) 1994-1999 Matra Datavision
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

// Modified 22/09/1997 by PMN : Refonte du a l'introduction de F(t) dans
//             le cas des 2 lignes guides
// Modified:	Mon Jan 18 11:06:46 1999
//		dans Init(Path, Nsections) : 
//              les parametres des sections doivent etre strict. croissants
//		dans Init(Path, FirstSect, LastSect) :
//		il faut placer les 2 sections au debut de la trajectoire

#include <Adaptor3d_CurveOnSurface.hxx>
#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_HCurveOnSurface.hxx>
#include <Approx_SweepApproximation.hxx>
#include <ElCLib.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Circle.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomFill_AppSweep.hxx>
#include <GeomFill_CircularBlendFunc.hxx>
#include <GeomFill_ConstantBiNormal.hxx>
#include <GeomFill_CorrectedFrenet.hxx>
#include <GeomFill_CurveAndTrihedron.hxx>
#include <GeomFill_Darboux.hxx>
#include <GeomFill_Fixed.hxx>
#include <GeomFill_Frenet.hxx>
#include <GeomFill_GuideTrihedronAC.hxx>
#include <GeomFill_GuideTrihedronPlan.hxx>
#include <GeomFill_Line.hxx>
#include <GeomFill_LocationGuide.hxx>
#include <GeomFill_LocationLaw.hxx>
#include <GeomFill_NSections.hxx>
#include <GeomFill_Pipe.hxx>
#include <GeomFill_Profiler.hxx>
#include <GeomFill_SectionLaw.hxx>
#include <GeomFill_SectionPlacement.hxx>
#include <GeomFill_Sweep.hxx>
#include <GeomFill_SweepSectionGenerator.hxx>
#include <GeomFill_UniformSection.hxx>
#include <GeomLib.hxx>
#include <GeomLProp_CLProps.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Torus.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <TColGeom_SequenceOfCurve.hxx>
#include <TColgp_Array1OfPnt.hxx>

#include <stdio.h>
#ifdef OCCT_DEBUG
static Standard_Boolean Affich = Standard_False;
static Standard_Integer NbSections = 0;
#endif

#ifdef DRAW
#include <DrawTrSurf.hxx>
#include <Geom_Curve.hxx>
#endif

static Standard_Boolean CheckSense(const TColGeom_SequenceOfCurve& Seq1,
				         TColGeom_SequenceOfCurve& Seq2)
{
  // initialisation
  Standard_Boolean no_sing = Standard_True;
  Seq2.Clear();

  Handle(Geom_Curve) C1 = Seq1.Value(1);
  Standard_Real f = C1->FirstParameter(), l = C1->LastParameter();
  Standard_Integer iP, NP = 21;
  TColgp_Array1OfPnt Tab(1,NP);
  Standard_Real u = f, h = Abs(f-l) / 20.;
  for (iP=1;iP<=NP;iP++) {
    C1->D0(u,Tab(iP));
    u += h;
    if ((u-f)*(u-l)>0.0) u = l;
  }
  gp_Ax2 AxeRef, Axe;
  gp_Pnt Pos;
  Standard_Boolean sing;
  GeomLib::AxeOfInertia(Tab,AxeRef,sing);

  // si la section est une droite, ca ne marche pas
  if (sing) no_sing = Standard_False;

  Pos = AxeRef.Location();
  Standard_Real alpha1, alpha2, alpha3;
  gp_Pnt P1, P2;
  u = (f+l-h)/2 - h;
  C1->D0(u,P1);
  u += h;
  C1->D0(u,P2);
  alpha1 = gp_Vec(Pos,P1).AngleWithRef(gp_Vec(Pos,P2),AxeRef.Direction());
  P1 = P2;
  u += h;
  C1->D0(u,P2);
  alpha2 = gp_Vec(Pos,P1).AngleWithRef(gp_Vec(Pos,P2),AxeRef.Direction());
  P1 = P2;
  u += h;
  C1->D0(u,P2);
  alpha3 = gp_Vec(Pos,P1).AngleWithRef(gp_Vec(Pos,P2),AxeRef.Direction());
  Seq2.Append(C1);

  for (Standard_Integer iseq=2; iseq<=Seq1.Length(); iseq++) {
    // discretisation de C2
    Handle(Geom_Curve) C2 = Seq1.Value(iseq);
    f = C2->FirstParameter();
    l = C2->LastParameter();
    u = f;
    for (iP=1;iP<=NP;iP++) {
      C2->D0(u,Tab(iP));
      u += h;
      if ((u-f)*(u-l)>0.0) u = l;
    }
    GeomLib::AxeOfInertia(Tab,Axe,sing);  

    // si la section est une droite, ca ne marche pas
    if (sing) no_sing = Standard_False;

    Pos = Axe.Location();
    Standard_Real beta1, beta2, beta3;
    u = (f+l-h)/2 - h;
    C2->D0(u,P1);
    u += h;
    C2->D0(u,P2);
    beta1 = gp_Vec(Pos,P1).AngleWithRef(gp_Vec(Pos,P2),AxeRef.Direction());
    P1 = P2;
    u += h;
    C2->D0(u,P2);
    beta2 = gp_Vec(Pos,P1).AngleWithRef(gp_Vec(Pos,P2),AxeRef.Direction());
    P1 = P2;
    u += h;
    C2->D0(u,P2);
    beta3 = gp_Vec(Pos,P1).AngleWithRef(gp_Vec(Pos,P2),AxeRef.Direction());
    
    // meme sens ?
    Standard_Boolean ok = Standard_True, 
                pasnul1 = ( Abs(alpha1) > Precision::Confusion() )
		          && ( Abs(beta1) > Precision::Confusion() ),
                pasnul2 = ( Abs(alpha2) > Precision::Confusion() )
		          && ( Abs(beta2) > Precision::Confusion() ),
                pasnul3 = ( Abs(alpha3) > Precision::Confusion() )
		          && ( Abs(beta3) > Precision::Confusion() );
    if (pasnul1 && pasnul2 && pasnul3) {
      if (alpha1*beta1>0.0) 
	ok = (alpha2*beta2>0.0) || (alpha3*beta3>0.0);
      else
	ok = (alpha2*beta2>0.0) && (alpha3*beta3>0.0);
    }
    else if (pasnul1 && pasnul2 && !pasnul3) 
      ok = (alpha1*beta1>0.0) || (alpha2*beta2>0.0);
    else if (pasnul1 && !pasnul2 && pasnul3) 
      ok = (alpha1*beta1>0.0) || (alpha3*beta3>0.0);
    else if (!pasnul1 && pasnul2 && pasnul3) 
      ok = (alpha2*beta2>0.0) || (alpha3*beta3>0.0);
    else if (pasnul1) 
	ok = (alpha1*beta1>0.0);
    else if (pasnul2) 
	ok = (alpha2*beta2>0.0);
    else if (pasnul3) 
	ok = (alpha3*beta3>0.0);
    
    if (no_sing && !ok) {
      C2 ->Reverse();
    }
    Seq2.Append(C2);
  }

  return no_sing;
}

//=======================================================================
//function : GeomFill_Pipe
//purpose  : constructor with no parameters. 
//=======================================================================

GeomFill_Pipe::GeomFill_Pipe() : myIsDone(Standard_False),myExchUV(Standard_False),myKPart(Standard_False)
{
  Init();
}


//=======================================================================
//function : GeomFill_Pipe
//purpose  : 
//=======================================================================

GeomFill_Pipe::GeomFill_Pipe(const Handle(Geom_Curve)& Path, 
                             const Standard_Real Radius) 
     : myIsDone(Standard_False),myExchUV(Standard_False),myKPart(Standard_False)
{
  Init();
  Init(Path, Radius);
}

//=======================================================================
//function : GeomFill_Pipe
//purpose  : 
//=======================================================================

GeomFill_Pipe::GeomFill_Pipe(const Handle(Geom_Curve)& Path,
                             const Handle(Geom_Curve)& FirstSect,
                             const GeomFill_Trihedron Option) 
     : myIsDone(Standard_False),myExchUV(Standard_False),myKPart(Standard_False)
{
  Init();
  Init(Path, FirstSect, Option);
}

//=======================================================================
//function : GeomFill_Pipe
//purpose  : 
//=======================================================================

GeomFill_Pipe::GeomFill_Pipe(const Handle(Geom2d_Curve)& Path,
                             const Handle(Geom_Surface)& Support,
                             const Handle(Geom_Curve)& FirstSect) 
     : myIsDone(Standard_False),myExchUV(Standard_False),myKPart(Standard_False)
{
  Init();
  Init(Path, Support, FirstSect);
}

//=======================================================================
//function : GeomFill_Pipe
//purpose  : 
//=======================================================================

GeomFill_Pipe::GeomFill_Pipe(const Handle(Geom_Curve)& Path,
                             const Handle(Geom_Curve)& FirstSect,
                             const Handle(Geom_Curve)& LastSect)
     : myIsDone(Standard_False),myExchUV(Standard_False),myKPart(Standard_False)
{
  Init();
  Init(Path, FirstSect, LastSect);
}


//=======================================================================
//function : GeomFill_Pipe
//purpose  : 
//=======================================================================

GeomFill_Pipe::GeomFill_Pipe(const Handle(Geom_Curve)& Path,
                             const TColGeom_SequenceOfCurve& NSections)
     : myIsDone(Standard_False),myExchUV(Standard_False),myKPart(Standard_False)
{
  Init();
  Init(Path, NSections);
}

//=======================================================================
//function : GeomFill_Pipe
//purpose  : 
//=======================================================================

GeomFill_Pipe::GeomFill_Pipe(const Handle(Geom_Curve)& Path,
                             const Handle(Geom_Curve)& Curve1,
                             const gp_Dir& Direction)
     : myIsDone(Standard_False),myExchUV(Standard_False), myKPart(Standard_False)
{
 Init(Path, Curve1, Direction); 
}
//=======================================================================
//function : GeomFill_Pipe
//purpose  : 
//=======================================================================

GeomFill_Pipe::GeomFill_Pipe(const Handle(Geom_Curve)& Path,
                             const Handle(Geom_Curve)& Curve1,
                             const Handle(Geom_Curve)& Curve2,
                             const Standard_Real       Radius)
     : myIsDone(Standard_False),myExchUV(Standard_False),myKPart(Standard_False)
{
  Init();
  Handle(GeomAdaptor_HCurve) AdpPath = 
    new GeomAdaptor_HCurve( Path);
  Handle(GeomAdaptor_HCurve) AdpCurve1 = 
    new GeomAdaptor_HCurve( Curve1);
  Handle(GeomAdaptor_HCurve) AdpCurve2 = 
    new GeomAdaptor_HCurve( Curve2);

  Init(AdpPath, AdpCurve1, AdpCurve2, Radius);
}


//=======================================================================
//function : GeomFill_Pipe
//purpose  : 
//=======================================================================

GeomFill_Pipe::GeomFill_Pipe(const Handle(Adaptor3d_HCurve)& Path, 
                             const Handle(Adaptor3d_HCurve)& Curve1,
                             const Handle(Adaptor3d_HCurve)& Curve2,
                             const Standard_Real Radius)
     : myIsDone(Standard_False),myExchUV(Standard_False),myKPart(Standard_False)
{
  Init();
  Init(Path,Curve1,Curve2,Radius);
}


//=======================================================================


//=======================================================================
//function : GeomFill_Pipe
//purpose  : pipe avec courbe guide 
//=======================================================================

GeomFill_Pipe::GeomFill_Pipe(const Handle(Geom_Curve)& Path,
                             const Handle(Adaptor3d_HCurve)& Guide,
			     const Handle(Geom_Curve)& FirstSect,
			     const Standard_Boolean byACR,
			     const Standard_Boolean rotat)
     : myIsDone(Standard_False),myExchUV(Standard_False),myKPart(Standard_False)
// Path : trajectoire
// Guide : courbe guide
// FirstSect : section
// NbPt : nb de points pour le calcul du triedre
{
  Init();
  Init(Path, Guide, FirstSect, byACR, rotat);
}



//=======================================================================
//function : Init
//purpose  : pipe avec courbe guide 
//=======================================================================

void GeomFill_Pipe::Init(const Handle(Geom_Curve)& Path,
			 const Handle(Adaptor3d_HCurve)& Guide,
			 const Handle(Geom_Curve)& FirstSect,
			 const Standard_Boolean byACR,
			 const Standard_Boolean rotat)
// Path : trajectoire
// Guide : courbe guide
// FirstSect : section
// rotat : vrai si on veu la rotation false sinon
// triedre : AC pour absc. curv. ou P pour plan ortho
{
  Standard_Real angle;
  myAdpPath = new (GeomAdaptor_HCurve) 
    (Handle(Geom_Curve)::DownCast(Path->Copy()));

  Handle (GeomFill_TrihedronWithGuide) TLaw;
  
  // loi de triedre
  if (byACR) {
    TLaw = new (GeomFill_GuideTrihedronAC)(Guide);
    TLaw->SetCurve(myAdpPath);
  }
  else {
    TLaw =  new (GeomFill_GuideTrihedronPlan)(Guide);
    TLaw->SetCurve(myAdpPath);      
  }  
  

// loi de positionnement
  Handle(GeomFill_LocationGuide) TheLoc = 
    new (GeomFill_LocationGuide) (TLaw);
  TheLoc->SetCurve(myAdpPath);

  GeomFill_SectionPlacement Place(TheLoc, FirstSect);
  Place.Perform(Precision::Confusion());

// loi de section
  mySec = new (GeomFill_UniformSection) (Place.Section(Standard_False),
					 myAdpPath->FirstParameter(), 
					 myAdpPath->LastParameter());

#ifdef DRAW
  if  (Affich) {
    char* Temp = "TheSect" ;
    DrawTrSurf::Set(Temp, FirstSect );
//    DrawTrSurf::Set("TheSect", FirstSect );
  }
#endif

 if (rotat)  TheLoc->Set(mySec,rotat, 
			 myAdpPath->FirstParameter(),
			 myAdpPath->LastParameter(),
			 0., angle);
 myLoc = TheLoc;
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void GeomFill_Pipe::Init()
{
  myType = 0;
  myError  = 0;
  myRadius = 0.;
  myKPart = Standard_True;
  myPolynomial = Standard_False;
  myAdpPath.Nullify();
  myAdpFirstSect.Nullify();
  myAdpLastSect.Nullify();
  myLoc.Nullify();
  mySec.Nullify(); 
}
//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void GeomFill_Pipe::Init(const Handle(Geom_Curve)& Path,
                         const Standard_Real       Radius)
{
// Ancienne methode
  myType   = 1;
  myError  = 0;
  myRadius = Radius;

// Nouvelle methode
  myAdpPath = new (GeomAdaptor_HCurve) (Path);
  Handle(Geom_Circle) C = new (Geom_Circle) (gp::XOY(), Radius);
  C->Rotate(gp::OZ(),M_PI/2.);
  
  mySec = new (GeomFill_UniformSection) (C, Path->FirstParameter(), 
					    Path->LastParameter());
  Handle (GeomFill_CorrectedFrenet)TLaw = new (GeomFill_CorrectedFrenet) ();
  myLoc = new (GeomFill_CurveAndTrihedron) (TLaw);
  myLoc->SetCurve(myAdpPath);

#ifdef DRAW
  if  (Affich) {
    char* Temp = "TheSect" ;
    DrawTrSurf::Set(Temp, C);   
//    DrawTrSurf::Set("TheSect", C);   
  }
#endif

}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void GeomFill_Pipe::Init(const Handle(Geom_Curve)& Path,
			 const Handle(Geom_Curve)& FirstSect,
			 const GeomFill_Trihedron Option)
{
  Handle(Geom_Curve) Sect;
  Handle(GeomFill_TrihedronLaw) TLaw;
  myAdpPath = new (GeomAdaptor_HCurve) 
    (Handle(Geom_Curve)::DownCast(Path->Copy()));
  Standard_Real param =  Path->FirstParameter();

// Construction de la loi de triedre
  switch (Option) {
  case GeomFill_IsCorrectedFrenet :
    {
      TLaw = new (GeomFill_CorrectedFrenet) ();
      break;
    }

  case GeomFill_IsDarboux :
#ifdef OCCT_DEBUG
    {
      cout << "Option Darboux: non realisable" << endl; 
    }
#endif
  case GeomFill_IsFrenet :
    {
      TLaw = new (GeomFill_Frenet) ();
      break; 
    }

  case GeomFill_IsFixed :
    {     
      Standard_Real Eps = 1.e-9;
      gp_Vec V1(0,0,1), V2(0,1,0);
      gp_Dir D;
      GeomLProp_CLProps CP(Path, param, 2, Eps);
      if (CP.IsTangentDefined()) {
	CP.Tangent(D);
	V1.SetXYZ(D.XYZ());
	V1.Normalize();
	if (CP.Curvature() > Eps) {
	  CP.Normal(D);
	  V2.SetXYZ(D.XYZ());
	  V2.Normalize();
	}
	else {
	  gp_Pnt P0(0., 0., 0.);
	  gp_Ax2 Axe(P0, D);
	  D = Axe.XDirection ();
	  V2.SetXYZ(D.XYZ());
	  V2.Normalize();
	}
      }
      TLaw = new (GeomFill_Fixed) (V1, V2);
      break;
    }

  case GeomFill_IsConstantNormal :
    {     
      TLaw = new (GeomFill_Frenet) ();
      myLoc = new (GeomFill_CurveAndTrihedron) (TLaw);
      myLoc->SetCurve(myAdpPath);
      GeomFill_SectionPlacement Place(myLoc, FirstSect);
      Place.Perform(Precision::Confusion());
      Standard_Real ponsec = Place.ParameterOnSection();
      
      Standard_Real Eps = 1.e-9;
      gp_Vec V(0,1,0);
      gp_Dir D;
      GeomLProp_CLProps CP(FirstSect, ponsec, 2, Eps);
      if (CP.IsTangentDefined()) {
        CP.Tangent(D);
        if (CP.Curvature() > Eps) {
          CP.Normal(D);
          V.SetXYZ(D.XYZ());
          V.Normalize();
        }
        else {
          gp_Pnt P0(0., 0., 0.);
          gp_Ax2 Axe(P0, D);
          D = Axe.XDirection ();
          V.SetXYZ(D.XYZ());
          V.Normalize();
        }
      }
      TLaw = new (GeomFill_ConstantBiNormal) (V);
      break;
    }

  default :
    {
      throw Standard_ConstructionError("GeomFill::Init : Unknown Option");
    }
  }
 
  if (!TLaw.IsNull()) {
    myLoc = new (GeomFill_CurveAndTrihedron) (TLaw);
    myLoc->SetCurve(myAdpPath);
    GeomFill_SectionPlacement Place(myLoc, FirstSect);
    Place.Perform(Precision::Confusion());
    param =  Place.ParameterOnPath();
    Sect = Place.Section(Standard_False);

#ifdef DRAW
    if  (Affich) {
      char* Temp = "TheSect" ;
      DrawTrSurf::Set(Temp,Sect);
//      DrawTrSurf::Set("TheSect",Sect);
      
    }
#endif
    mySec = new (GeomFill_UniformSection) (Sect, 
					   Path->FirstParameter(), 
					   Path->LastParameter());
  }
}

//=======================================================================
//function : Init
//purpose  : sweep using Darboux's trihedron
//=======================================================================

void GeomFill_Pipe::Init(const Handle(Geom2d_Curve)& Path,
                         const Handle(Geom_Surface)& Support,
                         const Handle(Geom_Curve)& FirstSect)
{
  Handle(Geom_Curve) Sect;
  Handle(GeomFill_TrihedronLaw) TLaw = new (GeomFill_Darboux)();
  myAdpPath = 
    new Adaptor3d_HCurveOnSurface(Adaptor3d_CurveOnSurface(
		       new Geom2dAdaptor_HCurve(Path), 
		       new GeomAdaptor_HSurface(Support)));
 
  myLoc = new (GeomFill_CurveAndTrihedron) (TLaw);
  myLoc->SetCurve(myAdpPath);
  GeomFill_SectionPlacement Place(myLoc, FirstSect);
  Place.Perform(myAdpPath, Precision::Confusion());
  Sect = Place.Section(Standard_False);
  
#ifdef DRAW
  if  (Affich) {
    char* temp = "TheSect" ;
    DrawTrSurf::Set(temp,Sect);
//    DrawTrSurf::Set("TheSect",Sect);
    
  }
#endif
  mySec = new (GeomFill_UniformSection) (Sect, 
					 myAdpPath->FirstParameter(), 
					 myAdpPath->LastParameter());  
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void GeomFill_Pipe::Init(const Handle(Geom_Curve)& Path, 
			 const Handle(Geom_Curve)& FirstSect,
			 const gp_Dir& Direction)
{
  Init();

  Handle(Geom_Curve) Sect;
  myAdpPath = new (GeomAdaptor_HCurve) 
    (Handle(Geom_Curve)::DownCast(Path->Copy()));
  gp_Vec V;
  V.SetXYZ(Direction.XYZ());
  Handle (GeomFill_ConstantBiNormal) TLaw = 
    new (GeomFill_ConstantBiNormal) (V);

  myLoc = new (GeomFill_CurveAndTrihedron) (TLaw);
  myLoc->SetCurve(myAdpPath);
  GeomFill_SectionPlacement Place(myLoc, FirstSect);
  Place.Perform(Precision::Confusion());
  Sect = Place.Section(Standard_False);

#ifdef DRAW
  if  (Affich) {
    char* temp = "TheSect" ;
    DrawTrSurf::Set(temp,Sect);    
//    DrawTrSurf::Set("TheSect",Sect);    
  }
#endif
  mySec = new (GeomFill_UniformSection) (Sect, 
					 Path->FirstParameter(), 
					 Path->LastParameter());
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void GeomFill_Pipe::Init(const Handle(Geom_Curve)& Path, 
			 const TColGeom_SequenceOfCurve& NSections)
{
  myType      = 3;
  myError  = 0;
  myRadius    = 0;

  Handle(GeomFill_TrihedronLaw) TLaw;
  TLaw = new (GeomFill_CorrectedFrenet) ();
  myAdpPath = new (GeomAdaptor_HCurve) 
    (Handle(Geom_Curve)::DownCast(Path->Copy()));
  if (!TLaw.IsNull()) {
    myLoc = new (GeomFill_CurveAndTrihedron) (TLaw);
    myLoc->SetCurve(myAdpPath);
    TColGeom_SequenceOfCurve SeqC;
    TColStd_SequenceOfReal SeqP;
    SeqC.Clear();
    SeqP.Clear();
    Standard_Integer i ;
    for ( i = 1; i<=NSections.Length(); i++) {
      GeomFill_SectionPlacement Place(myLoc, NSections(i));
      Place.Perform(Precision::Confusion());
      SeqP.Append(Place.ParameterOnPath());
      SeqC.Append(Place.Section(Standard_False));
    }

    // verification des orientations
    TColGeom_SequenceOfCurve NewSeq;
    if (CheckSense(SeqC,NewSeq)) SeqC = NewSeq;

    // verification des parametres
    Standard_Boolean play_again = Standard_True;
    while (play_again) {
      play_again = Standard_False;
      for (i = 1; i<=NSections.Length(); i++) {
	for (Standard_Integer j = i; j<=NSections.Length(); j++) {
	  if (SeqP.Value(i)>SeqP.Value(j)) {
	    SeqP.Exchange(i,j);
	    SeqC.Exchange(i,j);
	    play_again = Standard_True;
	  }
	}
      }
    }
    for ( i = 1; i<NSections.Length(); i++) {
      if ( Abs(SeqP.Value(i+1)-SeqP.Value(i)) < Precision::PConfusion()) {
       throw Standard_ConstructionError("GeomFill_Pipe::Init with NSections : invalid parameters");
      }
    }
    
    // creation de la NSections
    Standard_Real first = Path->FirstParameter(),
                  last = Path->LastParameter();
    Standard_Real deb,fin;
    deb = SeqC.First()->FirstParameter();
    fin = SeqC.First()->LastParameter();
    mySec = new (GeomFill_NSections) (SeqC,SeqP,deb,fin,first,last);
  }
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void GeomFill_Pipe::Init(const Handle(Geom_Curve)& Path, 
			 const Handle(Geom_Curve)& FirstSect,
			 const Handle(Geom_Curve)& LastSect)
{
  myType      = 3;
  myError  = 0;
  myRadius    = 0;
  Standard_Real first = Path->FirstParameter(),
                last = Path->LastParameter();
  Handle(GeomFill_TrihedronLaw) TLaw;
  TLaw = new (GeomFill_CorrectedFrenet) ();
  myAdpPath = new (GeomAdaptor_HCurve) 
    (Handle(Geom_Curve)::DownCast(Path->Copy()));

  if (!TLaw.IsNull()) {
    myLoc = new (GeomFill_CurveAndTrihedron) (TLaw);
    myLoc->SetCurve(myAdpPath);
    TColGeom_SequenceOfCurve SeqC;
    TColStd_SequenceOfReal SeqP;
    SeqC.Clear();
    SeqP.Clear();
    // sequence des sections
    GeomFill_SectionPlacement Pl1(myLoc, FirstSect);
    Pl1.Perform(first,Precision::Confusion());
    SeqC.Append(Pl1.Section(Standard_False));
    GeomFill_SectionPlacement Pl2(myLoc, LastSect);
    Pl2.Perform(first,Precision::Confusion());
    SeqC.Append(Pl2.Section(Standard_False));
    // sequence des parametres associes
    SeqP.Append(first);
    SeqP.Append(last);

    // verification de l'orientation
    TColGeom_SequenceOfCurve NewSeq;
    if (CheckSense(SeqC,NewSeq)) SeqC = NewSeq;

    // creation de la NSections
    Standard_Real deb,fin;
    deb =  SeqC.First()->FirstParameter();
    fin =  SeqC.First()->LastParameter();
    mySec = new (GeomFill_NSections) (SeqC,SeqP,deb,fin,first,last);
  }
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void GeomFill_Pipe::Init(const Handle(Adaptor3d_HCurve)& Path, 
			 const Handle(Adaptor3d_HCurve)& Curve1,
			 const Handle(Adaptor3d_HCurve)& Curve2,
			 const Standard_Real           Radius)
{
  myType         = 4;
  myError        = 0;
  myRadius       = Radius;
  myAdpPath      = Path;
  myAdpFirstSect = Curve1;
  myAdpLastSect  = Curve2;
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void GeomFill_Pipe::Perform(const Standard_Boolean WithParameters, 
			    const Standard_Boolean Polynomial) 
{

  if ( (! myLoc.IsNull()) && (! mySec.IsNull()) ) {
    Perform(1.e-4, Polynomial);
    return;
  }

  myPolynomial = Polynomial;
  // on traite la cas tuyau sur arete Type = 4
  if ( myPolynomial) {
    ApproxSurf(WithParameters);
    return;
  }   
  if ( !KPartT4() ) ApproxSurf(WithParameters);

}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void GeomFill_Pipe::Perform(const Standard_Real Tol,
			    const Standard_Boolean Polynomial,
			    const GeomAbs_Shape Conti,
			    const Standard_Integer DegMax,
			    const Standard_Integer NbMaxSegment)
{
 GeomAbs_Shape TheConti;
 switch (Conti) {
 case GeomAbs_C0:
   {
     TheConti = GeomAbs_C0;
   } 
   break; 
 case GeomAbs_G1:
 case GeomAbs_C1:
   {
     TheConti = GeomAbs_C1;
   } 
   break;
 case GeomAbs_G2:
 case GeomAbs_C2:
   {
     TheConti = GeomAbs_C2;
   } 
   break;
 default:
   TheConti = GeomAbs_C2;// On ne sait pas faire mieux !
 }
 Handle (Approx_SweepFunction) Func;
 Func.Nullify();

 if (myType == 4) { 
   if (!KPartT4()) {
     Func = new (GeomFill_CircularBlendFunc)(myAdpPath,  
					     myAdpFirstSect,
					     myAdpLastSect,
					     myRadius,
					     Polynomial);

     Approx_SweepApproximation App(Func);
     App.Perform(myAdpPath->FirstParameter(),
		 myAdpPath->LastParameter(),
		 Tol, Tol, 0., 0.01,
		 TheConti, DegMax, NbMaxSegment);
#ifdef OCCT_DEBUG
   cout << "Tuyau : ";
   App.Dump(cout);
   cout << endl;
#endif
     if (App.IsDone()) {
       mySurface = new Geom_BSplineSurface(App.SurfPoles(),
					   App.SurfWeights(),
					   App.SurfUKnots(),
					   App.SurfVKnots(),
					   App.SurfUMults(),
					   App.SurfVMults(),
					   App.UDegree(),
					   App.VDegree());
       myError = App.MaxErrorOnSurf();
       myIsDone = Standard_True;
     }
     //else {
     //  throw Standard_ConstructionError("GeomFill_Pipe::Perform : Cannot make a surface");
     //}
   }
 }
 else if ( (! myLoc.IsNull()) && (! mySec.IsNull()) ) {
   GeomFill_Sweep Sweep(myLoc, myKPart);
   Sweep.SetTolerance(Tol);
   Sweep.Build(mySec, GeomFill_Location, TheConti, DegMax, NbMaxSegment);   
   if (Sweep.IsDone()) {
      mySurface = Sweep.Surface();
      myError = Sweep.ErrorOnSurface();
      myIsDone = Standard_True;
   }
   //else {
   //  throw Standard_ConstructionError("GeomFill_Pipe::Perform : Cannot make a surface");
   //}
   }
 else {
   Perform(Standard_True, Polynomial);
 }
}    
   
   
//=======================================================================
//function : KPartT4
//purpose  : Pour gerer les cas particulier de type 4
//=======================================================================
Standard_Boolean GeomFill_Pipe::KPartT4()
{
 Standard_Boolean Ok = Standard_False;
 // -------    Cas du Cylindre  --------------------------
 if (myAdpPath->GetType() == GeomAbs_Line &&
     myAdpFirstSect->GetType() == GeomAbs_Line &&
     myAdpLastSect ->GetType() == GeomAbs_Line   ) {
     // try to generate a cylinder.
     gp_Ax1 A0 = myAdpPath     ->Line().Position();
     gp_Ax1 A1 = myAdpFirstSect->Line().Position();
     gp_Ax1 A2 = myAdpLastSect ->Line().Position();
     // direction must be the same.
     gp_Dir D0 = A0.Direction();
     gp_Dir D1 = A1.Direction();
     gp_Dir D2 = A2.Direction();
     if (!D0.IsEqual(D1,Precision::Angular()) ||
	 !D1.IsEqual(D2,Precision::Angular())   ) {
       return Ok;
     }

     // the length of the line must be te same
     Standard_Real L0 = 
       myAdpPath->LastParameter() - myAdpPath->FirstParameter();
     Standard_Real L1 = 
       myAdpFirstSect->LastParameter() - myAdpFirstSect->FirstParameter();
     Standard_Real L2 =
       myAdpLastSect->LastParameter() - myAdpLastSect->FirstParameter();
     if (Abs(L1-L0) > Precision::Confusion() ||
	 Abs(L2-L0) > Precision::Confusion()   ) {
       return Ok;
      }

     // the first points must be normal to the path.
     gp_Pnt P0 = myAdpPath     ->Value(myAdpPath     ->FirstParameter());
     gp_Pnt P1 = myAdpFirstSect->Value(myAdpFirstSect->FirstParameter());
     gp_Pnt P2 = myAdpLastSect ->Value(myAdpLastSect ->FirstParameter());
     gp_Dir V1(gp_Vec(P0,P1));
     gp_Dir V2(gp_Vec(P0,P2));
     if (Abs(V1.Dot(D0)) > Precision::Confusion() ||
	 Abs(V2.Dot(D0)) > Precision::Confusion()   )  return Ok;

     // the result is a cylindrical surface.
     gp_Dir X(V1), Y(V2), ZRef;
     ZRef = X.Crossed(Y);

     gp_Ax3 Axis( A0.Location(), D0, X);
     if ( ZRef.Dot(D0) < 0.)
       Axis.YReverse();

     // rotate the surface to set the iso U = 0 not in the result.
     Axis.Rotate(gp_Ax1(P0,ZRef),-M_PI/2.);
      
     mySurface = new Geom_CylindricalSurface( Axis, myRadius);
     Standard_Real Alpha = V1.AngleWithRef(V2,ZRef);
     mySurface = 
       new Geom_RectangularTrimmedSurface(mySurface,
					  M_PI/2. , 
					  M_PI/2. + Alpha,
					  myAdpPath->FirstParameter(),
					  myAdpPath->LastParameter());
     Ok = Standard_True; //C'est bien un cylindre
     myIsDone = Standard_True;
   }
 // -----------    Cas du tore  ----------------------------------
 else if (myAdpPath->GetType()      == GeomAbs_Circle &&
	  myAdpFirstSect->GetType() == GeomAbs_Circle &&
	  myAdpLastSect->GetType()  == GeomAbs_Circle   ) {
   // try to generate a toroidal surface.    
   // les 3 cercles doivent avoir meme angle d'ouverture
   Standard_Real Alp0 =
     myAdpPath->FirstParameter() - myAdpPath->LastParameter();
   Standard_Real Alp1 =
     myAdpFirstSect->FirstParameter() - myAdpFirstSect->LastParameter();
   Standard_Real Alp2 =
     myAdpLastSect->FirstParameter() - myAdpLastSect->LastParameter();

   if (Abs(Alp0-Alp1) > Precision::Angular() ||
       Abs(Alp0-Alp2) > Precision::Angular()   ) return Ok;

   gp_Ax2 A0 = myAdpPath     ->Circle().Position();
   gp_Ax2 A1 = myAdpFirstSect->Circle().Position();
   gp_Ax2 A2 = myAdpLastSect ->Circle().Position();
   gp_Dir D0 = A0.Direction();
   gp_Dir D1 = A1.Direction();
   gp_Dir D2 = A2.Direction();
   gp_Pnt P0 = myAdpPath     ->Value(myAdpPath     ->FirstParameter());
   gp_Pnt P1 = myAdpFirstSect->Value(myAdpFirstSect->FirstParameter());
   gp_Pnt P2 = myAdpLastSect ->Value(myAdpLastSect ->FirstParameter());

   // les 3 directions doivent etre egales.
   if (!D0.IsEqual(D1,Precision::Angular()) ||
       !D1.IsEqual(D2,Precision::Angular())   )  return Ok;

   // les 3 ax1 doivent etre confondus.
   gp_Lin L(A0.Axis());
   if (!L.Contains(A1.Location(),Precision::Confusion()) ||
       !L.Contains(A2.Location(),Precision::Confusion())   )  return Ok;

   // les 3 premiers points doivent etre dans la meme section.
   gp_Dir V1(gp_Vec(P0,P1));
   gp_Dir V2(gp_Vec(P0,P2));
   gp_Circ Ci = myAdpPath->Circle();
   gp_Vec YRef = 
     ElCLib::CircleDN(myAdpPath->FirstParameter(),A0, Ci.Radius(), 1);
   if (Abs(V1.Dot(YRef)) > Precision::Confusion() ||
       Abs(V2.Dot(YRef)) > Precision::Confusion()   )  return Ok;

   // OK it`s a Toroidal Surface !!  OUF !!
   gp_Torus T(A0,Ci.Radius(),myRadius);
   gp_Vec XRef(A0.Location(),P0);
   // au maximum on fait un tore d`ouverture en V = PI
   Standard_Real VV1 = V1.AngleWithRef(XRef,YRef);
   Standard_Real VV2 = V2.AngleWithRef(XRef,YRef);
   Standard_Real deltaV = V2.AngleWithRef(V1,YRef);
   if (deltaV < 0.) {
     T.VReverse();
     VV1 = -VV1;
     VV2 = 2*M_PI + VV1 - deltaV;
   }
   mySurface = new Geom_RectangularTrimmedSurface
     (new Geom_ToroidalSurface(T),
      myAdpPath->FirstParameter(),myAdpPath->LastParameter(),VV1,VV2);
   myExchUV = Standard_True;
   Ok = Standard_True;
   myIsDone = Standard_True;
 }

 return Ok;
}

//=======================================================================
//function : ApproxSurf
//purpose  : 
//=======================================================================
void GeomFill_Pipe::ApproxSurf(const Standard_Boolean WithParameters) {

  // Traitment of general case. 
  // generate a sequence of the section by <SweepSectionGenerator> 
  // and approximate this sequence. 
  
  if (myType != 4) throw Standard_ConstructionError("GeomFill_Pipe");
  GeomFill_SweepSectionGenerator Section(myAdpPath, myAdpFirstSect,
					   myAdpLastSect,myRadius);

  Section.Perform(myPolynomial);
  
#ifdef OCCT_DEBUG
  if ( Affich) {
    Standard_Integer NbPoles,NbKnots,Degree,NbPoles2d;
    Section.GetShape(NbPoles,NbKnots,Degree,NbPoles2d);

    TColgp_Array1OfPnt      Poles  (1,NbPoles);
    TColgp_Array1OfPnt2d    Poles2d(1,NbPoles);
    TColStd_Array1OfReal    Weights(1,NbPoles);
    TColStd_Array1OfInteger Mults  (1,NbKnots);
    TColStd_Array1OfReal    Knots  (1,NbKnots);
    Section.Knots(Knots);
    Section.Mults(Mults);

    for (Standard_Integer i = 1; i <= Section.NbSections(); i++) {
      NbSections++;
      Section.Section(i,Poles,Poles2d,Weights);
      Handle(Geom_BSplineCurve) BS = 
	new Geom_BSplineCurve(Poles,Weights,Knots,Mults,Degree);
#ifdef DRAW
      char name[256];
      sprintf(name,"SECT_%d",NbSections);
      DrawTrSurf::Set(name,BS);
#endif
    }
  }
#endif

  Handle(GeomFill_Line) Line = new GeomFill_Line(Section.NbSections());
  Standard_Integer NbIt = 0;
  Standard_Real T3d =  Precision::Approximation();
  Standard_Real T2d =  Precision::PApproximation();
  GeomFill_AppSweep App( 4, 8, T3d, T2d, NbIt, WithParameters);
  
  App.Perform( Line, Section, 30);

  if ( !App.IsDone()) {
#ifdef OCCT_DEBUG
    // on affiche les sections sous debug
    Standard_Integer NbPoles,NbKnots,Degree,NbPoles2d;
    Section.GetShape(NbPoles,NbKnots,Degree,NbPoles2d);

    TColgp_Array1OfPnt      Poles  (1,NbPoles);
    TColgp_Array1OfPnt2d    Poles2d(1,NbPoles);
    TColStd_Array1OfReal    Weights(1,NbPoles);
    TColStd_Array1OfInteger Mults  (1,NbKnots);
    TColStd_Array1OfReal    Knots  (1,NbKnots);
    Section.Knots(Knots);
    Section.Mults(Mults);

    for (Standard_Integer i = 1; i <= Section.NbSections(); i++) {
      Section.Section(i,Poles,Poles2d,Weights);
      Handle(Geom_BSplineCurve) BS = 
	new Geom_BSplineCurve(Poles,Weights,Knots,Mults,Degree);
#ifdef DRAW
      char name[256];
      sprintf(name,"sect_%d",i);
      DrawTrSurf::Set(name,BS);
#endif
    }
#endif
    //throw StdFail_NotDone("Pipe : App not done");
  }
  else {
    Standard_Integer UDegree, VDegree, NbUPoles, NbVPoles, NbUKnots, 
    NbVKnots;
    App.SurfShape(UDegree, VDegree, NbUPoles, NbVPoles, NbUKnots, NbVKnots);
    
    mySurface = new Geom_BSplineSurface(App.SurfPoles(),
					App.SurfWeights(),
					App.SurfUKnots(),
					App.SurfVKnots(),
					App.SurfUMults(),
					App.SurfVMults(),
					App.UDegree(),
					App.VDegree());
    Standard_Real t2d;
    App.TolReached(myError, t2d);
    myIsDone = Standard_True;
  }
}
