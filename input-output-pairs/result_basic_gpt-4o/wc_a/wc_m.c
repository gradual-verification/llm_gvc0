#include "stdlib.h"
#include "stdio.h"
#include "malloc.h"
#include <stdbool.h>

//@ #include "listex.gh"

/*@
fixpoint int wcount(list<char> cs, bool inword) {
  switch(cs) {
    case nil: return inword ? 1 : 0;
    case cons(h, t): return 0 == h ? (inword ? 1 : 0) : (' ' == h ? ((inword ? 1 : 0) + wcount(t, false)) : wcount(t, true));
  }
}

predicate is_word(char c) = c != 0 && c != ' ';

lemma void wcount_non_negative(list<char> cs, bool inword)
  requires true;
  ensures 0 <= wcount(cs, inword);
{
  switch(cs) {
    case nil: break;
    case cons(h, t):
      if (h == 0) {
        // just evaluate the expression
      } else if (h == ' ') {
        wcount_non_negative(t, false);
      } else {
        wcount_non_negative(t, true);
      }
  }
}

@*/

int wc(char* string, bool inword)
//@ requires [?f]string(string, ?cs);
//@ ensures [f]string(string, cs) &*& result == wcount(cs, inword);
{
  //@ open string(string, cs);
  char head = *string;
  if(head == 0) {
    //@ close string(string, cs);
    return inword ? 1 : 0;
  } else {
    if(head == ' ') {
      int result = wc(string + 1, false);
      //@ close string(string, cons(head, tail(string, cs)));
      return inword ? 1 + result : result;
    } else {
      int result = wc(string + 1, true);
      //@ close string(string, cons(head, tail(string, cs)));
      return result;
    }
  }
}

void test() 
//@ requires true;
//@ ensures true;
{
  int nb = wc("This line of text contains 8 words.", false);
  assert(nb == 7);  // the assertion should be corrected based on actual word count to match the code
}

int main(int argc, char** argv) //@ : main
//@ requires 0 <= argc &*& [_]argv(argv, argc, _);
//@ ensures true;
{
  bool inword = false; struct file* fp = 0; char* buff = 0; int total = 0; char* res = 0;
  if(argc < 2) { puts("No input file specified."); return -1; }
  fp = fopen(* (argv + 1), "r");
  buff = malloc(100);
  if(buff == 0 || fp == 0) { abort(); }
  res = fgets(buff, 100, fp);
  while(res != 0)
  {
    int tmp = wc(buff, inword);
    if (total > INT_MAX - tmp) {
      break;
    }
    total = total + tmp;
    res = fgets(buff, 100, fp);
  }
  printf("%i", total);
  free(buff);
  fclose(fp);
  return 0;
}
