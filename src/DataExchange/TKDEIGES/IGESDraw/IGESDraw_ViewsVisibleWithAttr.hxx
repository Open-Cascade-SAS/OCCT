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

#include <IGESData_ViewKindEntity.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESData_LineFontEntity.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESGraph_Color.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESData_IGESEntity.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESData_ViewKindEntity.hxx>
#include <Standard_Integer.hxx>
class IGESData_LineFontEntity;
class IGESGraph_Color;
class IGESData_IGESEntity;

//! defines IGESViewsVisibleWithAttr, Type <402>, Form <4>
//! in package IGESDraw
//!
//! This class is extension of Class ViewsVisible. It is used
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
  Standard_EXPORT void Init(const occ::handle<NCollection_HArray1<occ::handle<IGESData_ViewKindEntity>>>&  allViewEntities,
                            const occ::handle<NCollection_HArray1<int>>&          allLineFonts,
                            const occ::handle<NCollection_HArray1<occ::handle<IGESData_LineFontEntity>>>& allLineDefinitions,
                            const occ::handle<NCollection_HArray1<int>>&          allColorValues,
                            const occ::handle<NCollection_HArray1<occ::handle<IGESGraph_Color>>>&          allColorDefinitions,
                            const occ::handle<NCollection_HArray1<int>>&          allLineWeights,
                            const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>&      allDisplayEntities);

  //! Changes only the list of Displayed Entities (Null allowed)
  Standard_EXPORT void InitImplied(const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>& allDisplayEntity);

  //! Returns False (for a complex view)
  Standard_EXPORT bool IsSingle() const override;

  //! returns the number of Views containing the view visible, line font,
  //! color number, and line weight information
  Standard_EXPORT int NbViews() const override;

  //! returns the number of entities which have this particular set of
  //! display characteristic, or zero if no Entities specified
  Standard_EXPORT int NbDisplayedEntities() const;

  //! returns the Index'th ViewKindEntity entity
  //! raises exception if Index <= 0 or Index > NbViews()
  Standard_EXPORT occ::handle<IGESData_ViewKindEntity> ViewItem(const int Index) const
    override;

  //! returns the Index'th Line font value or zero
  //! raises exception if Index <= 0 or Index > NbViews()
  Standard_EXPORT int LineFontValue(const int Index) const;

  //! returns True if the Index'th Line Font Definition is specified
  //! else returns False
  //! raises exception if Index <= 0 or Index > NbViews()
  Standard_EXPORT bool IsFontDefinition(const int Index) const;

  //! returns the Index'th Line Font Definition Entity or NULL(0)
  //! raises exception if Index <= 0 or Index > NbViews()
  Standard_EXPORT occ::handle<IGESData_LineFontEntity> FontDefinition(
    const int Index) const;

  //! returns the Index'th Color number value
  //! raises exception if Index <= 0 or Index > NbViews()
  Standard_EXPORT int ColorValue(const int Index) const;

  //! returns True if Index'th Color Definition is specified
  //! else returns False
  //! raises exception if Index <= 0 or Index > NbViews()
  Standard_EXPORT bool IsColorDefinition(const int Index) const;

  //! returns the Index'th Color Definition Entity
  //! raises exception if Index <= 0 or Index > NbViews()
  Standard_EXPORT occ::handle<IGESGraph_Color> ColorDefinition(const int Index) const;

  //! returns the Index'th Color Line Weight
  //! raises exception if Index <= 0 or Index > NbViews()
  Standard_EXPORT int LineWeightItem(const int Index) const;

  //! returns Index'th Display entity with this particular characteristics
  //! raises exception if Index <= 0 or Index > NbEntities()
  Standard_EXPORT occ::handle<IGESData_IGESEntity> DisplayedEntity(const int Index) const;

  DEFINE_STANDARD_RTTIEXT(IGESDraw_ViewsVisibleWithAttr, IGESData_ViewKindEntity)

private:
  occ::handle<NCollection_HArray1<occ::handle<IGESData_ViewKindEntity>>>  theViewEntities;
  occ::handle<NCollection_HArray1<int>>          theLineFonts;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_LineFontEntity>>> theLineDefinitions;
  occ::handle<NCollection_HArray1<int>>          theColorValues;
  occ::handle<NCollection_HArray1<occ::handle<IGESGraph_Color>>>          theColorDefinitions;
  occ::handle<NCollection_HArray1<int>>          theLineWeights;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>      theDisplayEntities;
};

#endif // _IGESDraw_ViewsVisibleWithAttr_HeaderFile
