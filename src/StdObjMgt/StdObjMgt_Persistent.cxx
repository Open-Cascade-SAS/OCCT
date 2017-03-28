// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <StdObjMgt_Persistent.hxx>

StdObjMgt_Persistent::StdObjMgt_Persistent()
  : myTypeNum(0)
  , myRefNum(0)
{

}

//=======================================================================
//function : ImportDocument
//purpose  : Import transient document from the persistent data
//           (to be overriden by document class;
//           does nothing by default for other classes).
//=======================================================================
void StdObjMgt_Persistent::ImportDocument
  (const Handle(TDocStd_Document)&) const {}

//=======================================================================
//function : CreateAttribute
//purpose  : Create an empty transient attribuite
//           (to be overriden by attribute classes;
//           does nothing and returns a null handle by default for other classes)
//=======================================================================
Handle(TDF_Attribute) StdObjMgt_Persistent::CreateAttribute()
  { return Handle(TDF_Attribute)(); }

//=======================================================================
//function : GetAttribute
//purpose  : Get transient attribuite for the persistent data
//           (to be overriden by attribute classes;
//           returns a null handle by default for non-attribute classes)
//=======================================================================
Handle(TDF_Attribute) StdObjMgt_Persistent::GetAttribute() const
  { return Handle(TDF_Attribute)(); }

//=======================================================================
//function : ImportAttribute
//purpose  : Import transient attribuite from the persistent data
//           (to be overriden by attribute classes;
//           does nothing by default for non-attribute classes)
//=======================================================================
void StdObjMgt_Persistent::ImportAttribute() {}

//=======================================================================
//function : AsciiString
//purpose  : Get referenced ASCII string
//           (to be overriden by ASCII string class;
//           returns a null handle by default for other classes)
//=======================================================================
Handle(TCollection_HAsciiString) StdObjMgt_Persistent::AsciiString() const
  { return Handle(TCollection_HAsciiString)(); }

//=======================================================================
//function : ExtString
//purpose  : Get referenced extended string
//           (to be overriden by extended string class;
//           returns a null handle by default for other classes)
//=======================================================================
Handle(TCollection_HExtendedString) StdObjMgt_Persistent::ExtString() const
  { return Handle(TCollection_HExtendedString)(); }

//=======================================================================
//function : Label
//purpose  : Get a label expressed by referenced extended string
//           (to be overriden by extended string class;
//           returns a null label by default for other classes)
//=======================================================================
TDF_Label StdObjMgt_Persistent::Label (const Handle(TDF_Data)&) const
  { return TDF_Label(); }
