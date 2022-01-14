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
#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <IGESData_GlobalSection.hxx>
#include <IGESData_Array1OfDirPart.hxx>
#include <IGESData_ReadStage.hxx>
#include <Interface_FileReaderData.hxx>
#include <Standard_Integer.hxx>
#include <Standard_CString.hxx>
#include <Interface_ParamType.hxx>
class Interface_ParamSet;
class Interface_Check;
class IGESData_DirPart;


class IGESData_IGESReaderData;
DEFINE_STANDARD_HANDLE(IGESData_IGESReaderData, Interface_FileReaderData)

//! specific FileReaderData for IGES
//! contains header as GlobalSection, and for each Entity, its
//! directory part as DirPart, list of Parameters as ParamSet
//! Each Item has a DirPart, plus classically a ParamSet and the
//! correspondent recognized Entity (inherited from FileReaderData)
//! Parameters are accessed through specific objects, ParamReaders
class IGESData_IGESReaderData : public Interface_FileReaderData
{

public:

  
  //! creates IGESReaderData correctly dimensionned (for arrays)
  //! <nbe> count of entities, that is, half nb of directory lines
  //! <nbp> : count of parameters
  Standard_EXPORT IGESData_IGESReaderData(const Standard_Integer nbe, const Standard_Integer nbp);
  
  //! adds a start line to start section
  Standard_EXPORT void AddStartLine (const Standard_CString aval);
  
  //! Returns the Start Section in once
  Standard_EXPORT Handle(TColStd_HSequenceOfHAsciiString) StartSection() const;
  
  //! adds a parameter to global section's parameter list
  Standard_EXPORT void AddGlobal (const Interface_ParamType atype, const Standard_CString aval);
  
  //! reads header (as GlobalSection) content from the ParamSet
  //! after it has been filled by successive calls to AddGlobal
  Standard_EXPORT void SetGlobalSection();
  
  //! returns header as GlobalSection
  Standard_EXPORT const IGESData_GlobalSection& GlobalSection() const;
  
  //! fills a DirPart, designated by its rank (that is, (N+1)/2 if N
  //! is its first number in section D)
  Standard_EXPORT void SetDirPart (const Standard_Integer num, const Standard_Integer i1, const Standard_Integer i2, const Standard_Integer i3, const Standard_Integer i4, const Standard_Integer i5, const Standard_Integer i6, const Standard_Integer i7, const Standard_Integer i8, const Standard_Integer i9, const Standard_Integer i10, const Standard_Integer i11, const Standard_Integer i12, const Standard_Integer i13, const Standard_Integer i14, const Standard_Integer i15, const Standard_Integer i16, const Standard_Integer i17, const Standard_CString res1, const Standard_CString res2, const Standard_CString label, const Standard_CString subs);
  
  //! returns DirPart identified by record no (half Dsect number)
  Standard_EXPORT const IGESData_DirPart& DirPart (const Standard_Integer num) const;
  
  //! returns values recorded in directory part n0 <num>
  Standard_EXPORT void DirValues (const Standard_Integer num, Standard_Integer& i1, Standard_Integer& i2, Standard_Integer& i3, Standard_Integer& i4, Standard_Integer& i5, Standard_Integer& i6, Standard_Integer& i7, Standard_Integer& i8, Standard_Integer& i9, Standard_Integer& i10, Standard_Integer& i11, Standard_Integer& i12, Standard_Integer& i13, Standard_Integer& i14, Standard_Integer& i15, Standard_Integer& i16, Standard_Integer& i17, Standard_CString& res1, Standard_CString& res2, Standard_CString& label, Standard_CString& subs) const;
  
  //! returns "type" and "form" info from a directory part
  Standard_EXPORT IGESData_IGESType DirType (const Standard_Integer num) const;
  
  //! Returns count of recorded Entities (i.e. size of Directory)
  Standard_EXPORT virtual Standard_Integer NbEntities() const Standard_OVERRIDE;
  
  //! determines next suitable record from num; that is num+1 except
  //! for last one which gives 0
  Standard_EXPORT Standard_Integer FindNextRecord (const Standard_Integer num) const Standard_OVERRIDE;
  
  //! determines reference numbers in EntityNumber fields (called by
  //! SetEntities from IGESReaderTool)
  //! works on "Integer" type Parameters, because IGES does not
  //! distinguish Integer and Entity Refs : every Integer which is
  //! odd and less than twice NbRecords can be an Entity Ref ...
  //! (Ref Number is then (N+1)/2 if N is the Integer Value)
  Standard_EXPORT void SetEntityNumbers();
  
  //! Returns the recorded Global Check
  Standard_EXPORT Handle(Interface_Check) GlobalCheck() const;
  
  //! allows to set a default line weight, will be later applied at
  //! load time, on Entities which have no specified line weight
  Standard_EXPORT void SetDefaultLineWeight (const Standard_Real defw);
  
  //! Returns the recorded Default Line Weight, if there is
  //! (else, returns 0)
  Standard_EXPORT Standard_Real DefaultLineWeight() const;




  DEFINE_STANDARD_RTTIEXT(IGESData_IGESReaderData,Interface_FileReaderData)

protected:




private:


  IGESData_IGESType thectyp;
  Handle(TColStd_HSequenceOfHAsciiString) thestar;
  Handle(Interface_ParamSet) theparh;
  IGESData_GlobalSection thehead;
  IGESData_Array1OfDirPart thedirs;
  IGESData_ReadStage thestep;
  Standard_Real thedefw;
  Handle(Interface_Check) thechk;


};







#endif // _IGESData_IGESReaderData_HeaderFile
