#include <SPI.h>
#include <Game.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>
extern const byte font5x7[];

#define WORLD_W 16
#define WORLD_H 12
uint8_t  byteWidth = 2;

Game game;

extern const byte PROGMEM logo[];
extern const byte PROGMEM world[];
extern const byte PROGMEM tiles[];
extern const byte PROGMEM Tank[];
extern const byte PROGMEM bossTank[];
extern const byte PROGMEM playerTank[];
extern const byte PROGMEM splash[];
extern const byte PROGMEM crate[];
extern const byte PROGMEM fullHeart[];
extern const byte PROGMEM halfHeart[];
extern const byte PROGMEM emptyHeart[];

#define playerW 6
#define playerH 6
byte playerSpeed;
int playerX;
int playerY;
char playerLife;
#define playerLifeMax 6
byte playerDir;
int cameraX;
int cameraY;
byte shake_magnitude;
byte shake_timeLeft;
const PROGMEM uint16_t player_damage_sound[] = {
  0x0045,0x564,0x0000};

#define NUMTankS 16
#define INITNUMTankS 4
#define TankSRATE 6 
#define BOSSFREQ 15
#define BOSSRATE 1
byte boss_nextSpawn;
byte boss_freq;
byte activeTanks;
int  Tanks_x[NUMTankS];
int  Tanks_y[NUMTankS];
byte Tanks_dir[NUMTankS];
char Tanks_life[NUMTankS];
byte Tanks_size[NUMTankS];
byte Tank_maxLife = 20;
byte boss_maxLife = 60;
#define boss_size 6
#define Tank_size 4
const PROGMEM uint16_t Tank_death_sound[] = {
  0x0045,0x184,0x0000};

#define NUMSPLASH 16
boolean splash_active[NUMSPLASH];
int splash_x[NUMSPLASH]; 
int splash_y[NUMSPLASH];
int splash_dir[NUMSPLASH];

#define NUMBULLETS 10
int     bullets_x[NUMBULLETS];
int     bullets_y[NUMBULLETS];
byte    bullets_dir[NUMBULLETS];
boolean bullets_active[NUMBULLETS]; 
byte    bullets_weapon[NUMBULLETS]; 

int     blast_x;
int     blast_y;
byte    blast_lifespan; 
byte    blast_bullet; 
#define NUMWEAPONS 5
byte currentWeapon; 
byte nextShot;
const char strAWM[] PROGMEM = "AWM";
const char strM3[] PROGMEM = "M3";
const char strAK[] PROGMEM = "AK47";
const char strB40[] PROGMEM = "B40";
const char strRPK[] PROGMEM = "RPK";
const char*  const weapon_name[NUMWEAPONS] PROGMEM = {
  strAWM,strM3, strAK, strB40, strRPK};
//cỡ nòng
const byte weapon_size[NUMWEAPONS] = {
  2, 1, 2, 3, 2};
//sức sát thương của các súng
const byte weapon_damage[NUMWEAPONS] = {
  20, 7, 10, 7, 10};
//tốc độ bắn của các súng
const byte weapon_rate[NUMWEAPONS] = {
  10, 1, 2, 20, 3};
//tốc độ đạn di chuyển.
const byte weapon_speed[NUMWEAPONS] = {
  4, 5, 3, 2, 5};
//độ ổn định
const byte weapon_spread[NUMWEAPONS] = {
  1, 2, 1, 0, 2};
//đẩy lùi tanks khi bắn
const byte weapon_ennemyRecoil[NUMWEAPONS] = {
  3, 2, 3, 0, 3};
//giật lùi khi bắn
const byte weapon_playerRecoil[NUMWEAPONS] = {
  0, 0, 1, 3, 2};
//số lượng đạn của các súng
const unsigned int weapon_ammo[NUMWEAPONS] = {
  9999, 300, 150, 15, 75};
unsigned int ammo;

const uint16_t magnum_sound[] PROGMEM = {
  0x0045,0x7049,0x17C,0x784D,0x42C,0x0000};
const uint16_t awm_sound[] PROGMEM = {
  0x0045, 0x0154, 0x0000};
const uint16_t m3_sound[] PROGMEM = {
  0x0045, 0x014C, 0x0000};
const uint16_t ak_sound[] PROGMEM = {
  0x0045, 0x012C, 0x0000};
const uint16_t b40_sound[] PROGMEM = {
  0x0045,0x140,0x8141,0x7849,0x788D,0x52C,0x0000};
const uint16_t rpk_sound[] PROGMEM = {
  0x0045,0x8101,0x7F30,0x0000};
const uint16_t* const weapons_sounds[NUMWEAPONS] PROGMEM= {
  awm_sound, m3_sound, ak_sound, awm_sound, rpk_sound};
const uint16_t blast_sound[] PROGMEM = {
  0x0045,0x7849,0x784D,0xA28,0x0000};
int crate_x, crate_y;
const uint16_t power_up[] PROGMEM = {
  0x0005,0x140,0x150,0x15C,0x170,0x180,0x16C,0x154,0x160,0x174,0x184,0x14C,0x15C,0x168,0x17C,0x18C,0x0000
};

#define RANKMAX 5
int score = 0;
int kills = 0;
int highscore[RANKMAX];
#define NAMELENGTH 10
char name[RANKMAX][NAMELENGTH+1];

void setup() {

  Serial.print(byteWidth);
  game.begin();
  game.display.setFont(font5x7);
  game.titleScreen(logo);
//  for (int i = 0; i < 512; i++) {
//    EEPROM.write(i, 0);
//    delay(5); //minimum delay 5 second
//  }

  loadHighscore();
  initGame();
}


void loop() {
  pause();
  game.titleScreen(logo);  
}


boolean screenCoord(int absoluteX, int absoluteY, int &x, int &y){
  x = absoluteX - cameraX + 8;
  x = (x >= 0) ? x%(WORLD_W*8) : WORLD_W*8 + x%(WORLD_W*8);
  x -= 8;
  y = absoluteY - cameraY + 8;
  y = (y >= 0) ? y%(WORLD_H*8) : WORLD_H*8 + y%(WORLD_H*8);
  y -= 8;
  if((x > LCDWIDTH) || (y > LCDHEIGHT))
    return false;
  return true;
}

#define wrap(i,imax) ((imax+i)%(imax))

//MOVE XYDS
void moveXYDS(int &x, int &y, byte &dir, char speed){
  switch(dir){ 
  case 0: 
    y -= speed;
    break;
  case 1: 
    x -= speed;
    break;
  case 2: 
    y += speed;
    break;
  case 3: 
    x += speed;
    break;
  }
  x = wrap(x, WORLD_W*8);
  y = wrap(y, WORLD_H*8);
}

//DISTANCE BETWEEN
byte distanceBetween(int pos1, int pos2, int worldSize){
  byte dist = abs(pos1 - pos2);
  dist = (dist < worldSize/2) ? dist : worldSize - dist;
  return dist; 
}





