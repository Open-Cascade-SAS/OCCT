// Created on: 1998-12-16
// Created by: Roman LYGIN
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _IGESToBRep_IGESBoundary_HeaderFile
#define _IGESToBRep_IGESBoundary_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESToBRep_CurveAndSurface.hxx>
#include <TopoDS_Face.hxx>
#include <gp_Trsf2d.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
class IGESData_IGESEntity;
class ShapeExtend_WireData;


class IGESToBRep_IGESBoundary;
DEFINE_STANDARD_HANDLE(IGESToBRep_IGESBoundary, Standard_Transient)

//! This class is intended to translate IGES boundary entity
//! (142-CurveOnSurface, 141-Boundary or 508-Loop) into the wire.
//! Methods Transfer are virtual and are redefined in Advanced
//! Data Exchange to optimize the translation and take into
//! account advanced parameters.
class IGESToBRep_IGESBoundary : public Standard_Transient
{

public:

  
  //! Empty constructor
  Standard_EXPORT IGESToBRep_IGESBoundary();
  
  //! Empty constructor
  Standard_EXPORT IGESToBRep_IGESBoundary(const IGESToBRep_CurveAndSurface& CS);
  
  //! Inits the object with parameters common for all
  //! types of IGES boundaries.
  //! <CS>: object to be used for retrieving translation parameters
  //! and sending messages,
  //! <entity>: boundary entity to be processed,
  //! <face>, <trans>, <uFact>: as for IGESToBRep_TopoCurve
  //! <filepreference>: preferred representation (2 or 3) given
  //! in the IGES file
  Standard_EXPORT void Init (const IGESToBRep_CurveAndSurface& CS, const Handle(IGESData_IGESEntity)& entity, const TopoDS_Face& face, const gp_Trsf2d& trans, const Standard_Real uFact, const Standard_Integer filepreference);
  
  //! Returns the resulting wire
    Handle(ShapeExtend_WireData) WireData() const;
  
  //! Returns the wire from 3D curves (edges contain 3D curves
  //! and may contain pcurves)
    Handle(ShapeExtend_WireData) WireData3d() const;
  
  //! Returns the wire from 2D curves (edges contain pcurves
  //! only)
    Handle(ShapeExtend_WireData) WireData2d() const;
  
  //! Translates 141 and 142 entities.
  //! Returns True if the curve has been successfully translated,
  //! otherwise returns False.
  //! <okCurve..>: flags that indicate whether corresponding
  //! representation has been successfully translated
  //! (must be set to True before first call),
  //! <curve3d>: model space curve for 142 and current model space
  //! curve for 141,
  //! <toreverse3d>: False for 142 and current orientation flag
  //! for 141,
  //! <curves2d>: 1 parameter space curve for 142 or list of
  //! them for current model space curves for 141,
  //! <number>: 1 for 142 and rank number of model space curve for 141.
  Standard_EXPORT Standard_Boolean Transfer (Standard_Boolean& okCurve, Standard_Boolean& okCurve3d, Standard_Boolean& okCurve2d, const Handle(IGESData_IGESEntity)& curve3d, const Standard_Boolean toreverse3d, const Handle(IGESData_HArray1OfIGESEntity)& curves2d, const Standard_Integer number);
  
  //! Translates 508 entity.
  //! Returns True if the curve has been successfully translated,
  //! otherwise returns False.
  //! Input object IGESBoundary must be created and initialized
  //! before.
  //! <okCurve..>: flags that indicate whether corresponding
  //! representation has been successfully translated
  //! (must be set to True before first call),
  //! <curve3d>: result of translation of current edge,
  //! <curves2d>: list of parameter space curves for edge,
  //! <toreverse2d>: orientation flag of current edge in respect
  //! to its model space curve,
  //! <number>: rank number of edge,
  //! <lsewd>: returns the result of translation of current edge.
  Standard_EXPORT Standard_Boolean Transfer (Standard_Boolean& okCurve, Standard_Boolean& okCurve3d, Standard_Boolean& okCurve2d, const Handle(ShapeExtend_WireData)& curve3d, const Handle(IGESData_HArray1OfIGESEntity)& curves2d, const Standard_Boolean toreverse2d, const Standard_Integer number, Handle(ShapeExtend_WireData)& lsewd);
  
  //! Checks result of translation of IGES boundary entities
  //! (types 141, 142 or 508).
  //! Checks consistency of 2D and 3D representations and keeps
  //! only one if they are inconsistent.
  //! <result>: result of translation (returned by Transfer),
  //! <checkclosure>: False for 142 without parent 144 entity,
  //! otherwise True,
  //! <okCurve3d>, <okCurve2d>: those returned by Transfer.
  Standard_EXPORT virtual void Check (const Standard_Boolean result, const Standard_Boolean checkclosure, const Standard_Boolean okCurve3d, const Standard_Boolean okCurve2d);




  DEFINE_STANDARD_RTTIEXT(IGESToBRep_IGESBoundary,Standard_Transient)

protected:

  
  //! Methods called by both Transfer methods.
  Standard_EXPORT virtual Standard_Boolean Transfer (Standard_Boolean& okCurve, Standard_Boolean& okCurve3d, Standard_Boolean& okCurve2d, const Handle(IGESData_IGESEntity)& icurve3d, const Handle(ShapeExtend_WireData)& scurve3d, const Standard_Boolean usescurve, const Standard_Boolean toreverse3d, const Handle(IGESData_HArray1OfIGESEntity)& curves2d, const Standard_Boolean toreverse2d, const Standard_Integer number, Handle(ShapeExtend_WireData)& lsewd);
  
  Standard_EXPORT static void ReverseCurves3d (const Handle(ShapeExtend_WireData)& sewd);
  
  Standard_EXPORT static void ReverseCurves2d (const Handle(ShapeExtend_WireData)& sewd, const TopoDS_Face& face);

  IGESToBRep_CurveAndSurface myCS;
  Handle(IGESData_IGESEntity) myentity;
  Handle(ShapeExtend_WireData) mysewd;
  Handle(ShapeExtend_WireData) mysewd3d;
  Handle(ShapeExtend_WireData) mysewd2d;
  TopoDS_Face myface;
  gp_Trsf2d mytrsf;
  Standard_Real myuFact;
  Standard_Integer myfilepreference;


private:




};


#include <IGESToBRep_IGESBoundary.lxx>





#endif // _IGESToBRep_IGESBoundary_HeaderFile
