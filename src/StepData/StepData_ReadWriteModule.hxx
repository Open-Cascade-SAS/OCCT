// Created on: 1993-01-25
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

#ifndef _StepData_ReadWriteModule_HeaderFile
#define _StepData_ReadWriteModule_HeaderFile

#include <Standard.hxx>

#include <Interface_ReaderModule.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
class Interface_FileReaderData;
class TCollection_AsciiString;
class Interface_Check;
class Standard_Transient;
class StepData_StepReaderData;
class StepData_StepWriter;


class StepData_ReadWriteModule;
DEFINE_STANDARD_HANDLE(StepData_ReadWriteModule, Interface_ReaderModule)

//! Defines basic File Access Module (Recognize, Read, Write)
//! That is : ReaderModule (Recognize & Read) + Write for
//! StepWriter (for a more centralized description)
//! Warning : A sub-class of ReadWriteModule, which belongs to a particular
//! Protocol, must use the same definition for Case Numbers (give
//! the same Value for a StepType defined as a String from a File
//! as the Protocol does for the corresponding Entity)
class StepData_ReadWriteModule : public Interface_ReaderModule
{

public:

  
  //! Translate the Type of record <num> in <data> to a positive
  //! Case Number, or 0 if failed.
  //! Works with a StepReaderData, in which the Type of an Entity
  //! is defined as a String : Reads the RecordType <num> then calls
  //! CaseNum (this type)
  //! Warning : The methods CaseStep, StepType and Recognize,
  //! must be in phase (triplets CaseNum-StepType-Type of Object)
  Standard_EXPORT Standard_Integer CaseNum (const Handle(Interface_FileReaderData)& data, const Standard_Integer num) const Standard_OVERRIDE;
  
  //! Defines Case Numbers corresponding to the recognized Types
  //! Called by CaseNum (data,num) above for a Simple Type Entity
  //! Warning : CaseStep must give the same Value as Protocol does for the
  //! Entity type which corresponds to this Type given as a String
  Standard_EXPORT virtual Standard_Integer CaseStep (const TCollection_AsciiString& atype) const = 0;
  
  //! Same a above but for a Complex Type Entity ("Plex")
  //! The provided Default recognizes nothing
  Standard_EXPORT virtual Standard_Integer CaseStep (const TColStd_SequenceOfAsciiString& types) const;
  
  //! Returns True if the Case Number corresponds to a Complex Type
  //! ("Plex"). Remember that all possible combinations must be
  //! aknowledged to be processed
  //! Default is False for all cases. For a Protocol which defines
  //! possible Plexes, this method must be redefined.
  Standard_EXPORT virtual Standard_Boolean IsComplex (const Standard_Integer CN) const;
  
  //! Function specific to STEP, which delivers the StepType as it
  //! is recorded in and read from a File compliant with STEP.
  //! This method is symmetric to the method CaseStep.
  //! StepType can be different from Dynamic Type's name, but
  //! belongs to the same class of Object.
  //! Returns an empty String if <CN> is zero.
  //! Warning : For a Complex Type Entity, returns an Empty String
  //! (Complex Type must be managed by users)
  Standard_EXPORT virtual const TCollection_AsciiString& StepType (const Standard_Integer CN) const = 0;
  
  //! Function specific to STEP. Some STEP Types have a short form
  //! This method can be redefined to fill it
  //! By default, returns an empty string, which is then interpreted
  //! to take normal form from StepType
  Standard_EXPORT virtual TCollection_AsciiString ShortType (const Standard_Integer CN) const;
  
  //! Function specific to STEP, which delivers the list of types
  //! which corresponds to a complex type. If <CN> is not for a
  //! complex type, this method returns False. Else it returns True
  //! and fills the list in alphabetic order.
  //! The default returns False. To be redefined as required.
  Standard_EXPORT virtual Standard_Boolean ComplexType (const Standard_Integer CN, TColStd_SequenceOfAsciiString& types) const;
  
  //! General Read Function, calls ReadStep
  Standard_EXPORT void Read (const Standard_Integer CN, const Handle(Interface_FileReaderData)& data, const Standard_Integer num, Handle(Interface_Check)& ach, const Handle(Standard_Transient)& ent) const Standard_OVERRIDE;
  
  //! Specific Read Function. Works with StepReaderData
  Standard_EXPORT virtual void ReadStep (const Standard_Integer CN, const Handle(StepData_StepReaderData)& data, const Standard_Integer num, Handle(Interface_Check)& ach, const Handle(Standard_Transient)& ent) const = 0;
  
  //! Write Function, switched by CaseNum
  Standard_EXPORT virtual void WriteStep (const Standard_Integer CN, StepData_StepWriter& SW, const Handle(Standard_Transient)& ent) const = 0;




  DEFINE_STANDARD_RTTIEXT(StepData_ReadWriteModule,Interface_ReaderModule)

protected:




private:




};







#endif // _StepData_ReadWriteModule_HeaderFile
