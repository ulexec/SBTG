 ; uses 32 rounds by default
    ; 80 bytes
    
    bits 32
    
    %define ROUNDS 32
    
    global tea
    global _tea
    
tea:
_tea:
    pushad
    mov    edi, [esp+32+4]    ; edi = mk
    mov    esi, [esp+32+8]    ; esi = data
    push   esi                ; save pointer to data buffer
    lodsd                     ;
    xchg   eax, ebx           
    lodsd               
    xchg   eax, ebx           ; v0=v[0], v1=v[1]
    mov    esi, 0x9e3779b9    ; sum = 0x9e3779b9
    xor    ecx, ecx           ; 
tx_L0:
    ; if(sum == 0x9e3779b9*33) break;
    cmp    esi, (0x9e3779b9 * (ROUNDS+1)) & 0xFFFFFFFF
    je     tx_L2

    mov    edx, ebx           ; t = ((v1 << 4) + k[idx%4]);
    shl    edx, 4             ; 
    add    edx, [edi+ecx*4]   ;
    
    lea    ebp, [ebx+esi]     ; t ^= (v1 + sum);
    xor    edx, ebp
    
    mov    ebp, ebx           ; t ^= ((v1 >> 5) + k[(idx+1)%4]);
    shr    ebp, 5
    add    ebp, [edi+ecx*4+4]
    xor    edx, ebp
    
    add    eax, edx           ; v0 += t;
    xchg   eax, ebx           ; t=v0; v0=v1; v1=t;
    
    test   cl, 3              ; if (idx & 3)
    lea    ecx, [ecx+2] 
    je     tx_L0
    xor    ecx, ecx
    add    esi, 0x9e3779b9    ; sum += 0x9e3779b9
    jmp    tx_L0
tx_L2:
    pop    edi                ; restore pointer to data buffer
    stosd                     ; v[0] = v0
    xchg   eax, ebx
    stosd                     ; v[1] = v1
    popad
    ret
