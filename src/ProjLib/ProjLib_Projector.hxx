// Created on: 1993-08-11
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

#ifndef _ProjLib_Projector_HeaderFile
#define _ProjLib_Projector_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomAbs_CurveType.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Parab2d.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class Geom2d_BSplineCurve;
class Geom2d_BezierCurve;
class Standard_NoSuchObject;
class Standard_NotImplemented;
class gp_Lin2d;
class gp_Circ2d;
class gp_Elips2d;
class gp_Hypr2d;
class gp_Parab2d;
class gp_Lin;
class gp_Circ;
class gp_Elips;
class gp_Parab;
class gp_Hypr;


//! Root class for projection algorithms, stores the result.
class ProjLib_Projector 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Sets the type to OtherCurve
  Standard_EXPORT ProjLib_Projector();
  Standard_EXPORT virtual ~ProjLib_Projector();
  
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Set isDone = Standard_True;
  Standard_EXPORT void Done();
  
  Standard_EXPORT GeomAbs_CurveType GetType() const;
  
  Standard_EXPORT void SetBSpline (const Handle(Geom2d_BSplineCurve)& C);
  
  Standard_EXPORT void SetBezier (const Handle(Geom2d_BezierCurve)& C);
  
  Standard_EXPORT void SetType (const GeomAbs_CurveType Type);
  
  Standard_EXPORT Standard_Boolean IsPeriodic() const;
  
  Standard_EXPORT void SetPeriodic();
  
  Standard_EXPORT const gp_Lin2d& Line() const;
  
  Standard_EXPORT const gp_Circ2d& Circle() const;
  
  Standard_EXPORT const gp_Elips2d& Ellipse() const;
  
  Standard_EXPORT const gp_Hypr2d& Hyperbola() const;
  
  Standard_EXPORT const gp_Parab2d& Parabola() const;
  
  Standard_EXPORT Handle(Geom2d_BezierCurve) Bezier() const;
  
  Standard_EXPORT Handle(Geom2d_BSplineCurve) BSpline() const;
  
  Standard_EXPORT virtual void Project (const gp_Lin& L);
  
  Standard_EXPORT virtual void Project (const gp_Circ& C);
  
  Standard_EXPORT virtual void Project (const gp_Elips& E);
  
  Standard_EXPORT virtual void Project (const gp_Parab& P);
  
  Standard_EXPORT virtual void Project (const gp_Hypr& H);
  
  //! Translates the 2d curve
  //! to set the part of the curve [CFirst, CLast]
  //! in the range [ UFirst, UFirst + Period [
  Standard_EXPORT void UFrame (const Standard_Real CFirst, const Standard_Real CLast, const Standard_Real UFirst, const Standard_Real Period);
  
  //! Translates the 2d curve
  //! to set the part of the curve [CFirst, CLast]
  //! in the range [ VFirst, VFirst + Period [
  Standard_EXPORT void VFrame (const Standard_Real CFirst, const Standard_Real CLast, const Standard_Real VFirst, const Standard_Real Period);




protected:



  GeomAbs_CurveType myType;
  gp_Lin2d myLin;
  gp_Circ2d myCirc;
  gp_Elips2d myElips;
  gp_Hypr2d myHypr;
  gp_Parab2d myParab;
  Handle(Geom2d_BSplineCurve) myBSpline;
  Handle(Geom2d_BezierCurve) myBezier;
  Standard_Boolean myIsPeriodic;
  Standard_Boolean isDone;


private:





};







#endif // _ProjLib_Projector_HeaderFile
