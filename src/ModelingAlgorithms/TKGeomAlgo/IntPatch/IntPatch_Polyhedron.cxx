// Created on: 1993-02-03
// Created by: Laurent BUCHARD
// Copyright (c) 1993-1999 Matra Datavision
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
#include <GeomGridEval_Surface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <IntCurveSurface_PolyhedronUtils.pxx>
#include <IntPatch_HInterTool.hxx>
#include <IntPatch_Polyhedron.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>

#include <cstdio>
#define MSG_DEBUG 0

#define LONGUEUR_MINI_EDGE_TRIANGLE 1e-14
#define DEFLECTION_COEFF 1.1
#define NBMAXUV 30

namespace PolyUtils = IntCurveSurface_PolyhedronUtils;

//================================================================================
static int NbPOnU(const occ::handle<Adaptor3d_Surface>& S)
{
  const double u0   = S->FirstUParameter();
  const double u1   = S->LastUParameter();
  const int    nbpu = IntPatch_HInterTool::NbSamplesU(S, u0, u1);
  return (nbpu > NBMAXUV ? NBMAXUV : nbpu);
}

//================================================================================
static int NbPOnV(const occ::handle<Adaptor3d_Surface>& S)
{
  const double v0   = S->FirstVParameter();
  const double v1   = S->LastVParameter();
  const int    nbpv = IntPatch_HInterTool::NbSamplesV(S, v0, v1);
  return (nbpv > NBMAXUV ? NBMAXUV : nbpv);
}

//=================================================================================================

void IntPatch_Polyhedron::Destroy()
{
  gp_Pnt* CMyPnts = (gp_Pnt*)C_MyPnts;
  delete[] CMyPnts;
  double* CMyU = (double*)C_MyU;
  delete[] CMyU;
  double* CMyV = (double*)C_MyV;
  delete[] CMyV;
  C_MyPnts = C_MyU = C_MyV = nullptr;
}

//=================================================================================================

IntPatch_Polyhedron::IntPatch_Polyhedron(const occ::handle<Adaptor3d_Surface>& Surface)
    : TheDeflection(Epsilon(100.)),
      nbdeltaU(NbPOnU(Surface)),
      nbdeltaV(NbPOnV(Surface)),
      C_MyPnts(nullptr),
      C_MyU(nullptr),
      C_MyV(nullptr),
      UMinSingular(IntPatch_HInterTool::SingularOnVMin(Surface)),
      UMaxSingular(IntPatch_HInterTool::SingularOnVMin(Surface)),
      VMinSingular(IntPatch_HInterTool::SingularOnVMin(Surface)),
      VMaxSingular(IntPatch_HInterTool::SingularOnVMin(Surface))
{
  const int t       = (nbdeltaU + 1) * (nbdeltaV + 1) + 1;
  gp_Pnt*   CMyPnts = new gp_Pnt[t];
  double*   CMyU    = new double[t];
  double*   CMyV    = new double[t];
  C_MyPnts          = CMyPnts;
  C_MyU             = CMyU;
  C_MyV             = CMyV;

  const double u0 = Surface->FirstUParameter();
  const double u1 = Surface->LastUParameter();
  const double v0 = Surface->FirstVParameter();
  const double v1 = Surface->LastVParameter();

  // Build UV parameter arrays
  NCollection_Array1<double> aUParams(0, nbdeltaU);
  NCollection_Array1<double> aVParams(0, nbdeltaV);
  const double               U1mU0sNbdeltaU = (u1 - u0) / (double)nbdeltaU;
  const double               V1mV0sNbdeltaV = (v1 - v0) / (double)nbdeltaV;

  for (int i = 0; i <= nbdeltaU; ++i)
  {
    aUParams.SetValue(i, u0 + i * U1mU0sNbdeltaU);
  }
  for (int j = 0; j <= nbdeltaV; ++j)
  {
    aVParams.SetValue(j, v0 + j * V1mV0sNbdeltaV);
  }

  // Use grid evaluator for batch point evaluation
  GeomGridEval_Surface anEval;
  anEval.Initialize(*Surface);
  NCollection_Array2<gp_Pnt> aGridPnts = anEval.EvaluateGrid(aUParams, aVParams);

  // Copy to internal arrays and build bounding box
  int Index = 1;
  for (int i1 = 0; i1 <= nbdeltaU; ++i1)
  {
    for (int i2 = 0; i2 <= nbdeltaV; ++i2)
    {
      CMyPnts[Index] = aGridPnts.Value(i1 + 1, i2 + 1);
      CMyU[Index]    = aUParams.Value(i1);
      CMyV[Index]    = aVParams.Value(i2);
      TheBnd.Add(CMyPnts[Index]);
      Index++;
    }
  }

  // Compute max deflection using surface adaptor
  double tol = PolyUtils::ComputeMaxDeflection(*Surface, *this, NbTriangles());
  tol *= DEFLECTION_COEFF;

  DeflectionOverEstimation(tol);
  FillBounding();
}

//=================================================================================================

IntPatch_Polyhedron::IntPatch_Polyhedron(const occ::handle<Adaptor3d_Surface>& Surface,
                                         const int                             nbu,
                                         const int                             nbv)
    : TheDeflection(Epsilon(100.)),
      nbdeltaU(nbu),
      nbdeltaV(nbv),
      C_MyPnts(nullptr),
      C_MyU(nullptr),
      C_MyV(nullptr),
      UMinSingular(IntPatch_HInterTool::SingularOnVMin(Surface)),
      UMaxSingular(IntPatch_HInterTool::SingularOnVMin(Surface)),
      VMinSingular(IntPatch_HInterTool::SingularOnVMin(Surface)),
      VMaxSingular(IntPatch_HInterTool::SingularOnVMin(Surface))
{
  const int t       = (nbdeltaU + 1) * (nbdeltaV + 1) + 1;
  gp_Pnt*   CMyPnts = new gp_Pnt[t];
  double*   CMyU    = new double[t];
  double*   CMyV    = new double[t];
  C_MyPnts          = CMyPnts;
  C_MyU             = CMyU;
  C_MyV             = CMyV;

  const double u0 = Surface->FirstUParameter();
  const double u1 = Surface->LastUParameter();
  const double v0 = Surface->FirstVParameter();
  const double v1 = Surface->LastVParameter();

  // Build UV parameter arrays
  NCollection_Array1<double> aUParams(0, nbdeltaU);
  NCollection_Array1<double> aVParams(0, nbdeltaV);
  const double               U1mU0sNbdeltaU = (u1 - u0) / (double)nbdeltaU;
  const double               V1mV0sNbdeltaV = (v1 - v0) / (double)nbdeltaV;

  for (int i = 0; i <= nbdeltaU; ++i)
  {
    aUParams.SetValue(i, u0 + i * U1mU0sNbdeltaU);
  }
  for (int j = 0; j <= nbdeltaV; ++j)
  {
    aVParams.SetValue(j, v0 + j * V1mV0sNbdeltaV);
  }

  // Use grid evaluator for batch point evaluation
  GeomGridEval_Surface anEval;
  anEval.Initialize(*Surface);
  NCollection_Array2<gp_Pnt> aGridPnts = anEval.EvaluateGrid(aUParams, aVParams);

  // Copy to internal arrays and build bounding box
  int Index = 1;
  for (int i1 = 0; i1 <= nbdeltaU; ++i1)
  {
    for (int i2 = 0; i2 <= nbdeltaV; ++i2)
    {
      CMyPnts[Index] = aGridPnts.Value(i1 + 1, i2 + 1);
      CMyU[Index]    = aUParams.Value(i1);
      CMyV[Index]    = aVParams.Value(i2);
      TheBnd.Add(CMyPnts[Index]);
      Index++;
    }
  }

  // Compute max deflection using surface adaptor
  double tol = PolyUtils::ComputeMaxDeflection(*Surface, *this, NbTriangles());
  tol *= DEFLECTION_COEFF;

  DeflectionOverEstimation(tol);
  FillBounding();
}

//=================================================================================================

void IntPatch_Polyhedron::Parameters(const int Index, double& U, double& V) const
{
  U = ((double*)C_MyU)[Index];
  V = ((double*)C_MyV)[Index];
}

//=================================================================================================

void IntPatch_Polyhedron::DeflectionOverEstimation(const double flec)
{
  if (flec < 0.0001)
  {
    TheDeflection = 0.0001;
    TheBnd.Enlarge(0.0001);
  }
  else
  {
    TheDeflection = flec;
    TheBnd.Enlarge(flec);
  }
}

//=================================================================================================

double IntPatch_Polyhedron::DeflectionOverEstimation() const
{
  return TheDeflection;
}

//=================================================================================================

const Bnd_Box& IntPatch_Polyhedron::Bounding() const
{
  return TheBnd;
}

//=================================================================================================

void IntPatch_Polyhedron::FillBounding()
{
  TheComponentsBnd = new NCollection_HArray1<Bnd_Box>(1, NbTriangles());
  Bnd_Box Boite;
  int     p1, p2, p3;
  int     nbtriangles = NbTriangles();
  for (int iTri = 1; iTri <= nbtriangles; iTri++)
  {
    Triangle(iTri, p1, p2, p3);
    Boite.SetVoid();
    const gp_Pnt& P1 = Point(p1);
    const gp_Pnt& P2 = Point(p2);
    const gp_Pnt& P3 = Point(p3);
    if (P1.SquareDistance(P2) > LONGUEUR_MINI_EDGE_TRIANGLE)
    {
      if (P1.SquareDistance(P3) > LONGUEUR_MINI_EDGE_TRIANGLE)
      {
        if (P2.SquareDistance(P3) > LONGUEUR_MINI_EDGE_TRIANGLE)
        {
          Boite.Add(P1);
          Boite.Add(P2);
          Boite.Add(P3);
        }
      }
    }
    Boite.Enlarge(TheDeflection);
    TheComponentsBnd->SetValue(iTri, Boite);
  }
}

//=================================================================================================

const occ::handle<NCollection_HArray1<Bnd_Box>>& IntPatch_Polyhedron::ComponentsBounding() const
{
  return TheComponentsBnd;
}

//=================================================================================================

int IntPatch_Polyhedron::NbTriangles() const
{
  return nbdeltaU * nbdeltaV * 2;
}

//=================================================================================================

int IntPatch_Polyhedron::NbPoints() const
{
  return (nbdeltaU + 1) * (nbdeltaV + 1);
}

//=================================================================================================

int IntPatch_Polyhedron::TriConnex(const int Triang,
                                   const int Pivot,
                                   const int Pedge,
                                   int&      TriCon,
                                   int&      OtherP) const
{

  int Pivotm1    = Pivot - 1;
  int nbdeltaVp1 = nbdeltaV + 1;
  int nbdeltaVm2 = nbdeltaV + nbdeltaV;

  // Pivot position in the MaTriangle :
  int ligP = Pivotm1 / nbdeltaVp1;
  int colP = Pivotm1 - ligP * nbdeltaVp1;

  // Point sur Edge position in the MaTriangle and edge typ :
  int ligE = 0, colE = 0, typE = 0;
  if (Pedge != 0)
  {
    ligE = (Pedge - 1) / nbdeltaVp1;
    colE = (Pedge - 1) - (ligE * nbdeltaVp1);
    // Horizontal
    if (ligP == ligE)
      typE = 1;
    // Vertical
    else if (colP == colE)
      typE = 2;
    // Oblique
    else
      typE = 3;
  }
  else
  {
    typE = 0;
  }

  // Triangle position General case :
  int linT = 0, colT = 0;
  int linO = 0, colO = 0;
  int t, tt;
  if (Triang != 0)
  {
    t    = (Triang - 1) / (nbdeltaVm2);
    tt   = (Triang - 1) - t * nbdeltaVm2;
    linT = 1 + t;
    colT = 1 + tt;
    if (typE == 0)
    {
      if (ligP == linT)
      {
        ligE = ligP - 1;
        colE = colP - 1;
        typE = 3;
      }
      else
      {
        if (colT == ligP + ligP)
        {
          ligE = ligP;
          colE = colP - 1;
          typE = 1;
        }
        else
        {
          ligE = ligP + 1;
          colE = colP + 1;
          typE = 3;
        }
      }
    }
    switch (typE)
    {
      case 1: // Horizontal
        if (linT == ligP)
        {
          linT++;
          linO = ligP + 1;
          colO = (colP > colE) ? colP : colE; //--colO=Max(colP, colE);
        }
        else
        {
          linT--;
          linO = ligP - 1;
          colO = (colP < colE) ? colP : colE; //--colO=Min(colP, colE);
        }
        break;
      case 2: // Vertical
        if (colT == (colP + colP))
        {
          colT++;
          linO = (ligP > ligE) ? ligP : ligE; //--linO=Max(ligP, ligE);
          colO = colP + 1;
        }
        else
        {
          colT--;
          linO = (ligP < ligE) ? ligP : ligE; //--linO=Min(ligP, ligE);
          colO = colP - 1;
        }
        break;
      case 3: // Oblique
        if ((colT & 1) == 0)
        {
          colT--;
          linO = (ligP > ligE) ? ligP : ligE; //--linO=Max(ligP, ligE);
          colO = (colP < colE) ? colP : colE; //--colO=Min(colP, colE);
        }
        else
        {
          colT++;
          linO = (ligP < ligE) ? ligP : ligE; //--linO=Min(ligP, ligE);
          colO = (colP > colE) ? colP : colE; //--colO=Max(colP, colE);
        }
        break;
    }
  }
  else
  {
    // Unknown Triangle position :
    if (Pedge == 0)
    {
      // Unknown edge :
      linT = (1 > ligP) ? 1 : ligP;                   //--linT=Max(1, ligP);
      colT = (1 > (colP + colP)) ? 1 : (colP + colP); //--colT=Max(1, colP+colP);
      if (ligP == 0)
        linO = ligP + 1;
      else
        linO = ligP - 1;
      colO = colP;
    }
    else
    {
      // Known edge We take the left or down connectivity :
      switch (typE)
      {
        case 1: // Horizontal
          linT = ligP + 1;
          colT = (colP > colE) ? colP : colE; //--colT=Max(colP,colE);
          colT += colT;
          linO = ligP + 1;
          colO = (colP > colE) ? colP : colE; //--colO=Max(colP,colE);
          break;
        case 2:                               // Vertical
          linT = (ligP > ligE) ? ligP : ligE; //--linT=Max(ligP, ligE);
          colT = colP + colP;
          linO = (ligP < ligE) ? ligP : ligE; //--linO=Min(ligP, ligE);
          colO = colP - 1;
          break;
        case 3:                               // Oblique
          linT = (ligP > ligE) ? ligP : ligE; //--linT=Max(ligP, ligE);
          colT = colP + colE;
          linO = (ligP > ligE) ? ligP : ligE; //--linO=Max(ligP, ligE);
          colO = (colP < colE) ? colP : colE; //--colO=Min(colP, colE);
          break;
      }
    }
  }

  TriCon = (linT - 1) * nbdeltaVm2 + colT;

  if (linT < 1)
  {
    linO = 0;
    colO = colP + colP - colE;
    if (colO < 0)
    {
      colO = 0;
      linO = 1;
    }
    else if (colO > nbdeltaV)
    {
      colO = nbdeltaV;
      linO = 1;
    }
    TriCon = 0;
  }
  else if (linT > nbdeltaU)
  {
    linO = nbdeltaU;
    colO = colP + colP - colE;
    if (colO < 0)
    {
      colO = 0;
      linO = nbdeltaU - 1;
    }
    else if (colO > nbdeltaV)
    {
      colO = nbdeltaV;
      linO = nbdeltaU - 1;
    }
    TriCon = 0;
  }

  if (colT < 1)
  {
    colO = 0;
    linO = ligP + ligP - ligE;
    if (linO < 0)
    {
      linO = 0;
      colO = 1;
    }
    else if (linO > nbdeltaU)
    {
      linO = nbdeltaU;
      colO = 1;
    }
    TriCon = 0;
  }
  else if (colT > nbdeltaV)
  {
    colO = nbdeltaV;
    linO = ligP + ligP - ligE;
    if (linO < 0)
    {
      linO = 0;
      colO = nbdeltaV - 1;
    }
    else if (linO > nbdeltaU)
    {
      linO = nbdeltaU;
      colO = nbdeltaV - 1;
    }
    TriCon = 0;
  }

  OtherP = linO * nbdeltaVp1 + colO + 1;

  //----------------------------------------------------
  //-- Detection des cas ou le triangle retourne est
  //-- invalide. Dans ce cas, on retourne le triangle
  //-- suivant par un nouvel appel a TriConnex.
  //--
  //-- Si En entree : Point(Pivot)==Point(Pedge)
  //-- Alors on retourne OtherP a 0
  //-- et Tricon = Triangle
  //--
  if (Point(Pivot).SquareDistance(Point(Pedge)) <= LONGUEUR_MINI_EDGE_TRIANGLE)
  {
    OtherP = 0;
    TriCon = Triang;
#if MSG_DEBUG
    std::cout << " Probleme ds IntCurveSurface_Polyhedron : Pivot et PEdge Confondus " << std::endl;
#endif
    return (TriCon);
  }
  if (Point(OtherP).SquareDistance(Point(Pedge)) <= LONGUEUR_MINI_EDGE_TRIANGLE)
  {
#if MSG_DEBUG
    std::cout << " Probleme ds IntCurveSurface_Polyhedron : OtherP et PEdge Confondus "
              << std::endl;
#endif
    return (0); //-- BUG NON CORRIGE ( a revoir le role de nbdeltaU et nbdeltaV)
                //    int TempTri,TempOtherP;
                //    TempTri = TriCon;
                //    TempOtherP = OtherP;
                //    return(TriConnex(TempTri,Pivot,TempOtherP,TriCon,OtherP));
  }
  return TriCon;
}

//=================================================================================================

void IntPatch_Polyhedron::PlaneEquation(const int Triang,
                                        gp_XYZ&   NormalVector,
                                        double&   PolarDistance) const
{
  int i1, i2, i3;
  Triangle(Triang, i1, i2, i3);

  gp_XYZ Pointi1(Point(i1).XYZ());
  gp_XYZ Pointi2(Point(i2).XYZ());
  gp_XYZ Pointi3(Point(i3).XYZ());

  gp_XYZ v1 = Pointi2 - Pointi1;
  gp_XYZ v2 = Pointi3 - Pointi2;
  gp_XYZ v3 = Pointi1 - Pointi3;

  if (v1.SquareModulus() <= LONGUEUR_MINI_EDGE_TRIANGLE)
  {
    NormalVector.SetCoord(1.0, 0.0, 0.0);
    return;
  }
  if (v2.SquareModulus() <= LONGUEUR_MINI_EDGE_TRIANGLE)
  {
    NormalVector.SetCoord(1.0, 0.0, 0.0);
    return;
  }
  if (v3.SquareModulus() <= LONGUEUR_MINI_EDGE_TRIANGLE)
  {
    NormalVector.SetCoord(1.0, 0.0, 0.0);
    return;
  }

  NormalVector    = (v1 ^ v2) + (v2 ^ v3) + (v3 ^ v1);
  double aNormLen = NormalVector.Modulus();
  if (aNormLen < gp::Resolution())
  {
    PolarDistance = 0.;
  }
  else
  {
    NormalVector.Divide(aNormLen);
    PolarDistance = NormalVector * Point(i1).XYZ();
  }
}

//=================================================================================================

bool IntPatch_Polyhedron::Contain(const int Triang, const gp_Pnt& ThePnt) const
{
  int i1, i2, i3;
  Triangle(Triang, i1, i2, i3);
  gp_XYZ Pointi1(Point(i1).XYZ());
  gp_XYZ Pointi2(Point(i2).XYZ());
  gp_XYZ Pointi3(Point(i3).XYZ());

  gp_XYZ v1 = (Pointi2 - Pointi1) ^ (ThePnt.XYZ() - Pointi1);
  gp_XYZ v2 = (Pointi3 - Pointi2) ^ (ThePnt.XYZ() - Pointi2);
  gp_XYZ v3 = (Pointi1 - Pointi3) ^ (ThePnt.XYZ() - Pointi3);
  return v1 * v2 >= 0. && v2 * v3 >= 0. && v3 * v1 >= 0.;
}

//=================================================================================================

void IntPatch_Polyhedron::Dump() const {}

//=================================================================================================

void IntPatch_Polyhedron::Size(int& nbdu, int& nbdv) const
{
  nbdu = nbdeltaU;
  nbdv = nbdeltaV;
}

//=================================================================================================

void IntPatch_Polyhedron::Triangle(const int Index, int& P1, int& P2, int& P3) const
{
  int line   = 1 + ((Index - 1) / (nbdeltaV * 2));
  int colon  = 1 + ((Index - 1) % (nbdeltaV * 2));
  int colpnt = (colon + 1) / 2;

  // General formula = (line-1)*(nbdeltaV+1)+colpnt

  //  Position of P1 = MesXYZ(line,colpnt);
  P1 = (line - 1) * (nbdeltaV + 1) + colpnt;

  //  Position of P2= MesXYZ(line+1,colpnt+((colon-1)%2));
  P2 = line * (nbdeltaV + 1) + colpnt + ((colon - 1) % 2);

  //  Position of P3= MesXYZ(line+(colon%2),colpnt+1);
  P3 = (line - 1 + (colon % 2)) * (nbdeltaV + 1) + colpnt + 1;
  //-- printf("\nTriangle %4d    P1:%4d   P2:%4d   P3:%4d",Index,P1,P2,P3);
}

//=======================================================================
// function : Point
//=======================================================================
const gp_Pnt& IntPatch_Polyhedron::Point(const int Index, double& U, double& V) const
{
  gp_Pnt* CMyPnts = (gp_Pnt*)C_MyPnts;
  double* CMyU    = (double*)C_MyU;
  double* CMyV    = (double*)C_MyV;
  U               = CMyU[Index];
  V               = CMyV[Index];
  return CMyPnts[Index];
}

//=======================================================================
// function : Point
//=======================================================================
const gp_Pnt& IntPatch_Polyhedron::Point(const int Index) const
{
  gp_Pnt* CMyPnts = (gp_Pnt*)C_MyPnts;
  return CMyPnts[Index];
}

//=======================================================================
// function : Point
//=======================================================================
void IntPatch_Polyhedron::Point(const gp_Pnt& /*p*/,
                                const int /*lig*/,
                                const int /*col*/,
                                const double /*u*/,
                                const double /*v*/)
{
  // printf("\n IntPatch_Polyhedron::Point : Ne dois pas etre appelle\n");
}

//=======================================================================
// function : Point
//=======================================================================
void IntPatch_Polyhedron::Point(const int Index, gp_Pnt& P) const
{
  gp_Pnt* CMyPnts = (gp_Pnt*)C_MyPnts;
  P               = CMyPnts[Index];
}

//=======================================================================
