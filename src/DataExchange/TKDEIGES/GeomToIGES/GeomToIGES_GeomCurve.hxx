// Created on: 1994-11-17
// Created by: Marie Jose MARTZ
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

#ifndef _GeomToIGES_GeomCurve_HeaderFile
#define _GeomToIGES_GeomCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomToIGES_GeomEntity.hxx>
class IGESData_IGESEntity;
class Geom_Curve;
class Geom_BoundedCurve;
class Geom_BSplineCurve;
class Geom_BezierCurve;
class Geom_TrimmedCurve;
class Geom_Conic;
class Geom_Circle;
class Geom_Ellipse;
class Geom_Hyperbola;
class Geom_Line;
class Geom_Parabola;
class Geom_OffsetCurve;

//! This class implements the transfer of the Curve Entity from Geom
//! To IGES. These can be :
//! Curve
//! . BoundedCurve
//! * BSplineCurve
//! * BezierCurve
//! * TrimmedCurve
//! . Conic
//! * Circle
//! * Ellipse
//! * Hyperbloa
//! * Line
//! * Parabola
//! . OffsetCurve
class GeomToIGES_GeomCurve : public GeomToIGES_GeomEntity
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT GeomToIGES_GeomCurve();

  //! Creates a tool GeomCurve ready to run and sets its
  //! fields as GE's.
  Standard_EXPORT GeomToIGES_GeomCurve(const GeomToIGES_GeomEntity& GE);

  //! Transfert  a  GeometryEntity which  answer True  to  the
  //! member : BRepToIGES::IsGeomCurve(Geometry).  If this
  //! Entity could not be converted, this member returns a NullEntity.
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferCurve(const Handle(Geom_Curve)& start,
                                                            const Standard_Real       Udeb,
                                                            const Standard_Real       Ufin);

  Standard_EXPORT Handle(IGESData_IGESEntity) TransferCurve(const Handle(Geom_BoundedCurve)& start,
                                                            const Standard_Real              Udeb,
                                                            const Standard_Real              Ufin);

  Standard_EXPORT Handle(IGESData_IGESEntity) TransferCurve(const Handle(Geom_BSplineCurve)& start,
                                                            const Standard_Real              Udeb,
                                                            const Standard_Real              Ufin);

  Standard_EXPORT Handle(IGESData_IGESEntity) TransferCurve(const Handle(Geom_BezierCurve)& start,
                                                            const Standard_Real             Udeb,
                                                            const Standard_Real             Ufin);

  Standard_EXPORT Handle(IGESData_IGESEntity) TransferCurve(const Handle(Geom_TrimmedCurve)& start,
                                                            const Standard_Real              Udeb,
                                                            const Standard_Real              Ufin);

  Standard_EXPORT Handle(IGESData_IGESEntity) TransferCurve(const Handle(Geom_Conic)& start,
                                                            const Standard_Real       Udeb,
                                                            const Standard_Real       Ufin);

  Standard_EXPORT Handle(IGESData_IGESEntity) TransferCurve(const Handle(Geom_Circle)& start,
                                                            const Standard_Real        Udeb,
                                                            const Standard_Real        Ufin);

  Standard_EXPORT Handle(IGESData_IGESEntity) TransferCurve(const Handle(Geom_Ellipse)& start,
                                                            const Standard_Real         Udeb,
                                                            const Standard_Real         Ufin);

  Standard_EXPORT Handle(IGESData_IGESEntity) TransferCurve(const Handle(Geom_Hyperbola)& start,
                                                            const Standard_Real           Udeb,
                                                            const Standard_Real           Ufin);

  Standard_EXPORT Handle(IGESData_IGESEntity) TransferCurve(const Handle(Geom_Line)& start,
                                                            const Standard_Real      Udeb,
                                                            const Standard_Real      Ufin);

  Standard_EXPORT Handle(IGESData_IGESEntity) TransferCurve(const Handle(Geom_Parabola)& start,
                                                            const Standard_Real          Udeb,
                                                            const Standard_Real          Ufin);

  Standard_EXPORT Handle(IGESData_IGESEntity) TransferCurve(const Handle(Geom_OffsetCurve)& start,
                                                            const Standard_Real             Udeb,
                                                            const Standard_Real             Ufin);

protected:
private:
};

#endif // _GeomToIGES_GeomCurve_HeaderFile
