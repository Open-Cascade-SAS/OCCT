// Created on: 1993-01-11
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

#ifndef _IGESDraw_NetworkSubfigure_HeaderFile
#define _IGESDraw_NetworkSubfigure_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_XYZ.hxx>
#include <Standard_Integer.hxx>
#include <IGESDraw_HArray1OfConnectPoint.hxx>
#include <IGESData_IGESEntity.hxx>
class IGESDraw_NetworkSubfigureDef;
class TCollection_HAsciiString;
class IGESGraph_TextDisplayTemplate;
class IGESDraw_ConnectPoint;

class IGESDraw_NetworkSubfigure;
DEFINE_STANDARD_HANDLE(IGESDraw_NetworkSubfigure, IGESData_IGESEntity)

//! defines IGES Network Subfigure Instance Entity,
//! Type <420> Form Number <0> in package IGESDraw
//!
//! Used to specify each instance of Network Subfigure
//! Definition Entity (Type 320, Form 0).
class IGESDraw_NetworkSubfigure : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESDraw_NetworkSubfigure();

  //! This method is used to set the fields of the class
  //! NetworkSubfigure
  //! - aDefinition      : Network Subfigure Definition Entity
  //! - aTranslation     : Translation data relative to the model
  //! space or the definition space
  //! - aScaleFactor     : Scale factors in the definition space
  //! - aTypeFlag        : Type flag
  //! - aDesignator      : Primary reference designator
  //! - aTemplate        : Primary reference designator Text
  //! display Template Entity
  //! - allConnectPoints : Associated Connect Point Entities
  Standard_EXPORT void Init(const Handle(IGESDraw_NetworkSubfigureDef)&   aDefinition,
                            const gp_XYZ&                                 aTranslation,
                            const gp_XYZ&                                 aScaleFactor,
                            const Standard_Integer                        aTypeFlag,
                            const Handle(TCollection_HAsciiString)&       aDesignator,
                            const Handle(IGESGraph_TextDisplayTemplate)&  aTemplate,
                            const Handle(IGESDraw_HArray1OfConnectPoint)& allConnectPoints);

  //! returns Network Subfigure Definition Entity specified by this entity
  Standard_EXPORT Handle(IGESDraw_NetworkSubfigureDef) SubfigureDefinition() const;

  //! returns Translation Data relative to either model space or to
  //! the definition space of a referring entity
  Standard_EXPORT gp_XYZ Translation() const;

  //! returns the Transformed Translation Data relative to either model
  //! space or to the definition space of a referring entity
  Standard_EXPORT gp_XYZ TransformedTranslation() const;

  //! returns Scale factor in definition space(x, y, z axes)
  Standard_EXPORT gp_XYZ ScaleFactors() const;

  //! returns Type Flag which implements the distinction between Logical
  //! design and Physical design data,and is required if both are present.
  //! Type Flag = 0 : Not specified (default)
  //! = 1 : Logical
  //! = 2 : Physical
  Standard_EXPORT Standard_Integer TypeFlag() const;

  //! returns the primary reference designator
  Standard_EXPORT Handle(TCollection_HAsciiString) ReferenceDesignator() const;

  //! returns True if Text Display Template Entity is specified,
  //! else False
  Standard_EXPORT Standard_Boolean HasDesignatorTemplate() const;

  //! returns primary reference designator Text Display Template Entity,
  //! or null. If null, no Text Display Template Entity specified
  Standard_EXPORT Handle(IGESGraph_TextDisplayTemplate) DesignatorTemplate() const;

  //! returns the number of associated Connect Point Entities
  Standard_EXPORT Standard_Integer NbConnectPoints() const;

  //! returns the Index'th  associated Connect point Entity
  //! raises exception if Index <= 0 or Index > NbConnectPoints()
  Standard_EXPORT Handle(IGESDraw_ConnectPoint) ConnectPoint(const Standard_Integer Index) const;

  DEFINE_STANDARD_RTTIEXT(IGESDraw_NetworkSubfigure, IGESData_IGESEntity)

protected:
private:
  Handle(IGESDraw_NetworkSubfigureDef)   theSubfigureDefinition;
  gp_XYZ                                 theTranslation;
  gp_XYZ                                 theScaleFactor;
  Standard_Integer                       theTypeFlag;
  Handle(TCollection_HAsciiString)       theDesignator;
  Handle(IGESGraph_TextDisplayTemplate)  theDesignatorTemplate;
  Handle(IGESDraw_HArray1OfConnectPoint) theConnectPoints;
};

#endif // _IGESDraw_NetworkSubfigure_HeaderFile
