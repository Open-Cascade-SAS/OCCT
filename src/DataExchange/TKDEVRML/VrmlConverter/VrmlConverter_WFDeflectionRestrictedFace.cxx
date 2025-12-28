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

#include <Adaptor2d_Curve2d.hxx>
#include <Adaptor3d_IsoCurve.hxx>
#include <Bnd_Box.hxx>
#include <Bnd_Box2d.hxx>
#include <BndLib_AddSurface.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Hatch_Hatcher.hxx>
#include <Precision.hxx>
#include <StdPrs_ToolRFace.hxx>
#include <Vrml_Material.hxx>
#include <Vrml_Separator.hxx>
#include <VrmlConverter_DeflectionCurve.hxx>
#include <VrmlConverter_Drawer.hxx>
#include <VrmlConverter_IsoAspect.hxx>
#include <VrmlConverter_WFDeflectionRestrictedFace.hxx>

//=================================================================================================

static double GetDeflection(const occ::handle<BRepAdaptor_Surface>&  aFace,
                                   const occ::handle<VrmlConverter_Drawer>& aDrawer)
{

  double theRequestedDeflection;
  if (aDrawer->TypeOfDeflection() == Aspect_TOD_RELATIVE) // TOD_RELATIVE, TOD_ABSOLUTE
  {
    Bnd_Box box;
    BndLib_AddSurface::Add(aFace->Surface(), Precision::Confusion(), box);

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

void VrmlConverter_WFDeflectionRestrictedFace::Add(Standard_OStream&                   anOStream,
                                                   const occ::handle<BRepAdaptor_Surface>&  aFace,
                                                   const bool              DrawUIso,
                                                   const bool              DrawVIso,
                                                   const double                 Deflection,
                                                   const int              NBUiso,
                                                   const int              NBViso,
                                                   const occ::handle<VrmlConverter_Drawer>& aDrawer)
{

  StdPrs_ToolRFace ToolRst(aFace);
  double    UF, UL, VF, VL;
  UF = aFace->FirstUParameter();
  UL = aFace->LastUParameter();
  VF = aFace->FirstVParameter();
  VL = aFace->LastVParameter();

  double aLimit = aDrawer->MaximalParameterValue();

  // compute bounds of the restriction
  double    UMin, UMax, VMin, VMax;
  int i;

  UMin = UF;
  UMax = UL;
  VMin = VF;
  VMax = VL;

  // load the isos
  Hatch_Hatcher    isobuild(1.e-5, ToolRst.IsOriented());
  bool UClosed = aFace->IsUClosed();
  bool VClosed = aFace->IsVClosed();

  if (!UClosed)
  {
    UMin = UMin + (UMax - UMin) / 1000.;
    UMax = UMax - (UMax - UMin) / 1000.;
  }

  if (!VClosed)
  {
    VMin = VMin + (VMax - VMin) / 1000.;
    VMax = VMax - (VMax - VMin) / 1000.;
  }

  if (DrawUIso)
  {
    if (NBUiso > 0)
    {
      UClosed          = false;
      double du = UClosed ? (UMax - UMin) / NBUiso : (UMax - UMin) / (1 + NBUiso);
      for (i = 1; i <= NBUiso; i++)
      {
        isobuild.AddXLine(UMin + du * i);
      }
    }
  }
  if (DrawVIso)
  {
    if (NBViso > 0)
    {
      VClosed          = false;
      double dv = VClosed ? (VMax - VMin) / NBViso : (VMax - VMin) / (1 + NBViso);
      for (i = 1; i <= NBViso; i++)
      {
        isobuild.AddYLine(VMin + dv * i);
      }
    }
  }

  // trim the isos
  gp_Pnt2d      P1, P2;
  double U1, U2;
  gp_Pnt        dummypnt;
  for (ToolRst.Init(); ToolRst.More(); ToolRst.Next())
  {
    TopAbs_Orientation       Orient    = ToolRst.Orientation();
    const Adaptor2d_Curve2d* TheRCurve = &ToolRst.Value();
    if (TheRCurve->GetType() != GeomAbs_Line)
    {
      GCPnts_QuasiUniformDeflection UDP(*TheRCurve, Deflection);
      if (UDP.IsDone())
      {
        int NumberOfPoints = UDP.NbPoints();
        if (NumberOfPoints >= 2)
        {
          dummypnt = UDP.Value(1);
          P2.SetCoord(dummypnt.X(), dummypnt.Y());
          for (i = 2; i <= NumberOfPoints; i++)
          {
            P1       = P2;
            dummypnt = UDP.Value(i);
            P2.SetCoord(dummypnt.X(), dummypnt.Y());
            if (Orient == TopAbs_FORWARD)
              isobuild.Trim(P1, P2);
            else
              isobuild.Trim(P2, P1);
          }
        }
      }
      else
      {
        std::cout << "Cannot evaluate curve on surface" << std::endl;
      }
    }
    else
    {
      U1 = TheRCurve->FirstParameter();
      U2 = TheRCurve->LastParameter();
      P1 = TheRCurve->Value(U1);
      P2 = TheRCurve->Value(U2);
      if (Orient == TopAbs_FORWARD)
        isobuild.Trim(P1, P2);
      else
        isobuild.Trim(P2, P1);
    }
  }

  // draw the isos

  Adaptor3d_IsoCurve anIso;
  anIso.Load(aFace);
  int NumberOfLines = isobuild.NbLines();

  occ::handle<VrmlConverter_LineAspect> latmp = new VrmlConverter_LineAspect;
  latmp->SetMaterial(aDrawer->LineAspect()->Material());
  latmp->SetHasMaterial(aDrawer->LineAspect()->HasMaterial());

  occ::handle<VrmlConverter_IsoAspect> iautmp = new VrmlConverter_IsoAspect;
  iautmp->SetMaterial(aDrawer->UIsoAspect()->Material());
  iautmp->SetHasMaterial(aDrawer->UIsoAspect()->HasMaterial());
  iautmp->SetNumber(aDrawer->UIsoAspect()->Number());

  occ::handle<VrmlConverter_IsoAspect> iavtmp = new VrmlConverter_IsoAspect;
  iavtmp->SetMaterial(aDrawer->VIsoAspect()->Material());
  iavtmp->SetHasMaterial(aDrawer->VIsoAspect()->HasMaterial());
  iavtmp->SetNumber(aDrawer->VIsoAspect()->Number());

  occ::handle<VrmlConverter_LineAspect> laU = new VrmlConverter_LineAspect;
  occ::handle<VrmlConverter_LineAspect> laV = new VrmlConverter_LineAspect;

  laU = aDrawer->UIsoAspect();
  laV = aDrawer->VIsoAspect();

  Vrml_Separator SE1;
  Vrml_Separator SE2;
  Vrml_Separator SE3;

  bool flag = false; // to check a call of Vrml_Separator.Print(anOStream)

  SE1.Print(anOStream);

  if (DrawUIso)
  {
    if (NBUiso > 0)
    {

      if (laU->HasMaterial())
      {

        occ::handle<Vrml_Material> MU;
        MU = laU->Material();

        MU->Print(anOStream);
        laU->SetHasMaterial(false);

        flag = true;
        // Separator 2 {
        SE2.Print(anOStream);
      }
      aDrawer->SetLineAspect(laU);

      for (i = 1; i <= NumberOfLines; i++)
      {
        if (isobuild.IsXLine(i))
        {
          int NumberOfIntervals = isobuild.NbIntervals(i);
          double    Coord             = isobuild.Coordinate(i);
          for (int j = 1; j <= NumberOfIntervals; j++)
          {
            double b1 = isobuild.Start(i, j), b2 = isobuild.End(i, j);

            b1 = b1 == RealFirst() ? -aLimit : b1;
            b2 = b2 == RealLast() ? aLimit : b2;

            anIso.Load(GeomAbs_IsoU, Coord, b1, b2);

            VrmlConverter_DeflectionCurve::Add(anOStream, anIso, Deflection, aDrawer);
          }
        }
      }
      if (flag)
      {
        // Separator 2 }
        SE2.Print(anOStream);
        flag = false;
      }
    }
  }

  if (DrawVIso)
  {
    if (NBViso > 0)
    {
      if (laV->HasMaterial())
      {

        occ::handle<Vrml_Material> MV;
        MV = laV->Material();

        MV->Print(anOStream);
        laV->SetHasMaterial(false);
        flag = true;

        // Separator 3 {
        SE3.Print(anOStream);
      }

      aDrawer->SetLineAspect(laV);

      for (i = 1; i <= NumberOfLines; i++)
      {
        if (isobuild.IsYLine(i))
        {
          int NumberOfIntervals = isobuild.NbIntervals(i);
          double    Coord             = isobuild.Coordinate(i);
          for (int j = 1; j <= NumberOfIntervals; j++)
          {
            double b1 = isobuild.Start(i, j), b2 = isobuild.End(i, j);

            b1 = b1 == RealFirst() ? -aLimit : b1;
            b2 = b2 == RealLast() ? aLimit : b2;

            anIso.Load(GeomAbs_IsoV, Coord, b1, b2);

            VrmlConverter_DeflectionCurve::Add(anOStream, anIso, Deflection, aDrawer);
          }
        }
      }

      if (flag)
      {
        // Separator 3 }
        SE3.Print(anOStream);
        flag = false;
      }
    }
  }

  // Separator 1 }
  SE1.Print(anOStream);

  aDrawer->SetLineAspect(latmp);
  aDrawer->SetUIsoAspect(iautmp);
  aDrawer->SetVIsoAspect(iavtmp);
}

//=================================================================================================

void VrmlConverter_WFDeflectionRestrictedFace::Add(Standard_OStream&                   anOStream,
                                                   const occ::handle<BRepAdaptor_Surface>&  aFace,
                                                   const occ::handle<VrmlConverter_Drawer>& aDrawer)
{

  double Deflection = GetDeflection(aFace, aDrawer);

  int finu = aDrawer->UIsoAspect()->Number();
  int finv = aDrawer->VIsoAspect()->Number();

  VrmlConverter_WFDeflectionRestrictedFace::Add(anOStream,
                                                aFace,
                                                true,
                                                true,
                                                Deflection,
                                                finu,
                                                finv,
                                                aDrawer);
}

//=================================================================================================

void VrmlConverter_WFDeflectionRestrictedFace::AddUIso(Standard_OStream&                  anOStream,
                                                       const occ::handle<BRepAdaptor_Surface>& aFace,
                                                       const occ::handle<VrmlConverter_Drawer>& aDrawer)
{

  double Deflection = GetDeflection(aFace, aDrawer);

  int finu = aDrawer->UIsoAspect()->Number();
  int finv = aDrawer->VIsoAspect()->Number();
  VrmlConverter_WFDeflectionRestrictedFace::Add(anOStream,
                                                aFace,
                                                true,
                                                false,
                                                Deflection,
                                                finu,
                                                finv,
                                                aDrawer);
}

//=================================================================================================

void VrmlConverter_WFDeflectionRestrictedFace::AddVIso(Standard_OStream&                  anOStream,
                                                       const occ::handle<BRepAdaptor_Surface>& aFace,
                                                       const occ::handle<VrmlConverter_Drawer>& aDrawer)
{

  double Deflection = GetDeflection(aFace, aDrawer);

  int finu = aDrawer->UIsoAspect()->Number();
  int finv = aDrawer->VIsoAspect()->Number();
  VrmlConverter_WFDeflectionRestrictedFace::Add(anOStream,
                                                aFace,
                                                false,
                                                true,
                                                Deflection,
                                                finu,
                                                finv,
                                                aDrawer);
}
