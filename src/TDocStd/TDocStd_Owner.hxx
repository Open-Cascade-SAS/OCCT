// Created on: 1999-07-12
// Created by: Denis PASCAL
// Copyright (c) 1999 Matra Datavision
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

#ifndef _TDocStd_Owner_HeaderFile
#define _TDocStd_Owner_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TDF_Attribute.hxx>
#include <Standard_OStream.hxx>
class TDocStd_Document;
class Standard_GUID;
class TDF_Data;
class TDF_RelocationTable;


class TDocStd_Owner;
DEFINE_STANDARD_HANDLE(TDocStd_Owner, TDF_Attribute)

//! This  attribute located  at  the  root label  of the
//! framework contains  a   back reference to   the  owner
//! TDocStd_Document, providing access to the document from
//! any label.  private class Owner;
class TDocStd_Owner : public TDF_Attribute
{

public:

  
  //! class methods
  //! =============
  Standard_EXPORT static const Standard_GUID& GetID();
  
  Standard_EXPORT static void SetDocument (const Handle(TDF_Data)& indata, const Handle(TDocStd_Document)& doc);

  Standard_EXPORT static void SetDocument (const Handle(TDF_Data)& indata, TDocStd_Document* doc);

  //! Owner methods
  //! ===============
  Standard_EXPORT static Handle(TDocStd_Document) GetDocument (const Handle(TDF_Data)& ofdata);
  
  Standard_EXPORT TDocStd_Owner();
  
  Standard_EXPORT void SetDocument (const Handle(TDocStd_Document)& document);

  Standard_EXPORT void SetDocument (TDocStd_Document* document);

  Standard_EXPORT Handle(TDocStd_Document) GetDocument() const;
  
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& With) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& Into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_OStream& Dump (Standard_OStream& anOS) const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(TDocStd_Owner,TDF_Attribute)

protected:




private:

  //! It keeps pointer to the document to avoid handles cyclic dependency
  TDocStd_Document* myDocument;


};







#endif // _TDocStd_Owner_HeaderFile
