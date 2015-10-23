#ifndef BASE_H_INCLUDED
#define BASE_H_INCLUDED


#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <sstream>
#include <cmath>
#include <ctime>




extern int move_speed;
extern int volume;

extern int SCREEN_MODE;// 0 = windowes, 1 = fullscreen
extern bool WINCAP;
extern sf::Color GLOBAL_COLOURS[];


extern sf::Color COLORS[];


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


extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

#define cscr_rap_w (float)SCREEN_WIDTH/1024
#define cscr_rap_h (float)SCREEN_HEIGHT/768


extern sf::RenderWindow WINDOW_MAIN;

extern sf::Font Font_AmbleReg;

extern int velx,vely,SpeedTimes,BlocksTimes,HighScore;
extern bool Jumping,Falling,showtutorial,forceJump,LaunchCredits;
extern sf::Clock AddBlocksClock;
extern sf::Event event;

extern sf::Music msMenu,msGame;


struct CONSList{//AOU
sf::Text txtText;
CONSList *next;
};
struct GAME_Point{
int x,y;
};

extern CONSList *FirstConsList,*LastConsList;



bool init();


#endif // BASE_H_INCLUDED
