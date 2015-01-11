; GEN02_MidiMonitor.csd
; Written by Iain McCurdy, 2014

; display of MIDI notes, key velocity and controllers (MIDI channel 1)


<Cabbage>
form caption("GEN02 Midi Monitor"), size(610, 425), colour( 50, 50, 50),pluginID("MiMo"), guirefresh(20)

image     bounds(  0,  0, 610,110), colour(75, 85, 90, 100), line(2), shape(sharp)
table     bounds(  5,  5, 600, 74), channel("notes"),  tableNumbers(1), tablecolour("yellow"), identchannel("table1"), drawmode(4), amprange(0,127)
label     bounds(  5, 83, 600, 12), text("Notes and Velocities"), fontcolour("white")
rslider   bounds(  5, 80, 60, 25), text("Last Note"),     fontcolour("white"), textbox(1), channel("LastNote"), range(0,127,0,1,1)
rslider   bounds( 64, 80, 80, 25), text("Last Velocity"), fontcolour("white"), textbox(1), channel("LastVel"),  range(0,127,0,1,1)

image    bounds(  0,115,610,110), colour(75, 85, 90, 100), line(2), shape(sharp)
table    bounds(  5,120,600, 74), channel("controllers"),  tableNumbers(2), tablecolour("lime"), identchannel("table2"), drawmode(4), amprange(0,127)
label    bounds(  5,198,600, 12), text("Controllers by Channel"), fontcolour("white")
rslider  bounds(  5,195,120, 25), text("Last Controller Value"), fontcolour("white"), textbox(1), channel("LastCtrl"), range(0,127,0,1,1)
label    bounds(413,201, 90, 12),  text("MIDI Channel:"), fontcolour("white")
combobox bounds(500,200, 33, 15), channel("CtrlChannel"), value(1), text("1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16")

image    bounds(  0,230, 610,110), colour(75, 85, 90, 100), line(2), shape(sharp)
table    bounds(  5,235,600, 74), channel("controllers2"),  tableNumbers(3), tablecolour("LightBlue"), identchannel("table3"), drawmode(4), amprange(0,127)
label    bounds(  5,313,600, 12), text("Controllers by Number"), fontcolour("white")
hslider  bounds(380,308,220, 23), fontcolour("white"), textbox(1), channel("CtrlNumber"), range(0,127,1,1,1)
label    bounds(380,324,220, 11), text("Controller Number"), fontcolour("white")

keyboard bounds(  0,345,610, 80)

</Cabbage>                                                   
                    
<CsoundSynthesizer>                                                                                                 

<CsOptions>                                                     
-dm0 -n -+rtmidi=null -M0                                        
</CsOptions>
                                  
<CsInstruments>

sr 		= 	44100	;SAMPLE RATE
ksmps 		= 	128	;NUMBER OF AUDIO SAMPLES IN EACH CONTROL CYCLE
nchnls 		= 	2	;NUMBER OF CHANNELS (1=MONO)
0dbfs		=	1	;MAXIMUM AMPLITUDE     

massign	0,1

gisine		ftgen	0,0,4096,10,1

; tables for notes active with velocities
ginotes		ftgen	1,0, 128,-2, 0
gicontrollers	ftgen	2,0, 128,-2, 0
gicontrollers2	ftgen	3,0, 16,-2, 0

instr	1
 inum	notnum 
 	chnset	inum,"LastNote"
 
 ivel	veloc	0,127
 	chnset	ivel,"LastVel"
 	tableiw	ivel,inum,ginotes
 krel	release
 if krel==1 then
 	tablew	1-krel,inum,ginotes
 endif
 
 xtratim	0.1
 krel	release
 if krel==1 then
  chnset	1-krel,"LastVel"
  chnset	1-krel,"LastNote"
 endif
  
 	chnset	"tablenumber(1)", "table1"	; update table display	
endin                                                                                                                     

opcode CtrlRead,0,iiii
 ichan,icount,ilast,itab	xin
 kval	ctrl7	ichan,icount,0,127
 if changed(kval)==1 then
  chnset	kval,"LastCtrl"
 endif
 	tablew	kval,icount,itab
 if icount<ilast then
  CtrlRead	ichan,icount+1,ilast,itab
 endif
endop

opcode CtrlReadByNumber,0,iii
 ictrl,icount,itab	xin
 kval	ctrl7	icount,ictrl,0,127
 ;if changed(kval)==1 then
 ; chnset	kval,"LastCtrl"
 ;endif
 	tablew	kval,icount-1,itab
 if icount<16 then
  CtrlReadByNumber	ictrl,icount+1,itab
 endif
endop

instr	2
 kCtrlChannel	chnget	"CtrlChannel"
 kCtrlChannel	init	1
 
 if changed(kCtrlChannel)==1 then
  reinit RESTART
 endif
 RESTART:
 gicontrollers	ftgen	2,0, 128,-2, 0 
 CtrlRead	i(kCtrlChannel),0,127,gicontrollers
 rireturn

 kCtrlNumber	chnget	"CtrlNumber"
 if changed(kCtrlNumber)==1 then
  reinit RESTART2
 endif
 RESTART2:
 gicontrollers2	ftgen	3,0, 16,-2, 0 
 CtrlReadByNumber	i(kCtrlNumber),1,gicontrollers2
 rireturn

 if metro(20)==1 then
 	chnset	"tablenumber(2)", "table2"	; update table display	
 	chnset	"tablenumber(3)", "table3"	; update table display	
 endif

endin

</CsInstruments>

<CsScore>
i 2 0 [3600*24*7]		; instrument to update function tables and table displays
</CsScore>                            

</CsoundSynthesizer>