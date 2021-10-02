// Created on: 2000-05-18
// Created by: Peter KURNEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _BOPTest_HeaderFile
#define _BOPTest_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <BOPAlgo_Operation.hxx>

#include <Draw_Interpretor.hxx>
class Message_Report;

class BOPTest 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static void AllCommands (Draw_Interpretor& aDI);
  
  Standard_EXPORT static void BOPCommands (Draw_Interpretor& aDI);
  
  Standard_EXPORT static void CheckCommands (Draw_Interpretor& aDI);
  
  Standard_EXPORT static void TolerCommands (Draw_Interpretor& aDI);
  
  Standard_EXPORT static void LowCommands (Draw_Interpretor& aDI);
  
  Standard_EXPORT static void ObjCommands (Draw_Interpretor& aDI);
  
  Standard_EXPORT static void PartitionCommands (Draw_Interpretor& aDI);
  
  Standard_EXPORT static void APICommands (Draw_Interpretor& aDI);
  
  Standard_EXPORT static void OptionCommands (Draw_Interpretor& aDI);
  
  Standard_EXPORT static void Factory (Draw_Interpretor& aDI);

  Standard_EXPORT static void DebugCommands  (Draw_Interpretor& aDI);

  Standard_EXPORT static void CellsCommands  (Draw_Interpretor& aDI);
  
  Standard_EXPORT static void UtilityCommands (Draw_Interpretor& aDI);

  Standard_EXPORT static void RemoveFeaturesCommands (Draw_Interpretor& aDI);

  Standard_EXPORT static void PeriodicityCommands (Draw_Interpretor& aDI);

  Standard_EXPORT static void MkConnectedCommands (Draw_Interpretor& aDI);

  //! Prints errors and warnings if any and draws attached shapes 
  //! if flag BOPTest_Objects::DrawWarnShapes() is set
  Standard_EXPORT static void ReportAlerts (const Handle(Message_Report)& theReport);

  //! Returns operation type according to the given string.
  //! For numeric values, the number correspond to the order in enum.
  Standard_EXPORT static BOPAlgo_Operation GetOperationType(const Standard_CString theOp);

protected:

private:

friend class BOPTest_Objects;
friend class BOPTest_DrawableShape;

};

#endif // _BOPTest_HeaderFile
