// Created on: 2002-04-09
// Created by: QA Admin
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

#ifndef _QABugs_PresentableObject_HeaderFile
#define _QABugs_PresentableObject_HeaderFile

#include <AIS_InteractiveObject.hxx>

DEFINE_STANDARD_HANDLE(QABugs_PresentableObject, AIS_InteractiveObject)

class QABugs_PresentableObject : public AIS_InteractiveObject
{
  DEFINE_STANDARD_RTTIEXT(QABugs_PresentableObject, AIS_InteractiveObject)
public:
  Standard_EXPORT QABugs_PresentableObject(
    const PrsMgr_TypeOfPresentation3d aTypeOfPresentation3d = PrsMgr_TOP_AllView);

protected:
  Standard_EXPORT virtual void ComputeSelection(const Handle(SelectMgr_Selection)& theSel,
                                                const Standard_Integer theMode) Standard_OVERRIDE;

  Standard_EXPORT virtual void Compute(const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                       const Handle(Prs3d_Presentation)&         thePrs,
                                       const Standard_Integer theMode) Standard_OVERRIDE;
};

#endif // _QABugs_PresentableObject_HeaderFile
