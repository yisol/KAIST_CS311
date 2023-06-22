	.data
data1:	.word	100
data2:	.word	200
data3:	.word	0x12345678
	.word	0x12341
	.text
main:
	subu $9, $3, $2 //1c
cs311:
	addu $0, $0, $0 //30
cs313:
	addiu $31, $31, 0x1	
	lui	$3, 0x1000
	lw	$5, 0($3)
	sw 	$5, 0($3) 
	j cs311 //60
