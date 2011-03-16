//=======================================================================
// File:       	TDocStd.cxx
// Language:    C++	
// Version:     QDF	
// Design:     	
// Purpose:    	
// Copyright:   Matra-Datavision	1997
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

