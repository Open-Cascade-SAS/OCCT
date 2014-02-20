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

//  Include a utiliser pour appeler IGESFile_Read


#ifndef IGESFile_Read_HeaderFile
#define IGESFile_Read_HeaderFile

#include <IGESData_IGESModel.hxx>
#include <IGESData_Protocol.hxx>
#include <IGESData_FileRecognizer.hxx>


Standard_EXPORT Standard_Integer IGESFile_Read
  (char* nomfic,
   const Handle(IGESData_IGESModel)& amodel,
   const Handle(IGESData_Protocol)& protocol);

Standard_EXPORT Standard_Integer IGESFile_ReadFNES
  (char* nomfic,
   const Handle(IGESData_IGESModel)& amodel,
   const Handle(IGESData_Protocol)& protocol);

Standard_EXPORT Standard_Integer IGESFile_Read
  (char* nomfic,
   const Handle(IGESData_IGESModel)& amodel,
   const Handle(IGESData_Protocol)& protocol,
   const Handle(IGESData_FileRecognizer)& reco,
   const Standard_Boolean modefnes = Standard_False);

#endif
