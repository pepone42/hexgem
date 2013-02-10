CC=gcc -g -Wall
CFLAGS=`sdl-config --cflags`
LDFLAGS=`sdl-config --libs` -lSDL_mixer

CCPND=pandora-gcc -g -Wall
CFLAGSPND=`pandora-sdl-config --cflags` -DPANDORA
LDFLAGSPND=`pandora-sdl-config --libs` -lSDL_mixer
PNDTOOL=~/devel/pandora-dev/sdk_utils/
PANDORAIP=192.168.1.7
PANDORADEST=/media/mmcblk0p1/pandora/apps
PANDORAUSER=pepone

OBJECT =	../build/host/board.o	../build/host/main.o	../build/host/screen.o 	\
			../build/host/fskip.o	../build/host/random.o	../build/host/hiscore.o \
			../build/host/sound.o

OBJECTPND =	../build/pandora/board.o ../build/pandora/main.o 	../build/pandora/screen.o	\
			../build/pandora/fskip.o ../build/pandora/random.o 	../build/pandora/hiscore.o 	\
			../build/pandora/sound.o

all : ../build/host/hexgem

../hexgem.pnd : ../build/pandora/hexgem
	cp -rf ../pnd/ ../build/pandora/
	cp ../build/pandora/hexgem ../build/pandora/pnd
	$(PNDTOOL)/pnd_make.sh -d ../build/pandora/pnd -p ../hexgem.pnd -i ../build/pandora/pnd/hexgem.png

pnd : ../hexgem.pnd

push : ../hexgem.pnd
	scp ../hexgem.pnd  $(PANDORAUSER)@$(PANDORAIP):$(PANDORADEST)


../build/host/hexgem : $(OBJECT)
	$(CC) $^ -o $@ $(LDFLAGS)

../build/host/%.o : %.c
	mkdir -p ../build/host
	$(CC) -c $< -o $@ $(CFLAGS)

../build/pandora/hexgem : $(OBJECTPND)
	$(CCPND) $^ -o $@ $(LDFLAGSPND)

../build/pandora/%.o : %.c
	mkdir -p ../build/pandora
	$(CCPND) -c $< -o $@ $(CFLAGSPND)


clean:
	rm -rf ../build/host/* ../build/pandora/* ../hexgem.pnd ../sc.dat
