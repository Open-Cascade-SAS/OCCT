#include <Standard_OutOfRange.hxx> 
#include <Standard_NegativeValue.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_NotImplemented.hxx>
//#include <DBC_DBVArray.hxx>
#include <DBC_BaseArray.ixx>

#ifdef OBJS
#include <Standard_Persistent.hxx>

// taille a partir de laquelle la tableau cre son propre container
//
#define DBC_TAILLE_PLAFOND_CREATION_SEGMENT 10000 

#include <StandardOBJS_PersistentMacro.hxx>
#include <StandardOBJS_PrimitiveTypes.hxx>

// just use for internal generic identifiaction of Item
//
static PStandard_ArrayNode DBC_tmpItem;

#endif

#ifdef OO2
#include <StandardDB_O2Handle.hxx>
#endif

// --------------------------------------------------------------------
// BaseArray implementation  : Temporary arrays known as variable-size arrays.
//                          BaseArray of BaseArray is not supported in this version.
// 
// Last Revision : Feb,10 1992 J.P Tirault
//                 . Implementation of Lock/Unlock methods :
//                 . Implementation of ShallowCopy,ShallowDump,IsSameState.
//                               
// Last Revision : April,4 1992 J.P Tirault, R Barreto
//                 . Implementation for Object store.
//                 . Implementation for Memory.
//                               
// Last Revision : September,17 1992 J.P Tirault
//                 . Free old area in Resize method. 
// Last revision : January,18 1995 N. Massip
//                   . Implementation for ObjectStore 
//		     with an c++ persistent array
//   This version store all the information for the Item
// in case of non primitive type, the handle, the Standard oid,
// and the object itself are stored
// if the oid_standard disapear, it may become interresting to
// store only the object itself. So a special version who store
// only the Object is join in comment with each function who
// may be different, the Haeder of this ccomment is like this :
//
//   For no Handle Storing
     /*
	<code>
     */
//   Last revision : April 19 1992 N. Massip
//                     . Implementation for o2 technology
//                     with an o2_array for 4.5
//                     should be d_Varray for 4.6
//                Use an Collection of StandardDB_O2Handle
//                for none primitive type to avoid to import
//                Handle_Standard_Persistent.
// --------------------------------------------------------------------
//   Last revision : January 1996 JMS/CLE
//                   portage OBJY 3.8




// --------------------------------------------------------------------
//                                 ShallowDump
// --------------------------------------------------------------------
//void DBC_BaseArray::ShallowDump( Standard_OStream& S) const
void DBC_BaseArray::ShallowDump( Standard_OStream& ) const
{
}


// --------------------------------------------------------------------
//                                  Constructor
// --------------------------------------------------------------------
DBC_BaseArray::DBC_BaseArray (const Standard_Integer Size) 
#ifdef OBJY
     //==== Call the father's constructor.
     : myData((unsigned int) Size)
#endif

#ifdef OO2
     : myData((unsigned int) Size)
#endif
{
  Standard_NegativeValue_Raise_if((Size < 0), "Attempt to create aBaseArray with a bad size");
  mySize = Size;
// Standard_Integer i;
#ifdef OBJSold
  INTERNAL_MODE_OODB_START {
    os_segment *seg = Storage_CurrentContainer(); 
    if ( Size > DBC_TAILLE_PLAFOND_CREATION_SEGMENT ) { 
      os_database *db = seg->database_of();
      Storage_CurrentContainer() = db->create_segment();
    }        
    myData = Standard_PersistentAllocation(DBC_tmpItem, Size);
    Storage_CurrentContainer() = seg;
  }
  INTERNAL_MODE_OODB_END;
  
#endif


}


DBC_BaseArray::DBC_BaseArray () 

#ifdef OBJY
   //==== Call the father's constructor.
   : myData(0)
#endif

#ifdef OO2
     : myData((unsigned int) 0)
#endif
{
  mySize = 0;

#ifdef OBJS
  myData = NULL;
#endif
}


// --------------------------------------------------------------------
//                                  Constructor

// --------------------------------------------------------------------
DBC_BaseArray::DBC_BaseArray (const DBC_BaseArray& Other) 
#ifdef OBJY
     //==== Call the father's constructor.
     : myData(Other.myData)
#endif

// utilisable pour d_array mais pas o2_list
#ifdef OO2
     //==== Call the father's constructor.
     : myData(Other.myData)
#endif
{

  mySize = Other.mySize;

#ifdef OBJSold
  INTERNAL_MODE_OODB_START {    
    myData = Standard_PersistentAllocation(DBC_tmpItem, mySize);
    for(Standard_Integer i=0; i<mySize; i++)
      myData[i] = Other.myData[i];
  }
  INTERNAL_MODE_OODB_END;   
#endif


}

void DBC_BaseArray::Delete()
{}


// --------------------------------------------------------------------
//                                  Lock
//
//   - Call the oodb method update which 
// --------------------------------------------------------------------
Standard_Address DBC_BaseArray::Lock () const
{
#ifdef OBJY
  return (Standard_Address) &(myData.elem(0));
#endif

#ifdef OO2
  return (Standard_Address) &(Value(0));
#endif

#ifdef OBJS
  Standard_Address adr;
  INTERNAL_MODE_OODB_START {
    adr = (Standard_Address) &myData;
  }
  INTERNAL_MODE_OODB_END;
  return adr;
#endif

#ifdef CSFDB
  return (Standard_Address) myData;
#endif
}


// --------------------------------------------------------------------
//                                  Unlock
// --------------------------------------------------------------------
void DBC_BaseArray::Unlock() const
{
}


#ifdef OO2
// Liste de fonctionnalire permettent de manipuler StandardDB_O2Handle et
// les types primitifs de maniere tranparente
//
inline Item &O2Handle_getValue(const StandardDB_O2Handle &val) {

  //Item *hd = (Item *) val.getHandle();
  //return *hd; 
  return * ((Item *) val.getHandle());
}
// polymorphisme avec type primitif

inline Standard_Boolean      &O2Handle_getValue(const Standard_Boolean &val) {
  return (Standard_Boolean) val;
};
inline Standard_Integer      &O2Handle_getValue(const Standard_Integer &val){
  return (Standard_Integer) val;
};
inline Standard_Real         &O2Handle_getValue(const Standard_Real &val){
  return (Standard_Real) val;
};
inline Standard_Character    &O2Handle_getValue(const Standard_Character &val){
  return (Standard_Character) val;
};
inline Standard_ExtCharacter &O2Handle_getValue(const Standard_ExtCharacter &val){
  return (Standard_ExtCharacter) val;
};
inline Standard_CString      &O2Handle_getValue(const Standard_CString &val){
  return (Standard_CString) val;
};
inline Standard_ExtString    &O2Handle_getValue(const Standard_ExtString &val){
  return (Standard_ExtString) val;
};
inline Standard_Address      &O2Handle_getValue(const Standard_Address &val){
  return (Standard_Address ) val;
};
inline Standard_ShortReal    &O2Handle_getValue(const Standard_ShortReal &val){
  return (Standard_ShortReal) val;
};
#endif
 


