// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

// This preprocessor directive is a kludge to get around
// a bug in the Sun Workshop 5.0 compiler, it keeps the
// /usr/include/memory.h file from being #included
// with an incompatible extern "C" definition of memchr
// October 18, 2000  <rboehne@ricardo-us.com>
#if __SUNPRO_CC == 0x500
#define	_MEMORY_H
#endif

//#ifndef OCCT_DEBUG
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#define No_Standard_DimensionError
//#endif

#include <cmath>

#include <math_Recipes.hxx>

#include <Standard_Failure.hxx>
#include <Standard_NotImplemented.hxx>

#include <math_Vector.hxx>
#include <math_IntegerVector.hxx>
#include <math_Matrix.hxx>

namespace {
static inline Standard_Real PYTHAG (const Standard_Real a, const Standard_Real b)
{
  Standard_Real at = fabs (a), bt = fabs (b), ct = 0.;
  if (at > bt) {
    ct = bt / at;
    ct = at * sqrt (1.0 + ct * ct);
  } else if (bt) {
    ct = at / bt;
    ct = bt * sqrt (1.0 + ct * ct);
  }
  return ct;
}
}

#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))


#define ROTATE(a,i,j,k,l) g=a(i,j);\
                          h=a(k,l);\
                          a(i,j)=g-s*(h+g*tau);\
                          a(k,l)=h+s*(g-h*tau);

#define M 714025
#define IA 1366
#define IC 150889

static void EigenSort(math_Vector& d, math_Matrix& v) { // descending order

      Standard_Integer k, i, j;
      Standard_Real p;
      Standard_Integer n = d.Length();

      for(i = 1; i < n; i++) {
        p = d(k = i);
        for(j = i + 1; j <= n; j++)
          if(d(j) >= p) p = d(k = j);
        if(k != i) {
          d(k) = d(i);
          d(i) = p;
          for(j = 1; j <= n; j++) {
            p = v(j, i);
            v(j, i) = v(j, k);
            v(j, k) = p;
          }
	}
      }
   }

Standard_Integer Jacobi(math_Matrix& a, math_Vector& d, math_Matrix& v, Standard_Integer& nrot) {

      Standard_Integer n = a.RowNumber();
      Standard_Integer j, iq, ip, i;
      Standard_Real tresh, theta, tau, t, sm, s, h, g, c;
      math_Vector b(1, n);
      math_Vector z(1, n);

      for(ip = 1; ip <= n; ip++) {
        for(iq = 1; iq <= n; iq++)
	  v(ip, iq) = 0.0;
        v(ip, ip) = 1.0;
      }
      for(ip = 1; ip <= n; ip++) {
        b(ip) = d(ip) = a(ip, ip);
        z(ip) = 0.0;
      }
      nrot = 0;
      for(i = 1; i <= 50; i++) {
        sm = 0.0;
        for(ip = 1; ip < n; ip++) {
          for(iq = ip + 1; iq <= n; iq++)
            sm += fabs(a(ip, iq));
	}
        if(sm == 0.0) {
	  EigenSort(d, v);
	  return math_Status_OK;
	}
        if(i < 4) {
          tresh = 0.2 * sm / (n * n);
        }
        else {
          tresh = 0.0;
        }
        for(ip = 1; ip < n; ip++) {
          for(iq = ip + 1; iq <= n; iq++) {
            g = 100.0 * fabs(a(ip, iq));
            if(i > 4 && 
               fabs(d(ip)) + g == fabs(d(ip)) &&
               fabs(d(iq)) + g == fabs(d(iq))) a(ip, iq) = 0.0;
            else if(fabs(a(ip, iq)) > tresh) {
              h = d(iq) - d(ip);
              if(fabs(h) + g == fabs(h)) 
                t = a(ip, iq) / h;
              else {
                theta = 0.5 * h / a(ip, iq);
                t = 1.0 / (fabs(theta) + sqrt(1.0 + theta * theta));
                if(theta < 0.0) t = -t;
              }
              c = 1.0 / sqrt(1 + t * t);
              s = t * c;
              tau = s / (1.0 + c);
              h = t * a(ip, iq);
              z(ip) -= h;
              z(iq) += h;
              d(ip) -= h;
              d(iq) += h;
              a(ip, iq) = 0.0;
              for(j = 1; j < ip; j++) {
                ROTATE(a, j, ip, j, iq)
              }
              for(j = ip + 1; j < iq; j++) {
                ROTATE(a, ip, j, j, iq)
	      }
              for(j = iq + 1; j <= n; j++) {
                ROTATE(a, ip, j, iq, j)
              }
              for(j = 1; j <= n; j++) {
                ROTATE(v, j, ip, j, iq)
	      }
              nrot++;
            }
          }
        }
        for(ip = 1; ip <= n; ip++) {
          b(ip) += z(ip);
          d(ip) = b(ip);
          z(ip) = 0.0;
        }
      }
      EigenSort(d, v);
      return math_Status_NoConvergence;
}

Standard_Integer LU_Decompose(math_Matrix& a, 
                     math_IntegerVector& indx, 
                     Standard_Real&   d, 
                     math_Vector& vv,
                     Standard_Real    TINY) { 

     Standard_Integer i, imax=0, j, k;
     Standard_Real big, dum, sum, temp;

     Standard_Integer n = a.RowNumber();
     d = 1.0;

     for(i = 1; i <= n; i++) {
       big = 0.0;
       for (j = 1; j <= n; j++) 
         if((temp = fabs(a(i, j))) > big) big = temp;
       if(big <= TINY) { 
         return math_Status_SingularMatrix;
       }
       vv(i) = 1.0 / big;
     }
     for(j = 1; j <= n; j++) {
       for(i = 1; i < j; i++) {
         sum = a(i,j);
         for(k = 1; k < i; k++)
	   sum -= a(i,k) * a(k,j);
         a(i,j) = sum;
       }
       big = 0.0;
       for(i = j; i <= n; i++) {
         sum = a(i,j);
         for(k = 1; k < j; k++) 
           sum -= a(i,k) * a(k,j);
         a(i,j) = sum;
         // Note that comparison is made so as to have imax updated even if argument is NAN, Inf or IND, see #25559
         if((dum = vv(i) * fabs(sum)) < big)
         {
           continue;
         }
         big = dum;
         imax = i;
       }
       if(j != imax) {
         for(k = 1; k <= n; k++) {
           dum = a(imax,k);
           a(imax,k) = a(j,k);
           a(j,k) = dum;
         }
         d = -d;
         vv(imax) = vv(j);
       }
       indx(j) = imax;
       if(fabs(a(j, j)) <= TINY) {
         return math_Status_SingularMatrix;
       }
       if(j != n) {
         dum = 1.0 / (a(j,j));
         for(i = j + 1; i <= n; i++)
	   a(i,j) *= dum;
       }
     }
     return math_Status_OK;
}

Standard_Integer LU_Decompose(math_Matrix& a, 
                    math_IntegerVector& indx, 
                    Standard_Real&   d, 
                    Standard_Real    TINY) { 

     math_Vector vv(1, a.RowNumber());
     return LU_Decompose(a, indx, d, vv, TINY);
}

void LU_Solve(const math_Matrix& a,
              const math_IntegerVector& indx, 
              math_Vector& b) {

     Standard_Integer i, ii = 0, ip, j;
     Standard_Real sum;

     Standard_Integer n=a.RowNumber();
     Standard_Integer nblow=b.Lower()-1;
     for(i = 1; i <= n; i++) {
       ip = indx(i);
       sum = b(ip+nblow);
       b(ip+nblow) = b(i+nblow);
       if(ii) 
         for(j = ii; j < i; j++)
	   sum -= a(i,j) * b(j+nblow);
       else if(sum) ii = i;
       b(i+nblow) = sum;
     }
     for(i = n; i >= 1; i--) {
       sum = b(i+nblow);
       for(j = i + 1; j <= n; j++)
	 sum -= a(i,j) * b(j+nblow);
       b(i+nblow) = sum / a(i,i);
     }
}

Standard_Integer LU_Invert(math_Matrix& a) {

     Standard_Integer n=a.RowNumber();
     math_Matrix inv(1, n, 1, n);
     math_Vector col(1, n);
     math_IntegerVector indx(1, n);
     Standard_Real d;
     Standard_Integer i, j;

     Standard_Integer Error = LU_Decompose(a, indx, d);
     if(!Error) {
       for(j = 1; j <= n; j++) {
         for(i = 1; i <= n; i++)
	   col(i) = 0.0;
         col(j) = 1.0;
         LU_Solve(a, indx, col);
         for(i = 1; i <= n; i++)
	   inv(i,j) = col(i);
       }
       for(j = 1; j <= n; j++) {
         for(i = 1; i <= n; i++) {
           a(i,j) = inv(i,j);
         }
       }
     }
    
     return Error;
}

Standard_Integer SVD_Decompose(math_Matrix& a,
                     math_Vector& w,
                     math_Matrix& v) {

     math_Vector rv1(1, a.ColNumber());
     return SVD_Decompose(a, w, v, rv1);
   }


Standard_Integer SVD_Decompose(math_Matrix& a,
                     math_Vector& w,
                     math_Matrix& v,
                     math_Vector& rv1) {

     Standard_Integer flag, i, its, j, jj, k, l=0, nm=0;
     Standard_Real ar, aw, aik, aki, c, f, h, s, x, y, z;
     Standard_Real anorm = 0.0, g = 0.0, scale = 0.0;
     Standard_Integer m = a.RowNumber();
     Standard_Integer n = a.ColNumber();

     for(i = 1; i <= n; i++) {
       l = i + 1;
       rv1(i) = scale * g;
       g = s = scale = 0.0;
       if(i <= m) {
         for(k = i; k <= m; k++) {
	   aki = a(k,i);
	   if (aki > 0) scale += aki;
	   else         scale -= aki;
	 }
         if(scale) {
           for(k = i; k <= m; k++) {
             a(k,i) /= scale;
             s += a(k,i) * a(k,i);
           }
           f = a(i,i);
           g = -SIGN(sqrt(s), f);
           h = f * g - s;
           a(i,i) = f - g;
           if(i != n) {
             for(j = l; j <= n; j++) {
               for(s = 0.0, k = i; k <= m; k++)
		 s += a(k,i) * a(k,j);
               f = s / h;
               for(k = i; k <= m; k++)
		 a(k,j) += f * a(k,i);
             }
           }
           for(k = i; k <= m; k++)
	     a(k,i) *= scale;
         }
       }
       w(i) = scale * g;
       g = s = scale = 0.0;
       if(i <= m && i != n) {
         for(k = l; k <= n; k++) {
	   aik = a(i,k);
	   if (aik > 0) scale += aik;
	   else         scale -= aik;
	 }
         if(scale) {
           for(k = l; k <= n; k++) {
             a(i,k) /= scale;
             s += a(i,k) * a(i,k);
           } 
           f = a(i,l);
           g = -SIGN(sqrt(s), f);
           h = f * g - s;
           a(i,l) = f - g;
           for (k = l; k <= n; k++)
	     rv1(k) = a(i,k) / h;
           if(i != m) {
             for(j = l; j <=m; j++) {
                for(s = 0.0, k = l; k <= n; k++)
		  s += a(j,k) * a(i,k);
                for(k = l; k <=n; k++)
		  a(j,k) += s * rv1(k);
             }
           }
           for (k = l; k <= n; k++)
	     a(i,k) *= scale;
         }
       }
       aw = w(i);
       if (aw < 0) aw = - aw;
       ar = rv1(i);
       if (ar > 0) ar = aw + ar;
       else        ar = aw - ar;
       if (anorm < ar) anorm = ar;
     }
     for(i = n; i >= 1; i--) {
       if(i < n) {
         if(g) {
           for(j = l; j <= n; j++)
	     v(j,i) = (a(i,j) / a(i,l)) / g;
           for(j = l; j <= n; j++) {
             for(s = 0.0, k = l; k <= n; k++)
	       s += a(i,k) * v(k,j);
             for(k = l; k <= n; k++)
	       v(k,j) += s * v(k,i);
           }
         }
         for(j = l; j <= n; j++)
	   v(i,j) = v(j,i) = 0.0;
       } 
       v(i,i) = 1.0;
       g = rv1(i);
       l = i;
     }
     for(i = n; i >= 1; i--) {
       l = i + 1;
       g = w(i);
       if(i < n) for(j = l; j <= n; j++)
	 a(i,j) = 0.0;
       if(g) {
         g = 1.0 / g;
         if(i != n) {
           for(j = l; j <= n; j++) {
             for(s = 0.0, k = l; k <= m; k++)
	       s += a(k,i) * a(k,j);
             f = (s / a(i,i)) * g;
             for(k = i; k <= m; k++)
	       a(k,j) += f * a(k,i);
           }
         }
         for(j = i; j <= m; j++)
	   a(j,i) *= g;
       }
       else {
         for(j = i; j <= m; j++)
	   a(j,i) = 0.0;
       }
       ++a(i,i);
     }
     for(k = n; k >= 1; k--) {
       for(its = 1; its <= 30; its++) {
         flag = 1;
         for(l = k; l >= 1; l--) {
           nm = l - 1;
           if(fabs(rv1(l)) + anorm == anorm) {
             flag = 0;
             break;
           }
           if(fabs(w(nm)) + anorm == anorm) break;
         }
         if(flag) {
           c = 0.0;
           s = 1.0;
           for(i = l; i <= k; i++) {
             f = s * rv1(i);
             if(fabs(f) + anorm != anorm) {
               g = w(i);
               h = PYTHAG(f, g);
               w(i) = h;
               h = 1.0 / h;
               c = g * h;
               s = (-f * h);
               for(j = 1; j <= m; j++) {
                 y = a(j,nm);
                 z = a(j,i);
                 a(j,nm) = y * c + z * s;
                 a(j,i) = z * c - y * s;
               }
             }
           }
         }
         z = w(k);
         if(l == k) {
           if(z < 0.0) {
             w(k) = -z;
             for(j = 1; j <= n; j++)
	       v(j,k) = (-v(j,k));
           }
           break;
         }
         if(its == 30) {
           return math_Status_NoConvergence;
         }
         x = w(l);
         nm = k - 1;
         y = w(nm);
         g = rv1(nm);
         h = rv1(k);
         f = ((y - z) * (y + z) + (g - h) * (g + h)) / ( 2.0 * h * y);
         g = PYTHAG(f, 1.0);
         f = ((x - z) * (x + z) + h * ((y / ( f + SIGN(g, f))) - h)) / x;
         
         c = s = 1.0;
         for(j = l; j <= nm; j++) {
           i = j + 1;
           g = rv1(i);
           y = w(i);
           h = s * g;
           g = c * g;
           z = PYTHAG(f, h);
           rv1(j) = z;
           c = f / z;
           s = h / z;
           f = x * c + g * s;
           g = g * c - x * s;
           h = y * s;
           y = y * c;
           for(jj = 1; jj <= n; jj++) {
             x = v(jj,j);
             z = v(jj,i);
             v(jj,j) = x * c + z * s;
             v(jj,i) = z * c - x * s;
           }
           z = PYTHAG(f, h);
           w(j) = z;
           if(z) {
             z = 1.0 / z;
             c = f * z;
             s = h * z;
           }
           f = (c * g) + (s * y);
           x = (c * y) - (s * g);
           for(jj = 1; jj <= m; jj++) {
             y = a(jj,j);
             z = a(jj,i);
             a(jj,j) = y * c + z * s;
             a(jj,i) = z * c - y * s;
           }
         }
         rv1(l) = 0.0;
         rv1(k) = f;
         w(k) = x;
       }
     }
     return math_Status_OK;
}

void SVD_Solve(const math_Matrix& u,
               const math_Vector& w,
               const math_Matrix& v,
               const math_Vector& b,
               math_Vector& x) {

     Standard_Integer jj, j, i;
     Standard_Real s;

     Standard_Integer m = u.RowNumber();
     Standard_Integer n = u.ColNumber();
     math_Vector tmp(1, n);

     for(j = 1; j <= n; j++) {
       s = 0.0;
       if(w(j)) {
         for(i = 1; i <= m; i++)
	   s += u(i,j) * b(i);
         s /= w(j);
       }
       tmp(j) = s;
     }
     for(j = 1; j <= n; j++) {
       s = 0.0;
       for(jj = 1; jj <= n; jj++)
	 s += v(j,jj) * tmp(jj);
       x(j) = s;
     }
}  

Standard_Integer DACTCL_Decompose(math_Vector& a, 
			const math_IntegerVector& indx,
                        const Standard_Real MinPivot) {

     Standard_Integer i, j, Neq = indx.Length();
     Standard_Integer jr, jd, jh, is, ie, k, ir, id, ih, mh;
     Standard_Integer idot, idot1, idot2;
     Standard_Real aa, d, dot;
     Standard_Boolean diag;

     jr = 0;
     for (j = 1; j <= Neq; j++) {
       diag = Standard_False;
       jd = indx(j);
       jh = jd-jr;
       is = j-jh+2;
       if (jh-2 == 0) diag = Standard_True;
       if (jh-2 > 0) {
	 ie = j-1;
	 k = jr+2;
	 id = indx(is-1);
	 // Reduction des coefficients non diagonaux:
	 // =========================================
	 for (i = is; i <= ie; i++) {
	   ir = id;
	   id = indx(i);
	   ih = id - ir - 1;
	   mh = i  - is + 1;
	   if (ih > mh) ih = mh;
	   if (ih > 0.0) {
	     dot = 0.0;
	     idot1 = k-ih-1;
	     idot2 = id-ih-1;
	     for (idot = 1; idot <= ih; idot++) {
	       dot = dot +a(idot1+idot)*a(idot2+idot);
	     }
	     a(k) = a(k)-dot;
	   }
	   k++;
	 }
	 diag = Standard_True;
       }

       if (diag) {
	 // Reduction des coefficients diagonaux:
	 // =====================================
	 ir = jr+1;
	 ie = jd-1;
	 k = j-jd;
	 for (i = ir; i <= ie; i++) {
	   id = indx(k+i);
	   aa = a(id);
	   if (aa < 0) aa = - aa;
	   if (aa <= MinPivot) 
	     return math_Status_SingularMatrix;
	   d = a(i);
	   a(i) = d/a(id);
	   a(jd) = a(jd)-d*a(i);
	 }

       }
       jr = jd;
     }
     return math_Status_OK;
}


Standard_Integer DACTCL_Solve(const math_Vector& a, 
		    math_Vector& b,
		    const math_IntegerVector& indx,
                    const Standard_Real MinPivot) {

     Standard_Integer i, j, Neq = indx.Length();
     Standard_Integer jr, jd, jh, is, k, id;
     Standard_Integer jh1, idot, idot1, idot2;
     Standard_Real aa, d, dot;

     jr = 0;
     for (j = 1; j <= Neq; j++) {
       jd = indx(j);
       jh = jd-jr;
       is = j-jh+2;

       // Reduction du second membre:
       // ===========================
       dot = 0.0;
       idot1 = jr;
       idot2 = is-2;
       jh1 = jh-1;
       for (idot = 1; idot <= jh1; idot++) {
	 dot = dot + a(idot1+idot)*b(idot2+idot);
       }
       b(j) = b(j)-dot;
       
       jr = jd;
     }

     // Division par les pivots diagonaux:
     // ==================================
     for (i = 1; i <= Neq; i++) {
       id = indx(i);
       aa = a(id);
       if (aa < 0) aa = - aa;
       if (aa <= MinPivot) 
	 return math_Status_SingularMatrix;
       b(i) = b(i)/a(id);
     }

     
     // Substitution arriere:
     // =====================
     jd = indx(Neq);
     for (j = Neq-1; j > 0; j--) {
       d = b(j+1);
       jr = indx(j);
       if (jd-jr > 1) {
	 is = j-jd+jr+2;
	 k = jr-is+1;
	 for (i = is; i <= j; i++) {
	   b(i) = b(i)-a(i+k)*d;
	 }
       }
       jd = jr;
     }
     return math_Status_OK;

}

