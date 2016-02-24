CPPFLAGS =	/c -I ..\..\zrlib\src -I..\..\Detours\include  -GX

OBJS = gf_props.obj gf_util.obj

OUTDIR = ..\build
LINK32 = link.exe
LIBS = ..\..\zrlib\build\zrlib.lib \
	..\..\Detours\lib\detours.lib \
	..\..\Detours\lib\detoured.lib \
	user32.lib \
	Shell32.lib \
	Gdi32.lib
#kernel32.lib  user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib 

TARGET = gf_startgame dt_ws2_32  gf_tcpadaptor 

dt_ws2_32:	$@.obj $(OBJS)
	$(LINK32) $(LIBS) ws2_32.lib $(LINK32_FLAGS)  /dll /out:$(OUTDIR)\gf.dll $(OBJS) $@.obj 

gf_chk_mem_dll:	$@.obj $(OBJS)
	$(LINK32) $(LIBS) ws2_32.lib $(LINK32_FLAGS)  /dll /out:$(OUTDIR)\gf_chk_mem.dll $(OBJS) $@.obj 

gf_props_sample: $@.obj  $(OBJS)
	$(CPP) $(LIBS) ws2_32.lib $(OBJS) $@.obj -o $(OUTDIR)\$@.exe

gf_startgame: $@.obj ac_wc3.obj gf_udpspd.obj gf_hard.obj $(OBJS)
	$(CPP) $(LIBS)  Winmm.lib ws2_32.lib Advapi32.lib winsdk\Psapi.lib $(OBJS) $@.obj  ac_wc3.obj gf_udpspd.obj gf_hard.obj -o $(OUTDIR)\$@.exe

gf_chk_mem: $@.obj $(OBJS)
	$(CPP) $(LIBS)  Winmm.lib ws2_32.lib Advapi32.lib winsdk\Psapi.lib $(OBJS) $@.obj  -o $(OUTDIR)\$@.exe

#gf_updater: $@.obj $(OBJS)
#	$(LINK32) /subsystem:windows $(LIBS)  ws2_32.lib $(OBJS) $@.obj /out:$(OUTDIR)\$@.exe

gf_tcpadaptor: $@.obj $(OBJS)
	$(LINK32) /subsystem:windows $(LIBS)  ws2_32.lib $(OBJS) $@.obj /out:$(OUTDIR)\$@.exe

ac_all_test: $@.obj ac_wc3.obj $(OBJS)
	$(LINK32)  $(LIBS) Advapi32.lib ws2_32.lib winsdk\Psapi.lib $(OBJS) $@.obj ac_wc3.obj /out:$(OUTDIR)\$@.exe

test_udp: $@.obj  $(OBJS)
	$(LINK32)  $(LIBS) Winmm.lib ws2_32.lib $(OBJS) $@.obj  /out:$(OUTDIR)\$@.exe

test_tcp: $@.obj  $(OBJS)
	$(LINK32)  $(LIBS) Winmm.lib ws2_32.lib $(OBJS) $@.obj  /out:$(OUTDIR)\$@.exe


all:	$(TARGET) 

clean:
	del $(OBJS) *.obj $(OUTDIR)\dt_ws2_32.dll $(OUTDIR)\gf_startgame.exe $(OUTDIR)\gf_tcpadaptor.exe *.lib $(OUTDIR)\gf.dll
