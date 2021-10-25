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
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESDraw_HArray1OfConnectPoint.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>
#include <IGESGraph_HArray1OfTextDisplayTemplate.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Boolean.hxx>
class IGESDraw_ConnectPoint;
class TCollection_HAsciiString;
class IGESGraph_TextDisplayTemplate;


class IGESAppli_PipingFlow;
DEFINE_STANDARD_HANDLE(IGESAppli_PipingFlow, IGESData_IGESEntity)

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
  Standard_EXPORT void Init (const Standard_Integer nbContextFlags, const Standard_Integer aFlowType, const Handle(IGESData_HArray1OfIGESEntity)& allFlowAssocs, const Handle(IGESDraw_HArray1OfConnectPoint)& allConnectPoints, const Handle(IGESData_HArray1OfIGESEntity)& allJoins, const Handle(Interface_HArray1OfHAsciiString)& allFlowNames, const Handle(IGESGraph_HArray1OfTextDisplayTemplate)& allTextDisps, const Handle(IGESData_HArray1OfIGESEntity)& allContFlowAssocs);
  
  //! forces NbContextFalgs to 1, returns True if changed
  Standard_EXPORT Standard_Boolean OwnCorrect();
  
  //! returns number of Count of Context Flags, always = 1
  Standard_EXPORT Standard_Integer NbContextFlags() const;
  
  //! returns number of Piping Flow Associativity Entities
  Standard_EXPORT Standard_Integer NbFlowAssociativities() const;
  
  //! returns number of Connect Point Entities
  Standard_EXPORT Standard_Integer NbConnectPoints() const;
  
  //! returns number of Join Entities
  Standard_EXPORT Standard_Integer NbJoins() const;
  
  //! returns number of Flow Names
  Standard_EXPORT Standard_Integer NbFlowNames() const;
  
  //! returns number of Text Display Template Entities
  Standard_EXPORT Standard_Integer NbTextDisplayTemplates() const;
  
  //! returns number of Continuation Piping Flow Associativities
  Standard_EXPORT Standard_Integer NbContFlowAssociativities() const;
  
  //! returns Type of Flow = 0 : Not specified,
  //! 1 : Logical,
  //! 2 : Physical
  Standard_EXPORT Standard_Integer TypeOfFlow() const;
  
  //! returns Piping Flow Associativity Entity
  //! raises exception if Index <= 0 or Index > NbFlowAssociativities()
  Standard_EXPORT Handle(IGESData_IGESEntity) FlowAssociativity (const Standard_Integer Index) const;
  
  //! returns Connect Point Entity
  //! raises exception if Index <= 0 or Index > NbConnectPoints()
  Standard_EXPORT Handle(IGESDraw_ConnectPoint) ConnectPoint (const Standard_Integer Index) const;
  
  //! returns Join Entity
  //! raises exception if Index <= 0 or Index > NbJoins()
  Standard_EXPORT Handle(IGESData_IGESEntity) Join (const Standard_Integer Index) const;
  
  //! returns Flow Name
  //! raises exception if Index <= 0 or Index > NbFlowNames()
  Standard_EXPORT Handle(TCollection_HAsciiString) FlowName (const Standard_Integer Index) const;
  
  //! returns Text Display Template Entity
  //! raises exception if Index <= 0 or Index > NbTextDisplayTemplates()
  Standard_EXPORT Handle(IGESGraph_TextDisplayTemplate) TextDisplayTemplate (const Standard_Integer Index) const;
  
  //! returns Continuation Piping Flow Associativity Entity
  //! raises exception if Index <= 0 or Index > NbContFlowAssociativities()
  Standard_EXPORT Handle(IGESData_IGESEntity) ContFlowAssociativity (const Standard_Integer Index) const;




  DEFINE_STANDARD_RTTIEXT(IGESAppli_PipingFlow,IGESData_IGESEntity)

protected:




private:


  Standard_Integer theNbContextFlags;
  Standard_Integer theTypeOfFlow;
  Handle(IGESData_HArray1OfIGESEntity) theFlowAssociativities;
  Handle(IGESDraw_HArray1OfConnectPoint) theConnectPoints;
  Handle(IGESData_HArray1OfIGESEntity) theJoins;
  Handle(Interface_HArray1OfHAsciiString) theFlowNames;
  Handle(IGESGraph_HArray1OfTextDisplayTemplate) theTextDisplayTemplates;
  Handle(IGESData_HArray1OfIGESEntity) theContFlowAssociativities;


};







#endif // _IGESAppli_PipingFlow_HeaderFile
