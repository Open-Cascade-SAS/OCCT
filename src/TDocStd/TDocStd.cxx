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

//=======================================================================
// Language:    C++	
// Version:     QDF	
// Design:     	
// Purpose:    	
//=======================================================================

// Declarations:	

#include <TDocStd.ixx>
#include <CDF.hxx>
//#include <LCTLOFF.h>
#include <OSD_Environment.hxx>

#include <TDocStd_XLink.hxx>



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

