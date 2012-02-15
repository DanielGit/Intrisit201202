
#define FRAC_BIT   8
#define EXP_FRAC_BIT   16
#define FIX_DIV(a, b)   ((int)((((long long)a)<<FRAC_BIT) / (b)))
#define FIX_MULT(a, b)  ((int)((((long long)a) * (b)) >> FRAC_BIT))
#define FIXF2_MULT(a, b)  ((int)((((long long)a) * (b)) >> (2*FRAC_BIT)))
#define FIX_MULT3(a, b, c)  ((int)((((((long long)a) * (b)) >> FRAC_BIT) * (c)) >> FRAC_BIT))
#define FLOAT(a)  (float)(a / ((float)(1 << FRAC_BIT)))
#define FIXED(a)  ((int)(((float)(a)) * (1 << FRAC_BIT)))

