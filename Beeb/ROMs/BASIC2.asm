0000 c9 01    CMP #&01
0002 f0 1f    BEQ &0023
0004 60       RTS     
0005 ea       NOP     
0006 60       RTS     
0007 0e 01 42 ASL &4201
000a 41 53    EOR (&53;X)
000c 49 43    EOR #&43
000e 00       BRK     
000f 28       PLP     
0010 43       ???
0011 29 31    AND #&31
0013 39 38 32 AND &3238,Y
0016 20 41 63 JSR &6341
0019 6f       ???
001a 72       ???
001b 6e 0a 0d ROR &0d0a
001e 00       BRK     
001f 00       BRK     
0020 80       ???
0021 00       BRK     
0022 00       BRK     
0023 a9 84    LDA #&84
0025 20 f4 ff JSR &fff4
0028 86 06    STX &06
002a 84 07    STY &07
002c a9 83    LDA #&83
002e 20 f4 ff JSR &fff4
0031 84 18    STY &18
0033 a2 00    LDX #&00
0035 86 1f    STX &1f
0037 8e 02 04 STX &0402
003a 8e 03 04 STX &0403
003d ca       DEX     
003e 86 23    STX &23
0040 a2 0a    LDX #&0a
0042 8e 00 04 STX &0400
0045 ca       DEX     
0046 8e 01 04 STX &0401
0049 a9 01    LDA #&01
004b 25 11    AND &11
004d 05 0d    ORA &0d
004f 05 0e    ORA &0e
0051 05 0f    ORA &0f
0053 05 10    ORA &10
0055 d0 0c    BNE &0063
0057 a9 41    LDA #&41
0059 85 0d    STA &0d
005b a9 52    LDA #&52
005d 85 0e    STA &0e
005f a9 57    LDA #&57
0061 85 0f    STA &0f
0063 a9 02    LDA #&02
0065 8d 02 02 STA &0202
0068 a9 b4    LDA #&b4
006a 8d 03 02 STA &0203
006d 58       CLI     
006e 4c dd 8a JMP &8add
0071 41 4e    EOR (&4e;X)
0073 44       ???
0074 80       ???
0075 00       BRK     
0076 41 42    EOR (&42;X)
0078 53       ???
0079 94 00    STY &00,X
007b 41 43    EOR (&43;X)
007d 53       ???
007e 95 00    STA &00,X
0080 41 44    EOR (&44;X)
0082 56 41    LSR &41,X
0084 4c 96 00 JMP &0096
0087 41 53    EOR (&53;X)
0089 43       ???
008a 97       ???
008b 00       BRK     
008c 41 53    EOR (&53;X)
008e 4e 98 00 LSR &0098
0091 41 54    EOR (&54;X)
0093 4e 99 00 LSR &0099
0096 41 55    EOR (&55;X)
0098 54       ???
0099 4f       ???
009a c6 10    DEC &10
009c 42       ???
009d 47       ???
009e 45 54    EOR &54
00a0 9a       TXS     
00a1 01 42    ORA (&42;X)
00a3 50 55    BVC &00fa
00a5 54       ???
00a6 d5 03    CMP &03,X
00a8 43       ???
00a9 4f       ???
00aa 4c 4f 55 JMP &554f
00ad 52       ???
00ae fb       ???
00af 02       ???
00b0 43       ???
00b1 41 4c    EOR (&4c;X)
00b3 4c d6 02 JMP &02d6
00b6 43       ???
00b7 48       PHA     
00b8 41 49    EOR (&49;X)
00ba 4e d7 02 LSR &02d7
00bd 43       ???
00be 48       PHA     
00bf 52       ???
00c0 24 bd    BIT &bd
00c2 00       BRK     
00c3 43       ???
00c4 4c 45 41 JMP &4145
00c7 52       ???
00c8 d8       CLD     
00c9 01 43    ORA (&43;X)
00cb 4c 4f 53 JMP &534f
00ce 45 d9    EOR &d9
00d0 03       ???
00d1 43       ???
00d2 4c 47 da JMP &da47
00d5 01 43    ORA (&43;X)
00d7 4c 53 db JMP &db53
00da 01 43    ORA (&43;X)
00dc 4f       ???
00dd 53       ???
00de 9b       ???
00df 00       BRK     
00e0 43       ???
00e1 4f       ???
00e2 55 4e    EOR &4e,X
00e4 54       ???
00e5 9c       ???
00e6 01 44    ORA (&44;X)
00e8 41 54    EOR (&54;X)
00ea 41 dc    EOR (&dc;X)
00ec 20 44 45 JSR &4544
00ef 47       ???
00f0 9d 00 44 STA &4400,X
00f3 45 46    EOR &46
00f5 dd 00 44 CMP &4400,X
00f8 45 4c    EOR &4c
00fa 45 54    EOR &54
00fc 45 c7    EOR &c7
00fe 10 44    BPL &0144
0100 49 56    EOR #&56
0102 81 00    STA (&00;X)
0104 44       ???
0105 49 4d    EOR #&4d
0107 de 02 44 DEC &4402,X
010a 52       ???
010b 41 57    EOR (&57;X)
010d df       ???
010e 02       ???
010f 45 4e    EOR &4e
0111 44       ???
0112 50 52    BVC &0166
0114 4f       ???
0115 43       ???
0116 e1 01    SBC (&01;X)
0118 45 4e    EOR &4e
011a 44       ???
011b e0 01    CPX #&01
011d 45 4e    EOR &4e
011f 56 45    LSR &45,X
0121 4c 4f 50 JMP &504f
0124 45 e2    EOR &e2
0126 02       ???
0127 45 4c    EOR &4c
0129 53       ???
012a 45 8b    EOR &8b
012c 14       ???
012d 45 56    EOR &56
012f 41 4c    EOR (&4c;X)
0131 a0 00    LDY #&00
0133 45 52    EOR &52
0135 4c 9e 01 JMP &019e
0138 45 52    EOR &52
013a 52       ???
013b 4f       ???
013c 52       ???
013d 85 04    STA &04
013f 45 4f    EOR &4f
0141 46 c5    LSR &c5
0143 01 45    ORA (&45;X)
0145 4f       ???
0146 52       ???
0147 82       ???
0148 00       BRK     
0149 45 52    EOR &52
014b 52       ???
014c 9f       ???
014d 01 45    ORA (&45;X)
014f 58       CLI     
0150 50 a1    BVC &00f3
0152 00       BRK     
0153 45 58    EOR &58
0155 54       ???
0156 a2 01    LDX #&01
0158 46 4f    LSR &4f
015a 52       ???
015b e3       ???
015c 02       ???
015d 46 41    LSR &41
015f 4c 53 45 JMP &4553
0162 a3       ???
0163 01 46    ORA (&46;X)
0165 4e a4 08 LSR &08a4
0168 47       ???
0169 4f       ???
016a 54       ???
016b 4f       ???
016c e5 12    SBC &12
016e 47       ???
016f 45 54    EOR &54
0171 24 be    BIT &be
0173 00       BRK     
0174 47       ???
0175 45 54    EOR &54
0177 a5 00    LDA &00
0179 47       ???
017a 4f       ???
017b 53       ???
017c 55 42    EOR &42,X
017e e4 12    CPX &12
0180 47       ???
0181 43       ???
0182 4f       ???
0183 4c e6 02 JMP &02e6
0186 48       PHA     
0187 49 4d    EOR #&4d
0189 45 4d    EOR &4d
018b 93       ???
018c 43       ???
018d 49 4e    EOR #&4e
018f 50 55    BVC &01e6
0191 54       ???
0192 e8       INX     
0193 02       ???
0194 49 46    EOR #&46
0196 e7       ???
0197 02       ???
0198 49 4e    EOR #&4e
019a 4b       ???
019b 45 59    EOR &59
019d 24 bf    BIT &bf
019f 00       BRK     
01a0 49 4e    EOR #&4e
01a2 4b       ???
01a3 45 59    EOR &59
01a5 a6 00    LDX &00
01a7 49 4e    EOR #&4e
01a9 54       ???
01aa a8       TAY     
01ab 00       BRK     
01ac 49 4e    EOR #&4e
01ae 53       ???
01af 54       ???
01b0 52       ???
01b1 28       PLP     
01b2 a7       ???
01b3 00       BRK     
01b4 4c 49 53 JMP &5349
01b7 54       ???
01b8 c9 10    CMP #&10
01ba 4c 49 4e JMP &4e49
01bd 45 86    EOR &86
01bf 00       BRK     
01c0 4c 4f 41 JMP &414f
01c3 44       ???
01c4 c8       INY     
01c5 02       ???
01c6 4c 4f 4d JMP &4d4f
01c9 45 4d    EOR &4d
01cb 92       ???
01cc 43       ???
01cd 4c 4f 43 JMP &434f
01d0 41 4c    EOR (&4c;X)
01d2 ea       NOP     
01d3 02       ???
01d4 4c 45 46 JMP &4645
01d7 54       ???
01d8 24 28    BIT &28
01da c0 00    CPY #&00
01dc 4c 45 4e JMP &4e45
01df a9 00    LDA #&00
01e1 4c 45 54 JMP &5445
01e4 e9 04    SBC #&04
01e6 4c 4f 47 JMP &474f
01e9 ab       ???
01ea 00       BRK     
01eb 4c 4e aa JMP &aa4e
01ee 00       BRK     
01ef 4d 49 44 EOR &4449
01f2 24 28    BIT &28
01f4 c1 00    CMP (&00;X)
01f6 4d 4f 44 EOR &444f
01f9 45 eb    EOR &eb
01fb 02       ???
01fc 4d 4f 44 EOR &444f
01ff 83       ???
0200 00       BRK     
0201 4d 4f 56 EOR &564f
0204 45 ec    EOR &ec
0206 02       ???
0207 4e 45 58 LSR &5845
020a 54       ???
020b ed 02 4e SBC &4e02
020e 45 57    EOR &57
0210 ca       DEX     
0211 01 4e    ORA (&4e;X)
0213 4f       ???
0214 54       ???
0215 ac 00 4f LDY &4f00
0218 4c 44 cb JMP &cb44
021b 01 4f    ORA (&4f;X)
021d 4e ee 02 LSR &02ee
0220 4f       ???
0221 46 46    LSR &46
0223 87       ???
0224 00       BRK     
0225 4f       ???
0226 52       ???
0227 84 00    STY &00
0229 4f       ???
022a 50 45    BVC &0271
022c 4e 49 4e LSR &4e49
022f 8e 00 4f STX &4f00
0232 50 45    BVC &0279
0234 4e 4f 55 LSR &554f
0237 54       ???
0238 ae 00 4f LDX &4f00
023b 50 45    BVC &0282
023d 4e 55 50 LSR &5055
0240 ad 00 4f LDA &4f00
0243 53       ???
0244 43       ???
0245 4c 49 ff JMP &ff49
0248 02       ???
0249 50 52    BVC &029d
024b 49 4e    EOR #&4e
024d 54       ???
024e f1 02    SBC (&02),Y
0250 50 41    BVC &0293
0252 47       ???
0253 45 90    EOR &90
0255 43       ???
0256 50 54    BVC &02ac
0258 52       ???
0259 8f       ???
025a 43       ???
025b 50 49    BVC &02a6
025d af       ???
025e 01 50    ORA (&50;X)
0260 4c 4f 54 JMP &544f
0263 f0 02    BEQ &0267
0265 50 4f    BVC &02b6
0267 49 4e    EOR #&4e
0269 54       ???
026a 28       PLP     
026b b0 00    BCS &026d
026d 50 52    BVC &02c1
026f 4f       ???
0270 43       ???
0271 f2       ???
0272 0a       ASL A
0273 50 4f    BVC &02c4
0275 53       ???
0276 b1 01    LDA (&01),Y
0278 52       ???
0279 45 54    EOR &54
027b 55 52    EOR &52,X
027d 4e f8 01 LSR &01f8
0280 52       ???
0281 45 50    EOR &50
0283 45 41    EOR &41
0285 54       ???
0286 f5 00    SBC &00,X
0288 52       ???
0289 45 50    EOR &50
028b 4f       ???
028c 52       ???
028d 54       ???
028e f6 01    INC &01,X
0290 52       ???
0291 45 41    EOR &41
0293 44       ???
0294 f3       ???
0295 02       ???
0296 52       ???
0297 45 4d    EOR &4d
0299 f4       ???
029a 20 52 55 JSR &5552
029d 4e f9 01 LSR &01f9
02a0 52       ???
02a1 41 44    EOR (&44;X)
02a3 b2       ???
02a4 00       BRK     
02a5 52       ???
02a6 45 53    EOR &53
02a8 54       ???
02a9 4f       ???
02aa 52       ???
02ab 45 f7    EOR &f7
02ad 12       ???
02ae 52       ???
02af 49 47    EOR #&47
02b1 48       PHA     
02b2 54       ???
02b3 24 28    BIT &28
02b5 c2       ???
02b6 00       BRK     
02b7 52       ???
02b8 4e 44 b3 LSR &b344
02bb 01 52    ORA (&52;X)
02bd 45 4e    EOR &4e
02bf 55 4d    EOR &4d,X
02c1 42       ???
02c2 45 52    EOR &52
02c4 cc 10 53 CPY &5310
02c7 54       ???
02c8 45 50    EOR &50
02ca 88       DEY     
02cb 00       BRK     
02cc 53       ???
02cd 41 56    EOR (&56;X)
02cf 45 cd    EOR &cd
02d1 02       ???
02d2 53       ???
02d3 47       ???
02d4 4e b4 00 LSR &00b4
02d7 53       ???
02d8 49 4e    EOR #&4e
02da b5 00    LDA &00,X
02dc 53       ???
02dd 51 52    EOR (&52),Y
02df b6 00    LDX &00,Y
02e1 53       ???
02e2 50 43    BVC &0327
02e4 89       ???
02e5 00       BRK     
02e6 53       ???
02e7 54       ???
02e8 52       ???
02e9 24 c3    BIT &c3
02eb 00       BRK     
02ec 53       ???
02ed 54       ???
02ee 52       ???
02ef 49 4e    EOR #&4e
02f1 47       ???
02f2 24 28    BIT &28
02f4 c4 00    CPY &00
02f6 53       ???
02f7 4f       ???
02f8 55 4e    EOR &4e,X
02fa 44       ???
02fb d4       ???
02fc 02       ???
02fd 53       ???
02fe 54       ???
02ff 4f       ???
0300 50 fa    BVC &02fc
0302 01 54    ORA (&54;X)
0304 41 4e    EOR (&4e;X)
0306 b7       ???
0307 00       BRK     
0308 54       ???
0309 48       PHA     
030a 45 4e    EOR &4e
030c 8c 14 54 STY &5414
030f 4f       ???
0310 b8       CLV     
0311 00       BRK     
0312 54       ???
0313 41 42    EOR (&42;X)
0315 28       PLP     
0316 8a       TXA     
0317 00       BRK     
0318 54       ???
0319 52       ???
031a 41 43    EOR (&43;X)
031c 45 fc    EOR &fc
031e 12       ???
031f 54       ???
0320 49 4d    EOR #&4d
0322 45 91    EOR &91
0324 43       ???
0325 54       ???
0326 52       ???
0327 55 45    EOR &45,X
0329 b9 01 55 LDA &5501,Y
032c 4e 54 49 LSR &4954
032f 4c fd 02 JMP &02fd
0332 55 53    EOR &53,X
0334 52       ???
0335 ba       TSX     
0336 00       BRK     
0337 56 44    LSR &44,X
0339 55 ef    EOR &ef,X
033b 02       ???
033c 56 41    LSR &41,X
033e 4c bb 00 JMP &00bb
0341 56 50    LSR &50,X
0343 4f       ???
0344 53       ???
0345 bc 01 57 LDY &5701,X
0348 49 44    EOR #&44
034a 54       ???
034b 48       PHA     
034c fe 02 50 INC &5002,X
034f 41 47    EOR (&47;X)
0351 45 d0    EOR &d0
0353 00       BRK     
0354 50 54    BVC &03aa
0356 52       ???
0357 cf       ???
0358 00       BRK     
0359 54       ???
035a 49 4d    EOR #&4d
035c 45 d1    EOR &d1
035e 00       BRK     
035f 4c 4f 4d JMP &4d4f
0362 45 4d    EOR &4d
0364 d2       ???
0365 00       BRK     
0366 48       PHA     
0367 49 4d    EOR #&4d
0369 45 4d    EOR &4d
036b d3       ???
036c 00       BRK     
036d 78       SEI     
036e 47       ???
036f c0 b4    CPY #&b4
0371 fc       ???
0372 03       ???
0373 6a       ROR A
0374 d4       ???
0375 33       ???
0376 9e       ???
0377 da       ???
0378 07       ???
0379 6f       ???
037a 8d f7 c2 STA &c2f7
037d 9f       ???
037e a6 e9    LDX &e9
0380 91 46    STA (&46),Y
0382 ca       DEX     
0383 95 b9    STA &b9,X
0385 ad e2 78 LDA &78e2
0388 d1 fe    CMP (&fe),Y
038a a8       TAY     
038b d1 80    CMP (&80),Y
038d 7c       ???
038e cb       ???
038f 41 6d    EOR (&6d;X)
0391 b1 49    LDA (&49),Y
0393 88       DEY     
0394 98       TYA     
0395 b4 be    LDY &be,X
0397 dc       ???
0398 c4 d2    CPY &d2
039a 2f       ???
039b 76 bd    ROR &bd,X
039d bf       ???
039e 26 cc    ROL &cc
03a0 39 ee 94 AND &94ee,Y
03a3 c2       ???
03a4 b8       CLV     
03a5 ac 31 24 LDY &2431
03a8 9c       ???
03a9 da       ???
03aa b6 a3    LDX &a3,Y
03ac f3       ???
03ad 2a       ROL A
03ae 30 83    BMI &0333
03b0 c9 6f    CMP #&6f
03b2 5d 4c 58 EOR &584c,X
03b5 d2       ???
03b6 2a       ROL A
03b7 8d 99 bd STA &bd99
03ba c4 7d    CPY &7d
03bc 7d 2f e8 ADC &e82f,X
03bf c8       INY     
03c0 56 72    LSR &72,X
03c2 c4 88    CPY &88
03c4 cc 7a c2 CPY &c27a
03c7 44       ???
03c8 e4 23    CPX &23
03ca 9a       TXS     
03cb e4 95    CPX &95
03cd 15 2f    ORA &2f,X
03cf f1 9a    SBC (&9a),Y
03d1 04       ???
03d2 1f       ???
03d3 7d e4 e4 ADC &e4e4,X
03d6 e6 b6    INC &b6
03d8 11 d0    ORA (&d0),Y
03da 8e 95 b1 STX &b195
03dd a0 c2    LDY #&c2
03df bf       ???
03e0 bf       ???
03e1 ae ae ae LDX &aeae
03e4 af       ???
03e5 ad a8 ab LDA &aba8
03e8 ac a8 a9 LDY &a9a8
03eb bf       ???
03ec a9 ae    LDA #&ae
03ee ab       ???
03ef af       ???
03f0 af       ???
03f1 ab       ???
03f2 aa       TAX     
03f3 bf       ???
03f4 ae b1 af LDX &afb1
03f7 ac ac ac LDY &acac
03fa ae a7 ab LDX &aba7
03fd ac bf bf LDY &bfbf
0400 ab       ???
0401 ab       ???
0402 ab       ???
0403 ab       ???
0404 af       ???
0405 ab       ???
0406 a9 a7    LDA #&a7
0408 a6 ae    LDX &ae
040a ac ab ac LDY &acab
040d ab       ???
040e b3       ???
040f af       ???
0410 b0 af    BCS &03c1
0412 b0 af    BCS &03c3
0414 b0 b0    BCS &03c6
0416 ac 90 8f LDY &8f90
0419 bf       ???
041a b5 8a    LDA &8a,X
041c 8a       TXA     
041d 8f       ???
041e be 98 bf LDX &bf98,Y
0421 92       ???
0422 92       ???
0423 92       ???
0424 92       ???
0425 b4 bf    LDY &bf,X
0427 8e bf 92 STX &92bf
042a bf       ???
042b 8e 8e 8b STX &8b8e
042e 8b       ???
042f 91 93    STA (&93),Y
0431 8a       TXA     
0432 93       ???
0433 b4 b7    LDY &b7,X
0435 b8       CLV     
0436 b8       CLV     
0437 93       ???
0438 98       TYA     
0439 ba       TSX     
043a 8b       ???
043b 93       ???
043c 93       ???
043d 93       ???
043e b6 b9    LDX &b9,Y
0440 94 93    STY &93,X
0442 8d 93 bb STA &bb93
0445 8b       ???
0446 bb       ???
0447 bf       ???
0448 ba       TSX     
0449 b8       CLV     
044a bd 8a 93 LDA &938a,X
044d 92       ???
044e bb       ???
044f b4 be    LDY &be,X
0451 4b       ???
0452 83       ???
0453 84 89    STY &89
0455 96 b8    STX &b8,Y
0457 b9 d8 d9 LDA &d9d8,Y
045a f0 01    BEQ &045d
045c 10 81    BPL &03df
045e 90 89    BCC &03e9
0460 93       ???
0461 a3       ???
0462 a4 a9    LDY &a9
0464 38       SEC     
0465 39 78 01 AND &0178,Y
0468 13       ???
0469 21 63    AND (&63;X)
046b 73       ???
046c b1 a9    LDA (&a9),Y
046e c5 0c    CMP &0c
0470 c3       ???
0471 d3       ???
0472 c4 f2    CPY &f2
0474 41 83    EOR (&83;X)
0476 b0 81    BCS &03f9
0478 43       ???
0479 6c 72 ec JMP (&ec72)
047c f2       ???
047d a3       ???
047e c3       ???
047f 18       CLC     
0480 19 34 b0 ORA &b034,Y
0483 72       ???
0484 98       TYA     
0485 99 81 98 STA &9881,Y
0488 99 14 35 STA &3514,Y
048b 0a       ASL A
048c 0d 0d 0d ORA &0d0d
048f 0d 10 10 ORA &1010
0492 25 25    AND &25
0494 39 41 41 AND &4141,Y
0497 41 41    EOR (&41;X)
0499 4a       LSR A
049a 4a       LSR A
049b 4c 4c 4c JMP &4c4c
049e 50 50    BVC &04f0
04a0 52       ???
04a1 53       ???
04a2 53       ???
04a3 53       ???
04a4 08       PHP     
04a5 08       PHP     
04a6 08       PHP     
04a7 09 09    ORA #&09
04a9 0a       ASL A
04aa 0a       ASL A
04ab 0a       ASL A
04ac 05 15    ORA &15
04ae 3e 04 0d ROL &0d04,X
04b1 30 4c    BMI &04ff
04b3 06 32    ASL &32
04b5 49 49    EOR #&49
04b7 10 25    BPL &04de
04b9 0e 0e 09 ASL &090e
04bc 29 2a    AND #&2a
04be 30 30    BMI &04f0
04c0 4e 4e 4e LSR &4e4e
04c3 3e 16 00 ROL &0016,X
04c6 18       CLC     
04c7 d8       CLD     
04c8 58       CLI     
04c9 b8       CLV     
04ca ca       DEX     
04cb 88       DEY     
04cc e8       INX     
04cd c8       INY     
04ce ea       NOP     
04cf 48       PHA     
04d0 08       PHP     
04d1 68       PLA     
04d2 28       PLP     
04d3 40       RTI     
04d4 60       RTS     
04d5 38       SEC     
04d6 f8       SED     
04d7 78       SEI     
04d8 aa       TAX     
04d9 a8       TAY     
04da ba       TSX     
04db 8a       TXA     
04dc 9a       TXS     
04dd 98       TYA     
04de 90 b0    BCC &0490
04e0 f0 30    BEQ &0512
04e2 d0 10    BNE &04f4
04e4 50 70    BVC &0556
04e6 21 41    AND (&41;X)
04e8 01 61    ORA (&61;X)
04ea c1 a1    CMP (&a1;X)
04ec e1 06    SBC (&06;X)
04ee 46 26    LSR &26
04f0 66 c6    ROR &c6
04f2 e6 e0    INC &e0
04f4 c0 20    CPY #&20
04f6 4c 20 a2 JMP &a220
04f9 a0 81    LDY #&81
04fb 86 84    STX &84
04fd a9 ff    LDA #&ff
04ff 85 28    STA &28
0501 4c a3 8b JMP &8ba3
0504 a9 03    LDA #&03
0506 85 28    STA &28
0508 20 97 8a JSR &8a97
050b c9 5d    CMP #&5d
050d f0 ee    BEQ &04fd
050f 20 6d 98 JSR &986d
0512 c6 0a    DEC &0a
0514 20 ba 85 JSR &85ba
0517 c6 0a    DEC &0a
0519 a5 28    LDA &28
051b 4a       LSR A
051c 90 60    BCC &057e
051e a5 1e    LDA &1e
0520 69 04    ADC #&04
0522 85 3f    STA &3f
0524 a5 38    LDA &38
0526 20 45 b5 JSR &b545
0529 a5 37    LDA &37
052b 20 62 b5 JSR &b562
052e a2 fc    LDX #&fc
0530 a4 39    LDY &39
0532 10 02    BPL &0536
0534 a4 36    LDY &36
0536 84 38    STY &38
0538 f0 1c    BEQ &0556
053a a0 00    LDY #&00
053c e8       INX     
053d d0 0d    BNE &054c
053f 20 25 bc JSR &bc25
0542 a6 3f    LDX &3f
0544 20 65 b5 JSR &b565
0547 ca       DEX     
0548 d0 fa    BNE &0544
054a a2 fd    LDX #&fd
054c b1 3a    LDA (&3a),Y
054e 20 62 b5 JSR &b562
0551 c8       INY     
0552 c6 38    DEC &38
0554 d0 e6    BNE &053c
0556 e8       INX     
0557 10 0c    BPL &0565
0559 20 65 b5 JSR &b565
055c 20 58 b5 JSR &b558
055f 20 58 b5 JSR &b558
0562 4c 56 85 JMP &8556
0565 a0 00    LDY #&00
0567 b1 0b    LDA (&0b),Y
0569 c9 3a    CMP #&3a
056b f0 0a    BEQ &0577
056d c9 0d    CMP #&0d
056f f0 0a    BEQ &057b
0571 20 0e b5 JSR &b50e
0574 c8       INY     
0575 d0 f0    BNE &0567
0577 c4 0a    CPY &0a
0579 90 f6    BCC &0571
057b 20 25 bc JSR &bc25
057e a4 0a    LDY &0a
0580 88       DEY     
0581 c8       INY     
0582 b1 0b    LDA (&0b),Y
0584 c9 3a    CMP #&3a
0586 f0 04    BEQ &058c
0588 c9 0d    CMP #&0d
058a d0 f5    BNE &0581
058c 20 59 98 JSR &9859
058f 88       DEY     
0590 b1 0b    LDA (&0b),Y
0592 c9 3a    CMP #&3a
0594 f0 0c    BEQ &05a2
0596 a5 0c    LDA &0c
0598 c9 07    CMP #&07
059a d0 03    BNE &059f
059c 4c f6 8a JMP &8af6
059f 20 90 98 JSR &9890
05a2 4c 08 85 JMP &8508
05a5 20 82 95 JSR &9582
05a8 f0 5a    BEQ &0604
05aa b0 58    BCS &0604
05ac 20 94 bd JSR &bd94
05af 20 3a ae JSR &ae3a
05b2 85 27    STA &27
05b4 20 b4 b4 JSR &b4b4
05b7 20 27 88 JSR &8827
05ba a2 03    LDX #&03
05bc 20 97 8a JSR &8a97
05bf a0 00    LDY #&00
05c1 84 3d    STY &3d
05c3 c9 3a    CMP #&3a
05c5 f0 64    BEQ &062b
05c7 c9 0d    CMP #&0d
05c9 f0 60    BEQ &062b
05cb c9 5c    CMP #&5c
05cd f0 5c    BEQ &062b
05cf c9 2e    CMP #&2e
05d1 f0 d2    BEQ &05a5
05d3 c6 0a    DEC &0a
05d5 a4 0a    LDY &0a
05d7 e6 0a    INC &0a
05d9 b1 0b    LDA (&0b),Y
05db 30 2a    BMI &0607
05dd c9 20    CMP #&20
05df f0 10    BEQ &05f1
05e1 a0 05    LDY #&05
05e3 0a       ASL A
05e4 0a       ASL A
05e5 0a       ASL A
05e6 0a       ASL A
05e7 26 3d    ROL &3d
05e9 26 3e    ROL &3e
05eb 88       DEY     
05ec d0 f8    BNE &05e6
05ee ca       DEX     
05ef d0 e4    BNE &05d5
05f1 a2 3a    LDX #&3a
05f3 a5 3d    LDA &3d
05f5 dd 50 84 CMP &8450,X
05f8 d0 07    BNE &0601
05fa bc 8a 84 LDY &848a,X
05fd c4 3e    CPY &3e
05ff f0 1f    BEQ &0620
0601 ca       DEX     
0602 d0 f1    BNE &05f5
0604 4c 2a 98 JMP &982a
0607 a2 22    LDX #&22
0609 c9 80    CMP #&80
060b f0 13    BEQ &0620
060d e8       INX     
060e c9 82    CMP #&82
0610 f0 0e    BEQ &0620
0612 e8       INX     
0613 c9 84    CMP #&84
0615 d0 ed    BNE &0604
0617 e6 0a    INC &0a
0619 c8       INY     
061a b1 0b    LDA (&0b),Y
061c c9 41    CMP #&41
061e d0 e4    BNE &0604
0620 bd c4 84 LDA &84c4,X
0623 85 29    STA &29
0625 a0 01    LDY #&01
0627 e0 1a    CPX #&1a
0629 b0 48    BCS &0673
062b ad 40 04 LDA &0440
062e 85 37    STA &37
0630 84 39    STY &39
0632 a6 28    LDX &28
0634 e0 04    CPX #&04
0636 ae 41 04 LDX &0441
0639 86 38    STX &38
063b 90 06    BCC &0643
063d ad 3c 04 LDA &043c
0640 ae 3d 04 LDX &043d
0643 85 3a    STA &3a
0645 86 3b    STX &3b
0647 98       TYA     
0648 f0 28    BEQ &0672
064a 10 04    BPL &0650
064c a4 36    LDY &36
064e f0 22    BEQ &0672
0650 88       DEY     
0651 b9 29 00 LDA &0029,Y
0654 24 39    BIT &39
0656 10 03    BPL &065b
0658 b9 00 06 LDA &0600,Y
065b 91 3a    STA (&3a),Y
065d ee 40 04 INC &0440
0660 d0 03    BNE &0665
0662 ee 41 04 INC &0441
0665 90 08    BCC &066f
0667 ee 3c 04 INC &043c
066a d0 03    BNE &066f
066c ee 3d 04 INC &043d
066f 98       TYA     
0670 d0 de    BNE &0650
0672 60       RTS     
0673 e0 22    CPX #&22
0675 b0 40    BCS &06b7
0677 20 21 88 JSR &8821
067a 18       CLC     
067b a5 2a    LDA &2a
067d ed 40 04 SBC &0440
0680 a8       TAY     
0681 a5 2b    LDA &2b
0683 ed 41 04 SBC &0441
0686 c0 01    CPY #&01
0688 88       DEY     
0689 e9 00    SBC #&00
068b f0 25    BEQ &06b2
068d c9 ff    CMP #&ff
068f f0 1c    BEQ &06ad
0691 a5 28    LDA &28
0693 4a       LSR A
0694 f0 0f    BEQ &06a5
0696 00       BRK     
0697 01 4f    ORA (&4f;X)
0699 75 74    ADC &74,X
069b 20 6f 66 JSR &666f
069e 20 72 61 JSR &6172
06a1 6e 67 65 ROR &6567
06a4 00       BRK     
06a5 a8       TAY     
06a6 84 2a    STY &2a
06a8 a0 02    LDY #&02
06aa 4c 2b 86 JMP &862b
06ad 98       TYA     
06ae 30 f6    BMI &06a6
06b0 10 df    BPL &0691
06b2 98       TYA     
06b3 10 f1    BPL &06a6
06b5 30 da    BMI &0691
06b7 e0 29    CPX #&29
06b9 b0 18    BCS &06d3
06bb 20 97 8a JSR &8a97
06be c9 23    CMP #&23
06c0 d0 18    BNE &06da
06c2 20 2f 88 JSR &882f
06c5 20 21 88 JSR &8821
06c8 a5 2b    LDA &2b
06ca f0 dc    BEQ &06a8
06cc 00       BRK     
06cd 02       ???
06ce 42       ???
06cf 79 74 65 ADC &6574,Y
06d2 00       BRK     
06d3 e0 36    CPX #&36
06d5 d0 68    BNE &073f
06d7 20 97 8a JSR &8a97
06da c9 28    CMP #&28
06dc d0 37    BNE &0715
06de 20 21 88 JSR &8821
06e1 20 97 8a JSR &8a97
06e4 c9 29    CMP #&29
06e6 d0 13    BNE &06fb
06e8 20 97 8a JSR &8a97
06eb c9 2c    CMP #&2c
06ed d0 1e    BNE &070d
06ef 20 2c 88 JSR &882c
06f2 20 97 8a JSR &8a97
06f5 c9 59    CMP #&59
06f7 d0 14    BNE &070d
06f9 f0 cd    BEQ &06c8
06fb c9 2c    CMP #&2c
06fd d0 0e    BNE &070d
06ff 20 97 8a JSR &8a97
0702 c9 58    CMP #&58
0704 d0 07    BNE &070d
0706 20 97 8a JSR &8a97
0709 c9 29    CMP #&29
070b f0 bb    BEQ &06c8
070d 00       BRK     
070e 03       ???
070f 49 6e    EOR #&6e
0711 64       ???
0712 65 78    ADC &78
0714 00       BRK     
0715 c6 0a    DEC &0a
0717 20 21 88 JSR &8821
071a 20 97 8a JSR &8a97
071d c9 2c    CMP #&2c
071f d0 14    BNE &0735
0721 20 2c 88 JSR &882c
0724 20 97 8a JSR &8a97
0727 c9 58    CMP #&58
0729 f0 0a    BEQ &0735
072b c9 59    CMP #&59
072d d0 de    BNE &070d
072f 20 2f 88 JSR &882f
0732 4c 9a 87 JMP &879a
0735 20 32 88 JSR &8832
0738 a5 2b    LDA &2b
073a d0 f3    BNE &072f
073c 4c a8 86 JMP &86a8
073f e0 2f    CPX #&2f
0741 b0 2b    BCS &076e
0743 e0 2d    CPX #&2d
0745 b0 09    BCS &0750
0747 20 97 8a JSR &8a97
074a c9 41    CMP #&41
074c f0 19    BEQ &0767
074e c6 0a    DEC &0a
0750 20 21 88 JSR &8821
0753 20 97 8a JSR &8a97
0756 c9 2c    CMP #&2c
0758 d0 de    BNE &0738
075a 20 2c 88 JSR &882c
075d 20 97 8a JSR &8a97
0760 c9 58    CMP #&58
0762 f0 d4    BEQ &0738
0764 4c 0d 87 JMP &870d
0767 20 32 88 JSR &8832
076a a0 01    LDY #&01
076c d0 2e    BNE &079c
076e e0 32    CPX #&32
0770 b0 16    BCS &0788
0772 e0 31    CPX #&31
0774 f0 0c    BEQ &0782
0776 20 97 8a JSR &8a97
0779 c9 23    CMP #&23
077b d0 03    BNE &0780
077d 4c c5 86 JMP &86c5
0780 c6 0a    DEC &0a
0782 20 21 88 JSR &8821
0785 4c 35 87 JMP &8735
0788 e0 33    CPX #&33
078a f0 0b    BEQ &0797
078c b0 24    BCS &07b2
078e 20 97 8a JSR &8a97
0791 c9 28    CMP #&28
0793 f0 0a    BEQ &079f
0795 c6 0a    DEC &0a
0797 20 21 88 JSR &8821
079a a0 03    LDY #&03
079c 4c 2b 86 JMP &862b
079f 20 2c 88 JSR &882c
07a2 20 2c 88 JSR &882c
07a5 20 21 88 JSR &8821
07a8 20 97 8a JSR &8a97
07ab c9 29    CMP #&29
07ad f0 eb    BEQ &079a
07af 4c 0d 87 JMP &870d
07b2 e0 39    CPX #&39
07b4 b0 5d    BCS &0813
07b6 a5 3d    LDA &3d
07b8 49 01    EOR #&01
07ba 29 1f    AND #&1f
07bc 48       PHA     
07bd e0 37    CPX #&37
07bf b0 2f    BCS &07f0
07c1 20 97 8a JSR &8a97
07c4 c9 23    CMP #&23
07c6 d0 04    BNE &07cc
07c8 68       PLA     
07c9 4c c5 86 JMP &86c5
07cc c6 0a    DEC &0a
07ce 20 21 88 JSR &8821
07d1 68       PLA     
07d2 85 37    STA &37
07d4 20 97 8a JSR &8a97
07d7 c9 2c    CMP #&2c
07d9 f0 03    BEQ &07de
07db 4c 35 87 JMP &8735
07de 20 97 8a JSR &8a97
07e1 29 1f    AND #&1f
07e3 c5 37    CMP &37
07e5 d0 06    BNE &07ed
07e7 20 2c 88 JSR &882c
07ea 4c 35 87 JMP &8735
07ed 4c 0d 87 JMP &870d
07f0 20 21 88 JSR &8821
07f3 68       PLA     
07f4 85 37    STA &37
07f6 20 97 8a JSR &8a97
07f9 c9 2c    CMP #&2c
07fb d0 13    BNE &0810
07fd 20 97 8a JSR &8a97
0800 29 1f    AND #&1f
0802 c5 37    CMP &37
0804 d0 e7    BNE &07ed
0806 20 2c 88 JSR &882c
0809 a5 2b    LDA &2b
080b f0 03    BEQ &0810
080d 4c cc 86 JMP &86cc
0810 4c 38 87 JMP &8738
0813 d0 25    BNE &083a
0815 20 21 88 JSR &8821
0818 a5 2a    LDA &2a
081a 85 28    STA &28
081c a0 00    LDY #&00
081e 4c 2b 86 JMP &862b
0821 20 1d 9b JSR &9b1d
0824 20 f0 92 JSR &92f0
0827 a4 1b    LDY &1b
0829 84 0a    STY &0a
082b 60       RTS     
082c 20 2f 88 JSR &882f
082f 20 32 88 JSR &8832
0832 a5 29    LDA &29
0834 18       CLC     
0835 69 04    ADC #&04
0837 85 29    STA &29
0839 60       RTS     
083a a2 01    LDX #&01
083c a4 0a    LDY &0a
083e e6 0a    INC &0a
0840 b1 0b    LDA (&0b),Y
0842 c9 42    CMP #&42
0844 f0 12    BEQ &0858
0846 e8       INX     
0847 c9 57    CMP #&57
0849 f0 0d    BEQ &0858
084b a2 04    LDX #&04
084d c9 44    CMP #&44
084f f0 07    BEQ &0858
0851 c9 53    CMP #&53
0853 f0 15    BEQ &086a
0855 4c 2a 98 JMP &982a
0858 8a       TXA     
0859 48       PHA     
085a 20 21 88 JSR &8821
085d a2 29    LDX #&29
085f 20 44 be JSR &be44
0862 68       PLA     
0863 a8       TAY     
0864 4c 2b 86 JMP &862b
0867 4c 0e 8c JMP &8c0e
086a a5 28    LDA &28
086c 48       PHA     
086d 20 1d 9b JSR &9b1d
0870 d0 f5    BNE &0867
0872 68       PLA     
0873 85 28    STA &28
0875 20 27 88 JSR &8827
0878 a0 ff    LDY #&ff
087a d0 e8    BNE &0864
087c 48       PHA     
087d 18       CLC     
087e 98       TYA     
087f 65 37    ADC &37
0881 85 39    STA &39
0883 a0 00    LDY #&00
0885 98       TYA     
0886 65 38    ADC &38
0888 85 3a    STA &3a
088a 68       PLA     
088b 91 37    STA (&37),Y
088d c8       INY     
088e b1 39    LDA (&39),Y
0890 91 37    STA (&37),Y
0892 c9 0d    CMP #&0d
0894 d0 f7    BNE &088d
0896 60       RTS     
0897 29 0f    AND #&0f
0899 85 3d    STA &3d
089b 84 3e    STY &3e
089d c8       INY     
089e b1 37    LDA (&37),Y
08a0 c9 3a    CMP #&3a
08a2 b0 36    BCS &08da
08a4 c9 30    CMP #&30
08a6 90 32    BCC &08da
08a8 29 0f    AND #&0f
08aa 48       PHA     
08ab a6 3e    LDX &3e
08ad a5 3d    LDA &3d
08af 0a       ASL A
08b0 26 3e    ROL &3e
08b2 30 21    BMI &08d5
08b4 0a       ASL A
08b5 26 3e    ROL &3e
08b7 30 1c    BMI &08d5
08b9 65 3d    ADC &3d
08bb 85 3d    STA &3d
08bd 8a       TXA     
08be 65 3e    ADC &3e
08c0 06 3d    ASL &3d
08c2 2a       ROL A
08c3 30 10    BMI &08d5
08c5 b0 0e    BCS &08d5
08c7 85 3e    STA &3e
08c9 68       PLA     
08ca 65 3d    ADC &3d
08cc 85 3d    STA &3d
08ce 90 cd    BCC &089d
08d0 e6 3e    INC &3e
08d2 10 c9    BPL &089d
08d4 48       PHA     
08d5 68       PLA     
08d6 a0 00    LDY #&00
08d8 38       SEC     
08d9 60       RTS     
08da 88       DEY     
08db a9 8d    LDA #&8d
08dd 20 7c 88 JSR &887c
08e0 a5 37    LDA &37
08e2 69 02    ADC #&02
08e4 85 39    STA &39
08e6 a5 38    LDA &38
08e8 69 00    ADC #&00
08ea 85 3a    STA &3a
08ec b1 37    LDA (&37),Y
08ee 91 39    STA (&39),Y
08f0 88       DEY     
08f1 d0 f9    BNE &08ec
08f3 a0 03    LDY #&03
08f5 a5 3e    LDA &3e
08f7 09 40    ORA #&40
08f9 91 37    STA (&37),Y
08fb 88       DEY     
08fc a5 3d    LDA &3d
08fe 29 3f    AND #&3f
0900 09 40    ORA #&40
0902 91 37    STA (&37),Y
0904 88       DEY     
0905 a5 3d    LDA &3d
0907 29 c0    AND #&c0
0909 85 3d    STA &3d
090b a5 3e    LDA &3e
090d 29 c0    AND #&c0
090f 4a       LSR A
0910 4a       LSR A
0911 05 3d    ORA &3d
0913 4a       LSR A
0914 4a       LSR A
0915 49 54    EOR #&54
0917 91 37    STA (&37),Y
0919 20 44 89 JSR &8944
091c 20 44 89 JSR &8944
091f 20 44 89 JSR &8944
0922 a0 00    LDY #&00
0924 18       CLC     
0925 60       RTS     
0926 c9 7b    CMP #&7b
0928 b0 fa    BCS &0924
092a c9 5f    CMP #&5f
092c b0 0e    BCS &093c
092e c9 5b    CMP #&5b
0930 b0 f2    BCS &0924
0932 c9 41    CMP #&41
0934 b0 06    BCS &093c
0936 c9 3a    CMP #&3a
0938 b0 ea    BCS &0924
093a c9 30    CMP #&30
093c 60       RTS     
093d c9 2e    CMP #&2e
093f d0 f5    BNE &0936
0941 60       RTS     
0942 b1 37    LDA (&37),Y
0944 e6 37    INC &37
0946 d0 02    BNE &094a
0948 e6 38    INC &38
094a 60       RTS     
094b 20 44 89 JSR &8944
094e b1 37    LDA (&37),Y
0950 60       RTS     
0951 a0 00    LDY #&00
0953 84 3b    STY &3b
0955 84 3c    STY &3c
0957 b1 37    LDA (&37),Y
0959 c9 0d    CMP #&0d
095b f0 ed    BEQ &094a
095d c9 20    CMP #&20
095f d0 05    BNE &0966
0961 20 44 89 JSR &8944
0964 d0 f1    BNE &0957
0966 c9 26    CMP #&26
0968 d0 12    BNE &097c
096a 20 4b 89 JSR &894b
096d 20 36 89 JSR &8936
0970 b0 f8    BCS &096a
0972 c9 41    CMP #&41
0974 90 e1    BCC &0957
0976 c9 47    CMP #&47
0978 90 f0    BCC &096a
097a b0 db    BCS &0957
097c c9 22    CMP #&22
097e d0 0c    BNE &098c
0980 20 4b 89 JSR &894b
0983 c9 22    CMP #&22
0985 f0 da    BEQ &0961
0987 c9 0d    CMP #&0d
0989 d0 f5    BNE &0980
098b 60       RTS     
098c c9 3a    CMP #&3a
098e d0 06    BNE &0996
0990 84 3b    STY &3b
0992 84 3c    STY &3c
0994 f0 cb    BEQ &0961
0996 c9 2c    CMP #&2c
0998 f0 c7    BEQ &0961
099a c9 2a    CMP #&2a
099c d0 05    BNE &09a3
099e a5 3b    LDA &3b
09a0 d0 41    BNE &09e3
09a2 60       RTS     
09a3 c9 2e    CMP #&2e
09a5 f0 0e    BEQ &09b5
09a7 20 36 89 JSR &8936
09aa 90 33    BCC &09df
09ac a6 3c    LDX &3c
09ae f0 05    BEQ &09b5
09b0 20 97 88 JSR &8897
09b3 90 34    BCC &09e9
09b5 b1 37    LDA (&37),Y
09b7 20 3d 89 JSR &893d
09ba 90 06    BCC &09c2
09bc 20 44 89 JSR &8944
09bf 4c b5 89 JMP &89b5
09c2 a2 ff    LDX #&ff
09c4 86 3b    STX &3b
09c6 84 3c    STY &3c
09c8 4c 57 89 JMP &8957
09cb 20 26 89 JSR &8926
09ce 90 13    BCC &09e3
09d0 a0 00    LDY #&00
09d2 b1 37    LDA (&37),Y
09d4 20 26 89 JSR &8926
09d7 90 e9    BCC &09c2
09d9 20 44 89 JSR &8944
09dc 4c d2 89 JMP &89d2
09df c9 41    CMP #&41
09e1 b0 09    BCS &09ec
09e3 a2 ff    LDX #&ff
09e5 86 3b    STX &3b
09e7 84 3c    STY &3c
09e9 4c 61 89 JMP &8961
09ec c9 58    CMP #&58
09ee b0 db    BCS &09cb
09f0 a2 71    LDX #&71
09f2 86 39    STX &39
09f4 a2 80    LDX #&80
09f6 86 3a    STX &3a
09f8 d1 39    CMP (&39),Y
09fa 90 d6    BCC &09d2
09fc d0 0f    BNE &0a0d
09fe c8       INY     
09ff b1 39    LDA (&39),Y
0a01 30 34    BMI &0a37
0a03 d1 37    CMP (&37),Y
0a05 f0 f7    BEQ &09fe
0a07 b1 37    LDA (&37),Y
0a09 c9 2e    CMP #&2e
0a0b f0 0b    BEQ &0a18
0a0d c8       INY     
0a0e b1 39    LDA (&39),Y
0a10 10 fb    BPL &0a0d
0a12 c9 fe    CMP #&fe
0a14 d0 0f    BNE &0a25
0a16 b0 b8    BCS &09d0
0a18 c8       INY     
0a19 b1 39    LDA (&39),Y
0a1b 30 1a    BMI &0a37
0a1d e6 39    INC &39
0a1f d0 f8    BNE &0a19
0a21 e6 3a    INC &3a
0a23 d0 f4    BNE &0a19
0a25 38       SEC     
0a26 c8       INY     
0a27 98       TYA     
0a28 65 39    ADC &39
0a2a 85 39    STA &39
0a2c 90 02    BCC &0a30
0a2e e6 3a    INC &3a
0a30 a0 00    LDY #&00
0a32 b1 37    LDA (&37),Y
0a34 4c f8 89 JMP &89f8
0a37 aa       TAX     
0a38 c8       INY     
0a39 b1 39    LDA (&39),Y
0a3b 85 3d    STA &3d
0a3d 88       DEY     
0a3e 4a       LSR A
0a3f 90 07    BCC &0a48
0a41 b1 37    LDA (&37),Y
0a43 20 26 89 JSR &8926
0a46 b0 88    BCS &09d0
0a48 8a       TXA     
0a49 24 3d    BIT &3d
0a4b 50 07    BVC &0a54
0a4d a6 3b    LDX &3b
0a4f d0 03    BNE &0a54
0a51 18       CLC     
0a52 69 40    ADC #&40
0a54 88       DEY     
0a55 20 7c 88 JSR &887c
0a58 a0 00    LDY #&00
0a5a a2 ff    LDX #&ff
0a5c a5 3d    LDA &3d
0a5e 4a       LSR A
0a5f 4a       LSR A
0a60 90 04    BCC &0a66
0a62 86 3b    STX &3b
0a64 84 3c    STY &3c
0a66 4a       LSR A
0a67 90 04    BCC &0a6d
0a69 84 3b    STY &3b
0a6b 84 3c    STY &3c
0a6d 4a       LSR A
0a6e 90 11    BCC &0a81
0a70 48       PHA     
0a71 c8       INY     
0a72 b1 37    LDA (&37),Y
0a74 20 26 89 JSR &8926
0a77 90 06    BCC &0a7f
0a79 20 44 89 JSR &8944
0a7c 4c 72 8a JMP &8a72
0a7f 88       DEY     
0a80 68       PLA     
0a81 4a       LSR A
0a82 90 02    BCC &0a86
0a84 86 3c    STX &3c
0a86 4a       LSR A
0a87 b0 0d    BCS &0a96
0a89 4c 61 89 JMP &8961
0a8c a4 1b    LDY &1b
0a8e e6 1b    INC &1b
0a90 b1 19    LDA (&19),Y
0a92 c9 20    CMP #&20
0a94 f0 f6    BEQ &0a8c
0a96 60       RTS     
0a97 a4 0a    LDY &0a
0a99 e6 0a    INC &0a
0a9b b1 0b    LDA (&0b),Y
0a9d c9 20    CMP #&20
0a9f f0 f6    BEQ &0a97
0aa1 60       RTS     
0aa2 00       BRK     
0aa3 05 4d    ORA &4d
0aa5 69 73    ADC #&73
0aa7 73       ???
0aa8 69 6e    ADC #&6e
0aaa 67       ???
0aab 20 2c 00 JSR &002c
0aae 20 8c 8a JSR &8a8c
0ab1 c9 2c    CMP #&2c
0ab3 d0 ed    BNE &0aa2
0ab5 60       RTS     
0ab6 20 57 98 JSR &9857
0ab9 a5 18    LDA &18
0abb 85 38    STA &38
0abd a9 00    LDA #&00
0abf 85 37    STA &37
0ac1 91 37    STA (&37),Y
0ac3 20 6f be JSR &be6f
0ac6 d0 2b    BNE &0af3
0ac8 20 57 98 JSR &9857
0acb 20 6f be JSR &be6f
0ace d0 26    BNE &0af6
0ad0 20 57 98 JSR &9857
0ad3 00       BRK     
0ad4 00       BRK     
0ad5 53       ???
0ad6 54       ???
0ad7 4f       ???
0ad8 50 00    BVC &0ada
0ada 20 57 98 JSR &9857
0add a9 0d    LDA #&0d
0adf a4 18    LDY &18
0ae1 84 13    STY &13
0ae3 a0 00    LDY #&00
0ae5 84 12    STY &12
0ae7 84 20    STY &20
0ae9 91 12    STA (&12),Y
0aeb a9 ff    LDA #&ff
0aed c8       INY     
0aee 91 12    STA (&12),Y
0af0 c8       INY     
0af1 84 12    STY &12
0af3 20 20 bd JSR &bd20
0af6 a0 07    LDY #&07
0af8 84 0c    STY &0c
0afa a0 00    LDY #&00
0afc 84 0b    STY &0b
0afe a9 33    LDA #&33
0b00 85 16    STA &16
0b02 a9 b4    LDA #&b4
0b04 85 17    STA &17
0b06 a9 3e    LDA #&3e
0b08 20 02 bc JSR &bc02
0b0b a9 33    LDA #&33
0b0d 85 16    STA &16
0b0f a9 b4    LDA #&b4
0b11 85 17    STA &17
0b13 a2 ff    LDX #&ff
0b15 86 28    STX &28
0b17 86 3c    STX &3c
0b19 9a       TXS     
0b1a 20 3a bd JSR &bd3a
0b1d a8       TAY     
0b1e a5 0b    LDA &0b
0b20 85 37    STA &37
0b22 a5 0c    LDA &0c
0b24 85 38    STA &38
0b26 84 3b    STY &3b
0b28 84 0a    STY &0a
0b2a 20 57 89 JSR &8957
0b2d 20 df 97 JSR &97df
0b30 90 06    BCC &0b38
0b32 20 8d bc JSR &bc8d
0b35 4c f3 8a JMP &8af3
0b38 20 97 8a JSR &8a97
0b3b c9 c6    CMP #&c6
0b3d b0 72    BCS &0bb1
0b3f 90 7e    BCC &0bbf
0b41 4c f6 8a JMP &8af6
0b44 4c 04 85 JMP &8504
0b47 ba       TSX     
0b48 e0 fc    CPX #&fc
0b4a b0 0d    BCS &0b59
0b4c ad ff 01 LDA &01ff
0b4f c9 a4    CMP #&a4
0b51 d0 06    BNE &0b59
0b53 20 1d 9b JSR &9b1d
0b56 4c 4c 98 JMP &984c
0b59 00       BRK     
0b5a 07       ???
0b5b 4e 6f 20 LSR &206f
0b5e a4 00    LDY &00
0b60 a4 0a    LDY &0a
0b62 88       DEY     
0b63 b1 0b    LDA (&0b),Y
0b65 c9 3d    CMP #&3d
0b67 f0 de    BEQ &0b47
0b69 c9 2a    CMP #&2a
0b6b f0 06    BEQ &0b73
0b6d c9 5b    CMP #&5b
0b6f f0 d3    BEQ &0b44
0b71 d0 23    BNE &0b96
0b73 20 6d 98 JSR &986d
0b76 a6 0b    LDX &0b
0b78 a4 0c    LDY &0c
0b7a 20 f7 ff JSR &fff7
0b7d a9 0d    LDA #&0d
0b7f a4 0a    LDY &0a
0b81 88       DEY     
0b82 c8       INY     
0b83 d1 0b    CMP (&0b),Y
0b85 d0 fb    BNE &0b82
0b87 c9 8b    CMP #&8b
0b89 f0 f2    BEQ &0b7d
0b8b a5 0c    LDA &0c
0b8d c9 07    CMP #&07
0b8f f0 b0    BEQ &0b41
0b91 20 90 98 JSR &9890
0b94 d0 0d    BNE &0ba3
0b96 c6 0a    DEC &0a
0b98 20 57 98 JSR &9857
0b9b a0 00    LDY #&00
0b9d b1 0b    LDA (&0b),Y
0b9f c9 3a    CMP #&3a
0ba1 d0 e4    BNE &0b87
0ba3 a4 0a    LDY &0a
0ba5 e6 0a    INC &0a
0ba7 b1 0b    LDA (&0b),Y
0ba9 c9 20    CMP #&20
0bab f0 f6    BEQ &0ba3
0bad c9 cf    CMP #&cf
0baf 90 0e    BCC &0bbf
0bb1 aa       TAX     
0bb2 bd df 82 LDA &82df,X
0bb5 85 37    STA &37
0bb7 bd 51 83 LDA &8351,X
0bba 85 38    STA &38
0bbc 6c 37 00 JMP (&0037)
0bbf a6 0b    LDX &0b
0bc1 86 19    STX &19
0bc3 a6 0c    LDX &0c
0bc5 86 1a    STX &1a
0bc7 84 1b    STY &1b
0bc9 20 dd 95 JSR &95dd
0bcc d0 1b    BNE &0be9
0bce b0 90    BCS &0b60
0bd0 86 1b    STX &1b
0bd2 20 41 98 JSR &9841
0bd5 20 fc 94 JSR &94fc
0bd8 a2 05    LDX #&05
0bda e4 2c    CPX &2c
0bdc d0 01    BNE &0bdf
0bde e8       INX     
0bdf 20 31 95 JSR &9531
0be2 c6 0a    DEC &0a
0be4 20 82 95 JSR &9582
0be7 f0 22    BEQ &0c0b
0be9 90 10    BCC &0bfb
0beb 20 94 bd JSR &bd94
0bee 20 13 98 JSR &9813
0bf1 a5 27    LDA &27
0bf3 d0 19    BNE &0c0e
0bf5 20 1e 8c JSR &8c1e
0bf8 4c 9b 8b JMP &8b9b
0bfb 20 94 bd JSR &bd94
0bfe 20 13 98 JSR &9813
0c01 a5 27    LDA &27
0c03 f0 09    BEQ &0c0e
0c05 20 b4 b4 JSR &b4b4
0c08 4c 9b 8b JMP &8b9b
0c0b 4c 2a 98 JMP &982a
0c0e 00       BRK     
0c0f 06 54    ASL &54
0c11 79 70 65 ADC &6570,Y
0c14 20 6d 69 JSR &696d
0c17 73       ???
0c18 6d 61 74 ADC &7461
0c1b 63       ???
0c1c 68       PLA     
0c1d 00       BRK     
0c1e 20 ea bd JSR &bdea
0c21 a5 2c    LDA &2c
0c23 c9 80    CMP #&80
0c25 f0 7b    BEQ &0ca2
0c27 a0 02    LDY #&02
0c29 b1 2a    LDA (&2a),Y
0c2b c5 36    CMP &36
0c2d b0 55    BCS &0c84
0c2f a5 02    LDA &02
0c31 85 2c    STA &2c
0c33 a5 03    LDA &03
0c35 85 2d    STA &2d
0c37 a5 36    LDA &36
0c39 c9 08    CMP #&08
0c3b 90 06    BCC &0c43
0c3d 69 07    ADC #&07
0c3f 90 02    BCC &0c43
0c41 a9 ff    LDA #&ff
0c43 18       CLC     
0c44 48       PHA     
0c45 aa       TAX     
0c46 b1 2a    LDA (&2a),Y
0c48 a0 00    LDY #&00
0c4a 71 2a    ADC (&2a),Y
0c4c 45 02    EOR &02
0c4e d0 0f    BNE &0c5f
0c50 c8       INY     
0c51 71 2a    ADC (&2a),Y
0c53 45 03    EOR &03
0c55 d0 08    BNE &0c5f
0c57 85 2d    STA &2d
0c59 8a       TXA     
0c5a c8       INY     
0c5b 38       SEC     
0c5c f1 2a    SBC (&2a),Y
0c5e aa       TAX     
0c5f 8a       TXA     
0c60 18       CLC     
0c61 65 02    ADC &02
0c63 a8       TAY     
0c64 a5 03    LDA &03
0c66 69 00    ADC #&00
0c68 c4 04    CPY &04
0c6a aa       TAX     
0c6b e5 05    SBC &05
0c6d b0 48    BCS &0cb7
0c6f 84 02    STY &02
0c71 86 03    STX &03
0c73 68       PLA     
0c74 a0 02    LDY #&02
0c76 91 2a    STA (&2a),Y
0c78 88       DEY     
0c79 a5 2d    LDA &2d
0c7b f0 07    BEQ &0c84
0c7d 91 2a    STA (&2a),Y
0c7f 88       DEY     
0c80 a5 2c    LDA &2c
0c82 91 2a    STA (&2a),Y
0c84 a0 03    LDY #&03
0c86 a5 36    LDA &36
0c88 91 2a    STA (&2a),Y
0c8a f0 15    BEQ &0ca1
0c8c 88       DEY     
0c8d 88       DEY     
0c8e b1 2a    LDA (&2a),Y
0c90 85 2d    STA &2d
0c92 88       DEY     
0c93 b1 2a    LDA (&2a),Y
0c95 85 2c    STA &2c
0c97 b9 00 06 LDA &0600,Y
0c9a 91 2c    STA (&2c),Y
0c9c c8       INY     
0c9d c4 36    CPY &36
0c9f d0 f6    BNE &0c97
0ca1 60       RTS     
0ca2 20 ba be JSR &beba
0ca5 c0 00    CPY #&00
0ca7 f0 0b    BEQ &0cb4
0ca9 b9 00 06 LDA &0600,Y
0cac 91 2a    STA (&2a),Y
0cae 88       DEY     
0caf d0 f8    BNE &0ca9
0cb1 ad 00 06 LDA &0600
0cb4 91 2a    STA (&2a),Y
0cb6 60       RTS     
0cb7 00       BRK     
0cb8 00       BRK     
0cb9 4e 6f 20 LSR &206f
0cbc 72       ???
0cbd 6f       ???
0cbe 6f       ???
0cbf 6d 00 a5 ADC &a500
0cc2 39 c9 80 AND &80c9,Y
0cc5 f0 27    BEQ &0cee
0cc7 90 3a    BCC &0d03
0cc9 a0 00    LDY #&00
0ccb b1 04    LDA (&04),Y
0ccd aa       TAX     
0cce f0 15    BEQ &0ce5
0cd0 b1 37    LDA (&37),Y
0cd2 e9 01    SBC #&01
0cd4 85 39    STA &39
0cd6 c8       INY     
0cd7 b1 37    LDA (&37),Y
0cd9 e9 00    SBC #&00
0cdb 85 3a    STA &3a
0cdd b1 04    LDA (&04),Y
0cdf 91 39    STA (&39),Y
0ce1 c8       INY     
0ce2 ca       DEX     
0ce3 d0 f8    BNE &0cdd
0ce5 a1 04    LDA (&04;X)
0ce7 a0 03    LDY #&03
0ce9 91 37    STA (&37),Y
0ceb 4c dc bd JMP &bddc
0cee a0 00    LDY #&00
0cf0 b1 04    LDA (&04),Y
0cf2 aa       TAX     
0cf3 f0 0a    BEQ &0cff
0cf5 c8       INY     
0cf6 b1 04    LDA (&04),Y
0cf8 88       DEY     
0cf9 91 37    STA (&37),Y
0cfb c8       INY     
0cfc ca       DEX     
0cfd d0 f6    BNE &0cf5
0cff a9 0d    LDA #&0d
0d01 d0 e6    BNE &0ce9
0d03 a0 00    LDY #&00
0d05 b1 04    LDA (&04),Y
0d07 91 37    STA (&37),Y
0d09 c8       INY     
0d0a c4 39    CPY &39
0d0c b0 18    BCS &0d26
0d0e b1 04    LDA (&04),Y
0d10 91 37    STA (&37),Y
0d12 c8       INY     
0d13 b1 04    LDA (&04),Y
0d15 91 37    STA (&37),Y
0d17 c8       INY     
0d18 b1 04    LDA (&04),Y
0d1a 91 37    STA (&37),Y
0d1c c8       INY     
0d1d c4 39    CPY &39
0d1f b0 05    BCS &0d26
0d21 b1 04    LDA (&04),Y
0d23 91 37    STA (&37),Y
0d25 c8       INY     
0d26 98       TYA     
0d27 18       CLC     
0d28 4c e1 bd JMP &bde1
0d2b c6 0a    DEC &0a
0d2d 20 a9 bf JSR &bfa9
0d30 98       TYA     
0d31 48       PHA     
0d32 20 8c 8a JSR &8a8c
0d35 c9 2c    CMP #&2c
0d37 d0 3e    BNE &0d77
0d39 20 29 9b JSR &9b29
0d3c 20 85 a3 JSR &a385
0d3f 68       PLA     
0d40 a8       TAY     
0d41 a5 27    LDA &27
0d43 20 d4 ff JSR &ffd4
0d46 aa       TAX     
0d47 f0 1b    BEQ &0d64
0d49 30 0c    BMI &0d57
0d4b a2 03    LDX #&03
0d4d b5 2a    LDA &2a,X
0d4f 20 d4 ff JSR &ffd4
0d52 ca       DEX     
0d53 10 f8    BPL &0d4d
0d55 30 d9    BMI &0d30
0d57 a2 04    LDX #&04
0d59 bd 6c 04 LDA &046c,X
0d5c 20 d4 ff JSR &ffd4
0d5f ca       DEX     
0d60 10 f7    BPL &0d59
0d62 30 cc    BMI &0d30
0d64 a5 36    LDA &36
0d66 20 d4 ff JSR &ffd4
0d69 aa       TAX     
0d6a f0 c4    BEQ &0d30
0d6c bd ff 05 LDA &05ff,X
0d6f 20 d4 ff JSR &ffd4
0d72 ca       DEX     
0d73 d0 f7    BNE &0d6c
0d75 f0 b9    BEQ &0d30
0d77 68       PLA     
0d78 84 0a    STY &0a
0d7a 4c 98 8b JMP &8b98
0d7d 20 25 bc JSR &bc25
0d80 4c 96 8b JMP &8b96
0d83 a9 00    LDA #&00
0d85 85 14    STA &14
0d87 85 15    STA &15
0d89 20 97 8a JSR &8a97
0d8c c9 3a    CMP #&3a
0d8e f0 f0    BEQ &0d80
0d90 c9 0d    CMP #&0d
0d92 f0 ec    BEQ &0d80
0d94 c9 8b    CMP #&8b
0d96 f0 e8    BEQ &0d80
0d98 d0 38    BNE &0dd2
0d9a 20 97 8a JSR &8a97
0d9d c9 23    CMP #&23
0d9f f0 8a    BEQ &0d2b
0da1 c6 0a    DEC &0a
0da3 4c bb 8d JMP &8dbb
0da6 ad 00 04 LDA &0400
0da9 f0 10    BEQ &0dbb
0dab a5 1e    LDA &1e
0dad f0 0c    BEQ &0dbb
0daf ed 00 04 SBC &0400
0db2 b0 f9    BCS &0dad
0db4 a8       TAY     
0db5 20 65 b5 JSR &b565
0db8 c8       INY     
0db9 d0 fa    BNE &0db5
0dbb 18       CLC     
0dbc ad 00 04 LDA &0400
0dbf 85 14    STA &14
0dc1 66 15    ROR &15
0dc3 20 97 8a JSR &8a97
0dc6 c9 3a    CMP #&3a
0dc8 f0 b3    BEQ &0d7d
0dca c9 0d    CMP #&0d
0dcc f0 af    BEQ &0d7d
0dce c9 8b    CMP #&8b
0dd0 f0 ab    BEQ &0d7d
0dd2 c9 7e    CMP #&7e
0dd4 f0 eb    BEQ &0dc1
0dd6 c9 2c    CMP #&2c
0dd8 f0 cc    BEQ &0da6
0dda c9 3b    CMP #&3b
0ddc f0 a5    BEQ &0d83
0dde 20 70 8e JSR &8e70
0de1 90 e0    BCC &0dc3
0de3 a5 14    LDA &14
0de5 48       PHA     
0de6 a5 15    LDA &15
0de8 48       PHA     
0de9 c6 1b    DEC &1b
0deb 20 29 9b JSR &9b29
0dee 68       PLA     
0def 85 15    STA &15
0df1 68       PLA     
0df2 85 14    STA &14
0df4 a5 1b    LDA &1b
0df6 85 0a    STA &0a
0df8 98       TYA     
0df9 f0 13    BEQ &0e0e
0dfb 20 df 9e JSR &9edf
0dfe a5 14    LDA &14
0e00 38       SEC     
0e01 e5 36    SBC &36
0e03 90 09    BCC &0e0e
0e05 f0 07    BEQ &0e0e
0e07 a8       TAY     
0e08 20 65 b5 JSR &b565
0e0b 88       DEY     
0e0c d0 fa    BNE &0e08
0e0e a5 36    LDA &36
0e10 f0 b1    BEQ &0dc3
0e12 a0 00    LDY #&00
0e14 b9 00 06 LDA &0600,Y
0e17 20 58 b5 JSR &b558
0e1a c8       INY     
0e1b c4 36    CPY &36
0e1d d0 f5    BNE &0e14
0e1f f0 a2    BEQ &0dc3
0e21 4c a2 8a JMP &8aa2
0e24 c9 2c    CMP #&2c
0e26 d0 f9    BNE &0e21
0e28 a5 2a    LDA &2a
0e2a 48       PHA     
0e2b 20 56 ae JSR &ae56
0e2e 20 f0 92 JSR &92f0
0e31 a9 1f    LDA #&1f
0e33 20 ee ff JSR &ffee
0e36 68       PLA     
0e37 20 ee ff JSR &ffee
0e3a 20 56 94 JSR &9456
0e3d 4c 6a 8e JMP &8e6a
0e40 20 dd 92 JSR &92dd
0e43 20 8c 8a JSR &8a8c
0e46 c9 29    CMP #&29
0e48 d0 da    BNE &0e24
0e4a a5 2a    LDA &2a
0e4c e5 1e    SBC &1e
0e4e f0 1a    BEQ &0e6a
0e50 a8       TAY     
0e51 b0 0c    BCS &0e5f
0e53 20 25 bc JSR &bc25
0e56 f0 03    BEQ &0e5b
0e58 20 e3 92 JSR &92e3
0e5b a4 2a    LDY &2a
0e5d f0 0b    BEQ &0e6a
0e5f 20 65 b5 JSR &b565
0e62 88       DEY     
0e63 d0 fa    BNE &0e5f
0e65 f0 03    BEQ &0e6a
0e67 20 25 bc JSR &bc25
0e6a 18       CLC     
0e6b a4 1b    LDY &1b
0e6d 84 0a    STY &0a
0e6f 60       RTS     
0e70 a6 0b    LDX &0b
0e72 86 19    STX &19
0e74 a6 0c    LDX &0c
0e76 86 1a    STX &1a
0e78 a6 0a    LDX &0a
0e7a 86 1b    STX &1b
0e7c c9 27    CMP #&27
0e7e f0 e7    BEQ &0e67
0e80 c9 8a    CMP #&8a
0e82 f0 bc    BEQ &0e40
0e84 c9 89    CMP #&89
0e86 f0 d0    BEQ &0e58
0e88 38       SEC     
0e89 60       RTS     
0e8a 20 97 8a JSR &8a97
0e8d 20 70 8e JSR &8e70
0e90 90 f7    BCC &0e89
0e92 c9 22    CMP #&22
0e94 f0 11    BEQ &0ea7
0e96 38       SEC     
0e97 60       RTS     
0e98 00       BRK     
0e99 09 4d    ORA #&4d
0e9b 69 73    ADC #&73
0e9d 73       ???
0e9e 69 6e    ADC #&6e
0ea0 67       ???
0ea1 20 22 00 JSR &0022
0ea4 20 58 b5 JSR &b558
0ea7 c8       INY     
0ea8 b1 19    LDA (&19),Y
0eaa c9 0d    CMP #&0d
0eac f0 ea    BEQ &0e98
0eae c9 22    CMP #&22
0eb0 d0 f2    BNE &0ea4
0eb2 c8       INY     
0eb3 84 1b    STY &1b
0eb5 b1 19    LDA (&19),Y
0eb7 c9 22    CMP #&22
0eb9 d0 af    BNE &0e6a
0ebb f0 e7    BEQ &0ea4
0ebd 20 57 98 JSR &9857
0ec0 a9 10    LDA #&10
0ec2 d0 08    BNE &0ecc
0ec4 20 57 98 JSR &9857
0ec7 20 28 bc JSR &bc28
0eca a9 0c    LDA #&0c
0ecc 20 ee ff JSR &ffee
0ecf 4c 9b 8b JMP &8b9b
0ed2 20 1d 9b JSR &9b1d
0ed5 20 ee 92 JSR &92ee
0ed8 20 94 bd JSR &bd94
0edb a0 00    LDY #&00
0edd 8c 00 06 STY &0600
0ee0 8c ff 06 STY &06ff
0ee3 20 8c 8a JSR &8a8c
0ee6 c9 2c    CMP #&2c
0ee8 d0 22    BNE &0f0c
0eea a4 1b    LDY &1b
0eec 20 d5 95 JSR &95d5
0eef f0 2a    BEQ &0f1b
0ef1 ac ff 06 LDY &06ff
0ef4 c8       INY     
0ef5 a5 2a    LDA &2a
0ef7 99 00 06 STA &0600,Y
0efa c8       INY     
0efb a5 2b    LDA &2b
0efd 99 00 06 STA &0600,Y
0f00 c8       INY     
0f01 a5 2c    LDA &2c
0f03 99 00 06 STA &0600,Y
0f06 ee 00 06 INC &0600
0f09 4c e0 8e JMP &8ee0
0f0c c6 1b    DEC &1b
0f0e 20 52 98 JSR &9852
0f11 20 ea bd JSR &bdea
0f14 20 1e 8f JSR &8f1e
0f17 d8       CLD     
0f18 4c 9b 8b JMP &8b9b
0f1b 4c 43 ae JMP &ae43
0f1e ad 0c 04 LDA &040c
0f21 4a       LSR A
0f22 ad 04 04 LDA &0404
0f25 ae 60 04 LDX &0460
0f28 ac 64 04 LDY &0464
0f2b 6c 2a 00 JMP (&002a)
0f2e 4c 2a 98 JMP &982a
0f31 20 df 97 JSR &97df
0f34 90 f8    BCC &0f2e
0f36 20 94 bd JSR &bd94
0f39 20 97 8a JSR &8a97
0f3c c9 2c    CMP #&2c
0f3e d0 ee    BNE &0f2e
0f40 20 df 97 JSR &97df
0f43 90 e9    BCC &0f2e
0f45 20 57 98 JSR &9857
0f48 a5 2a    LDA &2a
0f4a 85 39    STA &39
0f4c a5 2b    LDA &2b
0f4e 85 3a    STA &3a
0f50 20 ea bd JSR &bdea
0f53 20 2d bc JSR &bc2d
0f56 20 7b 98 JSR &987b
0f59 20 22 92 JSR &9222
0f5c a5 39    LDA &39
0f5e c5 2a    CMP &2a
0f60 a5 3a    LDA &3a
0f62 e5 2b    SBC &2b
0f64 b0 ed    BCS &0f53
0f66 4c f3 8a JMP &8af3
0f69 a9 0a    LDA #&0a
0f6b 20 d8 ae JSR &aed8
0f6e 20 df 97 JSR &97df
0f71 20 94 bd JSR &bd94
0f74 a9 0a    LDA #&0a
0f76 20 d8 ae JSR &aed8
0f79 20 97 8a JSR &8a97
0f7c c9 2c    CMP #&2c
0f7e d0 0d    BNE &0f8d
0f80 20 df 97 JSR &97df
0f83 a5 2b    LDA &2b
0f85 d0 58    BNE &0fdf
0f87 a5 2a    LDA &2a
0f89 f0 54    BEQ &0fdf
0f8b e6 0a    INC &0a
0f8d c6 0a    DEC &0a
0f8f 4c 57 98 JMP &9857
0f92 a5 12    LDA &12
0f94 85 3b    STA &3b
0f96 a5 13    LDA &13
0f98 85 3c    STA &3c
0f9a a5 18    LDA &18
0f9c 85 38    STA &38
0f9e a9 01    LDA #&01
0fa0 85 37    STA &37
0fa2 60       RTS     
0fa3 20 69 8f JSR &8f69
0fa6 a2 39    LDX #&39
0fa8 20 0d be JSR &be0d
0fab 20 6f be JSR &be6f
0fae 20 92 8f JSR &8f92
0fb1 a0 00    LDY #&00
0fb3 b1 37    LDA (&37),Y
0fb5 30 30    BMI &0fe7
0fb7 91 3b    STA (&3b),Y
0fb9 c8       INY     
0fba b1 37    LDA (&37),Y
0fbc 91 3b    STA (&3b),Y
0fbe 38       SEC     
0fbf 98       TYA     
0fc0 65 3b    ADC &3b
0fc2 85 3b    STA &3b
0fc4 aa       TAX     
0fc5 a5 3c    LDA &3c
0fc7 69 00    ADC #&00
0fc9 85 3c    STA &3c
0fcb e4 06    CPX &06
0fcd e5 07    SBC &07
0fcf b0 05    BCS &0fd6
0fd1 20 9f 90 JSR &909f
0fd4 90 db    BCC &0fb1
0fd6 00       BRK     
0fd7 00       BRK     
0fd8 cc 20 73 CPY &7320
0fdb 70 61    BVS &103e
0fdd 63       ???
0fde 65 00    ADC &00
0fe0 00       BRK     
0fe1 53       ???
0fe2 69 6c    ADC #&6c
0fe4 6c 79 00 JMP (&0079)
0fe7 20 9a 8f JSR &8f9a
0fea a0 00    LDY #&00
0fec b1 37    LDA (&37),Y
0fee 30 1d    BMI &100d
0ff0 a5 3a    LDA &3a
0ff2 91 37    STA (&37),Y
0ff4 a5 39    LDA &39
0ff6 c8       INY     
0ff7 91 37    STA (&37),Y
0ff9 18       CLC     
0ffa a5 2a    LDA &2a
0ffc 65 39    ADC &39
0ffe 85 39    STA &39
1000 a9 00    LDA #&00
1002 65 3a    ADC &3a
1004 29 7f    AND #&7f
1006 85 3a    STA &3a
1008 20 9f 90 JSR &909f
100b 90 dd    BCC &0fea
100d a5 18    LDA &18
100f 85 0c    STA &0c
1011 a0 00    LDY #&00
1013 84 0b    STY &0b
1015 c8       INY     
1016 b1 0b    LDA (&0b),Y
1018 30 20    BMI &103a
101a a0 04    LDY #&04
101c b1 0b    LDA (&0b),Y
101e c9 8d    CMP #&8d
1020 f0 1b    BEQ &103d
1022 c8       INY     
1023 c9 0d    CMP #&0d
1025 d0 f5    BNE &101c
1027 b1 0b    LDA (&0b),Y
1029 30 0f    BMI &103a
102b a0 03    LDY #&03
102d b1 0b    LDA (&0b),Y
102f 18       CLC     
1030 65 0b    ADC &0b
1032 85 0b    STA &0b
1034 90 e4    BCC &101a
1036 e6 0c    INC &0c
1038 b0 e0    BCS &101a
103a 4c f3 8a JMP &8af3
103d 20 eb 97 JSR &97eb
1040 20 92 8f JSR &8f92
1043 a0 00    LDY #&00
1045 b1 37    LDA (&37),Y
1047 30 37    BMI &1080
1049 b1 3b    LDA (&3b),Y
104b c8       INY     
104c c5 2b    CMP &2b
104e d0 21    BNE &1071
1050 b1 3b    LDA (&3b),Y
1052 c5 2a    CMP &2a
1054 d0 1b    BNE &1071
1056 b1 37    LDA (&37),Y
1058 85 3d    STA &3d
105a 88       DEY     
105b b1 37    LDA (&37),Y
105d 85 3e    STA &3e
105f a4 0a    LDY &0a
1061 88       DEY     
1062 a5 0b    LDA &0b
1064 85 37    STA &37
1066 a5 0c    LDA &0c
1068 85 38    STA &38
106a 20 f5 88 JSR &88f5
106d a4 0a    LDY &0a
106f d0 ab    BNE &101c
1071 20 9f 90 JSR &909f
1074 a5 3b    LDA &3b
1076 69 02    ADC #&02
1078 85 3b    STA &3b
107a 90 c7    BCC &1043
107c e6 3c    INC &3c
107e b0 c3    BCS &1043
1080 20 cf bf JSR &bfcf
1083 46 61    LSR &61
1085 69 6c    ADC #&6c
1087 65 64    ADC &64
1089 20 61 74 JSR &7461
108c 20 c8 b1 JSR &b1c8
108f 0b       ???
1090 85 2b    STA &2b
1092 c8       INY     
1093 b1 0b    LDA (&0b),Y
1095 85 2a    STA &2a
1097 20 1f 99 JSR &991f
109a 20 25 bc JSR &bc25
109d f0 ce    BEQ &106d
109f c8       INY     
10a0 b1 37    LDA (&37),Y
10a2 65 37    ADC &37
10a4 85 37    STA &37
10a6 90 03    BCC &10ab
10a8 e6 38    INC &38
10aa 18       CLC     
10ab 60       RTS     
10ac 20 69 8f JSR &8f69
10af a5 2a    LDA &2a
10b1 48       PHA     
10b2 20 ea bd JSR &bdea
10b5 20 94 bd JSR &bd94
10b8 20 23 99 JSR &9923
10bb a9 20    LDA #&20
10bd 20 02 bc JSR &bc02
10c0 20 ea bd JSR &bdea
10c3 20 51 89 JSR &8951
10c6 20 8d bc JSR &bc8d
10c9 20 20 bd JSR &bd20
10cc 68       PLA     
10cd 48       PHA     
10ce 18       CLC     
10cf 65 2a    ADC &2a
10d1 85 2a    STA &2a
10d3 90 e0    BCC &10b5
10d5 e6 2b    INC &2b
10d7 10 dc    BPL &10b5
10d9 4c f3 8a JMP &8af3
10dc 4c 18 92 JMP &9218
10df c6 0a    DEC &0a
10e1 20 82 95 JSR &9582
10e4 f0 41    BEQ &1127
10e6 b0 3f    BCS &1127
10e8 20 94 bd JSR &bd94
10eb 20 dd 92 JSR &92dd
10ee 20 22 92 JSR &9222
10f1 a5 2d    LDA &2d
10f3 05 2c    ORA &2c
10f5 d0 30    BNE &1127
10f7 18       CLC     
10f8 a5 2a    LDA &2a
10fa 65 02    ADC &02
10fc a8       TAY     
10fd a5 2b    LDA &2b
10ff 65 03    ADC &03
1101 aa       TAX     
1102 c4 04    CPY &04
1104 e5 05    SBC &05
1106 b0 d4    BCS &10dc
1108 a5 02    LDA &02
110a 85 2a    STA &2a
110c a5 03    LDA &03
110e 85 2b    STA &2b
1110 84 02    STY &02
1112 86 03    STX &03
1114 a9 00    LDA #&00
1116 85 2c    STA &2c
1118 85 2d    STA &2d
111a a9 40    LDA #&40
111c 85 27    STA &27
111e 20 b4 b4 JSR &b4b4
1121 20 27 88 JSR &8827
1124 4c 0b 92 JMP &920b
1127 00       BRK     
1128 0a       ASL A
1129 42       ???
112a 61 64    ADC (&64;X)
112c 20 de 00 JSR &00de
112f 20 97 8a JSR &8a97
1132 98       TYA     
1133 18       CLC     
1134 65 0b    ADC &0b
1136 a6 0c    LDX &0c
1138 90 02    BCC &113c
113a e8       INX     
113b 18       CLC     
113c e9 00    SBC #&00
113e 85 37    STA &37
1140 8a       TXA     
1141 e9 00    SBC #&00
1143 85 38    STA &38
1145 a2 05    LDX #&05
1147 86 3f    STX &3f
1149 a6 0a    LDX &0a
114b 20 59 95 JSR &9559
114e c0 01    CPY #&01
1150 f0 d5    BEQ &1127
1152 c9 28    CMP #&28
1154 f0 15    BEQ &116b
1156 c9 24    CMP #&24
1158 f0 04    BEQ &115e
115a c9 25    CMP #&25
115c d0 0a    BNE &1168
115e c6 3f    DEC &3f
1160 c8       INY     
1161 e8       INX     
1162 b1 37    LDA (&37),Y
1164 c9 28    CMP #&28
1166 f0 03    BEQ &116b
1168 4c df 90 JMP &90df
116b 84 39    STY &39
116d 86 0a    STX &0a
116f 20 69 94 JSR &9469
1172 d0 b3    BNE &1127
1174 20 fc 94 JSR &94fc
1177 a2 01    LDX #&01
1179 20 31 95 JSR &9531
117c a5 3f    LDA &3f
117e 48       PHA     
117f a9 01    LDA #&01
1181 48       PHA     
1182 20 d8 ae JSR &aed8
1185 20 94 bd JSR &bd94
1188 20 21 88 JSR &8821
118b a5 2b    LDA &2b
118d 29 c0    AND #&c0
118f 05 2c    ORA &2c
1191 05 2d    ORA &2d
1193 d0 92    BNE &1127
1195 20 22 92 JSR &9222
1198 68       PLA     
1199 a8       TAY     
119a a5 2a    LDA &2a
119c 91 02    STA (&02),Y
119e c8       INY     
119f a5 2b    LDA &2b
11a1 91 02    STA (&02),Y
11a3 c8       INY     
11a4 98       TYA     
11a5 48       PHA     
11a6 20 31 92 JSR &9231
11a9 20 97 8a JSR &8a97
11ac c9 2c    CMP #&2c
11ae f0 d5    BEQ &1185
11b0 c9 29    CMP #&29
11b2 f0 03    BEQ &11b7
11b4 4c 27 91 JMP &9127
11b7 68       PLA     
11b8 85 15    STA &15
11ba 68       PLA     
11bb 85 3f    STA &3f
11bd a9 00    LDA #&00
11bf 85 40    STA &40
11c1 20 36 92 JSR &9236
11c4 a0 00    LDY #&00
11c6 a5 15    LDA &15
11c8 91 02    STA (&02),Y
11ca 65 2a    ADC &2a
11cc 85 2a    STA &2a
11ce 90 02    BCC &11d2
11d0 e6 2b    INC &2b
11d2 a5 03    LDA &03
11d4 85 38    STA &38
11d6 a5 02    LDA &02
11d8 85 37    STA &37
11da 18       CLC     
11db 65 2a    ADC &2a
11dd a8       TAY     
11de a5 2b    LDA &2b
11e0 65 03    ADC &03
11e2 b0 34    BCS &1218
11e4 aa       TAX     
11e5 c4 04    CPY &04
11e7 e5 05    SBC &05
11e9 b0 2d    BCS &1218
11eb 84 02    STY &02
11ed 86 03    STX &03
11ef a5 37    LDA &37
11f1 65 15    ADC &15
11f3 a8       TAY     
11f4 a9 00    LDA #&00
11f6 85 37    STA &37
11f8 90 02    BCC &11fc
11fa e6 38    INC &38
11fc 91 37    STA (&37),Y
11fe c8       INY     
11ff d0 02    BNE &1203
1201 e6 38    INC &38
1203 c4 02    CPY &02
1205 d0 f5    BNE &11fc
1207 e4 38    CPX &38
1209 d0 f1    BNE &11fc
120b 20 97 8a JSR &8a97
120e c9 2c    CMP #&2c
1210 f0 03    BEQ &1215
1212 4c 96 8b JMP &8b96
1215 4c 2f 91 JMP &912f
1218 00       BRK     
1219 0b       ???
121a de 20 73 DEC &7320,X
121d 70 61    BVS &1280
121f 63       ???
1220 65 00    ADC &00
1222 e6 2a    INC &2a
1224 d0 0a    BNE &1230
1226 e6 2b    INC &2b
1228 d0 06    BNE &1230
122a e6 2c    INC &2c
122c d0 02    BNE &1230
122e e6 2d    INC &2d
1230 60       RTS     
1231 a2 3f    LDX #&3f
1233 20 0d be JSR &be0d
1236 a2 00    LDX #&00
1238 a0 00    LDY #&00
123a 46 40    LSR &40
123c 66 3f    ROR &3f
123e 90 0b    BCC &124b
1240 18       CLC     
1241 98       TYA     
1242 65 2a    ADC &2a
1244 a8       TAY     
1245 8a       TXA     
1246 65 2b    ADC &2b
1248 aa       TAX     
1249 b0 0f    BCS &125a
124b 06 2a    ASL &2a
124d 26 2b    ROL &2b
124f a5 3f    LDA &3f
1251 05 40    ORA &40
1253 d0 e5    BNE &123a
1255 84 2a    STY &2a
1257 86 2b    STX &2b
1259 60       RTS     
125a 4c 27 91 JMP &9127
125d 20 eb 92 JSR &92eb
1260 a5 2a    LDA &2a
1262 85 06    STA &06
1264 85 04    STA &04
1266 a5 2b    LDA &2b
1268 85 07    STA &07
126a 85 05    STA &05
126c 4c 9b 8b JMP &8b9b
126f 20 eb 92 JSR &92eb
1272 a5 2a    LDA &2a
1274 85 00    STA &00
1276 85 02    STA &02
1278 a5 2b    LDA &2b
127a 85 01    STA &01
127c 85 03    STA &03
127e 20 2f bd JSR &bd2f
1281 f0 07    BEQ &128a
1283 20 eb 92 JSR &92eb
1286 a5 2b    LDA &2b
1288 85 18    STA &18
128a 4c 9b 8b JMP &8b9b
128d 20 57 98 JSR &9857
1290 20 20 bd JSR &bd20
1293 f0 f5    BEQ &128a
1295 20 df 97 JSR &97df
1298 b0 0b    BCS &12a5
129a c9 ee    CMP #&ee
129c f0 19    BEQ &12b7
129e c9 87    CMP #&87
12a0 f0 1e    BEQ &12c0
12a2 20 21 88 JSR &8821
12a5 20 57 98 JSR &9857
12a8 a5 2a    LDA &2a
12aa 85 21    STA &21
12ac a5 2b    LDA &2b
12ae 85 22    STA &22
12b0 a9 ff    LDA #&ff
12b2 85 20    STA &20
12b4 4c 9b 8b JMP &8b9b
12b7 e6 0a    INC &0a
12b9 20 57 98 JSR &9857
12bc a9 ff    LDA #&ff
12be d0 ee    BNE &12ae
12c0 e6 0a    INC &0a
12c2 20 57 98 JSR &9857
12c5 a9 00    LDA #&00
12c7 f0 e9    BEQ &12b2
12c9 20 eb 92 JSR &92eb
12cc a2 2a    LDX #&2a
12ce a0 00    LDY #&00
12d0 84 2e    STY &2e
12d2 a9 02    LDA #&02
12d4 20 f1 ff JSR &fff1
12d7 4c 9b 8b JMP &8b9b
12da 20 ae 8a JSR &8aae
12dd 20 29 9b JSR &9b29
12e0 4c f0 92 JMP &92f0
12e3 20 ec ad JSR &adec
12e6 f0 0f    BEQ &12f7
12e8 30 0a    BMI &12f4
12ea 60       RTS     
12eb 20 07 98 JSR &9807
12ee a5 27    LDA &27
12f0 f0 05    BEQ &12f7
12f2 10 f6    BPL &12ea
12f4 4c e4 a3 JMP &a3e4
12f7 4c 0e 8c JMP &8c0e
12fa 20 ec ad JSR &adec
12fd f0 f8    BEQ &12f7
12ff 30 e9    BMI &12ea
1301 4c be a2 JMP &a2be
1304 a5 0b    LDA &0b
1306 85 19    STA &19
1308 a5 0c    LDA &0c
130a 85 1a    STA &1a
130c a5 0a    LDA &0a
130e 85 1b    STA &1b
1310 a9 f2    LDA #&f2
1312 20 97 b1 JSR &b197
1315 20 52 98 JSR &9852
1318 4c 9b 8b JMP &8b9b
131b a0 03    LDY #&03
131d a9 00    LDA #&00
131f 91 2a    STA (&2a),Y
1321 f0 1e    BEQ &1341
1323 ba       TSX     
1324 e0 fc    CPX #&fc
1326 b0 43    BCS &136b
1328 20 82 95 JSR &9582
132b f0 26    BEQ &1353
132d 20 0d b3 JSR &b30d
1330 a4 2c    LDY &2c
1332 30 e7    BMI &131b
1334 20 94 bd JSR &bd94
1337 a9 00    LDA #&00
1339 20 d8 ae JSR &aed8
133c 85 27    STA &27
133e 20 b4 b4 JSR &b4b4
1341 ba       TSX     
1342 fe 06 01 INC &0106,X
1345 a4 1b    LDY &1b
1347 84 0a    STY &0a
1349 20 97 8a JSR &8a97
134c c9 2c    CMP #&2c
134e f0 d3    BEQ &1323
1350 4c 96 8b JMP &8b96
1353 4c 98 8b JMP &8b98
1356 ba       TSX     
1357 e0 fc    CPX #&fc
1359 b0 0a    BCS &1365
135b ad ff 01 LDA &01ff
135e c9 f2    CMP #&f2
1360 d0 03    BNE &1365
1362 4c 57 98 JMP &9857
1365 00       BRK     
1366 0d 4e 6f ORA &6f4e
1369 20 f2 00 JSR &00f2
136c 0c       ???
136d 4e 6f 74 LSR &746f
1370 20 ea 00 JSR &00ea
1373 19 42 61 ORA &6142,Y
1376 64       ???
1377 20 eb 00 JSR &00eb
137a 20 21 88 JSR &8821
137d a5 2a    LDA &2a
137f 48       PHA     
1380 20 da 92 JSR &92da
1383 20 52 98 JSR &9852
1386 a9 12    LDA #&12
1388 20 ee ff JSR &ffee
138b 4c da 93 JMP &93da
138e a9 11    LDA #&11
1390 48       PHA     
1391 20 21 88 JSR &8821
1394 20 57 98 JSR &9857
1397 4c da 93 JMP &93da
139a a9 16    LDA #&16
139c 48       PHA     
139d 20 21 88 JSR &8821
13a0 20 57 98 JSR &9857
13a3 20 e7 be JSR &bee7
13a6 e0 ff    CPX #&ff
13a8 d0 2d    BNE &13d7
13aa c0 ff    CPY #&ff
13ac d0 29    BNE &13d7
13ae a5 04    LDA &04
13b0 c5 06    CMP &06
13b2 d0 be    BNE &1372
13b4 a5 05    LDA &05
13b6 c5 07    CMP &07
13b8 d0 b8    BNE &1372
13ba a6 2a    LDX &2a
13bc a9 85    LDA #&85
13be 20 f4 ff JSR &fff4
13c1 e4 02    CPX &02
13c3 98       TYA     
13c4 e5 03    SBC &03
13c6 90 aa    BCC &1372
13c8 e4 12    CPX &12
13ca 98       TYA     
13cb e5 13    SBC &13
13cd 90 a3    BCC &1372
13cf 86 06    STX &06
13d1 86 04    STX &04
13d3 84 07    STY &07
13d5 84 05    STY &05
13d7 20 28 bc JSR &bc28
13da 68       PLA     
13db 20 ee ff JSR &ffee
13de 20 56 94 JSR &9456
13e1 4c 9b 8b JMP &8b9b
13e4 a9 04    LDA #&04
13e6 d0 02    BNE &13ea
13e8 a9 05    LDA #&05
13ea 48       PHA     
13eb 20 1d 9b JSR &9b1d
13ee 4c fd 93 JMP &93fd
13f1 20 21 88 JSR &8821
13f4 a5 2a    LDA &2a
13f6 48       PHA     
13f7 20 ae 8a JSR &8aae
13fa 20 29 9b JSR &9b29
13fd 20 ee 92 JSR &92ee
1400 20 94 bd JSR &bd94
1403 20 da 92 JSR &92da
1406 20 52 98 JSR &9852
1409 a9 19    LDA #&19
140b 20 ee ff JSR &ffee
140e 68       PLA     
140f 20 ee ff JSR &ffee
1412 20 0b be JSR &be0b
1415 a5 37    LDA &37
1417 20 ee ff JSR &ffee
141a a5 38    LDA &38
141c 20 ee ff JSR &ffee
141f 20 56 94 JSR &9456
1422 a5 2b    LDA &2b
1424 20 ee ff JSR &ffee
1427 4c 9b 8b JMP &8b9b
142a a5 2b    LDA &2b
142c 20 ee ff JSR &ffee
142f 20 97 8a JSR &8a97
1432 c9 3a    CMP #&3a
1434 f0 1d    BEQ &1453
1436 c9 0d    CMP #&0d
1438 f0 19    BEQ &1453
143a c9 8b    CMP #&8b
143c f0 15    BEQ &1453
143e c6 0a    DEC &0a
1440 20 21 88 JSR &8821
1443 20 56 94 JSR &9456
1446 20 97 8a JSR &8a97
1449 c9 2c    CMP #&2c
144b f0 e2    BEQ &142f
144d c9 3b    CMP #&3b
144f d0 e1    BNE &1432
1451 f0 d7    BEQ &142a
1453 4c 96 8b JMP &8b96
1456 a5 2a    LDA &2a
1458 6c 0e 02 JMP (&020e)
145b a0 01    LDY #&01
145d b1 37    LDA (&37),Y
145f a0 f6    LDY #&f6
1461 c9 f2    CMP #&f2
1463 f0 0a    BEQ &146f
1465 a0 f8    LDY #&f8
1467 d0 06    BNE &146f
1469 a0 01    LDY #&01
146b b1 37    LDA (&37),Y
146d 0a       ASL A
146e a8       TAY     
146f b9 00 04 LDA &0400,Y
1472 85 3a    STA &3a
1474 b9 01 04 LDA &0401,Y
1477 85 3b    STA &3b
1479 a5 3b    LDA &3b
147b f0 35    BEQ &14b2
147d a0 00    LDY #&00
147f b1 3a    LDA (&3a),Y
1481 85 3c    STA &3c
1483 c8       INY     
1484 b1 3a    LDA (&3a),Y
1486 85 3d    STA &3d
1488 c8       INY     
1489 b1 3a    LDA (&3a),Y
148b d0 0d    BNE &149a
148d 88       DEY     
148e c4 39    CPY &39
1490 d0 21    BNE &14b3
1492 c8       INY     
1493 b0 12    BCS &14a7
1495 c8       INY     
1496 b1 3a    LDA (&3a),Y
1498 f0 19    BEQ &14b3
149a d1 37    CMP (&37),Y
149c d0 15    BNE &14b3
149e c4 39    CPY &39
14a0 d0 f3    BNE &1495
14a2 c8       INY     
14a3 b1 3a    LDA (&3a),Y
14a5 d0 0c    BNE &14b3
14a7 98       TYA     
14a8 65 3a    ADC &3a
14aa 85 2a    STA &2a
14ac a5 3b    LDA &3b
14ae 69 00    ADC #&00
14b0 85 2b    STA &2b
14b2 60       RTS     
14b3 a5 3d    LDA &3d
14b5 f0 fb    BEQ &14b2
14b7 a0 00    LDY #&00
14b9 b1 3c    LDA (&3c),Y
14bb 85 3a    STA &3a
14bd c8       INY     
14be b1 3c    LDA (&3c),Y
14c0 85 3b    STA &3b
14c2 c8       INY     
14c3 b1 3c    LDA (&3c),Y
14c5 d0 0d    BNE &14d4
14c7 88       DEY     
14c8 c4 39    CPY &39
14ca d0 ad    BNE &1479
14cc c8       INY     
14cd b0 12    BCS &14e1
14cf c8       INY     
14d0 b1 3c    LDA (&3c),Y
14d2 f0 a5    BEQ &1479
14d4 d1 37    CMP (&37),Y
14d6 d0 a1    BNE &1479
14d8 c4 39    CPY &39
14da d0 f3    BNE &14cf
14dc c8       INY     
14dd b1 3c    LDA (&3c),Y
14df d0 98    BNE &1479
14e1 98       TYA     
14e2 65 3c    ADC &3c
14e4 85 2a    STA &2a
14e6 a5 3d    LDA &3d
14e8 69 00    ADC #&00
14ea 85 2b    STA &2b
14ec 60       RTS     
14ed a0 01    LDY #&01
14ef b1 37    LDA (&37),Y
14f1 aa       TAX     
14f2 a9 f6    LDA #&f6
14f4 e0 f2    CPX #&f2
14f6 f0 09    BEQ &1501
14f8 a9 f8    LDA #&f8
14fa d0 05    BNE &1501
14fc a0 01    LDY #&01
14fe b1 37    LDA (&37),Y
1500 0a       ASL A
1501 85 3a    STA &3a
1503 a9 04    LDA #&04
1505 85 3b    STA &3b
1507 b1 3a    LDA (&3a),Y
1509 f0 0b    BEQ &1516
150b aa       TAX     
150c 88       DEY     
150d b1 3a    LDA (&3a),Y
150f 85 3a    STA &3a
1511 86 3b    STX &3b
1513 c8       INY     
1514 10 f1    BPL &1507
1516 a5 03    LDA &03
1518 91 3a    STA (&3a),Y
151a a5 02    LDA &02
151c 88       DEY     
151d 91 3a    STA (&3a),Y
151f 98       TYA     
1520 c8       INY     
1521 91 02    STA (&02),Y
1523 c4 39    CPY &39
1525 f0 31    BEQ &1558
1527 c8       INY     
1528 b1 37    LDA (&37),Y
152a 91 02    STA (&02),Y
152c c4 39    CPY &39
152e d0 f7    BNE &1527
1530 60       RTS     
1531 a9 00    LDA #&00
1533 c8       INY     
1534 91 02    STA (&02),Y
1536 ca       DEX     
1537 d0 fa    BNE &1533
1539 38       SEC     
153a 98       TYA     
153b 65 02    ADC &02
153d 90 02    BCC &1541
153f e6 03    INC &03
1541 a4 03    LDY &03
1543 c4 05    CPY &05
1545 90 0f    BCC &1556
1547 d0 04    BNE &154d
1549 c5 04    CMP &04
154b 90 09    BCC &1556
154d a9 00    LDA #&00
154f a0 01    LDY #&01
1551 91 3a    STA (&3a),Y
1553 4c b7 8c JMP &8cb7
1556 85 02    STA &02
1558 60       RTS     
1559 a0 01    LDY #&01
155b b1 37    LDA (&37),Y
155d c9 30    CMP #&30
155f 90 18    BCC &1579
1561 c9 40    CMP #&40
1563 b0 0c    BCS &1571
1565 c9 3a    CMP #&3a
1567 b0 10    BCS &1579
1569 c0 01    CPY #&01
156b f0 0c    BEQ &1579
156d e8       INX     
156e c8       INY     
156f d0 ea    BNE &155b
1571 c9 5f    CMP #&5f
1573 b0 05    BCS &157a
1575 c9 5b    CMP #&5b
1577 90 f4    BCC &156d
1579 60       RTS     
157a c9 7b    CMP #&7b
157c 90 ef    BCC &156d
157e 60       RTS     
157f 20 31 95 JSR &9531
1582 20 c9 95 JSR &95c9
1585 d0 1d    BNE &15a4
1587 b0 1b    BCS &15a4
1589 20 fc 94 JSR &94fc
158c a2 05    LDX #&05
158e e4 2c    CPX &2c
1590 d0 ed    BNE &157f
1592 e8       INX     
1593 d0 ea    BNE &157f
1595 c9 21    CMP #&21
1597 f0 0c    BEQ &15a5
1599 c9 24    CMP #&24
159b f0 13    BEQ &15b0
159d 49 3f    EOR #&3f
159f f0 06    BEQ &15a7
15a1 a9 00    LDA #&00
15a3 38       SEC     
15a4 60       RTS     
15a5 a9 04    LDA #&04
15a7 48       PHA     
15a8 e6 1b    INC &1b
15aa 20 e3 92 JSR &92e3
15ad 4c 9f 96 JMP &969f
15b0 e6 1b    INC &1b
15b2 20 e3 92 JSR &92e3
15b5 a5 2b    LDA &2b
15b7 f0 06    BEQ &15bf
15b9 a9 80    LDA #&80
15bb 85 2c    STA &2c
15bd 38       SEC     
15be 60       RTS     
15bf 00       BRK     
15c0 08       PHP     
15c1 24 20    BIT &20
15c3 72       ???
15c4 61 6e    ADC (&6e;X)
15c6 67       ???
15c7 65 00    ADC &00
15c9 a5 0b    LDA &0b
15cb 85 19    STA &19
15cd a5 0c    LDA &0c
15cf 85 1a    STA &1a
15d1 a4 0a    LDY &0a
15d3 88       DEY     
15d4 c8       INY     
15d5 84 1b    STY &1b
15d7 b1 19    LDA (&19),Y
15d9 c9 20    CMP #&20
15db f0 f7    BEQ &15d4
15dd c9 40    CMP #&40
15df 90 b4    BCC &1595
15e1 c9 5b    CMP #&5b
15e3 b0 1a    BCS &15ff
15e5 0a       ASL A
15e6 0a       ASL A
15e7 85 2a    STA &2a
15e9 a9 04    LDA #&04
15eb 85 2b    STA &2b
15ed c8       INY     
15ee b1 19    LDA (&19),Y
15f0 c8       INY     
15f1 c9 25    CMP #&25
15f3 d0 0a    BNE &15ff
15f5 a2 04    LDX #&04
15f7 86 2c    STX &2c
15f9 b1 19    LDA (&19),Y
15fb c9 28    CMP #&28
15fd d0 66    BNE &1665
15ff a2 05    LDX #&05
1601 86 2c    STX &2c
1603 a5 1b    LDA &1b
1605 18       CLC     
1606 65 19    ADC &19
1608 a6 1a    LDX &1a
160a 90 02    BCC &160e
160c e8       INX     
160d 18       CLC     
160e e9 00    SBC #&00
1610 85 37    STA &37
1612 b0 01    BCS &1615
1614 ca       DEX     
1615 86 38    STX &38
1617 a6 1b    LDX &1b
1619 a0 01    LDY #&01
161b b1 37    LDA (&37),Y
161d c9 41    CMP #&41
161f b0 0c    BCS &162d
1621 c9 30    CMP #&30
1623 90 1c    BCC &1641
1625 c9 3a    CMP #&3a
1627 b0 18    BCS &1641
1629 e8       INX     
162a c8       INY     
162b d0 ee    BNE &161b
162d c9 5b    CMP #&5b
162f b0 04    BCS &1635
1631 e8       INX     
1632 c8       INY     
1633 d0 e6    BNE &161b
1635 c9 5f    CMP #&5f
1637 90 08    BCC &1641
1639 c9 7b    CMP #&7b
163b b0 04    BCS &1641
163d e8       INX     
163e c8       INY     
163f d0 da    BNE &161b
1641 88       DEY     
1642 f0 2f    BEQ &1673
1644 c9 24    CMP #&24
1646 f0 67    BEQ &16af
1648 c9 25    CMP #&25
164a d0 08    BNE &1654
164c c6 2c    DEC &2c
164e c8       INY     
164f e8       INX     
1650 c8       INY     
1651 b1 37    LDA (&37),Y
1653 88       DEY     
1654 84 39    STY &39
1656 c9 28    CMP #&28
1658 f0 4c    BEQ &16a6
165a 20 69 94 JSR &9469
165d f0 18    BEQ &1677
165f 86 1b    STX &1b
1661 a4 1b    LDY &1b
1663 b1 19    LDA (&19),Y
1665 c9 21    CMP #&21
1667 f0 16    BEQ &167f
1669 c9 3f    CMP #&3f
166b f0 0e    BEQ &167b
166d 18       CLC     
166e 84 1b    STY &1b
1670 a9 ff    LDA #&ff
1672 60       RTS     
1673 a9 00    LDA #&00
1675 38       SEC     
1676 60       RTS     
1677 a9 00    LDA #&00
1679 18       CLC     
167a 60       RTS     
167b a9 00    LDA #&00
167d f0 02    BEQ &1681
167f a9 04    LDA #&04
1681 48       PHA     
1682 c8       INY     
1683 84 1b    STY &1b
1685 20 2c b3 JSR &b32c
1688 20 f0 92 JSR &92f0
168b a5 2b    LDA &2b
168d 48       PHA     
168e a5 2a    LDA &2a
1690 48       PHA     
1691 20 e3 92 JSR &92e3
1694 18       CLC     
1695 68       PLA     
1696 65 2a    ADC &2a
1698 85 2a    STA &2a
169a 68       PLA     
169b 65 2b    ADC &2b
169d 85 2b    STA &2b
169f 68       PLA     
16a0 85 2c    STA &2c
16a2 18       CLC     
16a3 a9 ff    LDA #&ff
16a5 60       RTS     
16a6 e8       INX     
16a7 e6 39    INC &39
16a9 20 df 96 JSR &96df
16ac 4c 61 96 JMP &9661
16af e8       INX     
16b0 c8       INY     
16b1 84 39    STY &39
16b3 c8       INY     
16b4 c6 2c    DEC &2c
16b6 b1 37    LDA (&37),Y
16b8 c9 28    CMP #&28
16ba f0 0d    BEQ &16c9
16bc 20 69 94 JSR &9469
16bf f0 b6    BEQ &1677
16c1 86 1b    STX &1b
16c3 a9 81    LDA #&81
16c5 85 2c    STA &2c
16c7 38       SEC     
16c8 60       RTS     
16c9 e8       INX     
16ca 84 39    STY &39
16cc c6 2c    DEC &2c
16ce 20 df 96 JSR &96df
16d1 a9 81    LDA #&81
16d3 85 2c    STA &2c
16d5 38       SEC     
16d6 60       RTS     
16d7 00       BRK     
16d8 0e 41 72 ASL &7241
16db 72       ???
16dc 61 79    ADC (&79;X)
16de 00       BRK     
16df 20 69 94 JSR &9469
16e2 f0 f3    BEQ &16d7
16e4 86 1b    STX &1b
16e6 a5 2c    LDA &2c
16e8 48       PHA     
16e9 a5 2a    LDA &2a
16eb 48       PHA     
16ec a5 2b    LDA &2b
16ee 48       PHA     
16ef a0 00    LDY #&00
16f1 b1 2a    LDA (&2a),Y
16f3 c9 04    CMP #&04
16f5 90 75    BCC &176c
16f7 98       TYA     
16f8 20 d8 ae JSR &aed8
16fb a9 01    LDA #&01
16fd 85 2d    STA &2d
16ff 20 94 bd JSR &bd94
1702 20 dd 92 JSR &92dd
1705 e6 1b    INC &1b
1707 e0 2c    CPX #&2c
1709 d0 cc    BNE &16d7
170b a2 39    LDX #&39
170d 20 0d be JSR &be0d
1710 a4 3c    LDY &3c
1712 68       PLA     
1713 85 38    STA &38
1715 68       PLA     
1716 85 37    STA &37
1718 48       PHA     
1719 a5 38    LDA &38
171b 48       PHA     
171c 20 ba 97 JSR &97ba
171f 84 2d    STY &2d
1721 b1 37    LDA (&37),Y
1723 85 3f    STA &3f
1725 c8       INY     
1726 b1 37    LDA (&37),Y
1728 85 40    STA &40
172a a5 2a    LDA &2a
172c 65 39    ADC &39
172e 85 2a    STA &2a
1730 a5 2b    LDA &2b
1732 65 3a    ADC &3a
1734 85 2b    STA &2b
1736 20 36 92 JSR &9236
1739 a0 00    LDY #&00
173b 38       SEC     
173c b1 37    LDA (&37),Y
173e e5 2d    SBC &2d
1740 c9 03    CMP #&03
1742 b0 bb    BCS &16ff
1744 20 94 bd JSR &bd94
1747 20 56 ae JSR &ae56
174a 20 f0 92 JSR &92f0
174d 68       PLA     
174e 85 38    STA &38
1750 68       PLA     
1751 85 37    STA &37
1753 a2 39    LDX #&39
1755 20 0d be JSR &be0d
1758 a4 3c    LDY &3c
175a 20 ba 97 JSR &97ba
175d 18       CLC     
175e a5 39    LDA &39
1760 65 2a    ADC &2a
1762 85 2a    STA &2a
1764 a5 3a    LDA &3a
1766 65 2b    ADC &2b
1768 85 2b    STA &2b
176a 90 11    BCC &177d
176c 20 56 ae JSR &ae56
176f 20 f0 92 JSR &92f0
1772 68       PLA     
1773 85 38    STA &38
1775 68       PLA     
1776 85 37    STA &37
1778 a0 01    LDY #&01
177a 20 ba 97 JSR &97ba
177d 68       PLA     
177e 85 2c    STA &2c
1780 c9 05    CMP #&05
1782 d0 17    BNE &179b
1784 a6 2b    LDX &2b
1786 a5 2a    LDA &2a
1788 06 2a    ASL &2a
178a 26 2b    ROL &2b
178c 06 2a    ASL &2a
178e 26 2b    ROL &2b
1790 65 2a    ADC &2a
1792 85 2a    STA &2a
1794 8a       TXA     
1795 65 2b    ADC &2b
1797 85 2b    STA &2b
1799 90 08    BCC &17a3
179b 06 2a    ASL &2a
179d 26 2b    ROL &2b
179f 06 2a    ASL &2a
17a1 26 2b    ROL &2b
17a3 98       TYA     
17a4 65 2a    ADC &2a
17a6 85 2a    STA &2a
17a8 90 03    BCC &17ad
17aa e6 2b    INC &2b
17ac 18       CLC     
17ad a5 37    LDA &37
17af 65 2a    ADC &2a
17b1 85 2a    STA &2a
17b3 a5 38    LDA &38
17b5 65 2b    ADC &2b
17b7 85 2b    STA &2b
17b9 60       RTS     
17ba a5 2b    LDA &2b
17bc 29 c0    AND #&c0
17be 05 2c    ORA &2c
17c0 05 2d    ORA &2d
17c2 d0 0d    BNE &17d1
17c4 a5 2a    LDA &2a
17c6 d1 37    CMP (&37),Y
17c8 c8       INY     
17c9 a5 2b    LDA &2b
17cb f1 37    SBC (&37),Y
17cd b0 02    BCS &17d1
17cf c8       INY     
17d0 60       RTS     
17d1 00       BRK     
17d2 0f       ???
17d3 53       ???
17d4 75 62    ADC &62,X
17d6 73       ???
17d7 63       ???
17d8 72       ???
17d9 69 70    ADC #&70
17db 74       ???
17dc 00       BRK     
17dd e6 0a    INC &0a
17df a4 0a    LDY &0a
17e1 b1 0b    LDA (&0b),Y
17e3 c9 20    CMP #&20
17e5 f0 f6    BEQ &17dd
17e7 c9 8d    CMP #&8d
17e9 d0 1a    BNE &1805
17eb c8       INY     
17ec b1 0b    LDA (&0b),Y
17ee 0a       ASL A
17ef 0a       ASL A
17f0 aa       TAX     
17f1 29 c0    AND #&c0
17f3 c8       INY     
17f4 51 0b    EOR (&0b),Y
17f6 85 2a    STA &2a
17f8 8a       TXA     
17f9 0a       ASL A
17fa 0a       ASL A
17fb c8       INY     
17fc 51 0b    EOR (&0b),Y
17fe 85 2b    STA &2b
1800 c8       INY     
1801 84 0a    STY &0a
1803 38       SEC     
1804 60       RTS     
1805 18       CLC     
1806 60       RTS     
1807 a5 0b    LDA &0b
1809 85 19    STA &19
180b a5 0c    LDA &0c
180d 85 1a    STA &1a
180f a5 0a    LDA &0a
1811 85 1b    STA &1b
1813 a4 1b    LDY &1b
1815 e6 1b    INC &1b
1817 b1 19    LDA (&19),Y
1819 c9 20    CMP #&20
181b f0 f6    BEQ &1813
181d c9 3d    CMP #&3d
181f f0 28    BEQ &1849
1821 00       BRK     
1822 04       ???
1823 4d 69 73 EOR &7369
1826 74       ???
1827 61 6b    ADC (&6b;X)
1829 65 00    ADC &00
182b 10 53    BPL &1880
182d 79 6e 74 ADC &746e,Y
1830 61 78    ADC (&78;X)
1832 20 65 72 JSR &7265
1835 72       ???
1836 6f       ???
1837 72       ???
1838 00       BRK     
1839 11 45    ORA (&45),Y
183b 73       ???
183c 63       ???
183d 61 70    ADC (&70;X)
183f 65 00    ADC &00
1841 20 8c 8a JSR &8a8c
1844 c9 3d    CMP #&3d
1846 d0 d9    BNE &1821
1848 60       RTS     
1849 20 29 9b JSR &9b29
184c 8a       TXA     
184d a4 1b    LDY &1b
184f 4c 61 98 JMP &9861
1852 a4 1b    LDY &1b
1854 4c 59 98 JMP &9859
1857 a4 0a    LDY &0a
1859 88       DEY     
185a c8       INY     
185b b1 0b    LDA (&0b),Y
185d c9 20    CMP #&20
185f f0 f9    BEQ &185a
1861 c9 3a    CMP #&3a
1863 f0 08    BEQ &186d
1865 c9 0d    CMP #&0d
1867 f0 04    BEQ &186d
1869 c9 8b    CMP #&8b
186b d0 bd    BNE &182a
186d 18       CLC     
186e 98       TYA     
186f 65 0b    ADC &0b
1871 85 0b    STA &0b
1873 90 02    BCC &1877
1875 e6 0c    INC &0c
1877 a0 01    LDY #&01
1879 84 0a    STY &0a
187b 24 ff    BIT &ff
187d 30 b9    BMI &1838
187f 60       RTS     
1880 20 57 98 JSR &9857
1883 88       DEY     
1884 b1 0b    LDA (&0b),Y
1886 c9 3a    CMP #&3a
1888 f0 f5    BEQ &187f
188a a5 0c    LDA &0c
188c c9 07    CMP #&07
188e f0 2c    BEQ &18bc
1890 c8       INY     
1891 b1 0b    LDA (&0b),Y
1893 30 27    BMI &18bc
1895 a5 20    LDA &20
1897 f0 13    BEQ &18ac
1899 98       TYA     
189a 48       PHA     
189b c8       INY     
189c b1 0b    LDA (&0b),Y
189e 48       PHA     
189f 88       DEY     
18a0 b1 0b    LDA (&0b),Y
18a2 a8       TAY     
18a3 68       PLA     
18a4 20 ea ae JSR &aeea
18a7 20 05 99 JSR &9905
18aa 68       PLA     
18ab a8       TAY     
18ac c8       INY     
18ad 38       SEC     
18ae 98       TYA     
18af 65 0b    ADC &0b
18b1 85 0b    STA &0b
18b3 90 02    BCC &18b7
18b5 e6 0c    INC &0c
18b7 a0 01    LDY #&01
18b9 84 0a    STY &0a
18bb 60       RTS     
18bc 4c f6 8a JMP &8af6
18bf 4c 0e 8c JMP &8c0e
18c2 20 1d 9b JSR &9b1d
18c5 f0 f8    BEQ &18bf
18c7 10 03    BPL &18cc
18c9 20 e4 a3 JSR &a3e4
18cc a4 1b    LDY &1b
18ce 84 0a    STY &0a
18d0 a5 2a    LDA &2a
18d2 05 2b    ORA &2b
18d4 05 2c    ORA &2c
18d6 05 2d    ORA &2d
18d8 f0 17    BEQ &18f1
18da e0 8c    CPX #&8c
18dc f0 03    BEQ &18e1
18de 4c a3 8b JMP &8ba3
18e1 e6 0a    INC &0a
18e3 20 df 97 JSR &97df
18e6 90 f6    BCC &18de
18e8 20 af b9 JSR &b9af
18eb 20 77 98 JSR &9877
18ee 4c d2 b8 JMP &b8d2
18f1 a4 0a    LDY &0a
18f3 b1 0b    LDA (&0b),Y
18f5 c9 0d    CMP #&0d
18f7 f0 09    BEQ &1902
18f9 c8       INY     
18fa c9 8b    CMP #&8b
18fc d0 f5    BNE &18f3
18fe 84 0a    STY &0a
1900 f0 e1    BEQ &18e3
1902 4c 87 8b JMP &8b87
1905 a5 2a    LDA &2a
1907 c5 21    CMP &21
1909 a5 2b    LDA &2b
190b e5 22    SBC &22
190d b0 ac    BCS &18bb
190f a9 5b    LDA #&5b
1911 20 58 b5 JSR &b558
1914 20 1f 99 JSR &991f
1917 a9 5d    LDA #&5d
1919 20 58 b5 JSR &b558
191c 4c 65 b5 JMP &b565
191f a9 00    LDA #&00
1921 f0 02    BEQ &1925
1923 a9 05    LDA #&05
1925 85 14    STA &14
1927 a2 04    LDX #&04
1929 a9 00    LDA #&00
192b 95 3f    STA &3f,X
192d 38       SEC     
192e a5 2a    LDA &2a
1930 fd 6b 99 SBC &996b,X
1933 a8       TAY     
1934 a5 2b    LDA &2b
1936 fd b9 99 SBC &99b9,X
1939 90 08    BCC &1943
193b 85 2b    STA &2b
193d 84 2a    STY &2a
193f f6 3f    INC &3f,X
1941 d0 eb    BNE &192e
1943 ca       DEX     
1944 10 e3    BPL &1929
1946 a2 05    LDX #&05
1948 ca       DEX     
1949 f0 04    BEQ &194f
194b b5 3f    LDA &3f,X
194d f0 f9    BEQ &1948
194f 86 37    STX &37
1951 a5 14    LDA &14
1953 f0 0b    BEQ &1960
1955 e5 37    SBC &37
1957 f0 07    BEQ &1960
1959 a8       TAY     
195a 20 65 b5 JSR &b565
195d 88       DEY     
195e d0 fa    BNE &195a
1960 b5 3f    LDA &3f,X
1962 09 30    ORA #&30
1964 20 58 b5 JSR &b558
1967 ca       DEX     
1968 10 f6    BPL &1960
196a 60       RTS     
196b 01 0a    ORA (&0a;X)
196d 64       ???
196e e8       INX     
196f 10 a0    BPL &1911
1971 00       BRK     
1972 84 3d    STY &3d
1974 a5 18    LDA &18
1976 85 3e    STA &3e
1978 a0 01    LDY #&01
197a b1 3d    LDA (&3d),Y
197c c5 2b    CMP &2b
197e b0 0e    BCS &198e
1980 a0 03    LDY #&03
1982 b1 3d    LDA (&3d),Y
1984 65 3d    ADC &3d
1986 85 3d    STA &3d
1988 90 ee    BCC &1978
198a e6 3e    INC &3e
198c b0 ea    BCS &1978
198e d0 14    BNE &19a4
1990 a0 02    LDY #&02
1992 b1 3d    LDA (&3d),Y
1994 c5 2a    CMP &2a
1996 90 e8    BCC &1980
1998 d0 0a    BNE &19a4
199a 98       TYA     
199b 65 3d    ADC &3d
199d 85 3d    STA &3d
199f 90 03    BCC &19a4
19a1 e6 3e    INC &3e
19a3 18       CLC     
19a4 a0 02    LDY #&02
19a6 60       RTS     
19a7 00       BRK     
19a8 12       ???
19a9 44       ???
19aa 69 76    ADC #&76
19ac 69 73    ADC #&73
19ae 69 6f    ADC #&6f
19b0 6e 20 62 ROR &6220
19b3 79 20 7a ADC &7a20,Y
19b6 65 72    ADC &72
19b8 6f       ???
19b9 00       BRK     
19ba 00       BRK     
19bb 00       BRK     
19bc 03       ???
19bd 27       ???
19be a8       TAY     
19bf 20 f0 92 JSR &92f0
19c2 a5 2d    LDA &2d
19c4 48       PHA     
19c5 20 71 ad JSR &ad71
19c8 20 1d 9e JSR &9e1d
19cb 86 27    STX &27
19cd a8       TAY     
19ce 20 f0 92 JSR &92f0
19d1 68       PLA     
19d2 85 38    STA &38
19d4 45 2d    EOR &2d
19d6 85 37    STA &37
19d8 20 71 ad JSR &ad71
19db a2 39    LDX #&39
19dd 20 0d be JSR &be0d
19e0 84 3d    STY &3d
19e2 84 3e    STY &3e
19e4 84 3f    STY &3f
19e6 84 40    STY &40
19e8 a5 2d    LDA &2d
19ea 05 2a    ORA &2a
19ec 05 2b    ORA &2b
19ee 05 2c    ORA &2c
19f0 f0 b5    BEQ &19a7
19f2 a0 20    LDY #&20
19f4 88       DEY     
19f5 f0 41    BEQ &1a38
19f7 06 39    ASL &39
19f9 26 3a    ROL &3a
19fb 26 3b    ROL &3b
19fd 26 3c    ROL &3c
19ff 10 f3    BPL &19f4
1a01 26 39    ROL &39
1a03 26 3a    ROL &3a
1a05 26 3b    ROL &3b
1a07 26 3c    ROL &3c
1a09 26 3d    ROL &3d
1a0b 26 3e    ROL &3e
1a0d 26 3f    ROL &3f
1a0f 26 40    ROL &40
1a11 38       SEC     
1a12 a5 3d    LDA &3d
1a14 e5 2a    SBC &2a
1a16 48       PHA     
1a17 a5 3e    LDA &3e
1a19 e5 2b    SBC &2b
1a1b 48       PHA     
1a1c a5 3f    LDA &3f
1a1e e5 2c    SBC &2c
1a20 aa       TAX     
1a21 a5 40    LDA &40
1a23 e5 2d    SBC &2d
1a25 90 0c    BCC &1a33
1a27 85 40    STA &40
1a29 86 3f    STX &3f
1a2b 68       PLA     
1a2c 85 3e    STA &3e
1a2e 68       PLA     
1a2f 85 3d    STA &3d
1a31 b0 02    BCS &1a35
1a33 68       PLA     
1a34 68       PLA     
1a35 88       DEY     
1a36 d0 c9    BNE &1a01
1a38 60       RTS     
1a39 86 27    STX &27
1a3b 20 ea bd JSR &bdea
1a3e 20 51 bd JSR &bd51
1a41 20 be a2 JSR &a2be
1a44 20 1e a2 JSR &a21e
1a47 20 7e bd JSR &bd7e
1a4a 20 b5 a3 JSR &a3b5
1a4d 4c 62 9a JMP &9a62
1a50 20 51 bd JSR &bd51
1a53 20 42 9c JSR &9c42
1a56 86 27    STX &27
1a58 a8       TAY     
1a59 20 fd 92 JSR &92fd
1a5c 20 7e bd JSR &bd7e
1a5f 20 4e a3 JSR &a34e
1a62 a6 27    LDX &27
1a64 a0 00    LDY #&00
1a66 a5 3b    LDA &3b
1a68 29 80    AND #&80
1a6a 85 3b    STA &3b
1a6c a5 2e    LDA &2e
1a6e 29 80    AND #&80
1a70 c5 3b    CMP &3b
1a72 d0 1e    BNE &1a92
1a74 a5 3d    LDA &3d
1a76 c5 30    CMP &30
1a78 d0 19    BNE &1a93
1a7a a5 3e    LDA &3e
1a7c c5 31    CMP &31
1a7e d0 13    BNE &1a93
1a80 a5 3f    LDA &3f
1a82 c5 32    CMP &32
1a84 d0 0d    BNE &1a93
1a86 a5 40    LDA &40
1a88 c5 33    CMP &33
1a8a d0 07    BNE &1a93
1a8c a5 41    LDA &41
1a8e c5 34    CMP &34
1a90 d0 01    BNE &1a93
1a92 60       RTS     
1a93 6a       ROR A
1a94 45 3b    EOR &3b
1a96 2a       ROL A
1a97 a9 01    LDA #&01
1a99 60       RTS     
1a9a 4c 0e 8c JMP &8c0e
1a9d 8a       TXA     
1a9e f0 47    BEQ &1ae7
1aa0 30 ae    BMI &1a50
1aa2 20 94 bd JSR &bd94
1aa5 20 42 9c JSR &9c42
1aa8 a8       TAY     
1aa9 f0 ef    BEQ &1a9a
1aab 30 8c    BMI &1a39
1aad a5 2d    LDA &2d
1aaf 49 80    EOR #&80
1ab1 85 2d    STA &2d
1ab3 38       SEC     
1ab4 a0 00    LDY #&00
1ab6 b1 04    LDA (&04),Y
1ab8 e5 2a    SBC &2a
1aba 85 2a    STA &2a
1abc c8       INY     
1abd b1 04    LDA (&04),Y
1abf e5 2b    SBC &2b
1ac1 85 2b    STA &2b
1ac3 c8       INY     
1ac4 b1 04    LDA (&04),Y
1ac6 e5 2c    SBC &2c
1ac8 85 2c    STA &2c
1aca c8       INY     
1acb b1 04    LDA (&04),Y
1acd a0 00    LDY #&00
1acf 49 80    EOR #&80
1ad1 e5 2d    SBC &2d
1ad3 05 2a    ORA &2a
1ad5 05 2b    ORA &2b
1ad7 05 2c    ORA &2c
1ad9 08       PHP     
1ada 18       CLC     
1adb a9 04    LDA #&04
1add 65 04    ADC &04
1adf 85 04    STA &04
1ae1 90 02    BCC &1ae5
1ae3 e6 05    INC &05
1ae5 28       PLP     
1ae6 60       RTS     
1ae7 20 b2 bd JSR &bdb2
1aea 20 42 9c JSR &9c42
1aed a8       TAY     
1aee d0 aa    BNE &1a9a
1af0 86 37    STX &37
1af2 a6 36    LDX &36
1af4 a0 00    LDY #&00
1af6 b1 04    LDA (&04),Y
1af8 85 39    STA &39
1afa c5 36    CMP &36
1afc b0 01    BCS &1aff
1afe aa       TAX     
1aff 86 3a    STX &3a
1b01 a0 00    LDY #&00
1b03 c4 3a    CPY &3a
1b05 f0 0a    BEQ &1b11
1b07 c8       INY     
1b08 b1 04    LDA (&04),Y
1b0a d9 ff 05 CMP &05ff,Y
1b0d f0 f4    BEQ &1b03
1b0f d0 04    BNE &1b15
1b11 a5 39    LDA &39
1b13 c5 36    CMP &36
1b15 08       PHP     
1b16 20 dc bd JSR &bddc
1b19 a6 37    LDX &37
1b1b 28       PLP     
1b1c 60       RTS     
1b1d a5 0b    LDA &0b
1b1f 85 19    STA &19
1b21 a5 0c    LDA &0c
1b23 85 1a    STA &1a
1b25 a5 0a    LDA &0a
1b27 85 1b    STA &1b
1b29 20 72 9b JSR &9b72
1b2c e0 84    CPX #&84
1b2e f0 0a    BEQ &1b3a
1b30 e0 82    CPX #&82
1b32 f0 21    BEQ &1b55
1b34 c6 1b    DEC &1b
1b36 a8       TAY     
1b37 85 27    STA &27
1b39 60       RTS     
1b3a 20 6b 9b JSR &9b6b
1b3d a8       TAY     
1b3e 20 f0 92 JSR &92f0
1b41 a0 03    LDY #&03
1b43 b1 04    LDA (&04),Y
1b45 19 2a 00 ORA &002a,Y
1b48 99 2a 00 STA &002a,Y
1b4b 88       DEY     
1b4c 10 f5    BPL &1b43
1b4e 20 ff bd JSR &bdff
1b51 a9 40    LDA #&40
1b53 d0 d7    BNE &1b2c
1b55 20 6b 9b JSR &9b6b
1b58 a8       TAY     
1b59 20 f0 92 JSR &92f0
1b5c a0 03    LDY #&03
1b5e b1 04    LDA (&04),Y
1b60 59 2a 00 EOR &002a,Y
1b63 99 2a 00 STA &002a,Y
1b66 88       DEY     
1b67 10 f5    BPL &1b5e
1b69 30 e3    BMI &1b4e
1b6b a8       TAY     
1b6c 20 f0 92 JSR &92f0
1b6f 20 94 bd JSR &bd94
1b72 20 9c 9b JSR &9b9c
1b75 e0 80    CPX #&80
1b77 f0 01    BEQ &1b7a
1b79 60       RTS     
1b7a a8       TAY     
1b7b 20 f0 92 JSR &92f0
1b7e 20 94 bd JSR &bd94
1b81 20 9c 9b JSR &9b9c
1b84 a8       TAY     
1b85 20 f0 92 JSR &92f0
1b88 a0 03    LDY #&03
1b8a b1 04    LDA (&04),Y
1b8c 39 2a 00 AND &002a,Y
1b8f 99 2a 00 STA &002a,Y
1b92 88       DEY     
1b93 10 f5    BPL &1b8a
1b95 20 ff bd JSR &bdff
1b98 a9 40    LDA #&40
1b9a d0 d9    BNE &1b75
1b9c 20 42 9c JSR &9c42
1b9f e0 3f    CPX #&3f
1ba1 b0 04    BCS &1ba7
1ba3 e0 3c    CPX #&3c
1ba5 b0 01    BCS &1ba8
1ba7 60       RTS     
1ba8 f0 16    BEQ &1bc0
1baa e0 3e    CPX #&3e
1bac f0 3a    BEQ &1be8
1bae aa       TAX     
1baf 20 9e 9a JSR &9a9e
1bb2 d0 01    BNE &1bb5
1bb4 88       DEY     
1bb5 84 2a    STY &2a
1bb7 84 2b    STY &2b
1bb9 84 2c    STY &2c
1bbb 84 2d    STY &2d
1bbd a9 40    LDA #&40
1bbf 60       RTS     
1bc0 aa       TAX     
1bc1 a4 1b    LDY &1b
1bc3 b1 19    LDA (&19),Y
1bc5 c9 3d    CMP #&3d
1bc7 f0 0b    BEQ &1bd4
1bc9 c9 3e    CMP #&3e
1bcb f0 12    BEQ &1bdf
1bcd 20 9d 9a JSR &9a9d
1bd0 90 e2    BCC &1bb4
1bd2 b0 e1    BCS &1bb5
1bd4 e6 1b    INC &1b
1bd6 20 9d 9a JSR &9a9d
1bd9 f0 d9    BEQ &1bb4
1bdb 90 d7    BCC &1bb4
1bdd b0 d6    BCS &1bb5
1bdf e6 1b    INC &1b
1be1 20 9d 9a JSR &9a9d
1be4 d0 ce    BNE &1bb4
1be6 f0 cd    BEQ &1bb5
1be8 aa       TAX     
1be9 a4 1b    LDY &1b
1beb b1 19    LDA (&19),Y
1bed c9 3d    CMP #&3d
1bef f0 09    BEQ &1bfa
1bf1 20 9d 9a JSR &9a9d
1bf4 f0 bf    BEQ &1bb5
1bf6 b0 bc    BCS &1bb4
1bf8 90 bb    BCC &1bb5
1bfa e6 1b    INC &1b
1bfc 20 9d 9a JSR &9a9d
1bff b0 b3    BCS &1bb4
1c01 90 b2    BCC &1bb5
1c03 00       BRK     
1c04 13       ???
1c05 53       ???
1c06 74       ???
1c07 72       ???
1c08 69 6e    ADC #&6e
1c0a 67       ???
1c0b 20 74 6f JSR &6f74
1c0e 6f       ???
1c0f 20 6c 6f JSR &6f6c
1c12 6e 67 00 ROR &0067
1c15 20 b2 bd JSR &bdb2
1c18 20 20 9e JSR &9e20
1c1b a8       TAY     
1c1c d0 6a    BNE &1c88
1c1e 18       CLC     
1c1f 86 37    STX &37
1c21 a0 00    LDY #&00
1c23 b1 04    LDA (&04),Y
1c25 65 36    ADC &36
1c27 b0 da    BCS &1c03
1c29 aa       TAX     
1c2a 48       PHA     
1c2b a4 36    LDY &36
1c2d b9 ff 05 LDA &05ff,Y
1c30 9d ff 05 STA &05ff,X
1c33 ca       DEX     
1c34 88       DEY     
1c35 d0 f6    BNE &1c2d
1c37 20 cb bd JSR &bdcb
1c3a 68       PLA     
1c3b 85 36    STA &36
1c3d a6 37    LDX &37
1c3f 98       TYA     
1c40 f0 03    BEQ &1c45
1c42 20 d1 9d JSR &9dd1
1c45 e0 2b    CPX #&2b
1c47 f0 05    BEQ &1c4e
1c49 e0 2d    CPX #&2d
1c4b f0 68    BEQ &1cb5
1c4d 60       RTS     
1c4e a8       TAY     
1c4f f0 c4    BEQ &1c15
1c51 30 38    BMI &1c8b
1c53 20 ce 9d JSR &9dce
1c56 a8       TAY     
1c57 f0 2f    BEQ &1c88
1c59 30 4c    BMI &1ca7
1c5b a0 00    LDY #&00
1c5d 18       CLC     
1c5e b1 04    LDA (&04),Y
1c60 65 2a    ADC &2a
1c62 85 2a    STA &2a
1c64 c8       INY     
1c65 b1 04    LDA (&04),Y
1c67 65 2b    ADC &2b
1c69 85 2b    STA &2b
1c6b c8       INY     
1c6c b1 04    LDA (&04),Y
1c6e 65 2c    ADC &2c
1c70 85 2c    STA &2c
1c72 c8       INY     
1c73 b1 04    LDA (&04),Y
1c75 65 2d    ADC &2d
1c77 85 2d    STA &2d
1c79 18       CLC     
1c7a a5 04    LDA &04
1c7c 69 04    ADC #&04
1c7e 85 04    STA &04
1c80 a9 40    LDA #&40
1c82 90 c1    BCC &1c45
1c84 e6 05    INC &05
1c86 b0 bd    BCS &1c45
1c88 4c 0e 8c JMP &8c0e
1c8b 20 51 bd JSR &bd51
1c8e 20 d1 9d JSR &9dd1
1c91 a8       TAY     
1c92 f0 f4    BEQ &1c88
1c94 86 27    STX &27
1c96 30 03    BMI &1c9b
1c98 20 be a2 JSR &a2be
1c9b 20 7e bd JSR &bd7e
1c9e 20 00 a5 JSR &a500
1ca1 a6 27    LDX &27
1ca3 a9 ff    LDA #&ff
1ca5 d0 9e    BNE &1c45
1ca7 86 27    STX &27
1ca9 20 ea bd JSR &bdea
1cac 20 51 bd JSR &bd51
1caf 20 be a2 JSR &a2be
1cb2 4c 9b 9c JMP &9c9b
1cb5 a8       TAY     
1cb6 f0 d0    BEQ &1c88
1cb8 30 27    BMI &1ce1
1cba 20 ce 9d JSR &9dce
1cbd a8       TAY     
1cbe f0 c8    BEQ &1c88
1cc0 30 38    BMI &1cfa
1cc2 38       SEC     
1cc3 a0 00    LDY #&00
1cc5 b1 04    LDA (&04),Y
1cc7 e5 2a    SBC &2a
1cc9 85 2a    STA &2a
1ccb c8       INY     
1ccc b1 04    LDA (&04),Y
1cce e5 2b    SBC &2b
1cd0 85 2b    STA &2b
1cd2 c8       INY     
1cd3 b1 04    LDA (&04),Y
1cd5 e5 2c    SBC &2c
1cd7 85 2c    STA &2c
1cd9 c8       INY     
1cda b1 04    LDA (&04),Y
1cdc e5 2d    SBC &2d
1cde 4c 77 9c JMP &9c77
1ce1 20 51 bd JSR &bd51
1ce4 20 d1 9d JSR &9dd1
1ce7 a8       TAY     
1ce8 f0 9e    BEQ &1c88
1cea 86 27    STX &27
1cec 30 03    BMI &1cf1
1cee 20 be a2 JSR &a2be
1cf1 20 7e bd JSR &bd7e
1cf4 20 fd a4 JSR &a4fd
1cf7 4c a1 9c JMP &9ca1
1cfa 86 27    STX &27
1cfc 20 ea bd JSR &bdea
1cff 20 51 bd JSR &bd51
1d02 20 be a2 JSR &a2be
1d05 20 7e bd JSR &bd7e
1d08 20 d0 a4 JSR &a4d0
1d0b 4c a1 9c JMP &9ca1
1d0e 20 be a2 JSR &a2be
1d11 20 ea bd JSR &bdea
1d14 20 51 bd JSR &bd51
1d17 20 be a2 JSR &a2be
1d1a 4c 2c 9d JMP &9d2c
1d1d 20 be a2 JSR &a2be
1d20 20 51 bd JSR &bd51
1d23 20 20 9e JSR &9e20
1d26 86 27    STX &27
1d28 a8       TAY     
1d29 20 fd 92 JSR &92fd
1d2c 20 7e bd JSR &bd7e
1d2f 20 56 a6 JSR &a656
1d32 a9 ff    LDA #&ff
1d34 a6 27    LDX &27
1d36 4c d4 9d JMP &9dd4
1d39 4c 0e 8c JMP &8c0e
1d3c a8       TAY     
1d3d f0 fa    BEQ &1d39
1d3f 30 df    BMI &1d20
1d41 a5 2d    LDA &2d
1d43 c5 2c    CMP &2c
1d45 d0 d6    BNE &1d1d
1d47 a8       TAY     
1d48 f0 04    BEQ &1d4e
1d4a c9 ff    CMP #&ff
1d4c d0 cf    BNE &1d1d
1d4e 45 2b    EOR &2b
1d50 30 cb    BMI &1d1d
1d52 20 1d 9e JSR &9e1d
1d55 86 27    STX &27
1d57 a8       TAY     
1d58 f0 df    BEQ &1d39
1d5a 30 b5    BMI &1d11
1d5c a5 2d    LDA &2d
1d5e c5 2c    CMP &2c
1d60 d0 ac    BNE &1d0e
1d62 a8       TAY     
1d63 f0 04    BEQ &1d69
1d65 c9 ff    CMP #&ff
1d67 d0 a5    BNE &1d0e
1d69 45 2b    EOR &2b
1d6b 30 a1    BMI &1d0e
1d6d a5 2d    LDA &2d
1d6f 48       PHA     
1d70 20 71 ad JSR &ad71
1d73 a2 39    LDX #&39
1d75 20 44 be JSR &be44
1d78 20 ea bd JSR &bdea
1d7b 68       PLA     
1d7c 45 2d    EOR &2d
1d7e 85 37    STA &37
1d80 20 71 ad JSR &ad71
1d83 a0 00    LDY #&00
1d85 a2 00    LDX #&00
1d87 84 3f    STY &3f
1d89 84 40    STY &40
1d8b 46 3a    LSR &3a
1d8d 66 39    ROR &39
1d8f 90 15    BCC &1da6
1d91 18       CLC     
1d92 98       TYA     
1d93 65 2a    ADC &2a
1d95 a8       TAY     
1d96 8a       TXA     
1d97 65 2b    ADC &2b
1d99 aa       TAX     
1d9a a5 3f    LDA &3f
1d9c 65 2c    ADC &2c
1d9e 85 3f    STA &3f
1da0 a5 40    LDA &40
1da2 65 2d    ADC &2d
1da4 85 40    STA &40
1da6 06 2a    ASL &2a
1da8 26 2b    ROL &2b
1daa 26 2c    ROL &2c
1dac 26 2d    ROL &2d
1dae a5 39    LDA &39
1db0 05 3a    ORA &3a
1db2 d0 d7    BNE &1d8b
1db4 84 3d    STY &3d
1db6 86 3e    STX &3e
1db8 a5 37    LDA &37
1dba 08       PHP     
1dbb a2 3d    LDX #&3d
1dbd 20 56 af JSR &af56
1dc0 28       PLP     
1dc1 10 03    BPL &1dc6
1dc3 20 93 ad JSR &ad93
1dc6 a6 27    LDX &27
1dc8 4c d4 9d JMP &9dd4
1dcb 4c 3c 9d JMP &9d3c
1dce 20 94 bd JSR &bd94
1dd1 20 20 9e JSR &9e20
1dd4 e0 2a    CPX #&2a
1dd6 f0 f3    BEQ &1dcb
1dd8 e0 2f    CPX #&2f
1dda f0 09    BEQ &1de5
1ddc e0 83    CPX #&83
1dde f0 21    BEQ &1e01
1de0 e0 81    CPX #&81
1de2 f0 26    BEQ &1e0a
1de4 60       RTS     
1de5 a8       TAY     
1de6 20 fd 92 JSR &92fd
1de9 20 51 bd JSR &bd51
1dec 20 20 9e JSR &9e20
1def 86 27    STX &27
1df1 a8       TAY     
1df2 20 fd 92 JSR &92fd
1df5 20 7e bd JSR &bd7e
1df8 20 ad a6 JSR &a6ad
1dfb a6 27    LDX &27
1dfd a9 ff    LDA #&ff
1dff d0 d3    BNE &1dd4
1e01 20 be 99 JSR &99be
1e04 a5 38    LDA &38
1e06 08       PHP     
1e07 4c bb 9d JMP &9dbb
1e0a 20 be 99 JSR &99be
1e0d 26 39    ROL &39
1e0f 26 3a    ROL &3a
1e11 26 3b    ROL &3b
1e13 26 3c    ROL &3c
1e15 24 37    BIT &37
1e17 08       PHP     
1e18 a2 39    LDX #&39
1e1a 4c bd 9d JMP &9dbd
1e1d 20 94 bd JSR &bd94
1e20 20 ec ad JSR &adec
1e23 48       PHA     
1e24 a4 1b    LDY &1b
1e26 e6 1b    INC &1b
1e28 b1 19    LDA (&19),Y
1e2a c9 20    CMP #&20
1e2c f0 f6    BEQ &1e24
1e2e aa       TAX     
1e2f 68       PLA     
1e30 e0 5e    CPX #&5e
1e32 f0 01    BEQ &1e35
1e34 60       RTS     
1e35 a8       TAY     
1e36 20 fd 92 JSR &92fd
1e39 20 51 bd JSR &bd51
1e3c 20 fa 92 JSR &92fa
1e3f a5 30    LDA &30
1e41 c9 87    CMP #&87
1e43 b0 43    BCS &1e88
1e45 20 86 a4 JSR &a486
1e48 d0 0f    BNE &1e59
1e4a 20 7e bd JSR &bd7e
1e4d 20 b5 a3 JSR &a3b5
1e50 a5 4a    LDA &4a
1e52 20 12 ab JSR &ab12
1e55 a9 ff    LDA #&ff
1e57 d0 ca    BNE &1e23
1e59 20 81 a3 JSR &a381
1e5c a5 04    LDA &04
1e5e 85 4b    STA &4b
1e60 a5 05    LDA &05
1e62 85 4c    STA &4c
1e64 20 b5 a3 JSR &a3b5
1e67 a5 4a    LDA &4a
1e69 20 12 ab JSR &ab12
1e6c 20 7d a3 JSR &a37d
1e6f 20 7e bd JSR &bd7e
1e72 20 b5 a3 JSR &a3b5
1e75 20 01 a8 JSR &a801
1e78 20 d1 aa JSR &aad1
1e7b 20 94 aa JSR &aa94
1e7e 20 ed a7 JSR &a7ed
1e81 20 56 a6 JSR &a656
1e84 a9 ff    LDA #&ff
1e86 d0 9b    BNE &1e23
1e88 20 81 a3 JSR &a381
1e8b 20 99 a6 JSR &a699
1e8e d0 dc    BNE &1e6c
1e90 98       TYA     
1e91 10 03    BPL &1e96
1e93 20 e4 a3 JSR &a3e4
1e96 a2 00    LDX #&00
1e98 a0 00    LDY #&00
1e9a b9 2a 00 LDA &002a,Y
1e9d 48       PHA     
1e9e 29 0f    AND #&0f
1ea0 95 3f    STA &3f,X
1ea2 68       PLA     
1ea3 4a       LSR A
1ea4 4a       LSR A
1ea5 4a       LSR A
1ea6 4a       LSR A
1ea7 e8       INX     
1ea8 95 3f    STA &3f,X
1eaa e8       INX     
1eab c8       INY     
1eac c0 04    CPY #&04
1eae d0 ea    BNE &1e9a
1eb0 ca       DEX     
1eb1 f0 04    BEQ &1eb7
1eb3 b5 3f    LDA &3f,X
1eb5 f0 f9    BEQ &1eb0
1eb7 b5 3f    LDA &3f,X
1eb9 c9 0a    CMP #&0a
1ebb 90 02    BCC &1ebf
1ebd 69 06    ADC #&06
1ebf 69 30    ADC #&30
1ec1 20 66 a0 JSR &a066
1ec4 ca       DEX     
1ec5 10 f0    BPL &1eb7
1ec7 60       RTS     
1ec8 10 07    BPL &1ed1
1eca a9 2d    LDA #&2d
1ecc 85 2e    STA &2e
1ece 20 66 a0 JSR &a066
1ed1 a5 30    LDA &30
1ed3 c9 81    CMP #&81
1ed5 b0 4e    BCS &1f25
1ed7 20 f4 a1 JSR &a1f4
1eda c6 49    DEC &49
1edc 4c d1 9e JMP &9ed1
1edf ae 02 04 LDX &0402
1ee2 e0 03    CPX #&03
1ee4 90 02    BCC &1ee8
1ee6 a2 00    LDX #&00
1ee8 86 37    STX &37
1eea ad 01 04 LDA &0401
1eed f0 06    BEQ &1ef5
1eef c9 0a    CMP #&0a
1ef1 b0 06    BCS &1ef9
1ef3 90 06    BCC &1efb
1ef5 e0 02    CPX #&02
1ef7 f0 02    BEQ &1efb
1ef9 a9 0a    LDA #&0a
1efb 85 38    STA &38
1efd 85 4e    STA &4e
1eff a9 00    LDA #&00
1f01 85 36    STA &36
1f03 85 49    STA &49
1f05 24 15    BIT &15
1f07 30 87    BMI &1e90
1f09 98       TYA     
1f0a 30 03    BMI &1f0f
1f0c 20 be a2 JSR &a2be
1f0f 20 da a1 JSR &a1da
1f12 d0 b4    BNE &1ec8
1f14 a5 37    LDA &37
1f16 d0 05    BNE &1f1d
1f18 a9 30    LDA #&30
1f1a 4c 66 a0 JMP &a066
1f1d 4c 9c 9f JMP &9f9c
1f20 20 99 a6 JSR &a699
1f23 d0 0f    BNE &1f34
1f25 c9 84    CMP #&84
1f27 90 10    BCC &1f39
1f29 d0 06    BNE &1f31
1f2b a5 31    LDA &31
1f2d c9 a0    CMP #&a0
1f2f 90 08    BCC &1f39
1f31 20 4d a2 JSR &a24d
1f34 e6 49    INC &49
1f36 4c d1 9e JMP &9ed1
1f39 a5 35    LDA &35
1f3b 85 27    STA &27
1f3d 20 85 a3 JSR &a385
1f40 a5 4e    LDA &4e
1f42 85 38    STA &38
1f44 a6 37    LDX &37
1f46 e0 02    CPX #&02
1f48 d0 12    BNE &1f5c
1f4a 65 49    ADC &49
1f4c 30 52    BMI &1fa0
1f4e 85 38    STA &38
1f50 c9 0b    CMP #&0b
1f52 90 08    BCC &1f5c
1f54 a9 0a    LDA #&0a
1f56 85 38    STA &38
1f58 a9 00    LDA #&00
1f5a 85 37    STA &37
1f5c 20 86 a6 JSR &a686
1f5f a9 a0    LDA #&a0
1f61 85 31    STA &31
1f63 a9 83    LDA #&83
1f65 85 30    STA &30
1f67 a6 38    LDX &38
1f69 f0 06    BEQ &1f71
1f6b 20 4d a2 JSR &a24d
1f6e ca       DEX     
1f6f d0 fa    BNE &1f6b
1f71 20 f5 a7 JSR &a7f5
1f74 20 4e a3 JSR &a34e
1f77 a5 27    LDA &27
1f79 85 42    STA &42
1f7b 20 0b a5 JSR &a50b
1f7e a5 30    LDA &30
1f80 c9 84    CMP #&84
1f82 b0 0e    BCS &1f92
1f84 66 31    ROR &31
1f86 66 32    ROR &32
1f88 66 33    ROR &33
1f8a 66 34    ROR &34
1f8c 66 35    ROR &35
1f8e e6 30    INC &30
1f90 d0 ec    BNE &1f7e
1f92 a5 31    LDA &31
1f94 c9 a0    CMP #&a0
1f96 b0 88    BCS &1f20
1f98 a5 38    LDA &38
1f9a d0 11    BNE &1fad
1f9c c9 01    CMP #&01
1f9e f0 46    BEQ &1fe6
1fa0 20 86 a6 JSR &a686
1fa3 a9 00    LDA #&00
1fa5 85 49    STA &49
1fa7 a5 4e    LDA &4e
1fa9 85 38    STA &38
1fab e6 38    INC &38
1fad a9 01    LDA #&01
1faf c5 37    CMP &37
1fb1 f0 33    BEQ &1fe6
1fb3 a4 49    LDY &49
1fb5 30 0c    BMI &1fc3
1fb7 c4 38    CPY &38
1fb9 b0 2b    BCS &1fe6
1fbb a9 00    LDA #&00
1fbd 85 49    STA &49
1fbf c8       INY     
1fc0 98       TYA     
1fc1 d0 23    BNE &1fe6
1fc3 a5 37    LDA &37
1fc5 c9 02    CMP #&02
1fc7 f0 06    BEQ &1fcf
1fc9 a9 01    LDA #&01
1fcb c0 ff    CPY #&ff
1fcd d0 17    BNE &1fe6
1fcf a9 30    LDA #&30
1fd1 20 66 a0 JSR &a066
1fd4 a9 2e    LDA #&2e
1fd6 20 66 a0 JSR &a066
1fd9 a9 30    LDA #&30
1fdb e6 49    INC &49
1fdd f0 05    BEQ &1fe4
1fdf 20 66 a0 JSR &a066
1fe2 d0 f7    BNE &1fdb
1fe4 a9 80    LDA #&80
1fe6 85 4e    STA &4e
1fe8 20 40 a0 JSR &a040
1feb c6 4e    DEC &4e
1fed d0 05    BNE &1ff4
1fef a9 2e    LDA #&2e
1ff1 20 66 a0 JSR &a066
1ff4 c6 38    DEC &38
1ff6 d0 f0    BNE &1fe8
1ff8 a4 37    LDY &37
1ffa 88       DEY     
1ffb f0 18    BEQ &2015
1ffd 88       DEY     
1ffe f0 11    BEQ &2011
2000 a4 36    LDY &36
2002 88       DEY     
2003 b9 00 06 LDA &0600,Y
2006 c9 30    CMP #&30
2008 f0 f8    BEQ &2002
200a c9 2e    CMP #&2e
200c f0 01    BEQ &200f
200e c8       INY     
200f 84 36    STY &36
2011 a5 49    LDA &49
2013 f0 2a    BEQ &203f
2015 a9 45    LDA #&45
2017 20 66 a0 JSR &a066
201a a5 49    LDA &49
201c 10 0a    BPL &2028
201e a9 2d    LDA #&2d
2020 20 66 a0 JSR &a066
2023 38       SEC     
2024 a9 00    LDA #&00
2026 e5 49    SBC &49
2028 20 52 a0 JSR &a052
202b a5 37    LDA &37
202d f0 10    BEQ &203f
202f a9 20    LDA #&20
2031 a4 49    LDY &49
2033 30 03    BMI &2038
2035 20 66 a0 JSR &a066
2038 e0 00    CPX #&00
203a d0 03    BNE &203f
203c 4c 66 a0 JMP &a066
203f 60       RTS     
2040 a5 31    LDA &31
2042 4a       LSR A
2043 4a       LSR A
2044 4a       LSR A
2045 4a       LSR A
2046 20 64 a0 JSR &a064
2049 a5 31    LDA &31
204b 29 0f    AND #&0f
204d 85 31    STA &31
204f 4c 97 a1 JMP &a197
2052 a2 ff    LDX #&ff
2054 38       SEC     
2055 e8       INX     
2056 e9 0a    SBC #&0a
2058 b0 fb    BCS &2055
205a 69 0a    ADC #&0a
205c 48       PHA     
205d 8a       TXA     
205e f0 03    BEQ &2063
2060 20 64 a0 JSR &a064
2063 68       PLA     
2064 09 30    ORA #&30
2066 86 3b    STX &3b
2068 a6 36    LDX &36
206a 9d 00 06 STA &0600,X
206d a6 3b    LDX &3b
206f e6 36    INC &36
2071 60       RTS     
2072 18       CLC     
2073 86 35    STX &35
2075 20 da a1 JSR &a1da
2078 a9 ff    LDA #&ff
207a 60       RTS     
207b a2 00    LDX #&00
207d 86 31    STX &31
207f 86 32    STX &32
2081 86 33    STX &33
2083 86 34    STX &34
2085 86 35    STX &35
2087 86 48    STX &48
2089 86 49    STX &49
208b c9 2e    CMP #&2e
208d f0 11    BEQ &20a0
208f c9 3a    CMP #&3a
2091 b0 df    BCS &2072
2093 e9 2f    SBC #&2f
2095 30 db    BMI &2072
2097 85 35    STA &35
2099 c8       INY     
209a b1 19    LDA (&19),Y
209c c9 2e    CMP #&2e
209e d0 08    BNE &20a8
20a0 a5 48    LDA &48
20a2 d0 44    BNE &20e8
20a4 e6 48    INC &48
20a6 d0 f1    BNE &2099
20a8 c9 45    CMP #&45
20aa f0 35    BEQ &20e1
20ac c9 3a    CMP #&3a
20ae b0 38    BCS &20e8
20b0 e9 2f    SBC #&2f
20b2 90 34    BCC &20e8
20b4 a6 31    LDX &31
20b6 e0 18    CPX #&18
20b8 90 08    BCC &20c2
20ba a6 48    LDX &48
20bc d0 db    BNE &2099
20be e6 49    INC &49
20c0 b0 d7    BCS &2099
20c2 a6 48    LDX &48
20c4 f0 02    BEQ &20c8
20c6 c6 49    DEC &49
20c8 20 97 a1 JSR &a197
20cb 65 35    ADC &35
20cd 85 35    STA &35
20cf 90 c8    BCC &2099
20d1 e6 34    INC &34
20d3 d0 c4    BNE &2099
20d5 e6 33    INC &33
20d7 d0 c0    BNE &2099
20d9 e6 32    INC &32
20db d0 bc    BNE &2099
20dd e6 31    INC &31
20df d0 b8    BNE &2099
20e1 20 40 a1 JSR &a140
20e4 65 49    ADC &49
20e6 85 49    STA &49
20e8 84 1b    STY &1b
20ea a5 49    LDA &49
20ec 05 48    ORA &48
20ee f0 2f    BEQ &211f
20f0 20 da a1 JSR &a1da
20f3 f0 26    BEQ &211b
20f5 a9 a8    LDA #&a8
20f7 85 30    STA &30
20f9 a9 00    LDA #&00
20fb 85 2f    STA &2f
20fd 85 2e    STA &2e
20ff 20 03 a3 JSR &a303
2102 a5 49    LDA &49
2104 30 0b    BMI &2111
2106 f0 10    BEQ &2118
2108 20 f4 a1 JSR &a1f4
210b c6 49    DEC &49
210d d0 f9    BNE &2108
210f f0 07    BEQ &2118
2111 20 4d a2 JSR &a24d
2114 e6 49    INC &49
2116 d0 f9    BNE &2111
2118 20 5c a6 JSR &a65c
211b 38       SEC     
211c a9 ff    LDA #&ff
211e 60       RTS     
211f a5 32    LDA &32
2121 85 2d    STA &2d
2123 29 80    AND #&80
2125 05 31    ORA &31
2127 d0 cc    BNE &20f5
2129 a5 35    LDA &35
212b 85 2a    STA &2a
212d a5 34    LDA &34
212f 85 2b    STA &2b
2131 a5 33    LDA &33
2133 85 2c    STA &2c
2135 a9 40    LDA #&40
2137 38       SEC     
2138 60       RTS     
2139 20 4b a1 JSR &a14b
213c 49 ff    EOR #&ff
213e 38       SEC     
213f 60       RTS     
2140 c8       INY     
2141 b1 19    LDA (&19),Y
2143 c9 2d    CMP #&2d
2145 f0 f2    BEQ &2139
2147 c9 2b    CMP #&2b
2149 d0 03    BNE &214e
214b c8       INY     
214c b1 19    LDA (&19),Y
214e c9 3a    CMP #&3a
2150 b0 22    BCS &2174
2152 e9 2f    SBC #&2f
2154 90 1e    BCC &2174
2156 85 4a    STA &4a
2158 c8       INY     
2159 b1 19    LDA (&19),Y
215b c9 3a    CMP #&3a
215d b0 11    BCS &2170
215f e9 2f    SBC #&2f
2161 90 0d    BCC &2170
2163 c8       INY     
2164 85 43    STA &43
2166 a5 4a    LDA &4a
2168 0a       ASL A
2169 0a       ASL A
216a 65 4a    ADC &4a
216c 0a       ASL A
216d 65 43    ADC &43
216f 60       RTS     
2170 a5 4a    LDA &4a
2172 18       CLC     
2173 60       RTS     
2174 a9 00    LDA #&00
2176 18       CLC     
2177 60       RTS     
2178 a5 35    LDA &35
217a 65 42    ADC &42
217c 85 35    STA &35
217e a5 34    LDA &34
2180 65 41    ADC &41
2182 85 34    STA &34
2184 a5 33    LDA &33
2186 65 40    ADC &40
2188 85 33    STA &33
218a a5 32    LDA &32
218c 65 3f    ADC &3f
218e 85 32    STA &32
2190 a5 31    LDA &31
2192 65 3e    ADC &3e
2194 85 31    STA &31
2196 60       RTS     
2197 48       PHA     
2198 a6 34    LDX &34
219a a5 31    LDA &31
219c 48       PHA     
219d a5 32    LDA &32
219f 48       PHA     
21a0 a5 33    LDA &33
21a2 48       PHA     
21a3 a5 35    LDA &35
21a5 0a       ASL A
21a6 26 34    ROL &34
21a8 26 33    ROL &33
21aa 26 32    ROL &32
21ac 26 31    ROL &31
21ae 0a       ASL A
21af 26 34    ROL &34
21b1 26 33    ROL &33
21b3 26 32    ROL &32
21b5 26 31    ROL &31
21b7 65 35    ADC &35
21b9 85 35    STA &35
21bb 8a       TXA     
21bc 65 34    ADC &34
21be 85 34    STA &34
21c0 68       PLA     
21c1 65 33    ADC &33
21c3 85 33    STA &33
21c5 68       PLA     
21c6 65 32    ADC &32
21c8 85 32    STA &32
21ca 68       PLA     
21cb 65 31    ADC &31
21cd 06 35    ASL &35
21cf 26 34    ROL &34
21d1 26 33    ROL &33
21d3 26 32    ROL &32
21d5 2a       ROL A
21d6 85 31    STA &31
21d8 68       PLA     
21d9 60       RTS     
21da a5 31    LDA &31
21dc 05 32    ORA &32
21de 05 33    ORA &33
21e0 05 34    ORA &34
21e2 05 35    ORA &35
21e4 f0 07    BEQ &21ed
21e6 a5 2e    LDA &2e
21e8 d0 09    BNE &21f3
21ea a9 01    LDA #&01
21ec 60       RTS     
21ed 85 2e    STA &2e
21ef 85 30    STA &30
21f1 85 2f    STA &2f
21f3 60       RTS     
21f4 18       CLC     
21f5 a5 30    LDA &30
21f7 69 03    ADC #&03
21f9 85 30    STA &30
21fb 90 02    BCC &21ff
21fd e6 2f    INC &2f
21ff 20 1e a2 JSR &a21e
2202 20 42 a2 JSR &a242
2205 20 42 a2 JSR &a242
2208 20 78 a1 JSR &a178
220b 90 10    BCC &221d
220d 66 31    ROR &31
220f 66 32    ROR &32
2211 66 33    ROR &33
2213 66 34    ROR &34
2215 66 35    ROR &35
2217 e6 30    INC &30
2219 d0 02    BNE &221d
221b e6 2f    INC &2f
221d 60       RTS     
221e a5 2e    LDA &2e
2220 85 3b    STA &3b
2222 a5 2f    LDA &2f
2224 85 3c    STA &3c
2226 a5 30    LDA &30
2228 85 3d    STA &3d
222a a5 31    LDA &31
222c 85 3e    STA &3e
222e a5 32    LDA &32
2230 85 3f    STA &3f
2232 a5 33    LDA &33
2234 85 40    STA &40
2236 a5 34    LDA &34
2238 85 41    STA &41
223a a5 35    LDA &35
223c 85 42    STA &42
223e 60       RTS     
223f 20 1e a2 JSR &a21e
2242 46 3e    LSR &3e
2244 66 3f    ROR &3f
2246 66 40    ROR &40
2248 66 41    ROR &41
224a 66 42    ROR &42
224c 60       RTS     
224d 38       SEC     
224e a5 30    LDA &30
2250 e9 04    SBC #&04
2252 85 30    STA &30
2254 b0 02    BCS &2258
2256 c6 2f    DEC &2f
2258 20 3f a2 JSR &a23f
225b 20 08 a2 JSR &a208
225e 20 3f a2 JSR &a23f
2261 20 42 a2 JSR &a242
2264 20 42 a2 JSR &a242
2267 20 42 a2 JSR &a242
226a 20 08 a2 JSR &a208
226d a9 00    LDA #&00
226f 85 3e    STA &3e
2271 a5 31    LDA &31
2273 85 3f    STA &3f
2275 a5 32    LDA &32
2277 85 40    STA &40
2279 a5 33    LDA &33
227b 85 41    STA &41
227d a5 34    LDA &34
227f 85 42    STA &42
2281 a5 35    LDA &35
2283 2a       ROL A
2284 20 08 a2 JSR &a208
2287 a9 00    LDA #&00
2289 85 3e    STA &3e
228b 85 3f    STA &3f
228d a5 31    LDA &31
228f 85 40    STA &40
2291 a5 32    LDA &32
2293 85 41    STA &41
2295 a5 33    LDA &33
2297 85 42    STA &42
2299 a5 34    LDA &34
229b 2a       ROL A
229c 20 08 a2 JSR &a208
229f a5 32    LDA &32
22a1 2a       ROL A
22a2 a5 31    LDA &31
22a4 65 35    ADC &35
22a6 85 35    STA &35
22a8 90 13    BCC &22bd
22aa e6 34    INC &34
22ac d0 0f    BNE &22bd
22ae e6 33    INC &33
22b0 d0 0b    BNE &22bd
22b2 e6 32    INC &32
22b4 d0 07    BNE &22bd
22b6 e6 31    INC &31
22b8 d0 03    BNE &22bd
22ba 4c 0b a2 JMP &a20b
22bd 60       RTS     
22be a2 00    LDX #&00
22c0 86 35    STX &35
22c2 86 2f    STX &2f
22c4 a5 2d    LDA &2d
22c6 10 05    BPL &22cd
22c8 20 93 ad JSR &ad93
22cb a2 ff    LDX #&ff
22cd 86 2e    STX &2e
22cf a5 2a    LDA &2a
22d1 85 34    STA &34
22d3 a5 2b    LDA &2b
22d5 85 33    STA &33
22d7 a5 2c    LDA &2c
22d9 85 32    STA &32
22db a5 2d    LDA &2d
22dd 85 31    STA &31
22df a9 a0    LDA #&a0
22e1 85 30    STA &30
22e3 4c 03 a3 JMP &a303
22e6 85 2e    STA &2e
22e8 85 30    STA &30
22ea 85 2f    STA &2f
22ec 60       RTS     
22ed 48       PHA     
22ee 20 86 a6 JSR &a686
22f1 68       PLA     
22f2 f0 f8    BEQ &22ec
22f4 10 07    BPL &22fd
22f6 85 2e    STA &2e
22f8 a9 00    LDA #&00
22fa 38       SEC     
22fb e5 2e    SBC &2e
22fd 85 31    STA &31
22ff a9 88    LDA #&88
2301 85 30    STA &30
2303 a5 31    LDA &31
2305 30 e5    BMI &22ec
2307 05 32    ORA &32
2309 05 33    ORA &33
230b 05 34    ORA &34
230d 05 35    ORA &35
230f f0 d5    BEQ &22e6
2311 a5 30    LDA &30
2313 a4 31    LDY &31
2315 30 d5    BMI &22ec
2317 d0 21    BNE &233a
2319 a6 32    LDX &32
231b 86 31    STX &31
231d a6 33    LDX &33
231f 86 32    STX &32
2321 a6 34    LDX &34
2323 86 33    STX &33
2325 a6 35    LDX &35
2327 86 34    STX &34
2329 84 35    STY &35
232b 38       SEC     
232c e9 08    SBC #&08
232e 85 30    STA &30
2330 b0 e1    BCS &2313
2332 c6 2f    DEC &2f
2334 90 dd    BCC &2313
2336 a4 31    LDY &31
2338 30 b2    BMI &22ec
233a 06 35    ASL &35
233c 26 34    ROL &34
233e 26 33    ROL &33
2340 26 32    ROL &32
2342 26 31    ROL &31
2344 e9 00    SBC #&00
2346 85 30    STA &30
2348 b0 ec    BCS &2336
234a c6 2f    DEC &2f
234c 90 e8    BCC &2336
234e a0 04    LDY #&04
2350 b1 4b    LDA (&4b),Y
2352 85 41    STA &41
2354 88       DEY     
2355 b1 4b    LDA (&4b),Y
2357 85 40    STA &40
2359 88       DEY     
235a b1 4b    LDA (&4b),Y
235c 85 3f    STA &3f
235e 88       DEY     
235f b1 4b    LDA (&4b),Y
2361 85 3b    STA &3b
2363 88       DEY     
2364 84 42    STY &42
2366 84 3c    STY &3c
2368 b1 4b    LDA (&4b),Y
236a 85 3d    STA &3d
236c 05 3b    ORA &3b
236e 05 3f    ORA &3f
2370 05 40    ORA &40
2372 05 41    ORA &41
2374 f0 04    BEQ &237a
2376 a5 3b    LDA &3b
2378 09 80    ORA #&80
237a 85 3e    STA &3e
237c 60       RTS     
237d a9 71    LDA #&71
237f d0 06    BNE &2387
2381 a9 76    LDA #&76
2383 d0 02    BNE &2387
2385 a9 6c    LDA #&6c
2387 85 4b    STA &4b
2389 a9 04    LDA #&04
238b 85 4c    STA &4c
238d a0 00    LDY #&00
238f a5 30    LDA &30
2391 91 4b    STA (&4b),Y
2393 c8       INY     
2394 a5 2e    LDA &2e
2396 29 80    AND #&80
2398 85 2e    STA &2e
239a a5 31    LDA &31
239c 29 7f    AND #&7f
239e 05 2e    ORA &2e
23a0 91 4b    STA (&4b),Y
23a2 a5 32    LDA &32
23a4 c8       INY     
23a5 91 4b    STA (&4b),Y
23a7 a5 33    LDA &33
23a9 c8       INY     
23aa 91 4b    STA (&4b),Y
23ac a5 34    LDA &34
23ae c8       INY     
23af 91 4b    STA (&4b),Y
23b1 60       RTS     
23b2 20 f5 a7 JSR &a7f5
23b5 a0 04    LDY #&04
23b7 b1 4b    LDA (&4b),Y
23b9 85 34    STA &34
23bb 88       DEY     
23bc b1 4b    LDA (&4b),Y
23be 85 33    STA &33
23c0 88       DEY     
23c1 b1 4b    LDA (&4b),Y
23c3 85 32    STA &32
23c5 88       DEY     
23c6 b1 4b    LDA (&4b),Y
23c8 85 2e    STA &2e
23ca 88       DEY     
23cb b1 4b    LDA (&4b),Y
23cd 85 30    STA &30
23cf 84 35    STY &35
23d1 84 2f    STY &2f
23d3 05 2e    ORA &2e
23d5 05 32    ORA &32
23d7 05 33    ORA &33
23d9 05 34    ORA &34
23db f0 04    BEQ &23e1
23dd a5 2e    LDA &2e
23df 09 80    ORA #&80
23e1 85 31    STA &31
23e3 60       RTS     
23e4 20 fe a3 JSR &a3fe
23e7 a5 31    LDA &31
23e9 85 2d    STA &2d
23eb a5 32    LDA &32
23ed 85 2c    STA &2c
23ef a5 33    LDA &33
23f1 85 2b    STA &2b
23f3 a5 34    LDA &34
23f5 85 2a    STA &2a
23f7 60       RTS     
23f8 20 1e a2 JSR &a21e
23fb 4c 86 a6 JMP &a686
23fe a5 30    LDA &30
2400 10 f6    BPL &23f8
2402 20 53 a4 JSR &a453
2405 20 da a1 JSR &a1da
2408 d0 32    BNE &243c
240a f0 5c    BEQ &2468
240c a5 30    LDA &30
240e c9 a0    CMP #&a0
2410 b0 54    BCS &2466
2412 c9 99    CMP #&99
2414 b0 26    BCS &243c
2416 69 08    ADC #&08
2418 85 30    STA &30
241a a5 40    LDA &40
241c 85 41    STA &41
241e a5 3f    LDA &3f
2420 85 40    STA &40
2422 a5 3e    LDA &3e
2424 85 3f    STA &3f
2426 a5 34    LDA &34
2428 85 3e    STA &3e
242a a5 33    LDA &33
242c 85 34    STA &34
242e a5 32    LDA &32
2430 85 33    STA &33
2432 a5 31    LDA &31
2434 85 32    STA &32
2436 a9 00    LDA #&00
2438 85 31    STA &31
243a f0 d0    BEQ &240c
243c 46 31    LSR &31
243e 66 32    ROR &32
2440 66 33    ROR &33
2442 66 34    ROR &34
2444 66 3e    ROR &3e
2446 66 3f    ROR &3f
2448 66 40    ROR &40
244a 66 41    ROR &41
244c e6 30    INC &30
244e d0 bc    BNE &240c
2450 4c 6c a6 JMP &a66c
2453 a9 00    LDA #&00
2455 85 3b    STA &3b
2457 85 3c    STA &3c
2459 85 3d    STA &3d
245b 85 3e    STA &3e
245d 85 3f    STA &3f
245f 85 40    STA &40
2461 85 41    STA &41
2463 85 42    STA &42
2465 60       RTS     
2466 d0 e8    BNE &2450
2468 a5 2e    LDA &2e
246a 10 19    BPL &2485
246c 38       SEC     
246d a9 00    LDA #&00
246f e5 34    SBC &34
2471 85 34    STA &34
2473 a9 00    LDA #&00
2475 e5 33    SBC &33
2477 85 33    STA &33
2479 a9 00    LDA #&00
247b e5 32    SBC &32
247d 85 32    STA &32
247f a9 00    LDA #&00
2481 e5 31    SBC &31
2483 85 31    STA &31
2485 60       RTS     
2486 a5 30    LDA &30
2488 30 07    BMI &2491
248a a9 00    LDA #&00
248c 85 4a    STA &4a
248e 4c da a1 JMP &a1da
2491 20 fe a3 JSR &a3fe
2494 a5 34    LDA &34
2496 85 4a    STA &4a
2498 20 e8 a4 JSR &a4e8
249b a9 80    LDA #&80
249d 85 30    STA &30
249f a6 31    LDX &31
24a1 10 10    BPL &24b3
24a3 45 2e    EOR &2e
24a5 85 2e    STA &2e
24a7 10 05    BPL &24ae
24a9 e6 4a    INC &4a
24ab 4c b0 a4 JMP &a4b0
24ae c6 4a    DEC &4a
24b0 20 6c a4 JSR &a46c
24b3 4c 03 a3 JMP &a303
24b6 e6 34    INC &34
24b8 d0 0c    BNE &24c6
24ba e6 33    INC &33
24bc d0 08    BNE &24c6
24be e6 32    INC &32
24c0 d0 04    BNE &24c6
24c2 e6 31    INC &31
24c4 f0 8a    BEQ &2450
24c6 60       RTS     
24c7 20 6c a4 JSR &a46c
24ca 20 b6 a4 JSR &a4b6
24cd 4c 6c a4 JMP &a46c
24d0 20 fd a4 JSR &a4fd
24d3 4c 7e ad JMP &ad7e
24d6 20 4e a3 JSR &a34e
24d9 20 8d a3 JSR &a38d
24dc a5 3b    LDA &3b
24de 85 2e    STA &2e
24e0 a5 3c    LDA &3c
24e2 85 2f    STA &2f
24e4 a5 3d    LDA &3d
24e6 85 30    STA &30
24e8 a5 3e    LDA &3e
24ea 85 31    STA &31
24ec a5 3f    LDA &3f
24ee 85 32    STA &32
24f0 a5 40    LDA &40
24f2 85 33    STA &33
24f4 a5 41    LDA &41
24f6 85 34    STA &34
24f8 a5 42    LDA &42
24fa 85 35    STA &35
24fc 60       RTS     
24fd 20 7e ad JSR &ad7e
2500 20 4e a3 JSR &a34e
2503 f0 f7    BEQ &24fc
2505 20 0b a5 JSR &a50b
2508 4c 5c a6 JMP &a65c
250b 20 da a1 JSR &a1da
250e f0 cc    BEQ &24dc
2510 a0 00    LDY #&00
2512 38       SEC     
2513 a5 30    LDA &30
2515 e5 3d    SBC &3d
2517 f0 77    BEQ &2590
2519 90 37    BCC &2552
251b c9 25    CMP #&25
251d b0 dd    BCS &24fc
251f 48       PHA     
2520 29 38    AND #&38
2522 f0 19    BEQ &253d
2524 4a       LSR A
2525 4a       LSR A
2526 4a       LSR A
2527 aa       TAX     
2528 a5 41    LDA &41
252a 85 42    STA &42
252c a5 40    LDA &40
252e 85 41    STA &41
2530 a5 3f    LDA &3f
2532 85 40    STA &40
2534 a5 3e    LDA &3e
2536 85 3f    STA &3f
2538 84 3e    STY &3e
253a ca       DEX     
253b d0 eb    BNE &2528
253d 68       PLA     
253e 29 07    AND #&07
2540 f0 4e    BEQ &2590
2542 aa       TAX     
2543 46 3e    LSR &3e
2545 66 3f    ROR &3f
2547 66 40    ROR &40
2549 66 41    ROR &41
254b 66 42    ROR &42
254d ca       DEX     
254e d0 f3    BNE &2543
2550 f0 3e    BEQ &2590
2552 38       SEC     
2553 a5 3d    LDA &3d
2555 e5 30    SBC &30
2557 c9 25    CMP #&25
2559 b0 81    BCS &24dc
255b 48       PHA     
255c 29 38    AND #&38
255e f0 19    BEQ &2579
2560 4a       LSR A
2561 4a       LSR A
2562 4a       LSR A
2563 aa       TAX     
2564 a5 34    LDA &34
2566 85 35    STA &35
2568 a5 33    LDA &33
256a 85 34    STA &34
256c a5 32    LDA &32
256e 85 33    STA &33
2570 a5 31    LDA &31
2572 85 32    STA &32
2574 84 31    STY &31
2576 ca       DEX     
2577 d0 eb    BNE &2564
2579 68       PLA     
257a 29 07    AND #&07
257c f0 0e    BEQ &258c
257e aa       TAX     
257f 46 31    LSR &31
2581 66 32    ROR &32
2583 66 33    ROR &33
2585 66 34    ROR &34
2587 66 35    ROR &35
2589 ca       DEX     
258a d0 f3    BNE &257f
258c a5 3d    LDA &3d
258e 85 30    STA &30
2590 a5 2e    LDA &2e
2592 45 3b    EOR &3b
2594 10 49    BPL &25df
2596 a5 31    LDA &31
2598 c5 3e    CMP &3e
259a d0 1b    BNE &25b7
259c a5 32    LDA &32
259e c5 3f    CMP &3f
25a0 d0 15    BNE &25b7
25a2 a5 33    LDA &33
25a4 c5 40    CMP &40
25a6 d0 0f    BNE &25b7
25a8 a5 34    LDA &34
25aa c5 41    CMP &41
25ac d0 09    BNE &25b7
25ae a5 35    LDA &35
25b0 c5 42    CMP &42
25b2 d0 03    BNE &25b7
25b4 4c 86 a6 JMP &a686
25b7 b0 2a    BCS &25e3
25b9 38       SEC     
25ba a5 42    LDA &42
25bc e5 35    SBC &35
25be 85 35    STA &35
25c0 a5 41    LDA &41
25c2 e5 34    SBC &34
25c4 85 34    STA &34
25c6 a5 40    LDA &40
25c8 e5 33    SBC &33
25ca 85 33    STA &33
25cc a5 3f    LDA &3f
25ce e5 32    SBC &32
25d0 85 32    STA &32
25d2 a5 3e    LDA &3e
25d4 e5 31    SBC &31
25d6 85 31    STA &31
25d8 a5 3b    LDA &3b
25da 85 2e    STA &2e
25dc 4c 03 a3 JMP &a303
25df 18       CLC     
25e0 4c 08 a2 JMP &a208
25e3 38       SEC     
25e4 a5 35    LDA &35
25e6 e5 42    SBC &42
25e8 85 35    STA &35
25ea a5 34    LDA &34
25ec e5 41    SBC &41
25ee 85 34    STA &34
25f0 a5 33    LDA &33
25f2 e5 40    SBC &40
25f4 85 33    STA &33
25f6 a5 32    LDA &32
25f8 e5 3f    SBC &3f
25fa 85 32    STA &32
25fc a5 31    LDA &31
25fe e5 3e    SBC &3e
2600 85 31    STA &31
2602 4c 03 a3 JMP &a303
2605 60       RTS     
2606 20 da a1 JSR &a1da
2609 f0 fa    BEQ &2605
260b 20 4e a3 JSR &a34e
260e d0 03    BNE &2613
2610 4c 86 a6 JMP &a686
2613 18       CLC     
2614 a5 30    LDA &30
2616 65 3d    ADC &3d
2618 90 03    BCC &261d
261a e6 2f    INC &2f
261c 18       CLC     
261d e9 7f    SBC #&7f
261f 85 30    STA &30
2621 b0 02    BCS &2625
2623 c6 2f    DEC &2f
2625 a2 05    LDX #&05
2627 a0 00    LDY #&00
2629 b5 30    LDA &30,X
262b 95 42    STA &42,X
262d 94 30    STY &30,X
262f ca       DEX     
2630 d0 f7    BNE &2629
2632 a5 2e    LDA &2e
2634 45 3b    EOR &3b
2636 85 2e    STA &2e
2638 a0 20    LDY #&20
263a 46 3e    LSR &3e
263c 66 3f    ROR &3f
263e 66 40    ROR &40
2640 66 41    ROR &41
2642 66 42    ROR &42
2644 06 46    ASL &46
2646 26 45    ROL &45
2648 26 44    ROL &44
264a 26 43    ROL &43
264c 90 04    BCC &2652
264e 18       CLC     
264f 20 78 a1 JSR &a178
2652 88       DEY     
2653 d0 e5    BNE &263a
2655 60       RTS     
2656 20 06 a6 JSR &a606
2659 20 03 a3 JSR &a303
265c a5 35    LDA &35
265e c9 80    CMP #&80
2660 90 1a    BCC &267c
2662 f0 12    BEQ &2676
2664 a9 ff    LDA #&ff
2666 20 a4 a2 JSR &a2a4
2669 4c 7c a6 JMP &a67c
266c 00       BRK     
266d 14       ???
266e 54       ???
266f 6f       ???
2670 6f       ???
2671 20 62 69 JSR &6962
2674 67       ???
2675 00       BRK     
2676 a5 34    LDA &34
2678 09 01    ORA #&01
267a 85 34    STA &34
267c a9 00    LDA #&00
267e 85 35    STA &35
2680 a5 2f    LDA &2f
2682 f0 14    BEQ &2698
2684 10 e6    BPL &266c
2686 a9 00    LDA #&00
2688 85 2e    STA &2e
268a 85 2f    STA &2f
268c 85 30    STA &30
268e 85 31    STA &31
2690 85 32    STA &32
2692 85 33    STA &33
2694 85 34    STA &34
2696 85 35    STA &35
2698 60       RTS     
2699 20 86 a6 JSR &a686
269c a0 80    LDY #&80
269e 84 31    STY &31
26a0 c8       INY     
26a1 84 30    STY &30
26a3 98       TYA     
26a4 60       RTS     
26a5 20 85 a3 JSR &a385
26a8 20 99 a6 JSR &a699
26ab d0 3a    BNE &26e7
26ad 20 da a1 JSR &a1da
26b0 f0 09    BEQ &26bb
26b2 20 1e a2 JSR &a21e
26b5 20 b5 a3 JSR &a3b5
26b8 d0 37    BNE &26f1
26ba 60       RTS     
26bb 4c a7 99 JMP &99a7
26be 20 fa 92 JSR &92fa
26c1 20 d3 a9 JSR &a9d3
26c4 a5 4a    LDA &4a
26c6 48       PHA     
26c7 20 e9 a7 JSR &a7e9
26ca 20 8d a3 JSR &a38d
26cd e6 4a    INC &4a
26cf 20 9e a9 JSR &a99e
26d2 20 e9 a7 JSR &a7e9
26d5 20 d6 a4 JSR &a4d6
26d8 68       PLA     
26d9 85 4a    STA &4a
26db 20 9e a9 JSR &a99e
26de 20 e9 a7 JSR &a7e9
26e1 20 e7 a6 JSR &a6e7
26e4 a9 ff    LDA #&ff
26e6 60       RTS     
26e7 20 da a1 JSR &a1da
26ea f0 ac    BEQ &2698
26ec 20 4e a3 JSR &a34e
26ef f0 ca    BEQ &26bb
26f1 a5 2e    LDA &2e
26f3 45 3b    EOR &3b
26f5 85 2e    STA &2e
26f7 38       SEC     
26f8 a5 30    LDA &30
26fa e5 3d    SBC &3d
26fc b0 03    BCS &2701
26fe c6 2f    DEC &2f
2700 38       SEC     
2701 69 80    ADC #&80
2703 85 30    STA &30
2705 90 03    BCC &270a
2707 e6 2f    INC &2f
2709 18       CLC     
270a a2 20    LDX #&20
270c b0 18    BCS &2726
270e a5 31    LDA &31
2710 c5 3e    CMP &3e
2712 d0 10    BNE &2724
2714 a5 32    LDA &32
2716 c5 3f    CMP &3f
2718 d0 0a    BNE &2724
271a a5 33    LDA &33
271c c5 40    CMP &40
271e d0 04    BNE &2724
2720 a5 34    LDA &34
2722 c5 41    CMP &41
2724 90 19    BCC &273f
2726 a5 34    LDA &34
2728 e5 41    SBC &41
272a 85 34    STA &34
272c a5 33    LDA &33
272e e5 40    SBC &40
2730 85 33    STA &33
2732 a5 32    LDA &32
2734 e5 3f    SBC &3f
2736 85 32    STA &32
2738 a5 31    LDA &31
273a e5 3e    SBC &3e
273c 85 31    STA &31
273e 38       SEC     
273f 26 46    ROL &46
2741 26 45    ROL &45
2743 26 44    ROL &44
2745 26 43    ROL &43
2747 06 34    ASL &34
2749 26 33    ROL &33
274b 26 32    ROL &32
274d 26 31    ROL &31
274f ca       DEX     
2750 d0 ba    BNE &270c
2752 a2 07    LDX #&07
2754 b0 18    BCS &276e
2756 a5 31    LDA &31
2758 c5 3e    CMP &3e
275a d0 10    BNE &276c
275c a5 32    LDA &32
275e c5 3f    CMP &3f
2760 d0 0a    BNE &276c
2762 a5 33    LDA &33
2764 c5 40    CMP &40
2766 d0 04    BNE &276c
2768 a5 34    LDA &34
276a c5 41    CMP &41
276c 90 19    BCC &2787
276e a5 34    LDA &34
2770 e5 41    SBC &41
2772 85 34    STA &34
2774 a5 33    LDA &33
2776 e5 40    SBC &40
2778 85 33    STA &33
277a a5 32    LDA &32
277c e5 3f    SBC &3f
277e 85 32    STA &32
2780 a5 31    LDA &31
2782 e5 3e    SBC &3e
2784 85 31    STA &31
2786 38       SEC     
2787 26 35    ROL &35
2789 06 34    ASL &34
278b 26 33    ROL &33
278d 26 32    ROL &32
278f 26 31    ROL &31
2791 ca       DEX     
2792 d0 c0    BNE &2754
2794 06 35    ASL &35
2796 a5 46    LDA &46
2798 85 34    STA &34
279a a5 45    LDA &45
279c 85 33    STA &33
279e a5 44    LDA &44
27a0 85 32    STA &32
27a2 a5 43    LDA &43
27a4 85 31    STA &31
27a6 4c 59 a6 JMP &a659
27a9 00       BRK     
27aa 15 2d    ORA &2d,X
27ac 76 65    ROR &65,X
27ae 20 72 6f JSR &6f72
27b1 6f       ???
27b2 74       ???
27b3 00       BRK     
27b4 20 fa 92 JSR &92fa
27b7 20 da a1 JSR &a1da
27ba f0 2a    BEQ &27e6
27bc 30 eb    BMI &27a9
27be 20 85 a3 JSR &a385
27c1 a5 30    LDA &30
27c3 4a       LSR A
27c4 69 40    ADC #&40
27c6 85 30    STA &30
27c8 a9 05    LDA #&05
27ca 85 4a    STA &4a
27cc 20 ed a7 JSR &a7ed
27cf 20 8d a3 JSR &a38d
27d2 a9 6c    LDA #&6c
27d4 85 4b    STA &4b
27d6 20 ad a6 JSR &a6ad
27d9 a9 71    LDA #&71
27db 85 4b    STA &4b
27dd 20 00 a5 JSR &a500
27e0 c6 30    DEC &30
27e2 c6 4a    DEC &4a
27e4 d0 e9    BNE &27cf
27e6 a9 ff    LDA #&ff
27e8 60       RTS     
27e9 a9 7b    LDA #&7b
27eb d0 0a    BNE &27f7
27ed a9 71    LDA #&71
27ef d0 06    BNE &27f7
27f1 a9 76    LDA #&76
27f3 d0 02    BNE &27f7
27f5 a9 6c    LDA #&6c
27f7 85 4b    STA &4b
27f9 a9 04    LDA #&04
27fb 85 4c    STA &4c
27fd 60       RTS     
27fe 20 fa 92 JSR &92fa
2801 20 da a1 JSR &a1da
2804 f0 02    BEQ &2808
2806 10 0c    BPL &2814
2808 00       BRK     
2809 16 4c    ASL &4c,X
280b 6f       ???
280c 67       ???
280d 20 72 61 JSR &6172
2810 6e 67 65 ROR &6567
2813 00       BRK     
2814 20 53 a4 JSR &a453
2817 a0 80    LDY #&80
2819 84 3b    STY &3b
281b 84 3e    STY &3e
281d c8       INY     
281e 84 3d    STY &3d
2820 a6 30    LDX &30
2822 f0 06    BEQ &282a
2824 a5 31    LDA &31
2826 c9 b5    CMP #&b5
2828 90 02    BCC &282c
282a e8       INX     
282b 88       DEY     
282c 8a       TXA     
282d 48       PHA     
282e 84 30    STY &30
2830 20 05 a5 JSR &a505
2833 a9 7b    LDA #&7b
2835 20 87 a3 JSR &a387
2838 a9 73    LDA #&73
283a a0 a8    LDY #&a8
283c 20 97 a8 JSR &a897
283f 20 e9 a7 JSR &a7e9
2842 20 56 a6 JSR &a656
2845 20 56 a6 JSR &a656
2848 20 00 a5 JSR &a500
284b 20 85 a3 JSR &a385
284e 68       PLA     
284f 38       SEC     
2850 e9 81    SBC #&81
2852 20 ed a2 JSR &a2ed
2855 a9 6e    LDA #&6e
2857 85 4b    STA &4b
2859 a9 a8    LDA #&a8
285b 85 4c    STA &4c
285d 20 56 a6 JSR &a656
2860 20 f5 a7 JSR &a7f5
2863 20 00 a5 JSR &a500
2866 a9 ff    LDA #&ff
2868 60       RTS     
2869 7f       ???
286a 5e 5b d8 LSR &d85b,X
286d aa       TAX     
286e 80       ???
286f 31 72    AND (&72),Y
2871 17       ???
2872 f8       SED     
2873 06 7a    ASL &7a
2875 12       ???
2876 38       SEC     
2877 a5 0b    LDA &0b
2879 88       DEY     
287a 79 0e 9f ADC &9f0e,Y
287d f3       ???
287e 7c       ???
287f 2a       ROL A
2880 ac 3f b5 LDY &b53f
2883 86 34    STX &34
2885 01 a2    ORA (&a2;X)
2887 7a       ???
2888 7f       ???
2889 63       ???
288a 8e 37 ec STX &ec37
288d 82       ???
288e 3f       ???
288f ff       ???
2890 ff       ???
2891 c1 7f    CMP (&7f;X)
2893 ff       ???
2894 ff       ???
2895 ff       ???
2896 ff       ???
2897 85 4d    STA &4d
2899 84 4e    STY &4e
289b 20 85 a3 JSR &a385
289e a0 00    LDY #&00
28a0 b1 4d    LDA (&4d),Y
28a2 85 48    STA &48
28a4 e6 4d    INC &4d
28a6 d0 02    BNE &28aa
28a8 e6 4e    INC &4e
28aa a5 4d    LDA &4d
28ac 85 4b    STA &4b
28ae a5 4e    LDA &4e
28b0 85 4c    STA &4c
28b2 20 b5 a3 JSR &a3b5
28b5 20 f5 a7 JSR &a7f5
28b8 20 ad a6 JSR &a6ad
28bb 18       CLC     
28bc a5 4d    LDA &4d
28be 69 05    ADC #&05
28c0 85 4d    STA &4d
28c2 85 4b    STA &4b
28c4 a5 4e    LDA &4e
28c6 69 00    ADC #&00
28c8 85 4e    STA &4e
28ca 85 4c    STA &4c
28cc 20 00 a5 JSR &a500
28cf c6 48    DEC &48
28d1 d0 e2    BNE &28b5
28d3 60       RTS     
28d4 20 da a8 JSR &a8da
28d7 4c 27 a9 JMP &a927
28da 20 fa 92 JSR &92fa
28dd 20 da a1 JSR &a1da
28e0 10 08    BPL &28ea
28e2 46 2e    LSR &2e
28e4 20 ea a8 JSR &a8ea
28e7 4c 16 a9 JMP &a916
28ea 20 81 a3 JSR &a381
28ed 20 b1 a9 JSR &a9b1
28f0 20 da a1 JSR &a1da
28f3 f0 09    BEQ &28fe
28f5 20 f1 a7 JSR &a7f1
28f8 20 ad a6 JSR &a6ad
28fb 4c 0a a9 JMP &a90a
28fe 20 55 aa JSR &aa55
2901 20 b5 a3 JSR &a3b5
2904 a9 ff    LDA #&ff
2906 60       RTS     
2907 20 fa 92 JSR &92fa
290a 20 da a1 JSR &a1da
290d f0 f5    BEQ &2904
290f 10 0a    BPL &291b
2911 46 2e    LSR &2e
2913 20 1b a9 JSR &a91b
2916 a9 80    LDA #&80
2918 85 2e    STA &2e
291a 60       RTS     
291b a5 30    LDA &30
291d c9 81    CMP #&81
291f 90 15    BCC &2936
2921 20 a5 a6 JSR &a6a5
2924 20 36 a9 JSR &a936
2927 20 48 aa JSR &aa48
292a 20 00 a5 JSR &a500
292d 20 4c aa JSR &aa4c
2930 20 00 a5 JSR &a500
2933 4c 7e ad JMP &ad7e
2936 a5 30    LDA &30
2938 c9 73    CMP #&73
293a 90 c8    BCC &2904
293c 20 81 a3 JSR &a381
293f 20 53 a4 JSR &a453
2942 a9 80    LDA #&80
2944 85 3d    STA &3d
2946 85 3e    STA &3e
2948 85 3b    STA &3b
294a 20 05 a5 JSR &a505
294d a9 5a    LDA #&5a
294f a0 a9    LDY #&a9
2951 20 97 a8 JSR &a897
2954 20 d1 aa JSR &aad1
2957 a9 ff    LDA #&ff
2959 60       RTS     
295a 09 85    ORA #&85
295c a3       ???
295d 59 e8 67 EOR &67e8,Y
2960 80       ???
2961 1c       ???
2962 9d 07 36 STA &3607,X
2965 80       ???
2966 57       ???
2967 bb       ???
2968 78       SEI     
2969 df       ???
296a 80       ???
296b ca       DEX     
296c 9a       TXS     
296d 0e 83 84 ASL &8483
2970 8c bb ca STY &cabb
2973 6e 81 95 ROR &9581
2976 96 06    STX &06,Y
2978 de 81 0a DEC &0a81,X
297b c7       ???
297c 6c 52 7f JMP (&7f52)
297f 7d ad 90 ADC &90ad,X
2982 a1 82    LDA (&82;X)
2984 fb       ???
2985 62       ???
2986 57       ???
2987 2f       ???
2988 80       ???
2989 6d 63 38 ADC &3863
298c 2c 20 fa BIT &fa20
298f 92       ???
2990 20 d3 a9 JSR &a9d3
2993 e6 4a    INC &4a
2995 4c 9e a9 JMP &a99e
2998 20 fa 92 JSR &92fa
299b 20 d3 a9 JSR &a9d3
299e a5 4a    LDA &4a
29a0 29 02    AND #&02
29a2 f0 06    BEQ &29aa
29a4 20 aa a9 JSR &a9aa
29a7 4c 7e ad JMP &ad7e
29aa 46 4a    LSR &4a
29ac 90 15    BCC &29c3
29ae 20 c3 a9 JSR &a9c3
29b1 20 85 a3 JSR &a385
29b4 20 56 a6 JSR &a656
29b7 20 8d a3 JSR &a38d
29ba 20 99 a6 JSR &a699
29bd 20 d0 a4 JSR &a4d0
29c0 4c b7 a7 JMP &a7b7
29c3 20 81 a3 JSR &a381
29c6 20 56 a6 JSR &a656
29c9 a9 72    LDA #&72
29cb a0 aa    LDY #&aa
29cd 20 97 a8 JSR &a897
29d0 4c d1 aa JMP &aad1
29d3 a5 30    LDA &30
29d5 c9 98    CMP #&98
29d7 b0 5f    BCS &2a38
29d9 20 85 a3 JSR &a385
29dc 20 55 aa JSR &aa55
29df 20 4e a3 JSR &a34e
29e2 a5 2e    LDA &2e
29e4 85 3b    STA &3b
29e6 c6 3d    DEC &3d
29e8 20 05 a5 JSR &a505
29eb 20 e7 a6 JSR &a6e7
29ee 20 fe a3 JSR &a3fe
29f1 a5 34    LDA &34
29f3 85 4a    STA &4a
29f5 05 33    ORA &33
29f7 05 32    ORA &32
29f9 05 31    ORA &31
29fb f0 38    BEQ &2a35
29fd a9 a0    LDA #&a0
29ff 85 30    STA &30
2a01 a0 00    LDY #&00
2a03 84 35    STY &35
2a05 a5 31    LDA &31
2a07 85 2e    STA &2e
2a09 10 03    BPL &2a0e
2a0b 20 6c a4 JSR &a46c
2a0e 20 03 a3 JSR &a303
2a11 20 7d a3 JSR &a37d
2a14 20 48 aa JSR &aa48
2a17 20 56 a6 JSR &a656
2a1a 20 f5 a7 JSR &a7f5
2a1d 20 00 a5 JSR &a500
2a20 20 8d a3 JSR &a38d
2a23 20 ed a7 JSR &a7ed
2a26 20 b5 a3 JSR &a3b5
2a29 20 4c aa JSR &aa4c
2a2c 20 56 a6 JSR &a656
2a2f 20 f5 a7 JSR &a7f5
2a32 4c 00 a5 JMP &a500
2a35 4c b2 a3 JMP &a3b2
2a38 00       BRK     
2a39 17       ???
2a3a 41 63    EOR (&63;X)
2a3c 63       ???
2a3d 75 72    ADC &72,X
2a3f 61 63    ADC (&63;X)
2a41 79 20 6c ADC &6c20,Y
2a44 6f       ???
2a45 73       ???
2a46 74       ???
2a47 00       BRK     
2a48 a9 59    LDA #&59
2a4a d0 02    BNE &2a4e
2a4c a9 5e    LDA #&5e
2a4e 85 4b    STA &4b
2a50 a9 aa    LDA #&aa
2a52 85 4c    STA &4c
2a54 60       RTS     
2a55 a9 63    LDA #&63
2a57 d0 f5    BNE &2a4e
2a59 81 c9    STA (&c9;X)
2a5b 10 00    BPL &2a5d
2a5d 00       BRK     
2a5e 6f       ???
2a5f 15 77    ORA &77,X
2a61 7a       ???
2a62 61 81    ADC (&81;X)
2a64 49 0f    EOR #&0f
2a66 da       ???
2a67 a2 7b    LDX #&7b
2a69 0e fa 35 ASL &35fa
2a6c 12       ???
2a6d 86 65    STX &65
2a6f 2e e0 d3 ROL &d3e0
2a72 05 84    ORA &84
2a74 8a       TXA     
2a75 ea       NOP     
2a76 0c       ???
2a77 1b       ???
2a78 84 1a    STY &1a
2a7a be bb 2b LDX &2bbb,Y
2a7d 84 37    STY &37
2a7f 45 55    EOR &55
2a81 ab       ???
2a82 82       ???
2a83 d5 55    CMP &55,X
2a85 57       ???
2a86 7c       ???
2a87 83       ???
2a88 c0 00    CPY #&00
2a8a 00       BRK     
2a8b 05 81    ORA &81
2a8d 00       BRK     
2a8e 00       BRK     
2a8f 00       BRK     
2a90 00       BRK     
2a91 20 fa 92 JSR &92fa
2a94 a5 30    LDA &30
2a96 c9 87    CMP #&87
2a98 90 1e    BCC &2ab8
2a9a d0 06    BNE &2aa2
2a9c a4 31    LDY &31
2a9e c0 b3    CPY #&b3
2aa0 90 16    BCC &2ab8
2aa2 a5 2e    LDA &2e
2aa4 10 06    BPL &2aac
2aa6 20 86 a6 JSR &a686
2aa9 a9 ff    LDA #&ff
2aab 60       RTS     
2aac 00       BRK     
2aad 18       CLC     
2aae 45 78    EOR &78
2ab0 70 20    BVS &2ad2
2ab2 72       ???
2ab3 61 6e    ADC (&6e;X)
2ab5 67       ???
2ab6 65 00    ADC &00
2ab8 20 86 a4 JSR &a486
2abb 20 da aa JSR &aada
2abe 20 81 a3 JSR &a381
2ac1 a9 e4    LDA #&e4
2ac3 85 4b    STA &4b
2ac5 a9 aa    LDA #&aa
2ac7 85 4c    STA &4c
2ac9 20 b5 a3 JSR &a3b5
2acc a5 4a    LDA &4a
2ace 20 12 ab JSR &ab12
2ad1 20 f1 a7 JSR &a7f1
2ad4 20 56 a6 JSR &a656
2ad7 a9 ff    LDA #&ff
2ad9 60       RTS     
2ada a9 e9    LDA #&e9
2adc a0 aa    LDY #&aa
2ade 20 97 a8 JSR &a897
2ae1 a9 ff    LDA #&ff
2ae3 60       RTS     
2ae4 82       ???
2ae5 2d f8 54 AND &54f8
2ae8 58       CLI     
2ae9 07       ???
2aea 83       ???
2aeb e0 20    CPX #&20
2aed 86 5b    STX &5b
2aef 82       ???
2af0 80       ???
2af1 53       ???
2af2 93       ???
2af3 b8       CLV     
2af4 83       ???
2af5 20 00 06 JSR &0600
2af8 a1 82    LDA (&82;X)
2afa 00       BRK     
2afb 00       BRK     
2afc 21 63    AND (&63;X)
2afe 82       ???
2aff c0 00    CPY #&00
2b01 00       BRK     
2b02 02       ???
2b03 82       ???
2b04 80       ???
2b05 00       BRK     
2b06 00       BRK     
2b07 0c       ???
2b08 81 00    STA (&00;X)
2b0a 00       BRK     
2b0b 00       BRK     
2b0c 00       BRK     
2b0d 81 00    STA (&00;X)
2b0f 00       BRK     
2b10 00       BRK     
2b11 00       BRK     
2b12 aa       TAX     
2b13 10 09    BPL &2b1e
2b15 ca       DEX     
2b16 8a       TXA     
2b17 49 ff    EOR #&ff
2b19 48       PHA     
2b1a 20 a5 a6 JSR &a6a5
2b1d 68       PLA     
2b1e 48       PHA     
2b1f 20 85 a3 JSR &a385
2b22 20 99 a6 JSR &a699
2b25 68       PLA     
2b26 f0 0a    BEQ &2b32
2b28 38       SEC     
2b29 e9 01    SBC #&01
2b2b 48       PHA     
2b2c 20 56 a6 JSR &a656
2b2f 4c 25 ab JMP &ab25
2b32 60       RTS     
2b33 20 e3 92 JSR &92e3
2b36 a6 2a    LDX &2a
2b38 a9 80    LDA #&80
2b3a 20 f4 ff JSR &fff4
2b3d 8a       TXA     
2b3e 4c ea ae JMP &aeea
2b41 20 dd 92 JSR &92dd
2b44 20 94 bd JSR &bd94
2b47 20 ae 8a JSR &8aae
2b4a 20 56 ae JSR &ae56
2b4d 20 f0 92 JSR &92f0
2b50 a5 2a    LDA &2a
2b52 48       PHA     
2b53 a5 2b    LDA &2b
2b55 48       PHA     
2b56 20 ea bd JSR &bdea
2b59 68       PLA     
2b5a 85 2d    STA &2d
2b5c 68       PLA     
2b5d 85 2c    STA &2c
2b5f a2 2a    LDX #&2a
2b61 a9 09    LDA #&09
2b63 20 f1 ff JSR &fff1
2b66 a5 2e    LDA &2e
2b68 30 33    BMI &2b9d
2b6a 4c d8 ae JMP &aed8
2b6d a9 86    LDA #&86
2b6f 20 f4 ff JSR &fff4
2b72 8a       TXA     
2b73 4c d8 ae JMP &aed8
2b76 a9 86    LDA #&86
2b78 20 f4 ff JSR &fff4
2b7b 98       TYA     
2b7c 4c d8 ae JMP &aed8
2b7f 20 da a1 JSR &a1da
2b82 f0 1e    BEQ &2ba2
2b84 10 1a    BPL &2ba0
2b86 30 15    BMI &2b9d
2b88 20 ec ad JSR &adec
2b8b f0 59    BEQ &2be6
2b8d 30 f0    BMI &2b7f
2b8f a5 2d    LDA &2d
2b91 05 2c    ORA &2c
2b93 05 2b    ORA &2b
2b95 05 2a    ORA &2a
2b97 f0 0c    BEQ &2ba5
2b99 a5 2d    LDA &2d
2b9b 10 03    BPL &2ba0
2b9d 4c c4 ac JMP &acc4
2ba0 a9 01    LDA #&01
2ba2 4c d8 ae JMP &aed8
2ba5 a9 40    LDA #&40
2ba7 60       RTS     
2ba8 20 fe a7 JSR &a7fe
2bab a0 69    LDY #&69
2bad a9 a8    LDA #&a8
2baf d0 07    BNE &2bb8
2bb1 20 fa 92 JSR &92fa
2bb4 a0 68    LDY #&68
2bb6 a9 aa    LDA #&aa
2bb8 84 4b    STY &4b
2bba 85 4c    STA &4c
2bbc 20 56 a6 JSR &a656
2bbf a9 ff    LDA #&ff
2bc1 60       RTS     
2bc2 20 fa 92 JSR &92fa
2bc5 a0 6d    LDY #&6d
2bc7 a9 aa    LDA #&aa
2bc9 d0 ed    BNE &2bb8
2bcb 20 fe a8 JSR &a8fe
2bce e6 30    INC &30
2bd0 a8       TAY     
2bd1 60       RTS     
2bd2 20 e3 92 JSR &92e3
2bd5 20 1e 8f JSR &8f1e
2bd8 85 2a    STA &2a
2bda 86 2b    STX &2b
2bdc 84 2c    STY &2c
2bde 08       PHP     
2bdf 68       PLA     
2be0 85 2d    STA &2d
2be2 d8       CLD     
2be3 a9 40    LDA #&40
2be5 60       RTS     
2be6 4c 0e 8c JMP &8c0e
2be9 20 ec ad JSR &adec
2bec d0 f8    BNE &2be6
2bee e6 36    INC &36
2bf0 a4 36    LDY &36
2bf2 a9 0d    LDA #&0d
2bf4 99 ff 05 STA &05ff,Y
2bf7 20 b2 bd JSR &bdb2
2bfa a5 19    LDA &19
2bfc 48       PHA     
2bfd a5 1a    LDA &1a
2bff 48       PHA     
2c00 a5 1b    LDA &1b
2c02 48       PHA     
2c03 a4 04    LDY &04
2c05 a6 05    LDX &05
2c07 c8       INY     
2c08 84 19    STY &19
2c0a 84 37    STY &37
2c0c d0 01    BNE &2c0f
2c0e e8       INX     
2c0f 86 1a    STX &1a
2c11 86 38    STX &38
2c13 a0 ff    LDY #&ff
2c15 84 3b    STY &3b
2c17 c8       INY     
2c18 84 1b    STY &1b
2c1a 20 55 89 JSR &8955
2c1d 20 29 9b JSR &9b29
2c20 20 dc bd JSR &bddc
2c23 68       PLA     
2c24 85 1b    STA &1b
2c26 68       PLA     
2c27 85 1a    STA &1a
2c29 68       PLA     
2c2a 85 19    STA &19
2c2c a5 27    LDA &27
2c2e 60       RTS     
2c2f 20 ec ad JSR &adec
2c32 d0 67    BNE &2c9b
2c34 a4 36    LDY &36
2c36 a9 00    LDA #&00
2c38 99 00 06 STA &0600,Y
2c3b a5 19    LDA &19
2c3d 48       PHA     
2c3e a5 1a    LDA &1a
2c40 48       PHA     
2c41 a5 1b    LDA &1b
2c43 48       PHA     
2c44 a9 00    LDA #&00
2c46 85 1b    STA &1b
2c48 a9 00    LDA #&00
2c4a 85 19    STA &19
2c4c a9 06    LDA #&06
2c4e 85 1a    STA &1a
2c50 20 8c 8a JSR &8a8c
2c53 c9 2d    CMP #&2d
2c55 f0 0f    BEQ &2c66
2c57 c9 2b    CMP #&2b
2c59 d0 03    BNE &2c5e
2c5b 20 8c 8a JSR &8a8c
2c5e c6 1b    DEC &1b
2c60 20 7b a0 JSR &a07b
2c63 4c 73 ac JMP &ac73
2c66 20 8c 8a JSR &8a8c
2c69 c6 1b    DEC &1b
2c6b 20 7b a0 JSR &a07b
2c6e 90 03    BCC &2c73
2c70 20 8f ad JSR &ad8f
2c73 85 27    STA &27
2c75 4c 23 ac JMP &ac23
2c78 20 ec ad JSR &adec
2c7b f0 1e    BEQ &2c9b
2c7d 10 1b    BPL &2c9a
2c7f a5 2e    LDA &2e
2c81 08       PHP     
2c82 20 fe a3 JSR &a3fe
2c85 28       PLP     
2c86 10 0d    BPL &2c95
2c88 a5 3e    LDA &3e
2c8a 05 3f    ORA &3f
2c8c 05 40    ORA &40
2c8e 05 41    ORA &41
2c90 f0 03    BEQ &2c95
2c92 20 c7 a4 JSR &a4c7
2c95 20 e7 a3 JSR &a3e7
2c98 a9 40    LDA #&40
2c9a 60       RTS     
2c9b 4c 0e 8c JMP &8c0e
2c9e 20 ec ad JSR &adec
2ca1 d0 f8    BNE &2c9b
2ca3 a5 36    LDA &36
2ca5 f0 1d    BEQ &2cc4
2ca7 ad 00 06 LDA &0600
2caa 4c d8 ae JMP &aed8
2cad 20 ad af JSR &afad
2cb0 c0 00    CPY #&00
2cb2 d0 10    BNE &2cc4
2cb4 8a       TXA     
2cb5 4c ea ae JMP &aeea
2cb8 20 b5 bf JSR &bfb5
2cbb aa       TAX     
2cbc a9 7f    LDA #&7f
2cbe 20 f4 ff JSR &fff4
2cc1 8a       TXA     
2cc2 f0 e6    BEQ &2caa
2cc4 a9 ff    LDA #&ff
2cc6 85 2a    STA &2a
2cc8 85 2b    STA &2b
2cca 85 2c    STA &2c
2ccc 85 2d    STA &2d
2cce a9 40    LDA #&40
2cd0 60       RTS     
2cd1 20 e3 92 JSR &92e3
2cd4 a2 03    LDX #&03
2cd6 b5 2a    LDA &2a,X
2cd8 49 ff    EOR #&ff
2cda 95 2a    STA &2a,X
2cdc ca       DEX     
2cdd 10 f7    BPL &2cd6
2cdf a9 40    LDA #&40
2ce1 60       RTS     
2ce2 20 29 9b JSR &9b29
2ce5 d0 b4    BNE &2c9b
2ce7 e0 2c    CPX #&2c
2ce9 d0 18    BNE &2d03
2ceb e6 1b    INC &1b
2ced 20 b2 bd JSR &bdb2
2cf0 20 29 9b JSR &9b29
2cf3 d0 a6    BNE &2c9b
2cf5 a9 01    LDA #&01
2cf7 85 2a    STA &2a
2cf9 e6 1b    INC &1b
2cfb e0 29    CPX #&29
2cfd f0 13    BEQ &2d12
2cff e0 2c    CPX #&2c
2d01 f0 03    BEQ &2d06
2d03 4c a2 8a JMP &8aa2
2d06 20 b2 bd JSR &bdb2
2d09 20 56 ae JSR &ae56
2d0c 20 f0 92 JSR &92f0
2d0f 20 cb bd JSR &bdcb
2d12 a0 00    LDY #&00
2d14 a6 2a    LDX &2a
2d16 d0 02    BNE &2d1a
2d18 a2 01    LDX #&01
2d1a 86 2a    STX &2a
2d1c 8a       TXA     
2d1d ca       DEX     
2d1e 86 2d    STX &2d
2d20 18       CLC     
2d21 65 04    ADC &04
2d23 85 37    STA &37
2d25 98       TYA     
2d26 65 05    ADC &05
2d28 85 38    STA &38
2d2a b1 04    LDA (&04),Y
2d2c 38       SEC     
2d2d e5 2d    SBC &2d
2d2f 90 21    BCC &2d52
2d31 e5 36    SBC &36
2d33 90 1d    BCC &2d52
2d35 69 00    ADC #&00
2d37 85 2b    STA &2b
2d39 20 dc bd JSR &bddc
2d3c a0 00    LDY #&00
2d3e a6 36    LDX &36
2d40 f0 0b    BEQ &2d4d
2d42 b1 37    LDA (&37),Y
2d44 d9 00 06 CMP &0600,Y
2d47 d0 10    BNE &2d59
2d49 c8       INY     
2d4a ca       DEX     
2d4b d0 f5    BNE &2d42
2d4d a5 2a    LDA &2a
2d4f 4c d8 ae JMP &aed8
2d52 20 dc bd JSR &bddc
2d55 a9 00    LDA #&00
2d57 f0 f6    BEQ &2d4f
2d59 e6 2a    INC &2a
2d5b c6 2b    DEC &2b
2d5d f0 f6    BEQ &2d55
2d5f e6 37    INC &37
2d61 d0 d9    BNE &2d3c
2d63 e6 38    INC &38
2d65 d0 d5    BNE &2d3c
2d67 4c 0e 8c JMP &8c0e
2d6a 20 ec ad JSR &adec
2d6d f0 f8    BEQ &2d67
2d6f 30 06    BMI &2d77
2d71 24 2d    BIT &2d
2d73 30 1e    BMI &2d93
2d75 10 33    BPL &2daa
2d77 20 da a1 JSR &a1da
2d7a 10 0d    BPL &2d89
2d7c 30 05    BMI &2d83
2d7e 20 da a1 JSR &a1da
2d81 f0 06    BEQ &2d89
2d83 a5 2e    LDA &2e
2d85 49 80    EOR #&80
2d87 85 2e    STA &2e
2d89 a9 ff    LDA #&ff
2d8b 60       RTS     
2d8c 20 02 ae JSR &ae02
2d8f f0 d6    BEQ &2d67
2d91 30 eb    BMI &2d7e
2d93 38       SEC     
2d94 a9 00    LDA #&00
2d96 a8       TAY     
2d97 e5 2a    SBC &2a
2d99 85 2a    STA &2a
2d9b 98       TYA     
2d9c e5 2b    SBC &2b
2d9e 85 2b    STA &2b
2da0 98       TYA     
2da1 e5 2c    SBC &2c
2da3 85 2c    STA &2c
2da5 98       TYA     
2da6 e5 2d    SBC &2d
2da8 85 2d    STA &2d
2daa a9 40    LDA #&40
2dac 60       RTS     
2dad 20 8c 8a JSR &8a8c
2db0 c9 22    CMP #&22
2db2 f0 15    BEQ &2dc9
2db4 a2 00    LDX #&00
2db6 b1 19    LDA (&19),Y
2db8 9d 00 06 STA &0600,X
2dbb c8       INY     
2dbc e8       INX     
2dbd c9 0d    CMP #&0d
2dbf f0 04    BEQ &2dc5
2dc1 c9 2c    CMP #&2c
2dc3 d0 f1    BNE &2db6
2dc5 88       DEY     
2dc6 4c e1 ad JMP &ade1
2dc9 a2 00    LDX #&00
2dcb c8       INY     
2dcc b1 19    LDA (&19),Y
2dce c9 0d    CMP #&0d
2dd0 f0 17    BEQ &2de9
2dd2 c8       INY     
2dd3 9d 00 06 STA &0600,X
2dd6 e8       INX     
2dd7 c9 22    CMP #&22
2dd9 d0 f1    BNE &2dcc
2ddb b1 19    LDA (&19),Y
2ddd c9 22    CMP #&22
2ddf f0 ea    BEQ &2dcb
2de1 ca       DEX     
2de2 86 36    STX &36
2de4 84 1b    STY &1b
2de6 a9 00    LDA #&00
2de8 60       RTS     
2de9 4c 98 8e JMP &8e98
2dec a4 1b    LDY &1b
2dee e6 1b    INC &1b
2df0 b1 19    LDA (&19),Y
2df2 c9 20    CMP #&20
2df4 f0 f6    BEQ &2dec
2df6 c9 2d    CMP #&2d
2df8 f0 92    BEQ &2d8c
2dfa c9 22    CMP #&22
2dfc f0 cb    BEQ &2dc9
2dfe c9 2b    CMP #&2b
2e00 d0 03    BNE &2e05
2e02 20 8c 8a JSR &8a8c
2e05 c9 8e    CMP #&8e
2e07 90 07    BCC &2e10
2e09 c9 c6    CMP #&c6
2e0b b0 36    BCS &2e43
2e0d 4c b1 8b JMP &8bb1
2e10 c9 3f    CMP #&3f
2e12 b0 0c    BCS &2e20
2e14 c9 2e    CMP #&2e
2e16 b0 12    BCS &2e2a
2e18 c9 26    CMP #&26
2e1a f0 51    BEQ &2e6d
2e1c c9 28    CMP #&28
2e1e f0 36    BEQ &2e56
2e20 c6 1b    DEC &1b
2e22 20 dd 95 JSR &95dd
2e25 f0 09    BEQ &2e30
2e27 4c 2c b3 JMP &b32c
2e2a 20 7b a0 JSR &a07b
2e2d 90 14    BCC &2e43
2e2f 60       RTS     
2e30 a5 28    LDA &28
2e32 29 02    AND #&02
2e34 d0 0d    BNE &2e43
2e36 b0 0b    BCS &2e43
2e38 86 1b    STX &1b
2e3a ad 40 04 LDA &0440
2e3d ac 41 04 LDY &0441
2e40 4c ea ae JMP &aeea
2e43 00       BRK     
2e44 1a       ???
2e45 4e 6f 20 LSR &206f
2e48 73       ???
2e49 75 63    ADC &63,X
2e4b 68       PLA     
2e4c 20 76 61 JSR &6176
2e4f 72       ???
2e50 69 61    ADC #&61
2e52 62       ???
2e53 6c 65 00 JMP (&0065)
2e56 20 29 9b JSR &9b29
2e59 e6 1b    INC &1b
2e5b e0 29    CPX #&29
2e5d d0 02    BNE &2e61
2e5f a8       TAY     
2e60 60       RTS     
2e61 00       BRK     
2e62 1b       ???
2e63 4d 69 73 EOR &7369
2e66 73       ???
2e67 69 6e    ADC #&6e
2e69 67       ???
2e6a 20 29 00 JSR &0029
2e6d a2 00    LDX #&00
2e6f 86 2a    STX &2a
2e71 86 2b    STX &2b
2e73 86 2c    STX &2c
2e75 86 2d    STX &2d
2e77 a4 1b    LDY &1b
2e79 b1 19    LDA (&19),Y
2e7b c9 30    CMP #&30
2e7d 90 23    BCC &2ea2
2e7f c9 3a    CMP #&3a
2e81 90 0a    BCC &2e8d
2e83 e9 37    SBC #&37
2e85 c9 0a    CMP #&0a
2e87 90 19    BCC &2ea2
2e89 c9 10    CMP #&10
2e8b b0 15    BCS &2ea2
2e8d 0a       ASL A
2e8e 0a       ASL A
2e8f 0a       ASL A
2e90 0a       ASL A
2e91 a2 03    LDX #&03
2e93 0a       ASL A
2e94 26 2a    ROL &2a
2e96 26 2b    ROL &2b
2e98 26 2c    ROL &2c
2e9a 26 2d    ROL &2d
2e9c ca       DEX     
2e9d 10 f4    BPL &2e93
2e9f c8       INY     
2ea0 d0 d7    BNE &2e79
2ea2 8a       TXA     
2ea3 10 05    BPL &2eaa
2ea5 84 1b    STY &1b
2ea7 a9 40    LDA #&40
2ea9 60       RTS     
2eaa 00       BRK     
2eab 1c       ???
2eac 42       ???
2ead 61 64    ADC (&64;X)
2eaf 20 48 45 JSR &4548
2eb2 58       CLI     
2eb3 00       BRK     
2eb4 a2 2a    LDX #&2a
2eb6 a0 00    LDY #&00
2eb8 a9 01    LDA #&01
2eba 20 f1 ff JSR &fff1
2ebd a9 40    LDA #&40
2ebf 60       RTS     
2ec0 a9 00    LDA #&00
2ec2 a4 18    LDY &18
2ec4 4c ea ae JMP &aeea
2ec7 4c 43 ae JMP &ae43
2eca a9 00    LDA #&00
2ecc f0 0a    BEQ &2ed8
2ece 4c 0e 8c JMP &8c0e
2ed1 20 ec ad JSR &adec
2ed4 d0 f8    BNE &2ece
2ed6 a5 36    LDA &36
2ed8 a0 00    LDY #&00
2eda f0 0e    BEQ &2eea
2edc a4 1b    LDY &1b
2ede b1 19    LDA (&19),Y
2ee0 c9 50    CMP #&50
2ee2 d0 e3    BNE &2ec7
2ee4 e6 1b    INC &1b
2ee6 a5 12    LDA &12
2ee8 a4 13    LDY &13
2eea 85 2a    STA &2a
2eec 84 2b    STY &2b
2eee a9 00    LDA #&00
2ef0 85 2c    STA &2c
2ef2 85 2d    STA &2d
2ef4 a9 40    LDA #&40
2ef6 60       RTS     
2ef7 a5 1e    LDA &1e
2ef9 4c d8 ae JMP &aed8
2efc a5 00    LDA &00
2efe a4 01    LDY &01
2f00 4c ea ae JMP &aeea
2f03 a5 06    LDA &06
2f05 a4 07    LDY &07
2f07 4c ea ae JMP &aeea
2f0a e6 1b    INC &1b
2f0c 20 56 ae JSR &ae56
2f0f 20 f0 92 JSR &92f0
2f12 a5 2d    LDA &2d
2f14 30 29    BMI &2f3f
2f16 05 2c    ORA &2c
2f18 05 2b    ORA &2b
2f1a d0 08    BNE &2f24
2f1c a5 2a    LDA &2a
2f1e f0 4c    BEQ &2f6c
2f20 c9 01    CMP #&01
2f22 f0 45    BEQ &2f69
2f24 20 be a2 JSR &a2be
2f27 20 51 bd JSR &bd51
2f2a 20 69 af JSR &af69
2f2d 20 7e bd JSR &bd7e
2f30 20 06 a6 JSR &a606
2f33 20 03 a3 JSR &a303
2f36 20 e4 a3 JSR &a3e4
2f39 20 22 92 JSR &9222
2f3c a9 40    LDA #&40
2f3e 60       RTS     
2f3f a2 0d    LDX #&0d
2f41 20 44 be JSR &be44
2f44 a9 40    LDA #&40
2f46 85 11    STA &11
2f48 60       RTS     
2f49 a4 1b    LDY &1b
2f4b b1 19    LDA (&19),Y
2f4d c9 28    CMP #&28
2f4f f0 b9    BEQ &2f0a
2f51 20 87 af JSR &af87
2f54 a2 0d    LDX #&0d
2f56 b5 00    LDA &00,X
2f58 85 2a    STA &2a
2f5a b5 01    LDA &01,X
2f5c 85 2b    STA &2b
2f5e b5 02    LDA &02,X
2f60 85 2c    STA &2c
2f62 b5 03    LDA &03,X
2f64 85 2d    STA &2d
2f66 a9 40    LDA #&40
2f68 60       RTS     
2f69 20 87 af JSR &af87
2f6c a2 00    LDX #&00
2f6e 86 2e    STX &2e
2f70 86 2f    STX &2f
2f72 86 35    STX &35
2f74 a9 80    LDA #&80
2f76 85 30    STA &30
2f78 b5 0d    LDA &0d,X
2f7a 95 31    STA &31,X
2f7c e8       INX     
2f7d e0 04    CPX #&04
2f7f d0 f7    BNE &2f78
2f81 20 59 a6 JSR &a659
2f84 a9 ff    LDA #&ff
2f86 60       RTS     
2f87 a0 20    LDY #&20
2f89 a5 0f    LDA &0f
2f8b 4a       LSR A
2f8c 4a       LSR A
2f8d 4a       LSR A
2f8e 45 11    EOR &11
2f90 6a       ROR A
2f91 26 0d    ROL &0d
2f93 26 0e    ROL &0e
2f95 26 0f    ROL &0f
2f97 26 10    ROL &10
2f99 26 11    ROL &11
2f9b 88       DEY     
2f9c d0 eb    BNE &2f89
2f9e 60       RTS     
2f9f a4 09    LDY &09
2fa1 a5 08    LDA &08
2fa3 4c ea ae JMP &aeea
2fa6 a0 00    LDY #&00
2fa8 b1 fd    LDA (&fd),Y
2faa 4c ea ae JMP &aeea
2fad 20 e3 92 JSR &92e3
2fb0 a9 81    LDA #&81
2fb2 a6 2a    LDX &2a
2fb4 a4 2b    LDY &2b
2fb6 4c f4 ff JMP &fff4
2fb9 20 e0 ff JSR &ffe0
2fbc 4c d8 ae JMP &aed8
2fbf 20 e0 ff JSR &ffe0
2fc2 8d 00 06 STA &0600
2fc5 a9 01    LDA #&01
2fc7 85 36    STA &36
2fc9 a9 00    LDA #&00
2fcb 60       RTS     
2fcc 20 29 9b JSR &9b29
2fcf d0 62    BNE &3033
2fd1 e0 2c    CPX #&2c
2fd3 d0 61    BNE &3036
2fd5 e6 1b    INC &1b
2fd7 20 b2 bd JSR &bdb2
2fda 20 56 ae JSR &ae56
2fdd 20 f0 92 JSR &92f0
2fe0 20 cb bd JSR &bdcb
2fe3 a5 2a    LDA &2a
2fe5 c5 36    CMP &36
2fe7 b0 02    BCS &2feb
2fe9 85 36    STA &36
2feb a9 00    LDA #&00
2fed 60       RTS     
2fee 20 29 9b JSR &9b29
2ff1 d0 40    BNE &3033
2ff3 e0 2c    CPX #&2c
2ff5 d0 3f    BNE &3036
2ff7 e6 1b    INC &1b
2ff9 20 b2 bd JSR &bdb2
2ffc 20 56 ae JSR &ae56
2fff 20 f0 92 JSR &92f0
3002 20 cb bd JSR &bdcb
3005 a5 36    LDA &36
3007 38       SEC     
3008 e5 2a    SBC &2a
300a 90 17    BCC &3023
300c f0 17    BEQ &3025
300e aa       TAX     
300f a5 2a    LDA &2a
3011 85 36    STA &36
3013 f0 10    BEQ &3025
3015 a0 00    LDY #&00
3017 bd 00 06 LDA &0600,X
301a 99 00 06 STA &0600,Y
301d e8       INX     
301e c8       INY     
301f c6 2a    DEC &2a
3021 d0 f4    BNE &3017
3023 a9 00    LDA #&00
3025 60       RTS     
3026 20 ad af JSR &afad
3029 8a       TXA     
302a c0 00    CPY #&00
302c f0 94    BEQ &2fc2
302e a9 00    LDA #&00
3030 85 36    STA &36
3032 60       RTS     
3033 4c 0e 8c JMP &8c0e
3036 4c a2 8a JMP &8aa2
3039 20 29 9b JSR &9b29
303c d0 f5    BNE &3033
303e e0 2c    CPX #&2c
3040 d0 f4    BNE &3036
3042 20 b2 bd JSR &bdb2
3045 e6 1b    INC &1b
3047 20 dd 92 JSR &92dd
304a a5 2a    LDA &2a
304c 48       PHA     
304d a9 ff    LDA #&ff
304f 85 2a    STA &2a
3051 e6 1b    INC &1b
3053 e0 29    CPX #&29
3055 f0 0a    BEQ &3061
3057 e0 2c    CPX #&2c
3059 d0 db    BNE &3036
305b 20 56 ae JSR &ae56
305e 20 f0 92 JSR &92f0
3061 20 cb bd JSR &bdcb
3064 68       PLA     
3065 a8       TAY     
3066 18       CLC     
3067 f0 06    BEQ &306f
3069 e5 36    SBC &36
306b b0 c1    BCS &302e
306d 88       DEY     
306e 98       TYA     
306f 85 2c    STA &2c
3071 aa       TAX     
3072 a0 00    LDY #&00
3074 a5 36    LDA &36
3076 38       SEC     
3077 e5 2c    SBC &2c
3079 c5 2a    CMP &2a
307b b0 02    BCS &307f
307d 85 2a    STA &2a
307f a5 2a    LDA &2a
3081 f0 ab    BEQ &302e
3083 bd 00 06 LDA &0600,X
3086 99 00 06 STA &0600,Y
3089 c8       INY     
308a e8       INX     
308b c4 2a    CPY &2a
308d d0 f4    BNE &3083
308f 84 36    STY &36
3091 a9 00    LDA #&00
3093 60       RTS     
3094 20 8c 8a JSR &8a8c
3097 a0 ff    LDY #&ff
3099 c9 7e    CMP #&7e
309b f0 04    BEQ &30a1
309d a0 00    LDY #&00
309f c6 1b    DEC &1b
30a1 98       TYA     
30a2 48       PHA     
30a3 20 ec ad JSR &adec
30a6 f0 17    BEQ &30bf
30a8 a8       TAY     
30a9 68       PLA     
30aa 85 15    STA &15
30ac ad 03 04 LDA &0403
30af d0 08    BNE &30b9
30b1 85 37    STA &37
30b3 20 f9 9e JSR &9ef9
30b6 a9 00    LDA #&00
30b8 60       RTS     
30b9 20 df 9e JSR &9edf
30bc a9 00    LDA #&00
30be 60       RTS     
30bf 4c 0e 8c JMP &8c0e
30c2 20 dd 92 JSR &92dd
30c5 20 94 bd JSR &bd94
30c8 20 ae 8a JSR &8aae
30cb 20 56 ae JSR &ae56
30ce d0 ef    BNE &30bf
30d0 20 ea bd JSR &bdea
30d3 a4 36    LDY &36
30d5 f0 1e    BEQ &30f5
30d7 a5 2a    LDA &2a
30d9 f0 1d    BEQ &30f8
30db c6 2a    DEC &2a
30dd f0 16    BEQ &30f5
30df a2 00    LDX #&00
30e1 bd 00 06 LDA &0600,X
30e4 99 00 06 STA &0600,Y
30e7 e8       INX     
30e8 c8       INY     
30e9 f0 10    BEQ &30fb
30eb e4 36    CPX &36
30ed 90 f2    BCC &30e1
30ef c6 2a    DEC &2a
30f1 d0 ec    BNE &30df
30f3 84 36    STY &36
30f5 a9 00    LDA #&00
30f7 60       RTS     
30f8 85 36    STA &36
30fa 60       RTS     
30fb 4c 03 9c JMP &9c03
30fe 68       PLA     
30ff 85 0c    STA &0c
3101 68       PLA     
3102 85 0b    STA &0b
3104 00       BRK     
3105 1d 4e 6f ORA &6f4e,X
3108 20 73 75 JSR &7573
310b 63       ???
310c 68       PLA     
310d 20 a4 2f JSR &2fa4
3110 f2       ???
3111 00       BRK     
3112 a5 18    LDA &18
3114 85 0c    STA &0c
3116 a9 00    LDA #&00
3118 85 0b    STA &0b
311a a0 01    LDY #&01
311c b1 0b    LDA (&0b),Y
311e 30 de    BMI &30fe
3120 a0 03    LDY #&03
3122 c8       INY     
3123 b1 0b    LDA (&0b),Y
3125 c9 20    CMP #&20
3127 f0 f9    BEQ &3122
3129 c9 dd    CMP #&dd
312b f0 0f    BEQ &313c
312d a0 03    LDY #&03
312f b1 0b    LDA (&0b),Y
3131 18       CLC     
3132 65 0b    ADC &0b
3134 85 0b    STA &0b
3136 90 e2    BCC &311a
3138 e6 0c    INC &0c
313a b0 de    BCS &311a
313c c8       INY     
313d 84 0a    STY &0a
313f 20 97 8a JSR &8a97
3142 98       TYA     
3143 aa       TAX     
3144 18       CLC     
3145 65 0b    ADC &0b
3147 a4 0c    LDY &0c
3149 90 02    BCC &314d
314b c8       INY     
314c 18       CLC     
314d e9 00    SBC #&00
314f 85 3c    STA &3c
3151 98       TYA     
3152 e9 00    SBC #&00
3154 85 3d    STA &3d
3156 a0 00    LDY #&00
3158 c8       INY     
3159 e8       INX     
315a b1 3c    LDA (&3c),Y
315c d1 37    CMP (&37),Y
315e d0 cd    BNE &312d
3160 c4 39    CPY &39
3162 d0 f4    BNE &3158
3164 c8       INY     
3165 b1 3c    LDA (&3c),Y
3167 20 26 89 JSR &8926
316a b0 c1    BCS &312d
316c 8a       TXA     
316d a8       TAY     
316e 20 6d 98 JSR &986d
3171 20 ed 94 JSR &94ed
3174 a2 01    LDX #&01
3176 20 31 95 JSR &9531
3179 a0 00    LDY #&00
317b a5 0b    LDA &0b
317d 91 02    STA (&02),Y
317f c8       INY     
3180 a5 0c    LDA &0c
3182 91 02    STA (&02),Y
3184 20 39 95 JSR &9539
3187 4c f4 b1 JMP &b1f4
318a 00       BRK     
318b 1e 42 61 ASL &6142,X
318e 64       ???
318f 20 63 61 JSR &6163
3192 6c 6c 00 JMP (&006c)
3195 a9 a4    LDA #&a4
3197 85 27    STA &27
3199 ba       TSX     
319a 8a       TXA     
319b 18       CLC     
319c 65 04    ADC &04
319e 20 2e be JSR &be2e
31a1 a0 00    LDY #&00
31a3 8a       TXA     
31a4 91 04    STA (&04),Y
31a6 e8       INX     
31a7 c8       INY     
31a8 bd 00 01 LDA &0100,X
31ab 91 04    STA (&04),Y
31ad e0 ff    CPX #&ff
31af d0 f5    BNE &31a6
31b1 9a       TXS     
31b2 a5 27    LDA &27
31b4 48       PHA     
31b5 a5 0a    LDA &0a
31b7 48       PHA     
31b8 a5 0b    LDA &0b
31ba 48       PHA     
31bb a5 0c    LDA &0c
31bd 48       PHA     
31be a5 1b    LDA &1b
31c0 aa       TAX     
31c1 18       CLC     
31c2 65 19    ADC &19
31c4 a4 1a    LDY &1a
31c6 90 02    BCC &31ca
31c8 c8       INY     
31c9 18       CLC     
31ca e9 01    SBC #&01
31cc 85 37    STA &37
31ce 98       TYA     
31cf e9 00    SBC #&00
31d1 85 38    STA &38
31d3 a0 02    LDY #&02
31d5 20 5b 95 JSR &955b
31d8 c0 02    CPY #&02
31da f0 ae    BEQ &318a
31dc 86 1b    STX &1b
31de 88       DEY     
31df 84 39    STY &39
31e1 20 5b 94 JSR &945b
31e4 d0 03    BNE &31e9
31e6 4c 12 b1 JMP &b112
31e9 a0 00    LDY #&00
31eb b1 2a    LDA (&2a),Y
31ed 85 0b    STA &0b
31ef c8       INY     
31f0 b1 2a    LDA (&2a),Y
31f2 85 0c    STA &0c
31f4 a9 00    LDA #&00
31f6 48       PHA     
31f7 85 0a    STA &0a
31f9 20 97 8a JSR &8a97
31fc c9 28    CMP #&28
31fe f0 4d    BEQ &324d
3200 c6 0a    DEC &0a
3202 a5 1b    LDA &1b
3204 48       PHA     
3205 a5 19    LDA &19
3207 48       PHA     
3208 a5 1a    LDA &1a
320a 48       PHA     
320b 20 a3 8b JSR &8ba3
320e 68       PLA     
320f 85 1a    STA &1a
3211 68       PLA     
3212 85 19    STA &19
3214 68       PLA     
3215 85 1b    STA &1b
3217 68       PLA     
3218 f0 0c    BEQ &3226
321a 85 3f    STA &3f
321c 20 0b be JSR &be0b
321f 20 c1 8c JSR &8cc1
3222 c6 3f    DEC &3f
3224 d0 f6    BNE &321c
3226 68       PLA     
3227 85 0c    STA &0c
3229 68       PLA     
322a 85 0b    STA &0b
322c 68       PLA     
322d 85 0a    STA &0a
322f 68       PLA     
3230 a0 00    LDY #&00
3232 b1 04    LDA (&04),Y
3234 aa       TAX     
3235 9a       TXS     
3236 c8       INY     
3237 e8       INX     
3238 b1 04    LDA (&04),Y
323a 9d 00 01 STA &0100,X
323d e0 ff    CPX #&ff
323f d0 f5    BNE &3236
3241 98       TYA     
3242 65 04    ADC &04
3244 85 04    STA &04
3246 90 02    BCC &324a
3248 e6 05    INC &05
324a a5 27    LDA &27
324c 60       RTS     
324d a5 1b    LDA &1b
324f 48       PHA     
3250 a5 19    LDA &19
3252 48       PHA     
3253 a5 1a    LDA &1a
3255 48       PHA     
3256 20 82 95 JSR &9582
3259 f0 5a    BEQ &32b5
325b a5 1b    LDA &1b
325d 85 0a    STA &0a
325f 68       PLA     
3260 85 1a    STA &1a
3262 68       PLA     
3263 85 19    STA &19
3265 68       PLA     
3266 85 1b    STA &1b
3268 68       PLA     
3269 aa       TAX     
326a a5 2c    LDA &2c
326c 48       PHA     
326d a5 2b    LDA &2b
326f 48       PHA     
3270 a5 2a    LDA &2a
3272 48       PHA     
3273 e8       INX     
3274 8a       TXA     
3275 48       PHA     
3276 20 0d b3 JSR &b30d
3279 20 97 8a JSR &8a97
327c c9 2c    CMP #&2c
327e f0 cd    BEQ &324d
3280 c9 29    CMP #&29
3282 d0 31    BNE &32b5
3284 a9 00    LDA #&00
3286 48       PHA     
3287 20 8c 8a JSR &8a8c
328a c9 28    CMP #&28
328c d0 27    BNE &32b5
328e 20 29 9b JSR &9b29
3291 20 90 bd JSR &bd90
3294 a5 27    LDA &27
3296 85 2d    STA &2d
3298 20 94 bd JSR &bd94
329b 68       PLA     
329c aa       TAX     
329d e8       INX     
329e 8a       TXA     
329f 48       PHA     
32a0 20 8c 8a JSR &8a8c
32a3 c9 2c    CMP #&2c
32a5 f0 e7    BEQ &328e
32a7 c9 29    CMP #&29
32a9 d0 0a    BNE &32b5
32ab 68       PLA     
32ac 68       PLA     
32ad 85 4d    STA &4d
32af 85 4e    STA &4e
32b1 e4 4d    CPX &4d
32b3 f0 15    BEQ &32ca
32b5 a2 fb    LDX #&fb
32b7 9a       TXS     
32b8 68       PLA     
32b9 85 0c    STA &0c
32bb 68       PLA     
32bc 85 0b    STA &0b
32be 00       BRK     
32bf 1f       ???
32c0 41 72    EOR (&72;X)
32c2 67       ???
32c3 75 6d    ADC &6d,X
32c5 65 6e    ADC &6e
32c7 74       ???
32c8 73       ???
32c9 00       BRK     
32ca 20 ea bd JSR &bdea
32cd 68       PLA     
32ce 85 2a    STA &2a
32d0 68       PLA     
32d1 85 2b    STA &2b
32d3 68       PLA     
32d4 85 2c    STA &2c
32d6 30 21    BMI &32f9
32d8 a5 2d    LDA &2d
32da f0 d9    BEQ &32b5
32dc 85 27    STA &27
32de a2 37    LDX #&37
32e0 20 44 be JSR &be44
32e3 a5 27    LDA &27
32e5 10 09    BPL &32f0
32e7 20 7e bd JSR &bd7e
32ea 20 b5 a3 JSR &a3b5
32ed 4c f3 b2 JMP &b2f3
32f0 20 ea bd JSR &bdea
32f3 20 b7 b4 JSR &b4b7
32f6 4c 03 b3 JMP &b303
32f9 a5 2d    LDA &2d
32fb d0 b8    BNE &32b5
32fd 20 cb bd JSR &bdcb
3300 20 21 8c JSR &8c21
3303 c6 4d    DEC &4d
3305 d0 c3    BNE &32ca
3307 a5 4e    LDA &4e
3309 48       PHA     
330a 4c 02 b2 JMP &b202
330d a4 2c    LDY &2c
330f c0 04    CPY #&04
3311 d0 05    BNE &3318
3313 a2 37    LDX #&37
3315 20 44 be JSR &be44
3318 20 2c b3 JSR &b32c
331b 08       PHP     
331c 20 90 bd JSR &bd90
331f 28       PLP     
3320 f0 07    BEQ &3329
3322 30 05    BMI &3329
3324 a2 37    LDX #&37
3326 20 56 af JSR &af56
3329 4c 94 bd JMP &bd94
332c a4 2c    LDY &2c
332e 30 54    BMI &3384
3330 f0 1d    BEQ &334f
3332 c0 05    CPY #&05
3334 f0 1e    BEQ &3354
3336 a0 03    LDY #&03
3338 b1 2a    LDA (&2a),Y
333a 85 2d    STA &2d
333c 88       DEY     
333d b1 2a    LDA (&2a),Y
333f 85 2c    STA &2c
3341 88       DEY     
3342 b1 2a    LDA (&2a),Y
3344 aa       TAX     
3345 88       DEY     
3346 b1 2a    LDA (&2a),Y
3348 85 2a    STA &2a
334a 86 2b    STX &2b
334c a9 40    LDA #&40
334e 60       RTS     
334f b1 2a    LDA (&2a),Y
3351 4c ea ae JMP &aeea
3354 88       DEY     
3355 b1 2a    LDA (&2a),Y
3357 85 34    STA &34
3359 88       DEY     
335a b1 2a    LDA (&2a),Y
335c 85 33    STA &33
335e 88       DEY     
335f b1 2a    LDA (&2a),Y
3361 85 32    STA &32
3363 88       DEY     
3364 b1 2a    LDA (&2a),Y
3366 85 2e    STA &2e
3368 88       DEY     
3369 b1 2a    LDA (&2a),Y
336b 85 30    STA &30
336d 84 35    STY &35
336f 84 2f    STY &2f
3371 05 2e    ORA &2e
3373 05 32    ORA &32
3375 05 33    ORA &33
3377 05 34    ORA &34
3379 f0 04    BEQ &337f
337b a5 2e    LDA &2e
337d 09 80    ORA #&80
337f 85 31    STA &31
3381 a9 ff    LDA #&ff
3383 60       RTS     
3384 c0 80    CPY #&80
3386 f0 1f    BEQ &33a7
3388 a0 03    LDY #&03
338a b1 2a    LDA (&2a),Y
338c 85 36    STA &36
338e f0 16    BEQ &33a6
3390 a0 01    LDY #&01
3392 b1 2a    LDA (&2a),Y
3394 85 38    STA &38
3396 88       DEY     
3397 b1 2a    LDA (&2a),Y
3399 85 37    STA &37
339b a4 36    LDY &36
339d 88       DEY     
339e b1 37    LDA (&37),Y
33a0 99 00 06 STA &0600,Y
33a3 98       TYA     
33a4 d0 f7    BNE &339d
33a6 60       RTS     
33a7 a5 2b    LDA &2b
33a9 f0 15    BEQ &33c0
33ab a0 00    LDY #&00
33ad b1 2a    LDA (&2a),Y
33af 99 00 06 STA &0600,Y
33b2 49 0d    EOR #&0d
33b4 f0 04    BEQ &33ba
33b6 c8       INY     
33b7 d0 f4    BNE &33ad
33b9 98       TYA     
33ba 84 36    STY &36
33bc 60       RTS     
33bd 20 e3 92 JSR &92e3
33c0 a5 2a    LDA &2a
33c2 4c c2 af JMP &afc2
33c5 a0 00    LDY #&00
33c7 84 08    STY &08
33c9 84 09    STY &09
33cb a6 18    LDX &18
33cd 86 38    STX &38
33cf 84 37    STY &37
33d1 a6 0c    LDX &0c
33d3 e0 07    CPX #&07
33d5 f0 2a    BEQ &3401
33d7 a6 0b    LDX &0b
33d9 20 42 89 JSR &8942
33dc c9 0d    CMP #&0d
33de d0 19    BNE &33f9
33e0 e4 37    CPX &37
33e2 a5 0c    LDA &0c
33e4 e5 38    SBC &38
33e6 90 19    BCC &3401
33e8 20 42 89 JSR &8942
33eb 09 00    ORA #&00
33ed 30 12    BMI &3401
33ef 85 09    STA &09
33f1 20 42 89 JSR &8942
33f4 85 08    STA &08
33f6 20 42 89 JSR &8942
33f9 e4 37    CPX &37
33fb a5 0c    LDA &0c
33fd e5 38    SBC &38
33ff b0 d8    BCS &33d9
3401 60       RTS     
3402 20 c5 b3 JSR &b3c5
3405 84 20    STY &20
3407 b1 fd    LDA (&fd),Y
3409 d0 08    BNE &3413
340b a9 33    LDA #&33
340d 85 16    STA &16
340f a9 b4    LDA #&b4
3411 85 17    STA &17
3413 a5 16    LDA &16
3415 85 0b    STA &0b
3417 a5 17    LDA &17
3419 85 0c    STA &0c
341b 20 3a bd JSR &bd3a
341e aa       TAX     
341f 86 0a    STX &0a
3421 a9 da    LDA #&da
3423 20 f4 ff JSR &fff4
3426 a9 7e    LDA #&7e
3428 20 f4 ff JSR &fff4
342b a2 ff    LDX #&ff
342d 86 28    STX &28
342f 9a       TXS     
3430 4c a3 8b JMP &8ba3
3433 f6 3a    INC &3a,X
3435 e7       ???
3436 9e       ???
3437 f1 22    SBC (&22),Y
3439 20 61 74 JSR &7461
343c 20 6c 69 JSR &696c
343f 6e 65 20 ROR &2065
3442 22       ???
3443 3b       ???
3444 9e       ???
3445 3a       ???
3446 e0 8b    CPX #&8b
3448 f1 3a    SBC (&3a),Y
344a e0 0d    CPX #&0d
344c 20 21 88 JSR &8821
344f a2 03    LDX #&03
3451 a5 2a    LDA &2a
3453 48       PHA     
3454 a5 2b    LDA &2b
3456 48       PHA     
3457 8a       TXA     
3458 48       PHA     
3459 20 da 92 JSR &92da
345c 68       PLA     
345d aa       TAX     
345e ca       DEX     
345f d0 f0    BNE &3451
3461 20 52 98 JSR &9852
3464 a5 2a    LDA &2a
3466 85 3d    STA &3d
3468 a5 2b    LDA &2b
346a 85 3e    STA &3e
346c a0 07    LDY #&07
346e a2 05    LDX #&05
3470 d0 1d    BNE &348f
3472 20 21 88 JSR &8821
3475 a2 0d    LDX #&0d
3477 a5 2a    LDA &2a
3479 48       PHA     
347a 8a       TXA     
347b 48       PHA     
347c 20 da 92 JSR &92da
347f 68       PLA     
3480 aa       TAX     
3481 ca       DEX     
3482 d0 f3    BNE &3477
3484 20 52 98 JSR &9852
3487 a5 2a    LDA &2a
3489 85 44    STA &44
348b a2 0c    LDX #&0c
348d a0 08    LDY #&08
348f 68       PLA     
3490 95 37    STA &37,X
3492 ca       DEX     
3493 10 fa    BPL &348f
3495 98       TYA     
3496 a2 37    LDX #&37
3498 a0 00    LDY #&00
349a 20 f1 ff JSR &fff1
349d 4c 9b 8b JMP &8b9b
34a0 20 21 88 JSR &8821
34a3 20 52 98 JSR &9852
34a6 a4 2a    LDY &2a
34a8 88       DEY     
34a9 84 23    STY &23
34ab 4c 9b 8b JMP &8b9b
34ae 4c 0e 8c JMP &8c0e
34b1 20 29 9b JSR &9b29
34b4 20 0b be JSR &be0b
34b7 a5 39    LDA &39
34b9 c9 05    CMP #&05
34bb f0 23    BEQ &34e0
34bd a5 27    LDA &27
34bf f0 ed    BEQ &34ae
34c1 10 03    BPL &34c6
34c3 20 e4 a3 JSR &a3e4
34c6 a0 00    LDY #&00
34c8 a5 2a    LDA &2a
34ca 91 37    STA (&37),Y
34cc a5 39    LDA &39
34ce f0 0f    BEQ &34df
34d0 a5 2b    LDA &2b
34d2 c8       INY     
34d3 91 37    STA (&37),Y
34d5 a5 2c    LDA &2c
34d7 c8       INY     
34d8 91 37    STA (&37),Y
34da a5 2d    LDA &2d
34dc c8       INY     
34dd 91 37    STA (&37),Y
34df 60       RTS     
34e0 a5 27    LDA &27
34e2 f0 ca    BEQ &34ae
34e4 30 03    BMI &34e9
34e6 20 be a2 JSR &a2be
34e9 a0 00    LDY #&00
34eb a5 30    LDA &30
34ed 91 37    STA (&37),Y
34ef c8       INY     
34f0 a5 2e    LDA &2e
34f2 29 80    AND #&80
34f4 85 2e    STA &2e
34f6 a5 31    LDA &31
34f8 29 7f    AND #&7f
34fa 05 2e    ORA &2e
34fc 91 37    STA (&37),Y
34fe c8       INY     
34ff a5 32    LDA &32
3501 91 37    STA (&37),Y
3503 c8       INY     
3504 a5 33    LDA &33
3506 91 37    STA (&37),Y
3508 c8       INY     
3509 a5 34    LDA &34
350b 91 37    STA (&37),Y
350d 60       RTS     
350e 85 37    STA &37
3510 c9 80    CMP #&80
3512 90 44    BCC &3558
3514 a9 71    LDA #&71
3516 85 38    STA &38
3518 a9 80    LDA #&80
351a 85 39    STA &39
351c 84 3a    STY &3a
351e a0 00    LDY #&00
3520 c8       INY     
3521 b1 38    LDA (&38),Y
3523 10 fb    BPL &3520
3525 c5 37    CMP &37
3527 f0 0d    BEQ &3536
3529 c8       INY     
352a 98       TYA     
352b 38       SEC     
352c 65 38    ADC &38
352e 85 38    STA &38
3530 90 ec    BCC &351e
3532 e6 39    INC &39
3534 b0 e8    BCS &351e
3536 a0 00    LDY #&00
3538 b1 38    LDA (&38),Y
353a 30 06    BMI &3542
353c 20 58 b5 JSR &b558
353f c8       INY     
3540 d0 f6    BNE &3538
3542 a4 3a    LDY &3a
3544 60       RTS     
3545 48       PHA     
3546 4a       LSR A
3547 4a       LSR A
3548 4a       LSR A
3549 4a       LSR A
354a 20 50 b5 JSR &b550
354d 68       PLA     
354e 29 0f    AND #&0f
3550 c9 0a    CMP #&0a
3552 90 02    BCC &3556
3554 69 06    ADC #&06
3556 69 30    ADC #&30
3558 c9 0d    CMP #&0d
355a d0 0b    BNE &3567
355c 20 ee ff JSR &ffee
355f 4c 28 bc JMP &bc28
3562 20 45 b5 JSR &b545
3565 a9 20    LDA #&20
3567 48       PHA     
3568 a5 23    LDA &23
356a c5 1e    CMP &1e
356c b0 03    BCS &3571
356e 20 25 bc JSR &bc25
3571 68       PLA     
3572 e6 1e    INC &1e
3574 6c 0e 02 JMP (&020e)
3577 25 1f    AND &1f
3579 f0 0e    BEQ &3589
357b 8a       TXA     
357c f0 0b    BEQ &3589
357e 30 e5    BMI &3565
3580 20 65 b5 JSR &b565
3583 20 58 b5 JSR &b558
3586 ca       DEX     
3587 d0 f7    BNE &3580
3589 60       RTS     
358a e6 0a    INC &0a
358c 20 1d 9b JSR &9b1d
358f 20 4c 98 JSR &984c
3592 20 ee 92 JSR &92ee
3595 a5 2a    LDA &2a
3597 85 1f    STA &1f
3599 4c f6 8a JMP &8af6
359c c8       INY     
359d b1 0b    LDA (&0b),Y
359f c9 4f    CMP #&4f
35a1 f0 e7    BEQ &358a
35a3 a9 00    LDA #&00
35a5 85 3b    STA &3b
35a7 85 3c    STA &3c
35a9 20 d8 ae JSR &aed8
35ac 20 df 97 JSR &97df
35af 08       PHP     
35b0 20 94 bd JSR &bd94
35b3 a9 ff    LDA #&ff
35b5 85 2a    STA &2a
35b7 a9 7f    LDA #&7f
35b9 85 2b    STA &2b
35bb 28       PLP     
35bc 90 11    BCC &35cf
35be 20 97 8a JSR &8a97
35c1 c9 2c    CMP #&2c
35c3 f0 13    BEQ &35d8
35c5 20 ea bd JSR &bdea
35c8 20 94 bd JSR &bd94
35cb c6 0a    DEC &0a
35cd 10 0c    BPL &35db
35cf 20 97 8a JSR &8a97
35d2 c9 2c    CMP #&2c
35d4 f0 02    BEQ &35d8
35d6 c6 0a    DEC &0a
35d8 20 df 97 JSR &97df
35db a5 2a    LDA &2a
35dd 85 31    STA &31
35df a5 2b    LDA &2b
35e1 85 32    STA &32
35e3 20 57 98 JSR &9857
35e6 20 6f be JSR &be6f
35e9 20 ea bd JSR &bdea
35ec 20 70 99 JSR &9970
35ef a5 3d    LDA &3d
35f1 85 0b    STA &0b
35f3 a5 3e    LDA &3e
35f5 85 0c    STA &0c
35f7 90 16    BCC &360f
35f9 88       DEY     
35fa b0 06    BCS &3602
35fc 20 25 bc JSR &bc25
35ff 20 6d 98 JSR &986d
3602 b1 0b    LDA (&0b),Y
3604 85 2b    STA &2b
3606 c8       INY     
3607 b1 0b    LDA (&0b),Y
3609 85 2a    STA &2a
360b c8       INY     
360c c8       INY     
360d 84 0a    STY &0a
360f a5 2a    LDA &2a
3611 18       CLC     
3612 e5 31    SBC &31
3614 a5 2b    LDA &2b
3616 e5 32    SBC &32
3618 90 03    BCC &361d
361a 4c f6 8a JMP &8af6
361d 20 23 99 JSR &9923
3620 a2 ff    LDX #&ff
3622 86 4d    STX &4d
3624 a9 01    LDA #&01
3626 20 77 b5 JSR &b577
3629 a6 3b    LDX &3b
362b a9 02    LDA #&02
362d 20 77 b5 JSR &b577
3630 a6 3c    LDX &3c
3632 a9 04    LDA #&04
3634 20 77 b5 JSR &b577
3637 a4 0a    LDY &0a
3639 b1 0b    LDA (&0b),Y
363b c9 0d    CMP #&0d
363d f0 bd    BEQ &35fc
363f c9 22    CMP #&22
3641 d0 0e    BNE &3651
3643 a9 ff    LDA #&ff
3645 45 4d    EOR &4d
3647 85 4d    STA &4d
3649 a9 22    LDA #&22
364b 20 58 b5 JSR &b558
364e c8       INY     
364f d0 e8    BNE &3639
3651 24 4d    BIT &4d
3653 10 f6    BPL &364b
3655 c9 8d    CMP #&8d
3657 d0 0f    BNE &3668
3659 20 eb 97 JSR &97eb
365c 84 0a    STY &0a
365e a9 00    LDA #&00
3660 85 14    STA &14
3662 20 1f 99 JSR &991f
3665 4c 37 b6 JMP &b637
3668 c9 e3    CMP #&e3
366a d0 02    BNE &366e
366c e6 3b    INC &3b
366e c9 ed    CMP #&ed
3670 d0 06    BNE &3678
3672 a6 3b    LDX &3b
3674 f0 02    BEQ &3678
3676 c6 3b    DEC &3b
3678 c9 f5    CMP #&f5
367a d0 02    BNE &367e
367c e6 3c    INC &3c
367e c9 fd    CMP #&fd
3680 d0 06    BNE &3688
3682 a6 3c    LDX &3c
3684 f0 02    BEQ &3688
3686 c6 3c    DEC &3c
3688 20 0e b5 JSR &b50e
368b c8       INY     
368c d0 ab    BNE &3639
368e 00       BRK     
368f 20 4e 6f JSR &6f4e
3692 20 e3 00 JSR &00e3
3695 20 c9 95 JSR &95c9
3698 d0 09    BNE &36a3
369a a6 26    LDX &26
369c f0 f0    BEQ &368e
369e b0 37    BCS &36d7
36a0 4c 2a 98 JMP &982a
36a3 b0 fb    BCS &36a0
36a5 a6 26    LDX &26
36a7 f0 e5    BEQ &368e
36a9 a5 2a    LDA &2a
36ab dd f1 04 CMP &04f1,X
36ae d0 0e    BNE &36be
36b0 a5 2b    LDA &2b
36b2 dd f2 04 CMP &04f2,X
36b5 d0 07    BNE &36be
36b7 a5 2c    LDA &2c
36b9 dd f3 04 CMP &04f3,X
36bc f0 19    BEQ &36d7
36be 8a       TXA     
36bf 38       SEC     
36c0 e9 0f    SBC #&0f
36c2 aa       TAX     
36c3 86 26    STX &26
36c5 d0 e2    BNE &36a9
36c7 00       BRK     
36c8 21 43    AND (&43;X)
36ca 61 6e    ADC (&6e;X)
36cc 27       ???
36cd 74       ???
36ce 20 4d 61 JSR &614d
36d1 74       ???
36d2 63       ???
36d3 68       PLA     
36d4 20 e3 00 JSR &00e3
36d7 bd f1 04 LDA &04f1,X
36da 85 2a    STA &2a
36dc bd f2 04 LDA &04f2,X
36df 85 2b    STA &2b
36e1 bc f3 04 LDY &04f3,X
36e4 c0 05    CPY #&05
36e6 f0 7e    BEQ &3766
36e8 a0 00    LDY #&00
36ea b1 2a    LDA (&2a),Y
36ec 7d f4 04 ADC &04f4,X
36ef 91 2a    STA (&2a),Y
36f1 85 37    STA &37
36f3 c8       INY     
36f4 b1 2a    LDA (&2a),Y
36f6 7d f5 04 ADC &04f5,X
36f9 91 2a    STA (&2a),Y
36fb 85 38    STA &38
36fd c8       INY     
36fe b1 2a    LDA (&2a),Y
3700 7d f6 04 ADC &04f6,X
3703 91 2a    STA (&2a),Y
3705 85 39    STA &39
3707 c8       INY     
3708 b1 2a    LDA (&2a),Y
370a 7d f7 04 ADC &04f7,X
370d 91 2a    STA (&2a),Y
370f a8       TAY     
3710 a5 37    LDA &37
3712 38       SEC     
3713 fd f9 04 SBC &04f9,X
3716 85 37    STA &37
3718 a5 38    LDA &38
371a fd fa 04 SBC &04fa,X
371d 85 38    STA &38
371f a5 39    LDA &39
3721 fd fb 04 SBC &04fb,X
3724 85 39    STA &39
3726 98       TYA     
3727 fd fc 04 SBC &04fc,X
372a 05 37    ORA &37
372c 05 38    ORA &38
372e 05 39    ORA &39
3730 f0 0f    BEQ &3741
3732 98       TYA     
3733 5d f7 04 EOR &04f7,X
3736 5d fc 04 EOR &04fc,X
3739 10 04    BPL &373f
373b b0 04    BCS &3741
373d 90 12    BCC &3751
373f b0 10    BCS &3751
3741 bc fe 04 LDY &04fe,X
3744 bd ff 04 LDA &04ff,X
3747 84 0b    STY &0b
3749 85 0c    STA &0c
374b 20 77 98 JSR &9877
374e 4c a3 8b JMP &8ba3
3751 a5 26    LDA &26
3753 38       SEC     
3754 e9 0f    SBC #&0f
3756 85 26    STA &26
3758 a4 1b    LDY &1b
375a 84 0a    STY &0a
375c 20 97 8a JSR &8a97
375f c9 2c    CMP #&2c
3761 d0 3e    BNE &37a1
3763 4c 95 b6 JMP &b695
3766 20 54 b3 JSR &b354
3769 a5 26    LDA &26
376b 18       CLC     
376c 69 f4    ADC #&f4
376e 85 4b    STA &4b
3770 a9 05    LDA #&05
3772 85 4c    STA &4c
3774 20 00 a5 JSR &a500
3777 a5 2a    LDA &2a
3779 85 37    STA &37
377b a5 2b    LDA &2b
377d 85 38    STA &38
377f 20 e9 b4 JSR &b4e9
3782 a5 26    LDA &26
3784 85 27    STA &27
3786 18       CLC     
3787 69 f9    ADC #&f9
3789 85 4b    STA &4b
378b a9 05    LDA #&05
378d 85 4c    STA &4c
378f 20 5f 9a JSR &9a5f
3792 f0 ad    BEQ &3741
3794 bd f5 04 LDA &04f5,X
3797 30 04    BMI &379d
3799 b0 a6    BCS &3741
379b 90 b4    BCC &3751
379d 90 a2    BCC &3741
379f b0 b0    BCS &3751
37a1 4c 96 8b JMP &8b96
37a4 00       BRK     
37a5 22       ???
37a6 e3       ???
37a7 20 76 61 JSR &6176
37aa 72       ???
37ab 69 61    ADC #&61
37ad 62       ???
37ae 6c 65 00 JMP (&0065)
37b1 23       ???
37b2 54       ???
37b3 6f       ???
37b4 6f       ???
37b5 20 6d 61 JSR &616d
37b8 6e 79 20 ROR &2079
37bb e3       ???
37bc 73       ???
37bd 00       BRK     
37be 24 4e    BIT &4e
37c0 6f       ???
37c1 20 b8 00 JSR &00b8
37c4 20 82 95 JSR &9582
37c7 f0 db    BEQ &37a4
37c9 b0 d9    BCS &37a4
37cb 20 94 bd JSR &bd94
37ce 20 41 98 JSR &9841
37d1 20 b1 b4 JSR &b4b1
37d4 a4 26    LDY &26
37d6 c0 96    CPY #&96
37d8 b0 d6    BCS &37b0
37da a5 37    LDA &37
37dc 99 00 05 STA &0500,Y
37df a5 38    LDA &38
37e1 99 01 05 STA &0501,Y
37e4 a5 39    LDA &39
37e6 99 02 05 STA &0502,Y
37e9 aa       TAX     
37ea 20 8c 8a JSR &8a8c
37ed c9 b8    CMP #&b8
37ef d0 cc    BNE &37bd
37f1 e0 05    CPX #&05
37f3 f0 5a    BEQ &384f
37f5 20 dd 92 JSR &92dd
37f8 a4 26    LDY &26
37fa a5 2a    LDA &2a
37fc 99 08 05 STA &0508,Y
37ff a5 2b    LDA &2b
3801 99 09 05 STA &0509,Y
3804 a5 2c    LDA &2c
3806 99 0a 05 STA &050a,Y
3809 a5 2d    LDA &2d
380b 99 0b 05 STA &050b,Y
380e a9 01    LDA #&01
3810 20 d8 ae JSR &aed8
3813 20 8c 8a JSR &8a8c
3816 c9 88    CMP #&88
3818 d0 05    BNE &381f
381a 20 dd 92 JSR &92dd
381d a4 1b    LDY &1b
381f 84 0a    STY &0a
3821 a4 26    LDY &26
3823 a5 2a    LDA &2a
3825 99 03 05 STA &0503,Y
3828 a5 2b    LDA &2b
382a 99 04 05 STA &0504,Y
382d a5 2c    LDA &2c
382f 99 05 05 STA &0505,Y
3832 a5 2d    LDA &2d
3834 99 06 05 STA &0506,Y
3837 20 80 98 JSR &9880
383a a4 26    LDY &26
383c a5 0b    LDA &0b
383e 99 0d 05 STA &050d,Y
3841 a5 0c    LDA &0c
3843 99 0e 05 STA &050e,Y
3846 18       CLC     
3847 98       TYA     
3848 69 0f    ADC #&0f
384a 85 26    STA &26
384c 4c a3 8b JMP &8ba3
384f 20 29 9b JSR &9b29
3852 20 fd 92 JSR &92fd
3855 a5 26    LDA &26
3857 18       CLC     
3858 69 08    ADC #&08
385a 85 4b    STA &4b
385c a9 05    LDA #&05
385e 85 4c    STA &4c
3860 20 8d a3 JSR &a38d
3863 20 99 a6 JSR &a699
3866 20 8c 8a JSR &8a8c
3869 c9 88    CMP #&88
386b d0 08    BNE &3875
386d 20 29 9b JSR &9b29
3870 20 fd 92 JSR &92fd
3873 a4 1b    LDY &1b
3875 84 0a    STY &0a
3877 a5 26    LDA &26
3879 18       CLC     
387a 69 03    ADC #&03
387c 85 4b    STA &4b
387e a9 05    LDA #&05
3880 85 4c    STA &4c
3882 20 8d a3 JSR &a38d
3885 4c 37 b8 JMP &b837
3888 20 9a b9 JSR &b99a
388b 20 57 98 JSR &9857
388e a4 25    LDY &25
3890 c0 1a    CPY #&1a
3892 b0 0e    BCS &38a2
3894 a5 0b    LDA &0b
3896 99 cc 05 STA &05cc,Y
3899 a5 0c    LDA &0c
389b 99 e6 05 STA &05e6,Y
389e e6 25    INC &25
38a0 90 30    BCC &38d2
38a2 00       BRK     
38a3 25 54    AND &54
38a5 6f       ???
38a6 6f       ???
38a7 20 6d 61 JSR &616d
38aa 6e 79 20 ROR &2079
38ad e4 73    CPX &73
38af 00       BRK     
38b0 26 4e    ROL &4e
38b2 6f       ???
38b3 20 e4 00 JSR &00e4
38b6 20 57 98 JSR &9857
38b9 a6 25    LDX &25
38bb f0 f2    BEQ &38af
38bd c6 25    DEC &25
38bf bc cb 05 LDY &05cb,X
38c2 bd e5 05 LDA &05e5,X
38c5 84 0b    STY &0b
38c7 85 0c    STA &0c
38c9 4c 9b 8b JMP &8b9b
38cc 20 9a b9 JSR &b99a
38cf 20 57 98 JSR &9857
38d2 a5 20    LDA &20
38d4 f0 03    BEQ &38d9
38d6 20 05 99 JSR &9905
38d9 a4 3d    LDY &3d
38db a5 3e    LDA &3e
38dd 84 0b    STY &0b
38df 85 0c    STA &0c
38e1 4c a3 8b JMP &8ba3
38e4 20 57 98 JSR &9857
38e7 a9 33    LDA #&33
38e9 85 16    STA &16
38eb a9 b4    LDA #&b4
38ed 85 17    STA &17
38ef 4c 9b 8b JMP &8b9b
38f2 20 97 8a JSR &8a97
38f5 c9 87    CMP #&87
38f7 f0 eb    BEQ &38e4
38f9 a4 0a    LDY &0a
38fb 88       DEY     
38fc 20 6d 98 JSR &986d
38ff a5 0b    LDA &0b
3901 85 16    STA &16
3903 a5 0c    LDA &0c
3905 85 17    STA &17
3907 4c 7d 8b JMP &8b7d
390a 00       BRK     
390b 27       ???
390c ee 20 73 INC &7320
390f 79 6e 74 ADC &746e,Y
3912 61 78    ADC (&78;X)
3914 00       BRK     
3915 20 97 8a JSR &8a97
3918 c9 85    CMP #&85
391a f0 d6    BEQ &38f2
391c c6 0a    DEC &0a
391e 20 1d 9b JSR &9b1d
3921 20 f0 92 JSR &92f0
3924 a4 1b    LDY &1b
3926 c8       INY     
3927 84 0a    STY &0a
3929 e0 e5    CPX #&e5
392b f0 04    BEQ &3931
392d e0 e4    CPX #&e4
392f d0 d9    BNE &390a
3931 8a       TXA     
3932 48       PHA     
3933 a5 2b    LDA &2b
3935 05 2c    ORA &2c
3937 05 2d    ORA &2d
3939 d0 42    BNE &397d
393b a6 2a    LDX &2a
393d f0 3e    BEQ &397d
393f ca       DEX     
3940 f0 1a    BEQ &395c
3942 a4 0a    LDY &0a
3944 b1 0b    LDA (&0b),Y
3946 c8       INY     
3947 c9 0d    CMP #&0d
3949 f0 32    BEQ &397d
394b c9 3a    CMP #&3a
394d f0 2e    BEQ &397d
394f c9 8b    CMP #&8b
3951 f0 2a    BEQ &397d
3953 c9 2c    CMP #&2c
3955 d0 ed    BNE &3944
3957 ca       DEX     
3958 d0 ea    BNE &3944
395a 84 0a    STY &0a
395c 20 9a b9 JSR &b99a
395f 68       PLA     
3960 c9 e4    CMP #&e4
3962 f0 06    BEQ &396a
3964 20 77 98 JSR &9877
3967 4c d2 b8 JMP &b8d2
396a a4 0a    LDY &0a
396c b1 0b    LDA (&0b),Y
396e c8       INY     
396f c9 0d    CMP #&0d
3971 f0 04    BEQ &3977
3973 c9 3a    CMP #&3a
3975 d0 f5    BNE &396c
3977 88       DEY     
3978 84 0a    STY &0a
397a 4c 8b b8 JMP &b88b
397d a4 0a    LDY &0a
397f 68       PLA     
3980 b1 0b    LDA (&0b),Y
3982 c8       INY     
3983 c9 8b    CMP #&8b
3985 f0 0e    BEQ &3995
3987 c9 0d    CMP #&0d
3989 d0 f5    BNE &3980
398b 00       BRK     
398c 28       PLP     
398d ee 20 72 INC &7220
3990 61 6e    ADC (&6e;X)
3992 67       ???
3993 65 00    ADC &00
3995 84 0a    STY &0a
3997 4c e3 98 JMP &98e3
399a 20 df 97 JSR &97df
399d b0 10    BCS &39af
399f 20 1d 9b JSR &9b1d
39a2 20 f0 92 JSR &92f0
39a5 a5 1b    LDA &1b
39a7 85 0a    STA &0a
39a9 a5 2b    LDA &2b
39ab 29 7f    AND #&7f
39ad 85 2b    STA &2b
39af 20 70 99 JSR &9970
39b2 b0 01    BCS &39b5
39b4 60       RTS     
39b5 00       BRK     
39b6 29 4e    AND #&4e
39b8 6f       ???
39b9 20 73 75 JSR &7573
39bc 63       ???
39bd 68       PLA     
39be 20 6c 69 JSR &696c
39c1 6e 65 00 ROR &0065
39c4 4c 0e 8c JMP &8c0e
39c7 4c 2a 98 JMP &982a
39ca 84 0a    STY &0a
39cc 4c 98 8b JMP &8b98
39cf c6 0a    DEC &0a
39d1 20 a9 bf JSR &bfa9
39d4 a5 1b    LDA &1b
39d6 85 0a    STA &0a
39d8 84 4d    STY &4d
39da 20 97 8a JSR &8a97
39dd c9 2c    CMP #&2c
39df d0 e9    BNE &39ca
39e1 a5 4d    LDA &4d
39e3 48       PHA     
39e4 20 82 95 JSR &9582
39e7 f0 de    BEQ &39c7
39e9 a5 1b    LDA &1b
39eb 85 0a    STA &0a
39ed 68       PLA     
39ee 85 4d    STA &4d
39f0 08       PHP     
39f1 20 94 bd JSR &bd94
39f4 a4 4d    LDY &4d
39f6 20 d7 ff JSR &ffd7
39f9 85 27    STA &27
39fb 28       PLP     
39fc 90 1b    BCC &3a19
39fe a5 27    LDA &27
3a00 d0 c2    BNE &39c4
3a02 20 d7 ff JSR &ffd7
3a05 85 36    STA &36
3a07 aa       TAX     
3a08 f0 09    BEQ &3a13
3a0a 20 d7 ff JSR &ffd7
3a0d 9d ff 05 STA &05ff,X
3a10 ca       DEX     
3a11 d0 f7    BNE &3a0a
3a13 20 1e 8c JSR &8c1e
3a16 4c da b9 JMP &b9da
3a19 a5 27    LDA &27
3a1b f0 a7    BEQ &39c4
3a1d 30 0c    BMI &3a2b
3a1f a2 03    LDX #&03
3a21 20 d7 ff JSR &ffd7
3a24 95 2a    STA &2a,X
3a26 ca       DEX     
3a27 10 f8    BPL &3a21
3a29 30 0e    BMI &3a39
3a2b a2 04    LDX #&04
3a2d 20 d7 ff JSR &ffd7
3a30 9d 6c 04 STA &046c,X
3a33 ca       DEX     
3a34 10 f7    BPL &3a2d
3a36 20 b2 a3 JSR &a3b2
3a39 20 b4 b4 JSR &b4b4
3a3c 4c da b9 JMP &b9da
3a3f 68       PLA     
3a40 68       PLA     
3a41 4c 98 8b JMP &8b98
3a44 20 97 8a JSR &8a97
3a47 c9 23    CMP #&23
3a49 f0 84    BEQ &39cf
3a4b c9 86    CMP #&86
3a4d f0 03    BEQ &3a52
3a4f c6 0a    DEC &0a
3a51 18       CLC     
3a52 66 4d    ROR &4d
3a54 46 4d    LSR &4d
3a56 a9 ff    LDA #&ff
3a58 85 4e    STA &4e
3a5a 20 8a 8e JSR &8e8a
3a5d b0 0a    BCS &3a69
3a5f 20 8a 8e JSR &8e8a
3a62 90 fb    BCC &3a5f
3a64 a2 ff    LDX #&ff
3a66 86 4e    STX &4e
3a68 18       CLC     
3a69 08       PHP     
3a6a 06 4d    ASL &4d
3a6c 28       PLP     
3a6d 66 4d    ROR &4d
3a6f c9 2c    CMP #&2c
3a71 f0 e7    BEQ &3a5a
3a73 c9 3b    CMP #&3b
3a75 f0 e3    BEQ &3a5a
3a77 c6 0a    DEC &0a
3a79 a5 4d    LDA &4d
3a7b 48       PHA     
3a7c a5 4e    LDA &4e
3a7e 48       PHA     
3a7f 20 82 95 JSR &9582
3a82 f0 bb    BEQ &3a3f
3a84 68       PLA     
3a85 85 4e    STA &4e
3a87 68       PLA     
3a88 85 4d    STA &4d
3a8a a5 1b    LDA &1b
3a8c 85 0a    STA &0a
3a8e 08       PHP     
3a8f 24 4d    BIT &4d
3a91 70 06    BVS &3a99
3a93 a5 4e    LDA &4e
3a95 c9 ff    CMP #&ff
3a97 d0 17    BNE &3ab0
3a99 24 4d    BIT &4d
3a9b 10 05    BPL &3aa2
3a9d a9 3f    LDA #&3f
3a9f 20 58 b5 JSR &b558
3aa2 20 fc bb JSR &bbfc
3aa5 84 36    STY &36
3aa7 06 4d    ASL &4d
3aa9 18       CLC     
3aaa 66 4d    ROR &4d
3aac 24 4d    BIT &4d
3aae 70 1d    BVS &3acd
3ab0 85 1b    STA &1b
3ab2 a9 00    LDA #&00
3ab4 85 19    STA &19
3ab6 a9 06    LDA #&06
3ab8 85 1a    STA &1a
3aba 20 ad ad JSR &adad
3abd 20 8c 8a JSR &8a8c
3ac0 c9 2c    CMP #&2c
3ac2 f0 06    BEQ &3aca
3ac4 c9 0d    CMP #&0d
3ac6 d0 f5    BNE &3abd
3ac8 a0 fe    LDY #&fe
3aca c8       INY     
3acb 84 4e    STY &4e
3acd 28       PLP     
3ace b0 0c    BCS &3adc
3ad0 20 94 bd JSR &bd94
3ad3 20 34 ac JSR &ac34
3ad6 20 b4 b4 JSR &b4b4
3ad9 4c 5a ba JMP &ba5a
3adc a9 00    LDA #&00
3ade 85 27    STA &27
3ae0 20 21 8c JSR &8c21
3ae3 4c 5a ba JMP &ba5a
3ae6 a0 00    LDY #&00
3ae8 84 3d    STY &3d
3aea a4 18    LDY &18
3aec 84 3e    STY &3e
3aee 20 97 8a JSR &8a97
3af1 c6 0a    DEC &0a
3af3 c9 3a    CMP #&3a
3af5 f0 10    BEQ &3b07
3af7 c9 0d    CMP #&0d
3af9 f0 0c    BEQ &3b07
3afb c9 8b    CMP #&8b
3afd f0 08    BEQ &3b07
3aff 20 9a b9 JSR &b99a
3b02 a0 01    LDY #&01
3b04 20 55 be JSR &be55
3b07 20 57 98 JSR &9857
3b0a a5 3d    LDA &3d
3b0c 85 1c    STA &1c
3b0e a5 3e    LDA &3e
3b10 85 1d    STA &1d
3b12 4c 9b 8b JMP &8b9b
3b15 20 97 8a JSR &8a97
3b18 c9 2c    CMP #&2c
3b1a f0 03    BEQ &3b1f
3b1c 4c 96 8b JMP &8b96
3b1f 20 82 95 JSR &9582
3b22 f0 f1    BEQ &3b15
3b24 b0 0c    BCS &3b32
3b26 20 50 bb JSR &bb50
3b29 20 94 bd JSR &bd94
3b2c 20 b1 b4 JSR &b4b1
3b2f 4c 40 bb JMP &bb40
3b32 20 50 bb JSR &bb50
3b35 20 94 bd JSR &bd94
3b38 20 ad ad JSR &adad
3b3b 85 27    STA &27
3b3d 20 1e 8c JSR &8c1e
3b40 18       CLC     
3b41 a5 1b    LDA &1b
3b43 65 19    ADC &19
3b45 85 1c    STA &1c
3b47 a5 1a    LDA &1a
3b49 69 00    ADC #&00
3b4b 85 1d    STA &1d
3b4d 4c 15 bb JMP &bb15
3b50 a5 1b    LDA &1b
3b52 85 0a    STA &0a
3b54 a5 1c    LDA &1c
3b56 85 19    STA &19
3b58 a5 1d    LDA &1d
3b5a 85 1a    STA &1a
3b5c a0 00    LDY #&00
3b5e 84 1b    STY &1b
3b60 20 8c 8a JSR &8a8c
3b63 c9 2c    CMP #&2c
3b65 f0 49    BEQ &3bb0
3b67 c9 dc    CMP #&dc
3b69 f0 45    BEQ &3bb0
3b6b c9 0d    CMP #&0d
3b6d f0 0b    BEQ &3b7a
3b6f 20 8c 8a JSR &8a8c
3b72 c9 2c    CMP #&2c
3b74 f0 3a    BEQ &3bb0
3b76 c9 0d    CMP #&0d
3b78 d0 f5    BNE &3b6f
3b7a a4 1b    LDY &1b
3b7c b1 19    LDA (&19),Y
3b7e 30 1c    BMI &3b9c
3b80 c8       INY     
3b81 c8       INY     
3b82 b1 19    LDA (&19),Y
3b84 aa       TAX     
3b85 c8       INY     
3b86 b1 19    LDA (&19),Y
3b88 c9 20    CMP #&20
3b8a f0 f9    BEQ &3b85
3b8c c9 dc    CMP #&dc
3b8e f0 1d    BEQ &3bad
3b90 8a       TXA     
3b91 18       CLC     
3b92 65 19    ADC &19
3b94 85 19    STA &19
3b96 90 e2    BCC &3b7a
3b98 e6 1a    INC &1a
3b9a b0 de    BCS &3b7a
3b9c 00       BRK     
3b9d 2a       ROL A
3b9e 4f       ???
3b9f 75 74    ADC &74,X
3ba1 20 6f 66 JSR &666f
3ba4 20 dc 00 JSR &00dc
3ba7 2b       ???
3ba8 4e 6f 20 LSR &206f
3bab f5 00    SBC &00,X
3bad c8       INY     
3bae 84 1b    STY &1b
3bb0 60       RTS     
3bb1 20 1d 9b JSR &9b1d
3bb4 20 4c 98 JSR &984c
3bb7 20 ee 92 JSR &92ee
3bba a6 24    LDX &24
3bbc f0 e8    BEQ &3ba6
3bbe a5 2a    LDA &2a
3bc0 05 2b    ORA &2b
3bc2 05 2c    ORA &2c
3bc4 05 2d    ORA &2d
3bc6 f0 05    BEQ &3bcd
3bc8 c6 24    DEC &24
3bca 4c 9b 8b JMP &8b9b
3bcd bc a3 05 LDY &05a3,X
3bd0 bd b7 05 LDA &05b7,X
3bd3 4c dd b8 JMP &b8dd
3bd6 00       BRK     
3bd7 2c 54 6f BIT &6f54
3bda 6f       ???
3bdb 20 6d 61 JSR &616d
3bde 6e 79 20 ROR &2079
3be1 f5 73    SBC &73,X
3be3 00       BRK     
3be4 a6 24    LDX &24
3be6 e0 14    CPX #&14
3be8 b0 ec    BCS &3bd6
3bea 20 6d 98 JSR &986d
3bed a5 0b    LDA &0b
3bef 9d a4 05 STA &05a4,X
3bf2 a5 0c    LDA &0c
3bf4 9d b8 05 STA &05b8,X
3bf7 e6 24    INC &24
3bf9 4c a3 8b JMP &8ba3
3bfc a0 00    LDY #&00
3bfe a9 06    LDA #&06
3c00 d0 07    BNE &3c09
3c02 20 58 b5 JSR &b558
3c05 a0 00    LDY #&00
3c07 a9 07    LDA #&07
3c09 84 37    STY &37
3c0b 85 38    STA &38
3c0d a9 ee    LDA #&ee
3c0f 85 39    STA &39
3c11 a9 20    LDA #&20
3c13 85 3a    STA &3a
3c15 a0 ff    LDY #&ff
3c17 84 3b    STY &3b
3c19 c8       INY     
3c1a a2 37    LDX #&37
3c1c 98       TYA     
3c1d 20 f1 ff JSR &fff1
3c20 90 06    BCC &3c28
3c22 4c 38 98 JMP &9838
3c25 20 e7 ff JSR &ffe7
3c28 a9 00    LDA #&00
3c2a 85 1e    STA &1e
3c2c 60       RTS     
3c2d 20 70 99 JSR &9970
3c30 b0 4e    BCS &3c80
3c32 a5 3d    LDA &3d
3c34 e9 02    SBC #&02
3c36 85 37    STA &37
3c38 85 3d    STA &3d
3c3a 85 12    STA &12
3c3c a5 3e    LDA &3e
3c3e e9 00    SBC #&00
3c40 85 38    STA &38
3c42 85 13    STA &13
3c44 85 3e    STA &3e
3c46 a0 03    LDY #&03
3c48 b1 37    LDA (&37),Y
3c4a 18       CLC     
3c4b 65 37    ADC &37
3c4d 85 37    STA &37
3c4f 90 02    BCC &3c53
3c51 e6 38    INC &38
3c53 a0 00    LDY #&00
3c55 b1 37    LDA (&37),Y
3c57 91 12    STA (&12),Y
3c59 c9 0d    CMP #&0d
3c5b f0 09    BEQ &3c66
3c5d c8       INY     
3c5e d0 f5    BNE &3c55
3c60 e6 38    INC &38
3c62 e6 13    INC &13
3c64 d0 ef    BNE &3c55
3c66 c8       INY     
3c67 d0 04    BNE &3c6d
3c69 e6 38    INC &38
3c6b e6 13    INC &13
3c6d b1 37    LDA (&37),Y
3c6f 91 12    STA (&12),Y
3c71 30 09    BMI &3c7c
3c73 20 81 bc JSR &bc81
3c76 20 81 bc JSR &bc81
3c79 4c 5d bc JMP &bc5d
3c7c 20 92 be JSR &be92
3c7f 18       CLC     
3c80 60       RTS     
3c81 c8       INY     
3c82 d0 04    BNE &3c88
3c84 e6 13    INC &13
3c86 e6 38    INC &38
3c88 b1 37    LDA (&37),Y
3c8a 91 12    STA (&12),Y
3c8c 60       RTS     
3c8d 84 3b    STY &3b
3c8f 20 2d bc JSR &bc2d
3c92 a0 07    LDY #&07
3c94 84 3c    STY &3c
3c96 a0 00    LDY #&00
3c98 a9 0d    LDA #&0d
3c9a d1 3b    CMP (&3b),Y
3c9c f0 72    BEQ &3d10
3c9e c8       INY     
3c9f d1 3b    CMP (&3b),Y
3ca1 d0 fb    BNE &3c9e
3ca3 c8       INY     
3ca4 c8       INY     
3ca5 c8       INY     
3ca6 84 3f    STY &3f
3ca8 e6 3f    INC &3f
3caa a5 12    LDA &12
3cac 85 39    STA &39
3cae a5 13    LDA &13
3cb0 85 3a    STA &3a
3cb2 20 92 be JSR &be92
3cb5 85 37    STA &37
3cb7 a5 13    LDA &13
3cb9 85 38    STA &38
3cbb 88       DEY     
3cbc a5 06    LDA &06
3cbe c5 12    CMP &12
3cc0 a5 07    LDA &07
3cc2 e5 13    SBC &13
3cc4 b0 10    BCS &3cd6
3cc6 20 6f be JSR &be6f
3cc9 20 20 bd JSR &bd20
3ccc 00       BRK     
3ccd 00       BRK     
3cce 86 20    STX &20
3cd0 73       ???
3cd1 70 61    BVS &3d34
3cd3 63       ???
3cd4 65 00    ADC &00
3cd6 b1 39    LDA (&39),Y
3cd8 91 37    STA (&37),Y
3cda 98       TYA     
3cdb d0 04    BNE &3ce1
3cdd c6 3a    DEC &3a
3cdf c6 38    DEC &38
3ce1 88       DEY     
3ce2 98       TYA     
3ce3 65 39    ADC &39
3ce5 a6 3a    LDX &3a
3ce7 90 01    BCC &3cea
3ce9 e8       INX     
3cea c5 3d    CMP &3d
3cec 8a       TXA     
3ced e5 3e    SBC &3e
3cef b0 e5    BCS &3cd6
3cf1 38       SEC     
3cf2 a0 01    LDY #&01
3cf4 a5 2b    LDA &2b
3cf6 91 3d    STA (&3d),Y
3cf8 c8       INY     
3cf9 a5 2a    LDA &2a
3cfb 91 3d    STA (&3d),Y
3cfd c8       INY     
3cfe a5 3f    LDA &3f
3d00 91 3d    STA (&3d),Y
3d02 20 56 be JSR &be56
3d05 a0 ff    LDY #&ff
3d07 c8       INY     
3d08 b1 3b    LDA (&3b),Y
3d0a 91 3d    STA (&3d),Y
3d0c c9 0d    CMP #&0d
3d0e d0 f7    BNE &3d07
3d10 60       RTS     
3d11 20 57 98 JSR &9857
3d14 20 20 bd JSR &bd20
3d17 a5 18    LDA &18
3d19 85 0c    STA &0c
3d1b 86 0b    STX &0b
3d1d 4c 0b 8b JMP &8b0b
3d20 a5 12    LDA &12
3d22 85 00    STA &00
3d24 85 02    STA &02
3d26 a5 13    LDA &13
3d28 85 01    STA &01
3d2a 85 03    STA &03
3d2c 20 3a bd JSR &bd3a
3d2f a2 80    LDX #&80
3d31 a9 00    LDA #&00
3d33 9d 7f 04 STA &047f,X
3d36 ca       DEX     
3d37 d0 fa    BNE &3d33
3d39 60       RTS     
3d3a a5 18    LDA &18
3d3c 85 1d    STA &1d
3d3e a5 06    LDA &06
3d40 85 04    STA &04
3d42 a5 07    LDA &07
3d44 85 05    STA &05
3d46 a9 00    LDA #&00
3d48 85 24    STA &24
3d4a 85 26    STA &26
3d4c 85 25    STA &25
3d4e 85 1c    STA &1c
3d50 60       RTS     
3d51 a5 04    LDA &04
3d53 38       SEC     
3d54 e9 05    SBC #&05
3d56 20 2e be JSR &be2e
3d59 a0 00    LDY #&00
3d5b a5 30    LDA &30
3d5d 91 04    STA (&04),Y
3d5f c8       INY     
3d60 a5 2e    LDA &2e
3d62 29 80    AND #&80
3d64 85 2e    STA &2e
3d66 a5 31    LDA &31
3d68 29 7f    AND #&7f
3d6a 05 2e    ORA &2e
3d6c 91 04    STA (&04),Y
3d6e c8       INY     
3d6f a5 32    LDA &32
3d71 91 04    STA (&04),Y
3d73 c8       INY     
3d74 a5 33    LDA &33
3d76 91 04    STA (&04),Y
3d78 c8       INY     
3d79 a5 34    LDA &34
3d7b 91 04    STA (&04),Y
3d7d 60       RTS     
3d7e a5 04    LDA &04
3d80 18       CLC     
3d81 85 4b    STA &4b
3d83 69 05    ADC #&05
3d85 85 04    STA &04
3d87 a5 05    LDA &05
3d89 85 4c    STA &4c
3d8b 69 00    ADC #&00
3d8d 85 05    STA &05
3d8f 60       RTS     
3d90 f0 20    BEQ &3db2
3d92 30 bd    BMI &3d51
3d94 a5 04    LDA &04
3d96 38       SEC     
3d97 e9 04    SBC #&04
3d99 20 2e be JSR &be2e
3d9c a0 03    LDY #&03
3d9e a5 2d    LDA &2d
3da0 91 04    STA (&04),Y
3da2 88       DEY     
3da3 a5 2c    LDA &2c
3da5 91 04    STA (&04),Y
3da7 88       DEY     
3da8 a5 2b    LDA &2b
3daa 91 04    STA (&04),Y
3dac 88       DEY     
3dad a5 2a    LDA &2a
3daf 91 04    STA (&04),Y
3db1 60       RTS     
3db2 18       CLC     
3db3 a5 04    LDA &04
3db5 e5 36    SBC &36
3db7 20 2e be JSR &be2e
3dba a4 36    LDY &36
3dbc f0 08    BEQ &3dc6
3dbe b9 ff 05 LDA &05ff,Y
3dc1 91 04    STA (&04),Y
3dc3 88       DEY     
3dc4 d0 f8    BNE &3dbe
3dc6 a5 36    LDA &36
3dc8 91 04    STA (&04),Y
3dca 60       RTS     
3dcb a0 00    LDY #&00
3dcd b1 04    LDA (&04),Y
3dcf 85 36    STA &36
3dd1 f0 09    BEQ &3ddc
3dd3 a8       TAY     
3dd4 b1 04    LDA (&04),Y
3dd6 99 ff 05 STA &05ff,Y
3dd9 88       DEY     
3dda d0 f8    BNE &3dd4
3ddc a0 00    LDY #&00
3dde b1 04    LDA (&04),Y
3de0 38       SEC     
3de1 65 04    ADC &04
3de3 85 04    STA &04
3de5 90 23    BCC &3e0a
3de7 e6 05    INC &05
3de9 60       RTS     
3dea a0 03    LDY #&03
3dec b1 04    LDA (&04),Y
3dee 85 2d    STA &2d
3df0 88       DEY     
3df1 b1 04    LDA (&04),Y
3df3 85 2c    STA &2c
3df5 88       DEY     
3df6 b1 04    LDA (&04),Y
3df8 85 2b    STA &2b
3dfa 88       DEY     
3dfb b1 04    LDA (&04),Y
3dfd 85 2a    STA &2a
3dff 18       CLC     
3e00 a5 04    LDA &04
3e02 69 04    ADC #&04
3e04 85 04    STA &04
3e06 90 02    BCC &3e0a
3e08 e6 05    INC &05
3e0a 60       RTS     
3e0b a2 37    LDX #&37
3e0d a0 03    LDY #&03
3e0f b1 04    LDA (&04),Y
3e11 95 03    STA &03,X
3e13 88       DEY     
3e14 b1 04    LDA (&04),Y
3e16 95 02    STA &02,X
3e18 88       DEY     
3e19 b1 04    LDA (&04),Y
3e1b 95 01    STA &01,X
3e1d 88       DEY     
3e1e b1 04    LDA (&04),Y
3e20 95 00    STA &00,X
3e22 18       CLC     
3e23 a5 04    LDA &04
3e25 69 04    ADC #&04
3e27 85 04    STA &04
3e29 90 df    BCC &3e0a
3e2b e6 05    INC &05
3e2d 60       RTS     
3e2e 85 04    STA &04
3e30 b0 02    BCS &3e34
3e32 c6 05    DEC &05
3e34 a4 05    LDY &05
3e36 c4 03    CPY &03
3e38 90 07    BCC &3e41
3e3a d0 04    BNE &3e40
3e3c c5 02    CMP &02
3e3e 90 01    BCC &3e41
3e40 60       RTS     
3e41 4c b7 8c JMP &8cb7
3e44 a5 2a    LDA &2a
3e46 95 00    STA &00,X
3e48 a5 2b    LDA &2b
3e4a 95 01    STA &01,X
3e4c a5 2c    LDA &2c
3e4e 95 02    STA &02,X
3e50 a5 2d    LDA &2d
3e52 95 03    STA &03,X
3e54 60       RTS     
3e55 18       CLC     
3e56 98       TYA     
3e57 65 3d    ADC &3d
3e59 85 3d    STA &3d
3e5b 90 02    BCC &3e5f
3e5d e6 3e    INC &3e
3e5f a0 01    LDY #&01
3e61 60       RTS     
3e62 20 dd be JSR &bedd
3e65 a8       TAY     
3e66 a9 ff    LDA #&ff
3e68 84 3d    STY &3d
3e6a a2 37    LDX #&37
3e6c 20 dd ff JSR &ffdd
3e6f a5 18    LDA &18
3e71 85 13    STA &13
3e73 a0 00    LDY #&00
3e75 84 12    STY &12
3e77 c8       INY     
3e78 88       DEY     
3e79 b1 12    LDA (&12),Y
3e7b c9 0d    CMP #&0d
3e7d d0 1f    BNE &3e9e
3e7f c8       INY     
3e80 b1 12    LDA (&12),Y
3e82 30 0c    BMI &3e90
3e84 a0 03    LDY #&03
3e86 b1 12    LDA (&12),Y
3e88 f0 14    BEQ &3e9e
3e8a 18       CLC     
3e8b 20 93 be JSR &be93
3e8e d0 e8    BNE &3e78
3e90 c8       INY     
3e91 18       CLC     
3e92 98       TYA     
3e93 65 12    ADC &12
3e95 85 12    STA &12
3e97 90 02    BCC &3e9b
3e99 e6 13    INC &13
3e9b a0 01    LDY #&01
3e9d 60       RTS     
3e9e 20 cf bf JSR &bfcf
3ea1 0d 42 61 ORA &6142
3ea4 64       ???
3ea5 20 70 72 JSR &7270
3ea8 6f       ???
3ea9 67       ???
3eaa 72       ???
3eab 61 6d    ADC (&6d;X)
3ead 0d ea 4c ORA &4cea
3eb0 f6 8a    INC &8a,X
3eb2 a9 00    LDA #&00
3eb4 85 37    STA &37
3eb6 a9 06    LDA #&06
3eb8 85 38    STA &38
3eba a4 36    LDY &36
3ebc a9 0d    LDA #&0d
3ebe 99 00 06 STA &0600,Y
3ec1 60       RTS     
3ec2 20 d2 be JSR &bed2
3ec5 a2 00    LDX #&00
3ec7 a0 06    LDY #&06
3ec9 20 f7 ff JSR &fff7
3ecc 4c 9b 8b JMP &8b9b
3ecf 4c 0e 8c JMP &8c0e
3ed2 20 1d 9b JSR &9b1d
3ed5 d0 f8    BNE &3ecf
3ed7 20 b2 be JSR &beb2
3eda 4c 4c 98 JMP &984c
3edd 20 d2 be JSR &bed2
3ee0 88       DEY     
3ee1 84 39    STY &39
3ee3 a5 18    LDA &18
3ee5 85 3a    STA &3a
3ee7 a9 82    LDA #&82
3ee9 20 f4 ff JSR &fff4
3eec 86 3b    STX &3b
3eee 84 3c    STY &3c
3ef0 a9 00    LDA #&00
3ef2 60       RTS     
3ef3 20 6f be JSR &be6f
3ef6 a5 12    LDA &12
3ef8 85 45    STA &45
3efa a5 13    LDA &13
3efc 85 46    STA &46
3efe a9 23    LDA #&23
3f00 85 3d    STA &3d
3f02 a9 80    LDA #&80
3f04 85 3e    STA &3e
3f06 a5 18    LDA &18
3f08 85 42    STA &42
3f0a 20 dd be JSR &bedd
3f0d 86 3f    STX &3f
3f0f 84 40    STY &40
3f11 86 43    STX &43
3f13 84 44    STY &44
3f15 86 47    STX &47
3f17 84 48    STY &48
3f19 85 41    STA &41
3f1b a8       TAY     
3f1c a2 37    LDX #&37
3f1e 20 dd ff JSR &ffdd
3f21 4c 9b 8b JMP &8b9b
3f24 20 62 be JSR &be62
3f27 4c f3 8a JMP &8af3
3f2a 20 62 be JSR &be62
3f2d 4c 14 bd JMP &bd14
3f30 20 a9 bf JSR &bfa9
3f33 48       PHA     
3f34 20 13 98 JSR &9813
3f37 20 ee 92 JSR &92ee
3f3a 68       PLA     
3f3b a8       TAY     
3f3c a2 2a    LDX #&2a
3f3e a9 01    LDA #&01
3f40 20 da ff JSR &ffda
3f43 4c 9b 8b JMP &8b9b
3f46 38       SEC     
3f47 a9 00    LDA #&00
3f49 2a       ROL A
3f4a 2a       ROL A
3f4b 48       PHA     
3f4c 20 b5 bf JSR &bfb5
3f4f a2 2a    LDX #&2a
3f51 68       PLA     
3f52 20 da ff JSR &ffda
3f55 a9 40    LDA #&40
3f57 60       RTS     
3f58 20 a9 bf JSR &bfa9
3f5b 48       PHA     
3f5c 20 ae 8a JSR &8aae
3f5f 20 49 98 JSR &9849
3f62 20 ee 92 JSR &92ee
3f65 68       PLA     
3f66 a8       TAY     
3f67 a5 2a    LDA &2a
3f69 20 d4 ff JSR &ffd4
3f6c 4c 9b 8b JMP &8b9b
3f6f 20 b5 bf JSR &bfb5
3f72 20 d7 ff JSR &ffd7
3f75 4c d8 ae JMP &aed8
3f78 a9 40    LDA #&40
3f7a d0 06    BNE &3f82
3f7c a9 80    LDA #&80
3f7e d0 02    BNE &3f82
3f80 a9 c0    LDA #&c0
3f82 48       PHA     
3f83 20 ec ad JSR &adec
3f86 d0 0e    BNE &3f96
3f88 20 ba be JSR &beba
3f8b a2 00    LDX #&00
3f8d a0 06    LDY #&06
3f8f 68       PLA     
3f90 20 ce ff JSR &ffce
3f93 4c d8 ae JMP &aed8
3f96 4c 0e 8c JMP &8c0e
3f99 20 a9 bf JSR &bfa9
3f9c 20 52 98 JSR &9852
3f9f a4 2a    LDY &2a
3fa1 a9 00    LDA #&00
3fa3 20 ce ff JSR &ffce
3fa6 4c 9b 8b JMP &8b9b
3fa9 a5 0a    LDA &0a
3fab 85 1b    STA &1b
3fad a5 0b    LDA &0b
3faf 85 19    STA &19
3fb1 a5 0c    LDA &0c
3fb3 85 1a    STA &1a
3fb5 20 8c 8a JSR &8a8c
3fb8 c9 23    CMP #&23
3fba d0 07    BNE &3fc3
3fbc 20 e3 92 JSR &92e3
3fbf a4 2a    LDY &2a
3fc1 98       TYA     
3fc2 60       RTS     
3fc3 00       BRK     
3fc4 2d 4d 69 AND &694d
3fc7 73       ???
3fc8 73       ???
3fc9 69 6e    ADC #&6e
3fcb 67       ???
3fcc 20 23 00 JSR &0023
3fcf 68       PLA     
3fd0 85 37    STA &37
3fd2 68       PLA     
3fd3 85 38    STA &38
3fd5 a0 00    LDY #&00
3fd7 f0 03    BEQ &3fdc
3fd9 20 e3 ff JSR &ffe3
3fdc 20 4b 89 JSR &894b
3fdf 10 f8    BPL &3fd9
3fe1 6c 37 00 JMP (&0037)
3fe4 20 57 98 JSR &9857
3fe7 20 25 bc JSR &bc25
3fea a0 01    LDY #&01
3fec b1 fd    LDA (&fd),Y
3fee f0 06    BEQ &3ff6
3ff0 20 0e b5 JSR &b50e
3ff3 c8       INY     
3ff4 d0 f6    BNE &3fec
3ff6 4c 9b 8b JMP &8b9b
3ff9 00       BRK     
3ffa 52       ???
3ffb 6f       ???
3ffc 67       ???
3ffd 65 72    ADC &72
3fff 00       BRK     
4000 00       BRK     
