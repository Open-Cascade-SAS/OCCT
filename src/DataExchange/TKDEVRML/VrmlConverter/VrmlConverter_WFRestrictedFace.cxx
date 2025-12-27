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

#include <Adaptor3d_IsoCurve.hxx>
#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <Hatch_Hatcher.hxx>
#include <Precision.hxx>
#include <StdPrs_ToolRFace.hxx>
#include <Vrml_Material.hxx>
#include <Vrml_Separator.hxx>
#include <VrmlConverter_Curve.hxx>
#include <VrmlConverter_Drawer.hxx>
#include <VrmlConverter_IsoAspect.hxx>
#include <VrmlConverter_WFRestrictedFace.hxx>

//=================================================================================================

void VrmlConverter_WFRestrictedFace::Add(Standard_OStream&                   anOStream,
                                         const occ::handle<BRepAdaptor_Surface>&  aFace,
                                         const bool              DrawUIso,
                                         const bool              DrawVIso,
                                         const int              NBUiso,
                                         const int              NBViso,
                                         const occ::handle<VrmlConverter_Drawer>& aDrawer)
{
  double    aLimit   = aDrawer->MaximalParameterValue();
  int nbPoints = aDrawer->Discretisation();

  StdPrs_ToolRFace ToolRst(aFace);

  // compute bounds of the restriction
  double    UMin, UMax, VMin, VMax;
  int i;
  gp_Pnt2d         P1, P2;
  Bnd_Box2d        B;

  for (ToolRst.Init(); ToolRst.More(); ToolRst.Next())
  {
    const Adaptor2d_Curve2d& TheRCurve = ToolRst.Value();
    BndLib_Add2dCurve::Add(TheRCurve, Precision::PConfusion(), B);
  }

  B.Get(UMin, VMin, UMax, VMax);

  // load the infinite isos
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
  double U1, U2, U, DU;

  for (ToolRst.Init(); ToolRst.More(); ToolRst.Next())
  {
    TopAbs_Orientation       Orient    = ToolRst.Orientation();
    const Adaptor2d_Curve2d* TheRCurve = &ToolRst.Value();
    U1                                 = TheRCurve->FirstParameter();
    U2                                 = TheRCurve->LastParameter();
    if (TheRCurve->GetType() != GeomAbs_Line)
    {
      DU = (U2 - U1) / (nbPoints - 1);
      P2 = TheRCurve->Value(U1);
      for (i = 2; i <= nbPoints; i++)
      {
        U  = U1 + (i - 1) * DU;
        P1 = P2;
        P2 = TheRCurve->Value(U);
        if (Orient == TopAbs_FORWARD)
          isobuild.Trim(P1, P2);
        else
          isobuild.Trim(P2, P1);
      }
    }
    else
    {
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

  //     std::cout << std::endl;

  //     std::cout << "Face:aDrawer>UMaterial1 = " << aDrawer->UIsoAspect()->HasMaterial() <<
  //     std::endl; std::cout << "Face:aDrawer>VMaterial1 = " <<
  //     aDrawer->VIsoAspect()->HasMaterial() << std::endl; std::cout << "Face:aDrawer>LineAspect1 =
  //     " << aDrawer->LineAspect()->HasMaterial() << std::endl; std::cout << "Face:la1 = " <<
  //     latmp->HasMaterial() << std::endl; std::cout << "Face:laU1 = " << laU->HasMaterial() <<
  //     std::endl; std::cout << "Face:laV1 = " << laV->HasMaterial() << std::endl; std::cout <<
  //     "Face:iau1 = " << iautmp->HasMaterial() << std::endl; std::cout << "Face:iav1 = " <<
  //     iavtmp->HasMaterial() << std::endl;

  // creation of Vrml objects

  // Separator 1 {
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

            VrmlConverter_Curve::Add(anIso, aDrawer, anOStream);
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

            VrmlConverter_Curve::Add(anIso, aDrawer, anOStream);
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

  //     std::cout << std::endl;

  //     std::cout << "Face:aDrawer>UMaterial4 = " << aDrawer->UIsoAspect()->HasMaterial() <<
  //     std::endl; std::cout << "Face:aDrawer>VMaterial4 = " <<
  //     aDrawer->VIsoAspect()->HasMaterial() << std::endl; std::cout << "Face:aDrawer>LineAspect4 =
  //     " << aDrawer->LineAspect()->HasMaterial() << std::endl; std::cout << "Face:la4 = " <<
  //     latmp->HasMaterial() << std::endl; std::cout << "Face:laU4 = " << laU->HasMaterial() <<
  //     std::endl; std::cout << "Face:laV4 = " << laV->HasMaterial() << std::endl; std::cout <<
  //     "Face:iau4 = " << iautmp->HasMaterial() << std::endl; std::cout << "Face:iav4 = " <<
  //     iavtmp->HasMaterial() << std::endl;
}

//=================================================================================================

void VrmlConverter_WFRestrictedFace::Add(Standard_OStream&                   anOStream,
                                         const occ::handle<BRepAdaptor_Surface>&  aFace,
                                         const occ::handle<VrmlConverter_Drawer>& aDrawer)
{

  int finu = aDrawer->UIsoAspect()->Number();
  int finv = aDrawer->VIsoAspect()->Number();

  VrmlConverter_WFRestrictedFace::Add(anOStream,
                                      aFace,
                                      true,
                                      true,
                                      finu,
                                      finv,
                                      aDrawer);
}

//=================================================================================================

void VrmlConverter_WFRestrictedFace::AddUIso(Standard_OStream&                   anOStream,
                                             const occ::handle<BRepAdaptor_Surface>&  aFace,
                                             const occ::handle<VrmlConverter_Drawer>& aDrawer)
{

  int finu = aDrawer->UIsoAspect()->Number();
  int finv = aDrawer->VIsoAspect()->Number();

  VrmlConverter_WFRestrictedFace::Add(anOStream,
                                      aFace,
                                      true,
                                      false,
                                      finu,
                                      finv,
                                      aDrawer);
}

//=================================================================================================

void VrmlConverter_WFRestrictedFace::AddVIso(Standard_OStream&                   anOStream,
                                             const occ::handle<BRepAdaptor_Surface>&  aFace,
                                             const occ::handle<VrmlConverter_Drawer>& aDrawer)
{

  int finu = aDrawer->UIsoAspect()->Number();
  int finv = aDrawer->VIsoAspect()->Number();

  VrmlConverter_WFRestrictedFace::Add(anOStream,
                                      aFace,
                                      false,
                                      true,
                                      finu,
                                      finv,
                                      aDrawer);
}
