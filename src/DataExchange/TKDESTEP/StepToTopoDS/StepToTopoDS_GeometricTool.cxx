// Created on: 1995-01-06
// Created by: Frederic MAUPAS
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

// pdn 11.01.99 #144 bm1_pe_t4 protection of exceptions in draw
//     abv 13.04.99 S4136: eliminate BRepAPI::Precision()

#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <ElCLib.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <Precision.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_Direction.hxx>
#include <StepGeom_Line.hxx>
#include <StepGeom_Pcurve.hxx>
#include <StepGeom_SeamCurve.hxx>
#include <StepGeom_Surface.hxx>
#include <StepGeom_SurfaceCurve.hxx>
#include <StepGeom_Vector.hxx>
#include <StepRepr_DefinitionalRepresentation.hxx>
#include <StepShape_EdgeLoop.hxx>
#include <StepShape_OrientedEdge.hxx>
#include <StepToTopoDS.hxx>
#include <StepToTopoDS_GeometricTool.hxx>
#include <Transfer_TransientProcess.hxx>

// ----------------------------------------------------------------------------
// Method  : HasPCurve
// Purpose : returns true if the surface curve has at least one pcurve lying
// on the surface
// ----------------------------------------------------------------------------
int StepToTopoDS_GeometricTool::PCurve(const occ::handle<StepGeom_SurfaceCurve>& SurfCurve,
                                                    const occ::handle<StepGeom_Surface>&      BasisSurf,
                                                    occ::handle<StepGeom_Pcurve>&             thePCurve,
                                                    const int               last)
{
  int NbAssGeom = SurfCurve->NbAssociatedGeometry();
  thePCurve.Nullify();
  for (int i = last + 1; i <= NbAssGeom; i++)
  {
    thePCurve = SurfCurve->AssociatedGeometryValue(i).Pcurve();
    if (!thePCurve.IsNull())
    {
      if (thePCurve->BasisSurface() == BasisSurf)
        return i;
    }
  }
  thePCurve.Nullify();
  return 0;
}

// ----------------------------------------------------------------------------
// Method  : IsSeamCurve
// Purpose : Two edges of the same wire references the same oriented edge
//           Then the surface_curve is a seam curve
// ----------------------------------------------------------------------------

bool StepToTopoDS_GeometricTool::IsSeamCurve(
  const occ::handle<StepGeom_SurfaceCurve>& SurfCurve,
  const occ::handle<StepGeom_Surface>&      Surf,
  const occ::handle<StepShape_Edge>&        StepEdge,
  const occ::handle<StepShape_EdgeLoop>&    EdgeLoop)
{
  if (SurfCurve->IsKind(STANDARD_TYPE(StepGeom_SeamCurve)))
    return true;

  if (SurfCurve->NbAssociatedGeometry() != 2)
    return false;

  occ::handle<StepGeom_Pcurve> StepPCurve1 = SurfCurve->AssociatedGeometryValue(1).Pcurve();
  occ::handle<StepGeom_Pcurve> StepPCurve2 = SurfCurve->AssociatedGeometryValue(2).Pcurve();

  // Do the two pcurves lye on the same surface ?

  if ((!StepPCurve1.IsNull() && !StepPCurve2.IsNull()) && (StepPCurve1->BasisSurface() == Surf)
      && (StepPCurve2->BasisSurface() == Surf))
  {

    int NbEdge = EdgeLoop->NbEdgeList();
    int nbOE   = 0;

    occ::handle<StepShape_OrientedEdge> OrEdge;

    for (int i = 1; i <= NbEdge; i++)
    {
      OrEdge = EdgeLoop->EdgeListValue(i);
      if (StepEdge == OrEdge->EdgeElement())
        nbOE++;
    }
    // two oriented edges of the same wire share the same edge
    if (nbOE == 2)
      return true;
  }
  return false;
}

// ----------------------------------------------------------------------------
// Method  : IsLikeSeam
// Purpose : The two pcurves lies on the same surface but on different wires.
//           This is typical situation in CATIA BRep : a cylinder is coded
//           with two faces on the same 'Closed' BSplineSurf, which in the
//           range of gp_Resolution is not identified as closed
// ----------------------------------------------------------------------------

bool StepToTopoDS_GeometricTool::IsLikeSeam(
  const occ::handle<StepGeom_SurfaceCurve>& SurfCurve,
  const occ::handle<StepGeom_Surface>&      Surf,
  const occ::handle<StepShape_Edge>&        StepEdge,
  const occ::handle<StepShape_EdgeLoop>&    EdgeLoop)
{
  if (SurfCurve->NbAssociatedGeometry() != 2)
    return false;

  occ::handle<StepGeom_Pcurve> StepPCurve1 = SurfCurve->AssociatedGeometryValue(1).Pcurve();
  occ::handle<StepGeom_Pcurve> StepPCurve2 = SurfCurve->AssociatedGeometryValue(2).Pcurve();

  // Do the two pcurves lye on the same surface ?

  if ((!StepPCurve1.IsNull() && !StepPCurve2.IsNull()) && (StepPCurve1->BasisSurface() == Surf)
      && (StepPCurve2->BasisSurface() == Surf))
  {

    int NbEdge = EdgeLoop->NbEdgeList();
    int nbOE   = 0;

    occ::handle<StepShape_OrientedEdge> OrEdge;

    for (int i = 1; i <= NbEdge; i++)
    {
      OrEdge = EdgeLoop->EdgeListValue(i);
      if (StepEdge == OrEdge->EdgeElement())
        nbOE++;
    }
    // the two oriented edges are not in the same wire
    if (nbOE == 1)
    {
      // check if the two pcurves are not identical ?
      occ::handle<StepGeom_Line> line1 =
        occ::down_cast<StepGeom_Line>(StepPCurve1->ReferenceToCurve()->ItemsValue(1));
      occ::handle<StepGeom_Line> line2 =
        occ::down_cast<StepGeom_Line>(StepPCurve2->ReferenceToCurve()->ItemsValue(1));
      if (!line1.IsNull() && !line2.IsNull())
      {
        // Same Origin in X OR Y && Same Vector ??
        // WITHIN A given tolerance !!!
        double DeltaX =
          std::abs(line1->Pnt()->CoordinatesValue(1) - line2->Pnt()->CoordinatesValue(1));
        double DeltaY =
          std::abs(line1->Pnt()->CoordinatesValue(2) - line2->Pnt()->CoordinatesValue(2));

        double DeltaDirX = std::abs(line1->Dir()->Orientation()->DirectionRatiosValue(1)
                                           - line2->Dir()->Orientation()->DirectionRatiosValue(1));
        double DeltaDirY = std::abs(line1->Dir()->Orientation()->DirectionRatiosValue(2)
                                           - line2->Dir()->Orientation()->DirectionRatiosValue(2));

        // clang-format off
        double preci2d = Precision::PConfusion(); //:S4136: Parametric(BRepAPI::Precision(),10);
        // clang-format on

        if ((DeltaX < preci2d) || (DeltaY < preci2d))
          return ((DeltaDirX < preci2d) && (DeltaDirY < preci2d));
        else
          return false;

        // Warning : la manipulation de tolerances dans ce contexte est un
        //           peu trop dangeureux.
        //           il serait preferable de plus de ne pas restreindre au
        //           cas de deux lignes.
        //           un mode plus convenable de detection serait de se servir
        //           des isos (ou bords naturels) de la surface de base
        //           et de detecter que les deux courbes se trouvent sur le
        //           bord de fermeture.
        //           il faut toutefois prevoir le cas ou les deux courbes
        //           sont confondues (ex : CATIA, "couture" de separation
        //           en deux faces d un support periodique.
        //  Ce travail reste evidement A FAIRE !!! ...
      }
      else
        return false;
    }
    return false;
  }
  return false;
}

// ----------------------------------------------------------------------------
// Method  : UpdateParam3d
// Purpose : According to the type of curve update parameter (w1>w2)
//           This situation occurs when an edge crosses the parametric origin.
// ----------------------------------------------------------------------------

bool StepToTopoDS_GeometricTool::UpdateParam3d(const occ::handle<Geom_Curve>& theCurve,
                                                           double&            w1,
                                                           double&            w2,
                                                           const double       preci)
{
  // w1 et/ou w2 peuvent etre en dehors des bornes naturelles de la courbe.
  // On donnera alors la valeur en bout a w1 et/ou w2

  double cf = theCurve->FirstParameter();
  double cl = theCurve->LastParameter();

  if (theCurve->IsKind(STANDARD_TYPE(Geom_BoundedCurve)) && !theCurve->IsClosed())
  {
    if (w1 < cf)
    {
#ifdef OCCT_DEBUG
      std::cout << "Update Edge First Parameter to Curve First Parameter" << std::endl;
#endif
      w1 = cf;
    }
    else if (w1 > cl)
    {
#ifdef OCCT_DEBUG
      std::cout << "Update Edge First Parameter to Curve Last Parameter" << std::endl;
#endif
      w1 = cl;
    }
    if (w2 < cf)
    {
#ifdef OCCT_DEBUG
      std::cout << "Update Edge Last Parameter to Curve First Parameter" << std::endl;
#endif
      w2 = cf;
    }
    else if (w2 > cl)
    {
#ifdef OCCT_DEBUG
      std::cout << "Update Edge Last Parameter to Curve Last Parameter" << std::endl;
#endif
      w2 = cl;
    }
  }

  if (w1 < w2)
    return true;

  if (theCurve->IsPeriodic())
  {
    // clang-format off
    ElCLib::AdjustPeriodic(cf, cl, Precision::PConfusion(), w1, w2); //:a7 abv 11 Feb 98: preci -> PConfusion()
    // clang-format on
  }
  else if (theCurve->IsClosed())
  {
    // l'un des points projecte se trouve sur l'origine du parametrage
    // de la courbe 3D. L algo a donne cl +- preci au lieu de cf ou vice-versa
    // DANGER precision 3d applique a une espace 1d

    // w2 = cf au lieu de w2 = cl
    if (std::abs(w2 - cf) < Precision::PConfusion() /*preci*/)
    {
      w2 = cl;
    }
    // w1 = cl au lieu de w1 = cf
    else if (std::abs(w1 - cl) < Precision::PConfusion() /*preci*/)
    {
      w1 = cf;
    }
    // on se trouve dans un cas ou l origine est traversee
    // illegal sur une courbe fermee non periodique
    // on inverse quand meme les parametres !!!!!!
    else
    {
      //: S4136 abv 20 Apr 99: r0701_ug.stp #6230: add check in 3d
      if (theCurve->Value(w1).Distance(theCurve->Value(cf)) < preci)
      {
        w1 = cf;
      }
      if (theCurve->Value(w2).Distance(theCurve->Value(cl)) < preci)
      {
        w2 = cl;
      }
      if (fabs(w2 - w1) < Precision::PConfusion())
      {
        w1 = cf;
        w2 = cl;
      }
      else if (w1 > w2)
      {
#ifdef OCCT_DEBUG
        std::cout << "Warning : parameter range of edge crossing non periodic curve origin"
                  << std::endl;
#endif
        double tmp = w1;
        w1                = w2;
        w2                = tmp;
      }
    }
  }
  // The curve is closed within the 3D tolerance
  else if (theCurve->IsKind(STANDARD_TYPE(Geom_BSplineCurve)))
  {
    occ::handle<Geom_BSplineCurve> aBSpline = occ::down_cast<Geom_BSplineCurve>(theCurve);
    if (aBSpline->StartPoint().Distance(aBSpline->EndPoint()) <= preci)
    {
      //: S4136	<= BRepAPI::Precision()) {
      // l'un des points projecte se trouve sur l'origine du parametrage
      // de la courbe 3D. L algo a donne cl +- preci au lieu de cf ou vice-versa
      // DANGER precision 3d applique a une espace 1d

      // w2 = cf au lieu de w2 = cl
      if (std::abs(w2 - cf) < Precision::PConfusion())
      {
        w2 = cl;
      }
      // w1 = cl au lieu de w1 = cf
      else if (std::abs(w1 - cl) < Precision::PConfusion())
      {
        w1 = cf;
      }
      // on se trouve dans un cas ou l origine est traversee
      // illegal sur une courbe fermee non periodique
      // on inverse quand meme les parametres !!!!!!
      else
      {
#ifdef OCCT_DEBUG
        std::cout << "Warning : parameter range of edge crossing non periodic curve origin"
                  << std::endl;
#endif
        double tmp = w1;
        w1                = w2;
        w2                = tmp;
      }
    }
    // abv 15.03.00 #72 bm1_pe_t4 protection of exceptions in draw
    else if (w1 > w2)
    {
#ifdef OCCT_DEBUG
      std::cout << "Warning: parameter range is bad; curve reversed" << std::endl;
#endif
      w1 = theCurve->ReversedParameter(w1);
      w2 = theCurve->ReversedParameter(w2);
      theCurve->Reverse();
    }
    //: j9 abv 11 Dec 98: PRO7747 #4875, after :j8:    else
    if (w1 == w2)
    { // gka 10.07.1998 file PRO7656 entity 33334
      w1 = cf;
      w2 = cl;
      return false;
    }
  }
  else
  {
#ifdef OCCT_DEBUG
    std::cout << "UpdateParam3d Failed" << std::endl;
    std::cout << "  - Curve Type : " << theCurve->DynamicType() << std::endl;
    std::cout << "  - Param 1    : " << w1 << std::endl;
    std::cout << "  - Param 2    : " << w2 << std::endl;
#endif
    // abv 15.03.00 #72 bm1_pe_t4 protection of exceptions in draw
    if (w1 > w2)
    {
#ifdef OCCT_DEBUG
      std::cout << "Warning: parameter range is bad; curve reversed" << std::endl;
#endif
      w1 = theCurve->ReversedParameter(w1);
      w2 = theCurve->ReversedParameter(w2);
      theCurve->Reverse();
    }
    // pdn 11.01.99 #144 bm1_pe_t4 protection of exceptions in draw
    if (w1 == w2)
    {
      w1 -= Precision::PConfusion();
      w2 += Precision::PConfusion();
    }
    return false;
  }
  return true;
}
