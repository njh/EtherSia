#ifndef Progmem_h
#define Progmem_h

#define PROGMEM
#define F(x) x
#define pgm_read_byte_near(x) *(x)
#define memcpy_P(dst, src, n) memcpy(dst, src, n)
#define memcmp_P(p1, p2, n) memcmp(p1, p2, n)
#define strcmp_P(s1, s2) strcmp(s1, s2)
#define strlen_P(str) strlen(str)
#define pgm_read_byte(addr) *(addr);

class __FlashStringHelper;

#endif