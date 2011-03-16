#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <Standard_PrimitiveTypes.hxx>
#include <Standard_Persistent.hxx>
//#include <Standard_Type.hxx>
//#include <Standard_Stream.hxx>
#include <Standard.hxx>

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef HAVE_STRING_H
# include <string.h>
#endif

//class Handle(Standard_Type);

//extern const Handle(Standard_Transient)  Standard_TransientNullHandle;
//extern const Handle(Standard_Persistent) Standard_PersistentNullHandle;
//const Handle(Standard_Transient)  Standard_TransientNullHandle;
//const Handle(Standard_Persistent) Standard_PersistentNullHandle;

//---------------------------------------------------

#ifdef WNT
# include <windows.h>
# include <winbase.h>
#endif

//=======================================================================
//function : CSFDB_Allocate
//purpose  : 
//=======================================================================

Standard_Address StandardCSFDB_Allocate(const Standard_Size aSize)
{
  return Standard::Allocate( aSize );
}


//=======================================================================
//function : Free
//purpose  : 
//=======================================================================

void StandardCSFDB_Free(Standard_Address& aStorage )
{
  Standard::Free( aStorage );
}

//=======================================================================
//function : Reallocate
//purpose  : 
//=======================================================================

Standard_EXPORT Standard_Address StandardCSFDB_Reallocate
                                        (Standard_Address&   aStorage,
                                         const Standard_Size ,
                                         const Standard_Size newSize)
{
  return Standard::Reallocate( aStorage, newSize );
}
