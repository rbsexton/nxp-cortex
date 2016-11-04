\ Access to the interconnect things.
\ Its got to match the C side.

$10000000 equ ICROOT

struct /INTER	\ -- size
	int inter.ticks 
end-struct

: ticks icroot inter.ticks @ ; 

