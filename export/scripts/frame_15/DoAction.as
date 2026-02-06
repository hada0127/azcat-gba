set("/:hiscore","0");
status = fscommand2("GetPersistentData","/:hiscore");
set("/:hiscore_view",eval("/:hiscore"));
fscommand2("SetQuality","high");
