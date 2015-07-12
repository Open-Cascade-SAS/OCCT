// Created on: 2001-05-03
// Created by: Igor FEOKTISTOV
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef _QANewModTopOpe_Limitation_HeaderFile
#define _QANewModTopOpe_Limitation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <QANewModTopOpe_CutPtr.hxx>
#include <QANewModTopOpe_CommonPtr.hxx>
#include <Standard_Boolean.hxx>
#include <QANewModTopOpe_ModeOfLimitation.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <TopTools_ListOfShape.hxx>
class TopoDS_Shape;


//! provides  cutting  shape  by  face  or  shell;
class QANewModTopOpe_Limitation  : public BRepBuilderAPI_MakeShape
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! initializes and  fills data structure for  cutting and
  //! makes  cutting according to orientation theCutTool and
  //! theMode.
  //! if theCutTool is not face or shell does nothing.
  Standard_EXPORT QANewModTopOpe_Limitation(const TopoDS_Shape& theObjectToCut, const TopoDS_Shape& theCutTool, const QANewModTopOpe_ModeOfLimitation theMode = QANewModTopOpe_Forward);
  
  //! makes cutting  according to  orientation theCutTool
  //! and  current value   of  myMode.  Does nothing  if
  //! result already  exists.
  Standard_EXPORT void Cut();
  
  Standard_EXPORT void SetMode (const QANewModTopOpe_ModeOfLimitation theMode);
  
  Standard_EXPORT QANewModTopOpe_ModeOfLimitation GetMode() const;
  
  //! Returns the first shape.
  Standard_EXPORT const TopoDS_Shape& Shape1() const;
  
  //! Returns the second shape.
  Standard_EXPORT const TopoDS_Shape& Shape2() const;
  
  //! Returns the list  of shapes modified from the shape
  //! <S>.
  Standard_EXPORT virtual const TopTools_ListOfShape& Modified (const TopoDS_Shape& S) Standard_OVERRIDE;
  
  //! Returns the list  of shapes generated from the shape <S>.
  //! For use in BRepNaming.
  Standard_EXPORT virtual const TopTools_ListOfShape& Generated (const TopoDS_Shape& S) Standard_OVERRIDE;
  
  //! Returns true if there is at least one modified shape.
  //! For use in BRepNaming.
  Standard_EXPORT virtual Standard_Boolean HasModified() const;
  
  //! Returns true if there is at least one generated shape.
  //! For use in BRepNaming.
  Standard_EXPORT virtual Standard_Boolean HasGenerated() const;
  
  //! Returns true if there is at least one deleted shape.
  //! For use in BRepNaming.
  Standard_EXPORT virtual Standard_Boolean HasDeleted() const;
  
  Standard_EXPORT virtual Standard_Boolean IsDeleted (const TopoDS_Shape& S) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Delete() Standard_OVERRIDE;
Standard_EXPORT  ~QANewModTopOpe_Limitation()  {Delete();}




protected:





private:



  TopoDS_Shape myResultFwd;
  TopoDS_Shape myResultRvs;
  TopoDS_Shape myObjectToCut;
  TopoDS_Shape myCutTool;
  QANewModTopOpe_CutPtr myCut;
  QANewModTopOpe_CommonPtr myCommon;
  Standard_Boolean myFwdIsDone;
  Standard_Boolean myRevIsDone;
  QANewModTopOpe_ModeOfLimitation myMode;


};







#endif // _QANewModTopOpe_Limitation_HeaderFile
