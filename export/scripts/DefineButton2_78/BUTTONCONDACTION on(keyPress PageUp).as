on(keyPress "<PageUp>"){
   if(eval("/:bomb_have") == "1")
   {
      tellTarget("/bomb")
      {
         gotoAndStop("ok");
         play();
      }
      set("/:bomb_have","0");
      set("/:bomb_use","1");
      tellTarget("/status_bomb")
      {
         gotoAndStop("no");
         play();
      }
   }
}
