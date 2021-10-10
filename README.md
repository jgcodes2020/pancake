# Pancake

<h3 style="color: red">TODO: add logo</h3>

Pancake is a library to interface with libsm64, a modified version of the [SM64 decompilation](https://github.com/n64decomp/sm64) currently included with [Wafel](https://github.com/branpk/wafel).

## FAQ
I'm probably going to get these questions a lot, so here goes.
### Why "Pancake"?
There are two tools currently in use by the SM64 TASing community: 
[STROOP](https://github.com/SM64-TAS-ABC/STROOP) and 
[Wafel](https://github.com/branpk/wafel), which together comprise the name of a
very certain Dutch dessert: *stroopwafel*. Since this library complements both STROOP and Wafel, I decided to go with the name **Pancake**, since people eat pancakes with stroopwafels, and as a subtle nod to pannenkoek2012.

### If STROOP and WAFEL are both backronyms, what does PANCAKE stand for?
**P**ointer **AN**alyzer **C**ollecting **A**ccess to **K**nown **E**ngine.  
(shoutouts to Notchmath for coming up with like 75% of this)

### Is it faster than Wafel API?
Pancake is likely slightly slower, but it is still blazing fast.  
Reasons include:
- libdwarf (the only DWARF parser I found for Windows) is slow
- Wafel caches the address of globals during data path compilation, Pancake doesn't (for safety reasons)

## Building instructions
You'll need:
- A relatively recent CMake version (>=3.15)
- The Conan package manager (`pip install conan`)
- On Windows, NASM. This is because the Microsoft assembler doesn't support the `incbin` directive.
- Recommended: Ninja (`pip install ninja`) to improve build times

### Standalone


### As a Conan package
Run `conan export . pancake/0.2.0@local/beta` in the project folder. This exports Pancake as a Conan 
recipe that you can use in your projects. You will need to build from source.


## Legal obligations
libdwarf is licensed under the LGPL. To comply, here are instructions on relinking:
1. Make a CMake imported target for your libdwarf. Here's a template.
```
add_library(libdwarf_custom STATIC IMPORTED)
set_target_properties(libdwarf_custom PROPERTIES
  IMPORTED_LOCATION ${LIBDWARF_LIBFILE}
  INTERFACE_INCLUDE_DIRECTORIES ${LIBDWARF_INCLUDE_DIRS}
)
```
  - Replace \${LIBDWARF_LIBFILE} with the path to your library.
  - Replace \${LIBDWARF_INCLUDE_DIRS} with the path to your include directory.
2. Replace `CONAN_PKG::libdwarf` with `libdwarf_custom` or whatever you named your cmake target.
3. You're done, rebuild again and test!

This library is licensed under the Mozilla Public License, version 2. See the license at LICENSE.md 
for full information.