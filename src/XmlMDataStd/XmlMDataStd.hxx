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

#ifndef _XmlMDataStd_HeaderFile
#define _XmlMDataStd_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
class XmlMDF_ADriverTable;
class Message_Messenger;
class XmlMDataStd_GenericExtStringDriver;
class XmlMDataStd_IntegerDriver;
class XmlMDataStd_RealDriver;
class XmlMDataStd_IntegerArrayDriver;
class XmlMDataStd_RealArrayDriver;
class XmlMDataStd_ExtStringArrayDriver;
class XmlMDataStd_UAttributeDriver;
class XmlMDataStd_VariableDriver;
class XmlMDataStd_ExpressionDriver;
class XmlMDataStd_RelationDriver;
class XmlMDataStd_TreeNodeDriver;
class XmlMDataStd_GenericEmptyDriver;
class XmlMDataStd_IntegerListDriver;
class XmlMDataStd_RealListDriver;
class XmlMDataStd_ExtStringListDriver;
class XmlMDataStd_BooleanListDriver;
class XmlMDataStd_ReferenceListDriver;
class XmlMDataStd_BooleanArrayDriver;
class XmlMDataStd_ReferenceArrayDriver;
class XmlMDataStd_ByteArrayDriver;
class XmlMDataStd_NamedDataDriver;
class XmlMDataStd_AsciiStringDriver;
class XmlMDataStd_IntPackedMapDriver;


//! Storage and Retrieval drivers for modelling attributes.
//! Transient attributes are defined in package TDataStd.
class XmlMDataStd 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Adds the attribute drivers to <aDriverTable>.
  Standard_EXPORT static void AddDrivers (const Handle(XmlMDF_ADriverTable)& aDriverTable, const Handle(Message_Messenger)& anMsgDrv);


protected:





private:




friend class XmlMDataStd_GenericExtStringDriver;
friend class XmlMDataStd_IntegerDriver;
friend class XmlMDataStd_RealDriver;
friend class XmlMDataStd_IntegerArrayDriver;
friend class XmlMDataStd_RealArrayDriver;
friend class XmlMDataStd_ExtStringArrayDriver;
friend class XmlMDataStd_UAttributeDriver;
friend class XmlMDataStd_VariableDriver;
friend class XmlMDataStd_ExpressionDriver;
friend class XmlMDataStd_RelationDriver;
friend class XmlMDataStd_TreeNodeDriver;
friend class XmlMDataStd_GenericEmptyDriver;
friend class XmlMDataStd_IntegerListDriver;
friend class XmlMDataStd_RealListDriver;
friend class XmlMDataStd_ExtStringListDriver;
friend class XmlMDataStd_BooleanListDriver;
friend class XmlMDataStd_ReferenceListDriver;
friend class XmlMDataStd_BooleanArrayDriver;
friend class XmlMDataStd_ReferenceArrayDriver;
friend class XmlMDataStd_ByteArrayDriver;
friend class XmlMDataStd_NamedDataDriver;
friend class XmlMDataStd_AsciiStringDriver;
friend class XmlMDataStd_IntPackedMapDriver;

};







#endif // _XmlMDataStd_HeaderFile
