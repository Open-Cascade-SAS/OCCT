// Created on: 2004-05-20
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
#include <BinTools_CurveSet.hxx>
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
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Parab.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array1.hxx>
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

BinTools_CurveSet::BinTools_CurveSet() {}

//=================================================================================================

void BinTools_CurveSet::Clear()
{
  myMap.Clear();
}

//=================================================================================================

int BinTools_CurveSet::Add(const occ::handle<Geom_Curve>& C)
{
  return (C.IsNull()) ? 0 : myMap.Add(C);
}

//=================================================================================================

occ::handle<Geom_Curve> BinTools_CurveSet::Curve(const int I) const
{
  if (I == 0)
  {
    occ::handle<Geom_Curve> dummy;
    return dummy;
  }
  else
    return occ::down_cast<Geom_Curve>(myMap(I));
}

//=================================================================================================

int BinTools_CurveSet::Index(const occ::handle<Geom_Curve>& S) const
{
  return S.IsNull() ? 0 : myMap.FindIndex(S);
}

//=======================================================================
// function : operator << (Geom_Line)
// purpose  :
//=======================================================================

static BinTools_OStream& operator<<(BinTools_OStream& OS, const occ::handle<Geom_Line>& L)
{
  OS << (uint8_t)LINE;
  gp_Lin C = L->Lin();
  OS << C.Location();  // Pnt
  OS << C.Direction(); // Dir
  return OS;
}

//=======================================================================
// function :  operator <<(Geom_Circle)
// purpose  :
//=======================================================================

static BinTools_OStream& operator<<(BinTools_OStream& OS, const occ::handle<Geom_Circle>& CC)
{
  OS << (uint8_t)CIRCLE;
  gp_Circ C = CC->Circ();
  OS << C.Location();
  OS << C.Axis().Direction();
  OS << C.XAxis().Direction();
  OS << C.YAxis().Direction();
  OS << C.Radius();
  return OS;
}

//=======================================================================
// function : operator <<(Geom_Ellipse)
// purpose  :
//=======================================================================

static BinTools_OStream& operator<<(BinTools_OStream& OS, const occ::handle<Geom_Ellipse>& E)
{
  OS << (uint8_t)ELLIPSE;
  gp_Elips C = E->Elips();
  OS << C.Location();
  OS << C.Axis().Direction();
  OS << C.XAxis().Direction();
  OS << C.YAxis().Direction();
  OS << C.MajorRadius();
  OS << C.MinorRadius();
  return OS;
}

//=======================================================================
// function : operator <<(Geom_Parabola)
// purpose  :
//=======================================================================

static BinTools_OStream& operator<<(BinTools_OStream& OS, const occ::handle<Geom_Parabola>& P)
{
  OS << (uint8_t)PARABOLA;
  gp_Parab C = P->Parab();
  OS << C.Location();
  OS << C.Axis().Direction();
  OS << C.XAxis().Direction();
  OS << C.YAxis().Direction();
  OS << C.Focal();
  return OS;
}

//=======================================================================
// function : operator <<(Geom_Hyperbola)
// purpose  :
//=======================================================================

static BinTools_OStream& operator<<(BinTools_OStream& OS, const occ::handle<Geom_Hyperbola>& H)
{
  OS << (uint8_t)HYPERBOLA;
  gp_Hypr C = H->Hypr();
  OS << C.Location();
  OS << C.Axis().Direction();
  OS << C.XAxis().Direction();
  OS << C.YAxis().Direction();
  OS << C.MajorRadius();
  OS << C.MinorRadius();
  return OS;
}

//=======================================================================
// function : operator <<(Geom_BezierCurve)
// purpose  :
//=======================================================================

static BinTools_OStream& operator<<(BinTools_OStream& OS, const occ::handle<Geom_BezierCurve>& B)
{
  OS << (uint8_t)BEZIER;
  bool aRational = B->IsRational() ? 1 : 0;
  OS << aRational; // rational
  // poles and weights
  int i, aDegree = B->Degree();
  OS << (char16_t)aDegree; //<< Degree
  for (i = 1; i <= aDegree + 1; i++)
  {
    OS << B->Pole(i); // Pnt
    if (aRational)
      OS << B->Weight(i); // Real
  }
  return OS;
}

//=======================================================================
// function : operator <<(Geom_BSplineCurve)
// purpose  :
//=======================================================================

static BinTools_OStream& operator<<(BinTools_OStream& OS, const occ::handle<Geom_BSplineCurve>& B)
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
    OS << B->Pole(i); // Pnt
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
// function : operator <<(Geom_TrimmedCurve)
// purpose  :
//=======================================================================

static BinTools_OStream& operator<<(BinTools_OStream& OS, const occ::handle<Geom_TrimmedCurve>& C)
{
  OS << (uint8_t)TRIMMED;
  OS << C->FirstParameter();
  OS << C->LastParameter();
  BinTools_CurveSet::WriteCurve(C->BasisCurve(), OS);
  return OS;
}

//=======================================================================
// function : operator <<(Geom_OffsetCurve)
// purpose  :
//=======================================================================

static BinTools_OStream& operator<<(BinTools_OStream& OS, const occ::handle<Geom_OffsetCurve>& C)
{
  OS << (uint8_t)OFFSET;
  OS << C->Offset(); // Offset
  OS << C->Direction();
  BinTools_CurveSet::WriteCurve(C->BasisCurve(), OS);
  return OS;
}

//=================================================================================================

void BinTools_CurveSet::WriteCurve(const occ::handle<Geom_Curve>& C, BinTools_OStream& OS)
{
  occ::handle<Standard_Type> TheType = C->DynamicType();
  try
  {
    OCC_CATCH_SIGNALS
    if (TheType == STANDARD_TYPE(Geom_Line))
    {
      OS << occ::down_cast<Geom_Line>(C);
    }
    else if (TheType == STANDARD_TYPE(Geom_Circle))
    {
      OS << occ::down_cast<Geom_Circle>(C);
    }
    else if (TheType == STANDARD_TYPE(Geom_Ellipse))
    {
      OS << occ::down_cast<Geom_Ellipse>(C);
    }
    else if (TheType == STANDARD_TYPE(Geom_Parabola))
    {
      OS << occ::down_cast<Geom_Parabola>(C);
    }
    else if (TheType == STANDARD_TYPE(Geom_Hyperbola))
    {
      OS << occ::down_cast<Geom_Hyperbola>(C);
    }
    else if (TheType == STANDARD_TYPE(Geom_BezierCurve))
    {
      OS << occ::down_cast<Geom_BezierCurve>(C);
    }
    else if (TheType == STANDARD_TYPE(Geom_BSplineCurve))
    {
      OS << occ::down_cast<Geom_BSplineCurve>(C);
    }
    else if (TheType == STANDARD_TYPE(Geom_TrimmedCurve))
    {
      OS << occ::down_cast<Geom_TrimmedCurve>(C);
    }
    else if (TheType == STANDARD_TYPE(Geom_OffsetCurve))
    {
      OS << occ::down_cast<Geom_OffsetCurve>(C);
    }
    else
    {
      throw Standard_Failure("UNKNOWN CURVE TYPE");
    }
  }
  catch (Standard_Failure const& anException)
  {
    Standard_SStream aMsg;
    aMsg << "EXCEPTION in BinTools_CurveSet::WriteCurve(..)" << std::endl;
    aMsg << anException << std::endl;
    throw Standard_Failure(aMsg.str().c_str());
  }
}

//=================================================================================================

void BinTools_CurveSet::Write(Standard_OStream& OS, const Message_ProgressRange& theRange) const
{
  int      i, nbcurv = myMap.Extent();
  Message_ProgressScope aPS(theRange, "Writing curves", nbcurv);
  OS << "Curves " << nbcurv << "\n";
  BinTools_OStream aStream(OS);
  for (i = 1; i <= nbcurv && aPS.More(); i++, aPS.Next())
  {
    WriteCurve(occ::down_cast<Geom_Curve>(myMap(i)), aStream);
  }
}

//=================================================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, gp_Pnt& P)
{
  double X = 0., Y = 0., Z = 0.;
  BinTools::GetReal(IS, X);
  BinTools::GetReal(IS, Y);
  BinTools::GetReal(IS, Z);
  P.SetCoord(X, Y, Z);
  return IS;
}

//=================================================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, gp_Dir& D)
{
  double X = 0., Y = 0., Z = 0.;
  BinTools::GetReal(IS, X);
  BinTools::GetReal(IS, Y);
  BinTools::GetReal(IS, Z);
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
  gp_Pnt        P(0., 0., 0.);
  gp_Dir        A(gp_Dir::D::X), AX(gp_Dir::D::X), AY(gp_Dir::D::X);
  double R = 0.;
  IS >> P >> A >> AX >> AY;
  BinTools::GetReal(IS, R);
  C = new Geom_Circle(gp_Ax2(P, A, AX), R);
  return IS;
}

//=================================================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom_Ellipse>& E)
{
  gp_Pnt        P(0., 0., 0.);
  gp_Dir        A(gp_Dir::D::X), AX(gp_Dir::D::X), AY(gp_Dir::D::X);
  double R1 = 0., R2 = 0.;
  IS >> P >> A >> AX >> AY;
  BinTools::GetReal(IS, R1);
  BinTools::GetReal(IS, R2);
  E = new Geom_Ellipse(gp_Ax2(P, A, AX), R1, R2);
  return IS;
}

//=================================================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom_Parabola>& C)
{
  gp_Pnt        P(0., 0., 0.);
  gp_Dir        A(gp_Dir::D::X), AX(gp_Dir::D::X), AY(gp_Dir::D::X);
  double R1 = 0.;
  IS >> P >> A >> AX >> AY;
  BinTools::GetReal(IS, R1);
  C = new Geom_Parabola(gp_Ax2(P, A, AX), R1);
  return IS;
}

//=================================================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom_Hyperbola>& H)
{
  gp_Pnt        P(0., 0., 0.);
  gp_Dir        A(gp_Dir::D::X), AX(gp_Dir::D::X), AY(gp_Dir::D::X);
  double R1 = 0., R2 = 0.;
  IS >> P >> A >> AX >> AY;
  BinTools::GetReal(IS, R1);
  BinTools::GetReal(IS, R2);
  H = new Geom_Hyperbola(gp_Ax2(P, A, AX), R1, R2);
  return IS;
}

//=================================================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom_BezierCurve>& B)
{
  bool rational = false;
  BinTools::GetBool(IS, rational);

  // poles and weights
  int i = 0, degree = 0;
  // degree;
  char16_t aVal = '\0';
  BinTools::GetExtChar(IS, aVal);
  degree = (int)aVal;

  NCollection_Array1<gp_Pnt>   poles(1, degree + 1);
  NCollection_Array1<double> weights(1, degree + 1);

  for (i = 1; i <= degree + 1; i++)
  {
    IS >> poles(i); // Pnt
    if (rational)
      // weights(i);
      BinTools::GetReal(IS, weights(i));
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
  BinTools::GetBool(IS, rational);
  BinTools::GetBool(IS, periodic);

  // poles and weights
  int      i = 0, degree = 0, nbpoles = 0, nbknots = 0;
  char16_t aVal = '\0';
  BinTools::GetExtChar(IS, aVal);
  degree = (int)aVal;

  BinTools::GetInteger(IS, nbpoles);

  BinTools::GetInteger(IS, nbknots);

  NCollection_Array1<gp_Pnt>   poles(1, nbpoles);
  NCollection_Array1<double> weights(1, nbpoles);

  for (i = 1; i <= nbpoles; i++)
  {
    IS >> poles(i); // Pnt
    if (rational)
      BinTools::GetReal(IS, weights(i));
  }

  NCollection_Array1<double>    knots(1, nbknots);
  NCollection_Array1<int> mults(1, nbknots);

  for (i = 1; i <= nbknots; i++)
  {
    BinTools::GetReal(IS, knots(i));
    BinTools::GetInteger(IS, mults(i));
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
  BinTools::GetReal(IS, p1); // FirstParameter
  BinTools::GetReal(IS, p2); // LastParameter
  occ::handle<Geom_Curve> BC;
  BinTools_CurveSet::ReadCurve(IS, BC);
  C = new Geom_TrimmedCurve(BC, p1, p2);
  return IS;
}

//=================================================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom_OffsetCurve>& C)
{
  double p = 0.;
  BinTools::GetReal(IS, p); // Offset
  gp_Dir D(gp_Dir::D::X);
  IS >> D;
  occ::handle<Geom_Curve> BC;
  BinTools_CurveSet::ReadCurve(IS, BC);
  C = new Geom_OffsetCurve(BC, p, D);
  return IS;
}

//=================================================================================================

Standard_IStream& BinTools_CurveSet::ReadCurve(Standard_IStream& IS, occ::handle<Geom_Curve>& C)
{
  try
  {
    OCC_CATCH_SIGNALS
    const uint8_t ctype = (uint8_t)IS.get();

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
        C = NULL;
        throw Standard_Failure("UNKNOWN CURVE TYPE");
      }
    }
  }
  catch (Standard_Failure const& anException)
  {
    C = NULL;
    Standard_SStream aMsg;
    aMsg << "EXCEPTION in BinTools_CurveSet::ReadCurve(..)" << std::endl;
    aMsg << anException << std::endl;
    throw Standard_Failure(aMsg.str().c_str());
  }
  return IS;
}

//=================================================================================================

void BinTools_CurveSet::Read(Standard_IStream& IS, const Message_ProgressRange& theRange)
{
  char buffer[255];
  IS >> buffer;
  if (IS.fail() || strcmp(buffer, "Curves"))
  {
    Standard_SStream aMsg;
    aMsg << "BinTools_CurveSet::Read:  Not a Curve table" << std::endl;
#ifdef OCCT_DEBUG
    std::cout << "CurveSet buffer: " << buffer << std::endl;
#endif
    throw Standard_Failure(aMsg.str().c_str());
    return;
  }

  occ::handle<Geom_Curve> C;
  int   i, nbcurve;
  IS >> nbcurve;

  Message_ProgressScope aPS(theRange, "Reading curves", nbcurve);

  IS.get(); // remove <lf>
  for (i = 1; i <= nbcurve && aPS.More(); i++, aPS.Next())
  {
    BinTools_CurveSet::ReadCurve(IS, C);
    myMap.Add(C);
  }
}
