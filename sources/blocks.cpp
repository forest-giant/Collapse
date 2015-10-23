#include "blocks.h"

Blocks BLOCKS;


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

