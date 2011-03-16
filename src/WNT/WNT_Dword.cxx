#ifndef __WNT_Dword_HeaderFile
# include <WNT_Dword.hxx>
#endif  // __WNT_Dword_HeaderFile

const Handle( Standard_Type )& STANDARD_TYPE( WNT_Dword ) {

 static Handle( Standard_Type ) _aType = new Standard_Type (
                                              "WNT_Dword", sizeof ( WNT_Dword )
                                             );

 return _aType;

}  // end function
