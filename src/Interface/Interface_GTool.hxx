// Created on: 1998-01-08
// Created by: Christian CAILLET
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _Interface_GTool_HeaderFile
#define _Interface_GTool_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Interface_GeneralLib.hxx>
#include <Interface_DataMapOfTransientInteger.hxx>
#include <TColStd_IndexedDataMapOfTransientTransient.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
class Interface_Protocol;
class Interface_SignType;
class Interface_InterfaceModel;
class Interface_GeneralModule;


class Interface_GTool;
DEFINE_STANDARD_HANDLE(Interface_GTool, Standard_Transient)

//! GTool - General Tool for a Model
//! Provides the functions performed by Protocol/GeneralModule for
//! entities of a Model, and recorded in a GeneralLib
//! Optimized : once an entity has been queried, the GeneralLib is
//! not longer queried
//! Shareable between several users : as a Handle
class Interface_GTool : public Standard_Transient
{

public:

  
  //! Creates an empty, not set, GTool
  Standard_EXPORT Interface_GTool();
  
  //! Creates a GTool from a Protocol
  //! Optional starting count of entities
  Standard_EXPORT Interface_GTool(const Handle(Interface_Protocol)& proto, const Standard_Integer nbent = 0);
  
  //! Sets a new SignType
  Standard_EXPORT void SetSignType (const Handle(Interface_SignType)& sign);
  
  //! Returns the SignType. Can be null
  Standard_EXPORT Handle(Interface_SignType) SignType() const;
  
  //! Returns the Signature for a Transient Object in a Model
  //! It calls SignType to do that
  //! If SignType is not defined, return ClassName of <ent>
  Standard_EXPORT Standard_CString SignValue (const Handle(Standard_Transient)& ent, const Handle(Interface_InterfaceModel)& model) const;
  
  //! Returns the Name of the SignType, or "Class Name"
  Standard_EXPORT Standard_CString SignName() const;
  
  //! Sets a new Protocol
  //! if <enforce> is False and the new Protocol equates the old one
  //! then nothing is done
  Standard_EXPORT void SetProtocol (const Handle(Interface_Protocol)& proto, const Standard_Boolean enforce = Standard_False);
  
  //! Returns the Protocol.  Warning : it can be Null
  Standard_EXPORT Handle(Interface_Protocol) Protocol() const;
  
  //! Returns the GeneralLib itself
  Standard_EXPORT Interface_GeneralLib& Lib();
  
  //! Reservates maps for a count of entities
  //! <enforce> False : minimum count
  //! <enforce> True  : clears former reservations
  //! Does not clear the maps
  Standard_EXPORT void Reservate (const Standard_Integer nb, const Standard_Boolean enforce = Standard_False);
  
  //! Clears the maps which record, for each already recorded entity
  //! its Module and Case Number
  Standard_EXPORT void ClearEntities();
  
  //! Selects for an entity, its Module and Case Number
  //! It is optimised : once done for each entity, the result is
  //! mapped and the GeneralLib is not longer queried
  //! <enforce> True overpasses this optimisation
  Standard_EXPORT Standard_Boolean Select (const Handle(Standard_Transient)& ent, Handle(Interface_GeneralModule)& gmod, Standard_Integer& CN, const Standard_Boolean enforce = Standard_False);




  DEFINE_STANDARD_RTTIEXT(Interface_GTool,Standard_Transient)

protected:




private:


  Handle(Interface_Protocol) theproto;
  Handle(Interface_SignType) thesign;
  Interface_GeneralLib thelib;
  Interface_DataMapOfTransientInteger thentnum;
  TColStd_IndexedDataMapOfTransientTransient thentmod;


};







#endif // _Interface_GTool_HeaderFile
