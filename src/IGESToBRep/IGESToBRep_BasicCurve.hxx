// Created on: 1994-03-14
// Created by: s:	Christophe GUYOT & Frederic UNTEREINER
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

#ifndef _IGESToBRep_BasicCurve_HeaderFile
#define _IGESToBRep_BasicCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <IGESToBRep_CurveAndSurface.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
class Geom_Curve;
class IGESData_IGESEntity;
class Geom2d_Curve;
class IGESGeom_BSplineCurve;
class IGESGeom_CircularArc;
class IGESGeom_ConicArc;
class Geom_BSplineCurve;
class IGESGeom_CopiousData;
class Geom2d_BSplineCurve;
class IGESGeom_Line;
class IGESGeom_SplineCurve;
class Geom_Transformation;
class IGESGeom_TransformationMatrix;


//! Provides methods to transfer basic geometric curves entities
//! from IGES to CASCADE.
//! These can be :
//! * Circular arc
//! * Conic arc
//! * Spline curve
//! * BSpline curve
//! * Line
//! * Copious data
//! * Point
//! * Transformation matrix
class IGESToBRep_BasicCurve  : public IGESToBRep_CurveAndSurface
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates  a tool BasicCurve  ready  to  run, with
  //! epsilons  set  to  1.E-04,  TheModeTopo  to  True,  the
  //! optimization of  the continuity to False.
  Standard_EXPORT IGESToBRep_BasicCurve();
  
  //! Creates a tool BasicCurve ready to run and sets its
  //! fields as CS's.
  Standard_EXPORT IGESToBRep_BasicCurve(const IGESToBRep_CurveAndSurface& CS);
  
  //! Creates a tool BasicCurve ready to run.
  Standard_EXPORT IGESToBRep_BasicCurve(const Standard_Real eps, const Standard_Real epsGeom, const Standard_Real epsCoeff, const Standard_Boolean mode, const Standard_Boolean modeapprox, const Standard_Boolean optimized);
  
  //! Transfert  a  IGESEntity which  answer True  to  the
  //! member : IGESToBRep::IsBasicCurve(IGESEntity).  If this
  //! Entity could not be converted, this member returns a NullEntity.
  Standard_EXPORT Handle(Geom_Curve) TransferBasicCurve (const Handle(IGESData_IGESEntity)& start);
  
  //! Transfert  a  IGESEntity which  answer True  to  the
  //! member : IGESToBRep::IsBasicCurve(IGESEntity).
  //! The IGESEntity must be a curve UV and its associed TRSF must
  //! be planar .If this Entity could not be converted, this member
  //! returns a NullEntity.
  Standard_EXPORT Handle(Geom2d_Curve) Transfer2dBasicCurve (const Handle(IGESData_IGESEntity)& start);
  
  Standard_EXPORT Handle(Geom_Curve) TransferBSplineCurve (const Handle(IGESGeom_BSplineCurve)& start);
  
  Standard_EXPORT Handle(Geom2d_Curve) Transfer2dBSplineCurve (const Handle(IGESGeom_BSplineCurve)& start);
  
  Standard_EXPORT Handle(Geom_Curve) TransferCircularArc (const Handle(IGESGeom_CircularArc)& start);
  
  Standard_EXPORT Handle(Geom2d_Curve) Transfer2dCircularArc (const Handle(IGESGeom_CircularArc)& start);
  
  Standard_EXPORT Handle(Geom_Curve) TransferConicArc (const Handle(IGESGeom_ConicArc)& start);
  
  Standard_EXPORT Handle(Geom2d_Curve) Transfer2dConicArc (const Handle(IGESGeom_ConicArc)& start);
  
  Standard_EXPORT Handle(Geom_BSplineCurve) TransferCopiousData (const Handle(IGESGeom_CopiousData)& start);
  
  Standard_EXPORT Handle(Geom2d_BSplineCurve) Transfer2dCopiousData (const Handle(IGESGeom_CopiousData)& start);
  
  Standard_EXPORT Handle(Geom_Curve) TransferLine (const Handle(IGESGeom_Line)& start);
  
  Standard_EXPORT Handle(Geom2d_Curve) Transfer2dLine (const Handle(IGESGeom_Line)& start);
  
  Standard_EXPORT Handle(Geom_BSplineCurve) TransferSplineCurve (const Handle(IGESGeom_SplineCurve)& start);
  
  Standard_EXPORT Handle(Geom2d_BSplineCurve) Transfer2dSplineCurve (const Handle(IGESGeom_SplineCurve)& start);
  
  Standard_EXPORT Handle(Geom_Transformation) TransferTransformation (const Handle(IGESGeom_TransformationMatrix)& start);




protected:





private:





};







#endif // _IGESToBRep_BasicCurve_HeaderFile
