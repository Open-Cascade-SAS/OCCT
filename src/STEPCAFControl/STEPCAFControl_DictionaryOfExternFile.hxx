// Created on: 2000-04-09
// Created by: Sergey MOZOKHIN
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _STEPCAFControl_DictionaryOfExternFile_HeaderFile
#define _STEPCAFControl_DictionaryOfExternFile_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Character.hxx>
#include <MMgt_TShared.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
#include <Standard_Size.hxx>
#include <Standard_Integer.hxx>
class STEPCAFControl_ExternFile;
class Standard_NoSuchObject;
class STEPCAFControl_IteratorOfDictionaryOfExternFile;
class STEPCAFControl_StackItemOfDictionaryOfExternFile;
class TCollection_AsciiString;


class STEPCAFControl_DictionaryOfExternFile;
DEFINE_STANDARD_HANDLE(STEPCAFControl_DictionaryOfExternFile, MMgt_TShared)


class STEPCAFControl_DictionaryOfExternFile : public MMgt_TShared
{

public:

  
  Standard_EXPORT STEPCAFControl_DictionaryOfExternFile();
  
  Standard_EXPORT Standard_Boolean HasItem (const Standard_CString name, const Standard_Boolean exact = Standard_False) const;
  
  Standard_EXPORT Standard_Boolean HasItem (const TCollection_AsciiString& name, const Standard_Boolean exact = Standard_True) const;
  
  Standard_EXPORT const Handle(STEPCAFControl_ExternFile)& Item (const Standard_CString name, const Standard_Boolean exact = Standard_True) const;
  
  Standard_EXPORT const Handle(STEPCAFControl_ExternFile)& Item (const TCollection_AsciiString& name, const Standard_Boolean exact = Standard_True) const;
  
  Standard_EXPORT Standard_Boolean GetItem (const Standard_CString name, Handle(STEPCAFControl_ExternFile)& anitem, const Standard_Boolean exact = Standard_True) const;
  
  Standard_EXPORT Standard_Boolean GetItem (const TCollection_AsciiString& name, Handle(STEPCAFControl_ExternFile)& anitem, const Standard_Boolean exact = Standard_True) const;
  
  Standard_EXPORT void SetItem (const Standard_CString name, const Handle(STEPCAFControl_ExternFile)& anitem, const Standard_Boolean exact = Standard_True);
  
  Standard_EXPORT void SetItem (const TCollection_AsciiString& name, const Handle(STEPCAFControl_ExternFile)& anitem, const Standard_Boolean exact = Standard_True);
  
  Standard_EXPORT Handle(STEPCAFControl_ExternFile)& NewItem (const Standard_CString name, Standard_Boolean& isvalued, const Standard_Boolean exact = Standard_True);
  
  Standard_EXPORT Handle(STEPCAFControl_ExternFile)& NewItem (const TCollection_AsciiString& name, Standard_Boolean& isvalued, const Standard_Boolean exact = Standard_True);
  
  Standard_EXPORT Standard_Boolean RemoveItem (const Standard_CString name, const Standard_Boolean cln = Standard_True, const Standard_Boolean exact = Standard_True);
  
  Standard_EXPORT Standard_Boolean RemoveItem (const TCollection_AsciiString& name, const Standard_Boolean cln = Standard_True, const Standard_Boolean exact = Standard_True);
  
  Standard_EXPORT void Clean();
  
  Standard_EXPORT Standard_Boolean IsEmpty() const;
  
  Standard_EXPORT void Clear();
  
  Standard_EXPORT Handle(STEPCAFControl_DictionaryOfExternFile) Copy() const;
  
  Standard_EXPORT Standard_Boolean Complete (Handle(STEPCAFControl_DictionaryOfExternFile)& acell) const;


friend class STEPCAFControl_IteratorOfDictionaryOfExternFile;


  DEFINE_STANDARD_RTTI(STEPCAFControl_DictionaryOfExternFile,MMgt_TShared)

protected:




private:

  
  Standard_EXPORT void SetChar (const Standard_Character car);
  
  Standard_EXPORT Standard_Boolean HasSub() const;
  
  Standard_EXPORT Handle(STEPCAFControl_DictionaryOfExternFile) Sub() const;
  
  Standard_EXPORT Standard_Boolean HasNext() const;
  
  Standard_EXPORT Handle(STEPCAFControl_DictionaryOfExternFile) Next() const;
  
  Standard_EXPORT void SetSub (const Handle(STEPCAFControl_DictionaryOfExternFile)& acell);
  
  Standard_EXPORT void SetNext (const Handle(STEPCAFControl_DictionaryOfExternFile)& acell);
  
  Standard_EXPORT void SearchCell (const Standard_CString name, const Standard_Size lmax, const Standard_Character car, const Standard_Size level, Handle(STEPCAFControl_DictionaryOfExternFile)& acell, Standard_Size& reslev, Standard_Integer& stat) const;
  
  Standard_EXPORT void NewCell (const Standard_CString name, const Standard_Size namlen, Handle(STEPCAFControl_DictionaryOfExternFile)& acell, const Standard_Size reslev, const Standard_Integer stat);
  
  Standard_EXPORT Standard_Boolean HasIt() const;
  
  Standard_EXPORT const Handle(STEPCAFControl_ExternFile)& It() const;
  
  Standard_EXPORT Handle(STEPCAFControl_ExternFile)& ItAdr();
  
  Standard_EXPORT void SetIt (const Handle(STEPCAFControl_ExternFile)& anitem);
  
  Standard_EXPORT void DeclIt();
  
  Standard_EXPORT void RemoveIt();
  
  Standard_EXPORT Standard_Character CellChar() const;
  
  Standard_EXPORT void GetCopied (const Handle(STEPCAFControl_DictionaryOfExternFile)& fromcell);

  Standard_Character thecars[4];
  Handle(STEPCAFControl_DictionaryOfExternFile) thesub;
  Handle(STEPCAFControl_DictionaryOfExternFile) thenext;
  Handle(STEPCAFControl_ExternFile) theitem;


};







#endif // _STEPCAFControl_DictionaryOfExternFile_HeaderFile
