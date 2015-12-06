void shoot(){
  if(ammo){
    if(nextShot == 0){
      for(byte thisBullet = 0; thisBullet < NUMBULLETS; thisBullet++){
        if(!bullets_active[thisBullet]){ //look for the first inactive bullet
          bullets_active[thisBullet] = true; //set it to active as it's fired
          bullets_weapon[thisBullet] = currentWeapon;

          nextShot = weapon_rate[currentWeapon];
          ammo--;
          
          char spreadMax = weapon_spread[currentWeapon];
          char spreadMin = (weapon_size[currentWeapon]%2==0) ? -spreadMax : -spreadMax-1;
          bullets_x[thisBullet] = playerX + playerW/2 + random(spreadMin,spreadMax+1) - weapon_size[currentWeapon]/2;
          bullets_y[thisBullet] = playerY + playerH/2 + random(spreadMin,spreadMax+1) - weapon_size[currentWeapon]/2;

          bullets_dir[thisBullet] = playerDir;
          blast_bullet = thisBullet;
          if(((currentWeapon != 1)&&(currentWeapon!=4))||(game.frameCount%2))
          {
            
          }
          else{
            game.sound.playPattern((uint16_t*)pgm_read_word(&(weapons_sounds[currentWeapon])), 0);
          }
          if(currentWeapon == 1){//
            if(random()%2)
              game.sound.playPattern(b40_sound, 0);
          }
         
          byte recoil = weapon_playerRecoil[currentWeapon];
          moveXYDS(playerX, playerY, playerDir, -recoil);
          for(byte i = 0; i<recoil; i++){
            if(collideWorld(playerX, playerY, playerW, playerH))
              moveXYDS(playerX, playerY, playerDir, 1);
            else
              break;
          }
          if(currentWeapon == 4){
            shake_magnitude = 1;
            shake_timeLeft = 2;
          }
          break;
        }
      }
    }
  }
  else{
    currentWeapon = max(0, currentWeapon-1);
    ammo = weapon_ammo[currentWeapon];
    nextShot = 20;
    game.popup(F("Out of ammo!"), 30);
  }
}


void moveBullets(){
  for(byte thisBullet = 0; thisBullet < NUMBULLETS; thisBullet++){
    if(bullets_active[thisBullet]){
      byte s = weapon_size[bullets_weapon[thisBullet]];
      moveXYDS(bullets_x[thisBullet], bullets_y[thisBullet], bullets_dir[thisBullet], weapon_speed[bullets_weapon[thisBullet]]);
      if(collideWorld(bullets_x[thisBullet], bullets_y[thisBullet], s, s)){
        bullets_active[thisBullet] = false;
        if(bullets_weapon[thisBullet] == 3){
          blast_x = bullets_x[thisBullet];
          blast_y = bullets_y[thisBullet];
          blast_lifespan = 8;
          game.sound.playPattern(blast_sound, 0);
        }
        else{
        }
        continue;
      }
      for(byte thisTank=0; thisTank<activeTanks; thisTank++){
        if(game.collideRectRect(bullets_x[thisBullet], bullets_y[thisBullet], s, s,
        Tanks_x[thisTank], Tanks_y[thisTank], Tanks_size[thisTank], Tanks_size[thisTank])){
          if(bullets_weapon[thisBullet] == 3){
            blast_x = bullets_x[thisBullet];
            blast_y = bullets_y[thisBullet];
            blast_lifespan = 8;
            game.sound.playPattern(blast_sound, 0);
          } 
          else {
            damageTank(thisTank, thisBullet);
          }
          bullets_active[thisBullet] = false;
          break;
        }
      }

    }
  }
}


void explode(){
  if(blast_lifespan){
    blast_lifespan--;
    shake_magnitude = 4;
    shake_timeLeft = 2;    
    byte s = 10 + random (0,6);
    int x = blast_x + random(-4,4) -s/2;
    int y = blast_y + random(-4,4) -s/2;
    for(byte thisTank=0; thisTank<activeTanks; thisTank++){
      if(game.collideRectRect(Tanks_x[thisTank], Tanks_y[thisTank], Tanks_size[thisTank], Tanks_size[thisTank],
      x,y,s,s))
        damageTank(thisTank,blast_bullet);
    }
    
    int x_screen, y_screen;
    if(screenCoord(x, y, x_screen, y_screen)){
      game.display.setColor(INVERT);
      game.display.fillRect(x_screen, y_screen, s, s);
      game.display.setColor(BLACK);
    }
  }
}

void drawBullets(){
  for(byte thisBullet = 0; thisBullet < NUMBULLETS; thisBullet++){
    if(bullets_active[thisBullet]){
      int x, y;
      if(screenCoord(bullets_x[thisBullet], bullets_y[thisBullet], x, y)){
        byte s = weapon_size[bullets_weapon[thisBullet]];
        if(s==1)
          game.display.drawPixel(x, y);
        else
          game.display.fillRect(x, y, s, s);
      }
    }
  }
}

void drawAmmoOverlay(){
  if(ammo){
    game.display.cursorX = 0;
    game.display.cursorY = LCDHEIGHT-game.display.fontHeight;
    game.display.print((const __FlashStringHelper*)pgm_read_word(weapon_name+currentWeapon));
    if(nextShot>2)
      game.display.fillRect(-2,LCDHEIGHT-2,nextShot,2);
    if(currentWeapon > 0){  
      byte xOffset = 0;
      if (ammo < 100)
        xOffset += game.display.fontWidth;
      if (ammo < 10)
        xOffset += game.display.fontWidth;
      game.display.cursorX = LCDWIDTH-3*game.display.fontWidth+xOffset;
      game.display.cursorY = LCDHEIGHT-game.display.fontHeight;
      game.display.print(ammo);
    } 
    else {
      game.display.cursorX = LCDWIDTH-3*game.display.fontWidth;
      game.display.cursorY = LCDHEIGHT-game.display.fontHeight;
      game.display.print(F(""));
    }
  }
}

void setSplash(int x, int y){
  for(byte thisSplash = 0; thisSplash < NUMSPLASH; thisSplash++){
    if(!splash_active[thisSplash]){ 
      splash_active[thisSplash] = true; 
      splash_x[thisSplash] = x;
      splash_y[thisSplash] = y;
      splash_dir[thisSplash] = random(0,5);
      break;
    }
  }
}

void drawSplashes(){
  for(byte thisSplash = 0; thisSplash < NUMSPLASH; thisSplash++){
    if(splash_active[thisSplash]){
      int x, y;
      if(screenCoord(splash_x[thisSplash], splash_y[thisSplash], x, y)){ 
        game.display.drawBitmap(x-2, y-2, splash, splash_dir[thisSplash], NOFLIP);
      }
      else{ 
        splash_active[thisSplash] = false;
      }
    }
  }
}

