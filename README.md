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

### If STROOP and WAFEL are both acronyms, what does PANCAKE stand for?
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
- Recommended: Ninja (`pip install ninja`) to improve build times

### Standalone
Same as any other ordinary CMake package. There are no options at the moment, because I haven't seen the need for it.

### As a Conan package
Run `conan export . pancake/0.2.0@local/beta` in the project folder. This exports Pancake as a Conan 
recipe that you can use in your projects.


## Legal issues, acknowledgements, etc.
I am currently not willing to license Pancake as open source, simply due to the non-licensing of the Super Mario 64 decompilation. If I can be 100% sure Nintendo isn't going to chase after me, I will officially open-source this.