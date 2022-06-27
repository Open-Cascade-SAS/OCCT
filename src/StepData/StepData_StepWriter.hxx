// Created on: 1992-02-11
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

#ifndef _StepData_StepWriter_HeaderFile
#define _StepData_StepWriter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <Interface_LineBuffer.hxx>
#include <Standard_Integer.hxx>
#include <Interface_FloatWriter.hxx>
#include <Interface_CheckIterator.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <Standard_CString.hxx>
#include <StepData_Logical.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <Standard_OStream.hxx>
class StepData_StepModel;
class StepData_Protocol;
class StepData_WriterLib;
class TCollection_AsciiString;
class TCollection_HAsciiString;
class StepData_Field;
class StepData_PDescr;
class StepData_SelectMember;
class StepData_FieldList;
class StepData_ESDescr;
class Standard_Transient;


//! manages atomic file writing, under control of StepModel (for
//! general organisation of file) and each class of Transient
//! (for its own parameters) : prepares text to be written then
//! writes it
//! A stream cannot be used because Step limits line length at 72
//! In more, a specific object offers more appropriate functions
class StepData_StepWriter 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates an empty StepWriter from a StepModel. The StepModel
  //! provides the Number of Entities, as identifiers for File
  Standard_EXPORT StepData_StepWriter(const Handle(StepData_StepModel)& amodel);
  
  //! ModeLabel controls how to display entity ids :
  //! 0 (D) gives entity number in the model
  //! 1 gives the already recorded label (else, its number)
  //! Warning : conflicts are not controlled
  Standard_EXPORT Standard_Integer& LabelMode();
  
  //! TypeMode  controls the type form to use :
  //! 0 (D) for normal long form
  //! 1 for short form (if a type name has no short form, normal
  //! long form is then used)
  Standard_EXPORT Standard_Integer& TypeMode();
  
  //! Returns the embedded FloatWriter, which controls sending Reals
  //! Use this method to access FloatWriter in order to consult or
  //! change its options (MainFormat, FormatForRange,ZeroSuppress),
  //! because it is returned as the address of its field
  Standard_EXPORT Interface_FloatWriter& FloatWriter();
  
  //! Declares the Entity Number <numscope> to correspond to a Scope
  //! which contains the Entity Number <numin>. Several calls to the
  //! same <numscope> add Entities in this Scope, in this order.
  //! Error if <numin> is already declared in the Scope
  //! Warning : the declaration of the Scopes is assumed to be consistent,
  //! i.e. <numin> is not referenced from outside this Scope
  //! (not checked here)
  Standard_EXPORT void SetScope (const Standard_Integer numscope, const Standard_Integer numin);
  
  //! Returns True if an Entity identified by its Number is in a Scope
  Standard_EXPORT Standard_Boolean IsInScope (const Standard_Integer num) const;
  
  //! Sends the complete Model, included HEADER and DATA Sections
  //! Works with a WriterLib defined through a Protocol
  //! If <headeronly> is given True, only the HEADER Section is sent
  //! (used to Dump the Header of a StepModel)
  Standard_EXPORT void SendModel (const Handle(StepData_Protocol)& protocol, const Standard_Boolean headeronly = Standard_False);
  
  //! Begins model header
  Standard_EXPORT void SendHeader();
  
  //! Begins data section; error if EndSec was not set
  Standard_EXPORT void SendData();
  
  //! Send an Entity of the Data Section. If it corresponds to a
  //! Scope, also Sends the Scope information and contained Items
  Standard_EXPORT void SendEntity (const Standard_Integer nument, const StepData_WriterLib& lib);
  
  //! sets end of section; to be done before passing to next one
  Standard_EXPORT void EndSec();
  
  //! sets end of file; error is EndSec was not set
  Standard_EXPORT void EndFile();
  
  //! flushes current line; if empty, flushes it (defines a new
  //! empty line) if evenempty is True; else, skips it
  Standard_EXPORT void NewLine (const Standard_Boolean evenempty);
  
  //! joins current line to last one, only if new length is 72 max
  //! if newline is True, a new current line begins; else, current
  //! line is set to the last line (once joined) itself an can be
  //! completed
  Standard_EXPORT void JoinLast (const Standard_Boolean newline);
  
  //! asks that further indentations will begin at position of
  //! entity first opening bracket; else they begin at zero (def)
  //! for each sublist level, two more blancks are added at beginning
  //! (except for text continuation, which must begin at true zero)
  Standard_EXPORT void Indent (const Standard_Boolean onent);
  
  //! begins an entity with an ident plus '=' (at beginning of line)
  //! entity ident is its Number given by the containing Model
  //! Warning : <ident> must be, either Number or Label, according LabelMode
  Standard_EXPORT void SendIdent (const Standard_Integer ident);
  
  //! sets a begin of Scope (ends this line)
  Standard_EXPORT void SendScope();
  
  //! sets an end of Scope  (on a separate line)
  Standard_EXPORT void SendEndscope();
  
  //! sets a comment mark : if mode is True, begins Comment zone,
  //! if mode is False, ends Comment zone (if one is begun)
  Standard_EXPORT void Comment (const Standard_Boolean mode);
  
  //! sends a comment. Error if we are not inside a comment zone
  Standard_EXPORT void SendComment (const Handle(TCollection_HAsciiString)& text);
  
  //! same as above but accepts a CString (ex.: "..." directly)
  Standard_EXPORT void SendComment (const Standard_CString text);
  
  //! sets entity's StepType, opens brakets, starts param no to 0
  //! params are separated by comma
  //! Remark : for a Multiple Type Entity (see Express ANDOR clause)
  //! StartComplex must be called before sending components, then
  //! each "Component" must be sent separately (one call to
  //! StartEntity for each one) : the Type which precedes is then
  //! automatically closed. Once all the components have been sent,
  //! EndComplex must be called, then and only then EndEntity
  Standard_EXPORT void StartEntity (const TCollection_AsciiString& atype);
  
  //! sends the start of a complex entity, which is a simple open
  //! bracket (without increasing braket level)
  //! It must be called JUST AFTER SendEntity and BEFORE sending
  //! components, each one begins by StartEntity
  Standard_EXPORT void StartComplex();
  
  //! sends the end of a complex entity : a simple closed bracket
  //! It must be called AFTER sending all the components and BEFORE
  //! the final call to EndEntity
  Standard_EXPORT void EndComplex();
  
  //! Sends the content of a field, controlled by its descriptor
  //! If the descriptor is not defined, follows the description
  //! detained by the field itself
  Standard_EXPORT void SendField (const StepData_Field& fild, const Handle(StepData_PDescr)& descr);
  
  //! Sends a SelectMember, which cab be named or not
  Standard_EXPORT void SendSelect (const Handle(StepData_SelectMember)& sm, const Handle(StepData_PDescr)& descr);
  
  //! Send the content of an entity as being a FieldList controlled
  //! by its descriptor. This includes start and end brackets but
  //! not the entity type
  Standard_EXPORT void SendList (const StepData_FieldList& list, const Handle(StepData_ESDescr)& descr);
  
  //! open a sublist by a '('
  Standard_EXPORT void OpenSub();
  
  //! open a sublist with its type then a '('
  Standard_EXPORT void OpenTypedSub (const Standard_CString subtype);
  
  //! closes a sublist by a ')'
  Standard_EXPORT void CloseSub();
  
  //! prepares adding a parameter (that is, adds ',' except for
  //! first one); normally for internal use; can be used to send
  //! a totally empty parameter (with no literal value)
  Standard_EXPORT void AddParam();
  
  //! sends an integer parameter
  Standard_EXPORT void Send (const Standard_Integer val);
  
  //! sends a real parameter (works with FloatWriter)
  Standard_EXPORT void Send (const Standard_Real val);
  
  //! sends a text given as string (it will be set between '...')
  Standard_EXPORT void Send (const TCollection_AsciiString& val);
  
  //! sends a reference to an entity (its identifier with '#')
  //! REMARK 1 : a Null <val> is interpreted as "Undefined"
  //! REMARK 2 : for an HAsciiString which is not recorded in the
  //! Model, it is send as its String Content, between quotes
  Standard_EXPORT void Send (const Handle(Standard_Transient)& val);
  
  //! sends a Boolean as .T. for True or .F. for False
  //! (it is an useful case of Enum, which is built-in)
  Standard_EXPORT void SendBoolean (const Standard_Boolean val);
  
  //! sends a Logical as .T. or .F. or .U. according its Value
  //! (it is a standard case of Enum for Step, and is built-in)
  Standard_EXPORT void SendLogical (const StepData_Logical val);
  
  //! sends a string exactly as it is given
  Standard_EXPORT void SendString (const TCollection_AsciiString& val);
  
  //! sends a string exactly as it is given
  Standard_EXPORT void SendString (const Standard_CString val);
  
  //! sends an enum given by String (literal expression)
  //! adds '.' around it if not done
  //! Remark : val can be computed by class EnumTool from StepData:
  //! StepWriter.SendEnum (myenum.Text(enumval));
  Standard_EXPORT void SendEnum (const TCollection_AsciiString& val);
  
  //! sends an enum given by String (literal expression)
  //! adds '.' around it if not done
  Standard_EXPORT void SendEnum (const Standard_CString val);
  
  //! sends an array of real
  Standard_EXPORT void SendArrReal (const Handle(TColStd_HArray1OfReal)& anArr);
  
  //! sends an undefined (optional absent) parameter (by '$')
  Standard_EXPORT void SendUndef();
  
  //! sends a "Derived" parameter (by '*'). A Derived Parameter has
  //! been inherited from a Super-Type then redefined as being
  //! computed by a function. Hence its value in file is senseless.
  Standard_EXPORT void SendDerived();
  
  //! sends end of entity (closing bracket plus ';')
  //! Error if count of opened-closed brackets is not null
  Standard_EXPORT void EndEntity();
  
  //! Returns the check-list, which has received possible checks :
  //! for unknown entities, badly loaded ones, null or unknown
  //! references
  Standard_EXPORT Interface_CheckIterator CheckList() const;
  
  //! Returns count of Lines
  Standard_EXPORT Standard_Integer NbLines() const;
  
  //! Returns a Line given its rank in the File
  Standard_EXPORT Handle(TCollection_HAsciiString) Line (const Standard_Integer num) const;
  
  //! writes result on an output defined as an OStream
  //! then clears it
  Standard_EXPORT Standard_Boolean Print (Standard_OStream& S);




protected:





private:

  
  //! adds a string to current line; first flushes it if full
  //! (72 char); more allows to ask a reserve at end of line : flush
  //! is done if remaining length (to 72) is less than <more>
  Standard_EXPORT void AddString (const TCollection_AsciiString& str, const Standard_Integer more = 0);
  
  //! Same as above, but the string is given by CString + Length
  Standard_EXPORT void AddString (const Standard_CString str, const Standard_Integer lnstr, const Standard_Integer more = 0);


  Handle(StepData_StepModel) themodel;
  Handle(TColStd_HSequenceOfHAsciiString) thefile;
  Interface_LineBuffer thecurr;
  Standard_Boolean thesect;
  Standard_Boolean thecomm;
  Standard_Boolean thefirst;
  Standard_Boolean themult;
  Standard_Integer thelevel;
  Standard_Boolean theindent;
  Standard_Integer theindval;
  Standard_Integer thetypmode;
  Interface_FloatWriter thefloatw;
  Interface_CheckIterator thechecks;
  Standard_Integer thenum;
  Standard_Integer thelabmode;
  Handle(TColStd_HArray1OfInteger) thescopebeg;
  Handle(TColStd_HArray1OfInteger) thescopeend;
  Handle(TColStd_HArray1OfInteger) thescopenext;


};







#endif // _StepData_StepWriter_HeaderFile
