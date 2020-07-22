#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "puzzle.h"

#define MAX_PAD 5
#define TRUE 1
#define FALSE 0


/* create array to hold the formatted CW Puzzle display */
static char display[WIDTH][HEIGHT];

/* function prototype */
int printStr( const char *str, int i, int j, const int num, const char mode );


/* function "printCW" prints the crossWord puzzle into the array "display" */
long *setCW( const CrossWord p[], int size ) {
   int i = 0;       /* horizontal index */
   int j = 2;       /* vertical index */
   int index;       /* index of the array "p" */
   int vertOff;     /* vertical offset */
   int horizOff;    /* horizontal offset */
   int intsctWith;  /* index of the string which the present string intersected with */
   int hSpace;      /* horizontal space */
   int pHTrail = 0; /* previous horizontal trail */
   int pVOff = 0;   /* previous vertical offset */
   int vertTrail;   /* corrent vertical trail */
   int farJ = 0;
   short jumpedTrail; /* signifies if previous vertical trail was jumped */

   /* create array to remember vital information about the words printed on array "display" */
   static long remember[WORDS];

   /* clear display */
   for ( j = 0; j < HEIGHT; j++ ) {

      for ( i = 0; i < WIDTH; i++ ){
         display[i][j] = '\0';
      } /* end inner for */

   } /* end outer for */

   /* loop for every puzzle word in the first half of the array "p" */
   i = 0;
   j = 2;
   for ( index = 0; index < ( size + 1 ) / 2; index++ ) {
      hSpace = WIDTH - i;
      jumpedTrail = FALSE;

      /* if there is no intersection assign 0 to "vertOff" and "HorizOff"; otherwise assign appropriate values to them */
      if ( p[index].intersect == NO_INTERSECT ) {
         vertOff = 0;
         horizOff = 0;
      }
      else {
         intsctWith = p[index].intersect / 100;
         vertOff = 2 * ( 1 + p[intsctWith].intersect % 100 );
         horizOff = 2 * ( 1 + p[index].intersect % 100 );
      }

      /* assign appropriate values to "i" and "j" based on the availablility of enough horizontal space or not */
      if ( pVOff == vertOff ) {

         if ( hSpace >= ( 2 + strlen( p[index].word ) ) * 2 ) {  /* there is enough horizontal space */
            i += 2;
         }
         else if ( horizOff > pHTrail ) {

            if ( hSpace + pHTrail >= ( 2 + strlen( p[index].word ) ) * 2 ) {
               j += 4;
               i = WIDTH - ( hSpace + pHTrail ) + 2;
            }
            else {
               j += ( farJ - j ) + vertOff + 2;
               i = rand() % MAX_PAD;
               jumpedTrail = TRUE;
            } /* end else */

         } /* end else if */
         else if ( horizOff <= pHTrail ) {

            if ( hSpace + horizOff >= ( 2 + strlen( p[index].word ) ) * 2 ) {
               j += 4;
               i = WIDTH - ( hSpace + horizOff ) + 2;
            }
            else {
               j += ( farJ - j ) + vertOff + 2;
               i = rand() % MAX_PAD;
               jumpedTrail = TRUE;
            } /* end else */

         } /* end else if */

      } /* end outermost if */
      else {  /* pVOff != vertOff */

         if ( horizOff > pHTrail ) {

            if ( hSpace + pHTrail >= ( 2 + strlen( p[index].word ) ) * 2 ) {
               j += vertOff - pVOff;
               i = WIDTH - ( hSpace + pHTrail ) + 2;
            }
            else {
               j += ( farJ - j ) + vertOff + 2;
               i = rand() % MAX_PAD;
               jumpedTrail = TRUE;
            } /* end else */

         } /* end if */
         else { /* horizOff <= pHTrail */

            if ( hSpace + horizOff >= ( 2 + strlen( p[index].word ) ) * 2 ) {
               j += vertOff - pVOff;
               i = WIDTH - ( hSpace + horizOff ) +  2;
            }
            else {
               j += ( farJ - j ) + vertOff + 2;
               i = rand() % MAX_PAD;
               jumpedTrail = TRUE;
            } /* end innermost else */

         } /* end inner else */

      } /* end outer else */


      /* test if "j" is out of bounds */
      if ( p[index].intersect == NO_INTERSECT ) {

         if ( j >= HEIGHT ) {
            return NULL;
         } /* end inner if */

      } /* end outer if */
      else {
         vertTrail = 2 * strlen( p[intsctWith].word ) - vertOff;

         if ( j + 1 + vertTrail >= HEIGHT ) {
            return NULL;
         } /* end if */

      } /* end else */


      /* print the string on "display" */
      printStr( p[index].word, i, j, index + 1, 'H' );

      /* store the length of the string, the "i" and the "j" into the external array "remember" for future use */
      remember[index] = ( strlen( p[index].word ) * 10000 ) + ( i * 100 ) + j;

      /* if there is no intersection, update relevant variables; else print the intersect-word too */
      if ( p[index].intersect == NO_INTERSECT ) {
         pHTrail = 2 * strlen( p[index].word ) - horizOff;
         pVOff = vertOff;
      }
      else {
         printStr( p[intsctWith].word, i + horizOff, j - vertOff, intsctWith + 1, 'V' );

         /* store the length of the string, the "i" and the "j" into the external array "remember" for future use */
         remember[intsctWith] = ( strlen( p[intsctWith].word ) * 10000 ) + ( ( i + horizOff ) * 100 ) + ( j - vertOff );

         /* update the values of relevant variables */
         pHTrail = 2 * strlen( p[index].word ) - horizOff;
         pVOff = vertOff;
      }

      /* update the values of pVTrail as appropriate */
      if ( jumpedTrail ) {

         if ( p[index].intersect == NO_INTERSECT ) {
            farJ = j + 2;
         }
         else {
             farJ = j + 2 + vertTrail;
         } /* end else */

      } /* end if */
      else {

         if ( p[index].intersect != NO_INTERSECT && j + 2 + vertTrail > farJ ) {
            farJ = j + 2 + vertTrail;
         } /* end if */

      } /* end else */

      i += ( 1 + strlen( p[index].word ) ) * 2;
   } /* end for */

   for ( ; index < size; index++ ) {

      if ( p[index].intersect == NO_INTERSECT ) {
         i += 2;

         if ( i + 1 < WIDTH && j + ( 1 + strlen( p[index]. word ) ) * 2 < HEIGHT ) {
            printStr( p[index].word, i, j, index + 1, 'V' );

            /* store the length of the string, the "i" and the "j" into the external array "remember" for future use */
            remember[index] = ( strlen( p[index].word ) * 10000 ) + ( i * 100 ) + j;
         }
         else {
            return NULL;
         }
      }
   }

   return remember;
} /* end function "printCW" */


/* function "printStr" prints a string on the array display;
   'H' or 'V' as its last argument means print the string horizontally or vertically respectively;
*/
int printStr( const char *str, int i, int j, const int num, const char mode ) {

   switch ( mode ) {
      case 'H':
         display[i][j] = '0' + num;
         ++i;
         display[i][j] = '|';
         ++i;
         break;

      case 'V':
         display[i][j] = '0' + num;
         ++j;
         display[i][j] = '_';
         ++j;
         break;
       default:
         return -1;
    } /* end switch */

   for ( ; *str != '\0'; str++ ) {

      switch ( mode ) {
         case 'H':
            display[i][j] = *str;
            display[i][j + 1] = '_';
            display[i][j - 1] = '_';
            ++i;
            display[i][j] = '|';
            ++i;
            break;

         case 'V':
            display[i][j] = *str;
            display[i + 1][j] = '|';
            display[i - 1][j] = '|';
            ++j;
            display[i][j] = '_';
            ++j;
            break;
      } /* end switch */

   } /* end for */

   return 1;

} /* end function "printStr" */


/* function "showDisplay" prints display on standard output */
void showDisplay( void ) {
   int i, j;

   for ( i = 1; i <= WIDTH + 2; i++ ) {
         printf( "%c", '=' );
   }

   putchar( '\n' );
   for ( j = 0; j < HEIGHT; j++ ) {

      putchar( '=' );
      for ( i = 0; i < WIDTH; i++ ){
         putchar( display[i][j] );
      }

      putchar( '=' );
      putchar( '\n' );

   } /* end for */

   for ( i = 1; i <= WIDTH + 2; i++ ) {
         printf( "%c", '=' );
   }

} /* end function "showDisplay" */


/* function "updateCW" updates the CrossWord puzzle with a new string. NB: this function does not do error checking */
void updateCW( char *str, int num, const long *rememb ) {
   str[rememb[num - 1] / 10000] = '\0';

   printStr( str, ( rememb[num - 1] % 10000 ) / 100, rememb[num - 1] % 100, num, ( num <= ( WORDS + 1 ) / 2 ) ? 'H' : 'V' );
} /* end function "updateCW" */
