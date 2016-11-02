;Descriptor info
;	DA_	: Descriptor Attribute
;	D	: Data Segement
;	C	: Code Segement
;	S	: System Segement
;	R	: Read-only
;	RW	; Read Write
;	A	; Avaliable

DA_32		EQU	4000h	; 32 bit segement
DA_LIMIT_4K	EQU	8000h	; limit 4K
DA_DPL0		EQU	00h	; DPL = 0
DA_DPL1		EQU	20h	; DPL = 1
DA_DPL2		EQU	40h	; DPL = 2
DA_DPL3		EQU	60h	; DPL = 3

DA_DR		EQU	90h	; Read-only
DA_DRW		EQU	92h	; Read Write
DA_DRWA		EQU	93h	; Read Write found
DA_C		EQU	98h	; Execute
DA_CR		EQU	9Ah	; Execute Read
DA_CCO		EQU	9Ch	; Execute 1
DA_CCOR		EQU	9Eh	; Execute Read 1

DA_LDT		EQU	82h	; Local Descriptor
DA_TaskGate	EQU	85h	; Task Gate
DA_386TSS	EQU	89h	; 386 Task State
DA_386CGate	EQU	8Ch	; 386 C Gate 
DA_386IGate	EQU	8Eh	; 386 Intrrupte Gate
DA_386TGate	EQU	8Fh	; 386 Task Gate

;
;SA_	: Selector Attribute
;

SA_RPL0		EQU	0;
SA_RPL1		EQU	1;
SA_RPL2		EQU	2;
SA_RPL3		EQU	3

SA_TIG		EQU	0;
SA_TIL		EQU	4;

; page
PG_P		EQU	1; Page Present
PG_RWR		EQU	0; R/W Write/Execute
PG_RWW		EQU	2; R/W Write/Read/Execute
PG_USS		EQU	0; U/S	System
PG_USU		EQU	4; U/S  User

; usage: Descriptor Base, Limit, Attr
;	Base:	dd
;	Limit:	dd (low 20 bits available)
;	Attr:	dw(lower 4 bit of higher bytes are always 0)
%macro Descriptor 3
	dw	%2 & 0FFFFh
	dw	%1 & 0FFFFh
	db	(%1 >> 16) & 0FFh
	dw	((%2 >> 8) & 0F00h) | (%3 & 0F0FFh) 
	db	(%1 >> 24) & 0FFh
%endmacro ;


;usage: Gate Selector, Offset, DCount, Attr
;	Seletor:	dw
;	Offset:		dd
;	DCount:		db
;	Attr:		db
%macro Gate 4
	dw	(%2 & 0FFFFh)
	dw	%1
	dw	(%3 & 1Fh) | ((%4 << 8) & 0FF00h)
	dw	((%2 >> 16) & 0FFFFh)
%endmacro ;


