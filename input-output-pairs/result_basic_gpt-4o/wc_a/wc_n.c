#include "stdlib.h"
#include "stdio.h"
#include "malloc.h"
#include <stdbool.h>
#include "assert.h"

/*@
predicate chars(char *arr, list<char> cs) =
  arr == 0 ?
    cs == nil
  :
    arr |-> ?c &*&
    malloc_block_chars(arr, _) &*&
    chars(arr + 1, ?cs1) &*&
    cs == cons(c, cs1);
@*/

/*@
requires chars(string, ?cs);
ensures chars(string, cs) &*& result == count_words(cs, inword) &*& (result >= 0);
predicate_family count_words(list<char> cs, bool inword) = 
  switch(cs) {
    case nil: return inword ? 1 : 0;
    case cons(h, t):
      return (h == ' ' ? 
              (inword ? 1 + count_words(t, false) : count_words(t, false))
              : count_words(t, true));
  };
@*/
int wc(char* string, bool inword) 
{
  //@ open chars(string, _);
  char head = *string;
  if(head == 0) {
    //@ close chars(string, _);
    return inword ? 1 : 0;
  } else {
    if(head == ' ') {
      int result = wc(string + 1, false);
      //@ close chars(string, _);
      return inword ? 1 + result : result;
    } else {
      int result = wc(string + 1, true);
      //@ close chars(string, _);
      return result;
    }
  }
}

// Verification of this particular test is dependent on actual behavior.
/*** 
 * Description:
The `test` function is a test function to validate the `wc` function.
*/
void test()
{
  int nb = wc("This line of text contains 8 words.", false);
  assert(nb == 7);
}

/*** 
 * Description:
The `main` function main, a simplified illustrative example.
*/
int main(int argc, char** argv)
{
  if(argc < 2) { puts("No input file specified."); return -1; }
  //@ open chars(argv[1], ?cs);
  //@ close chars(argv[1], cs);
  return 0;
}
