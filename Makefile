CC = gcc
CFLAGS = -g -Wall

all: test_assign1

test_assign1: test_assign1_1.o storage_mgr.o dberror.o
	$(CC) $(CFLAGS) -o test_assign1 test_assign1_1.o storage_mgr.o dberror.o

test_assign1_1.o: test_assign1_1.c dberror.h storage_mgr.h test_helper.h
	$(CC) $(CFLAGS) -c test_assign1_1.c

storage_mgr.o: storage_mgr.c storage_mgr.h dberror.h
	$(CC) $(CFLAGS) -c storage_mgr.c

dberror.o: dberror.c dberror.h
	$(CC) $(CFLAGS) -c dberror.c

clean:
	rm -f test_assign1 *.o *~