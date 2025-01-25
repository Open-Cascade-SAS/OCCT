// Created on: 2009-05-05
// Created by: Sergey ZARITCHNY <sergey.zaritchny@opencascade.com>
// Copyright (c) 2009-2014 OPEN CASCADE SAS
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

#ifndef _DNaming_BooleanOperationDriver_HeaderFile
#define _DNaming_BooleanOperationDriver_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TFunction_Driver.hxx>
#include <Standard_Integer.hxx>
class TFunction_Logbook;
class TDF_Label;
class BRepAlgoAPI_BooleanOperation;
class TFunction_Function;

class DNaming_BooleanOperationDriver;
DEFINE_STANDARD_HANDLE(DNaming_BooleanOperationDriver, TFunction_Driver)

//! Driver  for Fuse,  Cut,  Common
class DNaming_BooleanOperationDriver : public TFunction_Driver
{

public:
  //! Constructor
  //! validation
  //! ==========
  Standard_EXPORT DNaming_BooleanOperationDriver();

  //! Validates labels of a function in <log>.
  //! In regeneration mode this method must be called (by the
  //! solver) even if the function is not executed, to build
  //! the valid label scope.
  //! execution of function
  //! ======================
  Standard_EXPORT virtual void Validate(Handle(TFunction_Logbook)& theLog) const Standard_OVERRIDE;

  //! Analyse in <log> if the loaded function must be executed
  //! (i.e.arguments are modified) or not.
  //! If the Function label itself is modified, the function must
  //! be executed.
  Standard_EXPORT virtual Standard_Boolean MustExecute(
    const Handle(TFunction_Logbook)& theLog) const Standard_OVERRIDE;

  //! Execute the function and push in <log> the impacted
  //! labels (see method SetImpacted).
  Standard_EXPORT virtual Standard_Integer Execute(Handle(TFunction_Logbook)& theLog) const
    Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(DNaming_BooleanOperationDriver, TFunction_Driver)

protected:
private:
  Standard_EXPORT void LoadNamingDS(const TDF_Label&              theResultLabel,
                                    BRepAlgoAPI_BooleanOperation& MS) const;

  Standard_EXPORT void LoadSectionNDS(const TDF_Label&              theResultLabel,
                                      BRepAlgoAPI_BooleanOperation& MS) const;

  Standard_EXPORT Standard_Boolean
    CheckAndLoad(BRepAlgoAPI_BooleanOperation&     theMkOpe,
                 const Handle(TFunction_Function)& theFunction) const;
};

#endif // _DNaming_BooleanOperationDriver_HeaderFile
