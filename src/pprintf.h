#ifndef _PPRINTF_H_
#define _PPRINTF_H_

#define PPRINTF_PROMPT "--MORE--"

void pprintf_init();
int pprintf(const char *format, ...);
int pputchar(int c);

#endif /* _PPRINTF_H_ */
