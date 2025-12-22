// Copyright (c) 2025 OPEN CASCADE SAS
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

#ifndef _GeomAdaptor_TransformedSurface_HeaderFile
#define _GeomAdaptor_TransformedSurface_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Trsf.hxx>

DEFINE_STANDARD_HANDLE(GeomAdaptor_TransformedSurface, Adaptor3d_Surface)

//! An adaptor for surfaces with an applied transformation.
//!
//! This class wraps a GeomAdaptor_Surface and applies a gp_Trsf transformation
//! to all point and derivative evaluations. It serves as a base class for
//! BRepAdaptor_Surface and allows batch evaluation with transformations in
//! GeomGridEval_Surface.
//!
//! The evaluation methods (Value, D0, D1, D2, D3, DN) are marked final
//! to enable optimizations in grid evaluation.
class GeomAdaptor_TransformedSurface : public Adaptor3d_Surface
{
  DEFINE_STANDARD_RTTIEXT(GeomAdaptor_TransformedSurface, Adaptor3d_Surface)
public:
  //! Creates an undefined surface with identity transformation.
  Standard_EXPORT GeomAdaptor_TransformedSurface();

  //! Creates a surface adaptor with transformation.
  //! @param theSurface underlying geometry
  //! @param theTrsf transformation to apply
  Standard_EXPORT GeomAdaptor_TransformedSurface(const Handle(Geom_Surface)& theSurface,
                                                 const gp_Trsf&              theTrsf);

  //! Creates a surface adaptor with transformation and parameter bounds.
  //! @param theSurface underlying geometry
  //! @param theUFirst minimum U parameter
  //! @param theULast maximum U parameter
  //! @param theVFirst minimum V parameter
  //! @param theVLast maximum V parameter
  //! @param theTrsf transformation to apply
  //! @param theTolU tolerance in U direction
  //! @param theTolV tolerance in V direction
  Standard_EXPORT GeomAdaptor_TransformedSurface(const Handle(Geom_Surface)& theSurface,
                                                 const Standard_Real         theUFirst,
                                                 const Standard_Real         theULast,
                                                 const Standard_Real         theVFirst,
                                                 const Standard_Real         theVLast,
                                                 const gp_Trsf&              theTrsf,
                                                 const Standard_Real         theTolU = 0.0,
                                                 const Standard_Real         theTolV = 0.0);

  //! Shallow copy of adaptor.
  Standard_EXPORT virtual Handle(Adaptor3d_Surface) ShallowCopy() const Standard_OVERRIDE;

  //! Loads the surface geometry.
  //! @param theSurface underlying geometry
  void Load(const Handle(Geom_Surface)& theSurface) { mySurf.Load(theSurface); }

  //! Loads the surface geometry with parameter bounds.
  //! @param theSurface underlying geometry
  //! @param theUFirst minimum U parameter
  //! @param theULast maximum U parameter
  //! @param theVFirst minimum V parameter
  //! @param theVLast maximum V parameter
  //! @param theTolU tolerance in U direction
  //! @param theTolV tolerance in V direction
  void Load(const Handle(Geom_Surface)& theSurface,
            const Standard_Real         theUFirst,
            const Standard_Real         theULast,
            const Standard_Real         theVFirst,
            const Standard_Real         theVLast,
            const Standard_Real         theTolU = 0.0,
            const Standard_Real         theTolV = 0.0)
  {
    mySurf.Load(theSurface, theUFirst, theULast, theVFirst, theVLast, theTolU, theTolV);
  }

  //! Sets the transformation.
  //! @param theTrsf transformation to apply
  void SetTrsf(const gp_Trsf& theTrsf) { myTrsf = theTrsf; }

  //! Returns the transformation.
  const gp_Trsf& Trsf() const { return myTrsf; }

  //! Returns the underlying GeomAdaptor_Surface.
  const GeomAdaptor_Surface& Surface() const { return mySurf; }

  //! Returns the underlying GeomAdaptor_Surface for modification.
  GeomAdaptor_Surface& ChangeSurface() { return mySurf; }

  //! Returns the underlying Geom_Surface.
  const Handle(Geom_Surface)& GeomSurface() const { return mySurf.Surface(); }

  // Parameter range methods - delegate to underlying surface
  virtual Standard_Real FirstUParameter() const Standard_OVERRIDE
  {
    return mySurf.FirstUParameter();
  }

  virtual Standard_Real LastUParameter() const Standard_OVERRIDE { return mySurf.LastUParameter(); }

  virtual Standard_Real FirstVParameter() const Standard_OVERRIDE
  {
    return mySurf.FirstVParameter();
  }

  virtual Standard_Real LastVParameter() const Standard_OVERRIDE { return mySurf.LastVParameter(); }

  virtual GeomAbs_Shape UContinuity() const Standard_OVERRIDE { return mySurf.UContinuity(); }

  virtual GeomAbs_Shape VContinuity() const Standard_OVERRIDE { return mySurf.VContinuity(); }

  virtual Standard_Integer NbUIntervals(const GeomAbs_Shape theS) const Standard_OVERRIDE
  {
    return mySurf.NbUIntervals(theS);
  }

  virtual Standard_Integer NbVIntervals(const GeomAbs_Shape theS) const Standard_OVERRIDE
  {
    return mySurf.NbVIntervals(theS);
  }

  Standard_EXPORT void UIntervals(TColStd_Array1OfReal& theT,
                                  const GeomAbs_Shape   theS) const Standard_OVERRIDE;

  Standard_EXPORT void VIntervals(TColStd_Array1OfReal& theT,
                                  const GeomAbs_Shape   theS) const Standard_OVERRIDE;

  Standard_EXPORT Handle(Adaptor3d_Surface) UTrim(const Standard_Real theFirst,
                                                  const Standard_Real theLast,
                                                  const Standard_Real theTol) const
    Standard_OVERRIDE;

  Standard_EXPORT Handle(Adaptor3d_Surface) VTrim(const Standard_Real theFirst,
                                                  const Standard_Real theLast,
                                                  const Standard_Real theTol) const
    Standard_OVERRIDE;

  virtual Standard_Boolean IsUClosed() const Standard_OVERRIDE { return mySurf.IsUClosed(); }

  virtual Standard_Boolean IsVClosed() const Standard_OVERRIDE { return mySurf.IsVClosed(); }

  virtual Standard_Boolean IsUPeriodic() const Standard_OVERRIDE { return mySurf.IsUPeriodic(); }

  virtual Standard_Real UPeriod() const Standard_OVERRIDE { return mySurf.UPeriod(); }

  virtual Standard_Boolean IsVPeriodic() const Standard_OVERRIDE { return mySurf.IsVPeriodic(); }

  virtual Standard_Real VPeriod() const Standard_OVERRIDE { return mySurf.VPeriod(); }

  //! Computes the point of parameters U,V on the surface.
  //! Applies transformation after evaluation.
  Standard_EXPORT gp_Pnt Value(const Standard_Real theU,
                               const Standard_Real theV) const Standard_OVERRIDE final;

  //! Computes the point of parameters U,V on the surface.
  //! Applies transformation after evaluation.
  Standard_EXPORT void D0(const Standard_Real theU,
                          const Standard_Real theV,
                          gp_Pnt&             theP) const Standard_OVERRIDE final;

  //! Computes the point and the first derivatives on the surface.
  //! Applies transformation after evaluation.
  Standard_EXPORT void D1(const Standard_Real theU,
                          const Standard_Real theV,
                          gp_Pnt&             theP,
                          gp_Vec&             theD1U,
                          gp_Vec&             theD1V) const Standard_OVERRIDE final;

  //! Computes the point, the first and second derivatives on the surface.
  //! Applies transformation after evaluation.
  Standard_EXPORT void D2(const Standard_Real theU,
                          const Standard_Real theV,
                          gp_Pnt&             theP,
                          gp_Vec&             theD1U,
                          gp_Vec&             theD1V,
                          gp_Vec&             theD2U,
                          gp_Vec&             theD2V,
                          gp_Vec&             theD2UV) const Standard_OVERRIDE final;

  //! Computes the point, the first, second and third derivatives on the surface.
  //! Applies transformation after evaluation.
  Standard_EXPORT void D3(const Standard_Real theU,
                          const Standard_Real theV,
                          gp_Pnt&             theP,
                          gp_Vec&             theD1U,
                          gp_Vec&             theD1V,
                          gp_Vec&             theD2U,
                          gp_Vec&             theD2V,
                          gp_Vec&             theD2UV,
                          gp_Vec&             theD3U,
                          gp_Vec&             theD3V,
                          gp_Vec&             theD3UUV,
                          gp_Vec&             theD3UVV) const Standard_OVERRIDE final;

  //! Computes the derivative of order Nu in the direction U and Nv in the direction V.
  //! Applies transformation after evaluation.
  Standard_EXPORT gp_Vec DN(const Standard_Real    theU,
                            const Standard_Real    theV,
                            const Standard_Integer theNu,
                            const Standard_Integer theNv) const Standard_OVERRIDE final;

  virtual Standard_Real UResolution(const Standard_Real theR3d) const Standard_OVERRIDE
  {
    return mySurf.UResolution(theR3d);
  }

  virtual Standard_Real VResolution(const Standard_Real theR3d) const Standard_OVERRIDE
  {
    return mySurf.VResolution(theR3d);
  }

  virtual GeomAbs_SurfaceType GetType() const Standard_OVERRIDE { return mySurf.GetType(); }

  Standard_EXPORT gp_Pln Plane() const Standard_OVERRIDE;

  Standard_EXPORT gp_Cylinder Cylinder() const Standard_OVERRIDE;

  Standard_EXPORT gp_Cone Cone() const Standard_OVERRIDE;

  Standard_EXPORT gp_Sphere Sphere() const Standard_OVERRIDE;

  Standard_EXPORT gp_Torus Torus() const Standard_OVERRIDE;

  virtual Standard_Integer UDegree() const Standard_OVERRIDE { return mySurf.UDegree(); }

  virtual Standard_Integer NbUPoles() const Standard_OVERRIDE { return mySurf.NbUPoles(); }

  virtual Standard_Integer VDegree() const Standard_OVERRIDE { return mySurf.VDegree(); }

  virtual Standard_Integer NbVPoles() const Standard_OVERRIDE { return mySurf.NbVPoles(); }

  virtual Standard_Integer NbUKnots() const Standard_OVERRIDE { return mySurf.NbUKnots(); }

  virtual Standard_Integer NbVKnots() const Standard_OVERRIDE { return mySurf.NbVKnots(); }

  virtual Standard_Boolean IsURational() const Standard_OVERRIDE { return mySurf.IsURational(); }

  virtual Standard_Boolean IsVRational() const Standard_OVERRIDE { return mySurf.IsVRational(); }

  Standard_EXPORT Handle(Geom_BezierSurface) Bezier() const Standard_OVERRIDE;

  Standard_EXPORT Handle(Geom_BSplineSurface) BSpline() const Standard_OVERRIDE;

  Standard_EXPORT gp_Ax1 AxeOfRevolution() const Standard_OVERRIDE;

  Standard_EXPORT gp_Dir Direction() const Standard_OVERRIDE;

  Standard_EXPORT Handle(Adaptor3d_Curve) BasisCurve() const Standard_OVERRIDE;

  Standard_EXPORT Handle(Adaptor3d_Surface) BasisSurface() const Standard_OVERRIDE;

  Standard_EXPORT Standard_Real OffsetValue() const Standard_OVERRIDE;

protected:
  GeomAdaptor_Surface mySurf;
  gp_Trsf             myTrsf;
};

#endif // _GeomAdaptor_TransformedSurface_HeaderFile
