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
