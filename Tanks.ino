boolean spawnTank(byte thisTank){
  boolean okay = false;
  byte timout = 0;
  Tanks_size[thisTank] = Tank_size;
  Tanks_life[thisTank] = Tank_maxLife;
  if(!boss_nextSpawn){ //tạo một bossTank
    boss_freq = max(boss_freq - BOSSRATE, 1);
    boss_nextSpawn = boss_freq;
    Tanks_size[thisTank] = boss_size;
    Tanks_life[thisTank] = boss_maxLife;
  }
  while(okay == false){ 
    
    Tanks_x[thisTank] = random(0, WORLD_W*2) * 4;
    Tanks_y[thisTank] = random(0, WORLD_H*2) * 4;
    okay = true;
    
    if(checkTankCollisions(thisTank)){
      okay = false;
      continue;
    }
    
    if(wrap(Tanks_x[thisTank] - cameraX, WORLD_W*8) < LCDWIDTH){
      okay = false;
      continue;
    }
    if(wrap(Tanks_y[thisTank] - cameraY, WORLD_H*8) < LCDHEIGHT){
      okay = false;
      continue;
    }
  }
  
  Tanks_dir[thisTank] = random() % 4;
  return true;
}

boolean spawnTanks(){
  for(byte thisTank=0; thisTank<activeTanks; thisTank++){ //put Tanks far away
    Tanks_x[thisTank] = 9999;
    Tanks_y[thisTank] = 9999;
  }
  for(byte thisTank=0; thisTank<activeTanks; thisTank++){
    if(!spawnTank(thisTank))
      return false; 
  }
  return true;
}


void moveTanks(){
  for(byte thisTank=0; thisTank<activeTanks; thisTank++){    
    int x = wrap(Tanks_x[thisTank] - cameraX, WORLD_W*8);
    int y = wrap(Tanks_y[thisTank] - cameraY, WORLD_H*8);    
    if( (distanceBetween(Tanks_x[thisTank], playerX, WORLD_W*8) < (LCDWIDTH+32)) &&  (distanceBetween(Tanks_y[thisTank], playerY, WORLD_H*8) < (LCDHEIGHT+32))){
      moveXYDS(Tanks_x[thisTank], Tanks_y[thisTank], Tanks_dir[thisTank], 1);
      if(checkTankCollisions(thisTank)){
        moveXYDS(Tanks_x[thisTank], Tanks_y[thisTank], Tanks_dir[thisTank], -1);
        Tanks_dir[thisTank] = random()%4;
        continue;
      }
      if(random(0,32)==0){ 
        Tanks_dir[thisTank] = random()%4;
        continue;
      }
      if(random(0,16)==0){
        if(random(0,2)){
          
          if((LCDWIDTH/2 - x) > 0){
            Tanks_dir[thisTank] = 3;
          }
          else{ 
            Tanks_dir[thisTank] = 1;
          }
        }
        else {
         
          if((LCDHEIGHT/2 - y) > 0){ 
            Tanks_dir[thisTank] = 2;
          }
          else{ 
            Tanks_dir[thisTank] = 0;
          }
        }
      }
    }
  }
}


boolean checkTankCollisions(byte thisTank){
 
  if(collideWorld(Tanks_x[thisTank], Tanks_y[thisTank], Tanks_size[thisTank], Tanks_size[thisTank]))
    return true;
  if(collideOtherTanks(thisTank))
    return true;
  return false;
}



boolean collideOtherTanks(byte thisTank){
  for(byte otherTank=0; otherTank<activeTanks; otherTank++){
    if(thisTank == otherTank)
      continue;
    if(game.collideRectRect(Tanks_x[thisTank], Tanks_y[thisTank], Tanks_size[thisTank], Tanks_size[thisTank],
    Tanks_x[otherTank], Tanks_y[otherTank], Tanks_size[otherTank], Tanks_size[otherTank])){
      return true;
    }
  }
  return false;
}


void drawTanks(){
  for(byte thisTank=0; thisTank<activeTanks; thisTank++){
    int x, y;
    if(screenCoord(Tanks_x[thisTank], Tanks_y[thisTank], x, y)){
      if(Tanks_size[thisTank] != boss_size)
        game.display.drawBitmap(x-2, y-2, Tank, Tanks_dir[thisTank], NOFLIP);
      else
        game.display.drawBitmap(x-1, y-1, bossTank, Tanks_dir[thisTank], NOFLIP);
    }
  }
}


void damageTank(byte thisTank, byte thisBullet){
  Tanks_life[thisTank] -= weapon_damage[bullets_weapon[thisBullet]];
  
  byte recoil = weapon_ennemyRecoil[bullets_weapon[thisBullet]];
  if(Tanks_size[thisTank] == boss_size)
    recoil /= 4;
  moveXYDS(Tanks_x[thisTank], Tanks_y[thisTank], bullets_dir[thisBullet], recoil);
  if(checkTankCollisions(thisTank))
    moveXYDS(Tanks_x[thisTank], Tanks_y[thisTank], bullets_dir[thisBullet], -recoil);
  Tanks_dir[thisTank] = (bullets_dir[thisBullet] + 2) % 4;

  if(Tanks_life[thisTank] <= 0){
    score++; 
    kills++;
    if(kills>1){
      switch(kills){
        
      case 2:
        game.popup(F(" Double Kill"), 30);
        break;
      case 3:
        game.popup(F(" Triple Kill"), 30);
        break;
      case 4:
        game.popup(F(" Quadra Kill"), 30);
        break;
      case 5:
        game.popup(F(" Penta Kill"), 30);
        break;
      case 6:
        game.popup(F(" Legendary"), 30);
        break;
      default:
        game.popup(F("  Rampage"), 30);
        break;
      }
    }
    boss_nextSpawn--;
    
    if(bullets_weapon[thisBullet]!=3){
      game.sound.playPattern(Tank_death_sound,0);
    }
   
    if(Tanks_size[thisTank] == boss_size)
      score += 4;
    //tạo vết
    setSplash(Tanks_x[thisTank], Tanks_y[thisTank]);
    int x, y;
    if(screenCoord(Tanks_x[thisTank], Tanks_y[thisTank], x, y)){
      game.display.fillRect(x-1, y-1, Tanks_size[thisTank]+1, Tanks_size[thisTank]+1);
    }
   
    spawnTank(thisTank);
    if(activeTanks < NUMTankS){
      if(activeTanks < (kills/TankSRATE)+INITNUMTankS){ 
        activeTanks++; 
        spawnTank(activeTanks-1);
      }
    }
  }
}



