// File:	ShapeProcess_OperFunc.hxx
// Created:	Tue Feb 29 15:28:37 2000
// Author:	data exchange team
//		<det@kinox>


#ifndef ShapeProcess_OperFunc_HeaderFile
#define ShapeProcess_OperFunc_HeaderFile

#include <Standard_Type.hxx>
#include <Handle_ShapeProcess_Context.hxx>

typedef Standard_Boolean (*ShapeProcess_OperFunc) (const Handle(ShapeProcess_Context)& context);

//Standard_EXPORT Handle(Standard_Type)& STANDARD_TYPE(ShapeProcess_OperFunc);

#endif
