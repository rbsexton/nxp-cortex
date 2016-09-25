\ serLE_p.fth - Polled Driver for the NXP LPC11Uxx Cortex-M0
\ Assumes that the hardware is already setup by the launcher.
\

\ ********************
\ *S Serial primitives
\ ********************

$00 equ UART_RXDATA
$00 equ UART_TXDATA

$14 equ UART_STATUS \ LSR
bit0 equ UART_STATUS_BYTEREADY \ RDR
bit5 equ UART_STATUS_HASROOM   \ THRE

internal

: +FaultConsole	( -- )  ;
\ *G Because this is a polled driver, *\fo{+FaultConsole} for
\ ** fault handling is a *\fo{NOOP}. See *\i{Cortex/FaultCortex.fth}
\ ** for more details.

: (seremit)	\ char base --
\ *G Transmit a character on the given UART.
  begin
    dup UART_STATUS + @ UART_STATUS_HASROOM and 		\ Tx FIFO full test
  until
  UART_TXDATA + !
;

: (sertype)	\ caddr len base --
\ *G Transmit a string on the given UART.
  -rot bounds
  ?do  i c@ over (seremit)  loop
  drop
;

: (sercr)	\ base --
\ *G Transmit a CR/LF pair on the given UART.
  $0D over (seremit)  $0A swap (seremit)
;

: (serkey?)	\ base -- t/f
\ *G Return true if the given UART has a character avilable to read.
  UART_STATUS + @ UART_STATUS_BYTEREADY and \ Rx FIFO empty test
;

: (serkey)	\ base -- char
\ *G Wait for a character to come available on the given UART and
\ ** return the character.
  begin
[ tasking? ] [if]  pause  [then]
    dup (serkey?) 
  until
  UART_RXDATA + @ $FF and
;

: initUART	\ divisor22 base --
  drop drop
;

external

\ ********
\ *S UART0
\ ********

useUART0? [if]

: init-ser0	; 

: seremit0	\ char --
\ *G Transmit a character on UART0.
  _UART0 (seremit)  ;
: sertype0	\ c-addr len --
\ *G Transmit a string on UART0.
  _UART0 (sertype)  ;
: sercr0	\ --
\ *G Issue a CR/LF pair to UART0.
  _UART0 (sercr)  ;
: serkey?0	\ -- flag
\ *G Return true if UART0 has a character available.
  _UART0 (serkey?)  ;
: serkey0	\ -- char
\ *G Wait for a character on UART0.
  _UART0 (serkey)  ;
create Console0	\ -- addr ; OUT managed by upper driver
\ *G Generic I/O device for UART0.
  ' serkey0 ,		\ -- char ; receive char
  ' serkey?0 ,		\ -- flag ; check receive char
  ' seremit0 ,		\ -- char ; display char
  ' sertype0 ,		\ caddr len -- ; display string
  ' sercr0 ,		\ -- ; display new line
console-port 0 = [if]
  console0 constant console
\ *G *\fo{CONSOLE} is the device used by the Forth system for interaction.
\ ** It may be changed by one of the phrases of the form:
\ *C   <device>  dup opvec !  ipvec !
\ *C   <device>  SetConsole
[then]

[then]


\ ************************
\ *S System initialisation
\ ************************

: init-ser	\ --
\ *G Initialise all serial ports
  [ useUART0? ] [if]  init-ser0  [then]
;


\ ======
\ *> ###
\ ======

decimal
