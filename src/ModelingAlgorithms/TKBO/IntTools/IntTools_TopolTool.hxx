// Created on: 2002-04-18
// Created by: Michael KLOKOV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _IntTools_TopolTool_HeaderFile
#define _IntTools_TopolTool_HeaderFile

#include <Adaptor3d_TopolTool.hxx>
#include <Adaptor3d_Surface.hxx>

class gp_Pnt2d;
class gp_Pnt;

//! Class redefine methods of TopolTool from Adaptor3d
//! concerning sample points
class IntTools_TopolTool : public Adaptor3d_TopolTool
{

public:
  //! Empty constructor
  Standard_EXPORT IntTools_TopolTool();

  //! Initializes me by surface
  Standard_EXPORT IntTools_TopolTool(const occ::handle<Adaptor3d_Surface>& theSurface);

  //! Redefined empty initializer
  //!
  //! Warning:
  //! Raises the exception NotImplemented
  Standard_EXPORT void Initialize() override;

  //! Initializes me by surface
  Standard_EXPORT void Initialize(const occ::handle<Adaptor3d_Surface>& theSurface) override;

  Standard_EXPORT void ComputeSamplePoints() override;

  //! Computes the sample-points for the intersections algorithms
  Standard_EXPORT int NbSamplesU() override;

  //! Computes the sample-points for the intersections algorithms
  Standard_EXPORT int NbSamplesV() override;

  //! Computes the sample-points for the intersections algorithms
  Standard_EXPORT int NbSamples() override;

  //! Returns a 2d point from surface myS
  //! and a corresponded 3d point
  //! for given index.
  //! The index should be from 1 to NbSamples()
  Standard_EXPORT void SamplePoint(const int Index, gp_Pnt2d& P2d, gp_Pnt& P3d) override;

  //! compute the sample-points for the intersections algorithms
  //! by adaptive algorithm for BSpline surfaces. For other surfaces algorithm
  //! is the same as in method ComputeSamplePoints(), but only fill arrays of U
  //! and V sample parameters;
  //! theDefl is a required deflection
  //! theNUmin, theNVmin are minimal nb points for U and V.
  Standard_EXPORT void SamplePnts(const double theDefl,
                                  const int    theNUmin,
                                  const int    theNVmin) override;

  DEFINE_STANDARD_RTTIEXT(IntTools_TopolTool, Adaptor3d_TopolTool)

private:
  int    myNbSmplU;
  int    myNbSmplV;
  double myU0;
  double myV0;
  double myDU;
  double myDV;
};

#endif // _IntTools_TopolTool_HeaderFile
