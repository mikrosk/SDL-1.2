/*
 * Macros to set up the VDI parameter block
 */

static inline void
_vdi_trap_esc (VDIPB * vdipb,
               long cntrl_0_1, long cntrl_3, long cntrl_5, short handle)
{
	__asm__ volatile (
		"movea.l	%0,%%a0\n\t"	/* &vdipb */
		"move.l	%%a0,%%d1\n\t"
		"move.l	(%%a0),%%a0\n\t"	/* vdipb->control */
		"move.l	%1,(%%a0)+\n\t"	/* cntrl_0, cntrl_1 */
		"move.l	%2,(%%a0)+\n\t"	/* cntrl_2, cntrl_3 */
		"move.l	%3,(%%a0)+\n\t"	/* cntrl_4, cntrl_5 */
		"move.w	%4,(%%a0)\n\t"	/* handle */
		"move.w	#115,%%d0\n\t"	/* 0x0073 */
		"trap	#2"
		:
		: "g"(vdipb), "g"(cntrl_0_1), "g"(cntrl_3), "g"(cntrl_5), "g"(handle)
		: "d0", "d1", "d2", "a0", "a1", "a2", "memory", "cc"
	);
}
#define VDI_TRAP_ESC(vdipb, handle, opcode, subop, cntrl_1, cntrl_3) \
	_vdi_trap_esc (&vdipb, (opcode##uL<<16)|cntrl_1, cntrl_3, subop, handle)

static inline void
_vdi_trap_00 (VDIPB * vdipb, long cntrl_0_1, short handle)
{
	__asm__ volatile (
		"movea.l %0,%%a0\n\t"	/* &vdipb */
		"move.l  %%a0,%%d1\n\t"
		"move.l  (%%a0),%%a0\n\t"	/* vdipb->control */
		"move.l  %1,(%%a0)+\n\t"	/* cntrl_0, cntrl_1 */
		"moveq   #0,%%d0\n\t"
		"move.l  %%d0,(%%a0)+\n\t"	/* cntrl_2, cntrl_3 */
		"move.l  %%d0,(%%a0)+\n\t"	/* cntrl_4, cntrl_5 */
		"move.w  %2,(%%a0)\n\t"	/* handle */
		"move.w  #115,%%d0\n\t"	/* 0x0073 */
		"trap    #2"
		:
		: "g"(vdipb), "g"(cntrl_0_1), "g"(handle)
		: "d0", "d1", "d2", "a0", "a1", "a2", "memory", "cc"
	);
}
#define VDI_TRAP_00(vdipb, handle, opcode) \
	_vdi_trap_00 (&vdipb, (opcode##uL<<16), handle)

#define VDI_TRAP(vdipb, handle, opcode, cntrl_1, cntrl_3) \
	VDI_TRAP_ESC(vdipb, handle, opcode, 0, cntrl_1, cntrl_3)

#define VDI_PARAMS(a,b,c,d,e) \
	VDIPB vdi_params;         \
	vdi_params.control = a;   \
	vdi_params.intin   = b;   \
	vdi_params.ptsin   = c;   \
	vdi_params.intout  = d;   \
	vdi_params.ptsout  = e;

/* special feature for VDI bindings: set VDIPB::intout and VDIPB::ptsout to
 * vdi_dummy array intead of NULL against crashes if some crazy VDI drivers
 * tries to write something in ptsout/intout.
 */ 
#define USE_VDI_DUMMY 1

#if USE_VDI_DUMMY
	/* use dummy array vdi_dummy[] from vdi_dummy.c */
extern short vdi_dummy[];
#else
	/* replace vdi_dummy in VDIPB by NULL pointer */
	#define vdi_dummy 0L
#endif

#define N_PTRINTS (sizeof(void *) / sizeof(short))

#ifdef __GNUC__

/* to avoid "dereferencing type-punned pointer" */
static __inline long *__vdi_array_long(short n, short *array)
{
	return ((long *)(array   +n));
}

static __inline void **__vdi_array_ptr(short n, short *array)
{
	return ((void**)(array + n));
}

#else

#define __vdi_array_ptr(n, array)   ((void **)(array + (n)))
#define __vdi_array_long(n, array)   ((long *)(array + (n)))

#endif

#define vdi_intin_long(n)  *__vdi_array_long(n, vdi_intin)
#define vdi_intout_long(n)  *__vdi_array_long(n, vdi_intout)
#define vdi_ptsin_long(n)  *__vdi_array_long(n, vdi_ptsin)
#define vdi_ptsout_long(n)  *__vdi_array_long(n, vdi_ptsout)

#define vdi_control_ptr(n, t)  *((t *)__vdi_array_ptr(7 + (n) * N_PTRINTS, vdi_control))
#define vdi_intin_ptr(n, t)  *((t *)__vdi_array_ptr(n, vdi_intin))
#define vdi_intout_ptr(n, t)  *((t *)__vdi_array_ptr(n, vdi_intout))
#define vdi_ptsin_ptr(n, t)  *((t *)__vdi_array_ptr(n, vdi_ptsin))
#define vdi_ptsout_ptr(n, t)  *((t *)__vdi_array_ptr(n, vdi_ptsout))

