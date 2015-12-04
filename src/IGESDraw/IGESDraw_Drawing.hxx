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

#ifndef _IGESDraw_Drawing_HeaderFile
#define _IGESDraw_Drawing_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESDraw_HArray1OfViewKindEntity.hxx>
#include <TColgp_HArray1OfXY.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class Standard_DimensionMismatch;
class Standard_OutOfRange;
class IGESData_ViewKindEntity;
class gp_Pnt2d;
class IGESData_IGESEntity;
class gp_XY;
class gp_XYZ;


class IGESDraw_Drawing;
DEFINE_STANDARD_HANDLE(IGESDraw_Drawing, IGESData_IGESEntity)

//! defines IGESDrawing, Type <404> Form <0>
//! in package IGESDraw
//!
//! Specifies a drawing as a collection of annotation entities
//! defined in drawing space, and views which together
//! constitute a single representation of a part
class IGESDraw_Drawing : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESDraw_Drawing();
  
  //! This method is used to set the fields of the class
  //! Drawing
  //! - allViews       : Pointers to DEs of View entities
  //! - allViewOrigins : Origin coordinates of transformed Views
  //! - allAnnotations : Pointers to DEs of Annotation entities
  //! raises exception if Lengths of allViews and allViewOrigins are
  //! not same.
  Standard_EXPORT void Init (const Handle(IGESDraw_HArray1OfViewKindEntity)& allViews, const Handle(TColgp_HArray1OfXY)& allViewOrigins, const Handle(IGESData_HArray1OfIGESEntity)& allAnnotations);
  
  //! returns the number of view pointers in <me>
  Standard_EXPORT Standard_Integer NbViews() const;
  
  //! returns the ViewKindEntity indicated by ViewIndex
  //! raises an exception if ViewIndex <= 0 or ViewIndex > NbViews().
  Standard_EXPORT Handle(IGESData_ViewKindEntity) ViewItem (const Standard_Integer ViewIndex) const;
  
  //! returns the Drawing space coordinates of the origin of the
  //! Transformed view indicated by TViewIndex
  //! raises an exception if TViewIndex <= 0 or TViewIndex > NbViews().
  Standard_EXPORT gp_Pnt2d ViewOrigin (const Standard_Integer TViewIndex) const;
  
  //! returns the number of Annotation entities in <me>
  Standard_EXPORT Standard_Integer NbAnnotations() const;
  
  //! returns the Annotation entity in this Drawing, indicated by the
  //! AnnotationIndex
  //! raises an exception if AnnotationIndex <= 0 or
  //! AnnotationIndex > NbAnnotations().
  Standard_EXPORT Handle(IGESData_IGESEntity) Annotation (const Standard_Integer AnnotationIndex) const;
  
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




  DEFINE_STANDARD_RTTIEXT(IGESDraw_Drawing,IGESData_IGESEntity)

protected:




private:


  Handle(IGESDraw_HArray1OfViewKindEntity) theViews;
  Handle(TColgp_HArray1OfXY) theViewOrigins;
  Handle(IGESData_HArray1OfIGESEntity) theAnnotations;


};







#endif // _IGESDraw_Drawing_HeaderFile
