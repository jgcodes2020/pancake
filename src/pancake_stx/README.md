# Pancake Standard Extensions

A few extra classes that attach new features to the 
C++ Standard Template Library.

Right now, this is:
- `stx::overload`: A class which inherits `operator()` from a set of functors.
- `std::hash<pair>`: A hash for std::pair, based on OpenJDK's algorithm for combining hashes.