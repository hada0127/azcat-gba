if("240" < _Y)
{
   _X = random("205") + "1";
   set("/:item_type",random("4") + "1");
   tellTarget("/item_move/item")
   {
      if(eval("/:item_type") == "1")
      {
         gotoAndStop("i1");
         play();
      }
      else if(eval("/:item_type") == "2")
      {
         gotoAndStop("i2");
         play();
      }
      else if(eval("/:item_type") == "3")
      {
         gotoAndStop("i3");
         play();
      }
      else if(eval("/:item_type") == "4")
      {
         gotoAndStop("i4");
         play();
      }
   }
   v_accel = "0";
   v_sit = "0";
   v_sit_cnt = "0";
   if("149" < eval("/:item_cnt"))
   {
      _Y = "-40";
   }
}
else if("-50" < _Y)
{
   if(eval("/:bomb_use") == "0")
   {
      set("/:item_cnt","0");
      _Y = _Y + "1" + v_accel;
      v_accel += "0.5";
      if(v_sit_cnt >= "45")
      {
         _Y = "250";
         v_sit = "1";
      }
      if(_Y >= "207")
      {
         if(v_sit == "0")
         {
            _Y = "207";
            v_sit_cnt += "1";
         }
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
