// Created on: 1998-09-21
// Created by: LECLERE Florence
// Copyright (c) 1998-1999 Matra Datavision
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

#ifdef OCCT_DEBUG

#include <Standard_Type.hxx>

static Standard_Boolean BRepFeat_traceFEAT = Standard_True;
Standard_EXPORT void BRepFeat_SettraceFEAT(const Standard_Boolean b) 
{ BRepFeat_traceFEAT = b; }
Standard_EXPORT Standard_Boolean BRepFeat_GettraceFEAT() 
{ return BRepFeat_traceFEAT; }

static Standard_Boolean BRepFeat_traceFEATFORM = Standard_False;
Standard_EXPORT void BRepFeat_SettraceFEATFORM(const Standard_Boolean b) 
{ BRepFeat_traceFEATFORM = b; }
Standard_EXPORT Standard_Boolean BRepFeat_GettraceFEATFORM() 
{ return BRepFeat_traceFEATFORM; }

static Standard_Boolean BRepFeat_traceFEATPRISM = Standard_False;
Standard_EXPORT void BRepFeat_SettraceFEATPRISM(const Standard_Boolean b) 
{ BRepFeat_traceFEATPRISM = b; }
Standard_EXPORT Standard_Boolean BRepFeat_GettraceFEATPRISM() 
{ return BRepFeat_traceFEATPRISM; }

static Standard_Boolean BRepFeat_traceFEATRIB = Standard_False;
Standard_EXPORT void BRepFeat_SettraceFEATRIB(const Standard_Boolean b) 
{ BRepFeat_traceFEATRIB = b; }
Standard_EXPORT Standard_Boolean BRepFeat_GettraceFEATRIB() 
{ return BRepFeat_traceFEATRIB; }

static Standard_Boolean BRepFeat_traceFEATDRAFT = Standard_False;
Standard_EXPORT void BRepFeat_SettraceFEATDRAFT(const Standard_Boolean b) 
{ BRepFeat_traceFEATDRAFT = b; }
Standard_EXPORT Standard_Boolean BRepFeat_GettraceFEATDRAFT() 
{ return BRepFeat_traceFEATDRAFT; }

static Standard_Boolean BRepFeat_contextCHRONO = Standard_False;
Standard_EXPORT void BRepFeat_SetcontextCHRONO(const Standard_Boolean b) 
{ BRepFeat_contextCHRONO = b; }
Standard_EXPORT Standard_Boolean BRepFeat_GetcontextCHRONO() {
  Standard_Boolean b = BRepFeat_contextCHRONO;
  if (b) cout<<"context (BRepFeat) CHRONO actif"<<endl;
  return b;
}

#endif


