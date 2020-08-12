#include "entdump.h"

int read_bsp(FILE *in, dheader_t &header, std::vector<uint8_t> &buf, const char *filename) {
    int i;
    size_t len;

    fseek(in, 0, SEEK_END);
    len = ftell(in);
    fseek(in, 0, SEEK_SET);

    if (len < sizeof(dheader_t)) {
        return 1; // az: just in case
    }

    buf.resize(len);
    fread(&buf[0], len, 1, in);

    //map header structs onto the buffer
    header = *(dheader_t *) buf.data();
    for (i = 0; i < sizeof(dheader_t) / 4; i++)
        ((int *) &header)[i] = ((int *) &header)[i];

    //r1: check header pointers point within allocated data
    for (i = 0; i < HEADER_LUMPS; i++) {
        //for some reason there are unused lumps with invalid values
        if (i == LUMP_POP)
            continue;

        if (header.lumps[i].fileofs < 0 || header.lumps[i].length < 0 ||
            header.lumps[i].fileofs + header.lumps[i].length > len) {
            Com_Error(ERR_DROP,
                      "%s: lump %d offset %d of size %d is out of bounds\n"
                      "%s is probably truncated or otherwise corrupted",
                      __func__, i, header.lumps[i].fileofs,
                      header.lumps[i].length, filename);
        }
    }

    if (header.version != BSPVERSION) {
        Com_Error(ERR_CONTINUE, "%s is not a valid BSP file.", filename);
        return 1;
    }

    return 0;
}
