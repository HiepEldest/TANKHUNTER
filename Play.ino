void initGame(){
  if(score > highscore[RANKMAX-1]){ 
    saveHighscore();
  }
  score = 0;
  kills = 0;
  currentWeapon = 0;
  ammo = 9999;
  nextShot = 0;
  shake_timeLeft = 0;
  playerLife = playerLifeMax;
  boss_freq = BOSSFREQ;
  boss_nextSpawn = boss_freq;
  activeTanks = INITNUMTankS;
  do{
    do{
     
      playerX = random(0, WORLD_W) * 8;
      playerY = random(0, WORLD_H) * 8;
    }
    while(collideWorld(playerX, playerY, playerW, playerH));
    cameraX = playerX - LCDWIDTH/2 + playerW/2;
    cameraY = playerY - LCDHEIGHT/2 + playerW/2;
  } 
  while(!spawnTanks()); 
 
  spawnCrate();
  
  for(byte thisBullet = 0; thisBullet < NUMBULLETS; thisBullet++){
    bullets_active[thisBullet] = false;
  }
 
  for(byte thisSplash = 0; thisSplash < NUMSPLASH; thisSplash++){
    splash_active[thisSplash] = false;
  }
  blast_lifespan = 0; 
}

void play(){
  byte i = 0; 
  while(i < 10){
    if(game.update()){
      game.display.fontSize = 1;
      game.display.cursorX = 10;
      game.display.cursorY = 20;
      game.display.print(F("LET'S GO!"));
      i++;
    }
  }
  
  game.popup(F("\25shoot \26:run"),30);
  while(true){
    if(game.update()){
      
      if(game.buttons.pressed(BTN_C)){
        game.sound.playCancel();
        return;
      }
     
      boolean moved = false;
      if(game.buttons.repeat(BTN_RIGHT, 1)){
        playerDir = 3;
        moved = true;
      } 
      else{
        if(game.buttons.repeat(BTN_LEFT, 1)){
          playerDir = 1;
          moved = true;
        }
      }
      if(game.buttons.repeat(BTN_DOWN, 1)){
        playerDir = 2;
        moved = true;
      }
      else{
        if(game.buttons.repeat(BTN_UP, 1)){
          playerDir = 0;
          moved = true;
        }
      }
    
      if(moved){
        moveXYDS(playerX, playerY, playerDir, playerSpeed);       
        if(collideWorld(playerX, playerY, playerW, playerH))
          moveXYDS(playerX, playerY, playerDir, -playerSpeed);
      }
      cameraX = playerX + playerW/2 - LCDWIDTH/2;
      cameraY = playerY + playerH/2 - LCDHEIGHT/2;
      drawWorld(cameraX, cameraY); 
      int x, y;
      screenCoord(playerX, playerY, x, y); 
      game.display.drawBitmap(x-1, y-1, playerTank, playerDir, NOFLIP);
      byte thisSprite = 0;
      moveTanks();
      drawTanks();     
      if(nextShot)
        nextShot--;
      if(game.buttons.repeat(BTN_A, 1) && !game.buttons.repeat(BTN_B, 1)){
        shoot();
      }
      if (game.buttons.repeat(BTN_B, 1)){
        playerSpeed = 2;
      } 
      else {
        playerSpeed = 1;
      }
      moveBullets();
      drawBullets();
      explode();
      drawSplashes();
      collideCrate();
      drawCrate();
      
      for(byte i=0; i<=playerLifeMax/2; i+=1){
        if((i*2)<=playerLife){
          game.display.drawBitmap(LCDWIDTH-i*9+2, 0, fullHeart);
        }
        else{
          game.display.setColor(WHITE);
          game.display.drawBitmap(LCDWIDTH-i*9+2, 0, fullHeart);
          game.display.setColor(BLACK,WHITE);
          game.display.drawBitmap(LCDWIDTH-i*9+2, 0, emptyHeart);
        }
      }
      if(playerLife%2){
        game.display.drawBitmap(LCDWIDTH-(playerLife/2+1)*9+2, 0, halfHeart);
      }
      drawAmmoOverlay();
      displayScore(); 
      for(byte thisTank=0; thisTank<activeTanks; thisTank++){
       
        if(game.collideRectRect(Tanks_x[thisTank],Tanks_y[thisTank], Tanks_size[thisTank], Tanks_size[thisTank],
        playerX, playerY, playerW, playerH)){
          playerLife--;
          if(Tanks_size[thisTank] == boss_size){ 
            playerLife--;
          }
          game.sound.playPattern(player_damage_sound, 0);
          spawnTank(thisTank);
          if(playerLife < 0){           
            byte timer=0;
            while(1){              
              if(game.update()){                
                drawTanks();
                drawBullets();
                drawSplashes();
                drawCrate();
                drawAmmoOverlay();
                displayScore();
                drawWorld(cameraX, cameraY);
                game.display.drawBitmap(x-1, y-1, playerTank, playerDir, NOFLIP);
                game.display.setColor(WHITE);
                game.display.fillRect(0,0,timer*2,LCDHEIGHT);
                game.display.fillRect(LCDWIDTH-timer*2,0,timer*2,LCDHEIGHT);
                game.display.setColor(BLACK, WHITE);
                game.display.cursorX = 12;
                game.display.cursorY = 1;
               
                game.display.print(F("GAME OVER!"));
                timer++;
                if(timer==((LCDWIDTH/4)+10))
                  break;
              }
            }
            while(1){
              if(game.update()){
               
                if(score > highscore[RANKMAX-1]){ 
                  game.display.cursorX = 2+random(0,2);
                  game.display.cursorY = 0+random(0,2);
                  game.display.print(F("NEW HIGHSCORE"));
                }
               
                else{ 
                  game.display.cursorX = 12;
                  game.display.cursorY = 1;
                  game.display.print(F("GAME OVER!"));
                }
                
                game.display.cursorX = 0;
                game.display.cursorY = 12;
                game.display.print(F("You made $"));
                game.display.print(score);
                game.display.print(F("\nby killing\n"));
                game.display.print(kills);
                game.display.print(F(" tanks."));
                game.display.cursorX = 0;
                game.display.cursorY = 40;
                game.display.print(F("\x15:accept"));
                
                if(game.buttons.pressed(BTN_A)){
                  game.sound.playOK();
                  break;
                }
              }
            }
            
            initGame();
            break;
          }
        }
      }

    }
  } 
}

void shakeScreen(){
  if(shake_timeLeft){
    shake_timeLeft--;
    cameraX += random(-shake_magnitude,shake_magnitude+1);
    cameraY += random(-shake_magnitude,shake_magnitude+1);
  }
}

void displayScore(){
  game.display.cursorX = 0;
  game.display.cursorY = 0;
  game.display.print('$');
  game.display.println(score);
}

void saveHighscore(){  
  game.keyboard(name[RANKMAX-1], NAMELENGTH+1);
  highscore[RANKMAX-1] = score;
  for(byte i=RANKMAX-1; i>0; i--){
    if(highscore[i-1] < highscore[i]){
      char tempName[NAMELENGTH];
      strcpy(tempName, name[i-1]);
      strcpy(name[i-1], name[i]);
      strcpy(name[i], tempName);
      unsigned int tempScore;
      tempScore = highscore[i-1];
      highscore[i-1] = highscore[i];
      highscore[i] = tempScore;
    }
    else{
      break;
    }
  }
  for(byte thisScore = 0; thisScore < RANKMAX; thisScore++){
    for(byte i=0; i<NAMELENGTH; i++){
      EEPROM.write(i + thisScore*(NAMELENGTH+2), name[thisScore][i]);
    }
    EEPROM.write(NAMELENGTH + thisScore*(NAMELENGTH+2), highscore[thisScore] & 0x00FF); //LSB
    EEPROM.write(NAMELENGTH+1 + thisScore*(NAMELENGTH+2), (highscore[thisScore] >> 8) & 0x00FF); //MSB
  }
  displayHighScores();
}




