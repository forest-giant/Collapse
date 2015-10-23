#ifndef BLOCKS_H_INCLUDED
#define BLOCKS_H_INCLUDED

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include "base.h"

extern class Blocks{

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

#endif // BLOCKS_H_INCLUDED
