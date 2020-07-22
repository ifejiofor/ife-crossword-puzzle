#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "puzzle.h"

#define inputStr( str, size, fPtr ) { int j; \
                                         for ( j = 0; j < size - 1 && ( str[j] = fgetc( fPtr ) ) != '\t'; j++ ) ; \
                                         str[j] = '\0'; }
#define MAX 10


/* function "getPuzzle" gets a random set of words for the CrossWord puzzle from a text file */
int getPuzzle( CrossWord puzz[], int size ) {
   int randIndex[size];  /* array to hold random indexes of words to be taken from the file */
   int i;
   int index;
   int maxIndex;
   char c;
   FILE *filePtr;
   srand( time( NULL ) );

   /* open dictionary file; exit if dictionary file could not be opened */
   if ( ( filePtr = fopen( "Dictionary.txt", "r" ) ) == NULL ) {
      printf( "Dictionary file could not be opened.\n" );
      return EOF;
   }
   else {

      /* fill the array with random indexes */
      for ( i = 0; i < size; i++ ) {
         randIndex[i] = rand() % MAX;

         /* eliminate any repetition */
         int j;
         for ( j = 0; j < i; j++ ) {

            if ( randIndex[j] == randIndex[i] ) {  /* repetition detected */
               randIndex[i] = rand() % MAX;
               j = -1;  /* set "j" to -1 so that after "j++" statement in the for is executed, "j" will be 0 */
            } /* end if */

         } /* end inner for */

      } /* end outer for */

      /* determine the maximum index */
      maxIndex = randIndex[0];
      for ( i = 1; i < size; i++ ) {

         if ( randIndex[i] > maxIndex ) {
            maxIndex = randIndex[i];
         } /* end if */

      } /* end for */


      index = 0;

      while ( index <= maxIndex ) {

         /* check if "index" matches with any value in "randIndex" */
         for ( i = 0; i < size && randIndex[i] != index ; i++ )
            ;

         if ( i < size ) {  /* randIndex[i] matches with index */
            inputStr( puzz[i].word, LENGTH, filePtr );
         }

         /* find the end of the present line */
         while ( ( c = fgetc( filePtr ) ) != '\n' && !feof( filePtr ) )
            ;

         if ( c == '\n' ) {
            ++index;
         }
         else {
            printf( "Something is wrong with the dictionary file.\n" );
            fclose( filePtr );
            return EOF;
         } /* end else */

      } /* end while */

      fclose( filePtr );
   } /* end else */

   /* open the file "Indexes.dat" to save the random indexes; exit if the file could not be opened */
   if ( ( filePtr = fopen( "Indexes.dat", "wb" ) ) == NULL ) {
      printf( "Error! File to save indexes cannot be opened.\n" );
      return EOF;
   }
   else {

      for ( i = 0; i < size; i++ ) {
         fwrite( &randIndex[i], sizeof(randIndex[i]), 1, filePtr );
      }

      fclose( filePtr );
   } /* end else */

   return i;

} /* end function "getPuzzle" */


/* functon "showHints" prints the hints of the crossword puzzle on the standard output */
void showHints( int size ) {
   int randIndex[size];
   int i;
   int index;
   char c;
   FILE *fPtr;

   /* open file "Indexes.dat" to read indexes; exit if file could not be opened */
   if ( ( fPtr = fopen( "Indexes.dat", "rb" ) ) == NULL ) {
      printf( "Error! File to read indexes cannot be opened.\n" );
      return;
   }
   else {

      for ( i = 0; i < size; i++ ) {
         fread( &randIndex[i], sizeof(randIndex[i]), 1, fPtr );
      }

      fclose( fPtr );
   } /* end else */

   /* open the dictionary file; exit if the file cannot be opened */
   if ( ( fPtr = fopen( "Dictionary.txt", "r" ) ) == NULL ) {
      printf( "Error! Dictionary file could not be opened.\n" );
      return;
   }
   else {
      printf( "\nHints:\n" );

      for ( i = 0; i < size; i++ ) {
         rewind( fPtr );
         printf( "  %d - ", i + 1  );

         /* search for the appropriate word in the dictionary file */
         index = 0;
         while ( !feof( fPtr ) && index < randIndex[i] ) {

            if ( fgetc( fPtr ) == '\n' ) {
               ++index;
            } /* end if */

         } /* end while */

         /* skip the name of the word */
         while ( !feof( fPtr ) && isalpha( fgetc( fPtr ) ) )
            ;

         /* skip the spaces seperating the word and its meaning */
         while ( !feof( fPtr ) && isspace( c = fgetc( fPtr ) ) )
            ;

         fputc( c, stdout );
         while ( !feof( fPtr ) && ( c = fgetc( fPtr ) ) != '\n' ) {
            fputc( c, stdout );
         }

         fputc( '\n', stdout );
      } /* end for */

      fclose( fPtr );
   } /* end else */

} /* end function "showHints" */


/* function "savePuzz" save the puzzle in a file */
void savePuzz( const CrossWord *puzz, int size ) {
   FILE *fPtr = NULL, *fPtr1 = NULL;
   int i;
   int index;

   /* open the file to save game; exit if file could not be opened */
   if ( ( fPtr = fopen( "SavedGame.dat", "wb" ) ) == NULL ) {
      printf( "File to load saved puzzle cannot be opened.\n" );
   }
   else {

      /* write the puzzle filled by the user into the file */
      for ( i = 0; i < size; i++ ) {
         fwrite( &puzz[i].word, LENGTH * sizeof(char), 1, fPtr );
      }

      fclose( fPtr );
   }

   /* save the indexes in "Indexes.dat" and save them in "SavedIndexes.dat" */
   /* open the the respective files; exit any of the files could not be opened */
   if ( ( fPtr = fopen( "SavedIndexes.dat", "wb" ) ) == NULL || ( fPtr1 = fopen( "Indexes.dat", "rb" ) ) == NULL ) {
      printf( "Error in saving puzzle: File could not be opened.\n" );
   }
   else {

      for ( i = 0; i < size; i++ ) {
         fread( &index, sizeof(int), 1, fPtr1 );
         fwrite( &index, sizeof(int), 1, fPtr );
      } /* end for */

   } /* end else */

   if ( fPtr != NULL ) {
      fclose( fPtr );
   }

   if ( fPtr1 != NULL ) {
      fclose( fPtr1 );
   }

} /* end function "savePuzz" */


/* function "loadMaster" loads a saved the master puzzle from the file "SavedGame.dat" */
int loadMaster( CrossWord *mastPuzz, int size ) {
   FILE *fPtr, *fPtr1;
   int randIndex[size];
   int i;
   int index;
   int maxIndex;
   char c;

   /* open the file to save game; exit if file could not be opened */
   if ( ( fPtr = fopen( "SavedIndexes.dat", "rb" ) ) == NULL ) {
      printf( "File to save puzzle cannot be opened.\n" );
      return EOF;
   }
   else {

      if ( ( fPtr1 = fopen( "Indexes.dat", "wb" ) ) == NULL ) {
         printf( "File of indexes could not be opened.\n" );
         return EOF;
      }

      /* read the random indexes from the file, also write them into "Indexes.dat" */
      for ( i = 0; i < size; i++ ) {
         fread( &randIndex[i], sizeof(int), 1, fPtr );
         fwrite( &randIndex[i], sizeof(int), 1, fPtr1 );
      }

      fclose( fPtr );
      fclose( fPtr1 );
   } /* end else */


   /* open dictionary file exit if dictionary file could not be opened */
   if ( ( fPtr = fopen( "Dictionary.txt", "r" ) ) == NULL ) {
      printf( "Dictionary file could not be opened.\n" );
   }
   else {
      /* determine the maximum index */
      maxIndex = randIndex[0];
      for ( i = 1; i < size; i++ ) {

         if ( randIndex[i] > maxIndex ) {
            maxIndex = randIndex[i];
         } /* end if */

      } /* end for */


      index = 0;

      while ( index <= maxIndex ) {

         /* check if "index" matches with any value in "randIndex" */
         for ( i = 0; i < size && randIndex[i] != index ; i++ )
            ;

         if ( i < size ) {  /* randIndex[i] matches with index */
            inputStr( mastPuzz[i].word, LENGTH, fPtr );
         }

         /* find the end of the present line */
         while ( ( c = fgetc( fPtr ) ) != '\n' && !feof( fPtr ) )
            ;

         if ( c == '\n' ) {
            ++index;
         }
         else {
            printf( "Something is wrong with the dictionary file.\n" );
            fclose( fPtr );
            return EOF;
         } /* end else */

      } /* end while */

      fclose( fPtr );
   } /* end else */

   return i;
} /* end function "loadMaster" */


/* function "loadPuzz" loads the puzzle saved by the user */
int loadPuzz ( CrossWord *p1, const CrossWord *p0, int size ) {
   int i, j, k;
   FILE *fPtr;

   /* open the file; exit if file could not be opened */
   if ( ( fPtr = fopen( "SavedGame.dat", "rb" ) ) == NULL ) {
      printf( "File to load saved game could not be opened.\n" );
      return EOF;
   }
   else {

      for ( i = 0; i < size; i++ ) {
         fread( &p1[i].word, LENGTH * sizeof(char), 1, fPtr );

         j = strlen( p1[i].word );
         k = strlen( p0[i].word );

         /* pad "p1" with spaces to make the length of string equal to that of "p0" */
         for ( ; j < k; j++ ) {
            p1[i].word[j] = ' ';
         }

         p1[i].word[j] = '\0';

         /* set the intersection in "p1" */
         p1[i].intersect = p0[i].intersect;
      } /* end for */

      fclose( fPtr );
   } /* end else */

   return i;
} /* end function "loadPuzz" */
