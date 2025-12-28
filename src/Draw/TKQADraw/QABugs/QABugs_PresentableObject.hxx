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

class QABugs_PresentableObject : public AIS_InteractiveObject
{
  DEFINE_STANDARD_RTTIEXT(QABugs_PresentableObject, AIS_InteractiveObject)
public:
  Standard_EXPORT QABugs_PresentableObject(
    const PrsMgr_TypeOfPresentation3d aTypeOfPresentation3d = PrsMgr_TOP_AllView);

protected:
  Standard_EXPORT void ComputeSelection(const occ::handle<SelectMgr_Selection>& theSel,
                                                const int theMode) override;

  Standard_EXPORT void Compute(const occ::handle<PrsMgr_PresentationManager>& thePrsMgr,
                                       const occ::handle<Prs3d_Presentation>&         thePrs,
                                       const int theMode) override;
};

#endif // _QABugs_PresentableObject_HeaderFile
