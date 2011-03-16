// File:	TDF_AttributeDelta.cxx
//      	----------------------
// Author:	DAUTRY Philippe
//		<fid@fox.paris1.matra-dtv.fr>
// Copyright:	Matra Datavision 1997

// Version:	0.0
// History:	Version	Date		Purpose
//		0.0	Sep  8 1997	Creation



#include <TDF_AttributeDelta.ixx>

#include <Standard_GUID.hxx>

#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>

#include <TCollection_AsciiString.hxx>

//=======================================================================
//function : TDF_AttributeDelta
//purpose  : 
//=======================================================================

TDF_AttributeDelta::TDF_AttributeDelta
(const Handle(TDF_Attribute)& anAttribute)
: myAttribute(anAttribute),
  myLabel(anAttribute->Label())
{}


//=======================================================================
//function : Label
//purpose  : 
//=======================================================================

TDF_Label TDF_AttributeDelta::Label() const
{ return myLabel; }


//=======================================================================
//function : Attribute
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TDF_AttributeDelta::Attribute() const
{ return myAttribute; }


//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

Standard_GUID TDF_AttributeDelta::ID() const
{ return myAttribute->ID(); }


//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDF_AttributeDelta::Dump(Standard_OStream& OS) const
{
  static TCollection_AsciiString entry;
  TDF_Tool::Entry(Label(),entry);
  OS<<this->DynamicType()->Name()<<" at "<<entry;
  OS<<" on "<<Attribute()->DynamicType()->Name();
  return OS;
}
