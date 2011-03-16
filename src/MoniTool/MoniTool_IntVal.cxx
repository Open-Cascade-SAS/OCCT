#include <MoniTool_IntVal.ixx>

MoniTool_IntVal::MoniTool_IntVal  (const Standard_Integer val)    {  theval = val;  }

Standard_Integer  MoniTool_IntVal::Value () const  {  return theval;  }

Standard_Integer&  MoniTool_IntVal::CValue ()  {  return theval;  }
