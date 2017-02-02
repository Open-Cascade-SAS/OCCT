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


#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <GeomTools.hxx>
#include <GeomTools_Curve2dSet.hxx>
#include <GeomTools_UndefinedTypeHandler.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab2d.hxx>
#include <Message_ProgressIndicator.hxx>
#include <Message_ProgressSentry.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Stream.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
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
//function : GeomTools_Curve2dSet
//purpose  : 
//=======================================================================

GeomTools_Curve2dSet::GeomTools_Curve2dSet() 
{
}


//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void  GeomTools_Curve2dSet::Clear()
{
  myMap.Clear();
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

Standard_Integer  GeomTools_Curve2dSet::Add(const Handle(Geom2d_Curve)& S)
{
  return myMap.Add(S);
}


//=======================================================================
//function : Curve2d
//purpose  : 
//=======================================================================

Handle(Geom2d_Curve)  GeomTools_Curve2dSet::Curve2d
       (const Standard_Integer I)const 
{
  if (I <= 0 || I > myMap.Extent())
     return Handle(Geom2d_Curve)();

  return  Handle(Geom2d_Curve)::DownCast(myMap(I));
}


//=======================================================================
//function : Index
//purpose  : 
//=======================================================================

Standard_Integer  GeomTools_Curve2dSet::Index
  (const Handle(Geom2d_Curve)& S)const 
{
  return myMap.FindIndex(S);
}

//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

static void Print(const gp_Pnt2d P,
		  Standard_OStream& OS,
		  const Standard_Boolean compact)
{
  OS << P.X();
  if (!compact) OS << ",";
  OS << " ";
  OS << P.Y();
  OS << " ";
}

//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

static void Print(const gp_Dir2d D,
		  Standard_OStream& OS,
		  const Standard_Boolean compact)
{
  OS << D.X();
  if (!compact) OS << ",";
  OS << " ";
  OS << D.Y();
  OS << " ";
}

//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

static void Print(const Handle(Geom2d_Line)& L,
			 Standard_OStream& OS,
			 const Standard_Boolean compact)
{
  if (compact)
    OS << LINE << " ";
  else
    OS << "Line";

  gp_Lin2d C2d = L->Lin2d();
  if (!compact) OS << "\n  Origin :";
  Print(C2d.Location(),OS,compact);
  if (!compact) OS << "\n  Axis   :";
  Print(C2d.Direction(),OS,compact);
  if (!compact) OS << "\n";
  OS << "\n";
}

//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

static void Print(const Handle(Geom2d_Circle)& C,
			 Standard_OStream& OS,
			 const Standard_Boolean compact)
{
  if (compact)
    OS << CIRCLE << " ";
  else
    OS << "Circle";

  gp_Circ2d C2d = C->Circ2d();
  if (!compact) OS << "\n  Center :";
  Print(C2d.Location(),OS,compact);
  if (!compact) OS << "\n  XAxis  :";
  Print(C2d.XAxis().Direction(),OS,compact);
  if (!compact) OS << "\n  YAxis  :";
  Print(C2d.YAxis().Direction(),OS,compact);
  if (!compact) OS << "\n  Radius :";
  OS << C2d.Radius();
  if (!compact) OS << "\n";
  OS << "\n";
}

//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

static void Print(const Handle(Geom2d_Ellipse)& E,
			 Standard_OStream& OS,
			 const Standard_Boolean compact)
{
  if (compact)
    OS << ELLIPSE << " ";
  else
    OS << "Ellipse";

  gp_Elips2d C2d = E->Elips2d();
  if (!compact) OS << "\n  Center :";
  Print(C2d.Location(),OS,compact);
  if (!compact) OS << "\n  XAxis  :";
  Print(C2d.XAxis().Direction(),OS,compact);
  if (!compact) OS << "\n  YAxis  :";
  Print(C2d.YAxis().Direction(),OS,compact);
  if (!compact) OS << "\n  Radii  :";
  OS << C2d.MajorRadius();
  if (!compact) OS << ",";
  OS << " ";
  OS << C2d.MinorRadius();
  if (!compact) OS << "\n";
  OS << "\n";
}

//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

static void Print(const Handle(Geom2d_Parabola)& P,
			 Standard_OStream& OS,
			 const Standard_Boolean compact)
{
  if (compact)
    OS << PARABOLA << " ";
  else
    OS << "Parabola";

  gp_Parab2d C2d = P->Parab2d();
  if (!compact) OS << "\n  Center :";
  Print(C2d.Location(),OS,compact);
  if (!compact) OS << "\n  XAxis  :";
  Print(C2d.Axis().XAxis().Direction(),OS,compact);
  if (!compact) OS << "\n  YAxis  :";
  Print(C2d.Axis().YAxis().Direction(),OS,compact);
  if (!compact) OS << "\n  Focal  :";
  OS << C2d.Focal();
  if (!compact) OS << "\n";
  OS << "\n";
}

//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

static void Print(const Handle(Geom2d_Hyperbola)& H,
			 Standard_OStream& OS,
			 const Standard_Boolean compact)
{
  if (compact)
    OS << HYPERBOLA << " ";
  else
    OS << "Hyperbola";

  gp_Hypr2d C2d = H->Hypr2d();
  if (!compact) OS << "\n  Center :";
  Print(C2d.Location(),OS,compact);
  if (!compact) OS << "\n  XAxis  :";
  Print(C2d.XAxis().Direction(),OS,compact);
  if (!compact) OS << "\n  YAxis  :";
  Print(C2d.YAxis().Direction(),OS,compact);
  if (!compact) OS << "\n  Radii  :";
  OS << C2d.MajorRadius();
  if (!compact) OS << ",";
  OS << " ";
  OS << C2d.MinorRadius();
  if (!compact) OS << "\n";
  OS << "\n";
}

//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

static void Print(const Handle(Geom2d_BezierCurve)& B,
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
  if (!compact) OS << "\n";
  OS << "\n";
}

//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

static void Print(const Handle(Geom2d_BSplineCurve)& B,
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
    OS << (periodic ? 1 : 0) << " ";
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
  if (!compact) OS << " Knots";
  
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
    else OS << " ";
    OS << B->Knot(i) << " " << B->Multiplicity(i);
  }
  if (!compact) OS << "\n";
  OS << "\n";
}

//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

static void Print(const Handle(Geom2d_TrimmedCurve)& C,
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
  GeomTools_Curve2dSet::PrintCurve2d(C->BasisCurve(),OS,compact);
}

//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

static void Print(const Handle(Geom2d_OffsetCurve)& C,
			 Standard_OStream& OS,
			 const Standard_Boolean compact)
{
  if (compact)
    OS << OFFSET << " ";
  else
    OS << "OffsetCurve";
  if (!compact) OS << "Offset : ";
  OS << C->Offset() <<  "\n";
  if (!compact) OS << "Basis curve :\n";
  GeomTools_Curve2dSet::PrintCurve2d(C->BasisCurve(),OS,compact);
}

//=======================================================================
//function : PrintCurve2d
//purpose  : 
//=======================================================================

void GeomTools_Curve2dSet::PrintCurve2d(const Handle(Geom2d_Curve)& C,
					Standard_OStream& OS,
					const Standard_Boolean compact)
{
  Handle(Standard_Type) TheType = C->DynamicType();

  if ( TheType ==  STANDARD_TYPE(Geom2d_Circle)) {
    Print(Handle(Geom2d_Circle)::DownCast(C),OS,compact);
  }
  else if ( TheType ==STANDARD_TYPE(Geom2d_Line)) {
    Print(Handle(Geom2d_Line)::DownCast(C),OS,compact);
  }
  else if ( TheType == STANDARD_TYPE(Geom2d_Ellipse)) {
    Print(Handle(Geom2d_Ellipse)::DownCast(C),OS,compact);
  }
  else if ( TheType == STANDARD_TYPE(Geom2d_Parabola)) {
    Print(Handle(Geom2d_Parabola)::DownCast(C),OS,compact);
  }
  else if ( TheType == STANDARD_TYPE(Geom2d_Hyperbola)) {
    Print(Handle(Geom2d_Hyperbola)::DownCast(C),OS,compact);
  }
  else if ( TheType == STANDARD_TYPE(Geom2d_BezierCurve)) {
    Print(Handle(Geom2d_BezierCurve)::DownCast(C),OS,compact);
  }
  else if ( TheType == STANDARD_TYPE(Geom2d_BSplineCurve)) {
    Print(Handle(Geom2d_BSplineCurve)::DownCast(C),OS,compact);
  }
  else if ( TheType == STANDARD_TYPE(Geom2d_TrimmedCurve)) {
    Print(Handle(Geom2d_TrimmedCurve)::DownCast(C),OS,compact);
  }
  else if ( TheType == STANDARD_TYPE(Geom2d_OffsetCurve)) {
    Print(Handle(Geom2d_OffsetCurve)::DownCast(C),OS,compact);
  }
  else {
    GeomTools::GetUndefinedTypeHandler()->PrintCurve2d(C,OS,compact);
    //if (!compact)
    //  OS << "****** UNKNOWN CURVE2d TYPE ******\n";
    //else 
    //  cout << "****** UNKNOWN CURVE2d TYPE ******" << endl;
  }
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void  GeomTools_Curve2dSet::Dump(Standard_OStream& OS)const 
{
  Standard_Integer i, nbsurf = myMap.Extent();
  OS << "\n -------\n";
  OS << "Dump of "<< nbsurf << " Curve2ds ";
  OS << "\n -------\n\n";

  for (i = 1; i <= nbsurf; i++) {
    OS << setw(4) << i << " : ";
    PrintCurve2d(Handle(Geom2d_Curve)::DownCast(myMap(i)),OS,Standard_False);
  }
}


//=======================================================================
//function : Write
//purpose  : 
//=======================================================================

void  GeomTools_Curve2dSet::Write(Standard_OStream& OS)const 
{
  std::streamsize prec = OS.precision(17);

  Standard_Integer i, nbsurf = myMap.Extent();
  OS << "Curve2ds "<< nbsurf << "\n";
  //OCC19559
  Handle(Message_ProgressIndicator) progress = GetProgress();
  Message_ProgressSentry PS(progress, "2D Curves", 0, nbsurf, 1);
  for (i = 1; i <= nbsurf && PS.More(); i++, PS.Next()) {
    PrintCurve2d(Handle(Geom2d_Curve)::DownCast(myMap(i)),OS,Standard_True);
  }
  OS.precision(prec);
}


//=======================================================================
//function : ReadPnt2d
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, gp_Pnt2d& P)
{
  Standard_Real X=0.,Y=0.;
  GeomTools::GetReal(IS, X);
  GeomTools::GetReal(IS, Y);
  P.SetCoord(X,Y);
  return IS;
}

//=======================================================================
//function : ReadDir2d
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, gp_Dir2d& D)
{
  Standard_Real X=0.,Y=0.;
  GeomTools::GetReal(IS, X);
  GeomTools::GetReal(IS, Y);
  D.SetCoord(X,Y);
  return IS;
}


//=======================================================================
//function : ReadCurve2d
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom2d_Line)& L)
{
  gp_Pnt2d P(0.,0.);
  gp_Dir2d AX(1.,0.);
  IS >> P >> AX;
  L = new Geom2d_Line(P,AX);
  return IS;
}

//=======================================================================
//function : ReadCurve2d
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom2d_Circle)& C)
{
  gp_Pnt2d P(0.,0.);
  gp_Dir2d AX(1.,0.),AY(1.,0.);
  Standard_Real R=0.;
  IS >> P >> AX >> AY;
  GeomTools::GetReal(IS, R);
  C = new Geom2d_Circle(gp_Ax22d(P,AX,AY),R);
  return IS;
}

//=======================================================================
//function : ReadCurve2d
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom2d_Ellipse)& E)
{
  gp_Pnt2d P(0.,0.);
  gp_Dir2d AX(1.,0.),AY(1.,0.);
  Standard_Real R1=0.,R2=0.;
  IS >> P >> AX >> AY;
  GeomTools::GetReal(IS, R1);
  GeomTools::GetReal(IS, R2);
  E = new Geom2d_Ellipse(gp_Ax22d(P,AX,AY),R1,R2);
  return IS;
}

//=======================================================================
//function : ReadCurve2d
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom2d_Parabola)& C)
{
  gp_Pnt2d P(0.,0.);
  gp_Dir2d AX(1.,0.),AY(1.,0.);
  Standard_Real R1=0.;
  IS >> P >> AX >> AY;
  GeomTools::GetReal(IS, R1);
  C = new Geom2d_Parabola(gp_Ax22d(P,AX,AY),R1);
  return IS;
}

//=======================================================================
//function : ReadCurve2d
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom2d_Hyperbola)& H)
{
  gp_Pnt2d P(0.,0.);
  gp_Dir2d AX(1.,0.),AY(1.,0.);
  Standard_Real R1=0.,R2=0.;
  IS >> P >> AX >> AY;
  GeomTools::GetReal(IS, R1);
  GeomTools::GetReal(IS, R2);
  H = new Geom2d_Hyperbola(gp_Ax22d(P,AX,AY),R1,R2);
  return IS;
}

//=======================================================================
//function : ReadCurve2d
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom2d_BezierCurve)& B)
{
  Standard_Boolean rational=Standard_False;
  IS >> rational;

  // poles and weights
  Standard_Integer i=0,degree=0;
  IS >> degree;

  TColgp_Array1OfPnt2d poles(1,degree+1);
  TColStd_Array1OfReal weights(1,degree+1);
  
  for (i = 1; i <= degree+1; i++) {
    IS >> poles(i);
    if (rational)
      GeomTools::GetReal(IS, weights(i));
  }

  if (rational)
    B = new Geom2d_BezierCurve(poles,weights);
  else
    B = new Geom2d_BezierCurve(poles);

  return IS;
}

//=======================================================================
//function : ReadCurve2d
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom2d_BSplineCurve)& B)
{
  Standard_Boolean rational=Standard_False,periodic=Standard_False;
  IS >> rational >> periodic;

  // poles and weights
  Standard_Integer i=0,degree=0,nbpoles=0,nbknots=0;
  IS >> degree >> nbpoles >> nbknots;

  TColgp_Array1OfPnt2d poles(1,nbpoles);
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
    B = new Geom2d_BSplineCurve(poles,weights,knots,mults,degree,periodic);
  else
    B = new Geom2d_BSplineCurve(poles,knots,mults,degree,periodic);
  
  return IS;
}

//=======================================================================
//function : ReadCurve2d
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom2d_TrimmedCurve)& C)
{
  Standard_Real p1=0.,p2=0.;
  GeomTools::GetReal(IS, p1);
  GeomTools::GetReal(IS, p2);
  Handle(Geom2d_Curve) BC = GeomTools_Curve2dSet::ReadCurve2d(IS);
  C = new Geom2d_TrimmedCurve(BC,p1,p2);
  return IS;
}

//=======================================================================
//function : ReadCurve2d
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom2d_OffsetCurve)& C)
{
  Standard_Real p=0.;
  GeomTools::GetReal(IS, p);
  Handle(Geom2d_Curve) BC = GeomTools_Curve2dSet::ReadCurve2d(IS);
  C = new Geom2d_OffsetCurve(BC,p);
  return IS;
}

//=======================================================================
//function : ReadCurve2d
//purpose  : 
//=======================================================================

Handle(Geom2d_Curve) GeomTools_Curve2dSet::ReadCurve2d(Standard_IStream& IS)
{
  Standard_Integer ctype;

  Handle(Geom2d_Curve) C;
  try {
    OCC_CATCH_SIGNALS
    IS >> ctype;
    switch (ctype) {

    case LINE :
      {
        Handle(Geom2d_Line) CC;
        IS >> CC;
        C = CC;
      }
      break;

    case CIRCLE :
      {
        Handle(Geom2d_Circle) CC;
        IS >> CC;
        C = CC;
      }
      break;

    case ELLIPSE :
      {
        Handle(Geom2d_Ellipse) CC;
        IS >> CC;
        C = CC;
      }
      break;

    case PARABOLA :
      {
        Handle(Geom2d_Parabola) CC;
        IS >> CC;
        C = CC;
      }
      break;

    case HYPERBOLA :
      {
        Handle(Geom2d_Hyperbola) CC;
        IS >> CC;
        C = CC;
      }
      break;

    case BEZIER :
      {
        Handle(Geom2d_BezierCurve) CC;
        IS >> CC;
        C = CC;
      }
      break;

    case BSPLINE :
      {
        Handle(Geom2d_BSplineCurve) CC;
        IS >> CC;
        C = CC;
      }
      break;

    case TRIMMED :
      {
        Handle(Geom2d_TrimmedCurve) CC;
        IS >> CC;
        C = CC;
      }
      break;

    case OFFSET :
      {
        Handle(Geom2d_OffsetCurve) CC;
        IS >> CC;
        C = CC;
      }
      break;
      
    default:
      {
        Handle(Geom2d_Curve) CC;
        GeomTools::GetUndefinedTypeHandler()->ReadCurve2d(ctype,IS,CC);
        C = CC;
      }
      break;
    }
  }
  catch(Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
    cout <<"EXCEPTION in GeomTools_Curve2dSet::ReadCurve2d(..)!!!" << endl;
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

void  GeomTools_Curve2dSet::Read(Standard_IStream& IS)
{
  char buffer[255];
  IS >> buffer;
  if (strcmp(buffer,"Curve2ds")) {
    cout << "Not a Curve2d table"<<endl;
    return;
  }

  Standard_Integer i, nbcurve;
  IS >> nbcurve;
  //OCC19559
  Handle(Message_ProgressIndicator) progress = GetProgress();
  Message_ProgressSentry PS(progress, "2D Curves", 0, nbcurve, 1);
  for (i = 1; i <= nbcurve && PS.More(); i++, PS.Next()) {
    Handle(Geom2d_Curve) C = GeomTools_Curve2dSet::ReadCurve2d (IS);
    myMap.Add(C);
  }
}

//=======================================================================
//function : GetProgress
//purpose  : 
//=======================================================================

Handle(Message_ProgressIndicator) GeomTools_Curve2dSet::GetProgress() const
{
  return myProgress;
}

//=======================================================================
//function : SetProgress
//purpose  : 
//=======================================================================

void GeomTools_Curve2dSet::SetProgress(const Handle(Message_ProgressIndicator)& PR)
{
  myProgress = PR;
}


