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
sd x1, 0x00(x10)
sd x2, 0x08(x10)
sd x8, 0x10(x10)
sd x9, 0x18(x10)
sd x18, 0x20(x10)
sd x19, 0x28(x10)
sd x20, 0x30(x10)
sd x21, 0x38(x10)
sd x22, 0x40(x10)
sd x23, 0x48(x10)
sd x24, 0x50(x10)
sd x25, 0x58(x10)
sd x26, 0x60(x10)
sd x27, 0x68(x10)
fsw f8, 0x70(x10)
fsw f9, 0x74(x10)
fsw f18, 0x78(x10)
fsw f19, 0x7c(x10)
fsw f20, 0x80(x10)
fsw f21, 0x84(x10)
fsw f22, 0x88(x10)
fsw f23, 0x8c(x10)
fsw f24, 0x90(x10)
fsw f25, 0x94(x10)
fsw f26, 0x98(x10)
fsw f27, 0x9c(x10)
sd x1, 0xa0(x10)

ld x1, 0x00(x11)
ld x2, 0x08(x11)
ld x8, 0x10(x11)
ld x9, 0x18(x11)
ld x18, 0x20(x11)
ld x19, 0x28(x11)
ld x20, 0x30(x11)
ld x21, 0x38(x11)
ld x22, 0x40(x11)
ld x23, 0x48(x11)
ld x24, 0x50(x11)
ld x25, 0x58(x11)
ld x26, 0x60(x11)
ld x27, 0x68(x11)
flw f8, 0x70(x11)
flw f9, 0x74(x11)
flw f18, 0x78(x11)
flw f19, 0x7c(x11)
flw f20, 0x80(x11)
flw f21, 0x84(x11)
flw f22, 0x88(x11)
flw f23, 0x8c(x11)
flw f24, 0x90(x11)
flw f25, 0x94(x11)
flw f26, 0x98(x11)
flw f27, 0x9c(x11)
ld t0, 0xa0(x11)

jr t0
