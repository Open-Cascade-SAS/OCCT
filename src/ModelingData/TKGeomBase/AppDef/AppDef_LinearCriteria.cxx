// Created on: 1998-11-30
// Created by: Igor FEOKTISTOV
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

#include <AppDef_LinearCriteria.hxx>
#include <AppDef_MultiLine.hxx>
#include <AppDef_MyLineTool.hxx>
#include <FEmTool_Curve.hxx>
#include <FEmTool_LinearFlexion.hxx>
#include <FEmTool_LinearJerk.hxx>
#include <FEmTool_LinearTension.hxx>
#include <GeomAbs_Shape.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <math_Gauss.hxx>
#include <math_Matrix.hxx>
#include <PLib_HermitJacobi.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Type.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AppDef_LinearCriteria, AppDef_SmoothCriterion)

static int order(const PLib_HermitJacobi& B)
{
  return B.NivConstr();
}

//=================================================================================================

AppDef_LinearCriteria::AppDef_LinearCriteria(const AppDef_MultiLine& SSP,
                                             const int  FirstPoint,
                                             const int  LastPoint)
    : mySSP(SSP),
      myQuadraticWeight(0.0),
      myQualityWeight(0.0),
      myPntWeight(FirstPoint, LastPoint),
      myLength(0.0),
      myE(0),
      IF(0),
      IL(0)
{
  memset(myEstimation, 0, sizeof(myEstimation));
  memset(myPercent, 0, sizeof(myPercent));
  myPntWeight.Init(1.);
}

//=================================================================================================

void AppDef_LinearCriteria::SetParameters(const occ::handle<NCollection_HArray1<double>>& Parameters)
{
  myParameters = Parameters;
  myE          = 0; // Cache become invalid.
}

//=================================================================================================

void AppDef_LinearCriteria::SetCurve(const occ::handle<FEmTool_Curve>& C)
{

  if (myCurve.IsNull())
  {
    myCurve = C;

    int MxDeg = myCurve->Base().WorkDegree(), NbDim = myCurve->Dimension(),
                     Order = order(myCurve->Base());

    GeomAbs_Shape ConstraintOrder = GeomAbs_C0;
    switch (Order)
    {
      case 0:
        ConstraintOrder = GeomAbs_C0;
        break;
      case 1:
        ConstraintOrder = GeomAbs_C1;
        break;
      case 2:
        ConstraintOrder = GeomAbs_C2;
    }

    myCriteria[0] = new FEmTool_LinearTension(MxDeg, ConstraintOrder);
    myCriteria[1] = new FEmTool_LinearFlexion(MxDeg, ConstraintOrder);
    myCriteria[2] = new FEmTool_LinearJerk(MxDeg, ConstraintOrder);

    occ::handle<NCollection_HArray2<double>> Coeff = new NCollection_HArray2<double>(0, 0, 1, NbDim);

    myCriteria[0]->Set(Coeff);
    myCriteria[1]->Set(Coeff);
    myCriteria[2]->Set(Coeff);
  }
  else if (myCurve != C)
  {

    int OldMxDeg = myCurve->Base().WorkDegree(), OldNbDim = myCurve->Dimension(),
                     OldOrder = order(myCurve->Base());

    myCurve = C;

    int MxDeg = myCurve->Base().WorkDegree(), NbDim = myCurve->Dimension(),
                     Order = order(myCurve->Base());

    if (MxDeg != OldMxDeg || Order != OldOrder)
    {

      GeomAbs_Shape ConstraintOrder = GeomAbs_C0;
      switch (Order)
      {
        case 0:
          ConstraintOrder = GeomAbs_C0;
          break;
        case 1:
          ConstraintOrder = GeomAbs_C1;
          break;
        case 2:
          ConstraintOrder = GeomAbs_C2;
      }

      myCriteria[0] = new FEmTool_LinearTension(MxDeg, ConstraintOrder);
      myCriteria[1] = new FEmTool_LinearFlexion(MxDeg, ConstraintOrder);
      myCriteria[2] = new FEmTool_LinearJerk(MxDeg, ConstraintOrder);

      occ::handle<NCollection_HArray2<double>> Coeff = new NCollection_HArray2<double>(0, 0, 1, NbDim);

      myCriteria[0]->Set(Coeff);
      myCriteria[1]->Set(Coeff);
      myCriteria[2]->Set(Coeff);
    }
    else if (NbDim != OldNbDim)
    {

      occ::handle<NCollection_HArray2<double>> Coeff = new NCollection_HArray2<double>(0, 0, 1, NbDim);

      myCriteria[0]->Set(Coeff);
      myCriteria[1]->Set(Coeff);
      myCriteria[2]->Set(Coeff);
    }
  }
}

//=================================================================================================

void AppDef_LinearCriteria::GetCurve(occ::handle<FEmTool_Curve>& C) const
{
  C = myCurve;
}

//=================================================================================================

void AppDef_LinearCriteria::SetEstimation(const double E1,
                                          const double E2,
                                          const double E3)
{
  myEstimation[0] = E1;
  myEstimation[1] = E2;
  myEstimation[2] = E3;
}

double& AppDef_LinearCriteria::EstLength()
{
  return myLength;
}

//=================================================================================================

void AppDef_LinearCriteria::GetEstimation(double& E1,
                                          double& E2,
                                          double& E3) const
{
  E1 = myEstimation[0];
  E2 = myEstimation[1];
  E3 = myEstimation[2];
}

//=================================================================================================

occ::handle<NCollection_HArray2<occ::handle<NCollection_HArray1<int>>>> AppDef_LinearCriteria::AssemblyTable() const
{
  if (myCurve.IsNull())
    throw Standard_DomainError("AppDef_LinearCriteria::AssemblyTable");

  int NbDim = myCurve->Dimension(), NbElm = myCurve->NbElements(),
                   nc1   = order(myCurve->Base()) + 1;
  int MxDeg = myCurve->Base().WorkDegree();

  occ::handle<NCollection_HArray2<occ::handle<NCollection_HArray1<int>>>> AssTable = new NCollection_HArray2<occ::handle<NCollection_HArray1<int>>>(1, NbDim, 1, NbElm);

  occ::handle<NCollection_HArray1<int>> GlobIndex, Aux;

  int i, el = 1, dim = 1, NbGlobVar = 0, gi0;

  // For dim = 1
  // For first element (el = 1)
  GlobIndex = new NCollection_HArray1<int>(0, MxDeg);

  for (i = 0; i < nc1; i++)
  {
    NbGlobVar++;
    GlobIndex->SetValue(i, NbGlobVar);
  }
  gi0 = MxDeg - 2 * nc1 + 1;
  for (i = nc1; i < 2 * nc1; i++)
  {
    NbGlobVar++;
    GlobIndex->SetValue(i, NbGlobVar + gi0);
  }
  for (i = 2 * nc1; i <= MxDeg; i++)
  {
    NbGlobVar++;
    GlobIndex->SetValue(i, NbGlobVar - nc1);
  }
  gi0 = NbGlobVar - nc1 + 1;
  AssTable->SetValue(dim, el, GlobIndex);

  // For rest elements
  for (el = 2; el <= NbElm; el++)
  {
    GlobIndex = new NCollection_HArray1<int>(0, MxDeg);
    for (i = 0; i < nc1; i++)
      GlobIndex->SetValue(i, gi0 + i);

    gi0 = MxDeg - 2 * nc1 + 1;
    for (i = nc1; i < 2 * nc1; i++)
    {
      NbGlobVar++;
      GlobIndex->SetValue(i, NbGlobVar + gi0);
    }
    for (i = 2 * nc1; i <= MxDeg; i++)
    {
      NbGlobVar++;
      GlobIndex->SetValue(i, NbGlobVar - nc1);
    }
    gi0 = NbGlobVar - nc1 + 1;
    AssTable->SetValue(dim, el, GlobIndex);
  }

  // For other dimensions
  gi0 = NbGlobVar;
  for (dim = 2; dim <= NbDim; dim++)
  {
    for (el = 1; el <= NbElm; el++)
    {
      Aux       = AssTable->Value(1, el);
      GlobIndex = new NCollection_HArray1<int>(0, MxDeg);
      for (i = 0; i <= MxDeg; i++)
        GlobIndex->SetValue(i, Aux->Value(i) + NbGlobVar);
      AssTable->SetValue(dim, el, GlobIndex);
    }
    NbGlobVar += gi0;
  }

  return AssTable;
}

//=================================================================================================

occ::handle<NCollection_HArray2<int>> AppDef_LinearCriteria::DependenceTable() const
{
  if (myCurve.IsNull())
    throw Standard_DomainError("AppDef_LinearCriteria::DependenceTable");

  int Dim = myCurve->Dimension();

  occ::handle<NCollection_HArray2<int>> DepTab = new NCollection_HArray2<int>(1, Dim, 1, Dim, 0);
  int                 i;
  for (i = 1; i <= Dim; i++)
    DepTab->SetValue(i, i, 1);

  return DepTab;
}

//=================================================================================================

int AppDef_LinearCriteria::QualityValues(const double J1min,
                                                      const double J2min,
                                                      const double J3min,
                                                      double&      J1,
                                                      double&      J2,
                                                      double&      J3)
{
  if (myCurve.IsNull())
    throw Standard_DomainError("AppDef_LinearCriteria::QualityValues");

  int NbDim = myCurve->Dimension(), NbElm = myCurve->NbElements();

  NCollection_Array1<double>&         Knots = myCurve->Knots();
  occ::handle<NCollection_HArray2<double>> Coeff;

  int el, deg = 0, curdeg, i;
  double    UFirst, ULast;

  J1 = J2 = J3 = 0.;
  for (el = 1; el <= NbElm; el++)
  {

    curdeg = myCurve->Degree(el);
    if (deg != curdeg)
    {
      deg   = curdeg;
      Coeff = new NCollection_HArray2<double>(0, deg, 1, NbDim);
    }

    myCurve->GetElement(el, Coeff->ChangeArray2());

    UFirst = Knots(el);
    ULast  = Knots(el + 1);

    myCriteria[0]->Set(Coeff);
    myCriteria[0]->Set(UFirst, ULast);
    J1 = J1 + myCriteria[0]->Value();

    myCriteria[1]->Set(Coeff);
    myCriteria[1]->Set(UFirst, ULast);
    J2 = J2 + myCriteria[1]->Value();

    myCriteria[2]->Set(Coeff);
    myCriteria[2]->Set(UFirst, ULast);
    J3 = J3 + myCriteria[2]->Value();
  }

  // Calculation of ICDANA - see MOTEST.f
  //  double JEsMin[3] = {.01, .001, .001}; // from MOTLIS.f
  double JEsMin[3];
  JEsMin[0] = J1min;
  JEsMin[1] = J2min;
  JEsMin[2] = J3min;
  double ValCri[3];
  ValCri[0] = J1;
  ValCri[1] = J2;
  ValCri[2] = J3;

  int ICDANA = 0;

  //   (2) Test l'amelioration des estimations
  //       (critere sureleve => Non minimisation )

  for (i = 0; i <= 2; i++)
  {
    if ((ValCri[i] < 0.8 * myEstimation[i]) && (myEstimation[i] > JEsMin[i]))
    {
      if (ICDANA < 1)
        ICDANA = 1;
      if (ValCri[i] < 0.1 * myEstimation[i])
        ICDANA = 2;
      myEstimation[i] = std::max(1.05 * ValCri[i], JEsMin[i]);
    }
  }

  //  (3) Mise a jours des Estimation
  //     (critere sous-estimer => mauvais conditionement)
  if (ValCri[0] > myEstimation[0] * 2)
  {
    myEstimation[0] += ValCri[0] * .1;
    if (ICDANA == 0)
    {
      if (ValCri[0] > myEstimation[0] * 10)
      {
        ICDANA = 2;
      }
      else
      {
        ICDANA = 1;
      }
    }
    else
    {
      ICDANA = 2;
    }
  }
  if (ValCri[1] > myEstimation[1] * 20)
  {
    myEstimation[1] += ValCri[1] * .1;
    if (ICDANA == 0)
    {
      if (ValCri[1] > myEstimation[1] * 100)
      {
        ICDANA = 2;
      }
      else
      {
        ICDANA = 1;
      }
    }
    else
    {
      ICDANA = 2;
    }
  }
  if (ValCri[2] > myEstimation[2] * 20)
  {
    myEstimation[2] += ValCri[2] * .05;
    if (ICDANA == 0)
    {
      if (ValCri[2] > myEstimation[2] * 100)
      {
        ICDANA = 2;
      }
      else
      {
        ICDANA = 1;
      }
    }
    else
    {
      ICDANA = 2;
    }
  }

  return ICDANA;
}

//=================================================================================================

void AppDef_LinearCriteria::ErrorValues(double& MaxError,
                                        double& QuadraticError,
                                        double& AverageError)
{
  if (myCurve.IsNull())
    throw Standard_DomainError("AppDef_LinearCriteria::ErrorValues");

  int NbDim = myCurve->Dimension();

  int myNbP2d = AppDef_MyLineTool::NbP2d(mySSP),
                   myNbP3d = AppDef_MyLineTool::NbP3d(mySSP);

  if (NbDim != (2 * myNbP2d + 3 * myNbP3d))
    throw Standard_DomainError("AppDef_LinearCriteria::ErrorValues");

  NCollection_Array1<gp_Pnt>   TabP3d(1, std::max(1, myNbP3d));
  NCollection_Array1<gp_Pnt2d> TabP2d(1, std::max(1, myNbP2d));
  NCollection_Array1<double> BasePoint(1, NbDim);
  gp_Pnt2d             P2d;
  gp_Pnt               P3d;

  int i, ipnt, c0 = 0;
  double    SqrDist, Dist;

  MaxError = QuadraticError = AverageError = 0.;

  for (i = myParameters->Lower(); i <= myParameters->Upper(); i++)
  {

    myCurve->D0(myParameters->Value(i), BasePoint);

    c0 = 0;
    AppDef_MyLineTool::Value(mySSP, i, TabP3d);
    for (ipnt = 1; ipnt <= myNbP3d; ipnt++)
    {
      P3d.SetCoord(BasePoint(c0 + 1), BasePoint(c0 + 2), BasePoint(c0 + 3));
      SqrDist  = P3d.SquareDistance(TabP3d(ipnt));
      Dist     = std::sqrt(SqrDist);
      MaxError = std::max(MaxError, Dist);
      QuadraticError += SqrDist;
      AverageError += Dist;
      c0 += 3;
    }

    if (myNbP3d == 0)
      AppDef_MyLineTool::Value(mySSP, i, TabP2d);
    else
      AppDef_MyLineTool::Value(mySSP, i, TabP3d, TabP2d);
    for (ipnt = 1; ipnt <= myNbP2d; ipnt++)
    {
      P2d.SetCoord(BasePoint(c0 + 1), BasePoint(c0 + 2));
      SqrDist  = P2d.SquareDistance(TabP2d(ipnt));
      Dist     = std::sqrt(SqrDist);
      MaxError = std::max(MaxError, Dist);
      QuadraticError += SqrDist;
      AverageError += Dist;
      c0 += 2;
    }
  }
}

//=================================================================================================

void AppDef_LinearCriteria::Hessian(const int Element,
                                    const int Dimension1,
                                    const int Dimension2,
                                    math_Matrix&           H)
{
  if (myCurve.IsNull())
    throw Standard_DomainError("AppDef_LinearCriteria::Hessian");

  if (DependenceTable()->Value(Dimension1, Dimension2) == 0)
    throw Standard_DomainError("AppDef_LinearCriteria::Hessian");

  int // NbDim = myCurve->Dimension(),
    MxDeg = myCurve->Base().WorkDegree(),
    //                   Deg   = myCurve->Degree(Element),
    Order = order(myCurve->Base());

  math_Matrix AuxH(0, H.RowNumber() - 1, 0, H.ColNumber() - 1, 0.);

  NCollection_Array1<double>& Knots = myCurve->Knots();
  double         UFirst, ULast;

  UFirst = Knots(Element);
  ULast  = Knots(Element + 1);

  int icrit;

  // Quality criterion part of Hessian

  H.Init(0);

  for (icrit = 0; icrit <= 2; icrit++)
  {
    myCriteria[icrit]->Set(UFirst, ULast);
    myCriteria[icrit]->Hessian(Dimension1, Dimension2, AuxH);
    H += (myQualityWeight * myPercent[icrit] / myEstimation[icrit]) * AuxH;
  }

  // Least square part of Hessian

  AuxH.Init(0.);

  double    coeff = (ULast - UFirst) / 2., curcoeff, poid;
  int ipnt, ii, degH = 2 * Order + 1;

  int k1, k2, i, j, i0 = H.LowerRow(), j0 = H.LowerCol(), i1, j1,
                                 di = myPntWeight.Lower() - myParameters->Lower();

  // BuilCache
  if (myE != Element)
    BuildCache(Element);

  // Compute the least square Hessian
  for (ii = 1, ipnt = IF; ipnt <= IL; ipnt++, ii += (MxDeg + 1))
  {
    poid                    = myPntWeight(di + ipnt) * 2.;
    const double* BV = &myCache->Value(ii);

    // Hermite*Hermite part of matrix
    for (i = 0; i <= degH; i++)
    {
      k1       = (i <= Order) ? i : i - Order - 1;
      curcoeff = std::pow(coeff, k1) * poid * BV[i];

      // Hermite*Hermite part of matrix
      for (j = i; j <= degH; j++)
      {
        k2 = (j <= Order) ? j : j - Order - 1;
        AuxH(i, j) += curcoeff * std::pow(coeff, k2) * BV[j];
      }
      // Hermite*Jacobi part of matrix
      for (j = degH + 1; j <= MxDeg; j++)
      {
        AuxH(i, j) += curcoeff * BV[j];
      }
    }

    // Jacoby*Jacobi part of matrix
    for (i = degH + 1; i <= MxDeg; i++)
    {
      curcoeff = BV[i] * poid;
      for (j = i; j <= MxDeg; j++)
      {
        AuxH(i, j) += curcoeff * BV[j];
      }
    }
  }

  i1 = i0;
  for (i = 0; i <= MxDeg; i++)
  {
    j1 = j0 + i;
    for (j = i; j <= MxDeg; j++)
    {
      H(i1, j1) += myQuadraticWeight * AuxH(i, j);
      H(j1, i1) = H(i1, j1);
      j1++;
    }
    i1++;
  }
}

//=================================================================================================

void AppDef_LinearCriteria::Gradient(const int Element,
                                     const int Dimension,
                                     math_Vector&           G)
{
  if (myCurve.IsNull())
    throw Standard_DomainError("AppDef_LinearCriteria::ErrorValues");

  int myNbP2d = AppDef_MyLineTool::NbP2d(mySSP),
                   myNbP3d = AppDef_MyLineTool::NbP3d(mySSP);

  if (Dimension > (2 * myNbP2d + 3 * myNbP3d))
    throw Standard_DomainError("AppDef_LinearCriteria::ErrorValues");

  NCollection_Array1<gp_Pnt>   TabP3d(1, std::max(1, myNbP3d));
  NCollection_Array1<gp_Pnt2d> TabP2d(1, std::max(1, myNbP2d));

  bool In3d;
  int IndPnt, IndCrd;

  if (Dimension <= 3 * myNbP3d)
  {
    In3d   = true;
    IndCrd = Dimension % 3;
    IndPnt = Dimension / 3;
    if (IndCrd == 0)
      IndCrd = 3;
    else
      IndPnt++;
  }
  else
  {
    In3d   = false;
    IndCrd = (Dimension - 3 * myNbP3d) % 2;
    IndPnt = (Dimension - 3 * myNbP3d) / 2;
    if (IndCrd == 0)
      IndCrd = 2;
    else
      IndPnt++;
  }

  NCollection_Array1<double>& Knots = myCurve->Knots();
  double         UFirst, ULast, Pnt;
  UFirst              = Knots(Element);
  ULast               = Knots(Element + 1);
  double coeff = (ULast - UFirst) / 2;

  int // Deg   = myCurve->Degree(Element),
    Order = order(myCurve->Base());

  const PLib_HermitJacobi& myBase = myCurve->Base();
  int         MxDeg  = myBase.WorkDegree();

  double    curcoeff;
  int degH = 2 * Order + 1;
  int ipnt, k, i, ii, i0 = G.Lower(), di = myPntWeight.Lower() - myParameters->Lower();

  if (myE != Element)
    BuildCache(Element);

  G.Init(0.);

  const double* BV = &myCache->Value(1);
  BV--;

  for (ii = 1, ipnt = IF; ipnt <= IL; ipnt++)
  {
    if (In3d)
    {
      AppDef_MyLineTool::Value(mySSP, ipnt, TabP3d);
      Pnt = TabP3d(IndPnt).Coord(IndCrd);
    }
    else
    {
      if (myNbP3d == 0)
        AppDef_MyLineTool::Value(mySSP, ipnt, TabP2d);
      else
        AppDef_MyLineTool::Value(mySSP, ipnt, TabP3d, TabP2d);
      Pnt = TabP2d(IndPnt).Coord(IndCrd);
    }

    curcoeff = Pnt * myPntWeight(di + ipnt);
    for (i = 0; i <= MxDeg; i++, ii++)
      G(i0 + i) += BV[ii] * curcoeff;
  }

  G *= 2. * myQuadraticWeight;

  for (i = 0; i <= degH; i++)
  {
    k        = (i <= Order) ? i : i - Order - 1;
    curcoeff = std::pow(coeff, k);
    G(i0 + i) *= curcoeff;
  }
}

//=================================================================================================

void AppDef_LinearCriteria::InputVector(const math_Vector&                    X,
                                        const occ::handle<NCollection_HArray2<occ::handle<NCollection_HArray1<int>>>>& AssTable)
{
  int NbDim = myCurve->Dimension(), NbElm = myCurve->NbElements();
  int MxDeg = 0;
  MxDeg                  = myCurve->Base().WorkDegree();
  NCollection_Array2<double> CoeffEl(0, MxDeg, 1, NbDim);

  occ::handle<NCollection_HArray1<int>> GlobIndex;

  int el, dim, i, i0 = X.Lower() - 1;

  for (el = 1; el <= NbElm; el++)
  {
    for (dim = 1; dim <= NbDim; dim++)
    {
      GlobIndex = AssTable->Value(dim, el);
      for (i = 0; i <= MxDeg; i++)
        CoeffEl(i, dim) = X(i0 + GlobIndex->Value(i));
    }
    myCurve->SetDegree(el, MxDeg);
    myCurve->SetElement(el, CoeffEl);
  }
}

//=================================================================================================

void AppDef_LinearCriteria::SetWeight(const double QuadraticWeight,
                                      const double QualityWeight,
                                      const double percentJ1,
                                      const double percentJ2,
                                      const double percentJ3)
{
  if (QuadraticWeight < 0. || QualityWeight < 0.)
    throw Standard_DomainError("AppDef_LinearCriteria::SetWeight");
  if (percentJ1 < 0. || percentJ2 < 0. || percentJ3 < 0.)
    throw Standard_DomainError("AppDef_LinearCriteria::SetWeight");

  myQuadraticWeight = QuadraticWeight;
  myQualityWeight   = QualityWeight;

  double Total = percentJ1 + percentJ2 + percentJ3;
  myPercent[0]        = percentJ1 / Total;
  myPercent[1]        = percentJ2 / Total;
  myPercent[2]        = percentJ3 / Total;
}

//=================================================================================================

void AppDef_LinearCriteria::GetWeight(double& QuadraticWeight,
                                      double& QualityWeight) const
{

  QuadraticWeight = myQuadraticWeight;
  QualityWeight   = myQualityWeight;
}

//=================================================================================================

void AppDef_LinearCriteria::SetWeight(const NCollection_Array1<double>& Weight)
{
  myPntWeight = Weight;
}

//=================================================================================================

void AppDef_LinearCriteria::BuildCache(const int Element)
{
  double t;
  double UFirst, ULast;

  UFirst = myCurve->Knots()(Element);
  ULast  = myCurve->Knots()(Element + 1);

  IF = 0;
  for (int ipnt = myParameters->Lower(); ipnt <= myParameters->Upper(); ipnt++)
  {
    t = myParameters->Value(ipnt);
    if ((t > UFirst && t <= ULast) || (Element == 1 && t == UFirst))
    {
      if (IF == 0)
        IF = ipnt;
      IL = ipnt;
    }
    else if (t > ULast)
      break;
  }

  if (IF != 0)
  {
    const PLib_HermitJacobi& myBase = myCurve->Base();
    int         order  = myBase.WorkDegree() + 1;
    myCache                         = new NCollection_HArray1<double>(1, (IL - IF + 1) * (order));

    for (int ipnt = IF, ii = 1; ipnt <= IL; ipnt++, ii += order)
    {
      double*       cache = &myCache->ChangeValue(ii);
      NCollection_Array1<double> BasicValue(cache[0], 0, order - 1);
      t                   = myParameters->Value(ipnt);
      double coeff = 2. / (ULast - UFirst), c0 = -(ULast + UFirst) / 2., s;
      s = (t + c0) * coeff;
      myBase.D0(s, BasicValue);
    }
  }
  else
  { // no points in the interval.
    IF = IL;
    IL--;
  }
  myE = Element;
}
