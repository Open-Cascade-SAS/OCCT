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


#ifndef _ShapePersistent_Geom_Surface_HeaderFile
#define _ShapePersistent_Geom_Surface_HeaderFile

#include <ShapePersistent_Geom.hxx>
#include <ShapePersistent_HArray2.hxx>
#include <StdLPersistent_HArray1.hxx>
#include <StdLPersistent_HArray2.hxx>

#include <Geom_Plane.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>

#include <gp_Ax3.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>

class gp_Dir;
class gp_Pnt;


class ShapePersistent_Geom_Surface : private ShapePersistent_Geom
{
  typedef Surface::PersistentBase pBase;

  class pSweptData : protected StdObjMgt_ContentTypes
  {
  public:
    inline void Read (StdObjMgt_ReadData& theReadData)
      { theReadData >> myBasisCurve >> myDirection; }

  protected:
    Reference            <Curve>  myBasisCurve;
    StdObject_gp::Object <gp_Dir> myDirection;
  };

  struct pSwept : pBase, pSweptData {};

  class pLinearExtrusion : public pSwept
  {
  public:
    virtual Handle(Geom_Surface) Import() const;
  };

  class pRevolution : public pSwept
  {
  public:
    inline void Read (StdObjMgt_ReadData& theReadData)
    {
      pSwept::Read (theReadData);
      theReadData >> myLocation;
    }

    virtual Handle(Geom_Surface) Import() const;

  private:
    StdObject_gp::Object<gp_Pnt> myLocation;
  };

  typedef pBase pBounded;

  class pBezier : public pBounded
  {
  public:
    inline void Read (StdObjMgt_ReadData& theReadData)
      { theReadData >> myURational >> myVRational >> myPoles >> myWeights; }

    virtual Handle(Geom_Surface) Import() const;

  private:
    Value     <Standard_Boolean>             myURational;
    Value     <Standard_Boolean>             myVRational;
    Reference <ShapePersistent_HArray2::Pnt> myPoles;
    Reference <StdLPersistent_HArray2::Real> myWeights;
  };

  class pBSpline : public pBounded
  {
  public:
    inline void Read (StdObjMgt_ReadData& theReadData)
    {
      theReadData >> myURational >> myVRational;
      theReadData >> myUPeriodic >> myVPeriodic;
      theReadData >> myUSpineDegree >> myVSpineDegree;
      theReadData >> myPoles;
      theReadData >> myWeights;
      theReadData >> myUKnots >> myVKnots;
      theReadData >> myUMultiplicities >> myVMultiplicities;
    }

    virtual Handle(Geom_Surface) Import() const;

  private:
    Value     <Standard_Boolean>                myURational;
    Value     <Standard_Boolean>                myVRational;
    Value     <Standard_Boolean>                myUPeriodic;
    Value     <Standard_Boolean>                myVPeriodic;
    Value     <Standard_Integer>                myUSpineDegree;
    Value     <Standard_Integer>                myVSpineDegree;
    Reference <ShapePersistent_HArray2::Pnt>    myPoles;
    Reference <StdLPersistent_HArray2::Real>    myWeights;
    Reference <StdLPersistent_HArray1::Real>    myUKnots;
    Reference <StdLPersistent_HArray1::Real>    myVKnots;
    Reference <StdLPersistent_HArray1::Integer> myUMultiplicities;
    Reference <StdLPersistent_HArray1::Integer> myVMultiplicities;
  };

  class pRectangularTrimmed : public pBounded
  {
  public:
    inline void Read (StdObjMgt_ReadData& theReadData)
    {
      theReadData >> myBasisSurface;
      theReadData >> myFirstU >> myLastU >> myFirstV >> myLastV;
    }

    virtual Handle(Geom_Surface) Import() const;

  private:
    Reference<Surface>   myBasisSurface;
    Value<Standard_Real> myFirstU;
    Value<Standard_Real> myLastU;
    Value<Standard_Real> myFirstV;
    Value<Standard_Real> myLastV;
  };

  class pOffset : public pBase
  {
  public:
    inline void Read (StdObjMgt_ReadData& theReadData)
      { theReadData >> myBasisSurface >> myOffsetValue; }

    virtual Handle(Geom_Surface) Import() const;

  private:
    Reference<Surface>   myBasisSurface;
    Value<Standard_Real> myOffsetValue;
  };

public:
  typedef subBase_gp<Surface, gp_Ax3>                                Elementary;
  typedef instance<Elementary, Geom_Plane             , gp_Ax3>      Plane;
  typedef instance<Elementary, Geom_ConicalSurface    , gp_Cone>     Conical;
  typedef instance<Elementary, Geom_CylindricalSurface, gp_Cylinder> Cylindrical;
  typedef instance<Elementary, Geom_SphericalSurface  , gp_Sphere>   Spherical;
  typedef instance<Elementary, Geom_ToroidalSurface   , gp_Torus>    Toroidal;

  typedef subBase<Surface, pSweptData>                               Swept;
  typedef Delayed<Swept, pLinearExtrusion>                           LinearExtrusion;
  typedef Delayed<Swept, pRevolution>                                Revolution;

  typedef subBase_empty<Surface>                                     Bounded;
  typedef Delayed<Bounded, pBezier>                                  Bezier;
  typedef Delayed<Bounded, pBSpline>                                 BSpline;
  typedef Delayed<Bounded, pRectangularTrimmed>                      RectangularTrimmed;

  typedef Delayed<Surface, pOffset>                                  Offset;
};

#endif
