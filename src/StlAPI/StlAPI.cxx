#include <StlAPI.ixx>
#include <StlAPI_Writer.hxx>
#include <StlAPI_Reader.hxx>

void StlAPI::Write(const TopoDS_Shape& aShape,
		   const Standard_CString aFile,
		   const Standard_Boolean aAsciiMode) 
{
  StlAPI_Writer writer;
  writer.ASCIIMode() = aAsciiMode;
  writer.Write (aShape, aFile);
}


void StlAPI::Read(TopoDS_Shape& aShape,const Standard_CString aFile)
{
  StlAPI_Reader reader;
  reader.Read (aShape, aFile);
}
