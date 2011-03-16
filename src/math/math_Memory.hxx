#ifndef math_Memory_HeaderFile 
#define math_Memory_HeaderFile 

#include <string.h>

// uniquement parce que memmove n'existe pas sur SUN
#ifndef WNT
void *reverse_move(void *s1, void *s2, int size);

inline void *memmove(void *s1, void *s2, int size) {

/*
     void *result;

     if(s2 < s1) {
       result = reverse_move(s1, s2, size);
     }
     else {
       result = memcpy(s1, s2, size);
     }      
     return result;
*/
     return memcpy(s1, s2, size);
   }
#endif
#endif
