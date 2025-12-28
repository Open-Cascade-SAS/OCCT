// Created on: 1998-03-16
// Created by: Pierre BARRAS
// Copyright (c) 1998-1999 Matra Datavision
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

//    gka 30.04.99 S4137: extended for all types of surfaces

#include <Geom_BezierSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <gp_Ax1.hxx>
#include <Precision.hxx>
#include <ShapeExtend.hxx>
#include <ShapeExtend_CompositeSurface.hxx>
#include <ShapeUpgrade.hxx>
#include <ShapeUpgrade_SplitCurve3d.hxx>
#include <ShapeUpgrade_SplitSurface.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Type.hxx>
#include <Geom_Curve.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <TopoDS_Edge.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeUpgrade_SplitSurface, Standard_Transient)

//=================================================================================================

ShapeUpgrade_SplitSurface::ShapeUpgrade_SplitSurface()
    : myNbResultingRow(0),
      myNbResultingCol(0),
      myStatus(0)
{
}

//=================================================================================================

void ShapeUpgrade_SplitSurface::Init(const occ::handle<Geom_Surface>& S)
{
  myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);

  myUSplitValues   = new NCollection_HSequence<double>();
  myVSplitValues   = new NCollection_HSequence<double>();
  mySurface        = S;
  myResSurfaces    = new ShapeExtend_CompositeSurface();
  myNbResultingRow = 1;
  myNbResultingCol = 1;
  double U1, U2, V1, V2;
  mySurface->Bounds(U1, U2, V1, V2);

  myUSplitValues->Append(U1);
  myUSplitValues->Append(U2);

  myVSplitValues->Append(V1);
  myVSplitValues->Append(V2);
}

//=================================================================================================

void ShapeUpgrade_SplitSurface::Init(const occ::handle<Geom_Surface>& S,
                                     const double                     UFirst,
                                     const double                     ULast,
                                     const double                     VFirst,
                                     const double                     VLast,
                                     const double                     theArea)
{
  myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);

  mySurface      = S;
  myResSurfaces  = new ShapeExtend_CompositeSurface();
  myUSplitValues = new NCollection_HSequence<double>();
  myVSplitValues = new NCollection_HSequence<double>();

  myNbResultingRow = 1;
  myNbResultingCol = 1;

  myArea = theArea;

  double U1, U2, V1, V2;
  mySurface->Bounds(U1, U2, V1, V2);
  constexpr double precision = Precision::PConfusion();
  if (mySurface->IsUPeriodic() && ULast - UFirst <= U2 - U1 + precision)
  {
    U1 = UFirst;
    U2 = U1 + mySurface->UPeriod();
  }
  if (mySurface->IsVPeriodic() && VLast - VFirst <= V2 - V1 + precision)
  {
    V1 = VFirst;
    V2 = V1 + mySurface->VPeriod();
  }
  double UF, UL, VF, VL;
  if (UFirst > U2 - precision || ULast < U1 - precision)
  {
    UF = U1;
    UL = U2;
  }
  else
  {
    UF = std::max(U1, UFirst);
    UL = std::min(U2, ULast);
  }
  if (VFirst > V2 - precision || VLast < V1 - precision)
  {
    VF = V1;
    VL = V2;
  }
  else
  {
    VF = std::max(V1, VFirst);
    VL = std::min(V2, VLast);
  }

  if (myArea != 0.)
  {
    //<myArea> is set and will be used with <myUsize> and <myVsize>
    // in further computations
    double                                      Umid = (UF + UL) / 2, Vmid = (VF + VL) / 2;
    occ::handle<Geom_RectangularTrimmedSurface> aTrSurf =
      new Geom_RectangularTrimmedSurface(mySurface, UF, UL, VF, VL);
    occ::handle<Geom_Curve> anUiso     = aTrSurf->UIso(Umid);
    occ::handle<Geom_Curve> aViso      = aTrSurf->VIso(Vmid);
    TopoDS_Edge             anEdgeUiso = BRepLib_MakeEdge(anUiso);
    TopoDS_Edge             anEdgeViso = BRepLib_MakeEdge(aViso);
    GProp_GProps            aGprop1, aGprop2;
    BRepGProp::LinearProperties(anEdgeViso, aGprop1);
    myUsize = aGprop1.Mass();
    BRepGProp::LinearProperties(anEdgeUiso, aGprop2);
    myVsize = aGprop2.Mass();
  }

  if (UL - UF < precision)
  {
    double p2 = precision / 2.;
    UF -= p2;
    UL += p2;
  }
  if (VL - VF < precision)
  {
    double p2 = precision / 2.;
    VF -= p2;
    VL += p2;
  }

  myUSplitValues->Append(UF);
  myUSplitValues->Append(UL);
  myVSplitValues->Append(VF);
  myVSplitValues->Append(VL);
}

//=================================================================================================

void ShapeUpgrade_SplitSurface::SetUSplitValues(
  const occ::handle<NCollection_HSequence<double>>& UValues)
{
  if (UValues.IsNull())
    return;
  constexpr double precision = Precision::PConfusion();
  double UFirst = myUSplitValues->Value(1), ULast = myUSplitValues->Value(myUSplitValues->Length());
  int    i   = 1;
  int    len = UValues->Length();

  for (int ku = 2; ku <= myUSplitValues->Length(); ku++)
  {
    ULast = myUSplitValues->Value(ku);
    for (; i <= len; i++)
    {
      if ((UFirst + precision) >= UValues->Value(i))
        continue;
      if ((ULast - precision) <= UValues->Value(i))
        break;
      myUSplitValues->InsertBefore(ku++, UValues->Value(i));
    }
    UFirst = ULast;
  }
}

//=================================================================================================

void ShapeUpgrade_SplitSurface::SetVSplitValues(
  const occ::handle<NCollection_HSequence<double>>& VValues)
{
  if (VValues.IsNull())
    return;
  constexpr double precision = Precision::PConfusion();
  double VFirst = myVSplitValues->Value(1), VLast = myVSplitValues->Value(myVSplitValues->Length());
  int    i   = 1;
  int    len = VValues->Length();
  for (int kv = 2; kv <= myVSplitValues->Length(); kv++)
  {
    VLast = myVSplitValues->Value(kv);
    for (; i <= len; i++)
    {
      if ((VFirst + precision) >= VValues->Value(i))
        continue;
      if ((VLast - precision) <= VValues->Value(i))
        break;
      myVSplitValues->InsertBefore(kv++, VValues->Value(i));
    }
    VFirst = VLast;
  }
}

//=================================================================================================

void ShapeUpgrade_SplitSurface::Build(const bool Segment)
{

  double UFirst = myUSplitValues->Value(1);
  double ULast  = myUSplitValues->Value(myUSplitValues->Length());
  double VFirst = myVSplitValues->Value(1);
  double VLast  = myVSplitValues->Value(myVSplitValues->Length());

  if (myUSplitValues->Length() > 2 || myVSplitValues->Length() > 2)
    myStatus = ShapeExtend::EncodeStatus(ShapeExtend_DONE1);

  double U1, U2, V1, V2;
  mySurface->Bounds(U1, U2, V1, V2);
  if (mySurface->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution)))
  {
    occ::handle<Geom_SurfaceOfRevolution> Surface =
      occ::down_cast<Geom_SurfaceOfRevolution>(mySurface);
    occ::handle<Geom_Curve>   BasCurve = Surface->BasisCurve();
    ShapeUpgrade_SplitCurve3d spc;
    spc.Init(BasCurve, VFirst, VLast);
    spc.SetSplitValues(myVSplitValues);
    spc.Build(Segment);
    occ::handle<NCollection_HArray2<occ::handle<Geom_Surface>>> Surfaces;
    myNbResultingCol = spc.GetCurves()->Length();
    if (myUSplitValues->Length() > 2)
    {
      myNbResultingRow = myUSplitValues->Length() - 1;
      Surfaces         = new NCollection_HArray2<occ::handle<Geom_Surface>>(1,
                                                                    myNbResultingRow,
                                                                    1,
                                                                    myNbResultingCol);
      for (int nc = 1; nc <= myNbResultingCol; nc++)
      {
        occ::handle<Geom_SurfaceOfRevolution> NewSurfaceRev =
          new Geom_SurfaceOfRevolution(spc.GetCurves()->Value(nc), Surface->Axis());
        double U1p, U2p, V1p, V2p;
        NewSurfaceRev->Bounds(U1p, U2p, V1p, V2p);
        for (int nc1 = 1; nc1 <= myNbResultingRow; nc1++)
        {
          occ::handle<Geom_RectangularTrimmedSurface> NewSurf =
            new Geom_RectangularTrimmedSurface(NewSurfaceRev,
                                               myUSplitValues->Value(nc1),
                                               myUSplitValues->Value(nc1 + 1),
                                               V1p,
                                               V2p);
          Surfaces->SetValue(nc1, nc, NewSurf);
        }
      }
    }
    else
    {
      Surfaces = new NCollection_HArray2<occ::handle<Geom_Surface>>(1, 1, 1, myNbResultingCol);

      for (int nc = 1; nc <= spc.GetCurves()->Length(); nc++)
      {
        occ::handle<Geom_SurfaceOfRevolution> NewSurfaceRev =
          new Geom_SurfaceOfRevolution(spc.GetCurves()->Value(nc), Surface->Axis());
        NewSurfaceRev->Bounds(U1, U2, V1, V2);
        if (UFirst == U1 && ULast == U2)
          Surfaces->SetValue(1, nc, NewSurfaceRev);
        else
        {
          occ::handle<Geom_RectangularTrimmedSurface> NewSurf =
            new Geom_RectangularTrimmedSurface(NewSurfaceRev,
                                               UFirst,
                                               ULast,
                                               V1,
                                               V2); // pdn correction for main seq
          Surfaces->SetValue(1, nc, NewSurf);
        }
      }
    }
    myResSurfaces->Init(Surfaces);
    myResSurfaces->SetUFirstValue(myUSplitValues->Sequence().First());
    myResSurfaces->SetVFirstValue(myVSplitValues->Sequence().First());
    if (spc.Status(ShapeExtend_DONE1))
      myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
    if (spc.Status(ShapeExtend_DONE2))
      myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
    if (spc.Status(ShapeExtend_DONE3))
      myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE3);
    return;
  }

  if (mySurface->IsKind(STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)))
  {
    occ::handle<Geom_SurfaceOfLinearExtrusion> Surface =
      occ::down_cast<Geom_SurfaceOfLinearExtrusion>(mySurface);
    occ::handle<Geom_Curve>   BasCurve = Surface->BasisCurve();
    ShapeUpgrade_SplitCurve3d spc;
    spc.Init(BasCurve, UFirst, ULast);
    spc.SetSplitValues(myUSplitValues);
    spc.Build(Segment);
    myNbResultingRow = spc.GetCurves()->Length();
    occ::handle<NCollection_HArray2<occ::handle<Geom_Surface>>> Surfaces;
    if (myVSplitValues->Length() > 2)
    {
      myNbResultingCol = myVSplitValues->Length() - 1;
      Surfaces         = new NCollection_HArray2<occ::handle<Geom_Surface>>(1,
                                                                    myNbResultingRow,
                                                                    1,
                                                                    myNbResultingCol);
      for (int nc1 = 1; nc1 <= myNbResultingRow; nc1++)
      {
        occ::handle<Geom_SurfaceOfLinearExtrusion> NewSurfaceEx =
          new Geom_SurfaceOfLinearExtrusion(spc.GetCurves()->Value(nc1), Surface->Direction());
        double U1p, U2p, V1p, V2p;
        NewSurfaceEx->Bounds(U1p, U2p, V1p, V2p);
        for (int nc2 = 1; nc2 <= myNbResultingCol; nc2++)
        {
          occ::handle<Geom_RectangularTrimmedSurface> NewSurf =
            new Geom_RectangularTrimmedSurface(NewSurfaceEx,
                                               U1p,
                                               U2p,
                                               myVSplitValues->Value(nc2),
                                               myVSplitValues->Value(nc2 + 1));
          Surfaces->SetValue(nc1, nc2, NewSurf);
        }
      }
    }
    else
    {
      Surfaces = new NCollection_HArray2<occ::handle<Geom_Surface>>(1, myNbResultingRow, 1, 1);

      for (int nc1 = 1; nc1 <= myNbResultingRow; nc1++)
      {
        occ::handle<Geom_SurfaceOfLinearExtrusion> NewSurfaceEx =
          new Geom_SurfaceOfLinearExtrusion(spc.GetCurves()->Value(nc1), Surface->Direction());
        NewSurfaceEx->Bounds(U1, U2, V1, V2);
        if (VFirst == V1 && VLast == V2)
          Surfaces->SetValue(nc1, 1, NewSurfaceEx);
        else
        {
          occ::handle<Geom_RectangularTrimmedSurface> NewSurf =
            new Geom_RectangularTrimmedSurface(NewSurfaceEx,
                                               std::max(U1, UFirst),
                                               std::min(ULast, U2),
                                               std::max(VFirst, V1),
                                               std::min(VLast, V2));
          Surfaces->SetValue(nc1, 1, NewSurf);
        }
      }
    }
    myResSurfaces->Init(Surfaces);
    myResSurfaces->SetUFirstValue(myUSplitValues->Sequence().First());
    myResSurfaces->SetVFirstValue(myVSplitValues->Sequence().First());
    if (spc.Status(ShapeExtend_DONE1))
      myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
    if (spc.Status(ShapeExtend_DONE2))
      myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
    if (spc.Status(ShapeExtend_DONE3))
      myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE3);
    return;
  }

  if (mySurface->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
  {
    occ::handle<Geom_RectangularTrimmedSurface> tmp =
      occ::down_cast<Geom_RectangularTrimmedSurface>(mySurface);
    occ::handle<Geom_Surface> theSurf = tmp->BasisSurface();
    ShapeUpgrade_SplitSurface sps;
    sps.Init(theSurf, UFirst, ULast, VFirst, VLast);
    sps.SetUSplitValues(myUSplitValues);
    sps.SetVSplitValues(myVSplitValues);
    sps.myStatus = myStatus;
    sps.Build(Segment);
    myStatus |= sps.myStatus;
    myResSurfaces = sps.myResSurfaces;
    return;
  }
  else if (mySurface->IsKind(STANDARD_TYPE(Geom_OffsetSurface)))
  {
    occ::handle<Geom_OffsetSurface> tmp     = occ::down_cast<Geom_OffsetSurface>(mySurface);
    occ::handle<Geom_Surface>       theSurf = tmp->BasisSurface();
    ShapeUpgrade_SplitSurface       sps;
    sps.Init(theSurf, UFirst, ULast, VFirst, VLast);
    sps.SetUSplitValues(myUSplitValues);
    sps.SetVSplitValues(myVSplitValues);
    sps.myStatus = myStatus;
    sps.Build(Segment);
    occ::handle<NCollection_HArray2<occ::handle<Geom_Surface>>> Patches =
      new NCollection_HArray2<occ::handle<Geom_Surface>>(1,
                                                         sps.ResSurfaces()->NbUPatches(),
                                                         1,
                                                         sps.ResSurfaces()->NbVPatches());
    for (int i1 = 1; i1 <= sps.ResSurfaces()->NbUPatches(); i1++)
    {
      for (int j1 = 1; j1 <= sps.ResSurfaces()->NbVPatches(); j1++)
      {
        occ::handle<Geom_OffsetSurface> NewOffSur =
          new Geom_OffsetSurface(sps.ResSurfaces()->Patch(i1, j1), tmp->Offset());
        Patches->SetValue(i1, j1, NewOffSur);
      }
    }
    myResSurfaces->Init(Patches);
    myResSurfaces->SetUFirstValue(myUSplitValues->Sequence().First());
    myResSurfaces->SetVFirstValue(myVSplitValues->Sequence().First());
    myStatus |= sps.myStatus;
    return;
  }

  // splitting the surfaces:
  myNbResultingRow = myUSplitValues->Length() - 1;
  myNbResultingCol = myVSplitValues->Length() - 1;
  occ::handle<NCollection_HArray2<occ::handle<Geom_Surface>>> Surfaces =
    new NCollection_HArray2<occ::handle<Geom_Surface>>(1, myNbResultingRow, 1, myNbResultingCol);
  bool isBSpline = mySurface->IsKind(STANDARD_TYPE(Geom_BSplineSurface));
  bool isBezier  = mySurface->IsKind(STANDARD_TYPE(Geom_BezierSurface));

  // double U1,U2,V1,V2;
  // U1=UFirst;
  // U2 = ULast;
  // V1 = VFirst;
  // V2 = VLast;

  if (myNbResultingRow == 1 && myNbResultingCol == 1)
  {
    mySurface->Bounds(U1, U2, V1, V2);
    bool filled = true;
    if (std::abs(U1 - UFirst) < Precision::PConfusion()
        && std::abs(U2 - ULast) < Precision::PConfusion()
        && std::abs(V1 - VFirst) < Precision::PConfusion()
        && std::abs(V2 - VLast) < Precision::PConfusion())
      Surfaces->SetValue(1, 1, mySurface);
    else if (!Segment || !mySurface->IsKind(STANDARD_TYPE(Geom_BSplineSurface))
             || !Status(ShapeExtend_DONE2))
    {
      // pdn copying of surface
      occ::handle<Geom_Surface> tmp = occ::down_cast<Geom_Surface>(mySurface->Copy());
      occ::handle<Geom_RectangularTrimmedSurface> Surf =
        new Geom_RectangularTrimmedSurface(tmp, UFirst, ULast, VFirst, VLast);
      Surfaces->SetValue(1, 1, Surf);
    }
    else
      filled = false;
    if (filled)
    {
      myResSurfaces->Init(Surfaces);
      myResSurfaces->SetUFirstValue(myUSplitValues->Sequence().First());
      myResSurfaces->SetVFirstValue(myVSplitValues->Sequence().First());
      return;
    }
  }
  if (mySurface->IsKind(STANDARD_TYPE(Geom_BSplineSurface)))
  {
    occ::handle<Geom_BSplineSurface> BsSurface =
      occ::down_cast<Geom_BSplineSurface>(mySurface->Copy());
    int FirstInd = BsSurface->FirstUKnotIndex(), LastInd = BsSurface->LastUKnotIndex();
    int j = FirstInd;
    for (int ii = 1; ii <= myUSplitValues->Length(); ii++)
    {
      double spval = myUSplitValues->Value(ii);
      for (; j <= LastInd; j++)
      {
        if (spval > BsSurface->UKnot(j) + Precision::PConfusion())
          continue;
        if (spval < BsSurface->UKnot(j) - Precision::PConfusion())
          break;
        myUSplitValues->ChangeValue(ii) = BsSurface->UKnot(j);
      }
      if (j == LastInd)
        break;
    }
    FirstInd = BsSurface->FirstVKnotIndex(), LastInd = BsSurface->LastVKnotIndex();
    j = FirstInd;
    for (int ii1 = 1; ii1 <= myVSplitValues->Length(); ii1++)
    {
      double spval = myVSplitValues->Value(ii1);
      for (; j <= LastInd; j++)
      {
        if (spval > BsSurface->VKnot(j) + Precision::PConfusion())
          continue;
        if (spval < BsSurface->VKnot(j) - Precision::PConfusion())
          break;
        myVSplitValues->ChangeValue(ii1) = BsSurface->VKnot(j);
      }
      if (j == LastInd)
        break;
    }
  }
  U1 = myUSplitValues->Value(1);
  V1 = myVSplitValues->Value(1);
  for (int irow = 2; irow <= myUSplitValues->Length(); irow++)
  {
    U2 = myUSplitValues->Value(irow);
    for (int icol = 2; icol <= myVSplitValues->Length(); icol++)
    {
      V2 = myVSplitValues->Value(icol);
      //      if (ShapeUpgrade::Debug())  {
      //	std::cout<<".. bounds    ="<<U1    <<","<<U2   <<","<<V1    <<","<<V2   <<std::endl;
      //	std::cout<<".. -> pos ="<<irow  <<","<<icol<<std::endl;
      //      }
      // creates a copy of theSurf before to segment:
      occ::handle<Geom_Surface> theNew = occ::down_cast<Geom_Surface>(mySurface->Copy());
      if (isBSpline || isBezier)
      {
        try
        {
          OCC_CATCH_SIGNALS
          if (isBSpline)
            occ::down_cast<Geom_BSplineSurface>(theNew)->Segment(U1, U2, V1, V2);
          else if (isBezier)
          {
            // pdn K4L+ (work around)
            //  double u1 = 2*U1 - 1;
            //  double u2 = 2*U2 - 1;
            //  double v1 = 2*V1 - 1;
            //  double v2 = 2*V2 - 1;
            // rln C30 (direct use)
            double u1 = U1;
            double u2 = U2;
            double v1 = V1;
            double v2 = V2;
            occ::down_cast<Geom_BezierSurface>(theNew)->Segment(u1, u2, v1, v2);
          }
          myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE3);
        }
        catch (Standard_Failure const& anException)
        {
#ifdef OCCT_DEBUG
          std::cout << "Warning: ShapeUpgrade_SplitSurface::Build(): Exception in Segment      :";
          anException.Print(std::cout);
          std::cout << std::endl;
#endif
          (void)anException;
          occ::handle<Geom_Surface> theNewSurf = theNew;
          theNew = new Geom_RectangularTrimmedSurface(theNewSurf, U1, U2, V1, V2);
        }
        Surfaces->SetValue((irow - 1), (icol - 1), theNew);
      }
      else
      {
        // not a BSpline: trimming instead of segmentation
        occ::handle<Geom_RectangularTrimmedSurface> SplittedSurf =
          new Geom_RectangularTrimmedSurface(theNew, U1, U2, V1, V2);
        Surfaces->SetValue((irow - 1), (icol - 1), SplittedSurf);
      }

      V1 = V2;
    }
    U1 = U2;
    V1 = myVSplitValues->Value(1);
  }
  int                        nbU = myUSplitValues->Length();
  NCollection_Array1<double> UJoints(1, nbU);
  int                        i; // svv Jan 10 2000 : porting on DEC
  for (i = 1; i <= nbU; i++)
    UJoints(i) = myUSplitValues->Value(i);

  int                        nbV = myVSplitValues->Length();
  NCollection_Array1<double> VJoints(1, nbV);
  for (i = 1; i <= nbV; i++)
    VJoints(i) = myVSplitValues->Value(i);
  myResSurfaces->Init(Surfaces, UJoints, VJoints);
  //  if (ShapeUpgrade::Debug()) std::cout<<"SplitSurface::Build - end"<<std::endl;
}

//=================================================================================================

const occ::handle<NCollection_HSequence<double>>& ShapeUpgrade_SplitSurface::USplitValues() const
{
  return myUSplitValues;
}

//=================================================================================================

const occ::handle<NCollection_HSequence<double>>& ShapeUpgrade_SplitSurface::VSplitValues() const
{
  return myVSplitValues;
}

//=================================================================================================

void ShapeUpgrade_SplitSurface::Perform(const bool Segment)
{
  Compute(Segment);
  //  SetUSplitValues(myUSplitValues);
  //  SetVSplitValues(myVSplitValues);
  Build(Segment);
}

//=================================================================================================

void ShapeUpgrade_SplitSurface::Compute(const bool /*Segment*/)
{
  myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
}

//=================================================================================================

bool ShapeUpgrade_SplitSurface::Status(const ShapeExtend_Status status) const
{
  return ShapeExtend::DecodeStatus(myStatus, status);
}

const occ::handle<ShapeExtend_CompositeSurface>& ShapeUpgrade_SplitSurface::ResSurfaces() const
{
  return myResSurfaces;
}
