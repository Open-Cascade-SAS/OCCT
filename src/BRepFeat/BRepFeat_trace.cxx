// File:	BRepFeat_trace.cxx
// Created:	Mon Sep 21 17:16:39 1998
// Author:	LECLERE Florence
//		<flo@partox.paris1.matra-dtv.fr>


#ifdef DEB

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


