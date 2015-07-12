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

#ifndef _QANewBRepNaming_Limitation_HeaderFile
#define _QANewBRepNaming_Limitation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <QANewBRepNaming_BooleanOperationFeat.hxx>
class TDF_Label;
class QANewModTopOpe_Limitation;



class QANewBRepNaming_Limitation  : public QANewBRepNaming_BooleanOperationFeat
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT QANewBRepNaming_Limitation();
  
  Standard_EXPORT QANewBRepNaming_Limitation(const TDF_Label& ResultLabel);
  
  Standard_EXPORT void Load (QANewModTopOpe_Limitation& MakeShape) const;




protected:

  
  //! Loads the content of the result.
  Standard_EXPORT void LoadContent (QANewModTopOpe_Limitation& MakeShape) const;
  
  //! Loads the result.
  Standard_EXPORT void LoadResult (QANewModTopOpe_Limitation& MakeShape) const;
  
  //! Loads the deletion of the degenerated edges.
  Standard_EXPORT void LoadDegenerated (QANewModTopOpe_Limitation& MakeShape) const;
  
  //! A default implementation for naming of a wire as an object of
  //! a boolean operation.
  Standard_EXPORT void LoadWire (QANewModTopOpe_Limitation& MakeShape) const;
  
  //! A default implementation for naming of a shell as an object of
  //! a boolean operation.
  Standard_EXPORT void LoadShell (QANewModTopOpe_Limitation& MakeShape) const;




private:





};







#endif // _QANewBRepNaming_Limitation_HeaderFile
