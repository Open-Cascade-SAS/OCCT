#include <IFSelect.ixx>
#include <IFSelect_SessionFile.hxx>


//  Methodes de confort, evitant de devoir connaitre SessionFile, qui est un
//  Tool non destine a l export (en particulier, pas un Handle)


    Standard_Boolean  IFSelect::SaveSession
  (const Handle(IFSelect_WorkSession)& WS, const Standard_CString file)
{
  IFSelect_SessionFile sesfile(WS,file);
  return sesfile.IsDone();
}

    Standard_Boolean  IFSelect::RestoreSession
  (const Handle(IFSelect_WorkSession)& WS, const Standard_CString file)
{
  IFSelect_SessionFile sesfile(WS);
  return (sesfile.Read(file) == 0);
}
