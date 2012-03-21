// Created on: 1997-07-30
// Created by: Denis PASCAL
// Copyright (c) 1997-1999 Matra Datavision
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



#include <TDataStd.ixx>

#include <TDF_AttributeIterator.hxx>
#include <TDF_AttributeList.hxx>
#include <TDF_Tool.hxx>
#include <TDF_TagSource.hxx>
#include <TDF_Reference.hxx>
#include <TDF_ListIteratorOfAttributeList.hxx>
#include <TDF_ListIteratorOfLabelList.hxx>
#include <TDF_MapIteratorOfLabelMap.hxx>
#include <TDF_DataSet.hxx>  
#include <TDF_ChildIterator.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDataStd_ExtStringArray.hxx>

#include <Standard_GUID.hxx>



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

