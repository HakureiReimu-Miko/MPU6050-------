CD Objects
srec_cat.exe -Disable_Sequence_Warning T5L51.hex -Intel -o T5L51.bin -Binary 
copy "T5L51.bin" "Z:\��̬����\�����\DWIN_SET"
if exist "Z:\U��\DWIN_SET" (
copy "T5L51.bin" "Z:\U��\DWIN_SET"
)
:copy "T5L51.bin" "Z:\���峬\���Project\�����\DEMO\DWIN_SET"
:copy "T5L51.hex" "Z:\���峬\���Project\�����\DEMO\DWIN_SET"
