# Pancake

<h3 style="color: red">TODO: add logo</h3>

Pancake is a library to interface with a closed-source modified version of the 
[SM64 decompilation](https://github.com/n64decomp/sm64) currently included in 
[Wafel](https://github.com/branpk/wafel).

## FAQ
I'm probably going to get these questions a lot, so here goes.
### Why "Pancake"?
There are two tools currently in use by the SM64 TASing community: 
[STROOP](https://github.com/SM64-TAS-ABC/STROOP) and 
[Wafel](https://github.com/branpk/wafel), which together comprise the name of a
very certain Dutch dessert: *stroopwafel*. Since this library complements both 
STROOP and Wafel, I decided to go with the name **Pancake**, since people eat 
pancakes with stroopwafels, and as a subtle nod to pannenkoek2012.
### If STROOP and WAFEL are both acronyms, what does PANCAKE stand for?
**P**ointer **AN**alyzer **C**ollecting **A**ccess to **K**nown **E**ngine.  
(shoutouts to Notchmath for coming up with like 75% of this)

## Building
I will be posting prebuilt versions when the library is feature-complete. If 
you want to build this for yourself or work on it, you'll need the following 
tools:

- Python 3.8
- MSYS2 with MinGW-w64 (see [this StackOverflow thing](https://stackoverflow.com/questions/30069830/how-to-install-mingw-w64-and-msys2))
- Meson (install using `pip install meson`)
- Ninja (install using `pip install ninja` or download from their [GitHub](https://github.com/ninja-build/ninja/releases/tag/v1.10.2))

Then clone this repository and run `setup.bat` or `"script\setup.py"`.
Dependencies are bundled in .zip archives and Meson will unpack these on setup.

To build, run `build.bat debug` or `build.bat release`.