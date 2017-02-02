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


#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomTools.hxx>
#include <GeomTools_CurveSet.hxx>
#include <GeomTools_UndefinedTypeHandler.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Parab.hxx>
#include <Message_ProgressIndicator.hxx>
#include <Message_ProgressSentry.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Stream.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>

#define LINE      1
#define CIRCLE    2
#define ELLIPSE   3
#define PARABOLA  4
#define HYPERBOLA 5
#define BEZIER    6
#define BSPLINE   7
#define TRIMMED   8
#define OFFSET    9

//=======================================================================
//function : GeomTools_CurveSet
//purpose  : 
//=======================================================================

GeomTools_CurveSet::GeomTools_CurveSet() 
{
}


//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void  GeomTools_CurveSet::Clear()
{
  myMap.Clear();
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

Standard_Integer  GeomTools_CurveSet::Add(const Handle(Geom_Curve)& C)
{
  return  (C.IsNull()) ? 0 : myMap.Add(C);
}


//=======================================================================
//function : Curve
//purpose  : 
//=======================================================================

Handle(Geom_Curve)  GeomTools_CurveSet::Curve
       (const Standard_Integer I)const 
{
  if (I <= 0 || I > myMap.Extent())
	return Handle(Geom_Curve)();
  return Handle(Geom_Curve)::DownCast(myMap(I));
}


//=======================================================================
//function : Index
//purpose  : 
//=======================================================================

Standard_Integer  GeomTools_CurveSet::Index
  (const Handle(Geom_Curve)& S)const 
{
  return S.IsNull() ? 0 : myMap.FindIndex(S);
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

static void Print(const Handle(Geom_Line)& L,
			 Standard_OStream& OS,
			 const Standard_Boolean compact)
{
  if (compact)
    OS << LINE << " ";
  else
    OS << "Line";

  gp_Lin C = L->Lin();
  if (!compact) OS << "\n  Origin :";
  Print(C.Location(),OS,compact);
  if (!compact) OS << "\n  Axis   :";
  Print(C.Direction(),OS,compact);
  if (!compact) OS << "\n";
  OS << "\n";
}

//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

static void Print(const Handle(Geom_Circle)& CC,
			 Standard_OStream& OS,
			 const Standard_Boolean compact)
{
  if (compact)
    OS << CIRCLE << " ";
  else
    OS << "Circle";

  gp_Circ C = CC->Circ();
  if (!compact) OS << "\n  Center :";
  Print(C.Location(),OS,compact);
  if (!compact) OS << "\n  Axis   :";
  Print(C.Axis().Direction(),OS,compact);
  if (!compact) OS << "\n  XAxis  :";
  Print(C.XAxis().Direction(),OS,compact);
  if (!compact) OS << "\n  YAxis  :";
  Print(C.YAxis().Direction(),OS,compact);
  if (!compact) OS << "\n  Radius :";
  OS << C.Radius();
  if (!compact) OS << "\n";
  OS << "\n";
}

//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

static void Print(const Handle(Geom_Ellipse)& E,
			 Standard_OStream& OS,
			 const Standard_Boolean compact)
{
  if (compact)
    OS << ELLIPSE << " ";
  else
    OS << "Ellipse";

  gp_Elips C = E->Elips();
  if (!compact) OS << "\n  Center :";
  Print(C.Location(),OS,compact);
  if (!compact) OS << "\n  Axis   :";
  Print(C.Axis().Direction(),OS,compact);
  if (!compact) OS << "\n  XAxis  :";
  Print(C.XAxis().Direction(),OS,compact);
  if (!compact) OS << "\n  YAxis  :";
  Print(C.YAxis().Direction(),OS,compact);
  if (!compact) OS << "\n  Radii  :";
  OS << C.MajorRadius();
  if (!compact) OS << ",";
  OS << " ";
  OS << C.MinorRadius();
  if (!compact) OS << "\n";
  OS << "\n";
}

//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

static void Print(const Handle(Geom_Parabola)& P,
			 Standard_OStream& OS,
			 const Standard_Boolean compact)
{
  if (compact)
    OS << PARABOLA << " ";
  else
    OS << "Parabola";

  gp_Parab C = P->Parab();
  if (!compact) OS << "\n  Center :";
  Print(C.Location(),OS,compact);
  if (!compact) OS << "\n  Axis   :";
  Print(C.Axis().Direction(),OS,compact);
  if (!compact) OS << "\n  XAxis  :";
  Print(C.XAxis().Direction(),OS,compact);
  if (!compact) OS << "\n  YAxis  :";
  Print(C.YAxis().Direction(),OS,compact);
  if (!compact) OS << "\n  Focal  :";
  OS << C.Focal();
  if (!compact) OS << "\n";
  OS << "\n";
}

//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

static void Print(const Handle(Geom_Hyperbola)& H,
			 Standard_OStream& OS,
			 const Standard_Boolean compact)
{
  if (compact)
    OS << HYPERBOLA << " ";
  else
    OS << "Hyperbola";

  gp_Hypr C = H->Hypr();
  if (!compact) OS << "\n  Center :";
  Print(C.Location(),OS,compact);
  if (!compact) OS << "\n  Axis   :";
  Print(C.Axis().Direction(),OS,compact);
  if (!compact) OS << "\n  XAxis  :";
  Print(C.XAxis().Direction(),OS,compact);
  if (!compact) OS << "\n  YAxis  :";
  Print(C.YAxis().Direction(),OS,compact);
  if (!compact) OS << "\n  Radii  :";
  OS << C.MajorRadius();
  if (!compact) OS << ",";
  OS << " ";
  OS << C.MinorRadius();
  if (!compact) OS << "\n";
  OS << "\n";
}

//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

static void Print(const Handle(Geom_BezierCurve)& B,
			 Standard_OStream& OS,
			 const Standard_Boolean compact)
{
  if (compact)
    OS << BEZIER << " ";
  else
    OS << "BezierCurve";

  Standard_Boolean rational = B->IsRational();
  if (compact)
    OS << (rational ? 1 : 0) << " ";
  else {
    if (rational) 
      OS << " rational";
  }

  // poles and weights
  Standard_Integer i,degree = B->Degree();
  if (!compact) OS << "\n  Degree :";
  OS << degree << " ";
  
  for (i = 1; i <= degree+1; i++) {
    if (!compact) OS << "\n  "<<setw(2)<<i<<" : ";
    Print(B->Pole(i),OS,compact);
    if (rational)
      OS << " " << B->Weight(i);
    if (compact)
      OS << " ";
  }
  OS << "\n";
  if (!compact) OS << "\n";
}

//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

static void Print(const Handle(Geom_BSplineCurve)& B,
			 Standard_OStream& OS,
			 const Standard_Boolean compact)
{
  if (compact)
    OS << BSPLINE << " ";
  else
    OS << "BSplineCurve";


  Standard_Boolean rational = B->IsRational();
  if (compact)
    OS << (rational ? 1 : 0) << " ";
  else {
    if (rational) 
      OS << " rational";
  }

  Standard_Boolean periodic = B->IsPeriodic();
  if (compact)
    OS << (periodic ? 1 : 0)<< " ";
  else {
    if (periodic) 
      OS << " periodic";
  }

  // poles and weights
  Standard_Integer i,degree,nbpoles,nbknots;
  degree = B->Degree();
  nbpoles = B->NbPoles();
  nbknots = B->NbKnots();
  if (!compact) OS << "\n  Degree ";
  else OS << " ";
  OS << degree;
  if (!compact) OS << ",";
  OS << " ";
  OS << nbpoles;
  if (!compact) OS << " Poles,";
  OS << " ";
  OS << nbknots << " ";
  if (!compact) OS << " Knots\n";
  
  if (!compact) OS << "Poles :\n";
  for (i = 1; i <= nbpoles; i++) {
    if (!compact) OS << "\n  "<<setw(2)<<i<<" : ";
    else OS << " ";
    Print(B->Pole(i),OS,compact);
    if (rational)
      OS << " " << B->Weight(i);
  }
  OS << "\n";

  if (!compact) OS << "Knots :\n";
  for (i = 1; i <= nbknots; i++) {
    if (!compact) OS << "\n  "<<setw(2)<<i<<" : ";
    OS << " " << B->Knot(i) << " " << B->Multiplicity(i);
  }

  OS << "\n";
  if (!compact) OS << "\n";
}

//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

static void Print(const Handle(Geom_TrimmedCurve)& C,
			 Standard_OStream& OS,
			 const Standard_Boolean compact)
{
  if (compact)
    OS << TRIMMED << " ";
  else
    OS << "Trimmed curve\n";
  if (!compact) OS << "Parameters : ";
  OS << C->FirstParameter() << " " << C->LastParameter() << "\n";
  if (!compact) OS << "Basis curve :\n";
  GeomTools_CurveSet::PrintCurve(C->BasisCurve(),OS,compact);
}

//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

static void Print(const Handle(Geom_OffsetCurve)& C,
			 Standard_OStream& OS,
			 const Standard_Boolean compact)
{
  if (compact)
    OS << OFFSET << " ";
  else
    OS << "OffsetCurve";
  if (!compact) OS << "Offset : ";
  OS << C->Offset() <<  "\n";
  if (!compact) OS << "Direction : ";
  Print(C->Direction(),OS,compact);
  OS << "\n";
  if (!compact) OS << "Basis curve :\n";
  GeomTools_CurveSet::PrintCurve(C->BasisCurve(),OS,compact);
}

//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

void GeomTools_CurveSet::PrintCurve(const Handle(Geom_Curve)& C,
				    Standard_OStream& OS,
				    const Standard_Boolean compact)
{
  Handle(Standard_Type) TheType = C->DynamicType();

  if ( TheType ==STANDARD_TYPE(Geom_Line)) {
    Print(Handle(Geom_Line)::DownCast(C),OS,compact);
  }
  else if ( TheType ==  STANDARD_TYPE(Geom_Circle)) {
    Print(Handle(Geom_Circle)::DownCast(C),OS,compact);
  }
  else if ( TheType == STANDARD_TYPE(Geom_Ellipse)) {
    Print(Handle(Geom_Ellipse)::DownCast(C),OS,compact);
  }
  else if ( TheType == STANDARD_TYPE(Geom_Parabola)) {
    Print(Handle(Geom_Parabola)::DownCast(C),OS,compact);
  }
  else if ( TheType == STANDARD_TYPE(Geom_Hyperbola)) {
    Print(Handle(Geom_Hyperbola)::DownCast(C),OS,compact);
  }
  else if ( TheType == STANDARD_TYPE(Geom_BezierCurve)) {
    Print(Handle(Geom_BezierCurve)::DownCast(C),OS,compact);
  }
  else if ( TheType == STANDARD_TYPE(Geom_BSplineCurve)) {
    Print(Handle(Geom_BSplineCurve)::DownCast(C),OS,compact);
  }
  else if ( TheType == STANDARD_TYPE(Geom_TrimmedCurve)) {
    Print(Handle(Geom_TrimmedCurve)::DownCast(C),OS,compact);
  }
  else if ( TheType == STANDARD_TYPE(Geom_OffsetCurve)) {
    Print(Handle(Geom_OffsetCurve)::DownCast(C),OS,compact);
  }
  else {
    GeomTools::GetUndefinedTypeHandler()->PrintCurve(C,OS,compact);
    //if (!compact)
    //  OS << "****** UNKNOWN CURVE TYPE ******\n";
    //else 
    //  cout << "****** UNKNOWN CURVE TYPE ******" << endl;
  }
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void  GeomTools_CurveSet::Dump(Standard_OStream& OS)const 
{
  Standard_Integer i, nbsurf = myMap.Extent();
  OS << "\n -------\n";
  OS << "Dump of "<< nbsurf << " Curves ";
  OS << "\n -------\n\n";

  for (i = 1; i <= nbsurf; i++) {
    OS << setw(4) << i << " : ";
    PrintCurve(Handle(Geom_Curve)::DownCast(myMap(i)),OS,Standard_False);
  }
}


//=======================================================================
//function : Write
//purpose  : 
//=======================================================================

void  GeomTools_CurveSet::Write(Standard_OStream& OS)const 
{
  std::streamsize  prec = OS.precision(17);

  Standard_Integer i, nbcurve = myMap.Extent();
  OS << "Curves "<< nbcurve << "\n";
    //OCC19559
  Handle(Message_ProgressIndicator) progress = GetProgress();
  Message_ProgressSentry PS(progress, "3D Curves", 0, nbcurve, 1);
  for (i = 1; i <= nbcurve && PS.More(); i++, PS.Next()) {
    PrintCurve(Handle(Geom_Curve)::DownCast(myMap(i)),OS,Standard_True);
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
//function : ReadCurve
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom_Line)& L)
{
  gp_Pnt P(0.,0.,0.);
  gp_Dir AX(1.,0.,0.);
  IS >> P >> AX;
  L = new Geom_Line(P,AX);
  return IS;
}

//=======================================================================
//function : ReadCurve
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom_Circle)& C)
{
  gp_Pnt P(0.,0.,0.);
  gp_Dir A(1.,0.,0.),AX(1.,0.,0.),AY(1.,0.,0.);
  Standard_Real R=0.;
  IS >> P >> A >> AX >> AY;
  GeomTools::GetReal(IS, R);
  C = new Geom_Circle(gp_Ax2(P,A,AX),R);
  return IS;
}

//=======================================================================
//function : ReadCurve
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom_Ellipse)& E)
{
  gp_Pnt P(0.,0.,0.);
  gp_Dir A(1.,0.,0.),AX(1.,0.,0.),AY(1.,0.,0.);
  Standard_Real R1=0.,R2=0.;
  IS >> P >> A >> AX >> AY;
  GeomTools::GetReal(IS, R1);
  GeomTools::GetReal(IS, R2);
  E = new Geom_Ellipse(gp_Ax2(P,A,AX),R1,R2);
  return IS;
}

//=======================================================================
//function : ReadCurve
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom_Parabola)& C)
{
  gp_Pnt P(0.,0.,0.);
  gp_Dir A(1.,0.,0.),AX(1.,0.,0.),AY(1.,0.,0.);
  Standard_Real R1=0.;
  IS >> P >> A >> AX >> AY;
  GeomTools::GetReal(IS, R1);
  C = new Geom_Parabola(gp_Ax2(P,A,AX),R1);
  return IS;
}

//=======================================================================
//function : ReadCurve
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom_Hyperbola)& H)
{
  gp_Pnt P(0.,0.,0.);
  gp_Dir A(1.,0.,0.),AX(1.,0.,0.),AY(1.,0.,0.);
  Standard_Real R1=0.,R2=0.;
  IS >> P >> A >> AX >> AY;
  GeomTools::GetReal(IS, R1);
  GeomTools::GetReal(IS, R2);
  H = new Geom_Hyperbola(gp_Ax2(P,A,AX),R1,R2);
  return IS;
}

//=======================================================================
//function : ReadCurve
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom_BezierCurve)& B)
{
  Standard_Boolean rational=Standard_False;
  IS >> rational;

  // poles and weights
  Standard_Integer i=0,degree=0;
  IS >> degree;

  TColgp_Array1OfPnt poles(1,degree+1);
  TColStd_Array1OfReal weights(1,degree+1);
  
  for (i = 1; i <= degree+1; i++) {
    IS >> poles(i);
    if (rational)
      GeomTools::GetReal(IS, weights(i));
  }

  if (rational)
    B = new Geom_BezierCurve(poles,weights);
  else
    B = new Geom_BezierCurve(poles);

  return IS;
}

//=======================================================================
//function : ReadCurve
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom_BSplineCurve)& B)
{

  Standard_Boolean rational=Standard_False,periodic=Standard_False;
  IS >> rational >> periodic;

  // poles and weights
  Standard_Integer i=0,degree=0,nbpoles=0,nbknots=0;
  IS >> degree >> nbpoles >> nbknots;

  TColgp_Array1OfPnt poles(1,nbpoles);
  TColStd_Array1OfReal weights(1,nbpoles);
  
  for (i = 1; i <= nbpoles; i++) {
    IS >> poles(i);
    if (rational)
      GeomTools::GetReal(IS, weights(i));
  }

  TColStd_Array1OfReal knots(1,nbknots);
  TColStd_Array1OfInteger mults(1,nbknots);

  for (i = 1; i <= nbknots; i++) {
    GeomTools::GetReal(IS, knots(i)); 
    IS >> mults(i);
  }

  if (rational)
    B = new Geom_BSplineCurve(poles,weights,knots,mults,degree,periodic);
  else
    B = new Geom_BSplineCurve(poles,knots,mults,degree,periodic);
  
  return IS;
}

//=======================================================================
//function : ReadCurve
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom_TrimmedCurve)& C)
{
  Standard_Real p1=0.,p2=0.;
  GeomTools::GetReal(IS, p1);
  GeomTools::GetReal(IS, p2);
  Handle(Geom_Curve) BC = GeomTools_CurveSet::ReadCurve(IS);
  C = new Geom_TrimmedCurve(BC,p1,p2);
  return IS;
}

//=======================================================================
//function : ReadCurve
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom_OffsetCurve)& C)
{
  Standard_Real p=0.;
  GeomTools::GetReal(IS, p);
  gp_Dir D(1.,0.,0.);
  IS >> D;
  Handle(Geom_Curve) BC = GeomTools_CurveSet::ReadCurve(IS);
  C = new Geom_OffsetCurve(BC,p,D);
  return IS;
}

//=======================================================================
//function : ReadCurve
//purpose  : 
//=======================================================================

Handle(Geom_Curve) GeomTools_CurveSet::ReadCurve (Standard_IStream& IS)
{
  Standard_Integer ctype;

  Handle(Geom_Curve) C;
  try {
    OCC_CATCH_SIGNALS
    IS >> ctype;
    switch (ctype) {

    case LINE :
      {
        Handle(Geom_Line) CC;
        IS >> CC;
        C = CC;
      }
      break;

    case CIRCLE :
      {
        Handle(Geom_Circle) CC;
        IS >> CC;
        C = CC;
      }
      break;

    case ELLIPSE :
      {
        Handle(Geom_Ellipse) CC;
        IS >> CC;
        C = CC;
      }
      break;

    case PARABOLA :
      {
        Handle(Geom_Parabola) CC;
        IS >> CC;
        C = CC;
      }
      break;

    case HYPERBOLA :
      {
        Handle(Geom_Hyperbola) CC;
        IS >> CC;
        C = CC;
      }
      break;

    case BEZIER :
      {
        Handle(Geom_BezierCurve) CC;
        IS >> CC;
        C = CC;
      }
      break;

    case BSPLINE :
      {
        Handle(Geom_BSplineCurve) CC;
        IS >> CC;
        C = CC;
      }
      break;

    case TRIMMED :
      {
        Handle(Geom_TrimmedCurve) CC;
        IS >> CC;
        C = CC;
      }
      break;

    case OFFSET :
      {
        Handle(Geom_OffsetCurve) CC;
        IS >> CC;
        C = CC;
      }
      break;
      
    default:
      {
        Handle(Geom_Curve) CC;
        GeomTools::GetUndefinedTypeHandler()->ReadCurve(ctype,IS,CC);
        C = CC;
      }
    }
  }
  catch(Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
    cout <<"EXCEPTION in GeomTools_CurveSet::ReadCurve(..)!!!" << endl;
    cout << anException << endl;
#endif
    (void)anException;
  }
  return C;
}

//=======================================================================
//function : Read
//purpose  : 
//=======================================================================

void  GeomTools_CurveSet::Read(Standard_IStream& IS)
{
  char buffer[255];
  IS >> buffer;
  if (strcmp(buffer,"Curves")) {
    cout << "Not a Curve table"<<endl;
    return;
  }

  Standard_Integer i, nbcurve;
  IS >> nbcurve;
  //OCC19559
  Handle(Message_ProgressIndicator) progress = GetProgress();
  Message_ProgressSentry PS(progress, "3D Curves", 0, nbcurve, 1);
  for (i = 1; i <= nbcurve && PS.More(); i++, PS.Next()) {
    Handle(Geom_Curve) C = GeomTools_CurveSet::ReadCurve (IS);
    myMap.Add(C);
  }
}

//=======================================================================
//function : GetProgress
//purpose  : 
//=======================================================================

Handle(Message_ProgressIndicator) GeomTools_CurveSet::GetProgress() const
{
  return myProgress;
}

//=======================================================================
//function : SetProgress
//purpose  : 
//=======================================================================

void GeomTools_CurveSet::SetProgress(const Handle(Message_ProgressIndicator)& PR)
{
  myProgress = PR;
}


