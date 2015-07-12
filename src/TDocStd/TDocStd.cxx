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

//=======================================================================
// Language:    C++	
// Version:     QDF	
// Design:     	
// Purpose:    	
//=======================================================================
// Declarations:	

#include <CDF.hxx>
#include <OSD_Environment.hxx>
#include <TDocStd.hxx>
#include <TDocStd_XLink.hxx>

//#include <LCTLOFF.h>
//=======================================================================
//function : Application
//purpose  : 
//=======================================================================
// Handle(TDocStd_Application) TDocStd::Application
// (const Standard_Boolean UseDocAPI)
// {
//   static Handle(TDocStd_Application) theAppli;
//   if (theAppli.IsNull()) theAppli = new TDocStd_Application(UseDocAPI);
//   OSD_Environment CSFLicense("CSF_EngineName");
//   TCollection_AsciiString LicenseAscii = CSFLicense .Value();
//   Standard_Boolean HasLicense = Standard_False;
//   if (!LicenseAscii.IsEmpty()) {
//     if (LicenseAscii=="DsgPEngine")
//       HasLicense = Standard_True;
//   }
//   if (HasLicense) {
//     CDF::GetLicense(AED100) ; 
//     CDF::GetLicense(AED300) ; 
//   }
//   return theAppli;
// }
//=======================================================================
//function : InitApplication
//purpose  : 
//=======================================================================
// Handle(TDocStd_Application) TDocStd::InitApplication
// (const Standard_Boolean UseDocAPI)
// {
//   // No init yet.
//   return TDocStd::Application(UseDocAPI);
// }
//=======================================================================
//function : IDList
//purpose  : 
//=======================================================================
void TDocStd::IDList(TDF_IDList& anIDList)
{ anIDList.Append(TDocStd_XLink::GetID()); }

