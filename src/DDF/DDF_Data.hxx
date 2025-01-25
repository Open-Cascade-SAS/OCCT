// Created by: DAUTRY Philippe
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _DDF_Data_HeaderFile
#define _DDF_Data_HeaderFile

#include <Standard.hxx>

#include <Draw_Drawable3D.hxx>
#include <Standard_OStream.hxx>
#include <Draw_Interpretor.hxx>
class TDF_Data;
class Draw_Display;

class DDF_Data;
DEFINE_STANDARD_HANDLE(DDF_Data, Draw_Drawable3D)

//! Encapsulates a data framework from TDF in a drawable object
class DDF_Data : public Draw_Drawable3D
{

public:
  Standard_EXPORT DDF_Data(const Handle(TDF_Data)& aDF);

  Standard_EXPORT void DrawOn(Draw_Display& dis) const Standard_OVERRIDE;

  Standard_EXPORT virtual Handle(Draw_Drawable3D) Copy() const Standard_OVERRIDE;

  Standard_EXPORT virtual void Dump(Standard_OStream& S) const Standard_OVERRIDE;

  Standard_EXPORT void DataFramework(const Handle(TDF_Data)& aDF);

  Standard_EXPORT Handle(TDF_Data) DataFramework() const;

  Standard_EXPORT virtual void Whatis(Draw_Interpretor& I) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(DDF_Data, Draw_Drawable3D)

protected:
private:
  Handle(TDF_Data) myDF;
};

#endif // _DDF_Data_HeaderFile
