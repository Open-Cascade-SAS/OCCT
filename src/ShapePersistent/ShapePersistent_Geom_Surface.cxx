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

#include <ShapePersistent_Geom_Surface.hxx>
#include <StdLPersistent_HArray1.hxx>
#include <ShapePersistent_HArray2.hxx>

#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_OffsetSurface.hxx>

#include <gp_Ax1.hxx>


Handle(Geom_Surface) ShapePersistent_Geom_Surface::pLinearExtrusion::Import()
  const
{
  if (myBasisCurve.IsNull())
    return NULL;

  return new Geom_SurfaceOfLinearExtrusion
    (myBasisCurve->Import(), myDirection);
}

Handle(Geom_Surface) ShapePersistent_Geom_Surface::pRevolution::Import() const
{
  if (myBasisCurve.IsNull())
    return NULL;

  return new Geom_SurfaceOfRevolution
    (myBasisCurve->Import(), gp_Ax1 (myLocation, myDirection));
}

Handle(Geom_Surface) ShapePersistent_Geom_Surface::pBezier::Import() const
{
  if (myPoles.IsNull())
    return NULL;

  if (myURational || myVRational)
  {
    if (myWeights.IsNull())
      return NULL;
    return new Geom_BezierSurface (*myPoles->Array(), *myWeights->Array());
  }
  else
    return new Geom_BezierSurface (*myPoles->Array());
}

Handle(Geom_Surface) ShapePersistent_Geom_Surface::pBSpline::Import() const
{
  if (myPoles.IsNull() || myUKnots.IsNull() || myVKnots.IsNull()
   || myUMultiplicities.IsNull() || myVMultiplicities.IsNull())
    return NULL;

  if (myURational || myVRational)
  {
    if (myWeights.IsNull())
      return NULL;

    return new Geom_BSplineSurface (*myPoles->Array(),
                                    *myWeights->Array(),
                                    *myUKnots->Array(),
                                    *myVKnots->Array(),
                                    *myUMultiplicities->Array(),
                                    *myVMultiplicities->Array(),
                                    myUSpineDegree,
                                    myVSpineDegree,
                                    myUPeriodic,
                                    myVPeriodic);
  }
  else
    return new Geom_BSplineSurface (*myPoles->Array(),
                                    *myUKnots->Array(),
                                    *myVKnots->Array(),
                                    *myUMultiplicities->Array(),
                                    *myVMultiplicities->Array(),
                                    myUSpineDegree,
                                    myVSpineDegree,
                                    myUPeriodic,
                                    myVPeriodic);
}

Handle(Geom_Surface) ShapePersistent_Geom_Surface::pRectangularTrimmed::Import()
  const
{
  if (myBasisSurface.IsNull())
    return NULL;

  return new Geom_RectangularTrimmedSurface
    (myBasisSurface->Import(), myFirstU, myLastU, myFirstV, myLastV);
}

Handle(Geom_Surface) ShapePersistent_Geom_Surface::pOffset::Import() const
{
  if (myBasisSurface.IsNull())
    return NULL;

  return new Geom_OffsetSurface (myBasisSurface->Import(), myOffsetValue);
}

//=======================================================================
// Elementary
//=======================================================================
template<>
Standard_CString ShapePersistent_Geom::subBase_gp<ShapePersistent_Geom::Surface,
                                                  gp_Ax3>
  ::PName() const { return "PGeom_ElementarySurface"; }

//=======================================================================
// Plane
//=======================================================================
template<>
Standard_CString ShapePersistent_Geom::instance<ShapePersistent_Geom::subBase_gp<ShapePersistent_Geom::Surface, gp_Ax3>,
                                                Geom_Plane,
                                                gp_Ax3>
  ::PName() const { return "PGeom_Plane"; }

template<>
void ShapePersistent_Geom::instance<ShapePersistent_Geom::subBase_gp<ShapePersistent_Geom::Surface, gp_Ax3>,
                                    Geom_Plane,
                                    gp_Ax3>
  ::Write(StdObjMgt_WriteData& theWriteData) const
{
  Handle(Geom_Plane) aMyGeom =
    Handle(Geom_Plane)::DownCast(myTransient);
  theWriteData << aMyGeom->Position();
}

Handle(ShapePersistent_Geom::Surface) 
ShapePersistent_Geom_Surface::Translate(const Handle(Geom_Plane)& theSurf,
                                        StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(ShapePersistent_Geom::Surface) aPS;
  if (!theSurf.IsNull())
  {
    if (theMap.IsBound(theSurf))
      aPS = Handle(ShapePersistent_Geom::Surface)::DownCast(theMap.Find(theSurf));
    else
    {
      Handle(Plane) aPP = new Plane;
      aPP->myTransient = theSurf;
      aPS = aPP;
    }
  }
  return aPS;
}

//=======================================================================
// Conical
//=======================================================================
template<>
Standard_CString ShapePersistent_Geom::instance<ShapePersistent_Geom::subBase_gp<ShapePersistent_Geom::Surface, gp_Ax3>,
                                                Geom_ConicalSurface,
                                                gp_Cone>
  ::PName() const { return "PGeom_ConicalSurface"; }

template<>
void ShapePersistent_Geom::instance<ShapePersistent_Geom::subBase_gp<ShapePersistent_Geom::Surface, gp_Ax3>,
                                    Geom_ConicalSurface,
                                    gp_Cone>
  ::Write(StdObjMgt_WriteData& theWriteData) const
{
  Handle(Geom_ConicalSurface) aMyGeom =
    Handle(Geom_ConicalSurface)::DownCast(myTransient);
  theWriteData << aMyGeom->Cone();
}

Handle(ShapePersistent_Geom::Surface) 
ShapePersistent_Geom_Surface::Translate(const Handle(Geom_ConicalSurface)& theSurf,
                                        StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(ShapePersistent_Geom::Surface) aPS;
  if (!theSurf.IsNull())
  {
    if (theMap.IsBound(theSurf))
      aPS = Handle(ShapePersistent_Geom::Surface)::DownCast(theMap.Find(theSurf));
    else
    {
      Handle(Conical) aPCon = new Conical;
      aPCon->myTransient = theSurf;
      aPS = aPCon;
    }
  }
  return aPS;
}

//=======================================================================
// Cylindrical
//=======================================================================
template<>
Standard_CString ShapePersistent_Geom::instance<ShapePersistent_Geom::subBase_gp<ShapePersistent_Geom::Surface, gp_Ax3>,
                                                Geom_CylindricalSurface,
                                                gp_Cylinder>
  ::PName() const { return "PGeom_CylindricalSurface"; }

template<>
void ShapePersistent_Geom::instance<ShapePersistent_Geom::subBase_gp<ShapePersistent_Geom::Surface, gp_Ax3>,
                                    Geom_CylindricalSurface,
                                    gp_Cylinder>
  ::Write(StdObjMgt_WriteData& theWriteData) const
{
  Handle(Geom_CylindricalSurface) aMyGeom =
    Handle(Geom_CylindricalSurface)::DownCast(myTransient);
  theWriteData << aMyGeom->Cylinder();
}

Handle(ShapePersistent_Geom::Surface) 
ShapePersistent_Geom_Surface::Translate(const Handle(Geom_CylindricalSurface)& theSurf,
                                        StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(ShapePersistent_Geom::Surface) aPS;
  if (!theSurf.IsNull())
  {
    if (theMap.IsBound(theSurf))
      aPS = Handle(ShapePersistent_Geom::Surface)::DownCast(theMap.Find(theSurf));
    else
    {
      Handle(Cylindrical) aPCyl = new Cylindrical;
      aPCyl->myTransient = theSurf;
      aPS = aPCyl;
    }
  }
  return aPS;
}

//=======================================================================
// Spherical
//=======================================================================
template<>
Standard_CString ShapePersistent_Geom::instance<ShapePersistent_Geom::subBase_gp<ShapePersistent_Geom::Surface, gp_Ax3>,
                                                Geom_SphericalSurface,
                                                gp_Sphere>
  ::PName() const { return "PGeom_SphericalSurface"; }

template<>
void ShapePersistent_Geom::instance<ShapePersistent_Geom::subBase_gp<ShapePersistent_Geom::Surface, gp_Ax3>,
                                    Geom_SphericalSurface,
                                    gp_Sphere>
  ::Write(StdObjMgt_WriteData& theWriteData) const
{
  Handle(Geom_SphericalSurface) aMyGeom =
    Handle(Geom_SphericalSurface)::DownCast(myTransient);
  theWriteData << aMyGeom->Sphere();
}

Handle(ShapePersistent_Geom::Surface) 
ShapePersistent_Geom_Surface::Translate(const Handle(Geom_SphericalSurface)& theSurf,
                                        StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(ShapePersistent_Geom::Surface) aPS;
  if (!theSurf.IsNull())
  {
    if (theMap.IsBound(theSurf))
      aPS = Handle(ShapePersistent_Geom::Surface)::DownCast(theMap.Find(theSurf));
    else
    {
      Handle(Spherical) aPSph = new Spherical;
      aPSph->myTransient = theSurf;
      aPS = aPSph;
    }
  }
  return aPS;
}

//=======================================================================
// Toroidal
//=======================================================================
template<>
Standard_CString ShapePersistent_Geom::instance<ShapePersistent_Geom::subBase_gp<ShapePersistent_Geom::Surface, gp_Ax3>,
                                                Geom_ToroidalSurface,
                                                gp_Torus>
  ::PName() const { return "PGeom_ToroidalSurface"; }

template<>
void ShapePersistent_Geom::instance<ShapePersistent_Geom::subBase_gp<ShapePersistent_Geom::Surface, gp_Ax3>,
                                    Geom_ToroidalSurface,
                                    gp_Torus>
  ::Write(StdObjMgt_WriteData& theWriteData) const
{
  Handle(Geom_ToroidalSurface) aMyGeom =
    Handle(Geom_ToroidalSurface)::DownCast(myTransient);
  theWriteData << aMyGeom->Torus();
}

Handle(ShapePersistent_Geom::Surface) 
ShapePersistent_Geom_Surface::Translate(const Handle(Geom_ToroidalSurface)& theSurf,
                                        StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(ShapePersistent_Geom::Surface) aPS;
  if (!theSurf.IsNull())
  {
    if (theMap.IsBound(theSurf))
      aPS = Handle(ShapePersistent_Geom::Surface)::DownCast(theMap.Find(theSurf));
    else
    {
      Handle(Toroidal) aPTor = new Toroidal;
      aPTor->myTransient = theSurf;
      aPS = aPTor;
    }
  }
  return aPS;
}

//=======================================================================
// LinearExtrusion
//=======================================================================
Handle(ShapePersistent_Geom::Surface)
ShapePersistent_Geom_Surface::Translate(const Handle(Geom_SurfaceOfLinearExtrusion)& theSurf,
                                        StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(ShapePersistent_Geom::Surface) aPS;
  if (!theSurf.IsNull())
  {
    if (theMap.IsBound(theSurf))
      aPS = Handle(ShapePersistent_Geom::Surface)::DownCast(theMap.Find(theSurf));
    else
    {
      Handle(LinearExtrusion) aPLE = new LinearExtrusion;
      Handle(pLinearExtrusion) aPpLE = new pLinearExtrusion;
      aPpLE->myDirection = theSurf->Direction();
      aPpLE->myBasisCurve = ShapePersistent_Geom::Translate(theSurf->BasisCurve(), theMap);
      aPLE->myPersistent = aPpLE;
      aPS = aPLE;
    }
  }
  return aPS;
}

//=======================================================================
// Revolution
//=======================================================================
Handle(ShapePersistent_Geom::Surface)
ShapePersistent_Geom_Surface::Translate(const Handle(Geom_SurfaceOfRevolution)& theSurf,
                                        StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(ShapePersistent_Geom::Surface) aPS;
  if (!theSurf.IsNull())
  {
    if (theMap.IsBound(theSurf))
      aPS = Handle(ShapePersistent_Geom::Surface)::DownCast(theMap.Find(theSurf));
    else
    {
      Handle(Revolution) aPR = new Revolution;
      Handle(pRevolution) aPpR = new pRevolution;
      aPpR->myLocation = theSurf->Location();
      aPpR->myDirection = theSurf->Direction();
      aPpR->myBasisCurve = ShapePersistent_Geom::Translate(theSurf->BasisCurve(), theMap);
      aPR->myPersistent = aPpR;
      aPS = aPR;
    }
  }
  return aPS;
}

//=======================================================================
// 
//=======================================================================
Handle(ShapePersistent_Geom::Surface)
ShapePersistent_Geom_Surface::Translate(const Handle(Geom_BezierSurface)& theSurf,
                                        StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(ShapePersistent_Geom::Surface) aPS;
  if (!theSurf.IsNull())
  {
    if (theMap.IsBound(theSurf))
      aPS = Handle(ShapePersistent_Geom::Surface)::DownCast(theMap.Find(theSurf));
    else
    {
      Handle(Bezier) aPB = new Bezier;
      Handle(pBezier) aPpB = new pBezier;
      aPpB->myURational = theSurf->IsURational();
      aPpB->myVRational = theSurf->IsVRational();
      aPpB->myPoles = StdLPersistent_HArray2::Translate<TColgp_HArray2OfPnt>("PColgp_HArray2OfPnt", theSurf->Poles());
      if (theSurf->IsURational() || theSurf->IsVRational()) {
        aPpB->myWeights = StdLPersistent_HArray2::Translate<TColStd_HArray2OfReal>(*theSurf->Weights());
      }
      aPB->myPersistent = aPpB;
      aPS = aPB;
    }
  }
  return aPS;
}

//=======================================================================
// BSpline
//=======================================================================
Handle(ShapePersistent_Geom::Surface)
ShapePersistent_Geom_Surface::Translate(const Handle(Geom_BSplineSurface)& theSurf,
                                        StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(ShapePersistent_Geom::Surface) aPS;
  if (!theSurf.IsNull())
  {
    if (theMap.IsBound(theSurf))
      aPS = Handle(ShapePersistent_Geom::Surface)::DownCast(theMap.Find(theSurf));
    else
    {
      Handle(BSpline) aPBS = new BSpline;
      Handle(pBSpline) aPpBS = new pBSpline;
      aPpBS->myURational = theSurf->IsURational();
      aPpBS->myVRational = theSurf->IsVRational();
      aPpBS->myUPeriodic = theSurf->IsUPeriodic();
      aPpBS->myVPeriodic = theSurf->IsVPeriodic();
      aPpBS->myUSpineDegree = theSurf->UDegree();
      aPpBS->myVSpineDegree = theSurf->VDegree();
      aPpBS->myPoles = StdLPersistent_HArray2::Translate<TColgp_HArray2OfPnt>("PColgp_HArray2OfPnt", theSurf->Poles());
      if (theSurf->IsURational() || theSurf->IsVRational()) {
        aPpBS->myWeights = StdLPersistent_HArray2::Translate<TColStd_HArray2OfReal>(*theSurf->Weights());
      }
      aPpBS->myUKnots = StdLPersistent_HArray1::Translate<TColStd_HArray1OfReal>(theSurf->UKnots());
      aPpBS->myVKnots = StdLPersistent_HArray1::Translate<TColStd_HArray1OfReal>(theSurf->VKnots());
      aPpBS->myUMultiplicities = StdLPersistent_HArray1::Translate<TColStd_HArray1OfInteger>(theSurf->UMultiplicities());
      aPpBS->myVMultiplicities = StdLPersistent_HArray1::Translate<TColStd_HArray1OfInteger>(theSurf->VMultiplicities());

      aPBS->myPersistent = aPpBS;
      aPS = aPBS;
    }
  }
  return aPS;
}

//=======================================================================
// RectangularTrimmed
//=======================================================================
Handle(ShapePersistent_Geom::Surface)
ShapePersistent_Geom_Surface::Translate(const Handle(Geom_RectangularTrimmedSurface)& theSurf,
                                        StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(ShapePersistent_Geom::Surface) aPS;
  if (!theSurf.IsNull())
  {
    if (theMap.IsBound(theSurf))
      aPS = Handle(ShapePersistent_Geom::Surface)::DownCast(theMap.Find(theSurf));
    else
    {
      Handle(RectangularTrimmed) aPRT = new RectangularTrimmed;
      Handle(pRectangularTrimmed) aPpRT = new pRectangularTrimmed;
      theSurf->Bounds(aPpRT->myFirstU, aPpRT->myLastU, aPpRT->myFirstV, aPpRT->myLastV);
      aPpRT->myBasisSurface = ShapePersistent_Geom::Translate(theSurf->BasisSurface(), theMap);
      aPRT->myPersistent = aPpRT;
      aPS = aPRT;
    }
  }
  return aPS;
}

//=======================================================================
// Offset
//=======================================================================
Handle(ShapePersistent_Geom::Surface)
ShapePersistent_Geom_Surface::Translate(const Handle(Geom_OffsetSurface)& theSurf,
                                        StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(ShapePersistent_Geom::Surface) aPS;
  if (!theSurf.IsNull())
  {
    if (theMap.IsBound(theSurf))
      aPS = Handle(ShapePersistent_Geom::Surface)::DownCast(theMap.Find(theSurf));
    else
    {
      Handle(Offset) aPO = new Offset;
      Handle(pOffset) aPpO = new pOffset;
      aPpO->myOffsetValue = theSurf->Offset();
      aPpO->myBasisSurface = ShapePersistent_Geom::Translate(theSurf->BasisSurface(), theMap);
      aPO->myPersistent = aPpO;
      aPS = aPO;
    }
  }
  return aPS;
}
