// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen ( Anand NATRAJAN )
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

#ifndef _IGESAppli_PipingFlow_HeaderFile
#define _IGESAppli_PipingFlow_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <IGESData_IGESEntity.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESDraw_ConnectPoint.hxx>
#include <TCollection_HAsciiString.hxx>
#include <IGESGraph_TextDisplayTemplate.hxx>
class IGESDraw_ConnectPoint;
class TCollection_HAsciiString;
class IGESGraph_TextDisplayTemplate;

//! defines PipingFlow, Type <402> Form <20>
//! in package IGESAppli
//! Represents a single fluid flow path
class IGESAppli_PipingFlow : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESAppli_PipingFlow();

  //! This method is used to set the fields of the class
  //! PipingFlow
  //! - nbContextFlags    : Count of Context Flags, always = 1
  //! - aFlowType         : Type of Flow, default = 0
  //! - allFlowAssocs     : PipingFlow Associativity Entities
  //! - allConnectPoints  : Connect Point Entities
  //! - allJoins          : Join Entities
  //! - allFlowNames      : PipingFlow Names
  //! - allTextDispTs     : Text Display Template Entities
  //! - allContFlowAssocs : Continuation Flow Associativity Entities
  Standard_EXPORT void Init(
    const int                                                                      nbContextFlags,
    const int                                                                      aFlowType,
    const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>&      allFlowAssocs,
    const occ::handle<NCollection_HArray1<occ::handle<IGESDraw_ConnectPoint>>>&    allConnectPoints,
    const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>&      allJoins,
    const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& allFlowNames,
    const occ::handle<NCollection_HArray1<occ::handle<IGESGraph_TextDisplayTemplate>>>&
                                                                              allTextDisps,
    const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>& allContFlowAssocs);

  //! forces NbContextFalgs to 1, returns True if changed
  Standard_EXPORT bool OwnCorrect();

  //! returns number of Count of Context Flags, always = 1
  Standard_EXPORT int NbContextFlags() const;

  //! returns number of Piping Flow Associativity Entities
  Standard_EXPORT int NbFlowAssociativities() const;

  //! returns number of Connect Point Entities
  Standard_EXPORT int NbConnectPoints() const;

  //! returns number of Join Entities
  Standard_EXPORT int NbJoins() const;

  //! returns number of Flow Names
  Standard_EXPORT int NbFlowNames() const;

  //! returns number of Text Display Template Entities
  Standard_EXPORT int NbTextDisplayTemplates() const;

  //! returns number of Continuation Piping Flow Associativities
  Standard_EXPORT int NbContFlowAssociativities() const;

  //! returns Type of Flow = 0 : Not specified,
  //! 1 : Logical,
  //! 2 : Physical
  Standard_EXPORT int TypeOfFlow() const;

  //! returns Piping Flow Associativity Entity
  //! raises exception if Index <= 0 or Index > NbFlowAssociativities()
  Standard_EXPORT occ::handle<IGESData_IGESEntity> FlowAssociativity(const int Index) const;

  //! returns Connect Point Entity
  //! raises exception if Index <= 0 or Index > NbConnectPoints()
  Standard_EXPORT occ::handle<IGESDraw_ConnectPoint> ConnectPoint(const int Index) const;

  //! returns Join Entity
  //! raises exception if Index <= 0 or Index > NbJoins()
  Standard_EXPORT occ::handle<IGESData_IGESEntity> Join(const int Index) const;

  //! returns Flow Name
  //! raises exception if Index <= 0 or Index > NbFlowNames()
  Standard_EXPORT occ::handle<TCollection_HAsciiString> FlowName(const int Index) const;

  //! returns Text Display Template Entity
  //! raises exception if Index <= 0 or Index > NbTextDisplayTemplates()
  Standard_EXPORT occ::handle<IGESGraph_TextDisplayTemplate> TextDisplayTemplate(
    const int Index) const;

  //! returns Continuation Piping Flow Associativity Entity
  //! raises exception if Index <= 0 or Index > NbContFlowAssociativities()
  Standard_EXPORT occ::handle<IGESData_IGESEntity> ContFlowAssociativity(const int Index) const;

  DEFINE_STANDARD_RTTIEXT(IGESAppli_PipingFlow, IGESData_IGESEntity)

private:
  int                                                                     theNbContextFlags;
  int                                                                     theTypeOfFlow;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>      theFlowAssociativities;
  occ::handle<NCollection_HArray1<occ::handle<IGESDraw_ConnectPoint>>>    theConnectPoints;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>      theJoins;
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> theFlowNames;
  occ::handle<NCollection_HArray1<occ::handle<IGESGraph_TextDisplayTemplate>>>
                                                                     theTextDisplayTemplates;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> theContFlowAssociativities;
};

#endif // _IGESAppli_PipingFlow_HeaderFile
