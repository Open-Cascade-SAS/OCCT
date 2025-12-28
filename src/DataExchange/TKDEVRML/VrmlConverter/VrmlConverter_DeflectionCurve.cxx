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

#include <Adaptor3d_Curve.hxx>
#include <Bnd_Box.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <gp_Circ.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Vrml_Coordinate3.hxx>
#include <Vrml_IndexedLineSet.hxx>
#include <Vrml_Material.hxx>
#include <Vrml_Separator.hxx>
#include <VrmlConverter_DeflectionCurve.hxx>
#include <VrmlConverter_Drawer.hxx>
#include <VrmlConverter_LineAspect.hxx>

//=================================================================================================

static void FindLimits(const Adaptor3d_Curve& aCurve,
                       const double    aLimit,
                       double&         First,
                       double&         Last)
{
  First                     = aCurve.FirstParameter();
  Last                      = aCurve.LastParameter();
  bool firstInf = Precision::IsNegativeInfinite(First);
  bool lastInf  = Precision::IsPositiveInfinite(Last);

  if (firstInf || lastInf)
  {
    gp_Pnt        P1, P2;
    double delta = 1;
    if (firstInf && lastInf)
    {
      do
      {
        delta *= 2;
        First = -delta;
        Last  = delta;
        aCurve.D0(First, P1);
        aCurve.D0(Last, P2);
      } while (P1.Distance(P2) < aLimit);
    }
    else if (firstInf)
    {
      aCurve.D0(Last, P2);
      do
      {
        delta *= 2;
        First = Last - delta;
        aCurve.D0(First, P1);
      } while (P1.Distance(P2) < aLimit);
    }
    else if (lastInf)
    {
      aCurve.D0(First, P1);
      do
      {
        delta *= 2;
        Last = First + delta;
        aCurve.D0(Last, P2);
      } while (P1.Distance(P2) < aLimit);
    }
  }
}

//=================================================================================================

static void PrintPoints(occ::handle<NCollection_HArray1<gp_Vec>>&        aHAV1,
                        occ::handle<NCollection_HArray1<int>>&   aHAI1,
                        const occ::handle<VrmlConverter_Drawer>& aDrawer,
                        Standard_OStream&                   anOStream)
{
  // creation of Vrml objects
  occ::handle<VrmlConverter_LineAspect> LA = new VrmlConverter_LineAspect;
  LA                                  = aDrawer->LineAspect();

  // Separator 1 {
  Vrml_Separator SE1;
  SE1.Print(anOStream);
  // Material
  if (LA->HasMaterial())
  {

    occ::handle<Vrml_Material> M;
    M = LA->Material();

    M->Print(anOStream);
  }
  // Coordinate3
  occ::handle<Vrml_Coordinate3> C3 = new Vrml_Coordinate3(aHAV1);
  C3->Print(anOStream);
  // IndexedLineSet
  Vrml_IndexedLineSet ILS;
  ILS.SetCoordIndex(aHAI1);
  ILS.Print(anOStream);
  // Separator 1 }
  SE1.Print(anOStream);
}

//=================================================================================================

static void DrawCurve(Adaptor3d_Curve&                    aCurve,
                      const double                 TheDeflection,
                      const double                 U1,
                      const double                 U2,
                      const occ::handle<VrmlConverter_Drawer>& aDrawer, // for passing of LineAspect
                      Standard_OStream&                   anOStream)
{
  int                 i;
  bool                 key = false;
  occ::handle<NCollection_HArray1<gp_Vec>>      HAV1;
  occ::handle<NCollection_HArray1<int>> HAI1;

  switch (aCurve.GetType())
  {
    case GeomAbs_Line: {
      gp_Vec V;
      key  = true;
      HAV1 = new NCollection_HArray1<gp_Vec>(1, 2);
      HAI1 = new NCollection_HArray1<int>(1, 3);

      // array of coordinates of line
      gp_Pnt p = aCurve.Value(U1);
      V.SetX(p.X());
      V.SetY(p.Y());
      V.SetZ(p.Z());
      HAV1->SetValue(1, V);

      p = aCurve.Value(U2);
      V.SetX(p.X());
      V.SetY(p.Y());
      V.SetZ(p.Z());
      HAV1->SetValue(2, V);

      // array of indexes of line
      HAI1->SetValue(1, 0);
      HAI1->SetValue(2, 1);
      HAI1->SetValue(3, -1);
    }
    break;
    case GeomAbs_Circle: {
      double Radius = aCurve.Circle().Radius();
      if (!Precision::IsInfinite(Radius))
      {
        double    DU = std::sqrt(8.0 * TheDeflection / Radius);
        int N  = int(std::abs(U2 - U1) / DU);

        if (N > 0)
        {

          gp_Vec V;
          key  = true;
          HAV1 = new NCollection_HArray1<gp_Vec>(1, N + 1);
          HAI1 = new NCollection_HArray1<int>(1, N + 2);

          DU = (U2 - U1) / N;
          double U;
          gp_Pnt        p;

          for (int Index = 1; Index <= N + 1; Index++)
          {
            U = U1 + (Index - 1) * DU;
            p = aCurve.Value(U);

            V.SetX(p.X());
            V.SetY(p.Y());
            V.SetZ(p.Z());
            HAV1->SetValue(Index, V);
            HAI1->SetValue(Index, Index - 1);
          }
          /*
               if( HAV1->Value(1).IsEqual( HAV1->Value(N+1),Precision::Confusion(),
             Precision::Angular() ) )
                 {
                   HAI1->SetValue(N+1, 0);
                 }
          */
          HAI1->SetValue(HAI1->Upper(), -1);
        }
      }
    }
    break;

    default: {
      GCPnts_QuasiUniformDeflection Algo(aCurve, TheDeflection, U1, U2);
      if (Algo.IsDone())
      {

        int NumberOfPoints = Algo.NbPoints();
        if (NumberOfPoints > 0)
        {

          gp_Vec V;
          key  = true;
          HAV1 = new NCollection_HArray1<gp_Vec>(1, NumberOfPoints);
          HAI1 = new NCollection_HArray1<int>(1, NumberOfPoints + 1);
          gp_Pnt p;

          for (i = 1; i <= NumberOfPoints; i++)
          {
            p = Algo.Value(i);
            V.SetX(p.X());
            V.SetY(p.Y());
            V.SetZ(p.Z());
            HAV1->SetValue(i, V);
          }

          for (i = HAI1->Lower(); i < HAI1->Upper(); i++)
          {
            HAI1->SetValue(i, i - 1);
          }
          HAI1->SetValue(HAI1->Upper(), -1);
        }
      }
      // else
      // cannot draw with respect to a maximal chordial deviation
    }
  }

  // std::cout  << " Array HAI1 - coordIndex " << std::endl;
  //
  // for ( i=HAI1->Lower(); i <= HAI1->Upper(); i++ )
  //   {
  //     std::cout << HAI1->Value(i) << std::endl;
  //   }

  if (key)
  {
    PrintPoints(HAV1, HAI1, aDrawer, anOStream);
  }
}

//=================================================================================================

static double GetDeflection(const Adaptor3d_Curve&              aCurve,
                                   const double                 U1,
                                   const double                 U2,
                                   const occ::handle<VrmlConverter_Drawer>& aDrawer)
{

  double theRequestedDeflection;
  if (aDrawer->TypeOfDeflection() == Aspect_TOD_RELATIVE) // TOD_RELATIVE, TOD_ABSOLUTE
  {
    Bnd_Box box;
    BndLib_Add3dCurve::Add(aCurve, U1, U2, Precision::Confusion(), box);

    double Xmin, Xmax, Ymin, Ymax, Zmin, Zmax, diagonal;
    box.Get(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
    if (!(box.IsOpenXmin() || box.IsOpenXmax() || box.IsOpenYmin() || box.IsOpenYmax()
          || box.IsOpenZmin() || box.IsOpenZmax()))
    {
      diagonal = std::sqrt((Xmax - Xmin) * (Xmax - Xmin) + (Ymax - Ymin) * (Ymax - Ymin)
                           + (Zmax - Zmin) * (Zmax - Zmin));
      diagonal = std::max(diagonal, Precision::Confusion());
      theRequestedDeflection = aDrawer->DeviationCoefficient() * diagonal;
    }
    else
    {
      diagonal               = 1000000.;
      theRequestedDeflection = aDrawer->DeviationCoefficient() * diagonal;
    }
    //      std::cout << "diagonal = " << diagonal << std::endl;
    //      std::cout << "theRequestedDeflection = " << theRequestedDeflection << std::endl;
  }
  else
  {
    theRequestedDeflection = aDrawer->MaximalChordialDeviation();
  }
  return theRequestedDeflection;
}

//=================================================================================================

void VrmlConverter_DeflectionCurve::Add(Standard_OStream&                   anOStream,
                                        Adaptor3d_Curve&                    aCurve,
                                        const occ::handle<VrmlConverter_Drawer>& aDrawer)
{

  double V1, V2;
  double aLimit = aDrawer->MaximalParameterValue();
  FindLimits(aCurve, aLimit, V1, V2);

  double theRequestedDeflection = GetDeflection(aCurve, V1, V2, aDrawer);

  DrawCurve(aCurve, theRequestedDeflection, V1, V2, aDrawer, anOStream);
}

//=================================================================================================

void VrmlConverter_DeflectionCurve::Add(Standard_OStream&                   anOStream,
                                        Adaptor3d_Curve&                    aCurve,
                                        const double                 U1,
                                        const double                 U2,
                                        const occ::handle<VrmlConverter_Drawer>& aDrawer)
{
  double V1 = U1;
  double V2 = U2;

  if (Precision::IsNegativeInfinite(V1))
    V1 = -aDrawer->MaximalParameterValue();
  if (Precision::IsPositiveInfinite(V2))
    V2 = aDrawer->MaximalParameterValue();

  double theRequestedDeflection = GetDeflection(aCurve, V1, V2, aDrawer);
  DrawCurve(aCurve, theRequestedDeflection, V1, V2, aDrawer, anOStream);
}

//=================================================================================================

void VrmlConverter_DeflectionCurve::Add(Standard_OStream&   anOStream,
                                        Adaptor3d_Curve&    aCurve,
                                        const double aDeflection,
                                        const double aLimit)
{
  double V1, V2;
  FindLimits(aCurve, aLimit, V1, V2);

  occ::handle<VrmlConverter_Drawer>     aDrawer = new VrmlConverter_Drawer;
  occ::handle<VrmlConverter_LineAspect> la      = new VrmlConverter_LineAspect;
  aDrawer->SetLineAspect(la);

  DrawCurve(aCurve, aDeflection, V1, V2, aDrawer, anOStream);
}

//=================================================================================================

void VrmlConverter_DeflectionCurve::Add(Standard_OStream&                   anOStream,
                                        Adaptor3d_Curve&                    aCurve,
                                        const double                 aDeflection,
                                        const occ::handle<VrmlConverter_Drawer>& aDrawer)
{
  double aLimit = aDrawer->MaximalParameterValue();
  double V1, V2;
  FindLimits(aCurve, aLimit, V1, V2);

  DrawCurve(aCurve, aDeflection, V1, V2, aDrawer, anOStream);
}

//=================================================================================================

void VrmlConverter_DeflectionCurve::Add(Standard_OStream&   anOStream,
                                        Adaptor3d_Curve&    aCurve,
                                        const double U1,
                                        const double U2,
                                        const double aDeflection)
{
  occ::handle<VrmlConverter_Drawer>     aDrawer = new VrmlConverter_Drawer;
  occ::handle<VrmlConverter_LineAspect> la      = new VrmlConverter_LineAspect;
  aDrawer->SetLineAspect(la);

  DrawCurve(aCurve, aDeflection, U1, U2, aDrawer, anOStream);
}

//=================================================================================================

void VrmlConverter_DeflectionCurve::Add(Standard_OStream&                    anOStream,
                                        const Adaptor3d_Curve&               aCurve,
                                        const occ::handle<NCollection_HArray1<double>>& aParams,
                                        const int               aNbNodes,
                                        const occ::handle<VrmlConverter_Drawer>&  aDrawer)
{
  occ::handle<NCollection_HArray1<gp_Vec>>      aHAV1 = new NCollection_HArray1<gp_Vec>(1, aNbNodes);
  occ::handle<NCollection_HArray1<int>> aHAI1 = new NCollection_HArray1<int>(1, aNbNodes + 1);

  int i;
  gp_Pnt           aPoint;
  gp_Vec           aVec;
  for (i = 1; i <= aNbNodes; i++)
  {
    double aParam = aParams->Value(aParams->Lower() + i - 1);
    aPoint               = aCurve.Value(aParam);
    aVec.SetX(aPoint.X());
    aVec.SetY(aPoint.Y());
    aVec.SetZ(aPoint.Z());
    aHAV1->SetValue(i, aVec);
  }

  for (i = aHAI1->Lower(); i < aHAI1->Upper(); i++)
  {
    aHAI1->SetValue(i, i - 1);
  }
  aHAI1->SetValue(aHAI1->Upper(), -1);

  PrintPoints(aHAV1, aHAI1, aDrawer, anOStream);
}
