#include "stdlib.h"

struct Counter {
  int value;
};

/*@
predicate Counter(struct Counter* c, int v) =
  c->value |-> v;
@*/
/*@
predicate Counter_functional_behavior(struct Counter* c, int v) =
  c->value |-> v;
@*/
struct Counter* init(int v)
//@ requires true;
  //@ ensures Counter(result, v);

{
  struct Counter* c = malloc(sizeof(struct Counter));
  if (c == 0) {
    abort();
  }
  c->value = v;

  //@ close Counter(c, v);
  return c;
}


void increment(struct Counter* c)
  //@ requires Counter(c, ?v) &*& v < INT_MAX;
  //@ ensures Counter(c, v+1);

{
  
  int tmp = c->value;
  c->value = tmp + 1;
  
}


void dispose(struct Counter* c)
  //@ requires Counter(c, _);
  //@ ensures true;
{
  
  free(c);
}



void swap(struct Counter* c1, struct Counter* c2)
  //@ requires Counter(c1, ?v1) &*& Counter(c2, ?v2);
  //@ ensures Counter(c1, v2) &*& Counter(c2, v1); 
{
  
  int tmp1 = c1->value;
  int tmp2 = c2->value;
  c2->value = tmp1;
  c1->value = tmp2;
  
}

int get(struct Counter* c)
  //@ requires Counter(c, ?v);
  //@ ensures Counter(c, v) &*& result==v; 
{

  int tmp = c->value;

  return tmp;
}

int main() //@ : main
  //@ requires emp;
  //@ ensures emp;
{
  struct Counter* c1 = init(0); struct Counter* c2 = init(5);

  increment(c1); swap(c1, c2); int tmp = get(c2);
 
  
  dispose(c1); dispose(c2);
  return 0;
}
