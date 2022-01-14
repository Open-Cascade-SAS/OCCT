// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen (Kiran)
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

#ifndef _IGESGeom_ConicArc_HeaderFile
#define _IGESGeom_ConicArc_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_XY.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Integer.hxx>
class gp_Pnt2d;
class gp_Pnt;
class gp_Dir;


class IGESGeom_ConicArc;
DEFINE_STANDARD_HANDLE(IGESGeom_ConicArc, IGESData_IGESEntity)

//! defines IGESConicArc, Type <104> Form <0-3>  in package IGESGeom
//! A conic arc is a bounded connected portion of a parent
//! conic curve which consists of more than one point. The
//! parent conic curve is either an ellipse, a parabola, or
//! a hyperbola. The definition space coordinate system is
//! always chosen so that the conic arc lies in a plane either
//! coincident with or parallel to XT, YT plane. Within such
//! a plane a conic is defined by the six coefficients in the
//! following equation.
//! A*XT^2 + B*XT*YT + C*YT^2 + D*XT + E*YT + F = 0
class IGESGeom_ConicArc : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESGeom_ConicArc();
  
  //! This method is used to set the fields of the class
  //! ConicalArc
  //! - A, B, C, D, E, F : Coefficients of the equation
  //! defining conic arc
  //! - ZT               : Parallel ZT displacement of the arc
  //! from XT, YT plane.
  //! - aStart           : Starting point of the conic arc
  //! - anEnd            : End point of the conic arc
  Standard_EXPORT void Init (const Standard_Real A, const Standard_Real B, const Standard_Real C, const Standard_Real D, const Standard_Real E, const Standard_Real F, const Standard_Real ZT, const gp_XY& aStart, const gp_XY& anEnd);
  
  //! sets the Form Number equal to ComputedFormNumber,
  //! returns True if changed
  Standard_EXPORT Standard_Boolean OwnCorrect();
  
  //! Computes the Form Number according to the equation
  //! 1 for Ellipse, 2 for Hyperbola, 3 for Parabola
  Standard_EXPORT Standard_Integer ComputedFormNumber() const;
  
  Standard_EXPORT void Equation (Standard_Real& A, Standard_Real& B, Standard_Real& C, Standard_Real& D, Standard_Real& E, Standard_Real& F) const;
  
  //! returns the Z displacement of the arc from XT, YT plane
  Standard_EXPORT Standard_Real ZPlane() const;
  
  //! returns the starting point of the arc
  Standard_EXPORT gp_Pnt2d StartPoint() const;
  
  //! returns the starting point of the arc after applying
  //! Transf. Matrix
  Standard_EXPORT gp_Pnt TransformedStartPoint() const;
  
  //! returns the end point of the arc
  Standard_EXPORT gp_Pnt2d EndPoint() const;
  
  //! returns the end point of the arc after applying
  //! Transf. Matrix
  Standard_EXPORT gp_Pnt TransformedEndPoint() const;
  
  //! returns True if parent conic curve is an ellipse
  Standard_EXPORT Standard_Boolean IsFromEllipse() const;
  
  //! returns True if parent conic curve is a parabola
  Standard_EXPORT Standard_Boolean IsFromParabola() const;
  
  //! returns True if parent conic curve is a hyperbola
  Standard_EXPORT Standard_Boolean IsFromHyperbola() const;
  
  //! returns True if StartPoint = EndPoint
  Standard_EXPORT Standard_Boolean IsClosed() const;
  
  //! Z-Axis of conic (i.e. [0,0,1])
  Standard_EXPORT gp_Dir Axis() const;
  
  //! Z-Axis after applying Trans. Matrix
  Standard_EXPORT gp_Dir TransformedAxis() const;
  
  //! Returns a Definition computed from equation, easier to use
  //! <Center> : the center of the conic (meaningless for
  //! a parabola) (defined with Z displacement)
  //! <MainAxis> : the Main Axis of the conic (for a Circle,
  //! arbitrary the X Axis)
  //! <Rmin,Rmax> : Minor and Major Radii of the conic
  //! For a Circle, Rmin = Rmax,
  //! For a Parabola, Rmin = Rmax = the Focal
  //! Warning : the basic definition (by equation) is not very stable,
  //! limit cases may be approximative
  Standard_EXPORT void Definition (gp_Pnt& Center, gp_Dir& MainAxis, Standard_Real& rmin, Standard_Real& rmax) const;
  
  //! Same as Definition, but the Location is applied on the
  //! Center and the MainAxis
  Standard_EXPORT void TransformedDefinition (gp_Pnt& Center, gp_Dir& MainAxis, Standard_Real& rmin, Standard_Real& rmax) const;
  
  //! Computes and returns the coordinates of the definition of
  //! a comic from its equation. Used by Definition &
  //! TransformedDefinition, or may be called directly if needed
  Standard_EXPORT void ComputedDefinition (Standard_Real& Xcen, Standard_Real& Ycen, Standard_Real& Xax, Standard_Real& Yax, Standard_Real& Rmin, Standard_Real& Rmax) const;




  DEFINE_STANDARD_RTTIEXT(IGESGeom_ConicArc,IGESData_IGESEntity)

protected:




private:


  Standard_Real theA;
  Standard_Real theB;
  Standard_Real theC;
  Standard_Real theD;
  Standard_Real theE;
  Standard_Real theF;
  Standard_Real theZT;
  gp_XY theStart;
  gp_XY theEnd;


};







#endif // _IGESGeom_ConicArc_HeaderFile
