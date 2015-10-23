#include "base.h"

int move_speed;
int volume;
bool WINCAP = false;
sf::Color GLOBAL_COLOURS[4];

int SCREEN_MODE;// 0 = windowes, 1 = fullscreen
int SCREEN_WIDTH;
int SCREEN_HEIGHT;




sf::RenderWindow WINDOW_MAIN;

sf::Font Font_AmbleReg;

int velx,vely,SpeedTimes,BlocksTimes,HighScore;
bool Jumping,Falling,showtutorial,forceJump,LaunchCredits = false;
sf::Clock AddBlocksClock;
sf::Event event;

sf::Music msMenu,msGame;



sf::Color COLORS[] = {  sf::Color::Blue,
                        sf::Color::Cyan,
                        sf::Color::Green,
                        sf::Color::Magenta,
                        sf::Color::Red,
                        sf::Color::Yellow};



CONSList *FirstConsList,*LastConsList;



bool init(){

if( SCREEN_MODE == 0 )//Windowed
WINDOW_MAIN.create( sf::VideoMode( SCREEN_WIDTH, SCREEN_HEIGHT ),"Collapse!",sf::Style::Default );
else//Fullscreen
WINDOW_MAIN.create( sf::VideoMode( SCREEN_WIDTH, SCREEN_HEIGHT ),"Collapse!",sf::Style::Fullscreen );

if( WINDOW_MAIN.isOpen() == false ) return false;
std::cout<<"Window Loaded\n";

WINDOW_MAIN.setFramerateLimit(60);

//Init Fonts
Font_AmbleReg.loadFromFile( "Data//Fonts//Amble-Regular.ttf" );

std::cout<<"Fonts Loaded\n";


GLOBAL_COLOURS[ 0 ] = sf::Color::Black;
GLOBAL_COLOURS[ 1 ] = sf::Color::White;
GLOBAL_COLOURS[ 2 ] = sf::Color::Black;
GLOBAL_COLOURS[ 3 ] = sf::Color::Black;

//play music
msMenu.openFromFile( "Data//Music//01 A Night Of Dizzy Spells.ogg" );
msGame.openFromFile( "Data//Music//04 All of Us.ogg" );
//set volume
msMenu.setVolume( volume );
msGame.setVolume( volume );
msMenu.setLoop( true );
msGame.setLoop( true );
return true;
}
