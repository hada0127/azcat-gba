tellTarget("/status")
{
   if(eval("/:player_life") == "3")
   {
      gotoAndStop("life3");
   }
   else if(eval("/:player_life") == "2")
   {
      gotoAndStop("life2");
   }
   else if(eval("/:player_life") == "1")
   {
      gotoAndStop("life1");
   }
   else if(eval("/:player_life") < "1")
   {
      gotoAndStop("dead");
      play();
   }
}
if("1040" < eval("/:score"))
{
   set("/:v_cat_qty","43");
}
else if("1030" < eval("/:score"))
{
   set("/:v_cat_qty","42");
}
else if("1020" < eval("/:score"))
{
   set("/:v_cat_qty","41");
}
else if("1010" < eval("/:score"))
{
   set("/:v_cat_qty","40");
}
else if("1000" < eval("/:score"))
{
   set("/:v_cat_qty","39");
}
else if("990" < eval("/:score"))
{
   set("/:v_cat_accel","17");
}
else if("980" < eval("/:score"))
{
   set("/:v_cat_accel","16");
}
else if("960" < eval("/:score"))
{
   set("/:v_cat_qty","35");
}
else if("940" < eval("/:score"))
{
   set("/:v_cat_accel","14");
}
else if("920" < eval("/:score"))
{
   set("/:v_cat_accel","13");
}
else if("900" < eval("/:score"))
{
   set("/:v_cat_qty","30");
}
else if("850" < eval("/:score"))
{
   set("/:v_cat_qty","28");
}
else if("800" < eval("/:score"))
{
   set("/:v_cat_accel","12");
}
else if("750" < eval("/:score"))
{
   set("/:v_cat_qty","25");
}
else if("700" < eval("/:score"))
{
   set("/:v_cat_accel","11");
}
else if("650" < eval("/:score"))
{
   set("/:v_cat_qty","23");
}
else if("600" < eval("/:score"))
{
   set("/:v_cat_qty","21");
}
else if("550" < eval("/:score"))
{
   set("/:v_cat_qty","19");
}
else if("500" < eval("/:score"))
{
   set("/:v_cat_qty","17");
}
else if("450" < eval("/:score"))
{
   set("/:v_cat_accel","10");
}
else if("220" < eval("/:score"))
{
   set("/:v_cat_qty","14");
}
else if("400" < eval("/:score"))
{
   set("/:v_cat_accel","9");
}
else if("360" < eval("/:score"))
{
   set("/:v_cat_accel","8");
}
else if("220" < eval("/:score"))
{
   set("/:v_cat_qty","12");
}
else if("280" < eval("/:score"))
{
   set("/:v_cat_accel","7");
}
else if("240" < eval("/:score"))
{
   set("/:v_cat_accel","6");
}
else if("220" < eval("/:score"))
{
   set("/:v_cat_qty","10");
}
else if("200" < eval("/:score"))
{
   set("/:v_cat_accel","5");
}
else if("150" < eval("/:score"))
{
   set("/:v_cat_qty","8");
}
else if("100" < eval("/:score"))
{
   set("/:v_cat_accel","4");
}
else if("80" < eval("/:score"))
{
   set("/:v_cat_qty","6");
}
else if("50" < eval("/:score"))
{
   set("/:v_cat_accel","3");
}
else if("30" < eval("/:score"))
{
   set("/:v_cat_qty","5");
}
else if("20" < eval("/:score"))
{
   set("/:v_cat_qty","4");
}
else if("10" < eval("/:score"))
{
   set("/:v_cat_accel","2");
}
else if("5" < eval("/:score"))
{
   set("/:v_cat_qty","3");
}
else if("2" < eval("/:score"))
{
   set("/:v_cat_qty","2");
}
else if("1" < eval("/:score"))
{
   set("/:v_cat_qty","1");
}
else
{
   set("/:v_cat_qty","0");
   set("/:v_cat_accel","1");
}
if("900" < eval("/:score"))
{
   tellTarget("/bg_matrix")
   {
      gotoAndStop("ok");
      play();
   }
}
else if("500" < eval("/:score"))
{
   tellTarget("/bg_matrix")
   {
      gotoAndStop("night");
      play();
   }
}
i = "0";
while(i < "33")
{
   if(eval("/:v_cat_qty") >= i)
   {
      set("/cat" add i add ":move_flag","yes");
   }
   else
   {
      set("/cat" add i add ":move_flag","no");
   }
   i += "1";
}
set("/:item_cnt",eval("/:item_cnt") + "1");
