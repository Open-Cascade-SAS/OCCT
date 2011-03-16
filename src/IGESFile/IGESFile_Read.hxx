//  Include a utiliser pour appeler IGESFile_Read


#ifndef IGESFile_Read_HeaderFile
#define IGESFile_Read_HeaderFile

#include <IGESData_IGESModel.hxx>
#include <IGESData_Protocol.hxx>
#include <IGESData_FileRecognizer.hxx>


Standard_EXPORT Standard_Integer IGESFile_Read
  (char* nomfic,
   const Handle(IGESData_IGESModel)& amodel,
   const Handle(IGESData_Protocol)& protocol);

Standard_EXPORT Standard_Integer IGESFile_ReadFNES
  (char* nomfic,
   const Handle(IGESData_IGESModel)& amodel,
   const Handle(IGESData_Protocol)& protocol);

Standard_EXPORT Standard_Integer IGESFile_Read
  (char* nomfic,
   const Handle(IGESData_IGESModel)& amodel,
   const Handle(IGESData_Protocol)& protocol,
   const Handle(IGESData_FileRecognizer)& reco,
   const Standard_Boolean modefnes = Standard_False);

#endif
