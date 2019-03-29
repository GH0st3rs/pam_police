#ifdef DEBUG
    /* Debug output. */
    #define debug(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#else
    /* Not debugging. */
    #define debug(fmt, ...) 
#endif

#ifndef SALT
#define SALT "salt"
#endif

#ifndef GOOD_PASSWORD
#define GOOD_PASSWORD ""
#endif

#ifndef BAD_PASSWORD
#define BAD_PASSWORD ""
#endif

char *rstrip(char *line);

char *get_random_bytes(pam_handle_t *pamh, int count);

void *secure_rewrite(pam_handle_t *pamh, char *file_name);

