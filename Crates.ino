void spawnCrate(){
  boolean okay = false;
  while (okay == false){ 
    crate_x = random(0, WORLD_W) * 8;
    crate_y = random(0, WORLD_H) * 8;
    okay = true;
    if(collideWorld(crate_x, crate_y, 8, 8)){
      okay = false;
    }   
    int x, y;
    if(screenCoord(crate_x, crate_y, x, y)){
      okay = false;
    }
  }
}


void collideCrate(){
  if(game.collideRectRect(crate_x+2, crate_y+2, 4, 4, playerX, playerY, playerW, playerH)){
    if (score <5){
      game.popup(F("Earn $5 first"), 15);
      return;
    }
    if(currentWeapon<(NUMWEAPONS-1)){
      game.popup(F(" Upgraded!"), 30);
      game.sound.playPattern(power_up,0);
    }
    else{
      game.popup(F(" Refilled!"), 30);
    }
    score -= 5;
    spawnCrate();
    currentWeapon = min(NUMWEAPONS-1, currentWeapon+1); 
    ammo = weapon_ammo[currentWeapon];
    playerLife = min(playerLife+1, playerLifeMax);
  }
}

void drawCrate(){
  int x, y;
  if(screenCoord(crate_x, crate_y, x, y)){
    //vẽ thùng chứa vũ khí
    game.display.drawBitmap(x, y, crate);
  }
}
