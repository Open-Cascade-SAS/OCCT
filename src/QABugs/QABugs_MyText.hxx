// Created on: 2002-04-10
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

#ifndef _QABugs_MyText_HeaderFile
#define _QABugs_MyText_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Pnt.hxx>
#include <TCollection_ExtendedString.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Standard_CString.hxx>
#include <Standard_Real.hxx>
#include <AIS_InteractiveObject.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <Standard_Integer.hxx>
#include <SelectMgr_Selection.hxx>
class TCollection_ExtendedString;
class gp_Pnt;
class Prs3d_Presentation;


class QABugs_MyText;
DEFINE_STANDARD_HANDLE(QABugs_MyText, AIS_InteractiveObject)


class QABugs_MyText : public AIS_InteractiveObject
{

public:

  
  Standard_EXPORT QABugs_MyText(const TCollection_ExtendedString& aText, const gp_Pnt& aPosition);
  
  Standard_EXPORT QABugs_MyText(const TCollection_ExtendedString& aText, const gp_Pnt& aPosition, const Standard_CString aFont, const Quantity_NameOfColor aColor, const Standard_Real aHeight);




  DEFINE_STANDARD_RTTI(QABugs_MyText,AIS_InteractiveObject)

protected:

  
  Standard_EXPORT virtual void Compute (const Handle(PrsMgr_PresentationManager3d)& aPresentationManager, const Handle(Prs3d_Presentation)& aPresentation, const Standard_Integer aMode = 0) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void ComputeSelection (const Handle(SelectMgr_Selection)& aSelection, const Standard_Integer aMode) Standard_OVERRIDE;



private:


  gp_Pnt myPosition;
  TCollection_ExtendedString myText;
  Quantity_NameOfColor myNameOfColor;
  Standard_CString myNameOfFont;
  Standard_Real myHeight;


};







#endif // _QABugs_MyText_HeaderFile
