// Created on: 2000-10-31
// Created by: Vladislav ROMASHKO
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

#ifndef _QANewBRepNaming_Fuse_HeaderFile
#define _QANewBRepNaming_Fuse_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <QANewBRepNaming_BooleanOperationFeat.hxx>
class TDF_Label;
class BRepAlgoAPI_BooleanOperation;



class QANewBRepNaming_Fuse  : public QANewBRepNaming_BooleanOperationFeat
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT QANewBRepNaming_Fuse();
  
  Standard_EXPORT QANewBRepNaming_Fuse(const TDF_Label& ResultLabel);
  
  Standard_EXPORT void Load (BRepAlgoAPI_BooleanOperation& MakeShape) const;




protected:





private:





};







#endif // _QANewBRepNaming_Fuse_HeaderFile
