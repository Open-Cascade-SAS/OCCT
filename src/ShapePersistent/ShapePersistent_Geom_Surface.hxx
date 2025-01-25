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
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_OffsetSurface.hxx>

#include <gp_Ax3.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>

class ShapePersistent_Geom_Surface : private ShapePersistent_Geom
{
  typedef Surface::PersistentBase pBase;

  class pSweptData
  {
    friend class ShapePersistent_Geom_Surface;

  public:
    inline void Read(StdObjMgt_ReadData& theReadData)
    {
      theReadData >> myBasisCurve >> myDirection;
    }

    inline void Write(StdObjMgt_WriteData& theWriteData) const
    {
      theWriteData << myBasisCurve << myDirection;
    }

    inline void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
    {
      theChildren.Append(myBasisCurve);
    }

  protected:
    Handle(Curve) myBasisCurve;
    gp_Dir        myDirection;
  };

  struct pSwept : pBase, pSweptData
  {
    inline Standard_CString PName() const { return "PGeom_SweptSurface"; }
  };

  class pLinearExtrusion : public pSwept
  {
    friend class ShapePersistent_Geom_Surface;

  public:
    virtual Handle(Geom_Surface) Import() const;

    inline Standard_CString PName() const { return "PGeom_SurfaceOfLinearExtrusion"; }
  };

  class pRevolution : public pSwept
  {
    friend class ShapePersistent_Geom_Surface;

  public:
    inline void Read(StdObjMgt_ReadData& theReadData)
    {
      pSwept::Read(theReadData);
      theReadData >> myLocation;
    }

    inline void Write(StdObjMgt_WriteData& theWriteData) const
    {
      pSwept::Write(theWriteData);
      theWriteData << myLocation;
    }

    inline Standard_CString PName() const { return "PGeom_SurfaceOfRevolution"; }

    virtual Handle(Geom_Surface) Import() const;

  private:
    gp_Pnt myLocation;
  };

  typedef pBase pBounded;

  class pBezier : public pBounded
  {
    friend class ShapePersistent_Geom_Surface;

  public:
    pBezier()
        : myURational(Standard_False),
          myVRational(Standard_False)
    {
    }

    inline void Read(StdObjMgt_ReadData& theReadData)
    {
      theReadData >> myURational >> myVRational >> myPoles >> myWeights;
    }

    inline void Write(StdObjMgt_WriteData& theWriteData) const
    {
      theWriteData << myURational << myVRational << myPoles << myWeights;
    }

    inline void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
    {
      theChildren.Append(myPoles);
      theChildren.Append(myWeights);
    }

    inline Standard_CString PName() const { return "PGeom_BezierSurface"; }

    virtual Handle(Geom_Surface) Import() const;

  private:
    Standard_Boolean                     myURational;
    Standard_Boolean                     myVRational;
    Handle(ShapePersistent_HArray2::Pnt) myPoles;
    Handle(StdLPersistent_HArray2::Real) myWeights;
  };

  class pBSpline : public pBounded
  {
    friend class ShapePersistent_Geom_Surface;

  public:
    pBSpline()
        : myURational(Standard_False),
          myVRational(Standard_False),
          myUPeriodic(Standard_False),
          myVPeriodic(Standard_False),
          myUSpineDegree(0),
          myVSpineDegree(0)
    {
    }

    inline void Read(StdObjMgt_ReadData& theReadData)
    {
      theReadData >> myURational >> myVRational;
      theReadData >> myUPeriodic >> myVPeriodic;
      theReadData >> myUSpineDegree >> myVSpineDegree;
      theReadData >> myPoles;
      theReadData >> myWeights;
      theReadData >> myUKnots >> myVKnots;
      theReadData >> myUMultiplicities >> myVMultiplicities;
    }

    inline void Write(StdObjMgt_WriteData& theWriteData) const
    {
      theWriteData << myURational << myVRational;
      theWriteData << myUPeriodic << myVPeriodic;
      theWriteData << myUSpineDegree << myVSpineDegree;
      theWriteData << myPoles;
      theWriteData << myWeights;
      theWriteData << myUKnots << myVKnots;
      theWriteData << myUMultiplicities << myVMultiplicities;
    }

    inline void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
    {
      theChildren.Append(myPoles);
      theChildren.Append(myWeights);
      theChildren.Append(myUKnots);
      theChildren.Append(myVKnots);
      theChildren.Append(myUMultiplicities);
      theChildren.Append(myVMultiplicities);
    }

    inline Standard_CString PName() const { return "PGeom_BSplineSurface"; }

    virtual Handle(Geom_Surface) Import() const;

  private:
    Standard_Boolean                        myURational;
    Standard_Boolean                        myVRational;
    Standard_Boolean                        myUPeriodic;
    Standard_Boolean                        myVPeriodic;
    Standard_Integer                        myUSpineDegree;
    Standard_Integer                        myVSpineDegree;
    Handle(ShapePersistent_HArray2::Pnt)    myPoles;
    Handle(StdLPersistent_HArray2::Real)    myWeights;
    Handle(StdLPersistent_HArray1::Real)    myUKnots;
    Handle(StdLPersistent_HArray1::Real)    myVKnots;
    Handle(StdLPersistent_HArray1::Integer) myUMultiplicities;
    Handle(StdLPersistent_HArray1::Integer) myVMultiplicities;
  };

  class pRectangularTrimmed : public pBounded
  {
    friend class ShapePersistent_Geom_Surface;

  public:
    pRectangularTrimmed()
        : myFirstU(0.0),
          myLastU(0.0),
          myFirstV(0.0),
          myLastV(0.0)
    {
    }

    inline void Read(StdObjMgt_ReadData& theReadData)
    {
      theReadData >> myBasisSurface;
      theReadData >> myFirstU >> myLastU >> myFirstV >> myLastV;
    }

    inline void Write(StdObjMgt_WriteData& theWriteData) const
    {
      theWriteData << myBasisSurface;
      theWriteData << myFirstU << myLastU << myFirstV << myLastV;
    }

    inline void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
    {
      theChildren.Append(myBasisSurface);
    }

    inline Standard_CString PName() const { return "PGeom_RectangularTrimmedSurface"; }

    virtual Handle(Geom_Surface) Import() const;

  private:
    Handle(Surface) myBasisSurface;
    Standard_Real   myFirstU;
    Standard_Real   myLastU;
    Standard_Real   myFirstV;
    Standard_Real   myLastV;
  };

  class pOffset : public pBase
  {
    friend class ShapePersistent_Geom_Surface;

  public:
    pOffset()
        : myOffsetValue(0.0)
    {
    }

    inline void Read(StdObjMgt_ReadData& theReadData)
    {
      theReadData >> myBasisSurface >> myOffsetValue;
    }

    inline void Write(StdObjMgt_WriteData& theWriteData) const
    {
      theWriteData << myBasisSurface << myOffsetValue;
    }

    inline void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
    {
      theChildren.Append(myBasisSurface);
    }

    inline Standard_CString PName() const { return "PGeom_OffsetSurface"; }

    virtual Handle(Geom_Surface) Import() const;

  private:
    Handle(Surface) myBasisSurface;
    Standard_Real   myOffsetValue;
  };

public:
  typedef subBase_gp<Surface, gp_Ax3>                                Elementary;
  typedef instance<Elementary, Geom_Plane, gp_Ax3>                   Plane;
  typedef instance<Elementary, Geom_ConicalSurface, gp_Cone>         Conical;
  typedef instance<Elementary, Geom_CylindricalSurface, gp_Cylinder> Cylindrical;
  typedef instance<Elementary, Geom_SphericalSurface, gp_Sphere>     Spherical;
  typedef instance<Elementary, Geom_ToroidalSurface, gp_Torus>       Toroidal;

  typedef subBase<Surface, pSweptData>     Swept;
  typedef Delayed<Swept, pLinearExtrusion> LinearExtrusion;
  typedef Delayed<Swept, pRevolution>      Revolution;

  typedef subBase_empty<Surface>                Bounded;
  typedef Delayed<Bounded, pBezier>             Bezier;
  typedef Delayed<Bounded, pBSpline>            BSpline;
  typedef Delayed<Bounded, pRectangularTrimmed> RectangularTrimmed;

  typedef Delayed<Surface, pOffset> Offset;

public:
  //! Create a persistent object for a plane
  Standard_EXPORT static Handle(Surface) Translate(const Handle(Geom_Plane)&         theSurf,
                                                   StdObjMgt_TransientPersistentMap& theMap);
  //! Create a persistent object for a cylinder
  Standard_EXPORT static Handle(Surface) Translate(const Handle(Geom_CylindricalSurface)& theSurf,
                                                   StdObjMgt_TransientPersistentMap&      theMap);
  //! Create a persistent object for a cone
  Standard_EXPORT static Handle(Surface) Translate(const Handle(Geom_ConicalSurface)& theSurf,
                                                   StdObjMgt_TransientPersistentMap&  theMap);
  //! Create a persistent object for a sphere
  Standard_EXPORT static Handle(Surface) Translate(const Handle(Geom_SphericalSurface)& theSurf,
                                                   StdObjMgt_TransientPersistentMap&    theMap);
  //! Create a persistent object for a torus
  Standard_EXPORT static Handle(Surface) Translate(const Handle(Geom_ToroidalSurface)& theSurf,
                                                   StdObjMgt_TransientPersistentMap&   theMap);
  //! Create a persistent object for a surface of linear extrusion
  Standard_EXPORT static Handle(Surface) Translate(
    const Handle(Geom_SurfaceOfLinearExtrusion)& theSurf,
    StdObjMgt_TransientPersistentMap&            theMap);
  //! Create a persistent object for a surface of evolution
  Standard_EXPORT static Handle(Surface) Translate(const Handle(Geom_SurfaceOfRevolution)& theSurf,
                                                   StdObjMgt_TransientPersistentMap&       theMap);
  //! Create a persistent object for a Bezier surface
  Standard_EXPORT static Handle(Surface) Translate(const Handle(Geom_BezierSurface)& theSurf,
                                                   StdObjMgt_TransientPersistentMap& theMap);
  //! Create a persistent object for a BSpline surface
  Standard_EXPORT static Handle(Surface) Translate(const Handle(Geom_BSplineSurface)& theSurf,
                                                   StdObjMgt_TransientPersistentMap&  theMap);
  //! Create a persistent object for a rectangylar trimmed surface
  Standard_EXPORT static Handle(Surface) Translate(
    const Handle(Geom_RectangularTrimmedSurface)& theSurf,
    StdObjMgt_TransientPersistentMap&             theMap);
  //! Create a persistent object for an offset surface
  Standard_EXPORT static Handle(Surface) Translate(const Handle(Geom_OffsetSurface)& theSurf,
                                                   StdObjMgt_TransientPersistentMap& theMap);
};

//=======================================================================
// Elementary
//=======================================================================
template <>
Standard_CString ShapePersistent_Geom::subBase_gp<ShapePersistent_Geom::Surface, gp_Ax3>::PName()
  const;

//=======================================================================
// Plane
//=======================================================================
template <>
Standard_CString ShapePersistent_Geom::instance<
  ShapePersistent_Geom::subBase_gp<ShapePersistent_Geom::Surface, gp_Ax3>,
  Geom_Plane,
  gp_Ax3>::PName() const;

template <>
void ShapePersistent_Geom::instance<
  ShapePersistent_Geom::subBase_gp<ShapePersistent_Geom::Surface, gp_Ax3>,
  Geom_Plane,
  gp_Ax3>::Write(StdObjMgt_WriteData& theWriteData) const;

//=======================================================================
// Conical
//=======================================================================
template <>
Standard_CString ShapePersistent_Geom::instance<
  ShapePersistent_Geom::subBase_gp<ShapePersistent_Geom::Surface, gp_Ax3>,
  Geom_ConicalSurface,
  gp_Cone>::PName() const;

template <>
void ShapePersistent_Geom::instance<
  ShapePersistent_Geom::subBase_gp<ShapePersistent_Geom::Surface, gp_Ax3>,
  Geom_ConicalSurface,
  gp_Cone>::Write(StdObjMgt_WriteData& theWriteData) const;

//=======================================================================
// Cylindrical
//=======================================================================
template <>
Standard_CString ShapePersistent_Geom::instance<
  ShapePersistent_Geom::subBase_gp<ShapePersistent_Geom::Surface, gp_Ax3>,
  Geom_CylindricalSurface,
  gp_Cylinder>::PName() const;

template <>
void ShapePersistent_Geom::instance<
  ShapePersistent_Geom::subBase_gp<ShapePersistent_Geom::Surface, gp_Ax3>,
  Geom_CylindricalSurface,
  gp_Cylinder>::Write(StdObjMgt_WriteData& theWriteData) const;

//=======================================================================
// Spherical
//=======================================================================
template <>
Standard_CString ShapePersistent_Geom::instance<
  ShapePersistent_Geom::subBase_gp<ShapePersistent_Geom::Surface, gp_Ax3>,
  Geom_SphericalSurface,
  gp_Sphere>::PName() const;

template <>
void ShapePersistent_Geom::instance<
  ShapePersistent_Geom::subBase_gp<ShapePersistent_Geom::Surface, gp_Ax3>,
  Geom_SphericalSurface,
  gp_Sphere>::Write(StdObjMgt_WriteData& theWriteData) const;

//=======================================================================
// Toroidal
//=======================================================================
template <>
Standard_CString ShapePersistent_Geom::instance<
  ShapePersistent_Geom::subBase_gp<ShapePersistent_Geom::Surface, gp_Ax3>,
  Geom_ToroidalSurface,
  gp_Torus>::PName() const;

template <>
void ShapePersistent_Geom::instance<
  ShapePersistent_Geom::subBase_gp<ShapePersistent_Geom::Surface, gp_Ax3>,
  Geom_ToroidalSurface,
  gp_Torus>::Write(StdObjMgt_WriteData& theWriteData) const;

#endif
