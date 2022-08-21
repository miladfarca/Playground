# load address 1 into a register
mov eax, [1]

# add 5 to 10 = 15
mov eax, 5
add eax, 10

# subtract 1 from 15 = 14
sub eax, 1

# divide 14 by 2 = 7
mov ecx, 2
idiv, ecx

# OR 7 with 2 = 9
or eax, 2

# turn on the motherboard speaker
mov eax, 182
out 67, eax
mov eax, 169
out 66, eax
mov eax, 4
out 66, eax

in eax, 97
or eax, 3
out 97, eax

# turn it off
in eax, 97
and eax, 252
out 97, eax
