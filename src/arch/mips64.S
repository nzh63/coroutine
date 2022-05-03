/*
 * Copyright 2020-present nzh63
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
.globl _doSwichTo
.globl __doSwichTo
_doSwichTo:
__doSwichTo:
sd $s0, 0x00($a0)
sd $s1, 0x08($a0)
sd $s2, 0x10($a0)
sd $s3, 0x18($a0)
sd $s4, 0x20($a0)
sd $s5, 0x28($a0)
sd $s6, 0x30($a0)
sd $s7, 0x38($a0)
sd $gp, 0x40($a0)
sd $sp, 0x48($a0)
sd $s8, 0x50($a0)
sd $ra, 0x58($a0)
sd $ra, 0x60($a0)
s.d $f20, 0x68($a0)
s.d $f22, 0x70($a0)
s.d $f24, 0x78($a0)
s.d $f25, 0x80($a0)
s.d $f26, 0x88($a0)
s.d $f27, 0x90($a0)
s.d $f28, 0x98($a0)
s.d $f29, 0xa0($a0)
s.d $f30, 0xa8($a0)
s.d $f31, 0xb0($a0)

ld $s0, 0x00($a1)
ld $s1, 0x08($a1)
ld $s2, 0x10($a1)
ld $s3, 0x18($a1)
ld $s4, 0x20($a1)
ld $s5, 0x28($a1)
ld $s6, 0x30($a1)
ld $s7, 0x38($a1)
ld $gp, 0x40($a1)
ld $sp, 0x48($a1)
ld $s8, 0x50($a1)
ld $ra, 0x58($a1)
ld $t9, 0x60($a1)
l.d $f20, 0x68($a1)
l.d $f22, 0x70($a1)
l.d $f24, 0x78($a1)
l.d $f25, 0x80($a1)
l.d $f26, 0x88($a1)
l.d $f27, 0x90($a1)
l.d $f28, 0x98($a1)
l.d $f29, 0xa0($a1)
l.d $f30, 0xa8($a1)
l.d $f31, 0xb0($a1)

jr $t9

.globl _getGP
.globl __getGP
_getGP:
__getGP:
or $v0, $0, $gp
jr $ra

.globl _mips_guard
.globl __mips_guard
_mips_guard:
__mips_guard:
ld $t9, 0x0($sp)
daddi $sp, $sp, 8
jr $t9
