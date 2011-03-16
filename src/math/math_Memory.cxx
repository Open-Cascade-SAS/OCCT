//static const char* sccsid = "@(#)math_Memory.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
#include <math_Memory.hxx>

void *reverse_move(void *s1, void *s2, int size) {

   for(int i = size - 1; i >= 0; i--) {
     *((char *)s1 + i) = *((char *)s2 + i);
   }
   return s1;
}
