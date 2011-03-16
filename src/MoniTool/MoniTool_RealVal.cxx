#include <MoniTool_RealVal.ixx>

MoniTool_RealVal::MoniTool_RealVal  (const Standard_Real val)    {  theval = val;  }

Standard_Real  MoniTool_RealVal::Value () const  {  return theval;  }

Standard_Real&  MoniTool_RealVal::CValue ()  {  return theval;  }
