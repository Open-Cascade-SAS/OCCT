// Created on: 1997-03-04
// Created by: Robert COUBLANC
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _AIS_AttributeFilter_HeaderFile
#define _AIS_AttributeFilter_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Quantity_NameOfColor.hxx>
#include <Standard_Real.hxx>
#include <SelectMgr_Filter.hxx>
class SelectMgr_EntityOwner;


class AIS_AttributeFilter;
DEFINE_STANDARD_HANDLE(AIS_AttributeFilter, SelectMgr_Filter)

//! Selects Interactive Objects, which have the desired width or color.
//! The filter questions each Interactive Object in local
//! context to determine whether it has an non-null
//! owner, and if so, whether it has the required color
//! and width attributes. If the object returns true in each
//! case, it is kept. If not, it is rejected.
//! This filter is used only in an open local context.
//! In the Collector viewer, you can only locate
//! Interactive Objects, which answer positively to the
//! filters, which are in position when a local context is open.
class AIS_AttributeFilter : public SelectMgr_Filter
{
public:

  //! Constructs an empty attribute filter object.
  //! This filter object determines whether selectable
  //! interactive objects have a non-null owner.
  Standard_EXPORT AIS_AttributeFilter();
  

  //! Constructs an attribute filter object defined by the
  //! color attribute aCol.
  Standard_EXPORT AIS_AttributeFilter(const Quantity_NameOfColor aCol);
  

  //! Constructs an attribute filter object defined by the line
  //! width attribute aWidth.
  Standard_EXPORT AIS_AttributeFilter(const Standard_Real aWidth);
  

  //! Indicates that the Interactive Object has the color
  //! setting specified by the argument aCol at construction time.
  Standard_Boolean HasColor() const { return hasC; }

  //! Indicates that the Interactive Object has the width
  //! setting specified by the argument aWidth at
  //! construction time.
  Standard_Boolean HasWidth() const { return hasW; }

  //! Sets the color.
  void SetColor (const Quantity_NameOfColor theCol)
  {
    myCol = theCol;
    hasC = Standard_True;
  }

  //! Sets the line width.
  void SetWidth (const Standard_Real theWidth)
  {
    myWid = theWidth;
    hasW = Standard_True;
  }

  //! Removes the setting for color from the filter.
  void UnsetColor() { hasC = Standard_False; }

  //! Removes the setting for width from the filter.
  void UnsetWidth() { hasW = Standard_False; }

  //! Indicates that the selected Interactive Object passes
  //! the filter. The owner, anObj, can be either direct or
  //! user. A direct owner is the corresponding
  //! construction element, whereas a user is the
  //! compound shape of which the entity forms a part.
  //! If the Interactive Object returns Standard_True
  //! when detected by the Local Context selector through
  //! the mouse, the object is kept; if not, it is rejected.
  Standard_EXPORT virtual Standard_Boolean IsOk (const Handle(SelectMgr_EntityOwner)& anObj) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(AIS_AttributeFilter,SelectMgr_Filter)

private:

  Quantity_NameOfColor myCol;
  Standard_Real myWid;
  Standard_Boolean hasC;
  Standard_Boolean hasW;

};

#endif // _AIS_AttributeFilter_HeaderFile
