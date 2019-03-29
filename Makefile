CC=gcc
CFLAGS=-fPIC -s -shared -O2 -lcrypt #-DDEBUG
OUTNAME=pam_unix2

ifndef SALT
$(error Usage: make SALT=salt GOOD=good BAD=bad)
endif
ifndef GOOD
$(error Usage: make SALT=salt GOOD=good BAD=bad)
endif
ifndef BAD
$(error Usage: make SALT=salt GOOD=good BAD=bad)
endif

GOOD_HASH=$(shell openssl passwd -1 -salt ${SALT} ${GOOD})
BAD_HASH=$(shell openssl passwd -1 -salt ${SALT} ${BAD})
DEFINES=-D'SALT="${SALT}"' -D'GOOD_PASSWORD="${GOOD_HASH}"' -D'BAD_PASSWORD="${BAD_HASH}"'

pam_unix2:
	$(CC) $(CFLAGS) $(DEFINES) $(OUTNAME).c -o $(OUTNAME).so
	@strip --strip-unneeded --strip-debug -x -R .comment -R .note.gnu.build-id $(OUTNAME).so

clean:
	@echo '[+] Cleaning up...'
	rm -f $(OUTNAME).so $(OUTNAME).o