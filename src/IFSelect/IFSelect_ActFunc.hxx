// File:	IFSelect_ActFunc.hxx<2>
// Created:	Tue Feb 29 15:56:12 2000
// Author:	data exchange team
//		<det@kinox>

#ifndef _IFSelect_ActFunc_HeaderFile
#define _IFSelect_ActFunc_HeaderFile

#include <Standard_Type.hxx>
#include <Handle_IFSelect_SessionPilot.hxx>
#include <IFSelect_ReturnStatus.hxx>

typedef IFSelect_ReturnStatus (*IFSelect_ActFunc) (const Handle(IFSelect_SessionPilot)&);

Standard_EXPORT Handle(Standard_Type)& STANDARD_TYPE(IFSelect_ActFunc);

#endif
