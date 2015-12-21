// Created on: 1995-03-14
// Created by: Modelistation
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

#ifndef OCCT_DEBUG
#define No_Standard_OutOfRange
#define No_Standard_RangeError
#endif
#include <AppCont_LeastSquare.hxx>

#include <math.hxx>
#include <AppParCurves_MultiPoint.hxx>
#include <AppCont_ContMatrices.hxx>
#include <PLib.hxx>
#include <Precision.hxx>


//=======================================================================
//function : AppCont_LeastSquare
//purpose  : 
//=======================================================================
void AppCont_LeastSquare::FixSingleBorderPoint(const AppCont_Function&       theSSP,
                                               const Standard_Real           theU,
                                               const Standard_Real           theU0,
                                               const Standard_Real           theU1,
                                               NCollection_Array1<gp_Pnt2d>& theFix2d,
                                               NCollection_Array1<gp_Pnt>&   theFix)
{
  Standard_Integer aMaxIter = 15;
  Standard_Integer j, i2;
  NCollection_Array1<gp_Pnt>   aTabP(1, Max (myNbP, 1)),     aPrevP(1, Max (myNbP, 1));
  NCollection_Array1<gp_Pnt2d> aTabP2d(1,  Max (myNbP2d, 1)), aPrevP2d(1,  Max (myNbP2d, 1));
  Standard_Real aMult = ((theU - theU0) > (theU1 - theU)) ? 1.0: -1.0;
  Standard_Real aStartParam = theU,
                aCurrParam, aPrevDist = 1.0, aCurrDist = 1.0;

  Standard_Real du = -(theU1 - theU0) / 2.0 * aMult;
  Standard_Real eps = Epsilon(1.);
  Standard_Real dd = du, dec = .1;
  for (Standard_Integer anIter = 1; anIter < aMaxIter; anIter++)
  {
    dd *=  dec;
    aCurrParam = aStartParam + dd;
    theSSP.Value(aCurrParam, aTabP2d, aTabP);

    // from second iteration
    if (anIter > 1)
    {
      aCurrDist = 0.0;

      i2 = 1;
      for (j = 1; j <= myNbP; j++)
      {
        aCurrDist += aTabP(j).Distance(aPrevP(j));
        i2 += 3;
      }
      for (j = 1; j <= myNbP2d; j++)
      {
        aCurrDist += aTabP2d(j).Distance(aPrevP2d(j));
        i2 += 2;
      }

      // from the third iteration
      if (anIter > 2 && aCurrDist / aPrevDist > 10.0)
        break;
    }
    aPrevP = aTabP;
    aPrevP2d = aTabP2d;
    aPrevDist = aCurrDist;
    if(aPrevDist <= eps)
      break;
  }
  theFix2d = aPrevP2d;
  theFix   = aPrevP;
}


//=======================================================================
//function : AppCont_LeastSquare
//purpose  : 
//=======================================================================

AppCont_LeastSquare::AppCont_LeastSquare(const AppCont_Function&       SSP,
                                         const Standard_Real           U0,
                                         const Standard_Real           U1,
                                         const AppParCurves_Constraint FirstCons,
                                         const AppParCurves_Constraint LastCons,
                                         const Standard_Integer        Deg,
                                         const Standard_Integer        myNbPoints)
: mySCU(Deg+1),
  myPoints(1, myNbPoints, 1, 3 * SSP.GetNbOf3dPoints() + 2 * SSP.GetNbOf2dPoints()),
  myPoles(1, Deg + 1, 1, 3 * SSP.GetNbOf3dPoints() + 2 * SSP.GetNbOf2dPoints(), 0.0),
  myParam(1, myNbPoints),
  myVB(1, Deg+1, 1, myNbPoints),
  myPerInfo(1, 3 * SSP.GetNbOf3dPoints() + 2 * SSP.GetNbOf2dPoints() )
{
  myDone = Standard_False;
  myDegre = Deg;
  Standard_Integer i, j, k, c, i2;
  Standard_Integer classe = Deg + 1, cl1 = Deg;
  Standard_Real U, dU, Coeff, Coeff2;
  Standard_Real IBij, IBPij;

  Standard_Integer FirstP = 1, LastP = myNbPoints;
  Standard_Integer nbcol = 3 * SSP.GetNbOf3dPoints() + 2 * SSP.GetNbOf2dPoints();
  math_Matrix B(1, classe, 1, nbcol, 0.0);
  Standard_Integer bdeb = 1, bfin = classe;
  AppParCurves_Constraint myFirstC = FirstCons, myLastC = LastCons;
  SSP.GetNumberOfPoints(myNbP, myNbP2d);

  Standard_Integer i2plus1, i2plus2;
  myNbdiscret = myNbPoints;
  NCollection_Array1<gp_Pnt>   aTabP(1, Max (myNbP, 1));
  NCollection_Array1<gp_Pnt2d> aTabP2d(1, Max (myNbP2d, 1));
  NCollection_Array1<gp_Vec>   aTabV(1, Max (myNbP, 1));
  NCollection_Array1<gp_Vec2d> aTabV2d(1, Max (myNbP2d, 1));

  for(Standard_Integer aDimIdx = 1; aDimIdx <= myNbP * 3 + myNbP2d * 2; aDimIdx++)
  {
    SSP.PeriodInformation(aDimIdx, 
                          myPerInfo(aDimIdx).isPeriodic,
                          myPerInfo(aDimIdx).myPeriod);
  }

  Standard_Boolean Ok;
  if (myFirstC == AppParCurves_TangencyPoint) 
  {
    Ok = SSP.D1(U0, aTabV2d, aTabV);
    if (!Ok) myFirstC = AppParCurves_PassPoint;
  }

  if (myLastC == AppParCurves_TangencyPoint)
  {
    Ok = SSP.D1(U1, aTabV2d, aTabV);
    if (!Ok) myLastC = AppParCurves_PassPoint;
  }

  // Compute control points params on which approximation will be built.
  math_Vector GaussP(1, myNbPoints), GaussW(1, myNbPoints);
  math::GaussPoints(myNbPoints, GaussP);
  math::GaussWeights(myNbPoints, GaussW);
  math_Vector TheWeights(1, myNbPoints), VBParam(1, myNbPoints);
  dU = 0.5*(U1-U0);
  for (i = FirstP; i <= LastP; i++)
  {
    U  = 0.5 * (U1 + U0) + dU * GaussP(i);
    if (i <=  (myNbPoints+1)/2)
    {
      myParam(LastP - i + 1)  = U;
      VBParam(LastP - i + 1)  = 0.5 * (1 + GaussP(i));
      TheWeights(LastP - i + 1) = 0.5 * GaussW(i);
    }
    else
    {
      VBParam(i - (myNbPoints + 1) / 2)  = 0.5*(1 + GaussP(i));
      myParam(i - (myNbPoints + 1) / 2) = U;
      TheWeights(i - (myNbPoints+ 1) / 2) = 0.5 * GaussW(i);
    }
  }

  // Compute control points.
  for (i = FirstP; i <= LastP; i++)
  {
    U = myParam(i);
    SSP.Value(U, aTabP2d, aTabP);

    i2 = 1;
    for (j = 1; j <= myNbP; j++)
    {
      (aTabP(j)).Coord(myPoints(i, i2), myPoints(i, i2+1), myPoints(i, i2+2));
      i2 += 3;
    }
    for (j = 1; j <= myNbP2d; j++)
    {
      (aTabP2d(j)).Coord(myPoints(i, i2), myPoints(i, i2+1));
      i2 += 2;
    }
  }

  // Fix possible "period jump".
  Standard_Integer aMaxDim = 3 * myNbP + 2 * myNbP2d;
  for(Standard_Integer aDimIdx = 1; aDimIdx <= aMaxDim; aDimIdx++)
  {
    if (myPerInfo(aDimIdx).isPeriodic &&
        Abs (myPoints(1, aDimIdx) - myPoints(2, aDimIdx)) > myPerInfo(aDimIdx).myPeriod / 2.01 &&
        Abs (myPoints(2, aDimIdx) - myPoints(3, aDimIdx)) < myPerInfo(aDimIdx).myPeriod / 2.01)
    {
      Standard_Real aPeriodMult = (myPoints(1, aDimIdx) < myPoints(2, aDimIdx)) ? 1.0 : -1.0;
      Standard_Real aNewParam = myPoints(1, aDimIdx) + aPeriodMult * myPerInfo(aDimIdx).myPeriod;
      myPoints(1, aDimIdx) = aNewParam;
    }
  }
  for (Standard_Integer aPntIdx = 1; aPntIdx < myNbPoints; aPntIdx++)
  {
    for(Standard_Integer aDimIdx = 1; aDimIdx <= aMaxDim; aDimIdx++)
    {
      if (myPerInfo(aDimIdx).isPeriodic &&
        Abs ( myPoints(aPntIdx, aDimIdx) - myPoints(aPntIdx + 1, aDimIdx) ) > myPerInfo(aDimIdx).myPeriod / 2.01)
      {
        Standard_Real aPeriodMult = (myPoints(aPntIdx, aDimIdx) > myPoints(aPntIdx + 1, aDimIdx)) ? 1.0 : -1.0;
        Standard_Real aNewParam = myPoints(aPntIdx + 1, aDimIdx) + aPeriodMult * myPerInfo(aDimIdx).myPeriod;
        myPoints(aPntIdx + 1, aDimIdx) = aNewParam;
      }
    }
  }

  VBernstein(classe, myNbPoints, myVB);

  // Traitement du second membre:
  NCollection_Array1<Standard_Real> tmppoints(1, nbcol);

  for (c = 1; c <= classe; c++) 
  {
    tmppoints.Init(0.0);
    for (i = 1; i <= myNbPoints; i++)
    {
      Coeff = TheWeights(i) * myVB(c, i);
      for (j = 1; j <= nbcol; j++)
      {
        tmppoints(j) += myPoints(i, j)*Coeff;
      }
    }
    for (k = 1; k <= nbcol; k++)
    {
      B(c, k) += tmppoints(k);
    }
  }

  if (myFirstC == AppParCurves_NoConstraint &&
      myLastC  == AppParCurves_NoConstraint) {

    math_Matrix InvM(1, classe, 1, classe);
    InvMMatrix(classe, InvM);
    // Calcul direct des poles:

    for (i = 1; i <= classe; i++) {
      for (j = 1; j <= classe; j++) {
        IBij = InvM(i, j);
        for (k = 1; k <= nbcol; k++) {
          myPoles(i, k)   += IBij * B(j, k);
        }
      }
    }
  }


  else
  {
    math_Matrix M(1, classe, 1, classe);
    MMatrix(classe, M);
    NCollection_Array1<gp_Pnt2d> aFixP2d(1, Max (myNbP2d, 1));
    NCollection_Array1<gp_Pnt>   aFixP(1, Max (myNbP, 1));

    if (myFirstC == AppParCurves_PassPoint ||
        myFirstC == AppParCurves_TangencyPoint)
    {
      SSP.Value(U0, aTabP2d, aTabP);
      FixSingleBorderPoint(SSP, U0, U0, U1, aFixP2d, aFixP);

      i2 = 1;
      for (k = 1; k<= myNbP; k++)
      {
        if (aFixP(k).Distance(aTabP(k)) > 0.1)
          (aFixP(k)).Coord(myPoles(1, i2), myPoles(1, i2 + 1), myPoles(1, i2 + 2));
        else
          (aTabP(k)).Coord(myPoles(1, i2), myPoles(1, i2 + 1), myPoles(1, i2 + 2));
        i2 += 3;
      }
      for (k = 1; k<= myNbP2d; k++)
      {
        if (aFixP2d(k).Distance(aTabP2d(k)) > 0.1)
          (aFixP2d(k)).Coord(myPoles(1, i2), myPoles(1, i2 + 1));
        else
          (aTabP2d(k)).Coord(myPoles(1, i2), myPoles(1, i2 + 1));
        i2 += 2;
      }

      for (Standard_Integer aDimIdx = 1; aDimIdx <= aMaxDim; aDimIdx++) 
      {
        if (myPerInfo(aDimIdx).isPeriodic && 
            Abs ( myPoles(1, aDimIdx) - myPoints(1, aDimIdx) ) > myPerInfo(aDimIdx).myPeriod / 2.01 )
        {
          Standard_Real aMult = myPoles(1, aDimIdx) < myPoints(1, aDimIdx)? 1.0: -1.0;
          myPoles(1,aDimIdx) += aMult * myPerInfo(aDimIdx).myPeriod;
        }
      }
    }

    if (myLastC == AppParCurves_PassPoint ||
        myLastC == AppParCurves_TangencyPoint)
    {
      SSP.Value(U1, aTabP2d, aTabP);
      FixSingleBorderPoint(SSP, U1, U0, U1, aFixP2d, aFixP);

      i2 = 1;
      for (k = 1; k<= myNbP; k++)
      {
        if (aFixP(k).Distance(aTabP(k)) > 0.1)
          (aFixP(k)).Coord(myPoles(classe, i2), myPoles(classe, i2 + 1), myPoles(classe, i2 + 2));
        else
          (aTabP(k)).Coord(myPoles(classe, i2), myPoles(classe, i2 + 1), myPoles(classe, i2 + 2));
        i2 += 3;
      }
      for (k = 1; k<= myNbP2d; k++)
      {
        if (aFixP2d(k).Distance(aTabP2d(k)) > 0.1)
          (aFixP2d(k)).Coord(myPoles(classe, i2), myPoles(classe, i2 + 1));
        else
          (aTabP2d(k)).Coord(myPoles(classe, i2), myPoles(classe, i2 + 1));
        i2 += 2;
      }


      for (Standard_Integer aDimIdx = 1; aDimIdx <= 2; aDimIdx++) 
      {
        if (myPerInfo(aDimIdx).isPeriodic && 
          Abs ( myPoles(classe, aDimIdx) - myPoints(myNbPoints, aDimIdx) ) > myPerInfo(aDimIdx).myPeriod / 2.01 )
        {
          Standard_Real aMult = myPoles(classe, aDimIdx) < myPoints(myNbPoints, aDimIdx)? 1.0: -1.0;
          myPoles(classe,aDimIdx) += aMult * myPerInfo(aDimIdx).myPeriod;
        }
      }
    }

    if (myFirstC == AppParCurves_PassPoint) {
      bdeb = 2;
      // mise a jour du second membre:
      for (i = 1; i <= classe; i++) {
        Coeff = M(i, 1);
        for (k = 1; k <= nbcol; k++) {
          B(i, k) -= myPoles(1, k)*Coeff;
        }
      }
    }

    if (myLastC == AppParCurves_PassPoint) {
      bfin = cl1;
      for (i = 1; i <= classe; i++) {
        Coeff = M(i, classe);
        for (k = 1; k <= nbcol; k++) {
          B(i, k) -= myPoles(classe, k)*Coeff;
        }
      }
    }

    if (myFirstC == AppParCurves_TangencyPoint) {
      // On fixe le second pole::
      bdeb = 3;
      SSP.D1(U0, aTabV2d, aTabV);

      i2 = 1;
      Coeff = (U1-U0)/myDegre;
      for (k = 1; k<= myNbP; k++) {
        i2plus1 = i2+1; i2plus2 = i2+2;
        myPoles(2, i2)      = myPoles(1, i2)      + aTabV(k).X()*Coeff;
        myPoles(2, i2plus1) = myPoles(1, i2plus1) + aTabV(k).Y()*Coeff;
        myPoles(2, i2plus2) = myPoles(1, i2plus2) + aTabV(k).Z()*Coeff;
        i2 += 3;
      }
      for (k = 1; k<= myNbP2d; k++) {
        i2plus1 = i2+1;
        myPoles(2, i2)      = myPoles(1, i2)      + aTabV2d(k).X()*Coeff;
        myPoles(2, i2plus1) = myPoles(1, i2plus1) + aTabV2d(k).Y()*Coeff;
        i2 += 2;
      }

      for (i = 1; i <= classe; i++) {
        Coeff = M(i, 1); Coeff2 = M(i, 2);
        for (k = 1; k <= nbcol; k++) {
          B(i, k) -= myPoles(1, k)*Coeff+myPoles(2, k)*Coeff2;
        }
      }
    }

    if (myLastC == AppParCurves_TangencyPoint) {
      bfin = classe-2;
      SSP.D1(U1, aTabV2d, aTabV);
      i2 = 1;
      Coeff = (U1-U0)/myDegre;
      for (k = 1; k<= myNbP; k++) {
        i2plus1 = i2+1; i2plus2 = i2+2;
        myPoles(cl1,i2)      = myPoles(classe, i2)      - aTabV(k).X()*Coeff;
        myPoles(cl1,i2plus1) = myPoles(classe, i2plus1) - aTabV(k).Y()*Coeff;
        myPoles(cl1,i2plus2) = myPoles(classe, i2plus2) - aTabV(k).Z()*Coeff;
        i2 += 3;
      }
      for (k = 1; k<= myNbP2d; k++) {
        i2plus1 = i2+1; 
        myPoles(cl1,i2) = myPoles(classe, i2) - aTabV2d(k).X()*Coeff;
        myPoles(cl1,i2plus1) = myPoles(classe, i2plus1) - aTabV2d(k).Y()*Coeff;
        i2 += 2;
      }

      for (i = 1; i <= classe; i++) {
        Coeff = M(i, classe); Coeff2 = M(i, cl1);
        for (k = 1; k <= nbcol; k++) {
          B(i, k) -= myPoles(classe, k)*Coeff + myPoles(cl1, k)*Coeff2;
        }
      }
    }


    if (bdeb <= bfin) {
      math_Matrix B2(bdeb, bfin, 1, B.UpperCol(), 0.0);
      
      for (i = bdeb; i <= bfin; i++) {
        for (j = 1; j <= classe; j++) {
          Coeff = M(i, j);
          for (k = 1; k <= nbcol; k++) {
            B2(i, k) += B(j, k)*Coeff;
          }
        }
      }

      // Resolution:
      // ===========
      math_Matrix IBP(bdeb, bfin, bdeb, bfin);

      // dans IBPMatrix at IBTMatrix ne sont stockees que les resultats pour
      // une classe inferieure ou egale a 26 (pour l instant du moins.)

      if (bdeb == 2 && bfin == classe-1 && classe <= 26) {
        IBPMatrix(classe, IBP);
      }
      else if (bdeb == 3 && bfin == classe-2 && classe <= 26) {
        IBTMatrix(classe, IBP);
      }
      else {
        math_Matrix MP(1, classe, bdeb, bfin);
        for (i = 1; i <= classe; i++) {
          for (j = bdeb; j <= bfin; j++) {
            MP(i, j) = M(i, j);
          }
        }
        math_Matrix IBP1(bdeb, bfin, bdeb, bfin);
        IBP1 = MP.Transposed()*MP;
        IBP = IBP1.Inverse();
      }

      myDone = Standard_True;
      for (i = bdeb; i <= bfin; i++) {
        for (j = bdeb; j <= bfin; j++) {
          IBPij = IBP(i, j);;
          for (k = 1; k<= nbcol; k++) {
            myPoles(i, k)   += IBPij * B2(j, k);
          }
        }
      }
    }
  }
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

const AppParCurves_MultiCurve& AppCont_LeastSquare::Value() 
{

  Standard_Integer i, j, j2;
  gp_Pnt Pt;
  gp_Pnt2d Pt2d;
  Standard_Integer ideb = 1, ifin = myDegre+1;

  // On met le resultat dans les curves correspondantes
  for (i = ideb; i <= ifin; i++) {
    j2 = 1;
    AppParCurves_MultiPoint MPole(myNbP, myNbP2d);
    for (j = 1; j <= myNbP; j++) {
      Pt.SetCoord(myPoles(i, j2), myPoles(i, j2+1), myPoles(i,j2+2));
      MPole.SetPoint(j, Pt);
      j2 += 3;
    }
    for (j = myNbP+1;j <= myNbP+myNbP2d; j++) {
      Pt2d.SetCoord(myPoles(i, j2), myPoles(i, j2+1));
      MPole.SetPoint2d(j, Pt2d);
      j2 += 2;
    }
    mySCU.SetValue(i, MPole);
  }
  return mySCU;
}



//=======================================================================
//function : Error
//purpose  : 
//=======================================================================

void AppCont_LeastSquare::Error(Standard_Real& F, 
                                Standard_Real& MaxE3d,
                                Standard_Real& MaxE2d) const
{
  Standard_Integer i, j, k, c, i2, classe = myDegre + 1;
  Standard_Real Coeff, err3d = 0.0, err2d = 0.0;
  Standard_Integer ncol = myPoints.UpperCol() - myPoints.LowerCol() + 1;

  math_Matrix MyPoints(1, myNbdiscret, 1, ncol);
  MyPoints = myPoints;

  MaxE3d = MaxE2d = F = 0.0;

  NCollection_Array1<Standard_Real> tmppoles(1, ncol);

  for (c = 1; c <= classe; c++)
  {
    for (k = 1; k <= ncol; k++)
    {
      tmppoles(k) = myPoles(c, k);
    }
    for (i = 1; i <= myNbdiscret; i++)
    {
      Coeff = myVB(c, i);
      for (j = 1; j <= ncol; j++)
      {
        MyPoints(i, j) -= tmppoles(j) * Coeff;
      }
    }
  }

  Standard_Real e1, e2, e3;
  for (i = 1; i <= myNbdiscret; i++)
  {
    i2 = 1;
    for (j = 1; j<= myNbP; j++) {
      e1 = MyPoints(i, i2);
      e2 = MyPoints(i, i2+1);
      e3 = MyPoints(i, i2+2);
      err3d = e1*e1+e2*e2+e3*e3;
      MaxE3d = Max(MaxE3d, err3d);
      F += err3d;
      i2 += 3;
    }
    for (j = 1; j<= myNbP2d; j++) {
      e1 = MyPoints(i, i2);
      e2 = MyPoints(i, i2+1);
      err2d = e1*e1+e2*e2;
      MaxE2d = Max(MaxE2d, err2d);
      F += err2d;
      i2 += 2;
    }
  }

  MaxE3d = Sqrt(MaxE3d);
  MaxE2d = Sqrt(MaxE2d);

}


//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean AppCont_LeastSquare::IsDone() const
{
  return myDone;
}
