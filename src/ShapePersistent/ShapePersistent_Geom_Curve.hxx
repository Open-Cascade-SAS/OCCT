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


#ifndef _ShapePersistent_Geom_Curve_HeaderFile
#define _ShapePersistent_Geom_Curve_HeaderFile

#include <ShapePersistent_Geom.hxx>
#include <ShapePersistent_HArray1.hxx>
#include <StdLPersistent_HArray1.hxx>

#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Parabola.hxx>

#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Parab.hxx>

class gp_Dir;


class ShapePersistent_Geom_Curve : private ShapePersistent_Geom
{
  typedef Curve::PersistentBase pBase;

  typedef pBase pBounded;

  class pBezier : public pBounded
  {
  public:
    inline void Read (StdObjMgt_ReadData& theReadData)
      { theReadData >> myRational >> myPoles >> myWeights; }

    virtual Handle(Geom_Curve) Import() const;

  private:
    Value     <Standard_Boolean>             myRational;
    Reference <ShapePersistent_HArray1::Pnt> myPoles;
    Reference <StdLPersistent_HArray1::Real> myWeights;
  };

  class pBSpline : public pBounded
  {
  public:
    inline void Read (StdObjMgt_ReadData& theReadData)
    {
      theReadData >> myRational >> myPeriodic >> mySpineDegree;
      theReadData >> myPoles >> myWeights >> myKnots >> myMultiplicities;
    }

    virtual Handle(Geom_Curve) Import() const;

  private:
    Value     <Standard_Boolean>                myRational;
    Value     <Standard_Boolean>                myPeriodic;
    Value     <Standard_Integer>                mySpineDegree;
    Reference <ShapePersistent_HArray1::Pnt>    myPoles;
    Reference <StdLPersistent_HArray1::Real>    myWeights;
    Reference <StdLPersistent_HArray1::Real>    myKnots;
    Reference <StdLPersistent_HArray1::Integer> myMultiplicities;
  };

  class pTrimmed : public pBounded
  {
  public:
    inline void Read (StdObjMgt_ReadData& theReadData)
      { theReadData >> myBasisCurve >> myFirstU >> myLastU; }

    virtual Handle(Geom_Curve) Import() const;

  private:
    Reference<Curve>     myBasisCurve;
    Value<Standard_Real> myFirstU;
    Value<Standard_Real> myLastU;
  };

  class pOffset : public pBase
  {
  public:
    inline void Read (StdObjMgt_ReadData& theReadData)
      { theReadData >> myBasisCurve >> myOffsetDirection >> myOffsetValue; }

    virtual Handle(Geom_Curve) Import() const;

  private:
    Reference<Curve>             myBasisCurve;
    StdObject_gp::Object<gp_Dir> myOffsetDirection;
    Value<Standard_Real>         myOffsetValue;
  };

public:
  typedef instance<Curve, Geom_Line, gp_Ax1>        Line;

  typedef subBase_gp<Curve, gp_Ax2>                 Conic;
  typedef instance<Conic, Geom_Circle   , gp_Circ > Circle;
  typedef instance<Conic, Geom_Ellipse  , gp_Elips> Ellipse;
  typedef instance<Conic, Geom_Hyperbola, gp_Hypr > Hyperbola;
  typedef instance<Conic, Geom_Parabola , gp_Parab> Parabola;

  typedef subBase_empty<Curve>                      Bounded;
  typedef Delayed<Bounded, pBezier>                 Bezier;
  typedef Delayed<Bounded, pBSpline>                BSpline;
  typedef Delayed<Bounded, pTrimmed>                Trimmed;

  typedef Delayed<Curve, pOffset>                   Offset;
};

#endif
