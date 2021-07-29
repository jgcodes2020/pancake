# libsm64 Linux builds

__NOTE: if you're looking for a Windows build, get it from Wafel. I'm providing these here because they aren't in Wafel.__

These are builds of libsm64, for Ubuntu 21.04. I cannot provide them verbatim, because of Nintendo. However, I can provide them encrypted using the original ROM as a key. 

## Dependencies
You'll need `cryptography` from `pip`.

To unlock, run:
```bash
python3 fernet-lock.py unlock sm64_<version>.so.locked -k <your sm64 rom> -o sm64_<version>.so
```

If you want to lock them again (not that you would necessarily need to), it's the same:
```bash
python3 fernet-lock.py lock sm64_<version>.so -k <your sm64 rom> -o sm64_<version>.so.locked 
```
