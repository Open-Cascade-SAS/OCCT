#include <StepBasic_DocumentType.ixx>

StepBasic_DocumentType::StepBasic_DocumentType  ()    {  }

void  StepBasic_DocumentType::Init (const Handle(TCollection_HAsciiString)& apdt)
{  thepdt = apdt;  }

Handle(TCollection_HAsciiString)  StepBasic_DocumentType::ProductDataType () const
{  return thepdt;  }

void  StepBasic_DocumentType::SetProductDataType (const Handle(TCollection_HAsciiString)& apdt)
{  thepdt = apdt;  }
