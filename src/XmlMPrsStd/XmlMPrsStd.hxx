// Created on: 2001-07-09
// Created by: Julia DOROVSKIKH
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef _XmlMPrsStd_HeaderFile
#define _XmlMPrsStd_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class XmlMDF_ADriverTable;
class CDM_MessageDriver;
class XmlMPrsStd_PositionDriver;
class XmlMPrsStd_AISPresentationDriver;



class XmlMPrsStd 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Adds the attribute storage drivers to <aDriverTable>.
  Standard_EXPORT static void AddDrivers (const Handle(XmlMDF_ADriverTable)& aDriverTable, const Handle(CDM_MessageDriver)& theMessageDriver);




protected:





private:




friend class XmlMPrsStd_PositionDriver;
friend class XmlMPrsStd_AISPresentationDriver;

};







#endif // _XmlMPrsStd_HeaderFile
