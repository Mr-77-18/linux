all: sock_timeo raft raft_1 mess_test_client mess_test_service raft_2 test

sock_timeo: sock_timeo.c
	gcc sock_timeo.c -o sock_timeo

raft: raft.c
	gcc raft.c -o raft -pthread

raft_1: raft_1.c
	gcc raft_1.c -o raft_1 -pthread

mess_test_client: mess_test_client.c
	gcc -o mess_test_client mess_test_client.c -lnsl

mess_test_service: mess_test_service.c
	gcc -o mess_test_service mess_test_service.c -lnsl

raft_2: raft_2.c
	gcc -o raft_2 raft_2.c -lnsl -pthread

test: test.c
	gcc -o test test.c -pthread -lnsl

PHONY: clean

clean:
	rm sock_timeo raft raft_1 mess_test_service mess_test_client raft_2
