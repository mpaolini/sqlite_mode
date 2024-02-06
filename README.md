# XXX

useful commands

```
objdump -t -M intel mode.dylib

objdump --disassemble-symbols=_modeDataInit -M intel mode.dylib

otool -Iv mode.dylib
```


## lldb session

```
lldb -n sqlite3
b &modeDataInit
c
(lldb) frame variable
(ModeData *) data = 0x00007fbeb0016838
(lldb) frame variable *data
(ModeData) *data = {
  size = 0
  capacity = 0
  values = 0x0000000000000000
}
(lldb) po ((ModeData*) 0x00007fbeb0016838).size
0
(lldb) dis -F intel
mode.dylib`modeDataInit:
    0x10ada1de0 <+0>:  push   rbp
    0x10ada1de1 <+1>:  mov    rbp, rsp
->  0x10ada1de4 <+4>:  xorps  xmm0, xmm0
    0x10ada1de7 <+7>:  movups xmmword ptr [rdi], xmm0
    0x10ada1dea <+10>: mov    qword ptr [rdi + 0x10], 0x0
    0x10ada1df2 <+18>: xor    eax, eax
    0x10ada1df4 <+20>: pop    rbp
    0x10ada1df5 <+21>: ret    

(lldb) register read
General Purpose Registers:
       rax = 0x00007fbeb0016830
       rbx = 0x00007fbeb0016830
       rcx = 0x00007fbeb0016850
       rdx = 0x0000000000000000
       rdi = 0x00007fbeb0016838
       rsi = 0x0000000000000000
       rbp = 0x00000003087a46f0
       rsp = 0x00000003087a46f0
        r8 = 0x000000000000003b
        r9 = 0x0000000000000003
       r10 = 0x0000000000000000
       r11 = 0x0000000000000000
       r12 = 0x00007fbeb0013730
       r13 = 0x00007fbeb0013aa0
       r14 = 0x00007fbeb00167b0
       r15 = 0x00007fbeb00167e0
       rip = 0x000000010ada1de4  mode.dylib`modeDataInit + 4 at modemath.c:75:5
    rflags = 0x0000000000000246
        cs = 0x000000000000002b
        fs = 0x0000000000000000
        gs = 0x0000000000000000
```

### first and last two instructions

PIC function with zero variables start: save base pointer register to stack, move rsp to rpb. The next instruction would be to increment rbp, which we didnt do because this function has zero variables
    0x10ada1de0 <+0>:  push   rbp
    0x10ada1de1 <+1>:  mov    rbp, rsp

Compare with another funciton:

objdump -M intel --disassemble-symbols=_modeDataFinish mode.dylib 

```
0000000000003e00 <_modeDataFinish>:
    // return address currently saved in [rsp+8]
    // save the previous frame pointer
    3e00: 55                           	push	rbp
    3e01: 48 89 e5                     	mov	rbp, rsp
    // push 2 register in the stack
    3e04: 53                           	push	rbx
    3e05: 50                           	push	rax
    // worth noting other 128 bytes can be accessed down the stack in the "red zone"
    // ... stuff happens ...
    // pop stack & throw away rax (belongs to callee)
    3e20: 48 83 c4 08                  	add	rsp, 8
    // pop stack & restore rbx (belongs to caller)
    3e24: 5b                           	pop	rbx
    // pop stack & restore rbp (belongs to caller)
    3e25: 5d                           	pop	rbp
    // this will do something like mov rsp, [rsp+8] \n jmp [rsp]
    3e26: c3                           	ret
    3e27: 66 0f 1f 84 00 00 00 00 00   	nop	word ptr [rax + rax]
```

### other intstrcutions

First parameter to function (pointer to data) is passed in rdi

Return value is in eax

xor operations with the same register are used to zero the register

