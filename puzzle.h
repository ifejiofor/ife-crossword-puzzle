#define WIDTH 50
#define HEIGHT 30
#define WORDS 5
#define NO_INTERSECT -1
#define LENGTH 20

/* structure definition */
typedef struct {
   char word[LENGTH];  /* array to hold the letters of the word */
   int intersect;   /* holds an integer that represent the intersection of words in the puzzle */
} CrossWord;