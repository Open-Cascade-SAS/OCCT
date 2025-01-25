// Created: 2009-01-21
//
// Copyright (c) 2009-2013 OPEN CASCADE SAS
//
// This file is part of commercial software by OPEN CASCADE SAS,
// furnished in accordance with the terms and conditions of the contract
// and with the inclusion of this copyright notice.
// This file or any part thereof may not be provided or otherwise
// made available to any third party.
//
// No ownership title to the software is transferred hereby.
//
// OPEN CASCADE SAS makes no representation or warranties with respect to the
// performance of this software, and specifically disclaims any responsibility
// for any damages, special or consequential, connected with its use.

#ifndef _Geom2dConvert_PPoint_HeaderFile
#define _Geom2dConvert_PPoint_HeaderFile

#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>

class Adaptor2d_Curve2d;

//! Class representing a point on curve, with 2D coordinate and the tangent
class Geom2dConvert_PPoint
{
public:
  //! Empty constructor.
  Standard_EXPORT inline Geom2dConvert_PPoint()
      : myParameter(::RealLast()),
        myPoint(0., 0.),
        myD1(0., 0.)
  {
  }

  //! Constructor.
  Standard_EXPORT inline Geom2dConvert_PPoint(const Standard_Real theParameter,
                                              const gp_XY&        thePoint,
                                              const gp_XY&        theD1)
      : myParameter(theParameter),
        myPoint(thePoint),
        myD1(theD1)
  {
  }

  //! Constructor.
  Standard_EXPORT Geom2dConvert_PPoint(const Standard_Real      theParameter,
                                       const Adaptor2d_Curve2d& theAdaptor);

  //! Compute the distance between two 2d points.
  inline Standard_Real Dist(const Geom2dConvert_PPoint& theOth) const
  {
    return myPoint.Distance(theOth.myPoint);
  }

  //! Query the parameter value.
  inline Standard_Real Parameter() const { return myParameter; }

  //! Query the point location.
  inline const gp_XY& Point() const { return myPoint.XY(); }

  //! Query the first derivatives.
  inline const gp_XY& D1() const { return myD1.XY(); }

  //! Change the value of the derivative at the point.
  inline void SetD1(const gp_XY& theD1) { myD1.SetXY(theD1); }

  //! Compare two values of this type.
  Standard_EXPORT Standard_Boolean operator==(const Geom2dConvert_PPoint&) const;

  //! Compare two values of this type.
  Standard_EXPORT Standard_Boolean operator!=(const Geom2dConvert_PPoint&) const;

private:
  Standard_Real myParameter; //! Parameter value
  gp_Pnt2d      myPoint;     //! Point location
  gp_Vec2d      myD1;        //! derivatives by parameter (components of the tangent).
};

#endif
