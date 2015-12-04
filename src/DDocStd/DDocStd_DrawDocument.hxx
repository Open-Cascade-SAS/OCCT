// Created on: 2000-03-01
// Created by: Denis PASCAL
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

#ifndef _DDocStd_DrawDocument_HeaderFile
#define _DDocStd_DrawDocument_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <DDF_Data.hxx>
#include <Standard_OStream.hxx>
#include <Draw_Interpretor.hxx>
class TDocStd_Document;
class Draw_Display;
class Draw_Drawable3D;


class DDocStd_DrawDocument;
DEFINE_STANDARD_HANDLE(DDocStd_DrawDocument, DDF_Data)

//! draw variable for TDocStd_Document.
//! ==================================
class DDocStd_DrawDocument : public DDF_Data
{

public:

  
  Standard_EXPORT static Handle(DDocStd_DrawDocument) Find (const Handle(TDocStd_Document)& Doc);
  
  Standard_EXPORT DDocStd_DrawDocument(const Handle(TDocStd_Document)& Doc);
  
  Standard_EXPORT Handle(TDocStd_Document) GetDocument() const;
  
  Standard_EXPORT void DrawOn (Draw_Display& dis) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Handle(Draw_Drawable3D) Copy() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Dump (Standard_OStream& S) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Whatis (Draw_Interpretor& I) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(DDocStd_DrawDocument,DDF_Data)

protected:




private:


  Handle(TDocStd_Document) myDocument;


};







#endif // _DDocStd_DrawDocument_HeaderFile
