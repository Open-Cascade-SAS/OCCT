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

#ifndef _IGESData_ReadWriteModule_HeaderFile
#define _IGESData_ReadWriteModule_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Interface_ReaderModule.hxx>
#include <Standard_Integer.hxx>
class Standard_DomainError;
class Interface_FileReaderData;
class Interface_Check;
class Standard_Transient;
class IGESData_IGESEntity;
class IGESData_IGESReaderData;
class IGESData_ParamReader;
class IGESData_IGESWriter;


class IGESData_ReadWriteModule;
DEFINE_STANDARD_HANDLE(IGESData_ReadWriteModule, Interface_ReaderModule)

//! Defines basic File Access Module, under the control of
//! IGESReaderTool for Reading and IGESWriter for Writing :
//! Specific actions concern : Read and Write Own Parameters of
//! an IGESEntity.
//! The common parts (Directory Entry, Lists of Associativities
//! and Properties) are processed by IGESReaderTool & IGESWriter
//!
//! Each sub-class of ReadWriteModule is used in conjunction with
//! a sub-class of Protocol from IGESData and processes several
//! types of IGESEntity (typically, them of a package) :
//! The Protocol gives a unique positive integer Case Number for
//! each type of IGESEntity it recognizes, the corresponding
//! ReadWriteModule processes an Entity by using the Case Number
//! to known what is to do
//! On Reading, the general service NewVoid is used to create an
//! IGES Entity the first time
//!
//! Warning : Works with an IGESReaderData which stores "DE parts" of Items
class IGESData_ReadWriteModule : public Interface_ReaderModule
{

public:

  
  //! Translates the Type of record <num> in <data> to a positive
  //! Case Number, or 0 if failed.
  //! Works with IGESReaderData which provides Type & Form Numbers,
  //! and calls CaseIGES (see below)
  Standard_EXPORT Standard_Integer CaseNum (const Handle(Interface_FileReaderData)& data, const Standard_Integer num) const Standard_OVERRIDE;
  
  //! Defines Case Numbers corresponding to the Entity Types taken
  //! into account by a sub-class of ReadWriteModule (hence, each
  //! sub-class of ReadWriteModule has to redefine this method)
  //! Called by CaseNum. Its result will then be used to call
  //! Read, etc ...
  Standard_EXPORT virtual Standard_Integer CaseIGES (const Standard_Integer typenum, const Standard_Integer formnum) const = 0;
  
  //! General Read Function. See IGESReaderTool for more info
  Standard_EXPORT void Read (const Standard_Integer CN, const Handle(Interface_FileReaderData)& data, const Standard_Integer num, Handle(Interface_Check)& ach, const Handle(Standard_Transient)& ent) const Standard_OVERRIDE;
  
  //! Reads own parameters from file for an Entity; <PR> gives
  //! access to them, <IR> detains parameter types and values
  //! For each class, there must be a specific action provided
  //! Note that Properties and Associativities Lists are Read by
  //! specific methods (see below), they are called under control
  //! of reading process (only one call) according Stage recorded
  //! in ParamReader
  Standard_EXPORT virtual void ReadOwnParams (const Standard_Integer CN, const Handle(IGESData_IGESEntity)& ent, const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const = 0;
  
  //! Writes own parameters to IGESWriter; defined for each class
  //! (to be redefined for other IGES ReadWriteModules)
  //! Warning : Properties and Associativities are directly managed by
  //! WriteIGES, must not be sent by this method
  Standard_EXPORT virtual void WriteOwnParams (const Standard_Integer CN, const Handle(IGESData_IGESEntity)& ent, IGESData_IGESWriter& IW) const = 0;




  DEFINE_STANDARD_RTTIEXT(IGESData_ReadWriteModule,Interface_ReaderModule)

protected:




private:




};







#endif // _IGESData_ReadWriteModule_HeaderFile
