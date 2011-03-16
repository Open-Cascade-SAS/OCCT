#include <Standard_Transient.hxx>
#include <Standard_Persistent.hxx>
#include <Standard_Type.hxx>
#include <Standard_RangeError.hxx>

//============================================================================
Standard_EXPORT Standard_Integer HashCode(const Standard_Address me,const Standard_Integer Upper,const Handle(Standard_Type)& aType)
{
  char             *mecharPtr;
  Standard_Integer *meintPtr = (Standard_Integer *) me;
  Standard_Integer aHashCode, aRest, i, aSize = aType->Size();
  
  Standard_RangeError_Raise_if (Upper < 1,
      "Try to apply HashCode method with negative or null argument.");

  aRest = aSize % sizeof(Standard_Integer);

  aHashCode = 0;

  if (aSize == 0) 
    aHashCode = (Standard_Integer) ptrdiff_t(me);


  for (i = 0;  (unsigned int ) i < aSize / sizeof(Standard_Integer); i++)
  {
    aHashCode = aHashCode ^ *meintPtr;
    meintPtr++;
  }
  
  mecharPtr = (char *) meintPtr;

  for (i = 0; i < aRest; i++) 
  {
    aHashCode = aHashCode ^ *mecharPtr;
    mecharPtr++;
  }


  return HashCode( aHashCode , Upper) ;
}

//============================================================================
Standard_EXPORT Standard_Integer HashCode (const Handle(Standard_Transient)& me,
                                           const Standard_Integer Upper )
{
  Standard_RangeError_Raise_if (Upper < 1,
                                "Try to apply HashCode method "
                                "with negative or null argument.");

  const Standard_Integer I = (Standard_Integer) ptrdiff_t(me.operator->());
  return HashCode( I , Upper ) ;
}

//============================================================================
Standard_EXPORT Standard_Integer HashCode(const Handle(Standard_Persistent)& me,
                                          const Standard_Integer Upper )
{
  Standard_RangeError_Raise_if (Upper < 1,
                                "Try to apply HashCode method "
                                "with negative or null argument.");

  const Standard_Integer I = (Standard_Integer) ptrdiff_t(me.operator->());
  return HashCode( I , Upper) ;
}

