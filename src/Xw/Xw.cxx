

// File         Xw.cxx
// Created      September 1992
// Author       GG

//-Copyright    MatraDatavision 1991,1992

//-Version

//-Design       

//-Warning     

//-References

//-Language     C++ 2.1

//-Declarations

// for the class
#include <Xw.ixx>

// Routines C a declarer en extern
//extern "C" {
#include <Xw_Cextern.hxx>
//}

//-Aliases

//-Global data definitions

//-Local data definitions
static Standard_Integer MyTraceLevel = 0 ;
static Standard_Integer MyErrorLevel = 4 ;

//-Constructors

void Xw::SetTrace(const Standard_Integer TraceLevel,const Standard_Integer ErrorLevel) {
        MyTraceLevel = TraceLevel ;
	MyErrorLevel = ErrorLevel ;

	Xw_set_trace((int)MyTraceLevel,(int)MyErrorLevel) ;
}

Standard_Integer Xw::TraceLevel() {

        return MyTraceLevel ;
}

Standard_Integer Xw::ErrorLevel() {

        return MyErrorLevel ;
}
