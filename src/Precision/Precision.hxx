// Created on: 1993-02-17
// Created by: Remi LEQUETTE
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

#ifndef _Precision_HeaderFile
#define _Precision_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Real.hxx>

//! The Precision package offers a set of functions defining precision criteria
//! for use in conventional situations when comparing two numbers.
//! Generalities
//! It is not advisable to use floating number equality. Instead, the difference
//! between numbers must be compared with a given precision, i.e. :
//! Standard_Real x1, x2 ;
//! x1 = ...
//! x2 = ...
//! If ( x1 == x2 ) ...
//! should not be used and must be written as indicated below:
//! Standard_Real x1, x2 ;
//! Standard_Real Precision = ...
//! x1 = ...
//! x2 = ...
//! If ( Abs ( x1 - x2 ) < Precision ) ...
//! Likewise, when ordering floating numbers, you must take the following into account :
//! Standard_Real x1, x2 ;
//! Standard_Real Precision = ...
//! x1 = ...       ! a large number
//! x2 = ...       ! another large number
//! If ( x1 < x2 - Precision ) ...
//! is incorrect when x1 and x2 are large numbers ; it is better to write :
//! Standard_Real x1, x2 ;
//! Standard_Real Precision = ...
//! x1 = ...       ! a large number
//! x2 = ...       ! another large number
//! If ( x2 - x1 > Precision ) ...
//! Precision in Cas.Cade
//! Generally speaking, the precision criterion is not implicit in Cas.Cade. Low-level geometric
//! algorithms accept precision criteria as arguments. As a rule, they should not refer directly to
//! the precision criteria provided by the Precision package. On the other hand, high-level modeling
//! algorithms have to provide the low-level geometric algorithms that they call, with a precision
//! criteria. One way of doing this is to use the above precision criteria. Alternatively, the
//! high-level algorithms can have their own system for precision management. For example, the
//! Topology Data Structure stores precision criteria for each elementary shape (as a vertex, an
//! edge or a face). When a new topological object is constructed, the precision criteria are taken
//! from those provided by the Precision package, and stored in the related data structure. Later, a
//! topological algorithm which analyses these objects will work with the values stored in the data
//! structure. Also, if this algorithm is to build a new topological object, from these precision
//! criteria, it will compute a new precision criterion for the new topological object, and write it
//! into the data structure of the new topological object. The different precision criteria offered
//! by the Precision package, cover the most common requirements of geometric algorithms, such as
//! intersections, approximations, and so on. The choice of precision depends on the algorithm and
//! on the geometric space. The geometric space may be :
//! -   a "real" 2D or 3D space, where the lengths are measured in meters, millimeters, microns,
//! inches, etc ..., or
//! -   a "parametric" space, 1D on a curve or 2D on a surface, where lengths have no dimension.
//! The choice of precision criteria for real space depends on the choice of the product, as it is
//! based on the accuracy of the machine and the unit of measurement. The choice of precision
//! criteria for parametric space depends on both the accuracy of the machine and the dimensions of
//! the curve or the surface, since the parametric precision criterion and the real precision
//! criterion are linked : if the curve is defined by the equation P(t), the inequation : Abs ( t2 -
//! t1 ) < ParametricPrecision means that the parameters t1 and t2 are considered to be equal, and
//! the inequation : Distance ( P(t2) , P(t1) ) < RealPrecision means that the points P(t1) and
//! P(t2) are considered to be coincident. It seems to be the same idea, and it would be wonderful
//! if these two inequations were equivalent. Note that this is rarely the case ! What is provided
//! in this package? The Precision package provides :
//! -   a set of real space precision criteria for the algorithms, in view of checking distances and
//! angles,
//! -   a set of parametric space precision criteria for the algorithms, in view of checking both :
//! -   the equality of parameters in a parametric space,
//! -   or the coincidence of points in the real space, by using parameter values,
//! -   the notion of infinite value, composed of a value assumed to be infinite, and checking tests
//! designed to verify if any value could be considered as infinite. All the provided functions are
//! very simple. The returned values result from the adaptation of the applications developed by the
//! Open CASCADE company to Open CASCADE algorithms. The main interest of these functions lies in
//! that it incites engineers developing applications to ask questions on precision factors. Which
//! one is to be used in such or such case ? Tolerance criteria are context dependent. They must
//! first choose :
//! -   either to work in real space,
//! -   or to work in parametric space,
//! -   or to work in a combined real and parametric space.
//! They must next decide which precision factor will give the best answer to the current problem.
//! Within an application environment, it is crucial to master precision even though this process
//! may take a great deal of time.
class Precision
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns the recommended precision value
  //! when checking the equality of two angles (given in radians).
  //! Standard_Real Angle1 = ... , Angle2 = ... ;
  //! If ( Abs( Angle2 - Angle1 ) < Precision::Angular() ) ...
  //! The tolerance of angular equality may be used to check the parallelism of two vectors :
  //! gp_Vec V1, V2 ;
  //! V1 = ...
  //! V2 = ...
  //! If ( V1.IsParallel (V2, Precision::Angular() ) ) ...
  //! The tolerance of angular equality is equal to 1.e-12.
  //! Note : The tolerance of angular equality can be used when working with scalar products or
  //! cross products since sines and angles are equivalent for small angles. Therefore, in order to
  //! check whether two unit vectors are perpendicular :
  //! gp_Dir D1, D2 ;
  //! D1 = ...
  //! D2 = ...
  //! you can use :
  //! If ( Abs( D1.D2 ) < Precision::Angular() ) ...
  //! (although the function IsNormal does exist).
  static constexpr Standard_Real Angular() { return 1.e-12; }

  //! Returns the recommended precision value when
  //! checking coincidence of two points in real space.
  //! The tolerance of confusion is used for testing a 3D
  //! distance :
  //! -   Two points are considered to be coincident if their
  //! distance is smaller than the tolerance of confusion.
  //! gp_Pnt P1, P2 ;
  //! P1 = ...
  //! P2 = ...
  //! if ( P1.IsEqual ( P2 , Precision::Confusion() ) )
  //! then ...
  //! -   A vector is considered to be null if it has a null length :
  //! gp_Vec V ;
  //! V = ...
  //! if ( V.Magnitude() < Precision::Confusion() ) then ...
  //! The tolerance of confusion is equal to 1.e-7.
  //! The value of the tolerance of confusion is also used to
  //! define :
  //! -   the tolerance of intersection, and
  //! -   the tolerance of approximation.
  //! Note : As a rule, coordinate values in Cas.Cade are not
  //! dimensioned, so 1. represents one user unit, whatever
  //! value the unit may have : the millimeter, the meter, the
  //! inch, or any other unit. Let's say that Cas.Cade
  //! algorithms are written to be tuned essentially with
  //! mechanical design applications, on the basis of the
  //! millimeter. However, these algorithms may be used with
  //! any other unit but the tolerance criterion does no longer
  //! have the same signification.
  //! So pay particular attention to the type of your application,
  //! in relation with the impact of your unit on the precision criterion.
  //! -   For example in mechanical design, if the unit is the
  //! millimeter, the tolerance of confusion corresponds to a
  //! distance of 1 / 10000 micron, which is rather difficult to measure.
  //! -   However in other types of applications, such as
  //! cartography, where the kilometer is frequently used,
  //! the tolerance of confusion corresponds to a greater
  //! distance (1 / 10 millimeter). This distance
  //! becomes easily measurable, but only within a restricted
  //! space which contains some small objects of the complete scene.
  static constexpr Standard_Real Confusion() { return 1.e-7; }

  //! Returns square of Confusion.
  //! Created for speed and convenience.
  static constexpr Standard_Real SquareConfusion() { return Confusion() * Confusion(); }

  //! Returns the precision value in real space, frequently
  //! used by intersection algorithms to decide that a solution is reached.
  //! This function provides an acceptable level of precision
  //! for an intersection process to define the adjustment limits.
  //! The tolerance of intersection is designed to ensure
  //! that a point computed by an iterative algorithm as the
  //! intersection between two curves is indeed on the
  //! intersection. It is obvious that two tangent curves are
  //! close to each other, on a large distance. An iterative
  //! algorithm of intersection may find points on these
  //! curves within the scope of the confusion tolerance, but
  //! still far from the true intersection point. In order to force
  //! the intersection algorithm to continue the iteration
  //! process until a correct point is found on the tangent
  //! objects, the tolerance of intersection must be smaller
  //! than the tolerance of confusion.
  //! On the other hand, the tolerance of intersection must
  //! be large enough to minimize the time required by the
  //! process to converge to a solution.
  //! The tolerance of intersection is equal to :
  //! Precision::Confusion() / 100.
  //! (that is, 1.e-9).
  static constexpr Standard_Real Intersection() { return Confusion() * 0.01; }

  //! Returns the precision value in real space, frequently used
  //! by approximation algorithms.
  //! This function provides an acceptable level of precision for
  //! an approximation process to define adjustment limits.
  //! The tolerance of approximation is designed to ensure
  //! an acceptable computation time when performing an
  //! approximation process. That is why the tolerance of
  //! approximation is greater than the tolerance of confusion.
  //! The tolerance of approximation is equal to :
  //! Precision::Confusion() * 10.
  //! (that is, 1.e-6).
  //! You may use a smaller tolerance in an approximation
  //! algorithm, but this option might be costly.
  static constexpr Standard_Real Approximation() { return Confusion() * 10.0; }

  //! Convert a real  space precision  to  a  parametric
  //! space precision.   <T>  is the mean  value  of the
  //! length of the tangent of the curve or the surface.
  //!
  //! Value is P / T
  static constexpr Standard_Real Parametric(const Standard_Real P, const Standard_Real T)
  {
    return P / T;
  }

  //! Returns a precision value in parametric space, which may be used :
  //! -   to test the coincidence of two points in the real space,
  //! by using parameter values, or
  //! -   to test the equality of two parameter values in a parametric space.
  //! The parametric tolerance of confusion is designed to
  //! give a mean value in relation with the dimension of
  //! the curve or the surface. It considers that a variation of
  //! parameter equal to 1. along a curve (or an
  //! isoparametric curve of a surface) generates a segment
  //! whose length is equal to 100. (default value), or T.
  //! The parametric tolerance of confusion is equal to :
  //! -   Precision::Confusion() / 100., or Precision::Confusion() / T.
  //! The value of the parametric tolerance of confusion is also used to define :
  //! -   the parametric tolerance of intersection, and
  //! -   the parametric tolerance of approximation.
  //! Warning
  //! It is rather difficult to define a unique precision value in parametric space.
  //! -   First consider a curve (c) ; if M is the point of
  //! parameter u and M' the point of parameter u+du on
  //! the curve, call 'parametric tangent' at point M, for the
  //! variation du of the parameter, the quantity :
  //! T(u,du)=MM'/du (where MM' represents the
  //! distance between the two points M and M', in the real space).
  //! -   Consider the other curve resulting from a scaling
  //! transformation of (c) with a scale factor equal to
  //! 10. The 'parametric tangent' at the point of
  //! parameter u of this curve is ten times greater than the
  //! previous one. This shows that for two different curves,
  //! the distance between two points on the curve, resulting
  //! from the same variation of parameter du, may vary   considerably.
  //! -   Moreover, the variation of the parameter along the
  //! curve is generally not proportional to the curvilinear
  //! abscissa along the curve. So the distance between two
  //! points resulting from the same variation of parameter
  //! du, at two different points of a curve, may completely differ.
  //! -   Moreover, the parameterization of a surface may
  //! generate two quite different 'parametric tangent' values
  //! in the u or in the v parametric direction.
  //! -   Last, close to the poles of a sphere (the points which
  //! correspond to the values -Pi/2. and Pi/2. of the
  //! v parameter) the u parameter may change from 0 to
  //! 2.Pi without impacting on the resulting point.
  //! Therefore, take great care when adjusting a parametric
  //! tolerance to your own algorithm.
  static constexpr Standard_Real PConfusion(const Standard_Real T)
  {
    return Parametric(Confusion(), T);
  }

  //! Returns square of PConfusion.
  //! Created for speed and convenience.
  static constexpr Standard_Real SquarePConfusion() { return PConfusion() * PConfusion(); }

  //! Returns a precision value in parametric space, which
  //! may be used by intersection algorithms, to decide that
  //! a solution is reached. The purpose of this function is to
  //! provide an acceptable level of precision in parametric
  //! space, for an intersection process to define the adjustment limits.
  //! The parametric tolerance of intersection is
  //! designed to give a mean value in relation with the
  //! dimension of the curve or the surface. It considers
  //! that a variation of parameter equal to 1. along a curve
  //! (or an isoparametric curve of a surface) generates a
  //! segment whose length is equal to 100. (default value), or T.
  //! The parametric tolerance of intersection is equal to :
  //! -   Precision::Intersection() / 100., or Precision::Intersection() / T.
  static constexpr Standard_Real PIntersection(const Standard_Real T)
  {
    return Parametric(Intersection(), T);
  }

  //! Returns a precision value in parametric space, which may
  //! be used by approximation algorithms. The purpose of this
  //! function is to provide an acceptable level of precision in
  //! parametric space, for an approximation process to define
  //! the adjustment limits.
  //! The parametric tolerance of approximation is
  //! designed to give a mean value in relation with the
  //! dimension of the curve or the surface. It considers
  //! that a variation of parameter equal to 1. along a curve
  //! (or an isoparametric curve of a surface) generates a
  //! segment whose length is equal to 100. (default value), or T.
  //! The parametric tolerance of intersection is equal to :
  //! -   Precision::Approximation() / 100., or Precision::Approximation() / T.
  static constexpr Standard_Real PApproximation(const Standard_Real T)
  {
    return Parametric(Approximation(), T);
  }

  //! Convert a real  space precision  to  a  parametric
  //! space precision on a default curve.
  //!
  //! Value is Parametric(P,1.e+2)
  static constexpr Standard_Real Parametric(const Standard_Real P) { return P * 0.01; }

  //! Used  to test distances  in parametric  space on a
  //! default curve.
  //!
  //! This is Precision::Parametric(Precision::Confusion())
  static constexpr Standard_Real PConfusion() { return Confusion() * 0.01; }

  //! Used for Intersections  in parametric  space  on a
  //! default curve.
  //!
  //! This is Precision::Parametric(Precision::Intersection())
  static constexpr Standard_Real PIntersection() { return Intersection() * 0.01; }

  //! Used for  Approximations  in parametric space on a
  //! default curve.
  //!
  //! This is Precision::Parametric(Precision::Approximation())
  static constexpr Standard_Real PApproximation() { return Approximation() * 0.01; }

  //! Returns True if R may be considered as an infinite
  //! number. Currently Abs(R) > 1e100
  static inline Standard_Boolean IsInfinite(const Standard_Real R)
  {
    return Abs(R) >= (0.5 * Precision::Infinite());
  }

  //! Returns True if R may be considered as  a positive
  //! infinite number. Currently R > 1e100
  static constexpr Standard_Boolean IsPositiveInfinite(const Standard_Real R)
  {
    return R >= (0.5 * Precision::Infinite());
  }

  //! Returns True if R may  be considered as a negative
  //! infinite number. Currently R < -1e100
  static constexpr Standard_Boolean IsNegativeInfinite(const Standard_Real R)
  {
    return R <= -(0.5 * Precision::Infinite());
  }

  //! Returns a  big number that  can  be  considered as
  //! infinite. Use -Infinite() for a negative big number.
  static constexpr Standard_Real Infinite() { return 2.e+100; }
};

#endif // _Precision_HeaderFile
