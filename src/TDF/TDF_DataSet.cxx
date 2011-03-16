// File:	TDF_DataSet.cxx
//      	---------------
// Author:	DAUTRY Philippe
//		<fid@fox.paris1.matra-dtv.fr>
// Copyright:	Matra Datavision 1997

// Version:	0.0
// History:	Version	Date		Purpose
//		0.0	Mar 11 1997	Creation



#include <TDF_DataSet.ixx>

#include <TDF_MapIteratorOfAttributeMap.hxx>
#include <TDF_ListIteratorOfLabelList.hxx>
#include <TDF_MapIteratorOfLabelMap.hxx>


//=======================================================================
//function : TDF_DataSet
//purpose  : Creates a DataSet.
//=======================================================================

TDF_DataSet::TDF_DataSet()
{}


//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void TDF_DataSet::Clear()
{
  myRootLabels.Clear();
  myLabelMap.Clear();
  myAttributeMap.Clear();
}


//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDF_DataSet::Dump(Standard_OStream& anOS) const
{
  anOS<<"\t\t=====< TDF_DataSet dump >====="<<endl;
  anOS<<"Root Labels :"<<endl<<"============="<<endl;
  for (TDF_ListIteratorOfLabelList itr1(myRootLabels);
       itr1.More(); itr1.Next()) {
    itr1.Value().EntryDump(anOS);
    anOS<<" | ";
  }
  anOS<<endl<<"Labels :"<<endl<<"========"<<endl;
  for (TDF_MapIteratorOfLabelMap itr2(myLabelMap);
       itr2.More(); itr2.Next()) {
    itr2.Key().EntryDump(anOS);
    anOS<<" | ";
  }
  anOS<<endl<<"Attributes :"<<endl<<"============"<<endl<<endl;
  for (TDF_MapIteratorOfAttributeMap itr3(myAttributeMap);
       itr3.More(); itr3.Next()) {
    itr3.Key()->Label().EntryDump(anOS);
    anOS<<" \t";
    itr3.Key()->Dump(anOS);
    anOS<<endl;
  }
  anOS<<endl;
  return anOS;
}
