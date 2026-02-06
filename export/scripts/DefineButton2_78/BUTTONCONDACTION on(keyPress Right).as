on(keyPress "<Right>"){
   set("/:v_player_direction","right");
   tellTarget("/player")
   {
      v_accel = "0";
      gotoAndStop("right");
      play();
   }
}
