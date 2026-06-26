#include <errno.h>
#include <stdio.h>
#include <string.h>

#define EOF_MARKER_1D 0x1D
#define EOF_MARKER_1A 0x1A
#define MAX_PATH_BUFFER 1024
#define TEMP_EXT ".NZT"

static int process_file(const char *path) {
    FILE *in_file;
    FILE *out_file;
    int ch;
    char tmp_path[MAX_PATH_BUFFER];
    size_t path_len;
    char *dot;
    char *scan;

    path_len = strlen(path);
    if (path_len + sizeof(TEMP_EXT) >= sizeof(tmp_path)) {
        errno = ENAMETOOLONG;
        return 1;
    }

    in_file = fopen(path, "rb");
    if (in_file == NULL) {
        return 1;
    }

    memcpy(tmp_path, path, path_len + 1);

    /* DOS-safe temp file: replace existing extension, or append .NZT. */
    dot = NULL;
    for (scan = tmp_path; *scan != '\0'; ++scan) {
        if (*scan == '\\' || *scan == '/' || *scan == ':') {
            dot = NULL;
        } else if (*scan == '.') {
            dot = scan;
        }
    }

    if (dot != NULL) {
        strcpy(dot, TEMP_EXT);
    } else {
        strcat(tmp_path, TEMP_EXT);
    }

    out_file = fopen(tmp_path, "wb");
    if (out_file == NULL) {
        int saved_errno = errno;
        fclose(in_file);
        errno = saved_errno;
        return 1;
    }

    while ((ch = fgetc(in_file)) != EOF) {
        if (((unsigned char)ch) != EOF_MARKER_1D && ((unsigned char)ch) != EOF_MARKER_1A) {
            if (fputc(ch, out_file) == EOF) {
                int saved_errno = errno;
                fclose(in_file);
                fclose(out_file);
                remove(tmp_path);
                errno = saved_errno;
                return 1;
            }
        }
    }

    if (ferror(in_file)) {
        int saved_errno = errno;
        fclose(in_file);
        fclose(out_file);
        remove(tmp_path);
        errno = saved_errno;
        return 1;
    }

    if (fclose(in_file) != 0) {
        int saved_errno = errno;
        fclose(out_file);
        remove(tmp_path);
        errno = saved_errno;
        return 1;
    }

    if (fclose(out_file) != 0) {
        int saved_errno = errno;
        remove(tmp_path);
        errno = saved_errno;
        return 1;
    }

    if (remove(path) != 0) {
        int saved_errno = errno;
        remove(tmp_path);
        errno = saved_errno;
        return 1;
    }

    if (rename(tmp_path, path) != 0) {
        int saved_errno = errno;
        errno = saved_errno;
        return 1;
    }

    return 0;
}

static int noz_run(int argc, char **argv) {
    int i;
    int failed;

    if (argc < 2) {
        fprintf(stderr, "Usage: noz <file> [file ...]\n");
        return 1;
    }

    failed = 0;
    for (i = 1; i < argc; ++i) {
        if (process_file(argv[i]) != 0) {
            const char *reason = strerror(errno);
            if (reason == NULL || reason[0] == '\0') {
                reason = "unknown error";
            }
            fprintf(stderr, "Failed to process '%s': %s\n", argv[i], reason);
            failed = 1;
        }
    }

    return failed;
}

int main(int argc, char **argv) {
    return noz_run(argc, argv);
}
