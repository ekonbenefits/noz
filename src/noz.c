#include <errno.h>
#include <stdio.h>
#include <string.h>

#define EOF_MARKER 0x1D
#define MAX_PATH_BUFFER 1024

static const char *normalize_arg_path(const char *arg, char *buffer, size_t buffer_len) {
    size_t len;

    if (arg == NULL) {
        return "";
    }

    len = strlen(arg);
    if (len >= 2 && arg[0] == '"' && arg[len - 1] == '"') {
        size_t inner_len = len - 2;
        if (inner_len + 1 > buffer_len) {
            return arg;
        }

        memcpy(buffer, arg + 1, inner_len);
        buffer[inner_len] = '\0';
        return buffer;
    }

    return arg;
}

static int process_file(const char *path) {
    FILE *in_file;
    FILE *out_file;
    int ch;
    char tmp_path[MAX_PATH_BUFFER];
    size_t path_len;

    path_len = strlen(path);
    if (path_len + 8 >= sizeof(tmp_path)) {
        errno = ENAMETOOLONG;
        return 1;
    }

    in_file = fopen(path, "rb");
    if (in_file == NULL) {
        return 1;
    }

    memcpy(tmp_path, path, path_len + 1);
    strcat(tmp_path, ".noztmp");

    out_file = fopen(tmp_path, "wb");
    if (out_file == NULL) {
        int saved_errno = errno;
        fclose(in_file);
        errno = saved_errno;
        return 1;
    }

    while ((ch = fgetc(in_file)) != EOF) {
        if (((unsigned char)ch) != EOF_MARKER) {
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
    char normalized_path[MAX_PATH_BUFFER];

    if (argc < 2) {
        fprintf(stderr, "Usage: noz <file> [file ...]\n");
        return 1;
    }

    failed = 0;
    for (i = 1; i < argc; ++i) {
        const char *path = normalize_arg_path(argv[i], normalized_path, sizeof(normalized_path));

        if (process_file(path) != 0) {
            const char *reason = strerror(errno);
            if (reason == NULL || reason[0] == '\0') {
                reason = "unknown error";
            }
            fprintf(stderr, "Failed to process '%s': %s\n", path, reason);
            failed = 1;
        }
    }

    return failed;
}

int main(int argc, char **argv) {
    return noz_run(argc, argv);
}
