// Created on: 1993-02-04
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

#ifndef _Interface_ReaderModule_HeaderFile
#define _Interface_ReaderModule_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Interface_FileReaderData;
class Interface_Check;


class Interface_ReaderModule;
DEFINE_STANDARD_HANDLE(Interface_ReaderModule, Standard_Transient)

//! Defines unitary operations required to read an Entity from a
//! File (see FileReaderData, FileReaderTool), under control of
//! a FileReaderTool. The initial creation is performed by a
//! GeneralModule (set in GeneralLib). Then, which remains is
//! Loading data from the FileReaderData to the Entity
//!
//! To work, a GeneralModule has formerly recognized the Type read
//! from FileReaderData as a positive Case Number, then the
//! ReaderModule reads it according to this Case Number
class Interface_ReaderModule : public Standard_Transient
{

public:

  
  //! Translates the type of record <num> in <data> to a positive
  //! Case Number. If Recognition fails, must return 0
  Standard_EXPORT virtual Standard_Integer CaseNum (const Handle(Interface_FileReaderData)& data, const Standard_Integer num) const = 0;
  
  //! Performs the effective loading from <data>, record <num>,
  //! to the Entity <ent> formerly created
  //! In case of Error or Warning, fills <ach> with messages
  //! Remark that the Case Number comes from translating a record
  Standard_EXPORT virtual void Read (const Standard_Integer casenum, const Handle(Interface_FileReaderData)& data, const Standard_Integer num, Handle(Interface_Check)& ach, const Handle(Standard_Transient)& ent) const = 0;
  
  //! Specific operator (create+read) defaulted to do nothing.
  //! It can be redefined when it is not possible to work in two
  //! steps (NewVoid then Read). This occurs when no default
  //! constructor is defined : hence the result <ent> must be
  //! created with an effective definition from the reader.
  //! Remark : if NewRead is defined, Copy has nothing to do.
  //!
  //! Returns True if it has produced something, false else.
  //! If nothing was produced, <ach> should be filled : it will be
  //! treated as "Unrecognized case" by reader tool.
  Standard_EXPORT virtual Standard_Boolean NewRead (const Standard_Integer casenum, const Handle(Interface_FileReaderData)& data, const Standard_Integer num, Handle(Interface_Check)& ach, Handle(Standard_Transient)& ent) const;




  DEFINE_STANDARD_RTTIEXT(Interface_ReaderModule,Standard_Transient)

protected:




private:




};







#endif // _Interface_ReaderModule_HeaderFile
