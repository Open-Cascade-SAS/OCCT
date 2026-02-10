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

#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <ShapeAnalysis_Curve.hxx>
#include <ShapeCustom_Curve.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>

ShapeCustom_Curve::ShapeCustom_Curve() = default;

//=================================================================================================

ShapeCustom_Curve::ShapeCustom_Curve(const occ::handle<Geom_Curve>& C)
{
  Init(C);
}

//=================================================================================================

void ShapeCustom_Curve::Init(const occ::handle<Geom_Curve>& C)
{
  myCurve = C;
}

//=================================================================================================

occ::handle<Geom_Curve> ShapeCustom_Curve::ConvertToPeriodic(const bool   substitute,
                                                             const double preci)
{
  occ::handle<Geom_Curve>        newCurve;
  occ::handle<Geom_BSplineCurve> BSpl = occ::down_cast<Geom_BSplineCurve>(myCurve);
  if (BSpl.IsNull())
    return newCurve;

  // PTV 13.02.02: check if curve closed with tolerance
  bool closed = ShapeAnalysis_Curve::IsClosed(myCurve, preci);

  if (!closed)
    return newCurve;

  bool converted = false; //: p6

  if (closed && !BSpl->IsPeriodic() && BSpl->NbPoles() > 3)
  {
    bool set = true;
    // if degree+1 at ends, first change it to 1 by rearranging knots
    if (BSpl->Multiplicity(1) == BSpl->Degree() + 1
        && BSpl->Multiplicity(BSpl->NbKnots()) == BSpl->Degree() + 1)
    {
      const NCollection_Array1<gp_Pnt>& oldPoles   = BSpl->Poles();
      const NCollection_Array1<double>& oldWeights = BSpl->WeightsArray();
      int                               nbKnots    = BSpl->NbKnots();
      const NCollection_Array1<double>& oldKnots   = BSpl->Knots();
      const NCollection_Array1<int>&    oldMults   = BSpl->Multiplicities();

      NCollection_Array1<double> newKnots(1, nbKnots + 2);
      NCollection_Array1<int>    newMults(1, nbKnots + 2);
      double                     a =
        0.5 * (BSpl->Knot(2) - BSpl->Knot(1) + BSpl->Knot(nbKnots) - BSpl->Knot(nbKnots - 1));

      newKnots(1)           = oldKnots(1) - a;
      newKnots(nbKnots + 2) = oldKnots(nbKnots) + a;
      newMults(1) = newMults(nbKnots + 2) = 1;
      for (int i = 2; i <= nbKnots + 1; i++)
      {
        newKnots(i) = oldKnots(i - 1);
        newMults(i) = oldMults(i - 1);
      }
      newMults(2) = newMults(nbKnots + 1) = BSpl->Degree();
      occ::handle<Geom_BSplineCurve> res  = new Geom_BSplineCurve(oldPoles,
                                                                 oldWeights,
                                                                 newKnots,
                                                                 newMults,
                                                                 BSpl->Degree(),
                                                                 BSpl->IsPeriodic());
      BSpl                                = res;
    }
    else if (BSpl->Multiplicity(1) > BSpl->Degree()
             || BSpl->Multiplicity(BSpl->NbKnots()) > BSpl->Degree() + 1)
      set = false;
    if (set)
    {
      BSpl->SetPeriodic(); // make periodic
      converted = true;
    }
  }
#ifdef OCCT_DEBUG
  std::cout << "Warning: ShapeCustom_Surface: Closed BSplineSurface is caused to be periodic"
            << std::endl;
#endif
  if (!converted)
    return newCurve;
  newCurve = BSpl;
  if (substitute)
    myCurve = newCurve;
  return newCurve;
}
