if(eval("/:bomb_use") == "0")
{
   setProperty("/player", _X, getProperty("/player", _X) + "5" + v_accel + eval("/:player_accel"));
   v_accel += "0.2";
   if("189" < getProperty("/player", _X))
   {
      setProperty("/player", _X, "189");
   }
}
