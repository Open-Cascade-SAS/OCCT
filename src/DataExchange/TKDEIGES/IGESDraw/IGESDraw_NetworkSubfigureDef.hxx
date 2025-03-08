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

#ifndef _IGESDraw_NetworkSubfigureDef_HeaderFile
#define _IGESDraw_NetworkSubfigureDef_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESDraw_HArray1OfConnectPoint.hxx>
#include <IGESData_IGESEntity.hxx>
class TCollection_HAsciiString;
class IGESGraph_TextDisplayTemplate;
class IGESDraw_ConnectPoint;

class IGESDraw_NetworkSubfigureDef;
DEFINE_STANDARD_HANDLE(IGESDraw_NetworkSubfigureDef, IGESData_IGESEntity)

//! defines IGESNetworkSubfigureDef,
//! Type <320> Form Number <0> in package IGESDraw
//!
//! This class differs from the ordinary subfigure definition
//! in that it defines a specialized subfigure, one whose
//! instances may participate in networks.
//!
//! The Number of associated(child) Connect Point Entities
//! in the Network Subfigure Instance must match the number
//! in the Network Subfigure Definition, their order must
//! be identical, and any unused points of connection in
//! the instance must be indicated by a null(zero) pointer.
class IGESDraw_NetworkSubfigureDef : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESDraw_NetworkSubfigureDef();

  //! This method is used to set fields of the class
  //! NetworkSubfigureDef
  //! - aDepth           : Depth of Subfigure
  //! (indicating the amount of nesting)
  //! - aName            : Subfigure Name
  //! - allEntities      : Associated subfigures Entities exclusive
  //! of primary reference designator and
  //! Control Points.
  //! - aTypeFlag        : Type flag determines which Entity
  //! belongs in which design
  //! (Logical design or Physical design)
  //! - aDesignator      : Designator HAsciiString and its Template
  //! - allPointEntities : Associated Connect Point Entities
  Standard_EXPORT void Init(const Standard_Integer                        aDepth,
                            const Handle(TCollection_HAsciiString)&       aName,
                            const Handle(IGESData_HArray1OfIGESEntity)&   allEntities,
                            const Standard_Integer                        aTypeFlag,
                            const Handle(TCollection_HAsciiString)&       aDesignator,
                            const Handle(IGESGraph_TextDisplayTemplate)&  aTemplate,
                            const Handle(IGESDraw_HArray1OfConnectPoint)& allPointEntities);

  //! returns Depth of Subfigure(indication the amount of nesting)
  //! Note : The Depth is inclusive of both Network Subfigure Definition
  //! Entity and the Ordinary Subfigure Definition Entity.
  //! Thus, the two may be nested.
  Standard_EXPORT Standard_Integer Depth() const;

  //! returns the Subfigure Name
  Standard_EXPORT Handle(TCollection_HAsciiString) Name() const;

  //! returns Number of Associated(child) entries in subfigure exclusive
  //! of primary reference designator and Control Points
  Standard_EXPORT Standard_Integer NbEntities() const;

  //! returns the Index'th IGESEntity in subfigure exclusive of primary
  //! reference designator and Control Points
  //! raises exception if Index  <=0 or Index > NbEntities()
  Standard_EXPORT Handle(IGESData_IGESEntity) Entity(const Standard_Integer Index) const;

  //! return value = 0 : Not Specified
  //! = 1 : Logical  design
  //! = 2 : Physical design
  Standard_EXPORT Standard_Integer TypeFlag() const;

  //! returns Primary Reference Designator
  Standard_EXPORT Handle(TCollection_HAsciiString) Designator() const;

  //! returns True if Text Display Template is specified for
  //! primary designator else returns False
  Standard_EXPORT Standard_Boolean HasDesignatorTemplate() const;

  //! if Text Display Template specified then return TextDisplayTemplate
  //! else return NULL Handle
  Standard_EXPORT Handle(IGESGraph_TextDisplayTemplate) DesignatorTemplate() const;

  //! returns the Number Of Associated(child) Connect Point Entities
  Standard_EXPORT Standard_Integer NbPointEntities() const;

  //! returns True is Index'th Associated Connect Point Entity is present
  //! else returns False
  //! raises exception if Index is out of bound
  Standard_EXPORT Standard_Boolean HasPointEntity(const Standard_Integer Index) const;

  //! returns the Index'th Associated Connect Point Entity
  //! raises exception if Index <= 0 or Index > NbPointEntities()
  Standard_EXPORT Handle(IGESDraw_ConnectPoint) PointEntity(const Standard_Integer Index) const;

  DEFINE_STANDARD_RTTIEXT(IGESDraw_NetworkSubfigureDef, IGESData_IGESEntity)

protected:
private:
  Standard_Integer                       theDepth;
  Handle(TCollection_HAsciiString)       theName;
  Handle(IGESData_HArray1OfIGESEntity)   theEntities;
  Standard_Integer                       theTypeFlag;
  Handle(TCollection_HAsciiString)       theDesignator;
  Handle(IGESGraph_TextDisplayTemplate)  theDesignatorTemplate;
  Handle(IGESDraw_HArray1OfConnectPoint) thePointEntities;
};

#endif // _IGESDraw_NetworkSubfigureDef_HeaderFile
