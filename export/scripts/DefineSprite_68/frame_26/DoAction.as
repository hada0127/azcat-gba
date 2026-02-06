if(eval("/:bomb_use") == "0")
{
   setProperty("/player", _X, getProperty("/player", _X) - "5" - v_accel - eval("/:player_accel"));
   v_accel += "0.2";
   if(getProperty("/player", _X) < "0")
   {
      setProperty("/player", _X, "0");
   }
}
