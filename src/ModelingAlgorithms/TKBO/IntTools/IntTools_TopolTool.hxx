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

class IntTools_TopolTool;
DEFINE_STANDARD_HANDLE(IntTools_TopolTool, Adaptor3d_TopolTool)

//! Class redefine methods of TopolTool from Adaptor3d
//! concerning sample points
class IntTools_TopolTool : public Adaptor3d_TopolTool
{

public:
  //! Empty constructor
  Standard_EXPORT IntTools_TopolTool();

  //! Initializes me by surface
  Standard_EXPORT IntTools_TopolTool(const Handle(Adaptor3d_Surface)& theSurface);

  //! Redefined empty initializer
  //!
  //! Warning:
  //! Raises the exception NotImplemented
  Standard_EXPORT virtual void Initialize() Standard_OVERRIDE;

  //! Initializes me by surface
  Standard_EXPORT virtual void Initialize(const Handle(Adaptor3d_Surface)& theSurface)
    Standard_OVERRIDE;

  Standard_EXPORT virtual void ComputeSamplePoints() Standard_OVERRIDE;

  //! Computes the sample-points for the intersections algorithms
  Standard_EXPORT virtual Standard_Integer NbSamplesU() Standard_OVERRIDE;

  //! Computes the sample-points for the intersections algorithms
  Standard_EXPORT virtual Standard_Integer NbSamplesV() Standard_OVERRIDE;

  //! Computes the sample-points for the intersections algorithms
  Standard_EXPORT virtual Standard_Integer NbSamples() Standard_OVERRIDE;

  //! Returns a 2d point from surface myS
  //! and a corresponded 3d point
  //! for given index.
  //! The index should be from 1 to NbSamples()
  Standard_EXPORT virtual void SamplePoint(const Standard_Integer Index,
                                           gp_Pnt2d&              P2d,
                                           gp_Pnt&                P3d) Standard_OVERRIDE;

  //! compute the sample-points for the intersections algorithms
  //! by adaptive algorithm for BSpline surfaces. For other surfaces algorithm
  //! is the same as in method ComputeSamplePoints(), but only fill arrays of U
  //! and V sample parameters;
  //! theDefl is a required deflection
  //! theNUmin, theNVmin are minimal nb points for U and V.
  Standard_EXPORT virtual void SamplePnts(const Standard_Real    theDefl,
                                          const Standard_Integer theNUmin,
                                          const Standard_Integer theNVmin) Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IntTools_TopolTool, Adaptor3d_TopolTool)

protected:
private:
  Standard_Integer myNbSmplU;
  Standard_Integer myNbSmplV;
  Standard_Real    myU0;
  Standard_Real    myV0;
  Standard_Real    myDU;
  Standard_Real    myDV;
};

#endif // _IntTools_TopolTool_HeaderFile
