

/**
Collapse Game
Copyright (C) 2015 Mihai Nicolae

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**/




#include "run.h"
#include "base.h"
#include "blocks.h"







int main(){

///LOAD SETTINGS
std::ifstream in( "settings.txt" );
std::string sread;

in>>sread>>SCREEN_MODE;
if( SCREEN_MODE == -1 ) SCREEN_MODE = 1;
in>>sread>>SCREEN_WIDTH;
if( SCREEN_WIDTH == -1 )
{
    SCREEN_WIDTH  = sf::VideoMode().getDesktopMode().width;
    SCREEN_HEIGHT = sf::VideoMode().getDesktopMode().height;
}
else
in>>SCREEN_HEIGHT;

in>>sread>>volume;
in>>sread>>showtutorial;
in>>sread>>WINCAP;

in.close();

in.open( "Data//data.dat" );
in>>HighScore;
in.close();

if( SCREEN_HEIGHT_FORCED != -1 ) SCREEN_HEIGHT = SCREEN_HEIGHT_FORCED;
if( SCREEN_WIDTH_FORCED  != -1 ) SCREEN_WIDTH  =  SCREEN_WIDTH_FORCED;

///INIT PROGRAM
if( !init() ) return 1;
BLOCKS.init();

//RUN MAIN MENU
RUNMENU_MAIN();
//RUNGAME();


///SAVE SETTINGS
std::ofstream out( "settings.txt" );
out<<"mode: "    <<SCREEN_MODE<<'\n';
out<<"res: "     <<SCREEN_WIDTH<<' '<<SCREEN_HEIGHT<<'\n';
out<<"volume: "  <<volume<<'\n';
out<<"tutorial: "<<showtutorial<<'\n';
out<<"wincap: "  <<WINCAP;
out.close();

out.open( "Data//data.dat" );
out<<HighScore;
out.close();

WINDOW_MAIN.close();

return 0;
}


