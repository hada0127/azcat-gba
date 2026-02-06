fscommand2("SetSoftKeys","pUp","pDown");
fscommand2("SetQuality","low");
stopAllSounds();
set("/:player_life","3");
setProperty("/player", _X, "93");
set("/:player_accel","0");
tellTarget("/player")
{
   gotoAndStop("stop_left");
   play();
}
tellTarget("/bg_matrix")
{
   gotoAndStop("no");
   play();
}
set("/:score","0");
i = "0";
while(i < "33")
{
   set("/cat" add i add ":_y","250");
   i += "1";
}
set("/:bomb_have","0");
set("/:bomb_use","0");
setProperty("/item_move", _Y, "250");
tellTarget("/itemchat")
{
   gotoAndStop("no");
   play();
}
tellTarget("/status_bomb")
{
   gotoAndStop("no");
   play();
}
tellTarget("/bomb")
{
   gotoAndStop("no");
   play();
}
