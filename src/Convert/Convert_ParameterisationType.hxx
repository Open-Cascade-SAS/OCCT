// Created on: 1991-10-10
// Created by: Jean Claude VAUTHIER
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Convert_ParameterisationType_HeaderFile
#define _Convert_ParameterisationType_HeaderFile


//! Identifies a type of parameterization of a circle or ellipse represented as a BSpline curve.
//! For a circle with a center C and a radius R (for example a Geom2d_Circle or a Geom_Circle),
//! the natural parameterization is angular. It uses the angle Theta made by the vector CM with
//! the 'X Axis' of the circle's local coordinate system as parameter for the current point M. The
//! coordinates of the point M are as follows:
//! X   =   R *cos ( Theta )
//! y   =   R * sin ( Theta )
//! Similarly, for an ellipse with a center C, a major radius R and a minor radius r, the circle Circ
//! with center C and radius R (and located in the same plane as the ellipse) lends its natural
//! angular parameterization to the ellipse. This is achieved by an affine transformation in the plane
//! of the ellipse, in the ratio r / R, about the 'X Axis' of its local coordinate system. The
//! coordinates of the current point M are as follows:
//! X   =   R * cos ( Theta )
//! y   =   r * sin ( Theta )
//! The process of converting a circle or an ellipse into a rational or non-rational BSpline curve
//! transforms the Theta angular parameter into a parameter t. This ensures the rational or
//! polynomial parameterization of the resulting BSpline curve. Several types of parametric
//! transformations are available.
//! TgtThetaOver2
//! The most usual method is Convert_TgtThetaOver2 where the parameter t on the BSpline
//! curve is obtained by means of transformation of the following type:
//! t = tan ( Theta / 2 )
//! The result of this definition is:
//! cos ( Theta ) = ( 1. - t**2 ) / ( 1. + t**2 )
//! sin ( Theta ) =      2. * t / ( 1. + t**2 )
//! which ensures the rational parameterization of the circle or the ellipse. However, this is not the
//! most suitable parameterization method where the arc of the circle or ellipse has a large opening
//! angle. In such cases, the curve will be represented by a BSpline with intermediate knots. Each
//! span, i.e. each portion of curve between two different knot values, will use parameterization of
//! this type.
//! The number of spans is calculated using the following rule:
//! ( 1.2 * Delta / Pi ) + 1
//! where Delta is equal to the opening angle (in radians) of the arc of the circle (Delta is
//! equal to 2.* Pi in the case of a complete circle).
//! The resulting BSpline curve is "exact", i.e. computing any point of parameter t on the BSpline
//! curve gives an exact point on the circle or the ellipse.
//! TgtThetaOver2_N
//! Where N is equal to 1, 2, 3 or 4, this ensures the same type of parameterization as
//! Convert_TgtThetaOver2 but sets the number of spans in the resulting BSpline curve to N
//! rather than allowing the algorithm to make this calculation.
//! However, the opening angle Delta (parametric angle, given in radians) of the arc of the circle
//! (or of the ellipse) must comply with the following:
//! -   Delta <= 0.9999 * Pi for the Convert_TgtThetaOver2_1 method, or
//! -   Delta <= 1.9999 * Pi for the Convert_TgtThetaOver2_2 method.
//! QuasiAngular
//! The Convert_QuasiAngular method of parameterization uses a different type of rational
//! parameterization. This method ensures that the parameter t along the resulting BSpline curve is
//! very close to the natural parameterization angle Theta of the circle or ellipse (i.e. which uses
//! the functions sin ( Theta ) and cos ( Theta ).
//! The resulting BSpline curve is "exact", i.e. computing any point of parameter t on the BSpline
//! curve gives an exact point on the circle or the ellipse.
//! RationalC1
//! The Convert_RationalC1 method of parameterization uses a further type of rational
//! parameterization. This method ensures that the equation relating to the resulting BSpline curve
//! has a "C1" continuous denominator, which is not the case with the above methods. RationalC1
//! enhances the degree of continuity at the junction point of the different spans of the curve.
//! The resulting BSpline curve is "exact", i.e. computing any point of parameter t on the BSpline
//! curve gives an exact point on the circle or the ellipse.
//! Polynomial
//! The Convert_Polynomial method is used to produce polynomial (i.e. non-rational)
//! parameterization of the resulting BSpline curve with 8 poles (i.e. a polynomial degree equal to 7).
//! However, the result is an approximation of the circle or ellipse (i.e. computing the point of
//! parameter t on the BSpline curve does not give an exact point on the circle or the ellipse).
enum Convert_ParameterisationType
{
Convert_TgtThetaOver2,
Convert_TgtThetaOver2_1,
Convert_TgtThetaOver2_2,
Convert_TgtThetaOver2_3,
Convert_TgtThetaOver2_4,
Convert_QuasiAngular,
Convert_RationalC1,
Convert_Polynomial
};

#endif // _Convert_ParameterisationType_HeaderFile
