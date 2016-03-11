// Copyright (c) 2015 OPEN CASCADE SAS
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


#ifndef _ShapePersistent_Geom2d_Curve_HeaderFile
#define _ShapePersistent_Geom2d_Curve_HeaderFile

#include <ShapePersistent_Geom2d.hxx>
#include <ShapePersistent_HArray1.hxx>
#include <StdLPersistent_HArray1.hxx>

#include <Geom2d_Line.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Parabola.hxx>

#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Parab2d.hxx>


class ShapePersistent_Geom2d_Curve : protected ShapePersistent_Geom2d
{
  typedef Curve::PersistentBase pBase;

  typedef pBase pBounded;

  class pBezier : public pBounded
  {
  public:
    inline void Read (StdObjMgt_ReadData& theReadData)
      { theReadData >> myRational >> myPoles >> myWeights; }

    virtual Handle(Geom2d_Curve) Import() const;

  private:
    Standard_Boolean                       myRational;
    Handle(ShapePersistent_HArray1::Pnt2d) myPoles;
    Handle(StdLPersistent_HArray1::Real)   myWeights;
  };

  class pBSpline : public pBounded
  {
  public:
    inline void Read (StdObjMgt_ReadData& theReadData)
    {
      theReadData >> myRational >> myPeriodic >> mySpineDegree;
      theReadData >> myPoles >> myWeights >> myKnots >> myMultiplicities;
    }

    virtual Handle(Geom2d_Curve) Import() const;

  private:
    Standard_Boolean                        myRational;
    Standard_Boolean                        myPeriodic;
    Standard_Integer                        mySpineDegree;
    Handle(ShapePersistent_HArray1::Pnt2d)  myPoles;
    Handle(StdLPersistent_HArray1::Real)    myWeights;
    Handle(StdLPersistent_HArray1::Real)    myKnots;
    Handle(StdLPersistent_HArray1::Integer) myMultiplicities;
  };

  class pTrimmed : public pBounded
  {
  public:
    inline void Read (StdObjMgt_ReadData& theReadData)
      { theReadData >> myBasisCurve >> myFirstU >> myLastU; }

    virtual Handle(Geom2d_Curve) Import() const;

  private:
    Handle(Curve) myBasisCurve;
    Standard_Real myFirstU;
    Standard_Real myLastU;
  };

  class pOffset : public pBase
  {
  public:
    inline void Read (StdObjMgt_ReadData& theReadData)
      { theReadData >> myBasisCurve >> myOffsetValue; }

    virtual Handle(Geom2d_Curve) Import() const;

  private:
    Handle(Curve) myBasisCurve;
    Standard_Real myOffsetValue;
  };

public:
  typedef instance<Curve, Geom2d_Line, gp_Ax2d>         Line;

  typedef subBase_gp<Curve, gp_Ax22d>                   Conic;
  typedef instance<Conic, Geom2d_Circle   , gp_Circ2d > Circle;
  typedef instance<Conic, Geom2d_Ellipse  , gp_Elips2d> Ellipse;
  typedef instance<Conic, Geom2d_Hyperbola, gp_Hypr2d > Hyperbola;
  typedef instance<Conic, Geom2d_Parabola , gp_Parab2d> Parabola;

  typedef subBase_empty<Curve>                          Bounded;
  typedef Delayed<Bounded, pBezier>                     Bezier;
  typedef Delayed<Bounded, pBSpline>                    BSpline;
  typedef Delayed<Bounded, pTrimmed>                    Trimmed;

  typedef Delayed<Curve, pOffset>                       Offset;
};

#endif
