
#define PAUSEMENULENGTH 4
const char strPlay[] PROGMEM = "Play";
const char strRestart[] PROGMEM = "Restart";
const char strHighScores[] PROGMEM = "High scores";
const char strMainMenu[] PROGMEM = "Main Menu";


const char* const pauseMenu[PAUSEMENULENGTH] PROGMEM = {
  strPlay,
  strRestart,
  strHighScores,
  strMainMenu
};

void pause(){
  while(true){
    if(game.update()){
      
      switch(game.menu(pauseMenu, PAUSEMENULENGTH)){
      case 0: 
        delay(100);
        play();
        break;
      case 1:
        initGame();
        play();
        return;
      case 2: 
        displayHighScores();
        break;
      case 3:
        game.titleScreen(logo);
        break;
      default:
        return;
      }
    }
    
  }
}

void loadHighscore(){
  for(byte thisScore = 0; thisScore < RANKMAX; thisScore++){
    for(byte i=0; i<NAMELENGTH; i++){
      name[thisScore][i] = EEPROM.read(i + thisScore*(NAMELENGTH+2));
    }
    highscore[thisScore] = EEPROM.read(NAMELENGTH + thisScore*(NAMELENGTH+2)) & 0x00FF; //LSB
    highscore[thisScore] += (EEPROM.read(NAMELENGTH+1 + thisScore*(NAMELENGTH+2)) << 8) & 0xFF00; //MSB
    highscore[thisScore] = (highscore[thisScore]==0xFFFF) ? 0 : highscore[thisScore];
  }
} 


void displayHighScores(){
  while(true){
    if(game.update()){
      game.updateSound();
      game.display.cursorX = 9+random(0,2);
      game.display.cursorY = 0+random(0,2);
      game.display.println(F("HIGH SCORES"));
      game.display.textWrap = false;
      game.display.cursorX = 0;
      game.display.cursorY = game.display.fontHeight+1;
      for(byte thisScore=0; thisScore<RANKMAX; thisScore++){
       
        if(highscore[thisScore]==0)
          game.display.print('-');
        else
       
          game.display.print(name[thisScore]);
        game.display.cursorX = LCDWIDTH-3*game.display.fontWidth+1;
        game.display.cursorY = game.display.fontHeight+game.display.fontHeight*thisScore+1;
        game.display.println(highscore[thisScore]);
      }
     
      if(game.buttons.pressed(BTN_A) || game.buttons.pressed(BTN_B) || game.buttons.pressed(BTN_C)){
        game.sound.playOK();
        break;
      }
    }
  }
}



