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

#include <optional>
#include <variant>

class Geom_BezierSurface;
class Geom_BSplineSurface;
class Geom_Surface;

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
  //! Cached transformed data associated with the current transformation.
  struct TransformedSurfaceData
  {
    using PrimitiveData =
      std::variant<std::monostate, gp_Pln, gp_Cylinder, gp_Cone, gp_Sphere, gp_Torus>;

    PrimitiveData                  Primitive;
    occ::handle<Geom_Surface>      Surface;
    occ::handle<Adaptor3d_Curve>   BasisCurve;
    occ::handle<Adaptor3d_Surface> BasisSurface;
    std::optional<gp_Ax1>          Axis;
    std::optional<gp_Dir>          Direction;
    std::optional<double>          OffsetValue;
  };

  //! Creates an undefined surface with identity transformation.
  Standard_EXPORT GeomAdaptor_TransformedSurface();

  //! Creates a surface adaptor with transformation.
  //! @param theSurface underlying geometry
  //! @param theTrsf transformation to apply
  Standard_EXPORT GeomAdaptor_TransformedSurface(const occ::handle<Geom_Surface>& theSurface,
                                                 const gp_Trsf&                   theTrsf);

  //! Creates a surface adaptor with transformation and parameter bounds.
  //! @param theSurface underlying geometry
  //! @param theUFirst minimum U parameter
  //! @param theULast maximum U parameter
  //! @param theVFirst minimum V parameter
  //! @param theVLast maximum V parameter
  //! @param theTrsf transformation to apply
  //! @param theTolU tolerance in U direction
  //! @param theTolV tolerance in V direction
  Standard_EXPORT GeomAdaptor_TransformedSurface(const occ::handle<Geom_Surface>& theSurface,
                                                 const double                     theUFirst,
                                                 const double                     theULast,
                                                 const double                     theVFirst,
                                                 const double                     theVLast,
                                                 const gp_Trsf&                   theTrsf,
                                                 const double                     theTolU = 0.0,
                                                 const double                     theTolV = 0.0);

  //! Shallow copy of adaptor.
  Standard_EXPORT occ::handle<Adaptor3d_Surface> ShallowCopy() const override;

  //! Loads the surface geometry.
  //! @param theSurface underlying geometry
  //! @param theTrsf transformation to apply
  Standard_EXPORT void Load(const occ::handle<Geom_Surface>& theSurface,
                            const gp_Trsf&                   theTrsf = gp_Trsf());

  //! Loads the surface geometry with parameter bounds.
  //! @param theSurface underlying geometry
  //! @param theUFirst minimum U parameter
  //! @param theULast maximum U parameter
  //! @param theVFirst minimum V parameter
  //! @param theVLast maximum V parameter
  //! @param theTolU tolerance in U direction
  //! @param theTolV tolerance in V direction
  //! @param theTrsf transformation to apply
  Standard_EXPORT void Load(const occ::handle<Geom_Surface>& theSurface,
                            const double                     theUFirst,
                            const double                     theULast,
                            const double                     theVFirst,
                            const double                     theVLast,
                            const double                     theTolU = 0.0,
                            const double                     theTolV = 0.0,
                            const gp_Trsf&                   theTrsf = gp_Trsf());

  //! Sets the transformation.
  //! @param theTrsf transformation to apply
  Standard_EXPORT void SetTrsf(const gp_Trsf& theTrsf);

  //! Returns true if non-identity transformation is applied.
  bool HasTrsf() const { return myTrsf.Form() != gp_Identity; }

  //! Returns the transformation.
  const gp_Trsf& Trsf() const { return myTrsf; }

  //! Returns the underlying GeomAdaptor_Surface.
  const GeomAdaptor_Surface& Surface() const { return mySurf; }

  //! Returns the underlying original Geom_Surface without transformation applied.
  const occ::handle<Geom_Surface>& GeomSurfaceOriginal() const { return mySurf.Surface(); }

  //! Returns the transformed Geom_Surface cached for current state.
  Standard_EXPORT const occ::handle<Geom_Surface>& GeomSurfaceTransformed() const;

  //! Returns the underlying Geom_Surface.
  Standard_DEPRECATED("Use GeomSurfaceOriginal() or GeomSurfaceTransformed() instead")

  const occ::handle<Geom_Surface>& GeomSurface() const { return GeomSurfaceOriginal(); }

  // Parameter range methods - delegate to underlying surface
  double FirstUParameter() const override { return mySurf.FirstUParameter(); }

  double LastUParameter() const override { return mySurf.LastUParameter(); }

  double FirstVParameter() const override { return mySurf.FirstVParameter(); }

  double LastVParameter() const override { return mySurf.LastVParameter(); }

  GeomAbs_Shape UContinuity() const override { return mySurf.UContinuity(); }

  GeomAbs_Shape VContinuity() const override { return mySurf.VContinuity(); }

  int NbUIntervals(const GeomAbs_Shape theS) const override { return mySurf.NbUIntervals(theS); }

  int NbVIntervals(const GeomAbs_Shape theS) const override { return mySurf.NbVIntervals(theS); }

  Standard_EXPORT void UIntervals(NCollection_Array1<double>& theT,
                                  const GeomAbs_Shape         theS) const override;

  Standard_EXPORT void VIntervals(NCollection_Array1<double>& theT,
                                  const GeomAbs_Shape         theS) const override;

  Standard_EXPORT occ::handle<Adaptor3d_Surface> UTrim(const double theFirst,
                                                       const double theLast,
                                                       const double theTol) const override;

  Standard_EXPORT occ::handle<Adaptor3d_Surface> VTrim(const double theFirst,
                                                       const double theLast,
                                                       const double theTol) const override;

  bool IsUClosed() const override { return mySurf.IsUClosed(); }

  bool IsVClosed() const override { return mySurf.IsVClosed(); }

  bool IsUPeriodic() const override { return mySurf.IsUPeriodic(); }

  double UPeriod() const override { return mySurf.UPeriod(); }

  bool IsVPeriodic() const override { return mySurf.IsVPeriodic(); }

  double VPeriod() const override { return mySurf.VPeriod(); }

  //! Point evaluation. Applies transformation after evaluation.
  [[nodiscard]] Standard_EXPORT gp_Pnt EvalD0(const double theU, const double theV) const final;

  //! D1 evaluation. Applies transformation after evaluation.
  [[nodiscard]] Standard_EXPORT Geom_Surface::ResD1 EvalD1(const double theU,
                                                           const double theV) const final;

  //! D2 evaluation. Applies transformation after evaluation.
  [[nodiscard]] Standard_EXPORT Geom_Surface::ResD2 EvalD2(const double theU,
                                                           const double theV) const final;

  //! D3 evaluation. Applies transformation after evaluation.
  [[nodiscard]] Standard_EXPORT Geom_Surface::ResD3 EvalD3(const double theU,
                                                           const double theV) const final;

  //! DN evaluation. Applies transformation after evaluation.
  [[nodiscard]] Standard_EXPORT gp_Vec EvalDN(const double theU,
                                              const double theV,
                                              const int    theNu,
                                              const int    theNv) const final;

  double UResolution(const double theR3d) const override { return mySurf.UResolution(theR3d); }

  double VResolution(const double theR3d) const override { return mySurf.VResolution(theR3d); }

  GeomAbs_SurfaceType GetType() const override { return mySurf.GetType(); }

  Standard_EXPORT gp_Pln Plane() const override;

  Standard_EXPORT gp_Cylinder Cylinder() const override;

  Standard_EXPORT gp_Cone Cone() const override;

  Standard_EXPORT gp_Sphere Sphere() const override;

  Standard_EXPORT gp_Torus Torus() const override;

  int UDegree() const override { return mySurf.UDegree(); }

  int NbUPoles() const override { return mySurf.NbUPoles(); }

  int VDegree() const override { return mySurf.VDegree(); }

  int NbVPoles() const override { return mySurf.NbVPoles(); }

  int NbUKnots() const override { return mySurf.NbUKnots(); }

  int NbVKnots() const override { return mySurf.NbVKnots(); }

  bool IsURational() const override { return mySurf.IsURational(); }

  bool IsVRational() const override { return mySurf.IsVRational(); }

  Standard_EXPORT occ::handle<Geom_BezierSurface> Bezier() const override;

  Standard_EXPORT occ::handle<Geom_BSplineSurface> BSpline() const override;

  Standard_EXPORT gp_Ax1 AxeOfRevolution() const override;

  Standard_EXPORT gp_Dir Direction() const override;

  Standard_EXPORT occ::handle<Adaptor3d_Curve> BasisCurve() const override;

  Standard_EXPORT occ::handle<Adaptor3d_Surface> BasisSurface() const override;

  Standard_EXPORT double OffsetValue() const override;

protected:
  //! Rebuilds cached transformed geometry and related metadata
  //! for the currently loaded surface and transformation.
  void initTransformedCache();

protected:
  GeomAdaptor_Surface            mySurf;
  gp_Trsf                        myTrsf;
  mutable TransformedSurfaceData myTransformedData;
};

#endif // _GeomAdaptor_TransformedSurface_HeaderFile
