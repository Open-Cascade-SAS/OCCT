// Created on: 1992-04-07
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

#ifndef _IGESData_IGESWriter_HeaderFile
#define _IGESData_IGESWriter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <Standard_Character.hxx>
#include <IGESData_Array1OfDirPart.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <Standard_Integer.hxx>
#include <IGESData_ReadStage.hxx>
#include <Interface_LineBuffer.hxx>
#include <Interface_FloatWriter.hxx>
#include <Standard_CString.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Standard_OStream.hxx>

class IGESData_IGESModel;
class Interface_InterfaceMismatch;
class Interface_FloatWriter;
class IGESData_Protocol;
class IGESData_GlobalSection;
class IGESData_IGESEntity;
class TCollection_HAsciiString;
class gp_XY;
class gp_XYZ;

//! manages atomic file writing, under control of IGESModel :
//! prepare text to be sent then sends it
//! takes into account distinction between successive Sections
class IGESData_IGESWriter 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates an IGESWriter, empty ready to work
  //! (see the methods SendModel and Print)
  Standard_EXPORT IGESData_IGESWriter(const Handle(IGESData_IGESModel)& amodel);
  
  //! Default constructor (not used) to satisfy the compiler
  Standard_EXPORT IGESData_IGESWriter();
  
  //! Constructor by copy (not used) to satisfy the compiler
  Standard_EXPORT IGESData_IGESWriter(const IGESData_IGESWriter& other);
  
  //! Returns the embedded FloatWriter, which controls sending Reals
  //! Use this method to access FloatWriter in order to consult or
  //! change its options (MainFormat, FormatForRange,ZeroSuppress),
  //! because it is returned as the address of its field
  Standard_EXPORT Interface_FloatWriter& FloatWriter();
  
  //! Returns the write mode, in order to be read and/or changed
  //! Write Mode controls the way final print works
  //! 0 (D) : Normal IGES, 10 : FNES
  Standard_EXPORT Standard_Integer& WriteMode();
  
  //! Sends an additionnal Starting Line : this is the way used to
  //! send comments in an IGES File (at beginning of the file).
  //! If the line is more than 72 char.s long, it is splited into
  //! as many lines as required to send it completely
  Standard_EXPORT void SendStartLine (const Standard_CString startline);
  
  //! Sends the complete IGESModel (Global Section, Entities as
  //! Directory Entries & Parameter Lists, etc...)
  //! i.e. fills a list of texts. Once filled, it can be sent by
  //! method Print
  Standard_EXPORT void SendModel (const Handle(IGESData_Protocol)& protocol);
  
  //! declares sending of S section (only a declaration)
  //! error if state is not initial
  Standard_EXPORT void SectionS();
  
  //! prepares sending of header, from a GlobalSection (stores it)
  //! error if SectionS was not called just before
  //! takes in account special characters (Separator, EndMark)
  Standard_EXPORT void SectionG (const IGESData_GlobalSection& header);
  
  //! prepares sending of list of entities, as Sections D (directory
  //! list) and P (Parameters lists, one per entity)
  //! Entities will be then processed, one after the other
  //! error if SectionG has not be called just before
  Standard_EXPORT void SectionsDP();
  
  //! declares sending of T section (only a declaration)
  //! error if does not follow Entities sending
  Standard_EXPORT void SectionT();
  
  //! translates directory part of an Entity into a litteral DirPart
  //! Some infos are computed after sending parameters
  //! Error if not in sections DP or Stage not "Dir"
  Standard_EXPORT void DirPart (const Handle(IGESData_IGESEntity)& anent);
  
  //! sends own parameters of the entity, by sending firstly its
  //! type, then calling specific method WriteOwnParams
  //! Error if not in sections DP or Stage not "Own"
  Standard_EXPORT void OwnParams (const Handle(IGESData_IGESEntity)& anent);
  
  //! sends associativity list, as complement of parameters list
  //! error if not in sections DP or Stage not "Associativity"
  Standard_EXPORT void Associativities (const Handle(IGESData_IGESEntity)& anent);
  
  //! sends property list, as complement of parameters list
  //! error if not in sections DP or Stage not "Property"
  Standard_EXPORT void Properties (const Handle(IGESData_IGESEntity)& anent);
  
  //! declares end of sending an entity (ends param list by ';')
  Standard_EXPORT void EndEntity();
  
  //! sends a void parameter, that is null text
  Standard_EXPORT void SendVoid();
  
  //! sends an Integer parameter
  Standard_EXPORT void Send (const Standard_Integer val);
  
  //! sends a Boolean parameter as an Integer value 0(False)/1(True)
  Standard_EXPORT void SendBoolean (const Standard_Boolean val);
  
  //! sends a Real parameter. Works with FloatWriter
  Standard_EXPORT void Send (const Standard_Real val);
  
  //! sends a Text parameter under Hollerith form
  Standard_EXPORT void Send (const Handle(TCollection_HAsciiString)& val);
  
  //! sends a Reference to an Entity (if its Number is N, its
  //! pointer is 2*N-1)
  //! If <val> is Null, "0" will be sent
  //! If <negative> is True, "Pointer" is sent as negative
  Standard_EXPORT void Send (const Handle(IGESData_IGESEntity)& val, const Standard_Boolean negative = Standard_False);

  //! Helper method to avoid ambiguity of calls to above methods Send() for
  //! classes derived from IGESData_IGESEntity, for VC++ 10 and 11 compillers
  template <class T> 
  void Send (const Handle(T)& val, Standard_Boolean negative = Standard_False, 
             typename std::enable_if<std::is_base_of<IGESData_IGESEntity, T>::value>::type * = 0)
  { 
    Send ((const Handle(IGESData_IGESEntity)&)val, negative);
  }
  
  //! sends a parameter under its exact form given as a string
  Standard_EXPORT void SendString (const Handle(TCollection_HAsciiString)& val);
  
  //! Sends a XY, interpreted as a couple of 2 Reals (X & Y)
  Standard_EXPORT void Send (const gp_XY& val);
  
  //! Sends a XYZ, interpreted as a couple of 2 Reals (X , Y & Z)
  Standard_EXPORT void Send (const gp_XYZ& val);
  
  //! Returns the list of strings for a section given its rank
  //! 1 : Start (if not empty)  2 : Global  3 or 4 : Parameters
  //! RQ: no string list for Directory section
  //! An empty section gives a null handle
  Standard_EXPORT Handle(TColStd_HSequenceOfHAsciiString) SectionStrings (const Standard_Integer numsec) const;
  
  //! Writes result on an output defined as an OStream
  //! resolves stored infos at this time; in particular, numbers of
  //! lines used to adress P-section from D-section and final totals
  //! Takes WriteMode into account
  Standard_EXPORT Standard_Boolean Print (Standard_OStream& S) const;

private:
  
  //! Basic action of adding a string to current parameter list as a
  //! line; manages size limit (64 or 72 according Sestion G or P)
  //! <more>, if precised, requires that <more> characters will
  //! remain free on the current line once this AddString done
  Standard_EXPORT void AddString (const Handle(TCollection_HAsciiString)& val, const Standard_Integer more = 0);
  
  //! Basic action of adding a string to current parameter list as a
  //! line. Manages size limit (64 or 72 according Sestion G or P)
  //! <val> is the string and <lnval> its length. If <lnval> is not
  //! given, it is computed by strlen(val).
  //! <more>, if precised, requires that <more> characters will
  //! remain free on the current line once this AddString done
  Standard_EXPORT void AddString (const Standard_CString val, const Standard_Integer lnval = 0, const Standard_Integer more = 0);
  
  //! Adds a string defined as a single character (for instance, the
  //! parameter separator). Manages size limit
  //! <more>, if precised, requires that <more> characters will
  //! remain free on the current line once this AddString done
  Standard_EXPORT void AddChar (const Standard_Character val, const Standard_Integer more = 0);


private:
  
  Handle(IGESData_IGESModel) themodel;
  Handle(TColStd_HSequenceOfHAsciiString) thestar;
  Handle(TColStd_HSequenceOfHAsciiString) thehead;
  Standard_Character thesep;
  Standard_Character theendm;
  IGESData_Array1OfDirPart thedirs;
  TColStd_Array1OfInteger thepnum;
  Handle(TColStd_HSequenceOfHAsciiString) thepars;
  Standard_Integer thesect;
  IGESData_ReadStage thestep;
  Interface_LineBuffer thecurr;
  Standard_Integer themodew;
  Interface_FloatWriter thefloatw;
};

#endif // _IGESData_IGESWriter_HeaderFile
