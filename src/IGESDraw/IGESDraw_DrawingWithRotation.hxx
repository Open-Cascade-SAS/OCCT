// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( TCD )
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

#ifndef _IGESDraw_DrawingWithRotation_HeaderFile
#define _IGESDraw_DrawingWithRotation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESDraw_HArray1OfViewKindEntity.hxx>
#include <TColgp_HArray1OfXY.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
class IGESData_ViewKindEntity;
class gp_Pnt2d;
class gp_XY;
class gp_XYZ;


class IGESDraw_DrawingWithRotation;
DEFINE_STANDARD_HANDLE(IGESDraw_DrawingWithRotation, IGESData_IGESEntity)

//! defines IGESDrawingWithRotation, Type <404> Form <1>
//! in package IGESDraw
//!
//! Permits rotation, in addition to transformation and
//! scaling, between the view and drawing coordinate systems
class IGESDraw_DrawingWithRotation : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESDraw_DrawingWithRotation();
  
  //! This method is used to set the fields of the class
  //! DrawingWithRotation
  //! - allViews             : Pointers to View entities
  //! - allViewOrigins       : Origin coords of transformed views
  //! - allOrientationAngles : Orientation angles of transformed views
  //! - allAnnotations       : Pointers to Annotation entities
  //! raises exception if Lengths of allViews, allViewOrigins and
  //! allOrientationAngles are not same.
  Standard_EXPORT void Init (const Handle(IGESDraw_HArray1OfViewKindEntity)& allViews, const Handle(TColgp_HArray1OfXY)& allViewOrigins, const Handle(TColStd_HArray1OfReal)& allOrientationAngles, const Handle(IGESData_HArray1OfIGESEntity)& allAnnotations);
  
  //! returns the number of view pointers in <me>
  Standard_EXPORT Standard_Integer NbViews() const;
  
  //! returns the View entity indicated by Index
  //! raises an exception if Index <= 0 or Index > NbViews().
  Standard_EXPORT Handle(IGESData_ViewKindEntity) ViewItem (const Standard_Integer Index) const;
  
  //! returns the Drawing space coordinates of the origin of the
  //! Transformed view indicated by Index
  //! raises an exception if Index <= 0 or Index > NbViews().
  Standard_EXPORT gp_Pnt2d ViewOrigin (const Standard_Integer Index) const;
  
  //! returns the Orientation angle for the Transformed view
  //! indicated by Index
  //! raises an exception if Index <= 0 or Index > NbViews().
  Standard_EXPORT Standard_Real OrientationAngle (const Standard_Integer Index) const;
  
  //! returns the number of Annotation entities in <me>
  Standard_EXPORT Standard_Integer NbAnnotations() const;
  
  //! returns the Annotation entity in this Drawing, indicated by Index
  //! raises an exception if Index <= 0 or Index > NbAnnotations().
  Standard_EXPORT Handle(IGESData_IGESEntity) Annotation (const Standard_Integer Index) const;
  
  Standard_EXPORT gp_XY ViewToDrawing (const Standard_Integer NumView, const gp_XYZ& ViewCoords) const;
  
  //! Returns the Drawing Unit Value if it is specified (by a
  //! specific property entity)
  //! If not specified, returns False, and val as zero :
  //! unit to consider is then the model unit in GlobalSection
  Standard_EXPORT Standard_Boolean DrawingUnit (Standard_Real& value) const;
  
  //! Returns the Drawing Size if it is specified (by a
  //! specific property entity)
  //! If not specified, returns False, and X,Y as zero :
  //! unit to consider is then the model unit in GlobalSection
  Standard_EXPORT Standard_Boolean DrawingSize (Standard_Real& X, Standard_Real& Y) const;




  DEFINE_STANDARD_RTTIEXT(IGESDraw_DrawingWithRotation,IGESData_IGESEntity)

protected:




private:


  Handle(IGESDraw_HArray1OfViewKindEntity) theViews;
  Handle(TColgp_HArray1OfXY) theViewOrigins;
  Handle(TColStd_HArray1OfReal) theOrientationAngles;
  Handle(IGESData_HArray1OfIGESEntity) theAnnotations;


};







#endif // _IGESDraw_DrawingWithRotation_HeaderFile
