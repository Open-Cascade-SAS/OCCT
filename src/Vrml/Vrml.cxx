#include <Standard_Integer.hxx>
#include <Vrml.ixx>

Standard_OStream& Vrml::VrmlHeaderWriter(Standard_OStream& anOStream)
{
    anOStream << "#VRML V1.0 ascii" << endl;
    anOStream <<  endl;
    anOStream << "# (C) Copyright MATRA DATAVISION 1997" << endl;
    anOStream <<  endl;
    return anOStream;
}

Standard_OStream& Vrml::CommentWriter(const Standard_CString  aComment,
			                    Standard_OStream& anOStream) 
{
    anOStream << "# " << aComment << endl;
    return anOStream;
}
