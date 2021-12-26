// Created by: Julia GERASIMOVA
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

#ifndef _BlendFunc_GenChamfer_HeaderFile
#define _BlendFunc_GenChamfer_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <Blend_Function.hxx>
#include <math_Vector.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <GeomAbs_Shape.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array1OfVec2d.hxx>

class math_Matrix;
class gp_Lin;
class Blend_Point;

//! Deferred class for a function used to compute a general chamfer
class BlendFunc_GenChamfer  : public Blend_Function
{
public:

  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BlendFunc_GenChamfer(const Handle(Adaptor3d_Surface)& S1,
                                       const Handle(Adaptor3d_Surface)& S2,
                                       const Handle(Adaptor3d_Curve)& CG);
  
  //! returns the number of equations of the function.
  Standard_EXPORT Standard_Integer NbEquations() const Standard_OVERRIDE;
  
  //! returns the values <F> of the functions and the derivatives
  //! <D> for the variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean Values (const math_Vector& X,
                                           math_Vector& F,
                                           math_Matrix& D) Standard_OVERRIDE;
  
  //! Sets the value of the parameter along the guide line.
  //! This determines the plane in which the solution has
  //! to be found.
  Standard_EXPORT void Set (const Standard_Real Param) Standard_OVERRIDE = 0;
  
  //! Sets the bounds of the parametric interval on
  //! the guide line.
  //! This determines the derivatives in these values if the
  //! function is not Cn.
  Standard_EXPORT void Set (const Standard_Real First, const Standard_Real Last) Standard_OVERRIDE;
  
  Standard_EXPORT void GetTolerance (math_Vector& Tolerance,
                                     const Standard_Real Tol) const Standard_OVERRIDE;
  
  Standard_EXPORT void GetBounds (math_Vector& InfBound,
                                  math_Vector& SupBound) const Standard_OVERRIDE;
  
  //! Returns   the    minimal  Distance  between   two
  //! extremities of calculated sections.
  Standard_EXPORT Standard_Real GetMinimalDistance() const Standard_OVERRIDE;
  
  //! Sets the distances and the "quadrant".
  Standard_EXPORT virtual void Set (const Standard_Real Dist1,
                                    const Standard_Real Dist2,
                                    const Standard_Integer Choix) = 0;
  
  //! Returns False
  Standard_EXPORT Standard_Boolean IsRational() const Standard_OVERRIDE;
  
  //! Compute the minimal value of weight for each poles
  //! of all sections.
  Standard_EXPORT void GetMinimalWeight (TColStd_Array1OfReal& Weigths) const Standard_OVERRIDE;
  
  //! Returns  the number  of  intervals for  continuity
  //! <S>. May be one if Continuity(me) >= <S>
  Standard_EXPORT Standard_Integer NbIntervals (const GeomAbs_Shape S) const Standard_OVERRIDE;
  
  //! Stores in <T> the  parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide  enough room to  accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  //! raises
  //! OutOfRange from Standard
  Standard_EXPORT void Intervals (TColStd_Array1OfReal& T,
                                  const GeomAbs_Shape S) const Standard_OVERRIDE;
  
  Standard_EXPORT void GetShape (Standard_Integer& NbPoles,
                                 Standard_Integer& NbKnots,
                                 Standard_Integer& Degree,
                                 Standard_Integer& NbPoles2d) Standard_OVERRIDE;
  
  //! Returns the tolerance to reach in approximation
  //! to respecte
  //! BoundTol error at the Boundary
  //! AngleTol tangent error at the Boundary
  //! SurfTol error inside the surface.
  Standard_EXPORT void GetTolerance (const Standard_Real BoundTol,
                                     const Standard_Real SurfTol,
                                     const Standard_Real AngleTol,
                                     math_Vector& Tol3d,
                                     math_Vector& Tol1D) const Standard_OVERRIDE;
  
  Standard_EXPORT void Knots (TColStd_Array1OfReal& TKnots) Standard_OVERRIDE;
  
  Standard_EXPORT void Mults (TColStd_Array1OfInteger& TMults) Standard_OVERRIDE;
  
  //! Obsolete method
  Standard_EXPORT void Section (const Standard_Real Param,
                                const Standard_Real U1,
                                const Standard_Real V1,
                                const Standard_Real U2,
                                const Standard_Real V2,
                                Standard_Real& Pdeb,
                                Standard_Real& Pfin,
                                gp_Lin& C);
  
  //! Used for the first and last section
  Standard_EXPORT Standard_Boolean Section (const Blend_Point& P,
                                            TColgp_Array1OfPnt& Poles,
                                            TColgp_Array1OfVec& DPoles,
                                            TColgp_Array1OfVec& D2Poles,
                                            TColgp_Array1OfPnt2d& Poles2d,
                                            TColgp_Array1OfVec2d& DPoles2d,
                                            TColgp_Array1OfVec2d& D2Poles2d,
                                            TColStd_Array1OfReal& Weigths,
                                            TColStd_Array1OfReal& DWeigths,
                                            TColStd_Array1OfReal& D2Weigths) Standard_OVERRIDE;
  
  //! Used for the first and last section
  Standard_EXPORT Standard_Boolean Section (const Blend_Point& P,
                                            TColgp_Array1OfPnt& Poles,
                                            TColgp_Array1OfVec& DPoles,
                                            TColgp_Array1OfPnt2d& Poles2d,
                                            TColgp_Array1OfVec2d& DPoles2d,
                                            TColStd_Array1OfReal& Weigths,
                                            TColStd_Array1OfReal& DWeigths) Standard_OVERRIDE;
  
  Standard_EXPORT void Section (const Blend_Point& P,
                                TColgp_Array1OfPnt& Poles,
                                TColgp_Array1OfPnt2d& Poles2d,
                                TColStd_Array1OfReal& Weigths) Standard_OVERRIDE;
  
  Standard_EXPORT void Resolution (const Standard_Integer IC2d,
                                   const Standard_Real Tol,
                                   Standard_Real& TolU,
                                   Standard_Real& TolV) const Standard_OVERRIDE;




protected:

  Handle(Adaptor3d_Surface) surf1;
  Handle(Adaptor3d_Surface) surf2;
  Handle(Adaptor3d_Curve) curv;
  Standard_Integer choix;
  Standard_Real tol;
  Standard_Real distmin;


private:


};







#endif // _BlendFunc_GenChamfer_HeaderFile
