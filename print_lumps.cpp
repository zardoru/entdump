#include "entdump.h"

void print_bsp_entities(lump_t lump, std::vector<uint8_t> &buf, const char *filename) {
    std::vector<char> map_entitystring(MAX_MAP_ENTSTRING);

    if (lump.length > MAX_MAP_ENTSTRING) {
        Com_Error(ERR_CONTINUE, "Map has too large entity lump (%d > %d)",
                  lump.length, MAX_MAP_ENTSTRING);
        return;
    }
    if (lump.fileofs + lump.length > buf.size()) {
        Com_Error(ERR_CONTINUE, "Entity lump parameter error in file %s\n"
                                "lump offset %d + length %d exceeds filesize %d\n"
                                "the file is truncated or otherwise corrupted.\n",
                  filename, lump.fileofs, lump.length, buf.size());
        return;
    }

    memcpy(&map_entitystring[0], buf.data() + lump.fileofs, lump.length);

    // remove newline at end of lump string if present.
    if (!strcmp(&map_entitystring[strlen(map_entitystring.data()) - 1], "\n"))
        map_entitystring[strlen(map_entitystring.data()) - 1] = '\0';

    printf("%s\n", map_entitystring.data());
}

/*
=================
print_bsp_textures
=================
//QW// pulled this from quake2 engine source and modified it
to list textures used and to flag the missing ones.
*/
void print_bsp_textures(lump_t lump, std::vector<uint8_t> &buf, const char *filename) {
    std::vector<mapsurface_t> map_surfaces(MAX_MAP_TEXINFO);

    texinfo_t *in;
    mapsurface_t *out;
    mapsurface_t *list;
    int i;
    int j;
    int count;
    int uniques;
    size_t count_map_missing;

    count_map_missing = 0;
    in = (texinfo_t *) (buf.data() + lump.fileofs);
    if (lump.length % sizeof(*in)) {
        Com_Error(ERR_CONTINUE, "%s: funny lump size in %s",
                  __func__, filename);
        return;
    }
    count = lump.length / sizeof(*in);
    if (count < 1) {
        Com_Error(ERR_CONTINUE, "%s: Map with no surfaces: %s",
                  __func__, filename);
        return;
    }
    if (count > MAX_MAP_TEXINFO) {
        Com_Error(ERR_CONTINUE, "%s: Map has too many surfaces: %s",
                  __func__, filename);
        return;
    }

    out = &map_surfaces[0];
    uniques = 0;

    for (i = 0; i < count; i++, in++, out++) {
        strncpy(out->c.name, in->texture, sizeof(out->c.name) - 1);
        strncpy(out->rname, in->texture, sizeof(out->rname) - 1);
        out->c.flags = in->flags;
        out->c.value = in->value;
        out->dupe = 0;

        list = &map_surfaces[0];
        for (j = 0; j < count; j++, list++)    // identify each unique texture name
        {
            if (strcmp(list->rname, "") != 0
                && list != out
                && strcmp(list->rname, out->rname) == 0)
                out->dupe = 1;    //flag the duplicate
        }
        if (!out->dupe) {
            uniques++;
            printf("textures/%s.wal\n", out->rname);
        }
    }
}

