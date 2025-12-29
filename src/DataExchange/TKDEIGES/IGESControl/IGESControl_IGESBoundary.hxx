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
#include <IGESData_IGESEntity.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>
class IGESToBRep_CurveAndSurface;
class IGESData_IGESEntity;
class ShapeExtend_WireData;

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
  Standard_EXPORT void Check(const bool result,
                             const bool checkclosure,
                             const bool okCurve3d,
                             const bool okCurve2d) override;

  DEFINE_STANDARD_RTTIEXT(IGESControl_IGESBoundary, IGESToBRep_IGESBoundary)

protected:
  Standard_EXPORT bool Transfer(
    bool&                                                                     okCurve,
    bool&                                                                     okCurve3d,
    bool&                                                                     okCurve2d,
    const occ::handle<IGESData_IGESEntity>&                                   icurve3d,
    const occ::handle<ShapeExtend_WireData>&                                  scurve3d,
    const bool                                                                usescurve,
    const bool                                                                toreverse3d,
    const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>& curves2d,
    const bool                                                                toreverse2d,
    const int                                                                 number,
    occ::handle<ShapeExtend_WireData>&                                        lsewd) override;
};

#endif // _IGESControl_IGESBoundary_HeaderFile
