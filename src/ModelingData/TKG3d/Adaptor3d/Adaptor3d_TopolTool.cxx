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

#include <Adaptor3d_TopolTool.hxx>

#include <Adaptor2d_Line2d.hxx>
#include <Adaptor3d_Surface.hxx>
#include <Adaptor3d_HVertex.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Cone.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Adaptor3d_TopolTool, Standard_Transient)

#define myInfinite Precision::Infinite()

Adaptor3d_TopolTool::Adaptor3d_TopolTool()
    : myNbSamplesU(-1),
      myNbSamplesV(-1),
      nbRestr(0),
      idRestr(0),
      Uinf(0.0),
      Usup(0.0),
      Vinf(0.0),
      Vsup(0.0),
      nbVtx(0),
      idVtx(0)
{
}

Adaptor3d_TopolTool::Adaptor3d_TopolTool(const occ::handle<Adaptor3d_Surface>& S)
{
  Initialize(S);
}

void Adaptor3d_TopolTool::Initialize()
{
  throw Standard_NotImplemented("Adaptor3d_TopolTool::Initialize ()");
}

void Adaptor3d_TopolTool::Initialize(const occ::handle<Adaptor3d_Surface>& S)
{
  double pinf, psup, deltap;
  // Adaptor2d_Line2d  * Line2dPtr ;

  myNbSamplesU = -1;
  Uinf         = S->FirstUParameter(); // where UIntervalFirst ??
  Vinf         = S->FirstVParameter();
  Usup         = S->LastUParameter();
  Vsup         = S->LastVParameter();
  nbRestr      = 0;
  idRestr      = 0;

  bool Uinfinfinite = Precision::IsNegativeInfinite(Uinf);
  bool Usupinfinite = Precision::IsPositiveInfinite(Usup);
  bool Vinfinfinite = Precision::IsNegativeInfinite(Vinf);
  bool Vsupinfinite = Precision::IsPositiveInfinite(Vsup);

  if (!Vinfinfinite)
  {
    deltap = std::min(Usup - Uinf, 2. * myInfinite);
    if (Uinf >= -myInfinite)
    {
      pinf = Uinf;
      psup = pinf + deltap;
    }
    else if (Usup <= myInfinite)
    {
      psup = Usup;
      pinf = psup - deltap;
    }
    else
    {
      pinf = -myInfinite;
      psup = myInfinite;
    }

    // Line2dPtr = new Adaptor2d_Line2d(gp_Pnt2d(0.,Vinf),gp_Dir2d(gp_Dir2d::D::X),pinf,psup);
    // myRestr[nbRestr] = new Adaptor2d_Line2d(*Line2dPtr);
    myRestr[nbRestr] = new Adaptor2d_Line2d(
      Adaptor2d_Line2d(gp_Pnt2d(0., Vinf), gp_Dir2d(gp_Dir2d::D::X), pinf, psup));
    nbRestr++;
  }

  if (!Usupinfinite)
  {
    deltap = std::min(Vsup - Vinf, 2. * myInfinite);
    if (Vinf >= -myInfinite)
    {
      pinf = Vinf;
      psup = pinf + deltap;
    }
    else if (Vsup <= myInfinite)
    {
      psup = Vsup;
      pinf = psup - deltap;
    }
    else
    {
      pinf = -myInfinite;
      psup = myInfinite;
    }

    // Line2dPtr = new Adaptor2d_Line2d(gp_Pnt2d(Usup,0.),gp_Dir2d(gp_Dir2d::D::Y),pinf,psup);
    // myRestr[nbRestr] = new Adaptor2d_Line2d(*Line2dPtr);
    myRestr[nbRestr] = new Adaptor2d_Line2d(
      Adaptor2d_Line2d(gp_Pnt2d(Usup, 0.), gp_Dir2d(gp_Dir2d::D::Y), pinf, psup));
    nbRestr++;
  }

  if (!Vsupinfinite)
  {
    deltap = std::min(Usup - Uinf, 2. * myInfinite);
    if (-Usup >= -myInfinite)
    {
      pinf = -Usup;
      psup = pinf + deltap;
    }
    else if (-Uinf <= myInfinite)
    {
      psup = -Uinf;
      pinf = psup - deltap;
    }
    else
    {
      pinf = -myInfinite;
      psup = myInfinite;
    }

    // Line2dPtr = new Adaptor2d_Line2d(gp_Pnt2d(0.,Vsup),gp_Dir2d(gp_Dir2d::D::NX),pinf,psup);
    // myRestr[nbRestr] = new Adaptor2d_Line2d(*Line2dPtr);
    myRestr[nbRestr] = new Adaptor2d_Line2d(
      Adaptor2d_Line2d(gp_Pnt2d(0., Vsup), gp_Dir2d(gp_Dir2d::D::NX), pinf, psup));
    nbRestr++;
  }

  if (!Uinfinfinite)
  {
    deltap = std::min(Vsup - Vinf, 2. * myInfinite);
    if (-Vsup >= -myInfinite)
    {
      pinf = -Vsup;
      psup = pinf + deltap;
    }
    else if (-Vinf <= myInfinite)
    {
      psup = -Vinf;
      pinf = psup - deltap;
    }
    else
    {
      pinf = -myInfinite;
      psup = myInfinite;
    }

    // Line2dPtr = new Adaptor2d_Line2d(gp_Pnt2d(Uinf,0.),gp_Dir2d(gp_Dir2d::D::NY),pinf,psup);
    // myRestr[nbRestr] = new Adaptor2d_Line2d(*Line2dPtr);
    myRestr[nbRestr] = new Adaptor2d_Line2d(
      Adaptor2d_Line2d(gp_Pnt2d(Uinf, 0.), gp_Dir2d(gp_Dir2d::D::NY), pinf, psup));
    nbRestr++;
  }

  myS = S;

  if (nbRestr == 2 && S->GetType() == GeomAbs_Cone)
  {
    double U = 0., V = 0.;
    GetConeApexParam(S->Cone(), U, V);

    deltap = std::min(Usup - Uinf, 2. * myInfinite);
    if (Uinf >= -myInfinite)
    {
      pinf = Uinf;
      psup = pinf + deltap;
    }
    else if (Usup <= myInfinite)
    {
      psup = Usup;
      pinf = psup - deltap;
    }
    else
    {
      pinf = -myInfinite;
      psup = myInfinite;
    }

    // Line2dPtr = new Adaptor2d_Line2d(gp_Pnt2d(U,V),gp_Dir2d(gp_Dir2d::D::X),pinf,psup);
    // myRestr[nbRestr] = new Adaptor2d_Line2d(*Line2dPtr);
    myRestr[nbRestr] =
      new Adaptor2d_Line2d(Adaptor2d_Line2d(gp_Pnt2d(U, V), gp_Dir2d(gp_Dir2d::D::X), pinf, psup));
    nbRestr++;
  }
}

void Adaptor3d_TopolTool::Init()
{
  idRestr = 0;
}

bool Adaptor3d_TopolTool::More()
{
  return (idRestr < nbRestr);
}

occ::handle<Adaptor2d_Curve2d> Adaptor3d_TopolTool::Value()
{
  if (idRestr >= nbRestr)
  {
    throw Standard_DomainError();
  }
  return myRestr[idRestr];
}

void Adaptor3d_TopolTool::Next()
{
  idRestr++;
}

void Adaptor3d_TopolTool::Initialize(const occ::handle<Adaptor2d_Curve2d>& C)
{
  nbVtx = 0;
  idVtx = 0;
  double theUinf, theUsup;
  theUinf = C->FirstParameter();
  theUsup = C->LastParameter();
  //  if (!Precision::IsNegativeInfinite(theUinf)) {
  if (theUinf > -myInfinite)
  {
    myVtx[nbVtx] = new Adaptor3d_HVertex(C->Value(theUinf), TopAbs_FORWARD, 1.e-8);
    nbVtx++;
  }
  //  if (!Precision::IsPositiveInfinite(theUsup)) {
  if (theUsup < myInfinite)
  {
    myVtx[nbVtx] = new Adaptor3d_HVertex(C->Value(theUsup), TopAbs_REVERSED, 1.e-8);
    nbVtx++;
  }
}

void Adaptor3d_TopolTool::InitVertexIterator()
{
  idVtx = 0;
}

bool Adaptor3d_TopolTool::MoreVertex()
{
  return (idVtx < nbVtx);
}

occ::handle<Adaptor3d_HVertex> Adaptor3d_TopolTool::Vertex()
{
  if (idVtx >= nbVtx)
  {
    throw Standard_DomainError();
  }
  return myVtx[idVtx];
}

void Adaptor3d_TopolTool::NextVertex()
{
  idVtx++;
}

TopAbs_State Adaptor3d_TopolTool::Classify(const gp_Pnt2d& P, const double Tol, const bool)
//					 const bool RecadreOnPeriodic)
{

  double U = P.X();
  double V = P.Y();

  if (nbRestr == 4)
  {
    if ((U < Uinf - Tol) || (U > Usup + Tol) || (V < Vinf - Tol) || (V > Vsup + Tol))
    {
      return TopAbs_OUT;
    }
    if ((std::abs(U - Uinf) <= Tol) || (std::abs(U - Usup) <= Tol) || (std::abs(V - Vinf) <= Tol)
        || (std::abs(V - Vsup) <= Tol))
    {
      return TopAbs_ON;
    }
    return TopAbs_IN;
  }
  else if (nbRestr == 0)
  {
    return TopAbs_IN;
  }
  else
  {
    bool dansu, dansv, surumin, surumax, survmin, survmax;
    if (Precision::IsNegativeInfinite(Uinf) && Precision::IsPositiveInfinite(Usup))
    {
      dansu   = true;
      surumin = surumax = false;
    }
    else if (Precision::IsNegativeInfinite(Uinf))
    {
      surumin = false;
      if (U >= Usup + Tol)
      {
        dansu   = false;
        surumax = false;
      }
      else
      {
        dansu   = true;
        surumax = false;
        if (std::abs(U - Usup) <= Tol)
        {
          surumax = true;
        }
      }
    }
    else if (Precision::IsPositiveInfinite(Usup))
    {
      surumax = false;
      if (U < Uinf - Tol)
      {
        dansu   = false;
        surumin = false;
      }
      else
      {
        dansu   = true;
        surumin = false;
        if (std::abs(U - Uinf) <= Tol)
        {
          surumin = true;
        }
      }
    }
    else
    {
      if ((U < Uinf - Tol) || (U > Usup + Tol))
      {
        surumin = surumax = dansu = false;
      }
      else
      {
        dansu   = true;
        surumin = surumax = false;
        if (std::abs(U - Uinf) <= Tol)
        {
          surumin = true;
        }
        else if (std::abs(U - Usup) <= Tol)
        {
          surumax = true;
        }
      }
    }

    if (Precision::IsNegativeInfinite(Vinf) && Precision::IsPositiveInfinite(Vsup))
    {
      dansv   = true;
      survmin = survmax = false;
    }
    else if (Precision::IsNegativeInfinite(Vinf))
    {
      survmin = false;
      if (V > Vsup + Tol)
      {
        dansv   = false;
        survmax = false;
      }
      else
      {
        dansv   = true;
        survmax = false;
        if (std::abs(V - Vsup) <= Tol)
        {
          survmax = true;
        }
      }
    }
    else if (Precision::IsPositiveInfinite(Vsup))
    {
      survmax = false;
      if (V < Vinf - Tol)
      {
        dansv   = false;
        survmin = false;
      }
      else
      {
        dansv   = true;
        survmin = false;
        if (std::abs(V - Vinf) <= Tol)
        {
          survmin = true;
        }
      }
    }
    else
    {
      if ((V < Vinf - Tol) || (V > Vsup + Tol))
      {
        survmin = survmax = dansv = false;
      }
      else
      {
        dansv   = true;
        survmin = survmax = false;
        if (std::abs(V - Vinf) <= Tol)
        {
          survmin = true;
        }
        else if (std::abs(V - Vsup) <= Tol)
        {
          survmax = true;
        }
      }
    }

    if (!dansu || !dansv)
    {
      return TopAbs_OUT;
    }
    if (surumin || survmin || surumax || survmax)
    {
      return TopAbs_ON;
    }
    return TopAbs_IN;
  }
}

bool Adaptor3d_TopolTool::IsThePointOn(const gp_Pnt2d& P, const double Tol, const bool)
//					     const bool RecadreOnPeriodic)
{

  double U = P.X();
  double V = P.Y();

  if (nbRestr == 4)
  {
    if ((U < Uinf - Tol) || (U > Usup + Tol) || (V < Vinf - Tol) || (V > Vsup + Tol))
    {
      return (false);
    }
    if ((std::abs(U - Uinf) <= Tol) || (std::abs(U - Usup) <= Tol) || (std::abs(V - Vinf) <= Tol)
        || (std::abs(V - Vsup) <= Tol))
    {
      return (true);
    }
    return (false);
  }
  else if (nbRestr == 0)
  {
    return (false);
  }
  else
  {
    bool dansu, dansv, surumin, surumax, survmin, survmax;
    if (Precision::IsNegativeInfinite(Uinf) && Precision::IsPositiveInfinite(Usup))
    {
      dansu   = true;
      surumin = surumax = false;
    }
    else if (Precision::IsNegativeInfinite(Uinf))
    {
      surumin = false;
      if (U >= Usup + Tol)
      {
        dansu   = false;
        surumax = false;
      }
      else
      {
        dansu   = true;
        surumax = false;
        if (std::abs(U - Usup) <= Tol)
        {
          surumax = true;
        }
      }
    }
    else if (Precision::IsPositiveInfinite(Usup))
    {
      surumax = false;
      if (U < Uinf - Tol)
      {
        dansu   = false;
        surumin = false;
      }
      else
      {
        dansu   = true;
        surumin = false;
        if (std::abs(U - Uinf) <= Tol)
        {
          surumin = true;
        }
      }
    }
    else
    {
      if ((U < Uinf - Tol) || (U > Usup + Tol))
      {
        surumin = surumax = dansu = false;
      }
      else
      {
        dansu   = true;
        surumin = surumax = false;
        if (std::abs(U - Uinf) <= Tol)
        {
          surumin = true;
        }
        else if (std::abs(U - Usup) <= Tol)
        {
          surumax = true;
        }
      }
    }

    if (Precision::IsNegativeInfinite(Vinf) && Precision::IsPositiveInfinite(Vsup))
    {
      dansv   = true;
      survmin = survmax = false;
    }
    else if (Precision::IsNegativeInfinite(Vinf))
    {
      survmin = false;
      if (V > Vsup + Tol)
      {
        dansv   = false;
        survmax = false;
      }
      else
      {
        dansv   = true;
        survmax = false;
        if (std::abs(V - Vsup) <= Tol)
        {
          survmax = true;
        }
      }
    }
    else if (Precision::IsPositiveInfinite(Vsup))
    {
      survmax = false;
      if (V < Vinf - Tol)
      {
        dansv   = false;
        survmin = false;
      }
      else
      {
        dansv   = true;
        survmin = false;
        if (std::abs(V - Vinf) <= Tol)
        {
          survmin = true;
        }
      }
    }
    else
    {
      if ((V < Vinf - Tol) || (V > Vsup + Tol))
      {
        survmin = survmax = dansv = false;
      }
      else
      {
        dansv   = true;
        survmin = survmax = false;
        if (std::abs(V - Vinf) <= Tol)
        {
          survmin = true;
        }
        else if (std::abs(V - Vsup) <= Tol)
        {
          survmax = true;
        }
      }
    }

    if (!dansu || !dansv)
    {
      return (false);
    }
    if (surumin || survmin || surumax || survmax)
    {
      return (true);
    }
    return false;
  }
}

TopAbs_Orientation Adaptor3d_TopolTool::Orientation(const occ::handle<Adaptor2d_Curve2d>&)
{
  return TopAbs_FORWARD;
}

TopAbs_Orientation Adaptor3d_TopolTool::Orientation(const occ::handle<Adaptor3d_HVertex>& V)
{
  return V->Orientation();
}

bool Adaptor3d_TopolTool::Identical(const occ::handle<Adaptor3d_HVertex>& V1,
                                    const occ::handle<Adaptor3d_HVertex>& V2)
{
  return V1->IsSame(V2);
}

//-- ============================================================
//-- m e t h o d e s   u t i l i s e e s   p o u r   l e s
//--  s a m p l e s
//-- ============================================================
#include <NCollection_Array2.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <NCollection_Array1.hxx>
// #include <gce_MakeLin.hxx>
#include <gp_Lin.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>

#define myMinPnts                                                                                  \
  4 // Absolute possible minimum of sample points
    // Restriction of IntPolyh

static void Analyse(const NCollection_Array2<gp_Pnt>& array2,
                    const int                         nbup,
                    const int                         nbvp,
                    int&                              myNbSamplesU,
                    int&                              myNbSamplesV)
{
  gp_Vec Vi, Vip1;
  int    sh, nbch, i, j;

  sh   = 1;
  nbch = 0;
  if (nbvp > 2)
  {
    for (i = 2; i < nbup; i++)
    {
      const gp_Pnt& A = array2.Value(i, 1);
      const gp_Pnt& B = array2.Value(i, 2);
      const gp_Pnt& C = array2.Value(i, 3);
      Vi.SetCoord(C.X() - B.X() - B.X() + A.X(),
                  C.Y() - B.Y() - B.Y() + A.Y(),
                  C.Z() - B.Z() - B.Z() + A.Z());
      int locnbch = 0;
      for (j = 3; j < nbvp; j++)
      { //-- try
        const gp_Pnt& A1 = array2.Value(i, j - 1);
        const gp_Pnt& B1 = array2.Value(i, j);
        const gp_Pnt& C1 = array2.Value(i, j + 1);
        Vip1.SetCoord(C1.X() - B1.X() - B1.X() + A1.X(),
                      C1.Y() - B1.Y() - B1.Y() + A1.Y(),
                      C1.Z() - B1.Z() - B1.Z() + A1.Z());
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
  myNbSamplesV = nbch + 5;

  nbch = 0;
  if (nbup > 2)
  {
    for (j = 2; j < nbvp; j++)
    {
      const gp_Pnt& A = array2.Value(1, j);
      const gp_Pnt& B = array2.Value(2, j);
      const gp_Pnt& C = array2.Value(3, j);
      Vi.SetCoord(C.X() - B.X() - B.X() + A.X(),
                  C.Y() - B.Y() - B.Y() + A.Y(),
                  C.Z() - B.Z() - B.Z() + A.Z());
      int locnbch = 0;
      for (i = 3; i < nbup; i++)
      { //-- try
        const gp_Pnt& A1 = array2.Value(i - 1, j);
        const gp_Pnt& B1 = array2.Value(i, j);
        const gp_Pnt& C1 = array2.Value(i + 1, j);
        Vip1.SetCoord(C1.X() - B1.X() - B1.X() + A1.X(),
                      C1.Y() - B1.Y() - B1.Y() + A1.Y(),
                      C1.Z() - B1.Z() - B1.Z() + A1.Z());
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
  myNbSamplesU = nbch + 5;
}

void Adaptor3d_TopolTool::ComputeSamplePoints()
{
  const int aMaxNbSample = 50;

  double uinf, usup, vinf, vsup;
  uinf = myS->FirstUParameter();
  usup = myS->LastUParameter();
  vinf = myS->FirstVParameter();
  vsup = myS->LastVParameter();
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
  if (uinf == RealFirst() && usup == RealLast())
  {
    uinf = -1.e5;
    usup = 1.e5;
  }
  else if (uinf == RealFirst())
  {
    uinf = usup - 2.e5;
  }
  else if (usup == RealLast())
  {
    usup = uinf + 2.e5;
  }

  if (vinf == RealFirst() && vsup == RealLast())
  {
    vinf = -1.e5;
    vsup = 1.e5;
  }
  else if (vinf == RealFirst())
  {
    vinf = vsup - 2.e5;
  }
  else if (vsup == RealLast())
  {
    vsup = vinf + 2.e5;
  }

  int                 nbsu, nbsv;
  GeomAbs_SurfaceType typS = myS->GetType();
  switch (typS)
  {
    case GeomAbs_Plane: {
      nbsv = 2;
      nbsu = 2;
    }
    break;
    case GeomAbs_BezierSurface: {
      nbsv = 3 + myS->NbVPoles();
      nbsu = 3 + myS->NbUPoles();
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
    }
    break;
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere:
    case GeomAbs_Torus:
    case GeomAbs_SurfaceOfRevolution:
    case GeomAbs_SurfaceOfExtrusion: {
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

  //-- If the number of points is too great... analyze
  //--
  //--

  if (nbsu < 6)
    nbsu = 6;
  if (nbsv < 6)
    nbsv = 6;

  if (typS == GeomAbs_BSplineSurface)
  {
    if (nbsu > 8 || nbsv > 8)
    {
      const occ::handle<Geom_BSplineSurface>& Bspl = myS->BSpline();
      const NCollection_Array2<gp_Pnt>& array2 = Bspl->Poles();
      int                              nbup   = array2.NbRows();
      int                              nbvp   = array2.NbColumns();
      Analyse(array2, nbup, nbvp, nbsu, nbsv);
    }
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
  else if (typS == GeomAbs_BezierSurface)
  {
    if (nbsu > 8 || nbsv > 8)
    {
      const occ::handle<Geom_BezierSurface>&  Bez    = myS->Bezier();
      const NCollection_Array2<gp_Pnt>&      array2 = Bez->Poles();
      int                                    nbup   = array2.NbRows();
      int                                    nbvp   = array2.NbColumns();
      Analyse(array2, nbup, nbvp, nbsu, nbsv);
    }
  }

  myNbSamplesU = nbsu;
  myNbSamplesV = nbsv;
}

int Adaptor3d_TopolTool::NbSamplesU()
{
  if (myNbSamplesU < 0)
  {
    ComputeSamplePoints();
  }
  return (myNbSamplesU);
}

int Adaptor3d_TopolTool::NbSamplesV()
{
  if (myNbSamplesU < 0)
  {
    ComputeSamplePoints();
  }
  return (myNbSamplesV);
}

int Adaptor3d_TopolTool::NbSamples()
{
  if (myNbSamplesU < 0)
  {
    ComputeSamplePoints();
  }
  return (myNbSamplesU * myNbSamplesV);
}

void Adaptor3d_TopolTool::UParameters(NCollection_Array1<double>& theArray) const
{
  theArray = myUPars->Array1();
}

void Adaptor3d_TopolTool::VParameters(NCollection_Array1<double>& theArray) const
{
  theArray = myVPars->Array1();
}

void Adaptor3d_TopolTool::SamplePoint(const int i, gp_Pnt2d& P2d, gp_Pnt& P3d)
{
  int    iu, iv;
  double u, v;
  if (myUPars.IsNull())
  {
    double myDU = (Usup - Uinf) / (myNbSamplesU + 1);
    double myDV = (Vsup - Vinf) / (myNbSamplesV + 1);
    iv          = 1 + i / myNbSamplesU;
    iu          = 1 + i - (iv - 1) * myNbSamplesU;
    u           = Uinf + iu * myDU;
    v           = Vinf + iv * myDV;
  }
  else
  {
    iv = (i - 1) / myNbSamplesU + 1;
    iu = (i - 1) % myNbSamplesU + 1;
    u  = myUPars->Value(iu);
    v  = myVPars->Value(iv);
  }

  P2d.SetCoord(u, v);
  P3d = myS->Value(u, v);
}

bool Adaptor3d_TopolTool::DomainIsInfinite()
{
  if (Precision::IsNegativeInfinite(Uinf))
    return (true);
  if (Precision::IsPositiveInfinite(Usup))
    return (true);
  if (Precision::IsNegativeInfinite(Vinf))
    return (true);
  if (Precision::IsPositiveInfinite(Vsup))
    return (true);
  return (false);
}

//=================================================================================================

void* Adaptor3d_TopolTool::Edge() const
{
  return nullptr;
}

//=================================================================================================

bool Adaptor3d_TopolTool::Has3d() const
{
  return false;
}

//=================================================================================================

double Adaptor3d_TopolTool::Tol3d(const occ::handle<Adaptor2d_Curve2d>&) const
{
  throw Standard_DomainError("Adaptor3d_TopolTool: has no 3d representation");
}

//=================================================================================================

double Adaptor3d_TopolTool::Tol3d(const occ::handle<Adaptor3d_HVertex>&) const
{
  throw Standard_DomainError("Adaptor3d_TopolTool: has no 3d representation");
}

//=================================================================================================

gp_Pnt Adaptor3d_TopolTool::Pnt(const occ::handle<Adaptor3d_HVertex>&) const
{
  throw Standard_DomainError("Adaptor3d_TopolTool: has no 3d representation");
}

//=================================================================================================

void Adaptor3d_TopolTool::SamplePnts(const double theDefl, const int theNUmin, const int theNVmin)
{
  double uinf, usup, vinf, vsup;
  uinf = myS->FirstUParameter();
  usup = myS->LastUParameter();
  vinf = myS->FirstVParameter();
  vsup = myS->LastVParameter();
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
  if (uinf == RealFirst() && usup == RealLast())
  {
    uinf = -1.e5;
    usup = 1.e5;
  }
  else if (uinf == RealFirst())
  {
    uinf = usup - 2.e5;
  }
  else if (usup == RealLast())
  {
    usup = uinf + 2.e5;
  }

  if (vinf == RealFirst() && vsup == RealLast())
  {
    vinf = -1.e5;
    vsup = 1.e5;
  }
  else if (vinf == RealFirst())
  {
    vinf = vsup - 2.e5;
  }
  else if (vsup == RealLast())
  {
    vsup = vinf + 2.e5;
  }

  //   int nbsu,nbsv;
  GeomAbs_SurfaceType typS = myS->GetType();
  //   switch(typS) {
  //   case GeomAbs_Plane:          { nbsv=2; nbsu=2; } break;
  //   case GeomAbs_BezierSurface:  {
  //     nbsv=myS->NbVPoles();
  //     nbsu=myS->NbUPoles();
  //     nbsu = std::max(nbsu, theNUmin);
  //     nbsv = std::max(nbsv, theNVmin);
  //     if(nbsu>8 || nbsv>8) {
  //       const occ::handle<Geom_BezierSurface>& Bez = myS->Bezier();
  //       int nbup = Bez->NbUPoles();
  //       int nbvp = Bez->NbVPoles();
  //       NCollection_Array2<gp_Pnt> array2(1,nbup,1,nbvp);
  //       Bez->Poles(array2);
  //       Analyse(array2,nbup,nbvp,myNbSamplesU,myNbSamplesV);
  //     }
  //   }
  //     break;
  //   case GeomAbs_BSplineSurface: {
  if (typS == GeomAbs_BSplineSurface)
  {
    // Processing BSpline surface
    BSplSamplePnts(theDefl, theNUmin, theNVmin);
    return;
  }
  else
  {
    ComputeSamplePoints();
  }
  //   case GeomAbs_Cylinder:
  //   case GeomAbs_Cone:
  //   case GeomAbs_Sphere:
  //   case GeomAbs_Torus:
  //   case GeomAbs_SurfaceOfRevolution:
  //   case GeomAbs_SurfaceOfExtrusion:    { nbsv = std::max(15,theNVmin); nbsu=Max(15,theNUmin); }
  //   break; default:                            { nbsu = std::max(10,theNUmin);
  //   nbsv=Max(10,theNVmin); } break;
  //   }

  //   if(nbsu<6) nbsu=6;
  //   if(nbsv<6) nbsv=6;

  //   myNbSamplesU = nbsu;
  //   myNbSamplesV = nbsv;

  myUPars = new NCollection_HArray1<double>(1, myNbSamplesU);
  myVPars = new NCollection_HArray1<double>(1, myNbSamplesV);
  int    i;
  double t, dt = (usup - uinf) / (myNbSamplesU - 1);
  myUPars->SetValue(1, uinf);
  myUPars->SetValue(myNbSamplesU, usup);
  for (i = 2, t = uinf + dt; i < myNbSamplesU; ++i, t += dt)
  {
    myUPars->SetValue(i, t);
  }

  dt = (vsup - vinf) / (myNbSamplesV - 1);
  myVPars->SetValue(1, vinf);
  myVPars->SetValue(myNbSamplesV, vsup);
  for (i = 2, t = vinf + dt; i < myNbSamplesV; ++i, t += dt)
  {
    myVPars->SetValue(i, t);
  }

  return;
}

//=================================================================================================

void Adaptor3d_TopolTool::BSplSamplePnts(const double theDefl,
                                         const int    theNUmin,
                                         const int    theNVmin)
{
  const int                               aMaxPnts = 1001;
  const occ::handle<Geom_BSplineSurface>& aBS      = myS->BSpline();
  double                                  uinf, usup, vinf, vsup;
  uinf = myS->FirstUParameter();
  usup = myS->LastUParameter();
  vinf = myS->FirstVParameter();
  vsup = myS->LastVParameter();

  int    i, k, j = 1;
  double t1, t2, dt;
  int    ui1 = aBS->FirstUKnotIndex();
  int    ui2 = aBS->LastUKnotIndex();
  int    vi1 = aBS->FirstVKnotIndex();
  int    vi2 = aBS->LastVKnotIndex();

  for (i = ui1; i < ui2; ++i)
  {
    if (uinf >= aBS->UKnot(i) && uinf < aBS->UKnot(i + 1))
    {
      ui1 = i;
      break;
    }
  }

  for (i = ui2; i > ui1; --i)
  {
    if (usup <= aBS->UKnot(i) && usup > aBS->UKnot(i - 1))
    {
      ui2 = i;
      break;
    }
  }

  for (i = vi1; i < vi2; ++i)
  {
    if (vinf >= aBS->VKnot(i) && vinf < aBS->VKnot(i + 1))
    {
      vi1 = i;
      break;
    }
  }

  for (i = vi2; i > vi1; --i)
  {
    if (vsup <= aBS->VKnot(i) && vsup > aBS->VKnot(i - 1))
    {
      vi2 = i;
      break;
    }
  }

  int nbsu = ui2 - ui1 + 1;
  nbsu += (nbsu - 1) * (aBS->UDegree() - 1);
  int nbsv = vi2 - vi1 + 1;
  nbsv += (nbsv - 1) * (aBS->VDegree() - 1);
  bool bUuniform = false;
  bool bVuniform = false;

  // modified by NIZHNY-EMV Mon Jun 10 14:19:04 2013
  if (nbsu < theNUmin || nbsv < theNVmin)
  {
    int aNb;
    if (nbsu < nbsv)
    {
      aNb       = (int)(nbsv * ((double)theNUmin) / ((double)nbsu));
      aNb       = std::min(aNb, 30);
      bVuniform = (aNb > nbsv) ? true : bVuniform;
      nbsv      = bVuniform ? aNb : nbsv;
    }
    else
    {
      aNb       = (int)(nbsu * ((double)theNVmin) / ((double)nbsv));
      aNb       = std::min(aNb, 30);
      bUuniform = (aNb > nbsu) ? true : bUuniform;
      nbsu      = bUuniform ? aNb : nbsu;
    }
  }
  // modified by NIZHNY-EMV Mon Jun 10 14:19:05 2013

  if (nbsu < theNUmin)
  {
    nbsu      = theNUmin;
    bUuniform = true;
  }
  else if (nbsu > aMaxPnts)
  {
    nbsu      = aMaxPnts;
    bUuniform = true;
  }
  if (nbsv < theNVmin)
  {
    nbsv      = theNVmin;
    bVuniform = true;
  }
  else if (nbsv > aMaxPnts)
  {
    nbsv      = aMaxPnts;
    bVuniform = true;
  }

  NCollection_Array1<double> anUPars(1, nbsu);
  NCollection_Array1<bool>   anUFlg(1, nbsu);
  NCollection_Array1<double> aVPars(1, nbsv);
  NCollection_Array1<bool>   aVFlg(1, nbsv);

  // Filling of sample parameters
  if (bUuniform)
  {
    t1            = uinf;
    t2            = usup;
    dt            = (t2 - t1) / (nbsu - 1);
    anUPars(1)    = t1;
    anUFlg(1)     = false;
    anUPars(nbsu) = t2;
    anUFlg(nbsu)  = false;
    for (i = 2, t1 += dt; i < nbsu; ++i, t1 += dt)
    {
      anUPars(i) = t1;
      anUFlg(i)  = false;
    }
  }
  else
  {
    int nbi = aBS->UDegree();
    k       = 0;
    t1      = uinf;
    for (i = ui1 + 1; i <= ui2; ++i)
    {
      if (i == ui2)
        t2 = usup;
      else
        t2 = aBS->UKnot(i);
      dt = (t2 - t1) / nbi;
      j  = 1;
      do
      {
        ++k;
        anUPars(k) = t1;
        anUFlg(k)  = false;
        t1 += dt;
      } while (++j <= nbi);
      t1 = t2;
    }
    ++k;
    anUPars(k) = t1;
  }

  if (bVuniform)
  {
    t1           = vinf;
    t2           = vsup;
    dt           = (t2 - t1) / (nbsv - 1);
    aVPars(1)    = t1;
    aVFlg(1)     = false;
    aVPars(nbsv) = t2;
    aVFlg(nbsv)  = false;
    for (i = 2, t1 += dt; i < nbsv; ++i, t1 += dt)
    {
      aVPars(i) = t1;
      aVFlg(i)  = false;
    }
  }
  else
  {
    int nbi = aBS->VDegree();
    k       = 0;
    t1      = vinf;
    for (i = vi1 + 1; i <= vi2; ++i)
    {
      if (i == vi2)
        t2 = vsup;
      else
        t2 = aBS->VKnot(i);
      dt = (t2 - t1) / nbi;
      j  = 1;
      do
      {
        ++k;
        aVPars(k) = t1;
        aVFlg(k)  = false;
        t1 += dt;
      } while (++j <= nbi);
      t1 = t2;
    }
    ++k;
    aVPars(k) = t1;
  }

  // Analysis of deflection

  double aDefl2 = std::max(theDefl * theDefl, 1.e-9);
  double tol    = std::max(0.01 * aDefl2, 1.e-9);
  int    l;

  anUFlg(1)    = true;
  anUFlg(nbsu) = true;
  // myNbSamplesU = 2;
  for (i = 1; i <= nbsv; ++i)
  {
    t1         = aVPars(i);
    j          = 1;
    bool bCont = true;
    while (j < nbsu - 1 && bCont)
    {

      if (anUFlg(j + 1))
      {
        ++j;
        continue;
      }

      t2        = anUPars(j);
      gp_Pnt p1 = myS->Value(t2, t1);
      for (k = j + 2; k <= nbsu; ++k)
      {
        t2        = anUPars(k);
        gp_Pnt p2 = myS->Value(t2, t1);
        // gce_MakeLin MkLin(p1, p2);
        // const gp_Lin& lin = MkLin.Value();

        if (p1.SquareDistance(p2) <= tol)
          continue;

        gp_Lin lin(p1, gp_Dir(gp_Vec(p1, p2)));
        bool   ok = true;
        for (l = j + 1; l < k; ++l)
        {

          if (anUFlg(l))
          {
            ok = false;
            break;
          }

          gp_Pnt pp = myS->Value(anUPars(l), t1);
          double d  = lin.SquareDistance(pp);

          if (d <= aDefl2)
            continue;

          ok = false;
          break;
        }

        if (!ok)
        {
          j         = k - 1;
          anUFlg(j) = true;
          //++myNbSamplesU;
          break;
        }

        if (anUFlg(k))
        {
          j = k;
          break;
        }
      }

      if (k >= nbsu)
        bCont = false;
    }
  }

  myNbSamplesU = 0;
  for (i = 1; i <= nbsu; i++)
    if (anUFlg(i))
      myNbSamplesU++;

  if (myNbSamplesU < myMinPnts)
  {
    if (myNbSamplesU == 2)
    {
      //"uniform" distribution;
      int nn            = nbsu / myMinPnts;
      anUFlg(1 + nn)    = true;
      anUFlg(nbsu - nn) = true;
    }
    else
    { // myNbSamplesU == 3
      // insert in bigger segment
      i = 2;
      while (!anUFlg(i++))
        ;
      if (i < nbsu / 2)
        j = std::min(i + (nbsu - i) / 2, nbsu - 1);
      else
        j = std::max(i / 2, 2);
    }
    anUFlg(j)    = true;
    myNbSamplesU = myMinPnts;
  }

  aVFlg(1)    = true;
  aVFlg(nbsv) = true;
  // myNbSamplesV = 2;
  for (i = 1; i <= nbsu; ++i)
  {
    t1         = anUPars(i);
    j          = 1;
    bool bCont = true;
    while (j < nbsv - 1 && bCont)
    {

      if (aVFlg(j + 1))
      {
        ++j;
        continue;
      }

      t2        = aVPars(j);
      gp_Pnt p1 = myS->Value(t1, t2);
      for (k = j + 2; k <= nbsv; ++k)
      {
        t2        = aVPars(k);
        gp_Pnt p2 = myS->Value(t1, t2);

        if (p1.SquareDistance(p2) <= tol)
          continue;
        // gce_MakeLin MkLin(p1, p2);
        // const gp_Lin& lin = MkLin.Value();
        gp_Lin lin(p1, gp_Dir(gp_Vec(p1, p2)));
        bool   ok = true;
        for (l = j + 1; l < k; ++l)
        {

          if (aVFlg(l))
          {
            ok = false;
            break;
          }

          gp_Pnt pp = myS->Value(t1, aVPars(l));
          double d  = lin.SquareDistance(pp);

          if (d <= aDefl2)
            continue;

          ok = false;
          break;
        }

        if (!ok)
        {
          j        = k - 1;
          aVFlg(j) = true;
          //++myNbSamplesV;
          break;
        }

        if (aVFlg(k))
        {
          j = k;
          break;
        }
      }

      if (k >= nbsv)
        bCont = false;
    }
  }

  myNbSamplesV = 0;
  for (i = 1; i <= nbsv; i++)
    if (aVFlg(i))
      myNbSamplesV++;

  if (myNbSamplesV < myMinPnts)
  {
    if (myNbSamplesV == 2)
    {
      //"uniform" distribution;
      int nn           = nbsv / myMinPnts;
      aVFlg(1 + nn)    = true;
      aVFlg(nbsv - nn) = true;
      myNbSamplesV     = myMinPnts;
    }
    else
    { // myNbSamplesU == 3
      // insert in bigger segment
      i = 2;
      while (!aVFlg(i++))
        ;
      if (i < nbsv / 2)
        j = std::min(i + (nbsv - i) / 2, nbsv - 1);
      else
        j = std::max(i / 2, 2);
    }
    myNbSamplesV = myMinPnts;
    aVFlg(j)     = true;
  }
  //
  // modified by NIZNHY-PKV Fri Dec 16 10:05:01 2011f
  //
  bool bFlag;
  //
  // U
  bFlag = (myNbSamplesU < theNUmin);
  if (bFlag)
  {
    myNbSamplesU = nbsu;
  }
  //
  myUPars = new NCollection_HArray1<double>(1, myNbSamplesU);
  //
  for (j = 0, i = 1; i <= nbsu; ++i)
  {
    if (bFlag)
    {
      myUPars->SetValue(i, anUPars(i));
    }
    else
    {
      if (anUFlg(i))
      {
        ++j;
        myUPars->SetValue(j, anUPars(i));
      }
    }
  }
  //
  // V
  bFlag = (myNbSamplesV < theNVmin);
  if (bFlag)
  {
    myNbSamplesV = nbsv;
  }
  //
  myVPars = new NCollection_HArray1<double>(1, myNbSamplesV);
  //
  for (j = 0, i = 1; i <= nbsv; ++i)
  {
    if (bFlag)
    {
      myVPars->SetValue(i, aVPars(i));
    }
    else
    {
      if (aVFlg(i))
      {
        ++j;
        myVPars->SetValue(j, aVPars(i));
      }
    }
  }
  //
  /*
  myUPars = new NCollection_HArray1<double>(1, myNbSamplesU);
  myVPars = new NCollection_HArray1<double>(1, myNbSamplesV);

  j = 0;
  for(i = 1; i <= nbsu; ++i) {
    if(anUFlg(i)) {
      ++j;
      myUPars->SetValue(j,anUPars(i));
    }
  }

  j = 0;
  for(i = 1; i <= nbsv; ++i) {
    if(aVFlg(i)) {
      ++j;
      myVPars->SetValue(j,aVPars(i));
    }
  }
  */
  // modified by NIZNHY-PKV Mon Dec 26 12:25:35 2011t
}

//=================================================================================================

bool Adaptor3d_TopolTool::IsUniformSampling() const
{
  GeomAbs_SurfaceType typS = myS->GetType();

  return typS != GeomAbs_BSplineSurface;
}

//=======================================================================
// function : GetConeApexParam
// purpose  : Computes the cone's apex parameters
//=======================================================================
void Adaptor3d_TopolTool::GetConeApexParam(const gp_Cone& theC, double& theU, double& theV)
{
  const gp_Ax3& Pos    = theC.Position();
  double        Radius = theC.RefRadius();
  double        SAngle = theC.SemiAngle();
  const gp_Pnt& P      = theC.Apex();

  gp_Trsf T;
  T.SetTransformation(Pos);
  gp_Pnt Ploc = P.Transformed(T);

  if (Ploc.X() == 0.0 && Ploc.Y() == 0.0)
  {
    theU = 0.0;
  }
  else if (-Radius > Ploc.Z() * std::tan(SAngle))
  {
    // the point is at the `wrong` side of the apex
    theU = atan2(-Ploc.Y(), -Ploc.X());
  }
  else
  {
    theU = atan2(Ploc.Y(), Ploc.X());
  }

  if (theU < -1.e-16)
    theU += (M_PI + M_PI);
  else if (theU < 0)
    theU = 0;

  theV =
    sin(SAngle) * (Ploc.X() * cos(theU) + Ploc.Y() * sin(theU) - Radius) + cos(SAngle) * Ploc.Z();
}
