#include <StepData_DescrProtocol.ixx>
#include <StepData_DescrGeneral.hxx>
#include <StepData_DescrReadWrite.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_ReaderLib.hxx>
#include <StepData_WriterLib.hxx>


    StepData_DescrProtocol::StepData_DescrProtocol ()    {  }

    void  StepData_DescrProtocol::SetSchemaName (const Standard_CString name)
      {  thename.Clear();  thename.AssignCat (name);  }

    void  StepData_DescrProtocol::LibRecord () const
{
  if (!HasDescr()) return;  // rien a recorder dans la lib ?
  Handle(StepData_DescrGeneral)   gen = new StepData_DescrGeneral   (this);
  Handle(StepData_DescrReadWrite) rwm = new StepData_DescrReadWrite (this);
  Interface_GeneralLib::SetGlobal (gen,this);
  Interface_ReaderLib::SetGlobal   (rwm,this);
  StepData_WriterLib::SetGlobal   (rwm,this);
}

    Standard_CString  StepData_DescrProtocol::SchemaName () const
      {  return thename.ToCString();  }
