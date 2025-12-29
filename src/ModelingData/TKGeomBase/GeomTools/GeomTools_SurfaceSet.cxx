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
#include <Message_ProgressScope.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Macro.hxx>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <gp_Pnt.hxx>
#include <NCollection_Array2.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>

#define PLANE 1
#define CYLINDER 2
#define CONE 3
#define SPHERE 4
#define TORUS 5
#define LINEAREXTRUSION 6
#define REVOLUTION 7
#define BEZIER 8
#define BSPLINE 9
#define RECTANGULAR 10
#define OFFSET 11

//=================================================================================================

GeomTools_SurfaceSet::GeomTools_SurfaceSet() = default;

//=================================================================================================

void GeomTools_SurfaceSet::Clear()
{
  myMap.Clear();
}

//=================================================================================================

int GeomTools_SurfaceSet::Add(const occ::handle<Geom_Surface>& S)
{
  return myMap.Add(S);
}

//=================================================================================================

occ::handle<Geom_Surface> GeomTools_SurfaceSet::Surface(const int I) const
{
  if (I <= 0 || I > myMap.Extent())
    return occ::handle<Geom_Surface>();
  return occ::down_cast<Geom_Surface>(myMap(I));
}

//=================================================================================================

int GeomTools_SurfaceSet::Index(const occ::handle<Geom_Surface>& S) const
{
  return myMap.FindIndex(S);
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

static void Print(const occ::handle<Geom_Plane>& S, Standard_OStream& OS, const bool compact)
{
  if (compact)
    OS << PLANE << " ";
  else
    OS << "Plane";

  gp_Pln P = S->Pln();
  if (!compact)
    OS << "\n  Origin :";
  Print(P.Location(), OS, compact);
  if (!compact)
    OS << "\n  Axis   :";
  Print(P.Axis().Direction(), OS, compact);
  if (!compact)
    OS << "\n  XAxis  :";
  Print(P.XAxis().Direction(), OS, compact);
  if (!compact)
    OS << "\n  YAxis  :";
  Print(P.YAxis().Direction(), OS, compact);
  OS << "\n";
  if (!compact)
    OS << "\n";
}

//=================================================================================================

static void Print(const occ::handle<Geom_CylindricalSurface>& S,
                  Standard_OStream&                           OS,
                  const bool                                  compact)
{
  if (compact)
    OS << CYLINDER << " ";
  else
    OS << "CylindricalSurface";

  gp_Cylinder P = S->Cylinder();
  if (!compact)
    OS << "\n  Origin :";
  Print(P.Location(), OS, compact);
  if (!compact)
    OS << "\n  Axis   :";
  Print(P.Axis().Direction(), OS, compact);
  if (!compact)
    OS << "\n  XAxis  :";
  Print(P.XAxis().Direction(), OS, compact);
  if (!compact)
    OS << "\n  YAxis  :";
  Print(P.YAxis().Direction(), OS, compact);
  if (!compact)
    OS << "\n  Radius :";
  OS << P.Radius();
  OS << "\n";
  if (!compact)
    OS << "\n";
}

//=================================================================================================

static void Print(const occ::handle<Geom_ConicalSurface>& S,
                  Standard_OStream&                       OS,
                  const bool                              compact)
{
  if (compact)
    OS << CONE << " ";
  else
    OS << "ConicalSurface";

  gp_Cone P = S->Cone();
  if (!compact)
    OS << "\n  Origin :";
  Print(P.Location(), OS, compact);
  if (!compact)
    OS << "\n  Axis   :";
  Print(P.Axis().Direction(), OS, compact);
  if (!compact)
    OS << "\n  XAxis  :";
  Print(P.XAxis().Direction(), OS, compact);
  if (!compact)
    OS << "\n  YAxis  :";
  Print(P.YAxis().Direction(), OS, compact);
  if (!compact)
    OS << "\n  Radius :";
  OS << P.RefRadius();
  OS << "\n";
  if (!compact)
    OS << "\n  Angle :";
  OS << P.SemiAngle();
  OS << "\n";
  if (!compact)
    OS << "\n";
}

//=================================================================================================

static void Print(const occ::handle<Geom_SphericalSurface>& S,
                  Standard_OStream&                         OS,
                  const bool                                compact)
{
  if (compact)
    OS << SPHERE << " ";
  else
    OS << "SphericalSurface";

  gp_Sphere P = S->Sphere();
  if (!compact)
    OS << "\n  Center :";
  Print(P.Location(), OS, compact);
  if (!compact)
    OS << "\n  Axis   :";
  Print(P.Position().Axis().Direction(), OS, compact);
  if (!compact)
    OS << "\n  XAxis  :";
  Print(P.XAxis().Direction(), OS, compact);
  if (!compact)
    OS << "\n  YAxis  :";
  Print(P.YAxis().Direction(), OS, compact);
  if (!compact)
    OS << "\n  Radius :";
  OS << P.Radius();
  OS << "\n";
  if (!compact)
    OS << "\n";
}

//=================================================================================================

static void Print(const occ::handle<Geom_ToroidalSurface>& S,
                  Standard_OStream&                        OS,
                  const bool                               compact)
{
  if (compact)
    OS << TORUS << " ";
  else
    OS << "ToroidalSurface";

  gp_Torus P = S->Torus();
  if (!compact)
    OS << "\n  Origin :";
  Print(P.Location(), OS, compact);
  if (!compact)
    OS << "\n  Axis   :";
  Print(P.Axis().Direction(), OS, compact);
  if (!compact)
    OS << "\n  XAxis  :";
  Print(P.XAxis().Direction(), OS, compact);
  if (!compact)
    OS << "\n  YAxis  :";
  Print(P.YAxis().Direction(), OS, compact);
  if (!compact)
    OS << "\n  Radii  :";
  OS << P.MajorRadius() << " " << P.MinorRadius();
  OS << "\n";
  if (!compact)
    OS << "\n";
}

//=================================================================================================

static void Print(const occ::handle<Geom_SurfaceOfLinearExtrusion>& S,
                  Standard_OStream&                                 OS,
                  const bool                                        compact)
{
  if (compact)
    OS << LINEAREXTRUSION << " ";
  else
    OS << "SurfaceOfLinearExtrusion";

  if (!compact)
    OS << "\n  Direction :";
  Print(S->Direction(), OS, compact);
  if (!compact)
    OS << "\n  Basis curve : ";
  OS << "\n";
  GeomTools_CurveSet::PrintCurve(S->BasisCurve(), OS, compact);
}

//=================================================================================================

static void Print(const occ::handle<Geom_SurfaceOfRevolution>& S,
                  Standard_OStream&                            OS,
                  const bool                                   compact)
{
  if (compact)
    OS << REVOLUTION << " ";
  else
    OS << "SurfaceOfRevolution";

  if (!compact)
    OS << "\n  Origin    :";
  Print(S->Location(), OS, compact);
  if (!compact)
    OS << "\n  Direction :";
  Print(S->Direction(), OS, compact);
  if (!compact)
    OS << "\n  Basis curve : ";
  OS << "\n";
  GeomTools_CurveSet::PrintCurve(S->BasisCurve(), OS, compact);
}

//=================================================================================================

static void Print(const occ::handle<Geom_BezierSurface>& S,
                  Standard_OStream&                      OS,
                  const bool                             compact)
{
  if (compact)
    OS << BEZIER << " ";
  else
    OS << "BezierSurface";

  bool urational = S->IsURational();
  bool vrational = S->IsVRational();
  if (compact)
    OS << (urational ? 1 : 0) << " ";
  else
  {
    if (urational)
      OS << " urational";
  }
  if (compact)
    OS << (vrational ? 1 : 0) << " ";
  else
  {
    if (vrational)
      OS << " vrational";
  }

  if (!compact)
  {
    bool uclosed = S->IsUClosed();
    bool vclosed = S->IsVClosed();
    if (uclosed)
      OS << " uclosed";
    if (vclosed)
      OS << " vclosed";
  }

  // poles and weights
  int i, j, udegree, vdegree;
  udegree = S->UDegree();
  vdegree = S->VDegree();
  if (!compact)
    OS << "\n  Degrees :";
  OS << udegree << " " << vdegree << " ";

  for (i = 1; i <= udegree + 1; i++)
  {
    for (j = 1; j <= vdegree + 1; j++)
    {
      if (!compact)
        OS << "\n  " << std::setw(2) << i << ", " << std::setw(2) << j << " : ";
      Print(S->Pole(i, j), OS, compact);
      if (urational || vrational)
        OS << " " << S->Weight(i, j);
      if (compact)
        OS << " ";
    }
    OS << "\n";
  }
  OS << "\n";
  if (!compact)
    OS << "\n";
}

//=================================================================================================

static void Print(const occ::handle<Geom_BSplineSurface>& S,
                  Standard_OStream&                       OS,
                  const bool                              compact)
{
  if (compact)
    OS << BSPLINE << " ";
  else
    OS << "BSplineSurface";

  bool urational = S->IsURational();
  bool vrational = S->IsVRational();
  if (compact)
    OS << (urational ? 1 : 0) << " ";
  else
  {
    if (urational)
      OS << " urational";
  }
  if (compact)
    OS << (vrational ? 1 : 0) << " ";
  else
  {
    if (vrational)
      OS << " vrational";
  }

  bool uperiodic = S->IsUPeriodic();
  bool vperiodic = S->IsVPeriodic();
  if (compact)
    OS << (uperiodic ? 1 : 0) << " ";
  else
  {
    if (uperiodic)
      OS << " uperiodic";
  }
  if (compact)
    OS << (vperiodic ? 1 : 0) << " ";
  else
  {
    if (vperiodic)
      OS << " vperiodic";
  }

  if (!compact)
  {
    bool uclosed = S->IsUClosed();
    bool vclosed = S->IsVClosed();
    if (uclosed)
      OS << " uclosed";
    if (vclosed)
      OS << " vclosed";
  }

  // poles and weights
  int i, j, udegree, vdegree, nbupoles, nbvpoles, nbuknots, nbvknots;
  udegree  = S->UDegree();
  vdegree  = S->VDegree();
  nbupoles = S->NbUPoles();
  nbvpoles = S->NbVPoles();
  nbuknots = S->NbUKnots();
  nbvknots = S->NbVKnots();
  if (!compact)
    OS << "\n  Degrees :";
  OS << udegree << " " << vdegree << " ";
  if (!compact)
    OS << "\n  NbPoles :";
  OS << nbupoles << " " << nbvpoles << " ";
  if (!compact)
    OS << "\n  NbKnots :";
  OS << nbuknots << " " << nbvknots << " ";

  if (!compact)
    OS << "\n Poles :\n";
  for (i = 1; i <= nbupoles; i++)
  {
    for (j = 1; j <= nbvpoles; j++)
    {
      if (!compact)
        OS << "\n  " << std::setw(2) << i << ", " << std::setw(2) << j << " : ";
      Print(S->Pole(i, j), OS, compact);
      if (urational || vrational)
        OS << " " << S->Weight(i, j);
      if (compact)
        OS << " ";
    }
    OS << "\n";
  }
  OS << "\n";
  if (!compact)
    OS << "\n UKnots :\n";
  for (i = 1; i <= nbuknots; i++)
  {
    if (!compact)
      OS << "\n  " << std::setw(2) << i << " : ";
    OS << S->UKnot(i) << " " << S->UMultiplicity(i) << "\n";
  }
  OS << "\n";
  if (!compact)
    OS << "\n VKnots :\n";
  for (i = 1; i <= nbvknots; i++)
  {
    if (!compact)
      OS << "\n  " << std::setw(2) << i << " : ";
    OS << S->VKnot(i) << " " << S->VMultiplicity(i) << "\n";
  }
  OS << "\n";
  if (!compact)
    OS << "\n";
}

//=================================================================================================

static void Print(const occ::handle<Geom_RectangularTrimmedSurface>& S,
                  Standard_OStream&                                  OS,
                  const bool                                         compact)
{
  if (compact)
    OS << RECTANGULAR << " ";
  else
    OS << "RectangularTrimmedSurface";

  double U1, U2, V1, V2;
  S->Bounds(U1, U2, V1, V2);
  if (!compact)
    OS << "\nParameters : ";
  OS << U1 << " " << U2 << " " << V1 << " " << V2 << "\n";
  if (!compact)
    OS << "BasisSurface :\n";
  GeomTools_SurfaceSet::PrintSurface(S->BasisSurface(), OS, compact);
}

//=================================================================================================

static void Print(const occ::handle<Geom_OffsetSurface>& S,
                  Standard_OStream&                      OS,
                  const bool                             compact)
{
  if (compact)
    OS << OFFSET << " ";
  else
    OS << "OffsetSurface";

  if (!compact)
    OS << "\nOffset : ";
  OS << S->Offset() << "\n";
  if (!compact)
    OS << "BasisSurface :\n";
  GeomTools_SurfaceSet::PrintSurface(S->BasisSurface(), OS, compact);
}

//=================================================================================================

void GeomTools_SurfaceSet::PrintSurface(const occ::handle<Geom_Surface>& S,
                                        Standard_OStream&                OS,
                                        const bool                       compact)
{
  occ::handle<Standard_Type> TheType = S->DynamicType();

  if (TheType == STANDARD_TYPE(Geom_Plane))
  {
    Print(occ::down_cast<Geom_Plane>(S), OS, compact);
  }
  else if (TheType == STANDARD_TYPE(Geom_CylindricalSurface))
  {
    Print(occ::down_cast<Geom_CylindricalSurface>(S), OS, compact);
  }
  else if (TheType == STANDARD_TYPE(Geom_ConicalSurface))
  {
    Print(occ::down_cast<Geom_ConicalSurface>(S), OS, compact);
  }
  else if (TheType == STANDARD_TYPE(Geom_SphericalSurface))
  {
    Print(occ::down_cast<Geom_SphericalSurface>(S), OS, compact);
  }
  else if (TheType == STANDARD_TYPE(Geom_ToroidalSurface))
  {
    Print(occ::down_cast<Geom_ToroidalSurface>(S), OS, compact);
  }
  else if (TheType == STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion))
  {
    Print(occ::down_cast<Geom_SurfaceOfLinearExtrusion>(S), OS, compact);
  }
  else if (TheType == STANDARD_TYPE(Geom_SurfaceOfRevolution))
  {
    Print(occ::down_cast<Geom_SurfaceOfRevolution>(S), OS, compact);
  }
  else if (TheType == STANDARD_TYPE(Geom_BezierSurface))
  {
    Print(occ::down_cast<Geom_BezierSurface>(S), OS, compact);
  }
  else if (TheType == STANDARD_TYPE(Geom_BSplineSurface))
  {
    Print(occ::down_cast<Geom_BSplineSurface>(S), OS, compact);
  }
  else if (TheType == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
  {
    Print(occ::down_cast<Geom_RectangularTrimmedSurface>(S), OS, compact);
  }
  else if (TheType == STANDARD_TYPE(Geom_OffsetSurface))
  {
    Print(occ::down_cast<Geom_OffsetSurface>(S), OS, compact);
  }
  else
  {
    GeomTools::GetUndefinedTypeHandler()->PrintSurface(S, OS, compact);
    // if (!compact)
    //   OS << "***** Unknown Surface ********\n";
    // else
    //   std::cout << "***** Unknown Surface ********"<<std::endl;
  }
}

//=================================================================================================

void GeomTools_SurfaceSet::Dump(Standard_OStream& OS) const
{
  int i, nbsurf = myMap.Extent();
  OS << "\n -------\n";
  OS << "Dump of " << nbsurf << " surfaces ";
  OS << "\n -------\n\n";

  for (i = 1; i <= nbsurf; i++)
  {
    OS << std::setw(4) << i << " : ";
    PrintSurface(occ::down_cast<Geom_Surface>(myMap(i)), OS, false);
  }
}

//=================================================================================================

void GeomTools_SurfaceSet::Write(Standard_OStream&            OS,
                                 const Message_ProgressRange& theProgress) const
{
  std::streamsize prec = OS.precision(17);

  int i, nbsurf = myMap.Extent();
  OS << "Surfaces " << nbsurf << "\n";
  Message_ProgressScope aPS(theProgress, "Surfaces", nbsurf);
  for (i = 1; i <= nbsurf && aPS.More(); i++, aPS.Next())
  {
    PrintSurface(occ::down_cast<Geom_Surface>(myMap(i)), OS, true);
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

static Standard_IStream& operator>>(Standard_IStream& IS, gp_Ax3& A3)
{
  gp_Pnt P(0., 0., 0.);
  gp_Dir A(gp_Dir::D::X), AX(gp_Dir::D::X), AY(gp_Dir::D::X);
  IS >> P >> A >> AX >> AY;
  gp_Ax3 ax3(P, A, AX);
  if (AY.DotCross(A, AX) < 0)
    ax3.YReverse();
  A3 = ax3;
  return IS;
}

//=======================================================================
// function : operator>>
// purpose  :
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom_Plane>& S)
{
  gp_Ax3 A;
  IS >> A;
  S = new Geom_Plane(A);
  return IS;
}

//=======================================================================
// function : operator>>
// purpose  :
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom_CylindricalSurface>& S)
{
  gp_Ax3 A;
  double R = 0.;
  IS >> A;
  GeomTools::GetReal(IS, R);
  S = new Geom_CylindricalSurface(A, R);
  return IS;
}

//=======================================================================
// function : operator>>
// purpose  :
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom_ConicalSurface>& S)
{
  gp_Ax3 A;
  double R = 0., Ang = 0.;
  IS >> A;
  GeomTools::GetReal(IS, R);
  GeomTools::GetReal(IS, Ang);
  S = new Geom_ConicalSurface(A, Ang, R);
  return IS;
}

//=======================================================================
// function : operator>>
// purpose  :
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom_SphericalSurface>& S)
{
  gp_Ax3 A;
  double R = 0.;
  IS >> A;
  GeomTools::GetReal(IS, R);
  S = new Geom_SphericalSurface(A, R);
  return IS;
}

//=======================================================================
// function : operator>>
// purpose  :
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom_ToroidalSurface>& S)
{
  gp_Ax3 A;
  double R1 = 0., R2 = 0.;
  IS >> A;
  GeomTools::GetReal(IS, R1);
  GeomTools::GetReal(IS, R2);
  S = new Geom_ToroidalSurface(A, R1, R2);
  return IS;
}

//=======================================================================
// function : operator>>
// purpose  :
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream&                           IS,
                                    occ::handle<Geom_SurfaceOfLinearExtrusion>& S)
{
  gp_Dir D(gp_Dir::D::X);
  IS >> D;
  occ::handle<Geom_Curve> C = GeomTools_CurveSet::ReadCurve(IS);
  S                         = new Geom_SurfaceOfLinearExtrusion(C, D);
  return IS;
}

//=======================================================================
// function : operator>>
// purpose  :
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom_SurfaceOfRevolution>& S)
{
  gp_Pnt P(0., 0., 0.);
  gp_Dir D(gp_Dir::D::X);
  IS >> P >> D;
  occ::handle<Geom_Curve> C = GeomTools_CurveSet::ReadCurve(IS);
  S                         = new Geom_SurfaceOfRevolution(C, gp_Ax1(P, D));
  return IS;
}

//=======================================================================
// function : operator>>
// purpose  :
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom_BezierSurface>& S)
{
  bool urational = false, vrational = false;
  IS >> urational >> vrational;
  int udegree = 0, vdegree = 0;
  IS >> udegree >> vdegree;
  NCollection_Array2<gp_Pnt> poles(1, udegree + 1, 1, vdegree + 1);
  NCollection_Array2<double> weights(1, udegree + 1, 1, vdegree + 1);

  int i, j;
  for (i = 1; i <= udegree + 1; i++)
  {
    for (j = 1; j <= vdegree + 1; j++)
    {
      IS >> poles(i, j);
      if (urational || vrational)
        GeomTools::GetReal(IS, weights(i, j));
    }
  }

  if (urational || vrational)
    S = new Geom_BezierSurface(poles, weights);
  else
    S = new Geom_BezierSurface(poles);
  return IS;
}

//=======================================================================
// function : operator>>
// purpose  :
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom_BSplineSurface>& S)
{
  bool urational = false, vrational = false, uperiodic = false, vperiodic = false;
  IS >> urational >> vrational;
  IS >> uperiodic >> vperiodic;
  int udegree = 0, vdegree = 0, nbupoles = 0, nbvpoles = 0, nbuknots = 0, nbvknots = 0;
  IS >> udegree >> vdegree;
  IS >> nbupoles >> nbvpoles;
  IS >> nbuknots >> nbvknots;

  NCollection_Array2<gp_Pnt> poles(1, nbupoles, 1, nbvpoles);
  NCollection_Array2<double> weights(1, nbupoles, 1, nbvpoles);

  int i, j;
  for (i = 1; i <= nbupoles; i++)
  {
    for (j = 1; j <= nbvpoles; j++)
    {
      IS >> poles(i, j);
      if (urational || vrational)
        GeomTools::GetReal(IS, weights(i, j));
    }
  }

  NCollection_Array1<double> uknots(1, nbuknots);
  NCollection_Array1<int>    umults(1, nbuknots);
  for (i = 1; i <= nbuknots; i++)
  {
    GeomTools::GetReal(IS, uknots(i));
    IS >> umults(i);
  }

  NCollection_Array1<double> vknots(1, nbvknots);
  NCollection_Array1<int>    vmults(1, nbvknots);
  for (i = 1; i <= nbvknots; i++)
  {
    GeomTools::GetReal(IS, vknots(i));
    IS >> vmults(i);
  }

  if (urational || vrational)
    S = new Geom_BSplineSurface(poles,
                                weights,
                                uknots,
                                vknots,
                                umults,
                                vmults,
                                udegree,
                                vdegree,
                                uperiodic,
                                vperiodic);
  else
    S = new Geom_BSplineSurface(poles,
                                uknots,
                                vknots,
                                umults,
                                vmults,
                                udegree,
                                vdegree,
                                uperiodic,
                                vperiodic);
  return IS;
}

//=======================================================================
// function : operator>>
// purpose  :
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream&                            IS,
                                    occ::handle<Geom_RectangularTrimmedSurface>& S)
{
  double U1 = 0., U2 = 0., V1 = 0., V2 = 0.;
  GeomTools::GetReal(IS, U1);
  GeomTools::GetReal(IS, U2);
  GeomTools::GetReal(IS, V1);
  GeomTools::GetReal(IS, V2);
  occ::handle<Geom_Surface> BS = GeomTools_SurfaceSet::ReadSurface(IS);
  S                            = new Geom_RectangularTrimmedSurface(BS, U1, U2, V1, V2);
  return IS;
}

//=======================================================================
// function : operator>>
// purpose  :
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom_OffsetSurface>& S)
{
  double O = 0.;
  GeomTools::GetReal(IS, O);
  occ::handle<Geom_Surface> BS = GeomTools_SurfaceSet::ReadSurface(IS);
  S                            = new Geom_OffsetSurface(BS, O, true);
  return IS;
}

//=================================================================================================

occ::handle<Geom_Surface> GeomTools_SurfaceSet::ReadSurface(Standard_IStream& IS)
{
  int stype;

  occ::handle<Geom_Surface> S;
  try
  {
    OCC_CATCH_SIGNALS
    IS >> stype;
    switch (stype)
    {

      case PLANE: {
        occ::handle<Geom_Plane> SS;
        IS >> SS;
        S = SS;
      }
      break;

      case CYLINDER: {
        occ::handle<Geom_CylindricalSurface> SS;
        IS >> SS;
        S = SS;
      }
      break;

      case CONE: {
        occ::handle<Geom_ConicalSurface> SS;
        IS >> SS;
        S = SS;
      }
      break;

      case SPHERE: {
        occ::handle<Geom_SphericalSurface> SS;
        IS >> SS;
        S = SS;
      }
      break;

      case TORUS: {
        occ::handle<Geom_ToroidalSurface> SS;
        IS >> SS;
        S = SS;
      }
      break;

      case LINEAREXTRUSION: {
        occ::handle<Geom_SurfaceOfLinearExtrusion> SS;
        IS >> SS;
        S = SS;
      }
      break;

      case REVOLUTION: {
        occ::handle<Geom_SurfaceOfRevolution> SS;
        IS >> SS;
        S = SS;
      }
      break;

      case BEZIER: {
        occ::handle<Geom_BezierSurface> SS;
        IS >> SS;
        S = SS;
      }
      break;

      case BSPLINE: {
        occ::handle<Geom_BSplineSurface> SS;
        IS >> SS;
        S = SS;
      }
      break;

      case RECTANGULAR: {
        occ::handle<Geom_RectangularTrimmedSurface> SS;
        IS >> SS;
        S = SS;
      }
      break;

      case OFFSET: {
        occ::handle<Geom_OffsetSurface> SS;
        IS >> SS;
        S = SS;
      }
      break;

      default: {
        occ::handle<Geom_Surface> SS;
        GeomTools::GetUndefinedTypeHandler()->ReadSurface(stype, IS, SS);
        S = SS;
      }
      break;
    }
  }
  catch (Standard_Failure const& anException)
  {
#ifdef OCCT_DEBUG
    std::cout << "EXCEPTION in GeomTools_SurfaceSet::ReadSurface(..)!!!" << std::endl;
    std::cout << anException << std::endl;
#endif
    (void)anException;
  }
  return S;
}

//=================================================================================================

void GeomTools_SurfaceSet::Read(Standard_IStream& IS, const Message_ProgressRange& theProgress)
{
  char buffer[255];
  IS >> buffer;
  if (strcmp(buffer, "Surfaces"))
  {
    std::cout << "Not a surface table" << std::endl;
    return;
  }

  int i, nbsurf;
  IS >> nbsurf;
  Message_ProgressScope aPS(theProgress, "Surfaces", nbsurf);
  for (i = 1; i <= nbsurf && aPS.More(); i++, aPS.Next())
  {
    occ::handle<Geom_Surface> S = GeomTools_SurfaceSet::ReadSurface(IS);
    myMap.Add(S);
  }
}
