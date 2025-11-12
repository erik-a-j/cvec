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
| [`alloc_fn_t`](https://github.com/erik-a-j/cvec/blob/e43c4c166e6eedaafc6ac0635bb19b61efbda6fb/src/cvec_types.h#L23) | malloc equivilant |
