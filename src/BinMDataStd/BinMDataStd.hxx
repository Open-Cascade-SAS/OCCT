// Created on: 2002-10-30
// Created by: Michael SAZONOV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _BinMDataStd_HeaderFile
#define _BinMDataStd_HeaderFile

#include <Standard_GUID.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <BinObjMgt_Persistent.hxx>

class BinMDF_ADriverTable;
class CDM_MessageDriver;
class BinMDataStd_NameDriver;
class BinMDataStd_IntegerDriver;
class BinMDataStd_RealDriver;
class BinMDataStd_IntegerArrayDriver;
class BinMDataStd_RealArrayDriver;
class BinMDataStd_UAttributeDriver;
class BinMDataStd_DirectoryDriver;
class BinMDataStd_CommentDriver;
class BinMDataStd_VariableDriver;
class BinMDataStd_ExpressionDriver;
class BinMDataStd_RelationDriver;
class BinMDataStd_NoteBookDriver;
class BinMDataStd_TreeNodeDriver;
class BinMDataStd_ExtStringArrayDriver;
class BinMDataStd_TickDriver;
class BinMDataStd_AsciiStringDriver;
class BinMDataStd_IntPackedMapDriver;
class BinMDataStd_IntegerListDriver;
class BinMDataStd_RealListDriver;
class BinMDataStd_ExtStringListDriver;
class BinMDataStd_BooleanListDriver;
class BinMDataStd_ReferenceListDriver;
class BinMDataStd_BooleanArrayDriver;
class BinMDataStd_ReferenceArrayDriver;
class BinMDataStd_ByteArrayDriver;
class BinMDataStd_NamedDataDriver;

//! Storage and Retrieval drivers for modelling attributes.
class BinMDataStd 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Adds the attribute drivers to <theDriverTable>.
  Standard_EXPORT static void AddDrivers (const Handle(BinMDF_ADriverTable)& theDriverTable, const Handle(CDM_MessageDriver)& aMsgDrv);
  
  Standard_EXPORT static void SetDocumentVersion (const Standard_Integer DocVersion);
  
  Standard_EXPORT static Standard_Integer DocumentVersion();

template<class T>
static void SetAttributeID(const BinObjMgt_Persistent& theSource, const Handle(T)& anAtt)
{
  Standard_Boolean ok = Standard_True;
  if(BinMDataStd::DocumentVersion() > 9) { // process user defined guid
    const Standard_Integer& aPos = theSource.Position();
    Standard_GUID aGuid;
    ok = theSource >> aGuid;
    if (!ok) {
      theSource.SetPosition(aPos);
      anAtt->SetID(T::GetID());
      ok = Standard_True;
    } else {
      anAtt->SetID(aGuid);
    }
  } else
    anAtt->SetID(T::GetID());
}


protected:





private:




friend class BinMDataStd_NameDriver;
friend class BinMDataStd_IntegerDriver;
friend class BinMDataStd_RealDriver;
friend class BinMDataStd_IntegerArrayDriver;
friend class BinMDataStd_RealArrayDriver;
friend class BinMDataStd_UAttributeDriver;
friend class BinMDataStd_DirectoryDriver;
friend class BinMDataStd_CommentDriver;
friend class BinMDataStd_VariableDriver;
friend class BinMDataStd_ExpressionDriver;
friend class BinMDataStd_RelationDriver;
friend class BinMDataStd_NoteBookDriver;
friend class BinMDataStd_TreeNodeDriver;
friend class BinMDataStd_ExtStringArrayDriver;
friend class BinMDataStd_TickDriver;
friend class BinMDataStd_AsciiStringDriver;
friend class BinMDataStd_IntPackedMapDriver;
friend class BinMDataStd_IntegerListDriver;
friend class BinMDataStd_RealListDriver;
friend class BinMDataStd_ExtStringListDriver;
friend class BinMDataStd_BooleanListDriver;
friend class BinMDataStd_ReferenceListDriver;
friend class BinMDataStd_BooleanArrayDriver;
friend class BinMDataStd_ReferenceArrayDriver;
friend class BinMDataStd_ByteArrayDriver;
friend class BinMDataStd_NamedDataDriver;

};







#endif // _BinMDataStd_HeaderFile
