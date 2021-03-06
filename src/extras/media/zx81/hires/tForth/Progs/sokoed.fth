( SokoED                     )
( The SokoACE map editor     )

( [c] 2006  by Ricardo Fernandes Lopes )
( under the GNU General Public License )

( ------------ How to Use it -------------)
( Load the SokoACE map editor in the EO   )
( emulator.
(                                         )
( Create a new map 123 from scratch using:)
(  123 NEW                                )
(                                         )
( or load an existing map to edit with:   )
( "File/Load Memory Block" to address     )
( 32768, in EightyOne emulator. Edit map  )
( 8 using:                                )
(  8 MEM>BUF ED
(                                         )
( When finished, quit editor with Q,      )
( transfer the map from buffer to memory  )
( with:                                   )
(  8 BUF>MEM                              )
( then save maps using:                   )
(  "File/Save Memory Block"               )
( ----------------------------------------)

: TASK ;

\ Set print position to row n1 and column n2
: AT  ( n1 n2 -- )
   SWAP 33 * + 16530 ( screen)
   + 17339 ( cur_pos) ! ;
\ Leave greater of two numbers
: MAX  ( n1 n2 -- n3)
   2DUP <
   IF SWAP
   THEN
   DROP ;
\ Leave lesser of two numbers
: MIN  ( n1 n2 -- n3)
   2DUP >
   IF SWAP
   THEN
   DROP ;
: MOD /MOD DROP ;

\ Move n bytes from src to dst
CODE CMOVE ( src dst n -- ) HEX
  E1 C,         ( POP HL      ; 'dst'                   )
  EB C,         ( EX DE,HL    ; DE = 'dst', HL = IP     )
  E3 C,         ( EX [SP],HL  ; HL = 'src', IP on stack )
  ED C, B0 C,   ( LDIR        ; move 'n' bytes          )
  D1 C,         ( POP DE      ; restore IP              )
  C1 C,         ( POP BC      ; new TOS                 )
  NEXT          ( jp NEXT                               )
DECIMAL

( Graphic characters )
CREATE T 56 ALLOT
: GR 8 * T + DUP 8 + SWAP DO I C! LOOP ;
HEX
00 00 00 00 00 00 00 00 00 GR ( Not used     )
00 00 00 18 18 00 00 00 01 GR ( Target       )
3C 42 81 FF 99 99 7E 3C 02 GR ( Soko         )
3C 42 BD FF BD 99 7E 3C 03 GR ( Soko + Target)
00 7E 42 42 42 42 7E 00 04 GR ( Box          )
00 7E 42 5A 5A 42 7E 00 05 GR ( Box + Target )
FF AB D5 AB D5 AB D5 FF 06 GR ( Wall         )
DECIMAL
: SETGR 56 0 DO T I + C@ 12288 I + C! LOOP ;

( Screen elements)
01 CONSTANT TARGET
02 CONSTANT SOKO
04 CONSTANT BOX
06 CONSTANT WALL
32 CONSTANT BL

\ : +! ( n adr -- )  SWAP OVER @ + SWAP ! ;

    1 VARIABLE MAP#           ( Map number     )
16871 VARIABLE CURSOR         ( Cursor address )

( Movement directions)
-33 CONSTANT UP
 33 CONSTANT DOWN
 -1 CONSTANT LEFT
  1 CONSTANT RIGHT

( Displaying cursor)
: CURSOR-ON   CURSOR @ DUP C@ 128  OR SWAP C! ;
: CURSOR-OFF  CURSOR @ DUP C@ 127 AND SWAP C! ;

: GO ( dir -- , move cursor to given direction)
  CURSOR @ SWAP OVER +          ( Calc new cursor position)
  DUP 16530 - 33 MOD
  20 < IF SWAP THEN DROP        ( horizontal limit)
  16596 MAX 17242 MIN           ( vertical limit )
  CURSOR ! ;

: WALK ( c -- c , Move cursor )
  DUP ASCII I = IF    UP GO ELSE
  DUP ASCII K = IF  DOWN GO ELSE
  DUP ASCII J = IF  LEFT GO ELSE
  DUP ASCII L = IF RIGHT GO
  THEN THEN THEN THEN ;

( Place a character at cursor position )
: SET ( c -- )   CURSOR @ C! ;

: DRAW ( c -- c )
  DUP ASCII X = IF             0 SET ELSE
  DUP ASCII T = IF        TARGET SET ELSE
  DUP ASCII B = IF           BOX SET ELSE
  DUP ASCII O = IF  BOX TARGET + SET ELSE
  DUP ASCII S = IF          SOKO SET ELSE
  DUP ASCII Z = IF SOKO TARGET + SET ELSE
  DUP ASCII W = IF          WALL SET
  THEN THEN THEN THEN THEN THEN THEN ;

: .MAP#  1 0 AT ." MAP " MAP# @ . SPACE ;

: .FRAME
   0  0 AT ." �sokoace�map�editor� VERSION 1.0"
   2 21 AT ." i UP"
   3 21 AT ." k DOWN"
   4 21 AT ." j LEFT"
   5 21 AT ." l RIGHT"
   7 21 AT ." s SOKO"
   8 21 AT ." z SOKO+TRGT"
   9 21 AT ." b BOX"
  10 21 AT ." o BOX+TRGT"
  11 21 AT ." t TARGET"
  12 21 AT ." w WALL"
  13 21 AT ." x CLR CELL"
  15 21 AT ." c CLR MAP"
  16 21 AT ." n MAP + 1"
  17 21 AT ." p MAP - 1"
  18 21 AT ." m MAP ?"
  20 21 AT ." q QUIT"
  22  0 AT ." �by�ricardo�f�lopes�   (C) 2006"
  .MAP# ;

: #IN ( -- n , get a number from the user)
   BEGIN
    TIB @ DUP LBP ! INPUT
    BL WORD COUNT NUMBER
   UNTIL DROP
;

: PLAY ( c -- )
  WALK DRAW
  DUP ASCII C = IF  CLS .FRAME        ELSE
  DUP ASCII N = IF    1 MAP# +! .MAP# ELSE
  DUP ASCII P = IF   -1 MAP# +! .MAP# ELSE
  DUP ASCII M = IF  #IN MAP# !  .MAP#
  THEN THEN THEN THEN DROP ;

21 20 * CONSTANT MAPSIZE
CREATE BUF MAPSIZE ALLOT

: MAP ( -- adr n )   BUF MAPSIZE ;

( Copy Map on screen to buffer for saving)
: SCR>BUF
  16563 BUF ( src dst )
  21 0      ( 21 lines )
  DO
    20 0    ( 20 chars per line )
    DO
      OVER I + @ OVER I + !
      2
    +LOOP
    20 + SWAP
    33 + SWAP
  LOOP ;

( Copy Map on buffer to screen for editing)
: S-TYPE  ( work only for this proposit, d'nt check NEWLINE at the end of line)
  OVER + SWAP DO I C@ 191 AND 17339 @ C!
  1 17339 +! LOOP ;
: BUF>SCR
  1 0 AT
  BUF 21 0
  DO DUP 20 S-TYPE CR 20 + LOOP DROP ;

: ED ( Edit map on screen)
  CLS SETGR
  BUF>SCR .FRAME
  16870 CURSOR !
  BEGIN
    CURSOR-ON KEY CURSOR-OFF
    DUP ASCII Q = 0=
  WHILE
    PLAY
  REPEAT 
  DROP SCR>BUF  ( save screen to buffer for saving )
  CLS ABORT ;

( create a new map )
: NEW ( n -- , Start new map n)
  MAP# ! CLS .MAP# SCR>BUF ED ;

( Copy Map on buffer to memory slot)
: BUF>MEM ( n -- )
  MAPSIZE * 32768 +      ( destination )
  BUF SWAP MAPSIZE       ( origin and count )
  CMOVE
;

: MEM>BUF ( n -- )
  MAPSIZE * 32768 +      ( origin )
  BUF MAPSIZE            ( destination and count )
  CMOVE
;
 