# Pancake

<h3 style="color: red">TODO: add logo</h3>

Pancake is a library to interface with a closed-source modified version of the [SM64 decompilation](https://github.com/n64decomp/sm64) currently included in [Wafel](https://github.com/branpk/wafel).

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

## Can I use it right now?
I will be posting prebuilt versions when the library is feature-complete. 

### Building it yourself (Windows)
If you want to build this for yourself or work on it, you'll need the following tools:

- Python 3.8
- Visual Studio 2019
- Meson (install using `pip install meson`)
- Ninja (install using `pip install ninja` or download from their [GitHub](https://github.com/ninja-build/ninja/releases/tag/v1.10.2))

After installing MSYS2 and MinGW-w64, add `${MSYS_HOME}\mingw64\bin` to your path. Then, open your favourite Windows command prompt, `cd` to wherever you cloned the repo and run the following to build the project:

**cmd.exe**
```bat
setup
build debug
```
**PowerShell**
```ps
.\setup
.\build debug
```
The dependencies will be downloaded or unzipped automatically by Meson, so you need not worry about that. Well, except for one thing. This library depends on Wafel's libsm64. This should be accessible on the release of 0.7.2, but for now, you'll need to build from source on the master branch, then unlock libsm64 using an SM64 ROM.