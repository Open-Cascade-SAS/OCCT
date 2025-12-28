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
#include <BinTools_SurfaceSet.hxx>
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
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pln.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array2.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <Message_ProgressScope.hxx>

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

BinTools_SurfaceSet::BinTools_SurfaceSet() {}

//=================================================================================================

void BinTools_SurfaceSet::Clear()
{
  myMap.Clear();
}

//=================================================================================================

int BinTools_SurfaceSet::Add(const occ::handle<Geom_Surface>& S)
{
  return myMap.Add(S);
}

//=================================================================================================

occ::handle<Geom_Surface> BinTools_SurfaceSet::Surface(const int I) const
{
  return occ::down_cast<Geom_Surface>(myMap(I));
}

//=================================================================================================

int BinTools_SurfaceSet::Index(const occ::handle<Geom_Surface>& S) const
{
  return myMap.FindIndex(S);
}

//=======================================================================
// function : operator <<(Geom_Plane)
// purpose  :
//=======================================================================

static BinTools_OStream& operator<<(BinTools_OStream& OS, const occ::handle<Geom_Plane>& S)
{
  OS << (uint8_t)PLANE;
  gp_Pln P = S->Pln();
  OS << P.Location(); // Pnt
  OS << P.Axis().Direction();
  OS << P.XAxis().Direction();
  OS << P.YAxis().Direction();
  return OS;
}

//=======================================================================
// function : operator <<(Geom_CylindricalSurface)
// purpose  :
//=======================================================================

static BinTools_OStream& operator<<(BinTools_OStream&                           OS,
                                    const occ::handle<Geom_CylindricalSurface>& S)
{
  OS << (uint8_t)CYLINDER;
  gp_Cylinder P = S->Cylinder();
  OS << P.Location(); // Pnt
  OS << P.Axis().Direction();
  OS << P.XAxis().Direction();
  OS << P.YAxis().Direction();
  OS << P.Radius();
  return OS;
}

//=======================================================================
// function : operator <<(Geom_ConicalSurface)
// purpose  :
//=======================================================================

static BinTools_OStream& operator<<(BinTools_OStream& OS, const occ::handle<Geom_ConicalSurface>& S)
{
  OS << (uint8_t)CONE;
  gp_Cone P = S->Cone();
  OS << P.Location(); // Pnt
  OS << P.Axis().Direction();
  OS << P.XAxis().Direction();
  OS << P.YAxis().Direction();
  OS << P.RefRadius();
  OS << P.SemiAngle();
  return OS;
}

//=======================================================================
// function : operator <<(Geom_SphericalSurface)
// purpose  :
//=======================================================================

static BinTools_OStream& operator<<(BinTools_OStream&                         OS,
                                    const occ::handle<Geom_SphericalSurface>& S)
{
  OS << (uint8_t)SPHERE;
  gp_Sphere P = S->Sphere();
  OS << P.Location(); // Pnt
  OS << P.Position().Axis().Direction();
  OS << P.XAxis().Direction();
  OS << P.YAxis().Direction();
  OS << P.Radius();
  return OS;
}

//=======================================================================
// function : operator <<(Geom_ToroidalSurface)
// purpose  :
//=======================================================================

static BinTools_OStream& operator<<(BinTools_OStream&                        OS,
                                    const occ::handle<Geom_ToroidalSurface>& S)
{
  OS << (uint8_t)TORUS;
  gp_Torus P = S->Torus();
  OS << P.Location(); // Pnt
  OS << P.Axis().Direction();
  OS << P.XAxis().Direction();
  OS << P.YAxis().Direction();
  OS << P.MajorRadius();
  OS << P.MinorRadius();
  return OS;
}

//=======================================================================
// function : operator <<(Geom_SurfaceOfLinearExtrusion)
// purpose  :
//=======================================================================

static BinTools_OStream& operator<<(BinTools_OStream&                                 OS,
                                    const occ::handle<Geom_SurfaceOfLinearExtrusion>& S)
{
  OS << (uint8_t)LINEAREXTRUSION;
  OS << S->Direction();
  BinTools_CurveSet::WriteCurve(S->BasisCurve(), OS);
  return OS;
}

//=======================================================================
// function : operator <<(Geom_SurfaceOfRevolution)
// purpose  :
//=======================================================================

static BinTools_OStream& operator<<(BinTools_OStream&                            OS,
                                    const occ::handle<Geom_SurfaceOfRevolution>& S)
{
  OS << (uint8_t)REVOLUTION;
  OS << S->Location();
  OS << S->Direction();
  BinTools_CurveSet::WriteCurve(S->BasisCurve(), OS);
  return OS;
}

//=======================================================================
// function : operator <<(Geom_BezierSurface)
// purpose  :
//=======================================================================

static BinTools_OStream& operator<<(BinTools_OStream& OS, const occ::handle<Geom_BezierSurface>& S)
{
  OS << (uint8_t)BEZIER;
  bool urational = S->IsURational() ? 1 : 0;
  bool vrational = S->IsVRational() ? 1 : 0;
  OS << urational; // rational
  OS << vrational;

  // poles and weights
  int i, j, udegree, vdegree;
  udegree = S->UDegree();
  vdegree = S->VDegree();
  OS << (char16_t)udegree;
  OS << (char16_t)vdegree;
  for (i = 1; i <= udegree + 1; i++)
  {
    for (j = 1; j <= vdegree + 1; j++)
    {
      OS << S->Pole(i, j); // Pnt
      if (urational || vrational)
      {
        OS << S->Weight(i, j); // Real
      }
    }
  }
  return OS;
}

//=======================================================================
// function : operator <<(Geom_BSplineSurface)
// purpose  :
//=======================================================================

static BinTools_OStream& operator<<(BinTools_OStream& OS, const occ::handle<Geom_BSplineSurface>& S)
{
  OS << (uint8_t)BSPLINE;
  bool urational = S->IsURational() ? 1 : 0;
  bool vrational = S->IsVRational() ? 1 : 0;
  bool uperiodic = S->IsUPeriodic() ? 1 : 0;
  bool vperiodic = S->IsVPeriodic() ? 1 : 0;
  OS << urational;
  OS << vrational;
  OS << uperiodic;
  OS << vperiodic;

  // poles and weights
  int i, j, udegree, vdegree, nbupoles, nbvpoles, nbuknots, nbvknots;
  udegree  = S->UDegree();
  vdegree  = S->VDegree();
  nbupoles = S->NbUPoles();
  nbvpoles = S->NbVPoles();
  nbuknots = S->NbUKnots();
  nbvknots = S->NbVKnots();
  OS << (char16_t)udegree;
  OS << (char16_t)vdegree;
  OS << nbupoles;
  OS << nbvpoles;
  OS << nbuknots;
  OS << nbvknots;
  for (i = 1; i <= nbupoles; i++)
  {
    for (j = 1; j <= nbvpoles; j++)
    {
      OS << S->Pole(i, j); // Pnt
      if (urational || vrational)
        OS << S->Weight(i, j); // Real
    }
  }

  for (i = 1; i <= nbuknots; i++)
  {
    OS << S->UKnot(i);
    OS << S->UMultiplicity(i);
  }

  for (i = 1; i <= nbvknots; i++)
  {
    OS << S->VKnot(i);
    OS << S->VMultiplicity(i);
  }
  return OS;
}

//=======================================================================
// function :  operator <<(Geom_RectangularTrimmedSurface)
// purpose  :
//=======================================================================

static BinTools_OStream& operator<<(BinTools_OStream&                                  OS,
                                    const occ::handle<Geom_RectangularTrimmedSurface>& S)
{
  OS << (uint8_t)RECTANGULAR;
  double U1, U2, V1, V2;
  S->Bounds(U1, U2, V1, V2);
  OS << U1 << U2 << V1 << V2;
  BinTools_SurfaceSet::WriteSurface(S->BasisSurface(), OS);
  return OS;
}

//=======================================================================
// function : operator <<(Geom_OffsetSurface)
// purpose  :
//=======================================================================

static BinTools_OStream& operator<<(BinTools_OStream& OS, const occ::handle<Geom_OffsetSurface>& S)
{
  OS << (uint8_t)OFFSET;
  OS << S->Offset();
  BinTools_SurfaceSet::WriteSurface(S->BasisSurface(), OS);
  return OS;
}

//=================================================================================================

void BinTools_SurfaceSet::WriteSurface(const occ::handle<Geom_Surface>& S, BinTools_OStream& OS)
{
  occ::handle<Standard_Type> TheType = S->DynamicType();
  try
  {
    OCC_CATCH_SIGNALS
    if (TheType == STANDARD_TYPE(Geom_Plane))
    {
      OS << occ::down_cast<Geom_Plane>(S);
    }
    else if (TheType == STANDARD_TYPE(Geom_CylindricalSurface))
    {
      OS << occ::down_cast<Geom_CylindricalSurface>(S);
    }
    else if (TheType == STANDARD_TYPE(Geom_ConicalSurface))
    {
      OS << occ::down_cast<Geom_ConicalSurface>(S);
    }
    else if (TheType == STANDARD_TYPE(Geom_SphericalSurface))
    {
      OS << occ::down_cast<Geom_SphericalSurface>(S);
    }
    else if (TheType == STANDARD_TYPE(Geom_ToroidalSurface))
    {
      OS << occ::down_cast<Geom_ToroidalSurface>(S);
    }
    else if (TheType == STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion))
    {
      OS << occ::down_cast<Geom_SurfaceOfLinearExtrusion>(S);
    }
    else if (TheType == STANDARD_TYPE(Geom_SurfaceOfRevolution))
    {
      OS << occ::down_cast<Geom_SurfaceOfRevolution>(S);
    }
    else if (TheType == STANDARD_TYPE(Geom_BezierSurface))
    {
      OS << occ::down_cast<Geom_BezierSurface>(S);
    }
    else if (TheType == STANDARD_TYPE(Geom_BSplineSurface))
    {
      OS << occ::down_cast<Geom_BSplineSurface>(S);
    }
    else if (TheType == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
    {
      OS << occ::down_cast<Geom_RectangularTrimmedSurface>(S);
    }
    else if (TheType == STANDARD_TYPE(Geom_OffsetSurface))
    {
      OS << occ::down_cast<Geom_OffsetSurface>(S);
    }
    else
    {
      throw Standard_Failure("UNKNOWN SURFACE TYPE");
    }
  }
  catch (Standard_Failure const& anException)
  {
    Standard_SStream aMsg;
    aMsg << "EXCEPTION in BinTools_SurfaceSet::WriteSurface(..)" << std::endl;
    aMsg << anException << std::endl;
    throw Standard_Failure(aMsg.str().c_str());
  }
}

//=================================================================================================

void BinTools_SurfaceSet::Write(Standard_OStream& OS, const Message_ProgressRange& theRange) const
{

  int                   i, nbsurf = myMap.Extent();
  Message_ProgressScope aPS(theRange, "Writing surfaces", nbsurf);
  OS << "Surfaces " << nbsurf << "\n";
  BinTools_OStream aStream(OS);
  for (i = 1; i <= nbsurf && aPS.More(); i++, aPS.Next())
  {
    WriteSurface(occ::down_cast<Geom_Surface>(myMap(i)), aStream);
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
  BinTools::GetReal(IS, R);
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
  BinTools::GetReal(IS, R);
  BinTools::GetReal(IS, Ang);
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
  BinTools::GetReal(IS, R);
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
  BinTools::GetReal(IS, R1);
  BinTools::GetReal(IS, R2);
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
  gp_Dir                  D(gp_Dir::D::X);
  occ::handle<Geom_Curve> C;
  IS >> D;
  BinTools_CurveSet::ReadCurve(IS, C);
  S = new Geom_SurfaceOfLinearExtrusion(C, D);
  return IS;
}

//=======================================================================
// function : operator>>
// purpose  :
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom_SurfaceOfRevolution>& S)
{
  gp_Pnt                  P(0., 0., 0.);
  gp_Dir                  D(gp_Dir::D::X);
  occ::handle<Geom_Curve> C;
  IS >> P >> D;
  BinTools_CurveSet::ReadCurve(IS, C);
  S = new Geom_SurfaceOfRevolution(C, gp_Ax1(P, D));
  return IS;
}

//=======================================================================
// function : operator>>
// purpose  :
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom_BezierSurface>& S)
{
  //  std::cout << "BezierSurface:" <<std::endl;
  bool urational = false, vrational = false;
  BinTools::GetBool(IS, urational);
  BinTools::GetBool(IS, vrational);

  //  std::cout << "\turational = " << urational << " vrational = " << vrational<<std::endl;
  int      udegree = 0, vdegree = 0;
  char16_t aVal = '\0';
  BinTools::GetExtChar(IS, aVal);

  udegree = (int)aVal;
  BinTools::GetExtChar(IS, aVal);
  vdegree = (int)aVal;
  //  std::cout << "\ttudegree  = " << udegree << ", vdegree = " << vdegree << std::endl;

  NCollection_Array2<gp_Pnt> poles(1, udegree + 1, 1, vdegree + 1);
  NCollection_Array2<double> weights(1, udegree + 1, 1, vdegree + 1);

  int i, j;
  for (i = 1; i <= udegree + 1; i++)
  {
    for (j = 1; j <= vdegree + 1; j++)
    {
      IS >> poles(i, j); // Pnt
      //      std::cout <<"Pole X = " <<poles(i,j).X()<< " Y = " <<poles(i,j).Y()<< " Z = " <<
      //      poles(i,j).Z()<<std::endl;
      if (urational || vrational)
        BinTools::GetReal(IS, weights(i, j));
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
  BinTools::GetBool(IS, urational);
  BinTools::GetBool(IS, vrational);
  BinTools::GetBool(IS, uperiodic);
  BinTools::GetBool(IS, vperiodic);

  int      udegree = 0, vdegree = 0, nbupoles = 0, nbvpoles = 0, nbuknots = 0, nbvknots = 0;
  char16_t aVal = '\0';
  BinTools::GetExtChar(IS, aVal);
  udegree = (int)aVal;
  BinTools::GetExtChar(IS, aVal);
  vdegree = (int)aVal;
  BinTools::GetInteger(IS, nbupoles);
  BinTools::GetInteger(IS, nbvpoles);
  BinTools::GetInteger(IS, nbuknots);
  BinTools::GetInteger(IS, nbvknots);

  NCollection_Array2<gp_Pnt> poles(1, nbupoles, 1, nbvpoles);
  NCollection_Array2<double> weights(1, nbupoles, 1, nbvpoles);

  int i, j;
  for (i = 1; i <= nbupoles; i++)
  {
    for (j = 1; j <= nbvpoles; j++)
    {
      IS >> poles(i, j); // Pnt
      if (urational || vrational)
        BinTools::GetReal(IS, weights(i, j));
    }
  }

  NCollection_Array1<double> uknots(1, nbuknots);
  NCollection_Array1<int>    umults(1, nbuknots);
  for (i = 1; i <= nbuknots; i++)
  {
    BinTools::GetReal(IS, uknots(i));
    BinTools::GetInteger(IS, umults(i));
  }

  NCollection_Array1<double> vknots(1, nbvknots);
  NCollection_Array1<int>    vmults(1, nbvknots);
  for (i = 1; i <= nbvknots; i++)
  {
    BinTools::GetReal(IS, vknots(i));
    BinTools::GetInteger(IS, vmults(i));
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
  BinTools::GetReal(IS, U1);
  BinTools::GetReal(IS, U2);
  BinTools::GetReal(IS, V1);
  BinTools::GetReal(IS, V2);
  occ::handle<Geom_Surface> BS;
  BinTools_SurfaceSet::ReadSurface(IS, BS);
  S = new Geom_RectangularTrimmedSurface(BS, U1, U2, V1, V2);
  return IS;
}

//=======================================================================
// function : operator>>
// purpose  :
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, occ::handle<Geom_OffsetSurface>& S)
{
  double O = 0.;
  BinTools::GetReal(IS, O);
  occ::handle<Geom_Surface> BS;
  BinTools_SurfaceSet::ReadSurface(IS, BS);
  S = new Geom_OffsetSurface(BS, O);
  return IS;
}

//=================================================================================================

Standard_IStream& BinTools_SurfaceSet::ReadSurface(Standard_IStream&          IS,
                                                   occ::handle<Geom_Surface>& S)
{
  try
  {
    OCC_CATCH_SIGNALS
    const uint8_t stype = (uint8_t)IS.get();
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
        S = NULL;
        throw Standard_Failure("UNKNOWN SURFACE TYPE");
      }
      break;
    }
  }
  catch (Standard_Failure const& anException)
  {
    S = NULL;
    Standard_SStream aMsg;
    aMsg << "EXCEPTION in BinTools_SurfaceSet::ReadSurface(..)" << std::endl;
    aMsg << anException << std::endl;
    throw Standard_Failure(aMsg.str().c_str());
  }
  return IS;
}

//=================================================================================================

void BinTools_SurfaceSet::Read(Standard_IStream& IS, const Message_ProgressRange& theRange)
{
  char buffer[255];
  IS >> buffer;
  if (IS.fail() || strcmp(buffer, "Surfaces"))
  {
    Standard_SStream aMsg;
    aMsg << "BinTools_SurfaceSet::Read:  Not a surface table" << std::endl;
#ifdef OCCT_DEBUG
    std::cout << "SurfaceSet buffer: " << buffer << std::endl;
#endif
    throw Standard_Failure(aMsg.str().c_str());
    return;
  }

  occ::handle<Geom_Surface> S;
  int                       i, nbsurf;
  IS >> nbsurf;
  Message_ProgressScope aPS(theRange, "Reading surfaces", nbsurf);
  IS.get(); // remove <lf>
  for (i = 1; i <= nbsurf && aPS.More(); i++, aPS.Next())
  {
    BinTools_SurfaceSet::ReadSurface(IS, S);
    myMap.Add(S);
  }
}
