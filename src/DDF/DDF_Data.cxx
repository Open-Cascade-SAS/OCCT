// File:	DDF_Data.cxx
//      	------------
// Author:	VAUTHIER Jean-Claude & DAUTRY Philippe
// Copyright:	Matra Datavision 1997

// Version:	0.0
// History:	Version	Date		Purpose
//		0.0	Feb 10 1997	Creation



#include <DDF_Data.ixx>
#include <DDF.hxx>

#include <Standard_GUID.hxx>

#include <TCollection_AsciiString.hxx>

#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_IDFilter.hxx>
#include <TDF_Tool.hxx>



//=======================================================================
//function : DDF_Data
//purpose  : 
//=======================================================================

DDF_Data::DDF_Data(const Handle(TDF_Data)& aDF) : myDF (aDF) {}



//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void DDF_Data::DrawOn(Draw_Display& dis) const

{ cout<<"DDF_Data"<<endl; }



//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Draw_Drawable3D) DDF_Data::Copy() const { return new DDF_Data (myDF); }



//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void DDF_Data::Dump(Standard_OStream& S) const

{
  TDF_Tool::DeepDump(S,myDF);
}


//=======================================================================
//function : DataFramework
//purpose  : 
//=======================================================================

Handle(TDF_Data) DDF_Data::DataFramework () const { return myDF; }




//=======================================================================
//function : DataFramework
//purpose  : 
//=======================================================================

void DDF_Data::DataFramework (const Handle(TDF_Data)& aDF) 

{ myDF = aDF; }




//=======================================================================
//function : Whatis
//purpose  : 
//=======================================================================

void DDF_Data::Whatis (Draw_Interpretor& I) const

{
  I << "Data Framework";
}
