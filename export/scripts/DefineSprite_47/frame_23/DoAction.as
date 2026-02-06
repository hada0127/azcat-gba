tellTarget("/itemchat")
{
   if(eval("/:item_type") == "1")
   {
      if(eval("/:player_life") < "3")
      {
         set("/:player_life",eval("/:player_life") + "1");
      }
      else
      {
         set("/:score",eval("/:score") + "5");
      }
      gotoAndStop("i1");
      play();
   }
   else if(eval("/:item_type") == "2")
   {
      if(eval("/:bomb_have") == "0")
      {
         set("/:bomb_have",eval("/:bomb_have") + "1");
         tellTarget("/status_bomb")
         {
            gotoAndStop("ok");
            play();
         }
      }
      else
      {
         set("/:score",eval("/:score") + "5");
      }
      gotoAndStop("i2");
      play();
   }
   else if(eval("/:item_type") == "3")
   {
      if("1" < eval("/:player_life"))
      {
         set("/:player_life",eval("/:player_life") - "1");
      }
      else
      {
         set("/:score",eval("/:score") - "5");
      }
      gotoAndStop("i3");
      play();
   }
   else if(eval("/:item_type") == "4")
   {
      if(eval("/:player_accel") < "4")
      {
         set("/:player_accel",eval("/:player_accel") + "1");
      }
      else
      {
         set("/:score",eval("/:score") + "5");
      }
      gotoAndStop("i4");
      play();
   }
}
_Y = "250";
