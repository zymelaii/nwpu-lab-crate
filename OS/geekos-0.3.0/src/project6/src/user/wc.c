/*
 * ========================================================================
 *
 *          Project 5: 
 *
 *
 *
 *  By:
 *      Jeff Hollingsworth
 *  e-mail:
 *      hollings@cs.umd.edu
 *
 *  File:        wc.c
 *  Created on:  04/19/1996
 * 
 *  Contents:    A simple wc, read stdin and write count of lines and chars
 *               to stdout.
 * 
 *
 * ========================================================================
 */

#include <conio.h>
#include <process.h>
#include <fileio.h>
#include <string.h>

extern void exit(int);

int isspace(char ch)
{
    if ((ch == ' ') || (ch == '\t')) return 1;

    return 0;
}

int  main(int argc , char ** argv)
{
  int i;
  int ret;
  int inFd;
  char *ch;
  int done;
  int inSpaceMode;
  char buffer[512];
  int words, bytes, lines;
  int doBytes, doWords, doLines;

  doBytes = doWords = doLines = 1;

  inFd = 0;
  /* defaults back to off */
  for (i=1; i < argc; i++) {
     if (argv[i][0] == '-') {
	 doLines = doBytes = doWords = 0;
         ch = argv[i]+1;
         while (*ch) {
             switch (*ch) {
                 case 'l':
                        doLines++;
                        break;
                 case 'w':
                        doWords++;
                        break;
                 case 'c':
                        doBytes++;
                        break;
                 default:
                        Print("Invalid argument %s\n", argv[i]);
			Exit(1);
                        break;
             }
             ch++;
         }
     } else {
	 inFd = Open(argv[i], O_READ);
	 if (inFd <= 0) {
	     Print("Error opening %s: %s\n", argv[i], Get_Error_String(inFd));
	     Exit(1);
	 }
         break;
     }
  }

  done = 0;
  memset(buffer, '\0', sizeof(buffer));
  inSpaceMode = words = bytes = lines = 0;
  while (!done) {
      ret = Read(inFd, buffer, sizeof(buffer));
      if (ret <= 0) break;

      bytes += ret;

      for (i=0; i < ret; i++) {
          if ((buffer[i] == '\n') ||
              (buffer[i] == '\r')) {
              lines++;
	      if (!inSpaceMode) words++;
          }

          if (isspace(buffer[i])) {
             if (!inSpaceMode) words++;
             inSpaceMode = 1;
          } else {
             inSpaceMode = 0;
          }
      }
      memset(buffer, '\0', sizeof(buffer));
  }

  if(doLines) Print("%d    ", lines);
  if(doWords) Print("%d    ", words);
  if(doBytes) Print("%d    ", bytes);
  Print("\n");

  return 0;
}
