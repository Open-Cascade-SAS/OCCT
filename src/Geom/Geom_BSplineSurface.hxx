// Created on: 1993-03-09
// Created by: JCV
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _Geom_BSplineSurface_HeaderFile
#define _Geom_BSplineSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <GeomAbs_BSplKnotDistribution.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <TColgp_HArray2OfPnt.hxx>
#include <TColStd_HArray2OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <Standard_Real.hxx>
#include <Geom_BoundedSurface.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TColgp_Array1OfPnt.hxx>
class Standard_ConstructionError;
class Standard_DimensionError;
class Standard_DomainError;
class Standard_OutOfRange;
class Standard_NoSuchObject;
class Standard_RangeError;
class Geom_UndefinedDerivative;
class gp_Pnt;
class gp_Vec;
class Geom_Curve;
class gp_Trsf;
class Geom_Geometry;


class Geom_BSplineSurface;
DEFINE_STANDARD_HANDLE(Geom_BSplineSurface, Geom_BoundedSurface)

//! Describes a BSpline surface.
//! In each parametric direction, a BSpline surface can be:
//! - uniform or non-uniform,
//! - rational or non-rational,
//! - periodic or non-periodic.
//! A BSpline surface is defined by:
//! - its degrees, in the u and v parametric directions,
//! - its periodic characteristic, in the u and v parametric directions,
//! - a table of poles, also called control points (together
//! with the associated weights if the surface is rational), and
//! - a table of knots, together with the associated multiplicities.
//! The degree of a Geom_BSplineSurface is limited to
//! a value (25) which is defined and controlled by the
//! system. This value is returned by the function MaxDegree.
//! Poles and Weights
//! Poles and Weights are manipulated using two associative double arrays:
//! - the poles table, which is a double array of gp_Pnt points, and
//! - the weights table, which is a double array of reals.
//! The bounds of the poles and weights arrays are:
//! - 1 and NbUPoles for the row bounds (provided
//! that the BSpline surface is not periodic in the u
//! parametric direction), where NbUPoles is the
//! number of poles of the surface in the u parametric direction, and
//! - 1 and NbVPoles for the column bounds (provided
//! that the BSpline surface is not periodic in the v
//! parametric direction), where NbVPoles is the
//! number of poles of the surface in the v parametric direction.
//! The poles of the surface are the points used to shape
//! and reshape the surface. They comprise a rectangular network.
//! If the surface is not periodic:
//! - The points (1, 1), (NbUPoles, 1), (1,
//! NbVPoles), and (NbUPoles, NbVPoles)
//! are the four parametric "corners" of the surface.
//! - The first column of poles and the last column of
//! poles define two BSpline curves which delimit the
//! surface in the v parametric direction. These are the
//! v isoparametric curves corresponding to the two
//! bounds of the v parameter.
//! - The first row of poles and the last row of poles
//! define two BSpline curves which delimit the surface
//! in the u parametric direction. These are the u
//! isoparametric curves corresponding to the two bounds of the u parameter.
//! If the surface is periodic, these geometric properties are not verified.
//! It is more difficult to define a geometrical significance
//! for the weights. However they are useful for
//! representing a quadric surface precisely. Moreover, if
//! the weights of all the poles are equal, the surface has
//! a polynomial equation, and hence is a "non-rational surface".
//! The non-rational surface is a special, but frequently
//! used, case, where all poles have identical weights.
//! The weights are defined and used only in the case of
//! a rational surface. The rational characteristic is
//! defined in each parametric direction. A surface can be
//! rational in the u parametric direction, and
//! non-rational in the v parametric direction.
//! Knots and Multiplicities
//! For a Geom_BSplineSurface the table of knots is
//! made up of two increasing sequences of reals, without
//! repetition, one for each parametric direction. The
//! multiplicities define the repetition of the knots.
//! A BSpline surface comprises multiple contiguous
//! patches, which are themselves polynomial or rational
//! surfaces. The knots are the parameters of the
//! isoparametric curves which limit these contiguous
//! patches. The multiplicity of a knot on a BSpline
//! surface (in a given parametric direction) is related to
//! the degree of continuity of the surface at that knot in
//! that parametric direction:
//! Degree of continuity at knot(i) = Degree - Multi(i) where:
//! - Degree is the degree of the BSpline surface in
//! the given parametric direction, and
//! - Multi(i) is the multiplicity of knot number i in
//! the given parametric direction.
//! There are some special cases, where the knots are
//! regularly spaced in one parametric direction (i.e. the
//! difference between two consecutive knots is a constant).
//! - "Uniform": all the multiplicities are equal to 1.
//! - "Quasi-uniform": all the multiplicities are equal to 1,
//! except for the first and last knots in this parametric
//! direction, and these are equal to Degree + 1.
//! - "Piecewise Bezier": all the multiplicities are equal to
//! Degree except for the first and last knots, which
//! are equal to Degree + 1. This surface is a
//! concatenation of Bezier patches in the given
//! parametric direction.
//! If the BSpline surface is not periodic in a given
//! parametric direction, the bounds of the knots and
//! multiplicities tables are 1 and NbKnots, where
//! NbKnots is the number of knots of the BSpline
//! surface in that parametric direction.
//! If the BSpline surface is periodic in a given parametric
//! direction, and there are k periodic knots and p
//! periodic poles in that parametric direction:
//! - the period is such that:
//! period = Knot(k+1) - Knot(1), and
//! - the poles and knots tables in that parametric
//! direction can be considered as infinite tables, such that:
//! Knot(i+k) = Knot(i) + period, and
//! Pole(i+p) = Pole(i)
//! Note: The data structure tables for a periodic BSpline
//! surface are more complex than those of a non-periodic one.
//! References :
//! . A survey of curve and surface methods in CADG Wolfgang BOHM
//! CAGD 1 (1984)
//! . On de Boor-like algorithms and blossoming Wolfgang BOEHM
//! cagd 5 (1988)
//! . Blossoming and knot insertion algorithms for B-spline curves
//! Ronald N. GOLDMAN
//! . Modelisation des surfaces en CAO, Henri GIAUME Peugeot SA
//! . Curves and Surfaces for Computer Aided Geometric Design,
//! a practical guide Gerald Farin
class Geom_BSplineSurface : public Geom_BoundedSurface
{

public:

  
  //! Creates  a non-rational b-spline surface (weights
  //! default value is 1.).
  //! The following conditions must be verified.
  //! 0 < UDegree <= MaxDegree.
  //! UKnots.Length() == UMults.Length() >= 2
  //! UKnots(i) < UKnots(i+1) (Knots are increasing)
  //! 1 <= UMults(i) <= UDegree
  //! On a   non  uperiodic   surface    the  first and    last
  //! umultiplicities  may  be     UDegree+1  (this   is   even
  //! recommanded if you want the curve  to start and finish on
  //! the first and last pole).
  //! On a uperiodic     surface  the first    and   the   last
  //! umultiplicities must be the same.
  //! on non-uperiodic surfaces
  //! Poles.ColLength() == Sum(UMults(i)) - UDegree - 1 >= 2
  //! on uperiodic surfaces
  //! Poles.ColLength() == Sum(UMults(i)) except the first or last
  //! The previous conditions for U holds  also for V, with the
  //! RowLength of the poles.
  Standard_EXPORT Geom_BSplineSurface(const TColgp_Array2OfPnt& Poles, const TColStd_Array1OfReal& UKnots, const TColStd_Array1OfReal& VKnots, const TColStd_Array1OfInteger& UMults, const TColStd_Array1OfInteger& VMults, const Standard_Integer UDegree, const Standard_Integer VDegree, const Standard_Boolean UPeriodic = Standard_False, const Standard_Boolean VPeriodic = Standard_False);
  
  //! Creates  a non-rational b-spline surface (weights
  //! default value is 1.).
  //!
  //! The following conditions must be verified.
  //! 0 < UDegree <= MaxDegree.
  //!
  //! UKnots.Length() == UMults.Length() >= 2
  //!
  //! UKnots(i) < UKnots(i+1) (Knots are increasing)
  //! 1 <= UMults(i) <= UDegree
  //!
  //! On a   non  uperiodic   surface    the  first and    last
  //! umultiplicities  may  be     UDegree+1  (this   is   even
  //! recommanded if you want the curve  to start and finish on
  //! the first and last pole).
  //!
  //! On a uperiodic     surface  the first    and   the   last
  //! umultiplicities must be the same.
  //!
  //! on non-uperiodic surfaces
  //!
  //! Poles.ColLength() == Sum(UMults(i)) - UDegree - 1 >= 2
  //!
  //! on uperiodic surfaces
  //!
  //! Poles.ColLength() == Sum(UMults(i)) except the first or
  //! last
  //!
  //! The previous conditions for U holds  also for V, with the
  //! RowLength of the poles.
  Standard_EXPORT Geom_BSplineSurface(const TColgp_Array2OfPnt& Poles, const TColStd_Array2OfReal& Weights, const TColStd_Array1OfReal& UKnots, const TColStd_Array1OfReal& VKnots, const TColStd_Array1OfInteger& UMults, const TColStd_Array1OfInteger& VMults, const Standard_Integer UDegree, const Standard_Integer VDegree, const Standard_Boolean UPeriodic = Standard_False, const Standard_Boolean VPeriodic = Standard_False);
  
  //! Exchanges the u and v parametric directions on
  //! this BSpline surface.
  //! As a consequence:
  //! - the poles and weights tables are transposed,
  //! - the knots and multiplicities tables are exchanged,
  //! - degrees of continuity, and rational, periodic and
  //! uniform characteristics are exchanged, and
  //! - the orientation of the surface is inverted.
  Standard_EXPORT void ExchangeUV();
  
  //! Sets the surface U periodic.
  //! Modifies this surface to be periodic in the U 
  //! parametric direction.
  //! To become periodic in a given parametric direction a
  //! surface must be closed in that parametric direction,
  //! and the knot sequence relative to that direction must be periodic.
  //! To generate this periodic sequence of knots, the
  //! functions FirstUKnotIndex and LastUKnotIndex  are used to
  //! compute I1 and I2. These are the indexes, in the
  //! knot array associated with the given parametric
  //! direction, of the knots that correspond to the first and
  //! last parameters of this BSpline surface in the given
  //! parametric direction. Hence the period is:
  //! Knots(I1) - Knots(I2)
  //! As a result, the knots and poles tables are modified.
  //! Exceptions
  //! Standard_ConstructionError if the surface is not
  //! closed in the given parametric direction.
  Standard_EXPORT void SetUPeriodic();
  
  //! Sets the surface V periodic.
  //! Modifies this surface to be periodic in the V
  //! parametric direction.
  //! To become periodic in a given parametric direction a
  //! surface must be closed in that parametric direction,
  //! and the knot sequence relative to that direction must be periodic.
  //! To generate this periodic sequence of knots, the
  //! functions FirstVKnotIndex and LastVKnotIndex are used to
  //! compute I1 and I2. These are the indexes, in the
  //! knot array associated with the given parametric
  //! direction, of the knots that correspond to the first and
  //! last parameters of this BSpline surface in the given
  //! parametric direction. Hence the period is:
  //! Knots(I1) - Knots(I2)
  //! As a result, the knots and poles tables are modified.
  //! Exceptions
  //! Standard_ConstructionError if the surface is not
  //! closed in the given parametric direction.
  Standard_EXPORT void SetVPeriodic();
  
  //! returns the parameter normalized within
  //! the period if the surface is periodic : otherwise
  //! does not do anything
  Standard_EXPORT void PeriodicNormalization (Standard_Real& U, Standard_Real& V) const;
  
  //! Assigns the knot of index Index in the knots table in
  //! the corresponding parametric direction to be the
  //! origin of this periodic BSpline surface. As a
  //! consequence, the knots and poles tables are modified.
  //! Exceptions
  //! Standard_NoSuchObject if this BSpline surface is
  //! not periodic in the given parametric direction.
  //! Standard_DomainError if Index is outside the
  //! bounds of the knots table in the given parametric direction.
  Standard_EXPORT void SetUOrigin (const Standard_Integer Index);
  
  //! Assigns the knot of index Index in the knots table in
  //! the corresponding parametric direction to be the
  //! origin of this periodic BSpline surface. As a
  //! consequence, the knots and poles tables are modified.
  //! Exceptions
  //! Standard_NoSuchObject if this BSpline surface is
  //! not periodic in the given parametric direction.
  //! Standard_DomainError if Index is outside the
  //! bounds of the knots table in the given parametric direction.
  Standard_EXPORT void SetVOrigin (const Standard_Integer Index);
  
  //! Sets the surface U not periodic.
  //! Changes this BSpline surface into a non-periodic
  //! surface along U direction. 
  //! If this surface is already non-periodic, it is not modified.
  //! Note: the poles and knots tables are modified.
  Standard_EXPORT void SetUNotPeriodic();
  
  //! Sets the surface V not periodic.
  //! Changes this BSpline surface into a non-periodic
  //! surface along V direction. 
  //! If this surface is already non-periodic, it is not modified.
  //! Note: the poles and knots tables are modified.
  Standard_EXPORT void SetVNotPeriodic();
  
  //! Changes the orientation of this BSpline surface in the
  //! U parametric direction. The bounds of the
  //! surface are not changed but the given parametric
  //! direction is reversed. Hence the orientation of the
  //! surface is reversed.
  //! The knots and poles tables are modified.
  Standard_EXPORT void UReverse() Standard_OVERRIDE;
  
  //! Changes the orientation of this BSpline surface in the
  //! V parametric direction. The bounds of the
  //! surface are not changed but the given parametric
  //! direction is reversed. Hence the orientation of the
  //! surface is reversed.
  //! The knots and poles tables are modified.
  Standard_EXPORT void VReverse() Standard_OVERRIDE;
  
  //! Computes the u parameter on the modified
  //! surface, produced by reversing its U parametric
  //! direction, for the point of u parameter U,  on this BSpline surface.
  //! For a BSpline surface, these functions return respectively:
  //! - UFirst + ULast - U, 
  //! where UFirst, ULast are
  //! the values of the first and last parameters of this
  //! BSpline surface, in the u parametric directions.
  Standard_EXPORT Standard_Real UReversedParameter (const Standard_Real U) const Standard_OVERRIDE;
  
  //! Computes the v parameter on the modified
  //! surface, produced by reversing its V parametric
  //! direction, for the point of v parameter V on this BSpline surface.
  //! For a BSpline surface, these functions return respectively:
  //! - VFirst + VLast - V,
  //! VFirst and VLast are
  //! the values of the first and last parameters of this
  //! BSpline surface, in the v pametric directions.
  Standard_EXPORT Standard_Real VReversedParameter (const Standard_Real V) const Standard_OVERRIDE;
  
  //! Increases the degrees of this BSpline surface to
  //! UDegree and VDegree in the u and v parametric
  //! directions respectively. As a result, the tables of poles,
  //! weights and multiplicities are modified. The tables of
  //! knots is not changed.
  //! Note: Nothing is done if the given degree is less than
  //! or equal to the current degree in the corresponding
  //! parametric direction.
  //! Exceptions
  //! Standard_ConstructionError if UDegree or
  //! VDegree is greater than
  //! Geom_BSplineSurface::MaxDegree().
  Standard_EXPORT void IncreaseDegree (const Standard_Integer UDegree, const Standard_Integer VDegree);
  
  //! Inserts into the knots table for the U
  //! parametric direction of this BSpline surface:
  //! - the values of the array Knots, with their respective
  //! multiplicities, Mults.
  //! If the knot value to insert already exists in the table, its multiplicity is:
  //! - increased by M, if Add is true (the default), or
  //! - increased to M, if Add is false.
  //! The tolerance criterion used to check the equality of
  //! the knots is the larger of the values ParametricTolerance and
  //! Standard_Real::Epsilon(val), where val is the knot value to be inserted.
  //! Warning
  //! - If a given multiplicity coefficient is null, or negative, nothing is done.
  //! - The new multiplicity of a knot is limited to the degree of this BSpline surface in the
  //! corresponding parametric direction.
  //! Exceptions
  //! Standard_ConstructionError if a knot value to
  //! insert is outside the bounds of this BSpline surface in
  //! the specified parametric direction. The comparison
  //! uses the precision criterion ParametricTolerance.
  Standard_EXPORT void InsertUKnots (const TColStd_Array1OfReal& Knots, const TColStd_Array1OfInteger& Mults, const Standard_Real ParametricTolerance = 0.0, const Standard_Boolean Add = Standard_True);
  
  //! Inserts into the knots table for the V
  //! parametric direction of this BSpline surface:
  //! - the values of the array Knots, with their respective
  //! multiplicities, Mults.
  //! If the knot value to insert already exists in the table, its multiplicity is:
  //! - increased by M, if Add is true (the default), or
  //! - increased to M, if Add is false.
  //! The tolerance criterion used to check the equality of
  //! the knots is the larger of the values ParametricTolerance and
  //! Standard_Real::Epsilon(val), where val is the knot value to be inserted.
  //! Warning
  //! - If a given multiplicity coefficient is null, or negative, nothing is done.
  //! - The new multiplicity of a knot is limited to the degree of this BSpline surface in the
  //! corresponding parametric direction.
  //! Exceptions
  //! Standard_ConstructionError if a knot value to
  //! insert is outside the bounds of this BSpline surface in
  //! the specified parametric direction. The comparison
  //! uses the precision criterion ParametricTolerance.
  Standard_EXPORT void InsertVKnots (const TColStd_Array1OfReal& Knots, const TColStd_Array1OfInteger& Mults, const Standard_Real ParametricTolerance = 0.0, const Standard_Boolean Add = Standard_True);
  
  //! Reduces to M the multiplicity of the knot of index
  //! Index in the U parametric direction. If M is 0, the knot is removed.
  //! With a modification of this type, the table of poles is also modified.
  //! Two different algorithms are used systematically to
  //! compute the new poles of the surface. For each
  //! pole, the distance between the pole calculated
  //! using the first algorithm and the same pole
  //! calculated using the second algorithm, is checked. If
  //! this distance is less than Tolerance it ensures that
  //! the surface is not modified by more than Tolerance.
  //! Under these conditions, the function returns true;
  //! otherwise, it returns false.
  //! A low tolerance prevents modification of the
  //! surface. A high tolerance "smoothes" the surface.
  //! Exceptions
  //! Standard_OutOfRange if Index is outside the
  //! bounds of the knots table of this BSpline surface.
  Standard_EXPORT Standard_Boolean RemoveUKnot (const Standard_Integer Index, const Standard_Integer M, const Standard_Real Tolerance);
  
  //! Reduces to M the multiplicity of the knot of index
  //! Index in the V parametric direction. If M is 0, the knot is removed.
  //! With a modification of this type, the table of poles is also modified.
  //! Two different algorithms are used systematically to
  //! compute the new poles of the surface. For each
  //! pole, the distance between the pole calculated
  //! using the first algorithm and the same pole
  //! calculated using the second algorithm, is checked. If
  //! this distance is less than Tolerance it ensures that
  //! the surface is not modified by more than Tolerance.
  //! Under these conditions, the function returns true;
  //! otherwise, it returns false.
  //! A low tolerance prevents modification of the
  //! surface. A high tolerance "smoothes" the surface.
  //! Exceptions
  //! Standard_OutOfRange if Index is outside the
  //! bounds of the knots table of this BSpline surface.
  Standard_EXPORT Standard_Boolean RemoveVKnot (const Standard_Integer Index, const Standard_Integer M, const Standard_Real Tolerance);
  

  //! Increases the multiplicity of the knot of range UIndex
  //! in the UKnots sequence.
  //! M is the new multiplicity. M must be greater than the
  //! previous multiplicity and lower or equal to the degree
  //! of the surface in the U parametric direction.
  //! Raised if M is not in the range [1, UDegree]
  //!
  //! Raised if UIndex is not in the range [FirstUKnotIndex,
  //! LastUKnotIndex] given by the methods with the same name.
  Standard_EXPORT void IncreaseUMultiplicity (const Standard_Integer UIndex, const Standard_Integer M);
  

  //! Increases until order M the multiplicity of the set of knots
  //! FromI1,...., ToI2 in the U direction. This method can be used
  //! to make a B_spline surface into a PiecewiseBezier B_spline
  //! surface.
  //! If <me> was uniform, it can become non uniform.
  //!
  //! Raised if FromI1 or ToI2 is out of the range [FirstUKnotIndex,
  //! LastUKnotIndex].
  //!
  //! M should be greater than the previous multiplicity of the
  //! all the knots FromI1,..., ToI2 and lower or equal to the
  //! Degree of the surface in the U parametric direction.
  Standard_EXPORT void IncreaseUMultiplicity (const Standard_Integer FromI1, const Standard_Integer ToI2, const Standard_Integer M);
  

  //! Increments the multiplicity of the consecutives uknots FromI1..ToI2
  //! by step.   The multiplicity of each knot FromI1,.....,ToI2 must be
  //! lower or equal to the UDegree of the B_spline.
  //!
  //! Raised if FromI1 or ToI2 is not in the range
  //! [FirstUKnotIndex, LastUKnotIndex]
  //!
  //! Raised if one knot has a multiplicity greater than UDegree.
  Standard_EXPORT void IncrementUMultiplicity (const Standard_Integer FromI1, const Standard_Integer ToI2, const Standard_Integer Step);
  

  //! Increases the multiplicity of a knot in the V direction.
  //! M is the new multiplicity.
  //!
  //! M should be greater than the previous multiplicity and lower
  //! than the degree of the surface in the V parametric direction.
  //!
  //! Raised if VIndex is not in the range [FirstVKnotIndex,
  //! LastVKnotIndex] given by the methods with the same name.
  Standard_EXPORT void IncreaseVMultiplicity (const Standard_Integer VIndex, const Standard_Integer M);
  

  //! Increases until order M the multiplicity of the set of knots
  //! FromI1,...., ToI2 in the V direction. This method can be used to
  //! make a BSplineSurface into a PiecewiseBezier B_spline
  //! surface. If <me> was uniform, it can become non-uniform.
  //!
  //! Raised if FromI1 or ToI2 is out of the range [FirstVKnotIndex,
  //! LastVKnotIndex] given by the methods with the same name.
  //!
  //! M should be greater than the previous multiplicity of the
  //! all the knots FromI1,..., ToI2 and lower or equal to the
  //! Degree of the surface in the V parametric direction.
  Standard_EXPORT void IncreaseVMultiplicity (const Standard_Integer FromI1, const Standard_Integer ToI2, const Standard_Integer M);
  

  //! Increments the multiplicity of the consecutives vknots FromI1..ToI2
  //! by step.  The multiplicity of each knot FromI1,.....,ToI2 must be
  //! lower or equal to the VDegree of the B_spline.
  //!
  //! Raised if FromI1 or ToI2 is not in the range
  //! [FirstVKnotIndex, LastVKnotIndex]
  //!
  //! Raised if one knot has a multiplicity greater than VDegree.
  Standard_EXPORT void IncrementVMultiplicity (const Standard_Integer FromI1, const Standard_Integer ToI2, const Standard_Integer Step);
  

  //! Inserts a knot value in the sequence of UKnots. If U is a knot
  //! value this method increases the multiplicity of the knot if the
  //! previous multiplicity was lower than M else it does nothing. The
  //! tolerance criterion is ParametricTolerance. ParametricTolerance
  //! should be greater or equal than Resolution from package gp.
  //!
  //! Raised if U is out of the bounds [U1, U2] given by the methods
  //! Bounds, the criterion ParametricTolerance is used.
  //! Raised if M is not in the range [1, UDegree].
  Standard_EXPORT void InsertUKnot (const Standard_Real U, const Standard_Integer M, const Standard_Real ParametricTolerance, const Standard_Boolean Add = Standard_True);
  

  //! Inserts a knot value in the sequence of VKnots. If V is a knot
  //! value this method increases the multiplicity of the knot if the
  //! previous multiplicity was lower than M otherwise it does nothing.
  //! The tolerance criterion is ParametricTolerance.
  //! ParametricTolerance should be greater or equal than Resolution
  //! from package gp.
  //!
  //! raises if V is out of the Bounds [V1, V2] given by the methods
  //! Bounds, the criterion ParametricTolerance is used.
  //! raises if M is not in the range [1, VDegree].
  Standard_EXPORT void InsertVKnot (const Standard_Real V, const Standard_Integer M, const Standard_Real ParametricTolerance, const Standard_Boolean Add = Standard_True);
  

  //! Segments the surface between U1 and U2 in the U-Direction.
  //! between V1 and V2 in the V-Direction.
  //! The control points are modified, the first and the last point
  //! are not the same.
  //! Warnings :
  //! Even if <me> is not closed it can become closed after the
  //! segmentation for example if U1 or U2 are out of the bounds
  //! of the surface <me> or if the surface makes loop.
  //! raises if U2 < U1 or V2 < V1.
  //! Standard_DomainError if U2 - U1 exceeds the uperiod for uperiodic surfaces.
  //! i.e. ((U2 - U1) - UPeriod) > Precision::PConfusion().
  //! Standard_DomainError if V2 - V1 exceeds the vperiod for vperiodic surfaces.
  //! i.e. ((V2 - V1) - VPeriod) > Precision::PConfusion()).
  Standard_EXPORT void Segment (const Standard_Real U1, const Standard_Real U2, const Standard_Real V1, const Standard_Real V2);
  

  //! Segments the surface between U1 and U2 in the U-Direction.
  //! between V1 and V2 in the V-Direction.
  //!
  //! same as Segment but do nothing if U1 and U2 (resp. V1 and V2) are
  //! equal to the bounds in U (resp. in V) of <me>.
  //! For example, if <me> is periodic in V, it will be always periodic
  //! in V after the segmentation if the bounds in V are unchanged
  //!
  //! Warnings :
  //! Even if <me> is not closed it can become closed after the
  //! segmentation for example if U1 or U2 are out of the bounds
  //! of the surface <me> or if the surface makes loop.
  //! raises if U2 < U1 or V2 < V1.
  //! Standard_DomainError if U2 - U1 exceeds the uperiod for uperiodic surfaces.
  //! i.e. ((U2 - U1) - UPeriod) > Precision::PConfusion().
  //! Standard_DomainError if V2 - V1 exceeds the vperiod for vperiodic surfaces.
  //! i.e. ((V2 - V1) - VPeriod) > Precision::PConfusion()).
  Standard_EXPORT void CheckAndSegment (const Standard_Real U1, const Standard_Real U2, const Standard_Real V1, const Standard_Real V2);
  
  //! Substitutes the UKnots of range UIndex with K.
  //!
  //! Raised if UIndex < 1 or UIndex > NbUKnots
  //!
  //! Raised if K >= UKnots(UIndex+1) or K <= UKnots(UIndex-1)
  Standard_EXPORT void SetUKnot (const Standard_Integer UIndex, const Standard_Real K);
  
  //! Changes all the U-knots of the surface.
  //! The multiplicity of the knots are not modified.
  //!
  //! Raised if there is an index such that UK (Index+1) <= UK (Index).
  //!
  //! Raised if  UK.Lower() < 1 or UK.Upper() > NbUKnots
  Standard_EXPORT void SetUKnots (const TColStd_Array1OfReal& UK);
  

  //! Changes the value of the UKnots of range UIndex and
  //! increases its multiplicity.
  //!
  //! Raised if UIndex is not in the range [FirstUKnotIndex,
  //! LastUKnotIndex] given by the methods with the same name.
  //!
  //! Raised if K >= UKnots(UIndex+1) or K <= UKnots(UIndex-1)
  //! M must be lower than UDegree and greater than the previous
  //! multiplicity of the knot of range UIndex.
  Standard_EXPORT void SetUKnot (const Standard_Integer UIndex, const Standard_Real K, const Standard_Integer M);
  
  //! Substitutes the VKnots of range VIndex with K.
  //!
  //! Raised if VIndex < 1 or VIndex > NbVKnots
  //!
  //! Raised if K >= VKnots(VIndex+1) or K <= VKnots(VIndex-1)
  Standard_EXPORT void SetVKnot (const Standard_Integer VIndex, const Standard_Real K);
  
  //! Changes all the V-knots of the surface.
  //! The multiplicity of the knots are not modified.
  //!
  //! Raised if there is an index such that VK (Index+1) <= VK (Index).
  //!
  //! Raised if  VK.Lower() < 1 or VK.Upper() > NbVKnots
  Standard_EXPORT void SetVKnots (const TColStd_Array1OfReal& VK);
  

  //! Changes the value of the VKnots of range VIndex and increases
  //! its multiplicity.
  //!
  //! Raised if VIndex is not in the range [FirstVKnotIndex,
  //! LastVKnotIndex] given by the methods with the same name.
  //!
  //! Raised if K >= VKnots(VIndex+1) or K <= VKnots(VIndex-1)
  //! M must be lower than VDegree and greater than the previous
  //! multiplicity of the knot of range VIndex.
  Standard_EXPORT void SetVKnot (const Standard_Integer VIndex, const Standard_Real K, const Standard_Integer M);
  

  //! Locates the parametric value U in the sequence of UKnots.
  //! If "WithKnotRepetition" is True we consider the knot's
  //! representation with repetition of multiple knot value,
  //! otherwise  we consider the knot's representation with
  //! no repetition of multiple knot values.
  //! UKnots (I1) <= U <= UKnots (I2)
  //! . if I1 = I2  U is a knot value (the tolerance criterion
  //! ParametricTolerance is used).
  //! . if I1 < 1  => U < UKnots(1) - Abs(ParametricTolerance)
  //! . if I2 > NbUKnots => U > UKnots(NbUKnots)+Abs(ParametricTolerance)
  Standard_EXPORT void LocateU (const Standard_Real U, const Standard_Real ParametricTolerance, Standard_Integer& I1, Standard_Integer& I2, const Standard_Boolean WithKnotRepetition = Standard_False) const;
  

  //! Locates the parametric value V in the sequence of knots.
  //! If "WithKnotRepetition" is True we consider the knot's
  //! representation with repetition of multiple knot value,
  //! otherwise  we consider the knot's representation with
  //! no repetition of multiple knot values.
  //! VKnots (I1) <= V <= VKnots (I2)
  //! . if I1 = I2  V is a knot value (the tolerance criterion
  //! ParametricTolerance is used).
  //! . if I1 < 1  => V < VKnots(1) - Abs(ParametricTolerance)
  //! . if I2 > NbVKnots => V > VKnots(NbVKnots)+Abs(ParametricTolerance)
  //! poles insertion and removing
  //! The following methods are available only if the surface
  //! is Uniform or QuasiUniform in the considered direction
  //! The knot repartition is modified.
  Standard_EXPORT void LocateV (const Standard_Real V, const Standard_Real ParametricTolerance, Standard_Integer& I1, Standard_Integer& I2, const Standard_Boolean WithKnotRepetition = Standard_False) const;
  

  //! Substitutes the pole of range (UIndex, VIndex) with P.
  //! If the surface is rational the weight of range (UIndex, VIndex)
  //! is not modified.
  //!
  //! Raised if UIndex < 1 or UIndex > NbUPoles or VIndex < 1 or
  //! VIndex > NbVPoles.
  Standard_EXPORT void SetPole (const Standard_Integer UIndex, const Standard_Integer VIndex, const gp_Pnt& P);
  

  //! Substitutes the pole and the weight of range (UIndex, VIndex)
  //! with P and W.
  //!
  //! Raised if UIndex < 1 or UIndex > NbUPoles or VIndex < 1 or
  //! VIndex > NbVPoles.
  //! Raised if Weight <= Resolution from package gp.
  Standard_EXPORT void SetPole (const Standard_Integer UIndex, const Standard_Integer VIndex, const gp_Pnt& P, const Standard_Real Weight);
  

  //! Changes a column of poles or a part of this column.
  //! Raised if Vindex < 1 or VIndex > NbVPoles.
  //!
  //! Raised if CPoles.Lower() < 1 or CPoles.Upper() > NbUPoles.
  Standard_EXPORT void SetPoleCol (const Standard_Integer VIndex, const TColgp_Array1OfPnt& CPoles);
  

  //! Changes a column of poles or a part of this column with the
  //! corresponding weights. If the surface was rational it can
  //! become non rational. If the surface was non rational it can
  //! become rational.
  //! Raised if Vindex < 1 or VIndex > NbVPoles.
  //!
  //! Raised if CPoles.Lower() < 1 or CPoles.Upper() > NbUPoles
  //! Raised if the bounds of CPoleWeights are not the same as the
  //! bounds of CPoles.
  //! Raised if one of the weight value of CPoleWeights is lower or
  //! equal to Resolution from package gp.
  Standard_EXPORT void SetPoleCol (const Standard_Integer VIndex, const TColgp_Array1OfPnt& CPoles, const TColStd_Array1OfReal& CPoleWeights);
  

  //! Changes a row of poles or a part of this row with the
  //! corresponding weights. If the surface was rational it can
  //! become non rational. If the surface was non rational it can
  //! become rational.
  //! Raised if Uindex < 1 or UIndex > NbUPoles.
  //!
  //! Raised if CPoles.Lower() < 1 or CPoles.Upper() > NbVPoles
  //! raises if the bounds of CPoleWeights are not the same as the
  //! bounds of CPoles.
  //! Raised if one of the weight value of CPoleWeights is lower or
  //! equal to Resolution from package gp.
  Standard_EXPORT void SetPoleRow (const Standard_Integer UIndex, const TColgp_Array1OfPnt& CPoles, const TColStd_Array1OfReal& CPoleWeights);
  

  //! Changes a row of poles or a part of this row.
  //! Raised if Uindex < 1 or UIndex > NbUPoles.
  //!
  //! Raised if CPoles.Lower() < 1 or CPoles.Upper() > NbVPoles.
  Standard_EXPORT void SetPoleRow (const Standard_Integer UIndex, const TColgp_Array1OfPnt& CPoles);
  

  //! Changes the weight of the pole of range UIndex, VIndex.
  //! If the surface was non rational it can become rational.
  //! If the surface was rational it can become non rational.
  //!
  //! Raised if UIndex < 1 or UIndex > NbUPoles or VIndex < 1 or
  //! VIndex > NbVPoles
  //!
  //! Raised if weight is lower or equal to Resolution from
  //! package gp
  Standard_EXPORT void SetWeight (const Standard_Integer UIndex, const Standard_Integer VIndex, const Standard_Real Weight);
  

  //! Changes a column of weights of a part of this column.
  //!
  //! Raised if VIndex < 1 or VIndex > NbVPoles
  //!
  //! Raised if CPoleWeights.Lower() < 1 or
  //! CPoleWeights.Upper() > NbUPoles.
  //! Raised if a weight value is lower or equal to Resolution
  //! from package gp.
  Standard_EXPORT void SetWeightCol (const Standard_Integer VIndex, const TColStd_Array1OfReal& CPoleWeights);
  

  //! Changes a row of weights or a part of this row.
  //!
  //! Raised if UIndex < 1 or UIndex > NbUPoles
  //!
  //! Raised if CPoleWeights.Lower() < 1 or
  //! CPoleWeights.Upper() > NbVPoles.
  //! Raised  if a weight value is lower or equal to Resolution
  //! from package gp.
  Standard_EXPORT void SetWeightRow (const Standard_Integer UIndex, const TColStd_Array1OfReal& CPoleWeights);
  
  //! Move a point with parameter U and V to P.
  //! given u,v  as parameters)  to  reach a  new position
  //! UIndex1, UIndex2, VIndex1, VIndex2:
  //! indicates the poles which can be moved
  //! if Problem in BSplineBasis calculation, no change
  //! for the curve and
  //! UFirstIndex, VLastIndex = 0
  //! VFirstIndex, VLastIndex = 0
  //!
  //! Raised if UIndex1 < UIndex2 or VIndex1 < VIndex2 or
  //! UIndex1 < 1 || UIndex1 > NbUPoles or
  //! UIndex2 < 1 || UIndex2 > NbUPoles
  //! VIndex1 < 1 || VIndex1 > NbVPoles or
  //! VIndex2 < 1 || VIndex2 > NbVPoles
  //! characteristics of the surface
  Standard_EXPORT void MovePoint (const Standard_Real U, const Standard_Real V, const gp_Pnt& P, const Standard_Integer UIndex1, const Standard_Integer UIndex2, const Standard_Integer VIndex1, const Standard_Integer VIndex2, Standard_Integer& UFirstIndex, Standard_Integer& ULastIndex, Standard_Integer& VFirstIndex, Standard_Integer& VLastIndex);
  

  //! Returns true if the first control points row and the last
  //! control points row are identical. The tolerance criterion
  //! is Resolution from package gp.
  Standard_EXPORT Standard_Boolean IsUClosed() const Standard_OVERRIDE;
  

  //! Returns true if the first control points column and the
  //! last last control points column are identical.
  //! The tolerance criterion is Resolution from package gp.
  Standard_EXPORT Standard_Boolean IsVClosed() const Standard_OVERRIDE;
  

  //! Returns True if the order of continuity of the surface in the
  //! U direction  is N.
  //! Raised if N < 0.
  Standard_EXPORT Standard_Boolean IsCNu (const Standard_Integer N) const Standard_OVERRIDE;
  

  //! Returns True if the order of continuity of the surface
  //! in the V direction  is N.
  //! Raised if N < 0.
  Standard_EXPORT Standard_Boolean IsCNv (const Standard_Integer N) const Standard_OVERRIDE;
  

  //! Returns True if the surface is closed in the U direction
  //! and if the B-spline has been turned into a periodic surface
  //! using the function SetUPeriodic.
  Standard_EXPORT Standard_Boolean IsUPeriodic() const Standard_OVERRIDE;
  

  //! Returns False if for each row of weights all the weights
  //! are identical.
  //! The tolerance criterion is resolution from package gp.
  //! Example :
  //! |1.0, 1.0, 1.0|
  //! if Weights =  |0.5, 0.5, 0.5|   returns False
  //! |2.0, 2.0, 2.0|
  Standard_EXPORT Standard_Boolean IsURational() const;
  

  //! Returns True if the surface is closed in the V direction
  //! and if the B-spline has been turned into a periodic
  //! surface using the function SetVPeriodic.
  Standard_EXPORT Standard_Boolean IsVPeriodic() const Standard_OVERRIDE;
  

  //! Returns False if for each column of weights all the weights
  //! are identical.
  //! The tolerance criterion is resolution from package gp.
  //! Examples :
  //! |1.0, 2.0, 0.5|
  //! if Weights =  |1.0, 2.0, 0.5|   returns False
  //! |1.0, 2.0, 0.5|
  Standard_EXPORT Standard_Boolean IsVRational() const;
  

  //! Returns the parametric bounds of the surface.
  //! Warnings :
  //! These parametric values are the bounds of the array of
  //! knots UKnots and VKnots only if the first knots and the
  //! last knots have a multiplicity equal to UDegree + 1 or
  //! VDegree + 1
  Standard_EXPORT void Bounds (Standard_Real& U1, Standard_Real& U2, Standard_Real& V1, Standard_Real& V2) const Standard_OVERRIDE;
  

  //! Returns the continuity of the surface :
  //! C0 : only geometric continuity,
  //! C1 : continuity of the first derivative all along the Surface,
  //! C2 : continuity of the second derivative all along the Surface,
  //! C3 : continuity of the third derivative all along the Surface,
  //! CN : the order of continuity is infinite.
  //! A B-spline surface is infinitely continuously differentiable
  //! for the couple of parameters U, V such thats U != UKnots(i)
  //! and V != VKnots(i). The continuity of the surface at a knot
  //! value depends on the multiplicity of this knot.
  //! Example :
  //! If the surface is C1 in the V direction and C2 in the U
  //! direction this function returns Shape = C1.
  Standard_EXPORT GeomAbs_Shape Continuity() const Standard_OVERRIDE;
  

  //! Computes the Index of the UKnots which gives the first
  //! parametric value of the surface in the U direction.
  //! The UIso curve corresponding to this value is a
  //! boundary curve of the surface.
  Standard_EXPORT Standard_Integer FirstUKnotIndex() const;
  

  //! Computes the Index of the VKnots which gives the
  //! first parametric value of the surface in the V direction.
  //! The VIso curve corresponding to this knot is a boundary
  //! curve of the surface.
  Standard_EXPORT Standard_Integer FirstVKnotIndex() const;
  

  //! Computes the Index of the UKnots which gives the
  //! last parametric value of the surface in the U direction.
  //! The UIso curve corresponding to this knot is a boundary
  //! curve of the surface.
  Standard_EXPORT Standard_Integer LastUKnotIndex() const;
  

  //! Computes the Index of the VKnots which gives the
  //! last parametric value of the surface in the V direction.
  //! The VIso curve corresponding to this knot is a
  //! boundary curve of the surface.
  Standard_EXPORT Standard_Integer LastVKnotIndex() const;
  
  //! Returns the number of knots in the U direction.
  Standard_EXPORT Standard_Integer NbUKnots() const;
  
  //! Returns number of poles in the U direction.
  Standard_EXPORT Standard_Integer NbUPoles() const;
  
  //! Returns the number of knots in the V direction.
  Standard_EXPORT Standard_Integer NbVKnots() const;
  
  //! Returns the number of poles in the V direction.
  Standard_EXPORT Standard_Integer NbVPoles() const;
  

  //! Returns the pole of range (UIndex, VIndex).
  //!
  //! Raised if UIndex < 1 or UIndex > NbUPoles or VIndex < 1 or
  //! VIndex > NbVPoles.
  Standard_EXPORT const gp_Pnt& Pole(const Standard_Integer UIndex, const Standard_Integer VIndex) const;
  
  //! Returns the poles of the B-spline surface.
  //!
  //! Raised if the length of P in the U and V direction
  //! is not equal to NbUpoles and NbVPoles.
  Standard_EXPORT void Poles (TColgp_Array2OfPnt& P) const;
  
  //! Returns the poles of the B-spline surface.
  Standard_EXPORT const TColgp_Array2OfPnt& Poles() const;
  

  //! Returns the degree of the normalized B-splines Ni,n in the U
  //! direction.
  Standard_EXPORT Standard_Integer UDegree() const;
  

  //! Returns the Knot value of range UIndex.
  //! Raised if UIndex < 1 or UIndex > NbUKnots
  Standard_EXPORT Standard_Real UKnot (const Standard_Integer UIndex) const;
  

  //! Returns NonUniform or Uniform or QuasiUniform or
  //! PiecewiseBezier.  If all the knots differ by a
  //! positive constant from the preceding knot in the U
  //! direction the B-spline surface can be :
  //! - Uniform if all the knots are of multiplicity 1,
  //! - QuasiUniform if all the knots are of multiplicity 1
  //! except for the first and last knot which are of
  //! multiplicity Degree + 1,
  //! - PiecewiseBezier if the first and last knots have
  //! multiplicity Degree + 1 and if interior knots have
  //! multiplicity Degree
  //! otherwise the surface is non uniform in the U direction
  //! The tolerance criterion is Resolution from package gp.
  Standard_EXPORT GeomAbs_BSplKnotDistribution UKnotDistribution() const;
  
  //! Returns the knots in the U direction.
  //!
  //! Raised if the length of Ku is not equal to the number of knots
  //! in the U direction.
  Standard_EXPORT void UKnots (TColStd_Array1OfReal& Ku) const;
  
  //! Returns the knots in the U direction.
  Standard_EXPORT const TColStd_Array1OfReal& UKnots() const;
  
  //! Returns the uknots sequence.
  //! In this sequence the knots with a multiplicity greater than 1
  //! are repeated.
  //! Example :
  //! Ku = {k1, k1, k1, k2, k3, k3, k4, k4, k4}
  //!
  //! Raised if the length of Ku is not equal to NbUPoles + UDegree + 1
  Standard_EXPORT void UKnotSequence (TColStd_Array1OfReal& Ku) const;
  
  //! Returns the uknots sequence.
  //! In this sequence the knots with a multiplicity greater than 1
  //! are repeated.
  //! Example :
  //! Ku = {k1, k1, k1, k2, k3, k3, k4, k4, k4}
  Standard_EXPORT const TColStd_Array1OfReal& UKnotSequence() const;
  

  //! Returns the multiplicity value of knot of range UIndex in
  //! the u direction.
  //! Raised if UIndex < 1 or UIndex > NbUKnots.
  Standard_EXPORT Standard_Integer UMultiplicity (const Standard_Integer UIndex) const;
  

  //! Returns the multiplicities of the knots in the U direction.
  //!
  //! Raised if the length of Mu is not equal to the number of
  //! knots in the U direction.
  Standard_EXPORT void UMultiplicities (TColStd_Array1OfInteger& Mu) const;
  
  //! Returns the multiplicities of the knots in the U direction.
  Standard_EXPORT const TColStd_Array1OfInteger& UMultiplicities() const;
  

  //! Returns the degree of the normalized B-splines Ni,d in the
  //! V direction.
  Standard_EXPORT Standard_Integer VDegree() const;
  
  //! Returns the Knot value of range VIndex.
  //! Raised if VIndex < 1 or VIndex > NbVKnots
  Standard_EXPORT Standard_Real VKnot (const Standard_Integer VIndex) const;
  

  //! Returns NonUniform or Uniform or QuasiUniform or
  //! PiecewiseBezier. If all the knots differ by a positive
  //! constant from the preceding knot in the V direction the
  //! B-spline surface can be :
  //! - Uniform if all the knots are of multiplicity 1,
  //! - QuasiUniform if all the knots are of multiplicity 1
  //! except for the first and last knot which are of
  //! multiplicity Degree + 1,
  //! - PiecewiseBezier if the first and last knots have
  //! multiplicity  Degree + 1 and if interior knots have
  //! multiplicity Degree
  //! otherwise the surface is non uniform in the V direction.
  //! The tolerance criterion is Resolution from package gp.
  Standard_EXPORT GeomAbs_BSplKnotDistribution VKnotDistribution() const;
  
  //! Returns the knots in the V direction.
  //!
  //! Raised if the length of Kv is not equal to the number of
  //! knots in the V direction.
  Standard_EXPORT void VKnots (TColStd_Array1OfReal& Kv) const;
  
  //! Returns the knots in the V direction.
  Standard_EXPORT const TColStd_Array1OfReal& VKnots() const;
  
  //! Returns the vknots sequence.
  //! In this sequence the knots with a multiplicity greater than 1
  //! are repeated.
  //! Example :
  //! Kv = {k1, k1, k1, k2, k3, k3, k4, k4, k4}
  //!
  //! Raised if the length of Kv is not equal to NbVPoles + VDegree + 1
  Standard_EXPORT void VKnotSequence (TColStd_Array1OfReal& Kv) const;
  
  //! Returns the vknots sequence.
  //! In this sequence the knots with a multiplicity greater than 1
  //! are repeated.
  //! Example :
  //! Ku = {k1, k1, k1, k2, k3, k3, k4, k4, k4}
  Standard_EXPORT const TColStd_Array1OfReal& VKnotSequence() const;
  

  //! Returns the multiplicity value of knot of range VIndex in
  //! the v direction.
  //! Raised if VIndex < 1 or VIndex > NbVKnots
  Standard_EXPORT Standard_Integer VMultiplicity (const Standard_Integer VIndex) const;
  

  //! Returns the multiplicities of the knots in the V direction.
  //!
  //! Raised if the length of Mv is not equal to the number of
  //! knots in the V direction.
  Standard_EXPORT void VMultiplicities (TColStd_Array1OfInteger& Mv) const;
  
  //! Returns the multiplicities of the knots in the V direction.
  Standard_EXPORT const TColStd_Array1OfInteger& VMultiplicities() const;
  
  //! Returns the weight value of range UIndex, VIndex.
  //!
  //! Raised if UIndex < 1 or UIndex > NbUPoles or VIndex < 1
  //! or VIndex > NbVPoles.
  Standard_EXPORT Standard_Real Weight (const Standard_Integer UIndex, const Standard_Integer VIndex) const;
  
  //! Returns the weights of the B-spline surface.
  //!
  //! Raised if the length of W in the U and V direction is
  //! not equal to NbUPoles and NbVPoles.
  Standard_EXPORT void Weights (TColStd_Array2OfReal& W) const;
  
  //! Returns the weights of the B-spline surface.
  //! value and derivatives computation
  Standard_EXPORT const TColStd_Array2OfReal* Weights() const;
  
  Standard_EXPORT void D0 (const Standard_Real U, const Standard_Real V, gp_Pnt& P) const Standard_OVERRIDE;
  
  //! Raised if the continuity of the surface is not C1.
  Standard_EXPORT void D1 (const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V) const Standard_OVERRIDE;
  
  //! Raised if the continuity of the surface is not C2.
  Standard_EXPORT void D2 (const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V, gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV) const Standard_OVERRIDE;
  
  //! Raised if the continuity of the surface is not C3.
  Standard_EXPORT void D3 (const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V, gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV, gp_Vec& D3U, gp_Vec& D3V, gp_Vec& D3UUV, gp_Vec& D3UVV) const Standard_OVERRIDE;
  

  //! Nu is the order of derivation in the U parametric direction and
  //! Nv is the order of derivation in the V parametric direction.
  //!
  //! Raised if the continuity of the surface is not CNu in the U
  //! direction and CNv in the V direction.
  //!
  //! Raised if Nu + Nv < 1 or Nu < 0 or Nv < 0.
  //!
  //! The following functions computes the point for the
  //! parametric values (U, V) and the derivatives at
  //! this point on the B-spline surface patch delimited
  //! with the knots FromUK1, FromVK1 and the knots ToUK2,
  //! ToVK2.  (U, V) can be out of these parametric bounds
  //! but for the computation we only use the definition
  //! of the surface between these knots. This method is
  //! useful to compute local derivative, if the order of
  //! continuity of the whole surface is not greater enough.
  //! Inside the parametric knot's domain previously defined
  //! the evaluations are the same as if we consider the whole
  //! definition of the surface. Of course the evaluations are
  //! different outside this parametric domain.
  Standard_EXPORT gp_Vec DN (const Standard_Real U, const Standard_Real V, const Standard_Integer Nu, const Standard_Integer Nv) const Standard_OVERRIDE;
  
  //! Raised if FromUK1 = ToUK2 or FromVK1 = ToVK2.
  Standard_EXPORT void LocalD0 (const Standard_Real U, const Standard_Real V, const Standard_Integer FromUK1, const Standard_Integer ToUK2, const Standard_Integer FromVK1, const Standard_Integer ToVK2, gp_Pnt& P) const;
  

  //! Raised if the local continuity of the surface is not C1
  //! between the knots FromUK1, ToUK2 and FromVK1, ToVK2.
  //! Raised if FromUK1 = ToUK2 or FromVK1 = ToVK2.
  Standard_EXPORT void LocalD1 (const Standard_Real U, const Standard_Real V, const Standard_Integer FromUK1, const Standard_Integer ToUK2, const Standard_Integer FromVK1, const Standard_Integer ToVK2, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V) const;
  

  //! Raised if the local continuity of the surface is not C2
  //! between the knots FromUK1, ToUK2 and FromVK1, ToVK2.
  //! Raised if FromUK1 = ToUK2 or FromVK1 = ToVK2.
  Standard_EXPORT void LocalD2 (const Standard_Real U, const Standard_Real V, const Standard_Integer FromUK1, const Standard_Integer ToUK2, const Standard_Integer FromVK1, const Standard_Integer ToVK2, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V, gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV) const;
  

  //! Raised if the local continuity of the surface is not C3
  //! between the knots FromUK1, ToUK2 and FromVK1, ToVK2.
  //! Raised if FromUK1 = ToUK2 or FromVK1 = ToVK2.
  Standard_EXPORT void LocalD3 (const Standard_Real U, const Standard_Real V, const Standard_Integer FromUK1, const Standard_Integer ToUK2, const Standard_Integer FromVK1, const Standard_Integer ToVK2, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V, gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV, gp_Vec& D3U, gp_Vec& D3V, gp_Vec& D3UUV, gp_Vec& D3UVV) const;
  

  //! Raised if the local continuity of the surface is not CNu
  //! between the knots FromUK1, ToUK2 and CNv between the knots
  //! FromVK1, ToVK2.
  //! Raised if FromUK1 = ToUK2 or FromVK1 = ToVK2.
  Standard_EXPORT gp_Vec LocalDN (const Standard_Real U, const Standard_Real V, const Standard_Integer FromUK1, const Standard_Integer ToUK2, const Standard_Integer FromVK1, const Standard_Integer ToVK2, const Standard_Integer Nu, const Standard_Integer Nv) const;
  

  //! Computes the point of parameter U, V on the BSpline surface patch
  //! defines between the knots UK1 UK2, VK1, VK2. U can be out of the
  //! bounds [Knot UK1, Knot UK2] and V can be outof the bounds
  //! [Knot VK1, Knot VK2]  but for the computation we only use the
  //! definition of the surface between these knot values.
  //! Raises if FromUK1 = ToUK2 or FromVK1 = ToVK2.
  Standard_EXPORT gp_Pnt LocalValue (const Standard_Real U, const Standard_Real V, const Standard_Integer FromUK1, const Standard_Integer ToUK2, const Standard_Integer FromVK1, const Standard_Integer ToVK2) const;
  

  //! Computes the U isoparametric curve.
  //! A B-spline curve is returned.
  Standard_EXPORT Handle(Geom_Curve) UIso (const Standard_Real U) const Standard_OVERRIDE;
  

  //! Computes the V isoparametric curve.
  //! A B-spline curve is returned.
  Standard_EXPORT Handle(Geom_Curve) VIso (const Standard_Real V) const Standard_OVERRIDE;
  

  //! Computes the U isoparametric curve.
  //! If CheckRational=False, no try to make it non-rational.
  //! A B-spline curve is returned.
  Standard_EXPORT Handle(Geom_Curve) UIso (const Standard_Real U, const Standard_Boolean CheckRational) const;
  

  //! Computes the V isoparametric curve.
  //! If CheckRational=False, no try to make it non-rational.
  //! A B-spline curve is returned.
  //! transformations
  Standard_EXPORT Handle(Geom_Curve) VIso (const Standard_Real V, const Standard_Boolean CheckRational) const;
  
  //! Applies the transformation T to this BSpline surface.
  Standard_EXPORT void Transform (const gp_Trsf& T) Standard_OVERRIDE;
  

  //! Returns the value of the maximum degree of the normalized
  //! B-spline basis functions in the u and v directions.
  Standard_EXPORT static Standard_Integer MaxDegree();
  
  //! Computes two tolerance values for this BSpline
  //! surface, based on the given tolerance in 3D space
  //! Tolerance3D. The tolerances computed are:
  //! - UTolerance in the u parametric direction, and
  //! - VTolerance in the v parametric direction.
  //! If f(u,v) is the equation of this BSpline surface,
  //! UTolerance and VTolerance guarantee that :
  //! | u1 - u0 | < UTolerance and
  //! | v1 - v0 | < VTolerance
  //! ====> |f (u1,v1) - f (u0,v0)| < Tolerance3D
  Standard_EXPORT void Resolution (const Standard_Real Tolerance3D, Standard_Real& UTolerance, Standard_Real& VTolerance);
  
  //! Creates a new object which is a copy of this BSpline surface.
  Standard_EXPORT Handle(Geom_Geometry) Copy() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(Geom_BSplineSurface,Geom_BoundedSurface)

protected:




private:

  
  //! Recompute  the  flatknots,  the knotsdistribution, the
  //! continuity for U.
  Standard_EXPORT void UpdateUKnots();
  
  //! Recompute  the  flatknots,  the knotsdistribution, the
  //! continuity for V.
  Standard_EXPORT void UpdateVKnots();

  Standard_Boolean urational;
  Standard_Boolean vrational;
  Standard_Boolean uperiodic;
  Standard_Boolean vperiodic;
  GeomAbs_BSplKnotDistribution uknotSet;
  GeomAbs_BSplKnotDistribution vknotSet;
  GeomAbs_Shape Usmooth;
  GeomAbs_Shape Vsmooth;
  Standard_Integer udeg;
  Standard_Integer vdeg;
  Handle(TColgp_HArray2OfPnt) poles;
  Handle(TColStd_HArray2OfReal) weights;
  Handle(TColStd_HArray1OfReal) ufknots;
  Handle(TColStd_HArray1OfReal) vfknots;
  Handle(TColStd_HArray1OfReal) uknots;
  Handle(TColStd_HArray1OfReal) vknots;
  Handle(TColStd_HArray1OfInteger) umults;
  Handle(TColStd_HArray1OfInteger) vmults;
  Standard_Real umaxderivinv;
  Standard_Real vmaxderivinv;
  Standard_Boolean maxderivinvok;


};







#endif // _Geom_BSplineSurface_HeaderFile
