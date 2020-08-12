// From http://old.r1ch.net/stuff/r1q2/ - thanks R1ch.
//GPL etc.
//
// entdump is used for extracting entities from quake2 bsp files in text
// format for usage with the added ent file support in Xatrix+
//
// Build like: gcc -o entdump entdump.c
// Use like:  # ./entdump map.bsp > map.ent
// Nick I fixed so that the ent.file didn't have a double newline at the end.

// January 11, 2010, QwazyWabbit added texture file name output, usage info
// July 18, 2019, QwazyWabbit add missing texture flagging.
// Process wild-card filenames.
//

// August 12, 2020, zardoru rewrote this tool with C++.
/* zardoru's note: yeah this isn't 'idiomatic' c++17. whatever. */

#include "entdump.h"

// arg state
bool print_textures = false;
bool print_entities = true;
bool print_headers = false;

void parse_args(char **argv, int argc);

int main(int argc, char *argv[]) {
    FILE *in;

    if (argc > 1) {
        if (std::filesystem::is_directory(argv[1])) {
            print_textures_of_maps_in_directory(argv[1]);
            return EXIT_SUCCESS;
        } else {
            in = fopen(argv[argc - 1], "rb");
            if (!in) {
                fprintf(stderr, "FATAL ERROR: fopen() on %s failed.\n", argv[1]);
                return EXIT_FAILURE;
            }
        }
	}
	else {
        //print usage info
        printf("entdump (wyrmin.xyz edition) is used for extracting entities and textures from quake2 bsp files in text\n");
        printf("format for usage with ent and override file support.\n");
        printf("Usage: entdump mapname.bsp \n");
        printf("   or: entdump mapname.bsp > mapname.txt \n");
        printf("   or: entdump mapname.bsp | more \n");
        printf("   or: entdump -t mapname.bsp | more \n");
        printf("   or: entdump -t -c mapname.bsp | more \n");
        return EXIT_FAILURE;
    }

    const char *filename = argv[argc - 1];
    dheader_t header;
    std::vector<uint8_t> buf;

    parse_args(argv, argc);

    if (read_bsp(in, header, buf, filename))
        return EXIT_FAILURE;

    if (print_textures) {
        if (print_headers) {
            printf("Map textures:\n");
            printf("======================:\n");
        }
        print_bsp_textures(header.lumps[LUMP_TEXINFO], buf, filename);
    }

    if (print_entities) {
        if (print_headers) {
            printf("Map entities:\n");
            printf("======================:\n");
        }
        print_bsp_entities(header.lumps[LUMP_ENTITIES], buf, filename);
    }

    fclose(in);
    return EXIT_SUCCESS;
}

void parse_args(char **argv, int argc) {
    bool print_textures_seen = false;

    for (int i = 1; i < argc - 1; i++) {
        if (argv[i][0] == '-') {
            size_t arglen = strlen(argv[i]);

            // parse t, e, whatever
            for (size_t j = 1; j < arglen; j++) {
                char opt = argv[i][j];
                switch (opt) {
                    case 't':
                        print_textures = true;
                        if (!print_textures_seen) {
                            print_entities = false;
                            print_textures_seen = true;
                        }
                        break;
                    case 'e':
                        print_entities = true;
                        break;
                    case 'h':
                        print_headers = true;
                        break;
                    default:
                        printf("unknown option '%c'\n", opt);
                }
            }
        }
    }
}

void Com_Error(errortype_t code, const char *fmt, ...) {
    va_list argptr;
    static char msg[1024];

            va_start(argptr, fmt);
    vsnprintf(msg, 1024, fmt, argptr);
            va_end(argptr);

    fprintf(stdout, "ERROR: %s\n", msg);
    if (code == ERR_DROP)
        exit(EXIT_FAILURE);
}


/*
 Iterate over the files
 listing the textures used in each.
*/
int print_textures_of_maps_in_directory(char *filepath) {
    int status = 0;
    int nfiles = 0;

    for (auto &path : std::filesystem::directory_iterator(filepath)) {

        if (path.path().extension() != ".bsp")
            continue;

        printf("opening %s\n", path.path().string().c_str());

        std::string filename = path.path().string();
        std::vector<uint8_t> buf;
        dheader_t header;
        FILE *f = fopen(filename.c_str(), "rb");
        if (!read_bsp(f, header, buf, filename.c_str())) {
            if (print_headers) {
                printf("Map textures:\n");
                printf("======================:\n");
            }
            print_bsp_textures(header.lumps[LUMP_TEXINFO], buf, filename.c_str());
        }
        fclose(f);

        nfiles++;
    }

    printf("%i map files processed.\n", nfiles);
    return status;
}

