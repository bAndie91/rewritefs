void parse_args(int argc, char **argv, struct fuse_args *outargs);
char *rewrite(const char *path);

/*
 * Type definiton 
 */
struct regexp {
    pcre *regexp;
    pcre_extra *extra;
    int captures;
    char *raw;
};

struct rewrite_rule {
    struct regexp *filename_regexp;
    char *rewritten_path; /* NULL for "." */
    struct rewrite_rule *next;
};

struct rewrite_context {
    struct regexp *cmdline; /* NULL for all contexts */
    struct rewrite_rule *rules;
    struct rewrite_context *next;
};

struct config {
    char *config_file;
    char *orig_fs;
    char *mount_point;
    struct rewrite_context *contexts;
    int verbose;
    int auto_mkdir;
};

/*
 * Global variables
 */

struct config config;
