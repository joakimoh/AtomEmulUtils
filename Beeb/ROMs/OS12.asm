c000 00       BRK     
c001 00       BRK     
c002 00       BRK     
c003 00       BRK     
c004 00       BRK     
c005 00       BRK     
c006 00       BRK     
c007 00       BRK     
c008 18       CLC     
c009 18       CLC     
c00a 18       CLC     
c00b 18       CLC     
c00c 18       CLC     
c00d 00       BRK     
c00e 18       CLC     
c00f 00       BRK     
c010 6c 6c 6c JMP (&6c6c)
c013 00       BRK     
c014 00       BRK     
c015 00       BRK     
c016 00       BRK     
c017 00       BRK     
c018 36 36    ROL &36,X
c01a 7f       ???
c01b 36 7f    ROL &7f,X
c01d 36 36    ROL &36,X
c01f 00       BRK     
c020 0c       ???
c021 3f       ???
c022 68       PLA     
c023 3e 0b 7e ROL &7e0b,X
c026 18       CLC     
c027 00       BRK     
c028 60       RTS     
c029 66 0c    ROR &0c
c02b 18       CLC     
c02c 30 66    BMI &c094
c02e 06 00    ASL &00
c030 38       SEC     
c031 6c 6c 38 JMP (&386c)
c034 6d 66 3b ADC &3b66
c037 00       BRK     
c038 0c       ???
c039 18       CLC     
c03a 30 00    BMI &c03c
c03c 00       BRK     
c03d 00       BRK     
c03e 00       BRK     
c03f 00       BRK     
c040 0c       ???
c041 18       CLC     
c042 30 30    BMI &c074
c044 30 18    BMI &c05e
c046 0c       ???
c047 00       BRK     
c048 30 18    BMI &c062
c04a 0c       ???
c04b 0c       ???
c04c 0c       ???
c04d 18       CLC     
c04e 30 00    BMI &c050
c050 00       BRK     
c051 18       CLC     
c052 7e 3c 7e ROR &7e3c,X
c055 18       CLC     
c056 00       BRK     
c057 00       BRK     
c058 00       BRK     
c059 18       CLC     
c05a 18       CLC     
c05b 7e 18 18 ROR &1818,X
c05e 00       BRK     
c05f 00       BRK     
c060 00       BRK     
c061 00       BRK     
c062 00       BRK     
c063 00       BRK     
c064 00       BRK     
c065 18       CLC     
c066 18       CLC     
c067 30 00    BMI &c069
c069 00       BRK     
c06a 00       BRK     
c06b 7e 00 00 ROR &0000,X
c06e 00       BRK     
c06f 00       BRK     
c070 00       BRK     
c071 00       BRK     
c072 00       BRK     
c073 00       BRK     
c074 00       BRK     
c075 18       CLC     
c076 18       CLC     
c077 00       BRK     
c078 00       BRK     
c079 06 0c    ASL &0c
c07b 18       CLC     
c07c 30 60    BMI &c0de
c07e 00       BRK     
c07f 00       BRK     
c080 3c       ???
c081 66 6e    ROR &6e
c083 7e 76 66 ROR &6676,X
c086 3c       ???
c087 00       BRK     
c088 18       CLC     
c089 38       SEC     
c08a 18       CLC     
c08b 18       CLC     
c08c 18       CLC     
c08d 18       CLC     
c08e 7e 00 3c ROR &3c00,X
c091 66 06    ROR &06
c093 0c       ???
c094 18       CLC     
c095 30 7e    BMI &c115
c097 00       BRK     
c098 3c       ???
c099 66 06    ROR &06
c09b 1c       ???
c09c 06 66    ASL &66
c09e 3c       ???
c09f 00       BRK     
c0a0 0c       ???
c0a1 1c       ???
c0a2 3c       ???
c0a3 6c 7e 0c JMP (&0c7e)
c0a6 0c       ???
c0a7 00       BRK     
c0a8 7e 60 7c ROR &7c60,X
c0ab 06 06    ASL &06
c0ad 66 3c    ROR &3c
c0af 00       BRK     
c0b0 1c       ???
c0b1 30 60    BMI &c113
c0b3 7c       ???
c0b4 66 66    ROR &66
c0b6 3c       ???
c0b7 00       BRK     
c0b8 7e 06 0c ROR &0c06,X
c0bb 18       CLC     
c0bc 30 30    BMI &c0ee
c0be 30 00    BMI &c0c0
c0c0 3c       ???
c0c1 66 66    ROR &66
c0c3 3c       ???
c0c4 66 66    ROR &66
c0c6 3c       ???
c0c7 00       BRK     
c0c8 3c       ???
c0c9 66 66    ROR &66
c0cb 3e 06 0c ROL &0c06,X
c0ce 38       SEC     
c0cf 00       BRK     
c0d0 00       BRK     
c0d1 00       BRK     
c0d2 18       CLC     
c0d3 18       CLC     
c0d4 00       BRK     
c0d5 18       CLC     
c0d6 18       CLC     
c0d7 00       BRK     
c0d8 00       BRK     
c0d9 00       BRK     
c0da 18       CLC     
c0db 18       CLC     
c0dc 00       BRK     
c0dd 18       CLC     
c0de 18       CLC     
c0df 30 0c    BMI &c0ed
c0e1 18       CLC     
c0e2 30 60    BMI &c144
c0e4 30 18    BMI &c0fe
c0e6 0c       ???
c0e7 00       BRK     
c0e8 00       BRK     
c0e9 00       BRK     
c0ea 7e 00 7e ROR &7e00,X
c0ed 00       BRK     
c0ee 00       BRK     
c0ef 00       BRK     
c0f0 30 18    BMI &c10a
c0f2 0c       ???
c0f3 06 0c    ASL &0c
c0f5 18       CLC     
c0f6 30 00    BMI &c0f8
c0f8 3c       ???
c0f9 66 0c    ROR &0c
c0fb 18       CLC     
c0fc 18       CLC     
c0fd 00       BRK     
c0fe 18       CLC     
c0ff 00       BRK     
c100 3c       ???
c101 66 6e    ROR &6e
c103 6a       ROR A
c104 6e 60 3c ROR &3c60
c107 00       BRK     
c108 3c       ???
c109 66 66    ROR &66
c10b 7e 66 66 ROR &6666,X
c10e 66 00    ROR &00
c110 7c       ???
c111 66 66    ROR &66
c113 7c       ???
c114 66 66    ROR &66
c116 7c       ???
c117 00       BRK     
c118 3c       ???
c119 66 60    ROR &60
c11b 60       RTS     
c11c 60       RTS     
c11d 66 3c    ROR &3c
c11f 00       BRK     
c120 78       SEI     
c121 6c 66 66 JMP (&6666)
c124 66 6c    ROR &6c
c126 78       SEI     
c127 00       BRK     
c128 7e 60 60 ROR &6060,X
c12b 7c       ???
c12c 60       RTS     
c12d 60       RTS     
c12e 7e 00 7e ROR &7e00,X
c131 60       RTS     
c132 60       RTS     
c133 7c       ???
c134 60       RTS     
c135 60       RTS     
c136 60       RTS     
c137 00       BRK     
c138 3c       ???
c139 66 60    ROR &60
c13b 6e 66 66 ROR &6666
c13e 3c       ???
c13f 00       BRK     
c140 66 66    ROR &66
c142 66 7e    ROR &7e
c144 66 66    ROR &66
c146 66 00    ROR &00
c148 7e 18 18 ROR &1818,X
c14b 18       CLC     
c14c 18       CLC     
c14d 18       CLC     
c14e 7e 00 3e ROR &3e00,X
c151 0c       ???
c152 0c       ???
c153 0c       ???
c154 0c       ???
c155 6c 38 00 JMP (&0038)
c158 66 6c    ROR &6c
c15a 78       SEI     
c15b 70 78    BVS &c1d5
c15d 6c 66 00 JMP (&0066)
c160 60       RTS     
c161 60       RTS     
c162 60       RTS     
c163 60       RTS     
c164 60       RTS     
c165 60       RTS     
c166 7e 00 63 ROR &6300,X
c169 77       ???
c16a 7f       ???
c16b 6b       ???
c16c 6b       ???
c16d 63       ???
c16e 63       ???
c16f 00       BRK     
c170 66 66    ROR &66
c172 76 7e    ROR &7e,X
c174 6e 66 66 ROR &6666
c177 00       BRK     
c178 3c       ???
c179 66 66    ROR &66
c17b 66 66    ROR &66
c17d 66 3c    ROR &3c
c17f 00       BRK     
c180 7c       ???
c181 66 66    ROR &66
c183 7c       ???
c184 60       RTS     
c185 60       RTS     
c186 60       RTS     
c187 00       BRK     
c188 3c       ???
c189 66 66    ROR &66
c18b 66 6a    ROR &6a
c18d 6c 36 00 JMP (&0036)
c190 7c       ???
c191 66 66    ROR &66
c193 7c       ???
c194 6c 66 66 JMP (&6666)
c197 00       BRK     
c198 3c       ???
c199 66 60    ROR &60
c19b 3c       ???
c19c 06 66    ASL &66
c19e 3c       ???
c19f 00       BRK     
c1a0 7e 18 18 ROR &1818,X
c1a3 18       CLC     
c1a4 18       CLC     
c1a5 18       CLC     
c1a6 18       CLC     
c1a7 00       BRK     
c1a8 66 66    ROR &66
c1aa 66 66    ROR &66
c1ac 66 66    ROR &66
c1ae 3c       ???
c1af 00       BRK     
c1b0 66 66    ROR &66
c1b2 66 66    ROR &66
c1b4 66 3c    ROR &3c
c1b6 18       CLC     
c1b7 00       BRK     
c1b8 63       ???
c1b9 63       ???
c1ba 6b       ???
c1bb 6b       ???
c1bc 7f       ???
c1bd 77       ???
c1be 63       ???
c1bf 00       BRK     
c1c0 66 66    ROR &66
c1c2 3c       ???
c1c3 18       CLC     
c1c4 3c       ???
c1c5 66 66    ROR &66
c1c7 00       BRK     
c1c8 66 66    ROR &66
c1ca 66 3c    ROR &3c
c1cc 18       CLC     
c1cd 18       CLC     
c1ce 18       CLC     
c1cf 00       BRK     
c1d0 7e 06 0c ROR &0c06,X
c1d3 18       CLC     
c1d4 30 60    BMI &c236
c1d6 7e 00 7c ROR &7c00,X
c1d9 60       RTS     
c1da 60       RTS     
c1db 60       RTS     
c1dc 60       RTS     
c1dd 60       RTS     
c1de 7c       ???
c1df 00       BRK     
c1e0 00       BRK     
c1e1 60       RTS     
c1e2 30 18    BMI &c1fc
c1e4 0c       ???
c1e5 06 00    ASL &00
c1e7 00       BRK     
c1e8 3e 06 06 ROL &0606,X
c1eb 06 06    ASL &06
c1ed 06 3e    ASL &3e
c1ef 00       BRK     
c1f0 18       CLC     
c1f1 3c       ???
c1f2 66 42    ROR &42
c1f4 00       BRK     
c1f5 00       BRK     
c1f6 00       BRK     
c1f7 00       BRK     
c1f8 00       BRK     
c1f9 00       BRK     
c1fa 00       BRK     
c1fb 00       BRK     
c1fc 00       BRK     
c1fd 00       BRK     
c1fe 00       BRK     
c1ff ff       ???
c200 1c       ???
c201 36 30    ROL &30,X
c203 7c       ???
c204 30 30    BMI &c236
c206 7e 00 00 ROR &0000,X
c209 00       BRK     
c20a 3c       ???
c20b 06 3e    ASL &3e
c20d 66 3e    ROR &3e
c20f 00       BRK     
c210 60       RTS     
c211 60       RTS     
c212 7c       ???
c213 66 66    ROR &66
c215 66 7c    ROR &7c
c217 00       BRK     
c218 00       BRK     
c219 00       BRK     
c21a 3c       ???
c21b 66 60    ROR &60
c21d 66 3c    ROR &3c
c21f 00       BRK     
c220 06 06    ASL &06
c222 3e 66 66 ROL &6666,X
c225 66 3e    ROR &3e
c227 00       BRK     
c228 00       BRK     
c229 00       BRK     
c22a 3c       ???
c22b 66 7e    ROR &7e
c22d 60       RTS     
c22e 3c       ???
c22f 00       BRK     
c230 1c       ???
c231 30 30    BMI &c263
c233 7c       ???
c234 30 30    BMI &c266
c236 30 00    BMI &c238
c238 00       BRK     
c239 00       BRK     
c23a 3e 66 66 ROL &6666,X
c23d 3e 06 3c ROL &3c06,X
c240 60       RTS     
c241 60       RTS     
c242 7c       ???
c243 66 66    ROR &66
c245 66 66    ROR &66
c247 00       BRK     
c248 18       CLC     
c249 00       BRK     
c24a 38       SEC     
c24b 18       CLC     
c24c 18       CLC     
c24d 18       CLC     
c24e 3c       ???
c24f 00       BRK     
c250 18       CLC     
c251 00       BRK     
c252 38       SEC     
c253 18       CLC     
c254 18       CLC     
c255 18       CLC     
c256 18       CLC     
c257 70 60    BVS &c2b9
c259 60       RTS     
c25a 66 6c    ROR &6c
c25c 78       SEI     
c25d 6c 66 00 JMP (&0066)
c260 38       SEC     
c261 18       CLC     
c262 18       CLC     
c263 18       CLC     
c264 18       CLC     
c265 18       CLC     
c266 3c       ???
c267 00       BRK     
c268 00       BRK     
c269 00       BRK     
c26a 36 7f    ROL &7f,X
c26c 6b       ???
c26d 6b       ???
c26e 63       ???
c26f 00       BRK     
c270 00       BRK     
c271 00       BRK     
c272 7c       ???
c273 66 66    ROR &66
c275 66 66    ROR &66
c277 00       BRK     
c278 00       BRK     
c279 00       BRK     
c27a 3c       ???
c27b 66 66    ROR &66
c27d 66 3c    ROR &3c
c27f 00       BRK     
c280 00       BRK     
c281 00       BRK     
c282 7c       ???
c283 66 66    ROR &66
c285 7c       ???
c286 60       RTS     
c287 60       RTS     
c288 00       BRK     
c289 00       BRK     
c28a 3e 66 66 ROL &6666,X
c28d 3e 06 07 ROL &0706,X
c290 00       BRK     
c291 00       BRK     
c292 6c 76 60 JMP (&6076)
c295 60       RTS     
c296 60       RTS     
c297 00       BRK     
c298 00       BRK     
c299 00       BRK     
c29a 3e 60 3c ROL &3c60,X
c29d 06 7c    ASL &7c
c29f 00       BRK     
c2a0 30 30    BMI &c2d2
c2a2 7c       ???
c2a3 30 30    BMI &c2d5
c2a5 30 1c    BMI &c2c3
c2a7 00       BRK     
c2a8 00       BRK     
c2a9 00       BRK     
c2aa 66 66    ROR &66
c2ac 66 66    ROR &66
c2ae 3e 00 00 ROL &0000,X
c2b1 00       BRK     
c2b2 66 66    ROR &66
c2b4 66 3c    ROR &3c
c2b6 18       CLC     
c2b7 00       BRK     
c2b8 00       BRK     
c2b9 00       BRK     
c2ba 63       ???
c2bb 6b       ???
c2bc 6b       ???
c2bd 7f       ???
c2be 36 00    ROL &00,X
c2c0 00       BRK     
c2c1 00       BRK     
c2c2 66 3c    ROR &3c
c2c4 18       CLC     
c2c5 3c       ???
c2c6 66 00    ROR &00
c2c8 00       BRK     
c2c9 00       BRK     
c2ca 66 66    ROR &66
c2cc 66 3e    ROR &3e
c2ce 06 3c    ASL &3c
c2d0 00       BRK     
c2d1 00       BRK     
c2d2 7e 0c 18 ROR &180c,X
c2d5 30 7e    BMI &c355
c2d7 00       BRK     
c2d8 0c       ???
c2d9 18       CLC     
c2da 18       CLC     
c2db 70 18    BVS &c2f5
c2dd 18       CLC     
c2de 0c       ???
c2df 00       BRK     
c2e0 18       CLC     
c2e1 18       CLC     
c2e2 18       CLC     
c2e3 00       BRK     
c2e4 18       CLC     
c2e5 18       CLC     
c2e6 18       CLC     
c2e7 00       BRK     
c2e8 30 18    BMI &c302
c2ea 18       CLC     
c2eb 0e 18 18 ASL &1818
c2ee 30 00    BMI &c2f0
c2f0 31 6b    AND (&6b),Y
c2f2 46 00    LSR &00
c2f4 00       BRK     
c2f5 00       BRK     
c2f6 00       BRK     
c2f7 00       BRK     
c2f8 ff       ???
c2f9 ff       ???
c2fa ff       ???
c2fb ff       ???
c2fc ff       ???
c2fd ff       ???
c2fe ff       ???
c2ff ff       ???
c300 4c 1d cb JMP &cb1d
c303 0d 42 42 ORA &4242
c306 43       ???
c307 20 43 6f JSR &6f43
c30a 6d 70 75 ADC &7570
c30d 74       ???
c30e 65 72    ADC &72
c310 20 00 31 JSR &3100
c313 36 4b    ROL &4b,X
c315 07       ???
c316 00       BRK     
c317 33       ???
c318 32       ???
c319 4b       ???
c31a 07       ???
c31b 00       BRK     
c31c 08       PHP     
c31d 0d 0d 00 ORA &000d
c320 11 22    ORA (&22),Y
c322 33       ???
c323 44       ???
c324 55 66    EOR &66,X
c326 77       ???
c327 88       DEY     
c328 99 aa bb STA &bbaa,Y
c32b cc dd ee CPY &eedd
c32e ff       ???
c32f 00       BRK     
c330 55 aa    EOR &aa,X
c332 ff       ???
c333 11 3b    ORA (&3b),Y
c335 96 a1    STX &a1,Y
c337 ad b9 11 LDA &11b9
c33a 6f       ???
c33b c5 64    CMP &64
c33d f0 5b    BEQ &c39a
c33f 59 af 8d EOR &8daf,Y
c342 a6 c0    LDX &c0
c344 f9 fd 92 SBC &92fd,Y
c347 39 9b eb AND &eb9b,Y
c34a f1 39    SBC (&39),Y
c34c 8c bd 11 STY &11bd
c34f fa       ???
c350 a2 79    LDX #&79
c352 87       ???
c353 ac c5 2f LDY &2fc5
c356 c5 c5    CMP &c5
c358 c5 c5    CMP &c5
c35a c5 e8    CMP &e8
c35c c5 c6    CMP &c6
c35e c6 c6    DEC &c6
c360 c7       ???
c361 c7       ???
c362 c5 c5    CMP &c5
c364 c7       ???
c365 4f       ???
c366 4e 5b c8 LSR &c85b
c369 c5 5f    CMP &5f
c36b 57       ???
c36c 78       SEI     
c36d 6b       ???
c36e c9 c5    CMP #&c5
c370 3c       ???
c371 7c       ???
c372 c7       ???
c373 4e ca 00 LSR &00ca
c376 00       BRK     
c377 02       ???
c378 80       ???
c379 05 00    ORA &00
c37b 07       ???
c37c 80       ???
c37d 0a       ASL A
c37e 00       BRK     
c37f 0c       ???
c380 80       ???
c381 0f       ???
c382 00       BRK     
c383 11 80    ORA (&80),Y
c385 14       ???
c386 00       BRK     
c387 16 80    ASL &80,X
c389 19 00 1b ORA &1b00,Y
c38c 80       ???
c38d 1e 00 20 ASL &2000,X
c390 80       ???
c391 23       ???
c392 00       BRK     
c393 25 80    AND &80
c395 28       PLP     
c396 00       BRK     
c397 2a       ROL A
c398 80       ???
c399 2d 00 2f AND &2f00
c39c 80       ???
c39d 32       ???
c39e 00       BRK     
c39f 34       ???
c3a0 80       ???
c3a1 37       ???
c3a2 00       BRK     
c3a3 39 80 3c AND &3c80,Y
c3a6 00       BRK     
c3a7 3e 80 41 ROL &4180,X
c3aa 00       BRK     
c3ab 43       ???
c3ac 80       ???
c3ad 46 00    LSR &00
c3af 48       PHA     
c3b0 80       ???
c3b1 4b       ???
c3b2 00       BRK     
c3b3 4d 80 00 EOR &0080
c3b6 00       BRK     
c3b7 00       BRK     
c3b8 28       PLP     
c3b9 00       BRK     
c3ba 50 00    BVC &c3bc
c3bc 78       SEI     
c3bd 00       BRK     
c3be a0 00    LDY #&00
c3c0 c8       INY     
c3c1 00       BRK     
c3c2 f0 01    BEQ &c3c5
c3c4 18       CLC     
c3c5 01 40    ORA (&40;X)
c3c7 01 68    ORA (&68;X)
c3c9 01 90    ORA (&90;X)
c3cb 01 b8    ORA (&b8;X)
c3cd 01 e0    ORA (&e0;X)
c3cf 02       ???
c3d0 08       PHP     
c3d1 02       ???
c3d2 30 02    BMI &c3d6
c3d4 58       CLI     
c3d5 02       ???
c3d6 80       ???
c3d7 02       ???
c3d8 a8       TAY     
c3d9 02       ???
c3da d0 02    BNE &c3de
c3dc f8       SED     
c3dd 03       ???
c3de 20 03 48 JSR &4803
c3e1 03       ???
c3e2 70 03    BVS &c3e7
c3e4 98       TYA     
c3e5 03       ???
c3e6 c0 1f    CPY #&1f
c3e8 1f       ???
c3e9 1f       ???
c3ea 18       CLC     
c3eb 1f       ???
c3ec 1f       ???
c3ed 18       CLC     
c3ee 18       CLC     
c3ef 4f       ???
c3f0 27       ???
c3f1 13       ???
c3f2 4f       ???
c3f3 27       ???
c3f4 13       ???
c3f5 27       ???
c3f6 27       ???
c3f7 9c       ???
c3f8 d8       CLD     
c3f9 f4       ???
c3fa 9c       ???
c3fb 88       DEY     
c3fc c4 88    CPY &88
c3fe 4b       ???
c3ff 08       PHP     
c400 10 20    BPL &c422
c402 08       PHP     
c403 08       PHP     
c404 10 08    BPL &c40e
c406 01 aa    ORA (&aa;X)
c408 55 88    EOR &88,X
c40a 44       ???
c40b 22       ???
c40c 11 80    ORA (&80),Y
c40e 40       RTI     
c40f 20 10 08 JSR &0810
c412 04       ???
c413 02       ???
c414 01 03    ORA (&03;X)
c416 0f       ???
c417 01 01    ORA (&01;X)
c419 03       ???
c41a 01 00    ORA (&00;X)
c41c ff       ???
c41d 00       BRK     
c41e 00       BRK     
c41f ff       ???
c420 ff       ???
c421 ff       ???
c422 ff       ???
c423 00       BRK     
c424 00       BRK     
c425 ff       ???
c426 00       BRK     
c427 0f       ???
c428 f0 ff    BEQ &c429
c42a 00       BRK     
c42b 03       ???
c42c 0c       ???
c42d 0f       ???
c42e 30 33    BMI &c463
c430 3c       ???
c431 3f       ???
c432 c0 c3    CPY #&c3
c434 cc cf f0 CPY &f0cf
c437 f3       ???
c438 fc       ???
c439 ff       ???
c43a 07       ???
c43b 03       ???
c43c 01 00    ORA (&00;X)
c43e 07       ???
c43f 03       ???
c440 00       BRK     
c441 00       BRK     
c442 00       BRK     
c443 01 02    ORA (&02;X)
c445 02       ???
c446 03       ???
c447 04       ???
c448 00       BRK     
c449 06 02    ASL &02
c44b 0d 05 0d ORA &0d05
c44e 05 04    ORA &04
c450 04       ???
c451 0c       ???
c452 0c       ???
c453 04       ???
c454 02       ???
c455 32       ???
c456 7a       ???
c457 92       ???
c458 e6 50    INC &50
c45a 40       RTI     
c45b 28       PLP     
c45c 20 04 30 JSR &3004
c45f 40       RTI     
c460 58       CLI     
c461 60       RTS     
c462 7c       ???
c463 28       PLP     
c464 40       RTI     
c465 80       ???
c466 b5 75    LDA &75,X
c468 75 0b    ADC &0b,X
c46a 17       ???
c46b 23       ???
c46c 2f       ???
c46d 3b       ???
c46e 7f       ???
c46f 50 62    BVC &c4d3
c471 28       PLP     
c472 26 00    ROL &00
c474 20 22 01 JSR &0122
c477 07       ???
c478 67       ???
c479 08       PHP     
c47a 7f       ???
c47b 50 62    BVC &c4df
c47d 28       PLP     
c47e 1e 02 19 ASL &1902,X
c481 1b       ???
c482 01 09    ORA (&09;X)
c484 67       ???
c485 09 3f    ORA #&3f
c487 28       PLP     
c488 31 24    AND (&24),Y
c48a 26 00    ROL &00
c48c 20 22 01 JSR &0122
c48f 07       ???
c490 67       ???
c491 08       PHP     
c492 3f       ???
c493 28       PLP     
c494 31 24    AND (&24),Y
c496 1e 02 19 ASL &1902,X
c499 1b       ???
c49a 01 09    ORA (&09;X)
c49c 67       ???
c49d 09 3f    ORA #&3f
c49f 28       PLP     
c4a0 33       ???
c4a1 24 1e    BIT &1e
c4a3 02       ???
c4a4 19 1b 93 ORA &931b,Y
c4a7 12       ???
c4a8 72       ???
c4a9 13       ???
c4aa 86 d3    STX &d3
c4ac 7e d3 6a ROR &6ad3,X
c4af 74       ???
c4b0 42       ???
c4b1 4b       ???
c4b2 d3       ???
c4b3 d3       ???
c4b4 d3       ???
c4b5 d3       ???
c4b6 23       ???
c4b7 5f       ???
c4b8 60       RTS     
c4b9 23       ???
c4ba 04       ???
c4bb 05 06    ORA &06
c4bd 00       BRK     
c4be 01 02    ORA (&02;X)
c4c0 ae 6a 02 LDX &026a
c4c3 d0 4d    BNE &c512
c4c5 24 d0    BIT &d0
c4c7 50 0f    BVC &c4d8
c4c9 20 68 c5 JSR &c568
c4cc 20 6a cd JSR &cd6a
c4cf 30 07    BMI &c4d8
c4d1 c9 0d    CMP #&0d
c4d3 d0 03    BNE &c4d8
c4d5 20 18 d9 JSR &d918
c4d8 c9 7f    CMP #&7f
c4da f0 11    BEQ &c4ed
c4dc c9 20    CMP #&20
c4de 90 0f    BCC &c4ef
c4e0 24 d0    BIT &d0
c4e2 30 06    BMI &c4ea
c4e4 20 b7 cf JSR &cfb7
c4e7 20 64 c6 JSR &c664
c4ea 4c 5e c5 JMP &c55e
c4ed a9 20    LDA #&20
c4ef a8       TAY     
c4f0 b9 33 c3 LDA &c333,Y
c4f3 8d 5d 03 STA &035d
c4f6 b9 54 c3 LDA &c354,Y
c4f9 30 4a    BMI &c545
c4fb aa       TAX     
c4fc 09 f0    ORA #&f0
c4fe 8d 6a 02 STA &026a
c501 8a       TXA     
c502 4a       LSR A
c503 4a       LSR A
c504 4a       LSR A
c505 4a       LSR A
c506 18       CLC     
c507 69 c3    ADC #&c3
c509 8d 5e 03 STA &035e
c50c 24 d0    BIT &d0
c50e 70 1f    BVS &c52f
c510 18       CLC     
c511 60       RTS     
c512 9d 24 02 STA &0224,X
c515 e8       INX     
c516 8e 6a 02 STX &026a
c519 d0 17    BNE &c532
c51b 24 d0    BIT &d0
c51d 30 15    BMI &c534
c51f 70 05    BVS &c526
c521 20 f5 cc JSR &ccf5
c524 18       CLC     
c525 60       RTS     
c526 20 68 c5 JSR &c568
c529 20 6a cd JSR &cd6a
c52c 20 f5 cc JSR &ccf5
c52f 20 65 c5 JSR &c565
c532 18       CLC     
c533 60       RTS     
c534 ac 5e 03 LDY &035e
c537 c0 c5    CPY #&c5
c539 d0 f7    BNE &c532
c53b aa       TAX     
c53c a5 d0    LDA &d0
c53e 4a       LSR A
c53f 90 d0    BCC &c511
c541 8a       TXA     
c542 4c 1e e1 JMP &e11e
c545 8d 5e 03 STA &035e
c548 98       TYA     
c549 c9 08    CMP #&08
c54b 90 06    BCC &c553
c54d 49 ff    EOR #&ff
c54f c9 f2    CMP #&f2
c551 49 ff    EOR #&ff
c553 24 d0    BIT &d0
c555 30 29    BMI &c580
c557 08       PHP     
c558 20 f5 cc JSR &ccf5
c55b 28       PLP     
c55c 90 03    BCC &c561
c55e a5 d0    LDA &d0
c560 4a       LSR A
c561 24 d0    BIT &d0
c563 50 ac    BVC &c511
c565 20 7a cd JSR &cd7a
c568 08       PHP     
c569 48       PHA     
c56a a2 18    LDX #&18
c56c a0 64    LDY #&64
c56e 20 de cd JSR &cdde
c571 20 06 cf JSR &cf06
c574 20 02 ca JSR &ca02
c577 a5 d0    LDA &d0
c579 49 02    EOR #&02
c57b 85 d0    STA &d0
c57d 68       PLA     
c57e 28       PLP     
c57f 60       RTS     
c580 49 06    EOR #&06
c582 d0 08    BNE &c58c
c584 a9 7f    LDA #&7f
c586 90 20    BCC &c5a8
c588 a5 d0    LDA &d0
c58a 29 20    AND #&20
c58c 60       RTS     
c58d a0 00    LDY #&00
c58f 8c 69 02 STY &0269
c592 a9 04    LDA #&04
c594 d0 07    BNE &c59d
c596 20 a2 e1 JSR &e1a2
c599 a9 94    LDA #&94
c59b 49 95    EOR #&95
c59d 05 d0    ORA &d0
c59f d0 09    BNE &c5aa
c5a1 20 a2 e1 JSR &e1a2
c5a4 a9 0a    LDA #&0a
c5a6 49 f4    EOR #&f4
c5a8 25 d0    AND &d0
c5aa 85 d0    STA &d0
c5ac 60       RTS     
c5ad ad 61 03 LDA &0361
c5b0 f0 fa    BEQ &c5ac
c5b2 20 51 c9 JSR &c951
c5b5 a9 df    LDA #&df
c5b7 d0 ef    BNE &c5a8
c5b9 ad 61 03 LDA &0361
c5bc f0 ee    BEQ &c5ac
c5be a9 20    LDA #&20
c5c0 20 54 c9 JSR &c954
c5c3 d0 d8    BNE &c59d
c5c5 20 88 c5 JSR &c588
c5c8 d0 55    BNE &c61f
c5ca ce 18 03 DEC &0318
c5cd ae 18 03 LDX &0318
c5d0 ec 08 03 CPX &0308
c5d3 30 19    BMI &c5ee
c5d5 ad 4a 03 LDA &034a
c5d8 38       SEC     
c5d9 ed 4f 03 SBC &034f
c5dc aa       TAX     
c5dd ad 4b 03 LDA &034b
c5e0 e9 00    SBC #&00
c5e2 cd 4e 03 CMP &034e
c5e5 b0 03    BCS &c5ea
c5e7 6d 54 03 ADC &0354
c5ea a8       TAY     
c5eb 4c f6 c9 JMP &c9f6
c5ee ad 0a 03 LDA &030a
c5f1 8d 18 03 STA &0318
c5f4 ce 69 02 DEC &0269
c5f7 10 03    BPL &c5fc
c5f9 ee 69 02 INC &0269
c5fc ae 19 03 LDX &0319
c5ff ec 0b 03 CPX &030b
c602 f0 06    BEQ &c60a
c604 ce 19 03 DEC &0319
c607 4c af c6 JMP &c6af
c60a 18       CLC     
c60b 20 3f cd JSR &cd3f
c60e a9 08    LDA #&08
c610 24 d0    BIT &d0
c612 d0 05    BNE &c619
c614 20 94 c9 JSR &c994
c617 d0 03    BNE &c61c
c619 20 a4 cd JSR &cda4
c61c 4c ac c6 JMP &c6ac
c61f a2 00    LDX #&00
c621 86 db    STX &db
c623 20 0d d1 JSR &d10d
c626 a6 db    LDX &db
c628 38       SEC     
c629 bd 24 03 LDA &0324,X
c62c e9 08    SBC #&08
c62e 9d 24 03 STA &0324,X
c631 b0 03    BCS &c636
c633 de 25 03 DEC &0325,X
c636 a5 da    LDA &da
c638 d0 1e    BNE &c658
c63a 20 0d d1 JSR &d10d
c63d f0 19    BEQ &c658
c63f a6 db    LDX &db
c641 bd 04 03 LDA &0304,X
c644 e0 01    CPX #&01
c646 b0 02    BCS &c64a
c648 e9 06    SBC #&06
c64a 9d 24 03 STA &0324,X
c64d bd 05 03 LDA &0305,X
c650 e9 00    SBC #&00
c652 9d 25 03 STA &0325,X
c655 8a       TXA     
c656 f0 08    BEQ &c660
c658 4c b8 d1 JMP &d1b8
c65b 20 88 c5 JSR &c588
c65e f0 94    BEQ &c5f4
c660 a2 02    LDX #&02
c662 d0 52    BNE &c6b6
c664 a5 d0    LDA &d0
c666 29 20    AND #&20
c668 d0 4a    BNE &c6b4
c66a ae 18 03 LDX &0318
c66d ec 0a 03 CPX &030a
c670 b0 12    BCS &c684
c672 ee 18 03 INC &0318
c675 ad 4a 03 LDA &034a
c678 6d 4f 03 ADC &034f
c67b aa       TAX     
c67c ad 4b 03 LDA &034b
c67f 69 00    ADC #&00
c681 4c f6 c9 JMP &c9f6
c684 ad 08 03 LDA &0308
c687 8d 18 03 STA &0318
c68a 18       CLC     
c68b 20 e3 ca JSR &cae3
c68e ae 19 03 LDX &0319
c691 ec 09 03 CPX &0309
c694 b0 05    BCS &c69b
c696 ee 19 03 INC &0319
c699 90 14    BCC &c6af
c69b 20 3f cd JSR &cd3f
c69e a9 08    LDA #&08
c6a0 24 d0    BIT &d0
c6a2 d0 05    BNE &c6a9
c6a4 20 a4 c9 JSR &c9a4
c6a7 d0 03    BNE &c6ac
c6a9 20 ff cd JSR &cdff
c6ac 20 ac ce JSR &ceac
c6af 20 06 cf JSR &cf06
c6b2 90 7e    BCC &c732
c6b4 a2 00    LDX #&00
c6b6 86 db    STX &db
c6b8 20 0d d1 JSR &d10d
c6bb a6 db    LDX &db
c6bd 18       CLC     
c6be bd 24 03 LDA &0324,X
c6c1 69 08    ADC #&08
c6c3 9d 24 03 STA &0324,X
c6c6 90 03    BCC &c6cb
c6c8 fe 25 03 INC &0325,X
c6cb a5 da    LDA &da
c6cd d0 89    BNE &c658
c6cf 20 0d d1 JSR &d10d
c6d2 f0 84    BEQ &c658
c6d4 a6 db    LDX &db
c6d6 bd 00 03 LDA &0300,X
c6d9 e0 01    CPX #&01
c6db 90 02    BCC &c6df
c6dd 69 06    ADC #&06
c6df 9d 24 03 STA &0324,X
c6e2 bd 01 03 LDA &0301,X
c6e5 69 00    ADC #&00
c6e7 9d 25 03 STA &0325,X
c6ea 8a       TXA     
c6eb f0 08    BEQ &c6f5
c6ed 4c b8 d1 JMP &d1b8
c6f0 20 88 c5 JSR &c588
c6f3 f0 95    BEQ &c68a
c6f5 a2 02    LDX #&02
c6f7 4c 21 c6 JMP &c621
c6fa ae 55 03 LDX &0355
c6fd ad 21 03 LDA &0321
c700 cd 23 03 CMP &0323
c703 90 53    BCC &c758
c705 dd e7 c3 CMP &c3e7,X
c708 f0 02    BEQ &c70c
c70a b0 4c    BCS &c758
c70c ad 22 03 LDA &0322
c70f a8       TAY     
c710 dd ef c3 CMP &c3ef,X
c713 f0 02    BEQ &c717
c715 b0 41    BCS &c758
c717 38       SEC     
c718 ed 20 03 SBC &0320
c71b 30 3b    BMI &c758
c71d a8       TAY     
c71e 20 88 ca JSR &ca88
c721 a9 08    LDA #&08
c723 20 9d c5 JSR &c59d
c726 a2 20    LDX #&20
c728 a0 08    LDY #&08
c72a 20 8a d4 JSR &d48a
c72d 20 e8 ce JSR &cee8
c730 b0 47    BCS &c779
c732 4c 02 ca JMP &ca02
c735 a0 03    LDY #&03
c737 b1 f0    LDA (&f0),Y
c739 99 28 03 STA &0328,Y
c73c 88       DEY     
c73d 10 f8    BPL &c737
c73f a9 28    LDA #&28
c741 20 39 d8 JSR &d839
c744 a0 04    LDY #&04
c746 d0 08    BNE &c750
c748 2d 60 03 AND &0360
c74b aa       TAX     
c74c bd 6f 03 LDA &036f,X
c74f c8       INY     
c750 91 f0    STA (&f0),Y
c752 a9 00    LDA #&00
c754 c0 04    CPY #&04
c756 d0 f7    BNE &c74f
c758 60       RTS     
c759 20 88 c5 JSR &c588
c75c d0 5f    BNE &c7bd
c75e a5 d0    LDA &d0
c760 29 08    AND #&08
c762 d0 03    BNE &c767
c764 4c c1 cb JMP &cbc1
c767 ae 0b 03 LDX &030b
c76a 8e 19 03 STX &0319
c76d 20 ac ce JSR &ceac
c770 ae 19 03 LDX &0319
c773 ec 09 03 CPX &0309
c776 e8       INX     
c777 90 f1    BCC &c76a
c779 20 88 c5 JSR &c588
c77c f0 03    BEQ &c781
c77e 4c a6 cf JMP &cfa6
c781 8d 23 03 STA &0323
c784 8d 22 03 STA &0322
c787 20 88 c5 JSR &c588
c78a d0 cc    BNE &c758
c78c 20 a8 c7 JSR &c7a8
c78f 18       CLC     
c790 ad 22 03 LDA &0322
c793 6d 08 03 ADC &0308
c796 8d 18 03 STA &0318
c799 ad 23 03 LDA &0323
c79c 18       CLC     
c79d 6d 0b 03 ADC &030b
c7a0 8d 19 03 STA &0319
c7a3 20 e8 ce JSR &cee8
c7a6 90 8a    BCC &c732
c7a8 a2 18    LDX #&18
c7aa a0 28    LDY #&28
c7ac 4c de cd JMP &cdde
c7af 20 88 c5 JSR &c588
c7b2 f0 03    BEQ &c7b7
c7b4 4c ad cf JMP &cfad
c7b7 20 6e ce JSR &ce6e
c7ba 4c af c6 JMP &c6af
c7bd 20 a6 cf JSR &cfa6
c7c0 ad 61 03 LDA &0361
c7c3 f0 33    BEQ &c7f8
c7c5 ae 5a 03 LDX &035a
c7c8 ac 5c 03 LDY &035c
c7cb 20 b3 d0 JSR &d0b3
c7ce a2 00    LDX #&00
c7d0 a0 28    LDY #&28
c7d2 20 7c d4 JSR &d47c
c7d5 38       SEC     
c7d6 ad 06 03 LDA &0306
c7d9 ed 02 03 SBC &0302
c7dc a8       TAY     
c7dd c8       INY     
c7de 8c 30 03 STY &0330
c7e1 a2 2c    LDX #&2c
c7e3 a0 28    LDY #&28
c7e5 20 a6 d6 JSR &d6a6
c7e8 ad 2e 03 LDA &032e
c7eb d0 03    BNE &c7f0
c7ed ce 2f 03 DEC &032f
c7f0 ce 2e 03 DEC &032e
c7f3 ce 30 03 DEC &0330
c7f6 d0 e9    BNE &c7e1
c7f8 60       RTS     
c7f9 a0 00    LDY #&00
c7fb f0 02    BEQ &c7ff
c7fd a0 02    LDY #&02
c7ff ad 23 03 LDA &0323
c802 10 01    BPL &c805
c804 c8       INY     
c805 2d 60 03 AND &0360
c808 85 da    STA &da
c80a ad 60 03 LDA &0360
c80d f0 1c    BEQ &c82b
c80f 29 07    AND #&07
c811 18       CLC     
c812 65 da    ADC &da
c814 aa       TAX     
c815 bd 23 c4 LDA &c423,X
c818 99 57 03 STA &0357,Y
c81b c0 02    CPY #&02
c81d b0 0d    BCS &c82c
c81f ad 57 03 LDA &0357
c822 49 ff    EOR #&ff
c824 85 d3    STA &d3
c826 4d 58 03 EOR &0358
c829 85 d2    STA &d2
c82b 60       RTS     
c82c ad 22 03 LDA &0322
c82f 99 59 03 STA &0359,Y
c832 60       RTS     
c833 a9 20    LDA #&20
c835 8d 58 03 STA &0358
c838 60       RTS     
c839 a2 05    LDX #&05
c83b a9 00    LDA #&00
c83d 9d 57 03 STA &0357,X
c840 ca       DEX     
c841 10 fa    BPL &c83d
c843 ae 60 03 LDX &0360
c846 f0 eb    BEQ &c833
c848 a9 ff    LDA #&ff
c84a e0 0f    CPX #&0f
c84c d0 02    BNE &c850
c84e a9 3f    LDA #&3f
c850 8d 57 03 STA &0357
c853 8d 59 03 STA &0359
c856 49 ff    EOR #&ff
c858 85 d2    STA &d2
c85a 85 d3    STA &d3
c85c 8e 1f 03 STX &031f
c85f e0 03    CPX #&03
c861 f0 11    BEQ &c874
c863 90 20    BCC &c885
c865 8e 20 03 STX &0320
c868 20 92 c8 JSR &c892
c86b ce 20 03 DEC &0320
c86e ce 1f 03 DEC &031f
c871 10 f5    BPL &c868
c873 60       RTS     
c874 a2 07    LDX #&07
c876 8e 20 03 STX &0320
c879 20 92 c8 JSR &c892
c87c 4e 20 03 LSR &0320
c87f ce 1f 03 DEC &031f
c882 10 f5    BPL &c879
c884 60       RTS     
c885 a2 07    LDX #&07
c887 20 8f c8 JSR &c88f
c88a a2 00    LDX #&00
c88c 8e 1f 03 STX &031f
c88f 8e 20 03 STX &0320
c892 08       PHP     
c893 78       SEI     
c894 ad 1f 03 LDA &031f
c897 2d 60 03 AND &0360
c89a aa       TAX     
c89b ad 20 03 LDA &0320
c89e 29 0f    AND #&0f
c8a0 9d 6f 03 STA &036f,X
c8a3 a8       TAY     
c8a4 ad 60 03 LDA &0360
c8a7 85 fa    STA &fa
c8a9 c9 03    CMP #&03
c8ab 08       PHP     
c8ac 8a       TXA     
c8ad 6a       ROR A
c8ae 66 fa    ROR &fa
c8b0 b0 fb    BCS &c8ad
c8b2 06 fa    ASL &fa
c8b4 98       TYA     
c8b5 05 fa    ORA &fa
c8b7 aa       TAX     
c8b8 a0 00    LDY #&00
c8ba 28       PLP     
c8bb 08       PHP     
c8bc d0 0e    BNE &c8cc
c8be 29 60    AND #&60
c8c0 f0 09    BEQ &c8cb
c8c2 c9 60    CMP #&60
c8c4 f0 05    BEQ &c8cb
c8c6 8a       TXA     
c8c7 49 60    EOR #&60
c8c9 d0 01    BNE &c8cc
c8cb 8a       TXA     
c8cc 20 11 ea JSR &ea11
c8cf 98       TYA     
c8d0 38       SEC     
c8d1 6d 60 03 ADC &0360
c8d4 a8       TAY     
c8d5 8a       TXA     
c8d6 69 10    ADC #&10
c8d8 aa       TAX     
c8d9 c0 10    CPY #&10
c8db 90 dd    BCC &c8ba
c8dd 28       PLP     
c8de 28       PLP     
c8df 60       RTS     
c8e0 08       PHP     
c8e1 2d 60 03 AND &0360
c8e4 aa       TAX     
c8e5 c8       INY     
c8e6 b1 f0    LDA (&f0),Y
c8e8 4c 9e c8 JMP &c89e
c8eb ad 23 03 LDA &0323
c8ee 4c 33 cb JMP &cb33
c8f1 ad 1b 03 LDA &031b
c8f4 c9 20    CMP #&20
c8f6 90 47    BCC &c93f
c8f8 48       PHA     
c8f9 4a       LSR A
c8fa 4a       LSR A
c8fb 4a       LSR A
c8fc 4a       LSR A
c8fd 4a       LSR A
c8fe aa       TAX     
c8ff bd 0d c4 LDA &c40d,X
c902 2c 67 03 BIT &0367
c905 d0 20    BNE &c927
c907 0d 67 03 ORA &0367
c90a 8d 67 03 STA &0367
c90d 8a       TXA     
c90e 29 03    AND #&03
c910 18       CLC     
c911 69 bf    ADC #&bf
c913 85 df    STA &df
c915 bd 67 03 LDA &0367,X
c918 85 dd    STA &dd
c91a a0 00    LDY #&00
c91c 84 dc    STY &dc
c91e 84 de    STY &de
c920 b1 de    LDA (&de),Y
c922 91 dc    STA (&dc),Y
c924 88       DEY     
c925 d0 f9    BNE &c920
c927 68       PLA     
c928 20 3e d0 JSR &d03e
c92b a0 07    LDY #&07
c92d b9 1c 03 LDA &031c,Y
c930 91 de    STA (&de),Y
c932 88       DEY     
c933 10 f8    BPL &c92d
c935 60       RTS     
c936 68       PLA     
c937 60       RTS     
c938 ad 1f 03 LDA &031f
c93b 18       CLC     
c93c 6c 26 02 JMP (&0226)
c93f c9 01    CMP #&01
c941 90 15    BCC &c958
c943 d0 f7    BNE &c93c
c945 20 88 c5 JSR &c588
c948 d0 ed    BNE &c937
c94a a9 20    LDA #&20
c94c ac 1c 03 LDY &031c
c94f f0 03    BEQ &c954
c951 ad 5f 03 LDA &035f
c954 a0 0a    LDY #&0a
c956 d0 2d    BNE &c985
c958 ad 1d 03 LDA &031d
c95b ac 1c 03 LDY &031c
c95e c0 07    CPY #&07
c960 90 23    BCC &c985
c962 d0 03    BNE &c967
c964 6d 90 02 ADC &0290
c967 c0 08    CPY #&08
c969 d0 07    BNE &c972
c96b 09 00    ORA #&00
c96d 30 03    BMI &c972
c96f 4d 91 02 EOR &0291
c972 c0 0a    CPY #&0a
c974 d0 0f    BNE &c985
c976 8d 5f 03 STA &035f
c979 a8       TAY     
c97a a5 d0    LDA &d0
c97c 29 20    AND #&20
c97e 08       PHP     
c97f 98       TYA     
c980 a0 0a    LDY #&0a
c982 28       PLP     
c983 d0 06    BNE &c98b
c985 8c 00 fe STY &fe00
c988 8d 01 fe STA &fe01
c98b 60       RTS     
c98c ae 61 03 LDX &0361
c98f f0 a7    BEQ &c938
c991 4c 60 d0 JMP &d060
c994 ae 50 03 LDX &0350
c997 ad 51 03 LDA &0351
c99a 20 f8 cc JSR &ccf8
c99d b0 14    BCS &c9b3
c99f 6d 54 03 ADC &0354
c9a2 90 0f    BCC &c9b3
c9a4 ae 50 03 LDX &0350
c9a7 ad 51 03 LDA &0351
c9aa 20 d4 ca JSR &cad4
c9ad 10 04    BPL &c9b3
c9af 38       SEC     
c9b0 ed 54 03 SBC &0354
c9b3 8d 51 03 STA &0351
c9b6 8e 50 03 STX &0350
c9b9 a0 0c    LDY #&0c
c9bb d0 51    BNE &ca0e
c9bd a9 00    LDA #&00
c9bf a2 2c    LDX #&2c
c9c1 9d 00 03 STA &0300,X
c9c4 ca       DEX     
c9c5 10 fa    BPL &c9c1
c9c7 ae 55 03 LDX &0355
c9ca bc ef c3 LDY &c3ef,X
c9cd 8c 0a 03 STY &030a
c9d0 20 88 ca JSR &ca88
c9d3 bc e7 c3 LDY &c3e7,X
c9d6 8c 09 03 STY &0309
c9d9 a0 03    LDY #&03
c9db 8c 23 03 STY &0323
c9de c8       INY     
c9df 8c 21 03 STY &0321
c9e2 ce 22 03 DEC &0322
c9e5 ce 20 03 DEC &0320
c9e8 20 39 ca JSR &ca39
c9eb a9 f7    LDA #&f7
c9ed 20 a8 c5 JSR &c5a8
c9f0 ae 50 03 LDX &0350
c9f3 ad 51 03 LDA &0351
c9f6 8e 4a 03 STX &034a
c9f9 8d 4b 03 STA &034b
c9fc 10 04    BPL &ca02
c9fe 38       SEC     
c9ff ed 54 03 SBC &0354
ca02 86 d8    STX &d8
ca04 85 d9    STA &d9
ca06 ae 4a 03 LDX &034a
ca09 ad 4b 03 LDA &034b
ca0c a0 0e    LDY #&0e
ca0e 48       PHA     
ca0f ad 55 03 LDA &0355
ca12 c9 07    CMP #&07
ca14 68       PLA     
ca15 b0 10    BCS &ca27
ca17 86 da    STX &da
ca19 4a       LSR A
ca1a 66 da    ROR &da
ca1c 4a       LSR A
ca1d 66 da    ROR &da
ca1f 4a       LSR A
ca20 66 da    ROR &da
ca22 a6 da    LDX &da
ca24 4c 2b ca JMP &ca2b
ca27 e9 74    SBC #&74
ca29 49 20    EOR #&20
ca2b 8c 00 fe STY &fe00
ca2e 8d 01 fe STA &fe01
ca31 c8       INY     
ca32 8c 00 fe STY &fe00
ca35 8e 01 fe STX &fe01
ca38 60       RTS     
ca39 20 81 ca JSR &ca81
ca3c a2 1c    LDX #&1c
ca3e a0 2c    LDY #&2c
ca40 20 11 d4 JSR &d411
ca43 0d 2d 03 ORA &032d
ca46 30 39    BMI &ca81
ca48 a2 20    LDX #&20
ca4a 20 49 d1 JSR &d149
ca4d a2 1c    LDX #&1c
ca4f 20 49 d1 JSR &d149
ca52 ad 1f 03 LDA &031f
ca55 0d 1d 03 ORA &031d
ca58 30 27    BMI &ca81
ca5a ad 23 03 LDA &0323
ca5d d0 22    BNE &ca81
ca5f ae 55 03 LDX &0355
ca62 ad 21 03 LDA &0321
ca65 85 da    STA &da
ca67 ad 20 03 LDA &0320
ca6a 46 da    LSR &da
ca6c 6a       ROR A
ca6d 46 da    LSR &da
ca6f d0 10    BNE &ca81
ca71 6a       ROR A
ca72 4a       LSR A
ca73 dd ef c3 CMP &c3ef,X
ca76 f0 02    BEQ &ca7a
ca78 10 07    BPL &ca81
ca7a a0 00    LDY #&00
ca7c a2 1c    LDX #&1c
ca7e 20 7c d4 JSR &d47c
ca81 a2 10    LDX #&10
ca83 a0 28    LDY #&28
ca85 4c e6 cd JMP &cde6
ca88 c8       INY     
ca89 98       TYA     
ca8a a0 00    LDY #&00
ca8c 8c 4d 03 STY &034d
ca8f 8d 4c 03 STA &034c
ca92 ad 4f 03 LDA &034f
ca95 4a       LSR A
ca96 f0 09    BEQ &caa1
ca98 0e 4c 03 ASL &034c
ca9b 2e 4d 03 ROL &034d
ca9e 4a       LSR A
ca9f 90 f7    BCC &ca98
caa1 60       RTS     
caa2 a2 20    LDX #&20
caa4 a0 0c    LDY #&0c
caa6 20 8a d4 JSR &d48a
caa9 4c b8 d1 JMP &d1b8
caac 20 c5 c5 JSR &c5c5
caaf 20 88 c5 JSR &c588
cab2 d0 13    BNE &cac7
cab4 ae 60 03 LDX &0360
cab7 f0 09    BEQ &cac2
cab9 85 de    STA &de
cabb a9 c0    LDA #&c0
cabd 85 df    STA &df
cabf 4c bf cf JMP &cfbf
cac2 a9 20    LDA #&20
cac4 4c dc cf JMP &cfdc
cac7 a9 7f    LDA #&7f
cac9 20 3e d0 JSR &d03e
cacc ae 5a 03 LDX &035a
cacf a0 00    LDY #&00
cad1 4c 63 cf JMP &cf63
cad4 48       PHA     
cad5 8a       TXA     
cad6 18       CLC     
cad7 6d 52 03 ADC &0352
cada aa       TAX     
cadb 68       PLA     
cadc 6d 53 03 ADC &0353
cadf 60       RTS     
cae0 20 14 cb JSR &cb14
cae3 20 d9 e9 JSR &e9d9
cae6 90 02    BCC &caea
cae8 30 f6    BMI &cae0
caea a5 d0    LDA &d0
caec 49 04    EOR #&04
caee 29 46    AND #&46
caf0 d0 2a    BNE &cb1c
caf2 ad 69 02 LDA &0269
caf5 30 22    BMI &cb19
caf7 ad 19 03 LDA &0319
cafa cd 09 03 CMP &0309
cafd 90 1a    BCC &cb19
caff 4a       LSR A
cb00 4a       LSR A
cb01 38       SEC     
cb02 6d 69 02 ADC &0269
cb05 6d 0b 03 ADC &030b
cb08 cd 09 03 CMP &0309
cb0b 90 0c    BCC &cb19
cb0d 18       CLC     
cb0e 20 d9 e9 JSR &e9d9
cb11 38       SEC     
cb12 10 fa    BPL &cb0e
cb14 a9 ff    LDA #&ff
cb16 8d 69 02 STA &0269
cb19 ee 69 02 INC &0269
cb1c 60       RTS     
cb1d 48       PHA     
cb1e a2 7f    LDX #&7f
cb20 a9 00    LDA #&00
cb22 85 d0    STA &d0
cb24 9d ff 02 STA &02ff,X
cb27 ca       DEX     
cb28 d0 fa    BNE &cb24
cb2a 20 07 cd JSR &cd07
cb2d 68       PLA     
cb2e a2 7f    LDX #&7f
cb30 8e 66 03 STX &0366

.setMODE
cb33 2c 8e 02 BIT &028e
cb36 30 02    BMI &cb3a
cb38 09 04    ORA #&04
cb3a 29 07    AND #&07
cb3c aa       TAX     
cb3d 8e 55 03 STX &0355
cb40 bd 14 c4 LDA &c414,X
cb43 8d 60 03 STA &0360
cb46 bd ff c3 LDA &c3ff,X
cb49 8d 4f 03 STA &034f
cb4c bd 3a c4 LDA &c43a,X
cb4f 8d 61 03 STA &0361
cb52 d0 02    BNE &cb56
cb54 a9 07    LDA #&07
cb56 0a       ASL A
cb57 a8       TAY     
cb58 b9 06 c4 LDA &c406,Y
cb5b 8d 63 03 STA &0363
cb5e 0a       ASL A
cb5f 10 fd    BPL &cb5e
cb61 8d 62 03 STA &0362
cb64 bc 40 c4 LDY &c440,X
cb67 8c 56 03 STY &0356
cb6a b9 4f c4 LDA &c44f,Y
cb6d 20 f8 e9 JSR &e9f8
cb70 b9 4b c4 LDA &c44b,Y
cb73 20 f8 e9 JSR &e9f8
cb76 b9 59 c4 LDA &c459,Y
cb79 8d 54 03 STA &0354
cb7c b9 5e c4 LDA &c45e,Y
cb7f 8d 4e 03 STA &034e
cb82 98       TYA     
cb83 69 02    ADC #&02
cb85 49 07    EOR #&07
cb87 4a       LSR A
cb88 aa       TAX     
cb89 bd 66 c4 LDA &c466,X
cb8c 85 e0    STA &e0
cb8e a9 c3    LDA #&c3
cb90 85 e1    STA &e1
cb92 bd 63 c4 LDA &c463,X
cb95 8d 52 03 STA &0352
cb98 8e 53 03 STX &0353
cb9b a9 43    LDA #&43
cb9d 20 a8 c5 JSR &c5a8
cba0 ae 55 03 LDX &0355
cba3 bd f7 c3 LDA &c3f7,X
cba6 20 00 ea JSR &ea00
cba9 08       PHP     
cbaa 78       SEI     
cbab be 69 c4 LDX &c469,Y
cbae a0 0b    LDY #&0b
cbb0 bd 6e c4 LDA &c46e,X
cbb3 20 5e c9 JSR &c95e
cbb6 ca       DEX     
cbb7 88       DEY     
cbb8 10 f6    BPL &cbb0
cbba 28       PLP     
cbbb 20 39 c8 JSR &c839
cbbe 20 bd c9 JSR &c9bd
cbc1 a2 00    LDX #&00
cbc3 ad 4e 03 LDA &034e
cbc6 8e 50 03 STX &0350
cbc9 8d 51 03 STA &0351
cbcc 20 f6 c9 JSR &c9f6
cbcf a0 0c    LDY #&0c
cbd1 20 2b ca JSR &ca2b
cbd4 ad 58 03 LDA &0358
cbd7 ae 56 03 LDX &0356
cbda bc 54 c4 LDY &c454,X
cbdd 8c 5d 03 STY &035d
cbe0 a0 cc    LDY #&cc
cbe2 8c 5e 03 STY &035e
cbe5 a2 00    LDX #&00
cbe7 8e 69 02 STX &0269
cbea 8e 18 03 STX &0318
cbed 8e 19 03 STX &0319
cbf0 6c 5d 03 JMP (&035d)
cbf3 20 3e d0 JSR &d03e
cbf6 a0 00    LDY #&00
cbf8 b1 de    LDA (&de),Y
cbfa c8       INY     
cbfb 91 f0    STA (&f0),Y
cbfd c0 08    CPY #&08
cbff d0 f7    BNE &cbf8
cc01 60       RTS     
cc02 9d 00 30 STA &3000,X
cc05 9d 00 31 STA &3100,X
cc08 9d 00 32 STA &3200,X
cc0b 9d 00 33 STA &3300,X
cc0e 9d 00 34 STA &3400,X
cc11 9d 00 35 STA &3500,X
cc14 9d 00 36 STA &3600,X
cc17 9d 00 37 STA &3700,X
cc1a 9d 00 38 STA &3800,X
cc1d 9d 00 39 STA &3900,X
cc20 9d 00 3a STA &3a00,X
cc23 9d 00 3b STA &3b00,X
cc26 9d 00 3c STA &3c00,X
cc29 9d 00 3d STA &3d00,X
cc2c 9d 00 3e STA &3e00,X
cc2f 9d 00 3f STA &3f00,X
cc32 9d 00 40 STA &4000,X
cc35 9d 00 41 STA &4100,X
cc38 9d 00 42 STA &4200,X
cc3b 9d 00 43 STA &4300,X
cc3e 9d 00 44 STA &4400,X
cc41 9d 00 45 STA &4500,X
cc44 9d 00 46 STA &4600,X
cc47 9d 00 47 STA &4700,X
cc4a 9d 00 48 STA &4800,X
cc4d 9d 00 49 STA &4900,X
cc50 9d 00 4a STA &4a00,X
cc53 9d 00 4b STA &4b00,X
cc56 9d 00 4c STA &4c00,X
cc59 9d 00 4d STA &4d00,X
cc5c 9d 00 4e STA &4e00,X
cc5f 9d 00 4f STA &4f00,X
cc62 9d 00 50 STA &5000,X
cc65 9d 00 51 STA &5100,X
cc68 9d 00 52 STA &5200,X
cc6b 9d 00 53 STA &5300,X
cc6e 9d 00 54 STA &5400,X
cc71 9d 00 55 STA &5500,X
cc74 9d 00 56 STA &5600,X
cc77 9d 00 57 STA &5700,X
cc7a 9d 00 58 STA &5800,X
cc7d 9d 00 59 STA &5900,X
cc80 9d 00 5a STA &5a00,X
cc83 9d 00 5b STA &5b00,X
cc86 9d 00 5c STA &5c00,X
cc89 9d 00 5d STA &5d00,X
cc8c 9d 00 5e STA &5e00,X
cc8f 9d 00 5f STA &5f00,X
cc92 9d 00 60 STA &6000,X
cc95 9d 00 61 STA &6100,X
cc98 9d 00 62 STA &6200,X
cc9b 9d 00 63 STA &6300,X
cc9e 9d 00 64 STA &6400,X
cca1 9d 00 65 STA &6500,X
cca4 9d 00 66 STA &6600,X
cca7 9d 00 67 STA &6700,X
ccaa 9d 00 68 STA &6800,X
ccad 9d 00 69 STA &6900,X
ccb0 9d 00 6a STA &6a00,X
ccb3 9d 00 6b STA &6b00,X
ccb6 9d 00 6c STA &6c00,X
ccb9 9d 00 6d STA &6d00,X
ccbc 9d 00 6e STA &6e00,X
ccbf 9d 00 6f STA &6f00,X
ccc2 9d 00 70 STA &7000,X
ccc5 9d 00 71 STA &7100,X
ccc8 9d 00 72 STA &7200,X
cccb 9d 00 73 STA &7300,X
ccce 9d 00 74 STA &7400,X
ccd1 9d 00 75 STA &7500,X
ccd4 9d 00 76 STA &7600,X
ccd7 9d 00 77 STA &7700,X
ccda 9d 00 78 STA &7800,X
ccdd 9d 00 79 STA &7900,X
cce0 9d 00 7a STA &7a00,X
cce3 9d 00 7b STA &7b00,X
cce6 9d 00 7c STA &7c00,X
cce9 9d 00 7d STA &7d00,X
ccec 9d 00 7e STA &7e00,X
ccef 9d 00 7f STA &7f00,X
ccf2 e8       INX     
ccf3 f0 70    BEQ &cd65
ccf5 6c 5d 03 JMP (&035d)
ccf8 48       PHA     
ccf9 8a       TXA     
ccfa 38       SEC     
ccfb ed 52 03 SBC &0352
ccfe aa       TAX     
ccff 68       PLA     
cd00 ed 53 03 SBC &0353
cd03 cd 4e 03 CMP &034e
cd06 60       RTS     
cd07 a9 0f    LDA #&0f
cd09 8d 67 03 STA &0367
cd0c a9 0c    LDA #&0c
cd0e a0 06    LDY #&06
cd10 99 68 03 STA &0368,Y
cd13 88       DEY     
cd14 10 fa    BPL &cd10
cd16 e0 07    CPX #&07
cd18 90 02    BCC &cd1c
cd1a a2 06    LDX #&06
cd1c 8e 46 02 STX &0246
cd1f ad 43 02 LDA &0243
cd22 a2 00    LDX #&00
cd24 ec 46 02 CPX &0246
cd27 b0 0b    BCS &cd34
cd29 bc ba c4 LDY &c4ba,X
cd2c 99 68 03 STA &0368,Y
cd2f 69 01    ADC #&01
cd31 e8       INX     
cd32 d0 f0    BNE &cd24
cd34 8d 44 02 STA &0244
cd37 a8       TAY     
cd38 f0 cc    BEQ &cd06
cd3a a2 11    LDX #&11
cd3c 4c 68 f1 JMP &f168
cd3f a9 02    LDA #&02
cd41 24 d0    BIT &d0
cd43 d0 02    BNE &cd47
cd45 50 32    BVC &cd79
cd47 ad 09 03 LDA &0309
cd4a 90 03    BCC &cd4f
cd4c ad 0b 03 LDA &030b
cd4f 70 08    BVS &cd59
cd51 8d 19 03 STA &0319
cd54 68       PLA     
cd55 68       PLA     
cd56 4c af c6 JMP &c6af
cd59 08       PHP     
cd5a cd 65 03 CMP &0365
cd5d f0 19    BEQ &cd78
cd5f 28       PLP     
cd60 90 04    BCC &cd66
cd62 ce 65 03 DEC &0365
cd65 60       RTS     
cd66 ee 65 03 INC &0365
cd69 60       RTS     
cd6a 08       PHP     
cd6b 48       PHA     
cd6c ac 4f 03 LDY &034f
cd6f 88       DEY     
cd70 d0 1d    BNE &cd8f
cd72 ad 38 03 LDA &0338
cd75 91 d8    STA (&d8),Y
cd77 68       PLA     
cd78 28       PLP     
cd79 60       RTS     
cd7a 08       PHP     
cd7b 48       PHA     
cd7c ac 4f 03 LDY &034f
cd7f 88       DEY     
cd80 d0 0d    BNE &cd8f
cd82 b1 d8    LDA (&d8),Y
cd84 8d 38 03 STA &0338
cd87 ad 66 03 LDA &0366
cd8a 91 d8    STA (&d8),Y
cd8c 4c 77 cd JMP &cd77
cd8f a9 ff    LDA #&ff
cd91 c0 1f    CPY #&1f
cd93 d0 02    BNE &cd97
cd95 a9 3f    LDA #&3f
cd97 85 da    STA &da
cd99 b1 d8    LDA (&d8),Y
cd9b 45 da    EOR &da
cd9d 91 d8    STA (&d8),Y
cd9f 88       DEY     
cda0 10 f7    BPL &cd99
cda2 30 d3    BMI &cd77
cda4 20 5b ce JSR &ce5b
cda7 ad 09 03 LDA &0309
cdaa 8d 19 03 STA &0319
cdad 20 06 cf JSR &cf06
cdb0 20 f8 cc JSR &ccf8
cdb3 b0 03    BCS &cdb8
cdb5 6d 54 03 ADC &0354
cdb8 85 db    STA &db
cdba 86 da    STX &da
cdbc 85 dc    STA &dc
cdbe b0 06    BCS &cdc6
cdc0 20 73 ce JSR &ce73
cdc3 4c ce cd JMP &cdce
cdc6 20 f8 cc JSR &ccf8
cdc9 90 f5    BCC &cdc0
cdcb 20 38 ce JSR &ce38
cdce a5 dc    LDA &dc
cdd0 a6 da    LDX &da
cdd2 85 d9    STA &d9
cdd4 86 d8    STX &d8
cdd6 c6 de    DEC &de
cdd8 d0 d6    BNE &cdb0
cdda a2 28    LDX #&28
cddc a0 18    LDY #&18
cdde a9 02    LDA #&02
cde0 d0 06    BNE &cde8
cde2 a2 24    LDX #&24
cde4 a0 14    LDY #&14
cde6 a9 04    LDA #&04
cde8 85 da    STA &da
cdea bd 00 03 LDA &0300,X
cded 48       PHA     
cdee b9 00 03 LDA &0300,Y
cdf1 9d 00 03 STA &0300,X
cdf4 68       PLA     
cdf5 99 00 03 STA &0300,Y
cdf8 e8       INX     
cdf9 c8       INY     
cdfa c6 da    DEC &da
cdfc d0 ec    BNE &cdea
cdfe 60       RTS     
cdff 20 5b ce JSR &ce5b
ce02 ac 0b 03 LDY &030b
ce05 8c 19 03 STY &0319
ce08 20 06 cf JSR &cf06
ce0b 20 d4 ca JSR &cad4
ce0e 10 04    BPL &ce14
ce10 38       SEC     
ce11 ed 54 03 SBC &0354
ce14 85 db    STA &db
ce16 86 da    STX &da
ce18 85 dc    STA &dc
ce1a 90 06    BCC &ce22
ce1c 20 73 ce JSR &ce73
ce1f 4c 2a ce JMP &ce2a
ce22 20 d4 ca JSR &cad4
ce25 30 f5    BMI &ce1c
ce27 20 38 ce JSR &ce38
ce2a a5 dc    LDA &dc
ce2c a6 da    LDX &da
ce2e 85 d9    STA &d9
ce30 86 d8    STX &d8
ce32 c6 de    DEC &de
ce34 d0 d5    BNE &ce0b
ce36 f0 a2    BEQ &cdda
ce38 ae 4d 03 LDX &034d
ce3b f0 10    BEQ &ce4d
ce3d a0 00    LDY #&00
ce3f b1 da    LDA (&da),Y
ce41 91 d8    STA (&d8),Y
ce43 c8       INY     
ce44 d0 f9    BNE &ce3f
ce46 e6 d9    INC &d9
ce48 e6 db    INC &db
ce4a ca       DEX     
ce4b d0 f2    BNE &ce3f
ce4d ac 4c 03 LDY &034c
ce50 f0 08    BEQ &ce5a
ce52 88       DEY     
ce53 b1 da    LDA (&da),Y
ce55 91 d8    STA (&d8),Y
ce57 98       TYA     
ce58 d0 f8    BNE &ce52
ce5a 60       RTS     
ce5b 20 da cd JSR &cdda
ce5e 38       SEC     
ce5f ad 09 03 LDA &0309
ce62 ed 0b 03 SBC &030b
ce65 85 de    STA &de
ce67 d0 05    BNE &ce6e
ce69 68       PLA     
ce6a 68       PLA     
ce6b 4c da cd JMP &cdda
ce6e ad 08 03 LDA &0308
ce71 10 70    BPL &cee3
ce73 a5 da    LDA &da
ce75 48       PHA     
ce76 38       SEC     
ce77 ad 0a 03 LDA &030a
ce7a ed 08 03 SBC &0308
ce7d 85 df    STA &df
ce7f ac 4f 03 LDY &034f
ce82 88       DEY     
ce83 b1 da    LDA (&da),Y
ce85 91 d8    STA (&d8),Y
ce87 88       DEY     
ce88 10 f9    BPL &ce83
ce8a a2 02    LDX #&02
ce8c 18       CLC     
ce8d b5 d8    LDA &d8,X
ce8f 6d 4f 03 ADC &034f
ce92 95 d8    STA &d8,X
ce94 b5 d9    LDA &d9,X
ce96 69 00    ADC #&00
ce98 10 04    BPL &ce9e
ce9a 38       SEC     
ce9b ed 54 03 SBC &0354
ce9e 95 d9    STA &d9,X
cea0 ca       DEX     
cea1 ca       DEX     
cea2 f0 e8    BEQ &ce8c
cea4 c6 df    DEC &df
cea6 10 d7    BPL &ce7f
cea8 68       PLA     
cea9 85 da    STA &da
ceab 60       RTS     
ceac ad 18 03 LDA &0318
ceaf 48       PHA     
ceb0 20 6e ce JSR &ce6e
ceb3 20 06 cf JSR &cf06
ceb6 38       SEC     
ceb7 ad 0a 03 LDA &030a
ceba ed 08 03 SBC &0308
cebd 85 dc    STA &dc
cebf ad 58 03 LDA &0358
cec2 ac 4f 03 LDY &034f
cec5 88       DEY     
cec6 91 d8    STA (&d8),Y
cec8 d0 fb    BNE &cec5
ceca 8a       TXA     
cecb 18       CLC     
cecc 6d 4f 03 ADC &034f
cecf aa       TAX     
ced0 a5 d9    LDA &d9
ced2 69 00    ADC #&00
ced4 10 04    BPL &ceda
ced6 38       SEC     
ced7 ed 54 03 SBC &0354
ceda 86 d8    STX &d8
cedc 85 d9    STA &d9
cede c6 dc    DEC &dc
cee0 10 dd    BPL &cebf
cee2 68       PLA     
cee3 8d 18 03 STA &0318
cee6 38       SEC     
cee7 60       RTS     
cee8 ae 18 03 LDX &0318
ceeb ec 08 03 CPX &0308
ceee 30 f6    BMI &cee6
cef0 ec 0a 03 CPX &030a
cef3 f0 02    BEQ &cef7
cef5 10 ef    BPL &cee6
cef7 ae 19 03 LDX &0319
cefa ec 0b 03 CPX &030b
cefd 30 e7    BMI &cee6
ceff ec 09 03 CPX &0309
cf02 f0 02    BEQ &cf06
cf04 10 e0    BPL &cee6
cf06 ad 19 03 LDA &0319
cf09 0a       ASL A
cf0a a8       TAY     
cf0b b1 e0    LDA (&e0),Y
cf0d 85 d9    STA &d9
cf0f c8       INY     
cf10 a9 02    LDA #&02
cf12 2d 56 03 AND &0356
cf15 08       PHP     
cf16 b1 e0    LDA (&e0),Y
cf18 28       PLP     
cf19 f0 03    BEQ &cf1e
cf1b 46 d9    LSR &d9
cf1d 6a       ROR A
cf1e 6d 50 03 ADC &0350
cf21 85 d8    STA &d8
cf23 a5 d9    LDA &d9
cf25 6d 51 03 ADC &0351
cf28 a8       TAY     
cf29 ad 18 03 LDA &0318
cf2c ae 4f 03 LDX &034f
cf2f ca       DEX     
cf30 f0 12    BEQ &cf44
cf32 e0 0f    CPX #&0f
cf34 f0 03    BEQ &cf39
cf36 90 02    BCC &cf3a
cf38 0a       ASL A
cf39 0a       ASL A
cf3a 0a       ASL A
cf3b 0a       ASL A
cf3c 90 02    BCC &cf40
cf3e c8       INY     
cf3f c8       INY     
cf40 0a       ASL A
cf41 90 02    BCC &cf45
cf43 c8       INY     
cf44 18       CLC     
cf45 65 d8    ADC &d8
cf47 85 d8    STA &d8
cf49 8d 4a 03 STA &034a
cf4c aa       TAX     
cf4d 98       TYA     
cf4e 69 00    ADC #&00
cf50 8d 4b 03 STA &034b
cf53 10 04    BPL &cf59
cf55 38       SEC     
cf56 ed 54 03 SBC &0354
cf59 85 d9    STA &d9
cf5b 18       CLC     
cf5c 60       RTS     
cf5d ae 59 03 LDX &0359
cf60 ac 5b 03 LDY &035b
cf63 20 b3 d0 JSR &d0b3
cf66 20 86 d4 JSR &d486
cf69 a0 00    LDY #&00
cf6b 84 dc    STY &dc
cf6d a4 dc    LDY &dc
cf6f b1 de    LDA (&de),Y
cf71 f0 13    BEQ &cf86
cf73 85 dd    STA &dd
cf75 10 03    BPL &cf7a
cf77 20 e3 d0 JSR &d0e3
cf7a ee 24 03 INC &0324
cf7d d0 03    BNE &cf82
cf7f ee 25 03 INC &0325
cf82 06 dd    ASL &dd
cf84 d0 ef    BNE &cf75
cf86 a2 28    LDX #&28
cf88 a0 24    LDY #&24
cf8a 20 82 d4 JSR &d482
cf8d ac 26 03 LDY &0326
cf90 d0 03    BNE &cf95
cf92 ce 27 03 DEC &0327
cf95 ce 26 03 DEC &0326
cf98 a4 dc    LDY &dc
cf9a c8       INY     
cf9b c0 08    CPY #&08
cf9d d0 cc    BNE &cf6b
cf9f a2 28    LDX #&28
cfa1 a0 24    LDY #&24
cfa3 4c 8a d4 JMP &d48a
cfa6 a2 06    LDX #&06
cfa8 a0 26    LDY #&26
cfaa 20 82 d4 JSR &d482
cfad a2 00    LDX #&00
cfaf a0 24    LDY #&24
cfb1 20 82 d4 JSR &d482
cfb4 4c b8 d1 JMP &d1b8
cfb7 ae 60 03 LDX &0360
cfba f0 20    BEQ &cfdc
cfbc 20 3e d0 JSR &d03e
cfbf ae 60 03 LDX &0360
cfc2 a5 d0    LDA &d0
cfc4 29 20    AND #&20
cfc6 d0 95    BNE &cf5d
cfc8 a0 07    LDY #&07
cfca e0 03    CPX #&03
cfcc f0 20    BEQ &cfee
cfce b0 4e    BCS &d01e
cfd0 b1 de    LDA (&de),Y
cfd2 05 d2    ORA &d2
cfd4 45 d3    EOR &d3
cfd6 91 d8    STA (&d8),Y
cfd8 88       DEY     
cfd9 10 f5    BPL &cfd0
cfdb 60       RTS     
cfdc a0 02    LDY #&02
cfde d9 b6 c4 CMP &c4b6,Y
cfe1 f0 06    BEQ &cfe9
cfe3 88       DEY     
cfe4 10 f8    BPL &cfde
cfe6 81 d8    STA (&d8;X)
cfe8 60       RTS     
cfe9 b9 b7 c4 LDA &c4b7,Y
cfec d0 f8    BNE &cfe6
cfee b1 de    LDA (&de),Y
cff0 48       PHA     
cff1 4a       LSR A
cff2 4a       LSR A
cff3 4a       LSR A
cff4 4a       LSR A
cff5 aa       TAX     
cff6 bd 1f c3 LDA &c31f,X
cff9 05 d2    ORA &d2
cffb 45 d3    EOR &d3
cffd 91 d8    STA (&d8),Y
cfff 98       TYA     
d000 18       CLC     
d001 69 08    ADC #&08
d003 a8       TAY     
d004 68       PLA     
d005 29 0f    AND #&0f
d007 aa       TAX     
d008 bd 1f c3 LDA &c31f,X
d00b 05 d2    ORA &d2
d00d 45 d3    EOR &d3
d00f 91 d8    STA (&d8),Y
d011 98       TYA     
d012 e9 08    SBC #&08
d014 a8       TAY     
d015 10 d7    BPL &cfee
d017 60       RTS     
d018 98       TYA     
d019 e9 21    SBC #&21
d01b 30 fa    BMI &d017
d01d a8       TAY     
d01e b1 de    LDA (&de),Y
d020 85 dc    STA &dc
d022 38       SEC     
d023 a9 00    LDA #&00
d025 26 dc    ROL &dc
d027 f0 ef    BEQ &d018
d029 2a       ROL A
d02a 06 dc    ASL &dc
d02c 2a       ROL A
d02d aa       TAX     
d02e bd 2f c3 LDA &c32f,X
d031 05 d2    ORA &d2
d033 45 d3    EOR &d3
d035 91 d8    STA (&d8),Y
d037 18       CLC     
d038 98       TYA     
d039 69 08    ADC #&08
d03b a8       TAY     
d03c 90 e5    BCC &d023
d03e 0a       ASL A
d03f 2a       ROL A
d040 2a       ROL A
d041 85 de    STA &de
d043 29 03    AND #&03
d045 2a       ROL A
d046 aa       TAX     
d047 29 03    AND #&03
d049 69 bf    ADC #&bf
d04b a8       TAY     
d04c bd 0d c4 LDA &c40d,X
d04f 2c 67 03 BIT &0367
d052 f0 03    BEQ &d057
d054 bc 67 03 LDY &0367,X
d057 84 df    STY &df
d059 a5 de    LDA &de
d05b 29 f8    AND #&f8
d05d 85 de    STA &de
d05f 60       RTS     
d060 a2 20    LDX #&20
d062 20 4d d1 JSR &d14d
d065 ad 1f 03 LDA &031f
d068 c9 04    CMP #&04
d06a f0 6d    BEQ &d0d9
d06c a0 05    LDY #&05
d06e 29 03    AND #&03
d070 f0 0e    BEQ &d080
d072 4a       LSR A
d073 b0 03    BCS &d078
d075 88       DEY     
d076 d0 08    BNE &d080
d078 aa       TAX     
d079 bc 5b 03 LDY &035b,X
d07c bd 59 03 LDA &0359,X
d07f aa       TAX     
d080 20 b3 d0 JSR &d0b3
d083 ad 1f 03 LDA &031f
d086 30 23    BMI &d0ab
d088 0a       ASL A
d089 10 3b    BPL &d0c6
d08b 29 f0    AND #&f0
d08d 0a       ASL A
d08e f0 46    BEQ &d0d6
d090 49 40    EOR #&40
d092 f0 14    BEQ &d0a8
d094 48       PHA     
d095 20 dc d0 JSR &d0dc
d098 68       PLA     
d099 49 60    EOR #&60
d09b f0 11    BEQ &d0ae
d09d c9 40    CMP #&40
d09f d0 0a    BNE &d0ab
d0a1 a9 02    LDA #&02
d0a3 85 dc    STA &dc
d0a5 4c 06 d5 JMP &d506
d0a8 4c ea d5 JMP &d5ea
d0ab 4c 38 c9 JMP &c938
d0ae 85 dc    STA &dc
d0b0 4c bf d4 JMP &d4bf
d0b3 8a       TXA     
d0b4 19 1c c4 ORA &c41c,Y
d0b7 59 1d c4 EOR &c41d,Y
d0ba 85 d4    STA &d4
d0bc 8a       TXA     
d0bd 19 1b c4 ORA &c41b,Y
d0c0 59 20 c4 EOR &c420,Y
d0c3 85 d5    STA &d5
d0c5 60       RTS     
d0c6 0a       ASL A
d0c7 30 e2    BMI &d0ab
d0c9 0a       ASL A
d0ca 0a       ASL A
d0cb 10 03    BPL &d0d0
d0cd 20 eb d0 JSR &d0eb
d0d0 20 ed d1 JSR &d1ed
d0d3 4c d9 d0 JMP &d0d9
d0d6 20 eb d0 JSR &d0eb
d0d9 20 e2 cd JSR &cde2
d0dc a0 24    LDY #&24
d0de a2 20    LDX #&20
d0e0 4c 8a d4 JMP &d48a
d0e3 a2 24    LDX #&24
d0e5 20 5f d8 JSR &d85f
d0e8 f0 06    BEQ &d0f0
d0ea 60       RTS     
d0eb 20 5d d8 JSR &d85d
d0ee d0 13    BNE &d103
d0f0 ac 1a 03 LDY &031a
d0f3 a5 d1    LDA &d1
d0f5 25 d4    AND &d4
d0f7 11 d6    ORA (&d6),Y
d0f9 85 da    STA &da
d0fb a5 d5    LDA &d5
d0fd 25 d1    AND &d1
d0ff 45 da    EOR &da
d101 91 d6    STA (&d6),Y
d103 60       RTS     
d104 b1 d6    LDA (&d6),Y
d106 05 d4    ORA &d4
d108 45 d5    EOR &d5
d10a 91 d6    STA (&d6),Y
d10c 60       RTS     
d10d a2 24    LDX #&24
d10f a0 00    LDY #&00
d111 84 da    STY &da
d113 a0 02    LDY #&02
d115 20 28 d1 JSR &d128
d118 06 da    ASL &da
d11a 06 da    ASL &da
d11c ca       DEX     
d11d ca       DEX     
d11e a0 00    LDY #&00
d120 20 28 d1 JSR &d128
d123 e8       INX     
d124 e8       INX     
d125 a5 da    LDA &da
d127 60       RTS     
d128 bd 02 03 LDA &0302,X
d12b d9 00 03 CMP &0300,Y
d12e bd 03 03 LDA &0303,X
d131 f9 01 03 SBC &0301,Y
d134 30 10    BMI &d146
d136 b9 04 03 LDA &0304,Y
d139 dd 02 03 CMP &0302,X
d13c b9 05 03 LDA &0305,Y
d13f fd 03 03 SBC &0303,X
d142 10 04    BPL &d148
d144 e6 da    INC &da
d146 e6 da    INC &da
d148 60       RTS     
d149 a9 ff    LDA #&ff
d14b d0 03    BNE &d150
d14d ad 1f 03 LDA &031f
d150 85 da    STA &da
d152 a0 02    LDY #&02
d154 20 76 d1 JSR &d176
d157 20 ad d1 JSR &d1ad
d15a a0 00    LDY #&00
d15c ca       DEX     
d15d ca       DEX     
d15e 20 76 d1 JSR &d176
d161 ac 61 03 LDY &0361
d164 c0 03    CPY #&03
d166 f0 05    BEQ &d16d
d168 b0 06    BCS &d170
d16a 20 ad d1 JSR &d1ad
d16d 20 ad d1 JSR &d1ad
d170 ad 56 03 LDA &0356
d173 d0 38    BNE &d1ad
d175 60       RTS     
d176 18       CLC     
d177 a5 da    LDA &da
d179 29 04    AND #&04
d17b f0 09    BEQ &d186
d17d bd 02 03 LDA &0302,X
d180 48       PHA     
d181 bd 03 03 LDA &0303,X
d184 90 0e    BCC &d194
d186 bd 02 03 LDA &0302,X
d189 79 10 03 ADC &0310,Y
d18c 48       PHA     
d18d bd 03 03 LDA &0303,X
d190 79 11 03 ADC &0311,Y
d193 18       CLC     
d194 99 11 03 STA &0311,Y
d197 79 0d 03 ADC &030d,Y
d19a 9d 03 03 STA &0303,X
d19d 68       PLA     
d19e 99 10 03 STA &0310,Y
d1a1 18       CLC     
d1a2 79 0c 03 ADC &030c,Y
d1a5 9d 02 03 STA &0302,X
d1a8 90 03    BCC &d1ad
d1aa fe 03 03 INC &0303,X
d1ad bd 03 03 LDA &0303,X
d1b0 0a       ASL A
d1b1 7e 03 03 ROR &0303,X
d1b4 7e 02 03 ROR &0302,X
d1b7 60       RTS     
d1b8 a0 10    LDY #&10
d1ba 20 88 d4 JSR &d488
d1bd a2 02    LDX #&02
d1bf a0 02    LDY #&02
d1c1 20 d5 d1 JSR &d1d5
d1c4 a2 00    LDX #&00
d1c6 a0 04    LDY #&04
d1c8 ad 61 03 LDA &0361
d1cb 88       DEY     
d1cc 4a       LSR A
d1cd d0 fc    BNE &d1cb
d1cf ad 56 03 LDA &0356
d1d2 f0 01    BEQ &d1d5
d1d4 c8       INY     
d1d5 1e 10 03 ASL &0310,X
d1d8 3e 11 03 ROL &0311,X
d1db 88       DEY     
d1dc d0 f7    BNE &d1d5
d1de 38       SEC     
d1df 20 e3 d1 JSR &d1e3
d1e2 e8       INX     
d1e3 bd 10 03 LDA &0310,X
d1e6 fd 0c 03 SBC &030c,X
d1e9 9d 10 03 STA &0310,X
d1ec 60       RTS     
d1ed 20 0d d4 JSR &d40d
d1f0 ad 2b 03 LDA &032b
d1f3 4d 29 03 EOR &0329
d1f6 30 0f    BMI &d207
d1f8 ad 2a 03 LDA &032a
d1fb cd 28 03 CMP &0328
d1fe ad 2b 03 LDA &032b
d201 ed 29 03 SBC &0329
d204 4c 14 d2 JMP &d214
d207 ad 28 03 LDA &0328
d20a 18       CLC     
d20b 6d 2a 03 ADC &032a
d20e ad 29 03 LDA &0329
d211 6d 2b 03 ADC &032b
d214 6a       ROR A
d215 a2 00    LDX #&00
d217 4d 2b 03 EOR &032b
d21a 10 02    BPL &d21e
d21c a2 02    LDX #&02
d21e 86 de    STX &de
d220 bd aa c4 LDA &c4aa,X
d223 8d 5d 03 STA &035d
d226 bd ab c4 LDA &c4ab,X
d229 8d 5e 03 STA &035e
d22c bd 29 03 LDA &0329,X
d22f 10 04    BPL &d235
d231 a2 24    LDX #&24
d233 d0 02    BNE &d237
d235 a2 20    LDX #&20
d237 86 df    STX &df
d239 a0 2c    LDY #&2c
d23b 20 8a d4 JSR &d48a
d23e a5 df    LDA &df
d240 49 04    EOR #&04
d242 85 dd    STA &dd
d244 05 de    ORA &de
d246 aa       TAX     
d247 20 80 d4 JSR &d480
d24a ad 1f 03 LDA &031f
d24d 29 10    AND #&10
d24f 0a       ASL A
d250 0a       ASL A
d251 0a       ASL A
d252 85 db    STA &db
d254 a2 2c    LDX #&2c
d256 20 0f d1 JSR &d10f
d259 85 dc    STA &dc
d25b f0 06    BEQ &d263
d25d a9 40    LDA #&40
d25f 05 db    ORA &db
d261 85 db    STA &db
d263 a6 dd    LDX &dd
d265 20 0f d1 JSR &d10f
d268 24 dc    BIT &dc
d26a f0 01    BEQ &d26d
d26c 60       RTS     
d26d a6 de    LDX &de
d26f f0 02    BEQ &d273
d271 4a       LSR A
d272 4a       LSR A
d273 29 02    AND #&02
d275 f0 07    BEQ &d27e
d277 8a       TXA     
d278 09 04    ORA #&04
d27a aa       TAX     
d27b 20 80 d4 JSR &d480
d27e 20 2c d4 JSR &d42c
d281 a5 de    LDA &de
d283 49 02    EOR #&02
d285 aa       TAX     
d286 a8       TAY     
d287 ad 29 03 LDA &0329
d28a 4d 2b 03 EOR &032b
d28d 10 01    BPL &d290
d28f e8       INX     
d290 bd ae c4 LDA &c4ae,X
d293 8d 32 03 STA &0332
d296 bd b2 c4 LDA &c4b2,X
d299 8d 33 03 STA &0333
d29c a9 7f    LDA #&7f
d29e 8d 34 03 STA &0334
d2a1 24 db    BIT &db
d2a3 70 29    BVS &d2ce
d2a5 bd 47 c4 LDA &c447,X
d2a8 aa       TAX     
d2a9 38       SEC     
d2aa bd 00 03 LDA &0300,X
d2ad f9 2c 03 SBC &032c,Y
d2b0 85 da    STA &da
d2b2 bd 01 03 LDA &0301,X
d2b5 f9 2d 03 SBC &032d,Y
d2b8 a4 da    LDY &da
d2ba aa       TAX     
d2bb 10 03    BPL &d2c0
d2bd 20 9b d4 JSR &d49b
d2c0 aa       TAX     
d2c1 c8       INY     
d2c2 d0 01    BNE &d2c5
d2c4 e8       INX     
d2c5 8a       TXA     
d2c6 f0 02    BEQ &d2ca
d2c8 a0 00    LDY #&00
d2ca 84 df    STY &df
d2cc f0 09    BEQ &d2d7
d2ce 8a       TXA     
d2cf 4a       LSR A
d2d0 6a       ROR A
d2d1 09 02    ORA #&02
d2d3 45 de    EOR &de
d2d5 85 de    STA &de
d2d7 a2 2c    LDX #&2c
d2d9 20 64 d8 JSR &d864
d2dc a6 dc    LDX &dc
d2de d0 02    BNE &d2e2
d2e0 c6 dd    DEC &dd
d2e2 ca       DEX     
d2e3 a5 db    LDA &db
d2e5 f0 1f    BEQ &d306
d2e7 10 10    BPL &d2f9
d2e9 2c 34 03 BIT &0334
d2ec 10 05    BPL &d2f3
d2ee ce 34 03 DEC &0334
d2f1 d0 23    BNE &d316
d2f3 ee 34 03 INC &0334
d2f6 0a       ASL A
d2f7 10 0d    BPL &d306
d2f9 86 dc    STX &dc
d2fb a2 2c    LDX #&2c
d2fd 20 5f d8 JSR &d85f
d300 a6 dc    LDX &dc
d302 09 00    ORA #&00
d304 d0 10    BNE &d316
d306 a5 d1    LDA &d1
d308 25 d4    AND &d4
d30a 11 d6    ORA (&d6),Y
d30c 85 da    STA &da
d30e a5 d5    LDA &d5
d310 25 d1    AND &d1
d312 45 da    EOR &da
d314 91 d6    STA (&d6),Y
d316 38       SEC     
d317 ad 35 03 LDA &0335
d31a ed 37 03 SBC &0337
d31d 8d 35 03 STA &0335
d320 ad 36 03 LDA &0336
d323 ed 38 03 SBC &0338
d326 b0 11    BCS &d339
d328 85 da    STA &da
d32a ad 35 03 LDA &0335
d32d 6d 39 03 ADC &0339
d330 8d 35 03 STA &0335
d333 a5 da    LDA &da
d335 6d 3a 03 ADC &033a
d338 18       CLC     
d339 8d 36 03 STA &0336
d33c 08       PHP     
d33d b0 09    BCS &d348
d33f 6c 32 03 JMP (&0332)
d342 88       DEY     
d343 10 03    BPL &d348
d345 20 d3 d3 JSR &d3d3
d348 6c 5d 03 JMP (&035d)
d34b c8       INY     
d34c c0 08    CPY #&08
d34e d0 f8    BNE &d348
d350 18       CLC     
d351 a5 d6    LDA &d6
d353 6d 52 03 ADC &0352
d356 85 d6    STA &d6
d358 a5 d7    LDA &d7
d35a 6d 53 03 ADC &0353
d35d 10 04    BPL &d363
d35f 38       SEC     
d360 ed 54 03 SBC &0354
d363 85 d7    STA &d7
d365 a0 00    LDY #&00
d367 6c 5d 03 JMP (&035d)
d36a 46 d1    LSR &d1
d36c 90 da    BCC &d348
d36e 20 ed d3 JSR &d3ed
d371 6c 5d 03 JMP (&035d)
d374 06 d1    ASL &d1
d376 90 d0    BCC &d348
d378 20 fd d3 JSR &d3fd
d37b 6c 5d 03 JMP (&035d)
d37e 88       DEY     
d37f 10 0c    BPL &d38d
d381 20 d3 d3 JSR &d3d3
d384 d0 07    BNE &d38d
d386 46 d1    LSR &d1
d388 90 03    BCC &d38d
d38a 20 ed d3 JSR &d3ed
d38d 28       PLP     
d38e e8       INX     
d38f d0 04    BNE &d395
d391 e6 dd    INC &dd
d393 f0 0a    BEQ &d39f
d395 24 db    BIT &db
d397 70 07    BVS &d3a0
d399 b0 35    BCS &d3d0
d39b c6 df    DEC &df
d39d d0 31    BNE &d3d0
d39f 60       RTS     
d3a0 a5 de    LDA &de
d3a2 86 dc    STX &dc
d3a4 29 02    AND #&02
d3a6 aa       TAX     
d3a7 b0 19    BCS &d3c2
d3a9 24 de    BIT &de
d3ab 30 0a    BMI &d3b7
d3ad fe 2c 03 INC &032c,X
d3b0 d0 10    BNE &d3c2
d3b2 fe 2d 03 INC &032d,X
d3b5 90 0b    BCC &d3c2
d3b7 bd 2c 03 LDA &032c,X
d3ba d0 03    BNE &d3bf
d3bc de 2d 03 DEC &032d,X
d3bf de 2c 03 DEC &032c,X
d3c2 8a       TXA     
d3c3 49 02    EOR #&02
d3c5 aa       TAX     
d3c6 fe 2c 03 INC &032c,X
d3c9 d0 03    BNE &d3ce
d3cb fe 2d 03 INC &032d,X
d3ce a6 dc    LDX &dc
d3d0 4c e3 d2 JMP &d2e3
d3d3 38       SEC     
d3d4 a5 d6    LDA &d6
d3d6 ed 52 03 SBC &0352
d3d9 85 d6    STA &d6
d3db a5 d7    LDA &d7
d3dd ed 53 03 SBC &0353
d3e0 cd 4e 03 CMP &034e
d3e3 b0 03    BCS &d3e8
d3e5 6d 54 03 ADC &0354
d3e8 85 d7    STA &d7
d3ea a0 07    LDY #&07
d3ec 60       RTS     
d3ed ad 62 03 LDA &0362
d3f0 85 d1    STA &d1
d3f2 a5 d6    LDA &d6
d3f4 69 07    ADC #&07
d3f6 85 d6    STA &d6
d3f8 90 02    BCC &d3fc
d3fa e6 d7    INC &d7
d3fc 60       RTS     
d3fd ad 63 03 LDA &0363
d400 85 d1    STA &d1
d402 a5 d6    LDA &d6
d404 d0 02    BNE &d408
d406 c6 d7    DEC &d7
d408 e9 08    SBC #&08
d40a 85 d6    STA &d6
d40c 60       RTS     
d40d a0 28    LDY #&28
d40f a2 20    LDX #&20
d411 20 18 d4 JSR &d418
d414 e8       INX     
d415 e8       INX     
d416 c8       INY     
d417 c8       INY     
d418 38       SEC     
d419 bd 04 03 LDA &0304,X
d41c fd 00 03 SBC &0300,X
d41f 99 00 03 STA &0300,Y
d422 bd 05 03 LDA &0305,X
d425 fd 01 03 SBC &0301,X
d428 99 01 03 STA &0301,Y
d42b 60       RTS     
d42c a5 de    LDA &de
d42e d0 07    BNE &d437
d430 a2 28    LDX #&28
d432 a0 2a    LDY #&2a
d434 20 de cd JSR &cdde
d437 a2 28    LDX #&28
d439 a0 37    LDY #&37
d43b 20 8a d4 JSR &d48a
d43e 38       SEC     
d43f a6 de    LDX &de
d441 ad 30 03 LDA &0330
d444 fd 2c 03 SBC &032c,X
d447 a8       TAY     
d448 ad 31 03 LDA &0331
d44b fd 2d 03 SBC &032d,X
d44e 30 03    BMI &d453
d450 20 9b d4 JSR &d49b
d453 85 dd    STA &dd
d455 84 dc    STY &dc
d457 a2 35    LDX #&35
d459 20 67 d4 JSR &d467
d45c 4a       LSR A
d45d 9d 01 03 STA &0301,X
d460 98       TYA     
d461 6a       ROR A
d462 9d 00 03 STA &0300,X
d465 ca       DEX     
d466 ca       DEX     
d467 bc 04 03 LDY &0304,X
d46a bd 05 03 LDA &0305,X
d46d 10 0c    BPL &d47b
d46f 20 9b d4 JSR &d49b
d472 9d 05 03 STA &0305,X
d475 48       PHA     
d476 98       TYA     
d477 9d 04 03 STA &0304,X
d47a 68       PLA     
d47b 60       RTS     
d47c a9 08    LDA #&08
d47e d0 0c    BNE &d48c
d480 a0 30    LDY #&30
d482 a9 02    LDA #&02
d484 d0 06    BNE &d48c
d486 a0 28    LDY #&28
d488 a2 24    LDX #&24
d48a a9 04    LDA #&04
d48c 85 da    STA &da
d48e bd 00 03 LDA &0300,X
d491 99 00 03 STA &0300,Y
d494 e8       INX     
d495 c8       INY     
d496 c6 da    DEC &da
d498 d0 f4    BNE &d48e
d49a 60       RTS     
d49b 48       PHA     
d49c 98       TYA     
d49d 49 ff    EOR #&ff
d49f a8       TAY     
d4a0 68       PLA     
d4a1 49 ff    EOR #&ff
d4a3 c8       INY     
d4a4 d0 03    BNE &d4a9
d4a6 18       CLC     
d4a7 69 01    ADC #&01
d4a9 60       RTS     
d4aa 20 5d d8 JSR &d85d
d4ad d0 08    BNE &d4b7
d4af b1 d6    LDA (&d6),Y
d4b1 4d 5a 03 EOR &035a
d4b4 85 da    STA &da
d4b6 60       RTS     
d4b7 68       PLA     
d4b8 68       PLA     
d4b9 ee 26 03 INC &0326
d4bc 4c 45 d5 JMP &d545
d4bf 20 aa d4 JSR &d4aa
d4c2 25 d1    AND &d1
d4c4 d0 f3    BNE &d4b9
d4c6 a2 00    LDX #&00
d4c8 20 92 d5 JSR &d592
d4cb f0 2d    BEQ &d4fa
d4cd ac 1a 03 LDY &031a
d4d0 06 d1    ASL &d1
d4d2 b0 05    BCS &d4d9
d4d4 20 74 d5 JSR &d574
d4d7 90 21    BCC &d4fa
d4d9 20 fd d3 JSR &d3fd
d4dc b1 d6    LDA (&d6),Y
d4de 4d 5a 03 EOR &035a
d4e1 85 da    STA &da
d4e3 d0 12    BNE &d4f7
d4e5 38       SEC     
d4e6 8a       TXA     
d4e7 6d 61 03 ADC &0361
d4ea 90 04    BCC &d4f0
d4ec e6 db    INC &db
d4ee 10 07    BPL &d4f7
d4f0 aa       TAX     
d4f1 20 04 d1 JSR &d104
d4f4 38       SEC     
d4f5 b0 e2    BCS &d4d9
d4f7 20 74 d5 JSR &d574
d4fa a0 00    LDY #&00
d4fc 20 ac d5 JSR &d5ac
d4ff a0 20    LDY #&20
d501 a2 24    LDX #&24
d503 20 e6 cd JSR &cde6
d506 20 aa d4 JSR &d4aa
d509 a2 04    LDX #&04
d50b 20 92 d5 JSR &d592
d50e 8a       TXA     
d50f d0 02    BNE &d513
d511 c6 db    DEC &db
d513 ca       DEX     
d514 20 4b d5 JSR &d54b
d517 90 27    BCC &d540
d519 20 ed d3 JSR &d3ed
d51c b1 d6    LDA (&d6),Y
d51e 4d 5a 03 EOR &035a
d521 85 da    STA &da
d523 a5 dc    LDA &dc
d525 d0 ed    BNE &d514
d527 a5 da    LDA &da
d529 d0 12    BNE &d53d
d52b 38       SEC     
d52c 8a       TXA     
d52d 6d 61 03 ADC &0361
d530 90 04    BCC &d536
d532 e6 db    INC &db
d534 10 07    BPL &d53d
d536 aa       TAX     
d537 20 04 d1 JSR &d104
d53a 38       SEC     
d53b b0 dc    BCS &d519
d53d 20 4b d5 JSR &d54b
d540 a0 04    LDY #&04
d542 20 ac d5 JSR &d5ac
d545 20 d9 d0 JSR &d0d9
d548 4c b8 d1 JMP &d1b8
d54b a5 d1    LDA &d1
d54d 48       PHA     
d54e 18       CLC     
d54f 90 0f    BCC &d560
d551 68       PLA     
d552 e8       INX     
d553 d0 04    BNE &d559
d555 e6 db    INC &db
d557 10 16    BPL &d56f
d559 46 d1    LSR &d1
d55b b0 12    BCS &d56f
d55d 05 d1    ORA &d1
d55f 48       PHA     
d560 a5 d1    LDA &d1
d562 24 da    BIT &da
d564 08       PHP     
d565 68       PLA     
d566 45 dc    EOR &dc
d568 48       PHA     
d569 28       PLP     
d56a f0 e5    BEQ &d551
d56c 68       PLA     
d56d 45 d1    EOR &d1
d56f 85 d1    STA &d1
d571 4c f0 d0 JMP &d0f0
d574 a9 00    LDA #&00
d576 18       CLC     
d577 90 0a    BCC &d583
d579 e8       INX     
d57a d0 04    BNE &d580
d57c e6 db    INC &db
d57e 10 ef    BPL &d56f
d580 0a       ASL A
d581 b0 0b    BCS &d58e
d583 05 d1    ORA &d1
d585 24 da    BIT &da
d587 f0 f0    BEQ &d579
d589 45 d1    EOR &d1
d58b 4a       LSR A
d58c 90 e1    BCC &d56f
d58e 6a       ROR A
d58f 38       SEC     
d590 b0 dd    BCS &d56f
d592 bd 00 03 LDA &0300,X
d595 38       SEC     
d596 ed 20 03 SBC &0320
d599 a8       TAY     
d59a bd 01 03 LDA &0301,X
d59d ed 21 03 SBC &0321
d5a0 30 03    BMI &d5a5
d5a2 20 9b d4 JSR &d49b
d5a5 85 db    STA &db
d5a7 98       TYA     
d5a8 aa       TAX     
d5a9 05 db    ORA &db
d5ab 60       RTS     
d5ac 84 da    STY &da
d5ae 8a       TXA     
d5af a8       TAY     
d5b0 a5 db    LDA &db
d5b2 30 02    BMI &d5b6
d5b4 a9 00    LDA #&00
d5b6 a6 da    LDX &da
d5b8 d0 03    BNE &d5bd
d5ba 20 9b d4 JSR &d49b
d5bd 48       PHA     
d5be 18       CLC     
d5bf 98       TYA     
d5c0 7d 00 03 ADC &0300,X
d5c3 8d 20 03 STA &0320
d5c6 68       PLA     
d5c7 7d 01 03 ADC &0301,X
d5ca 8d 21 03 STA &0321
d5cd 60       RTS     
d5ce a9 03    LDA #&03
d5d0 20 d5 d5 JSR &d5d5
d5d3 a9 07    LDA #&07
d5d5 48       PHA     
d5d6 20 e2 cd JSR &cde2
d5d9 20 b8 d1 JSR &d1b8
d5dc a2 03    LDX #&03
d5de 68       PLA     
d5df a8       TAY     
d5e0 bd 10 03 LDA &0310,X
d5e3 91 f0    STA (&f0),Y
d5e5 88       DEY     
d5e6 ca       DEX     
d5e7 10 f7    BPL &d5e0
d5e9 60       RTS     
d5ea a2 20    LDX #&20
d5ec a0 3e    LDY #&3e
d5ee 20 7c d4 JSR &d47c
d5f1 20 32 d6 JSR &d632
d5f4 a2 14    LDX #&14
d5f6 a0 24    LDY #&24
d5f8 20 36 d6 JSR &d636
d5fb 20 32 d6 JSR &d632
d5fe a2 20    LDX #&20
d600 a0 2a    LDY #&2a
d602 20 11 d4 JSR &d411
d605 ad 2b 03 LDA &032b
d608 8d 32 03 STA &0332
d60b a2 28    LDX #&28
d60d 20 59 d4 JSR &d459
d610 a0 2e    LDY #&2e
d612 20 de d0 JSR &d0de
d615 20 e2 cd JSR &cde2
d618 18       CLC     
d619 20 58 d6 JSR &d658
d61c 20 e2 cd JSR &cde2
d61f a2 20    LDX #&20
d621 20 e4 cd JSR &cde4
d624 38       SEC     
d625 20 58 d6 JSR &d658
d628 a2 3e    LDX #&3e
d62a a0 20    LDY #&20
d62c 20 7c d4 JSR &d47c
d62f 4c d9 d0 JMP &d0d9
d632 a2 20    LDX #&20
d634 a0 14    LDY #&14
d636 bd 02 03 LDA &0302,X
d639 d9 02 03 CMP &0302,Y
d63c bd 03 03 LDA &0303,X
d63f f9 03 03 SBC &0303,Y
d642 30 13    BMI &d657
d644 4c e6 cd JMP &cde6
d647 ad 18 03 LDA &0318
d64a 38       SEC     
d64b ed 08 03 SBC &0308
d64e aa       TAX     
d64f ad 19 03 LDA &0319
d652 38       SEC     
d653 ed 0b 03 SBC &030b
d656 a8       TAY     
d657 60       RTS     
d658 08       PHP     
d659 a2 20    LDX #&20
d65b a0 35    LDY #&35
d65d 20 11 d4 JSR &d411
d660 ad 36 03 LDA &0336
d663 8d 3d 03 STA &033d
d666 a2 33    LDX #&33
d668 20 59 d4 JSR &d459
d66b a0 39    LDY #&39
d66d 20 de d0 JSR &d0de
d670 38       SEC     
d671 ad 22 03 LDA &0322
d674 ed 26 03 SBC &0326
d677 8d 1b 03 STA &031b
d67a ad 23 03 LDA &0323
d67d ed 27 03 SBC &0327
d680 8d 1c 03 STA &031c
d683 0d 1b 03 ORA &031b
d686 f0 17    BEQ &d69f
d688 20 a2 d6 JSR &d6a2
d68b a2 33    LDX #&33
d68d 20 74 d7 JSR &d774
d690 a2 28    LDX #&28
d692 20 74 d7 JSR &d774
d695 ee 1b 03 INC &031b
d698 d0 ee    BNE &d688
d69a ee 1c 03 INC &031c
d69d d0 e9    BNE &d688
d69f 28       PLP     
d6a0 90 b5    BCC &d657
d6a2 a2 39    LDX #&39
d6a4 a0 2e    LDY #&2e
d6a6 86 de    STX &de
d6a8 bd 00 03 LDA &0300,X
d6ab d9 00 03 CMP &0300,Y
d6ae bd 01 03 LDA &0301,X
d6b1 f9 01 03 SBC &0301,Y
d6b4 30 06    BMI &d6bc
d6b6 98       TYA     
d6b7 a4 de    LDY &de
d6b9 aa       TAX     
d6ba 86 de    STX &de
d6bc 84 df    STY &df
d6be b9 00 03 LDA &0300,Y
d6c1 48       PHA     
d6c2 b9 01 03 LDA &0301,Y
d6c5 48       PHA     
d6c6 a6 df    LDX &df
d6c8 20 0f d1 JSR &d10f
d6cb f0 0d    BEQ &d6da
d6cd c9 02    CMP #&02
d6cf d0 3d    BNE &d70e
d6d1 a2 04    LDX #&04
d6d3 a4 df    LDY &df
d6d5 20 82 d4 JSR &d482
d6d8 a6 df    LDX &df
d6da 20 64 d8 JSR &d864
d6dd a6 de    LDX &de
d6df 20 0f d1 JSR &d10f
d6e2 4a       LSR A
d6e3 d0 29    BNE &d70e
d6e5 90 02    BCC &d6e9
d6e7 a2 00    LDX #&00
d6e9 a4 df    LDY &df
d6eb 38       SEC     
d6ec b9 00 03 LDA &0300,Y
d6ef fd 00 03 SBC &0300,X
d6f2 85 dc    STA &dc
d6f4 b9 01 03 LDA &0301,Y
d6f7 fd 01 03 SBC &0301,X
d6fa 85 dd    STA &dd
d6fc a9 00    LDA #&00
d6fe 0a       ASL A
d6ff 05 d1    ORA &d1
d701 a4 dc    LDY &dc
d703 d0 14    BNE &d719
d705 c6 dd    DEC &dd
d707 10 10    BPL &d719
d709 85 d1    STA &d1
d70b 20 f0 d0 JSR &d0f0
d70e a6 df    LDX &df
d710 68       PLA     
d711 9d 01 03 STA &0301,X
d714 68       PLA     
d715 9d 00 03 STA &0300,X
d718 60       RTS     
d719 c6 dc    DEC &dc
d71b aa       TAX     
d71c 10 e0    BPL &d6fe
d71e 85 d1    STA &d1
d720 20 f0 d0 JSR &d0f0
d723 a6 dc    LDX &dc
d725 e8       INX     
d726 d0 02    BNE &d72a
d728 e6 dd    INC &dd
d72a 8a       TXA     
d72b 48       PHA     
d72c 46 dd    LSR &dd
d72e 6a       ROR A
d72f ac 61 03 LDY &0361
d732 c0 03    CPY #&03
d734 f0 05    BEQ &d73b
d736 90 06    BCC &d73e
d738 46 dd    LSR &dd
d73a 6a       ROR A
d73b 46 dd    LSR &dd
d73d 4a       LSR A
d73e ac 1a 03 LDY &031a
d741 aa       TAX     
d742 f0 0f    BEQ &d753
d744 98       TYA     
d745 38       SEC     
d746 e9 08    SBC #&08
d748 a8       TAY     
d749 b0 02    BCS &d74d
d74b c6 d7    DEC &d7
d74d 20 04 d1 JSR &d104
d750 ca       DEX     
d751 d0 f1    BNE &d744
d753 68       PLA     
d754 2d 61 03 AND &0361
d757 f0 b5    BEQ &d70e
d759 aa       TAX     
d75a a9 00    LDA #&00
d75c 0a       ASL A
d75d 0d 63 03 ORA &0363
d760 ca       DEX     
d761 d0 f9    BNE &d75c
d763 85 d1    STA &d1
d765 98       TYA     
d766 38       SEC     
d767 e9 08    SBC #&08
d769 a8       TAY     
d76a b0 02    BCS &d76e
d76c c6 d7    DEC &d7
d76e 20 f3 d0 JSR &d0f3
d771 4c 0e d7 JMP &d70e
d774 fe 08 03 INC &0308,X
d777 d0 03    BNE &d77c
d779 fe 09 03 INC &0309,X
d77c 38       SEC     
d77d bd 00 03 LDA &0300,X
d780 fd 02 03 SBC &0302,X
d783 9d 00 03 STA &0300,X
d786 bd 01 03 LDA &0301,X
d789 fd 03 03 SBC &0303,X
d78c 9d 01 03 STA &0301,X
d78f 10 30    BPL &d7c1
d791 bd 0a 03 LDA &030a,X
d794 30 0b    BMI &d7a1
d796 fe 06 03 INC &0306,X
d799 d0 11    BNE &d7ac
d79b fe 07 03 INC &0307,X
d79e 4c ac d7 JMP &d7ac
d7a1 bd 06 03 LDA &0306,X
d7a4 d0 03    BNE &d7a9
d7a6 de 07 03 DEC &0307,X
d7a9 de 06 03 DEC &0306,X
d7ac 18       CLC     
d7ad bd 00 03 LDA &0300,X
d7b0 7d 04 03 ADC &0304,X
d7b3 9d 00 03 STA &0300,X
d7b6 bd 01 03 LDA &0301,X
d7b9 7d 05 03 ADC &0305,X
d7bc 9d 01 03 STA &0301,X
d7bf 30 d0    BMI &d791
d7c1 60       RTS     
d7c2 ac 60 03 LDY &0360
d7c5 d0 15    BNE &d7dc
d7c7 b1 d8    LDA (&d8),Y
d7c9 a0 02    LDY #&02
d7cb d9 b7 c4 CMP &c4b7,Y
d7ce d0 04    BNE &d7d4
d7d0 b9 b6 c4 LDA &c4b6,Y
d7d3 88       DEY     
d7d4 88       DEY     
d7d5 10 f4    BPL &d7cb
d7d7 ac 55 03 LDY &0355
d7da aa       TAX     
d7db 60       RTS     
d7dc 20 08 d8 JSR &d808
d7df a2 20    LDX #&20
d7e1 8a       TXA     
d7e2 48       PHA     
d7e3 20 3e d0 JSR &d03e
d7e6 68       PLA     
d7e7 aa       TAX     
d7e8 a0 07    LDY #&07
d7ea b9 28 03 LDA &0328,Y
d7ed d1 de    CMP (&de),Y
d7ef d0 08    BNE &d7f9
d7f1 88       DEY     
d7f2 10 f6    BPL &d7ea
d7f4 8a       TXA     
d7f5 e0 7f    CPX #&7f
d7f7 d0 de    BNE &d7d7
d7f9 e8       INX     
d7fa a5 de    LDA &de
d7fc 18       CLC     
d7fd 69 08    ADC #&08
d7ff 85 de    STA &de
d801 d0 e5    BNE &d7e8
d803 8a       TXA     
d804 d0 db    BNE &d7e1
d806 f0 cf    BEQ &d7d7
d808 a0 07    LDY #&07
d80a 84 da    STY &da
d80c a9 01    LDA #&01
d80e 85 db    STA &db
d810 ad 62 03 LDA &0362
d813 85 dc    STA &dc
d815 b1 d8    LDA (&d8),Y
d817 4d 58 03 EOR &0358
d81a 18       CLC     
d81b 24 dc    BIT &dc
d81d f0 01    BEQ &d820
d81f 38       SEC     
d820 26 db    ROL &db
d822 b0 0a    BCS &d82e
d824 46 dc    LSR &dc
d826 90 f3    BCC &d81b
d828 98       TYA     
d829 69 07    ADC #&07
d82b a8       TAY     
d82c 90 e2    BCC &d810
d82e a4 da    LDY &da
d830 a5 db    LDA &db
d832 99 28 03 STA &0328,Y
d835 88       DEY     
d836 10 d2    BPL &d80a
d838 60       RTS     
d839 48       PHA     
d83a aa       TAX     
d83b 20 49 d1 JSR &d149
d83e 68       PLA     
d83f aa       TAX     
d840 20 5f d8 JSR &d85f
d843 d0 15    BNE &d85a
d845 b1 d6    LDA (&d6),Y
d847 0a       ASL A
d848 26 da    ROL &da
d84a 06 d1    ASL &d1
d84c 08       PHP     
d84d b0 02    BCS &d851
d84f 46 da    LSR &da
d851 28       PLP     
d852 d0 f3    BNE &d847
d854 a5 da    LDA &da
d856 2d 60 03 AND &0360
d859 60       RTS     
d85a a9 ff    LDA #&ff
d85c 60       RTS     
d85d a2 20    LDX #&20
d85f 20 0f d1 JSR &d10f
d862 d0 f8    BNE &d85c
d864 bd 02 03 LDA &0302,X
d867 49 ff    EOR #&ff
d869 a8       TAY     
d86a 29 07    AND #&07
d86c 8d 1a 03 STA &031a
d86f 98       TYA     
d870 4a       LSR A
d871 4a       LSR A
d872 4a       LSR A
d873 0a       ASL A
d874 a8       TAY     
d875 b1 e0    LDA (&e0),Y
d877 85 da    STA &da
d879 c8       INY     
d87a b1 e0    LDA (&e0),Y
d87c ac 56 03 LDY &0356
d87f f0 03    BEQ &d884
d881 46 da    LSR &da
d883 6a       ROR A
d884 6d 50 03 ADC &0350
d887 85 d6    STA &d6
d889 a5 da    LDA &da
d88b 6d 51 03 ADC &0351
d88e 85 d7    STA &d7
d890 bd 01 03 LDA &0301,X
d893 85 da    STA &da
d895 bd 00 03 LDA &0300,X
d898 48       PHA     
d899 2d 61 03 AND &0361
d89c 6d 61 03 ADC &0361
d89f a8       TAY     
d8a0 b9 06 c4 LDA &c406,Y
d8a3 85 d1    STA &d1
d8a5 68       PLA     
d8a6 ac 61 03 LDY &0361
d8a9 c0 03    CPY #&03
d8ab f0 05    BEQ &d8b2
d8ad b0 06    BCS &d8b5
d8af 0a       ASL A
d8b0 26 da    ROL &da
d8b2 0a       ASL A
d8b3 26 da    ROL &da
d8b5 29 f8    AND #&f8
d8b7 18       CLC     
d8b8 65 d6    ADC &d6
d8ba 85 d6    STA &d6
d8bc a5 da    LDA &da
d8be 65 d7    ADC &d7
d8c0 10 04    BPL &d8c6
d8c2 38       SEC     
d8c3 ed 54 03 SBC &0354
d8c6 85 d7    STA &d7
d8c8 ac 1a 03 LDY &031a
d8cb a9 00    LDA #&00
d8cd 60       RTS     
d8ce 48       PHA     
d8cf a9 a0    LDA #&a0
d8d1 ae 6a 02 LDX &026a
d8d4 d0 40    BNE &d916
d8d6 24 d0    BIT &d0
d8d8 d0 3c    BNE &d916
d8da 70 19    BVS &d8f5
d8dc ad 5f 03 LDA &035f
d8df 29 9f    AND #&9f
d8e1 09 40    ORA #&40
d8e3 20 54 c9 JSR &c954
d8e6 a2 18    LDX #&18
d8e8 a0 64    LDY #&64
d8ea 20 82 d4 JSR &d482
d8ed 20 7a cd JSR &cd7a
d8f0 a9 02    LDA #&02
d8f2 20 9d c5 JSR &c59d
d8f5 a9 bf    LDA #&bf
d8f7 20 a8 c5 JSR &c5a8
d8fa 68       PLA     
d8fb 29 7f    AND #&7f
d8fd 20 c0 c4 JSR &c4c0
d900 a9 40    LDA #&40
d902 4c 9d c5 JMP &c59d
d905 a9 20    LDA #&20
d907 24 d0    BIT &d0
d909 50 c0    BVC &d8cb
d90b d0 be    BNE &d8cb
d90d 20 c2 d7 JSR &d7c2
d910 f0 05    BEQ &d917
d912 48       PHA     
d913 20 64 c6 JSR &c664
d916 68       PLA     
d917 60       RTS     
d918 a9 bd    LDA #&bd
d91a 20 a8 c5 JSR &c5a8
d91d 20 51 c9 JSR &c951
d920 a9 0d    LDA #&0d
d922 60       RTS     
d923 ae 55 03 LDX &0355
d926 8a       TXA     
d927 29 07    AND #&07
d929 a8       TAY     
d92a be 40 c4 LDX &c440,Y
d92d bd 5e c4 LDA &c45e,X
d930 a2 00    LDX #&00
d932 2c 8e 02 BIT &028e
d935 30 07    BMI &d93e
d937 29 3f    AND #&3f
d939 c0 04    CPY #&04
d93b b0 01    BCS &d93e
d93d 8a       TXA     
d93e a8       TAY     
d93f 60       RTS     
d940 10 e3    BPL &d925
d942 54       ???
d943 dc       ???
d944 93       ???
d945 dc       ???
d946 89       ???
d947 de 89 df DEC &df89,X
d94a 72       ???
d94b e7       ???
d94c eb       ???
d94d e7       ???
d94e a4 e0    LDY &e0
d950 c5 de    CMP &de
d952 7d f2 8e ADC &8ef2,X
d955 f1 c9    SBC (&c9),Y
d957 f4       ???
d958 29 f5    AND #&f5
d95a a6 ff    LDX &ff
d95c ca       DEX     
d95d f3       ???
d95e b1 f1    LDA (&f1),Y
d960 a6 ff    LDX &ff
d962 a6 ff    LDX &ff
d964 a6 ff    LDX &ff
d966 a6 ff    LDX &ff
d968 02       ???
d969 ef       ???
d96a b3       ???
d96b e4 64    CPX &64
d96d e4 d1    CPX &d1
d96f e1 a6    SBC (&a6;X)
d971 ff       ???
d972 a6 ff    LDX &ff
d974 a6 ff    LDX &ff
d976 90 01    BCC &d979
d978 9f       ???
d979 0d a1 02 ORA &02a1
d97c 2b       ???
d97d f0 00    BEQ &d97f
d97f 03       ???
d980 00       BRK     
d981 00       BRK     
d982 ff       ???
d983 00       BRK     
d984 00       BRK     
d985 01 00    ORA (&00;X)
d987 00       BRK     
d988 00       BRK     
d989 00       BRK     
d98a 00       BRK     
d98b ff       ???
d98c 04       ???
d98d 04       ???
d98e 00       BRK     
d98f ff       ???
d990 56 19    LSR &19,X
d992 19 19 32 ORA &3219,Y
d995 08       PHP     
d996 00       BRK     
d997 00       BRK     
d998 00       BRK     
d999 00       BRK     
d99a 20 09 00 JSR &0009
d99d 00       BRK     
d99e 00       BRK     
d99f 00       BRK     
d9a0 00       BRK     
d9a1 50 00    BVC &d9a3
d9a3 03       ???
d9a4 90 64    BCC &da0a
d9a6 06 81    ASL &81
d9a8 00       BRK     
d9a9 00       BRK     
d9aa 00       BRK     
d9ab 09 1b    ORA #&1b
d9ad 01 d0    ORA (&d0;X)
d9af e0 f0    CPX #&f0
d9b1 01 80    ORA (&80;X)
d9b3 90 00    BCC &d9b5
d9b5 00       BRK     
d9b6 00       BRK     
d9b7 ff       ???
d9b8 ff       ???
d9b9 ff       ???
d9ba 00       BRK     
d9bb 00       BRK     
d9bc 00       BRK     
d9bd 00       BRK     
d9be 00       BRK     
d9bf 00       BRK     
d9c0 00       BRK     
d9c1 00       BRK     
d9c2 64       ???
d9c3 05 ff    ORA &ff
d9c5 01 0a    ORA (&0a;X)
d9c7 00       BRK     
d9c8 00       BRK     
d9c9 00       BRK     
d9ca 00       BRK     
d9cb 00       BRK     
d9cc ff       ???
d9cd a9 40    LDA #&40
d9cf 8d 00 0d STA &0d00
d9d2 78       SEI     
d9d3 d8       CLD     
d9d4 a2 ff    LDX #&ff
d9d6 9a       TXS     
d9d7 ad 4e fe LDA &fe4e
d9da 0a       ASL A
d9db 48       PHA     
d9dc f0 09    BEQ &d9e7
d9de ad 58 02 LDA &0258
d9e1 4a       LSR A
d9e2 c9 01    CMP #&01
d9e4 d0 1d    BNE &da03
d9e6 4a       LSR A
d9e7 a2 04    LDX #&04
d9e9 86 01    STX &01
d9eb 85 00    STA &00
d9ed a8       TAY     
d9ee 91 00    STA (&00),Y
d9f0 c5 01    CMP &01
d9f2 f0 09    BEQ &d9fd
d9f4 c8       INY     
d9f5 d0 f7    BNE &d9ee
d9f7 c8       INY     
d9f8 e8       INX     
d9f9 e6 01    INC &01
d9fb 10 f1    BPL &d9ee
d9fd 8e 8e 02 STX &028e
da00 8e 84 02 STX &0284
da03 a2 0f    LDX #&0f
da05 8e 42 fe STX &fe42
da08 ca       DEX     
da09 8e 40 fe STX &fe40
da0c e0 09    CPX #&09
da0e b0 f8    BCS &da08
da10 e8       INX     
da11 8a       TXA     
da12 20 2a f0 JSR &f02a
da15 e0 80    CPX #&80
da17 66 fc    ROR &fc
da19 aa       TAX     
da1a ca       DEX     
da1b d0 f4    BNE &da11
da1d 8e 8d 02 STX &028d
da20 26 fc    ROL &fc
da22 20 eb ee JSR &eeeb
da25 6a       ROR A
da26 a2 9c    LDX #&9c
da28 a0 8d    LDY #&8d
da2a 68       PLA     
da2b f0 09    BEQ &da36
da2d a0 7e    LDY #&7e
da2f 90 11    BCC &da42
da31 a0 87    LDY #&87
da33 ee 8d 02 INC &028d
da36 ee 8d 02 INC &028d
da39 a5 fc    LDA &fc
da3b 49 ff    EOR #&ff
da3d 8d 8f 02 STA &028f
da40 a2 90    LDX #&90
da42 a9 00    LDA #&00
da44 e0 ce    CPX #&ce
da46 90 02    BCC &da4a
da48 a9 ff    LDA #&ff
da4a 9d 00 02 STA &0200,X
da4d e8       INX     
da4e d0 f4    BNE &da44
da50 8d 63 fe STA &fe63
da53 8a       TXA     
da54 a2 e2    LDX #&e2
da56 95 00    STA &00,X
da58 e8       INX     
da59 d0 fb    BNE &da56
da5b b9 3f d9 LDA &d93f,Y
da5e 99 ff 01 STA &01ff,Y
da61 88       DEY     
da62 d0 f7    BNE &da5b
da64 a9 62    LDA #&62
da66 85 ed    STA &ed
da68 20 0a fb JSR &fb0a
da6b a9 7f    LDA #&7f
da6d e8       INX     
da6e 9d 4d fe STA &fe4d,X
da71 9d 6d fe STA &fe6d,X
da74 ca       DEX     
da75 10 f7    BPL &da6e
da77 58       CLI     
da78 78       SEI     
da79 24 fc    BIT &fc
da7b 50 03    BVC &da80
da7d 20 55 f0 JSR &f055
da80 a2 f2    LDX #&f2
da82 8e 4e fe STX &fe4e
da85 a2 04    LDX #&04
da87 8e 4c fe STX &fe4c
da8a a9 60    LDA #&60
da8c 8d 4b fe STA &fe4b
da8f a9 0e    LDA #&0e
da91 8d 46 fe STA &fe46
da94 8d 6c fe STA &fe6c
da97 8d c0 fe STA &fec0
da9a cd 6c fe CMP &fe6c
da9d f0 03    BEQ &daa2
da9f ee 77 02 INC &0277
daa2 a9 27    LDA #&27
daa4 8d 47 fe STA &fe47
daa7 8d 45 fe STA &fe45
daaa 20 60 ec JSR &ec60
daad ad 82 02 LDA &0282
dab0 29 7f    AND #&7f
dab2 20 a7 e6 JSR &e6a7
dab5 ae 84 02 LDX &0284
dab8 f0 03    BEQ &dabd
daba 20 c8 e9 JSR &e9c8
dabd 20 16 dc JSR &dc16
dac0 a2 03    LDX #&03
dac2 ac 07 80 LDY &8007
dac5 b9 00 80 LDA &8000,Y
dac8 dd 0c df CMP &df0c,X
dacb d0 2e    BNE &dafb
dacd c8       INY     
dace ca       DEX     
dacf 10 f4    BPL &dac5
dad1 a6 f4    LDX &f4
dad3 a4 f4    LDY &f4
dad5 c8       INY     
dad6 c0 10    CPY #&10
dad8 b0 25    BCS &daff
dada 98       TYA     
dadb 49 ff    EOR #&ff
dadd 85 fa    STA &fa
dadf a9 7f    LDA #&7f
dae1 85 fb    STA &fb
dae3 8c 30 fe STY &fe30
dae6 b1 fa    LDA (&fa),Y
dae8 8e 30 fe STX &fe30
daeb d1 fa    CMP (&fa),Y
daed d0 e6    BNE &dad5
daef e6 fa    INC &fa
daf1 d0 f0    BNE &dae3
daf3 e6 fb    INC &fb
daf5 a5 fb    LDA &fb
daf7 c9 84    CMP #&84
daf9 90 e8    BCC &dae3
dafb a6 f4    LDX &f4
dafd 10 0d    BPL &db0c
daff ad 06 80 LDA &8006
db02 9d a1 02 STA &02a1,X
db05 29 8f    AND #&8f
db07 d0 03    BNE &db0c
db09 8e 4b 02 STX &024b
db0c e8       INX     
db0d e0 10    CPX #&10
db0f 90 ac    BCC &dabd
db11 2c 40 fe BIT &fe40
db14 30 11    BMI &db27
db16 ce 7b 02 DEC &027b
db19 a0 ff    LDY #&ff
db1b 20 7f ee JSR &ee7f
db1e ca       DEX     
db1f d0 f8    BNE &db19
db21 8e 48 fe STX &fe48
db24 8e 49 fe STX &fe49
db27 ad 8f 02 LDA &028f
db2a 20 00 c3 JSR &c300
db2d a0 ca    LDY #&ca
db2f 20 f1 e4 JSR &e4f1
db32 20 d9 ea JSR &ead9
db35 20 40 f1 JSR &f140
db38 a9 81    LDA #&81
db3a 8d e0 fe STA &fee0
db3d ad e0 fe LDA &fee0
db40 6a       ROR A
db41 90 0a    BCC &db4d
db43 a2 ff    LDX #&ff
db45 20 68 f1 JSR &f168
db48 d0 03    BNE &db4d
db4a ce 7a 02 DEC &027a
db4d a0 0e    LDY #&0e
db4f a2 01    LDX #&01
db51 20 68 f1 JSR &f168
db54 a2 02    LDX #&02
db56 20 68 f1 JSR &f168
db59 8c 43 02 STY &0243
db5c 8c 44 02 STY &0244
db5f a2 fe    LDX #&fe
db61 ac 7a 02 LDY &027a
db64 20 68 f1 JSR &f168
db67 2d 67 02 AND &0267
db6a 10 1b    BPL &db87
db6c a0 02    LDY #&02
db6e 20 a9 de JSR &dea9
db71 ad 8d 02 LDA &028d
db74 f0 0c    BEQ &db82
db76 a0 16    LDY #&16
db78 2c 8e 02 BIT &028e
db7b 30 02    BMI &db7f
db7d a0 11    LDY #&11
db7f 20 a9 de JSR &dea9
db82 a0 1b    LDY #&1b
db84 20 a9 de JSR &dea9
db87 38       SEC     
db88 20 d9 ea JSR &ead9
db8b 20 d9 e9 JSR &e9d9
db8e 08       PHP     
db8f 68       PLA     
db90 4a       LSR A
db91 4a       LSR A
db92 4a       LSR A
db93 4a       LSR A
db94 4d 8f 02 EOR &028f
db97 29 08    AND #&08
db99 a8       TAY     
db9a a2 03    LDX #&03
db9c 20 68 f1 JSR &f168
db9f f0 1d    BEQ &dbbe
dba1 98       TYA     
dba2 d0 14    BNE &dbb8
dba4 a9 8d    LDA #&8d
dba6 20 35 f1 JSR &f135
dba9 a2 d2    LDX #&d2
dbab a0 ea    LDY #&ea
dbad ce 67 02 DEC &0267
dbb0 20 f7 ff JSR &fff7
dbb3 ee 67 02 INC &0267
dbb6 d0 06    BNE &dbbe
dbb8 a9 00    LDA #&00
dbba aa       TAX     
dbbb 20 37 f1 JSR &f137
dbbe ad 8d 02 LDA &028d
dbc1 d0 05    BNE &dbc8
dbc3 ae 8c 02 LDX &028c
dbc6 10 1e    BPL &dbe6
dbc8 a2 0f    LDX #&0f
dbca bd a1 02 LDA &02a1,X
dbcd 2a       ROL A
dbce 30 16    BMI &dbe6
dbd0 ca       DEX     
dbd1 10 f7    BPL &dbca
dbd3 a9 00    LDA #&00
dbd5 2c 7a 02 BIT &027a
dbd8 30 2e    BMI &dc08
dbda 00       BRK     
dbdb f9 4c 61 SBC &614c,Y
dbde 6e 67 75 ROR &7567
dbe1 61 67    ADC (&67;X)
dbe3 65 3f    ADC &3f
dbe5 00       BRK     
dbe6 18       CLC     
dbe7 08       PHP     
dbe8 8e 8c 02 STX &028c
dbeb 20 16 dc JSR &dc16
dbee a9 80    LDA #&80
dbf0 a0 08    LDY #&08
dbf2 20 ab de JSR &deab
dbf5 84 fd    STY &fd
dbf7 20 e7 ff JSR &ffe7
dbfa 20 e7 ff JSR &ffe7
dbfd 28       PLP     
dbfe a9 01    LDA #&01
dc00 2c 7a 02 BIT &027a
dc03 30 03    BMI &dc08
dc05 4c 00 80 JMP &8000
dc08 4c 00 04 JMP &0400
dc0b a6 f4    LDX &f4
dc0d 84 f4    STY &f4
dc0f 8c 30 fe STY &fe30
dc12 a0 00    LDY #&00
dc14 b1 f6    LDA (&f6),Y
dc16 86 f4    STX &f4
dc18 8e 30 fe STX &fe30
dc1b 60       RTS     
dc1c 85 fc    STA &fc
dc1e 68       PLA     
dc1f 48       PHA     
dc20 29 10    AND #&10
dc22 d0 03    BNE &dc27
dc24 6c 04 02 JMP (&0204)
dc27 8a       TXA     
dc28 48       PHA     
dc29 ba       TSX     
dc2a bd 03 01 LDA &0103,X
dc2d d8       CLD     
dc2e 38       SEC     
dc2f e9 01    SBC #&01
dc31 85 fd    STA &fd
dc33 bd 04 01 LDA &0104,X
dc36 e9 00    SBC #&00
dc38 85 fe    STA &fe
dc3a a5 f4    LDA &f4
dc3c 8d 4a 02 STA &024a
dc3f 86 f0    STX &f0
dc41 a2 06    LDX #&06
dc43 20 68 f1 JSR &f168
dc46 ae 8c 02 LDX &028c
dc49 20 16 dc JSR &dc16
dc4c 68       PLA     
dc4d aa       TAX     
dc4e a5 fc    LDA &fc
dc50 58       CLI     
dc51 6c 02 02 JMP (&0202)
dc54 a0 00    LDY #&00
dc56 20 b1 de JSR &deb1
dc59 ad 67 02 LDA &0267
dc5c 6a       ROR A
dc5d b0 fe    BCS &dc5d
dc5f 20 e7 ff JSR &ffe7
dc62 20 e7 ff JSR &ffe7
dc65 4c b8 db JMP &dbb8
dc68 38       SEC     
dc69 6e 4f 02 ROR &024f
dc6c 2c 50 02 BIT &0250
dc6f 10 07    BPL &dc78
dc71 20 41 e7 JSR &e741
dc74 a2 00    LDX #&00
dc76 b0 02    BCS &dc7a
dc78 a2 40    LDX #&40
dc7a 4c 7a e1 JMP &e17a
dc7d ac 09 fe LDY &fe09
dc80 29 3a    AND #&3a
dc82 d0 34    BNE &dcb8
dc84 ae 5c 02 LDX &025c
dc87 d0 09    BNE &dc92
dc89 e8       INX     
dc8a 20 f3 e4 JSR &e4f3
dc8d 20 41 e7 JSR &e741
dc90 90 e6    BCC &dc78
dc92 60       RTS     
dc93 d8       CLD     
dc94 a5 fc    LDA &fc
dc96 48       PHA     
dc97 8a       TXA     
dc98 48       PHA     
dc99 98       TYA     
dc9a 48       PHA     
dc9b a9 de    LDA #&de
dc9d 48       PHA     
dc9e a9 81    LDA #&81
dca0 48       PHA     
dca1 b8       CLV     
dca2 ad 08 fe LDA &fe08
dca5 70 02    BVS &dca9
dca7 10 5d    BPL &dd06
dca9 a6 ea    LDX &ea
dcab ca       DEX     
dcac 30 30    BMI &dcde
dcae 70 2d    BVS &dcdd
dcb0 4c 88 f5 JMP &f588
dcb3 ac 09 fe LDY &fe09
dcb6 2a       ROL A
dcb7 0a       ASL A
dcb8 aa       TAX     
dcb9 98       TYA     
dcba a0 07    LDY #&07
dcbc 4c 94 e4 JMP &e494
dcbf a2 02    LDX #&02
dcc1 20 60 e4 JSR &e460
dcc4 90 10    BCC &dcd6
dcc6 ad 85 02 LDA &0285
dcc9 c9 02    CMP #&02
dccb d0 9b    BNE &dc68
dccd e8       INX     
dcce 20 60 e4 JSR &e460
dcd1 6e d2 02 ROR &02d2
dcd4 30 92    BMI &dc68
dcd6 8d 09 fe STA &fe09
dcd9 a9 e7    LDA #&e7
dcdb 85 ea    STA &ea
dcdd 60       RTS     
dcde 2d 78 02 AND &0278
dce1 4a       LSR A
dce2 90 07    BCC &dceb
dce4 70 05    BVS &dceb
dce6 ac 50 02 LDY &0250
dce9 30 92    BMI &dc7d
dceb 4a       LSR A
dcec 6a       ROR A
dced b0 c4    BCS &dcb3
dcef 30 ce    BMI &dcbf
dcf1 70 ea    BVS &dcdd
dcf3 a2 05    LDX #&05
dcf5 20 68 f1 JSR &f168
dcf8 f0 e3    BEQ &dcdd
dcfa 68       PLA     
dcfb 68       PLA     
dcfc 68       PLA     
dcfd a8       TAY     
dcfe 68       PLA     
dcff aa       TAX     
dd00 68       PLA     
dd01 85 fc    STA &fc
dd03 6c 06 02 JMP (&0206)
dd06 ad 4d fe LDA &fe4d
dd09 10 3c    BPL &dd47
dd0b 2d 79 02 AND &0279
dd0e 2d 4e fe AND &fe4e
dd11 6a       ROR A
dd12 6a       ROR A
dd13 90 54    BCC &dd69
dd15 ce 40 02 DEC &0240
dd18 a5 ea    LDA &ea
dd1a 10 02    BPL &dd1e
dd1c e6 ea    INC &ea
dd1e ad 51 02 LDA &0251
dd21 f0 1a    BEQ &dd3d
dd23 ce 51 02 DEC &0251
dd26 d0 15    BNE &dd3d
dd28 ae 52 02 LDX &0252
dd2b ad 48 02 LDA &0248
dd2e 4a       LSR A
dd2f 90 03    BCC &dd34
dd31 ae 53 02 LDX &0253
dd34 2a       ROL A
dd35 49 01    EOR #&01
dd37 20 00 ea JSR &ea00
dd3a 8e 51 02 STX &0251
dd3d a0 04    LDY #&04
dd3f 20 94 e4 JSR &e494
dd42 a9 02    LDA #&02
dd44 4c 6e de JMP &de6e
dd47 ad 6d fe LDA &fe6d
dd4a 10 a7    BPL &dcf3
dd4c 2d 77 02 AND &0277
dd4f 2d 6e fe AND &fe6e
dd52 6a       ROR A
dd53 6a       ROR A
dd54 90 9d    BCC &dcf3
dd56 ac 85 02 LDY &0285
dd59 88       DEY     
dd5a d0 97    BNE &dcf3
dd5c a9 02    LDA #&02
dd5e 8d 6d fe STA &fe6d
dd61 8d 6e fe STA &fe6e
dd64 a2 03    LDX #&03
dd66 4c 3a e1 JMP &e13a
dd69 2a       ROL A
dd6a 2a       ROL A
dd6b 2a       ROL A
dd6c 2a       ROL A
dd6d 10 5b    BPL &ddca
dd6f a9 20    LDA #&20
dd71 a2 00    LDX #&00
dd73 8d 4d fe STA &fe4d
dd76 8e 49 fe STX &fe49
dd79 a2 08    LDX #&08
dd7b 86 fb    STX &fb
dd7d 20 5b e4 JSR &e45b
dd80 6e d7 02 ROR &02d7
dd83 30 44    BMI &ddc9
dd85 a8       TAY     
dd86 f0 05    BEQ &dd8d
dd88 20 6d ee JSR &ee6d
dd8b 30 3c    BMI &ddc9
dd8d 20 60 e4 JSR &e460
dd90 85 f5    STA &f5
dd92 20 60 e4 JSR &e460
dd95 85 f7    STA &f7
dd97 20 60 e4 JSR &e460
dd9a 85 f6    STA &f6
dd9c a4 f5    LDY &f5
dd9e f0 1b    BEQ &ddbb
dda0 10 16    BPL &ddb8
dda2 24 f5    BIT &f5
dda4 70 05    BVS &ddab
dda6 20 bb ee JSR &eebb
dda9 50 07    BVC &ddb2
ddab 06 f6    ASL &f6
ddad 26 f7    ROL &f7
ddaf 20 3b ee JSR &ee3b
ddb2 ac 61 02 LDY &0261
ddb5 4c 7f ee JMP &ee7f
ddb8 20 7f ee JSR &ee7f
ddbb a4 f6    LDY &f6
ddbd 20 7f ee JSR &ee7f
ddc0 a4 f7    LDY &f7
ddc2 20 7f ee JSR &ee7f
ddc5 46 fb    LSR &fb
ddc7 d0 b4    BNE &dd7d
ddc9 60       RTS     
ddca 90 7b    BCC &de47
ddcc a9 40    LDA #&40
ddce 8d 4d fe STA &fe4d
ddd1 ad 83 02 LDA &0283
ddd4 aa       TAX     
ddd5 49 0f    EOR #&0f
ddd7 48       PHA     
ddd8 a8       TAY     
ddd9 bd 91 02 LDA &0291,X
dddc 69 00    ADC #&00
ddde 99 91 02 STA &0291,Y
dde1 ca       DEX     
dde2 f0 03    BEQ &dde7
dde4 88       DEY     
dde5 d0 f2    BNE &ddd9
dde7 68       PLA     
dde8 8d 83 02 STA &0283
ddeb a2 05    LDX #&05
dded fe 9b 02 INC &029b,X
ddf0 d0 08    BNE &ddfa
ddf2 ca       DEX     
ddf3 d0 f8    BNE &dded
ddf5 a0 05    LDY #&05
ddf7 20 94 e4 JSR &e494
ddfa ad b1 02 LDA &02b1
ddfd d0 08    BNE &de07
ddff ad b2 02 LDA &02b2
de02 f0 06    BEQ &de0a
de04 ce b2 02 DEC &02b2
de07 ce b1 02 DEC &02b1
de0a 2c ce 02 BIT &02ce
de0d 10 0b    BPL &de1a
de0f ee ce 02 INC &02ce
de12 58       CLI     
de13 20 47 eb JSR &eb47
de16 78       SEI     
de17 ce ce 02 DEC &02ce
de1a 2c d7 02 BIT &02d7
de1d 30 0c    BMI &de2b
de1f 20 6d ee JSR &ee6d
de22 49 a0    EOR #&a0
de24 c9 60    CMP #&60
de26 90 03    BCC &de2b
de28 20 79 dd JSR &dd79
de2b 2c b7 d9 BIT &d9b7
de2e 20 a2 dc JSR &dca2
de31 a5 ec    LDA &ec
de33 05 ed    ORA &ed
de35 2d 42 02 AND &0242
de38 f0 04    BEQ &de3e
de3a 38       SEC     
de3b 20 65 f0 JSR &f065
de3e 20 9b e1 JSR &e19b
de41 2c c0 fe BIT &fec0
de44 70 04    BVS &de4a
de46 60       RTS     
de47 2a       ROL A
de48 10 28    BPL &de72
de4a ae 4c 02 LDX &024c
de4d f0 1d    BEQ &de6c
de4f ad c2 fe LDA &fec2
de52 9d b5 02 STA &02b5,X
de55 ad c1 fe LDA &fec1
de58 9d b9 02 STA &02b9,X
de5b 8e be 02 STX &02be
de5e a0 03    LDY #&03
de60 20 94 e4 JSR &e494
de63 ca       DEX     
de64 d0 03    BNE &de69
de66 ae 4d 02 LDX &024d
de69 20 8f de JSR &de8f
de6c a9 10    LDA #&10
de6e 8d 4d fe STA &fe4d
de71 60       RTS     
de72 2a       ROL A
de73 2a       ROL A
de74 2a       ROL A
de75 2a       ROL A
de76 10 07    BPL &de7f
de78 20 65 f0 JSR &f065
de7b a9 01    LDA #&01
de7d d0 ef    BNE &de6e
de7f 4c f3 dc JMP &dcf3

restoreRegistersAndReturnFromInterrupt
de82 68       PLA     
de83 a8       TAY     
de84 68       PLA     
de85 aa       TAX     
de86 68       PLA     
de87 85 fc    STA &fc
de89 a5 fc    LDA &fc
de8b 40       RTI     
de8c 8c be 02 STY &02be
de8f e0 05    CPX #&05
de91 90 02    BCC &de95
de93 a2 04    LDX #&04
de95 8e 4c 02 STX &024c
de98 ac 4e 02 LDY &024e
de9b 88       DEY     
de9c 98       TYA     
de9d 29 08    AND #&08
de9f 18       CLC     
dea0 6d 4c 02 ADC &024c
dea3 e9 00    SBC #&00
dea5 8d c0 fe STA &fec0
dea8 60       RTS     
dea9 a9 c3    LDA #&c3
deab 85 fe    STA &fe
dead a9 00    LDA #&00
deaf 85 fd    STA &fd
deb1 c8       INY     
deb2 b1 fd    LDA (&fd),Y
deb4 20 e3 ff JSR &ffe3
deb7 aa       TAX     
deb8 d0 f7    BNE &deb1
deba 60       RTS     
debb 8e b1 02 STX &02b1
debe 8c b2 02 STY &02b2
dec1 a9 ff    LDA #&ff
dec3 d0 02    BNE &dec7
dec5 a9 00    LDA #&00
dec7 85 e6    STA &e6
dec9 8a       TXA     
deca 48       PHA     
decb 98       TYA     
decc 48       PHA     
decd ac 56 02 LDY &0256
ded0 f0 14    BEQ &dee6
ded2 38       SEC     
ded3 66 eb    ROR &eb
ded5 20 d7 ff JSR &ffd7
ded8 08       PHP     
ded9 46 eb    LSR &eb
dedb 28       PLP     
dedc 90 25    BCC &df03
dede a9 00    LDA #&00
dee0 8d 56 02 STA &0256
dee3 20 ce ff JSR &ffce
dee6 24 ff    BIT &ff
dee8 30 16    BMI &df00
deea ae 41 02 LDX &0241
deed 20 77 e5 JSR &e577
def0 90 11    BCC &df03
def2 24 e6    BIT &e6
def4 50 f0    BVC &dee6
def6 ad b1 02 LDA &02b1
def9 0d b2 02 ORA &02b2
defc d0 e8    BNE &dee6
defe b0 05    BCS &df05
df00 38       SEC     
df01 a9 1b    LDA #&1b
df03 85 e6    STA &e6
df05 68       PLA     
df06 a8       TAY     
df07 68       PLA     
df08 aa       TAX     
df09 a5 e6    LDA &e6
df0b 60       RTS     
df0c 29 43    AND #&43
df0e 28       PLP     
df0f 00       BRK     
df10 2e e0 31 ROL &31e0
df13 05 46    ORA &46
df15 58       CLI     
df16 e3       ???
df17 42       ???
df18 ff       ???
df19 42       ???
df1a 41 53    EOR (&53;X)
df1c 49 43    EOR #&43
df1e e0 18    CPX #&18
df20 00       BRK     
df21 43       ???
df22 41 54    EOR (&54;X)
df24 e0 31    CPX #&31
df26 05 43    ORA &43
df28 4f       ???
df29 44       ???
df2a 45 e3    EOR &e3
df2c 48       PHA     
df2d 88       DEY     
df2e 45 58    EOR &58
df30 45 43    EOR &43
df32 f6 8d    INC &8d,X
df34 00       BRK     
df35 48       PHA     
df36 45 4c    EOR &4c
df38 50 f0    BVC &df2a
df3a b9 ff 4b LDA &4bff,Y
df3d 45 59    EOR &59
df3f e3       ???
df40 27       ???
df41 ff       ???
df42 4c 4f 41 JMP &414f
df45 44       ???
df46 e2       ???
df47 3c       ???
df48 00       BRK     
df49 4c 49 4e JMP &4e49
df4c 45 e6    EOR &e6
df4e 59 01 4d EOR &4d01,Y
df51 4f       ???
df52 54       ???
df53 4f       ???
df54 52       ???
df55 e3       ???
df56 48       PHA     
df57 89       ???
df58 4f       ???
df59 50 54    BVC &dfaf
df5b e3       ???
df5c 48       PHA     
df5d 8b       ???
df5e 52       ???
df5f 55 4e    EOR &4e,X
df61 e0 31    CPX #&31
df63 04       ???
df64 52       ???
df65 4f       ???
df66 4d e3 48 EOR &48e3
df69 8d 53 41 STA &4153
df6c 56 45    LSR &45,X
df6e e2       ???
df6f 3e 00 53 ROL &5300,X
df72 50 4f    BVC &dfc3
df74 4f       ???
df75 4c e2 81 JMP &81e2
df78 00       BRK     
df79 54       ???
df7a 41 50    EOR (&50;X)
df7c 45 e3    EOR &e3
df7e 48       PHA     
df7f 8c 54 56 STY &5654
df82 e3       ???
df83 48       PHA     
df84 90 e0    BCC &df66
df86 31 03    AND (&03),Y
df88 00       BRK     
df89 86 f2    STX &f2
df8b 84 f3    STY &f3
df8d a9 08    LDA #&08
df8f 20 31 e0 JSR &e031
df92 a0 00    LDY #&00
df94 b1 f2    LDA (&f2),Y
df96 c9 0d    CMP #&0d
df98 f0 04    BEQ &df9e
df9a c8       INY     
df9b d0 f7    BNE &df94
df9d 60       RTS     
df9e a0 ff    LDY #&ff
dfa0 20 39 e0 JSR &e039
dfa3 f0 72    BEQ &e017
dfa5 c9 2a    CMP #&2a
dfa7 f0 f7    BEQ &dfa0
dfa9 20 3a e0 JSR &e03a
dfac f0 69    BEQ &e017
dfae c9 7c    CMP #&7c
dfb0 f0 65    BEQ &e017
dfb2 c9 2f    CMP #&2f
dfb4 d0 08    BNE &dfbe
dfb6 c8       INY     
dfb7 20 09 e0 JSR &e009
dfba a9 02    LDA #&02
dfbc d0 73    BNE &e031
dfbe 84 e6    STY &e6
dfc0 a2 00    LDX #&00
dfc2 f0 13    BEQ &dfd7
dfc4 5d 10 df EOR &df10,X
dfc7 29 df    AND #&df
dfc9 d0 17    BNE &dfe2
dfcb c8       INY     
dfcc 18       CLC     
dfcd b0 25    BCS &dff4
dfcf e8       INX     
dfd0 b1 f2    LDA (&f2),Y
dfd2 20 e3 e4 JSR &e4e3
dfd5 90 ed    BCC &dfc4
dfd7 bd 10 df LDA &df10,X
dfda 30 16    BMI &dff2
dfdc b1 f2    LDA (&f2),Y
dfde c9 2e    CMP #&2e
dfe0 f0 04    BEQ &dfe6
dfe2 18       CLC     
dfe3 a4 e6    LDY &e6
dfe5 88       DEY     
dfe6 c8       INY     
dfe7 e8       INX     
dfe8 e8       INX     
dfe9 bd 0e df LDA &df0e,X
dfec f0 33    BEQ &e021
dfee 10 f8    BPL &dfe8
dff0 30 db    BMI &dfcd
dff2 e8       INX     
dff3 e8       INX     
dff4 ca       DEX     
dff5 ca       DEX     
dff6 48       PHA     
dff7 bd 11 df LDA &df11,X
dffa 48       PHA     
dffb 20 3a e0 JSR &e03a
dffe 18       CLC     
dfff 08       PHP     
e000 20 04 e0 JSR &e004
e003 40       RTI     
e004 bd 12 df LDA &df12,X
e007 30 0e    BMI &e017
e009 98       TYA     
e00a bc 12 df LDY &df12,X
e00d 18       CLC     
e00e 65 f2    ADC &f2
e010 aa       TAX     
e011 98       TYA     
e012 a4 f3    LDY &f3
e014 90 01    BCC &e017
e016 c8       INY     
e017 60       RTS     
e018 ae 4b 02 LDX &024b
e01b 30 04    BMI &e021
e01d 38       SEC     
e01e 4c e7 db JMP &dbe7
e021 a4 e6    LDY &e6
e023 a2 04    LDX #&04
e025 20 68 f1 JSR &f168
e028 f0 ed    BEQ &e017
e02a a5 e6    LDA &e6
e02c 20 0d e0 JSR &e00d
e02f a9 03    LDA #&03
e031 6c 1e 02 JMP (&021e)
e034 0a       ASL A
e035 29 01    AND #&01
e037 10 f8    BPL &e031
e039 c8       INY     
e03a b1 f2    LDA (&f2),Y
e03c c9 20    CMP #&20
e03e f0 f9    BEQ &e039
e040 c9 0d    CMP #&0d
e042 60       RTS     
e043 90 f5    BCC &e03a
e045 20 3a e0 JSR &e03a
e048 c9 2c    CMP #&2c
e04a d0 f4    BNE &e040
e04c c8       INY     
e04d 60       RTS     
e04e 20 3a e0 JSR &e03a
e051 20 7d e0 JSR &e07d
e054 90 37    BCC &e08d
e056 85 e6    STA &e6
e058 20 7c e0 JSR &e07c
e05b 90 19    BCC &e076
e05d aa       TAX     
e05e a5 e6    LDA &e6
e060 0a       ASL A
e061 b0 2a    BCS &e08d
e063 0a       ASL A
e064 b0 27    BCS &e08d
e066 65 e6    ADC &e6
e068 b0 23    BCS &e08d
e06a 0a       ASL A
e06b b0 20    BCS &e08d
e06d 85 e6    STA &e6
e06f 8a       TXA     
e070 65 e6    ADC &e6
e072 b0 19    BCS &e08d
e074 90 e0    BCC &e056
e076 a6 e6    LDX &e6
e078 c9 0d    CMP #&0d
e07a 38       SEC     
e07b 60       RTS     
e07c c8       INY     
e07d b1 f2    LDA (&f2),Y
e07f c9 3a    CMP #&3a
e081 b0 0a    BCS &e08d
e083 c9 30    CMP #&30
e085 90 06    BCC &e08d
e087 29 0f    AND #&0f
e089 60       RTS     
e08a 20 45 e0 JSR &e045
e08d 18       CLC     
e08e 60       RTS     
e08f 20 7d e0 JSR &e07d
e092 b0 0e    BCS &e0a2
e094 29 df    AND #&df
e096 c9 47    CMP #&47
e098 b0 f0    BCS &e08a
e09a c9 41    CMP #&41
e09c 90 ec    BCC &e08a
e09e 08       PHP     
e09f e9 37    SBC #&37
e0a1 28       PLP     
e0a2 c8       INY     
e0a3 60       RTS     
e0a4 48       PHA     
e0a5 8a       TXA     
e0a6 48       PHA     
e0a7 98       TYA     
e0a8 48       PHA     
e0a9 ba       TSX     
e0aa bd 03 01 LDA &0103,X
e0ad 48       PHA     
e0ae 2c 60 02 BIT &0260
e0b1 10 08    BPL &e0bb
e0b3 a8       TAY     
e0b4 a9 04    LDA #&04
e0b6 20 7e e5 JSR &e57e
e0b9 b0 52    BCS &e10d
e0bb 18       CLC     
e0bc a9 02    LDA #&02
e0be 2c 7c 02 BIT &027c
e0c1 d0 05    BNE &e0c8
e0c3 68       PLA     
e0c4 48       PHA     
e0c5 20 c0 c4 JSR &c4c0
e0c8 a9 08    LDA #&08
e0ca 2c 7c 02 BIT &027c
e0cd d0 02    BNE &e0d1
e0cf 90 05    BCC &e0d6
e0d1 68       PLA     
e0d2 48       PHA     
e0d3 20 14 e1 JSR &e114
e0d6 ad 7c 02 LDA &027c
e0d9 6a       ROR A
e0da 90 1b    BCC &e0f7
e0dc a4 ea    LDY &ea
e0de 88       DEY     
e0df 10 16    BPL &e0f7
e0e1 68       PLA     
e0e2 48       PHA     
e0e3 08       PHP     
e0e4 78       SEI     
e0e5 a2 02    LDX #&02
e0e7 48       PHA     
e0e8 20 5b e4 JSR &e45b
e0eb 90 03    BCC &e0f0
e0ed 20 70 e1 JSR &e170
e0f0 68       PLA     
e0f1 a2 02    LDX #&02
e0f3 20 f8 e1 JSR &e1f8
e0f6 28       PLP     
e0f7 a9 10    LDA #&10
e0f9 2c 7c 02 BIT &027c
e0fc d0 0f    BNE &e10d
e0fe ac 57 02 LDY &0257
e101 f0 0a    BEQ &e10d
e103 68       PLA     
e104 48       PHA     
e105 38       SEC     
e106 66 eb    ROR &eb
e108 20 d4 ff JSR &ffd4
e10b 46 eb    LSR &eb
e10d 68       PLA     
e10e 68       PLA     
e10f a8       TAY     
e110 68       PLA     
e111 aa       TAX     
e112 68       PLA     
e113 60       RTS     
e114 2c 7c 02 BIT &027c
e117 70 20    BVS &e139
e119 cd 86 02 CMP &0286
e11c f0 1b    BEQ &e139
e11e 08       PHP     
e11f 78       SEI     
e120 aa       TAX     
e121 a9 04    LDA #&04
e123 2c 7c 02 BIT &027c
e126 d0 10    BNE &e138
e128 8a       TXA     
e129 a2 03    LDX #&03
e12b 20 f8 e1 JSR &e1f8
e12e b0 08    BCS &e138
e130 2c d2 02 BIT &02d2
e133 10 03    BPL &e138
e135 20 3a e1 JSR &e13a
e138 28       PLP     
e139 60       RTS     
e13a ad 85 02 LDA &0285
e13d f0 6e    BEQ &e1ad
e13f c9 01    CMP #&01
e141 d0 21    BNE &e164
e143 20 60 e4 JSR &e460
e146 6e d2 02 ROR &02d2
e149 30 45    BMI &e190
e14b a0 82    LDY #&82
e14d 8c 6e fe STY &fe6e
e150 8d 61 fe STA &fe61
e153 ad 6c fe LDA &fe6c
e156 29 f1    AND #&f1
e158 09 0c    ORA #&0c
e15a 8d 6c fe STA &fe6c
e15d 09 0e    ORA #&0e
e15f 8d 6c fe STA &fe6c
e162 d0 2c    BNE &e190
e164 c9 02    CMP #&02
e166 d0 29    BNE &e191
e168 a4 ea    LDY &ea
e16a 88       DEY     
e16b 10 40    BPL &e1ad
e16d 4e d2 02 LSR &02d2
e170 4e 4f 02 LSR &024f
e173 20 41 e7 JSR &e741
e176 90 18    BCC &e190
e178 a2 20    LDX #&20
e17a a0 9f    LDY #&9f
e17c 08       PHP     
e17d 78       SEI     
e17e 98       TYA     
e17f 86 fa    STX &fa
e181 2d 50 02 AND &0250
e184 45 fa    EOR &fa
e186 ae 50 02 LDX &0250
e189 8d 50 02 STA &0250
e18c 8d 08 fe STA &fe08
e18f 28       PLP     
e190 60       RTS     
e191 18       CLC     
e192 a9 01    LDA #&01
e194 20 a2 e1 JSR &e1a2
e197 6e d2 02 ROR &02d2
e19a 60       RTS     
e19b 2c d2 02 BIT &02d2
e19e 30 fa    BMI &e19a
e1a0 a9 00    LDA #&00
e1a2 a2 03    LDX #&03
e1a4 ac 85 02 LDY &0285
e1a7 20 7e e5 JSR &e57e
e1aa 6c 22 02 JMP (&0222)
e1ad 18       CLC     
e1ae 48       PHA     
e1af 08       PHP     
e1b0 78       SEI     
e1b1 b0 08    BCS &e1bb
e1b3 bd ad e9 LDA &e9ad,X
e1b6 10 03    BPL &e1bb
e1b8 20 a2 ec JSR &eca2
e1bb 38       SEC     
e1bc 7e cf 02 ROR &02cf,X
e1bf e0 02    CPX #&02
e1c1 b0 08    BCS &e1cb
e1c3 a9 00    LDA #&00
e1c5 8d 68 02 STA &0268
e1c8 8d 6a 02 STA &026a
e1cb 20 3b e7 JSR &e73b
e1ce 28       PLP     
e1cf 68       PLA     
e1d0 60       RTS     
e1d1 50 07    BVC &e1da
e1d3 bd d8 02 LDA &02d8,X
e1d6 9d e1 02 STA &02e1,X
e1d9 60       RTS     
e1da 08       PHP     
e1db 78       SEI     
e1dc 08       PHP     
e1dd 38       SEC     
e1de bd e1 02 LDA &02e1,X
e1e1 fd d8 02 SBC &02d8,X
e1e4 b0 04    BCS &e1ea
e1e6 38       SEC     
e1e7 fd 47 e4 SBC &e447,X
e1ea 28       PLP     
e1eb 90 06    BCC &e1f3
e1ed 18       CLC     
e1ee 7d 47 e4 ADC &e447,X
e1f1 49 ff    EOR #&ff
e1f3 a0 00    LDY #&00
e1f5 aa       TAX     
e1f6 28       PLP     
e1f7 60       RTS     
e1f8 78       SEI     
e1f9 20 b0 e4 JSR &e4b0
e1fc 90 0f    BCC &e20d
e1fe 20 ea e9 JSR &e9ea
e201 08       PHP     
e202 48       PHA     
e203 20 eb ee JSR &eeeb
e206 68       PLA     
e207 28       PLP     
e208 30 03    BMI &e20d
e20a 58       CLI     
e20b b0 eb    BCS &e1f8
e20d 60       RTS     
e20e 48       PHA     
e20f a9 00    LDA #&00
e211 9d ee 02 STA &02ee,X
e214 9d ef 02 STA &02ef,X
e217 9d f0 02 STA &02f0,X
e21a 9d f1 02 STA &02f1,X
e21d 68       PLA     
e21e 60       RTS     
e21f 84 e6    STY &e6
e221 2a       ROL A
e222 2a       ROL A
e223 2a       ROL A
e224 2a       ROL A
e225 a0 04    LDY #&04
e227 2a       ROL A
e228 3e ee 02 ROL &02ee,X
e22b 3e ef 02 ROL &02ef,X
e22e 3e f0 02 ROL &02f0,X
e231 3e f1 02 ROL &02f1,X
e234 b0 31    BCS &e267
e236 88       DEY     
e237 d0 ee    BNE &e227
e239 a4 e6    LDY &e6
e23b 60       RTS     
e23c a9 ff    LDA #&ff
e23e 86 f2    STX &f2
e240 84 f3    STY &f3
e242 8e ee 02 STX &02ee
e245 8c ef 02 STY &02ef
e248 48       PHA     
e249 a2 02    LDX #&02
e24b 20 0e e2 JSR &e20e
e24e a0 ff    LDY #&ff
e250 8c f4 02 STY &02f4
e253 c8       INY     
e254 20 1d ea JSR &ea1d
e257 20 2f ea JSR &ea2f
e25a 90 fb    BCC &e257
e25c 68       PLA     
e25d 48       PHA     
e25e f0 62    BEQ &e2c2
e260 20 ad e2 JSR &e2ad
e263 b0 3b    BCS &e2a0
e265 f0 3e    BEQ &e2a5
e267 00       BRK     
e268 fc       ???
e269 42       ???
e26a 61 64    ADC (&64;X)
e26c 20 61 64 JSR &6461
e26f 64       ???
e270 72       ???
e271 65 73    ADC &73
e273 73       ???
e274 00       BRK     
e275 a2 10    LDX #&10
e277 20 68 f1 JSR &f168
e27a f0 23    BEQ &e29f
e27c 20 8b f6 JSR &f68b
e27f a9 00    LDA #&00
e281 08       PHP     
e282 84 e6    STY &e6
e284 ac 57 02 LDY &0257
e287 8d 57 02 STA &0257
e28a f0 03    BEQ &e28f
e28c 20 ce ff JSR &ffce
e28f a4 e6    LDY &e6
e291 28       PLP     
e292 f0 0b    BEQ &e29f
e294 a9 80    LDA #&80
e296 20 ce ff JSR &ffce
e299 a8       TAY     
e29a f0 74    BEQ &e310
e29c 8d 57 02 STA &0257
e29f 60       RTS     
e2a0 d0 6e    BNE &e310
e2a2 ee f4 02 INC &02f4
e2a5 a2 ee    LDX #&ee
e2a7 a0 02    LDY #&02
e2a9 68       PLA     
e2aa 4c dd ff JMP &ffdd
e2ad 20 3a e0 JSR &e03a
e2b0 20 8f e0 JSR &e08f
e2b3 90 0c    BCC &e2c1
e2b5 20 0e e2 JSR &e20e
e2b8 20 1f e2 JSR &e21f
e2bb 20 8f e0 JSR &e08f
e2be b0 f8    BCS &e2b8
e2c0 38       SEC     
e2c1 60       RTS     
e2c2 a2 0a    LDX #&0a
e2c4 20 ad e2 JSR &e2ad
e2c7 90 47    BCC &e310
e2c9 b8       CLV     
e2ca b1 f2    LDA (&f2),Y
e2cc c9 2b    CMP #&2b
e2ce d0 04    BNE &e2d4
e2d0 2c b7 d9 BIT &d9b7
e2d3 c8       INY     
e2d4 a2 0e    LDX #&0e
e2d6 20 ad e2 JSR &e2ad
e2d9 90 35    BCC &e310
e2db 08       PHP     
e2dc 50 0f    BVC &e2ed
e2de a2 fc    LDX #&fc
e2e0 18       CLC     
e2e1 bd fc 01 LDA &01fc,X
e2e4 7d 00 02 ADC &0200,X
e2e7 9d 00 02 STA &0200,X
e2ea e8       INX     
e2eb d0 f4    BNE &e2e1
e2ed a2 03    LDX #&03
e2ef bd f8 02 LDA &02f8,X
e2f2 9d f4 02 STA &02f4,X
e2f5 9d f0 02 STA &02f0,X
e2f8 ca       DEX     
e2f9 10 f4    BPL &e2ef
e2fb 28       PLP     
e2fc f0 a7    BEQ &e2a5
e2fe a2 06    LDX #&06
e300 20 ad e2 JSR &e2ad
e303 90 0b    BCC &e310
e305 f0 9e    BEQ &e2a5
e307 a2 02    LDX #&02
e309 20 ad e2 JSR &e2ad
e30c 90 02    BCC &e310
e30e f0 95    BEQ &e2a5
e310 00       BRK     
e311 fe 42 61 INC &6142,X
e314 64       ???
e315 20 63 6f JSR &6f63
e318 6d 6d 61 ADC &616d
e31b 6e 64 00 ROR &0064
e31e fb       ???
e31f 42       ???
e320 61 64    ADC (&64;X)
e322 20 6b 65 JSR &656b
e325 79 00 20 ADC &2000,Y
e328 4e e0 90 LSR &90e0
e32b f1 e0    SBC (&e0),Y
e32d 10 b0    BPL &e2df
e32f ed 20 45 SBC &4520
e332 e0 08    CPX #&08
e334 ae 10 0b LDX &0b10
e337 98       TYA     
e338 48       PHA     
e339 20 d1 e3 JSR &e3d1
e33c 68       PLA     
e33d a8       TAY     
e33e 28       PLP     
e33f d0 36    BNE &e377
e341 60       RTS     
e342 20 4e e0 JSR &e04e
e345 90 c9    BCC &e310
e347 8a       TXA     
e348 48       PHA     
e349 a9 00    LDA #&00
e34b 85 e5    STA &e5
e34d 85 e4    STA &e4
e34f 20 43 e0 JSR &e043
e352 f0 18    BEQ &e36c
e354 20 4e e0 JSR &e04e
e357 90 b7    BCC &e310
e359 86 e5    STX &e5
e35b 20 45 e0 JSR &e045
e35e f0 0c    BEQ &e36c
e360 20 4e e0 JSR &e04e
e363 90 ab    BCC &e310
e365 86 e4    STX &e4
e367 20 3a e0 JSR &e03a
e36a d0 a4    BNE &e310
e36c a4 e4    LDY &e4
e36e a6 e5    LDX &e5
e370 68       PLA     
e371 20 f4 ff JSR &fff4
e374 70 9a    BVS &e310
e376 60       RTS     
e377 38       SEC     
e378 20 1e ea JSR &ea1e
e37b 20 2f ea JSR &ea2f
e37e b0 08    BCS &e388
e380 e8       INX     
e381 f0 9a    BEQ &e31d
e383 9d 00 0b STA &0b00,X
e386 90 f3    BCC &e37b
e388 d0 93    BNE &e31d
e38a 08       PHP     
e38b 78       SEI     
e38c 20 d1 e3 JSR &e3d1
e38f a2 10    LDX #&10
e391 e4 e6    CPX &e6
e393 f0 0e    BEQ &e3a3
e395 bd 00 0b LDA &0b00,X
e398 d9 00 0b CMP &0b00,Y
e39b d0 06    BNE &e3a3
e39d ad 10 0b LDA &0b10
e3a0 9d 00 0b STA &0b00,X
e3a3 ca       DEX     
e3a4 10 eb    BPL &e391
e3a6 28       PLP     
e3a7 60       RTS     
e3a8 08       PHP     
e3a9 78       SEI     
e3aa ad 10 0b LDA &0b10
e3ad 38       SEC     
e3ae f9 00 0b SBC &0b00,Y
e3b1 85 fb    STA &fb
e3b3 8a       TXA     
e3b4 48       PHA     
e3b5 a2 10    LDX #&10
e3b7 bd 00 0b LDA &0b00,X
e3ba 38       SEC     
e3bb f9 00 0b SBC &0b00,Y
e3be 90 08    BCC &e3c8
e3c0 f0 06    BEQ &e3c8
e3c2 c5 fb    CMP &fb
e3c4 b0 02    BCS &e3c8
e3c6 85 fb    STA &fb
e3c8 ca       DEX     
e3c9 10 ec    BPL &e3b7
e3cb 68       PLA     
e3cc aa       TAX     
e3cd a5 fb    LDA &fb
e3cf 28       PLP     
e3d0 60       RTS     
e3d1 08       PHP     
e3d2 78       SEI     
e3d3 8a       TXA     
e3d4 48       PHA     
e3d5 a4 e6    LDY &e6
e3d7 20 a8 e3 JSR &e3a8
e3da b9 00 0b LDA &0b00,Y
e3dd a8       TAY     
e3de 18       CLC     
e3df 65 fb    ADC &fb
e3e1 aa       TAX     
e3e2 85 fa    STA &fa
e3e4 ad 68 02 LDA &0268
e3e7 f0 0d    BEQ &e3f6
e3e9 00       BRK     
e3ea fa       ???
e3eb 4b       ???
e3ec 65 79    ADC &79
e3ee 20 69 6e JSR &6e69
e3f1 20 75 73 JSR &7375
e3f4 65 00    ADC &00
e3f6 ce 84 02 DEC &0284
e3f9 68       PLA     
e3fa 38       SEC     
e3fb e5 fa    SBC &fa
e3fd 85 fa    STA &fa
e3ff f0 0c    BEQ &e40d
e401 bd 01 0b LDA &0b01,X
e404 99 01 0b STA &0b01,Y
e407 c8       INY     
e408 e8       INX     
e409 c6 fa    DEC &fa
e40b d0 f4    BNE &e401
e40d 98       TYA     
e40e 48       PHA     
e40f a4 e6    LDY &e6
e411 a2 10    LDX #&10
e413 bd 00 0b LDA &0b00,X
e416 d9 00 0b CMP &0b00,Y
e419 90 07    BCC &e422
e41b f0 05    BEQ &e422
e41d e5 fb    SBC &fb
e41f 9d 00 0b STA &0b00,X
e422 ca       DEX     
e423 10 ee    BPL &e413
e425 ad 10 0b LDA &0b10
e428 99 00 0b STA &0b00,Y
e42b 68       PLA     
e42c 8d 10 0b STA &0b10
e42f aa       TAX     
e430 ee 84 02 INC &0284
e433 28       PLP     
e434 60       RTS     
e435 03       ???
e436 0a       ASL A
e437 08       PHP     
e438 07       ???
e439 07       ???
e43a 07       ???
e43b 07       ???
e43c 07       ???
e43d 09 00    ORA #&00
e43f 00       BRK     
e440 c0 c0    CPY #&c0
e442 50 60    BVC &e4a4
e444 70 80    BVS &e3c6
e446 00       BRK     
e447 e0 00    CPX #&00
e449 40       RTI     
e44a c0 f0    CPY #&f0
e44c f0 f0    BEQ &e43e
e44e f0 c0    BEQ &e410
e450 bd 3e e4 LDA &e43e,X
e453 85 fa    STA &fa
e455 bd 35 e4 LDA &e435,X
e458 85 fb    STA &fb
e45a 60       RTS     
e45b 2c b7 d9 BIT &d9b7
e45e 70 01    BVS &e461
e460 b8       CLV     
e461 6c 2c 02 JMP (&022c)
e464 08       PHP     
e465 78       SEI     
e466 bd d8 02 LDA &02d8,X
e469 dd e1 02 CMP &02e1,X
e46c f0 72    BEQ &e4e0
e46e a8       TAY     
e46f 20 50 e4 JSR &e450
e472 b1 fa    LDA (&fa),Y
e474 70 1b    BVS &e491
e476 48       PHA     
e477 c8       INY     
e478 98       TYA     
e479 d0 03    BNE &e47e
e47b bd 47 e4 LDA &e447,X
e47e 9d d8 02 STA &02d8,X
e481 e0 02    CPX #&02
e483 90 0a    BCC &e48f
e485 dd e1 02 CMP &02e1,X
e488 d0 05    BNE &e48f
e48a a0 00    LDY #&00
e48c 20 94 e4 JSR &e494
e48f 68       PLA     
e490 a8       TAY     
e491 28       PLP     
e492 18       CLC     
e493 60       RTS     
e494 08       PHP     
e495 78       SEI     
e496 48       PHA     
e497 85 fa    STA &fa
e499 b9 bf 02 LDA &02bf,Y
e49c f0 41    BEQ &e4df
e49e 98       TYA     
e49f a4 fa    LDY &fa
e4a1 20 a5 f0 JSR &f0a5
e4a4 68       PLA     
e4a5 28       PLP     
e4a6 18       CLC     
e4a7 60       RTS     
e4a8 98       TYA     
e4a9 a0 02    LDY #&02
e4ab 20 94 e4 JSR &e494
e4ae a8       TAY     
e4af 98       TYA     
e4b0 6c 2a 02 JMP (&022a)
e4b3 08       PHP     
e4b4 78       SEI     
e4b5 48       PHA     
e4b6 bc e1 02 LDY &02e1,X
e4b9 c8       INY     
e4ba d0 03    BNE &e4bf
e4bc bc 47 e4 LDY &e447,X
e4bf 98       TYA     
e4c0 dd d8 02 CMP &02d8,X
e4c3 f0 0f    BEQ &e4d4
e4c5 bc e1 02 LDY &02e1,X
e4c8 9d e1 02 STA &02e1,X
e4cb 20 50 e4 JSR &e450
e4ce 68       PLA     
e4cf 91 fa    STA (&fa),Y
e4d1 28       PLP     
e4d2 18       CLC     
e4d3 60       RTS     
e4d4 68       PLA     
e4d5 e0 02    CPX #&02
e4d7 b0 07    BCS &e4e0
e4d9 a0 01    LDY #&01
e4db 20 94 e4 JSR &e494
e4de 48       PHA     
e4df 68       PLA     
e4e0 28       PLP     
e4e1 38       SEC     
e4e2 60       RTS     
e4e3 48       PHA     
e4e4 29 df    AND #&df
e4e6 c9 41    CMP #&41
e4e8 90 04    BCC &e4ee
e4ea c9 5b    CMP #&5b
e4ec 90 01    BCC &e4ef
e4ee 38       SEC     
e4ef 68       PLA     
e4f0 60       RTS     
e4f1 a2 00    LDX #&00
e4f3 8a       TXA     
e4f4 2d 45 02 AND &0245
e4f7 d0 b6    BNE &e4af
e4f9 98       TYA     
e4fa 4d 6c 02 EOR &026c
e4fd 0d 75 02 ORA &0275
e500 d0 a6    BNE &e4a8
e502 ad 58 02 LDA &0258
e505 6a       ROR A
e506 98       TYA     
e507 b0 0a    BCS &e513
e509 a0 06    LDY #&06
e50b 20 94 e4 JSR &e494
e50e 90 03    BCC &e513
e510 20 74 e6 JSR &e674
e513 18       CLC     
e514 60       RTS     
e515 6a       ROR A
e516 68       PLA     
e517 b0 79    BCS &e592
e519 98       TYA     
e51a 48       PHA     
e51b 4a       LSR A
e51c 4a       LSR A
e51d 4a       LSR A
e51e 4a       LSR A
e51f 49 04    EOR #&04
e521 a8       TAY     
e522 b9 65 02 LDA &0265,Y
e525 c9 01    CMP #&01
e527 f0 6b    BEQ &e594
e529 68       PLA     
e52a 90 0d    BCC &e539
e52c 29 0f    AND #&0f
e52e 18       CLC     
e52f 79 65 02 ADC &0265,Y
e532 18       CLC     
e533 60       RTS     
e534 20 6f e8 JSR &e86f
e537 68       PLA     
e538 aa       TAX     
e539 20 60 e4 JSR &e460
e53c b0 55    BCS &e593
e53e 48       PHA     
e53f e0 01    CPX #&01
e541 d0 06    BNE &e549
e543 20 73 e1 JSR &e173
e546 a2 01    LDX #&01
e548 38       SEC     
e549 68       PLA     
e54a 90 05    BCC &e551
e54c ac 45 02 LDY &0245
e54f d0 41    BNE &e592
e551 a8       TAY     
e552 10 3e    BPL &e592
e554 29 0f    AND #&0f
e556 c9 0b    CMP #&0b
e558 90 bf    BCC &e519
e55a 69 7b    ADC #&7b
e55c 48       PHA     
e55d ad 7d 02 LDA &027d
e560 d0 b3    BNE &e515
e562 ad 7c 02 LDA &027c
e565 6a       ROR A
e566 6a       ROR A
e567 68       PLA     
e568 b0 cf    BCS &e539
e56a c9 87    CMP #&87
e56c f0 38    BEQ &e5a6
e56e a8       TAY     
e56f 8a       TXA     
e570 48       PHA     
e571 98       TYA     
e572 20 ce d8 JSR &d8ce
e575 68       PLA     
e576 aa       TAX     
e577 2c 5f 02 BIT &025f
e57a 10 05    BPL &e581
e57c a9 06    LDA #&06
e57e 6c 24 02 JMP (&0224)
e581 ad 68 02 LDA &0268
e584 f0 b3    BEQ &e539
e586 ac c9 02 LDY &02c9
e589 b9 01 0b LDA &0b01,Y
e58c ee c9 02 INC &02c9
e58f ce 68 02 DEC &0268
e592 18       CLC     
e593 60       RTS     
e594 68       PLA     
e595 29 0f    AND #&0f
e597 a8       TAY     
e598 20 a8 e3 JSR &e3a8
e59b 8d 68 02 STA &0268
e59e b9 00 0b LDA &0b00,Y
e5a1 8d c9 02 STA &02c9
e5a4 d0 d1    BNE &e577
e5a6 8a       TXA     
e5a7 48       PHA     
e5a8 20 05 d9 JSR &d905
e5ab a8       TAY     
e5ac f0 86    BEQ &e534
e5ae 68       PLA     
e5af aa       TAX     
e5b0 98       TYA     
e5b1 18       CLC     
e5b2 60       RTS     
e5b3 21 e8    AND (&e8;X)
e5b5 88       DEY     
e5b6 e9 d3    SBC #&d3
e5b8 e6 97    INC &97
e5ba e9 97    SBC #&97
e5bc e9 76    SBC #&76
e5be e9 88    SBC #&88
e5c0 e9 8b    SBC #&8b
e5c2 e6 89    INC &89
e5c4 e6 b0    INC &b0
e5c6 e6 b2    INC &b2
e5c8 e6 95    INC &95
e5ca e9 8c    SBC #&8c
e5cc e9 f9    SBC #&f9
e5ce e6 fa    INC &fa
e5d0 e6 a8    INC &a8
e5d2 f0 06    BEQ &e5da
e5d4 e7       ???
e5d5 8c de c8 STY &c8de
e5d8 e9 b6    SBC #&b6
e5da e9 07    SBC #&07
e5dc cd b4 f0 CMP &f0b4
e5df 6c e8 d9 JMP (&d9e8)
e5e2 e9 75    SBC #&75
e5e4 e2       ???
e5e5 45 f0    EOR &f0
e5e7 cf       ???
e5e8 f0 cd    BEQ &e5b7
e5ea f0 97    BEQ &e583
e5ec e1 73    SBC (&73;X)
e5ee e6 74    INC &74
e5f0 e6 5c    INC &5c
e5f2 e6 35    INC &35
e5f4 e0 4f    CPX #&4f
e5f6 e7       ???
e5f7 13       ???
e5f8 e7       ???
e5f9 29 e7    AND #&e7
e5fb 85 f0    STA &f0
e5fd 23       ???
e5fe d9 26 d9 CMP &d926,Y
e601 47       ???
e602 d6 c2    DEC &c2,X
e604 d7       ???
e605 57       ???
e606 e6 7f    INC &7f
e608 e6 af    INC &af
e60a e4 34    CPX &34
e60c e0 35    CPX #&35
e60e f1 35    SBC (&35),Y
e610 f1 e7    SBC (&e7),Y
e612 db       ???
e613 68       PLA     
e614 f1 e3    SBC (&e3),Y
e616 ea       NOP     
e617 60       RTS     
e618 e4 aa    CPX &aa
e61a ff       ???
e61b f4       ???
e61c ea       NOP     
e61d ae ff f9 LDX &f9ff
e620 ea       NOP     
e621 b2       ???
e622 ff       ???
e623 fe ea 5b INC &5bea,X
e626 e4 f3    CPX &f3
e628 e4 ff    CPX &ff
e62a e9 10    SBC #&10
e62c ea       NOP     
e62d 7c       ???
e62e e1 a7    SBC (&a7;X)
e630 ff       ???
e631 6d ee 7f ADC &7fee
e634 ee c0 e9 INC &e9c0
e637 9c       ???
e638 e9 59    SBC #&59
e63a e6 02    INC &02
e63c e9 d5    SBC #&d5
e63e e8       INX     
e63f e8       INX     
e640 e8       INX     
e641 d1 e8    CMP (&e8),Y
e643 e4 e8    CPX &e8
e645 03       ???
e646 e8       INX     
e647 0b       ???
e648 e8       INX     
e649 2d e8 ae AND &aee8
e64c e8       INX     
e64d 35 c7    AND &c7,X
e64f f3       ???
e650 cb       ???
e651 48       PHA     
e652 c7       ???
e653 e0 c8    CPX #&c8
e655 ce d5 a9 DEC &a9d5
e658 00       BRK     
e659 6c 00 02 JMP (&0200)
e65c a2 00    LDX #&00
e65e 24 ff    BIT &ff
e660 10 11    BPL &e673
e662 ad 76 02 LDA &0276
e665 d0 0a    BNE &e671
e667 58       CLI     
e668 8d 69 02 STA &0269
e66b 20 8d f6 JSR &f68d
e66e 20 aa f0 JSR &f0aa
e671 a2 ff    LDX #&ff
e673 18       CLC     
e674 66 ff    ROR &ff
e676 2c 7a 02 BIT &027a
e679 30 01    BMI &e67c
e67b 60       RTS     
e67c 4c 03 04 JMP &0403
e67f ad 82 02 LDA &0282
e682 a8       TAY     
e683 2a       ROL A
e684 e0 01    CPX #&01
e686 6a       ROR A
e687 50 1e    BVC &e6a7
e689 a9 38    LDA #&38
e68b 49 3f    EOR #&3f
e68d 85 fa    STA &fa
e68f ac 82 02 LDY &0282
e692 e0 09    CPX #&09
e694 b0 17    BCS &e6ad
e696 3d ad e9 AND &e9ad,X
e699 85 fb    STA &fb
e69b 98       TYA     
e69c 05 fa    ORA &fa
e69e 45 fa    EOR &fa
e6a0 05 fb    ORA &fb
e6a2 09 40    ORA #&40
e6a4 4d 5d 02 EOR &025d
e6a7 8d 82 02 STA &0282
e6aa 8d 10 fe STA &fe10
e6ad 98       TYA     
e6ae aa       TAX     
e6af 60       RTS     
e6b0 c8       INY     
e6b1 18       CLC     
e6b2 b9 52 02 LDA &0252,Y
e6b5 48       PHA     
e6b6 8a       TXA     
e6b7 99 52 02 STA &0252,Y
e6ba 68       PLA     
e6bb a8       TAY     
e6bc ad 51 02 LDA &0251
e6bf d0 10    BNE &e6d1
e6c1 8e 51 02 STX &0251
e6c4 ad 48 02 LDA &0248
e6c7 08       PHP     
e6c8 6a       ROR A
e6c9 28       PLP     
e6ca 2a       ROL A
e6cb 8d 48 02 STA &0248
e6ce 8d 20 fe STA &fe20
e6d1 50 da    BVC &e6ad
e6d3 8a       TXA     
e6d4 29 01    AND #&01
e6d6 48       PHA     
e6d7 ad 50 02 LDA &0250
e6da 2a       ROL A
e6db e0 01    CPX #&01
e6dd 6a       ROR A
e6de cd 50 02 CMP &0250
e6e1 08       PHP     
e6e2 8d 50 02 STA &0250
e6e5 8d 08 fe STA &fe08
e6e8 20 73 e1 JSR &e173
e6eb 28       PLP     
e6ec f0 03    BEQ &e6f1
e6ee 2c 09 fe BIT &fe09
e6f1 ae 41 02 LDX &0241
e6f4 68       PLA     
e6f5 8d 41 02 STA &0241
e6f8 60       RTS     
e6f9 98       TYA     
e6fa e0 0a    CPX #&0a
e6fc b0 b0    BCS &e6ae
e6fe bc bf 02 LDY &02bf,X
e701 9d bf 02 STA &02bf,X
e704 50 a7    BVC &e6ad
e706 f0 03    BEQ &e70b
e708 20 8c de JSR &de8c
e70b ad 4d 02 LDA &024d
e70e 8e 4d 02 STX &024d
e711 aa       TAX     
e712 60       RTS     
e713 98       TYA     
e714 30 0b    BMI &e721
e716 58       CLI     
e717 20 bb de JSR &debb
e71a b0 03    BCS &e71f
e71c aa       TAX     
e71d a9 00    LDA #&00
e71f a8       TAY     
e720 60       RTS     
e721 8a       TXA     
e722 49 7f    EOR #&7f
e724 aa       TAX     
e725 20 68 f0 JSR &f068
e728 2a       ROL A
e729 a2 ff    LDX #&ff
e72b a0 ff    LDY #&ff
e72d b0 02    BCS &e731
e72f e8       INX     
e730 c8       INY     
e731 60       RTS     
e732 8a       TXA     
e733 49 ff    EOR #&ff
e735 aa       TAX     
e736 e0 02    CPX #&02
e738 b8       CLV     
e739 50 03    BVC &e73e
e73b 2c b7 d9 BIT &d9b7
e73e 6c 2e 02 JMP (&022e)
e741 38       SEC     
e742 a2 01    LDX #&01
e744 20 38 e7 JSR &e738
e747 c0 01    CPY #&01
e749 b0 03    BCS &e74e
e74b ec 5b 02 CPX &025b
e74e 60       RTS     
e74f 30 e1    BMI &e732
e751 f0 0c    BEQ &e75f
e753 e0 05    CPX #&05
e755 b0 d2    BCS &e729
e757 bc b9 02 LDY &02b9,X
e75a bd b5 02 LDA &02b5,X
e75d aa       TAX     
e75e 60       RTS     
e75f ad 40 fe LDA &fe40
e762 6a       ROR A
e763 6a       ROR A
e764 6a       ROR A
e765 6a       ROR A
e766 49 ff    EOR #&ff
e768 29 03    AND #&03
e76a ac be 02 LDY &02be
e76d 8e be 02 STX &02be
e770 aa       TAX     
e771 60       RTS     
e772 48       PHA     
e773 08       PHP     
e774 78       SEI     
e775 85 ef    STA &ef
e777 86 f0    STX &f0
e779 84 f1    STY &f1
e77b a2 07    LDX #&07
e77d c9 75    CMP #&75
e77f 90 41    BCC &e7c2
e781 c9 a1    CMP #&a1
e783 90 09    BCC &e78e
e785 c9 a6    CMP #&a6
e787 90 3f    BCC &e7c8
e789 18       CLC     
e78a a9 a1    LDA #&a1
e78c 69 00    ADC #&00
e78e 38       SEC     
e78f e9 5f    SBC #&5f
e791 0a       ASL A
e792 38       SEC     
e793 84 f1    STY &f1
e795 a8       TAY     
e796 2c 5e 02 BIT &025e
e799 10 07    BPL &e7a2
e79b 8a       TXA     
e79c b8       CLV     
e79d 20 7e e5 JSR &e57e
e7a0 70 1a    BVS &e7bc
e7a2 b9 b4 e5 LDA &e5b4,Y
e7a5 85 fb    STA &fb
e7a7 b9 b3 e5 LDA &e5b3,Y
e7aa 85 fa    STA &fa
e7ac a5 ef    LDA &ef
e7ae a4 f1    LDY &f1
e7b0 b0 04    BCS &e7b6
e7b2 a0 00    LDY #&00
e7b4 b1 f0    LDA (&f0),Y
e7b6 38       SEC     
e7b7 a6 f0    LDX &f0
e7b9 20 58 f0 JSR &f058
e7bc 6a       ROR A
e7bd 28       PLP     
e7be 2a       ROL A
e7bf 68       PLA     
e7c0 b8       CLV     
e7c1 60       RTS     
e7c2 a0 00    LDY #&00
e7c4 c9 16    CMP #&16
e7c6 90 c9    BCC &e791
e7c8 08       PHP     
e7c9 08       PHP     
e7ca 68       PLA     
e7cb 68       PLA     
e7cc 20 68 f1 JSR &f168
e7cf d0 05    BNE &e7d6
e7d1 a6 f0    LDX &f0
e7d3 4c bc e7 JMP &e7bc
e7d6 28       PLP     
e7d7 68       PLA     
e7d8 2c b7 d9 BIT &d9b7
e7db 60       RTS     
e7dc a5 eb    LDA &eb
e7de 30 32    BMI &e812
e7e0 a9 08    LDA #&08
e7e2 25 e2    AND &e2
e7e4 d0 04    BNE &e7ea
e7e6 a9 88    LDA #&88
e7e8 25 bb    AND &bb
e7ea 60       RTS     
e7eb 48       PHA     
e7ec 08       PHP     
e7ed 78       SEI     
e7ee 85 ef    STA &ef
e7f0 86 f0    STX &f0
e7f2 84 f1    STY &f1
e7f4 a2 08    LDX #&08
e7f6 c9 e0    CMP #&e0
e7f8 b0 90    BCS &e78a
e7fa c9 0e    CMP #&0e
e7fc b0 ca    BCS &e7c8
e7fe 69 44    ADC #&44
e800 0a       ASL A
e801 90 90    BCC &e793
e803 20 15 e8 JSR &e815
e806 a1 f9    LDA (&f9;X)
e808 91 f0    STA (&f0),Y
e80a 60       RTS     
e80b 20 15 e8 JSR &e815
e80e b1 f0    LDA (&f0),Y
e810 81 f9    STA (&f9;X)
e812 a9 00    LDA #&00
e814 60       RTS     
e815 85 fa    STA &fa
e817 c8       INY     
e818 b1 f0    LDA (&f0),Y
e81a 85 fb    STA &fb
e81c a0 04    LDY #&04
e81e a2 01    LDX #&01
e820 60       RTS     
e821 d0 fb    BNE &e81e
e823 00       BRK     
e824 f7       ???
e825 4f       ???
e826 53       ???
e827 20 31 2e JSR &2e31
e82a 32       ???
e82b 30 00    BMI &e82d
e82d c8       INY     
e82e b1 f0    LDA (&f0),Y
e830 c9 ff    CMP #&ff
e832 f0 59    BEQ &e88d
e834 c9 20    CMP #&20
e836 a2 08    LDX #&08
e838 b0 90    BCS &e7ca
e83a 88       DEY     
e83b 20 c9 e8 JSR &e8c9
e83e 09 04    ORA #&04
e840 aa       TAX     
e841 90 05    BCC &e848
e843 20 ae e1 JSR &e1ae
e846 a0 01    LDY #&01
e848 20 c9 e8 JSR &e8c9
e84b 85 fa    STA &fa
e84d 08       PHP     
e84e a0 06    LDY #&06
e850 b1 f0    LDA (&f0),Y
e852 48       PHA     
e853 a0 04    LDY #&04
e855 b1 f0    LDA (&f0),Y
e857 48       PHA     
e858 a0 02    LDY #&02
e85a b1 f0    LDA (&f0),Y
e85c 2a       ROL A
e85d 38       SEC     
e85e e9 02    SBC #&02
e860 0a       ASL A
e861 0a       ASL A
e862 05 fa    ORA &fa
e864 20 f8 e1 JSR &e1f8
e867 90 1e    BCC &e887
e869 68       PLA     
e86a 68       PLA     
e86b 28       PLP     
e86c a6 d0    LDX &d0
e86e 60       RTS     
e86f 08       PHP     
e870 78       SEI     
e871 ad 63 02 LDA &0263
e874 29 07    AND #&07
e876 09 04    ORA #&04
e878 aa       TAX     
e879 ad 64 02 LDA &0264
e87c 20 b0 e4 JSR &e4b0
e87f ad 66 02 LDA &0266
e882 48       PHA     
e883 ad 65 02 LDA &0265
e886 48       PHA     
e887 38       SEC     
e888 7e 00 08 ROR &0800,X
e88b 30 17    BMI &e8a4
e88d 08       PHP     
e88e c8       INY     
e88f b1 f0    LDA (&f0),Y
e891 48       PHA     
e892 c8       INY     
e893 b1 f0    LDA (&f0),Y
e895 48       PHA     
e896 a0 00    LDY #&00
e898 b1 f0    LDA (&f0),Y
e89a a2 08    LDX #&08
e89c 20 f8 e1 JSR &e1f8
e89f b0 c8    BCS &e869
e8a1 6e d7 02 ROR &02d7
e8a4 68       PLA     
e8a5 20 b0 e4 JSR &e4b0
e8a8 68       PLA     
e8a9 20 b0 e4 JSR &e4b0
e8ac 28       PLP     
e8ad 60       RTS     
e8ae e9 01    SBC #&01
e8b0 0a       ASL A
e8b1 0a       ASL A
e8b2 0a       ASL A
e8b3 0a       ASL A
e8b4 09 0f    ORA #&0f
e8b6 aa       TAX     
e8b7 a9 00    LDA #&00
e8b9 a0 10    LDY #&10
e8bb c0 0e    CPY #&0e
e8bd b0 02    BCS &e8c1
e8bf b1 f0    LDA (&f0),Y
e8c1 9d c0 08 STA &08c0,X
e8c4 ca       DEX     
e8c5 88       DEY     
e8c6 d0 f3    BNE &e8bb
e8c8 60       RTS     
e8c9 b1 f0    LDA (&f0),Y
e8cb c9 10    CMP #&10
e8cd 29 03    AND #&03
e8cf c8       INY     
e8d0 60       RTS     
e8d1 a2 0f    LDX #&0f
e8d3 d0 03    BNE &e8d8
e8d5 ae 83 02 LDX &0283
e8d8 a0 04    LDY #&04
e8da bd 8d 02 LDA &028d,X
e8dd 91 f0    STA (&f0),Y
e8df e8       INX     
e8e0 88       DEY     
e8e1 10 f7    BPL &e8da
e8e3 60       RTS     
e8e4 a9 0f    LDA #&0f
e8e6 d0 06    BNE &e8ee
e8e8 ad 83 02 LDA &0283
e8eb 49 0f    EOR #&0f
e8ed 18       CLC     
e8ee 48       PHA     
e8ef aa       TAX     
e8f0 a0 04    LDY #&04
e8f2 b1 f0    LDA (&f0),Y
e8f4 9d 8d 02 STA &028d,X
e8f7 e8       INX     
e8f8 88       DEY     
e8f9 10 f7    BPL &e8f2
e8fb 68       PLA     
e8fc b0 e5    BCS &e8e3
e8fe 8d 83 02 STA &0283
e901 60       RTS     
e902 a0 04    LDY #&04
e904 b1 f0    LDA (&f0),Y
e906 99 b1 02 STA &02b1,Y
e909 88       DEY     
e90a c0 02    CPY #&02
e90c b0 f6    BCS &e904
e90e b1 f0    LDA (&f0),Y
e910 85 e9    STA &e9
e912 88       DEY     
e913 8c 69 02 STY &0269
e916 b1 f0    LDA (&f0),Y
e918 85 e8    STA &e8
e91a 58       CLI     
e91b 90 07    BCC &e924
e91d a9 07    LDA #&07
e91f 88       DEY     
e920 c8       INY     
e921 20 ee ff JSR &ffee
e924 20 e0 ff JSR &ffe0
e927 b0 49    BCS &e972
e929 aa       TAX     
e92a ad 7c 02 LDA &027c
e92d 6a       ROR A
e92e 6a       ROR A
e92f 8a       TXA     
e930 b0 05    BCS &e937
e932 ae 6a 02 LDX &026a
e935 d0 ea    BNE &e921
e937 c9 7f    CMP #&7f
e939 d0 07    BNE &e942
e93b c0 00    CPY #&00
e93d f0 e5    BEQ &e924
e93f 88       DEY     
e940 b0 df    BCS &e921
e942 c9 15    CMP #&15
e944 d0 0d    BNE &e953
e946 98       TYA     
e947 f0 db    BEQ &e924
e949 a9 7f    LDA #&7f
e94b 20 ee ff JSR &ffee
e94e 88       DEY     
e94f d0 fa    BNE &e94b
e951 f0 d1    BEQ &e924
e953 91 e8    STA (&e8),Y
e955 c9 0d    CMP #&0d
e957 f0 13    BEQ &e96c
e959 cc b3 02 CPY &02b3
e95c b0 bf    BCS &e91d
e95e cd b4 02 CMP &02b4
e961 90 bc    BCC &e91f
e963 cd b5 02 CMP &02b5
e966 f0 b8    BEQ &e920
e968 90 b6    BCC &e920
e96a b0 b3    BCS &e91f
e96c 20 e7 ff JSR &ffe7
e96f 20 7e e5 JSR &e57e
e972 a5 ff    LDA &ff
e974 2a       ROL A
e975 60       RTS     
e976 58       CLI     
e977 78       SEI     
e978 24 ff    BIT &ff
e97a 30 30    BMI &e9ac
e97c 2c d2 02 BIT &02d2
e97f 10 f5    BPL &e976
e981 20 a4 e1 JSR &e1a4
e984 a0 00    LDY #&00
e986 84 f1    STY &f1
e988 09 f0    ORA #&f0
e98a d0 0e    BNE &e99a
e98c d0 07    BNE &e995
e98e a2 32    LDX #&32
e990 8e 54 02 STX &0254
e993 a2 08    LDX #&08
e995 69 cf    ADC #&cf
e997 18       CLC     
e998 69 e9    ADC #&e9
e99a 86 f0    STX &f0
e99c a8       TAY     
e99d b9 90 01 LDA &0190,Y
e9a0 aa       TAX     
e9a1 25 f1    AND &f1
e9a3 45 f0    EOR &f0
e9a5 99 90 01 STA &0190,Y
e9a8 b9 91 01 LDA &0191,Y
e9ab a8       TAY     
e9ac 60       RTS     
e9ad 64       ???
e9ae 7f       ???
e9af 5b       ???
e9b0 6d c9 f6 ADC &f6c9
e9b3 d2       ???
e9b4 e4 40    CPX &40
e9b6 ad 40 02 LDA &0240
e9b9 58       CLI     
e9ba 78       SEI     
e9bb cd 40 02 CMP &0240
e9be f0 f9    BEQ &e9b9
e9c0 bc 01 03 LDY &0301,X
e9c3 bd 00 03 LDA &0300,X
e9c6 aa       TAX     
e9c7 60       RTS     
e9c8 a9 10    LDA #&10
e9ca 8d 84 02 STA &0284
e9cd a2 00    LDX #&00
e9cf 9d 00 0b STA &0b00,X
e9d2 e8       INX     
e9d3 d0 fa    BNE &e9cf
e9d5 8e 84 02 STX &0284
e9d8 60       RTS     
e9d9 08       PHP     
e9da 78       SEI     
e9db a9 40    LDA #&40
e9dd 20 ea e9 JSR &e9ea
e9e0 30 05    BMI &e9e7
e9e2 18       CLC     
e9e3 b8       CLV     
e9e4 20 68 f0 JSR &f068
e9e7 28       PLP     
e9e8 2a       ROL A
e9e9 60       RTS     
e9ea 90 09    BCC &e9f5
e9ec a0 07    LDY #&07
e9ee 8c 40 fe STY &fe40
e9f1 88       DEY     
e9f2 8c 40 fe STY &fe40
e9f5 24 ff    BIT &ff
e9f7 60       RTS     
e9f8 08       PHP     
e9f9 78       SEI     
e9fa 8d 40 fe STA &fe40
e9fd 28       PLP     
e9fe 60       RTS     
e9ff 8a       TXA     
ea00 08       PHP     
ea01 78       SEI     
ea02 8d 48 02 STA &0248
ea05 8d 20 fe STA &fe20
ea08 ad 53 02 LDA &0253
ea0b 8d 51 02 STA &0251
ea0e 28       PLP     
ea0f 60       RTS     
ea10 8a       TXA     
ea11 49 07    EOR #&07
ea13 08       PHP     
ea14 78       SEI     
ea15 8d 49 02 STA &0249
ea18 8d 21 fe STA &fe21
ea1b 28       PLP     
ea1c 60       RTS     
ea1d 18       CLC     
ea1e 66 e4    ROR &e4
ea20 20 3a e0 JSR &e03a
ea23 c8       INY     
ea24 c9 22    CMP #&22
ea26 f0 02    BEQ &ea2a
ea28 88       DEY     
ea29 18       CLC     
ea2a 66 e4    ROR &e4
ea2c c9 0d    CMP #&0d
ea2e 60       RTS     
ea2f a9 00    LDA #&00
ea31 85 e5    STA &e5
ea33 b1 f2    LDA (&f2),Y
ea35 c9 0d    CMP #&0d
ea37 d0 06    BNE &ea3f
ea39 24 e4    BIT &e4
ea3b 30 52    BMI &ea8f
ea3d 10 1b    BPL &ea5a
ea3f c9 20    CMP #&20
ea41 90 4c    BCC &ea8f
ea43 d0 06    BNE &ea4b
ea45 24 e4    BIT &e4
ea47 30 40    BMI &ea89
ea49 50 0f    BVC &ea5a
ea4b c9 22    CMP #&22
ea4d d0 10    BNE &ea5f
ea4f 24 e4    BIT &e4
ea51 10 36    BPL &ea89
ea53 c8       INY     
ea54 b1 f2    LDA (&f2),Y
ea56 c9 22    CMP #&22
ea58 f0 2f    BEQ &ea89
ea5a 20 3a e0 JSR &e03a
ea5d 38       SEC     
ea5e 60       RTS     
ea5f c9 7c    CMP #&7c
ea61 d0 26    BNE &ea89
ea63 c8       INY     
ea64 b1 f2    LDA (&f2),Y
ea66 c9 7c    CMP #&7c
ea68 f0 1f    BEQ &ea89
ea6a c9 22    CMP #&22
ea6c f0 1b    BEQ &ea89
ea6e c9 21    CMP #&21
ea70 d0 05    BNE &ea77
ea72 c8       INY     
ea73 a9 80    LDA #&80
ea75 d0 ba    BNE &ea31
ea77 c9 20    CMP #&20
ea79 90 14    BCC &ea8f
ea7b c9 3f    CMP #&3f
ea7d f0 08    BEQ &ea87
ea7f 20 bf ea JSR &eabf
ea82 2c b7 d9 BIT &d9b7
ea85 70 03    BVS &ea8a
ea87 a9 7f    LDA #&7f
ea89 b8       CLV     
ea8a c8       INY     
ea8b 05 e5    ORA &e5
ea8d 18       CLC     
ea8e 60       RTS     
ea8f 00       BRK     
ea90 fd 42 61 SBC &6142,X
ea93 64       ???
ea94 20 73 74 JSR &7473
ea97 72       ???
ea98 69 6e    ADC #&6e
ea9a 67       ???
ea9b 00       BRK     
ea9c c9 30    CMP #&30
ea9e f0 1e    BEQ &eabe
eaa0 c9 40    CMP #&40
eaa2 f0 1a    BEQ &eabe
eaa4 90 12    BCC &eab8
eaa6 c9 7f    CMP #&7f
eaa8 f0 14    BEQ &eabe
eaaa b0 10    BCS &eabc
eaac 49 30    EOR #&30
eaae c9 6f    CMP #&6f
eab0 f0 04    BEQ &eab6
eab2 c9 50    CMP #&50
eab4 d0 02    BNE &eab8
eab6 49 1f    EOR #&1f
eab8 c9 21    CMP #&21
eaba 90 02    BCC &eabe
eabc 49 10    EOR #&10
eabe 60       RTS     
eabf c9 7f    CMP #&7f
eac1 f0 0e    BEQ &ead1
eac3 b0 e7    BCS &eaac
eac5 c9 60    CMP #&60
eac7 d0 02    BNE &eacb
eac9 a9 5f    LDA #&5f
eacb c9 40    CMP #&40
eacd 90 02    BCC &ead1
eacf 29 1f    AND #&1f
ead1 60       RTS     
ead2 2f       ???
ead3 21 42    AND (&42;X)
ead5 4f       ???
ead6 4f       ???
ead7 54       ???
ead8 0d ad 87 ORA &87ad
eadb 02       ???
eadc 49 4c    EOR #&4c
eade d0 13    BNE &eaf3
eae0 4c 87 02 JMP &0287
eae3 ad 90 02 LDA &0290
eae6 8e 90 02 STX &0290
eae9 aa       TAX     
eaea 98       TYA     
eaeb 29 01    AND #&01
eaed ac 91 02 LDY &0291
eaf0 8d 91 02 STA &0291
eaf3 60       RTS     
eaf4 98       TYA     
eaf5 9d 00 fc STA &fc00,X
eaf8 60       RTS     
eaf9 98       TYA     
eafa 9d 00 fd STA &fd00,X
eafd 60       RTS     
eafe 98       TYA     
eaff 9d 00 fe STA &fe00,X
eb02 60       RTS     
eb03 a9 04    LDA #&04
eb05 9d 08 08 STA &0808,X
eb08 a9 c0    LDA #&c0
eb0a 9d 04 08 STA &0804,X
eb0d ac 62 02 LDY &0262
eb10 f0 02    BEQ &eb14
eb12 a9 c0    LDA #&c0
eb14 38       SEC     
eb15 e9 40    SBC #&40
eb17 4a       LSR A
eb18 4a       LSR A
eb19 4a       LSR A
eb1a 49 0f    EOR #&0f
eb1c 1d 3c eb ORA &eb3c,X
eb1f 09 10    ORA #&10
eb21 08       PHP     
eb22 78       SEI     
eb23 a0 ff    LDY #&ff
eb25 8c 43 fe STY &fe43
eb28 8d 4f fe STA &fe4f
eb2b c8       INY     
eb2c 8c 40 fe STY &fe40
eb2f a0 02    LDY #&02
eb31 88       DEY     
eb32 d0 fd    BNE &eb31
eb34 a0 08    LDY #&08
eb36 8c 40 fe STY &fe40
eb39 a0 04    LDY #&04
eb3b 88       DEY     
eb3c d0 fd    BNE &eb3b
eb3e 28       PLP     
eb3f 60       RTS     
eb40 e0 c0    CPX #&c0
eb42 a0 80    LDY #&80
eb44 4c 59 ec JMP &ec59
eb47 a9 00    LDA #&00
eb49 8d 3b 08 STA &083b
eb4c ad 38 08 LDA &0838
eb4f d0 06    BNE &eb57
eb51 ee 3b 08 INC &083b
eb54 ce 38 08 DEC &0838
eb57 a2 08    LDX #&08
eb59 ca       DEX     
eb5a bd 00 08 LDA &0800,X
eb5d f0 e5    BEQ &eb44
eb5f bd cf 02 LDA &02cf,X
eb62 30 05    BMI &eb69
eb64 bd 18 08 LDA &0818,X
eb67 d0 03    BNE &eb6c
eb69 20 6b ec JSR &ec6b
eb6c bd 18 08 LDA &0818,X
eb6f f0 13    BEQ &eb84
eb71 c9 ff    CMP #&ff
eb73 f0 12    BEQ &eb87
eb75 de 1c 08 DEC &081c,X
eb78 d0 0d    BNE &eb87
eb7a a9 05    LDA #&05
eb7c 9d 1c 08 STA &081c,X
eb7f de 18 08 DEC &0818,X
eb82 d0 03    BNE &eb87
eb84 20 6b ec JSR &ec6b
eb87 bd 24 08 LDA &0824,X
eb8a f0 05    BEQ &eb91
eb8c de 24 08 DEC &0824,X
eb8f d0 b3    BNE &eb44
eb91 bc 20 08 LDY &0820,X
eb94 c0 ff    CPY #&ff
eb96 f0 ac    BEQ &eb44
eb98 b9 c0 08 LDA &08c0,Y
eb9b 29 7f    AND #&7f
eb9d 9d 24 08 STA &0824,X
eba0 bd 08 08 LDA &0808,X
eba3 c9 04    CMP #&04
eba5 f0 60    BEQ &ec07
eba7 bd 08 08 LDA &0808,X
ebaa 18       CLC     
ebab 7d 20 08 ADC &0820,X
ebae a8       TAY     
ebaf b9 cb 08 LDA &08cb,Y
ebb2 38       SEC     
ebb3 e9 3f    SBC #&3f
ebb5 8d 3a 08 STA &083a
ebb8 b9 c7 08 LDA &08c7,Y
ebbb 8d 39 08 STA &0839
ebbe bd 04 08 LDA &0804,X
ebc1 48       PHA     
ebc2 18       CLC     
ebc3 6d 39 08 ADC &0839
ebc6 50 07    BVC &ebcf
ebc8 2a       ROL A
ebc9 a9 3f    LDA #&3f
ebcb b0 02    BCS &ebcf
ebcd 49 ff    EOR #&ff
ebcf 9d 04 08 STA &0804,X
ebd2 2a       ROL A
ebd3 5d 04 08 EOR &0804,X
ebd6 10 09    BPL &ebe1
ebd8 a9 3f    LDA #&3f
ebda 90 02    BCC &ebde
ebdc 49 ff    EOR #&ff
ebde 9d 04 08 STA &0804,X
ebe1 ce 39 08 DEC &0839
ebe4 bd 04 08 LDA &0804,X
ebe7 38       SEC     
ebe8 ed 3a 08 SBC &083a
ebeb 4d 39 08 EOR &0839
ebee 30 09    BMI &ebf9
ebf0 ad 3a 08 LDA &083a
ebf3 9d 04 08 STA &0804,X
ebf6 fe 08 08 INC &0808,X
ebf9 68       PLA     
ebfa 5d 04 08 EOR &0804,X
ebfd 29 f8    AND #&f8
ebff f0 06    BEQ &ec07
ec01 bd 04 08 LDA &0804,X
ec04 20 0a eb JSR &eb0a
ec07 bd 10 08 LDA &0810,X
ec0a c9 03    CMP #&03
ec0c f0 4b    BEQ &ec59
ec0e bd 14 08 LDA &0814,X
ec11 d0 2a    BNE &ec3d
ec13 fe 10 08 INC &0810,X
ec16 bd 10 08 LDA &0810,X
ec19 c9 03    CMP #&03
ec1b d0 10    BNE &ec2d
ec1d bc 20 08 LDY &0820,X
ec20 b9 c0 08 LDA &08c0,Y
ec23 30 34    BMI &ec59
ec25 a9 00    LDA #&00
ec27 9d 30 08 STA &0830,X
ec2a 9d 10 08 STA &0810,X
ec2d bd 10 08 LDA &0810,X
ec30 18       CLC     
ec31 7d 20 08 ADC &0820,X
ec34 a8       TAY     
ec35 b9 c4 08 LDA &08c4,Y
ec38 9d 14 08 STA &0814,X
ec3b f0 1c    BEQ &ec59
ec3d de 14 08 DEC &0814,X
ec40 bd 20 08 LDA &0820,X
ec43 18       CLC     
ec44 7d 10 08 ADC &0810,X
ec47 a8       TAY     
ec48 b9 c1 08 LDA &08c1,Y
ec4b 18       CLC     
ec4c 7d 30 08 ADC &0830,X
ec4f 9d 30 08 STA &0830,X
ec52 18       CLC     
ec53 7d 0c 08 ADC &080c,X
ec56 20 01 ed JSR &ed01
ec59 e0 04    CPX #&04
ec5b f0 0d    BEQ &ec6a
ec5d 4c 59 eb JMP &eb59
ec60 a2 08    LDX #&08
ec62 ca       DEX     
ec63 20 a2 ec JSR &eca2
ec66 e0 04    CPX #&04
ec68 d0 f8    BNE &ec62
ec6a 60       RTS     
ec6b bd 08 08 LDA &0808,X
ec6e c9 04    CMP #&04
ec70 f0 05    BEQ &ec77
ec72 a9 03    LDA #&03
ec74 9d 08 08 STA &0808,X
ec77 bd cf 02 LDA &02cf,X
ec7a f0 14    BEQ &ec90
ec7c a9 00    LDA #&00
ec7e 9d cf 02 STA &02cf,X
ec81 a0 04    LDY #&04
ec83 99 2b 08 STA &082b,Y
ec86 88       DEY     
ec87 d0 fa    BNE &ec83
ec89 9d 18 08 STA &0818,X
ec8c 88       DEY     
ec8d 8c 38 08 STY &0838
ec90 bd 28 08 LDA &0828,X
ec93 f0 46    BEQ &ecdb
ec95 ad 3b 08 LDA &083b
ec98 f0 36    BEQ &ecd0
ec9a a9 00    LDA #&00
ec9c 9d 28 08 STA &0828,X
ec9f 4c 98 ed JMP &ed98
eca2 20 03 eb JSR &eb03
eca5 98       TYA     
eca6 9d 18 08 STA &0818,X
eca9 9d cf 02 STA &02cf,X
ecac 9d 00 08 STA &0800,X
ecaf a0 03    LDY #&03
ecb1 99 2c 08 STA &082c,Y
ecb4 88       DEY     
ecb5 10 fa    BPL &ecb1
ecb7 8c 38 08 STY &0838
ecba 30 4a    BMI &ed06
ecbc 08       PHP     
ecbd 78       SEI     
ecbe bd 08 08 LDA &0808,X
ecc1 c9 04    CMP #&04
ecc3 d0 0a    BNE &eccf
ecc5 20 5b e4 JSR &e45b
ecc8 90 05    BCC &eccf
ecca a9 00    LDA #&00
eccc 9d 00 08 STA &0800,X
eccf 28       PLP     
ecd0 bc 20 08 LDY &0820,X
ecd3 c0 ff    CPY #&ff
ecd5 d0 03    BNE &ecda
ecd7 20 03 eb JSR &eb03
ecda 60       RTS     
ecdb 20 5b e4 JSR &e45b
ecde b0 dc    BCS &ecbc
ece0 29 03    AND #&03
ece2 f0 bb    BEQ &ec9f
ece4 ad 38 08 LDA &0838
ece7 f0 15    BEQ &ecfe
ece9 fe 28 08 INC &0828,X
ecec 2c 38 08 BIT &0838
ecef 10 0a    BPL &ecfb
ecf1 20 5b e4 JSR &e45b
ecf4 29 03    AND #&03
ecf6 8d 38 08 STA &0838
ecf9 10 03    BPL &ecfe
ecfb ce 38 08 DEC &0838
ecfe 4c d0 ec JMP &ecd0
ed01 dd 2c 08 CMP &082c,X
ed04 f0 d4    BEQ &ecda
ed06 9d 2c 08 STA &082c,X
ed09 e0 04    CPX #&04
ed0b d0 09    BNE &ed16
ed0d 29 0f    AND #&0f
ed0f 1d 3c eb ORA &eb3c,X
ed12 08       PHP     
ed13 4c 95 ed JMP &ed95
ed16 48       PHA     
ed17 29 03    AND #&03
ed19 8d 3c 08 STA &083c
ed1c a9 00    LDA #&00
ed1e 8d 3d 08 STA &083d
ed21 68       PLA     
ed22 4a       LSR A
ed23 4a       LSR A
ed24 c9 0c    CMP #&0c
ed26 90 07    BCC &ed2f
ed28 ee 3d 08 INC &083d
ed2b e9 0c    SBC #&0c
ed2d d0 f5    BNE &ed24
ed2f a8       TAY     
ed30 ad 3d 08 LDA &083d
ed33 48       PHA     
ed34 b9 fb ed LDA &edfb,Y
ed37 8d 3d 08 STA &083d
ed3a b9 07 ee LDA &ee07,Y
ed3d 48       PHA     
ed3e 29 03    AND #&03
ed40 8d 3e 08 STA &083e
ed43 68       PLA     
ed44 4a       LSR A
ed45 4a       LSR A
ed46 4a       LSR A
ed47 4a       LSR A
ed48 8d 3f 08 STA &083f
ed4b ad 3d 08 LDA &083d
ed4e ac 3c 08 LDY &083c
ed51 f0 0c    BEQ &ed5f
ed53 38       SEC     
ed54 ed 3f 08 SBC &083f
ed57 b0 03    BCS &ed5c
ed59 ce 3e 08 DEC &083e
ed5c 88       DEY     
ed5d d0 f4    BNE &ed53
ed5f 8d 3d 08 STA &083d
ed62 68       PLA     
ed63 a8       TAY     
ed64 f0 09    BEQ &ed6f
ed66 4e 3e 08 LSR &083e
ed69 6e 3d 08 ROR &083d
ed6c 88       DEY     
ed6d d0 f7    BNE &ed66
ed6f ad 3d 08 LDA &083d
ed72 18       CLC     
ed73 7d 3d c4 ADC &c43d,X
ed76 8d 3d 08 STA &083d
ed79 90 03    BCC &ed7e
ed7b ee 3e 08 INC &083e
ed7e 29 0f    AND #&0f
ed80 1d 3c eb ORA &eb3c,X
ed83 08       PHP     
ed84 78       SEI     
ed85 20 21 eb JSR &eb21
ed88 ad 3d 08 LDA &083d
ed8b 4e 3e 08 LSR &083e
ed8e 6a       ROR A
ed8f 4e 3e 08 LSR &083e
ed92 6a       ROR A
ed93 4a       LSR A
ed94 4a       LSR A
ed95 4c 22 eb JMP &eb22
ed98 08       PHP     
ed99 78       SEI     
ed9a 20 60 e4 JSR &e460
ed9d 48       PHA     
ed9e 29 04    AND #&04
eda0 f0 15    BEQ &edb7
eda2 68       PLA     
eda3 bc 20 08 LDY &0820,X
eda6 c0 ff    CPY #&ff
eda8 d0 03    BNE &edad
edaa 20 03 eb JSR &eb03
edad 20 60 e4 JSR &e460
edb0 20 60 e4 JSR &e460
edb3 28       PLP     
edb4 4c f7 ed JMP &edf7
edb7 68       PLA     
edb8 29 f8    AND #&f8
edba 0a       ASL A
edbb 90 0b    BCC &edc8
edbd 49 ff    EOR #&ff
edbf 4a       LSR A
edc0 38       SEC     
edc1 e9 40    SBC #&40
edc3 20 0a eb JSR &eb0a
edc6 a9 ff    LDA #&ff
edc8 9d 20 08 STA &0820,X
edcb a9 05    LDA #&05
edcd 9d 1c 08 STA &081c,X
edd0 a9 01    LDA #&01
edd2 9d 24 08 STA &0824,X
edd5 a9 00    LDA #&00
edd7 9d 14 08 STA &0814,X
edda 9d 08 08 STA &0808,X
eddd 9d 30 08 STA &0830,X
ede0 a9 ff    LDA #&ff
ede2 9d 10 08 STA &0810,X
ede5 20 60 e4 JSR &e460
ede8 9d 0c 08 STA &080c,X
edeb 20 60 e4 JSR &e460
edee 28       PLP     
edef 48       PHA     
edf0 bd 0c 08 LDA &080c,X
edf3 20 01 ed JSR &ed01
edf6 68       PLA     
edf7 9d 18 08 STA &0818,X
edfa 60       RTS     
edfb f0 b7    BEQ &edb4
edfd 82       ???
edfe 4f       ???
edff 20 f3 c8 JSR &c8f3
ee02 a0 7b    LDY #&7b
ee04 57       ???
ee05 35 16    AND &16,X
ee07 e7       ???
ee08 d7       ???
ee09 cb       ???
ee0a c3       ???
ee0b b7       ???
ee0c aa       TAX     
ee0d a2 9a    LDX #&9a
ee0f 92       ???
ee10 8a       TXA     
ee11 82       ???
ee12 7a       ???
ee13 a9 ef    LDA #&ef
ee15 85 f5    STA &f5
ee17 60       RTS     
ee18 a2 0d    LDX #&0d
ee1a e6 f5    INC &f5
ee1c a4 f5    LDY &f5
ee1e 10 39    BPL &ee59
ee20 a2 00    LDX #&00
ee22 86 f7    STX &f7
ee24 e8       INX     
ee25 86 f6    STX &f6
ee27 20 bb ee JSR &eebb
ee2a a2 03    LDX #&03
ee2c 20 62 ee JSR &ee62
ee2f dd 0c df CMP &df0c,X
ee32 d0 e4    BNE &ee18
ee34 ca       DEX     
ee35 10 f5    BPL &ee2c
ee37 a9 3e    LDA #&3e
ee39 85 f6    STA &f6
ee3b 20 bb ee JSR &eebb
ee3e a2 ff    LDX #&ff
ee40 20 62 ee JSR &ee62
ee43 a0 08    LDY #&08
ee45 0a       ASL A
ee46 76 f7    ROR &f7,X
ee48 88       DEY     
ee49 d0 fa    BNE &ee45
ee4b e8       INX     
ee4c f0 f2    BEQ &ee40
ee4e 18       CLC     
ee4f 90 6a    BCC &eebb
ee51 a2 0e    LDX #&0e
ee53 a4 f5    LDY &f5
ee55 30 0b    BMI &ee62
ee57 a0 ff    LDY #&ff
ee59 08       PHP     
ee5a 20 68 f1 JSR &f168
ee5d 28       PLP     
ee5e c9 01    CMP #&01
ee60 98       TYA     
ee61 60       RTS     
ee62 08       PHP     
ee63 78       SEI     
ee64 a0 10    LDY #&10
ee66 20 7f ee JSR &ee7f
ee69 a0 00    LDY #&00
ee6b f0 17    BEQ &ee84
ee6d a0 00    LDY #&00
ee6f f0 11    BEQ &ee82
ee71 48       PHA     
ee72 20 7a ee JSR &ee7a
ee75 68       PLA     
ee76 6a       ROR A
ee77 6a       ROR A
ee78 6a       ROR A
ee79 6a       ROR A
ee7a 29 0f    AND #&0f
ee7c 09 40    ORA #&40
ee7e a8       TAY     
ee7f 98       TYA     
ee80 a0 01    LDY #&01
ee82 08       PHP     
ee83 78       SEI     
ee84 2c 7b 02 BIT &027b
ee87 10 21    BPL &eeaa
ee89 48       PHA     
ee8a b9 75 f0 LDA &f075,Y
ee8d 8d 43 fe STA &fe43
ee90 68       PLA     
ee91 8d 4f fe STA &fe4f
ee94 b9 77 f0 LDA &f077,Y
ee97 8d 40 fe STA &fe40
ee9a 2c 40 fe BIT &fe40
ee9d 30 fb    BMI &ee9a
ee9f ad 4f fe LDA &fe4f
eea2 48       PHA     
eea3 b9 79 f0 LDA &f079,Y
eea6 8d 40 fe STA &fe40
eea9 68       PLA     
eeaa 28       PLP     
eeab a8       TAY     
eeac 60       RTS     
eead ad cb 03 LDA &03cb
eeb0 85 f6    STA &f6
eeb2 ad cc 03 LDA &03cc
eeb5 85 f7    STA &f7
eeb7 a5 f5    LDA &f5
eeb9 10 1e    BPL &eed9
eebb 08       PHP     
eebc 78       SEI     
eebd a5 f6    LDA &f6
eebf 20 71 ee JSR &ee71
eec2 a5 f5    LDA &f5
eec4 85 fa    STA &fa
eec6 a5 f7    LDA &f7
eec8 2a       ROL A
eec9 2a       ROL A
eeca 46 fa    LSR &fa
eecc 6a       ROR A
eecd 46 fa    LSR &fa
eecf 6a       ROR A
eed0 20 71 ee JSR &ee71
eed3 a5 fa    LDA &fa
eed5 20 7a ee JSR &ee7a
eed8 28       PLP     
eed9 60       RTS     
eeda a2 ff    LDX #&ff
eedc a5 ec    LDA &ec
eede 05 ed    ORA &ed
eee0 d0 06    BNE &eee8
eee2 a9 81    LDA #&81
eee4 8d 4e fe STA &fe4e
eee7 e8       INX     
eee8 8e 42 02 STX &0242
eeeb 08       PHP     
eeec ad 5a 02 LDA &025a
eeef 4a       LSR A
eef0 29 18    AND #&18
eef2 09 06    ORA #&06
eef4 8d 40 fe STA &fe40
eef7 4a       LSR A
eef8 09 07    ORA #&07
eefa 8d 40 fe STA &fe40
eefd 20 2e f1 JSR &f12e
ef00 68       PLA     
ef01 60       RTS     
ef02 50 0a    BVC &ef0e
ef04 a9 01    LDA #&01
ef06 8d 4e fe STA &fe4e
ef09 b0 08    BCS &ef13
ef0b 4c 0f f0 JMP &f00f
ef0e 90 06    BCC &ef16
ef10 4c d1 f0 JMP &f0d1
ef13 ee 42 02 INC &0242
ef16 ad 5a 02 LDA &025a
ef19 29 b7    AND #&b7
ef1b a2 00    LDX #&00
ef1d 20 2a f0 JSR &f02a
ef20 86 fa    STX &fa
ef22 b8       CLV     
ef23 10 05    BPL &ef2a
ef25 2c b7 d9 BIT &d9b7
ef28 09 08    ORA #&08
ef2a e8       INX     
ef2b 20 2a f0 JSR &f02a
ef2e 90 bb    BCC &eeeb
ef30 10 02    BPL &ef34
ef32 09 40    ORA #&40
ef34 8d 5a 02 STA &025a
ef37 a6 ec    LDX &ec
ef39 f0 12    BEQ &ef4d
ef3b 20 2a f0 JSR &f02a
ef3e 30 10    BMI &ef50
ef40 e4 ec    CPX &ec
ef42 86 ec    STX &ec
ef44 d0 07    BNE &ef4d
ef46 a2 00    LDX #&00
ef48 86 ec    STX &ec
ef4a 20 1f f0 JSR &f01f
ef4d 4c e9 ef JMP &efe9
ef50 e4 ec    CPX &ec
ef52 d0 ee    BNE &ef42
ef54 a5 e7    LDA &e7
ef56 f0 23    BEQ &ef7b
ef58 c6 e7    DEC &e7
ef5a d0 1f    BNE &ef7b
ef5c ad ca 02 LDA &02ca
ef5f 85 e7    STA &e7
ef61 ad 55 02 LDA &0255
ef64 8d ca 02 STA &02ca
ef67 ad 5a 02 LDA &025a
ef6a a6 ec    LDX &ec
ef6c e0 d0    CPX #&d0
ef6e d0 0e    BNE &ef7e
ef70 09 90    ORA #&90
ef72 49 a0    EOR #&a0
ef74 8d 5a 02 STA &025a
ef77 a9 00    LDA #&00
ef79 85 e7    STA &e7
ef7b 4c e9 ef JMP &efe9
ef7e e0 c0    CPX #&c0
ef80 d0 0f    BNE &ef91
ef82 09 a0    ORA #&a0
ef84 24 fa    BIT &fa
ef86 10 04    BPL &ef8c
ef88 09 10    ORA #&10
ef8a 49 80    EOR #&80
ef8c 49 90    EOR #&90
ef8e 4c 74 ef JMP &ef74
ef91 bd ab ef LDA &efab,X
ef94 d0 03    BNE &ef99
ef96 ad 6b 02 LDA &026b
ef99 ae 5a 02 LDX &025a
ef9c 86 fa    STX &fa
ef9e 26 fa    ROL &fa
efa0 10 07    BPL &efa9
efa2 a6 ed    LDX &ed
efa4 d0 a4    BNE &ef4a
efa6 20 bf ea JSR &eabf
efa9 26 fa    ROL &fa
efab 30 08    BMI &efb5
efad 20 9c ea JSR &ea9c
efb0 26 fa    ROL &fa
efb2 4c c1 ef JMP &efc1
efb5 26 fa    ROL &fa
efb7 30 0d    BMI &efc6
efb9 20 e3 e4 JSR &e4e3
efbc b0 08    BCS &efc6
efbe 20 9c ea JSR &ea9c
efc1 ae 5a 02 LDX &025a
efc4 10 0b    BPL &efd1
efc6 26 fa    ROL &fa
efc8 10 07    BPL &efd1
efca a6 ed    LDX &ed
efcc d0 d6    BNE &efa4
efce 20 9c ea JSR &ea9c
efd1 cd 6c 02 CMP &026c
efd4 d0 07    BNE &efdd
efd6 ae 75 02 LDX &0275
efd9 d0 02    BNE &efdd
efdb 86 e7    STX &e7
efdd a8       TAY     
efde 20 29 f1 JSR &f129
efe1 ad 59 02 LDA &0259
efe4 d0 03    BNE &efe9
efe6 20 f1 e4 JSR &e4f1
efe9 a6 ed    LDX &ed
efeb f0 0b    BEQ &eff8
efed 20 2a f0 JSR &f02a
eff0 86 ed    STX &ed
eff2 30 04    BMI &eff8
eff4 a2 00    LDX #&00
eff6 86 ed    STX &ed
eff8 a6 ed    LDX &ed
effa d0 16    BNE &f012
effc a0 ec    LDY #&ec
effe 20 cc f0 JSR &f0cc
f001 30 09    BMI &f00c
f003 a5 ec    LDA &ec
f005 85 ed    STA &ed
f007 86 ec    STX &ec
f009 20 1f f0 JSR &f01f
f00c 4c da ee JMP &eeda
f00f 20 2a f0 JSR &f02a
f012 a5 ec    LDA &ec
f014 d0 f6    BNE &f00c
f016 a0 ed    LDY #&ed
f018 20 cc f0 JSR &f0cc
f01b 30 ef    BMI &f00c
f01d 10 e8    BPL &f007
f01f a2 01    LDX #&01
f021 86 e7    STX &e7
f023 ae 54 02 LDX &0254
f026 8e ca 02 STX &02ca
f029 60       RTS     
f02a a0 03    LDY #&03
f02c 8c 40 fe STY &fe40
f02f a0 7f    LDY #&7f
f031 8c 43 fe STY &fe43
f034 8e 4f fe STX &fe4f
f037 ae 4f fe LDX &fe4f
f03a 60       RTS     
f03b 71 33    ADC (&33),Y
f03d 34       ???
f03e 35 84    AND &84,X
f040 38       SEC     
f041 87       ???
f042 2d 5e 8c AND &8c5e
f045 84 ec    STY &ec
f047 86 ed    STX &ed
f049 60       RTS     
f04a 00       BRK     
f04b 80       ???
f04c 77       ???
f04d 65 74    ADC &74
f04f 37       ???
f050 69 39    ADC #&39
f052 30 5f    BMI &f0b3
f054 8e 6c fe STX &fe6c
f057 fd 6c fa SBC &fa6c,X
f05a 00       BRK     
f05b 31 32    AND (&32),Y
f05d 64       ???
f05e 72       ???
f05f 36 75    ROL &75,X
f061 6f       ???
f062 70 5b    BVS &f0bf
f064 8f       ???
f065 2c b7 d9 BIT &d9b7
f068 6c 28 02 JMP (&0228)
f06b 01 61    ORA (&61;X)
f06d 78       SEI     
f06e 66 79    ROR &79
f070 6a       ROR A
f071 6b       ???
f072 40       RTI     
f073 3a       ???
f074 0d 00 ff ORA &ff00
f077 01 02    ORA (&02;X)
f079 09 0a    ORA #&0a
f07b 02       ???
f07c 73       ???
f07d 63       ???
f07e 67       ???
f07f 68       PLA     
f080 6e 6c 3b ROR &3b6c
f083 5d 7f ac EOR &ac7f,X
f086 44       ???
f087 02       ???
f088 a2 00    LDX #&00
f08a 60       RTS     
f08b 00       BRK     
f08c 7a       ???
f08d 20 76 62 JSR &6276
f090 6d 2c 2e ADC &2e2c
f093 2f       ???
f094 8b       ???
f095 ae 41 02 LDX &0241
f098 4c ad e1 JMP &e1ad
f09b 1b       ???
f09c 81 82    STA (&82;X)
f09e 83       ???
f09f 85 86    STA &86
f0a1 88       DEY     
f0a2 89       ???
f0a3 5c       ???
f0a4 8d 6c 20 STA &206c
f0a7 02       ???
f0a8 d0 eb    BNE &f095
f0aa a2 08    LDX #&08
f0ac 58       CLI     
f0ad 78       SEI     
f0ae 20 b4 f0 JSR &f0b4
f0b1 ca       DEX     
f0b2 10 f8    BPL &f0ac
f0b4 e0 09    CPX #&09
f0b6 90 e0    BCC &f098
f0b8 60       RTS     
f0b9 a2 09    LDX #&09
f0bb 20 68 f1 JSR &f168
f0be 20 4a fa JSR &fa4a
f0c1 0d 4f 53 ORA &534f
f0c4 20 31 2e JSR &2e31
f0c7 32       ???
f0c8 30 0d    BMI &f0d7
f0ca 00       BRK     
f0cb 60       RTS     
f0cc 18       CLC     
f0cd a2 10    LDX #&10
f0cf b0 97    BCS &f068
f0d1 8a       TXA     
f0d2 10 05    BPL &f0d9
f0d4 20 2a f0 JSR &f02a
f0d7 b0 55    BCS &f12e
f0d9 08       PHP     
f0da 90 02    BCC &f0de
f0dc a0 ee    LDY #&ee
f0de 99 df 01 STA &01df,Y
f0e1 a2 09    LDX #&09
f0e3 20 29 f1 JSR &f129
f0e6 a9 7f    LDA #&7f
f0e8 8d 43 fe STA &fe43
f0eb a9 03    LDA #&03
f0ed 8d 40 fe STA &fe40
f0f0 a9 0f    LDA #&0f
f0f2 8d 4f fe STA &fe4f
f0f5 a9 01    LDA #&01
f0f7 8d 4d fe STA &fe4d
f0fa 8e 4f fe STX &fe4f
f0fd 2c 4d fe BIT &fe4d
f100 f0 21    BEQ &f123
f102 8a       TXA     
f103 d9 df 01 CMP &01df,Y
f106 90 16    BCC &f11e
f108 8d 4f fe STA &fe4f
f10b 2c 4f fe BIT &fe4f
f10e 10 0e    BPL &f11e
f110 28       PLP     
f111 08       PHP     
f112 b0 13    BCS &f127
f114 48       PHA     
f115 59 00 00 EOR &0000,Y
f118 0a       ASL A
f119 c9 01    CMP #&01
f11b 68       PLA     
f11c b0 09    BCS &f127
f11e 18       CLC     
f11f 69 10    ADC #&10
f121 10 e0    BPL &f103
f123 ca       DEX     
f124 10 bd    BPL &f0e3
f126 8a       TXA     
f127 aa       TAX     
f128 28       PLP     
f129 20 2e f1 JSR &f12e
f12c 58       CLI     
f12d 78       SEI     
f12e a9 0b    LDA #&0b
f130 8d 40 fe STA &fe40
f133 8a       TXA     
f134 60       RTS     
f135 49 8c    EOR #&8c
f137 0a       ASL A
f138 8d 47 02 STA &0247
f13b e0 03    CPX #&03
f13d 4c 4b f1 JMP &f14b
f140 08       PHP     
f141 a9 a1    LDA #&a1
f143 85 e3    STA &e3
f145 a9 19    LDA #&19
f147 8d d1 03 STA &03d1
f14a 28       PLP     
f14b 08       PHP     
f14c a9 06    LDA #&06
f14e 20 31 e0 JSR &e031
f151 a2 06    LDX #&06
f153 28       PLP     
f154 f0 01    BEQ &f157
f156 ca       DEX     
f157 86 c6    STX &c6
f159 a2 0e    LDX #&0e
f15b bd 51 d9 LDA &d951,X
f15e 9d 11 02 STA &0211,X
f161 ca       DEX     
f162 d0 f7    BNE &f15b
f164 86 c2    STX &c2
f166 a2 0f    LDX #&0f
f168 a5 f4    LDA &f4
f16a 48       PHA     
f16b 8a       TXA     
f16c a2 0f    LDX #&0f
f16e fe a1 02 INC &02a1,X
f171 de a1 02 DEC &02a1,X
f174 10 0d    BPL &f183
f176 86 f4    STX &f4
f178 8e 30 fe STX &fe30
f17b 20 03 80 JSR &8003
f17e aa       TAX     
f17f f0 05    BEQ &f186
f181 a6 f4    LDX &f4
f183 ca       DEX     
f184 10 e8    BPL &f16e
f186 68       PLA     
f187 85 f4    STA &f4
f189 8d 30 fe STA &fe30
f18c 8a       TXA     
f18d 60       RTS     
f18e 09 00    ORA #&00
f190 d0 10    BNE &f1a2
f192 c0 00    CPY #&00
f194 d0 0c    BNE &f1a2
f196 a5 c6    LDA &c6
f198 29 fb    AND #&fb
f19a 0d 47 02 ORA &0247
f19d 0a       ASL A
f19e 0d 47 02 ORA &0247
f1a1 4a       LSR A
f1a2 60       RTS     
f1a3 4c f5 1d JMP &1df5
f1a6 f6 04    INC &04,X
f1a8 f3       ???
f1a9 0f       ???
f1aa e3       ???
f1ab 04       ???
f1ac f3       ???
f1ad 2a       ROL A
f1ae f3       ???
f1af 74       ???
f1b0 e2       ???
f1b1 c9 07    CMP #&07
f1b3 b0 ed    BCS &f1a2
f1b5 86 bc    STX &bc
f1b7 0a       ASL A
f1b8 aa       TAX     
f1b9 bd a4 f1 LDA &f1a4,X
f1bc 48       PHA     
f1bd bd a3 f1 LDA &f1a3,X
f1c0 48       PHA     
f1c1 a6 bc    LDX &bc
f1c3 60       RTS     
f1c4 08       PHP     
f1c5 48       PHA     
f1c6 20 27 fb JSR &fb27
f1c9 ad c2 03 LDA &03c2
f1cc 48       PHA     
f1cd 20 31 f6 JSR &f631
f1d0 68       PLA     
f1d1 f0 1a    BEQ &f1ed
f1d3 a2 03    LDX #&03
f1d5 a9 ff    LDA #&ff
f1d7 48       PHA     
f1d8 bd be 03 LDA &03be,X
f1db 95 b0    STA &b0,X
f1dd 68       PLA     
f1de 35 b0    AND &b0,X
f1e0 ca       DEX     
f1e1 10 f4    BPL &f1d7
f1e3 c9 ff    CMP #&ff
f1e5 d0 06    BNE &f1ed
f1e7 20 e8 fa JSR &fae8
f1ea 4c 67 e2 JMP &e267
f1ed ad ca 03 LDA &03ca
f1f0 4a       LSR A
f1f1 68       PLA     
f1f2 f0 0e    BEQ &f202
f1f4 90 13    BCC &f209
f1f6 20 f2 fa JSR &faf2
f1f9 00       BRK     
f1fa d5 4c    CMP &4c,X
f1fc 6f       ???
f1fd 63       ???
f1fe 6b       ???
f1ff 65 64    ADC &64
f201 00       BRK     
f202 90 05    BCC &f209
f204 a9 03    LDA #&03
f206 8d 58 02 STA &0258
f209 a9 30    LDA #&30
f20b 25 bb    AND &bb
f20d f0 04    BEQ &f213
f20f a5 c1    LDA &c1
f211 d0 0a    BNE &f21d
f213 98       TYA     
f214 48       PHA     
f215 20 bb fb JSR &fbbb
f218 68       PLA     
f219 a8       TAY     
f21a 20 d5 f7 JSR &f7d5
f21d 20 b4 f9 JSR &f9b4
f220 d0 33    BNE &f255
f222 20 69 fb JSR &fb69
f225 2c ca 03 BIT &03ca
f228 30 08    BMI &f232
f22a 20 6a f9 JSR &f96a
f22d 20 7b f7 JSR &f77b
f230 d0 d7    BNE &f209
f232 a0 0a    LDY #&0a
f234 a5 cc    LDA &cc
f236 91 c8    STA (&c8),Y
f238 c8       INY     
f239 a5 cd    LDA &cd
f23b 91 c8    STA (&c8),Y
f23d a9 00    LDA #&00
f23f c8       INY     
f240 91 c8    STA (&c8),Y
f242 c8       INY     
f243 91 c8    STA (&c8),Y
f245 28       PLP     
f246 20 e8 fa JSR &fae8
f249 24 ba    BIT &ba
f24b 30 07    BMI &f254
f24d 08       PHP     
f24e 20 46 fa JSR &fa46
f251 0d 00 28 ORA &2800
f254 60       RTS     
f255 20 37 f6 JSR &f637
f258 d0 af    BNE &f209
f25a 86 f2    STX &f2
f25c 84 f3    STY &f3
f25e a0 00    LDY #&00
f260 20 1d ea JSR &ea1d
f263 a2 00    LDX #&00
f265 20 2f ea JSR &ea2f
f268 b0 0d    BCS &f277
f26a f0 08    BEQ &f274
f26c 9d d2 03 STA &03d2,X
f26f e8       INX     
f270 e0 0b    CPX #&0b
f272 d0 f1    BNE &f265
f274 4c 8f ea JMP &ea8f
f277 a9 00    LDA #&00
f279 9d d2 03 STA &03d2,X
f27c 60       RTS     
f27d 48       PHA     
f27e 86 c8    STX &c8
f280 84 c9    STY &c9
f282 a0 00    LDY #&00
f284 b1 c8    LDA (&c8),Y
f286 aa       TAX     
f287 c8       INY     
f288 b1 c8    LDA (&c8),Y
f28a a8       TAY     
f28b 20 5a f2 JSR &f25a
f28e a0 02    LDY #&02
f290 b1 c8    LDA (&c8),Y
f292 99 bc 03 STA &03bc,Y
f295 99 ae 00 STA &00ae,Y
f298 c8       INY     
f299 c0 0a    CPY #&0a
f29b d0 f3    BNE &f290
f29d 68       PLA     
f29e f0 07    BEQ &f2a7
f2a0 c9 ff    CMP #&ff
f2a2 d0 b0    BNE &f254
f2a4 4c c4 f1 JMP &f1c4
f2a7 8d c6 03 STA &03c6
f2aa 8d c7 03 STA &03c7
f2ad b1 c8    LDA (&c8),Y
f2af 99 a6 00 STA &00a6,Y
f2b2 c8       INY     
f2b3 c0 12    CPY #&12
f2b5 d0 f6    BNE &f2ad
f2b7 8a       TXA     
f2b8 f0 ba    BEQ &f274
f2ba 20 27 fb JSR &fb27
f2bd 20 34 f9 JSR &f934
f2c0 a9 00    LDA #&00
f2c2 20 bd fb JSR &fbbd
f2c5 20 e2 fb JSR &fbe2
f2c8 38       SEC     
f2c9 a2 fd    LDX #&fd
f2cb bd b7 ff LDA &ffb7,X
f2ce fd b3 ff SBC &ffb3,X
f2d1 9d cb 02 STA &02cb,X
f2d4 e8       INX     
f2d5 d0 f4    BNE &f2cb
f2d7 a8       TAY     
f2d8 d0 0e    BNE &f2e8
f2da ec c8 03 CPX &03c8
f2dd a9 01    LDA #&01
f2df ed c9 03 SBC &03c9
f2e2 90 04    BCC &f2e8
f2e4 a2 80    LDX #&80
f2e6 d0 08    BNE &f2f0
f2e8 a9 01    LDA #&01
f2ea 8d c9 03 STA &03c9
f2ed 8e c8 03 STX &03c8
f2f0 8e ca 03 STX &03ca
f2f3 20 ec f7 JSR &f7ec
f2f6 30 49    BMI &f341
f2f8 20 6a f9 JSR &f96a
f2fb ee c6 03 INC &03c6
f2fe d0 c8    BNE &f2c8
f300 ee c7 03 INC &03c7
f303 d0 c3    BNE &f2c8
f305 20 5a f2 JSR &f25a
f308 a2 ff    LDX #&ff
f30a 8e c2 03 STX &03c2
f30d 20 c4 f1 JSR &f1c4
f310 2c 7a 02 BIT &027a
f313 10 0a    BPL &f31f
f315 ad c4 03 LDA &03c4
f318 2d c5 03 AND &03c5
f31b c9 ff    CMP #&ff
f31d d0 03    BNE &f322
f31f 6c c2 03 JMP (&03c2)
f322 a2 c2    LDX #&c2
f324 a0 03    LDY #&03
f326 a9 04    LDA #&04
f328 4c c7 fb JMP &fbc7
f32b a9 08    LDA #&08
f32d 20 44 f3 JSR &f344
f330 20 27 fb JSR &fb27
f333 a9 00    LDA #&00
f335 20 48 f3 JSR &f348
f338 20 fc fa JSR &fafc
f33b a9 f7    LDA #&f7
f33d 25 e2    AND &e2
f33f 85 e2    STA &e2
f341 60       RTS     
f342 a9 40    LDA #&40
f344 05 e2    ORA &e2
f346 d0 f7    BNE &f33f
f348 48       PHA     
f349 ad 47 02 LDA &0247
f34c f0 0b    BEQ &f359
f34e 20 13 ee JSR &ee13
f351 20 18 ee JSR &ee18
f354 90 03    BCC &f359
f356 b8       CLV     
f357 50 41    BVC &f39a
f359 20 7b f7 JSR &f77b
f35c ad c6 03 LDA &03c6
f35f 85 b4    STA &b4
f361 ad c7 03 LDA &03c7
f364 85 b5    STA &b5
f366 a2 ff    LDX #&ff
f368 8e df 03 STX &03df
f36b e8       INX     
f36c 86 ba    STX &ba
f36e f0 06    BEQ &f376
f370 20 69 fb JSR &fb69
f373 20 7b f7 JSR &f77b
f376 ad 47 02 LDA &0247
f379 f0 02    BEQ &f37d
f37b 50 1d    BVC &f39a
f37d 68       PLA     
f37e 48       PHA     
f37f f0 2d    BEQ &f3ae
f381 20 72 fa JSR &fa72
f384 d0 16    BNE &f39c
f386 a9 30    LDA #&30
f388 25 bb    AND &bb
f38a f0 0e    BEQ &f39a
f38c ad c6 03 LDA &03c6
f38f c5 b6    CMP &b6
f391 d0 09    BNE &f39c
f393 ad c7 03 LDA &03c7
f396 c5 b7    CMP &b7
f398 d0 02    BNE &f39c
f39a 68       PLA     
f39b 60       RTS     
f39c ad 47 02 LDA &0247
f39f f0 0d    BEQ &f3ae
f3a1 20 ad ee JSR &eead
f3a4 a9 ff    LDA #&ff
f3a6 8d c6 03 STA &03c6
f3a9 8d c7 03 STA &03c7
f3ac d0 c2    BNE &f370
f3ae 50 05    BVC &f3b5
f3b0 a9 ff    LDA #&ff
f3b2 20 d7 f7 JSR &f7d7
f3b5 a2 00    LDX #&00
f3b7 20 d9 f9 JSR &f9d9
f3ba ad 47 02 LDA &0247
f3bd f0 04    BEQ &f3c3
f3bf 24 bb    BIT &bb
f3c1 50 de    BVC &f3a1
f3c3 2c ca 03 BIT &03ca
f3c6 30 dc    BMI &f3a4
f3c8 10 a6    BPL &f370
f3ca 85 bc    STA &bc
f3cc 8a       TXA     
f3cd 48       PHA     
f3ce 98       TYA     
f3cf 48       PHA     
f3d0 a5 bc    LDA &bc
f3d2 d0 1e    BNE &f3f2
f3d4 98       TYA     
f3d5 d0 0c    BNE &f3e3
f3d7 20 75 e2 JSR &e275
f3da 20 78 f4 JSR &f478
f3dd 46 e2    LSR &e2
f3df 06 e2    ASL &e2
f3e1 90 0c    BCC &f3ef
f3e3 4a       LSR A
f3e4 b0 f7    BCS &f3dd
f3e6 4a       LSR A
f3e7 b0 03    BCS &f3ec
f3e9 4c b1 fb JMP &fbb1
f3ec 20 78 f4 JSR &f478
f3ef 4c 71 f4 JMP &f471
f3f2 20 5a f2 JSR &f25a
f3f5 24 bc    BIT &bc
f3f7 50 3d    BVC &f436
f3f9 a9 00    LDA #&00
f3fb 8d 9e 03 STA &039e
f3fe 8d dd 03 STA &03dd
f401 8d de 03 STA &03de
f404 a9 3e    LDA #&3e
f406 20 3d f3 JSR &f33d
f409 20 1a fb JSR &fb1a
f40c 08       PHP     
f40d 20 31 f6 JSR &f631
f410 20 b4 f6 JSR &f6b4
f413 28       PLP     
f414 a2 ff    LDX #&ff
f416 e8       INX     
f417 bd b2 03 LDA &03b2,X
f41a 9d a7 03 STA &03a7,X
f41d d0 f7    BNE &f416
f41f a9 01    LDA #&01
f421 20 44 f3 JSR &f344
f424 ad ea 02 LDA &02ea
f427 0d eb 02 ORA &02eb
f42a d0 03    BNE &f42f
f42c 20 42 f3 JSR &f342
f42f a9 01    LDA #&01
f431 0d 47 02 ORA &0247
f434 d0 39    BNE &f46f
f436 8a       TXA     
f437 d0 03    BNE &f43c
f439 4c 8f ea JMP &ea8f
f43c a2 ff    LDX #&ff
f43e e8       INX     
f43f bd d2 03 LDA &03d2,X
f442 9d 80 03 STA &0380,X
f445 d0 f7    BNE &f43e
f447 a9 ff    LDA #&ff
f449 a2 08    LDX #&08
f44b 9d 8b 03 STA &038b,X
f44e ca       DEX     
f44f d0 fa    BNE &f44b
f451 8a       TXA     
f452 a2 14    LDX #&14
f454 9d 80 03 STA &0380,X
f457 e8       INX     
f458 e0 1e    CPX #&1e
f45a d0 f8    BNE &f454
f45c 2e 97 03 ROL &0397
f45f 20 27 fb JSR &fb27
f462 20 34 f9 JSR &f934
f465 20 f2 fa JSR &faf2
f468 a9 02    LDA #&02
f46a 20 44 f3 JSR &f344
f46d a9 02    LDA #&02
f46f 85 bc    STA &bc
f471 68       PLA     
f472 a8       TAY     
f473 68       PLA     
f474 aa       TAX     
f475 a5 bc    LDA &bc
f477 60       RTS     
f478 a9 02    LDA #&02
f47a 25 e2    AND &e2
f47c f0 f9    BEQ &f477
f47e a9 00    LDA #&00
f480 8d 97 03 STA &0397
f483 a9 80    LDA #&80
f485 ae 9d 03 LDX &039d
f488 8e 96 03 STX &0396
f48b 8d 98 03 STA &0398
f48e 20 96 f4 JSR &f496
f491 a9 fd    LDA #&fd
f493 4c 3d f3 JMP &f33d
f496 20 1a fb JSR &fb1a
f499 a2 11    LDX #&11
f49b bd 8c 03 LDA &038c,X
f49e 9d be 03 STA &03be,X
f4a1 ca       DEX     
f4a2 10 f7    BPL &f49b
f4a4 86 b2    STX &b2
f4a6 86 b3    STX &b3
f4a8 e8       INX     
f4a9 86 b0    STX &b0
f4ab a9 09    LDA #&09
f4ad 85 b1    STA &b1
f4af a2 7f    LDX #&7f
f4b1 20 81 fb JSR &fb81
f4b4 8d df 03 STA &03df
f4b7 20 8e fb JSR &fb8e
f4ba 20 e2 fb JSR &fbe2
f4bd 20 ec f7 JSR &f7ec
f4c0 ee 94 03 INC &0394
f4c3 d0 03    BNE &f4c8
f4c5 ee 95 03 INC &0395
f4c8 60       RTS     
f4c9 8a       TXA     
f4ca 48       PHA     
f4cb 98       TYA     
f4cc 48       PHA     
f4cd a9 01    LDA #&01
f4cf 20 9c fb JSR &fb9c
f4d2 a5 e2    LDA &e2
f4d4 0a       ASL A
f4d5 b0 4c    BCS &f523
f4d7 0a       ASL A
f4d8 90 09    BCC &f4e3
f4da a9 80    LDA #&80
f4dc 20 44 f3 JSR &f344
f4df a9 fe    LDA #&fe
f4e1 b0 38    BCS &f51b
f4e3 ae 9e 03 LDX &039e
f4e6 e8       INX     
f4e7 ec ea 02 CPX &02ea
f4ea d0 2a    BNE &f516
f4ec 2c ec 02 BIT &02ec
f4ef 30 22    BMI &f513
f4f1 ad ed 02 LDA &02ed
f4f4 48       PHA     
f4f5 20 1a fb JSR &fb1a
f4f8 08       PHP     
f4f9 20 ac f6 JSR &f6ac
f4fc 28       PLP     
f4fd 68       PLA     
f4fe 85 bc    STA &bc
f500 18       CLC     
f501 2c ec 02 BIT &02ec
f504 10 17    BPL &f51d
f506 ad ea 02 LDA &02ea
f509 0d eb 02 ORA &02eb
f50c d0 0f    BNE &f51d
f50e 20 42 f3 JSR &f342
f511 d0 0a    BNE &f51d
f513 20 42 f3 JSR &f342
f516 ca       DEX     
f517 18       CLC     
f518 bd 00 0a LDA &0a00,X
f51b 85 bc    STA &bc
f51d ee 9e 03 INC &039e
f520 4c 71 f4 JMP &f471
f523 00       BRK     
f524 df       ???
f525 45 4f    EOR &4f
f527 46 00    LSR &00
f529 85 c4    STA &c4
f52b 8a       TXA     
f52c 48       PHA     
f52d 98       TYA     
f52e 48       PHA     
f52f a9 02    LDA #&02
f531 20 9c fb JSR &fb9c
f534 ae 9d 03 LDX &039d
f537 a5 c4    LDA &c4
f539 9d 00 09 STA &0900,X
f53c e8       INX     
f53d d0 06    BNE &f545
f53f 20 96 f4 JSR &f496
f542 20 f2 fa JSR &faf2
f545 ee 9d 03 INC &039d
f548 a5 c4    LDA &c4
f54a 4c 6f f4 JMP &f46f
f54d 8a       TXA     
f54e f0 2e    BEQ &f57e
f550 e0 03    CPX #&03
f552 f0 1f    BEQ &f573
f554 c0 03    CPY #&03
f556 b0 06    BCS &f55e
f558 ca       DEX     
f559 f0 06    BEQ &f561
f55b ca       DEX     
f55c f0 0a    BEQ &f568
f55e 4c 10 e3 JMP &e310
f561 a9 33    LDA #&33
f563 c8       INY     
f564 c8       INY     
f565 c8       INY     
f566 d0 02    BNE &f56a
f568 a9 cc    LDA #&cc
f56a c8       INY     
f56b 25 e3    AND &e3
f56d 19 81 f5 ORA &f581,Y
f570 85 e3    STA &e3
f572 60       RTS     
f573 98       TYA     
f574 30 02    BMI &f578
f576 d0 02    BNE &f57a
f578 a9 19    LDA #&19
f57a 8d d1 03 STA &03d1
f57d 60       RTS     
f57e a8       TAY     
f57f f0 ec    BEQ &f56d
f581 a1 00    LDA (&00;X)
f583 22       ???
f584 11 00    ORA (&00),Y
f586 88       DEY     
f587 cc c6 c0 CPY &c0c6
f58a ad 47 02 LDA &0247
f58d f0 07    BEQ &f596
f58f 20 51 ee JSR &ee51
f592 a8       TAY     
f593 18       CLC     
f594 90 1a    BCC &f5b0
f596 ad 08 fe LDA &fe08
f599 48       PHA     
f59a 29 02    AND #&02
f59c f0 0b    BEQ &f5a9
f59e a4 ca    LDY &ca
f5a0 f0 07    BEQ &f5a9
f5a2 68       PLA     
f5a3 a5 bd    LDA &bd
f5a5 8d 09 fe STA &fe09
f5a8 60       RTS     
f5a9 ac 09 fe LDY &fe09
f5ac 68       PLA     
f5ad 4a       LSR A
f5ae 4a       LSR A
f5af 4a       LSR A
f5b0 a6 c2    LDX &c2
f5b2 f0 69    BEQ &f61d
f5b4 ca       DEX     
f5b5 d0 06    BNE &f5bd
f5b7 90 64    BCC &f61d
f5b9 a0 02    LDY #&02
f5bb d0 5e    BNE &f61b
f5bd ca       DEX     
f5be d0 13    BNE &f5d3
f5c0 b0 5b    BCS &f61d
f5c2 98       TYA     
f5c3 20 78 fb JSR &fb78
f5c6 a0 03    LDY #&03
f5c8 c9 2a    CMP #&2a
f5ca f0 4f    BEQ &f61b
f5cc 20 50 fb JSR &fb50
f5cf a0 01    LDY #&01
f5d1 d0 48    BNE &f61b
f5d3 ca       DEX     
f5d4 d0 0c    BNE &f5e2
f5d6 b0 04    BCS &f5dc
f5d8 84 bd    STY &bd
f5da f0 41    BEQ &f61d
f5dc a9 80    LDA #&80
f5de 85 c0    STA &c0
f5e0 d0 3b    BNE &f61d
f5e2 ca       DEX     
f5e3 d0 29    BNE &f60e
f5e5 b0 2f    BCS &f616
f5e7 98       TYA     
f5e8 20 b0 f7 JSR &f7b0
f5eb a4 bc    LDY &bc
f5ed e6 bc    INC &bc
f5ef 24 bd    BIT &bd
f5f1 30 0d    BMI &f600
f5f3 20 d3 fb JSR &fbd3
f5f6 f0 05    BEQ &f5fd
f5f8 8e e5 fe STX &fee5
f5fb d0 03    BNE &f600
f5fd 8a       TXA     
f5fe 91 b0    STA (&b0),Y
f600 c8       INY     
f601 cc c8 03 CPY &03c8
f604 d0 17    BNE &f61d
f606 a9 01    LDA #&01
f608 85 bc    STA &bc
f60a a0 05    LDY #&05
f60c d0 0d    BNE &f61b
f60e 98       TYA     
f60f 20 b0 f7 JSR &f7b0
f612 c6 bc    DEC &bc
f614 10 07    BPL &f61d
f616 20 46 fb JSR &fb46
f619 a0 00    LDY #&00
f61b 84 c2    STY &c2
f61d 60       RTS     
f61e 48       PHA     
f61f 98       TYA     
f620 48       PHA     
f621 8a       TXA     
f622 a8       TAY     
f623 a9 03    LDA #&03
f625 20 9c fb JSR &fb9c
f628 a5 e2    LDA &e2
f62a 29 40    AND #&40
f62c aa       TAX     
f62d 68       PLA     
f62e a8       TAY     
f62f 68       PLA     
f630 60       RTS     
f631 a9 00    LDA #&00
f633 85 b4    STA &b4
f635 85 b5    STA &b5
f637 a5 b4    LDA &b4
f639 48       PHA     
f63a 85 b6    STA &b6
f63c a5 b5    LDA &b5
f63e 48       PHA     
f63f 85 b7    STA &b7
f641 20 46 fa JSR &fa46
f644 53       ???
f645 65 61    ADC &61
f647 72       ???
f648 63       ???
f649 68       PLA     
f64a 69 6e    ADC #&6e
f64c 67       ???
f64d 0d 00 a9 ORA &a900
f650 ff       ???
f651 20 48 f3 JSR &f348
f654 68       PLA     
f655 85 b5    STA &b5
f657 68       PLA     
f658 85 b4    STA &b4
f65a a5 b6    LDA &b6
f65c 05 b7    ORA &b7
f65e d0 0d    BNE &f66d
f660 85 b4    STA &b4
f662 85 b5    STA &b5
f664 a5 c1    LDA &c1
f666 d0 05    BNE &f66d
f668 a2 b1    LDX #&b1
f66a 20 81 fb JSR &fb81
f66d ad 47 02 LDA &0247
f670 f0 13    BEQ &f685
f672 70 11    BVS &f685
f674 00       BRK     
f675 d6 46    DEC &46,X
f677 69 6c    ADC #&6c
f679 65 20    ADC &20
f67b 6e 6f 74 ROR &746f
f67e 20 66 6f JSR &6f66
f681 75 6e    ADC &6e,X
f683 64       ???
f684 00       BRK     
f685 a0 ff    LDY #&ff
f687 8c df 03 STY &03df
f68a 60       RTS     
f68b a9 00    LDA #&00
f68d 08       PHP     
f68e 84 e6    STY &e6
f690 ac 56 02 LDY &0256
f693 8d 56 02 STA &0256
f696 f0 03    BEQ &f69b
f698 20 ce ff JSR &ffce
f69b a4 e6    LDY &e6
f69d 28       PLP     
f69e f0 0b    BEQ &f6ab
f6a0 a9 40    LDA #&40
f6a2 20 ce ff JSR &ffce
f6a5 a8       TAY     
f6a6 f0 cc    BEQ &f674
f6a8 8d 56 02 STA &0256
f6ab 60       RTS     
f6ac a2 a6    LDX #&a6
f6ae 20 81 fb JSR &fb81
f6b1 20 7b f7 JSR &f77b
f6b4 ad ca 03 LDA &03ca
f6b7 4a       LSR A
f6b8 90 03    BCC &f6bd
f6ba 4c f6 f1 JMP &f1f6
f6bd ad dd 03 LDA &03dd
f6c0 85 b4    STA &b4
f6c2 ad de 03 LDA &03de
f6c5 85 b5    STA &b5
f6c7 a9 00    LDA #&00
f6c9 85 b0    STA &b0
f6cb a9 0a    LDA #&0a
f6cd 85 b1    STA &b1
f6cf a9 ff    LDA #&ff
f6d1 85 b2    STA &b2
f6d3 85 b3    STA &b3
f6d5 20 d5 f7 JSR &f7d5
f6d8 20 b4 f9 JSR &f9b4
f6db d0 25    BNE &f702
f6dd ad ff 0a LDA &0aff
f6e0 8d ed 02 STA &02ed
f6e3 20 69 fb JSR &fb69
f6e6 8e dd 03 STX &03dd
f6e9 8c de 03 STY &03de
f6ec a2 02    LDX #&02
f6ee bd c8 03 LDA &03c8,X
f6f1 9d ea 02 STA &02ea,X
f6f4 ca       DEX     
f6f5 10 f7    BPL &f6ee
f6f7 2c ec 02 BIT &02ec
f6fa 10 03    BPL &f6ff
f6fc 20 49 f2 JSR &f249
f6ff 4c f2 fa JMP &faf2
f702 20 37 f6 JSR &f637
f705 d0 ad    BNE &f6b4
f707 c9 2a    CMP #&2a
f709 f0 37    BEQ &f742
f70b c9 23    CMP #&23
f70d d0 0f    BNE &f71e
f70f ee c6 03 INC &03c6
f712 d0 03    BNE &f717
f714 ee c7 03 INC &03c7
f717 a2 ff    LDX #&ff
f719 2c b7 d9 BIT &d9b7
f71c d0 55    BNE &f773
f71e a9 f7    LDA #&f7
f720 20 3d f3 JSR &f33d
f723 00       BRK     
f724 d7       ???
f725 42       ???
f726 61 64    ADC (&64;X)
f728 20 52 4f JSR &4f52
f72b 4d 00 a0 EOR &a000
f72e ff       ???
f72f 20 90 fb JSR &fb90
f732 a9 01    LDA #&01
f734 85 c2    STA &c2
f736 20 50 fb JSR &fb50
f739 20 95 f9 JSR &f995
f73c a9 03    LDA #&03
f73e c5 c2    CMP &c2
f740 d0 f7    BNE &f739
f742 a0 00    LDY #&00
f744 20 7c fb JSR &fb7c
f747 20 97 f7 JSR &f797
f74a 50 1a    BVC &f766
f74c 99 b2 03 STA &03b2,Y
f74f f0 06    BEQ &f757
f751 c8       INY     
f752 c0 0b    CPY #&0b
f754 d0 f1    BNE &f747
f756 88       DEY     
f757 a2 0c    LDX #&0c
f759 20 97 f7 JSR &f797
f75c 50 08    BVC &f766
f75e 9d b2 03 STA &03b2,X
f761 e8       INX     
f762 e0 1f    CPX #&1f
f764 d0 f3    BNE &f759
f766 98       TYA     
f767 aa       TAX     
f768 a9 00    LDA #&00
f76a 99 b2 03 STA &03b2,Y
f76d a5 be    LDA &be
f76f 05 bf    ORA &bf
f771 85 c1    STA &c1
f773 20 78 fb JSR &fb78
f776 84 c2    STY &c2
f778 8a       TXA     
f779 d0 59    BNE &f7d4
f77b ad 47 02 LDA &0247
f77e f0 ad    BEQ &f72d
f780 20 51 ee JSR &ee51
f783 c9 2b    CMP #&2b
f785 d0 80    BNE &f707
f787 a9 08    LDA #&08
f789 25 e2    AND &e2
f78b f0 03    BEQ &f790
f78d 20 4d f2 JSR &f24d
f790 20 18 ee JSR &ee18
f793 90 eb    BCC &f780
f795 b8       CLV     
f796 60       RTS     
f797 ad 47 02 LDA &0247
f79a f0 11    BEQ &f7ad
f79c 8a       TXA     
f79d 48       PHA     
f79e 98       TYA     
f79f 48       PHA     
f7a0 20 51 ee JSR &ee51
f7a3 85 bd    STA &bd
f7a5 a9 ff    LDA #&ff
f7a7 85 c0    STA &c0
f7a9 68       PLA     
f7aa a8       TAY     
f7ab 68       PLA     
f7ac aa       TAX     
f7ad 20 84 f8 JSR &f884
f7b0 08       PHP     
f7b1 48       PHA     
f7b2 38       SEC     
f7b3 66 cb    ROR &cb
f7b5 45 bf    EOR &bf
f7b7 85 bf    STA &bf
f7b9 a5 bf    LDA &bf
f7bb 2a       ROL A
f7bc 90 0c    BCC &f7ca
f7be 6a       ROR A
f7bf 49 08    EOR #&08
f7c1 85 bf    STA &bf
f7c3 a5 be    LDA &be
f7c5 49 10    EOR #&10
f7c7 85 be    STA &be
f7c9 38       SEC     
f7ca 26 be    ROL &be
f7cc 26 bf    ROL &bf
f7ce 46 cb    LSR &cb
f7d0 d0 e7    BNE &f7b9
f7d2 68       PLA     
f7d3 28       PLP     
f7d4 60       RTS     
f7d5 a9 00    LDA #&00
f7d7 85 bd    STA &bd
f7d9 a2 00    LDX #&00
f7db 86 bc    STX &bc
f7dd 50 0a    BVC &f7e9
f7df ad c8 03 LDA &03c8
f7e2 0d c9 03 ORA &03c9
f7e5 f0 02    BEQ &f7e9
f7e7 a2 04    LDX #&04
f7e9 86 c2    STX &c2
f7eb 60       RTS     
f7ec 08       PHP     
f7ed a2 03    LDX #&03
f7ef a9 00    LDA #&00
f7f1 9d cb 03 STA &03cb,X
f7f4 ca       DEX     
f7f5 10 fa    BPL &f7f1
f7f7 ad c6 03 LDA &03c6
f7fa 0d c7 03 ORA &03c7
f7fd d0 05    BNE &f804
f7ff 20 92 f8 JSR &f892
f802 f0 03    BEQ &f807
f804 20 96 f8 JSR &f896
f807 a9 2a    LDA #&2a
f809 85 bd    STA &bd
f80b 20 78 fb JSR &fb78
f80e 20 4a fb JSR &fb4a
f811 20 84 f8 JSR &f884
f814 88       DEY     
f815 c8       INY     
f816 b9 d2 03 LDA &03d2,Y
f819 99 b2 03 STA &03b2,Y
f81c 20 75 f8 JSR &f875
f81f d0 f4    BNE &f815
f821 a2 0c    LDX #&0c
f823 bd b2 03 LDA &03b2,X
f826 20 75 f8 JSR &f875
f829 e8       INX     
f82a e0 1d    CPX #&1d
f82c d0 f5    BNE &f823
f82e 20 7b f8 JSR &f87b
f831 ad c8 03 LDA &03c8
f834 0d c9 03 ORA &03c9
f837 f0 1c    BEQ &f855
f839 a0 00    LDY #&00
f83b 20 7c fb JSR &fb7c
f83e b1 b0    LDA (&b0),Y
f840 20 d3 fb JSR &fbd3
f843 f0 03    BEQ &f848
f845 ae e5 fe LDX &fee5
f848 8a       TXA     
f849 20 75 f8 JSR &f875
f84c c8       INY     
f84d cc c8 03 CPY &03c8
f850 d0 ec    BNE &f83e
f852 20 7b f8 JSR &f87b
f855 20 84 f8 JSR &f884
f858 20 84 f8 JSR &f884
f85b 20 46 fb JSR &fb46
f85e a9 01    LDA #&01
f860 20 98 f8 JSR &f898
f863 28       PLP     
f864 20 b9 f8 JSR &f8b9
f867 2c ca 03 BIT &03ca
f86a 10 08    BPL &f874
f86c 08       PHP     
f86d 20 92 f8 JSR &f892
f870 20 46 f2 JSR &f246
f873 28       PLP     
f874 60       RTS     
f875 20 82 f8 JSR &f882
f878 4c b0 f7 JMP &f7b0
f87b a5 bf    LDA &bf
f87d 20 82 f8 JSR &f882
f880 a5 be    LDA &be
f882 85 bd    STA &bd
f884 20 95 f9 JSR &f995
f887 24 c0    BIT &c0
f889 10 f9    BPL &f884
f88b a9 00    LDA #&00
f88d 85 c0    STA &c0
f88f a5 bd    LDA &bd
f891 60       RTS     
f892 a9 32    LDA #&32
f894 d0 02    BNE &f898
f896 a5 c7    LDA &c7
f898 a2 05    LDX #&05
f89a 8d 40 02 STA &0240
f89d 20 95 f9 JSR &f995
f8a0 2c 40 02 BIT &0240
f8a3 10 f8    BPL &f89d
f8a5 ca       DEX     
f8a6 d0 f2    BNE &f89a
f8a8 60       RTS     
f8a9 ad c6 03 LDA &03c6
f8ac 0d c7 03 ORA &03c7
f8af f0 05    BEQ &f8b6
f8b1 2c df 03 BIT &03df
f8b4 10 03    BPL &f8b9
f8b6 20 49 f2 JSR &f249
f8b9 a0 00    LDY #&00
f8bb 84 ba    STY &ba
f8bd ad ca 03 LDA &03ca
f8c0 8d df 03 STA &03df
f8c3 20 dc e7 JSR &e7dc
f8c6 f0 6b    BEQ &f933
f8c8 a9 0d    LDA #&0d
f8ca 20 ee ff JSR &ffee
f8cd b9 b2 03 LDA &03b2,Y
f8d0 f0 10    BEQ &f8e2
f8d2 c9 20    CMP #&20
f8d4 90 04    BCC &f8da
f8d6 c9 7f    CMP #&7f
f8d8 90 02    BCC &f8dc
f8da a9 3f    LDA #&3f
f8dc 20 ee ff JSR &ffee
f8df c8       INY     
f8e0 d0 eb    BNE &f8cd
f8e2 ad 47 02 LDA &0247
f8e5 f0 04    BEQ &f8eb
f8e7 24 bb    BIT &bb
f8e9 50 48    BVC &f933
f8eb 20 91 f9 JSR &f991
f8ee c8       INY     
f8ef c0 0b    CPY #&0b
f8f1 90 ef    BCC &f8e2
f8f3 ad c6 03 LDA &03c6
f8f6 aa       TAX     
f8f7 20 7a f9 JSR &f97a
f8fa 2c ca 03 BIT &03ca
f8fd 10 34    BPL &f933
f8ff 8a       TXA     
f900 18       CLC     
f901 6d c9 03 ADC &03c9
f904 85 cd    STA &cd
f906 20 75 f9 JSR &f975
f909 ad c8 03 LDA &03c8
f90c 85 cc    STA &cc
f90e 20 7a f9 JSR &f97a
f911 24 bb    BIT &bb
f913 50 1e    BVC &f933
f915 a2 04    LDX #&04
f917 20 91 f9 JSR &f991
f91a ca       DEX     
f91b d0 fa    BNE &f917
f91d a2 0f    LDX #&0f
f91f 20 27 f9 JSR &f927
f922 20 91 f9 JSR &f991
f925 a2 13    LDX #&13
f927 a0 04    LDY #&04
f929 bd b2 03 LDA &03b2,X
f92c 20 7a f9 JSR &f97a
f92f ca       DEX     
f930 88       DEY     
f931 d0 f6    BNE &f929
f933 60       RTS     
f934 ad 47 02 LDA &0247
f937 f0 03    BEQ &f93c
f939 4c 10 e3 JMP &e310
f93c 20 8e fb JSR &fb8e
f93f 20 e2 fb JSR &fbe2
f942 20 dc e7 JSR &e7dc
f945 f0 ec    BEQ &f933
f947 20 46 fa JSR &fa46
f94a 52       ???
f94b 45 43    EOR &43
f94d 4f       ???
f94e 52       ???
f94f 44       ???
f950 20 74 68 JSR &6874
f953 65 6e    ADC &6e
f955 20 52 45 JSR &4552
f958 54       ???
f959 55 52    EOR &52,X
f95b 4e 00 20 LSR &2000
f95e 95 f9    STA &f9,X
f960 20 e0 ff JSR &ffe0
f963 c9 0d    CMP #&0d
f965 d0 f6    BNE &f95d
f967 4c e7 ff JMP &ffe7
f96a e6 b1    INC &b1
f96c d0 06    BNE &f974
f96e e6 b2    INC &b2
f970 d0 02    BNE &f974
f972 e6 b3    INC &b3
f974 60       RTS     
f975 48       PHA     
f976 20 91 f9 JSR &f991
f979 68       PLA     
f97a 48       PHA     
f97b 4a       LSR A
f97c 4a       LSR A
f97d 4a       LSR A
f97e 4a       LSR A
f97f 20 83 f9 JSR &f983
f982 68       PLA     
f983 18       CLC     
f984 29 0f    AND #&0f
f986 69 30    ADC #&30
f988 c9 3a    CMP #&3a
f98a 90 02    BCC &f98e
f98c 69 06    ADC #&06
f98e 4c ee ff JMP &ffee
f991 a9 20    LDA #&20
f993 d0 f9    BNE &f98e
f995 08       PHP     
f996 24 eb    BIT &eb
f998 30 04    BMI &f99e
f99a 24 ff    BIT &ff
f99c 30 02    BMI &f9a0
f99e 28       PLP     
f99f 60       RTS     
f9a0 20 3b f3 JSR &f33b
f9a3 20 f2 fa JSR &faf2
f9a6 a9 7e    LDA #&7e
f9a8 20 f4 ff JSR &fff4
f9ab 00       BRK     
f9ac 11 45    ORA (&45),Y
f9ae 73       ???
f9af 63       ???
f9b0 61 70    ADC (&70;X)
f9b2 65 00    ADC &00
f9b4 98       TYA     
f9b5 f0 0d    BEQ &f9c4
f9b7 20 46 fa JSR &fa46
f9ba 0d 4c 6f ORA &6f4c
f9bd 61 64    ADC (&64;X)
f9bf 69 6e    ADC #&6e
f9c1 67       ???
f9c2 0d 00 85 ORA &8500
f9c5 ba       TSX     
f9c6 a2 ff    LDX #&ff
f9c8 a5 c1    LDA &c1
f9ca d0 0d    BNE &f9d9
f9cc 20 72 fa JSR &fa72
f9cf 08       PHP     
f9d0 a2 ff    LDX #&ff
f9d2 a0 99    LDY #&99
f9d4 a9 fa    LDA #&fa
f9d6 28       PLP     
f9d7 d0 1c    BNE &f9f5
f9d9 a0 8e    LDY #&8e
f9db a5 c1    LDA &c1
f9dd f0 04    BEQ &f9e3
f9df a9 fa    LDA #&fa
f9e1 d0 12    BNE &f9f5
f9e3 ad c6 03 LDA &03c6
f9e6 c5 b4    CMP &b4
f9e8 d0 07    BNE &f9f1
f9ea ad c7 03 LDA &03c7
f9ed c5 b5    CMP &b5
f9ef f0 13    BEQ &fa04
f9f1 a0 a4    LDY #&a4
f9f3 a9 fa    LDA #&fa
f9f5 48       PHA     
f9f6 98       TYA     
f9f7 48       PHA     
f9f8 8a       TXA     
f9f9 48       PHA     
f9fa 20 b6 f8 JSR &f8b6
f9fd 68       PLA     
f9fe aa       TAX     
f9ff 68       PLA     
fa00 a8       TAY     
fa01 68       PLA     
fa02 d0 14    BNE &fa18
fa04 8a       TXA     
fa05 48       PHA     
fa06 20 a9 f8 JSR &f8a9
fa09 20 d6 fa JSR &fad6
fa0c 68       PLA     
fa0d aa       TAX     
fa0e a5 be    LDA &be
fa10 05 bf    ORA &bf
fa12 f0 79    BEQ &fa8d
fa14 a0 8e    LDY #&8e
fa16 a9 fa    LDA #&fa
fa18 c6 ba    DEC &ba
fa1a 48       PHA     
fa1b 24 eb    BIT &eb
fa1d 30 0d    BMI &fa2c
fa1f 8a       TXA     
fa20 2d 47 02 AND &0247
fa23 d0 07    BNE &fa2c
fa25 8a       TXA     
fa26 29 11    AND #&11
fa28 25 bb    AND &bb
fa2a f0 10    BEQ &fa3c
fa2c 68       PLA     
fa2d 85 b9    STA &b9
fa2f 84 b8    STY &b8
fa31 20 8b f6 JSR &f68b
fa34 46 eb    LSR &eb
fa36 20 e8 fa JSR &fae8
fa39 6c b8 00 JMP (&00b8)
fa3c 68       PLA     
fa3d c8       INY     
fa3e d0 03    BNE &fa43
fa40 18       CLC     
fa41 69 01    ADC #&01
fa43 48       PHA     
fa44 98       TYA     
fa45 48       PHA     
fa46 20 dc e7 JSR &e7dc
fa49 a8       TAY     
fa4a 68       PLA     
fa4b 85 b8    STA &b8
fa4d 68       PLA     
fa4e 85 b9    STA &b9
fa50 98       TYA     
fa51 08       PHP     
fa52 e6 b8    INC &b8
fa54 d0 02    BNE &fa58
fa56 e6 b9    INC &b9
fa58 a0 00    LDY #&00
fa5a b1 b8    LDA (&b8),Y
fa5c f0 0a    BEQ &fa68
fa5e 28       PLP     
fa5f 08       PHP     
fa60 f0 f0    BEQ &fa52
fa62 20 e3 ff JSR &ffe3
fa65 4c 52 fa JMP &fa52
fa68 28       PLP     
fa69 e6 b8    INC &b8
fa6b d0 02    BNE &fa6f
fa6d e6 b9    INC &b9
fa6f 6c b8 00 JMP (&00b8)
fa72 a2 ff    LDX #&ff
fa74 e8       INX     
fa75 bd d2 03 LDA &03d2,X
fa78 d0 07    BNE &fa81
fa7a 8a       TXA     
fa7b f0 03    BEQ &fa80
fa7d bd b2 03 LDA &03b2,X
fa80 60       RTS     
fa81 20 e3 e4 JSR &e4e3
fa84 5d b2 03 EOR &03b2,X
fa87 b0 02    BCS &fa8b
fa89 29 df    AND #&df
fa8b f0 e7    BEQ &fa74
fa8d 60       RTS     
fa8e 00       BRK     
fa8f d8       CLD     
fa90 0d 44 61 ORA &6144
fa93 74       ???
fa94 61 3f    ADC (&3f;X)
fa96 00       BRK     
fa97 d0 15    BNE &faae
fa99 00       BRK     
fa9a db       ???
fa9b 0d 46 69 ORA &6946
fa9e 6c 65 3f JMP (&3f65)
faa1 00       BRK     
faa2 d0 0a    BNE &faae
faa4 00       BRK     
faa5 da       ???
faa6 0d 42 6c ORA &6c42
faa9 6f       ???
faaa 63       ???
faab 6b       ???
faac 3f       ???
faad 00       BRK     
faae a5 ba    LDA &ba
fab0 f0 21    BEQ &fad3
fab2 8a       TXA     
fab3 f0 1e    BEQ &fad3
fab5 a9 22    LDA #&22
fab7 24 bb    BIT &bb
fab9 f0 18    BEQ &fad3
fabb 20 46 fb JSR &fb46
fabe a8       TAY     
fabf 20 4a fa JSR &fa4a
fac2 0d 07 52 ORA &5207
fac5 65 77    ADC &77
fac7 69 6e    ADC #&6e
fac9 64       ???
faca 20 74 61 JSR &6174
facd 70 65    BVS &fb34
facf 0d 0d 00 ORA &000d
fad2 60       RTS     
fad3 20 4d f2 JSR &f24d
fad6 a5 c2    LDA &c2
fad8 f0 f8    BEQ &fad2
fada 20 95 f9 JSR &f995
fadd ad 47 02 LDA &0247
fae0 f0 f4    BEQ &fad6
fae2 20 88 f5 JSR &f588
fae5 4c d6 fa JMP &fad6
fae8 20 dc e7 JSR &e7dc
faeb f0 05    BEQ &faf2
faed a9 07    LDA #&07
faef 20 ee ff JSR &ffee
faf2 a9 80    LDA #&80
faf4 20 bd fb JSR &fbbd
faf7 a2 00    LDX #&00
faf9 20 95 fb JSR &fb95
fafc 08       PHP     
fafd 78       SEI     
fafe ad 82 02 LDA &0282
fb01 8d 10 fe STA &fe10
fb04 a9 00    LDA #&00
fb06 85 ea    STA &ea
fb08 f0 01    BEQ &fb0b
fb0a 08       PHP     
fb0b 20 46 fb JSR &fb46
fb0e ad 50 02 LDA &0250
fb11 4c 89 e1 JMP &e189
fb14 28       PLP     
fb15 24 ff    BIT &ff
fb17 10 18    BPL &fb31
fb19 60       RTS     
fb1a a5 e3    LDA &e3
fb1c 0a       ASL A
fb1d 0a       ASL A
fb1e 0a       ASL A
fb1f 0a       ASL A
fb20 85 bb    STA &bb
fb22 ad d1 03 LDA &03d1
fb25 d0 08    BNE &fb2f
fb27 a5 e3    LDA &e3
fb29 29 f0    AND #&f0
fb2b 85 bb    STA &bb
fb2d a9 06    LDA #&06
fb2f 85 c7    STA &c7
fb31 58       CLI     
fb32 08       PHP     
fb33 78       SEI     
fb34 2c 4f 02 BIT &024f
fb37 10 db    BPL &fb14
fb39 a5 ea    LDA &ea
fb3b 30 d7    BMI &fb14
fb3d a9 01    LDA #&01
fb3f 85 ea    STA &ea
fb41 20 46 fb JSR &fb46
fb44 28       PLP     
fb45 60       RTS     
fb46 a9 03    LDA #&03
fb48 d0 1b    BNE &fb65
fb4a a9 30    LDA #&30
fb4c 85 ca    STA &ca
fb4e d0 13    BNE &fb63
fb50 a9 05    LDA #&05
fb52 8d 10 fe STA &fe10
fb55 a2 ff    LDX #&ff
fb57 ca       DEX     
fb58 d0 fd    BNE &fb57
fb5a 86 ca    STX &ca
fb5c a9 85    LDA #&85
fb5e 8d 10 fe STA &fe10
fb61 a9 d0    LDA #&d0
fb63 05 c6    ORA &c6
fb65 8d 08 fe STA &fe08
fb68 60       RTS     
fb69 ae c6 03 LDX &03c6
fb6c ac c7 03 LDY &03c7
fb6f e8       INX     
fb70 86 b4    STX &b4
fb72 d0 01    BNE &fb75
fb74 c8       INY     
fb75 84 b5    STY &b5
fb77 60       RTS     
fb78 a0 00    LDY #&00
fb7a 84 c0    STY &c0
fb7c 84 be    STY &be
fb7e 84 bf    STY &bf
fb80 60       RTS     
fb81 a0 ff    LDY #&ff
fb83 c8       INY     
fb84 e8       INX     
fb85 bd 00 03 LDA &0300,X
fb88 99 d2 03 STA &03d2,Y
fb8b d0 f6    BNE &fb83
fb8d 60       RTS     
fb8e a0 00    LDY #&00
fb90 58       CLI     
fb91 a2 01    LDX #&01
fb93 84 c3    STY &c3
fb95 a9 89    LDA #&89
fb97 a4 c3    LDY &c3
fb99 4c f4 ff JMP &fff4
fb9c 85 bc    STA &bc
fb9e 98       TYA     
fb9f 4d 47 02 EOR &0247
fba2 a8       TAY     
fba3 a5 e2    LDA &e2
fba5 25 bc    AND &bc
fba7 4a       LSR A
fba8 88       DEY     
fba9 f0 04    BEQ &fbaf
fbab 4a       LSR A
fbac 88       DEY     
fbad d0 02    BNE &fbb1
fbaf b0 4d    BCS &fbfe
fbb1 00       BRK     
fbb2 de 43 68 DEC &6843,X
fbb5 61 6e    ADC (&6e;X)
fbb7 6e 65 6c ROR &6c65
fbba 00       BRK     
fbbb a9 01    LDA #&01
fbbd 20 d3 fb JSR &fbd3
fbc0 f0 3c    BEQ &fbfe
fbc2 8a       TXA     
fbc3 a2 b0    LDX #&b0
fbc5 a0 00    LDY #&00
fbc7 48       PHA     
fbc8 a9 c0    LDA #&c0
fbca 20 06 04 JSR &0406
fbcd 90 fb    BCC &fbca
fbcf 68       PLA     
fbd0 4c 06 04 JMP &0406
fbd3 aa       TAX     
fbd4 a5 b2    LDA &b2
fbd6 25 b3    AND &b3
fbd8 c9 ff    CMP #&ff
fbda f0 05    BEQ &fbe1
fbdc ad 7a 02 LDA &027a
fbdf 29 80    AND #&80
fbe1 60       RTS     
fbe2 a9 85    LDA #&85
fbe4 8d 10 fe STA &fe10
fbe7 20 46 fb JSR &fb46
fbea a9 10    LDA #&10
fbec 20 63 fb JSR &fb63
fbef 20 95 f9 JSR &f995
fbf2 ad 08 fe LDA &fe08
fbf5 29 02    AND #&02
fbf7 f0 f6    BEQ &fbef
fbf9 a9 aa    LDA #&aa
fbfb 8d 09 fe STA &fe09
fbfe 60       RTS     
fbff 00       BRK  
   
fc00 28       PLP     
fc01 43       ???
fc02 29 20    AND #&20
fc04 31 39    AND (&39),Y
fc06 38       SEC     
fc07 31 20    AND (&20),Y
fc09 41 63    EOR (&63;X)
fc0b 6f       ???
fc0c 72       ???
fc0d 6e 20 43 ROR &4320
fc10 6f       ???
fc11 6d 70 75 ADC &7570
fc14 74       ???
fc15 65 72    ADC &72
fc17 73       ???
fc18 20 4c 74 JSR &744c
fc1b 64       ???
fc1c 2e 54 68 ROL &6854
fc1f 61 6e    ADC (&6e;X)
fc21 6b       ???
fc22 73       ???
fc23 20 61 72 JSR &7261
fc26 65 20    ADC &20
fc28 64       ???
fc29 75 65    ADC &65,X
fc2b 20 74 6f JSR &6f74
fc2e 20 74 68 JSR &6874
fc31 65 20    ADC &20
fc33 66 6f    ROR &6f
fc35 6c 6c 6f JMP (&6f6c)
fc38 77       ???
fc39 69 6e    ADC #&6e
fc3b 67       ???
fc3c 20 63 6f JSR &6f63
fc3f 6e 74 72 ROR &7274
fc42 69 62    ADC #&62
fc44 75 74    ADC &74,X
fc46 6f       ???
fc47 72       ???
fc48 73       ???
fc49 20 74 6f JSR &6f74
fc4c 20 74 68 JSR &6874
fc4f 65 20    ADC &20
fc51 64       ???
fc52 65 76    ADC &76
fc54 65 6c    ADC &6c
fc56 6f       ???
fc57 70 6d    BVS &fcc6
fc59 65 6e    ADC &6e
fc5b 74       ???
fc5c 20 6f 66 JSR &666f
fc5f 20 74 68 JSR &6874
fc62 65 20    ADC &20
fc64 42       ???
fc65 42       ???
fc66 43       ???
fc67 20 43 6f JSR &6f43
fc6a 6d 70 75 ADC &7570
fc6d 74       ???
fc6e 65 72    ADC &72
fc70 20 28 61 JSR &6128
fc73 6d 6f 6e ADC &6e6f
fc76 67       ???
fc77 20 6f 74 JSR &746f
fc7a 68       PLA     
fc7b 65 72    ADC &72
fc7d 73       ???
fc7e 20 74 6f JSR &6f74
fc81 6f       ???
fc82 20 6e 75 JSR &756e
fc85 6d 65 72 ADC &7265
fc88 6f       ???
fc89 75 73    ADC &73,X
fc8b 20 74 6f JSR &6f74
fc8e 20 6d 65 JSR &656d
fc91 6e 74 69 ROR &6974
fc94 6f       ???
fc95 6e 29 3a ROR &3a29
fc98 2d 20 44 AND &4420
fc9b 61 76    ADC (&76;X)
fc9d 69 64    ADC #&64
fc9f 20 41 6c JSR &6c41
fca2 6c 65 6e JMP (&6e65)
fca5 2c 42 6f BIT &6f42
fca8 62       ???
fca9 20 41 75 JSR &7541
fcac 73       ???
fcad 74       ???
fcae 69 6e    ADC #&6e
fcb0 2c 52 61 BIT &6152
fcb3 6d 20 42 ADC &4220
fcb6 61 6e    ADC (&6e;X)
fcb8 65 72    ADC &72
fcba 6a       ROR A
fcbb 65 65    ADC &65
fcbd 2c 50 61 BIT &6150
fcc0 75 6c    ADC &6c,X
fcc2 20 42 6f JSR &6f42
fcc5 6e 64 2c ROR &2c64
fcc8 41 6c    EOR (&6c;X)
fcca 6c 65 6e JMP (&6e65)
fccd 20 42 6f JSR &6f42
fcd0 6f       ???
fcd1 74       ???
fcd2 68       PLA     
fcd3 72       ???
fcd4 6f       ???
fcd5 79 64 2c ADC &2c64,Y
fcd8 43       ???
fcd9 61 6d    ADC (&6d;X)
fcdb 62       ???
fcdc 72       ???
fcdd 69 64    ADC #&64
fcdf 67       ???
fce0 65 2c    ADC &2c
fce2 43       ???
fce3 6c 65 61 JMP (&6165)
fce6 72       ???
fce7 74       ???
fce8 6f       ???
fce9 6e 65 2c ROR &2c65
fcec 4a       LSR A
fced 6f       ???
fcee 68       PLA     
fcef 6e 20 43 ROR &4320
fcf2 6f       ???
fcf3 6c 6c 2c JMP (&2c6c)
fcf6 4a       LSR A
fcf7 6f       ???
fcf8 68       PLA     
fcf9 6e 20 43 ROR &4320
fcfc 6f       ???
fcfd 78       SEI     
fcfe 2c 41 6e BIT &6e41
fd01 64       ???
fd02 79 20 43 ADC &4320,Y
fd05 72       ???
fd06 69 70    ADC #&70
fd08 70 73    BVS &fd7d
fd0a 2c 43 68 BIT &6843
fd0d 72       ???
fd0e 69 73    ADC #&73
fd10 20 43 75 JSR &7543
fd13 72       ???
fd14 72       ???
fd15 79 2c 36 ADC &362c,Y
fd18 35 30    AND &30,X
fd1a 32       ???
fd1b 20 64 65 JSR &6564
fd1e 73       ???
fd1f 69 67    ADC #&67
fd21 6e 65 72 ROR &7265
fd24 73       ???
fd25 2c 4a 65 BIT &654a
fd28 72       ???
fd29 65 6d    ADC &6d
fd2b 79 20 44 ADC &4420,Y
fd2e 69 6f    ADC #&6f
fd30 6e 2c 54 ROR &542c
fd33 69 6d    ADC #&6d
fd35 20 44 6f JSR &6f44
fd38 62       ???
fd39 73       ???
fd3a 6f       ???
fd3b 6e 2c 4a ROR &4a2c
fd3e 6f       ???
fd3f 65 20    ADC &20
fd41 44       ???
fd42 75 6e    ADC &6e,X
fd44 6e 2c 50 ROR &502c
fd47 61 75    ADC (&75;X)
fd49 6c 20 46 JMP (&4620)
fd4c 61 72    ADC (&72;X)
fd4e 72       ???
fd4f 65 6c    ADC &6c
fd51 6c 2c 46 JMP (&462c)
fd54 65 72    ADC &72
fd56 72       ???
fd57 61 6e    ADC (&6e;X)
fd59 74       ???
fd5a 69 2c    ADC #&2c
fd5c 53       ???
fd5d 74       ???
fd5e 65 76    ADC &76
fd60 65 20    ADC &20
fd62 46 75    LSR &75
fd64 72       ???
fd65 62       ???
fd66 65 72    ADC &72
fd68 2c 4a 6f BIT &6f4a
fd6b 6e 20 47 ROR &4720
fd6e 69 62    ADC #&62
fd70 62       ???
fd71 6f       ???
fd72 6e 73 2c ROR &2c73
fd75 41 6e    EOR (&6e;X)
fd77 64       ???
fd78 72       ???
fd79 65 77    ADC &77
fd7b 20 47 6f JSR &6f47
fd7e 72       ???
fd7f 64       ???
fd80 6f       ???
fd81 6e 2c 4c ROR &4c2c
fd84 61 77    ADC (&77;X)
fd86 72       ???
fd87 65 6e    ADC &6e
fd89 63       ???
fd8a 65 20    ADC &20
fd8c 48       PHA     
fd8d 61 72    ADC (&72;X)
fd8f 64       ???
fd90 77       ???
fd91 69 63    ADC #&63
fd93 6b       ???
fd94 2c 44 79 BIT &7944
fd97 6c 61 6e JMP (&6e61)
fd9a 20 48 61 JSR &6148
fd9d 72       ???
fd9e 72       ???
fd9f 69 73    ADC #&73
fda1 2c 48 65 BIT &6548
fda4 72       ???
fda5 6d 61 6e ADC &6e61
fda8 6e 20 48 ROR &4820
fdab 61 75    ADC (&75;X)
fdad 73       ???
fdae 65 72    ADC &72
fdb0 2c 48 69 BIT &6948
fdb3 74       ???
fdb4 61 63    ADC (&63;X)
fdb6 68       PLA     
fdb7 69 2c    ADC #&2c
fdb9 41 6e    EOR (&6e;X)
fdbb 64       ???
fdbc 79 20 48 ADC &4820,Y
fdbf 6f       ???
fdc0 70 70    BVS &fe32
fdc2 65 72    ADC &72
fdc4 2c 49 43 BIT &4349
fdc7 4c 2c 4d JMP &4d2c
fdca 61 72    ADC (&72;X)
fdcc 74       ???
fdcd 69 6e    ADC #&6e
fdcf 20 4a 61 JSR &614a
fdd2 63       ???
fdd3 6b       ???
fdd4 73       ???
fdd5 6f       ???
fdd6 6e 2c 42 ROR &422c
fdd9 72       ???
fdda 69 61    ADC #&61
fddc 6e 20 4a ROR &4a20
fddf 6f       ???
fde0 6e 65 73 ROR &7365
fde3 2c 43 68 BIT &6843
fde6 72       ???
fde7 69 73    ADC #&73
fde9 20 4a 6f JSR &6f4a
fdec 72       ???
fded 64       ???
fdee 61 6e    ADC (&6e;X)
fdf0 2c 44 61 BIT &6144
fdf3 76 69    ROR &69,X
fdf5 64       ???
fdf6 20 4b 69 JSR &694b
fdf9 6e 67 2c ROR &2c67
fdfc 44       ???
fdfd 61 76    ADC (&76;X)
fdff 69 64    ADC #&64
fe01 20 4b 69 JSR &694b
fe04 74       ???
fe05 73       ???
fe06 6f       ???
fe07 6e 2c 50 ROR &502c
fe0a 61 75    ADC (&75;X)
fe0c 6c 20 4b JMP (&4b20)
fe0f 72       ???
fe10 69 77    ADC #&77
fe12 61 63    ADC (&63;X)
fe14 7a       ???
fe15 65 6b    ADC &6b
fe17 2c 43 6f BIT &6f43
fe1a 6d 70 75 ADC &7570
fe1d 74       ???
fe1e 65 72    ADC &72
fe20 20 4c 61 JSR &614c
fe23 62       ???
fe24 6f       ???
fe25 72       ???
fe26 61 74    ADC (&74;X)
fe28 6f       ???
fe29 72       ???
fe2a 79 2c 50 ADC &502c,Y
fe2d 65 74    ADC &74
fe2f 65 72    ADC &72
fe31 20 4d 69 JSR &694d
fe34 6c 6c 65 JMP (&656c)
fe37 72       ???
fe38 2c 41 72 BIT &7241
fe3b 74       ???
fe3c 68       PLA     
fe3d 75 72    ADC &72,X
fe3f 20 4e 6f JSR &6f4e
fe42 72       ???
fe43 6d 61 6e ADC &6e61
fe46 2c 47 6c BIT &6c47
fe49 79 6e 20 ADC &206e,Y
fe4c 50 68    BVC &feb6
fe4e 69 6c    ADC #&6c
fe50 6c 69 70 JMP (&7069)
fe53 73       ???
fe54 2c 4d 69 BIT &694d
fe57 6b       ???
fe58 65 20    ADC &20
fe5a 50 72    BVC &fece
fe5c 65 65    ADC &65
fe5e 73       ???
fe5f 2c 4a 6f BIT &6f4a
fe62 68       PLA     
fe63 6e 20 52 ROR &5220
fe66 61 64    ADC (&64;X)
fe68 63       ???
fe69 6c 69 66 JMP (&6669)
fe6c 66 65    ROR &65
fe6e 2c 57 69 BIT &6957
fe71 6c 62 65 JMP (&6562)
fe74 72       ???
fe75 66 6f    ROR &6f
fe77 72       ???
fe78 63       ???
fe79 65 20    ADC &20
fe7b 52       ???
fe7c 6f       ???
fe7d 61 64    ADC (&64;X)
fe7f 2c 50 65 BIT &6550
fe82 74       ???
fe83 65 72    ADC &72
fe85 20 52 6f JSR &6f52
fe88 62       ???
fe89 69 6e    ADC #&6e
fe8b 73       ???
fe8c 6f       ???
fe8d 6e 2c 52 ROR &522c
fe90 69 63    ADC #&63
fe92 68       PLA     
fe93 61 72    ADC (&72;X)
fe95 64       ???
fe96 20 52 75 JSR &7552
fe99 73       ???
fe9a 73       ???
fe9b 65 6c    ADC &6c
fe9d 6c 2c 4b JMP (&4b2c)
fea0 69 6d    ADC #&6d
fea2 20 53 70 JSR &7053
fea5 65 6e    ADC &6e
fea7 63       ???
fea8 65 2d    ADC &2d
feaa 4a       LSR A
feab 6f       ???
feac 6e 65 73 ROR &7365
feaf 2c 47 72 BIT &7247
feb2 61 68    ADC (&68;X)
feb4 61 6d    ADC (&6d;X)
feb6 20 54 65 JSR &6554
feb9 62       ???
feba 62       ???
febb 79 2c 4a ADC &4a2c,Y
febe 6f       ???
febf 6e 20 54 ROR &5420
fec2 68       PLA     
fec3 61 63    ADC (&63;X)
fec5 6b       ???
fec6 72       ???
fec7 61 79    ADC (&79;X)
fec9 2c 43 68 BIT &6843
fecc 72       ???
fecd 69 73    ADC #&73
fecf 20 54 75 JSR &7554
fed2 72       ???
fed3 6e 65 72 ROR &7265
fed6 2c 41 64 BIT &6441
fed9 72       ???
feda 69 61    ADC #&61
fedc 6e 20 57 ROR &5720
fedf 61 72    ADC (&72;X)
fee1 6e 65 72 ROR &7265
fee4 2c 52 6f BIT &6f52
fee7 67       ???
fee8 65 72    ADC &72
feea 20 57 69 JSR &6957
feed 6c 73 6f JMP (&6f73)
fef0 6e 2c 41 ROR &412c
fef3 6c 61 6e JMP (&6e61)
fef6 20 57 72 JSR &7257
fef9 69 67    ADC #&67
fefb 68       PLA     
fefc 74       ???
fefd 2e cd d9 ROL &d9cd


ff00 20 51 ff JSR &ff51
ff03 20 51 ff JSR &ff51
ff06 20 51 ff JSR &ff51
ff09 20 51 ff JSR &ff51
ff0c 20 51 ff JSR &ff51
ff0f 20 51 ff JSR &ff51
ff12 20 51 ff JSR &ff51
ff15 20 51 ff JSR &ff51
ff18 20 51 ff JSR &ff51
ff1b 20 51 ff JSR &ff51
ff1e 20 51 ff JSR &ff51
ff21 20 51 ff JSR &ff51
ff24 20 51 ff JSR &ff51
ff27 20 51 ff JSR &ff51
ff2a 20 51 ff JSR &ff51
ff2d 20 51 ff JSR &ff51
ff30 20 51 ff JSR &ff51
ff33 20 51 ff JSR &ff51
ff36 20 51 ff JSR &ff51
ff39 20 51 ff JSR &ff51
ff3c 20 51 ff JSR &ff51
ff3f 20 51 ff JSR &ff51
ff42 20 51 ff JSR &ff51
ff45 20 51 ff JSR &ff51
ff48 20 51 ff JSR &ff51
ff4b 20 51 ff JSR &ff51
ff4e 20 51 ff JSR &ff51
ff51 48       PHA     
ff52 48       PHA     
ff53 48       PHA     
ff54 48       PHA     
ff55 48       PHA     
ff56 08       PHP     
ff57 48       PHA     
ff58 8a       TXA     
ff59 48       PHA     
ff5a 98       TYA     
ff5b 48       PHA     
ff5c ba       TSX     
ff5d a9 ff    LDA #&ff
ff5f 9d 08 01 STA &0108,X
ff62 a9 88    LDA #&88
ff64 9d 07 01 STA &0107,X
ff67 bc 0a 01 LDY &010a,X
ff6a b9 9d 0d LDA &0d9d,Y
ff6d 9d 05 01 STA &0105,X
ff70 b9 9e 0d LDA &0d9e,Y
ff73 9d 06 01 STA &0106,X
ff76 a5 f4    LDA &f4
ff78 9d 09 01 STA &0109,X
ff7b b9 9f 0d LDA &0d9f,Y
ff7e 85 f4    STA &f4
ff80 8d 30 fe STA &fe30
ff83 68       PLA     
ff84 a8       TAY     
ff85 68       PLA     
ff86 aa       TAX     
ff87 68       PLA     
ff88 40       RTI     
ff89 08       PHP     
ff8a 48       PHA     
ff8b 8a       TXA     
ff8c 48       PHA     
ff8d ba       TSX     
ff8e bd 02 01 LDA &0102,X
ff91 9d 05 01 STA &0105,X
ff94 bd 03 01 LDA &0103,X
ff97 9d 06 01 STA &0106,X
ff9a 68       PLA     
ff9b aa       TAX     
ff9c 68       PLA     
ff9d 68       PLA     
ff9e 68       PLA     
ff9f 85 f4    STA &f4
ffa1 8d 30 fe STA &fe30
ffa4 68       PLA     
ffa5 28       PLP     
ffa6 60       RTS     
ffa7 8a       TXA     
ffa8 b0 2a    BCS &ffd4
ffaa bc 00 fc LDY &fc00,X
ffad 60       RTS     
ffae bc 00 fd LDY &fd00,X
ffb1 60       RTS     
ffb2 bc 00 fe LDY &fe00,X
ffb5 60       RTS     
ffb6 36 40    ROL &40,X
ffb8 d9 4c 0b CMP &0b4c,Y
ffbb dc       ???
ffbc 4c c0 c4 JMP &c4c0
ffbf 4c 94 e4 JMP &e494
ffc2 4c 1e ea JMP &ea1e
ffc5 4c 2f ea JMP &ea2f
ffc8 4c c5 de JMP &dec5
ffcb 4c a4 e0 JMP &e0a4
ffce 6c 1c 02 JMP (&021c)
ffd1 6c 1a 02 JMP (&021a)
ffd4 6c 18 02 JMP (&0218)
ffd7 6c 16 02 JMP (&0216)
ffda 6c 14 02 JMP (&0214)
ffdd 6c 12 02 JMP (&0212)
ffe0 6c 10 02 JMP (&0210)
ffe3 c9 0d    CMP #&0d
ffe5 d0 07    BNE &ffee
ffe7 a9 0a    LDA #&0a
ffe9 20 ee ff JSR &ffee
ffec a9 0d    LDA #&0d
ffee 6c 0e 02 JMP (&020e)
fff1 6c 0c 02 JMP (&020c)
fff4 6c 0a 02 JMP (&020a)
fff7 6c 08 02 JMP (&0208)
fffa 00       BRK     
fffb 0d cd d9 ORA &d9cd
fffe 1c       ???
ffff dc       ???
0000 dc       ???
