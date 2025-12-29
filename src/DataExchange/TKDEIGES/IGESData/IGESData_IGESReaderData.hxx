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

#ifndef _IGESData_IGESReaderData_HeaderFile
#define _IGESData_IGESReaderData_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESData_IGESType.hxx>
#include <TCollection_HAsciiString.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <IGESData_GlobalSection.hxx>
#include <IGESData_DirPart.hxx>
#include <NCollection_Array1.hxx>
#include <IGESData_ReadStage.hxx>
#include <Interface_FileReaderData.hxx>
#include <Standard_Integer.hxx>
#include <Standard_CString.hxx>
#include <Interface_ParamType.hxx>
class Interface_ParamSet;
class Interface_Check;
class IGESData_DirPart;

//! specific FileReaderData for IGES
//! contains header as GlobalSection, and for each Entity, its
//! directory part as DirPart, list of Parameters as ParamSet
//! Each Item has a DirPart, plus classically a ParamSet and the
//! correspondent recognized Entity (inherited from FileReaderData)
//! Parameters are accessed through specific objects, ParamReaders
class IGESData_IGESReaderData : public Interface_FileReaderData
{

public:
  //! creates IGESReaderData correctly dimensioned (for arrays)
  //! <nbe> count of entities, that is, half nb of directory lines
  //! <nbp> : count of parameters
  Standard_EXPORT IGESData_IGESReaderData(const int nbe, const int nbp);

  //! adds a start line to start section
  Standard_EXPORT void AddStartLine(const char* aval);

  //! Returns the Start Section in once
  Standard_EXPORT occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>>
                  StartSection() const;

  //! adds a parameter to global section's parameter list
  Standard_EXPORT void AddGlobal(const Interface_ParamType atype, const char* aval);

  //! reads header (as GlobalSection) content from the ParamSet
  //! after it has been filled by successive calls to AddGlobal
  Standard_EXPORT void SetGlobalSection();

  //! returns header as GlobalSection
  Standard_EXPORT const IGESData_GlobalSection& GlobalSection() const;

  //! fills a DirPart, designated by its rank (that is, (N+1)/2 if N
  //! is its first number in section D)
  Standard_EXPORT void SetDirPart(const int   num,
                                  const int   i1,
                                  const int   i2,
                                  const int   i3,
                                  const int   i4,
                                  const int   i5,
                                  const int   i6,
                                  const int   i7,
                                  const int   i8,
                                  const int   i9,
                                  const int   i10,
                                  const int   i11,
                                  const int   i12,
                                  const int   i13,
                                  const int   i14,
                                  const int   i15,
                                  const int   i16,
                                  const int   i17,
                                  const char* res1,
                                  const char* res2,
                                  const char* label,
                                  const char* subs);

  //! returns DirPart identified by record no (half Dsect number)
  Standard_EXPORT const IGESData_DirPart& DirPart(const int num) const;

  //! returns values recorded in directory part n0 <num>
  Standard_EXPORT void DirValues(const int    num,
                                 int&         i1,
                                 int&         i2,
                                 int&         i3,
                                 int&         i4,
                                 int&         i5,
                                 int&         i6,
                                 int&         i7,
                                 int&         i8,
                                 int&         i9,
                                 int&         i10,
                                 int&         i11,
                                 int&         i12,
                                 int&         i13,
                                 int&         i14,
                                 int&         i15,
                                 int&         i16,
                                 int&         i17,
                                 const char*& res1,
                                 const char*& res2,
                                 const char*& label,
                                 const char*& subs) const;

  //! returns "type" and "form" info from a directory part
  Standard_EXPORT IGESData_IGESType DirType(const int num) const;

  //! Returns count of recorded Entities (i.e. size of Directory)
  Standard_EXPORT int NbEntities() const override;

  //! determines next suitable record from num; that is num+1 except
  //! for last one which gives 0
  Standard_EXPORT int FindNextRecord(const int num) const override;

  //! determines reference numbers in EntityNumber fields (called by
  //! SetEntities from IGESReaderTool)
  //! works on "Integer" type Parameters, because IGES does not
  //! distinguish Integer and Entity Refs : every Integer which is
  //! odd and less than twice NbRecords can be an Entity Ref ...
  //! (Ref Number is then (N+1)/2 if N is the Integer Value)
  Standard_EXPORT void SetEntityNumbers();

  //! Returns the recorded Global Check
  Standard_EXPORT occ::handle<Interface_Check> GlobalCheck() const;

  //! allows to set a default line weight, will be later applied at
  //! load time, on Entities which have no specified line weight
  Standard_EXPORT void SetDefaultLineWeight(const double defw);

  //! Returns the recorded Default Line Weight, if there is
  //! (else, returns 0)
  Standard_EXPORT double DefaultLineWeight() const;

  DEFINE_STANDARD_RTTIEXT(IGESData_IGESReaderData, Interface_FileReaderData)

private:
  IGESData_IGESType                                                         thectyp;
  occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> thestar;
  occ::handle<Interface_ParamSet>                                           theparh;
  IGESData_GlobalSection                                                    thehead;
  NCollection_Array1<IGESData_DirPart>                                      thedirs;
  IGESData_ReadStage                                                        thestep;
  double                                                                    thedefw;
  occ::handle<Interface_Check>                                              thechk;
};

#endif // _IGESData_IGESReaderData_HeaderFile
