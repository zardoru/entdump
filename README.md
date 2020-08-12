entdump (wyrmin.xyz edition)
=====
This repository contains a rewrite of the entdump utility, made to dump entities and texture lists from quake 2 bsp files.

To limit the scope of the tool, the listing of textures no longer checks for existence of textures. It only lists them.

Usage examples:

```entdump a/directory```

Prints textures of the bsp files in the directory you point it to. It does not check it recursively.

```entdump map.bsp```

Prints entities of the bsp file.

```entdump -t map.bsp```

Prints textures of the bsp file.

```entdump -t -e map.bsp```

Prints textures and entities of the bsp file.