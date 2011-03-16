#define Standard_STATIC(type,name) type& name() \
                                   { \
                                     static type _##name; \
                                     return _##name; \
                                   }

#define Standard_STATIC_INIT(type,name,initfunc) type& name() \
                                                 { \
                                                   static type _##name = initfunc; \
                                                   return _##name; \
                                                 }



