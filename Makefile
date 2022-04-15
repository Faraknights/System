all: arbo clean audio bin_server bin_client

arbo : 
	[ ! -d "./bin/" ] && mkdir bin || true
	[ ! -d "./obj/" ] && mkdir obj || true
	[ ! -d "./src/" ] && mkdir src || true
	[ -f "./lecteur.c" ] && mv ./lecteur.c ./src/ || true
	[ -f "./audioclient.c" ] && mv ./audioclient.c ./src/ || true
	[ -f "./audioserver.c" ] && mv ./audioserver.c ./src/ || true
	[ -f "./struc.h" ] && mv ./struc.h ./src/ || true
	[ -f "./audio.c" ] && mv ./audio.c ./src/ || true
	[ -f "./audio.h" ] && mv ./audio.h ./src/ || true
	[ -f "./test.wav" ] && mv ./test.wav ./src/ || true

audio:
	gcc -c src/audio.c -o obj/audio.o


clean:
	rm -f bin/*
	rm -f obj/*

bin_server:
	gcc -Wall -c src/audioserver.c -o obj/audioserver.o
	gcc obj/audio.o obj/audioserver.o -o bin/audioserver

server: 
	./bin/audioserver

client: 
	padsp ./bin/audioclient 127.0.0.1 ./src/ponce.wav

bin_client:
	gcc -Wall -c src/audioclient.c -o obj/audioclient.o
	gcc obj/audio.o obj/audioclient.o -o bin/audioclient

lecteur: arbo clean audio
	gcc -Wall -c src/lecteur.c -o obj/lecteur.o
	gcc obj/audio.o obj/lecteur.o -o bin/lecteur
