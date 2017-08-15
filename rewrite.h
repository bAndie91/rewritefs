#include <sys/types.h>
#include <pthread.h>
#include <pcre.h>

/*
 * Type definition
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
    int autocreate;
    DIR *orig_dir;
};

enum type {
    CMDLINE,
    RULE,
    END
};

/*
 * Global variables
 */
extern struct config config;

void parse_args(int argc, char **argv, struct fuse_args *outargs);
const char *rewrite(const char *path, void **buf);

/* Lock for process EUID/EGID/umask */
extern pthread_rwlock_t rwlock;

#define RLOCK(expr) { \
    pthread_rwlock_rdlock(&rwlock);\
    expr; \
    pthread_rwlock_unlock(&rwlock); \
}

#define WLOCK(expr) { \
    pthread_rwlock_wrlock(&rwlock); \
    uid_t _euid = geteuid(); gid_t _egid = getegid(); mode_t _umask = umask(fuse_get_context()->umask); \
    seteuid(fuse_get_context()->uid); setegid(fuse_get_context()->gid); \
    expr; \
    seteuid(_euid); setegid(_egid); umask(_umask); \
    pthread_rwlock_unlock(&rwlock); \
}
