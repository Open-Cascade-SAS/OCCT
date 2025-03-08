// Created on: 1999-06-22
// Created by: Roman LYGIN
// Copyright (c) 1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _ShapeExtend_ComplexCurve_HeaderFile
#define _ShapeExtend_ComplexCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Geom_Curve.hxx>
#include <Standard_Integer.hxx>
#include <GeomAbs_Shape.hxx>
class gp_Trsf;
class gp_Pnt;
class gp_Vec;

class ShapeExtend_ComplexCurve;
DEFINE_STANDARD_HANDLE(ShapeExtend_ComplexCurve, Geom_Curve)

//! Defines a curve which consists of several segments.
//! Implements basic interface to it.
class ShapeExtend_ComplexCurve : public Geom_Curve
{

public:
  //! Returns number of curves
  Standard_EXPORT virtual Standard_Integer NbCurves() const = 0;

  //! Returns curve given by its index
  Standard_EXPORT virtual const Handle(Geom_Curve)& Curve(const Standard_Integer index) const = 0;

  //! Returns number of the curve for the given parameter U
  //! and local parameter UOut for the found curve
  Standard_EXPORT virtual Standard_Integer LocateParameter(const Standard_Real U,
                                                           Standard_Real&      UOut) const = 0;

  //! Returns global parameter for the whole curve according
  //! to the segment and local parameter on it
  Standard_EXPORT virtual Standard_Real LocalToGlobal(const Standard_Integer index,
                                                      const Standard_Real    Ulocal) const = 0;

  //! Applies transformation to each curve
  Standard_EXPORT virtual void Transform(const gp_Trsf& T) Standard_OVERRIDE;

  //! Returns 1 - U
  virtual Standard_Real ReversedParameter(const Standard_Real U) const Standard_OVERRIDE;

  //! Returns 0
  virtual Standard_Real FirstParameter() const Standard_OVERRIDE;

  //! Returns 1
  virtual Standard_Real LastParameter() const Standard_OVERRIDE;

  //! Returns True if the curve is closed
  virtual Standard_Boolean IsClosed() const Standard_OVERRIDE;

  //! Returns False
  virtual Standard_Boolean IsPeriodic() const Standard_OVERRIDE;

  //! Returns GeomAbs_C0
  virtual GeomAbs_Shape Continuity() const Standard_OVERRIDE;

  //! Returns False if N > 0
  virtual Standard_Boolean IsCN(const Standard_Integer N) const Standard_OVERRIDE;

  //! Returns point at parameter U.
  //! Finds appropriate curve and local parameter on it.
  Standard_EXPORT virtual void D0(const Standard_Real U, gp_Pnt& P) const Standard_OVERRIDE;

  Standard_EXPORT virtual void D1(const Standard_Real U,
                                  gp_Pnt&             P,
                                  gp_Vec&             V1) const Standard_OVERRIDE;

  Standard_EXPORT virtual void D2(const Standard_Real U,
                                  gp_Pnt&             P,
                                  gp_Vec&             V1,
                                  gp_Vec&             V2) const Standard_OVERRIDE;

  Standard_EXPORT virtual void D3(const Standard_Real U,
                                  gp_Pnt&             P,
                                  gp_Vec&             V1,
                                  gp_Vec&             V2,
                                  gp_Vec&             V3) const Standard_OVERRIDE;

  Standard_EXPORT virtual gp_Vec DN(const Standard_Real    U,
                                    const Standard_Integer N) const Standard_OVERRIDE;

  //! Returns scale factor for recomputing of deviatives.
  Standard_EXPORT virtual Standard_Real GetScaleFactor(const Standard_Integer ind) const = 0;

  //! Checks geometrical connectivity of the curves, including
  //! closure (sets fields myClosed)
  Standard_EXPORT Standard_Boolean CheckConnectivity(const Standard_Real Preci);

  DEFINE_STANDARD_RTTIEXT(ShapeExtend_ComplexCurve, Geom_Curve)

protected:
  Standard_EXPORT ShapeExtend_ComplexCurve();

  //! Transform the derivative according to its order
  Standard_EXPORT void TransformDN(gp_Vec&                V,
                                   const Standard_Integer ind,
                                   const Standard_Integer N) const;

  Standard_Boolean myClosed;

private:
};

#include <ShapeExtend_ComplexCurve.lxx>

#endif // _ShapeExtend_ComplexCurve_HeaderFile
