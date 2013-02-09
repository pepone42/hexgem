export SDL_VIDEODRIVER=omapdss
#export SDL_OMAP_LAYER_SIZE=640x480
export SDL_OMAP_LAYER_SIZE=fullscreen
export SDL_OMAP_VSYNC=1
sudo -n /usr/pandora/scripts/op_lcdrate.sh 60
./hexgem



