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

mov 0x04(%esp), %eax

mov %ebx, 0x00(%eax)
mov %ecx, 0x04(%eax)
mov %edx, 0x08(%eax)
mov %esi, 0x0c(%eax)
mov %edi, 0x10(%eax)
mov %esp, 0x14(%eax)
mov %ebp, 0x18(%eax)

mov 0x08(%esp), %eax

mov 0x00(%eax), %ebx
mov 0x04(%eax), %ecx
mov 0x08(%eax), %edx
mov 0x0c(%eax), %esi
mov 0x10(%eax), %edi
mov 0x14(%eax), %esp
mov 0x18(%eax), %ebp


ret
