\\ Verify decimal mode behavior
\\ Written by Bruce Clark.  This code is public domain.
\\
\\ http://www.6502.org/tutorials/decimal_mode.html#B
\\
\\ Returns:
\\   ERROR = 0 if the test passed
\\   ERROR = 1 if the test failed
\\
\\ This routine requires 17 bytes of RAM -- 1 byte each for:
\\   AR, CF, DA, DNVZC, ERROR, HA, HNVZC, N1, N1H, N1L, N2, N2L, NF, VF, and ZF
\\ and 2 bytes for N2H
\\
\\ Variables:
\\   N1 and N2 are the two numbers to be added or subtracted
\\   N1H, N1L, N2H, and N2L are the upper 4 bits and lower 4 bits of N1 and N2
\\   DA and DNVZC are the actual accumulator and flag results in decimal mode
\\   HA and HNVZC are the accumulator and flag results when N1 and N2 are
\\     added or subtracted using binary arithmetic
\\   AR, NF, VF, ZF, and CF are the predicted decimal mode accumulator and
\\     flag results, calculated using binary arithmetic
\\
\\ This program takes approximately 1 minute at 1 MHz (a few seconds more on
\\ a 65C02 than a 6502 or 65816)
\\




ORG $f000


.start


.test		LDY #1		\\ initialize Y (used to loop through carry flag values)
			STY ERROR   \\ store 1 in ERROR until the test passes
			LDA #0		\\ initialize N1 and N2
			STA N1
			STA N2
.loop1		LDA N2		\\ N2L = N2 & $0F
			AND #$0F    \\ 1] see text
			CMP #$0A    \\ Added to only check valid BCD no
			BCS next2
			STA N2L
			LDA N2		\\ N2H = N2 & $F0
			AND #$F0	\\ [2] see text
			CMP #$A0	\\ Added to only check valid BCD no
			BCS next2
			STA N2H
			ORA #$0F	\\ N2H+1 = (N2 & $F0) + $0F
			STA N2H+1
.loop2		LDA N1		\\ N1L = N1 & $0F
			AND #$0F	\\ [3] see text
			CMP #$0A	\\ Added to only check valid BCD no
			BCS next1
			STA N1L
			LDA N1		\\ N1H = N1 & $F0
			AND #$F0	\\ [4] see text
			CMP #$A0	\\ Added to only check valid BCD no
			BCS next1
			STA N1H
			JSR add
			JSR a6502
			JSR compare
			BNE done
			JSR sub
			JSR s6502
			JSR compare
			BNE done
.next1
			INC N1    \\ [5] see text
			BNE loop2 \\ loop through all 256 values of N1
.next2
			INC N2    \\ [6] see text
			BNE loop1 \\ loop through all 256 values of N2
			DEY
			BPL loop1 \\ loop through both values of the carry flag
			LDA #0    \\ test passed, so store 0 in ERROR
			STA ERROR
.done		LDA	N1
			LDA N2
			JMP stop

\\ Calculate the actual decimal mode accumulator and flags, the accumulator
\\ and flag results when N1 is added to N2 using binary arithmetic, the
\\ predicted accumulator result, the predicted carry flag, and the predicted
\\ V flag
\\
.add		SED       \\ decimal mode
			CPY #1    \\ set carry if Y = 1, clear carry if Y = 0
			LDA N1
			ADC N2
			STA DA    \\ actual accumulator result in decimal mode
			PHP
			PLA
			STA DNVZC \\ actual flags result in decimal mode

			CLD       \\ binary mode
			CPY #1    \\ set carry if Y = 1, clear carry if Y = 0
			LDA N1
			ADC N2
			STA HA    \\ accumulator result of N1+N2 using binary arithmetic

			PHP
			PLA
			STA HNVZC \\ flags result of N1+N2 using binary arithmetic
			CPY #1
			LDA N1L
			ADC N2L
			CMP #$0A
			LDX #0
			BCC a1
			INX
			ADC #5    \\ add 6 (carry is set)
			AND #$0F
			SEC
.a1			ORA N1H
\\
\\ if N1L + N2L <  $0A, then add N2 & $F0
\\ if N1L + N2L >= $0A, then add (N2 & $F0) + $0F + 1 (carry is set)
\\
			ADC N2H,X
			PHP
			BCS a2
			CMP #$A0
			BCC a3
.a2			ADC #$5F  \\ add $60 (carry is set)
			SEC
.a3			STA AR    \\ predicted accumulator result
			PHP
			PLA
			STA CF    \\ predicted carry result
			PLA
\\
\\ note that all 8 bits of the P register are stored in VF
\\
			STA VF    \\ predicted V flags
			RTS

\\ Calculate the actual decimal mode accumulator and flags, and the
\\ accumulator and flag results when N2 is subtracted from N1 using binary
\\ arithmetic
\\
.sub		SED       \\ decimal mode
			CPY #1    \\ set carry if Y = 1, clear carry if Y = 0
			LDA N1
			SBC N2
			STA DA    \\ actual accumulator result in decimal mode
			PHP
			PLA
			STA DNVZC \\ actual flags result in decimal mode
			 
			CLD       \\ binary mode
			CPY #1    \\ set carry if Y = 1, clear carry if Y = 0
			LDA N1
			SBC N2
			STA HA    \\ accumulator result of N1-N2 using binary arithmetic

			PHP
			PLA
			STA HNVZC \\ flags result of N1-N2 using binary arithmetic
			RTS

\\ Calculate the predicted SBC accumulator result for the 6502 and 65816

\\
.sub1		CPY #1    \\ set carry if Y = 1, clear carry if Y = 0
			LDA N1L
			SBC N2L
			LDX #0
			BCS s11
			INX
			SBC #5    \\ subtract 6 (carry is clear)
			AND #$0F
			CLC
.s11		ORA N1H
\\
\\ if N1L - N2L >= 0, then subtract N2 & $F0
\\ if N1L - N2L <  0, then subtract (N2 & $F0) + $0F + 1 (carry is clear)
\\
			SBC N2H,X
			BCS s12
			SBC #$5F  \\ subtract $60 (carry is clear)
.s12		STA AR
			RTS

\\ Compare accumulator actual results to predicted results
\\
\\ Return:
\\   Z flag = 1 (BEQ branch) if same
\\   Z flag = 0 (BNE branch) if different
\\
\\ Only check accumulator and C flag for 6502
\\
.compare    LDA DA
			CMP AR
			BNE c1
			LDA DNVZC \\ [7] see text
			EOR NF
			AND #$80  \\ mask off N flag
			BNE c1
			LDA DNVZC \\ [8] see text
			EOR VF
			AND #$40  \\ mask off V flag
			BNE c1    \\ [9] see text
			LDA DNVZC
			EOR ZF    \\ mask off Z flag
			AND #2
			BNE c1    \\ [10] see text
			LDA DNVZC
			EOR CF
			AND #1    \\ mask off C flag
.c1			RTS

\\ These routines store the predicted values for ADC and SBC for the 6502,
\\ 65C02, and 65816 in AR, CF, NF, VF, and ZF

.a6502		LDA VF
\\
\\ since all 8 bits of the P register were stored in VF, bit 7 of VF contains
\\ the N flag for NF
\\
			STA NF
			LDA HNVZC
			STA ZF
			RTS

.s6502		JSR sub1
			LDA HNVZC
			STA NF
			STA VF
			STA ZF
			STA CF
			RTS

ORG $f500
.stop
			JMP stop

\\ set RESET vector to program start
ORG $fffc
EQUW start


.end

\\ RAM variables
ORG $4000

.ERROR
EQUB    0

.N1L
EQUB    0

.N2L
EQUB   0

.N1H
EQUB    0

.N2H
EQUW    0

.N1
EQUB    0

.N2
EQUB    0

.DA
EQUB    0

.HA
EQUB    0

.DNVZC
EQUB    0

.HNVZC
EQUB	0

.AR
EQUB    0

.CF
EQUB    0

.NF
EQUB    0

.VF
EQUB    0

.ZF
EQUB    0




SAVE "BCD_test.rom", start, end