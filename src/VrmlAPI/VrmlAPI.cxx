// File:	VrmlAPI.cxx
// Created:	Tue May 30 15:52:16 2000
// Author:	Sergey MOZOKHIN
//		<smh@russox.nnov.matra-dtv.fr>


#include <VrmlAPI.ixx>
#include <VrmlAPI_Writer.hxx>
#include <TopoDS_Shape.hxx>

void VrmlAPI::Write(const TopoDS_Shape& aShape, const Standard_CString aFileName)  {
  VrmlAPI_Writer writer;
  writer.Write(aShape, aFileName);
}
