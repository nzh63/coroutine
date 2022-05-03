; Copyright 2020-present nzh63
;
; Licensed under the Apache License, Version 2.0 (the "License");
; you may not use this file except in compliance with the License.
; You may obtain a copy of the License at
;
;   http://www.apache.org/licenses/LICENSE-2.0
;
; Unless required by applicable law or agreed to in writing, software
; distributed under the License is distributed on an "AS IS" BASIS,
; WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; See the License for the specific language governing permissions and
; limitations under the License.
.model flat
.code
__doSwichTo@8 proc

mov eax, 04h[esp]

mov 00h[eax], ebx
mov 04h[eax], ecx
mov 08h[eax], edx
mov 0ch[eax], esi
mov 10h[eax], edi
mov 14h[eax], esp
mov 18h[eax], ebp

assume fs:nothing
mov ecx, fs:[04h]  ; stack_start
mov 1ch[eax], ecx
mov ecx, fs:[08h]  ; stack_end
mov 20h[eax], ecx
mov ecx, fs:[00h]  ; exception_registartion
mov 24h[eax], ecx

mov eax, 08h[esp]

mov ecx, 1ch[eax]  ; stack_start
mov fs:[04h], ecx
mov ecx, 20h[eax]  ; stack_end
mov fs:[08h], ecx
mov ecx, 24h[eax]  ; exception_registartion
mov fs:[00h], ecx

mov ebx, 00h[eax]
mov ecx, 04h[eax]
mov edx, 08h[eax]
mov esi, 0ch[eax]
mov edi, 10h[eax]
mov esp, 14h[eax]
mov ebp, 18h[eax]


ret 8

__doSwichTo@8 endp

end
