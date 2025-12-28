// Created on: 1998-05-12
// Created by: Roman BORISOV
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

#ifndef _Approx_CurvlinFunc_HeaderFile
#define _Approx_CurvlinFunc_HeaderFile

#include <Adaptor2d_Curve2d.hxx>
#include <Adaptor3d_Surface.hxx>
#include <GeomAbs_Shape.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

//! defines an abstract curve with
//! curvilinear parametrization
class Approx_CurvlinFunc : public Standard_Transient
{

public:
  Standard_EXPORT Approx_CurvlinFunc(const occ::handle<Adaptor3d_Curve>& C, const double Tol);

  Standard_EXPORT Approx_CurvlinFunc(const occ::handle<Adaptor2d_Curve2d>& C2D,
                                     const occ::handle<Adaptor3d_Surface>& S,
                                     const double              Tol);

  Standard_EXPORT Approx_CurvlinFunc(const occ::handle<Adaptor2d_Curve2d>& C2D1,
                                     const occ::handle<Adaptor2d_Curve2d>& C2D2,
                                     const occ::handle<Adaptor3d_Surface>& S1,
                                     const occ::handle<Adaptor3d_Surface>& S2,
                                     const double              Tol);

  //! ---Purpose Update the tolerance to used
  Standard_EXPORT void SetTol(const double Tol);

  Standard_EXPORT double FirstParameter() const;

  Standard_EXPORT double LastParameter() const;

  //! Returns the number of intervals for continuity
  //! <S>. May be one if Continuity(me) >= <S>
  Standard_EXPORT int NbIntervals(const GeomAbs_Shape S) const;

  //! Stores in <T> the parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide enough room to accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT void Intervals(NCollection_Array1<double>& T, const GeomAbs_Shape S) const;

  //! if First < 0 or Last > 1
  Standard_EXPORT void Trim(const double First,
                            const double Last,
                            const double Tol);

  //! Computes length of the curve.
  Standard_EXPORT void Length();

  //! Computes length of the curve segment.
  Standard_EXPORT double Length(Adaptor3d_Curve&    C,
                                       const double FirstU,
                                       const double LasrU) const;

  Standard_EXPORT double GetLength() const;

  //! returns original parameter corresponding S. if
  //! Case == 1 computation is performed on myC2D1 and mySurf1,
  //! otherwise it is done on myC2D2 and mySurf2.
  Standard_EXPORT double GetUParameter(Adaptor3d_Curve&       C,
                                              const double    S,
                                              const int NumberOfCurve) const;

  //! returns original parameter corresponding S.
  Standard_EXPORT double GetSParameter(const double U) const;

  //! if myCase != 1
  Standard_EXPORT bool EvalCase1(const double    S,
                                             const int Order,
                                             NCollection_Array1<double>&  Result) const;

  //! if myCase != 2
  Standard_EXPORT bool EvalCase2(const double    S,
                                             const int Order,
                                             NCollection_Array1<double>&  Result) const;

  //! if myCase != 3
  Standard_EXPORT bool EvalCase3(const double    S,
                                             const int Order,
                                             NCollection_Array1<double>&  Result);

  DEFINE_STANDARD_RTTIEXT(Approx_CurvlinFunc, Standard_Transient)

private:
  Standard_EXPORT void Init();

  Standard_EXPORT void Init(Adaptor3d_Curve&               C,
                            occ::handle<NCollection_HArray1<double>>& Si,
                            occ::handle<NCollection_HArray1<double>>& Ui) const;

  //! returns curvilinear parameter corresponding U.
  Standard_EXPORT double GetSParameter(Adaptor3d_Curve&    C,
                                              const double U,
                                              const double Length) const;

  Standard_EXPORT bool EvalCurOnSur(const double    S,
                                                const int Order,
                                                NCollection_Array1<double>&  Result,
                                                const int NumberOfCurve) const;

  occ::handle<Adaptor3d_Curve>       myC3D;
  occ::handle<Adaptor2d_Curve2d>     myC2D1;
  occ::handle<Adaptor2d_Curve2d>     myC2D2;
  occ::handle<Adaptor3d_Surface>     mySurf1;
  occ::handle<Adaptor3d_Surface>     mySurf2;
  int              myCase;
  double                 myFirstS;
  double                 myLastS;
  double                 myFirstU1;
  double                 myLastU1;
  double                 myFirstU2;
  double                 myLastU2;
  double                 myLength;
  double                 myLength1;
  double                 myLength2;
  double                 myTolLen;
  double                 myPrevS;
  double                 myPrevU;
  occ::handle<NCollection_HArray1<double>> myUi_1;
  occ::handle<NCollection_HArray1<double>> mySi_1;
  occ::handle<NCollection_HArray1<double>> myUi_2;
  occ::handle<NCollection_HArray1<double>> mySi_2;
};

#endif // _Approx_CurvlinFunc_HeaderFile
