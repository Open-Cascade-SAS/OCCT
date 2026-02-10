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

#include <Adaptor2d_Curve2d.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <BRepTopAdaptor_HVertex.hxx>
#include <BRepTopAdaptor_TopolTool.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Type.hxx>
#include <NCollection_Array2.hxx>
#include <TopoDS.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepTopAdaptor_TopolTool, Adaptor3d_TopolTool)

static void Analyse(const NCollection_Array2<gp_Pnt>& array2,
                    const int                         nbup,
                    const int                         nbvp,
                    int&                              myNbSamplesU,
                    int&                              myNbSamplesV);

//=================================================================================================

BRepTopAdaptor_TopolTool::BRepTopAdaptor_TopolTool()
    : myFClass2d(nullptr),
      myU0(0.0),
      myV0(0.0),
      myDU(0.0),
      myDV(0.0)
{
  myNbSamplesU = -1;
}

//=================================================================================================

BRepTopAdaptor_TopolTool::BRepTopAdaptor_TopolTool(const occ::handle<Adaptor3d_Surface>& S)
    : myFClass2d(nullptr)
{
  Initialize(S);
  // myS = S;
}

//=================================================================================================

void BRepTopAdaptor_TopolTool::Initialize()
{
  throw Standard_NotImplemented("BRepTopAdaptor_TopolTool::Initialize()");
}

//=================================================================================================

void BRepTopAdaptor_TopolTool::Initialize(const occ::handle<Adaptor3d_Surface>& S)
{
  occ::handle<BRepAdaptor_Surface> brhs = occ::down_cast<BRepAdaptor_Surface>(S);
  if (brhs.IsNull())
  {
    throw Standard_ConstructionError();
  }
  TopoDS_Shape s_wnt = brhs->Face();
  s_wnt.Orientation(TopAbs_FORWARD);
  myFace = TopoDS::Face(s_wnt);
  if (myFClass2d != nullptr)
  {
    delete (BRepTopAdaptor_FClass2d*)myFClass2d;
  }
  myFClass2d   = nullptr;
  myNbSamplesU = -1;
  myS          = S;
  myCurves.Clear();
  TopExp_Explorer ex(myFace, TopAbs_EDGE);
  for (; ex.More(); ex.Next())
  {
    occ::handle<BRepAdaptor_Curve2d> aCurve =
      new BRepAdaptor_Curve2d(BRepAdaptor_Curve2d(TopoDS::Edge(ex.Current()), myFace));
    myCurves.Append(aCurve);
  }
  myCIterator = NCollection_List<occ::handle<Standard_Transient>>::Iterator();
}

//=================================================================================================

void BRepTopAdaptor_TopolTool::Initialize(const occ::handle<Adaptor2d_Curve2d>& C)
{
  myCurve = occ::down_cast<BRepAdaptor_Curve2d>(C);
  if (myCurve.IsNull())
  {
    throw Standard_ConstructionError();
  }
}

//=================================================================================================

void BRepTopAdaptor_TopolTool::Init()
{
  myCIterator.Initialize(myCurves);
}

//=================================================================================================

bool BRepTopAdaptor_TopolTool::More()
{
  return myCIterator.More();
}

//=================================================================================================

void BRepTopAdaptor_TopolTool::Next()
{
  myCIterator.Next();
}

//=================================================================================================

occ::handle<Adaptor2d_Curve2d> BRepTopAdaptor_TopolTool::Value()
{
  return occ::down_cast<Adaptor2d_Curve2d>(myCIterator.Value());
}

// modified by NIZNHY-PKV Tue Mar 27 14:23:40 2001 f
//=================================================================================================

void* BRepTopAdaptor_TopolTool::Edge() const
{
  occ::handle<BRepAdaptor_Curve2d> aHCurve =
    occ::down_cast<BRepAdaptor_Curve2d>(myCIterator.Value());
  return (void*)(&aHCurve->Edge());
}

// modified by NIZNHY-PKV Tue Mar 27 14:23:43 2001 t
//=================================================================================================

void BRepTopAdaptor_TopolTool::InitVertexIterator()
{
  myVIterator.Init(myCurve->Edge(), TopAbs_VERTEX);
}

//=================================================================================================

bool BRepTopAdaptor_TopolTool::MoreVertex()
{
  return myVIterator.More();
}

void BRepTopAdaptor_TopolTool::NextVertex()
{
  myVIterator.Next();
}

//=================================================================================================

occ::handle<Adaptor3d_HVertex> BRepTopAdaptor_TopolTool::Vertex()
{
  return new BRepTopAdaptor_HVertex(TopoDS::Vertex(myVIterator.Current()), myCurve);
}

//=================================================================================================

TopAbs_State BRepTopAdaptor_TopolTool::Classify(const gp_Pnt2d& P,
                                                const double    Tol,
                                                const bool      RecadreOnPeriodic)
{
  if (myFace.IsNull())
    return TopAbs_UNKNOWN;
  if (myFClass2d == nullptr)
  {
    myFClass2d = (void*)new BRepTopAdaptor_FClass2d(myFace, Tol);
  }
  return (((BRepTopAdaptor_FClass2d*)myFClass2d)->Perform(P, RecadreOnPeriodic));
}

//=================================================================================================

bool BRepTopAdaptor_TopolTool::IsThePointOn(const gp_Pnt2d& P,
                                            const double    Tol,
                                            const bool      RecadreOnPeriodic)
{
  if (myFClass2d == nullptr)
  {
    myFClass2d = (void*)new BRepTopAdaptor_FClass2d(myFace, Tol);
  }
  return (TopAbs_ON
          == ((BRepTopAdaptor_FClass2d*)myFClass2d)->TestOnRestriction(P, Tol, RecadreOnPeriodic));
}

//=================================================================================================

void BRepTopAdaptor_TopolTool::Destroy()
{
  if (myFClass2d != nullptr)
  {
    delete (BRepTopAdaptor_FClass2d*)myFClass2d;
    myFClass2d = nullptr;
  }
}

//=================================================================================================

TopAbs_Orientation BRepTopAdaptor_TopolTool::Orientation(const occ::handle<Adaptor2d_Curve2d>& C)
{
  occ::handle<BRepAdaptor_Curve2d> brhc = occ::down_cast<BRepAdaptor_Curve2d>(C);
  return brhc->Edge().Orientation();
}

//=================================================================================================

TopAbs_Orientation BRepTopAdaptor_TopolTool::Orientation(const occ::handle<Adaptor3d_HVertex>& C)
{
  return Adaptor3d_TopolTool::Orientation(C);
}

//-- ============================================================
//-- methods  used for samples
//-- ============================================================

//=================================================================================================

void Analyse(const NCollection_Array2<gp_Pnt>& array2,
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
      { //-- test
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
      { //-- test
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
  myNbSamplesU = nbch + 5;
}

//=================================================================================================

void BRepTopAdaptor_TopolTool::ComputeSamplePoints()
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
    case GeomAbs_Torus: {
      //-- Set 15 for 2pi
      //-- Not enough ->25 for 2pi
      nbsu = (int)(8 * (usup - uinf));
      nbsv = (int)(7 * (vsup - vinf));
      if (nbsu < 5)
        nbsu = 5;
      if (nbsv < 5)
        nbsv = 5;
      if (nbsu > 30)
        nbsu = 30; // modif HRT buc60462
      if (nbsv > 15)
        nbsv = 15;
      //-- printf("\n nbsu=%d nbsv=%d\n",nbsu,nbsv);
    }
    break;
    case GeomAbs_SurfaceOfRevolution:
    case GeomAbs_SurfaceOfExtrusion: {
      nbsv = 15;
      nbsu = 25;
    }
    break;
    default: {
      nbsu = 10;
      nbsv = 10;
    }
    break;
  }

  //-- If the number of points is too great, analyze
  //--
  //--

  if (nbsu < 10)
    nbsu = 10;
  if (nbsv < 10)
    nbsv = 10;

  myNbSamplesU = nbsu;
  myNbSamplesV = nbsv;

  //-- printf("\n BRepTopAdaptor_TopolTool NbSu=%d NbSv=%d ",nbsu,nbsv);
  if (nbsu > 10 || nbsv > 10)
  {
    if (typS == GeomAbs_BSplineSurface)
    {
      const occ::handle<Geom_BSplineSurface>& Bspl   = myS->BSpline();
      int                                     nbup   = Bspl->NbUPoles();
      int                                     nbvp   = Bspl->NbVPoles();
      const NCollection_Array2<gp_Pnt>&       array2 = Bspl->Poles();
      Analyse(array2, nbup, nbvp, myNbSamplesU, myNbSamplesV);
      nbsu = myNbSamplesU;
      nbsv = myNbSamplesV;
      //-- printf("\n Apres analyse BSPline  NbSu=%d NbSv=%d ",myNbSamplesU,myNbSamplesV);
    }
    else if (typS == GeomAbs_BezierSurface)
    {
      const occ::handle<Geom_BezierSurface>& Bez    = myS->Bezier();
      int                                    nbup   = Bez->NbUPoles();
      int                                    nbvp   = Bez->NbVPoles();
      const NCollection_Array2<gp_Pnt>&      array2 = Bez->Poles();
      Analyse(array2, nbup, nbvp, myNbSamplesU, myNbSamplesV);
      nbsu = myNbSamplesU;
      nbsv = myNbSamplesV;
      //-- printf("\n Apres analyse Bezier  NbSu=%d NbSv=%d ",myNbSamplesU,myNbSamplesV);
    }
  }

  if (nbsu < 10)
    nbsu = 10;
  if (nbsv < 10)
    nbsv = 10;

  myNbSamplesU = nbsu;
  myNbSamplesV = nbsv;

  myU0 = uinf;
  myV0 = vinf;

  myDU = (usup - uinf) / (myNbSamplesU + 1);
  myDV = (vsup - vinf) / (myNbSamplesV + 1);
}

//=================================================================================================

int BRepTopAdaptor_TopolTool::NbSamplesU()
{
  if (myNbSamplesU < 0)
  {
    ComputeSamplePoints();
  }
  return (myNbSamplesU);
}

//=================================================================================================

int BRepTopAdaptor_TopolTool::NbSamplesV()
{
  if (myNbSamplesU < 0)
  {
    ComputeSamplePoints();
  }
  return (myNbSamplesV);
}

//=================================================================================================

int BRepTopAdaptor_TopolTool::NbSamples()
{
  if (myNbSamplesU < 0)
  {
    ComputeSamplePoints();
  }
  return (myNbSamplesU * myNbSamplesV);
}

//=================================================================================================

void BRepTopAdaptor_TopolTool::SamplePoint(const int i, gp_Pnt2d& P2d, gp_Pnt& P3d)
{
  int    iv = 1 + i / myNbSamplesU;
  int    iu = 1 + i - (iv - 1) * myNbSamplesU;
  double u  = myU0 + iu * myDU;
  double v  = myV0 + iv * myDV;
  P2d.SetCoord(u, v);
  P3d = myS->Value(u, v);
}

//=================================================================================================

bool BRepTopAdaptor_TopolTool::DomainIsInfinite()
{
  double uinf, usup, vinf, vsup;
  uinf = myS->FirstUParameter();
  usup = myS->LastUParameter();
  vinf = myS->FirstVParameter();
  vsup = myS->LastVParameter();
  if (Precision::IsNegativeInfinite(uinf))
    return (true);
  if (Precision::IsPositiveInfinite(usup))
    return (true);
  if (Precision::IsNegativeInfinite(vinf))
    return (true);
  if (Precision::IsPositiveInfinite(vsup))
    return (true);
  return (false);
}

//=================================================================================================

bool BRepTopAdaptor_TopolTool::Has3d() const
{
  return true;
}

//=================================================================================================

double BRepTopAdaptor_TopolTool::Tol3d(const occ::handle<Adaptor2d_Curve2d>& C) const
{
  occ::handle<BRepAdaptor_Curve2d> brhc = occ::down_cast<BRepAdaptor_Curve2d>(C);
  if (brhc.IsNull())
  {
    throw Standard_DomainError("BRepTopAdaptor_TopolTool: arc has no 3d representation");
  }

  const TopoDS_Edge& edge = brhc->Edge();
  if (edge.IsNull())
    throw Standard_DomainError("BRepTopAdaptor_TopolTool: arc has no 3d representation");
  return BRep_Tool::Tolerance(edge);
}

//=================================================================================================

double BRepTopAdaptor_TopolTool::Tol3d(const occ::handle<Adaptor3d_HVertex>& V) const
{
  occ::handle<BRepTopAdaptor_HVertex> brhv = occ::down_cast<BRepTopAdaptor_HVertex>(V);
  if (brhv.IsNull())
    throw Standard_DomainError("BRepTopAdaptor_TopolTool: vertex has no 3d representation");
  const TopoDS_Vertex& ver = brhv->Vertex();
  if (ver.IsNull())
    throw Standard_DomainError("BRepTopAdaptor_TopolTool: vertex has no 3d representation");
  return BRep_Tool::Tolerance(ver);
}

//=================================================================================================

gp_Pnt BRepTopAdaptor_TopolTool::Pnt(const occ::handle<Adaptor3d_HVertex>& V) const
{
  occ::handle<BRepTopAdaptor_HVertex> brhv = occ::down_cast<BRepTopAdaptor_HVertex>(V);
  if (brhv.IsNull())
    throw Standard_DomainError("BRepTopAdaptor_TopolTool: vertex has no 3d representation");
  const TopoDS_Vertex& ver = brhv->Vertex();
  if (ver.IsNull())
    throw Standard_DomainError("BRepTopAdaptor_TopolTool: vertex has no 3d representation");
  return BRep_Tool::Pnt(ver);
}
