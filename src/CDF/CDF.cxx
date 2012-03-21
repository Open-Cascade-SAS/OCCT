// Created on: 1998-03-11
// Created by: Jean-Louis Frenkel
// Copyright (c) 1998-1999 Matra Datavision
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



#include <CDF.ixx>
#include <Standard_Failure.hxx>

// Unused :
#ifdef DEB
static void CDF_CheckStatus(int LicenseStatus) {

  if (LicenseStatus != 0)  {
      
    switch (LicenseStatus) {
	
    case 1:   Standard_Failure::Raise("LICENSE_unauthorized"); break;
    case 2:   Standard_Failure::Raise("LICENSE_wrong_data"); break;
    case 3:   Standard_Failure::Raise("LICENSE_max_users"); break;
    case 4:   Standard_Failure::Raise("LICENSE_unspecified"); break;
    case 5:   Standard_Failure::Raise("LICENSE_pb_init"); break;
    case 6:   Standard_Failure::Raise("LICENSE_unspecified"); break;
    case 7:   Standard_Failure::Raise("LICENSE_cantopenfile"); break;
    case 8:   Standard_Failure::Raise("LICENSE_connexion"); break;
    case 9:   Standard_Failure::Raise("LICENSE_syntaxe"); break;
    default:  Standard_Failure::Raise("LICENSE_unspecified"); break;
    }
  }
}
#endif
void static CDF_InitApplication () {

  static Standard_Boolean FirstApplication = Standard_True;

  if(FirstApplication) {
    FirstApplication = Standard_False;
  }
}
void CDF::GetLicense(const Standard_Integer ){

  CDF_InitApplication();
  
}

Standard_Boolean CDF::IsAvailable(const Standard_Integer ) {

  CDF_InitApplication();
  return Standard_True;
}
