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

#include <ShapeAnalysis_Geom.hxx>

#include <gp_GTrsf.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <PointSetLib_Equation.hxx>
#include <Standard_ErrorHandler.hxx>

//=================================================================================================

bool ShapeAnalysis_Geom::NearestPlane(const NCollection_Array1<gp_Pnt>& thePnts,
                                      gp_Pln&                           thePln,
                                      double&                           theMaxDist)
{
  PointSetLib_Equation anEq(thePnts, 0.0);
  if (anEq.GetType() == PointSetLib_Equation::Type::None)
  {
    return false;
  }

  const double aDev1 = anEq.Extent(1);
  const double aDev2 = anEq.Extent(2);
  const double aDev3 = anEq.Extent(3);

  // Find the axis with the smallest extent - that's the plane normal candidate
  int anAxis = (aDev1 < aDev2) ? ((aDev1 < aDev3) ? 1 : 3) : ((aDev2 < aDev3) ? 2 : 3);

  // Check if the smallest extent is significantly smaller than the other two
  switch (anAxis)
  {
    case 1: {
      if ((2.0 * aDev1 > aDev2) || (2.0 * aDev1 > aDev3))
      {
        anAxis = 0;
      }
      else
      {
        thePln = gp_Pln(anEq.Barycentre(), anEq.PrincipalAxis(1));
      }
      break;
    }
    case 2: {
      if ((2.0 * aDev2 > aDev1) || (2.0 * aDev2 > aDev3))
      {
        anAxis = 0;
      }
      else
      {
        thePln = gp_Pln(anEq.Barycentre(), anEq.PrincipalAxis(2));
      }
      break;
    }
    case 3: {
      if ((2.0 * aDev3 > aDev2) || (2.0 * aDev3 > aDev1))
      {
        anAxis = 0;
      }
      else
      {
        thePln = gp_Pln(anEq.Barycentre(), anEq.PrincipalAxis(3));
      }
      break;
    }
  }

  theMaxDist = RealFirst();
  if (anAxis != 0)
  {
    for (int i = thePnts.Lower(); i <= thePnts.Upper(); ++i)
    {
      const double aD = thePln.Distance(thePnts(i));
      if (theMaxDist < aD)
      {
        theMaxDist = aD;
      }
    }
  }

  return (anAxis != 0);
}

//=================================================================================================

bool ShapeAnalysis_Geom::PositionTrsf(const occ::handle<NCollection_HArray2<double>>& coefs,
                                      gp_Trsf&                                        trsf,
                                      const double                                    unit,
                                      const double                                    prec)
{
  bool result = true;

  trsf = gp_Trsf();

  if (coefs.IsNull())
  {
    return true;
  }

  gp_GTrsf gtrsf;
  for (int i = 1; i <= 3; i++)
  {
    for (int j = 1; j <= 4; j++)
    {
      gtrsf.SetValue(i, j, coefs->Value(i, j));
    }
  }

  gp_XYZ v1(gtrsf.Value(1, 1), gtrsf.Value(2, 1), gtrsf.Value(3, 1));
  gp_XYZ v2(gtrsf.Value(1, 2), gtrsf.Value(2, 2), gtrsf.Value(3, 2));
  gp_XYZ v3(gtrsf.Value(1, 3), gtrsf.Value(2, 3), gtrsf.Value(3, 3));
  double m1 = v1.Modulus();
  double m2 = v2.Modulus();
  double m3 = v3.Modulus();

  if (m1 < prec || m2 < prec || m3 < prec)
  {
    return false;
  }
  double mm  = (m1 + m2 + m3) / 3.;
  double pmm = prec * mm;
  if (std::abs(m1 - mm) > pmm || std::abs(m2 - mm) > pmm || std::abs(m3 - mm) > pmm)
  {
    return false;
  }
  v1.Divide(m1);
  v2.Divide(m2);
  v3.Divide(m3);
  if (std::abs(v1.Dot(v2)) > prec || std::abs(v2.Dot(v3)) > prec || std::abs(v3.Dot(v1)) > prec)
  {
    return false;
  }

  if (v1.X() != 1 || v1.Y() != 0 || v1.Z() != 0 || v2.X() != 0 || v2.Y() != 1 || v2.Z() != 0
      || v3.X() != 0 || v3.Y() != 0 || v3.Z() != 1)
  {
    gp_Dir d1(v1);
    gp_Dir d2(v2);
    gp_Dir d3(v3);
    gp_Ax3 axes(gp_Pnt(0, 0, 0), d3, d1);
    d3.Cross(d1);
    if (d3.Dot(d2) < 0)
    {
      axes.YReverse();
    }
    trsf.SetTransformation(axes);
  }

  if (std::abs(mm - 1.) > prec)
  {
    trsf.SetScale(gp_Pnt(0, 0, 0), mm);
  }
  gp_Vec tp(gtrsf.TranslationPart());
  if (unit != 1.)
  {
    tp.Multiply(unit);
  }
  if (tp.X() != 0 || tp.Y() != 0 || tp.Z() != 0)
  {
    trsf.SetTranslationPart(tp);
  }

  return result;
}
