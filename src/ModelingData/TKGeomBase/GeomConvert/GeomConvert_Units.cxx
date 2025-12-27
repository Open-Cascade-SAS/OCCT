// Copyright (c) 2021 OPEN CASCADE SAS
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

#include <GeomConvert_Units.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Conic.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2dConvert.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <gp.hxx>
#include <gp_Dir2d.hxx>
#include <gp_GTrsf2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>

//=================================================================================================

occ::handle<Geom2d_Curve> GeomConvert_Units::RadianToDegree(const occ::handle<Geom2d_Curve>& theCurve2d,
                                                       const occ::handle<Geom_Surface>& theSurf,
                                                       const double         theLengthFactor,
                                                       const double theFactorRadianDegree)
{
  occ::handle<Geom2d_Curve> aCurve2d   = occ::down_cast<Geom2d_Curve>(theCurve2d->Copy());
  double        uFact      = 1.;
  double        vFact      = 1.;
  double        LengthFact = 1. / theLengthFactor;
  double        AngleFact  = theFactorRadianDegree; // 180./PI;  pilotable

  gp_Pnt2d   Pt1;
  gp_XY      pXY;
  gp_GTrsf2d tMatu, tMatv;

  //  theSurf is a CylindricalSurface or a ConicalSurface or
  //             a ToroidalSurface or a SphericalSurface or
  //             a SurfaceOfRevolution
  if (theSurf->IsKind(STANDARD_TYPE(Geom_SphericalSurface))
      || theSurf->IsKind(STANDARD_TYPE(Geom_ToroidalSurface)))
  {
    uFact = vFact = AngleFact;
  }
  else if (theSurf->IsKind(STANDARD_TYPE(Geom_CylindricalSurface)))
  {
    uFact = AngleFact;
    vFact = LengthFact;
  }
  else if (theSurf->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution)))
  {
    uFact = AngleFact;
  }
  else if (theSurf->IsKind(STANDARD_TYPE(Geom_ConicalSurface)))
  {
    occ::handle<Geom_ConicalSurface> conicS = occ::down_cast<Geom_ConicalSurface>(theSurf);
    double               semAng = conicS->SemiAngle();
    uFact                              = AngleFact;
    vFact                              = LengthFact * std::cos(semAng);
  }
  else if (theSurf->IsKind(STANDARD_TYPE(Geom_Plane)))
  {
    uFact = vFact = LengthFact;
    if (aCurve2d->IsKind(STANDARD_TYPE(Geom2d_Circle))
        || aCurve2d->IsKind(STANDARD_TYPE(Geom2d_Ellipse)))
    {
      gp_Trsf2d aT;
      aT.SetScale(gp::Origin2d(), LengthFact);
      aCurve2d->Transform(aT);
      return aCurve2d;
    }
  }
  else
  {
    return aCurve2d;
  }

  if (aCurve2d->IsKind(STANDARD_TYPE(Geom2d_Line)))
  {
    occ::handle<Geom2d_Line> aLine2d = occ::down_cast<Geom2d_Line>(aCurve2d);
    gp_Pnt2d            myLoc   = aLine2d->Location();
    gp_Dir2d            myDir   = aLine2d->Direction();
    gp_Pnt2d            myNewLoc;
    myNewLoc.SetCoord(myLoc.X() * uFact, myLoc.Y() * vFact);
    gp_Dir2d myNewDir;
    myNewDir.SetCoord(myDir.X() * uFact, myDir.Y() * vFact);
    occ::handle<Geom2d_Line> myNewLine2d = occ::down_cast<Geom2d_Line>(aLine2d->Copy());
    myNewLine2d->SetLocation(myNewLoc);
    myNewLine2d->SetDirection(myNewDir);
    return myNewLine2d;
  }
  else if (aCurve2d->IsKind(STANDARD_TYPE(Geom2d_Conic)))
  {
    if (aCurve2d->IsKind(STANDARD_TYPE(Geom2d_Circle))
        || aCurve2d->IsKind(STANDARD_TYPE(Geom2d_Ellipse)))
    {
      occ::handle<Geom2d_BSplineCurve> aBSpline2d = Geom2dConvert::CurveToBSplineCurve(aCurve2d);
      aCurve2d                               = aBSpline2d;
    }
    else if (aCurve2d->IsKind(STANDARD_TYPE(Geom2d_Parabola)))
    {
#ifdef OCCT_DEBUG
      std::cout << "PCURVE of Parabola type in U or V Periodic Surface" << std::endl;
      std::cout << "Parameters Not transformed to Degree" << std::endl;
#endif
    }
    else if (aCurve2d->IsKind(STANDARD_TYPE(Geom2d_Hyperbola)))
    {
#ifdef OCCT_DEBUG
      std::cout << "PCURVE of Hyperbola type in U or V Periodic Surface" << std::endl;
      std::cout << "Parameters Not transformed to Degree" << std::endl;
#endif
    }
  }

  // Compute affinity
  tMatu.SetAffinity(gp::OY2d(), uFact);
  tMatv.SetAffinity(gp::OX2d(), vFact);
  if (aCurve2d->IsKind(STANDARD_TYPE(Geom2d_BoundedCurve)))
  {
    if (aCurve2d->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)))
    {
      occ::handle<Geom2d_BSplineCurve> aBSpline2d = occ::down_cast<Geom2d_BSplineCurve>(aCurve2d);
      occ::handle<Geom2d_BSplineCurve> myNewBSpline2d =
        occ::down_cast<Geom2d_BSplineCurve>(aBSpline2d->Copy());
      int nbPol = aBSpline2d->NbPoles();
      for (int i = 1; i <= nbPol; i++)
      {
        pXY = aBSpline2d->Pole(i).XY();
        tMatu.Transforms(pXY);
        tMatv.Transforms(pXY);
        Pt1.SetXY(pXY);
        myNewBSpline2d->SetPole(i, Pt1);
      }
      return myNewBSpline2d;
    }
    else
    {
#ifdef OCCT_DEBUG
      std::cout << "PCURVE of Other Types of Bounded Curve in U or V Periodic Surface" << std::endl;
      std::cout << "Parameters Not transformed to Degree" << std::endl;
#endif
    }
  }
  return aCurve2d;
}

// ============================================================================
// Method : DegreeToRadian
// Purpose: 1. Change definition of the pcurves according to LengthFactor
//          2. STEP cylinder, torus, cone and sphere are parametrized
//             from 0 to 360 degree
//             Then pcurves parameter have to be transformed
//             from DEGREE to RADIAN
// ============================================================================
occ::handle<Geom2d_Curve> GeomConvert_Units::DegreeToRadian(const occ::handle<Geom2d_Curve>& thePcurve,
                                                       const occ::handle<Geom_Surface>& theSurface,
                                                       const double         theLengthFactor,
                                                       const double theFactorRadianDegree)
{
  occ::handle<Geom2d_Curve> aPcurve    = occ::down_cast<Geom2d_Curve>(thePcurve->Copy());
  double        uFact      = 1.;
  double        vFact      = 1.;
  double        LengthFact = theLengthFactor;
  double        AngleFact  = theFactorRadianDegree; // PI/180.;  pilotable

  gp_Pnt2d   Pt1;
  gp_XY      pXY;
  gp_GTrsf2d tMatu, tMatv;

  // What to change ??

  if (theSurface->IsKind(STANDARD_TYPE(Geom_SphericalSurface))
      || theSurface->IsKind(STANDARD_TYPE(Geom_ToroidalSurface)))
  {
    uFact = vFact = AngleFact;
  }
  else if (theSurface->IsKind(STANDARD_TYPE(Geom_CylindricalSurface)))
  {
    uFact = AngleFact;
    vFact = LengthFact;
  }
  else if (theSurface->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution)))
  {
    uFact = AngleFact;
  }
  else if (theSurface->IsKind(STANDARD_TYPE(Geom_ConicalSurface)))
  {
    occ::handle<Geom_ConicalSurface> conicS = occ::down_cast<Geom_ConicalSurface>(theSurface);
    double               semAng = conicS->SemiAngle();
    uFact                              = AngleFact;
    vFact                              = LengthFact / std::cos(semAng);
  }
  else if (theSurface->IsKind(STANDARD_TYPE(Geom_Plane)))
  {
    uFact = vFact = LengthFact;
    if (aPcurve->IsKind(STANDARD_TYPE(Geom2d_Circle))
        || aPcurve->IsKind(STANDARD_TYPE(Geom2d_Ellipse)))
    {
      gp_Trsf2d aT;
      aT.SetScale(gp::Origin2d(), LengthFact);
      aPcurve->Transform(aT);
      return aPcurve;
    }
  }
  else
  {
    return aPcurve;
  }

  if (aPcurve->IsKind(STANDARD_TYPE(Geom2d_Conic)))
  {
    if (aPcurve->IsKind(STANDARD_TYPE(Geom2d_Circle))
        || aPcurve->IsKind(STANDARD_TYPE(Geom2d_Ellipse)))
    {
      occ::handle<Geom2d_BSplineCurve> aBSpline2d = Geom2dConvert::CurveToBSplineCurve(aPcurve);
      aPcurve                                = aBSpline2d;
    }
    else if (aPcurve->IsKind(STANDARD_TYPE(Geom2d_Parabola)))
    {
#ifdef OCCT_DEBUG
      std::cout << "PCURVE of Parabola type" << std::endl;
      std::cout << "Parameters Not Yet transformed according to LengthUnit" << std::endl;
#endif
      return aPcurve;
    }
    else if (aPcurve->IsKind(STANDARD_TYPE(Geom2d_Hyperbola)))
    {
#ifdef OCCT_DEBUG
      std::cout << "PCURVE of Hyperbola type" << std::endl;
      std::cout << "Parameters Not Yet transformed according to LengthUnit" << std::endl;
#endif
      return aPcurve;
    }
  }

  // Compute affinity

  tMatu.SetAffinity(gp::OY2d(), uFact);
  tMatv.SetAffinity(gp::OX2d(), vFact);

  if (aPcurve->IsKind(STANDARD_TYPE(Geom2d_Line)))
  {
    occ::handle<Geom2d_Line> aLine2d = occ::down_cast<Geom2d_Line>(aPcurve);

    gp_Pnt2d myLoc = aLine2d->Location();
    gp_Dir2d myDir = aLine2d->Direction();

    gp_Pnt2d myNewLoc;
    myNewLoc.SetCoord(myLoc.X() * uFact, myLoc.Y() * vFact);

    gp_Dir2d myNewDir;
    myNewDir.SetCoord(myDir.X() * uFact, myDir.Y() * vFact);

    aLine2d->SetLocation(myNewLoc);
    aLine2d->SetDirection(myNewDir);

    aPcurve = aLine2d;
  }
  else if (aPcurve->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)))
  {
    occ::handle<Geom2d_BSplineCurve> aBSpline2d = occ::down_cast<Geom2d_BSplineCurve>(aPcurve);

    // transform the Poles of the BSplineCurve according to AngleFact and LengthFact

    int nbPol = aBSpline2d->NbPoles();
    for (int i = 1; i <= nbPol; i++)
    {
      pXY = aBSpline2d->Pole(i).XY();
      tMatu.Transforms(pXY);
      tMatv.Transforms(pXY);
      Pt1.SetXY(pXY);
      aBSpline2d->SetPole(i, Pt1);
    }
    aPcurve = aBSpline2d;
  }
  else
  {
#ifdef OCCT_DEBUG
    std::cout << "DegreeToRadian : Type " << aPcurve->DynamicType();
    std::cout << " not yet implemented" << std::endl;
#endif
  }
  return aPcurve;
}

//=================================================================================================

occ::handle<Geom2d_Curve> GeomConvert_Units::MirrorPCurve(const occ::handle<Geom2d_Curve>& theCurve)
{
  occ::handle<Geom2d_Curve> theMirrored = occ::down_cast<Geom2d_Curve>(theCurve->Copy());
  gp_Trsf2d            T;
  gp_Pnt2d             Loc(0., 0.);
  gp_Dir2d             Dir(gp_Dir2d::D::X);
  gp_Ax2d              ax2(Loc, Dir);
  T.SetMirror(ax2);
  theMirrored->Transform(T);
  return theMirrored;
}
