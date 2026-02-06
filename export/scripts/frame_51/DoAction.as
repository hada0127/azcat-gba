stopAllSounds();
fscommand2("SetQuality","high");
if(eval("/:hiscore") < eval("/:score"))
{
   status = fscommand2("SetPersistentData","hiscore=" add eval("/:score"));
}
if("3000" < eval("/:score"))
{
   tellTarget("/grade")
   {
      gotoAndStop("d100");
      play();
   }
}
else if("1000" < eval("/:score"))
{
   tellTarget("/grade")
   {
      gotoAndStop("d10");
      play();
   }
}
else if("900" < eval("/:score"))
{
   tellTarget("/grade")
   {
      gotoAndStop("d9");
      play();
   }
}
else if("800" < eval("/:score"))
{
   tellTarget("/grade")
   {
      gotoAndStop("d8");
      play();
   }
}
else if("700" < eval("/:score"))
{
   tellTarget("/grade")
   {
      gotoAndStop("d7");
      play();
   }
}
else if("600" < eval("/:score"))
{
   tellTarget("/grade")
   {
      gotoAndStop("d6");
      play();
   }
}
else if("500" < eval("/:score"))
{
   tellTarget("/grade")
   {
      gotoAndStop("d5");
      play();
   }
}
else if("400" < eval("/:score"))
{
   tellTarget("/grade")
   {
      gotoAndStop("d4");
      play();
   }
}
else if("300" < eval("/:score"))
{
   tellTarget("/grade")
   {
      gotoAndStop("d3");
      play();
   }
}
else if("200" < eval("/:score"))
{
   tellTarget("/grade")
   {
      gotoAndStop("d2");
      play();
   }
}
else if("100" < eval("/:score"))
{
   tellTarget("/grade")
   {
      gotoAndStop("d1");
      play();
   }
}
else if("90" < eval("/:score"))
{
   tellTarget("/grade")
   {
      gotoAndStop("g1");
      play();
   }
}
else if("80" < eval("/:score"))
{
   tellTarget("/grade")
   {
      gotoAndStop("g2");
      play();
   }
}
else if("70" < eval("/:score"))
{
   tellTarget("/grade")
   {
      gotoAndStop("g3");
      play();
   }
}
else if("60" < eval("/:score"))
{
   tellTarget("/grade")
   {
      gotoAndStop("g4");
      play();
   }
}
else if("50" < eval("/:score"))
{
   tellTarget("/grade")
   {
      gotoAndStop("g5");
      play();
   }
}
else if("40" < eval("/:score"))
{
   tellTarget("/grade")
   {
      gotoAndStop("g6");
      play();
   }
}
else if("30" < eval("/:score"))
{
   tellTarget("/grade")
   {
      gotoAndStop("g7");
      play();
   }
}
else if("20" < eval("/:score"))
{
   tellTarget("/grade")
   {
      gotoAndStop("g8");
      play();
   }
}
else if("10" < eval("/:score"))
{
   tellTarget("/grade")
   {
      gotoAndStop("g9");
      play();
   }
}
else
{
   tellTarget("/grade")
   {
      gotoAndStop("g10");
      play();
   }
}
