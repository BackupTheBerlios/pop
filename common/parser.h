#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdio.h>

typedef struct
{
 const char *str;
 int apos;
} parss;

typedef struct
{
 parss apars;
 char *temps;
 FILE *f;
} parsf;

int iszeichen(char c);
parss startparse(const char *str);
parsf startparsef(const char *fname);
void endparsef(parsf *f);
int parseline(parss *s,char *buf);
int parsefile(parsf *f,char *buf);

#endif

