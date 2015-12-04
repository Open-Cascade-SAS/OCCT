// Created on: 1992-04-06
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

#ifndef _IGESData_IGESModel_HeaderFile
#define _IGESData_IGESModel_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <IGESData_GlobalSection.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Standard_Integer.hxx>
#include <Standard_CString.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class Interface_InterfaceError;
class Message_Messenger;
class IGESData_GlobalSection;
class IGESData_IGESEntity;
class Interface_InterfaceModel;
class Interface_Check;
class Standard_Transient;
class TCollection_HAsciiString;


class IGESData_IGESModel;
DEFINE_STANDARD_HANDLE(IGESData_IGESModel, Interface_InterfaceModel)

//! Defines the file header and
//! entities for IGES files. These headers and entities result from
//! a complete data translation using the IGES data exchange processor.
//! Each entity is contained in a single model only and has a
//! unique identifier. You can access this identifier using the method Number.
//! Gives an access to the general data in the Start and the Global
//! sections of an IGES file.
//! The IGES file includes the following sections:
//! -Start,
//! -Global,
//! -Directory Entry,
//! -Parameter Data,
//! -Terminate
class IGESData_IGESModel : public Interface_InterfaceModel
{

public:

  
  Standard_EXPORT IGESData_IGESModel();
  
  //! Erases all data specific to IGES file Header (Start + Global)
  Standard_EXPORT void ClearHeader() Standard_OVERRIDE;
  
  //! Prints the IGES file header
  //! (Start and Global Sections) to the log file. The integer
  //! parameter is intended to be used as a level indicator but is not used at present.
  Standard_EXPORT void DumpHeader (const Handle(Message_Messenger)& S, const Standard_Integer level = 0) const Standard_OVERRIDE;
  
  //! Returns Model's Start Section (list of comment lines)
  Standard_EXPORT Handle(TColStd_HSequenceOfHAsciiString) StartSection() const;
  
  //! Returns the count of recorded Start Lines
  Standard_EXPORT Standard_Integer NbStartLines() const;
  
  //! Returns a line from the IGES file
  //! Start section by specifying its number. An empty string is
  //! returned if the number given is out of range, the range being
  //! from 1 to NbStartLines.
  Standard_EXPORT Standard_CString StartLine (const Standard_Integer num) const;
  
  //! Clears the IGES file Start Section
  Standard_EXPORT void ClearStartSection();
  
  //! Sets a new Start section from a list of strings.
  //! If copy is false, the Start section will be shared. Any
  //! modifications made to the strings later on, will have an effect on
  //! the Start section. If copy is true (default value),
  //! an independent copy of the strings is created and used as
  //! the Start section. Any modifications made to the strings
  //! later on, will have no effect on the Start section.
  Standard_EXPORT void SetStartSection (const Handle(TColStd_HSequenceOfHAsciiString)& list, const Standard_Boolean copy = Standard_True);
  
  //! Adds a new string to the existing
  //! Start section at the end if atnum is 0 or not given, or before
  //! atnumth line.
  Standard_EXPORT void AddStartLine (const Standard_CString line, const Standard_Integer atnum = 0);
  
  //! Returns the Global section of the IGES file.
  Standard_EXPORT const IGESData_GlobalSection& GlobalSection() const;
  
  //! Sets the Global section of the IGES file.
  Standard_EXPORT void SetGlobalSection (const IGESData_GlobalSection& header);
  
  //! Sets some of the Global section
  //! parameters with the values defined by the translation
  //! parameters. param may be:
  //! - receiver (value read in XSTEP.iges.header.receiver),
  //! - author (value read in XSTEP.iges.header.author),
  //! - company (value read in XSTEP.iges.header.company).
  //! The default value for param is an empty string.
  //! Returns True when done and if param is given, False if param is
  //! unknown or empty. Note: Set the unit in the IGES
  //! file Global section via IGESData_BasicEditor class.
  Standard_EXPORT Standard_Boolean ApplyStatic (const Standard_CString param = "");
  
  //! Returns an IGES entity given by its rank number.
  Standard_EXPORT Handle(IGESData_IGESEntity) Entity (const Standard_Integer num) const;
  
  //! Returns the equivalent DE Number for an Entity, i.e.
  //! 2*Number(ent)-1 , or 0 if <ent> is unknown from <me>
  //! This DE Number is used for File Writing for instance
  Standard_EXPORT Standard_Integer DNum (const Handle(IGESData_IGESEntity)& ent) const;
  
  //! gets Header (GlobalSection) from another Model
  Standard_EXPORT void GetFromAnother (const Handle(Interface_InterfaceModel)& other) Standard_OVERRIDE;
  
  //! Returns a New Empty Model, same type as <me> i.e. IGESModel
  Standard_EXPORT Handle(Interface_InterfaceModel) NewEmptyModel() const Standard_OVERRIDE;
  
  //! Checks that the IGES file Global
  //! section contains valid data that conforms to the IGES specifications.
  Standard_EXPORT virtual void VerifyCheck (Handle(Interface_Check)& ach) const Standard_OVERRIDE;
  
  //! Sets LineWeights of contained Entities according header data
  //! (MaxLineWeight and LineWeightGrad) or to a default value for
  //! undefined weights
  Standard_EXPORT void SetLineWeights (const Standard_Real defw);
  
  //! erases specific labels, i.e. does nothing
  Standard_EXPORT void ClearLabels() Standard_OVERRIDE;
  
  //! Prints label specific to IGES norm for a given entity, i.e.
  //! its directory entry number (2*Number-1)
  Standard_EXPORT void PrintLabel (const Handle(Standard_Transient)& ent, const Handle(Message_Messenger)& S) const Standard_OVERRIDE;
  
  //! Prints label specific to IGES norm  for a given -- --
  //! entity,  i.e.  its directory entry number (2*Number-1)
  //! in the log file format.
  Standard_EXPORT virtual void PrintToLog (const Handle(Standard_Transient)& ent, const Handle(Message_Messenger)& S) const Standard_OVERRIDE;
  
  //! Prints label specific to IGES norm for a given entity, i.e.
  //! its directory entry number (2*Number-1)
  Standard_EXPORT void PrintInfo (const Handle(Standard_Transient)& ent, const Handle(Message_Messenger)& S) const;
  
  //! Returns a string with the label attached to a given entity,
  //! i.e. a string "Dnn" with nn = directory entry number (2*N-1)
  Standard_EXPORT Handle(TCollection_HAsciiString) StringLabel (const Handle(Standard_Transient)& ent) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IGESData_IGESModel,Interface_InterfaceModel)

protected:




private:


  Handle(TColStd_HSequenceOfHAsciiString) thestart;
  IGESData_GlobalSection theheader;


};







#endif // _IGESData_IGESModel_HeaderFile
