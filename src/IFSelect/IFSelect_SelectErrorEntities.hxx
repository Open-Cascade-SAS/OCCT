// Created on: 1992-11-18
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _IFSelect_SelectErrorEntities_HeaderFile
#define _IFSelect_SelectErrorEntities_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_SelectExtract.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class Interface_InterfaceModel;
class TCollection_AsciiString;


class IFSelect_SelectErrorEntities;
DEFINE_STANDARD_HANDLE(IFSelect_SelectErrorEntities, IFSelect_SelectExtract)

//! A SelectErrorEntities sorts the Entities which are qualified
//! as "Error" (their Type has not been recognized) during reading
//! a File. This does not concern Entities which are syntactically
//! correct, but with incorrect data (for integrity constraints).
class IFSelect_SelectErrorEntities : public IFSelect_SelectExtract
{

public:

  
  //! Creates a SelectErrorEntities
  Standard_EXPORT IFSelect_SelectErrorEntities();
  
  //! Returns True for an Entity which is qualified as "Error", i.e.
  //! if <model> explicitly knows <ent> (through its Number) as
  //! Erroneous
  Standard_EXPORT Standard_Boolean Sort (const Standard_Integer rank, const Handle(Standard_Transient)& ent, const Handle(Interface_InterfaceModel)& model) const Standard_OVERRIDE;
  
  //! Returns a text defining the criterium : "Error Entities"
  Standard_EXPORT TCollection_AsciiString ExtractLabel() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IFSelect_SelectErrorEntities,IFSelect_SelectExtract)

protected:




private:




};







#endif // _IFSelect_SelectErrorEntities_HeaderFile
