// Created on: 1997-01-08
// Created by: VAUTHIER Jean-Claude
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

#ifndef _QADNaming_HeaderFile
#define _QADNaming_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_CString.hxx>
#include <TopTools_ListOfShape.hxx>
#include <Standard_Integer.hxx>
#include <Draw_Interpretor.hxx>
class TopoDS_Shape;
class TDF_Data;
class TCollection_AsciiString;
class TDF_Label;



class QADNaming 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static TopoDS_Shape CurrentShape (const Standard_CString ShapeEntry, const Handle(TDF_Data)& Data);
  
  Standard_EXPORT static void GetShape (const Standard_CString ShapeEntry, const Handle(TDF_Data)& Data, TopTools_ListOfShape& Shapes);
  
  //! theStatus = 0  Not  found,
  //! theStatus = 1  One  shape,
  //! theStatus = 2  More than one shape.
  Standard_EXPORT static TCollection_AsciiString GetEntry (const TopoDS_Shape& Shape, const Handle(TDF_Data)& Data, Standard_Integer& theStatus);
  
  //! returns label by first two arguments (df and entry string)
  Standard_EXPORT static Standard_Boolean Entry (const Standard_Address theArguments, TDF_Label& theLabel);
  
  Standard_EXPORT static void AllCommands (Draw_Interpretor& DI);
  
  //! commands relatives to NamedShape
  Standard_EXPORT static void BasicCommands (Draw_Interpretor& DI);
  
  //! loading NamedShape to the Data Framework
  Standard_EXPORT static void BuilderCommands (Draw_Interpretor& DI);
  
  //! loading NamedShape to the Data Framework
  Standard_EXPORT static void IteratorsCommands (Draw_Interpretor& DI);
  
  Standard_EXPORT static void ToolsCommands (Draw_Interpretor& DI);
  
  //! commands relatives to Naming
  Standard_EXPORT static void SelectionCommands (Draw_Interpretor& DI);




protected:





private:





};







#endif // _QADNaming_HeaderFile
