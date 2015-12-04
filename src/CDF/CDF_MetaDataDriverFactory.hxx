// Created on: 1997-11-17
// Created by: Jean-Louis Frenkel
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

#ifndef _CDF_MetaDataDriverFactory_HeaderFile
#define _CDF_MetaDataDriverFactory_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class CDF_MetaDataDriver;


class CDF_MetaDataDriverFactory;
DEFINE_STANDARD_HANDLE(CDF_MetaDataDriverFactory, Standard_Transient)


class CDF_MetaDataDriverFactory : public Standard_Transient
{

public:

  
  Standard_EXPORT virtual Handle(CDF_MetaDataDriver) Build() const = 0;




  DEFINE_STANDARD_RTTIEXT(CDF_MetaDataDriverFactory,Standard_Transient)

protected:




private:




};







#endif // _CDF_MetaDataDriverFactory_HeaderFile
