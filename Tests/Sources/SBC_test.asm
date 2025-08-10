ORG &f000

.start



.add
	LDX #0
	CLD
.add_loop
	CLC
	LDA add_data,X
	INX
	ADC add_data,X
	INX
	CPX #16
	BCC add_loop

.sub
	LDX #0
.sub_loop
	SEC
	LDA sub_data,X
	INX
	SBC sub_data,X
	INX
	CPX #16
	BCC sub_loop
	JMP stop

ORG $f500

.stop
		JMP stop

ORG $fa00

.add_data
EQUB	$50,$10
EQUB	$50,$50
EQUB	$50,$90
EQUB	$50,$d0
EQUB	$d0,$10
EQUB	$d0,$50
EQUB	$d0,$90
EQUB	$d0,$d0

.sub_data
EQUB	$50,$f0
EQUB	$50,$b0
EQUB	$50,$70
EQUB	$50,$30
EQUB	$d0,$f0
EQUB	$d0,$b0
EQUB	$d0,$70
EQUB	$d0,$30

ORG $fffc
EQUW start

.end

SAVE "SBC_test.rom", start, end