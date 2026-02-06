if("240" < _Y)
{
   _X = random("205") + "1";
   _Y = "-32" - random("160") + "1";
   v_accel = "0";
   v_sit = "0";
}
else if("-200" < _Y)
{
   §§push(move_flag eq "yes");
   if(move_flag eq "yes")
   {
      §§pop();
      §§push(eval("/:bomb_use") == "0");
   }
   if(§§pop())
   {
      _Y = _Y + "1" + v_accel + eval("/:v_cat_accel");
      v_accel += "0.5";
      if("200" < _Y)
      {
         if(v_sit == "0")
         {
            set("/:score",eval("/:score") + "1");
            _Y = "200";
            gotoAndStop("sit");
            play();
         }
      }
      else
      {
         §§push("170" < _Y);
         if("170" < _Y)
         {
            §§pop();
            §§push(_Y < "190");
         }
         if(§§pop())
         {
            §§push(getProperty("/player", _X) + getProperty("/player/hit", _X) < _X + _width);
            if(getProperty("/player", _X) + getProperty("/player/hit", _X) < _X + _width)
            {
               §§pop();
               §§push(_X < getProperty("/player", _X) + getProperty("/player/hit", _X) + getProperty("/player/hit", _width));
            }
            §§push(getProperty("/player", _X) + getProperty("/player/hit", _X) < _X + _width);
            if(getProperty("/player", _X) + getProperty("/player/hit", _X) < _X + _width)
            {
               §§pop();
               §§push(_X < getProperty("/player", _X) + getProperty("/player/hit", _X) + getProperty("/player/hit", _width));
            }
            if(!§§pop())
            {
               §§pop();
               §§push(getProperty("/player", _X) + getProperty("/player/hit", _X) < _X);
               if(getProperty("/player", _X) + getProperty("/player/hit", _X) < _X)
               {
                  §§pop();
                  §§push(_X < getProperty("/player", _X) + getProperty("/player/hit", _X) + getProperty("/player/hit", _width));
               }
            }
            if(§§pop())
            {
               gotoAndStop("hit");
               play();
            }
         }
      }
   }
   else if(eval("/:bomb_use") == "1")
   {
      _Y = "250";
   }
}
