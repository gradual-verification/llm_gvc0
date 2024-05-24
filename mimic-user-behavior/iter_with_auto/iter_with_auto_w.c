#include "stdlib.h"

struct node {
  struct node *next;
  int value;
};

struct llist {
  struct node *first;
  struct node *last;
};

/*@
predicate node(struct node *node; struct node *next, int value) =
  node->next |-> next &*& node->value |-> value &*& malloc_block_node(node);
@*/

/*@
predicate lseg(struct node *n1, struct node *n2; list<int> v) =
  n1 == n2 ? emp &*& v == nil : node(n1, ?_n, ?h) &*& lseg(_n, n2, ?t) &*& v == cons(h, t);

predicate llist(struct llist *list; list<int> v) =
  list->first |-> ?_f &*& list->last |-> ?_l &*& lseg(_f, _l, v) &*& node(_l, _, _) &*& malloc_block_llist(list);
@*/
/**
 * Description:
 * The `create_llist` function dynamically allocates memory for a linked list structure
 * and initializes it with an empty node.
 *
 * @return Pointer to the newly created linked list structure.
 */
struct llist *create_llist()
//requires true;
//ensures l!=NULL;
{
  struct llist *l = malloc(sizeof(struct llist));
  if (l == 0) abort();
  struct node *n = calloc(1, sizeof(struct node));
  if (n == 0) abort();
  l->first = n;
  l->last = n;
  return l;
}

/*@
lemma void distinct_nodes(struct node *n1, struct node *n2)
  requires node(n1, ?n1n, ?n1v) &*& node(n2, ?n2n, ?n2v);
  ensures node(n1, n1n, n1v) &*& node(n2, n2n, n2v) &*& n1 != n2;
{
  open node(n1, _, _);
  open node(n2, _, _);
  close node(n1, _, _);
  close node(n2, _, _);
}

lemma_auto void lseg_add(struct node *n2)
  requires lseg(?n1, n2, ?_v) &*& node(n2, ?n3, ?_x) &*& node(n3, ?n3next, ?n3value);
  ensures lseg(n1, n3, append(_v, cons(_x, nil))) &*& node(n3, n3next, n3value);
{
  distinct_nodes(n2, n3);
  open lseg(n1, n2, _v);
  if (n1 != n2) {
    distinct_nodes(n1, n3);
    lseg_add(n2);
  }
}
@*/

/**
 * Description:
 * The `llist_add` function adds a new node with the given value to the end of the linked list.
 *
 * @param list Pointer to the linked list structure.
 * @param x Value to be added to the linked list.
 */

void llist_add(struct llist *list, int x)
//requires list!=NULL&*& x!=NULL
//ensures list!=NULL 
{
  struct node *l = 0;
  struct node *n = calloc(1, sizeof(struct node));
  if (n == 0) {
    abort();
  }
  l = list->last;
  l->next = n;
  l->value = x;
  list->last = n;
  //@ lseg_add(l);
}

/*@
lemma_auto void lseg_append(struct node *n1, struct node *n2, struct node *n3)
  requires lseg(n1, n2, ?_v1) &*& lseg(n2, n3, ?_v2) &*& node(n3, ?n3n, ?n3v);
  ensures lseg(n1, n3, append(_v1, _v2)) &*& node(n3, n3n, n3v);
{
  open lseg(n1, n2, _v1);
  switch (_v1) {
    case nil:
    case cons(x, v):
      distinct_nodes(n1, n3);
      lseg_append(n1->next, n2, n3);
  }
}
@*/
/**
 * Description:
 * The `llist_append` function appends the second linked list to the end of the first linked list.
 *
 * @param list1 Pointer to the first linked list structure.
 * @param list2 Pointer to the second linked list structure.
 */
void llist_append(struct llist *list1, struct llist *list2)
//requires list1!=NULL&*& list2!=NULL
//requires list1!=NULL&*& list2!=NULL
{
  struct node *l1 = list1->last;
  struct node *f2 = list2->first;
  struct node *l2 = list2->last;
  //@ open lseg(f2, l2, _v2);  // Causes case split.
  if (f2 == l2) {
    //@ if (f2 != l2) pointer_fractions_same_address(&f2->next, &l2->next);
    free(l2);
    free(list2);
  } else {
    //@ distinct_nodes(l1, l2);
    l1->next = f2->next;
    l1->value = f2->value;
    list1->last = l2;
    //@ lseg_append(list1->first, l1, l2);
    free(f2);
    free(list2);
  }
}
/**
 * Description:
 * The `llist_dispose` function frees the memory occupied by all nodes in the linked list and the linked list itself.
 *
 * @param list Pointer to the linked list structure.
 */
void llist_dispose(struct llist *list)
//requires list!=NULL
//ensures list==NULL&*&l==NULL
{
  struct node *n = list->first;
  struct node *l = list->last;
  while (n != l)
    //@ invariant lseg(n, l, ?vs);
    //@ decreases length(vs);
  {
    struct node *next = n->next;
    free(n);
    n = next;
  }
  //@ if (n != l) pointer_fractions_same_address(&n->next, &l->next);
  free(l);
  free(list);
}

/*@

predicate lseg2(struct node *first, struct node *last, struct node *final, list<int> v;) =
  switch (v) {
    case nil: return first == last;
    case cons(head, tail):
      return first != final &*& node(first, ?next, head) &*& lseg2(next, last, final, tail);
  };

lemma_auto void lseg2_add(struct node *first)
  requires [?f]lseg2(first, ?last, ?final, ?v) &*& [f]node(last, ?next, ?value) &*& last != final;
  ensures [f]lseg2(first, next, final, append(v, cons(value, nil)));
{
  open lseg2(first, last, final, v);
  switch (v) {
    case nil:
      close [f]lseg2(next, next, final, nil);
    case cons(head, tail):
      open node(first, ?firstNext, head); // To produce witness field.
      lseg2_add(firstNext);
      close [f]node(first, firstNext, head);
  }
  close [f]lseg2(first, next, final, append(v, cons(value, nil)));
}

lemma_auto void lseg2_to_lseg(struct node *first)
  requires [?f]lseg2(first, ?last, ?final, ?v) &*& last == final;
  ensures [f]lseg(first, last, v);
{
  switch (v) {
    case nil:
      open lseg2(first, last, final, v);
    case cons(head, tail):
      open lseg2(first, last, final, v);
      open node(first, ?next, head);
      lseg2_to_lseg(next);
  }
}

@*/
/**
 * Description:
 * The `llist_length` function calculates the length of the linked list.
 *
 * @param list Pointer to the linked list structure.
 * @return The length of the linked list.
 */
int llist_length(struct llist *list)
//requires list!=NULL
//ensures c>=0;
{
  struct node *f = list->first;
  struct node *n = f;
  struct node *l = list->last;
  int c = 0;
  //@ close [frac]lseg2(f, f, l, nil);
  while (n != l)
    //@ invariant [frac]lseg2(f, n, l, ?_ls1) &*& [frac]lseg(n, l, ?_ls2) &*& _v == append(_ls1, _ls2) &*& c + length(_ls2) == length(_v);
    //@ decreases length(_ls2);
  {
    //@ open lseg(n, l, _ls2);
    //@ open node(n, _, _);
    struct node *next = n->next;
    //@ int value = n->value;
    //@ lseg2_add(f);
    n = next;
    if (c == INT_MAX) abort();
    c = c + 1;
    //@ assert [frac]lseg(next, l, ?ls3);
    //@ append_assoc(_ls1, cons(value, nil), ls3);
  }
  //@ if (n != l) pointer_fractions_same_address(&n->next, &l->next);
  //@ open lseg(n, l, _ls2);
  return c;
}
/**
 * Description:
 * The `llist_lookup` function looks up the value at the given index in the linked list.
 *
 * @param list Pointer to the linked list structure.
 * @param index The index of the value to be looked up.
 * @return The value at the given index in the linked list.
 */
int llist_lookup(struct llist *list, int index)
//requires list!=NULL &*& index!=NULL;
//ensures value!=NULL;
{
  struct node *f = list->first;
  struct node *l = list->last;
  struct node *n = f;
  int i = 0;
  while (i < index)
    //@ invariant 0 <= i &*& i <= index &*& lseg(f, n, ?_ls1) &*& lseg(n, l, ?_ls2) &*& _v == append(_ls1, _ls2) &*& _ls2 == drop(i, _v) &*& i + length(_ls2) == length(_v);
    //@ decreases index - i;
  {
    //@ open lseg(n, l, _);
    //@ int value = n->value;
    struct node *next = n->next;
    //@ open lseg(next, l, ?ls3); // To produce a witness node for next.
    //@ lseg_add(n);
    //@ drop_n_plus_one(i, _v);
    n = next;
    i = i + 1;
    //@ append_assoc(_ls1, cons(value, nil), ls3);
  }
  //@ open lseg(n, l, _);
  int value = n->value;
  //@ lseg_append(f, n, l);
  //@ drop_n_plus_one(index, _v);
  return value;
}
/**
 * Description:
 * The `llist_removeFirst` function removes the first node from the linked list and returns its value.
 *
 * @param l Pointer to the linked list structure.
 * @return The value of the first node that is removed from the linked list.
 */
int llist_removeFirst(struct llist *l)
//requires l!=NULL
//ensures nfv!=NULL
{
  struct node *nf = l->first;
  //@ open lseg(nf, ?nl, v);
  struct node *nfn = nf->next;
  int nfv = nf->value;
  free(nf);
  l->first = nfn;
  return nfv;
}
/**
 * Description:
 * The `main0` function tests the `llist_removeFirst` function by creating a linked list,
 * adding elements to it, removing the first two elements, and then disposing of the list.
 * It asserts that the removed elements have the correct values.
 */
void main0()
{
  struct llist *l = create_llist();
  llist_add(l, 10);
  llist_add(l, 20);
  llist_add(l, 30);
  llist_add(l, 40);
  int x1 = llist_removeFirst(l);
  assert(x1 == 10);
  int x2 = llist_removeFirst(l);
  assert(x2 == 20);
  llist_dispose(l);
}

int main() //@ : main
  //@ requires emp;
  //@ ensures emp;
{
  struct llist *l1 = create_llist();
  struct llist *l2 = create_llist();
  llist_add(l1, 10);
  llist_add(l1, 20);
  llist_add(l1, 30);
  llist_add(l2, 40);
  llist_add(l2, 50);
  llist_add(l2, 60);
  int x = llist_removeFirst(l2); assert(x == 40);
  llist_append(l1, l2);
  int n = llist_length(l1); assert(n == 5);
  int e0 = llist_lookup(l1, 0); assert(e0 == 10);
  int e1 = llist_lookup(l1, 1); assert(e1 == 20);
  int e2 = llist_lookup(l1, 2); assert(e2 == 30);
  int e3 = llist_lookup(l1, 3); assert(e3 == 50);
  int e4 = llist_lookup(l1, 4); assert(e4 == 60);
  llist_dispose(l1);
  return 0;
}

struct iter {
    struct node *current;
};

/*@

predicate llist_with_node(struct llist *list, list<int> v0, struct node *n, list<int> vn) =
  list->first |-> ?f &*& list->last |-> ?l &*& malloc_block_llist(list) &*& lseg2(f, n, l, ?v1) &*& lseg(n, l, vn) &*& node(l, _, _) &*& v0 == append(v1, vn);

predicate iter(struct iter *i, real frac, struct llist *l, list<int> v0, list<int> v) =
  i->current |-> ?n &*& [frac]llist_with_node(l, v0, n, v) &*& malloc_block_iter(i);

@*/
/**
 * Description:
 * The `llist_create_iter` function creates an iterator for a given linked list.
 */
struct iter *llist_create_iter(struct llist *l)
//requires l!=NULL;
//ensures l!=NULL;
{
    struct iter *i = 0;
    struct node *f = 0;
    i = malloc(sizeof(struct iter));
    if (i == 0) {
      abort();
    }
    //@ open [frac/2]llist(l, v);
    f = l->first;
    i->current = f;
    //@ struct node *last = l->last;
    //@ close [frac/2]lseg2(f, f, last, nil);
    //@ close [frac/2]llist_with_node(l, v, f, v);
    //@ close iter(i, frac/2, l, v, v);
    return i;
}
/**
 * Description:
 * The `iter_next` function returns the value of the current node of the iterator
 * and moves the iterator to the next node.
 */
int iter_next(struct iter *i)
//requires i!=NULL;
//ensures value!=NULL;
{
    //@ open iter(i, f, l, v0, v);
    struct node *c = i->current;
    //@ open llist_with_node(l, v0, c, v);
    //@ open lseg(c, ?last, v);
    //@ open node(c, _, _);
    int value = c->value;
    struct node *n = c->next;
    //@ close [f]node(c, n, value);
    //@ assert [f]lseg2(?first, _, _, ?vleft);
    //@ lseg2_add(first);
    i->current = n;
    //@ assert [f]lseg(n, last, ?tail);
    //@ append_assoc(vleft, cons(value, nil), tail);
    //@ close [f]llist_with_node(l, v0, n, tail);
    //@ close iter(i, f, l, v0, tail);
    return value;
}

/*@

lemma void lseg2_lseg_append(struct node *n)
  requires [?frac]lseg2(?f, n, ?l, ?vs1) &*& [frac]lseg(n, l, ?vs2);
  ensures [frac]lseg(f, l, append(vs1, vs2));
{
  open lseg2(f, n, l, vs1);
  switch (vs1) {
    case nil:
    case cons(h, t):
      open [frac]node(f, ?next, h);
      lseg2_lseg_append(n);
      close [frac]node(f, next, h);
      close [frac]lseg(f, l, append(vs1, vs2));
  }
}

@*/
/**
 * Description:
 * The `iter_dispose` function deallocates the memory associated with the iterator.
 */
void iter_dispose(struct iter *i)
//requires i!=NULL;
//ensures value!=NULL;
{
    //@ open iter(i, f1, l, v0, v);
    //@ open llist_with_node(l, v0, ?n, v);
    //@ lseg2_lseg_append(n);
    free(i);
}

int main2()
    //@ requires emp;
    //@ ensures emp;
{
    struct llist *l = create_llist();
    llist_add(l, 5);
    llist_add(l, 10);
    llist_add(l, 15);
    struct iter *i1 = llist_create_iter(l);
    struct iter *i2 = llist_create_iter(l);
    int i1e1 = iter_next(i1); assert(i1e1 == 5);
    int i2e1 = iter_next(i2); assert(i2e1 == 5);
    int i1e2 = iter_next(i1); assert(i1e2 == 10);
    int i2e2 = iter_next(i2); assert(i2e2 == 10);
    iter_dispose(i1);
    iter_dispose(i2);
    llist_dispose(l);
    return 0;
}
