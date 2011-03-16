// File:	CDF.cxx
// Created:	Wed Mar 11 11:03:45 1998
// Author:	Jean-Louis Frenkel
//		<rmi@frilox.paris1.matra-dtv.fr>


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
