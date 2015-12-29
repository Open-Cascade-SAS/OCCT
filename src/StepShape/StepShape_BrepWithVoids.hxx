// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _StepShape_BrepWithVoids_HeaderFile
#define _StepShape_BrepWithVoids_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepShape_HArray1OfOrientedClosedShell.hxx>
#include <StepShape_ManifoldSolidBrep.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class StepShape_ClosedShell;
class StepShape_OrientedClosedShell;


class StepShape_BrepWithVoids;
DEFINE_STANDARD_HANDLE(StepShape_BrepWithVoids, StepShape_ManifoldSolidBrep)


class StepShape_BrepWithVoids : public StepShape_ManifoldSolidBrep
{

public:

  
  //! Returns a BrepWithVoids
  Standard_EXPORT StepShape_BrepWithVoids();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(StepShape_ClosedShell)& aOuter, const Handle(StepShape_HArray1OfOrientedClosedShell)& aVoids);
  
  Standard_EXPORT void SetVoids (const Handle(StepShape_HArray1OfOrientedClosedShell)& aVoids);
  
  Standard_EXPORT Handle(StepShape_HArray1OfOrientedClosedShell) Voids() const;
  
  Standard_EXPORT Handle(StepShape_OrientedClosedShell) VoidsValue (const Standard_Integer num) const;
  
  Standard_EXPORT Standard_Integer NbVoids() const;




  DEFINE_STANDARD_RTTIEXT(StepShape_BrepWithVoids,StepShape_ManifoldSolidBrep)

protected:




private:


  Handle(StepShape_HArray1OfOrientedClosedShell) voids;


};







#endif // _StepShape_BrepWithVoids_HeaderFile
