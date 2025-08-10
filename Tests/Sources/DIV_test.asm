\\
\\ 16-bit division test
\\
\\  Divides the two-byte number NUM1 by the two-byte number NUM2, leaving the quotient in Q and the remainder in REM
\\

ORG $f000

.start

.div
        CLD
        LDA #0      \\Initialize REM to 0
        STA REM
        STA REM+1
        LDA NUM1    \\ Store NUM1 in Q
        STA Q
        LDA NUM1+1
        STA Q+1

        LDX #16     \\There are 16 bits in Q
.L1     ASL Q       \\Shift hi bit of Q into REM
        ROL Q+1     \\(vacating the lo bit, which will be used for the quotient)
        ROL REM
        ROL REM+1
        LDA REM
        SEC         \\Trial subtraction
        SBC NUM2
        TAY
        LDA REM+1
        SBC NUM2+1
        BCC L2      \\Did subtraction succeed?
        STA REM+1   \\If yes, save it
        STY REM
        INC Q       \\and record a 1 in the quotient
.L2     DEX
        BNE L1
        JMP stop


ORG $f500
.stop
			JMP stop

ORG $fa00

.NUM1   \\ 1234 <=> 0x04d2
EQUB    $d2,$04

.NUM2   \\ 10 <=> 0x000a
EQUB    $a,$00




ORG $fffc
EQUW start

.end

ORG $4000
.Q   \\ Should become 123 <=> 0x007b
EQUB    0,0

.REM    \\ Should become 4 <=> 0x0004
EQUB    0,0

SAVE "DIV_test.rom", start, end