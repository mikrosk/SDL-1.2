#ifndef __GNUC__
 #error "you loose"
#endif

/*
 * Macros to set up the AES parameter block
 */
static inline void _aes_trap (AESPB * aespb)
{
	__asm__ volatile (
		"move.l	%0,%%d1\n\t"	/* &aespb */
		"move.w	#200,%%d0\n\t"
		"trap	#2"
		:
		: "g"(aespb)
		: "d0","d1","d2","a0","a1","a2","memory","cc"
	);
}
#define AES_TRAP(aespb) _aes_trap(&aespb)

#define AES_PARAMS(opcode,num_intin,num_intout,num_addrin,num_addrout) \
	static short	aes_control[AES_CTRLMAX]={opcode,num_intin,num_intout,num_addrin,num_addrout}; \
	short			aes_intin[__builtin_constant_p(num_intin) ? num_intin : -1];			  \
	/* some bindings use #num_intout == 0, but AES writes to the array anyway */ \
	short			aes_intout[__builtin_constant_p(num_intout) ? (num_intout == 0 ? 1 : num_intout) : -1]; 		  \
	long			aes_addrin[__builtin_constant_p(num_addrin) ? num_addrin : -1]; 		  \
	long			aes_addrout[__builtin_constant_p(num_addrout) ? num_addrout : -1];		  \
														  \
	AESPB aes_params;									  \
	aes_params.control = &aes_control[0];				  \
	aes_params.global  = &global_aes[0];				  \
	aes_params.intin   = aes_intin; 				  \
	aes_params.intout  = aes_intout;				  \
	aes_params.addrin  = aes_addrin;				  \
	aes_params.addrout = aes_addrout

/* to avoid "dereferencing type-punned pointer" */
static __inline long *__aes_intout_long(short n, short *aes_intout)
{
	return ((long *)(aes_intout   +n));
}
#define aes_intout_long(n)  *__aes_intout_long(n, aes_intout)

static __inline long *__aes_intin_long(short n, short *aes_intin)
{
	return ((long *)(aes_intin   +n));
}
#define aes_intin_long(n)  *__aes_intin_long(n, aes_intin)

static __inline void **__aes_intout_ptr(short n, short *aes_intout)
{
	return ((void **)(aes_intout   +n));
}
#define aes_intout_ptr(n, t)  *((t *)__aes_intout_ptr(n, aes_intout))

static __inline void **__aes_intin_ptr(short n, short *aes_intin)
{
	return ((void **)(aes_intin   +n));
}
#define aes_intin_ptr(n, t)  *((t *)__aes_intin_ptr(n, aes_intin))

/* special feature for AES bindings: pointer in parameters (for return values)
 * could be NULL (nice idea by Martin Elsasser against dummy variables) 
 */
#define CHECK_NULLPTR 0


