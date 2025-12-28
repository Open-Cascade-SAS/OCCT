// Created on: 1995-10-25
// Created by: Bruno DUMORTIER
// Copyright (c) 1995-1999 Matra Datavision
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

#include <BRepOffset.hxx>
#include <BRep_Tool.hxx>
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
#include <gp_Ax1.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <NCollection_LocalArray.hxx>
#include <Precision.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

//=================================================================================================

occ::handle<Geom_Surface> BRepOffset::Surface(const occ::handle<Geom_Surface>& Surface,
                                              const double                     Offset,
                                              BRepOffset_Status&               theStatus,
                                              bool                             allowC0)
{
  constexpr double Tol = Precision::Confusion();

  theStatus = BRepOffset_Good;
  occ::handle<Geom_Surface> Result;

  occ::handle<Standard_Type> TheType = Surface->DynamicType();

  if (TheType == STANDARD_TYPE(Geom_Plane))
  {
    occ::handle<Geom_Plane> P = occ::down_cast<Geom_Plane>(Surface);
    gp_Vec                  T = P->Position().XDirection() ^ P->Position().YDirection();
    T *= Offset;
    Result = occ::down_cast<Geom_Plane>(P->Translated(T));
  }
  else if (TheType == STANDARD_TYPE(Geom_CylindricalSurface))
  {
    occ::handle<Geom_CylindricalSurface> C      = occ::down_cast<Geom_CylindricalSurface>(Surface);
    double                               Radius = C->Radius();
    gp_Ax3                               Axis   = C->Position();
    if (Axis.Direct())
      Radius += Offset;
    else
      Radius -= Offset;
    if (Radius >= Tol)
    {
      Result = new Geom_CylindricalSurface(Axis, Radius);
    }
    else if (Radius <= -Tol)
    {
      Axis.Rotate(gp_Ax1(Axis.Location(), Axis.Direction()), M_PI);
      Result    = new Geom_CylindricalSurface(Axis, std::abs(Radius));
      theStatus = BRepOffset_Reversed;
    }
    else
    {
      theStatus = BRepOffset_Degenerated;
    }
  }
  else if (TheType == STANDARD_TYPE(Geom_ConicalSurface))
  {
    occ::handle<Geom_ConicalSurface> C      = occ::down_cast<Geom_ConicalSurface>(Surface);
    double                           Alpha  = C->SemiAngle();
    double                           Radius = C->RefRadius() + Offset * std::cos(Alpha);
    gp_Ax3                           Axis   = C->Position();
    if (Radius >= 0.)
    {
      gp_Vec Z(Axis.Direction());
      Z *= -Offset * std::sin(Alpha);
      Axis.Translate(Z);
    }
    else
    {
      Radius = -Radius;
      gp_Vec Z(Axis.Direction());
      Z *= -Offset * std::sin(Alpha);
      Axis.Translate(Z);
      Axis.Rotate(gp_Ax1(Axis.Location(), Axis.Direction()), M_PI);
      Alpha = -Alpha;
    }
    Result = new Geom_ConicalSurface(Axis, Alpha, Radius);
  }
  else if (TheType == STANDARD_TYPE(Geom_SphericalSurface))
  {
    occ::handle<Geom_SphericalSurface> S      = occ::down_cast<Geom_SphericalSurface>(Surface);
    double                             Radius = S->Radius();
    gp_Ax3                             Axis   = S->Position();
    if (Axis.Direct())
      Radius += Offset;
    else
      Radius -= Offset;
    if (Radius >= Tol)
    {
      Result = new Geom_SphericalSurface(Axis, Radius);
    }
    else if (Radius <= -Tol)
    {
      Axis.Rotate(gp_Ax1(Axis.Location(), Axis.Direction()), M_PI);
      Axis.ZReverse();
      Result    = new Geom_SphericalSurface(Axis, -Radius);
      theStatus = BRepOffset_Reversed;
    }
    else
    {
      theStatus = BRepOffset_Degenerated;
    }
  }
  else if (TheType == STANDARD_TYPE(Geom_ToroidalSurface))
  {
    occ::handle<Geom_ToroidalSurface> S           = occ::down_cast<Geom_ToroidalSurface>(Surface);
    double                            MajorRadius = S->MajorRadius();
    double                            MinorRadius = S->MinorRadius();
    gp_Ax3                            Axis        = S->Position();
    if (MinorRadius < MajorRadius)
    { // A FINIR
      if (Axis.Direct())
        MinorRadius += Offset;
      else
        MinorRadius -= Offset;
      if (MinorRadius >= Tol)
      {
        Result = new Geom_ToroidalSurface(Axis, MajorRadius, MinorRadius);
      }
      else if (MinorRadius <= -Tol)
      {
        theStatus = BRepOffset_Reversed;
      }
      else
      {
        theStatus = BRepOffset_Degenerated;
      }
    }
  }
  else if (TheType == STANDARD_TYPE(Geom_SurfaceOfRevolution))
  {
  }
  else if (TheType == STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion))
  {
  }
  else if (TheType == STANDARD_TYPE(Geom_BSplineSurface))
  {
  }
  else if (TheType == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
  {
    occ::handle<Geom_RectangularTrimmedSurface> S =
      occ::down_cast<Geom_RectangularTrimmedSurface>(Surface);
    double U1, U2, V1, V2;
    S->Bounds(U1, U2, V1, V2);
    occ::handle<Geom_Surface> Off =
      BRepOffset::Surface(S->BasisSurface(), Offset, theStatus, allowC0);
    Result = new Geom_RectangularTrimmedSurface(Off, U1, U2, V1, V2);
  }
  else if (TheType == STANDARD_TYPE(Geom_OffsetSurface))
  {
  }

  if (Result.IsNull())
  {
    Result = new Geom_OffsetSurface(Surface, Offset, allowC0);
  }

  return Result;
}

//=================================================================================================

occ::handle<Geom_Surface> BRepOffset::CollapseSingularities(
  const occ::handle<Geom_Surface>& theSurface,
  const TopoDS_Face&               theFace,
  double                           thePrecision)
{
  // check surface type to see if it can be processed
  occ::handle<Standard_Type> aType = theSurface->DynamicType();
  if (aType != STANDARD_TYPE(Geom_BSplineSurface))
  {
    // for the moment, only bspline surfaces are treated;
    // in the future, bezier surfaces and surfaces of revolution can be also handled
    return theSurface;
  }

  // find singularities (vertices of degenerated edges)
  NCollection_List<gp_Pnt> aDegenPnt;
  NCollection_List<double> aDegenTol;
  for (TopExp_Explorer anExp(theFace, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    TopoDS_Edge anEdge = TopoDS::Edge(anExp.Current());
    if (!BRep_Tool::Degenerated(anEdge))
    {
      continue;
    }
    TopoDS_Vertex aV1, aV2;
    TopExp::Vertices(anEdge, aV1, aV2);
    if (!aV1.IsSame(aV2))
    {
      continue;
    }

    aDegenPnt.Append(BRep_Tool::Pnt(aV1));
    aDegenTol.Append(BRep_Tool::Tolerance(aV1));
  }

  // iterate by sides of the surface
  if (aType == STANDARD_TYPE(Geom_BSplineSurface))
  {
    occ::handle<Geom_BSplineSurface>  aBSpline = occ::down_cast<Geom_BSplineSurface>(theSurface);
    const NCollection_Array2<gp_Pnt>& aPoles   = aBSpline->Poles();

    occ::handle<Geom_BSplineSurface> aCopy;

    // iterate by sides: {U=0; V=0; U=1; V=1}
    int RowStart[4] = {aPoles.LowerRow(), aPoles.LowerRow(), aPoles.UpperRow(), aPoles.LowerRow()};
    int ColStart[4] = {aPoles.LowerCol(), aPoles.LowerCol(), aPoles.LowerCol(), aPoles.UpperCol()};
    int RowStep[4]  = {0, 1, 0, 1};
    int ColStep[4]  = {1, 0, 1, 0};
    int NbSteps[4]  = {aPoles.RowLength(),
                       aPoles.ColLength(),
                       aPoles.RowLength(),
                       aPoles.ColLength()};
    for (int iSide = 0; iSide < 4; iSide++)
    {
      // compute center of gravity of side poles
      gp_XYZ aSum;
      for (int iPole = 0; iPole < NbSteps[iSide]; iPole++)
      {
        aSum +=
          aPoles(RowStart[iSide] + iPole * RowStep[iSide], ColStart[iSide] + iPole * ColStep[iSide])
            .XYZ();
      }
      gp_Pnt aCenter(aSum / NbSteps[iSide]);

      // determine if all poles of the side fit into:
      bool isCollapsed = true; // aCenter precisely (with gp::Resolution())
      bool isSingular  = true; // aCenter with thePrecision
                               // clang-format off
      NCollection_LocalArray<bool,4> isDegenerated (aDegenPnt.Extent()); // degenerated vertex
                                                                       // clang-format on
      for (size_t iDegen = 0; iDegen < isDegenerated.Size(); ++iDegen)
        isDegenerated[iDegen] = true;
      for (int iPole = 0; iPole < NbSteps[iSide]; iPole++)
      {
        const gp_Pnt& aPole = aPoles(RowStart[iSide] + iPole * RowStep[iSide],
                                     ColStart[iSide] + iPole * ColStep[iSide]);

        // distance from CG
        double aDistCG = aCenter.Distance(aPole);
        if (aDistCG > gp::Resolution())
          isCollapsed = false;
        if (aDistCG > thePrecision)
          isSingular = false;

        // distances from degenerated points
        NCollection_List<gp_Pnt>::Iterator aDegPntIt(aDegenPnt);
        NCollection_List<double>::Iterator aDegTolIt(aDegenTol);
        for (size_t iDegen = 0; iDegen < isDegenerated.Size();
             aDegPntIt.Next(), aDegTolIt.Next(), ++iDegen)
        {
          if (isDegenerated[iDegen] && aDegPntIt.Value().Distance(aPole) >= aDegTolIt.Value())
          {
            isDegenerated[iDegen] = false;
          }
        }
      }
      if (isCollapsed)
      {
        continue; // already Ok, nothing to be done
      }

      // decide to collapse the side: either if it is singular with thePrecision,
      // or if it fits into one (and only one) degenerated point
      if (!isSingular)
      {
        int                                aNbFit = 0;
        NCollection_List<gp_Pnt>::Iterator aDegPntIt(aDegenPnt);
        NCollection_List<double>::Iterator aDegTolIt(aDegenTol);
        for (size_t iDegen = 0; iDegen < isDegenerated.Size(); ++iDegen)
        {
          if (isDegenerated[iDegen])
          {
            // remove degenerated point as soon as it fits at least one side, to prevent total
            // collapse
            aDegenPnt.Remove(aDegPntIt);
            aDegenTol.Remove(aDegTolIt);
            aNbFit++;
          }
          else
          {
            aDegPntIt.Next();
            aDegTolIt.Next();
          }
        }

        // if side fits more than one degenerated vertex, do not collapse it
        // to be on the safe side
        isSingular = (aNbFit == 1);
      }

      // do collapse
      if (isSingular)
      {
        if (aCopy.IsNull())
        {
          aCopy = occ::down_cast<Geom_BSplineSurface>(theSurface->Copy());
        }
        for (int iPole = 0; iPole < NbSteps[iSide]; iPole++)
        {
          aCopy->SetPole(RowStart[iSide] + iPole * RowStep[iSide],
                         ColStart[iSide] + iPole * ColStep[iSide],
                         aCenter);
        }
      }
    }

    if (!aCopy.IsNull())
      return aCopy;
  }

  return theSurface;
}
