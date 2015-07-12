// Created on: 1997-07-30
// Created by: Denis PASCAL
// Copyright (c) 1997-1999 Matra Datavision
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


#include <Standard_GUID.hxx>
#include <TDataStd.hxx>
#include <TDataStd_ExtStringArray.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_AttributeList.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_DataSet.hxx>
#include <TDF_ListIteratorOfAttributeList.hxx>
#include <TDF_ListIteratorOfLabelList.hxx>
#include <TDF_MapIteratorOfLabelMap.hxx>
#include <TDF_Reference.hxx>
#include <TDF_TagSource.hxx>
#include <TDF_Tool.hxx>

//=======================================================================
//function : IDList
//purpose  : 
//=======================================================================
void TDataStd::IDList(TDF_IDList& anIDList)
{  
  anIDList.Append(TDF_TagSource::GetID());  
  anIDList.Append(TDF_Reference::GetID());
  anIDList.Append(TDataStd_Integer::GetID()); 
  anIDList.Append(TDataStd_Name::GetID());  
  anIDList.Append(TDataStd_Real::GetID());  
  anIDList.Append(TDataStd_IntegerArray::GetID());
  anIDList.Append(TDataStd_RealArray::GetID());
  anIDList.Append(TDataStd_ExtStringArray::GetID());
  
}


//=======================================================================
//function : 
//purpose  : print the name of the real dimension
//=======================================================================

Standard_OStream& TDataStd::Print(const TDataStd_RealEnum C,  Standard_OStream& s)
{
  switch (C) {
  case TDataStd_SCALAR :
    {
      s << "SCALAR";  break;
    }
  case  TDataStd_LENGTH :
    {  
      s << "LENGTH"; break;
    }  
  case TDataStd_ANGULAR :
    { 
      s << "ANGULAR"; break;
    }
    default :
      {
	s << "UNKNOWN"; break;
      }
  }
  return s;
}

