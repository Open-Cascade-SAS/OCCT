#include <IntSurf_Transition.ixx>


IntSurf_Transition::IntSurf_Transition (const Standard_Boolean Tangent,
					const IntSurf_TypeTrans Type):
       tangent(Tangent),
       typetra(Type),
       situat(IntSurf_Unknown),
       oppos(Standard_False)

{}


IntSurf_Transition::IntSurf_Transition (const Standard_Boolean Tangent,
					const IntSurf_Situation Situ,
					const Standard_Boolean Oppos):
       tangent(Tangent),
       typetra(IntSurf_Touch),
       situat(Situ),
       oppos(Oppos)
{}


IntSurf_Transition::IntSurf_Transition ():
       tangent(Standard_False),
       typetra(IntSurf_Undecided),
       situat(IntSurf_Unknown),
       oppos(Standard_False)
{}
