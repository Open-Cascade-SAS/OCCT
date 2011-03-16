// File:	TDataStd.cxx
// Created:	Wed Jul 30 15:57:49 1997
// Author:	Denis PASCAL
//		<dp@dingox.paris1.matra-dtv.fr>


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

