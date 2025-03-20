// Created on: 1993-11-03
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

#ifndef _IFSelect_SessionFile_HeaderFile
#define _IFSelect_SessionFile_HeaderFile

#include <NCollection_DataMap.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include <Standard_Integer.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_CString.hxx>

class IFSelect_WorkSession;
class Standard_Transient;

//! A SessionFile is intended to manage access between a
//! WorkSession and an Ascii Form, to be considered as a Dump.
//! It allows to write the File from the WorkSession, and later
//! read the File to the WorkSession, by keeping required
//! descriptions (such as dependences).
//!
//! The produced File is under an Ascii Form, then it may be
//! easily consulted.
//! It is possible to cumulate reading of several Files. But in
//! case of Names conflict, the newer Names are forgottens.
//!
//! The Dump supports the description of XSTEP functionalities
//! (Sharing an Interface File, with Selections, Dispatches,
//! Modifiers ...) but does not refer to the Interface File
//! which is currently loaded.
//!
//! SessionFile works with a library of SessionDumper type objects
//!
//! The File is Produced as follows :
//! SessionFile produces all general Information (such as Int and
//! Text Parameters, Types and Inputs of Selections, Dispatches,
//! Modifiers ...) and calls the SessionDumpers to produce all
//! the particular Data : creation arguments, parameters to be set
//! It is Read in the same terms :
//! SessionFile reads and interprets all general Information,
//! and calls the SessionDumpers to recognize Types and for a
//! recognized Type create the corresponding Object with its
//! particular parameters as they were written.
//! The best way to work is to have one SessionDumper for each
//! consistent set of classes (e.g. a package).
class IFSelect_SessionFile
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a SessionFile, ready to read Files in order to load
  //! them into a given WorkSession.
  //! The following Read Operations must then be called.
  //! It is also possible to perform a Write, which produces a
  //! complete File of all the content of the WorkSession.
  Standard_EXPORT IFSelect_SessionFile(const Handle(IFSelect_WorkSession)& WS);

  //! Creates a SessionFile which Writes the content of a WorkSession
  //! to a File (directly calls Write)
  //! Then, IsDone acknowledges on the result of the Operation.
  //! But such a SessionFile may not Read a File to a WorkSession.
  Standard_EXPORT IFSelect_SessionFile(const Handle(IFSelect_WorkSession)& WS,
                                       const Standard_CString              filename);

  //! Clears the lines recorded whatever for writing or for reading
  Standard_EXPORT void ClearLines();

  //! Returns the count of recorded lines
  Standard_EXPORT Standard_Integer NbLines() const;

  //! Returns a line given its rank in the list of recorded lines
  Standard_EXPORT const TCollection_AsciiString& Line(const Standard_Integer num) const;

  //! Adds a line to the list of recorded lines
  Standard_EXPORT void AddLine(const Standard_CString line);

  //! Removes the last line. Can be called recursively.
  //! Does nothing if the list is empty
  Standard_EXPORT void RemoveLastLine();

  //! Writes the recorded lines to a file named <name> then clears
  //! the list of lines.
  //! Returns False (with no clearing) if the file could not be
  //! created
  Standard_EXPORT Standard_Boolean WriteFile(const Standard_CString name);

  //! Reads the recorded lines from a file named <name>, after
  //! having cleared the list (stops if RecognizeFile fails)
  //! Returns False (with no clearing) if the file could not be read
  Standard_EXPORT Standard_Boolean ReadFile(const Standard_CString name);

  //! Recognizes the header line. returns True if OK, False else
  Standard_EXPORT Standard_Boolean RecognizeFile(const Standard_CString headerline);

  //! Performs a Write Operation from a WorkSession to a File
  //! i.e. calls WriteSession then WriteEnd, and WriteFile
  //! Returned Value is : 0 for OK, -1 File could not be created,
  //! >0 Error during Write (see WriteSession)
  //! IsDone can be called too (will return True for OK)
  Standard_EXPORT Standard_Integer Write(const Standard_CString filename);

  //! Performs a Read Operation from a file to a WorkSession
  //! i.e. calls ReadFile, then ReadSession and ReadEnd
  //! Returned Value is : 0 for OK, -1 File could not be opened,
  //! >0 Error during Read  (see WriteSession)
  //! IsDone can be called too (will return True for OK)
  Standard_EXPORT Standard_Integer Read(const Standard_CString filename);

  //! Prepares the Write operation from a WorkSession (IFSelect) to
  //! a File, i.e. fills the list of lines (the file itself remains
  //! to be written; or NbLines/Line may be called)
  //! Important Remark : this excludes the reading of the last line,
  //! which is performed by WriteEnd
  //! Returns 0 if OK, status > 0 in case of error
  Standard_EXPORT Standard_Integer WriteSession();

  //! Writes the trailing line. It is separate from WriteSession,
  //! in order to allow to redefine WriteSession without touching
  //! WriteEnd (WriteSession defines the body of the file)
  //! WriteEnd fills the list of lines. Returns a status of error,
  //! 0 if OK, >0 else
  Standard_EXPORT Standard_Integer WriteEnd();

  //! Writes a line to the File. If <follow> is given, it is added
  //! at the following of the line. '\n' must be added for the end.
  Standard_EXPORT void WriteLine(const Standard_CString line, const Standard_Character follow = 0);

  //! Writes the Parameters own to each type of Item. Uses the
  //! Library of SessionDumpers
  //! Returns True if Done, False if <item> could not be treated
  //! (hence it remains written with no Own Parameter)
  Standard_EXPORT Standard_Boolean WriteOwn(const Handle(Standard_Transient)& item);

  //! Performs a Read Operation from a File to a WorkSession, i.e.
  //! reads the list of line (which must have already been loaded,
  //! by ReadFile or by calls to AddLine)
  //! Important Remark : this excludes the reading of the last line,
  //! which is performed by ReadEnd
  //! Returns 0 for OK, >0 status for Read Error (not a suitable
  //! File, or WorkSession given as Immutable at Creation Time)
  //! IsDone can be called too (will return True for OK)
  Standard_EXPORT Standard_Integer ReadSession();

  //! Reads the end of a file (its last line). Returns 0 if OK,
  //! status >0 in case of error (not a suitable end line).
  Standard_EXPORT Standard_Integer ReadEnd();

  //! Reads a Line and splits it into a set of alphanumeric items,
  //! which can then be queried by NbParams/ParamValue ...
  Standard_EXPORT Standard_Boolean ReadLine();

  //! Internal routine which processes a line into words
  //! and prepares its exploration
  Standard_EXPORT void SplitLine(const Standard_CString line);

  //! Tries to Read an Item, by calling the Library of Dumpers
  //! Sets the list of parameters of the line to be read from the
  //! first own one
  Standard_EXPORT Standard_Boolean ReadOwn(Handle(Standard_Transient)& item);

  //! Adds an Item to the WorkSession, taken as Name the first
  //! item of the read Line. If this Name is not a Name but a Number
  //! or if this Name is already recorded in the WorkSession, it
  //! adds the Item but with no Name. Then the Name is recorded
  //! in order to be used by the method ItemValue
  //! <active> commands to make <item> active or not in the session
  Standard_EXPORT void AddItem(const Handle(Standard_Transient)& item,
                               const Standard_Boolean            active = Standard_True);

  //! Returns True if the last Read or Write operation has been correctly performed.
  //! Else returns False.
  Standard_EXPORT Standard_Boolean IsDone() const;

  //! Returns the WorkSession on which a SessionFile works.
  //! Remark that it is returned as Immutable.
  Standard_EXPORT Handle(IFSelect_WorkSession) WorkSession() const;

  //! At beginning of writing an Item, writes its basics :
  //! - either its name in the session if it has one
  //! - or its relative number of item in the file, else (preceded by a '_')
  //! - then, its Dynamic Type (in the sense of cdl : pk_class)
  //! This basic description can be followed by the parameters
  //! which are used in the definition of the item.
  Standard_EXPORT void NewItem(const Standard_Integer ident, const Handle(Standard_Transient)& par);

  //! Sets Parameters to be sent as Own if <mode> is True (their
  //! Name or Number or Void Mark or Text Value is preceded by a
  //! Column sign ':') else they are sent normally
  //! Hence, the Own Parameter are clearly identified in the File
  Standard_EXPORT void SetOwn(const Standard_Boolean mode);

  //! During a Write action, commands to send a Void Parameter
  //! i.e. a Parameter which is present but undefined
  //! Its form will be the dollar sign : $
  Standard_EXPORT void SendVoid();

  //! During a Write action, commands to send the identification of
  //! a Parameter : if it is Null (undefined) it is send as Void ($)
  //! if it is Named in the WorkSession, its Name is sent preceded
  //! by ':', else a relative Ident Number is sent preceded by '#'
  //! (relative to the present Write, i.e. starting at one, without
  //! skip, and counted part from Named Items)
  Standard_EXPORT void SendItem(const Handle(Standard_Transient)& par);

  //! During a Write action, commands to send a Text without
  //! interpretation. It will be sent as well
  Standard_EXPORT void SendText(const Standard_CString text);

  //! Sets the rank of Last General Parameter to a new value. It is
  //! followed by the Fist Own Parameter of the item.
  //! Used by SessionFile after reading general parameters.
  Standard_EXPORT void SetLastGeneral(const Standard_Integer lastgen);

  //! During a Read operation, SessionFile processes sequentially the Items to read.
  //! For each one, it gives access to the list
  //! of its Parameters : they were defined by calls to
  //! SendVoid/SendParam/SendText during Writing the File.
  //! NbParams returns the count of Parameters for the line
  //! currently read.
  Standard_EXPORT Standard_Integer NbParams() const;

  //! Returns True if a Parameter, given its rank in the Own List
  //! (see NbOwnParams), is Void. Returns also True if <num> is
  //! out of range (undefined parameters)
  Standard_EXPORT Standard_Boolean IsVoid(const Standard_Integer num) const;

  //! Returns True if a Parameter, in the Own List (see NbOwnParams)
  //! is a Text (between "..."). Else it is an Item (Parameter,
  //! Selection, Dispatch ...), which can be Void.
  Standard_EXPORT Standard_Boolean IsText(const Standard_Integer num) const;

  //! Returns a Parameter (alphanumeric item of a line) as it
  //! has been read
  Standard_EXPORT const TCollection_AsciiString& ParamValue(const Standard_Integer num) const;

  //! Returns the content of a Text Parameter (without the quotes).
  //! Returns an empty string if the Parameter is not a Text.
  Standard_EXPORT TCollection_AsciiString TextValue(const Standard_Integer num) const;

  //! Returns a Parameter as an Item. Returns a Null Handle if the
  //! Parameter is a Text, or if it is defined as Void
  Standard_EXPORT Handle(Standard_Transient) ItemValue(const Standard_Integer num) const;

  //! Specific Destructor (closes the File if not yet done)
  Standard_EXPORT void Destroy();

  ~IFSelect_SessionFile() { Destroy(); }

protected:
  Handle(IFSelect_WorkSession)                                   thesess;
  Handle(TColStd_HArray1OfInteger)                               thenums;
  NCollection_DataMap<TCollection_AsciiString, Standard_Integer> thenames;
  Standard_Integer                                               thenl;
  TColStd_SequenceOfAsciiString                                  theline;

private:
  Standard_Boolean              themode;
  TColStd_SequenceOfAsciiString thelist;
  TCollection_AsciiString       thebuff;
  Standard_Integer              thelastgen;
  Standard_Boolean              thedone;
  Standard_Boolean              theownflag;
  Standard_Integer              thenewnum;
};

#endif // _IFSelect_SessionFile_HeaderFile
