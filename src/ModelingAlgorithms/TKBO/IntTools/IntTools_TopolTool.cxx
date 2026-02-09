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

#include <Adaptor3d_Surface.hxx>
#include <ElSLib.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <gp_Circ.hxx>
#include <gp_Cone.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <IntTools_TopolTool.hxx>
#include <Precision.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Type.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IntTools_TopolTool, Adaptor3d_TopolTool)

static void Analyse(const NCollection_Array2<gp_Pnt>& array2,
                    int&                              theNbSamplesU,
                    int&                              theNbSamplesV);

//=================================================================================================

IntTools_TopolTool::IntTools_TopolTool()
{
  myNbSmplU = 0;
  myNbSmplV = 0;
  myDU      = 1.;
  myDV      = 1.;
}

//=================================================================================================

IntTools_TopolTool::IntTools_TopolTool(const occ::handle<Adaptor3d_Surface>& theSurface)
{
  Initialize(theSurface);
  myNbSmplU = 0;
  myNbSmplV = 0;
  myDU      = 1.;
  myDV      = 1.;
}

//=================================================================================================

void IntTools_TopolTool::Initialize()
{
  throw Standard_NotImplemented("IntTools_TopolTool::Initialize ()");
}

//=================================================================================================

void IntTools_TopolTool::Initialize(const occ::handle<Adaptor3d_Surface>& theSurface)
{
  Adaptor3d_TopolTool::Initialize(theSurface);
  // myS = theSurface;
  myNbSmplU = 0;
  myNbSmplV = 0;
  myDU      = 1.;
  myDV      = 1.;
}

//=================================================================================================

void IntTools_TopolTool::ComputeSamplePoints()
{
  double uinf, usup, vinf, vsup;
  uinf                   = myS->FirstUParameter();
  usup                   = myS->LastUParameter();
  vinf                   = myS->FirstVParameter();
  vsup                   = myS->LastVParameter();
  const int aMaxNbSample = 50;

  if (usup < uinf)
  {
    double temp = uinf;
    uinf        = usup;
    usup        = temp;
  }
  if (vsup < vinf)
  {
    double temp = vinf;
    vinf        = vsup;
    vsup        = temp;
  }
  bool isbiguinf, isbigusup, isbigvinf, isbigvsup;
  isbiguinf = Precision::IsNegativeInfinite(uinf);
  isbigusup = Precision::IsPositiveInfinite(usup);
  isbigvinf = Precision::IsNegativeInfinite(vinf);
  isbigvsup = Precision::IsPositiveInfinite(vsup);

  if (isbiguinf && isbigusup)
  {
    uinf = -1.e5;
    usup = 1.e5;
  }
  else if (isbiguinf)
  {
    uinf = usup - 2.e5;
  }
  else if (isbigusup)
  {
    usup = uinf + 2.e5;
  }

  if (isbigvinf && isbigvsup)
  {
    vinf = -1.e5;
    vsup = 1.e5;
  }
  else if (isbigvinf)
  {
    vinf = vsup - 2.e5;
  }
  else if (isbigvsup)
  {
    vsup = vinf + 2.e5;
  }
  myU0 = uinf;
  myV0 = vinf;

  int                 nbsu = 0, nbsv = 0;
  GeomAbs_SurfaceType typS = myS->GetType();

  switch (typS)
  {
    case GeomAbs_Plane: {
      nbsu = 10;
      nbsv = 10;
    }
    break;
    case GeomAbs_Cylinder: {
      double aRadius     = myS->Cylinder().Radius();
      double aMaxAngle   = M_PI * 0.5;
      double aDeflection = 1.e-02;

      if (aRadius > aDeflection)
      {
        aMaxAngle = std::acos(1. - aDeflection / aRadius) * 2.;
      }
      if (aMaxAngle > Precision::Angular())
      {
        nbsu = int((usup - uinf) / aMaxAngle);
      }
      nbsv = (int)(vsup - vinf);
      nbsv /= 10;

      if (nbsu < 2)
        nbsu = 2;
      if (nbsv < 2)
        nbsv = 2;

      //    if(nbsu < 10) nbsu = 10;
      //    if(nbsv < 10) nbsv = 10;
      if (nbsu > aMaxNbSample)
        nbsu = aMaxNbSample;
      if (nbsv > aMaxNbSample)
        nbsv = aMaxNbSample;
    }
    break;
    case GeomAbs_Cone: {
      gp_Cone aCone = myS->Cone();
      gp_Circ aCircle =
        ElSLib::ConeVIso(aCone.Position(), aCone.RefRadius(), aCone.SemiAngle(), vinf);
      double aRadius = aCircle.Radius();
      aCircle = ElSLib::ConeVIso(aCone.Position(), aCone.RefRadius(), aCone.SemiAngle(), vsup);

      if (aRadius < aCircle.Radius())
        aRadius = aCircle.Radius();
      double aMaxAngle   = M_PI * 0.5;
      double aDeflection = 1.e-02;

      if (aRadius > aDeflection)
      {
        aMaxAngle = std::acos(1. - aDeflection / aRadius) * 2.;
      }

      if (aMaxAngle > Precision::Angular())
      {
        nbsu = int((usup - uinf) / aMaxAngle);
      }
      nbsv = (int)(vsup - vinf);
      nbsv /= 10;

      //     if(nbsu < 2) nbsu = 2;
      //     if(nbsv < 2) nbsv = 2;

      if (nbsu < 10)
        nbsu = 10;
      if (nbsv < 10)
        nbsv = 10;
      if (nbsu > aMaxNbSample)
        nbsu = aMaxNbSample;
      if (nbsv > aMaxNbSample)
        nbsv = aMaxNbSample;
    }
    break;
    case GeomAbs_Sphere:
    case GeomAbs_Torus: {
      gp_Circ aCircle;
      double  aRadius1, aRadius2;

      if (typS == GeomAbs_Torus)
      {
        gp_Torus aTorus = myS->Torus();
        aCircle =
          ElSLib::TorusUIso(aTorus.Position(), aTorus.MajorRadius(), aTorus.MinorRadius(), uinf);
        aRadius2 = aCircle.Radius();
        aCircle =
          ElSLib::TorusUIso(aTorus.Position(), aTorus.MajorRadius(), aTorus.MinorRadius(), usup);
        aRadius2 = (aRadius2 < aCircle.Radius()) ? aCircle.Radius() : aRadius2;

        aCircle =
          ElSLib::TorusVIso(aTorus.Position(), aTorus.MajorRadius(), aTorus.MinorRadius(), vinf);
        aRadius1 = aCircle.Radius();
        aCircle =
          ElSLib::TorusVIso(aTorus.Position(), aTorus.MajorRadius(), aTorus.MinorRadius(), vsup);
        aRadius1 = (aRadius1 < aCircle.Radius()) ? aCircle.Radius() : aRadius1;
      }
      else
      {
        gp_Sphere aSphere = myS->Sphere();
        aRadius1          = aSphere.Radius();
        aRadius2          = aSphere.Radius();
      }
      double aMaxAngle   = M_PI * 0.5;
      double aDeflection = 1.e-02;

      if (aRadius1 > aDeflection)
      {
        aMaxAngle = std::acos(1. - aDeflection / aRadius1) * 2.;
      }

      if (aMaxAngle > Precision::Angular())
      {
        nbsu = int((usup - uinf) / aMaxAngle);
      }
      aMaxAngle = M_PI * 0.5;

      if (aRadius2 > aDeflection)
      {
        aMaxAngle = std::acos(1. - aDeflection / aRadius2) * 2.;
      }

      if (aMaxAngle > Precision::Angular())
      {
        nbsv = int((vsup - vinf) / aMaxAngle);
      }
      if (nbsu < 10)
        nbsu = 10;
      if (nbsv < 10)
        nbsv = 10;
      if (nbsu > aMaxNbSample)
        nbsu = aMaxNbSample;
      if (nbsv > aMaxNbSample)
        nbsv = aMaxNbSample;
    }
    break;
    case GeomAbs_BezierSurface: {
      nbsv = 3 + myS->NbVPoles();
      nbsu = 3 + myS->NbUPoles();

      if (nbsu > 10 || nbsv > 10)
      {
        const NCollection_Array2<gp_Pnt>& array2 = myS->Bezier()->Poles();
        Analyse(array2, nbsu, nbsv);
      }

      if (nbsu < 10)
        nbsu = 10;
      if (nbsv < 10)
        nbsv = 10;
    }
    break;
    case GeomAbs_BSplineSurface: {
      nbsv = myS->NbVKnots();
      nbsv *= myS->VDegree();
      if (nbsv < 4)
        nbsv = 4;
      nbsu = myS->NbUKnots();
      nbsu *= myS->UDegree();
      if (nbsu < 4)
        nbsu = 4;

      if (nbsu > 10 || nbsv > 10)
      {
        const NCollection_Array2<gp_Pnt>& array2 = myS->BSpline()->Poles();
        Analyse(array2, nbsu, nbsv);
      }
      if (nbsu < 10)
        nbsu = 10;
      if (nbsv < 10)
        nbsv = 10;
      // Check anisotropy
      double anULen = (usup - uinf) / myS->UResolution(1.);
      double anVLen = (vsup - vinf) / myS->VResolution(1.);
      double aRatio = anULen / anVLen;
      if (aRatio >= 10.)
      {
        nbsu *= 2;
        nbsu = std::min(nbsu, aMaxNbSample);
      }
      else if (aRatio <= 0.1)
      {
        nbsv *= 2;
        nbsv = std::min(nbsv, aMaxNbSample);
      }
    }
    break;
    case GeomAbs_SurfaceOfExtrusion: {
      nbsu = 15;
      nbsv = (int)(vsup - vinf);
      nbsv /= 10;
      if (nbsv < 15)
        nbsv = 15;
      if (nbsv > aMaxNbSample)
        nbsv = aMaxNbSample;
    }
    break;
    case GeomAbs_SurfaceOfRevolution: {
      nbsv = 15;
      nbsu = 15;
    }
    break;
    default: {
      nbsu = 10;
      nbsv = 10;
    }
    break;
  }
  myNbSmplU = nbsu;
  myNbSmplV = nbsv;

  myNbSamplesU = myNbSmplU;
  myNbSamplesV = myNbSmplV;

  myDU = (usup - uinf) / (myNbSmplU + 1);
  myDV = (vsup - vinf) / (myNbSmplV + 1);
}

//=================================================================================================

int IntTools_TopolTool::NbSamplesU()
{
  if (myNbSmplU <= 0)
  {
    ComputeSamplePoints();
  }
  return myNbSmplU;
}

//=================================================================================================

int IntTools_TopolTool::NbSamplesV()
{
  if (myNbSmplV <= 0)
  {
    ComputeSamplePoints();
  }
  return myNbSmplV;
}

//=================================================================================================

int IntTools_TopolTool::NbSamples()
{
  if (myNbSmplU <= 0)
  {
    ComputeSamplePoints();
  }
  return (myNbSmplU * myNbSmplV);
}

//=================================================================================================

void IntTools_TopolTool::SamplePoint(const int Index, gp_Pnt2d& P2d, gp_Pnt& P3d)
{
  if (myUPars.IsNull())
  {
    if (myNbSmplU <= 0)
    {
      ComputeSamplePoints();
    }
    int    iv = 1 + Index / myNbSmplU;
    int    iu = 1 + Index - (iv - 1) * myNbSmplU;
    double u  = myU0 + iu * myDU;
    double v  = myV0 + iv * myDV;
    P2d.SetCoord(u, v);
    P3d = myS->Value(u, v);
  }
  else
    Adaptor3d_TopolTool::SamplePoint(Index, P2d, P3d);
}

//=================================================================================================

void Analyse(const NCollection_Array2<gp_Pnt>& array2, int& theNbSamplesU, int& theNbSamplesV)
{
  gp_Vec    Vi, Vip1;
  int       sh, nbch, i, j;
  const int nbup = array2.UpperRow() - array2.LowerRow() + 1;
  const int nbvp = array2.UpperCol() - array2.LowerCol() + 1;

  sh   = 1;
  nbch = 0;
  if (nbvp > 2)
  {

    for (i = array2.LowerRow(); i <= array2.UpperRow(); i++)
    {
      const gp_Pnt& A = array2.Value(i, 1);
      const gp_Pnt& B = array2.Value(i, 2);
      const gp_Pnt& C = array2.Value(i, 3);
      Vi.SetCoord(C.X() - B.X() - B.X() + A.X(),
                  C.Y() - B.Y() - B.Y() + A.Y(),
                  C.Z() - B.Z() - B.Z() + A.Z());
      int locnbch = 0;

      for (j = array2.LowerCol() + 2; j < array2.UpperCol(); j++)
      { //-- essai
        const gp_Pnt& Ax = array2.Value(i, j - 1);
        const gp_Pnt& Bx = array2.Value(i, j);
        const gp_Pnt& Cx = array2.Value(i, j + 1);
        Vip1.SetCoord(Cx.X() - Bx.X() - Bx.X() + Ax.X(),
                      Cx.Y() - Bx.Y() - Bx.Y() + Ax.Y(),
                      Cx.Z() - Bx.Z() - Bx.Z() + Ax.Z());
        double pd = Vi.Dot(Vip1);
        Vi        = Vip1;
        if (pd > 1.0e-7 || pd < -1.0e-7)
        {
          if (pd > 0)
          {
            if (sh == -1)
            {
              sh = 1;
              locnbch++;
            }
          }
          else
          {
            if (sh == 1)
            {
              sh = -1;
              locnbch++;
            }
          }
        }
      }
      if (locnbch > nbch)
      {
        nbch = locnbch;
      }
    }
  }
  theNbSamplesV = nbch + 5;

  nbch = 0;
  if (nbup > 2)
  {
    for (j = array2.LowerCol(); j <= array2.UpperCol(); j++)
    {
      const gp_Pnt& A = array2.Value(array2.LowerRow(), j);
      const gp_Pnt& B = array2.Value(array2.LowerRow() + 1, j);
      const gp_Pnt& C = array2.Value(array2.LowerRow() + 2, j);
      Vi.SetCoord(C.X() - B.X() - B.X() + A.X(),
                  C.Y() - B.Y() - B.Y() + A.Y(),
                  C.Z() - B.Z() - B.Z() + A.Z());
      int locnbch = 0;

      for (i = array2.LowerRow() + 2; i < array2.UpperRow(); i++)
      { //-- essai
        const gp_Pnt& Ax = array2.Value(i - 1, j);
        const gp_Pnt& Bx = array2.Value(i, j);
        const gp_Pnt& Cx = array2.Value(i + 1, j);
        Vip1.SetCoord(Cx.X() - Bx.X() - Bx.X() + Ax.X(),
                      Cx.Y() - Bx.Y() - Bx.Y() + Ax.Y(),
                      Cx.Z() - Bx.Z() - Bx.Z() + Ax.Z());
        double pd = Vi.Dot(Vip1);
        Vi        = Vip1;
        if (pd > 1.0e-7 || pd < -1.0e-7)
        {
          if (pd > 0)
          {
            if (sh == -1)
            {
              sh = 1;
              locnbch++;
            }
          }
          else
          {
            if (sh == 1)
            {
              sh = -1;
              locnbch++;
            }
          }
        }
      }
      if (locnbch > nbch)
        nbch = locnbch;
    }
  }
  theNbSamplesU = nbch + 5;
}

// Modified IFV
//=================================================================================================

void IntTools_TopolTool::SamplePnts(const double theDefl, const int theNUmin, const int theNVmin)
{
  Adaptor3d_TopolTool::SamplePnts(theDefl, theNUmin, theNVmin);

  myNbSmplU = Adaptor3d_TopolTool::NbSamplesU();
  myNbSmplV = Adaptor3d_TopolTool::NbSamplesV();

  myU0 = myUPars->Value(1);
  myV0 = myVPars->Value(1);

  myDU = (myUPars->Value(myNbSmplU) - myU0) / (myNbSmplU - 1);
  myDV = (myVPars->Value(myNbSmplV) - myV0) / (myNbSmplV - 1);
}
