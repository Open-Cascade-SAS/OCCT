// Created on: 1993-07-19
// Created by: Remi LEQUETTE
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


#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <GeomTools.hxx>
#include <GeomTools_CurveSet.hxx>
#include <GeomTools_SurfaceSet.hxx>
#include <GeomTools_UndefinedTypeHandler.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pln.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <Message_ProgressIndicator.hxx>
#include <Message_ProgressSentry.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Stream.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>

#define PLANE           1
#define CYLINDER        2
#define CONE            3
#define SPHERE          4
#define TORUS           5
#define LINEAREXTRUSION 6
#define REVOLUTION      7
#define BEZIER          8
#define BSPLINE         9
#define RECTANGULAR     10
#define OFFSET          11

//=======================================================================
//function : GeomTools_SurfaceSet
//purpose  : 
//=======================================================================

GeomTools_SurfaceSet::GeomTools_SurfaceSet() 
{
}


//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void  GeomTools_SurfaceSet::Clear()
{
  myMap.Clear();
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

Standard_Integer  GeomTools_SurfaceSet::Add(const Handle(Geom_Surface)& S)
{
  return myMap.Add(S);
}


//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================

Handle(Geom_Surface)  GeomTools_SurfaceSet::Surface
       (const Standard_Integer I)const 
{
  if (I <= 0 || I > myMap.Extent())
    return Handle(Geom_Surface)();
  return Handle(Geom_Surface)::DownCast(myMap(I));
}


//=======================================================================
//function : Index
//purpose  : 
//=======================================================================

Standard_Integer  GeomTools_SurfaceSet::Index
  (const Handle(Geom_Surface)& S)const 
{
  return myMap.FindIndex(S);
}

//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

static void Print(const gp_Pnt P,
		  Standard_OStream& OS,
		  const Standard_Boolean compact)
{
  OS << P.X();
  if (!compact) OS << ",";
  OS << " ";
  OS << P.Y();
  if (!compact) OS << ",";
  OS << " ";
  OS << P.Z();
  OS << " ";
}

//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

static void Print(const gp_Dir D,
		  Standard_OStream& OS,
		  const Standard_Boolean compact)
{
  OS << D.X();
  if (!compact) OS << ",";
  OS << " ";
  OS << D.Y();
  if (!compact) OS << ",";
  OS << " ";
  OS << D.Z();
  OS << " ";
}

//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

static void Print(const Handle(Geom_Plane)& S,
			 Standard_OStream& OS,
			 const Standard_Boolean compact)
{
  if (compact)
    OS << PLANE << " ";
  else
    OS << "Plane";
  
  gp_Pln P = S->Pln();
  if (!compact) OS << "\n  Origin :";
  Print(P.Location(),OS,compact);
  if (!compact) OS << "\n  Axis   :";
  Print(P.Axis().Direction(),OS,compact);
  if (!compact) OS << "\n  XAxis  :";
  Print(P.XAxis().Direction(),OS,compact);
  if (!compact) OS << "\n  YAxis  :";
  Print(P.YAxis().Direction(),OS,compact);
  OS << "\n";
  if (!compact) OS << "\n";
}


//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

static void Print(const Handle(Geom_CylindricalSurface)& S,
			 Standard_OStream& OS,
			 const Standard_Boolean compact)
{
  if (compact)
    OS << CYLINDER << " ";
  else
    OS << "CylindricalSurface";
  
  gp_Cylinder P = S->Cylinder();
  if (!compact) OS << "\n  Origin :";
  Print(P.Location(),OS,compact);
  if (!compact) OS << "\n  Axis   :";
  Print(P.Axis().Direction(),OS,compact);
  if (!compact) OS << "\n  XAxis  :";
  Print(P.XAxis().Direction(),OS,compact);
  if (!compact) OS << "\n  YAxis  :";
  Print(P.YAxis().Direction(),OS,compact);
  if (!compact) OS << "\n  Radius :";
  OS << P.Radius();
  OS << "\n";
  if (!compact) OS << "\n";
}


//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

static void Print(const Handle(Geom_ConicalSurface)& S,
			 Standard_OStream& OS,
			 const Standard_Boolean compact)
{
  if (compact)
    OS << CONE << " ";
  else
    OS << "ConicalSurface";
  
  gp_Cone P = S->Cone();
  if (!compact) OS << "\n  Origin :";
  Print(P.Location(),OS,compact);
  if (!compact) OS << "\n  Axis   :";
  Print(P.Axis().Direction(),OS,compact);
  if (!compact) OS << "\n  XAxis  :";
  Print(P.XAxis().Direction(),OS,compact);
  if (!compact) OS << "\n  YAxis  :";
  Print(P.YAxis().Direction(),OS,compact);
  if (!compact) OS << "\n  Radius :";
  OS << P.RefRadius();
  OS << "\n";
  if (!compact) OS << "\n  Angle :";
  OS << P.SemiAngle();
  OS << "\n";
  if (!compact) OS << "\n";
}


//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

static void Print(const Handle(Geom_SphericalSurface)& S,
			 Standard_OStream& OS,
			 const Standard_Boolean compact)
{
  if (compact)
    OS << SPHERE << " ";
  else
    OS << "SphericalSurface";

  gp_Sphere P = S->Sphere();
  if (!compact) OS << "\n  Center :";
  Print(P.Location(),OS,compact);
  if (!compact) OS << "\n  Axis   :";
  Print(P.Position().Axis().Direction(),OS,compact);
  if (!compact) OS << "\n  XAxis  :";
  Print(P.XAxis().Direction(),OS,compact);
  if (!compact) OS << "\n  YAxis  :";
  Print(P.YAxis().Direction(),OS,compact);
  if (!compact) OS << "\n  Radius :";
  OS << P.Radius();
  OS << "\n";
  if (!compact) OS << "\n";
}


//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

static void Print(const Handle(Geom_ToroidalSurface)& S,
			 Standard_OStream& OS,
			 const Standard_Boolean compact)
{
  if (compact)
    OS << TORUS << " ";
  else
    OS << "ToroidalSurface";
  
  gp_Torus P = S->Torus();
  if (!compact) OS << "\n  Origin :";
  Print(P.Location(),OS,compact);
  if (!compact) OS << "\n  Axis   :";
  Print(P.Axis().Direction(),OS,compact);
  if (!compact) OS << "\n  XAxis  :";
  Print(P.XAxis().Direction(),OS,compact);
  if (!compact) OS << "\n  YAxis  :";
  Print(P.YAxis().Direction(),OS,compact);
  if (!compact) OS << "\n  Radii  :";
  OS << P.MajorRadius() << " " << P.MinorRadius();
  OS << "\n";
  if (!compact) OS << "\n";
}


//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

static void Print(const Handle(Geom_SurfaceOfLinearExtrusion)& S,
			 Standard_OStream& OS,
			 const Standard_Boolean compact)
{
  if (compact)
    OS << LINEAREXTRUSION << " ";
  else
    OS << "SurfaceOfLinearExtrusion";

  if (!compact) OS << "\n  Direction :";
  Print(S->Direction(),OS,compact);
  if (!compact) OS << "\n  Basis curve : ";
  OS << "\n";
  GeomTools_CurveSet::PrintCurve(S->BasisCurve(),OS,compact);
}


//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

static void Print(const Handle(Geom_SurfaceOfRevolution)& S,
			 Standard_OStream& OS,
			 const Standard_Boolean compact)
{
  if (compact)
    OS << REVOLUTION <<" ";
  else
    OS << "SurfaceOfRevolution";

  if (!compact) OS << "\n  Origin    :";
  Print(S->Location(),OS,compact);
  if (!compact) OS << "\n  Direction :";
  Print(S->Direction(),OS,compact);
  if (!compact) OS << "\n  Basis curve : ";
  OS << "\n";
  GeomTools_CurveSet::PrintCurve(S->BasisCurve(),OS,compact);
}


//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

static void Print(const Handle(Geom_BezierSurface)& S,
			 Standard_OStream& OS,
			 const Standard_Boolean compact)
{
  if (compact)
    OS << BEZIER << " ";
  else
    OS << "BezierSurface";


  Standard_Boolean urational = S->IsURational();
  Standard_Boolean vrational = S->IsVRational();
  if (compact)
    OS << (urational ? 1 : 0) << " ";
  else {
    if (urational) 
      OS << " urational";
  }
  if (compact)
    OS << (vrational ? 1 : 0) << " ";
  else {
    if (vrational) 
      OS << " vrational";
  }

  if (!compact) {
    Standard_Boolean uclosed = S->IsUClosed();
    Standard_Boolean vclosed = S->IsVClosed();
    if (uclosed) 
      OS << " uclosed";
    if (vclosed)
      OS << " vclosed";
  }

  // poles and weights
  Standard_Integer i,j,udegree,vdegree;
  udegree = S->UDegree();
  vdegree = S->VDegree();
  if (!compact) OS << "\n  Degrees :";
  OS << udegree << " " << vdegree << " ";
  
  for (i = 1; i <= udegree+1; i++) {
    for (j = 1; j <= vdegree+1; j++) {
      if (!compact) OS << "\n  "<<setw(2)<<i<<", "<<setw(2)<<j<<" : ";
      Print(S->Pole(i,j),OS,compact);
      if (urational || vrational)
	OS << " " << S->Weight(i,j);
      if (compact)
	OS << " ";
    }
    OS << "\n";
  }
  OS << "\n";
  if (!compact) OS << "\n";
}


//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

static void Print(const Handle(Geom_BSplineSurface)& S,
			 Standard_OStream& OS,
			 const Standard_Boolean compact)
{
  if (compact)
    OS << BSPLINE << " ";
  else
    OS << "BSplineSurface";

  Standard_Boolean urational = S->IsURational();
  Standard_Boolean vrational = S->IsVRational();
  if (compact)
    OS << (urational ? 1 : 0) << " ";
  else {
    if (urational) 
      OS << " urational";
  }
  if (compact)
    OS << (vrational ? 1 : 0) << " ";
  else {
    if (vrational) 
      OS << " vrational";
  }

  Standard_Boolean uperiodic = S->IsUPeriodic();
  Standard_Boolean vperiodic = S->IsVPeriodic();
  if (compact)
    OS << (uperiodic ? 1 : 0) << " ";
  else {
    if (uperiodic) 
      OS << " uperiodic";
  }
  if (compact)
    OS << (vperiodic ? 1 : 0) << " ";
  else {
    if (vperiodic) 
      OS << " vperiodic";
  }

  if (!compact) {
    Standard_Boolean uclosed = S->IsUClosed();
    Standard_Boolean vclosed = S->IsVClosed();
    if (uclosed) 
      OS << " uclosed";
    if (vclosed)
      OS << " vclosed";
  }
  

  // poles and weights
  Standard_Integer i,j,udegree,vdegree,nbupoles,nbvpoles,nbuknots,nbvknots;
  udegree = S->UDegree();
  vdegree = S->VDegree();
  nbupoles = S->NbUPoles();
  nbvpoles = S->NbVPoles();
  nbuknots = S->NbUKnots();
  nbvknots = S->NbVKnots();
  if (!compact) OS << "\n  Degrees :";
  OS << udegree << " " << vdegree << " ";
  if (!compact) OS << "\n  NbPoles :";
  OS << nbupoles << " " << nbvpoles << " ";
  if (!compact) OS << "\n  NbKnots :";
  OS << nbuknots << " " << nbvknots << " ";

  if (!compact) OS << "\n Poles :\n";
  for (i = 1; i <= nbupoles; i++) {
    for (j = 1; j <= nbvpoles; j++) {
      if (!compact) OS << "\n  "<<setw(2)<<i<<", "<<setw(2)<<j<<" : ";
      Print(S->Pole(i,j),OS,compact);
      if (urational || vrational)
	OS << " " << S->Weight(i,j);
      if (compact)
	OS << " ";
    }
    OS << "\n";
  }
  OS << "\n";
  if (!compact) OS << "\n UKnots :\n";
  for (i = 1; i <= nbuknots; i++) {
    if (!compact) OS << "\n  "<<setw(2)<<i<<" : ";
    OS << S->UKnot(i) << " " << S->UMultiplicity(i) <<"\n";
  }
  OS << "\n";
  if (!compact) OS << "\n VKnots :\n";
  for (i = 1; i <= nbvknots; i++) {
    if (!compact) OS << "\n  "<<setw(2)<<i<<" : ";
    OS << S->VKnot(i) << " " << S->VMultiplicity(i) <<"\n";
  }
  OS << "\n";
  if (!compact) OS << "\n";
}


//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

static void Print(const Handle(Geom_RectangularTrimmedSurface)& S,
			 Standard_OStream& OS,
			 const Standard_Boolean compact)
{
  if (compact)
    OS << RECTANGULAR << " ";
  else
    OS << "RectangularTrimmedSurface";

  Standard_Real U1,U2,V1,V2;
  S->Bounds(U1,U2,V1,V2);
  if (!compact) OS << "\nParameters : ";
  OS << U1 << " " << U2 << " " << V1 << " " << V2 <<"\n";
  if (!compact) OS << "BasisSurface :\n";
  GeomTools_SurfaceSet::PrintSurface(S->BasisSurface(),OS,compact);
}


//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

static void Print(const Handle(Geom_OffsetSurface)& S,
			 Standard_OStream& OS,
			 const Standard_Boolean compact)
{
  if (compact)
    OS << OFFSET << " ";
  else
    OS << "OffsetSurface";

  if (!compact) OS << "\nOffset : ";
  OS << S->Offset() <<"\n";
  if (!compact) OS << "BasisSurface :\n";
  GeomTools_SurfaceSet::PrintSurface(S->BasisSurface(),OS,compact);
}


//=======================================================================
//function : PrintSurface
//purpose  : 
//=======================================================================

void GeomTools_SurfaceSet::PrintSurface(const Handle(Geom_Surface)& S,
					Standard_OStream& OS,
					const Standard_Boolean compact)
{
  Handle(Standard_Type) TheType = S->DynamicType();

  if ( TheType ==  STANDARD_TYPE(Geom_Plane)) {
    Print(Handle(Geom_Plane)::DownCast(S),OS,compact);
  }
  else if ( TheType ==  STANDARD_TYPE(Geom_CylindricalSurface)) {
    Print(Handle(Geom_CylindricalSurface)::DownCast(S),OS,compact);
  }
  else if ( TheType ==  STANDARD_TYPE(Geom_ConicalSurface)) {
    Print(Handle(Geom_ConicalSurface)::DownCast(S),OS,compact);
  }
  else if ( TheType ==  STANDARD_TYPE(Geom_SphericalSurface)) {
    Print(Handle(Geom_SphericalSurface)::DownCast(S),OS,compact);
  }
  else if ( TheType ==  STANDARD_TYPE(Geom_ToroidalSurface)) {
    Print(Handle(Geom_ToroidalSurface)::DownCast(S),OS,compact);
  }
  else if ( TheType ==  STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)) {
    Print(Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(S),OS,compact);
  }
  else if ( TheType ==  STANDARD_TYPE(Geom_SurfaceOfRevolution)) {
    Print(Handle(Geom_SurfaceOfRevolution)::DownCast(S),OS,compact);
  }
  else if ( TheType ==  STANDARD_TYPE(Geom_BezierSurface)) {
    Print(Handle(Geom_BezierSurface)::DownCast(S),OS,compact);
  }
  else if ( TheType ==  STANDARD_TYPE(Geom_BSplineSurface)) {
    Print(Handle(Geom_BSplineSurface)::DownCast(S),OS,compact);
  }
  else if ( TheType ==  STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
    Print(Handle(Geom_RectangularTrimmedSurface)::DownCast(S),OS,compact);
  }
  else if ( TheType ==  STANDARD_TYPE(Geom_OffsetSurface)) {
    Print(Handle(Geom_OffsetSurface)::DownCast(S),OS,compact);
  }
  else {
    GeomTools::GetUndefinedTypeHandler()->PrintSurface(S,OS,compact);
    //if (!compact)
    //  OS << "***** Unknown Surface ********\n";
    //else
    //  cout << "***** Unknown Surface ********"<<endl;
  }
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void  GeomTools_SurfaceSet::Dump(Standard_OStream& OS)const 
{
  Standard_Integer i, nbsurf = myMap.Extent();
  OS << "\n -------\n";
  OS << "Dump of "<< nbsurf << " surfaces ";
  OS << "\n -------\n\n";

  for (i = 1; i <= nbsurf; i++) {
    OS << setw(4) << i << " : ";
    PrintSurface(Handle(Geom_Surface)::DownCast(myMap(i)),OS,Standard_False);
  }
}


//=======================================================================
//function : Write
//purpose  : 
//=======================================================================

void  GeomTools_SurfaceSet::Write(Standard_OStream& OS)const 
{
  std::streamsize  prec = OS.precision(17);

  Standard_Integer i, nbsurf = myMap.Extent();
  OS << "Surfaces "<< nbsurf << "\n";
  //OCC19559
  Handle(Message_ProgressIndicator) progress = GetProgress();
  Message_ProgressSentry PS(progress, "Surfaces", 0, nbsurf, 1);
  for (i = 1; i <= nbsurf && PS.More(); i++, PS.Next()) {
    PrintSurface(Handle(Geom_Surface)::DownCast(myMap(i)),OS,Standard_True);
  }
  OS.precision(prec);
}


//=======================================================================
//function : ReadPnt
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, gp_Pnt& P)
{
  Standard_Real X=0.,Y=0.,Z=0.;
  GeomTools::GetReal(IS, X);
  GeomTools::GetReal(IS, Y);
  GeomTools::GetReal(IS, Z);
  P.SetCoord(X,Y,Z);
  return IS;
}

//=======================================================================
//function : ReadDir
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, gp_Dir& D)
{
  Standard_Real X=0.,Y=0.,Z=0.;
  GeomTools::GetReal(IS, X);
  GeomTools::GetReal(IS, Y);
  GeomTools::GetReal(IS, Z);
  D.SetCoord(X,Y,Z);
  return IS;
}

//=======================================================================
//function : ReadAx3
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, gp_Ax3& A3)
{
  gp_Pnt P(0.,0.,0.);
  gp_Dir A(1.,0.,0.),AX(1.,0.,0.),AY(1.,0.,0.);
  IS >> P >> A >> AX >> AY;
  gp_Ax3 ax3(P,A,AX);
  if (AY.DotCross(A,AX) < 0)
    ax3.YReverse();
  A3 = ax3;
  return IS;
}


//=======================================================================
//function : operator>>
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom_Plane)& S)
{
  gp_Ax3 A;
  IS >> A;
  S = new Geom_Plane(A);
  return IS;
}

//=======================================================================
//function : operator>>
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom_CylindricalSurface)& S)
{
  gp_Ax3 A;
  Standard_Real R=0.;
  IS >> A;
  GeomTools::GetReal(IS, R);
  S = new Geom_CylindricalSurface(A,R);
  return IS;
}

//=======================================================================
//function : operator>>
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom_ConicalSurface)& S)
{
  gp_Ax3 A;
  Standard_Real R=0.,Ang=0.;
  IS >> A;
  GeomTools::GetReal(IS, R);
  GeomTools::GetReal(IS, Ang);
  S = new Geom_ConicalSurface(A,Ang,R);
  return IS;
}

//=======================================================================
//function : operator>>
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom_SphericalSurface)& S)
{
  gp_Ax3 A;
  Standard_Real R=0.;
  IS >> A;
  GeomTools::GetReal(IS, R);
  S = new Geom_SphericalSurface(A,R);
  return IS;
}

//=======================================================================
//function : operator>>
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom_ToroidalSurface)& S)
{
  gp_Ax3 A;
  Standard_Real R1=0.,R2=0.;
  IS >> A;
  GeomTools::GetReal(IS, R1);
  GeomTools::GetReal(IS, R2);
  S = new Geom_ToroidalSurface(A,R1,R2);
  return IS;
}

//=======================================================================
//function : operator>>
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom_SurfaceOfLinearExtrusion)& S)
{
  gp_Dir D(1.,0.,0.);
  IS >> D;
  Handle(Geom_Curve) C = GeomTools_CurveSet::ReadCurve(IS);
  S = new Geom_SurfaceOfLinearExtrusion(C,D);
  return IS;
}

//=======================================================================
//function : operator>>
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom_SurfaceOfRevolution)& S)
{
  gp_Pnt P(0.,0.,0.);
  gp_Dir D(1.,0.,0.);
  IS >> P >> D;
  Handle(Geom_Curve) C = GeomTools_CurveSet::ReadCurve(IS);
  S = new Geom_SurfaceOfRevolution(C,gp_Ax1(P,D));
  return IS;
}

//=======================================================================
//function : operator>>
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom_BezierSurface)& S)
{
  Standard_Boolean urational=Standard_False, vrational=Standard_False;
  IS >> urational >> vrational;
  Standard_Integer udegree=0, vdegree=0;
  IS >> udegree >> vdegree;
  TColgp_Array2OfPnt poles(1,udegree+1,1,vdegree+1);
  TColStd_Array2OfReal weights(1,udegree+1,1,vdegree+1);
  
  Standard_Integer i,j;
  for (i = 1; i <= udegree+1; i++) {
    for (j = 1; j <= vdegree+1; j++) {
      IS >> poles(i,j);
      if (urational || vrational)
	GeomTools::GetReal(IS, weights(i,j));
    }
  }

  if (urational || vrational)
    S = new Geom_BezierSurface(poles,weights);
  else
    S = new Geom_BezierSurface(poles);
  return IS;
}

//=======================================================================
//function : operator>>
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom_BSplineSurface)& S)
{
  Standard_Boolean urational=Standard_False, vrational=Standard_False,
                   uperiodic=Standard_False, vperiodic=Standard_False;
  IS >> urational >> vrational;
  IS >> uperiodic >> vperiodic;
  Standard_Integer udegree=0, vdegree=0,nbupoles=0,nbvpoles=0,nbuknots=0,nbvknots=0;
  IS >> udegree >> vdegree;
  IS >> nbupoles >> nbvpoles;
  IS >> nbuknots >> nbvknots;

  TColgp_Array2OfPnt poles(1,nbupoles,1,nbvpoles);
  TColStd_Array2OfReal weights(1,nbupoles,1,nbvpoles);
  
  Standard_Integer i,j;
  for (i = 1; i <= nbupoles; i++) {
    for (j = 1; j <= nbvpoles; j++) {
      IS >> poles(i,j);
      if (urational || vrational)
	GeomTools::GetReal(IS, weights(i,j));
    }
  }

  TColStd_Array1OfReal uknots(1,nbuknots);
  TColStd_Array1OfInteger umults(1,nbuknots);
  for (i = 1; i <= nbuknots; i++) {
    GeomTools::GetReal(IS, uknots(i)); 
    IS >> umults(i);
  }

  TColStd_Array1OfReal vknots(1,nbvknots);
  TColStd_Array1OfInteger vmults(1,nbvknots);
  for (i = 1; i <= nbvknots; i++) {
    GeomTools::GetReal(IS, vknots(i));
    IS >> vmults(i);
  }

  if (urational || vrational)
    S = new Geom_BSplineSurface(poles,weights,uknots,vknots,umults,vmults,
				udegree,vdegree,uperiodic,vperiodic);
  else
    S = new Geom_BSplineSurface(poles,uknots,vknots,umults,vmults,
				udegree,vdegree,uperiodic,vperiodic);
  return IS;
}

//=======================================================================
//function : operator>>
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom_RectangularTrimmedSurface)& S)
{
  Standard_Real U1=0.,U2=0.,V1=0.,V2=0.;
  GeomTools::GetReal(IS, U1);
  GeomTools::GetReal(IS, U2);
  GeomTools::GetReal(IS, V1);
  GeomTools::GetReal(IS, V2);
  Handle(Geom_Surface) BS = GeomTools_SurfaceSet::ReadSurface(IS);
  S = new Geom_RectangularTrimmedSurface(BS,U1,U2,V1,V2);
  return IS;
}

//=======================================================================
//function : operator>>
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom_OffsetSurface)& S)
{
  Standard_Real O=0.;
  GeomTools::GetReal(IS, O);
  Handle(Geom_Surface) BS = GeomTools_SurfaceSet::ReadSurface(IS);
  S = new Geom_OffsetSurface(BS,O,Standard_True);
  return IS;
}


//=======================================================================
//function : ReadSurface
//purpose  : 
//=======================================================================

Handle(Geom_Surface) GeomTools_SurfaceSet::ReadSurface(Standard_IStream& IS)
{
  Standard_Integer stype;

  Handle(Geom_Surface) S;
  try {
    OCC_CATCH_SIGNALS
    IS >> stype;
    switch (stype) {

    case PLANE :
      {
        Handle(Geom_Plane) SS;
        IS >> SS;
        S = SS;
      }
      break;

    case CYLINDER :
      {
        Handle(Geom_CylindricalSurface) SS;
        IS >> SS;
        S = SS;
      }
      break;

    case CONE :
      {
        Handle(Geom_ConicalSurface) SS;
        IS >> SS;
        S = SS;
      }
      break;

    case SPHERE :
      {
        Handle(Geom_SphericalSurface) SS;
        IS >> SS;
        S = SS;
      }
      break;

    case TORUS :
      {
        Handle(Geom_ToroidalSurface) SS;
        IS >> SS;
        S = SS;
      }
      break;

    case LINEAREXTRUSION :
      {
        Handle(Geom_SurfaceOfLinearExtrusion) SS;
        IS >> SS;
        S = SS;
      }
      break;

    case REVOLUTION :
      {
        Handle(Geom_SurfaceOfRevolution) SS;
        IS >> SS;
        S = SS;
      }
      break;

    case BEZIER :
      {
        Handle(Geom_BezierSurface) SS;
        IS >> SS;
        S = SS;
      }
      break;

    case BSPLINE :
      {
        Handle(Geom_BSplineSurface) SS;
        IS >> SS;
        S = SS;
      }
      break;

    case RECTANGULAR :
      {
        Handle(Geom_RectangularTrimmedSurface) SS;
        IS >> SS;
        S = SS;
      }
      break;

    case OFFSET :
      {
        Handle(Geom_OffsetSurface) SS;
        IS >> SS;
        S = SS;
      }
      break;
      
    default :
      {
        Handle(Geom_Surface) SS;
        GeomTools::GetUndefinedTypeHandler()->ReadSurface(stype,IS,SS);
        S = SS;
      }
      break;
    }
  }
  catch(Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
    cout <<"EXCEPTION in GeomTools_SurfaceSet::ReadSurface(..)!!!" << endl;
    cout << anException << endl;
#endif
    (void)anException;
  }
  return S;
}

//=======================================================================
//function : Read
//purpose  : 
//=======================================================================

void  GeomTools_SurfaceSet::Read(Standard_IStream& IS)
{
  char buffer[255];
  IS >> buffer;
  if (strcmp(buffer,"Surfaces")) {
    cout << "Not a surface table"<<endl;
    return;
  }

  Standard_Integer i, nbsurf;
  IS >> nbsurf;
  //OCC19559
  Handle(Message_ProgressIndicator) progress = GetProgress();
  Message_ProgressSentry PS(progress, "Surfaces", 0, nbsurf, 1);
  for (i = 1; i <= nbsurf && PS.More(); i++, PS.Next()) {
    Handle(Geom_Surface) S = GeomTools_SurfaceSet::ReadSurface (IS);
    myMap.Add(S);
  }
}

//=======================================================================
//function : GetProgress
//purpose  : 
//=======================================================================

Handle(Message_ProgressIndicator) GeomTools_SurfaceSet::GetProgress() const
{
  return myProgress;
}

//=======================================================================
//function : SetProgress
//purpose  : 
//=======================================================================

void GeomTools_SurfaceSet::SetProgress(const Handle(Message_ProgressIndicator)& PR)
{
  myProgress = PR;
}



