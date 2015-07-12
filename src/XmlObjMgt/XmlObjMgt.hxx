// Created on: 2001-07-17
// Created by: Julia DOROVSKIKH <jfa@hotdox.nnov.matra-dtv.fr>
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef _XmlObjMgt_HeaderFile
#define _XmlObjMgt_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <XmlObjMgt_DOMString.hxx>
#include <Standard_Boolean.hxx>
#include <XmlObjMgt_Element.hxx>
#include <Standard_Integer.hxx>
#include <Standard_CString.hxx>
#include <Standard_Real.hxx>
class TCollection_ExtendedString;
class TCollection_AsciiString;
class XmlObjMgt_Persistent;
class XmlObjMgt_GP;
class XmlObjMgt_Array1;


//! This package defines services to manage the storage
//! grain of data produced by applications and those classes
//! to manage persistent extern reference.
class XmlObjMgt 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Define the name of XMLattribute 'ID' (to be used everywhere)
  Standard_EXPORT static const XmlObjMgt_DOMString& IdString();
  
  //! Add attribute <theElement extstring="theString" ...>
  Standard_EXPORT static Standard_Boolean SetExtendedString (XmlObjMgt_Element& theElement, const TCollection_ExtendedString& theString);
  
  //! Get attribute <theElement extstring="theString" ...>
  Standard_EXPORT static Standard_Boolean GetExtendedString (const XmlObjMgt_Element& theElement, TCollection_ExtendedString& theString);
  
  //! Returns the first child text node
  Standard_EXPORT static XmlObjMgt_DOMString GetStringValue (const XmlObjMgt_Element& theElement);
  
  //! Add theData as the last child text node to theElement
  //! isClearText(True) avoids analysis of the string and replacement
  //! of characters like '<' and '&' during XML file storage.
  //! Do NEVER set isClearText unless you have a hell of a reason
  Standard_EXPORT static void SetStringValue (XmlObjMgt_Element& theElement, const XmlObjMgt_DOMString& theData, const Standard_Boolean isClearText = Standard_False);
  
  //! Convert XPath expression (DOMString) into TagEntry string
  //! returns False on Error
  Standard_EXPORT static Standard_Boolean GetTagEntryString (const XmlObjMgt_DOMString& theTarget, TCollection_AsciiString& theTagEntry);
  
  //! Convert XPath expression (DOMString) into TagEntry string
  //! returns False on Error
  Standard_EXPORT static void SetTagEntryString (XmlObjMgt_DOMString& theSource, const TCollection_AsciiString& theTagEntry);
  
  Standard_EXPORT static XmlObjMgt_Element FindChildElement (const XmlObjMgt_Element& theSource, const Standard_Integer theObjId);
  
  Standard_EXPORT static XmlObjMgt_Element FindChildByRef (const XmlObjMgt_Element& theSource, const XmlObjMgt_DOMString& theRefName);
  
  Standard_EXPORT static XmlObjMgt_Element FindChildByName (const XmlObjMgt_Element& theSource, const XmlObjMgt_DOMString& theName);
  
  Standard_EXPORT static Standard_Boolean GetInteger (Standard_CString& theString, Standard_Integer& theValue);
  
  Standard_EXPORT static Standard_Boolean GetReal (Standard_CString& theString, Standard_Real& theValue);
  
  Standard_EXPORT static Standard_Boolean GetReal (const XmlObjMgt_DOMString& theString, Standard_Real& theValue);




protected:





private:




friend class XmlObjMgt_Persistent;
friend class XmlObjMgt_GP;
friend class XmlObjMgt_Array1;

};







#endif // _XmlObjMgt_HeaderFile
