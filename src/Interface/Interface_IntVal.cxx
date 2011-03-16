#include <Interface_IntVal.ixx>

Interface_IntVal::Interface_IntVal  ()    {  theval = 0;  }

Standard_Integer  Interface_IntVal::Value () const  {  return theval;  }

Standard_Integer&  Interface_IntVal::CValue ()  {  return theval;  }
