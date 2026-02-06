on(keyPress "<Left>"){
   set("/:v_player_direction","left");
   tellTarget("/player")
   {
      v_accel = "0";
      gotoAndStop("left");
      play();
   }
}
