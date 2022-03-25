arbo : 
	[ ! -d "./bin/" ] && mkdir bin || true
	[ ! -d "./obj/" ] && mkdir obj || true
	[ ! -d "./src/" ] && mkdir src || true
	[ -f "./lecteur.c" ] && mv ./lecteur.c ./src/ || true
	[ -f "./lecteur.c" ] && mv ./lecteur.c ./src/ || true
	[ -f "./lecteur.c" ] && mv ./lecteur.c ./src/ || true
	[ -f "./audio.c" ] && mv ./audio.c ./src/ || true
	[ -f "./audio.h" ] && mv ./audio.h ./src/ || true
	[ -f "./test.wav" ] && mv ./test.wav ./src/ || true

audio:
	gcc -c src/audio.c -o obj/audio.o


clean:
	rm -f bin/*
	rm -f obj/*

server:
	gcc -Wall -c src/audioserver.c -o obj/server.o
	gcc obj/audio.o obj/server.o -o bin/server

client:
	gcc -Wall -c src/audioclient.c -o obj/client.o
	gcc obj/audio.o obj/client.o -o bin/client

client_server: arbo clean audio server client

lecteur: arbo clean audio
	gcc -Wall -c src/lecteur.c -o obj/lecteur.o
	gcc obj/audio.o obj/lecteur.o -o bin/lecteur
