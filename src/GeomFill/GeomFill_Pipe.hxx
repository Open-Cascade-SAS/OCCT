// Created on: 1994-04-13
// Created by: Eric BONNARDEL
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _GeomFill_Pipe_HeaderFile
#define _GeomFill_Pipe_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <GeomFill_Trihedron.hxx>
#include <GeomAbs_Shape.hxx>
#include <TColGeom_SequenceOfCurve.hxx>
#include <GeomFill_PipeError.hxx>

class Geom_Surface;
class GeomFill_LocationLaw;
class GeomFill_SectionLaw;
class Geom_Curve;
class Geom2d_Curve;
class gp_Dir;

//! Describes functions to construct pipes. A pipe is built by
//! sweeping a curve (the section) along another curve (the path).
//! The Pipe class provides the following types of construction:
//! -   pipes with a circular section of constant radius,
//! -   pipes with a constant section,
//! -   pipes with a section evolving between two given curves.
//! All standard specific cases are detected in order to build,
//! where required, a plane, cylinder, cone, sphere, torus,
//! surface of linear extrusion or surface of revolution.
//! Generally speaking, the result is a BSpline surface (NURBS).
//! A Pipe object provides a framework for:
//! -   defining the pipe to be built,
//! -   implementing the construction algorithm, and
//! -   consulting the resulting surface.
//! There are several methods to instantiate a Pipe:
//! 1) give a path and  a radius : the section is
//! a circle.  This location  is the first  point
//! of the path,  and this direction is the first
//! derivate (calculate at  the  first point ) of
//! the path.
//!
//! 2) give a path and a section.
//! Differtent options are available
//! 2.a) Use the classical Frenet trihedron
//! - or the CorrectedFrenet trihedron
//! (To avoid twisted surface)
//! - or a constant trihedron to have all the sections
//! in a same plane
//! 2.b) Define a ConstantBinormal Direction to keep the
//! same angle between the Direction and the sections
//! along the sweep surface.
//! 2.c) Define the path by a surface and a 2dcurve,
//! the surface is used to define the trihedron's normal.
//! It is useful to keep a constant angle between
//! input surface and the pipe.                           --
//! 3) give a  path and two sections. The section
//! evaluate from First to Last Section.
//!
//! 3) give a  path and N sections. The section
//! evaluate from First to Last Section.
//!
//! In general case the result is a NURBS. But we
//! can  generate plane,  cylindrical, spherical,
//! conical, toroidal surface in some particular case.
//!
//! The natural parametrization of the result is:
//!
//! U-Direction along the section.
//! V-Direction along the path.
//!
//! But, in some particular case, the surface must
//! be construct otherwise.
//! The method "EchangeUV" return false in such cases.
class GeomFill_Pipe 
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! Constructs an empty algorithm for building pipes. Use
  //! the function Init to initialize it.
  Standard_EXPORT GeomFill_Pipe();
  
  Standard_EXPORT GeomFill_Pipe(const Handle(Geom_Curve)& Path, const Standard_Real Radius);
  
  //! Create  a  pipe  with  a  constant  section
  //! (<FirstSection>)  and a path (<Path>)
  //! Option can be  - GeomFill_IsCorrectedFrenet
  //! - GeomFill_IsFrenet
  //! - GeomFill_IsConstant
  Standard_EXPORT GeomFill_Pipe(const Handle(Geom_Curve)& Path, const Handle(Geom_Curve)& FirstSect, const GeomFill_Trihedron Option = GeomFill_IsCorrectedFrenet);
  
  //! Create  a  pipe  with  a  constant  section
  //! (<FirstSection>)  and a path defined by <Path> and <Support>
  Standard_EXPORT GeomFill_Pipe(const Handle(Geom2d_Curve)& Path, const Handle(Geom_Surface)& Support, const Handle(Geom_Curve)& FirstSect);
  
  //! Create  a  pipe with  a  constant section
  //! (<FirstSection>) and a   path <Path>  and a fixed
  //! binormal direction <Dir>
  Standard_EXPORT GeomFill_Pipe(const Handle(Geom_Curve)& Path, const Handle(Geom_Curve)& FirstSect, const gp_Dir& Dir);
  
  //! Create a pipe with an evolving section
  //! The section evaluate from First to Last Section
  Standard_EXPORT GeomFill_Pipe(const Handle(Geom_Curve)& Path, const Handle(Geom_Curve)& FirstSect, const Handle(Geom_Curve)& LastSect);
  
  //! Create a pipe with N  sections
  //! The section evaluate from First to Last Section
  Standard_EXPORT GeomFill_Pipe(const Handle(Geom_Curve)& Path, const TColGeom_SequenceOfCurve& NSections);
  
  //! Create  a pipe  with  a constant  radius with  2
  //! guide-line.
  Standard_EXPORT GeomFill_Pipe(const Handle(Geom_Curve)& Path, const Handle(Geom_Curve)& Curve1, const Handle(Geom_Curve)& Curve2, const Standard_Real Radius);
  
  //! Create  a pipe  with  a constant  radius with  2
  //! guide-line.
  Standard_EXPORT GeomFill_Pipe(const Handle(Adaptor3d_Curve)& Path, const Handle(Adaptor3d_Curve)& Curve1, const Handle(Adaptor3d_Curve)& Curve2, const Standard_Real Radius);
  
  //! Create a pipe with a constant section and  with 1
  //! guide-line.
  //! Use the function Perform to build the surface.
  //! All standard specific cases are detected in order to
  //! construct, according to the respective geometric
  //! nature of Path and the sections, a planar, cylindrical,
  //! conical, spherical or toroidal surface, a surface of
  //! linear extrusion or a surface of revolution.
  //! In the general case, the result is a BSpline surface
  //! (NURBS) built by approximation of a series of sections where:
  //! -   the number of sections N is chosen automatically
  //! by the algorithm according to the respective
  //! geometries of Path and the sections. N is greater than or equal to 2;
  //! -   N points Pi (with i in the range [ 1,N ]) are
  //! defined at regular intervals along the curve Path
  //! from its first point to its end point. At each point Pi,
  //! a coordinate system Ti is computed with Pi as
  //! origin, and with the tangential and normal vectors
  //! to Path defining two of its coordinate axes.
  //! In the case of a pipe with a constant circular section,
  //! the first section is a circle of radius Radius centered
  //! on the origin of Path and whose "Z Axis" is aligned
  //! along the vector tangential to the origin of Path. In the
  //! case of a pipe with a constant section, the first section
  //! is the curve FirstSect. In these two cases, the ith
  //! section (for values of i greater than 1) is obtained by
  //! applying to a copy of this first section the geometric
  //! transformation which transforms coordinate system
  //! T1 into coordinate system Ti.
  //! In the case of an evolving section, N-2 intermediate
  //! curves Si are first computed (if N is greater than 2,
  //! and with i in the range [ 2,N-1 ]) whose geometry
  //! evolves regularly from the curve S1=FirstSect to the
  //! curve SN=LastSect. The first section is FirstSect,
  //! and the ith section (for values of i greater than 1) is
  //! obtained by applying to the curve Si the geometric
  //! transformation which transforms coordinate system
  //! T1 into coordinate system Ti.
  Standard_EXPORT GeomFill_Pipe(const Handle(Geom_Curve)& Path, const Handle(Adaptor3d_Curve)& Guide, const Handle(Geom_Curve)& FirstSect, const Standard_Boolean ByACR, const Standard_Boolean rotat);
  
  Standard_EXPORT void Init (const Handle(Geom_Curve)& Path, const Standard_Real Radius);
  
  Standard_EXPORT void Init (const Handle(Geom_Curve)& Path, const Handle(Geom_Curve)& FirstSect, const GeomFill_Trihedron Option = GeomFill_IsCorrectedFrenet);
  
  Standard_EXPORT void Init (const Handle(Geom2d_Curve)& Path, const Handle(Geom_Surface)& Support, const Handle(Geom_Curve)& FirstSect);
  
  Standard_EXPORT void Init (const Handle(Geom_Curve)& Path, const Handle(Geom_Curve)& FirstSect, const gp_Dir& Dir);
  
  Standard_EXPORT void Init (const Handle(Geom_Curve)& Path, const Handle(Geom_Curve)& FirstSect, const Handle(Geom_Curve)& LastSect);
  
  Standard_EXPORT void Init (const Handle(Geom_Curve)& Path, const TColGeom_SequenceOfCurve& NSections);
  
  //! Create  a pipe  with  a constant  radius with  2
  //! guide-line.
  Standard_EXPORT void Init (const Handle(Adaptor3d_Curve)& Path, const Handle(Adaptor3d_Curve)& Curve1, const Handle(Adaptor3d_Curve)& Curve2, const Standard_Real Radius);
  

  //! Initializes this pipe algorithm to build the following surface:
  //! -   a pipe with a constant circular section of radius
  //! Radius along the path Path, or
  //! -   a pipe with constant section FirstSect along the path Path, or
  //! -   a pipe where the section evolves from FirstSect to
  //! LastSect along the path Path.
  //! Use the function Perform to build the surface.
  //! Note: a description of the resulting surface is given under Constructors.
  Standard_EXPORT void Init (const Handle(Geom_Curve)& Path, const Handle(Adaptor3d_Curve)& Guide, const Handle(Geom_Curve)& FirstSect, const Standard_Boolean ByACR, const Standard_Boolean rotat);
  
  //! Builds the pipe defined at the time of initialization of this
  //! algorithm. A description of the resulting surface is given under Constructors.
  //! If WithParameters (defaulted to false) is set to true, the
  //! approximation algorithm (used only in the general case
  //! of construction of a BSpline surface) builds the surface
  //! with a u parameter corresponding to the one of the path.
  //! Exceptions
  //! Standard_ConstructionError if a surface cannot be constructed from the data.
  //! Warning: It is the old Perform method, the next methode is recommended.
  Standard_EXPORT void Perform (const Standard_Boolean WithParameters = Standard_False, const Standard_Boolean myPolynomial = Standard_False);
  
  //! detects the  particular cases.  And compute the surface.
  //! if  none   particular  case  is  detected we make an approximation
  //! with respect of the Tolerance <Tol>, the continuty <Conti>, the
  //! maximum degree <MaxDegree>, the maximum number of span <NbMaxSegment>
  //! and the spine parametrization.
  //! If we can't create a surface with the data
  Standard_EXPORT void Perform (const Standard_Real Tol, const Standard_Boolean Polynomial, const GeomAbs_Shape Conti = GeomAbs_C1, const Standard_Integer MaxDegree = 11, const Standard_Integer NbMaxSegment = 30);
  
  //! Returns the surface built by this algorithm.
  //! Warning
  //! Do not use this function before the surface is built (in this
  //! case the function will return a null handle).
    const Handle(Geom_Surface)& Surface() const;
  
  //! The u parametric direction of the surface constructed by
  //! this algorithm usually corresponds to the evolution
  //! along the path and the v parametric direction
  //! corresponds to the evolution along the section(s).
  //! However, this rule is not respected when constructing
  //! certain specific Geom surfaces (typically cylindrical
  //! surfaces, surfaces of revolution, etc.) for which the
  //! parameterization is inversed.
  //! The ExchangeUV function checks for this, and returns
  //! true in all these specific cases.
  //! Warning
  //! Do not use this function before the surface is built.
  Standard_Boolean ExchangeUV() const;
  
  //! Sets a flag  to  try to   create as many   planes,
  //! cylinder,...    as  possible.  Default  value   is
  //! <Standard_False>.
    void GenerateParticularCase (const Standard_Boolean B);
  
  //! Returns the flag.
    Standard_Boolean GenerateParticularCase() const;
  
  //! Returns the approximation's error.  if the Surface
  //! is plane, cylinder ... this error can be 0.
    Standard_Real ErrorOnSurf() const;

  //! Returns whether approximation was done.
    Standard_Boolean IsDone() const;

  //! Returns execution status
  GeomFill_PipeError GetStatus() const
  {
    return myStatus;
  }

protected:





private:

  
  Standard_EXPORT void Init();
  
  //! The result  (<mySurface>)  is an approximation.  Using
  //! <SweepSectionGenerator>  to      do    that.        If
  //! <WithParameters>    is   set  to <Standard_True>,  the
  //! apprxoximation will be   done in respect to  the spine
  //! parametrization.
  Standard_EXPORT void ApproxSurf (const Standard_Boolean WithParameters);
  
  Standard_EXPORT Standard_Boolean KPartT4();

  GeomFill_PipeError myStatus;//!< Execution status
  Standard_Real myRadius;
  Standard_Real myError;
  Handle(Adaptor3d_Curve) myAdpPath;
  Handle(Adaptor3d_Curve) myAdpFirstSect;
  Handle(Adaptor3d_Curve) myAdpLastSect;
  Handle(Geom_Surface) mySurface;
  Handle(GeomFill_LocationLaw) myLoc;
  Handle(GeomFill_SectionLaw) mySec;
  Standard_Integer myType;
  Standard_Boolean myExchUV;
  Standard_Boolean myKPart;
  Standard_Boolean myPolynomial;
};


#include <GeomFill_Pipe.lxx>





#endif // _GeomFill_Pipe_HeaderFile
