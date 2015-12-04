// Created on: 1993-09-08
// Created by: Christian CAILLET
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

#ifndef _IGESData_DefaultSpecific_HeaderFile
#define _IGESData_DefaultSpecific_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESData_SpecificModule.hxx>
#include <Standard_Integer.hxx>
class IGESData_IGESEntity;
class IGESData_IGESDumper;
class Message_Messenger;


class IGESData_DefaultSpecific;
DEFINE_STANDARD_HANDLE(IGESData_DefaultSpecific, IGESData_SpecificModule)

//! Specific IGES Services for UndefinedEntity, FreeFormatEntity
class IGESData_DefaultSpecific : public IGESData_SpecificModule
{

public:

  
  //! Creates a DefaultSpecific and puts it into SpecificLib
  Standard_EXPORT IGESData_DefaultSpecific();
  
  //! Specific Dump for UndefinedEntity : it concerns only
  //! own parameters, the general data (Directory Part, Lists) are
  //! taken into account by the IGESDumper
  Standard_EXPORT void OwnDump (const Standard_Integer CN, const Handle(IGESData_IGESEntity)& ent, const IGESData_IGESDumper& dumper, const Handle(Message_Messenger)& S, const Standard_Integer own) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IGESData_DefaultSpecific,IGESData_SpecificModule)

protected:




private:




};







#endif // _IGESData_DefaultSpecific_HeaderFile
