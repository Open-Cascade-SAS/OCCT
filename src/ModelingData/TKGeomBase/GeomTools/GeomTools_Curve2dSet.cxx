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
#include <Message_ProgressScope.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Macro.hxx>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <gp_Pnt2d.hxx>
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

GeomTools_Curve2dSet::GeomTools_Curve2dSet() = default;

//=================================================================================================

void GeomTools_Curve2dSet::Clear()
{
  myMap.Clear();
}

//=================================================================================================

int GeomTools_Curve2dSet::Add(const occ::handle<Geom2d_Curve>& S)
{
  return myMap.Add(S);
}

//=================================================================================================

occ::handle<Geom2d_Curve> GeomTools_Curve2dSet::Curve2d(const int I) const
{
  if (I <= 0 || I > myMap.Extent())
    return occ::handle<Geom2d_Curve>();

  return occ::down_cast<Geom2d_Curve>(myMap(I));
}

//=================================================================================================

int GeomTools_Curve2dSet::Index(const occ::handle<Geom2d_Curve>& S) const
{
  return myMap.FindIndex(S);
}

//=================================================================================================

static void Print(const gp_Pnt2d P, Standard_OStream& OS, const bool compact)
{
  OS << P.X();
  if (!compact)
    OS << ",";
  OS << " ";
  OS << P.Y();
  OS << " ";
}

//=================================================================================================

static void Print(const gp_Dir2d D, Standard_OStream& OS, const bool compact)
{
  OS << D.X();
  if (!compact)
    OS << ",";
  OS << " ";
  OS << D.Y();
  OS << " ";
}

//=================================================================================================

static void Print(const occ::handle<Geom2d_Line>& L, Standard_OStream& OS, const bool compact)
{
  if (compact)
    OS << LINE << " ";
  else
    OS << "Line";

  gp_Lin2d C2d = L->Lin2d();
  if (!compact)
    OS << "\n  Origin :";
  Print(C2d.Location(), OS, compact);
  if (!compact)
    OS << "\n  Axis   :";
  Print(C2d.Direction(), OS, compact);
  if (!compact)
    OS << "\n";
  OS << "\n";
}

//=================================================================================================

static void Print(const occ::handle<Geom2d_Circle>& C, Standard_OStream& OS, const bool compact)
{
  if (compact)
    OS << CIRCLE << " ";
  else
    OS << "Circle";

  gp_Circ2d C2d = C->Circ2d();
  if (!compact)
    OS << "\n  Center :";
  Print(C2d.Location(), OS, compact);
  if (!compact)
    OS << "\n  XAxis  :";
  Print(C2d.XAxis().Direction(), OS, compact);
  if (!compact)
    OS << "\n  YAxis  :";
  Print(C2d.YAxis().Direction(), OS, compact);
  if (!compact)
    OS << "\n  Radius :";
  OS << C2d.Radius();
  if (!compact)
    OS << "\n";
  OS << "\n";
}

//=================================================================================================

static void Print(const occ::handle<Geom2d_Ellipse>& E, Standard_OStream& OS, const bool compact)
{
  if (compact)
    OS << ELLIPSE << " ";
  else
    OS << "Ellipse";

  gp_Elips2d C2d = E->Elips2d();
  if (!compact)
    OS << "\n  Center :";
  Print(C2d.Location(), OS, compact);
  if (!compact)
    OS << "\n  XAxis  :";
  Print(C2d.XAxis().Direction(), OS, compact);
  if (!compact)
    OS << "\n  YAxis  :";
  Print(C2d.YAxis().Direction(), OS, compact);
  if (!compact)
    OS << "\n  Radii  :";
  OS << C2d.MajorRadius();
  if (!compact)
    OS << ",";
  OS << " ";
  OS << C2d.MinorRadius();
  if (!compact)
    OS << "\n";
  OS << "\n";
}

//=================================================================================================

static void Print(const occ::handle<Geom2d_Parabola>& P, Standard_OStream& OS, const bool compact)
{
  if (compact)
    OS << PARABOLA << " ";
  else
    OS << "Parabola";

  gp_Parab2d C2d = P->Parab2d();
  if (!compact)
    OS << "\n  Center :";
  Print(C2d.Location(), OS, compact);
  if (!compact)
    OS << "\n  XAxis  :";
  Print(C2d.Axis().XAxis().Direction(), OS, compact);
  if (!compact)
    OS << "\n  YAxis  :";
  Print(C2d.Axis().YAxis().Direction(), OS, compact);
  if (!compact)
    OS << "\n  Focal  :";
  OS << C2d.Focal();
  if (!compact)
    OS << "\n";
  OS << "\n";
}

//=================================================================================================

static void Print(const occ::handle<Geom2d_Hyperbola>& H, Standard_OStream& OS, const bool compact)
{
  if (compact)
    OS << HYPERBOLA << " ";
  else
    OS << "Hyperbola";

  gp_Hypr2d C2d = H->Hypr2d();
  if (!compact)
    OS << "\n  Center :";
  Print(C2d.Location(), OS, compact);
  if (!compact)
    OS << "\n  XAxis  :";
  Print(C2d.XAxis().Direction(), OS, compact);
  if (!compact)
    OS << "\n  YAxis  :";
  Print(C2d.YAxis().Direction(), OS, compact);
  if (!compact)
    OS << "\n  Radii  :";
  OS << C2d.MajorRadius();
  if (!compact)
    OS << ",";
  OS << " ";
  OS << C2d.MinorRadius();
  if (!compact)
    OS << "\n";
  OS << "\n";
}

//=================================================================================================

static void Print(const occ::handle<Geom2d_BezierCurve>& B,
                  Standard_OStream&                      OS,
                  const bool                             compact)
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
  if (!compact)
    OS << "\n";
  OS << "\n";
}

//=================================================================================================

static void Print(const occ::handle<Geom2d_BSplineCurve>& B,
                  Standard_OStream&                       OS,
                  const bool                              compact)
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
    OS << " Knots";

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
    else
      OS << " ";
    OS << B->Knot(i) << " " << B->Multiplicity(i);
  }
  if (!compact)
    OS << "\n";
  OS << "\n";
}

//=================================================================================================

static void Print(const occ::handle<Geom2d_TrimmedCurve>& C,
                  Standard_OStream&                       OS,
                  const bool                              compact)
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
  GeomTools_Curve2dSet::PrintCurve2d(C->BasisCurve(), OS, compact);
}

//=================================================================================================

static void Print(const occ::handle<Geom2d_OffsetCurve>& C,
                  Standard_OStream&                      OS,
                  const bool                             compact)
{
  if (compact)
    OS << OFFSET << " ";
  else
    OS << "OffsetCurve";
  if (!compact)
    OS << "Offset : ";
  OS << C->Offset() << "\n";
  if (!compact)
    OS << "Basis curve :\n";
  GeomTools_Curve2dSet::PrintCurve2d(C->BasisCurve(), OS, compact);
}

//=================================================================================================

void GeomTools_Curve2dSet::PrintCurve2d(const occ::handle<Geom2d_Curve>& C,
                                        Standard_OStream&                OS,
                                        const bool                       compact)
{
  occ::handle<Standard_Type> TheType = C->DynamicType();

  if (TheType == STANDARD_TYPE(Geom2d_Circle))
  {
    Print(occ::down_cast<Geom2d_Circle>(C), OS, compact);
  }
  else if (TheType == STANDARD_TYPE(Geom2d_Line))
  {
    Print(occ::down_cast<Geom2d_Line>(C), OS, compact);
  }
  else if (TheType == STANDARD_TYPE(Geom2d_Ellipse))
  {
    Print(occ::down_cast<Geom2d_Ellipse>(C), OS, compact);
  }
  else if (TheType == STANDARD_TYPE(Geom2d_Parabola))
  {
    Print(occ::down_cast<Geom2d_Parabola>(C), OS, compact);
  }
  else if (TheType == STANDARD_TYPE(Geom2d_Hyperbola))
  {
    Print(occ::down_cast<Geom2d_Hyperbola>(C), OS, compact);
  }
  else if (TheType == STANDARD_TYPE(Geom2d_BezierCurve))
  {
    Print(occ::down_cast<Geom2d_BezierCurve>(C), OS, compact);
  }
  else if (TheType == STANDARD_TYPE(Geom2d_BSplineCurve))
  {
    Print(occ::down_cast<Geom2d_BSplineCurve>(C), OS, compact);
  }
  else if (TheType == STANDARD_TYPE(Geom2d_TrimmedCurve))
  {
    Print(occ::down_cast<Geom2d_TrimmedCurve>(C), OS, compact);
  }
  else if (TheType == STANDARD_TYPE(Geom2d_OffsetCurve))
  {
    Print(occ::down_cast<Geom2d_OffsetCurve>(C), OS, compact);
  }
  else
  {
    GeomTools::GetUndefinedTypeHandler()->PrintCurve2d(C, OS, compact);
    // if (!compact)
    //   OS << "****** UNKNOWN CURVE2d TYPE ******\n";
    // else
    //   std::cout << "****** UNKNOWN CURVE2d TYPE ******" << std::endl;
  }
}

//=================================================================================================

void GeomTools_Curve2dSet::Dump(Standard_OStream& OS) const
{
  int i, nbsurf = myMap.Extent();
  OS << "\n -------\n";
  OS << "Dump of " << nbsurf << " Curve2ds ";
  OS << "\n -------\n\n";

  for (i = 1; i <= nbsurf; i++)
  {
    OS << std::setw(4) << i << " : ";
    PrintCurve2d(occ::down_cast<Geom2d_Curve>(myMap(i)), OS, false);
  }
}

//=================================================================================================

void GeomTools_Curve2dSet::Write(Standard_OStream&            OS,
                                 const Message_ProgressRange& theProgress) const
{
  std::streamsize prec = OS.precision(17);

  int i, nbsurf = myMap.Extent();
  OS << "Curve2ds " << nbsurf << "\n";
  Message_ProgressScope aPS(theProgress, "2D Curves", nbsurf);
  for (i = 1; i <= nbsurf && aPS.More(); i++, aPS.Next())
  {
    PrintCurve2d(occ::down_cast<Geom2d_Curve>(myMap(i)), OS, true);
  }
  OS.precision(prec);
}

//=================================================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, gp_Pnt2d& P)
{
  double X = 0., Y = 0.;
  GeomTools::GetReal(IS, X);
  GeomTools::GetReal(IS, Y);
  P.SetCoord(X, Y);
  return IS;
}

//=================================================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, gp_Dir2d& D)
{
  double X = 0., Y = 0.;
  GeomTools::GetReal(IS, X);
  GeomTools::GetReal(IS, Y);
  D.SetCoord(X, Y);
  return IS;
}

//=================================================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom2d_Line>& L)
{
  gp_Pnt2d P(0., 0.);
  gp_Dir2d AX(gp_Dir2d::D::X);
  IS >> P >> AX;
  L = new Geom2d_Line(P, AX);
  return IS;
}

//=================================================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom2d_Circle>& C)
{
  gp_Pnt2d P(0., 0.);
  gp_Dir2d AX(gp_Dir2d::D::X), AY(gp_Dir2d::D::X);
  double   R = 0.;
  IS >> P >> AX >> AY;
  GeomTools::GetReal(IS, R);
  C = new Geom2d_Circle(gp_Ax22d(P, AX, AY), R);
  return IS;
}

//=================================================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom2d_Ellipse>& E)
{
  gp_Pnt2d P(0., 0.);
  gp_Dir2d AX(gp_Dir2d::D::X), AY(gp_Dir2d::D::X);
  double   R1 = 0., R2 = 0.;
  IS >> P >> AX >> AY;
  GeomTools::GetReal(IS, R1);
  GeomTools::GetReal(IS, R2);
  E = new Geom2d_Ellipse(gp_Ax22d(P, AX, AY), R1, R2);
  return IS;
}

//=================================================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom2d_Parabola>& C)
{
  gp_Pnt2d P(0., 0.);
  gp_Dir2d AX(gp_Dir2d::D::X), AY(gp_Dir2d::D::X);
  double   R1 = 0.;
  IS >> P >> AX >> AY;
  GeomTools::GetReal(IS, R1);
  C = new Geom2d_Parabola(gp_Ax22d(P, AX, AY), R1);
  return IS;
}

//=================================================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom2d_Hyperbola>& H)
{
  gp_Pnt2d P(0., 0.);
  gp_Dir2d AX(gp_Dir2d::D::X), AY(gp_Dir2d::D::X);
  double   R1 = 0., R2 = 0.;
  IS >> P >> AX >> AY;
  GeomTools::GetReal(IS, R1);
  GeomTools::GetReal(IS, R2);
  H = new Geom2d_Hyperbola(gp_Ax22d(P, AX, AY), R1, R2);
  return IS;
}

//=================================================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom2d_BezierCurve>& B)
{
  bool rational = false;
  IS >> rational;

  // poles and weights
  int i = 0, degree = 0;
  IS >> degree;

  NCollection_Array1<gp_Pnt2d> poles(1, degree + 1);
  NCollection_Array1<double>   weights(1, degree + 1);

  for (i = 1; i <= degree + 1; i++)
  {
    IS >> poles(i);
    if (rational)
      GeomTools::GetReal(IS, weights(i));
  }

  if (rational)
    B = new Geom2d_BezierCurve(poles, weights);
  else
    B = new Geom2d_BezierCurve(poles);

  return IS;
}

//=================================================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom2d_BSplineCurve>& B)
{
  bool rational = false, periodic = false;
  IS >> rational >> periodic;

  // poles and weights
  int i = 0, degree = 0, nbpoles = 0, nbknots = 0;
  IS >> degree >> nbpoles >> nbknots;

  NCollection_Array1<gp_Pnt2d> poles(1, nbpoles);
  NCollection_Array1<double>   weights(1, nbpoles);

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
    B = new Geom2d_BSplineCurve(poles, weights, knots, mults, degree, periodic);
  else
    B = new Geom2d_BSplineCurve(poles, knots, mults, degree, periodic);

  return IS;
}

//=================================================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom2d_TrimmedCurve>& C)
{
  double p1 = 0., p2 = 0.;
  GeomTools::GetReal(IS, p1);
  GeomTools::GetReal(IS, p2);
  occ::handle<Geom2d_Curve> BC = GeomTools_Curve2dSet::ReadCurve2d(IS);
  C                            = new Geom2d_TrimmedCurve(BC, p1, p2);
  return IS;
}

//=================================================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom2d_OffsetCurve>& C)
{
  double p = 0.;
  GeomTools::GetReal(IS, p);
  occ::handle<Geom2d_Curve> BC = GeomTools_Curve2dSet::ReadCurve2d(IS);
  C                            = new Geom2d_OffsetCurve(BC, p);
  return IS;
}

//=================================================================================================

occ::handle<Geom2d_Curve> GeomTools_Curve2dSet::ReadCurve2d(Standard_IStream& IS)
{
  int ctype;

  occ::handle<Geom2d_Curve> C;
  try
  {
    OCC_CATCH_SIGNALS
    IS >> ctype;
    switch (ctype)
    {

      case LINE: {
        occ::handle<Geom2d_Line> CC;
        IS >> CC;
        C = CC;
      }
      break;

      case CIRCLE: {
        occ::handle<Geom2d_Circle> CC;
        IS >> CC;
        C = CC;
      }
      break;

      case ELLIPSE: {
        occ::handle<Geom2d_Ellipse> CC;
        IS >> CC;
        C = CC;
      }
      break;

      case PARABOLA: {
        occ::handle<Geom2d_Parabola> CC;
        IS >> CC;
        C = CC;
      }
      break;

      case HYPERBOLA: {
        occ::handle<Geom2d_Hyperbola> CC;
        IS >> CC;
        C = CC;
      }
      break;

      case BEZIER: {
        occ::handle<Geom2d_BezierCurve> CC;
        IS >> CC;
        C = CC;
      }
      break;

      case BSPLINE: {
        occ::handle<Geom2d_BSplineCurve> CC;
        IS >> CC;
        C = CC;
      }
      break;

      case TRIMMED: {
        occ::handle<Geom2d_TrimmedCurve> CC;
        IS >> CC;
        C = CC;
      }
      break;

      case OFFSET: {
        occ::handle<Geom2d_OffsetCurve> CC;
        IS >> CC;
        C = CC;
      }
      break;

      default: {
        occ::handle<Geom2d_Curve> CC;
        GeomTools::GetUndefinedTypeHandler()->ReadCurve2d(ctype, IS, CC);
        C = CC;
      }
      break;
    }
  }
  catch (Standard_Failure const& anException)
  {
#ifdef OCCT_DEBUG
    std::cout << "EXCEPTION in GeomTools_Curve2dSet::ReadCurve2d(..)!!!" << std::endl;
    std::cout << anException << std::endl;
#endif
    (void)anException;
  }
  return C;
}

//=================================================================================================

void GeomTools_Curve2dSet::Read(Standard_IStream& IS, const Message_ProgressRange& theProgress)
{
  char buffer[255];
  IS >> buffer;
  if (strcmp(buffer, "Curve2ds"))
  {
    std::cout << "Not a Curve2d table" << std::endl;
    return;
  }

  int i, nbcurve;
  IS >> nbcurve;
  Message_ProgressScope aPS(theProgress, "2D Curves", nbcurve);
  for (i = 1; i <= nbcurve && aPS.More(); i++, aPS.Next())
  {
    occ::handle<Geom2d_Curve> C = GeomTools_Curve2dSet::ReadCurve2d(IS);
    myMap.Add(C);
  }
}
