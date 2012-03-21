// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <Interface_TypedValue.ixx>

#include <Dico_DictionaryOfInteger.hxx>
#include <Dico_IteratorOfDictionaryOfInteger.hxx>

#include <OSD_Path.hxx>
#include <stdio.h>


//static  char defmess[30]; svv #2


//  Fonctions Satisfies offertes en standard ...

// svv#2
//static Standard_Boolean StaticPath(const Handle(TCollection_HAsciiString)& val)
//{
//  OSD_Path apath;
//  return apath.IsValid (TCollection_AsciiString(val->ToCString()));
//}


Interface_TypedValue::Interface_TypedValue
  (const Standard_CString name,
   const Interface_ParamType type, const Standard_CString init)
    : MoniTool_TypedValue (name,
			   Interface_TypedValue::ParamTypeToValueType(type),
			   init)    {  }

    Interface_ParamType  Interface_TypedValue::Type () const
      {  return ValueTypeToParamType (ValueType());  }

    MoniTool_ValueType  Interface_TypedValue::ParamTypeToValueType
  (const Interface_ParamType type)
      {  return (MoniTool_ValueType) type;  }  // meme valeurs

    Interface_ParamType  Interface_TypedValue::ValueTypeToParamType
  (const MoniTool_ValueType type)
      {  return (Interface_ParamType) type;  }  // meme valeurs
