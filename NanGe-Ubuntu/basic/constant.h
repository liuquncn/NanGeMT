/* Constant.h */

#ifndef CONSTANT_H
#define CONSTANT_H

const int LANGNUM=5;
enum LANGCODE
{
  CHINESE,
  ENGLISH,
  MONGOLIAN,
  TIBETAN,
  JAPANESE,
  SEMANTIC=-1
};
const char * const LANGSTRING[] =
{
  "CHINESE",
  "ENGLISH",
  "MONGOLIAN",
  "TIBETAN",
  "JAPANESE"
};
/*
const int CHINESE=0;
const int ENGLISH=1;
const int MONGOLIAN=2;
const int TIBETAN=3;
const int JAPANESE=4;
const int SEMANTIC=-1;
*/

const int FILENAMELENGTH=120;
const int COMMANDLENGTH=250;
const int WORDLENGTH=32;
const int TEXTLENGTH=1024;
const int SENTLENGTH=1024;
const int LINELENGTH=1024;
const int TREELENGTH=32*SENTLENGTH;

const int SUCC=1;
const int FAIL=0;

const int YES=1;
const int NO=0;

const int WRONG=-1;

#ifndef TRUE
const int TRUE=1;
const int FALSE=0;
typedef int BOOL;
#endif

#endif // CONSTANT_H
