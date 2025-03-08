// Created on: 2000-02-05
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _IGESControl_IGESBoundary_HeaderFile
#define _IGESControl_IGESBoundary_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESToBRep_IGESBoundary.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <Standard_Integer.hxx>
class IGESToBRep_CurveAndSurface;
class IGESData_IGESEntity;
class ShapeExtend_WireData;

class IGESControl_IGESBoundary;
DEFINE_STANDARD_HANDLE(IGESControl_IGESBoundary, IGESToBRep_IGESBoundary)

//! Translates IGES boundary entity (types 141, 142 and 508)
//! in Advanced Data Exchange.
//! Redefines translation and treatment methods from inherited
//! open class IGESToBRep_IGESBoundary.
class IGESControl_IGESBoundary : public IGESToBRep_IGESBoundary
{

public:
  //! Creates an object and calls inherited constructor.
  Standard_EXPORT IGESControl_IGESBoundary();

  //! Creates an object and calls inherited constructor.
  Standard_EXPORT IGESControl_IGESBoundary(const IGESToBRep_CurveAndSurface& CS);

  //! Checks result of translation of IGES boundary entities
  //! (types 141, 142 or 508).
  //! Checks consistency of 2D and 3D representations and keeps
  //! only one if they are inconsistent.
  //! Checks the closure of resulting wire and if it is not closed,
  //! checks 2D and 3D representation and updates the resulting
  //! wire to contain only closed representation.
  Standard_EXPORT virtual void Check(const Standard_Boolean result,
                                     const Standard_Boolean checkclosure,
                                     const Standard_Boolean okCurve3d,
                                     const Standard_Boolean okCurve2d) Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESControl_IGESBoundary, IGESToBRep_IGESBoundary)

protected:
  Standard_EXPORT virtual Standard_Boolean Transfer(
    Standard_Boolean&                           okCurve,
    Standard_Boolean&                           okCurve3d,
    Standard_Boolean&                           okCurve2d,
    const Handle(IGESData_IGESEntity)&          icurve3d,
    const Handle(ShapeExtend_WireData)&         scurve3d,
    const Standard_Boolean                      usescurve,
    const Standard_Boolean                      toreverse3d,
    const Handle(IGESData_HArray1OfIGESEntity)& curves2d,
    const Standard_Boolean                      toreverse2d,
    const Standard_Integer                      number,
    Handle(ShapeExtend_WireData)&               lsewd) Standard_OVERRIDE;
};

#endif // _IGESControl_IGESBoundary_HeaderFile
