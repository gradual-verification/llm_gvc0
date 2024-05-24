#include "threading.h"

struct cell {
  int x;
  struct mutex* mutex;
  //@ box id;
};

/*@

fixpoint bool is_prefix(trace pref, trace trace) {
  switch(trace) {
    case zero: return pref == zero;
    case inc(trace0): return pref == trace || is_prefix(pref, trace0);
    case dec(trace0): return pref == trace || is_prefix(pref, trace0);
    case cas_(old, new, trace0): return pref == trace || is_prefix(pref, trace0);
  }
}

lemma void is_prefix_refl(trace t)
  requires true;
  ensures is_prefix(t, t) == true;
{
  switch(t) {
    case zero:
    case inc(trace0): 
    case dec(trace0): 
    case cas_(old, new, trace0): 
  }
}

box_class trace_extension(trace trace) {
  invariant true;
  
  action inc();
    requires true;
    ensures trace == inc(old_trace);
        
  action dec();
    requires true;
    ensures trace == dec(old_trace);
  
  action cas(int old, int new);
    requires true;
    ensures trace == cas_(old, new, old_trace);
  
  action read();
    requires true;
    ensures trace == old_trace;
    
  handle_predicate is_prefix_handle(trace prefix) {
        invariant is_prefix(prefix, trace) == true;
        
        preserved_by inc() {
        }
        preserved_by dec() {
        }
        preserved_by cas(old, new) {
        }
        preserved_by read() {
        }
  }
}

inductive trace = zero | inc(trace) | dec(trace) | cas_(int, int, trace);

typedef lemma void increment_only(struct cell* c, int v)();
  requires c->x |-> ?x;
  ensures c->x |-> x &*& v <= x;
@*/

/*@
predicate exists<t>(t x) = true;

predicate_ctor lock_invariant(struct cell* c, fixpoint(trace, bool) allowed)() =
  c->x |-> ?v &*& [_]c->id |-> ?id &*& malloc_block_cell(c) &*& exists(?trace) &*& trace_extension(id, trace) &*& execute_trace(trace) == v &*& allowed(trace) == true;
  
fixpoint int execute_trace(trace trace) {
  switch(trace) {
    case zero: return 0;
    case inc(trace0): return execute_trace(trace0) + 1;
    case dec(trace0): return execute_trace(trace0) - 1;
    case cas_(old, new, trace0): return execute_trace(trace0) == old ? new : execute_trace(trace0);
  }
}
  
predicate cell(struct cell* c, fixpoint(trace, bool) allowed;) =
  c->mutex |-> ?mutex &*& mutex(mutex, lock_invariant(c, allowed));
  
predicate observed(struct cell* c, trace trace) =
  [_]c->id |-> ?id &*& is_prefix_handle(?h, id, trace);
@*/
/***
 * Description:
This function creates a new cell object and initializes its fields. It also associates a mutex with the cell to enable thread-safe access.

Specification:

Precondition:
There exists a fixed-point function  that determines whether certain operations are permitted. 
The function allowed must evaluate to true when applied to the value zero.
Postcondition:
If the function returns NULL, it indicates that memory allocation failed. Otherwise, 
the function ensures that the returned cell object satisfies the cell predicate and has been observed with the value zero.
**/
struct cell* cell_create()
  //@ requires exists<fixpoint(trace, bool)>(?allowed) &*& allowed(zero) == true;
  //@ ensures result == 0 ? true : cell(result, allowed) &*& observed(result, zero);
{
  //@ open exists(_);
  struct cell* c = malloc(sizeof(struct cell));
  if(c == 0) return 0;
  c->x = 0;
  //@ close exists(zero);
  //@ create_box boxId = trace_extension(zero) and_handle h = is_prefix_handle(zero); 
  //@ c->id = boxId;
  //@ leak c->id |-> _;
  //@ close observed(c, zero);
  //@ close lock_invariant(c, allowed)();
  //@ close create_mutex_ghost_arg(lock_invariant(c, allowed));
  struct mutex* m = create_mutex();
  c->mutex = m;
  return c;
}

/*@
typedef lemma void inc_allowed(fixpoint(trace, bool) allowed)(trace t);
  requires allowed(t) == true;
  ensures allowed(inc(t)) == true;
@*/
/**
 * Specification:

Precondition:
The function is invoked with a pointer c to a cell object.
There exists a fractional permission f representing ownership of the cell object.
The cell object satisfies the cell predicate with a permission function allowed.
The increment operation is allowed according to the function is_inc_allowed, which is consistent with the permission function allowed.
The cell object has been observed with the trace trace0.
Postcondition:
Upon completion, the function ensures that the cell object remains unchanged except for the incremented value.
The permission function allowed remains consistent with the cell object's state.
The trace of observed changes to the cell object is updated to reflect the increment operation.
The trace of observed changes is a prefix of the previous trace.
*/
void increment(struct cell* c)
  //@ requires [?f]cell(c, ?allowed) &*& is_inc_allowed(?lem, allowed) &*& observed(c, ?trace0);
  //@ ensures [f]cell(c, allowed) &*& is_inc_allowed(lem, allowed) &*& observed(c, ?trace) &*& is_prefix(trace0, trace) == true;
{
  //@ open observed(c, trace0);
  //@ assert is_prefix_handle(?h, _, trace0);
  mutex_acquire(c->mutex);
  //@ open lock_invariant(c, allowed)();
  //@ open exists(?trace);
  c->x++;
  //@ assert trace_extension(?id, trace);
  /*@
    consuming_box_predicate trace_extension(id, trace)
    consuming_handle_predicate is_prefix_handle(h, trace0)
    perform_action inc() {
  @*/
  {
  
  }
  /*@
    }
    producing_box_predicate trace_extension(inc(trace))
    producing_handle_predicate is_prefix_handle(inc(trace));
  @*/
  //@ close observed(c, inc(trace));
  //@ lem(trace);
  //@ close exists(inc(trace));
  //@ close lock_invariant(c, allowed)();
  mutex_release(c->mutex);
}

/*@
typedef lemma void dec_allowed(fixpoint(trace, bool) allowed)(trace t);
  requires allowed(t) == true;
  ensures allowed(dec(t)) == true;
@*/

/**
 * Description:
This function decrements the value stored in a cell object while ensuring thread safety through mutex locking. It also observes changes to the cell's state and ensures that the decrement operation is allowed according to a specified condition.

Specification:

Precondition:
The function is invoked with a pointer c to a cell object.
There exists a fractional permission f representing ownership of the cell object.
The cell object satisfies the cell predicate with a permission function allowed.
The decrement operation is allowed according to the function is_dec_allowed, which is consistent with the permission function allowed.
The cell object has been observed with the trace trace0.
Postcondition:
Upon completion, the function ensures that the cell object remains unchanged except for the decremented value.
The permission function allowed remains consistent with the cell object's state.
The trace of observed changes to the cell object is updated to reflect the decrement operation.
The trace of observed changes is a prefix of the previous trace.
*/

void decrement(struct cell* c)
  //@ requires [?f]cell(c, ?allowed) &*& is_dec_allowed(?lem, allowed) &*& observed(c, ?trace0);
  //@ ensures [f]cell(c, allowed) &*& is_dec_allowed(lem, allowed) &*& observed(c, ?trace) &*& is_prefix(trace0, trace) == true;
{
  //@ open observed(c, trace0);
  //@ assert is_prefix_handle(?h, _, trace0);
  mutex_acquire(c->mutex);
  //@ open lock_invariant(c, allowed)();
  //@ open exists(?trace);
  c->x--;
  //@ assert trace_extension(?id, trace);
  /*@
    consuming_box_predicate trace_extension(id, trace)
    consuming_handle_predicate is_prefix_handle(h, trace0)
    perform_action dec() {
  @*/
  {
    
  }
  /*@
    }
    producing_box_predicate trace_extension(dec(trace))
    producing_handle_predicate is_prefix_handle(dec(trace));
  @*/
  //@ close observed(c, dec(trace));
  //@ lem(trace);
  //@ close exists(dec(trace));
  //@ close lock_invariant(c, allowed)();
  mutex_release(c->mutex);
}
/**The function is invoked with a pointer c to a cell object.
The cell object satisfies the cell predicate with a permission function allowed.
The CAS operation is allowed according to the function is_cas_allowed, which is consistent with the permission function allowed.
The cell object has been observed with the trace trace0.
Postcondition:
Upon completion, the function ensures that the cell object remains unchanged if the CAS operation fails.
If the CAS operation succeeds, the cell object's value is updated to the new value.
The permission function allowed remains consistent with the cell object's state.
The trace of observed changes to the cell object is updated to reflect the CAS operation.
The trace of observed changes is a prefix of the previous trace.
The function returns the original value stored in the cell object before the CAS operation.*/
/*@
typedef lemma void cas_allowed(fixpoint(trace, bool) allowed, int old, int new)(trace t);
  requires allowed(t) == true;
  ensures allowed(cas_(old, new, t)) == true;
@*/

int cas(struct cell* c, int old, int new)
  //@ requires [?f]cell(c, ?allowed) &*& is_cas_allowed(?lem, allowed, old, new) &*& observed(c, ?trace0);
  //@ ensures [f]cell(c, allowed) &*& is_cas_allowed(lem, allowed, old, new) &*& observed(c, ?trace) &*& allowed(trace) == true &*& is_prefix(trace0, trace) == true;
{
  //@ open observed(c, trace0);
  //@ assert is_prefix_handle(?h, _, trace0);
  int res;
  mutex_acquire(c->mutex);
  //@ open lock_invariant(c, allowed)();
  //@ open exists(?trace);
  res = c->x;
  if(c->x == old) {
    c->x = new;
  }
  //@ assert trace_extension(?id, trace);
  /*@
    consuming_box_predicate trace_extension(id, trace)
    consuming_handle_predicate is_prefix_handle(h, trace0)
    perform_action cas(old, new) {
  @*/
  {
    
  }
  /*@
    }
    producing_box_predicate trace_extension(cas_(old, new, trace))
    producing_handle_predicate is_prefix_handle(cas_(old, new, trace));
  @*/
  //@ close observed(c, cas_(old, new, trace));
  //@ lem(trace);
  //@ trace cs = cas_(old, new , trace);
  //@ close exists(cas_(old, new, trace));
  //@ close lock_invariant(c, allowed)();
  mutex_release(c->mutex);
  return res;
}
/**
 * Specification:

Precondition:
The function is invoked with a pointer c to a cell object.
The cell object satisfies the cell predicate with a permission function allowed.
The cell object has been observed with the trace trace0.
Postcondition:
Upon completion, the function ensures that the cell object remains unchanged.
The permission function allowed remains consistent with the cell object's state.
The observed changes to the cell object are updated to reflect the read operation.
The observed changes to the cell object form a prefix of the previous trace.
The value returned by the function corresponds to the value stored in the cell object at the time of observation.
*/
int get(struct cell* c)
  //@ requires [?f]cell(c, ?allowed) &*& observed(c, ?trace0);
  //@ ensures [f]cell(c, allowed) &*& observed(c, ?trace) &*& allowed(trace) == true &*& execute_trace(trace) == result &*& is_prefix(trace0, trace) == true;
{
  //@ open observed(c, trace0);
  //@ assert is_prefix_handle(?h, _, trace0);
  int res;
  mutex_acquire(c->mutex);
  //@ open lock_invariant(c, allowed)();
  //@ assert exists(?trace);
  res = c->x;
    //@ assert trace_extension(?id, trace);
  /*@
    consuming_box_predicate trace_extension(id, trace)
    consuming_handle_predicate is_prefix_handle(h, trace0)
    perform_action read() {
  @*/
  {
    //@ is_prefix_refl(trace);
  }
  /*@
    }
    producing_box_predicate trace_extension(trace)
    producing_handle_predicate is_prefix_handle(trace);
  @*/
  //@ close lock_invariant(c, allowed)();
  //@ close observed(c, trace);
  mutex_release(c->mutex);
  return res;
}

/*@
fixpoint bool incr_only(trace trace) {
  switch(trace) {
    case zero: return true;
    case inc(trace0): return incr_only(trace0);
    case dec(trace0): return false;
    case cas_(old, new, trace0): return old <= new && incr_only(trace0);
  }
}
@*/

/*@
lemma void prefix_smaller(trace t1, trace t2)
  requires incr_only(t1) == true &*& incr_only(t2) == true &*& is_prefix(t1, t2) == true;
  ensures execute_trace(t1) <= execute_trace(t2);
{
  switch(t2) {
    case zero:
    case inc(t0): if(t1 != t2) prefix_smaller(t1, t0);
    case dec(t0): 
    case cas_(old, new, t0): if(t1 != t2) prefix_smaller(t1, t0);
  }
}
@*/
/**
 * Description:
This function enforces a constraint where the value stored in a cell object can only be incremented.

Specification:

Precondition:
The function is invoked with a pointer c to a cell object.
The cell object satisfies the cell predicate with a permission function incr_only.
The cell object has been observed with the trace trace0.
Postcondition:
Upon completion, the function ensures that the permission function incr_only remains consistent with the cell object's state.
No observable changes are made to the cell object's state.
*/
void only_allow_incrementing(struct cell* c)
  //@ requires [?f]cell(c, incr_only) &*& observed(c, ?trace0);
  //@ ensures [f]cell(c, incr_only);
{
  int x1 = get(c);
  //@ assert observed(c, ?trace1);
  int x2 = get(c);
  //@ assert observed(c, ?trace2);
  //@ prefix_smaller(trace1, trace2);
  assert x1 <= x2;
  //@ leak observed(c, _);
}
