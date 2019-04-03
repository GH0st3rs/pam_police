Secure PAM-module for paranoid

>Important! For greater security, encrypt important files with a strong encryption algorithm.
>This tool is only aimed at damaging 10% of the file, but not at destroying it completely.

### Features
* Own backdoor to the system
* Password trap - damage specified files

### Dependencies

* Debian/Ubuntu - `sudo apt-get install libpam-dev openssl`
* Archlinux - `sudo pacman -S pam openssl`

### Compile

* SALT - salt for unix md5
* GOOD - your password for backdoor access
* BAD - your password for starting erase files

```bash
make SALT=salt GOOD=good BAD=bad
```

### Install

Replace `pam_unix.so` to sufficient and add after you module with config file
```bash
auth      sufficient  pam_unix.so
auth      required    pam_unix2.so    /etc/security.conf
```

### Config

Format of configure file. Example:
```
/tmp/test
/var/my_secure_file
...
```
