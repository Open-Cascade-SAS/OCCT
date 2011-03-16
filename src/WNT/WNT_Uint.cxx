#ifndef __WNT_Uint_HeaderFile
# include <WNT_Uint.hxx>
#endif  // __WNT_Uint_HeaderFile

const Handle( Standard_Type )& STANDARD_TYPE( WNT_Uint ) {

 static Handle( Standard_Type ) _aType = new Standard_Type (
                                              "WNT_Uint", sizeof ( WNT_Uint )
                                             );

 return _aType;

}  // end function
