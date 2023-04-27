CD Objects
srec_cat.exe -Disable_Sequence_Warning T5L51.hex -Intel -o T5L51.bin -Binary 
copy "T5L51.bin" "Z:\组态工程\艾瑞达\DWIN_SET"
if exist "Z:\U盘\DWIN_SET" (
copy "T5L51.bin" "Z:\U盘\DWIN_SET"
)
:copy "T5L51.bin" "Z:\贺彦超\软件Project\艾瑞达\DEMO\DWIN_SET"
:copy "T5L51.hex" "Z:\贺彦超\软件Project\艾瑞达\DEMO\DWIN_SET"
