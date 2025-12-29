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

#include "Geom_OsculatingSurface.pxx"

#include <BSplSLib.hxx>
#include <Convert_GridPolynomialToPoles.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Surface.hxx>
#include <PLib.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array2.hxx>
#include <gp_Vec.hxx>
#include <NCollection_HArray2.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

//=================================================================================================

Geom_OsculatingSurface::Geom_OsculatingSurface()
    : myTol(0.0),
      myAlong{false, false, false, false}
{
}

//=================================================================================================

Geom_OsculatingSurface::Geom_OsculatingSurface(const occ::handle<Geom_Surface>& theBS,
                                               double                           theTol)
    : myAlong{false, false, false, false}
{
  Init(theBS, theTol);
}

//=================================================================================================

Geom_OsculatingSurface::Geom_OsculatingSurface(const Geom_OsculatingSurface& theOther)

  = default;

//=================================================================================================

Geom_OsculatingSurface::Geom_OsculatingSurface(Geom_OsculatingSurface&& theOther) noexcept
    : myBasisSurf(std::move(theOther.myBasisSurf)),
      myTol(theOther.myTol),
      myOsculSurf1(std::move(theOther.myOsculSurf1)),
      myOsculSurf2(std::move(theOther.myOsculSurf2)),
      myKdeg(std::move(theOther.myKdeg)),
      myAlong(theOther.myAlong)
{
  theOther.myTol = 0.0;
  theOther.myAlong.fill(false);
}

//=================================================================================================

Geom_OsculatingSurface& Geom_OsculatingSurface::operator=(const Geom_OsculatingSurface& theOther)
{
  if (this != &theOther)
  {
    myBasisSurf  = theOther.myBasisSurf;
    myTol        = theOther.myTol;
    myOsculSurf1 = theOther.myOsculSurf1;
    myOsculSurf2 = theOther.myOsculSurf2;
    myKdeg       = theOther.myKdeg;
    myAlong      = theOther.myAlong;
  }
  return *this;
}

//=================================================================================================

Geom_OsculatingSurface& Geom_OsculatingSurface::operator=(
  Geom_OsculatingSurface&& theOther) noexcept
{
  if (this != &theOther)
  {
    myBasisSurf  = std::move(theOther.myBasisSurf);
    myTol        = theOther.myTol;
    myOsculSurf1 = std::move(theOther.myOsculSurf1);
    myOsculSurf2 = std::move(theOther.myOsculSurf2);
    myKdeg       = std::move(theOther.myKdeg);
    myAlong      = theOther.myAlong;

    theOther.myTol = 0.0;
    theOther.myAlong.fill(false);
  }
  return *this;
}

//=================================================================================================

void Geom_OsculatingSurface::Init(const occ::handle<Geom_Surface>& theBS, double theTol)
{
  clearOsculFlags();
  myTol            = theTol;
  double TolMin    = 0.; // consider all singularities below Tol, not just above 1.e-12
  bool   OsculSurf = true;
  myBasisSurf      = occ::down_cast<Geom_Surface>(theBS->Copy());
  myOsculSurf1.Clear();
  myOsculSurf2.Clear();
  myKdeg.Clear();

  if ((theBS->IsKind(STANDARD_TYPE(Geom_BSplineSurface)))
      || (theBS->IsKind(STANDARD_TYPE(Geom_BezierSurface))))
  {
    double U1 = 0, U2 = 0, V1 = 0, V2 = 0;

    int i = 1;
    theBS->Bounds(U1, U2, V1, V2);
    myAlong[0] = isQPunctual(theBS, V1, GeomAbs_IsoV, TolMin, theTol);
    myAlong[1] = isQPunctual(theBS, V2, GeomAbs_IsoV, TolMin, theTol);
    myAlong[2] = isQPunctual(theBS, U1, GeomAbs_IsoU, TolMin, theTol);
    myAlong[3] = isQPunctual(theBS, U2, GeomAbs_IsoU, TolMin, theTol);
#ifdef OCCT_DEBUG
    std::cout << myAlong[0] << std::endl
              << myAlong[1] << std::endl
              << myAlong[2] << std::endl
              << myAlong[3] << std::endl;
#endif
    if (myAlong[0] || myAlong[1] || myAlong[2] || myAlong[3])
    {
      occ::handle<Geom_BSplineSurface> InitSurf, L, S;
      if (theBS->IsKind(STANDARD_TYPE(Geom_BezierSurface)))
      {
        occ::handle<Geom_BezierSurface> BzS = occ::down_cast<Geom_BezierSurface>(theBS);
        NCollection_Array2<gp_Pnt>      P(1, BzS->NbUPoles(), 1, BzS->NbVPoles());
        NCollection_Array1<double>      UKnots(1, 2);
        NCollection_Array1<double>      VKnots(1, 2);
        NCollection_Array1<int>         UMults(1, 2);
        NCollection_Array1<int>         VMults(1, 2);
        for (i = 1; i <= 2; i++)
        {
          UKnots.SetValue(i, (i - 1));
          VKnots.SetValue(i, (i - 1));
          UMults.SetValue(i, BzS->UDegree() + 1);
          VMults.SetValue(i, BzS->VDegree() + 1);
        }
        BzS->Poles(P);
        InitSurf = new Geom_BSplineSurface(P,
                                           UKnots,
                                           VKnots,
                                           UMults,
                                           VMults,
                                           BzS->UDegree(),
                                           BzS->VDegree(),
                                           BzS->IsUPeriodic(),
                                           BzS->IsVPeriodic());
      }
      else
      {
        InitSurf = occ::down_cast<Geom_BSplineSurface>(myBasisSurf);
      }
#ifdef OCCT_DEBUG
      std::cout << "UDEG: " << InitSurf->UDegree() << std::endl;
      std::cout << "VDEG: " << InitSurf->VDegree() << std::endl;
#endif

      if (IsAlongU() && IsAlongV())
        clearOsculFlags();

      if ((IsAlongU() && InitSurf->VDegree() > 1) || (IsAlongV() && InitSurf->UDegree() > 1))
      {
        int  k = 0;
        bool IsQPunc;
        int  UKnot, VKnot;
        if (myAlong[0] || myAlong[1])
        {
          for (i = 1; i < InitSurf->NbUKnots(); i++)
          {
            if (myAlong[0])
            {
              S       = InitSurf;
              k       = 0;
              IsQPunc = true;
              UKnot   = i;
              VKnot   = 1;
              while (IsQPunc)
              {
                OsculSurf = buildOsculatingSurface(V1, UKnot, VKnot, S, L);
                if (!OsculSurf)
                  break;
                k++;
#ifdef OCCT_DEBUG
                std::cout << "1.k = " << k << std::endl;
#endif
                IsQPunc = isQPunctual(L, V1, GeomAbs_IsoV, 0., theTol);
                UKnot   = 1;
                VKnot   = 1;
                S       = L;
              }
              if (OsculSurf)
                myOsculSurf1.Append(L);
              else
                clearOsculFlags();
              if (myAlong[1] && OsculSurf)
              {
                S       = InitSurf;
                k       = 0;
                IsQPunc = true;
                UKnot   = i;
                VKnot   = InitSurf->NbVKnots() - 1;

                while (IsQPunc)
                {
                  OsculSurf = buildOsculatingSurface(V2, UKnot, VKnot, S, L);
                  if (!OsculSurf)
                    break;
                  k++;
#ifdef OCCT_DEBUG
                  std::cout << "2.k = " << k << std::endl;
#endif
                  IsQPunc = isQPunctual(L, V2, GeomAbs_IsoV, 0., theTol);
                  UKnot   = 1;
                  VKnot   = 1;
                  S       = L;
                }
                if (OsculSurf)
                {
                  myOsculSurf2.Append(L);
                  myKdeg.Append(k);
                }
              }
            }
            else
            {
              S       = InitSurf;
              k       = 0;
              IsQPunc = true;
              UKnot   = i;
              VKnot   = InitSurf->NbVKnots() - 1;
              while (IsQPunc)
              {
                OsculSurf = buildOsculatingSurface(V2, UKnot, VKnot, S, L);
                if (!OsculSurf)
                  break;
                k++;
#ifdef OCCT_DEBUG
                std::cout << "2.k = " << k << std::endl;
#endif
                IsQPunc = isQPunctual(L, V2, GeomAbs_IsoV, 0., theTol);
                UKnot   = 1;
                VKnot   = 1;
                S       = L;
              }
              if (OsculSurf)
              {
                myOsculSurf2.Append(L);
                myKdeg.Append(k);
              }
              else
                clearOsculFlags();
            }
          }
        }
        if (myAlong[2] || myAlong[3])
        {
          for (i = 1; i < InitSurf->NbVKnots(); i++)
          {
            if (myAlong[2])
            {
              S       = InitSurf;
              k       = 0;
              IsQPunc = true;
              UKnot   = 1;
              VKnot   = i;
              while (IsQPunc)
              {
                OsculSurf = buildOsculatingSurface(U1, UKnot, VKnot, S, L);
                if (!OsculSurf)
                  break;
                k++;
#ifdef OCCT_DEBUG
                std::cout << "1.k = " << k << std::endl;
#endif
                IsQPunc = isQPunctual(L, U1, GeomAbs_IsoU, 0., theTol);
                UKnot   = 1;
                VKnot   = 1;
                S       = L;
              }
              if (OsculSurf)
                myOsculSurf1.Append(L);
              else
                clearOsculFlags();
              if (myAlong[3] && OsculSurf)
              {
                S       = InitSurf;
                k       = 0;
                IsQPunc = true;
                UKnot   = InitSurf->NbUKnots() - 1;
                VKnot   = i;
                while (IsQPunc)
                {
                  OsculSurf = buildOsculatingSurface(U2, UKnot, VKnot, S, L);
                  if (!OsculSurf)
                    break;
                  k++;
#ifdef OCCT_DEBUG
                  std::cout << "2.k = " << k << std::endl;
#endif
                  IsQPunc = isQPunctual(L, U2, GeomAbs_IsoU, 0., theTol);
                  UKnot   = 1;
                  VKnot   = 1;
                  S       = L;
                }
                if (OsculSurf)
                {
                  myOsculSurf2.Append(L);
                  myKdeg.Append(k);
                }
              }
            }
            else
            {
              S       = InitSurf;
              k       = 0;
              IsQPunc = true;
              UKnot   = InitSurf->NbUKnots() - 1;
              VKnot   = i;
              while (IsQPunc)
              {
                OsculSurf = buildOsculatingSurface(U2, UKnot, VKnot, S, L);
                if (!OsculSurf)
                  break;
                k++;
#ifdef OCCT_DEBUG
                std::cout << "2.k = " << k << std::endl;
#endif
                IsQPunc = isQPunctual(L, U2, GeomAbs_IsoU, 0., theTol);
                UKnot   = 1;
                VKnot   = 1;
                S       = L;
              }
              if (OsculSurf)
              {
                myOsculSurf2.Append(L);
                myKdeg.Append(k);
              }
              else
                clearOsculFlags();
            }
          }
        }
      }
      else
      {
        clearOsculFlags();
      }
    }
  }
  else
    clearOsculFlags();
}

//=================================================================================================

bool Geom_OsculatingSurface::UOsculatingSurface(double                            theU,
                                                double                            theV,
                                                bool&                             theT,
                                                occ::handle<Geom_BSplineSurface>& theL) const
{
  bool along = false;
  if (myAlong[0] || myAlong[1])
  {
    int    NU = 1, NV = 1;
    double u1, u2, v1, v2;
    theT = false;
    myBasisSurf->Bounds(u1, u2, v1, v2);
    int  NbUK, NbVK;
    bool isToSkipSecond = false;
    if (myBasisSurf->IsKind(STANDARD_TYPE(Geom_BSplineSurface)))
    {
      occ::handle<Geom_BSplineSurface> BSur = occ::down_cast<Geom_BSplineSurface>(myBasisSurf);
      NbUK                                  = BSur->NbUKnots();
      NbVK                                  = BSur->NbVKnots();
      NCollection_Array1<double> UKnots(1, NbUK);
      NCollection_Array1<double> VKnots(1, NbVK);
      BSur->UKnots(UKnots);
      BSur->VKnots(VKnots);
      BSplCLib::Hunt(UKnots, theU, NU);
      BSplCLib::Hunt(VKnots, theV, NV);
      if (NU < 1)
        NU = 1;
      if (NU >= NbUK)
        NU = NbUK - 1;
      if (NbVK == 2 && NV == 1)
        // Need to find the closest end
        if (VKnots(NbVK) - theV > theV - VKnots(1))
          isToSkipSecond = true;
    }
    else
    {
      NU   = 1;
      NV   = 1;
      NbVK = 2;
    }

    if (myAlong[0] && NV == 1)
    {
      theL  = myOsculSurf1.Value(NU);
      along = true;
    }
    if (myAlong[1] && (NV == NbVK - 1) && !isToSkipSecond)
    {
      // theT means that derivative vector of osculating surface is opposite
      // to the original. This happens when (v-t)^k is negative, i.e.
      // difference between degrees (k) is odd and t is the last parameter
      if (myKdeg.Value(NU) % 2)
        theT = true;
      theL  = myOsculSurf2.Value(NU);
      along = true;
    }
  }
  return along;
}

//=================================================================================================

bool Geom_OsculatingSurface::VOsculatingSurface(double                            theU,
                                                double                            theV,
                                                bool&                             theT,
                                                occ::handle<Geom_BSplineSurface>& theL) const
{
  bool along = false;
  if (myAlong[2] || myAlong[3])
  {
    int    NU = 1, NV = 1;
    double u1, u2, v1, v2;
    theT = false;
    myBasisSurf->Bounds(u1, u2, v1, v2);
    int  NbUK, NbVK;
    bool isToSkipSecond = false;
    if (myBasisSurf->IsKind(STANDARD_TYPE(Geom_BSplineSurface)))
    {
      occ::handle<Geom_BSplineSurface> BSur = occ::down_cast<Geom_BSplineSurface>(myBasisSurf);
      NbUK                                  = BSur->NbUKnots();
      NbVK                                  = BSur->NbVKnots();
      NCollection_Array1<double> UKnots(1, NbUK);
      NCollection_Array1<double> VKnots(1, NbVK);
      BSur->UKnots(UKnots);
      BSur->VKnots(VKnots);
      BSplCLib::Hunt(UKnots, theU, NU);
      BSplCLib::Hunt(VKnots, theV, NV);
      if (NV < 1)
        NV = 1;
      if (NV >= NbVK)
        NV = NbVK - 1;
      if (NbUK == 2 && NU == 1)
        // Need to find the closest end
        if (UKnots(NbUK) - theU > theU - UKnots(1))
          isToSkipSecond = true;
    }
    else
    {
      NU   = 1;
      NV   = 1;
      NbUK = 2;
    }

    if (myAlong[2] && NU == 1)
    {
      theL  = myOsculSurf1.Value(NV);
      along = true;
    }
    if (myAlong[3] && (NU == NbUK - 1) && !isToSkipSecond)
    {
      if (myKdeg.Value(NV) % 2)
        theT = true;
      theL  = myOsculSurf2.Value(NV);
      along = true;
    }
  }
  return along;
}

//=================================================================================================

bool Geom_OsculatingSurface::buildOsculatingSurface(double theParam,
                                                    int    theSUKnot,
                                                    int    theSVKnot,
                                                    const occ::handle<Geom_BSplineSurface>& theBS,
                                                    occ::handle<Geom_BSplineSurface>& theBSpl) const
{
  bool OsculSurf = true;
#ifdef OCCT_DEBUG
  std::cout << "t = " << theParam << std::endl;
  std::cout << "======================================" << std::endl << std::endl;
#endif

  // for cache
  int    MinDegree, MaxDegree;
  double udeg, vdeg;
  udeg = theBS->UDegree();
  vdeg = theBS->VDegree();
  if ((IsAlongU() && vdeg <= 1) || (IsAlongV() && udeg <= 1))
  {
#ifdef OCCT_DEBUG
    std::cout << " surface osculatrice nulle " << std::endl;
#endif
    OsculSurf = false;
  }
  else
  {
    MinDegree = (int)std::min(udeg, vdeg);
    MaxDegree = (int)std::max(udeg, vdeg);

    NCollection_Array2<gp_Pnt> cachepoles(1, MaxDegree + 1, 1, MinDegree + 1);
    // end for cache

    // for polynomial grid
    int MaxUDegree, MaxVDegree;
    int UContinuity, VContinuity;

    occ::handle<NCollection_HArray2<int>> NumCoeffPerSurface =
      new NCollection_HArray2<int>(1, 1, 1, 2);
    occ::handle<NCollection_HArray1<double>> PolynomialUIntervals =
      new NCollection_HArray1<double>(1, 2);
    occ::handle<NCollection_HArray1<double>> PolynomialVIntervals =
      new NCollection_HArray1<double>(1, 2);
    occ::handle<NCollection_HArray1<double>> TrueUIntervals = new NCollection_HArray1<double>(1, 2);
    occ::handle<NCollection_HArray1<double>> TrueVIntervals = new NCollection_HArray1<double>(1, 2);
    MaxUDegree                                              = (int)udeg;
    MaxVDegree                                              = (int)vdeg;

    for (int i = 1; i <= 2; i++)
    {
      PolynomialUIntervals->ChangeValue(i) = i - 1;
      PolynomialVIntervals->ChangeValue(i) = i - 1;
      TrueUIntervals->ChangeValue(i)       = theBS->UKnot(theSUKnot + i - 1);
      TrueVIntervals->ChangeValue(i)       = theBS->VKnot(theSVKnot + i - 1);
    }

    int OscUNumCoeff = 0, OscVNumCoeff = 0;
    if (IsAlongU())
    {
#ifdef OCCT_DEBUG
      std::cout << ">>>>>>>>>>> AlongU" << std::endl;
#endif
      OscUNumCoeff = (int)udeg + 1;
      OscVNumCoeff = (int)vdeg;
    }
    if (IsAlongV())
    {
#ifdef OCCT_DEBUG
      std::cout << ">>>>>>>>>>> AlongV" << std::endl;
#endif
      OscUNumCoeff = (int)udeg;
      OscVNumCoeff = (int)vdeg + 1;
    }
    NumCoeffPerSurface->ChangeValue(1, 1) = OscUNumCoeff;
    NumCoeffPerSurface->ChangeValue(1, 2) = OscVNumCoeff;
    int nbc = NumCoeffPerSurface->Value(1, 1) * NumCoeffPerSurface->Value(1, 2) * 3;
    //
    if (nbc == 0)
    {
      return false;
    }
    //
    occ::handle<NCollection_HArray1<double>> Coefficients = new NCollection_HArray1<double>(1, nbc);
    //    end for polynomial grid

    //    building the cache
    int                        ULocalIndex, VLocalIndex;
    double                     ucacheparameter, vcacheparameter, uspanlength, vspanlength;
    NCollection_Array2<gp_Pnt> NewPoles(1, theBS->NbUPoles(), 1, theBS->NbVPoles());

    int aUfKnotsLength = theBS->NbUPoles() + theBS->UDegree() + 1;
    int aVfKnotsLength = theBS->NbVPoles() + theBS->VDegree() + 1;

    if (theBS->IsUPeriodic())
    {
      NCollection_Array1<int> aMults(1, theBS->NbUKnots());
      theBS->UMultiplicities(aMults);
      aUfKnotsLength = BSplCLib::KnotSequenceLength(aMults, theBS->UDegree(), true);
    }

    if (theBS->IsVPeriodic())
    {
      NCollection_Array1<int> aMults(1, theBS->NbVKnots());
      theBS->VMultiplicities(aMults);
      aVfKnotsLength = BSplCLib::KnotSequenceLength(aMults, theBS->VDegree(), true);
    }

    NCollection_Array1<double> UFlatKnots(1, aUfKnotsLength);
    NCollection_Array1<double> VFlatKnots(1, aVfKnotsLength);
    theBS->Poles(NewPoles);
    theBS->UKnotSequence(UFlatKnots);
    theBS->VKnotSequence(VFlatKnots);

    VLocalIndex     = 0;
    ULocalIndex     = 0;
    ucacheparameter = theBS->UKnot(theSUKnot);
    vcacheparameter = theBS->VKnot(theSVKnot);
    vspanlength     = theBS->VKnot(theSVKnot + 1) - theBS->VKnot(theSVKnot);
    uspanlength     = theBS->UKnot(theSUKnot + 1) - theBS->UKnot(theSUKnot);

    // On se ramene toujours a un parametrage tel que localement ce soit l'iso
    // u=0 ou v=0 qui soit degeneree

    bool IsVNegative = theParam > vcacheparameter + vspanlength / 2;
    bool IsUNegative = theParam > ucacheparameter + uspanlength / 2;

    if (IsAlongU() && (theParam > vcacheparameter + vspanlength / 2))
      vcacheparameter = vcacheparameter + vspanlength;
    if (IsAlongV() && (theParam > ucacheparameter + uspanlength / 2))
      ucacheparameter = ucacheparameter + uspanlength;

    BSplSLib::BuildCache(ucacheparameter,
                         vcacheparameter,
                         uspanlength,
                         vspanlength,
                         theBS->IsUPeriodic(),
                         theBS->IsVPeriodic(),
                         theBS->UDegree(),
                         theBS->VDegree(),
                         ULocalIndex,
                         VLocalIndex,
                         UFlatKnots,
                         VFlatKnots,
                         NewPoles,
                         BSplSLib::NoWeights(),
                         cachepoles,
                         BSplSLib::NoWeights());
    int                        m, n, index;
    NCollection_Array2<gp_Pnt> OscCoeff(1, OscUNumCoeff, 1, OscVNumCoeff);

    if (IsAlongU())
    {
      if (udeg > vdeg)
      {
        for (n = 1; n <= (int)udeg + 1; n++)
          for (m = 1; m <= (int)vdeg; m++)
            OscCoeff(n, m) = cachepoles(n, m + 1);
      }
      else
      {
        for (n = 1; n <= (int)udeg + 1; n++)
          for (m = 1; m <= (int)vdeg; m++)
            OscCoeff(n, m) = cachepoles(m + 1, n);
      }
      if (IsVNegative)
        PLib::VTrimming(-1, 0, OscCoeff, PLib::NoWeights2());

      index = 1;
      for (n = 1; n <= (int)udeg + 1; n++)
        for (m = 1; m <= (int)vdeg; m++)
        {
          Coefficients->ChangeValue(index++) = OscCoeff(n, m).X();
          Coefficients->ChangeValue(index++) = OscCoeff(n, m).Y();
          Coefficients->ChangeValue(index++) = OscCoeff(n, m).Z();
        }
    }

    if (IsAlongV())
    {
      if (udeg > vdeg)
      {
        for (n = 1; n <= (int)udeg; n++)
          for (m = 1; m <= (int)vdeg + 1; m++)
            OscCoeff(n, m) = cachepoles(n + 1, m);
      }
      else
      {
        for (n = 1; n <= (int)udeg; n++)
          for (m = 1; m <= (int)vdeg + 1; m++)
            OscCoeff(n, m) = cachepoles(m, n + 1);
      }
      if (IsUNegative)
        PLib::UTrimming(-1, 0, OscCoeff, PLib::NoWeights2());
      index = 1;
      for (n = 1; n <= (int)udeg; n++)
        for (m = 1; m <= (int)vdeg + 1; m++)
        {
          Coefficients->ChangeValue(index++) = OscCoeff(n, m).X();
          Coefficients->ChangeValue(index++) = OscCoeff(n, m).Y();
          Coefficients->ChangeValue(index++) = OscCoeff(n, m).Z();
        }
    }

    if (IsAlongU())
      MaxVDegree--;
    if (IsAlongV())
      MaxUDegree--;
    UContinuity = -1;
    VContinuity = -1;

    Convert_GridPolynomialToPoles Data(1,
                                       1,
                                       UContinuity,
                                       VContinuity,
                                       MaxUDegree,
                                       MaxVDegree,
                                       NumCoeffPerSurface,
                                       Coefficients,
                                       PolynomialUIntervals,
                                       PolynomialVIntervals,
                                       TrueUIntervals,
                                       TrueVIntervals);

    theBSpl = new Geom_BSplineSurface(Data.Poles()->Array2(),
                                      Data.UKnots()->Array1(),
                                      Data.VKnots()->Array1(),
                                      Data.UMultiplicities()->Array1(),
                                      Data.VMultiplicities()->Array1(),
                                      Data.UDegree(),
                                      Data.VDegree(),
                                      false,
                                      false);
#ifdef OCCT_DEBUG
    std::cout << "^====================================^" << std::endl << std::endl;
#endif
  }
  return OsculSurf;
}

//=================================================================================================

bool Geom_OsculatingSurface::isQPunctual(const occ::handle<Geom_Surface>& theS,
                                         double                           theParam,
                                         GeomAbs_IsoType                  theIT,
                                         double                           theTolMin,
                                         double                           theTolMax) const
{
  double U1 = 0, U2 = 0, V1 = 0, V2 = 0, T;
  bool   Along = true;
  theS->Bounds(U1, U2, V1, V2);
  gp_Vec D1U, D1V;
  gp_Pnt P;
  double Step, D1NormMax;
  if (theIT == GeomAbs_IsoV)
  {
    Step      = (U2 - U1) / 10;
    D1NormMax = 0.;
    for (T = U1; T <= U2; T = T + Step)
    {
      theS->D1(T, theParam, P, D1U, D1V);
      D1NormMax = std::max(D1NormMax, D1U.Magnitude());
    }

#ifdef OCCT_DEBUG
    std::cout << " D1NormMax = " << D1NormMax << std::endl;
#endif
    if (D1NormMax > theTolMax || D1NormMax < theTolMin)
      Along = false;
  }
  else
  {
    Step      = (V2 - V1) / 10;
    D1NormMax = 0.;
    for (T = V1; T <= V2; T = T + Step)
    {
      theS->D1(theParam, T, P, D1U, D1V);
      D1NormMax = std::max(D1NormMax, D1V.Magnitude());
    }
#ifdef OCCT_DEBUG
    std::cout << " D1NormMax = " << D1NormMax << std::endl;
#endif
    if (D1NormMax > theTolMax || D1NormMax < theTolMin)
      Along = false;
  }
  return Along;
}

//=================================================================================================

void Geom_OsculatingSurface::clearOsculFlags()
{
  myAlong.fill(false);
}
