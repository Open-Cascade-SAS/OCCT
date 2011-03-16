// File:	PDocStd_XLink.cxx
//      	--------------
// Author:	DAUTRY Philippe
//		<fid@fox.paris1.matra-dtv.fr>
// Copyright:	Matra Datavision 1997

// Version:	0.0
// History:	Version	Date		Purpose
//		0.0	Sep 17 1997	Creation



#include <PDocStd_XLink.ixx>


//=======================================================================
//function : PDocStd_XLink
//purpose  : 
//=======================================================================

PDocStd_XLink::PDocStd_XLink()
{}


//=======================================================================
//function : DocumentEntry
//purpose  : 
//=======================================================================

void PDocStd_XLink::DocumentEntry
(const Handle(PCollection_HAsciiString)& aDocEntry) 
{ myDocEntry = aDocEntry; }


//=======================================================================
//function : DocumentEntry
//purpose  : 
//=======================================================================

Handle(PCollection_HAsciiString) PDocStd_XLink::DocumentEntry() const
{ return myDocEntry; }


//=======================================================================
//function : LabelEntry
//purpose  : 
//=======================================================================

void PDocStd_XLink::LabelEntry
(const Handle(PCollection_HAsciiString)& aLabEntry) 
{ myLabEntry = aLabEntry; }


//=======================================================================
//function : LabelEntry
//purpose  : 
//=======================================================================

Handle(PCollection_HAsciiString) PDocStd_XLink::LabelEntry() const
{ return myLabEntry; }

