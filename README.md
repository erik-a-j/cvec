# cvec — Dynamic Array Implementation in C #

`cvec` is a lightweight, dependency-free, generic dynamic array implementation for C99 and later.  
It provides a flexible and type-safe API that can be customized via user-defined hooks.

---

## Features ##

- Written in **pure C99**, no external dependencies
- Optional [hooks](#hooks) for all of `cvec`'s internal operations

## Hooks ##

| Typedef                          | Description       |
| -------------------------------- | ----------------- |
| [`alloc_fn_t`](src/cvec_types.h) | malloc equivilant |
