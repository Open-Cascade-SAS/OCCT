// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen ( Niraj RANGWALA )
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

#ifndef _IGESDraw_ViewsVisibleWithAttr_HeaderFile
#define _IGESDraw_ViewsVisibleWithAttr_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESDraw_HArray1OfViewKindEntity.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <IGESBasic_HArray1OfLineFontEntity.hxx>
#include <IGESGraph_HArray1OfColor.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_ViewKindEntity.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class Standard_DimensionMismatch;
class Standard_OutOfRange;
class IGESData_ViewKindEntity;
class IGESData_LineFontEntity;
class IGESGraph_Color;
class IGESData_IGESEntity;


class IGESDraw_ViewsVisibleWithAttr;
DEFINE_STANDARD_HANDLE(IGESDraw_ViewsVisibleWithAttr, IGESData_ViewKindEntity)

//! defines IGESViewsVisibleWithAttr, Type <402>, Form <4>
//! in package IGESDraw
//!
//! This class is extension of Class ViewsVisible.  It is used
//! for those entities that are visible in multiple views, but
//! must have a different line font, color number, or
//! line weight in each view.
class IGESDraw_ViewsVisibleWithAttr : public IGESData_ViewKindEntity
{

public:

  
  Standard_EXPORT IGESDraw_ViewsVisibleWithAttr();
  
  //! This method is used to set fields of the class
  //! ViewsVisibleWithAttr
  //! - allViewEntities     : All View kind entities
  //! - allLineFonts        : All Line Font values or zero(0)
  //! - allLineDefinitions  : Line Font Definition
  //! (if Line Font value = 0)
  //! - allColorValues      : All Color values
  //! - allColorDefinitions : All Color Definition Entities
  //! - allLineWeights      : All Line Weight values
  //! - allDisplayEntities  : Entities which are member of
  //! this associativity
  //! raises exception if Lengths of allViewEntities, allLineFonts,
  //! allColorValues,allColorDefinitions, allLineWeights are not same
  Standard_EXPORT void Init (const Handle(IGESDraw_HArray1OfViewKindEntity)& allViewEntities, const Handle(TColStd_HArray1OfInteger)& allLineFonts, const Handle(IGESBasic_HArray1OfLineFontEntity)& allLineDefinitions, const Handle(TColStd_HArray1OfInteger)& allColorValues, const Handle(IGESGraph_HArray1OfColor)& allColorDefinitions, const Handle(TColStd_HArray1OfInteger)& allLineWeights, const Handle(IGESData_HArray1OfIGESEntity)& allDisplayEntities);
  
  //! Changes only the list of Displayed Entities (Null allowed)
  Standard_EXPORT void InitImplied (const Handle(IGESData_HArray1OfIGESEntity)& allDisplayEntity);
  
  //! Returns False (for a complex view)
  Standard_EXPORT Standard_Boolean IsSingle() const Standard_OVERRIDE;
  
  //! returns the number of Views containing the view visible, line font,
  //! color number, and line weight information
  Standard_EXPORT Standard_Integer NbViews() const Standard_OVERRIDE;
  
  //! returns the number of entities which have this particular set of
  //! display characteristic, or zero if no Entities specified
  Standard_EXPORT Standard_Integer NbDisplayedEntities() const;
  
  //! returns the Index'th ViewKindEntity entity
  //! raises exception if Index <= 0 or Index > NbViews()
  Standard_EXPORT Handle(IGESData_ViewKindEntity) ViewItem (const Standard_Integer Index) const Standard_OVERRIDE;
  
  //! returns the Index'th Line font value or zero
  //! raises exception if Index <= 0 or Index > NbViews()
  Standard_EXPORT Standard_Integer LineFontValue (const Standard_Integer Index) const;
  
  //! returns True if the Index'th Line Font Definition is specified
  //! else returns False
  //! raises exception if Index <= 0 or Index > NbViews()
  Standard_EXPORT Standard_Boolean IsFontDefinition (const Standard_Integer Index) const;
  
  //! returns the Index'th Line Font Definition Entity or NULL(0)
  //! raises exception if Index <= 0 or Index > NbViews()
  Standard_EXPORT Handle(IGESData_LineFontEntity) FontDefinition (const Standard_Integer Index) const;
  
  //! returns the Index'th Color number value
  //! raises exception if Index <= 0 or Index > NbViews()
  Standard_EXPORT Standard_Integer ColorValue (const Standard_Integer Index) const;
  
  //! returns True if Index'th Color Definition is specified
  //! else returns False
  //! raises exception if Index <= 0 or Index > NbViews()
  Standard_EXPORT Standard_Boolean IsColorDefinition (const Standard_Integer Index) const;
  
  //! returns the Index'th Color Definition Entity
  //! raises exception if Index <= 0 or Index > NbViews()
  Standard_EXPORT Handle(IGESGraph_Color) ColorDefinition (const Standard_Integer Index) const;
  
  //! returns the Index'th Color Line Weight
  //! raises exception if Index <= 0 or Index > NbViews()
  Standard_EXPORT Standard_Integer LineWeightItem (const Standard_Integer Index) const;
  
  //! returns Index'th Display entity with this particular characteristics
  //! raises exception if Index  <= 0 or Index > NbEntities()
  Standard_EXPORT Handle(IGESData_IGESEntity) DisplayedEntity (const Standard_Integer Index) const;




  DEFINE_STANDARD_RTTIEXT(IGESDraw_ViewsVisibleWithAttr,IGESData_ViewKindEntity)

protected:




private:


  Handle(IGESDraw_HArray1OfViewKindEntity) theViewEntities;
  Handle(TColStd_HArray1OfInteger) theLineFonts;
  Handle(IGESBasic_HArray1OfLineFontEntity) theLineDefinitions;
  Handle(TColStd_HArray1OfInteger) theColorValues;
  Handle(IGESGraph_HArray1OfColor) theColorDefinitions;
  Handle(TColStd_HArray1OfInteger) theLineWeights;
  Handle(IGESData_HArray1OfIGESEntity) theDisplayEntities;


};







#endif // _IGESDraw_ViewsVisibleWithAttr_HeaderFile
