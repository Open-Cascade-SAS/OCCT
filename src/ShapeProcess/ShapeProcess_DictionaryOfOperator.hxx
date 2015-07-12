// Created on: 2000-08-21
// Created by: Andrey BETENEV
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

#ifndef _ShapeProcess_DictionaryOfOperator_HeaderFile
#define _ShapeProcess_DictionaryOfOperator_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Character.hxx>
#include <MMgt_TShared.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
#include <Standard_Size.hxx>
#include <Standard_Integer.hxx>
class ShapeProcess_Operator;
class Standard_NoSuchObject;
class ShapeProcess_IteratorOfDictionaryOfOperator;
class ShapeProcess_StackItemOfDictionaryOfOperator;
class TCollection_AsciiString;


class ShapeProcess_DictionaryOfOperator;
DEFINE_STANDARD_HANDLE(ShapeProcess_DictionaryOfOperator, MMgt_TShared)


class ShapeProcess_DictionaryOfOperator : public MMgt_TShared
{

public:

  
  Standard_EXPORT ShapeProcess_DictionaryOfOperator();
  
  Standard_EXPORT Standard_Boolean HasItem (const Standard_CString name, const Standard_Boolean exact = Standard_False) const;
  
  Standard_EXPORT Standard_Boolean HasItem (const TCollection_AsciiString& name, const Standard_Boolean exact = Standard_True) const;
  
  Standard_EXPORT const Handle(ShapeProcess_Operator)& Item (const Standard_CString name, const Standard_Boolean exact = Standard_True) const;
  
  Standard_EXPORT const Handle(ShapeProcess_Operator)& Item (const TCollection_AsciiString& name, const Standard_Boolean exact = Standard_True) const;
  
  Standard_EXPORT Standard_Boolean GetItem (const Standard_CString name, Handle(ShapeProcess_Operator)& anitem, const Standard_Boolean exact = Standard_True) const;
  
  Standard_EXPORT Standard_Boolean GetItem (const TCollection_AsciiString& name, Handle(ShapeProcess_Operator)& anitem, const Standard_Boolean exact = Standard_True) const;
  
  Standard_EXPORT void SetItem (const Standard_CString name, const Handle(ShapeProcess_Operator)& anitem, const Standard_Boolean exact = Standard_True);
  
  Standard_EXPORT void SetItem (const TCollection_AsciiString& name, const Handle(ShapeProcess_Operator)& anitem, const Standard_Boolean exact = Standard_True);
  
  Standard_EXPORT Handle(ShapeProcess_Operator)& NewItem (const Standard_CString name, Standard_Boolean& isvalued, const Standard_Boolean exact = Standard_True);
  
  Standard_EXPORT Handle(ShapeProcess_Operator)& NewItem (const TCollection_AsciiString& name, Standard_Boolean& isvalued, const Standard_Boolean exact = Standard_True);
  
  Standard_EXPORT Standard_Boolean RemoveItem (const Standard_CString name, const Standard_Boolean cln = Standard_True, const Standard_Boolean exact = Standard_True);
  
  Standard_EXPORT Standard_Boolean RemoveItem (const TCollection_AsciiString& name, const Standard_Boolean cln = Standard_True, const Standard_Boolean exact = Standard_True);
  
  Standard_EXPORT void Clean();
  
  Standard_EXPORT Standard_Boolean IsEmpty() const;
  
  Standard_EXPORT void Clear();
  
  Standard_EXPORT Handle(ShapeProcess_DictionaryOfOperator) Copy() const;
  
  Standard_EXPORT Standard_Boolean Complete (Handle(ShapeProcess_DictionaryOfOperator)& acell) const;


friend class ShapeProcess_IteratorOfDictionaryOfOperator;


  DEFINE_STANDARD_RTTI(ShapeProcess_DictionaryOfOperator,MMgt_TShared)

protected:




private:

  
  Standard_EXPORT void SetChar (const Standard_Character car);
  
  Standard_EXPORT Standard_Boolean HasSub() const;
  
  Standard_EXPORT Handle(ShapeProcess_DictionaryOfOperator) Sub() const;
  
  Standard_EXPORT Standard_Boolean HasNext() const;
  
  Standard_EXPORT Handle(ShapeProcess_DictionaryOfOperator) Next() const;
  
  Standard_EXPORT void SetSub (const Handle(ShapeProcess_DictionaryOfOperator)& acell);
  
  Standard_EXPORT void SetNext (const Handle(ShapeProcess_DictionaryOfOperator)& acell);
  
  Standard_EXPORT void SearchCell (const Standard_CString name, const Standard_Size lmax, const Standard_Character car, const Standard_Size level, Handle(ShapeProcess_DictionaryOfOperator)& acell, Standard_Size& reslev, Standard_Integer& stat) const;
  
  Standard_EXPORT void NewCell (const Standard_CString name, const Standard_Size namlen, Handle(ShapeProcess_DictionaryOfOperator)& acell, const Standard_Size reslev, const Standard_Integer stat);
  
  Standard_EXPORT Standard_Boolean HasIt() const;
  
  Standard_EXPORT const Handle(ShapeProcess_Operator)& It() const;
  
  Standard_EXPORT Handle(ShapeProcess_Operator)& ItAdr();
  
  Standard_EXPORT void SetIt (const Handle(ShapeProcess_Operator)& anitem);
  
  Standard_EXPORT void DeclIt();
  
  Standard_EXPORT void RemoveIt();
  
  Standard_EXPORT Standard_Character CellChar() const;
  
  Standard_EXPORT void GetCopied (const Handle(ShapeProcess_DictionaryOfOperator)& fromcell);

  Standard_Character thecars[4];
  Handle(ShapeProcess_DictionaryOfOperator) thesub;
  Handle(ShapeProcess_DictionaryOfOperator) thenext;
  Handle(ShapeProcess_Operator) theitem;


};







#endif // _ShapeProcess_DictionaryOfOperator_HeaderFile
