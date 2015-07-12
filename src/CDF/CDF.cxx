// Created on: 1998-03-11
// Created by: Jean-Louis Frenkel
// Copyright (c) 1998-1999 Matra Datavision
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


#include <CDF.hxx>
#include <Standard_Failure.hxx>

static void CDF_InitApplication () {

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
