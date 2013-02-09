CC=gcc -g -Wall
CFLAGS=`sdl-config --cflags`
LDFLAGS=`sdl-config --libs` -lSDL_mixer

CCPND=pandora-gcc -g -Wall
CFLAGSPND=`pandora-sdl-config --cflags` -DPANDORA
LDFLAGSPND=`pandora-sdl-config --libs` -lSDL_mixer
PNDTOOL=~/opt/pandora-dev/sdk_utils/
PANDORAIP=192.168.1.7
PANDORADEST=/media/mmcblk0p1/pandora/apps
PANDORAUSER=pepone

OBJECT = board.o main.o screen.o fskip.o random.o hiscore.o sound.o
OBJECTPND = board.pndo main.pndo screen.pndo fskip.pndo random.pndo hiscore.pndo sound.pndo

all : puzzl hexgem.pandora

hexgem.pnd : hexgem.pandora 
	cp hexgem.pandora pnd
	$(PNDTOOL)/pnd_make.sh -d pnd/ -p hexgem.pnd -i pnd/hexgem.png

push : hexgem.pnd
	scp hexgem.pnd  $(PANDORAUSER)@$(PANDORAIP):$(PANDORADEST)


puzzl : $(OBJECT)
	$(CC) $^ -o $@ $(LDFLAGS)

%.o : %.c
	$(CC) -c $< -o $@ $(CFLAGS)

hexgem.pandora : $(OBJECTPND)
	$(CCPND) $^ -o $@ $(LDFLAGSPND)

%.pndo : %.c
	$(CCPND) -c $< -o $@ $(CFLAGSPND)


clean:
	rm *.o *.pndo puzzl hexgem.pandora
