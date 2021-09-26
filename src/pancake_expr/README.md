# Pancake Expressions

A parser and state-compiler for *expressions*.

*Expressions* are Pancake's way of deciding what data to retrieve,
and use a limited subset of C syntax.

Operator | Usage                                         |
:-------:|:---------------------------------------------:|
`.`      | Access a member                               |
`->`     | Access a member through a pointer             |
`[x]`    | Access an array index from a pointer or array |

For example, to access physical Mario's<sup><a href="#note-1">1</a></sup> x-coordinate:
```c++
void test() {
  sm64 instance("path/to/libsm64");
  float& pos = instance.get<float>("gMarioStates[0].pos[0]");
}
```
---
<span id="note-1">1</span>: I use *physical* and *visual* Mario to better distinguish what pannenkoek referred to in his videos as the Mario *struct* and the Mario *object*. 