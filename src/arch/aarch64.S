/*
 * Copyright 2020 nzh63
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
str x18, [x0, #0]
str x19, [x0, #8]
str x20, [x0, #16]
str x21, [x0, #24]
str x22, [x0, #32]
str x23, [x0, #40]
str x24, [x0, #48]
str x25, [x0, #56]
str x26, [x0, #64]
str x27, [x0, #72]
str x28, [x0, #80]
str fp, [x0, #88]
str lr, [x0, #96]
mov x3, sp
str x3, [x0, #104]
str lr, [x0, #112]
str d8, [x0, #120]
str d9, [x0, #128]
str d10, [x0, #136]
str d11, [x0, #144]
str d12, [x0, #152]
str d13, [x0, #160]
str d14, [x0, #168]
str d15, [x0, #174]

ldr x18, [x1, #0]
ldr x19, [x1, #8]
ldr x20, [x1, #16]
ldr x21, [x1, #24]
ldr x22, [x1, #32]
ldr x23, [x1, #40]
ldr x24, [x1, #48]
ldr x25, [x1, #56]
ldr x26, [x1, #64]
ldr x27, [x1, #72]
ldr x28, [x1, #80]
ldr fp, [x1, #88]
ldr lr, [x1, #96]
ldr x3, [x1, #104]
mov sp, x3
ldr x4, [x1, #112]
ldr d8, [x1, #120]
ldr d9, [x1, #128]
ldr d10, [x1, #136]
ldr d11, [x1, #144]
ldr d12, [x1, #152]
ldr d13, [x1, #160]
ldr d14, [x1, #168]
ldr d15, [x1, #174]

ret x4
