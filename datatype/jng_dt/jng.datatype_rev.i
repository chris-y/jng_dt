VERSION		EQU	1
REVISION	EQU	3

DATE	MACRO
		dc.b '12.3.2009'
		ENDM

VERS	MACRO
		dc.b 'jng.datatype 1.3'
		ENDM

VSTRING	MACRO
		dc.b 'jng.datatype 1.3 (12.3.2009)',13,10,0
		ENDM

VERSTAG	MACRO
		dc.b 0,'$VER: jng.datatype 1.3 (12.3.2009)',0
		ENDM
