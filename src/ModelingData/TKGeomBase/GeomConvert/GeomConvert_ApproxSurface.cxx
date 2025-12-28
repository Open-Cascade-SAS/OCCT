// Copyright (c) 1997-1999 Matra Datavision
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

#include <GeomConvert_ApproxSurface.hxx>

#include <Adaptor3d_Surface.hxx>
#include <AdvApp2Var_ApproxAFunc2Var.hxx>
#include <AdvApprox_PrefAndRec.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Precision.hxx>
#include <Standard_Real.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>

class GeomConvert_ApproxSurface_Eval : public AdvApp2Var_EvaluatorFunc2Var
{

public:
  GeomConvert_ApproxSurface_Eval(const occ::handle<Adaptor3d_Surface>& theAdaptor)
      : myAdaptor(theAdaptor)
  {
  }

  virtual void Evaluate(int* theDimension,
                        double*    theUStartEnd,
                        double*    theVStartEnd,
                        int* theFavorIso,
                        double*    theConstParam,
                        int* theNbParams,
                        double*    theParameters,
                        int* theUOrder,
                        int* theVOrder,
                        double*    theResult,
                        int* theErrorCode) const;

private:
  mutable occ::handle<Adaptor3d_Surface> myAdaptor;
};

void GeomConvert_ApproxSurface_Eval::Evaluate(int* Dimension,
                                              // Dimension
                                              double* UStartEnd,
                                              // StartEnd[2] in U
                                              double* VStartEnd,
                                              // StartEnd[2] in V
                                              int* FavorIso,
                                              // Choice of constante, 1 for U, 2 for V
                                              double* ConstParam,
                                              // Value of constant parameter
                                              int* NbParams,
                                              // Number of parameters N
                                              double* Parameters,
                                              // Values of parameters,
                                              int* UOrder,
                                              // Derivative Request in U
                                              int* VOrder,
                                              // Derivative Request in V
                                              double* Result,
                                              // Result[Dimension,N]
                                              int* ErrorCode) const
// Error Code
{
  *ErrorCode = 0;
  //  int idim;
  int jpar;
  double    Upar, Vpar;

  // Dimension incorrecte
  if (*Dimension != 3)
  {
    *ErrorCode = 1;
  }

  // Parametres incorrects
  /* if (*FavorIso==1) {
      Upar = *ConstParam;
      if (( Upar < UStartEnd[0] ) || ( Upar > UStartEnd[1] )) {
        *ErrorCode = 2;
      }
      for (jpar=1;jpar<=*NbParams;jpar++) {
        Vpar = Parameters[jpar-1];
        if (( Vpar < VStartEnd[0] ) || ( Vpar > VStartEnd[1] )) {
          *ErrorCode = 2;
        }
      }
   }
   else {
      Vpar = *ConstParam;
      if (( Vpar < VStartEnd[0] ) || ( Vpar > VStartEnd[1] )) {
        *ErrorCode = 2;
      }
      for (jpar=1;jpar<=*NbParams;jpar++) {
        Upar = Parameters[jpar-1];
        if (( Upar < UStartEnd[0] ) || ( Upar > UStartEnd[1] )) {
          *ErrorCode = 2;
        }
      }
   }*/

  // Initialisation

  myAdaptor = myAdaptor->UTrim(UStartEnd[0], UStartEnd[1], Precision::PConfusion());
  myAdaptor = myAdaptor->VTrim(VStartEnd[0], VStartEnd[1], Precision::PConfusion());
  /*
    for (idim=1;idim<=*Dimension;idim++) {
      for (jpar=1;jpar<=*NbParams;jpar++) {
        Result[idim-1+(jpar-1)*(*Dimension)] = 0.;
      }
    }*/

  int Order = *UOrder + *VOrder;
  gp_Pnt           pnt;
  gp_Vec           vect, v1, v2, v3, v4, v5, v6, v7, v8, v9;

  if (*FavorIso == 1)
  {
    Upar = *ConstParam;
    switch (Order)
    {
      case 0:
        for (jpar = 1; jpar <= *NbParams; jpar++)
        {
          Vpar                                  = Parameters[jpar - 1];
          pnt                                   = myAdaptor->Value(Upar, Vpar);
          Result[(jpar - 1) * (*Dimension)]     = pnt.X();
          Result[1 + (jpar - 1) * (*Dimension)] = pnt.Y();
          Result[2 + (jpar - 1) * (*Dimension)] = pnt.Z();
        }
        break;
      case 1:
        for (jpar = 1; jpar <= *NbParams; jpar++)
        {
          Vpar = Parameters[jpar - 1];
          myAdaptor->D1(Upar, Vpar, pnt, v1, v2);
          if (*UOrder == 1)
          {
            Result[(jpar - 1) * (*Dimension)]     = v1.X();
            Result[1 + (jpar - 1) * (*Dimension)] = v1.Y();
            Result[2 + (jpar - 1) * (*Dimension)] = v1.Z();
          }
          else
          {
            Result[(jpar - 1) * (*Dimension)]     = v2.X();
            Result[1 + (jpar - 1) * (*Dimension)] = v2.Y();
            Result[2 + (jpar - 1) * (*Dimension)] = v2.Z();
          }
        }
        break;
      case 2:
        for (jpar = 1; jpar <= *NbParams; jpar++)
        {
          Vpar = Parameters[jpar - 1];
          myAdaptor->D2(Upar, Vpar, pnt, v1, v2, v3, v4, v5);
          if (*UOrder == 2)
          {
            Result[(jpar - 1) * (*Dimension)]     = v3.X();
            Result[1 + (jpar - 1) * (*Dimension)] = v3.Y();
            Result[2 + (jpar - 1) * (*Dimension)] = v3.Z();
          }
          else if (*UOrder == 1)
          {
            Result[(jpar - 1) * (*Dimension)]     = v5.X();
            Result[1 + (jpar - 1) * (*Dimension)] = v5.Y();
            Result[2 + (jpar - 1) * (*Dimension)] = v5.Z();
          }
          else if (*UOrder == 0)
          {
            Result[(jpar - 1) * (*Dimension)]     = v4.X();
            Result[1 + (jpar - 1) * (*Dimension)] = v4.Y();
            Result[2 + (jpar - 1) * (*Dimension)] = v4.Z();
          }
        }
        break;
      case 3:
        for (jpar = 1; jpar <= *NbParams; jpar++)
        {
          Vpar = Parameters[jpar - 1];
          myAdaptor->D3(Upar, Vpar, pnt, v1, v2, v3, v4, v5, v6, v7, v8, v9);
          if (*UOrder == 2)
          {
            Result[(jpar - 1) * (*Dimension)]     = v8.X();
            Result[1 + (jpar - 1) * (*Dimension)] = v8.Y();
            Result[2 + (jpar - 1) * (*Dimension)] = v8.Z();
          }
          else if (*UOrder == 1)
          {
            Result[(jpar - 1) * (*Dimension)]     = v9.X();
            Result[1 + (jpar - 1) * (*Dimension)] = v9.Y();
            Result[2 + (jpar - 1) * (*Dimension)] = v9.Z();
          }
        }
        break;
      case 4:
        for (jpar = 1; jpar <= *NbParams; jpar++)
        {
          Vpar                                  = Parameters[jpar - 1];
          vect                                  = myAdaptor->DN(Upar, Vpar, *UOrder, *VOrder);
          Result[(jpar - 1) * (*Dimension)]     = vect.X();
          Result[1 + (jpar - 1) * (*Dimension)] = vect.Y();
          Result[2 + (jpar - 1) * (*Dimension)] = vect.Z();
        }
        break;
    }
  }
  else
  {
    Vpar = *ConstParam;
    switch (Order)
    {
      case 0:
        for (jpar = 1; jpar <= *NbParams; jpar++)
        {
          Upar                                  = Parameters[jpar - 1];
          pnt                                   = myAdaptor->Value(Upar, Vpar);
          Result[(jpar - 1) * (*Dimension)]     = pnt.X();
          Result[1 + (jpar - 1) * (*Dimension)] = pnt.Y();
          Result[2 + (jpar - 1) * (*Dimension)] = pnt.Z();
        }
        break;
      case 1:
        for (jpar = 1; jpar <= *NbParams; jpar++)
        {
          Upar = Parameters[jpar - 1];
          myAdaptor->D1(Upar, Vpar, pnt, v1, v2);
          if (*UOrder == 1)
          {
            Result[(jpar - 1) * (*Dimension)]     = v1.X();
            Result[1 + (jpar - 1) * (*Dimension)] = v1.Y();
            Result[2 + (jpar - 1) * (*Dimension)] = v1.Z();
          }
          else
          {
            Result[(jpar - 1) * (*Dimension)]     = v2.X();
            Result[1 + (jpar - 1) * (*Dimension)] = v2.Y();
            Result[2 + (jpar - 1) * (*Dimension)] = v2.Z();
          }
        }
        break;
      case 2:
        for (jpar = 1; jpar <= *NbParams; jpar++)
        {
          Upar = Parameters[jpar - 1];
          myAdaptor->D2(Upar, Vpar, pnt, v1, v2, v3, v4, v5);
          if (*UOrder == 2)
          {
            Result[(jpar - 1) * (*Dimension)]     = v3.X();
            Result[1 + (jpar - 1) * (*Dimension)] = v3.Y();
            Result[2 + (jpar - 1) * (*Dimension)] = v3.Z();
          }
          else if (*UOrder == 1)
          {
            Result[(jpar - 1) * (*Dimension)]     = v5.X();
            Result[1 + (jpar - 1) * (*Dimension)] = v5.Y();
            Result[2 + (jpar - 1) * (*Dimension)] = v5.Z();
          }
          else if (*UOrder == 0)
          {
            Result[(jpar - 1) * (*Dimension)]     = v4.X();
            Result[1 + (jpar - 1) * (*Dimension)] = v4.Y();
            Result[2 + (jpar - 1) * (*Dimension)] = v4.Z();
          }
        }
        break;
      case 3:
        for (jpar = 1; jpar <= *NbParams; jpar++)
        {
          Upar = Parameters[jpar - 1];
          myAdaptor->D3(Upar, Vpar, pnt, v1, v2, v3, v4, v5, v6, v7, v8, v9);
          if (*UOrder == 2)
          {
            Result[(jpar - 1) * (*Dimension)]     = v8.X();
            Result[1 + (jpar - 1) * (*Dimension)] = v8.Y();
            Result[2 + (jpar - 1) * (*Dimension)] = v8.Z();
          }
          else if (*UOrder == 1)
          {
            Result[(jpar - 1) * (*Dimension)]     = v9.X();
            Result[1 + (jpar - 1) * (*Dimension)] = v9.Y();
            Result[2 + (jpar - 1) * (*Dimension)] = v9.Z();
          }
        }
        break;
      case 4:
        for (jpar = 1; jpar <= *NbParams; jpar++)
        {
          Upar                                  = Parameters[jpar - 1];
          vect                                  = myAdaptor->DN(Upar, Vpar, *UOrder, *VOrder);
          Result[(jpar - 1) * (*Dimension)]     = vect.X();
          Result[1 + (jpar - 1) * (*Dimension)] = vect.Y();
          Result[2 + (jpar - 1) * (*Dimension)] = vect.Z();
        }
        break;
    }
  }
}

//=================================================================================================

GeomConvert_ApproxSurface::GeomConvert_ApproxSurface(const occ::handle<Geom_Surface>& Surf,
                                                     const double         Tol3d,
                                                     const GeomAbs_Shape         UContinuity,
                                                     const GeomAbs_Shape         VContinuity,
                                                     const int      MaxDegU,
                                                     const int      MaxDegV,
                                                     const int      MaxSegments,
                                                     const int      PrecisCode)
{
  occ::handle<Adaptor3d_Surface> aSurfAdaptor = new GeomAdaptor_Surface(Surf);
  Approximate(aSurfAdaptor,
              Tol3d,
              UContinuity,
              VContinuity,
              MaxDegU,
              MaxDegV,
              MaxSegments,
              PrecisCode);
}

GeomConvert_ApproxSurface::GeomConvert_ApproxSurface(const occ::handle<Adaptor3d_Surface>& Surf,
                                                     const double              Tol3d,
                                                     const GeomAbs_Shape              UContinuity,
                                                     const GeomAbs_Shape              VContinuity,
                                                     const int           MaxDegU,
                                                     const int           MaxDegV,
                                                     const int           MaxSegments,
                                                     const int           PrecisCode)
{
  Approximate(Surf, Tol3d, UContinuity, VContinuity, MaxDegU, MaxDegV, MaxSegments, PrecisCode);
}

void GeomConvert_ApproxSurface::Approximate(const occ::handle<Adaptor3d_Surface>& theSurf,
                                            const double              theTol3d,
                                            const GeomAbs_Shape              theUContinuity,
                                            const GeomAbs_Shape              theVContinuity,
                                            const int           theMaxDegU,
                                            const int           theMaxDegV,
                                            const int           theMaxSegments,
                                            const int           thePrecisCode)
{
  double U0 = theSurf->FirstUParameter();
  double U1 = theSurf->LastUParameter();
  double V0 = theSurf->FirstVParameter();
  double V1 = theSurf->LastVParameter();

  // " Init des nombres de sous-espaces et des tolerances"
  int              nb1 = 0, nb2 = 0, nb3 = 1;
  occ::handle<NCollection_HArray1<double>> nul1 = new NCollection_HArray1<double>(1, 1);
  nul1->SetValue(1, 0.);
  occ::handle<NCollection_HArray2<double>> nul2 = new NCollection_HArray2<double>(1, 1, 1, 4);
  nul2->SetValue(1, 1, 0.);
  nul2->SetValue(1, 2, 0.);
  nul2->SetValue(1, 3, 0.);
  nul2->SetValue(1, 4, 0.);
  occ::handle<NCollection_HArray1<double>> eps3D = new NCollection_HArray1<double>(1, 1);
  eps3D->SetValue(1, theTol3d);
  occ::handle<NCollection_HArray2<double>> epsfr = new NCollection_HArray2<double>(1, 1, 1, 4);
  epsfr->SetValue(1, 1, theTol3d);
  epsfr->SetValue(1, 2, theTol3d);
  epsfr->SetValue(1, 3, theTol3d);
  epsfr->SetValue(1, 4, theTol3d);

  // " Init du type d'iso"
  GeomAbs_IsoType  IsoType = GeomAbs_IsoV;
  int NbDec;

  NbDec = theSurf->NbUIntervals(GeomAbs_C2);
  NCollection_Array1<double> UDec_C2(1, NbDec + 1);
  theSurf->UIntervals(UDec_C2, GeomAbs_C2);
  NbDec = theSurf->NbVIntervals(GeomAbs_C2);
  NCollection_Array1<double> VDec_C2(1, NbDec + 1);
  theSurf->VIntervals(VDec_C2, GeomAbs_C2);

  NbDec = theSurf->NbUIntervals(GeomAbs_C3);
  NCollection_Array1<double> UDec_C3(1, NbDec + 1);
  theSurf->UIntervals(UDec_C3, GeomAbs_C3);

  NbDec = theSurf->NbVIntervals(GeomAbs_C3);
  NCollection_Array1<double> VDec_C3(1, NbDec + 1);
  theSurf->VIntervals(VDec_C3, GeomAbs_C3);
  // Approximation avec decoupe preferentiel
  // aux lieux de discontinuitees C2
  AdvApprox_PrefAndRec pUDec(UDec_C2, UDec_C3);
  AdvApprox_PrefAndRec pVDec(VDec_C2, VDec_C3);

  // POP pour WNT
  GeomConvert_ApproxSurface_Eval ev(theSurf);
  AdvApp2Var_ApproxAFunc2Var     approx(nb1,
                                    nb2,
                                    nb3,
                                    nul1,
                                    nul1,
                                    eps3D,
                                    nul2,
                                    nul2,
                                    epsfr,
                                    U0,
                                    U1,
                                    V0,
                                    V1,
                                    IsoType,
                                    theUContinuity,
                                    theVContinuity,
                                    thePrecisCode,
                                    theMaxDegU,
                                    theMaxDegV,
                                    theMaxSegments,
                                    ev,
                                    pUDec,
                                    pVDec);

  myMaxError  = approx.MaxError(3, 1);
  myBSplSurf  = approx.Surface(1);
  myIsDone    = approx.IsDone();
  myHasResult = approx.HasResult();
}

//=================================================================================================

occ::handle<Geom_BSplineSurface> GeomConvert_ApproxSurface::Surface() const
{
  return myBSplSurf;
}

//=================================================================================================

bool GeomConvert_ApproxSurface::IsDone() const
{
  return myIsDone;
}

//=================================================================================================

bool GeomConvert_ApproxSurface::HasResult() const
{
  return myHasResult;
}

//=================================================================================================

double GeomConvert_ApproxSurface::MaxError() const
{
  return myMaxError;
}

//=================================================================================================

void GeomConvert_ApproxSurface::Dump(Standard_OStream& o) const
{
  o << std::endl;
  if (!myHasResult)
  {
    o << "No result" << std::endl;
  }
  else
  {
    o << "Result max error :" << myMaxError << std::endl;
  }
  o << std::endl;
}
