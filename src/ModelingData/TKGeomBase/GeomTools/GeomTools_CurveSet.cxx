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
#include <Message_ProgressScope.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Macro.hxx>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>

#define LINE 1
#define CIRCLE 2
#define ELLIPSE 3
#define PARABOLA 4
#define HYPERBOLA 5
#define BEZIER 6
#define BSPLINE 7
#define TRIMMED 8
#define OFFSET 9

//=================================================================================================

GeomTools_CurveSet::GeomTools_CurveSet() {}

//=================================================================================================

void GeomTools_CurveSet::Clear()
{
  myMap.Clear();
}

//=================================================================================================

int GeomTools_CurveSet::Add(const occ::handle<Geom_Curve>& C)
{
  return (C.IsNull()) ? 0 : myMap.Add(C);
}

//=================================================================================================

occ::handle<Geom_Curve> GeomTools_CurveSet::Curve(const int I) const
{
  if (I <= 0 || I > myMap.Extent())
    return occ::handle<Geom_Curve>();
  return occ::down_cast<Geom_Curve>(myMap(I));
}

//=================================================================================================

int GeomTools_CurveSet::Index(const occ::handle<Geom_Curve>& S) const
{
  return S.IsNull() ? 0 : myMap.FindIndex(S);
}

//=================================================================================================

static void Print(const gp_Pnt& P, Standard_OStream& OS, const bool compact)
{
  OS << P.X();
  if (!compact)
    OS << ",";
  OS << " ";
  OS << P.Y();
  if (!compact)
    OS << ",";
  OS << " ";
  OS << P.Z();
  OS << " ";
}

//=================================================================================================

static void Print(const gp_Dir& D, Standard_OStream& OS, const bool compact)
{
  OS << D.X();
  if (!compact)
    OS << ",";
  OS << " ";
  OS << D.Y();
  if (!compact)
    OS << ",";
  OS << " ";
  OS << D.Z();
  OS << " ";
}

//=================================================================================================

static void Print(const occ::handle<Geom_Line>& L, Standard_OStream& OS, const bool compact)
{
  if (compact)
    OS << LINE << " ";
  else
    OS << "Line";

  gp_Lin C = L->Lin();
  if (!compact)
    OS << "\n  Origin :";
  Print(C.Location(), OS, compact);
  if (!compact)
    OS << "\n  Axis   :";
  Print(C.Direction(), OS, compact);
  if (!compact)
    OS << "\n";
  OS << "\n";
}

//=================================================================================================

static void Print(const occ::handle<Geom_Circle>& CC, Standard_OStream& OS, const bool compact)
{
  if (compact)
    OS << CIRCLE << " ";
  else
    OS << "Circle";

  gp_Circ C = CC->Circ();
  if (!compact)
    OS << "\n  Center :";
  Print(C.Location(), OS, compact);
  if (!compact)
    OS << "\n  Axis   :";
  Print(C.Axis().Direction(), OS, compact);
  if (!compact)
    OS << "\n  XAxis  :";
  Print(C.XAxis().Direction(), OS, compact);
  if (!compact)
    OS << "\n  YAxis  :";
  Print(C.YAxis().Direction(), OS, compact);
  if (!compact)
    OS << "\n  Radius :";
  OS << C.Radius();
  if (!compact)
    OS << "\n";
  OS << "\n";
}

//=================================================================================================

static void Print(const occ::handle<Geom_Ellipse>& E, Standard_OStream& OS, const bool compact)
{
  if (compact)
    OS << ELLIPSE << " ";
  else
    OS << "Ellipse";

  gp_Elips C = E->Elips();
  if (!compact)
    OS << "\n  Center :";
  Print(C.Location(), OS, compact);
  if (!compact)
    OS << "\n  Axis   :";
  Print(C.Axis().Direction(), OS, compact);
  if (!compact)
    OS << "\n  XAxis  :";
  Print(C.XAxis().Direction(), OS, compact);
  if (!compact)
    OS << "\n  YAxis  :";
  Print(C.YAxis().Direction(), OS, compact);
  if (!compact)
    OS << "\n  Radii  :";
  OS << C.MajorRadius();
  if (!compact)
    OS << ",";
  OS << " ";
  OS << C.MinorRadius();
  if (!compact)
    OS << "\n";
  OS << "\n";
}

//=================================================================================================

static void Print(const occ::handle<Geom_Parabola>& P, Standard_OStream& OS, const bool compact)
{
  if (compact)
    OS << PARABOLA << " ";
  else
    OS << "Parabola";

  gp_Parab C = P->Parab();
  if (!compact)
    OS << "\n  Center :";
  Print(C.Location(), OS, compact);
  if (!compact)
    OS << "\n  Axis   :";
  Print(C.Axis().Direction(), OS, compact);
  if (!compact)
    OS << "\n  XAxis  :";
  Print(C.XAxis().Direction(), OS, compact);
  if (!compact)
    OS << "\n  YAxis  :";
  Print(C.YAxis().Direction(), OS, compact);
  if (!compact)
    OS << "\n  Focal  :";
  OS << C.Focal();
  if (!compact)
    OS << "\n";
  OS << "\n";
}

//=================================================================================================

static void Print(const occ::handle<Geom_Hyperbola>& H, Standard_OStream& OS, const bool compact)
{
  if (compact)
    OS << HYPERBOLA << " ";
  else
    OS << "Hyperbola";

  gp_Hypr C = H->Hypr();
  if (!compact)
    OS << "\n  Center :";
  Print(C.Location(), OS, compact);
  if (!compact)
    OS << "\n  Axis   :";
  Print(C.Axis().Direction(), OS, compact);
  if (!compact)
    OS << "\n  XAxis  :";
  Print(C.XAxis().Direction(), OS, compact);
  if (!compact)
    OS << "\n  YAxis  :";
  Print(C.YAxis().Direction(), OS, compact);
  if (!compact)
    OS << "\n  Radii  :";
  OS << C.MajorRadius();
  if (!compact)
    OS << ",";
  OS << " ";
  OS << C.MinorRadius();
  if (!compact)
    OS << "\n";
  OS << "\n";
}

//=================================================================================================

static void Print(const occ::handle<Geom_BezierCurve>& B, Standard_OStream& OS, const bool compact)
{
  if (compact)
    OS << BEZIER << " ";
  else
    OS << "BezierCurve";

  bool rational = B->IsRational();
  if (compact)
    OS << (rational ? 1 : 0) << " ";
  else
  {
    if (rational)
      OS << " rational";
  }

  // poles and weights
  int i, degree = B->Degree();
  if (!compact)
    OS << "\n  Degree :";
  OS << degree << " ";

  for (i = 1; i <= degree + 1; i++)
  {
    if (!compact)
      OS << "\n  " << std::setw(2) << i << " : ";
    Print(B->Pole(i), OS, compact);
    if (rational)
      OS << " " << B->Weight(i);
    if (compact)
      OS << " ";
  }
  OS << "\n";
  if (!compact)
    OS << "\n";
}

//=================================================================================================

static void Print(const occ::handle<Geom_BSplineCurve>& B, Standard_OStream& OS, const bool compact)
{
  if (compact)
    OS << BSPLINE << " ";
  else
    OS << "BSplineCurve";

  bool rational = B->IsRational();
  if (compact)
    OS << (rational ? 1 : 0) << " ";
  else
  {
    if (rational)
      OS << " rational";
  }

  bool periodic = B->IsPeriodic();
  if (compact)
    OS << (periodic ? 1 : 0) << " ";
  else
  {
    if (periodic)
      OS << " periodic";
  }

  // poles and weights
  int i, degree, nbpoles, nbknots;
  degree  = B->Degree();
  nbpoles = B->NbPoles();
  nbknots = B->NbKnots();
  if (!compact)
    OS << "\n  Degree ";
  else
    OS << " ";
  OS << degree;
  if (!compact)
    OS << ",";
  OS << " ";
  OS << nbpoles;
  if (!compact)
    OS << " Poles,";
  OS << " ";
  OS << nbknots << " ";
  if (!compact)
    OS << " Knots\n";

  if (!compact)
    OS << "Poles :\n";
  for (i = 1; i <= nbpoles; i++)
  {
    if (!compact)
      OS << "\n  " << std::setw(2) << i << " : ";
    else
      OS << " ";
    Print(B->Pole(i), OS, compact);
    if (rational)
      OS << " " << B->Weight(i);
  }
  OS << "\n";

  if (!compact)
    OS << "Knots :\n";
  for (i = 1; i <= nbknots; i++)
  {
    if (!compact)
      OS << "\n  " << std::setw(2) << i << " : ";
    OS << " " << B->Knot(i) << " " << B->Multiplicity(i);
  }

  OS << "\n";
  if (!compact)
    OS << "\n";
}

//=================================================================================================

static void Print(const occ::handle<Geom_TrimmedCurve>& C, Standard_OStream& OS, const bool compact)
{
  if (compact)
    OS << TRIMMED << " ";
  else
    OS << "Trimmed curve\n";
  if (!compact)
    OS << "Parameters : ";
  OS << C->FirstParameter() << " " << C->LastParameter() << "\n";
  if (!compact)
    OS << "Basis curve :\n";
  GeomTools_CurveSet::PrintCurve(C->BasisCurve(), OS, compact);
}

//=================================================================================================

static void Print(const occ::handle<Geom_OffsetCurve>& C, Standard_OStream& OS, const bool compact)
{
  if (compact)
    OS << OFFSET << " ";
  else
    OS << "OffsetCurve";
  if (!compact)
    OS << "Offset : ";
  OS << C->Offset() << "\n";
  if (!compact)
    OS << "Direction : ";
  Print(C->Direction(), OS, compact);
  OS << "\n";
  if (!compact)
    OS << "Basis curve :\n";
  GeomTools_CurveSet::PrintCurve(C->BasisCurve(), OS, compact);
}

//=================================================================================================

void GeomTools_CurveSet::PrintCurve(const occ::handle<Geom_Curve>& C,
                                    Standard_OStream&              OS,
                                    const bool                     compact)
{
  occ::handle<Standard_Type> TheType = C->DynamicType();

  if (TheType == STANDARD_TYPE(Geom_Line))
  {
    Print(occ::down_cast<Geom_Line>(C), OS, compact);
  }
  else if (TheType == STANDARD_TYPE(Geom_Circle))
  {
    Print(occ::down_cast<Geom_Circle>(C), OS, compact);
  }
  else if (TheType == STANDARD_TYPE(Geom_Ellipse))
  {
    Print(occ::down_cast<Geom_Ellipse>(C), OS, compact);
  }
  else if (TheType == STANDARD_TYPE(Geom_Parabola))
  {
    Print(occ::down_cast<Geom_Parabola>(C), OS, compact);
  }
  else if (TheType == STANDARD_TYPE(Geom_Hyperbola))
  {
    Print(occ::down_cast<Geom_Hyperbola>(C), OS, compact);
  }
  else if (TheType == STANDARD_TYPE(Geom_BezierCurve))
  {
    Print(occ::down_cast<Geom_BezierCurve>(C), OS, compact);
  }
  else if (TheType == STANDARD_TYPE(Geom_BSplineCurve))
  {
    Print(occ::down_cast<Geom_BSplineCurve>(C), OS, compact);
  }
  else if (TheType == STANDARD_TYPE(Geom_TrimmedCurve))
  {
    Print(occ::down_cast<Geom_TrimmedCurve>(C), OS, compact);
  }
  else if (TheType == STANDARD_TYPE(Geom_OffsetCurve))
  {
    Print(occ::down_cast<Geom_OffsetCurve>(C), OS, compact);
  }
  else
  {
    GeomTools::GetUndefinedTypeHandler()->PrintCurve(C, OS, compact);
    // if (!compact)
    //   OS << "****** UNKNOWN CURVE TYPE ******\n";
    // else
    //   std::cout << "****** UNKNOWN CURVE TYPE ******" << std::endl;
  }
}

//=================================================================================================

void GeomTools_CurveSet::Dump(Standard_OStream& OS) const
{
  int i, nbsurf = myMap.Extent();
  OS << "\n -------\n";
  OS << "Dump of " << nbsurf << " Curves ";
  OS << "\n -------\n\n";

  for (i = 1; i <= nbsurf; i++)
  {
    OS << std::setw(4) << i << " : ";
    PrintCurve(occ::down_cast<Geom_Curve>(myMap(i)), OS, false);
  }
}

//=================================================================================================

void GeomTools_CurveSet::Write(Standard_OStream& OS, const Message_ProgressRange& theProgress) const
{
  std::streamsize prec = OS.precision(17);

  int i, nbcurve = myMap.Extent();
  OS << "Curves " << nbcurve << "\n";
  Message_ProgressScope aPS(theProgress, "3D Curves", nbcurve);
  for (i = 1; i <= nbcurve && aPS.More(); i++, aPS.Next())
  {
    PrintCurve(occ::down_cast<Geom_Curve>(myMap(i)), OS, true);
  }
  OS.precision(prec);
}

//=================================================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, gp_Pnt& P)
{
  double X = 0., Y = 0., Z = 0.;
  GeomTools::GetReal(IS, X);
  GeomTools::GetReal(IS, Y);
  GeomTools::GetReal(IS, Z);
  P.SetCoord(X, Y, Z);
  return IS;
}

//=================================================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, gp_Dir& D)
{
  double X = 0., Y = 0., Z = 0.;
  GeomTools::GetReal(IS, X);
  GeomTools::GetReal(IS, Y);
  GeomTools::GetReal(IS, Z);
  D.SetCoord(X, Y, Z);
  return IS;
}

//=================================================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom_Line>& L)
{
  gp_Pnt P(0., 0., 0.);
  gp_Dir AX(gp_Dir::D::X);
  IS >> P >> AX;
  L = new Geom_Line(P, AX);
  return IS;
}

//=================================================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom_Circle>& C)
{
  gp_Pnt P(0., 0., 0.);
  gp_Dir A(gp_Dir::D::X), AX(gp_Dir::D::X), AY(gp_Dir::D::X);
  double R = 0.;
  IS >> P >> A >> AX >> AY;
  GeomTools::GetReal(IS, R);
  C = new Geom_Circle(gp_Ax2(P, A, AX), R);
  return IS;
}

//=================================================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom_Ellipse>& E)
{
  gp_Pnt P(0., 0., 0.);
  gp_Dir A(gp_Dir::D::X), AX(gp_Dir::D::X), AY(gp_Dir::D::X);
  double R1 = 0., R2 = 0.;
  IS >> P >> A >> AX >> AY;
  GeomTools::GetReal(IS, R1);
  GeomTools::GetReal(IS, R2);
  E = new Geom_Ellipse(gp_Ax2(P, A, AX), R1, R2);
  return IS;
}

//=================================================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom_Parabola>& C)
{
  gp_Pnt P(0., 0., 0.);
  gp_Dir A(gp_Dir::D::X), AX(gp_Dir::D::X), AY(gp_Dir::D::X);
  double R1 = 0.;
  IS >> P >> A >> AX >> AY;
  GeomTools::GetReal(IS, R1);
  C = new Geom_Parabola(gp_Ax2(P, A, AX), R1);
  return IS;
}

//=================================================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom_Hyperbola>& H)
{
  gp_Pnt P(0., 0., 0.);
  gp_Dir A(gp_Dir::D::X), AX(gp_Dir::D::X), AY(gp_Dir::D::X);
  double R1 = 0., R2 = 0.;
  IS >> P >> A >> AX >> AY;
  GeomTools::GetReal(IS, R1);
  GeomTools::GetReal(IS, R2);
  H = new Geom_Hyperbola(gp_Ax2(P, A, AX), R1, R2);
  return IS;
}

//=================================================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom_BezierCurve>& B)
{
  bool rational = false;
  IS >> rational;

  // poles and weights
  int i = 0, degree = 0;
  IS >> degree;

  NCollection_Array1<gp_Pnt> poles(1, degree + 1);
  NCollection_Array1<double> weights(1, degree + 1);

  for (i = 1; i <= degree + 1; i++)
  {
    IS >> poles(i);
    if (rational)
      GeomTools::GetReal(IS, weights(i));
  }

  if (rational)
    B = new Geom_BezierCurve(poles, weights);
  else
    B = new Geom_BezierCurve(poles);

  return IS;
}

//=================================================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom_BSplineCurve>& B)
{

  bool rational = false, periodic = false;
  IS >> rational >> periodic;

  // poles and weights
  int i = 0, degree = 0, nbpoles = 0, nbknots = 0;
  IS >> degree >> nbpoles >> nbknots;

  NCollection_Array1<gp_Pnt> poles(1, nbpoles);
  NCollection_Array1<double> weights(1, nbpoles);

  for (i = 1; i <= nbpoles; i++)
  {
    IS >> poles(i);
    if (rational)
      GeomTools::GetReal(IS, weights(i));
  }

  NCollection_Array1<double> knots(1, nbknots);
  NCollection_Array1<int>    mults(1, nbknots);

  for (i = 1; i <= nbknots; i++)
  {
    GeomTools::GetReal(IS, knots(i));
    IS >> mults(i);
  }

  if (rational)
    B = new Geom_BSplineCurve(poles, weights, knots, mults, degree, periodic);
  else
    B = new Geom_BSplineCurve(poles, knots, mults, degree, periodic);

  return IS;
}

//=================================================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom_TrimmedCurve>& C)
{
  double p1 = 0., p2 = 0.;
  GeomTools::GetReal(IS, p1);
  GeomTools::GetReal(IS, p2);
  occ::handle<Geom_Curve> BC = GeomTools_CurveSet::ReadCurve(IS);
  C                          = new Geom_TrimmedCurve(BC, p1, p2);
  return IS;
}

//=================================================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom_OffsetCurve>& C)
{
  double p = 0.;
  GeomTools::GetReal(IS, p);
  gp_Dir D(gp_Dir::D::X);
  IS >> D;
  occ::handle<Geom_Curve> BC = GeomTools_CurveSet::ReadCurve(IS);
  C                          = new Geom_OffsetCurve(BC, p, D);
  return IS;
}

//=================================================================================================

occ::handle<Geom_Curve> GeomTools_CurveSet::ReadCurve(Standard_IStream& IS)
{
  int ctype;

  occ::handle<Geom_Curve> C;
  try
  {
    OCC_CATCH_SIGNALS
    IS >> ctype;
    switch (ctype)
    {

      case LINE: {
        occ::handle<Geom_Line> CC;
        IS >> CC;
        C = CC;
      }
      break;

      case CIRCLE: {
        occ::handle<Geom_Circle> CC;
        IS >> CC;
        C = CC;
      }
      break;

      case ELLIPSE: {
        occ::handle<Geom_Ellipse> CC;
        IS >> CC;
        C = CC;
      }
      break;

      case PARABOLA: {
        occ::handle<Geom_Parabola> CC;
        IS >> CC;
        C = CC;
      }
      break;

      case HYPERBOLA: {
        occ::handle<Geom_Hyperbola> CC;
        IS >> CC;
        C = CC;
      }
      break;

      case BEZIER: {
        occ::handle<Geom_BezierCurve> CC;
        IS >> CC;
        C = CC;
      }
      break;

      case BSPLINE: {
        occ::handle<Geom_BSplineCurve> CC;
        IS >> CC;
        C = CC;
      }
      break;

      case TRIMMED: {
        occ::handle<Geom_TrimmedCurve> CC;
        IS >> CC;
        C = CC;
      }
      break;

      case OFFSET: {
        occ::handle<Geom_OffsetCurve> CC;
        IS >> CC;
        C = CC;
      }
      break;

      default: {
        occ::handle<Geom_Curve> CC;
        GeomTools::GetUndefinedTypeHandler()->ReadCurve(ctype, IS, CC);
        C = CC;
      }
    }
  }
  catch (Standard_Failure const& anException)
  {
#ifdef OCCT_DEBUG
    std::cout << "EXCEPTION in GeomTools_CurveSet::ReadCurve(..)!!!" << std::endl;
    std::cout << anException << std::endl;
#endif
    (void)anException;
  }
  return C;
}

//=================================================================================================

void GeomTools_CurveSet::Read(Standard_IStream& IS, const Message_ProgressRange& theProgress)
{
  char buffer[255];
  IS >> buffer;
  if (strcmp(buffer, "Curves"))
  {
    std::cout << "Not a Curve table" << std::endl;
    return;
  }

  int i, nbcurve;
  IS >> nbcurve;
  Message_ProgressScope aPS(theProgress, "3D Curves", nbcurve);
  for (i = 1; i <= nbcurve && aPS.More(); i++, aPS.Next())
  {
    occ::handle<Geom_Curve> C = GeomTools_CurveSet::ReadCurve(IS);
    myMap.Add(C);
  }
}
