all: sock_timeo raft raft_1

sock_timeo: sock_timeo.c
	gcc sock_timeo.c -o sock_timeo

raft: raft.c
	gcc raft.c -o raft -pthread

raft_1: raft_1.c
	gcc raft_1.c -o raft_1 -pthread

PHONY: clean

clean:
	rm sock_timeo raft raft_1
