/* this program implements crossword puzzle */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "puzzle.h"

#define upperCase( str ) { int j; for ( j = 0; str[j] != '\0'; j++ ) \
                                     str[j] = toupper( str[j] ); }

void asgnIntsct( CrossWord p[], int size );
long *setCW( const CrossWord p[], int size );
void showDisplay( void );
void makeEmpty( CrossWord *p1, const CrossWord *p0, int size );
void updateCW( char *str, int num, const long *rememb );
int getPuzzle( CrossWord puzz[], int size );
void showHints( int size );
int loadPuzz ( CrossWord *p1, const CrossWord *p0, int size );
int loadMaster( CrossWord *mastPuzz, int size );
void savePuzz( const CrossWord *puzz, int size );


/* function "main" begins program execution */
int main( void ) {
   int choice = -9;
   int i;
   long *sett;

   /* create array to hold the words of the puzzle */
   CrossWord masterPuzz[WORDS];

   /* create array to hold the unfilled puzzle */
   CrossWord puzzle[WORDS];

   printf( "*** Welcome to IfeCrossWord Puzzles (c) 2015 ***\n"
           "*** I hope you enjoy your game ***\n" );

   do {
      /* accept and process input */
      printf( "\n\nDo you want to start a new game or continue your saved game?\n"
              "   1 -- Start new puzzle.\n"
              "   2 -- Load saved puzzle.\n" );
      scanf( "%d", &choice );
      fflush( stdin );

      /* validate user input */
      while ( choice != 1 && choice != 2 ) {
         printf( "\nInvalid coice!" );
         printf( "\n\nDo you want to start a new game or continue your saved game?\n"
                 "   1 -- Start new puzzle.\n"
                 "   2 -- Load saved puzzle.\n" );
         scanf( "%d", &choice );
         fflush( stdin );
      }

      /* perform appropriate action based on user's input */
      if ( choice == 1 ) {  /* start a new puzzle */

         /* get a random puzzle from dictionary file; exit if an error occurs while getting the puzzle */
         if ( getPuzzle( masterPuzz, WORDS ) == EOF ) {
            return 1;
         }
         else {
            /* assign appropriate intersections to the puzzle words */
            asgnIntsct( masterPuzz, WORDS );

            /* create an empty puzzle using "masterPuzz" as template */
            makeEmpty( puzzle, masterPuzz, WORDS );
         } /* end inner else */

      } /* end outer if */
      else {  /* choice == 2, load saved puzzle */
         /* get saved puzzle from dictionary file; exit if error occurs */
         if ( loadMaster( masterPuzz, WORDS ) == EOF ) {
            return 1;
         }
         else {
            /* assign appropriate intersections to the puzzle words */
            asgnIntsct( masterPuzz, WORDS );

            /* load the puzzle as was previously saved by user */
            if ( loadPuzz( puzzle, masterPuzz, WORDS ) == EOF ) {
               return 1;
            } /* end innermost if */

         } /* end inner else */

      } /* end outer else */


      /* set the puzzle ready to be displayed, then play game; exit if an error occurs */
      if ( ( sett = setCW( puzzle, WORDS ) ) == NULL ) {
         printf( "Error in setting up the puzzle for display.\n" );
         return 2;
      }

      /* accept and process user's input */
      printf( "\n\n"
              "  1 to 5 -- Specify a word you want to try to guess.\n"
              "  -1     -- Print the crossword puzzle.\n"
              "  -2     -- Show Hints.\n"
              "  -8     -- Start over.\n"
              "  -9     -- Quit.\n"
              "Enter a choice: " );
      scanf( "%d", &choice );
      fflush( stdin );

      while ( choice != -9 && choice != -8 ) {

         if ( choice >= 1 && choice <= WORDS ) {  /* enter a guess */
            printf( "Alright, type the guess you want to place in word no. %d:\n", choice );
            scanf( "%19[^\n]", puzzle[choice - 1].word );
            fflush( stdin );

            upperCase( puzzle[choice - 1].word );
            updateCW( puzzle[choice - 1].word, choice, sett );

            /* take care of the intersection if any */
            if ( !( puzzle[choice - 1].intersect == NO_INTERSECT ) ) {
               puzzle[puzzle[choice - 1].intersect / 100].word[puzzle[puzzle[choice - 1].intersect / 100].intersect % 100]
                  = puzzle[choice - 1].word[puzzle[choice - 1].intersect % 100];
            } /* end inner if */

         } /* end outer if */
         else if ( choice == -1 ) {  /* print the puzzle */
            showDisplay();
         } /* end else if */
         else if ( choice == -2 ) {  /* show hints */
            showHints( WORDS );
         }
         else {
            printf( "Invalid command.\n" );
         }

         /* prompt for another input */
         printf( "\n\n"
                 "  1 to 5 -- Specify a word you want to try a guess.\n"
                 "  -1     -- Print the crossword puzzle.\n"
                 "  -2     -- Show Hint.\n"
                 "  -8     -- Start over.\n"
                 "  -9     -- Quit.\n"
                 "Enter anoter choice: " );
         scanf( "%d", &choice );
         fflush( stdin );
      } /* end while */

      /* if user entered "-9" to indicate "Quit", determine if user won or lost the game */
      if ( choice == -9 ) {

         /* check if the crossword puzzle was completed correctly and print the appropriate message */
         for ( i = 0; i < WORDS && strcmp( puzzle[i].word, masterPuzz[i].word ) == 0; i++ )
            ;

         if ( i == WORDS ) {
            printf( "\nCongratulations! You won.\n" );
         }
         else {
            printf( "\nSorry, you did't win. Better luck next time.\n" );
         }

         /* save the puzzle if the user so wishes */
         printf( "\nIt will be nice to save your game before you quit\n"
                 "so that you don\'t loose your progress. What do you think\?\n\n"
                 "  1    -- Save game before quitting.\n"
                 "  2    -- Quit without saving game.\n" );
         scanf( "%d", &choice );

         if ( choice == 1 ) {
            savePuzz( puzzle, WORDS );
         } /* end inner if */

      } /* end outer if */


   } while ( choice == -8 );  /* loop while user entered "-8" to indicate "Start over" */


   return 0;
} /* end function "main" */


/* function "asgnIntsct" assigns specific intersection to each word of the puzzle */
void asgnIntsct( CrossWord p[], int size ) {
   int i, j;
   int found;  /* indicates if an intersection have been found */
   char *intsctPtr;

   /* set all the words in the puzzle to default intersection */
   for ( i = 0; i < size; i++ ) {
      p[i].intersect = NO_INTERSECT;
   }

   /* assign intersections where applicable */
   /* loop for every puzzle word in the first half of the array */
   for ( i = 0; i < ( size + 1 ) / 2; i++ ) {

      found = 0;
      j = ( size + 1 ) / 2;
      while ( !found && j < size ) {

         /* if a common character occurs, set the value of the structure members "intersect" appropriately */
         if ( p[j].intersect == NO_INTERSECT && ( intsctPtr = strpbrk( p[i].word, p[j].word ) ) != NULL ) {
            /* set "intersect" for p[i] */
            p[i].intersect = j * 100 + ( intsctPtr - &p[i].word[0] );

            /* find the position in "p[j].word" of the common character */
            intsctPtr = strchr( p[j].word, *intsctPtr );

            /* set "intersect" for p[j] */
            p[j].intersect = i * 100 + ( intsctPtr - &p[j].word[0] );

            found = 1;
         } /* end if */

         ++j;
      } /* end while */

   } /* end for */

} /* end function "asgnIntsct" */


/* function "makeEmpty" empties the pussle "p1" using "p0" as the master puzzle */
void makeEmpty( CrossWord *p1, const CrossWord *p0, int size ) {
   int i, j;

   for ( i = 0; i < size; i++ ) {

      /* put a blank in "p1" for every character in "p0" */
      for ( j = 0; p0[i].word[j] != '\0'; j++ ) {
         p1[i].word[j] = ' ';
      }
      p1[i].word[j] = '\0';

      /* set the intersection in "p1" */
      p1[i].intersect = p0[i].intersect;
   } /* end outer for */

} /* end function "makeEmpty" */
