#include <IntPatch_Line.ixx>


IntPatch_Line::IntPatch_Line (const Standard_Boolean Tang,
			      const IntSurf_TypeTrans Trans1,
			      const IntSurf_TypeTrans Trans2):
       tg(Tang),
       tS1(Trans1),tS2(Trans2),
       sit1(IntSurf_Unknown),sit2(IntSurf_Unknown),
       uS1(Standard_False),vS1(Standard_False),
       uS2(Standard_False),vS2(Standard_False)
{}

IntPatch_Line::IntPatch_Line (const Standard_Boolean Tang,
			      const IntSurf_Situation Situ1,
                              const IntSurf_Situation Situ2):
       tg(Tang),
       tS1(IntSurf_Touch),tS2(IntSurf_Touch),
       sit1(Situ1),sit2(Situ2),
       uS1(Standard_False),vS1(Standard_False),
       uS2(Standard_False),vS2(Standard_False)
{}

IntPatch_Line::IntPatch_Line (const Standard_Boolean Tang):
       tg(Tang),
       tS1(IntSurf_Undecided),tS2(IntSurf_Undecided),
       sit1(IntSurf_Unknown),sit2(IntSurf_Unknown),
       uS1(Standard_False),vS1(Standard_False),
       uS2(Standard_False),vS2(Standard_False)
{}


