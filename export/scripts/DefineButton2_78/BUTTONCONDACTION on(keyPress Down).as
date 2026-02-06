on(keyPress "<Down>"){
   tellTarget("/player")
   {
      if(eval("/:v_player_direction") eq "left")
      {
         gotoAndStop("stop_left");
         play();
      }
      else
      {
         gotoAndStop("stop_right");
         play();
      }
   }
}
