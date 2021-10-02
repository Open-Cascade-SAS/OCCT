// Created on: 2002-10-30
// Created by: Michael SAZONOV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _BinMDataXtd_HeaderFile
#define _BinMDataXtd_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
class BinMDF_ADriverTable;
class Message_Messenger;

//! Storage and Retrieval drivers for modelling attributes.
class BinMDataXtd 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Adds the attribute drivers to <theDriverTable>.
  Standard_EXPORT static void AddDrivers (const Handle(BinMDF_ADriverTable)& theDriverTable, const Handle(Message_Messenger)& aMsgDrv);
  
  Standard_EXPORT static void SetDocumentVersion (const Standard_Integer DocVersion);
  
  Standard_EXPORT static Standard_Integer DocumentVersion();




protected:





private:




friend class BinMDataXtd_GeometryDriver;
friend class BinMDataXtd_ConstraintDriver;
friend class BinMDataXtd_PatternStdDriver;
friend class BinMDataXtd_TriangulationDriver;

};







#endif // _BinMDataXtd_HeaderFile
