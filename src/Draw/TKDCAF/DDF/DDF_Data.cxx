// Created by: VAUTHIER Jean-Claude & DAUTRY Philippe
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

//      	------------
// Version:	0.0
// Version	Date		Purpose
//		0.0	Feb 10 1997	Creation

#include <DDF.hxx>
#include <DDF_Data.hxx>
#include <Draw_Display.hxx>
#include <Draw_Drawable3D.hxx>
#include <Standard_Type.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_Data.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DDF_Data, Draw_Drawable3D)

//=================================================================================================

DDF_Data::DDF_Data(const Handle(TDF_Data)& aDF)
    : myDF(aDF)
{
}

//=================================================================================================

void DDF_Data::DrawOn(Draw_Display& /*dis*/) const

{
  std::cout << "DDF_Data" << std::endl;
}

//=================================================================================================

Handle(Draw_Drawable3D) DDF_Data::Copy() const
{
  return new DDF_Data(myDF);
}

//=================================================================================================

void DDF_Data::Dump(Standard_OStream& S) const

{
  TDF_Tool::DeepDump(S, myDF);
}

//=================================================================================================

Handle(TDF_Data) DDF_Data::DataFramework() const
{
  return myDF;
}

//=================================================================================================

void DDF_Data::DataFramework(const Handle(TDF_Data)& aDF)

{
  myDF = aDF;
}

//=================================================================================================

void DDF_Data::Whatis(Draw_Interpretor& I) const

{
  I << "Data Framework";
}
