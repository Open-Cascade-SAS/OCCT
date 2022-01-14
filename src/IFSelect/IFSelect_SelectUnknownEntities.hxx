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

#ifndef _IFSelect_SelectUnknownEntities_HeaderFile
#define _IFSelect_SelectUnknownEntities_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_SelectExtract.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class Interface_InterfaceModel;
class TCollection_AsciiString;


class IFSelect_SelectUnknownEntities;
DEFINE_STANDARD_HANDLE(IFSelect_SelectUnknownEntities, IFSelect_SelectExtract)

//! A SelectUnknownEntities sorts the Entities which are qualified
//! as "Unknown" (their Type has not been recognized)
class IFSelect_SelectUnknownEntities : public IFSelect_SelectExtract
{

public:

  
  //! Creates a SelectUnknownEntities
  Standard_EXPORT IFSelect_SelectUnknownEntities();
  
  //! Returns True for an Entity which is qualified as "Unknown",
  //! i.e. if <model> known <ent> (through its Number) as Unknown
  Standard_EXPORT Standard_Boolean Sort (const Standard_Integer rank, const Handle(Standard_Transient)& ent, const Handle(Interface_InterfaceModel)& model) const Standard_OVERRIDE;
  
  //! Returns a text defining the criterium : "Recognized Entities"
  Standard_EXPORT TCollection_AsciiString ExtractLabel() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IFSelect_SelectUnknownEntities,IFSelect_SelectExtract)

protected:




private:




};







#endif // _IFSelect_SelectUnknownEntities_HeaderFile
