// Created on: 1993-09-06
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

#ifndef _IGESBasic_ReadWriteModule_HeaderFile
#define _IGESBasic_ReadWriteModule_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESData_ReadWriteModule.hxx>
#include <Standard_Integer.hxx>
class IGESData_IGESEntity;
class IGESData_IGESReaderData;
class IGESData_ParamReader;
class IGESData_IGESWriter;

class IGESBasic_ReadWriteModule;
DEFINE_STANDARD_HANDLE(IGESBasic_ReadWriteModule, IGESData_ReadWriteModule)

//! Defines basic File Access Module for IGESBasic (specific parts)
//! Specific actions concern : Read and Write Own Parameters of
//! an IGESEntity.
class IGESBasic_ReadWriteModule : public IGESData_ReadWriteModule
{

public:
  //! Creates a ReadWriteModule & puts it into ReaderLib & WriterLib
  Standard_EXPORT IGESBasic_ReadWriteModule();

  //! Defines Case Numbers for Entities of IGESBasic
  Standard_EXPORT Standard_Integer CaseIGES(const Standard_Integer typenum,
                                            const Standard_Integer formnum) const Standard_OVERRIDE;

  //! Reads own parameters from file for an Entity of IGESBasic
  Standard_EXPORT void ReadOwnParams(const Standard_Integer                 CN,
                                     const Handle(IGESData_IGESEntity)&     ent,
                                     const Handle(IGESData_IGESReaderData)& IR,
                                     IGESData_ParamReader& PR) const Standard_OVERRIDE;

  //! Writes own parameters to IGESWriter
  Standard_EXPORT void WriteOwnParams(const Standard_Integer             CN,
                                      const Handle(IGESData_IGESEntity)& ent,
                                      IGESData_IGESWriter& IW) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESBasic_ReadWriteModule, IGESData_ReadWriteModule)

protected:
private:
};

#endif // _IGESBasic_ReadWriteModule_HeaderFile
