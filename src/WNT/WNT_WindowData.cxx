#ifndef __WNT_WindowData_HeaderFile
# include <WNT_WindowData.hxx>
#endif  // __WNT_WindowData_HeaderFile

const Handle( Standard_Type )& STANDARD_TYPE( WNT_WindowData ) {

 static Handle( Standard_Type ) _aType = new Standard_Type (
                                              "WNT_WindowData",
                                               sizeof ( WNT_WindowData )
                                             );

 return _aType;

}  // end function
