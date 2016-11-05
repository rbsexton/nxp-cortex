\ Access to the interconnect things.
\ Its got to match the C side.

udata
create  ICROOT 4 allot
cdata

struct /INTER	\ -- size
	int inter.ticks 
end-struct

: ticks icroot inter.ticks @ ; 

