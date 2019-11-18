; -----------------------------------------------
; RC5 in x86 assembly (just encryption)
;
; https://people.csail.mit.edu/rivest/Rivest-rc5rev.pdf
;
; size: 120 bytes for single call
;
; global calls use cdecl convention
;
; -----------------------------------------------

  bits 32
  
  %ifndef BIN
    global rc5
    global _rc5
  %endif

  %define RC5_R 12            ; 20 to increase strength of cipher
  %define RC5_K (2*(RC5_R+1)) ; how many sub keys required
  
; rc5(void *mk, void *data) 
_rc5:
rc5:
    pushad
    mov    esi, [esp+32+4]    ; esi = mk
    mov    ebx, [esp+32+8]    ; ebx = data
    xor    ecx, ecx           ; ecx = 0
    mov    cl, RC5_K*4+16     ; L[4], S[26]
    sub    esp, ecx           ; esp = S
    ; copy 128-bit key to local buffer
    ; F(i,4)L[i]=K[i];
    mov    edi, esp           ; edi = L[4]
    mov    cl, 4
    rep    movsd
    ; initialize S
    ; F(i,RC5_K)S[i]=A,A+=0x9E3779B9;
    push   edi                 ; save S
    mov    eax, 0xB7E15163     ; eax = P
    mov    cl, RC5_K    
r5_L0:
    stosd                      ; S[i] = A
    add    eax, 0x9E3779B9     ; A += Q
    loop   r5_L0
    pop    edi                 ; restore S
    ; A = B = 0; k=S;
    mov    esi, ebx            ; esi = data
    mul    ecx                 ; eax = 0, edx = 0
    xor    ebx, ebx            ; ebx = 0
    ; F(i,RC5_K*3)
r5_L1:    
    xor    ebp, ebp            ; i % 26    
r5_L2:
    cmp    ebp, RC5_K
    je     r5_L1    

    ; A=S[i%RC5_K]=R(S[i%RC5_K]+(A+B),3) 
    add    eax, ebx            ; A += B
    add    eax, [edi+ebp*4]    ; A += S[i%26]
    rol    eax, 3              ; A  = R(A, 3)
    mov    [edi+ebp*4], eax    ; S[i%26] = A
    
    ; B=L[i% 4]=R(L[i%4]+(A+B),(A+B))
    add    ebx, eax            ; B += A
    mov    ecx, ebx            ; save A+B in ecx
    push   edx                 ; save i
    and    dl, 3               ; %= 4
    add    ebx, [edi+edx*4-16] ; B += L[i%4]    
    rol    ebx, cl             ; B = ROTL32(B, A+B)
    mov    [edi+edx*4-16], ebx ; L[i%4] = B    
    pop    edx                 ; restore i    
    inc    ebp
    inc    edx                 ; i++
    cmp    dl, RC5_K*3
    jnz    r5_L2    
    
    ; A=x[0]+*k++;B=x[1]+*k++;
    push   esi                 ; save ptr to data    
    lodsd                      ; A = x[0] + *k++;
    add    eax, [edi]
    scasd
    xchg   eax, ebx            ; X(A, B);
    lodsd                      ; A = x[1]
    mov    dl, RC5_K - 1   
    jmp    r5_L4
r5_L3:
    ; F(i,RC5_K-2)X=R(A^B,B&255)+*k++,A=B,B=X;
    xor   eax, ebx             ; A ^= B 
    mov   ecx, ebx             ; ecx = B 
    rol   eax, cl              ; A = R(A ^ B, B)
r5_L4:     
    add   eax, [edi]           ; A += *k; k++;
    scasd
    xchg  eax, ebx             ; X(A, B);
    dec   edx
    jnz   r5_L3    
    
    pop   esp                  ; restore ptr to data    
    xchg  esp, edi
    ; store ciphertext
    ; x[0]=A; x[1]=B;
    stosd                      ; x[0] = A
    xchg  eax, ebx
    stosd                      ; x[1] = B         
    popad
    ret
