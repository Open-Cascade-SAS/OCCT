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
#include <IGESData_IGESEntity.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESDraw_ConnectPoint.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESData_IGESEntity.hxx>
class TCollection_HAsciiString;
class IGESGraph_TextDisplayTemplate;
class IGESDraw_ConnectPoint;

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
  Standard_EXPORT void Init(const int                        aDepth,
                            const occ::handle<TCollection_HAsciiString>&       aName,
                            const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>&   allEntities,
                            const int                        aTypeFlag,
                            const occ::handle<TCollection_HAsciiString>&       aDesignator,
                            const occ::handle<IGESGraph_TextDisplayTemplate>&  aTemplate,
                            const occ::handle<NCollection_HArray1<occ::handle<IGESDraw_ConnectPoint>>>& allPointEntities);

  //! returns Depth of Subfigure(indication the amount of nesting)
  //! Note : The Depth is inclusive of both Network Subfigure Definition
  //! Entity and the Ordinary Subfigure Definition Entity.
  //! Thus, the two may be nested.
  Standard_EXPORT int Depth() const;

  //! returns the Subfigure Name
  Standard_EXPORT occ::handle<TCollection_HAsciiString> Name() const;

  //! returns Number of Associated(child) entries in subfigure exclusive
  //! of primary reference designator and Control Points
  Standard_EXPORT int NbEntities() const;

  //! returns the Index'th IGESEntity in subfigure exclusive of primary
  //! reference designator and Control Points
  //! raises exception if Index <=0 or Index > NbEntities()
  Standard_EXPORT occ::handle<IGESData_IGESEntity> Entity(const int Index) const;

  //! return value = 0 : Not Specified
  //! = 1 : Logical  design
  //! = 2 : Physical design
  Standard_EXPORT int TypeFlag() const;

  //! returns Primary Reference Designator
  Standard_EXPORT occ::handle<TCollection_HAsciiString> Designator() const;

  //! returns True if Text Display Template is specified for
  //! primary designator else returns False
  Standard_EXPORT bool HasDesignatorTemplate() const;

  //! if Text Display Template specified then return TextDisplayTemplate
  //! else return NULL Handle
  Standard_EXPORT occ::handle<IGESGraph_TextDisplayTemplate> DesignatorTemplate() const;

  //! returns the Number Of Associated(child) Connect Point Entities
  Standard_EXPORT int NbPointEntities() const;

  //! returns True is Index'th Associated Connect Point Entity is present
  //! else returns False
  //! raises exception if Index is out of bound
  Standard_EXPORT bool HasPointEntity(const int Index) const;

  //! returns the Index'th Associated Connect Point Entity
  //! raises exception if Index <= 0 or Index > NbPointEntities()
  Standard_EXPORT occ::handle<IGESDraw_ConnectPoint> PointEntity(const int Index) const;

  DEFINE_STANDARD_RTTIEXT(IGESDraw_NetworkSubfigureDef, IGESData_IGESEntity)

private:
  int                       theDepth;
  occ::handle<TCollection_HAsciiString>       theName;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>   theEntities;
  int                       theTypeFlag;
  occ::handle<TCollection_HAsciiString>       theDesignator;
  occ::handle<IGESGraph_TextDisplayTemplate>  theDesignatorTemplate;
  occ::handle<NCollection_HArray1<occ::handle<IGESDraw_ConnectPoint>>> thePointEntities;
};

#endif // _IGESDraw_NetworkSubfigureDef_HeaderFile
