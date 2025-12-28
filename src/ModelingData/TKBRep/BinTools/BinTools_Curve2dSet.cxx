// Created on: 2004-05-18
// Created by: Sergey ZARITCHNY
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

#include <BinTools.hxx>
#include <BinTools_Curve2dSet.hxx>
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
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab2d.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Macro.hxx>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <Message_ProgressScope.hxx>

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

BinTools_Curve2dSet::BinTools_Curve2dSet() {}

//=================================================================================================

void BinTools_Curve2dSet::Clear()
{
  myMap.Clear();
}

//=================================================================================================

int BinTools_Curve2dSet::Add(const occ::handle<Geom2d_Curve>& S)
{
  return myMap.Add(S);
}

//=================================================================================================

occ::handle<Geom2d_Curve> BinTools_Curve2dSet::Curve2d(const int I) const
{
  return occ::down_cast<Geom2d_Curve>(myMap(I));
}

//=================================================================================================

int BinTools_Curve2dSet::Index(const occ::handle<Geom2d_Curve>& S) const
{
  return myMap.FindIndex(S);
}

//=======================================================================
// function : operator << ((Geom2d_Line)& L)
// purpose  :
//=======================================================================

static BinTools_OStream& operator<<(BinTools_OStream& OS, const occ::handle<Geom2d_Line>& L)
{

  OS << (uint8_t)LINE;
  gp_Lin2d C2d = L->Lin2d();
  OS << C2d.Location();  // Pnt2d
  OS << C2d.Direction(); // Dir2d
  return OS;
}

//=======================================================================
// function : operator << ((Geom2d_Circle)& C)
// purpose  :
//=======================================================================

static BinTools_OStream& operator<<(BinTools_OStream& OS, const occ::handle<Geom2d_Circle>& C)
{
  OS << (uint8_t)CIRCLE;
  gp_Circ2d C2d = C->Circ2d();
  OS << C2d.Location();
  OS << C2d.XAxis().Direction();
  OS << C2d.YAxis().Direction();
  OS << C2d.Radius();
  return OS;
}

//=======================================================================
// function : operator <<(Geom2d_Ellipse)
// purpose  :
//=======================================================================

static BinTools_OStream& operator<<(BinTools_OStream& OS, const occ::handle<Geom2d_Ellipse>& E)
{
  OS << (uint8_t)ELLIPSE;
  gp_Elips2d C2d = E->Elips2d();
  OS << C2d.Location();
  OS << C2d.XAxis().Direction();
  OS << C2d.YAxis().Direction();
  OS << C2d.MajorRadius();
  OS << C2d.MinorRadius();
  return OS;
}

//=======================================================================
// function :  operator << (Geom2d_Parabola)
// purpose  :
//=======================================================================

static BinTools_OStream& operator<<(BinTools_OStream& OS, const occ::handle<Geom2d_Parabola>& P)
{
  OS << (uint8_t)PARABOLA;
  gp_Parab2d C2d = P->Parab2d();
  OS << C2d.Location();                 // Loc
  OS << C2d.Axis().XAxis().Direction(); // XDir
  OS << C2d.Axis().YAxis().Direction(); // YDir
  OS << C2d.Focal();                    // Focal
  return OS;
}

//=======================================================================
// function : operator <<(Geom2d_Hyperbola)
// purpose  :
//=======================================================================

static BinTools_OStream& operator<<(BinTools_OStream& OS, const occ::handle<Geom2d_Hyperbola>& H)
{
  OS << (uint8_t)HYPERBOLA;
  gp_Hypr2d C2d = H->Hypr2d();
  OS << C2d.Location();          // Loc
  OS << C2d.XAxis().Direction(); // XDir
  OS << C2d.YAxis().Direction(); // YDir
  OS << C2d.MajorRadius();       // MajR
  OS << C2d.MinorRadius();
  return OS;
}

//=======================================================================
// function : operator <<(Geom2d_BezierCurve)
// purpose  :
//=======================================================================

static BinTools_OStream& operator<<(BinTools_OStream& OS, const occ::handle<Geom2d_BezierCurve>& B)
{
  OS << (uint8_t)BEZIER;
  bool aRational = B->IsRational() ? 1 : 0;
  OS << aRational; // rational
  // poles and weights
  int i, aDegree = B->Degree();
  OS << (char16_t)aDegree; // Degree
  for (i = 1; i <= aDegree + 1; i++)
  {
    OS << B->Pole(i); // Pnt2d
    if (aRational)
      OS << B->Weight(i); // Real
  }
  return OS;
}

//=======================================================================
// function : operator <<(Geom2d_BSplineCurve)
// purpose  :
//=======================================================================

static BinTools_OStream& operator<<(BinTools_OStream& OS, const occ::handle<Geom2d_BSplineCurve>& B)
{
  OS << (uint8_t)BSPLINE;
  bool aRational = B->IsRational() ? 1 : 0;
  OS << aRational; // rational
  bool aPeriodic = B->IsPeriodic() ? 1 : 0;
  OS << aPeriodic; // periodic
  // poles and weights
  int i, aDegree, aNbPoles, aNbKnots;
  aDegree  = B->Degree();
  aNbPoles = B->NbPoles();
  aNbKnots = B->NbKnots();
  OS << (char16_t)aDegree;
  OS << aNbPoles;
  OS << aNbKnots;
  for (i = 1; i <= aNbPoles; i++)
  {
    OS << B->Pole(i); // Pnt2d
    if (aRational)
      OS << B->Weight(i);
  }

  for (i = 1; i <= aNbKnots; i++)
  {
    OS << B->Knot(i);
    OS << B->Multiplicity(i);
  }

  return OS;
}

//=======================================================================
// function : operator <<(Geom2d_TrimmedCurve)
// purpose  :
//=======================================================================

static BinTools_OStream& operator<<(BinTools_OStream& OS, const occ::handle<Geom2d_TrimmedCurve>& C)
{
  OS << (uint8_t)TRIMMED;
  OS << C->FirstParameter();
  OS << C->LastParameter();
  BinTools_Curve2dSet::WriteCurve2d(C->BasisCurve(), OS);
  return OS;
}

//=======================================================================
// function : operator <<(Geom2d_OffsetCurve)
// purpose  :
//=======================================================================

static BinTools_OStream& operator<<(BinTools_OStream& OS, const occ::handle<Geom2d_OffsetCurve>& C)
{
  OS << (uint8_t)OFFSET;
  OS << C->Offset(); // Offset
  BinTools_Curve2dSet::WriteCurve2d(C->BasisCurve(), OS);
  return OS;
}

//=================================================================================================

void BinTools_Curve2dSet::WriteCurve2d(const occ::handle<Geom2d_Curve>& C, BinTools_OStream& OS)
{
  occ::handle<Standard_Type> TheType = C->DynamicType();
  try
  {
    OCC_CATCH_SIGNALS
    if (TheType == STANDARD_TYPE(Geom2d_Circle))
    {
      OS << occ::down_cast<Geom2d_Circle>(C);
    }
    else if (TheType == STANDARD_TYPE(Geom2d_Line))
    {
      OS << occ::down_cast<Geom2d_Line>(C);
    }
    else if (TheType == STANDARD_TYPE(Geom2d_Ellipse))
    {
      OS << occ::down_cast<Geom2d_Ellipse>(C);
    }
    else if (TheType == STANDARD_TYPE(Geom2d_Parabola))
    {
      OS << occ::down_cast<Geom2d_Parabola>(C);
    }
    else if (TheType == STANDARD_TYPE(Geom2d_Hyperbola))
    {
      OS << occ::down_cast<Geom2d_Hyperbola>(C);
    }
    else if (TheType == STANDARD_TYPE(Geom2d_BezierCurve))
    {
      OS << occ::down_cast<Geom2d_BezierCurve>(C);
    }
    else if (TheType == STANDARD_TYPE(Geom2d_BSplineCurve))
    {
      OS << occ::down_cast<Geom2d_BSplineCurve>(C);
    }
    else if (TheType == STANDARD_TYPE(Geom2d_TrimmedCurve))
    {
      OS << occ::down_cast<Geom2d_TrimmedCurve>(C);
    }
    else if (TheType == STANDARD_TYPE(Geom2d_OffsetCurve))
    {
      OS << occ::down_cast<Geom2d_OffsetCurve>(C);
    }
    else
    {
      throw Standard_Failure("UNKNOWN CURVE2d TYPE");
    }
  }
  catch (Standard_Failure const& anException)
  {
    Standard_SStream aMsg;
    aMsg << "EXCEPTION in BinTools_Curve2dSet::WriteCurve2d(..)" << std::endl;
    aMsg << anException << std::endl;
    throw Standard_Failure(aMsg.str().c_str());
  }
}

//=================================================================================================

void BinTools_Curve2dSet::Write(Standard_OStream& OS, const Message_ProgressRange& theRange) const
{
  int                   i, aNbCurves = myMap.Extent();
  Message_ProgressScope aPS(theRange, "Writing 2D curves", aNbCurves);
  OS << "Curve2ds " << aNbCurves << "\n";
  BinTools_OStream aStream(OS);
  for (i = 1; i <= aNbCurves && aPS.More(); i++, aPS.Next())
  {
    WriteCurve2d(occ::down_cast<Geom2d_Curve>(myMap(i)), aStream);
  }
}

//=================================================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, gp_Pnt2d& P)
{
  double X = 0., Y = 0.;
  BinTools::GetReal(IS, X);
  BinTools::GetReal(IS, Y);
  P.SetCoord(X, Y);
  return IS;
}

//=================================================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, gp_Dir2d& D)
{
  double X = 0., Y = 0.;
  BinTools::GetReal(IS, X);
  BinTools::GetReal(IS, Y);
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
  BinTools::GetReal(IS, R);
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
  BinTools::GetReal(IS, R1);
  BinTools::GetReal(IS, R2);
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
  BinTools::GetReal(IS, R1);
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
  BinTools::GetReal(IS, R1);
  BinTools::GetReal(IS, R2);
  H = new Geom2d_Hyperbola(gp_Ax22d(P, AX, AY), R1, R2);
  return IS;
}

//=================================================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom2d_BezierCurve>& B)
{
  bool rational = false;
  BinTools::GetBool(IS, rational);

  // poles and weights
  int i = 0, degree = 0;
  // degree;
  char16_t aVal = '\0';
  BinTools::GetExtChar(IS, aVal);
  degree = (int)aVal;

  NCollection_Array1<gp_Pnt2d> poles(1, degree + 1);
  NCollection_Array1<double>   weights(1, degree + 1);

  for (i = 1; i <= degree + 1; i++)
  {
    IS >> poles(i); // Pnt2d
    if (rational)
      BinTools::GetReal(IS, weights(i));
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
  BinTools::GetBool(IS, rational);
  BinTools::GetBool(IS, periodic);
  // poles and weights
  int      i = 0, degree = 0, nbpoles = 0, nbknots = 0;
  char16_t aVal = '\0';

  BinTools::GetExtChar(IS, aVal);
  degree = (int)aVal;

  BinTools::GetInteger(IS, nbpoles);

  BinTools::GetInteger(IS, nbknots);

  NCollection_Array1<gp_Pnt2d> poles(1, nbpoles);
  NCollection_Array1<double>   weights(1, nbpoles);

  for (i = 1; i <= nbpoles; i++)
  {
    IS >> poles(i); // Pnt2d
    if (rational)
      BinTools::GetReal(IS, weights(i));
  }

  NCollection_Array1<double> knots(1, nbknots);
  NCollection_Array1<int>    mults(1, nbknots);

  for (i = 1; i <= nbknots; i++)
  {
    BinTools::GetReal(IS, knots(i));
    BinTools::GetInteger(IS, mults(i));
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
  BinTools::GetReal(IS, p1); // FirstParameter
  BinTools::GetReal(IS, p2); // LastParameter
  occ::handle<Geom2d_Curve> BC;
  BinTools_Curve2dSet::ReadCurve2d(IS, BC);
  C = new Geom2d_TrimmedCurve(BC, p1, p2);
  return IS;
}

//=================================================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom2d_OffsetCurve>& C)
{
  double p = 0.;
  BinTools::GetReal(IS, p); // Offset
  occ::handle<Geom2d_Curve> BC;
  BinTools_Curve2dSet::ReadCurve2d(IS, BC);
  C = new Geom2d_OffsetCurve(BC, p);
  return IS;
}

//=================================================================================================

Standard_IStream& BinTools_Curve2dSet::ReadCurve2d(Standard_IStream&          IS,
                                                   occ::handle<Geom2d_Curve>& C)
{
  try
  {
    OCC_CATCH_SIGNALS
    const uint8_t ctype = (uint8_t)IS.get();
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
        C = NULL;
        throw Standard_Failure("UNKNOWN CURVE2d TYPE");
      }
      break;
    }
  }
  catch (Standard_Failure const& anException)
  {
    C = NULL;
    Standard_SStream aMsg;
    aMsg << "EXCEPTION in BinTools_Curve2dSet::ReadCurve2d(...)" << std::endl;
    aMsg << anException << std::endl;
    throw Standard_Failure(aMsg.str().c_str());
  }
  return IS;
}

//=================================================================================================

void BinTools_Curve2dSet::Read(Standard_IStream& IS, const Message_ProgressRange& theRange)
{
  char buffer[255];

  IS >> buffer;
  if (IS.fail() || strcmp(buffer, "Curve2ds"))
  {
    Standard_SStream aMsg;
    aMsg << "BinTools_Curve2dSet::Read:  Not a Curve2d table" << std::endl;
#ifdef OCCT_DEBUG
    std::cout << "Curve2dSet buffer: " << buffer << std::endl;
#endif
    throw Standard_Failure(aMsg.str().c_str());
    return;
  }

  occ::handle<Geom2d_Curve> C;
  int                       i, aNbCurves;
  IS >> aNbCurves;
  Message_ProgressScope aPS(theRange, "Reading curves 2d", aNbCurves);
  IS.get(); // remove <lf>
  for (i = 1; i <= aNbCurves && aPS.More(); i++, aPS.Next())
  {
    BinTools_Curve2dSet::ReadCurve2d(IS, C);
    myMap.Add(C);
  }
}
