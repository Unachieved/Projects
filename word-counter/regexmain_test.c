#ifdef USE_SUBMITTY_MAIN

/* gcc -Wall -Werror -D USE_SUBMITTY_MAIN *.c   */

/* ./a.out regex1234.txt hw1-input01.txt        */

/* hw2-regex-test.c (F19) */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int regex_match( const char * filename, const char * regex, char *** matches );

int main( int argc, char * argv[] )
{
  if ( argc != 3 )
  {
    fprintf( stderr, "ERROR: Invalid arguments\n" );
    fprintf( stderr, "USAGE: %s <regex-file> <input-file>\n", argv[0] );
    return EXIT_FAILURE;
  }

  FILE * regexfile = fopen( argv[1], "r" );

  if ( regexfile == NULL )
  {
    perror( "fopen() failed" );
    return EXIT_FAILURE;
  }

  //char * regex = NULL;
  char regex[50];

  if ( fscanf( regexfile, "%s[^\n]", regex ) != 1 )
  {
    //free( regex );
    fprintf( stderr, "ERROR: regex file is incorrect\n" );
    return EXIT_FAILURE;
  }

  fclose( regexfile );

  //#ifdef DEBUG_MODE
  printf( "REGEX: \"%s\"\n", regex );
  //#endif


  /* call regex_match() to find matches of a given regex   */
  /*  in a given file (argv[2]); lines must be dynamically */
  /*   allocated in the regex_match() function call        */
  char ** lines = NULL;
  int matched_lines = regex_match( argv[2], regex, &lines );
  int i;

  /* display resulting matched lines */
  for ( i = 0 ; i < matched_lines ; i++ )
  {
    printf( "%s\n", lines[i] );
  }

  /* deallocate dynamically allocated memory */
  for ( i = 0 ; i < matched_lines ; i++ )
  {
    free( lines[i] );
  }

  free( lines );
  return EXIT_SUCCESS;
}

#endif