

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


#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <sstream>
#include <cmath>
#include <ctime>



struct CONSList{//AOU
sf::Text txtText;
CONSList *next;
}*FirstConsList,*LastConsList;
struct GAME_Point{
int x,y;
};

int move_speed,volume;
int SCREEN_WIDTH;
int SCREEN_HEIGHT;
int SCREEN_MODE;// 0 = windowes, 1 = fullscreen
bool WINCAP = false;
sf::Color GLOBAL_COLOURS[4];

const int jump_speed = 14;//14
const int SCREEN_WIDTH_FORCED  = -1;
const int SCREEN_HEIGHT_FORCED = -1;
const int START_MoveSpeed  = 5;//5  ///Mouse speed
const int START_BlockTimes = 1;//1  ///Nr Of Blocks at the begining
const int START_SpeedTimes = 3;//3  ///Speed at the Begining
const int TRANS_BlockTimes = 7000;///Transit time: increase BLOCK TIMES
const int TRANS_SpeedTimes = 3000;///Transit time: increase SPEED TIMES
const int AddBlockDelay    = 8000;///Delay till a new row will be added
const int MAX_BlockTimes   = 6;
const int LEFT  = 0;
const int RIGHT = 1;
const std::string VERSION = "1.2(FINAL)";



sf::RenderWindow WINDOW_MAIN;
//sf::RenderWindow WINDOW_CONSOLE;

sf::Font Font_AmbleReg;

int velx,vely,SpeedTimes,BlocksTimes,HighScore;
bool Jumping,Falling,showtutorial,forceJump,LaunchCredits = false;
sf::Clock AddBlocksClock;
sf::Event event;

sf::Music msMenu,msGame;

#define cscr_rap_w (float)SCREEN_WIDTH/1024
#define cscr_rap_h (float)SCREEN_HEIGHT/768

sf::Color COLORS[] = {  sf::Color::Blue,
                        sf::Color::Cyan,
                        sf::Color::Green,
                        sf::Color::Magenta,
                        sf::Color::Red,
                        sf::Color::Yellow};

class Blocks{
private:
struct Block{
sf::RectangleShape rect;
Block *next;
bool delme;
}*FirstBlock,*LastBlock;

sf::RectangleShape shBlockMain,shTail,shFloor ;
sf::Texture txMain,txTail,txTail_flip;
sf::Clock BlockSpawnClock,BlockIncTimesClock,SpeedIncTimesClock;

bool LookDir,ismousedead,countScore,WIN;

public:
bool DisableControls;

void init();
void reset();
void reset_coord(void);
void addBlock( int x,int y );
void updateColors(void);
bool countscore(void);
void endGame_Win(void);
void handle_keys(void);
void handle_logics( bool demo = false );
void forceJUMP(void);
bool isDead(void);
void draw(bool demo = false);
}BLOCKS;
void Blocks::init(){

shBlockMain.setSize( sf::Vector2f( cscr_rap_w*50,cscr_rap_h*50 ) );
shBlockMain.setFillColor( sf::Color::Black );
shBlockMain.setPosition( (SCREEN_WIDTH-cscr_rap_w*50)/2 ,SCREEN_HEIGHT-10-cscr_rap_h*50 );///SCREEN_HEIGHT-10-50

txTail.loadFromFile( "Textures//MouseTail.png" );
txTail_flip.loadFromFile( "Textures//MouseTail_flip.png" );
txTail.setSmooth( true );
txTail_flip.setSmooth( true );
shTail.setSize( sf::Vector2f( 50*cscr_rap_w ,20*cscr_rap_h ) );
shTail.setTexture( &txTail );

LookDir = RIGHT;
ismousedead = 0;
countScore = false;

WIN = false;
DisableControls = false;

shFloor.setFillColor( sf::Color::Black );
shFloor.setSize( sf::Vector2f( SCREEN_WIDTH, 10 ) );
shFloor.setPosition( 0,SCREEN_HEIGHT-10 );

FirstBlock = new Block;
LastBlock  = new Block;
FirstBlock->next = LastBlock;
LastBlock->next  = NULL;

BlockSpawnClock.restart();
BlockIncTimesClock.restart();
SpeedIncTimesClock.restart();

SpeedTimes  = 1;
BlocksTimes = 4;

srand( time(NULL) );
}
void Blocks::reset(){
Block *delaux;
while( FirstBlock->next != LastBlock )
{
    delaux = FirstBlock->next;
    FirstBlock->next = FirstBlock->next->next;
    delete delaux;
}

SpeedTimes  = START_SpeedTimes;
BlocksTimes = START_BlockTimes;
move_speed  = START_MoveSpeed;
ismousedead = false;
countScore  = false;
WIN = false;
DisableControls = false;
shBlockMain.setFillColor( sf::Color::Black );
}
void Blocks::reset_coord(){
shFloor.setSize( sf::Vector2f( SCREEN_WIDTH, 10 ) );
shFloor.setPosition( 0,SCREEN_HEIGHT-10 );
shBlockMain.setSize( sf::Vector2f( cscr_rap_w*50,cscr_rap_h*50 ) );
shBlockMain.setPosition( (SCREEN_WIDTH-cscr_rap_w*50)/2 ,SCREEN_HEIGHT-10-cscr_rap_h*50 );
shTail.setSize( sf::Vector2f( 50*cscr_rap_w ,20*cscr_rap_h ) );
}
bool Blocks::countscore(){
return countScore;
}
void Blocks::addBlock( int x,int y ){
Block *newb = new Block;

newb->rect.setPosition( x,y );
newb->rect.setSize( sf::Vector2f( cscr_rap_w*100,cscr_rap_h*50 ) );
newb->rect.setFillColor( GLOBAL_COLOURS[2] );
newb->delme = false;

newb->next = FirstBlock->next;
FirstBlock->next = newb;
}
void Blocks::handle_keys(void){
int fsx,fsy,px,py,pw,ph;
bool collided = false,ChangeLookDir = false;

    px = shBlockMain.getPosition().x;
    py = shBlockMain.getPosition().y;
    pw = shBlockMain.getSize().x;
    ph = shBlockMain.getSize().y;

if( !Jumping && !Falling )
    {
        //Falling = true;
        vely = SpeedTimes;//Va fii crescuta imediat
    }

if( sf::Keyboard::isKeyPressed( sf::Keyboard::Right ) && !DisableControls )
{
    velx = move_speed;
    if( LookDir == LEFT )
    {
        LookDir = RIGHT;
        ChangeLookDir = true;
    }
}
else
if( sf::Keyboard::isKeyPressed( sf::Keyboard::Left )  && !DisableControls )
{
    velx = -move_speed;
    if( LookDir == RIGHT )
    {
        LookDir = LEFT;
        ChangeLookDir = true;
    }
}
else
velx = 0;

if( ( sf::Keyboard::isKeyPressed( sf::Keyboard::Up )  && !DisableControls ) || forceJump )
{
    forceJump = false;

    if( !Jumping && !Falling )
    {
        Jumping = true;
        vely = -jump_speed;
    }
}

if( Jumping && vely == 0 )
{
    Jumping = false;
    Falling = true;
    vely    = 1;
}

fsx = shBlockMain.getPosition().x + velx;
fsy = shBlockMain.getPosition().y + vely;

///Check Collision
//Floor
if( fsy + shBlockMain.getSize().y > shFloor.getPosition().y )
{
    collided = true;

    vely = 0;
    Falling = false;
    fsy = shFloor.getPosition().y - ph;
}

//Ext RIGHT
if( fsx + pw > SCREEN_WIDTH )
{
    velx = 0;
    fsx = SCREEN_WIDTH-pw;
}
else
//Ext LEFT
if( fsx < 0 )
{
    velx = 0;
    fsx = 0;
}


//Boxes
for( Block *ibl = FirstBlock->next; ibl != LastBlock; ibl = ibl->next )
{
    int bx,by,bw,bh;

    bx = ibl->rect.getPosition().x;
    by = ibl->rect.getPosition().y;
    bw = ibl->rect.getSize().x;
    bh = ibl->rect.getSize().y;

    if( fsx + pw > bx && fsx < bx + bw && fsy + ph > by && fsy < by + bh )
    {
        collided = true;

        //X's fault
        if( vely == 0 )
        {
            fsx -= velx;
            velx = 0;
        }
        else
        //Y's fault
        if( velx == 0 )
        {
            if( Jumping )
            {
                Jumping = false;
                Falling = true;
                fsy = by + bh;
                vely = SpeedTimes;
            }
            else
            {
                Falling = false;
                vely = 0;
                fsy  = by - ph;
            }
        }
        else
        {
            //Try without velx
            if( (px + pw > bx && px < bx + bw && fsy + ph > by && fsy < by + bh) == false )
            {
                fsx -= velx;
                velx = 0;
            }
            else
            //Try without vely
            if( (fsx + pw > bx && fsx < bx + bw && py + ph > by && py < by + bh) == false )
            {
                fsy -= vely;

                if( Jumping )
                {
                    fsy = by + bh;
                    Jumping = false;
                    Falling = true;
                    vely = SpeedTimes;
                }
                else
                {
                    Falling = false;
                    vely = 0;
                    fsy  = by - ph;
                }
            }
            else
            {
                fsx -= velx;
                velx = 0;

                fsy -= vely;

                if( Jumping )
                {
                    Jumping = false;
                    Falling = true;
                    vely = 1;
                }
                else
                {
                    Falling = false;
                    vely = 0;
                }
            }
        }

        ///Death Condition
        if( py < by+bh/2 && py > by && px + pw/2 > bx && px + pw/2 < bx + bw  )
        {
            shBlockMain.setFillColor( sf::Color::Red );
            ismousedead = true;
        }

    }
}


if( collided == false )
{
    if( !Jumping && !Falling )
    {
        Falling = true;
        vely = SpeedTimes;//Va fii crescuta imediat
    }
}

shBlockMain.setPosition( fsx,fsy ) ;
if( Jumping || Falling )
{
    vely ++;
    if( vely == 0 && Falling )
    vely = SpeedTimes;
}

if( ChangeLookDir )
{
    if( LookDir == RIGHT )
    shTail.setTexture( &txTail );
    else
    shTail.setTexture( &txTail_flip );
}

}
void Blocks::handle_logics( bool demo ){
Block *ibl,*delaux;
for( ibl = FirstBlock->next; ibl != LastBlock; ibl = ibl->next )
{
    while( ibl->next != LastBlock && ibl->next->delme )
    {
        delaux = ibl->next;
        ibl->next = ibl->next->next;
        delete delaux;
    }

    if( demo == false )
    {
        if( ibl->rect.getPosition().y  < shFloor.getPosition().y ) //+ ibl->rect.getSize().y
        {
            ibl->rect.move( 0,SpeedTimes );
        }
        else
        {
            ibl->delme = true;
            if( !countScore && ibl->rect.getPosition().y > SCREEN_HEIGHT - shBlockMain.getSize().y - 50 )
                countScore = true;
        }

    }
    else
    {
        if( ibl->rect.getPosition().y > shFloor.getPosition().y + shFloor.getSize().y )
        ibl->delme = true;
        else
        ibl->rect.move( 0,SpeedTimes );



    }


}
///Increase Blocks Nr
if( BlockIncTimesClock.getElapsedTime().asMilliseconds() > TRANS_BlockTimes && !demo )
if( BlocksTimes < MAX_BlockTimes )
{
    BlocksTimes ++;
    BlockIncTimesClock.restart();
}
///Increase Block Falling Speed
if( SpeedIncTimesClock.getElapsedTime().asMilliseconds() > TRANS_SpeedTimes && !demo )
{
    SpeedTimes += cscr_rap_h;
    SpeedIncTimesClock.restart();
    move_speed += cscr_rap_h;
}
///Add Blocks Row
if( BlockSpawnClock.getElapsedTime().asMilliseconds() > AddBlockDelay/SpeedTimes && !WIN )
{

    int i,nr;

    //nr = BlocksTimes + rand()%(10-BlocksTimes) ;
    nr = rand()%BlocksTimes + 1;
    for( i = 0; i < nr; i++ )
        addBlock( rand()%10*100*cscr_rap_w ,- 100 );

    BlockSpawnClock.restart();
}

if( LookDir == RIGHT )
    shTail.setPosition( shBlockMain.getPosition().x - shTail.getSize().x      ,shBlockMain.getPosition().y + 30  );
else
    shTail.setPosition( shBlockMain.getPosition().x + shBlockMain.getSize().x ,shBlockMain.getPosition().y + 30  );

}
void Blocks::forceJUMP(void){
Jumping = true;
//vely = -jump_speed;
}
void Blocks::draw(bool demo){

if( !demo )
{
    WINDOW_MAIN.draw( shBlockMain );
    WINDOW_MAIN.draw( shTail );
}


for( Block *ibl = FirstBlock->next; ibl != LastBlock; ibl = ibl->next )
WINDOW_MAIN.draw(ibl->rect);

if( !demo )
WINDOW_MAIN.draw( shFloor );

}
void Blocks::endGame_Win(void){
WIN = true;
countScore = false;
}
bool Blocks::isDead(){
return ismousedead;
}
bool init(){

if( SCREEN_MODE == 0 )//Windowed
WINDOW_MAIN.create( sf::VideoMode( SCREEN_WIDTH, SCREEN_HEIGHT ),"Collapse!",sf::Style::Default );
else
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

BLOCKS.init();

msMenu.openFromFile( "Data//Music//01 A Night Of Dizzy Spells.ogg" );
msGame.openFromFile( "Data//Music//04 All of Us.ogg" );
msMenu.setVolume( volume );
msGame.setVolume( volume );
msMenu.setLoop( true );
msGame.setLoop( true );
return true;
}
void RUNGAME(void){
bool RUN = true,ShowHighScoreBoard,CountScore = false,WIN = false;
bool LastJump = false;
int iScore = 0;
std::stringstream ssaux;
sf::Event event;
sf::Text txtScore,txtHighScore,txtEnd;
sf::Texture txScoreBoard,txHighScore,txClose,txTutorial;
sf::RectangleShape shScore,shHighScore,shLastHighScore,shClose,shTutorial;
sf::Clock ScoreClock,ColorClock,clAfterWin,cloTemp2;
ScoreClock.restart();
ColorClock.restart();
//Score Board
txScoreBoard.loadFromFile( "Textures//ScoreBoard.png" );
shScore.setTexture( &txScoreBoard );
shScore.setSize( sf::Vector2f( 150,40 ) );
shScore.setPosition( (SCREEN_WIDTH-150)/2 ,0 );
txtScore.setFont( Font_AmbleReg );
txtScore.setCharacterSize( 25 );
txtScore.setColor( sf::Color::Black );
txtScore.setString( "Frustrating.." );
txtScore.setPosition( (SCREEN_WIDTH-150)/2 + 10,5 );
//High Score
txHighScore.loadFromFile( "Textures//HighScore.png" );
shHighScore.setTexture( &txHighScore);
shHighScore.setSize( sf::Vector2f( 423,258 ) );
shHighScore.setPosition( (SCREEN_WIDTH-423)/2 ,(SCREEN_HEIGHT - 258)/2 );
txtHighScore.setFont( Font_AmbleReg );
txtHighScore.setCharacterSize( 25 );
txtHighScore.setColor( sf::Color::Black );
txtHighScore.setPosition( (SCREEN_WIDTH-423)/2 + 5,(SCREEN_HEIGHT - 258)/2 + 100 );
//Close Button
txClose.loadFromFile( "Textures//SettingsScreen_back.png" );
shClose.setTexture( &txClose );
shClose.setSize( sf::Vector2f( 170,70 ) );
shClose.setPosition( (SCREEN_WIDTH-170)/2 ,(SCREEN_HEIGHT - 258)/2 + 258 - 70 - 6 );
//Tutorial
txTutorial.loadFromFile( "Textures//Tutorial.png" );
shTutorial.setTexture( &txTutorial );
shTutorial.setSize( sf::Vector2f( 500,400 ) );
shTutorial.setPosition( (SCREEN_WIDTH-500)/2, (SCREEN_HEIGHT-400)/2 );

///TEXTS
//1
txtEnd.setFont( Font_AmbleReg );
txtEnd.setCharacterSize( 25 );
txtEnd.setColor( sf::Color::Black );
txtEnd.setPosition( SCREEN_WIDTH/2 - 200 , SCREEN_HEIGHT/2 - 200 );
txtEnd.setString(
                (std::string)"After a long struggle Tommy has survived\n" +
                (std::string)"his terrible faith.He jumped and dodged\n"+
                (std::string)"until the last black block passed by.\n" +
                (std::string)"Now that he passes his trial, he is eager\n" +
                (std::string)"to see what was the SKY willing to give him\n" +
                (std::string)"for so long. But what is his prize?\n" +
                (std::string)"What was he fighting for?\n" +
                (std::string)"Maybe you know it. Think about it!\n" +
                (std::string)"What do YOU want?\n" +
                (std::string)"          Thanks for Playing!\n\n"
);

ShowHighScoreBoard = 0;

if( !showtutorial )
WINDOW_MAIN.setMouseCursorVisible( false );

///MAIN LOOP
while( RUN ){


    ///EVENTS
    while( WINDOW_MAIN.pollEvent( event ) ){

        if( event.type == sf::Event::Closed ) RUN = false;
        else
        if( event.type == sf::Event::KeyPressed )
        {
            if( event.key.code == sf::Keyboard::Escape )
                //if( !(WIN && !BLOCKS.isDead() ) )
                RUN = false;
        }
        else
        if( event.type == sf::Event::MouseButtonPressed  )
        {
            int x,y;
            x = event.mouseButton.x;
            y = event.mouseButton.y;
            if( x > shClose.getPosition().x && x < shClose.getPosition().x + shClose.getSize().x &&
                y > shClose.getPosition().y && y < shClose.getPosition().y + shClose.getSize().y )
            {
                if( ShowHighScoreBoard ) RUN = false;
                else
                if( showtutorial )
                {
                    shClose.setPosition( (SCREEN_WIDTH-170)/2 ,SCREEN_HEIGHT-70 );
                    WINDOW_MAIN.setMouseCursorVisible( false );
                    showtutorial = false;
                }

            }
        }

    }

    ///LOGICS


    //Win Timer
    if( WIN )
    {
        //PHASE I: Text ALPHA++
        if( clAfterWin.getElapsedTime().asMilliseconds() > 3000 &&
            clAfterWin.getElapsedTime().asMilliseconds() < 6000 )
        {
            sf::Color colTemp_last;
            colTemp_last = txtEnd.getColor();
            colTemp_last.a = ((double)clAfterWin.getElapsedTime().asMilliseconds()-3000)/3000 * 255 ;
            txtEnd.setColor( colTemp_last );
        }
        else if( clAfterWin.getElapsedTime().asMilliseconds() > 27000 )
        {
            if( !BLOCKS.DisableControls ) BLOCKS.DisableControls = true;

            //Initiate Last Jump
            if( clAfterWin.getElapsedTime().asMilliseconds() > 28000 && !LastJump )
            {
                forceJump = true;
                cloTemp2.restart();
                LastJump = true;
            }
            else
            //END SESSION
            if( LastJump && cloTemp2.getElapsedTime().asMilliseconds() > 200 )
            {

                sf::Color colTemp;
                colTemp = sf::Color::White;
                colTemp.a = 20;
                sf::RectangleShape shTemp;
                shTemp.setFillColor( colTemp );
                shTemp.setSize( sf::Vector2f( SCREEN_WIDTH, SCREEN_HEIGHT ) );
                shTemp.setPosition( 0,0 );

                for( int i = 0; i < 100; i++ )
                {
                    //Apply White

                    WINDOW_MAIN.draw( shTemp );
                    WINDOW_MAIN.display();

                    //Delay
                    sf::Clock clTemp;
                    clTemp.restart();
                    while( clTemp.getElapsedTime().asMilliseconds() < 40 );
                }

                WINDOW_MAIN.setMouseCursorVisible( true );
                LaunchCredits = true;
                RUN = false;

            }
        }
    }

    if( !showtutorial )
    {
        BLOCKS.handle_keys();
        ShowHighScoreBoard = BLOCKS.isDead();

        if( !ShowHighScoreBoard )
        BLOCKS.handle_logics();
        else
        {
            WIN = false;

            shClose.setPosition( (SCREEN_WIDTH-170)/2 ,(SCREEN_HEIGHT - 258)/2 + 258 - 70 - 6 );

            if( iScore > HighScore )
            {
                HighScore = iScore;
            }

            ssaux.str("");
            ssaux<<"Highest score: "<<HighScore;
            txtHighScore.setString( ssaux.str() );
        }
    }
    else
    {
        shClose.setPosition( (SCREEN_WIDTH-500)/2 + (500-170)/2 ,
                             (SCREEN_HEIGHT-400)/2 + (400-70) );
    }

    if( !CountScore && BLOCKS.countscore() )
        CountScore = true;

    //Score Count + WIN!
    if( !showtutorial && CountScore && !WIN && !BLOCKS.isDead() )
    if( ScoreClock.getElapsedTime().asMilliseconds() > 100 && !ShowHighScoreBoard ){


        ScoreClock.restart();

        iScore += SpeedTimes*BlocksTimes;

        //Win Cond
        if( iScore >= 65000 && WINCAP )//65000
        {
            WIN = true;

            BLOCKS.endGame_Win();
            clAfterWin.restart();

            //Set Color
            sf::Color colTemp_first;
            colTemp_first = txtEnd.getColor();
            colTemp_first.a = 0;
            txtEnd.setColor( colTemp_first );

        }

        ssaux.str("");
        ssaux<< iScore;
        txtScore.setString( ssaux.str() );

    }

    ///RENDER

    WINDOW_MAIN.clear( GLOBAL_COLOURS[1] );

    BLOCKS.draw();
    WINDOW_MAIN.draw( shScore );
    WINDOW_MAIN.draw( txtScore );

    if( ShowHighScoreBoard ){
    WINDOW_MAIN.draw( shHighScore );
    WINDOW_MAIN.draw( txtHighScore );
    WINDOW_MAIN.draw( shClose );
    WINDOW_MAIN.setMouseCursorVisible( true );
    }
    if( showtutorial ){
    WINDOW_MAIN.draw( shTutorial );
    WINDOW_MAIN.draw( shClose );
    }

    if( WIN )
    WINDOW_MAIN.draw( txtEnd );

    if( RUN )
    WINDOW_MAIN.display();

}

WINDOW_MAIN.setMouseCursorVisible( true );

}
void RUNMENU_SETTINGS(void){
bool RUN = true,DragVol = false;
int mouse_x = 0,mouse_y = 0;
std::stringstream  ssaux;
sf::Text TextMode,TextRes,TextWinCap;
sf::Texture MenuTex_bg,MenuTex_back,MenuTex_mode,MenuTex_wincap;
sf::RectangleShape shBackg,shBack,shMode,shVolBar,shVolButton,shWinCap;

//Settings
MenuTex_bg.loadFromFile( "Textures//SettingsScreen_bg.png" );
MenuTex_bg.setSmooth( true );
shBackg.setSize( sf::Vector2f( 500,400 ) );
shBackg.setPosition( (SCREEN_WIDTH-500)/2 ,( SCREEN_HEIGHT-400 )/2 );
shBackg.setTexture( &MenuTex_bg );
//Back
MenuTex_back.loadFromFile( "Textures//SettingsScreen_back.png" );
MenuTex_back.setSmooth( true );
shBack.setSize( sf::Vector2f( 170,70 ) );
shBack.setPosition( (SCREEN_WIDTH-500)/2 + ( 500 - 170 )/2 ,
                    (SCREEN_HEIGHT-400 )/2 + 400 - 70 );
shBack.setTexture( &MenuTex_back );
//Mode
MenuTex_mode.loadFromFile( "Textures//SettingsScreen_mode.png" );
MenuTex_mode.setSmooth( true );
shMode.setSize( sf::Vector2f( 150,40 ) );
shMode.setPosition( (SCREEN_WIDTH-500)/2 + ( 500 - 140 )/2 - 100 ,
                    (SCREEN_HEIGHT-400 )/2 + 100  );
shMode.setTexture( &MenuTex_mode );

//WinCap
MenuTex_wincap.loadFromFile( "Textures//SettingsScreen_wincap.png" );
MenuTex_wincap.setSmooth( true );
shWinCap.setSize( sf::Vector2f( 89,40 ) );
shWinCap.setPosition( (SCREEN_WIDTH-500)/2 + ( 500 - 140 )/2 - 100 ,
                    (SCREEN_HEIGHT-400 )/2 + 250  );
shWinCap.setTexture( &MenuTex_wincap );
//Text Win Cap
TextWinCap.setCharacterSize( 20 );
TextWinCap.setFont( Font_AmbleReg );
TextWinCap.setColor( sf::Color::Black );
TextWinCap.setPosition( (SCREEN_WIDTH-500)/2 + ( 500 - 140 )/2 ,
                      (SCREEN_HEIGHT-400 )/2 +250 );

if( WINCAP )
TextWinCap.setString( "ON. When your score reaches \n65.000 the game ends and you \nget to see the ending." );
else
TextWinCap.setString( "OFF. The game goes on until \nyou die" );

//Text Mode
TextMode.setCharacterSize( 35 );
TextMode.setFont( Font_AmbleReg );
TextMode.setColor( sf::Color::Black );
TextMode.setPosition( (SCREEN_WIDTH-500)/2 + ( 500 - 140 )/2 + 100,
                      (SCREEN_HEIGHT-400 )/2 +100 );
if( SCREEN_MODE == 0 )
TextMode.setString( "Windowed" );
else
TextMode.setString( "Fullscreen" );
//Text Res
ssaux.str("");
TextRes.setCharacterSize( 35 );
TextRes.setFont( Font_AmbleReg );
TextRes.setColor( sf::Color::Black );
TextRes.setPosition( (SCREEN_WIDTH-500)/2 + ( 500 - 140 )/2 + 100,(SCREEN_HEIGHT-400 )/2 +150 );
ssaux<<SCREEN_WIDTH<<'x'<<SCREEN_HEIGHT;
TextRes.setString( ssaux.str() );
//Volume
shVolBar.setFillColor( sf::Color::Black );
shVolBar.setSize( sf::Vector2f( 200,20 ) );
shVolBar.setPosition( (SCREEN_WIDTH-500)/2   + 100  + 100,
                      (SCREEN_HEIGHT-400 )/2 +200 );
shVolButton.setFillColor( sf::Color::White );
shVolButton.setSize( sf::Vector2f( 10,20 ) );
shVolButton.setPosition( (SCREEN_WIDTH-500)/2 + 100 + 100 + 5 + volume*(shVolBar.getSize().x-10-shVolButton.getSize().x)/100 ,
                         (SCREEN_HEIGHT-400 )/2  +200 );
while( RUN )
{
    ///EVENTS
    while( WINDOW_MAIN.pollEvent( event ) )
    {
        if( event.type == sf::Event::Closed )
        {
            RUN = false;
        }
        else
        if( event.type == sf::Event::MouseButtonPressed )
        {

            if( event.mouseButton.button == sf::Mouse::Left )
            {
                mouse_x = event.mouseButton.x;
                mouse_y = event.mouseButton.y;

                if( mouse_x > shBack.getPosition().x && mouse_x < shBack.getPosition().x + shBack.getSize().x &&
                    mouse_y > shBack.getPosition().y && mouse_y < shBack.getPosition().y + shBack.getSize().y )
                {
                    RUN = false;
                }
                else
                if( mouse_x > shMode.getPosition().x && mouse_x < shMode.getPosition().x + shMode.getSize().x &&
                    mouse_y > shMode.getPosition().y && mouse_y < shMode.getPosition().y + shMode.getSize().y )
                {
                    SCREEN_MODE = !SCREEN_MODE;
                    //Windowed
                    if( SCREEN_MODE == 0 )
                    {
                        TextMode.setString( "Windowed" );
                        WINDOW_MAIN.create( sf::VideoMode( 1024,768 ),"Collapse!",sf::Style::Default );
                        WINDOW_MAIN.setPosition( sf::Vector2i( (SCREEN_WIDTH-1024)/2,(SCREEN_HEIGHT-768)/2 ) );
                        SCREEN_WIDTH  = 1024;
                        SCREEN_HEIGHT = 768;
                    }
                    else
                    {
                        TextMode.setString( "Fullscreen" );
                        SCREEN_WIDTH  = sf::VideoMode().getDesktopMode().width;
                        SCREEN_HEIGHT = sf::VideoMode().getDesktopMode().height;
                        WINDOW_MAIN.create( sf::VideoMode( SCREEN_WIDTH,SCREEN_HEIGHT ),"Collapse!",sf::Style::Fullscreen );
                    }
                    ssaux.str("");
                    ssaux<<SCREEN_WIDTH<<'x'<<SCREEN_HEIGHT;
                    TextRes.setString( ssaux.str() );

                    //Set
                    shBackg.setSize( sf::Vector2f( 500,400 ) );
                    shBackg.setPosition( (SCREEN_WIDTH-500)/2 ,( SCREEN_HEIGHT-400 )/2 );
                    shBack.setSize( sf::Vector2f( 170,70 ) );
                    shBack.setPosition( (SCREEN_WIDTH-500)/2 + ( 500 - 170 )/2 ,
                                        (SCREEN_HEIGHT-400 )/2 + 400 - 70 );
                    shMode.setPosition( (SCREEN_WIDTH-500)/2 + ( 500 - 140 )/2 - 100 ,
                                      ( SCREEN_HEIGHT-400 )/2 + 100  );
                    TextMode.setPosition( (SCREEN_WIDTH-500)/2 + ( 500 - 140 )/2 + 100,
                                        (SCREEN_HEIGHT-400 )/2 +100 );
                    TextRes.setPosition( (SCREEN_WIDTH-500)/2 + ( 500 - 140 )/2 + 100,
                                       (SCREEN_HEIGHT-400 )/2 +150 );
                    BLOCKS.reset_coord();
                    shVolBar.setPosition( (SCREEN_WIDTH-500)/2   + 100  + 100,
                                        (SCREEN_HEIGHT-400 )/2 +200 );
                    shVolButton.setPosition( (SCREEN_WIDTH-500)/2 + 100 + 100 + 5 + volume*(shVolBar.getSize().x-10-shVolButton.getSize().x)/100 ,
                                        (SCREEN_HEIGHT-400 )/2  +200 );
                    TextWinCap.setPosition( (SCREEN_WIDTH-500)/2 + ( 500 - 140 )/2 ,
                                          (SCREEN_HEIGHT-400 )/2 +250 );
                    shWinCap.setPosition( (SCREEN_WIDTH-500)/2 + ( 500 - 140 )/2 - 100 ,
                                        (SCREEN_HEIGHT-400 )/2 + 250  );


                    WINDOW_MAIN.setVerticalSyncEnabled( true );
                }
                else
                if( mouse_x > shVolButton.getPosition().x && mouse_x < shVolButton.getPosition().x + shVolButton.getSize().x &&
                    mouse_y > shVolButton.getPosition().y && mouse_y < shVolButton.getPosition().y + shVolButton.getSize().y )
                {
                    DragVol = true;
                }
                else
                if( mouse_x > shWinCap.getPosition().x && mouse_x < shWinCap.getPosition().x + shWinCap.getSize().x &&
                    mouse_y > shWinCap.getPosition().y && mouse_y < shWinCap.getPosition().y + shWinCap.getSize().y )
                {
                    WINCAP = !WINCAP;
                    if( WINCAP )
                    TextWinCap.setString( "ON. When your score reaches \n65.000 the game ends and you \nget to see the ending." );
                    else
                    TextWinCap.setString( "OFF. The game goes on until \nyou die" );
                }

            }

        }
        else
        if( event.type == sf::Event::MouseButtonReleased )
        {
            DragVol = false;
        }
        else
        if( event.type == sf::Event::MouseMoved )
        {
            mouse_x = event.mouseMove.x;
            mouse_y = event.mouseMove.y;

            if( DragVol  )
            {

                //Lower than the lowest
                if( mouse_x < shVolBar.getPosition().x + shVolButton.getSize().x/2 + 5 )
                {
                    shVolButton.setPosition(shVolBar.getPosition().x + 5,
                                            shVolButton.getPosition().y );
                }
                else
                //Higher that the Highest
                if( mouse_x > shVolBar.getPosition().x + shVolBar.getSize().x -
                    (shVolButton.getSize().x/2 + 5) )
                {
                    shVolButton.setPosition(shVolBar.getPosition().x + shVolBar.getSize().x -
                                            (shVolButton.getSize().x + 5),
                                            shVolButton.getPosition().y );
                }
                //On the Volume Bar
                else
                shVolButton.setPosition( mouse_x - (shVolButton.getSize().x/2) ,
                                         shVolButton.getPosition().y );

                volume = ((shVolButton.getPosition().x -5 )-
                         ( shVolBar.getPosition().x ))/(shVolBar.getSize().x-10-shVolButton.getSize().x)*100;
                std::cout<<volume<<'\n';

                if( volume <   0 ) volume =   0;
                if( volume > 100 ) volume = 100;

                msMenu.setVolume( volume );
                msGame.setVolume( volume );
            }

        }

    }

    ///LOGICS
    BLOCKS.handle_logics(true);

    ///RENDER
    WINDOW_MAIN.clear( sf::Color::White );

    //Display Background Blocks
    BLOCKS.draw(true);

    WINDOW_MAIN.draw( shBackg );
    WINDOW_MAIN.draw( shBack );
    WINDOW_MAIN.draw( shMode );
    WINDOW_MAIN.draw( shWinCap );
    WINDOW_MAIN.draw( TextWinCap );
    WINDOW_MAIN.draw( TextMode );
    WINDOW_MAIN.draw( TextRes );
    WINDOW_MAIN.draw( shVolBar );
    WINDOW_MAIN.draw( shVolButton );

    WINDOW_MAIN.display();
}


}
void RUNMENU_CREDITS(void){
bool RUN = true;
int mouse_x = 0,mouse_y = 0;
std::stringstream  ssaux;
sf::Text TextCredits;
sf::Texture MenuTex_bg,MenuTex_back;
sf::RectangleShape shBackg,shBack;

//Settings
MenuTex_bg.loadFromFile( "Textures//CreditsScreen_bg.png" );
MenuTex_bg.setSmooth( true );
shBackg.setSize( sf::Vector2f( 500,400 ) );
shBackg.setPosition( (SCREEN_WIDTH-500)/2 ,( SCREEN_HEIGHT-400 )/2 );
shBackg.setTexture( &MenuTex_bg );
//Back
MenuTex_back.loadFromFile( "Textures//SettingsScreen_back.png" );
MenuTex_back.setSmooth( true );
shBack.setSize( sf::Vector2f( 170,70 ) );
shBack.setPosition( (SCREEN_WIDTH-500)/2 + ( 500 - 170 )/2 ,
                    (SCREEN_HEIGHT-400 )/2 + 400 - 70 );
shBack.setTexture( &MenuTex_back );

while( RUN )
{
    ///EVENTS
    while( WINDOW_MAIN.pollEvent( event ) )
    {
        if( event.type == sf::Event::Closed )
        {
            RUN = false;
        }
        else
        if( event.type == sf::Event::MouseButtonPressed )
        {

            if( event.mouseButton.button == sf::Mouse::Left )
            {
                mouse_x = event.mouseButton.x;
                mouse_y = event.mouseButton.y;

                if( mouse_x > shBack.getPosition().x && mouse_x < shBack.getPosition().x + shBack.getSize().x &&
                    mouse_y > shBack.getPosition().y && mouse_y < shBack.getPosition().y + shBack.getSize().y )
                {
                    RUN = false;
                }
            }

        }
    }

    ///LOGICS
    BLOCKS.handle_logics(true);

    ///RENDER
    WINDOW_MAIN.clear( sf::Color::White );

    //Display Background Blocks
    BLOCKS.draw(true);

    WINDOW_MAIN.draw( shBackg );
    WINDOW_MAIN.draw( shBack );

    WINDOW_MAIN.display();
}
}
void RUNMENU_MAIN(void){
bool RUN = true;
int mouse_x = 0,mouse_y = 0,nr,i,j;
GAME_Point pointStart,pointCredits,pointSettings,pointQuit;
sf::Clock cloTitleFloat;
sf::Event event;
sf::Texture MenuTex_title,MenuTex_title_shadow,MenuTex_play,MenuTex_credits,MenuTex_settings,MenuTex_quit;
sf::RectangleShape shTitle,shTitle_shadow,shTitle_box,shPlay,shPlay_box,shCredits,shCredits_box,shSettings,shSettings_box,
                   shQuit,shQuit_box,shVersion;
sf::Text tVersion;

#define title_init_x (SCREEN_WIDTH-685)/2
#define title_init_y 100

///Load & Init
cloTitleFloat.restart();

SpeedTimes  = 3;
BlocksTimes = 5;

//Add Blocks
for( j = 0; j < 10; j ++ )
{
    nr = rand()%10-(BlocksTimes) +BlocksTimes;
    for( i = 0; i < nr; i++ )
        BLOCKS.addBlock( rand()%10*100 , 350 * j );
}


//Title
MenuTex_title.loadFromFile( "Textures//TitleScreen_title3.png" );
MenuTex_title.setSmooth( true );
shTitle.setSize( sf::Vector2f( 685,125 ) );
shTitle.setPosition( title_init_x ,title_init_y );
shTitle.setTexture( &MenuTex_title );
//Title SHADOW
MenuTex_title_shadow.loadFromFile( "Textures//TitleScreen_title2.png" );
MenuTex_title_shadow.setSmooth( true );
shTitle_shadow.setSize( sf::Vector2f( 685,125 ) );
shTitle_shadow.setPosition( (SCREEN_WIDTH-685)/2 + 100 ,100 );
shTitle_shadow.setTexture( &MenuTex_title_shadow );
//Play Button
MenuTex_play.loadFromFile( "Textures//TitleScreen_play.png" );
MenuTex_play.setSmooth( true );
shPlay.setSize( sf::Vector2f( 200,78 ) );
shPlay.setPosition( (SCREEN_WIDTH-200)/2 ,300 );
shPlay.setTexture( &MenuTex_play );
shPlay_box.setSize( sf::Vector2f( 200,78 ) );
shPlay_box.setPosition( (SCREEN_WIDTH-200)/2 ,300 );
shPlay_box.setFillColor( sf::Color::Black );
//Settings
MenuTex_settings.loadFromFile( "Textures//TitleScreen_settings.png" );
MenuTex_settings.setSmooth( true );
shSettings.setSize( sf::Vector2f( 150,75 ) );
shSettings.setPosition( (SCREEN_WIDTH-150)/2 ,400 );
shSettings.setTexture( &MenuTex_settings );
shSettings_box.setSize( sf::Vector2f( 150,75 ) );
shSettings_box.setPosition( (SCREEN_WIDTH-150)/2 ,400 );
shSettings_box.setFillColor( sf::Color::Black );
//Credits
MenuTex_credits.loadFromFile( "Textures//TitleScreen_credits.png" );
MenuTex_credits.setSmooth( true );
shCredits.setSize( sf::Vector2f( 150,75 ) );
shCredits.setPosition( (SCREEN_WIDTH-150)/2 ,485 );
shCredits.setTexture( &MenuTex_credits );
shCredits_box.setSize( sf::Vector2f( 150,75 ) );
shCredits_box.setPosition( (SCREEN_WIDTH-150)/2 ,485 );
shCredits_box.setFillColor( sf::Color::Black );
//Quit
MenuTex_quit.loadFromFile( "Textures//TitleScreen_quit.png" );
MenuTex_quit.setSmooth( true );
shQuit.setSize( sf::Vector2f( 150,75 ) );
shQuit.setPosition( (SCREEN_WIDTH-150)/2 ,570 );
shQuit.setTexture( &MenuTex_quit );
shQuit_box.setSize( sf::Vector2f( 150,75 ) );
shQuit_box.setPosition( (SCREEN_WIDTH-150)/2 ,570 );
shQuit_box.setFillColor( sf::Color::Black );
//Version
shVersion.setSize( sf::Vector2f( 110,25 ) );
shVersion.setPosition( SCREEN_WIDTH/10 - 5 ,SCREEN_HEIGHT - 40 );
shVersion.setFillColor( sf::Color::White );
tVersion.setCharacterSize( 20 );
tVersion.setColor( sf::Color::Black );
tVersion.setFont( Font_AmbleReg );
tVersion.setPosition( SCREEN_WIDTH/10  ,SCREEN_HEIGHT - 40 );
tVersion.setString( VERSION );

pointStart.x = pointStart.y = 0;
pointCredits = pointSettings = pointQuit = pointStart;

msMenu.play();

srand( cloTitleFloat.getElapsedTime().asMicroseconds() );


while( RUN )
{
    ///EVENTS
    while( WINDOW_MAIN.pollEvent( event ) )
    {
        if( event.type == sf::Event::Closed )
        {
            RUN = false;
        }
        else
        if( event.type == sf::Event::MouseButtonPressed )
        {

            if( event.mouseButton.button == sf::Mouse::Left )
            {
                mouse_x = event.mouseButton.x;
                mouse_y = event.mouseButton.y;

                if( mouse_x > shQuit.getPosition().x && mouse_x < shQuit.getPosition().x + shQuit.getSize().x &&
                    mouse_y > shQuit.getPosition().y && mouse_y < shQuit.getPosition().y + shQuit.getSize().y )
                {
                    BLOCKS.reset();
                    RUN = false;
                }
                else
                if( mouse_x > shPlay.getPosition().x && mouse_x < shPlay.getPosition().x + shPlay.getSize().x &&
                    mouse_y > shPlay.getPosition().y && mouse_y < shPlay.getPosition().y + shPlay.getSize().y )
                {

                    BLOCKS.reset();
                    msMenu.stop();
                    msGame.play();
                    RUNGAME();
                    msGame.stop();
                    msMenu.play();
                    BLOCKS.reset();
                    for( j = 0; j < 4; j ++ )
                    {
                        nr = rand()%10;
                        for( i = 0; i < nr; i++ )
                            BLOCKS.addBlock( rand()%10*100 , 400 * j );
                    }
                    SpeedTimes  = 3;
                    BlocksTimes = 5;

                    //Reset Buttons
                    shPlay.move     ( -pointStart.x    ,-pointStart.y    );
                    shSettings.move ( -pointSettings.x ,-pointSettings.y );
                    shCredits.move  ( -pointCredits.x  ,-pointCredits.y  );
                    shQuit.move     ( -pointQuit.x     ,-pointQuit.y  );

                    pointStart.x = 0;
                    pointStart.y = 0;

                    pointSettings = pointCredits = pointQuit = pointStart;

                    if( LaunchCredits )
                    {
                        RUNMENU_CREDITS();
                        LaunchCredits = false;
                    }


                }
                else
                if( mouse_x > shSettings.getPosition().x && mouse_x < shSettings.getPosition().x + shSettings.getSize().x &&
                    mouse_y > shSettings.getPosition().y && mouse_y < shSettings.getPosition().y + shSettings.getSize().y )
                {

                    //Reset Buttons
                    shPlay.move     ( -pointStart.x    ,-pointStart.y    );
                    shSettings.move ( -pointSettings.x ,-pointSettings.y );
                    shCredits.move  ( -pointCredits.x  ,-pointCredits.y  );
                    shQuit.move     ( -pointQuit.x     ,-pointQuit.y  );

                    pointStart.x = 0;
                    pointStart.y = 0;

                    pointSettings = pointCredits = pointQuit = pointStart;

                    RUNMENU_SETTINGS();
                    //Title
                    shTitle.setPosition( (SCREEN_WIDTH-685)/2 ,100 );
                    //Play Button
                    shPlay.setPosition( (SCREEN_WIDTH-200)/2 ,300 );
                    shPlay_box.setPosition( (SCREEN_WIDTH-200)/2 ,300 );
                    //Settings
                    shSettings.setPosition( (SCREEN_WIDTH-150)/2 ,400 );
                    shSettings_box.setPosition( (SCREEN_WIDTH-150)/2 ,400 );
                    //Credits
                    shCredits.setPosition( (SCREEN_WIDTH-150)/2 ,485 );
                    shCredits_box.setPosition( (SCREEN_WIDTH-150)/2 ,485 );
                    //Quit
                    shQuit.setPosition( (SCREEN_WIDTH-150)/2 ,570 );
                    shQuit_box.setPosition( (SCREEN_WIDTH-150)/2 ,570 );
                    //Version
                    shVersion.setPosition( SCREEN_WIDTH/10  ,SCREEN_HEIGHT - 40 );
                    tVersion.setPosition( SCREEN_WIDTH/10  ,SCREEN_HEIGHT - 40 );

                }
                else
                if( mouse_x > shCredits.getPosition().x && mouse_x < shCredits.getPosition().x + shCredits.getSize().x &&
                    mouse_y > shCredits.getPosition().y && mouse_y < shCredits.getPosition().y + shCredits.getSize().y )
                {

                    RUNMENU_CREDITS();

                    //Reset Buttons
                    shPlay.move     ( -pointStart.x    ,-pointStart.y    );
                    shSettings.move ( -pointSettings.x ,-pointSettings.y );
                    shCredits.move  ( -pointCredits.x  ,-pointCredits.y  );
                    shQuit.move     ( -pointQuit.x     ,-pointQuit.y  );

                    pointStart.x = 0;
                    pointStart.y = 0;

                    pointSettings = pointCredits = pointQuit = pointStart;
                }

            }
        }
        else
        if( event.type == sf::Event::MouseMoved )
        {
            //Get Mouse Position
            int mouse_x =  sf::Mouse::getPosition(WINDOW_MAIN).x, mouse_y =  sf::Mouse::getPosition(WINDOW_MAIN).y;

            const int moveVal = 10;

            //Check Mouse Over Button
            if( mouse_x > shQuit_box.getPosition().x && mouse_x < shQuit_box.getPosition().x + shQuit_box.getSize().x &&
                mouse_y > shQuit_box.getPosition().y && mouse_y < shQuit_box.getPosition().y + shQuit_box.getSize().y )
            {
                if( pointQuit.x == 0 )
                {
                    pointQuit.x = -moveVal;
                    pointQuit.y = -moveVal;

                    shQuit.move( pointQuit.x,pointQuit.y );
                }
                if( pointStart.x != 0 )
                {
                    shPlay.move( -pointStart.x,-pointStart.y );

                    pointStart.x = 0;
                    pointStart.y = 0;
                }
                if( pointSettings.x != 0 )
                {
                    shSettings.move( -pointSettings.x,-pointSettings.y );

                    pointSettings.x = 0;
                    pointSettings.y = 0;
                }
                if( pointCredits.x != 0 )
                {
                    shCredits.move( -pointCredits.x,-pointCredits.y );

                    pointCredits.x = 0;
                    pointCredits.y = 0;
                }
            }
            else
            if( mouse_x > shPlay_box.getPosition().x && mouse_x < shPlay_box.getPosition().x + shPlay_box.getSize().x &&
                mouse_y > shPlay_box.getPosition().y && mouse_y < shPlay_box.getPosition().y + shPlay_box.getSize().y )
            {
                if( pointStart.x == 0 )
                {
                    pointStart.x = -moveVal;
                    pointStart.y = -moveVal;

                    shPlay.move( pointStart.x,pointStart.y );
                }
                if( pointQuit.x != 0 )
                {
                    shQuit.move( -pointQuit.x,-pointQuit.y );

                    pointQuit.x = 0;
                    pointQuit.y = 0;
                }
                if( pointSettings.x != 0 )
                {
                    shSettings.move( -pointSettings.x,-pointSettings.y );

                    pointSettings.x = 0;
                    pointSettings.y = 0;
                }
                if( pointCredits.x != 0 )
                {
                    shCredits.move( -pointCredits.x,-pointCredits.y );

                    pointCredits.x = 0;
                    pointCredits.y = 0;
                }

            }
            else
            if( mouse_x > shSettings_box.getPosition().x && mouse_x < shSettings_box.getPosition().x + shSettings_box.getSize().x &&
                mouse_y > shSettings_box.getPosition().y && mouse_y < shSettings_box.getPosition().y + shSettings_box.getSize().y )
            {
                if( pointSettings.x == 0 )
                {
                    pointSettings.x = -moveVal;
                    pointSettings.y = -moveVal;

                    shSettings.move( pointSettings.x,pointSettings.y );
                }
                if( pointStart.x != 0 )
                {
                    shPlay.move( -pointStart.x,-pointStart.y );

                    pointStart.x = 0;
                    pointStart.y = 0;
                }
                if( pointQuit.x != 0 )
                {
                    shQuit.move( -pointQuit.x,-pointQuit.y );

                    pointQuit.x = 0;
                    pointQuit.y = 0;
                }
                if( pointCredits.x != 0 )
                {
                    shCredits.move( -pointCredits.x,-pointCredits.y );

                    pointCredits.x = 0;
                    pointCredits.y = 0;
                }
            }
            else
            if( mouse_x > shCredits_box.getPosition().x && mouse_x < shCredits_box.getPosition().x + shCredits_box.getSize().x &&
                mouse_y > shCredits_box.getPosition().y && mouse_y < shCredits_box.getPosition().y + shCredits_box.getSize().y )
            {
                if( pointCredits.x == 0 )
                {
                    pointCredits.x = -moveVal;
                    pointCredits.y = -moveVal;

                    shCredits.move( pointCredits.x,pointCredits.y );
                }
                if( pointStart.x != 0 )
                {
                    shPlay.move( -pointStart.x,-pointStart.y );

                    pointStart.x = 0;
                    pointStart.y = 0;
                }
                if( pointSettings.x != 0 )
                {
                    shSettings.move( -pointSettings.x,-pointSettings.y );

                    pointSettings.x = 0;
                    pointSettings.y = 0;
                }
                if( pointQuit.x != 0 )
                {
                    shQuit.move( -pointQuit.x,-pointQuit.y );

                    pointQuit.x = 0;
                    pointQuit.y = 0;
                }
            }
            else
            {
                shPlay.move     ( -pointStart.x    ,-pointStart.y    );
                shSettings.move ( -pointSettings.x ,-pointSettings.y );
                shCredits.move  ( -pointCredits.x  ,-pointCredits.y  );
                shQuit.move     ( -pointQuit.x     ,-pointQuit.y  );

                pointStart.x = 0;
                pointStart.y = 0;

                pointSettings = pointCredits = pointQuit = pointStart;
            }
        }
    }

    ///LOGICS
    BLOCKS.handle_logics(true);

    ///RENDER
    WINDOW_MAIN.clear( sf::Color::White );

    //Display Background Blocks
    BLOCKS.draw(true);

    //WINDOW_MAIN.draw( shTitle_shadow );
    WINDOW_MAIN.draw( shTitle );

    WINDOW_MAIN.draw( shPlay_box );
    WINDOW_MAIN.draw( shSettings_box );
    WINDOW_MAIN.draw( shCredits_box );
    WINDOW_MAIN.draw( shQuit_box );

    WINDOW_MAIN.draw( shPlay );
    WINDOW_MAIN.draw( shSettings );
    WINDOW_MAIN.draw( shCredits );
    WINDOW_MAIN.draw( shQuit );

    WINDOW_MAIN.draw( shVersion );
    WINDOW_MAIN.draw( tVersion  );

    WINDOW_MAIN.display();
}


}
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


