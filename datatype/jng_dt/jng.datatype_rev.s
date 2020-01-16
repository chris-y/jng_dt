VERSION = 1
REVISION = 3

.macro DATE
.ascii "12.3.2009"
.endm

.macro VERS
.ascii "jng.datatype 1.3"
.endm

.macro VSTRING
.ascii "jng.datatype 1.3 (12.3.2009)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: jng.datatype 1.3 (12.3.2009)"
.byte 0
.endm
