#include <Transfer_BinderOfTransientInteger.ixx>

    Transfer_BinderOfTransientInteger::Transfer_BinderOfTransientInteger ()
    : theintval (0)    {  }

    void  Transfer_BinderOfTransientInteger::SetInteger
  (const Standard_Integer val)
      {  theintval = val;  }

    Standard_Integer  Transfer_BinderOfTransientInteger::Integer () const
      {  return theintval;  }
