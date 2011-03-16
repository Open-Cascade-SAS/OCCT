#include <StepBasic_ApprovalDateTime.ixx>

StepBasic_ApprovalDateTime::StepBasic_ApprovalDateTime ()  {  }

void  StepBasic_ApprovalDateTime::Init (const StepBasic_DateTimeSelect& aDateTime, const Handle(StepBasic_Approval)& aDatedApproval)
{
  theDateTime = aDateTime;
  theDatedApproval = aDatedApproval;
}

void  StepBasic_ApprovalDateTime::SetDateTime (const StepBasic_DateTimeSelect& aDateTime)
{  theDateTime = aDateTime;  }

StepBasic_DateTimeSelect  StepBasic_ApprovalDateTime::DateTime () const
{  return theDateTime;  }

void  StepBasic_ApprovalDateTime::SetDatedApproval (const Handle(StepBasic_Approval)& aDatedApproval)
{  theDatedApproval = aDatedApproval;  }

Handle(StepBasic_Approval)  StepBasic_ApprovalDateTime::DatedApproval () const
{  return theDatedApproval;  }
