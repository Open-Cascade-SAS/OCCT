// Created on: 1999-08-19
// Created by: Sergey RUIN
// Copyright (c) 1999-1999 Matra Datavision
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

#include <DDataStd.hxx>
#include <DDF.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Appli.hxx>
#include <DrawTrSurf.hxx>

#include <DDF.hxx>

#include <TDF_Data.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>
#include <TDF_AttributeSequence.hxx>
#include <TDF_AttributeList.hxx>
#include <TDF_ListIteratorOfAttributeList.hxx>

// ATTRIBUTES

#include <TDataStd.hxx>
#include <TDataStd_Name.hxx>

#include <TCollection_AsciiString.hxx>
#include <TDataStd_ListOfExtendedString.hxx>




//=======================================================================
//function : DDataStd_SetName
//purpose  : SetName (DF, entry, name [,guid])
//=======================================================================

static Standard_Integer DDataStd_SetName (Draw_Interpretor& di,
					  Standard_Integer nb, 
					  const char** arg) 
{

  if (nb == 4 || nb == 5) {     
    Handle(TDF_Data) DF;
    if (!DDF::GetDF(arg[1],DF)) return 1;
    TDF_Label L;
    DDF::AddLabel(DF, arg[2], L);
    if(L.IsNull()) di << "Label is not found"   << "\n";
	if(nb == 4) 
      TDataStd_Name::Set(L,TCollection_ExtendedString(arg[3],Standard_True)); 
	else {	
	  if (!Standard_GUID::CheckGUIDFormat(arg[4])) {
        di<<"DDataStd_SetReal: The format of GUID is invalid\n";
        return 1;
	  }
	  Standard_GUID guid(arg[4]);
	  TDataStd_Name::Set(L, guid, TCollection_ExtendedString(arg[3],Standard_True)); 
     }
    return 0;
  }
  di << "DDataStd_SetName : Error\n";
  return 1;
}

//#define DEB_DDataStd
//=======================================================================
//function : DDataStd_GetName
//purpose  : GetName (DF, entry [,guid])
//=======================================================================

static Standard_Integer DDataStd_GetName (Draw_Interpretor& di,
					  Standard_Integer nb, 
					  const char** arg) 
{   
  if (nb == 3 || nb == 4) {    
    Handle(TDF_Data) DF;
    if (!DDF::GetDF(arg[1],DF)) return 1; 
    TDF_Label L;
    DDF::FindLabel(DF, arg[2], L);
    if(L.IsNull()) di << "Label is not found"   << "\n";
	Standard_GUID aGuid (TDataStd_Name::GetID());
	if(nb == 4) {
      if (!Standard_GUID::CheckGUIDFormat(arg[3])) {
        di<<"DDataStd_GetAsciiString: The format of GUID is invalid\n";
        return 1;
	  }
	  aGuid = Standard_GUID(arg[3]);
	}
	Handle(TDataStd_Name) N;	  
	if( !L.FindAttribute(aGuid, N) ) {
      cout << "Name attribute is not found or not set"  << endl;
	  return 1;
	}
#ifdef DEB_DDataStd
	if(!N.IsNull()) 
      cout << "String = " << TCollection_AsciiString(N->Get(), '?').ToCString()  << endl;
#endif
    TCollection_AsciiString s(N->Get(),'?');
    di << s.ToCString();
    return 0;
  }
  di << "DDataStd_SetName : Error\n";
  return 1;
}




//=======================================================================
//function : SetCommands
//purpose  : 
//=======================================================================

void DDataStd::NameCommands (Draw_Interpretor& theCommands)
{  

  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;

  const char* g = "DDataStd : Name attribute commands";

  theCommands.Add ("SetName", 
                   "SetName (DF, entry, name [,guid])",
		   __FILE__, DDataStd_SetName, g);   

  theCommands.Add ("GetName", 
                   "GetNmae (DF, entry [,guid])",
                    __FILE__, DDataStd_GetName, g);  


}

