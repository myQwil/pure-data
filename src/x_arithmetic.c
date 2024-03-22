/* Copyright (c) 1997-1999 Miller Puckette.
* For information on usage and redistribution, and for a DISCLAIMER OF ALL
* WARRANTIES, see the file, "LICENSE.txt," in this distribution.  */

/* arithmetic: binops ala C language.  The 4 functions and relationals are
done on floats; the logical and bitwise binops convert their
inputs to int and their outputs back to float. */

#include "m_pd.h"
#include <math.h>

#if PD_FLOATSIZE == 32
# define POW powf
# define SIN sinf
# define COS cosf
# define ATAN atanf
# define ATAN2 atan2f
# define SQRT sqrtf
# define LOG logf
# define EXP expf
# define FABS fabsf
# define FLOOR floorf
# define MAXLOG 87.3365 /* log(FLT_MAX / 4.) */
#else
# define POW pow
# define SIN sin
# define COS cos
# define ATAN atan
# define ATAN2 atan2
# define SQRT sqrt
# define LOG log
# define EXP exp
# define FABS fabs
# define FLOOR floor
# define MAXLOG 708.396 /* log(DBL_MAX / 4.) */
#endif

typedef struct _binop
{
    t_object x_obj;
    t_float x_f1;
    t_float x_f2;
} t_binop;

static void *binop_new(t_class *class, t_floatarg f)
{
    t_binop *x = (t_binop *)pd_new(class);
    outlet_new(&x->x_obj, &s_float);
    floatinlet_new(&x->x_obj, &x->x_f2);
    x->x_f1 = 0;
    x->x_f2 = f;
    return (x);
}

static void binop_float(t_binop *x, t_float f)
{
    x->x_f1 = f;
    pd_bang((t_pd *)x);
}

static void *unop_new(t_class *class)
{
    t_object *x = (t_object *)pd_new(class);
    outlet_new(x, &s_float);
    return (x);
}


/* ---------------- binop1:  +  -  *  /  log  pow  max  min ----------------- */

static void plus_bang(t_binop *x)
{
    outlet_float(x->x_obj.ob_outlet, x->x_f1 + x->x_f2);
}

static void minus_bang(t_binop *x)
{
    outlet_float(x->x_obj.ob_outlet, x->x_f1 - x->x_f2);
}

static void times_bang(t_binop *x)
{
    outlet_float(x->x_obj.ob_outlet, x->x_f1 * x->x_f2);
}

static void over_bang(t_binop *x)
{
    outlet_float(x->x_obj.ob_outlet,
        (x->x_f2 != 0 ? x->x_f1 / x->x_f2 : 0));
}

static void log_bang(t_binop *x)
{
    t_float r;
    if (x->x_f1 <= 0)
        r = -1000;
    else if (x->x_f2 <= 0)
        r = LOG(x->x_f1);
    else r = LOG(x->x_f1)/LOG(x->x_f2);
    outlet_float(x->x_obj.ob_outlet, r);
}

static void pow_bang(t_binop *x)
{
    t_float r = (x->x_f1 == 0 && x->x_f2 < 0) ||
        (x->x_f1 < 0 && (x->x_f2 - (int)x->x_f2) != 0) ?
            0 : POW(x->x_f1, x->x_f2);
    outlet_float(x->x_obj.ob_outlet, r);
}

static void max_bang(t_binop *x)
{
    outlet_float(x->x_obj.ob_outlet,
        (x->x_f1 > x->x_f2 ? x->x_f1 : x->x_f2));
}

static void min_bang(t_binop *x)
{
    outlet_float(x->x_obj.ob_outlet,
        (x->x_f1 < x->x_f2 ? x->x_f1 : x->x_f2));
}


/* --------------------- binop2:  ==  !=  >  <  >=  <= ---------------------- */

static void ee_bang(t_binop *x)
{
    outlet_float(x->x_obj.ob_outlet, x->x_f1 == x->x_f2);
}

static void ne_bang(t_binop *x)
{
    outlet_float(x->x_obj.ob_outlet, x->x_f1 != x->x_f2);
}

static void gt_bang(t_binop *x)
{
    outlet_float(x->x_obj.ob_outlet, x->x_f1 > x->x_f2);
}

static void lt_bang(t_binop *x)
{
    outlet_float(x->x_obj.ob_outlet, x->x_f1 < x->x_f2);
}

static void ge_bang(t_binop *x)
{
    outlet_float(x->x_obj.ob_outlet, x->x_f1 >= x->x_f2);
}

static void le_bang(t_binop *x)
{
    outlet_float(x->x_obj.ob_outlet, x->x_f1 <= x->x_f2);
}


/* ------------ binop3:  &  |  &&  ||  <<  >>  %  mod  div  fmod ------------ */

static void ba_bang(t_binop *x)
{
    outlet_float(x->x_obj.ob_outlet, ((int)(x->x_f1)) & (int)(x->x_f2));
}

static void bo_bang(t_binop *x)
{
    outlet_float(x->x_obj.ob_outlet, ((int)(x->x_f1)) | (int)(x->x_f2));
}

static void la_bang(t_binop *x)
{
    outlet_float(x->x_obj.ob_outlet, ((int)(x->x_f1)) && (int)(x->x_f2));
}

static void lo_bang(t_binop *x)
{
    outlet_float(x->x_obj.ob_outlet, ((int)(x->x_f1)) || (int)(x->x_f2));
}

static void ls_bang(t_binop *x)
{
    outlet_float(x->x_obj.ob_outlet, ((int)(x->x_f1)) << (int)(x->x_f2));
}

static void rs_bang(t_binop *x)
{
    outlet_float(x->x_obj.ob_outlet, ((int)(x->x_f1)) >> (int)(x->x_f2));
}

static void rem_bang(t_binop *x)
{
    int n2 = x->x_f2;
        /* apparently "%" raises an exception for INT_MIN and -1 */
    if (n2 == -1)
        outlet_float(x->x_obj.ob_outlet, 0);
    else outlet_float(x->x_obj.ob_outlet, ((int)(x->x_f1)) % (n2 ? n2 : 1));
}

static void mod_bang(t_binop *x)
{
    int n2 = x->x_f2, result;
    if (n2 < 0) n2 = -n2;
    else if (!n2) n2 = 1;
    result = ((int)(x->x_f1)) % n2;
    if (result < 0) result += n2;
    outlet_float(x->x_obj.ob_outlet, (t_float)result);
}

static void div_bang(t_binop *x)
{
    int n1 = x->x_f1, n2 = x->x_f2, result;
    if (n2 < 0) n2 = -n2;
    else if (!n2) n2 = 1;
    if (n1 < 0) n1 -= (n2-1);
    result = n1 / n2;
    outlet_float(x->x_obj.ob_outlet, (t_float)result);
}

static void fmod_bang(t_binop *x)
{
    t_float f2 = x->x_f2;
    if (f2 < 0) f2 = -f2;
    else if (!f2) f2 = 1;
    outlet_float(x->x_obj.ob_outlet, x->x_f1 - f2 * FLOOR(x->x_f1 / f2));
}


/* -------------- trig functions:  sin  cos  tan  atan  atan2 --------------- */

static void sin_float(t_object *x, t_float f)
{
    outlet_float(x->ob_outlet, SIN(f));
}

static void cos_float(t_object *x, t_float f)
{
    outlet_float(x->ob_outlet, COS(f));
}

static void tan_float(t_object *x, t_float f)
{
    t_float c = cosf(f);
    t_float t = (c == 0 ? 0 : SIN(f)/c);
    outlet_float(x->ob_outlet, t);
}

static void atan_float(t_object *x, t_float f)
{
    outlet_float(x->ob_outlet, ATAN(f));
}

static void atan2_bang(t_binop *x)
{
    outlet_float(x->x_obj.ob_outlet,
        (x->x_f1 == 0 && x->x_f2 == 0 ? 0 : ATAN2(x->x_f1, x->x_f2)));
}


/* ------------------------- unop:  sqrt  exp  abs -------------------------- */

static void sqrt_float(t_object *x, t_float f)
{
    t_float r = (f > 0 ? SQRT(f) : 0);
    outlet_float(x->ob_outlet, r);
}

static void exp_float(t_object *x, t_float f)
{
    t_float g;
#ifdef _WIN32
    char buf[10];
#endif
    if (f > MAXLOG) f = MAXLOG;
    g = EXP(f);
    outlet_float(x->ob_outlet, g);
}

static void abs_float(t_object *x, t_float f)
{
    outlet_float(x->ob_outlet, FABS(f));
}


/* ---------------------------------- misc ---------------------------------- */

static t_class *clip_class;

typedef struct _clip
{
    t_object x_ob;
    t_float x_f1;
    t_float x_f2;
    t_float x_f3;
} t_clip;

static void *clip_new(t_floatarg f1, t_floatarg f2)
{
    t_clip *x = (t_clip *)pd_new(clip_class);
    floatinlet_new(&x->x_ob, &x->x_f2);
    floatinlet_new(&x->x_ob, &x->x_f3);
    outlet_new(&x->x_ob, &s_float);
    x->x_f2 = f1;
    x->x_f3 = f2;
    return (x);
}

static void clip_bang(t_clip *x)
{
        outlet_float(x->x_ob.ob_outlet, (x->x_f1 < x->x_f2 ? x->x_f2 : (
        x->x_f1 > x->x_f3 ? x->x_f3 : x->x_f1)));
}

static void clip_float(t_clip *x, t_float f)
{
        x->x_f1 = f;
        outlet_float(x->x_ob.ob_outlet, (x->x_f1 < x->x_f2 ? x->x_f2 : (
        x->x_f1 > x->x_f3 ? x->x_f3 : x->x_f1)));
}

static void clip_setup(void)
{
    clip_class = class_new(gensym("clip"), (t_newmethod)clip_new, 0,
        sizeof(t_clip), 0, A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addfloat(clip_class, clip_float);
    class_addbang(clip_class, clip_bang);
}


/* -------------------------------- packages -------------------------------- */

typedef enum _help
{
    BINOP1 = 1,
    BINOP2,
    BINOP3,
    TRIG,
    UNOP
} t_help;

typedef struct _pkg
{
    const char *name;
    void (*new);
    void (*fn);
    t_help help;
    t_class *class;
} t_pkg;

typedef t_floatarg F;
static void *plus_new(F), *minus_new(F), *times_new(F), *over_new(F),
    *log_new(F), *pow_new(F), *max_new(F), *min_new(F), *ee_new(F), *ne_new(F),
    *gt_new(F), *lt_new(F), *ge_new(F), *le_new(F), *ba_new(F), *bo_new(F),
    *la_new(F), *lo_new(F), *ls_new(F), *rs_new(F), *rem_new(F), *mod_new(F),
    *div_new(F), *fmod_new(F), *atan2_new(F);

static void *sin_new(), *cos_new(), *tan_new(), *atan_new(),
    *sqrt_new(), *exp_new(), *abs_new();

static t_pkg plus_pkg  = { .name="+",     .new=plus_new,  .fn=plus_bang,  .help=BINOP1 };
static t_pkg minus_pkg = { .name="-",     .new=minus_new, .fn=minus_bang, .help=BINOP1 };
static t_pkg times_pkg = { .name="*",     .new=times_new, .fn=times_bang, .help=BINOP1 };
static t_pkg over_pkg  = { .name="/",     .new=over_new,  .fn=over_bang,  .help=BINOP1 };
static t_pkg log_pkg   = { .name="log",   .new=log_new,   .fn=log_bang,   .help=BINOP1 };
static t_pkg pow_pkg   = { .name="pow",   .new=pow_new,   .fn=pow_bang,   .help=BINOP1 };
static t_pkg max_pkg   = { .name="max",   .new=max_new,   .fn=max_bang,   .help=BINOP1 };
static t_pkg min_pkg   = { .name="min",   .new=min_new,   .fn=min_bang,   .help=BINOP1 };
static t_pkg ee_pkg    = { .name="==",    .new=ee_new,    .fn=ee_bang,    .help=BINOP2 };
static t_pkg ne_pkg    = { .name="!=",    .new=ne_new,    .fn=ne_bang,    .help=BINOP2 };
static t_pkg gt_pkg    = { .name=">",     .new=gt_new,    .fn=gt_bang,    .help=BINOP2 };
static t_pkg lt_pkg    = { .name="<",     .new=lt_new,    .fn=lt_bang,    .help=BINOP2 };
static t_pkg ge_pkg    = { .name=">=",    .new=ge_new,    .fn=ge_bang,    .help=BINOP2 };
static t_pkg le_pkg    = { .name="<=",    .new=le_new,    .fn=le_bang,    .help=BINOP2 };
static t_pkg ba_pkg    = { .name="&",     .new=ba_new,    .fn=ba_bang,    .help=BINOP3 };
static t_pkg bo_pkg    = { .name="|",     .new=bo_new,    .fn=bo_bang,    .help=BINOP3 };
static t_pkg la_pkg    = { .name="&&",    .new=la_new,    .fn=la_bang,    .help=BINOP3 };
static t_pkg lo_pkg    = { .name="||",    .new=lo_new,    .fn=lo_bang,    .help=BINOP3 };
static t_pkg ls_pkg    = { .name="<<",    .new=ls_new,    .fn=ls_bang,    .help=BINOP3 };
static t_pkg rs_pkg    = { .name=">>",    .new=rs_new,    .fn=rs_bang,    .help=BINOP3 };
static t_pkg rem_pkg   = { .name="%",     .new=rem_new,   .fn=rem_bang,   .help=BINOP3 };
static t_pkg mod_pkg   = { .name="mod",   .new=mod_new,   .fn=mod_bang,   .help=BINOP3 };
static t_pkg div_pkg   = { .name="div",   .new=div_new,   .fn=div_bang,   .help=BINOP3 };
static t_pkg fmod_pkg  = { .name="fmod",  .new=fmod_new,  .fn=fmod_bang,  .help=BINOP3 };
static t_pkg atan2_pkg = { .name="atan2", .new=atan2_new, .fn=atan2_bang, .help=TRIG };

static t_pkg sin_pkg  = { .name="sin",  .new=sin_new,  .fn=sin_float,  .help=TRIG };
static t_pkg cos_pkg  = { .name="cos",  .new=cos_new,  .fn=cos_float,  .help=TRIG };
static t_pkg tan_pkg  = { .name="tan",  .new=tan_new,  .fn=tan_float,  .help=TRIG };
static t_pkg atan_pkg = { .name="atan", .new=atan_new, .fn=atan_float, .help=TRIG };
static t_pkg sqrt_pkg = { .name="sqrt", .new=sqrt_new, .fn=sqrt_float, .help=UNOP };
static t_pkg exp_pkg  = { .name="exp",  .new=exp_new,  .fn=exp_float,  .help=UNOP };
static t_pkg abs_pkg  = { .name="abs",  .new=abs_new,  .fn=abs_float,  .help=UNOP };


/* ------------------------------ new methods ------------------------------- */

static void *plus_new(t_floatarg f)   { return (binop_new(plus_pkg.class, f)); }
static void *minus_new(t_floatarg f)  { return (binop_new(minus_pkg.class, f)); }
static void *times_new(t_floatarg f)  { return (binop_new(times_pkg.class, f)); }
static void *over_new(t_floatarg f)   { return (binop_new(over_pkg.class, f)); }
static void *log_new(t_floatarg f)    { return (binop_new(log_pkg.class, f)); }
static void *pow_new(t_floatarg f)    { return (binop_new(pow_pkg.class, f)); }
static void *max_new(t_floatarg f)    { return (binop_new(max_pkg.class, f)); }
static void *min_new(t_floatarg f)    { return (binop_new(min_pkg.class, f)); }
static void *ee_new(t_floatarg f)     { return (binop_new(ee_pkg.class, f)); }
static void *ne_new(t_floatarg f)     { return (binop_new(ne_pkg.class, f)); }
static void *gt_new(t_floatarg f)     { return (binop_new(gt_pkg.class, f)); }
static void *lt_new(t_floatarg f)     { return (binop_new(lt_pkg.class, f)); }
static void *ge_new(t_floatarg f)     { return (binop_new(ge_pkg.class, f)); }
static void *le_new(t_floatarg f)     { return (binop_new(le_pkg.class, f)); }
static void *ba_new(t_floatarg f)     { return (binop_new(ba_pkg.class, f)); }
static void *bo_new(t_floatarg f)     { return (binop_new(bo_pkg.class, f)); }
static void *la_new(t_floatarg f)     { return (binop_new(la_pkg.class, f)); }
static void *lo_new(t_floatarg f)     { return (binop_new(lo_pkg.class, f)); }
static void *ls_new(t_floatarg f)     { return (binop_new(ls_pkg.class, f)); }
static void *rs_new(t_floatarg f)     { return (binop_new(rs_pkg.class, f)); }
static void *rem_new(t_floatarg f)    { return (binop_new(rem_pkg.class, f)); }
static void *mod_new(t_floatarg f)    { return (binop_new(mod_pkg.class, f)); }
static void *div_new(t_floatarg f)    { return (binop_new(div_pkg.class, f)); }
static void *fmod_new(t_floatarg f)   { return (binop_new(fmod_pkg.class, f)); }
static void *atan2_new(t_floatarg f)  { return (binop_new(atan2_pkg.class, f)); }

static void *sin_new(void)  { return unop_new(sin_pkg.class); }
static void *cos_new(void)  { return unop_new(cos_pkg.class); }
static void *tan_new(void)  { return unop_new(tan_pkg.class); }
static void *atan_new(void) { return unop_new(atan_pkg.class); }
static void *sqrt_new(void) { return unop_new(sqrt_pkg.class); }
static void *exp_new(void)  { return unop_new(exp_pkg.class); }
static void *abs_new(void)  { return unop_new(abs_pkg.class); }

void x_arithmetic_setup(void)
{
    t_symbol *binop1_sym = gensym("binops");
    t_symbol *binop23_sym = gensym("binops-other");
    t_symbol *trig_sym = gensym("trigonometric");
    t_symbol *unop_sym = gensym("unops");

    t_pkg *binops[] = {
        &plus_pkg, &minus_pkg, &times_pkg, &over_pkg,
        &log_pkg, &pow_pkg, &max_pkg, &min_pkg,
        &ee_pkg, &ne_pkg, &gt_pkg, &lt_pkg, &ge_pkg, &le_pkg,
        &ba_pkg, &la_pkg, &bo_pkg, &lo_pkg, &ls_pkg, &rs_pkg,
        &rem_pkg, &mod_pkg, &div_pkg, &fmod_pkg, &atan2_pkg,
        NULL
    };
    for (t_pkg **iter = binops; *iter; iter++)
    {
        t_pkg *pkg = *iter;
        pkg->class = class_new(gensym(pkg->name), (t_newmethod)pkg->new, 0,
            sizeof(t_binop), 0, A_DEFFLOAT, 0);
        class_addbang(pkg->class, pkg->fn);
        class_addfloat(pkg->class, binop_float);
        switch (pkg->help) {
            case BINOP1: class_sethelpsymbol(pkg->class, binop1_sym); break;
            case TRIG: class_sethelpsymbol(pkg->class, trig_sym); break;
            default: class_sethelpsymbol(pkg->class, binop23_sym); break;
        }
    }
    class_addcreator((t_newmethod)fmod_new, gensym("wrap"), A_DEFFLOAT, 0);

    t_pkg *unops[] = {
        &sin_pkg, &cos_pkg, &tan_pkg, &atan_pkg, &sqrt_pkg, &exp_pkg, &abs_pkg,
        NULL
    };
    for (t_pkg **iter = unops; *iter; iter++)
    {
        t_pkg *pkg = *iter;
        pkg->class = class_new(gensym(pkg->name), (t_newmethod)pkg->new, 0,
            sizeof(t_object), 0, 0);
        class_addfloat(pkg->class, binop_float);
        switch (pkg->help) {
            case TRIG: class_sethelpsymbol(pkg->class, trig_sym); break;
            default: class_sethelpsymbol(pkg->class, unop_sym); break;
        }
    }

    clip_setup();
}
