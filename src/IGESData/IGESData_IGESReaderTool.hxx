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

#ifndef _IGESData_IGESReaderTool_HeaderFile
#define _IGESData_IGESReaderTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Interface_GeneralLib.hxx>
#include <Interface_ReaderLib.hxx>
#include <Standard_Integer.hxx>
#include <IGESData_IGESType.hxx>
#include <IGESData_ReadStage.hxx>
#include <Standard_Real.hxx>
#include <Interface_FileReaderTool.hxx>
#include <Standard_Boolean.hxx>
class Interface_ParamList;
class IGESData_FileRecognizer;
class Interface_Check;
class IGESData_IGESReaderData;
class IGESData_Protocol;
class Standard_Transient;
class Interface_InterfaceModel;
class IGESData_IGESEntity;
class IGESData_DirPart;
class IGESData_ParamReader;


//! specific FileReaderTool for IGES
//! Parameters are accessed through specific objects, ParamReaders
class IGESData_IGESReaderTool  : public Interface_FileReaderTool
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! creates IGESReaderTool to work with an IGESReaderData and an
  //! IGES Protocol.
  //! Actually, no Lib is used
  Standard_EXPORT IGESData_IGESReaderTool(const Handle(IGESData_IGESReaderData)& reader, const Handle(IGESData_Protocol)& protocol);
  
  //! binds empty entities to records, works with the Protocol
  //! (from IGESData) stored and later used
  //! RQ : Actually, sets DNum into IGES Entities
  //! Also loads the list of parameters for ParamReader
  Standard_EXPORT void Prepare (const Handle(IGESData_FileRecognizer)& reco);
  
  //! recognizes records by asking Protocol (on data of DirType)
  Standard_EXPORT Standard_Boolean Recognize (const Standard_Integer num, Handle(Interface_Check)& ach, Handle(Standard_Transient)& ent) Standard_OVERRIDE;
  
  //! fills model's header, that is, its GlobalSection
  Standard_EXPORT void BeginRead (const Handle(Interface_InterfaceModel)& amodel) Standard_OVERRIDE;
  
  //! fills an entity, given record no; works by calling ReadDirPart
  //! then ReadParams (with help of a ParamReader), then if required
  //! ReadProps and ReadAssocs, from IGESEntity
  //! Returns True if no fail has been recorded
  Standard_EXPORT Standard_Boolean AnalyseRecord (const Standard_Integer num, const Handle(Standard_Transient)& anent, Handle(Interface_Check)& acheck) Standard_OVERRIDE;
  
  //! after reading entities, true line weights can be computed
  Standard_EXPORT virtual void EndRead (const Handle(Interface_InterfaceModel)& amodel) Standard_OVERRIDE;
  
  //! Reads directory part componants from file; DP is the litteral
  //! directory part, IR detains entities referenced by DP
  Standard_EXPORT void ReadDir (const Handle(IGESData_IGESEntity)& ent, const Handle(IGESData_IGESReaderData)& IR, const IGESData_DirPart& DP, Handle(Interface_Check)& ach) const;
  
  //! Performs Reading of own Parameters for each IGESEntity
  //! Works with the ReaderLib loaded with ReadWriteModules for IGES
  //! In case of failure, tries UndefinedEntity from IGES
  Standard_EXPORT void ReadOwnParams (const Handle(IGESData_IGESEntity)& ent, const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const;
  
  //! Reads Property List, if there is (if not, does nothing)
  //! criterium is : current parameter of PR remains inside params
  //! list, and Stage is "Own"
  //! Current parameter must be a positive integer, which value
  //! gives the length of the list; else, a Fail is produced (into
  //! Check of PR) and reading process is stopped
  Standard_EXPORT void ReadProps (const Handle(IGESData_IGESEntity)& ent, const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const;
  
  //! Reads Associativity List, if there is (if not, does nothing)
  //! criterium is : current parameter of PR remains inside params
  //! list, and Stage is "Own"
  //! Same conditions as above; in addition, no parameter must be
  //! let after the list once read
  //! Note that "Associated" entities are not declared "Shared"
  Standard_EXPORT void ReadAssocs (const Handle(IGESData_IGESEntity)& ent, const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const;




protected:





private:



  Handle(Interface_ParamList) thelist;
  Handle(IGESData_FileRecognizer) thereco;
  Interface_GeneralLib theglib;
  Interface_ReaderLib therlib;
  Standard_Integer thecnum;
  IGESData_IGESType thectyp;
  IGESData_ReadStage thestep;
  Handle(Interface_Check) thechk;
  Standard_Integer thegradweight;
  Standard_Real themaxweight;
  Standard_Real thedefweight;


};







#endif // _IGESData_IGESReaderTool_HeaderFile
