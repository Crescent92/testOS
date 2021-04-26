#ifndef _SIG_TRMP_
#define _SIG_TRMP_

#define SIGRET  6
/* 
 * User level signal trampoline function.
 * Used to return back from a signal
 * handler.
 */

void sig_trampoline(void);

#endif