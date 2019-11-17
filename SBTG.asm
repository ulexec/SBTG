[BITS 32]

struc SBTG_Param
  .frame_ptr: 		resd 1    
  .return_address: 	resd 1  
  .decrypt_buff: 	resd 1
  .decrypt_buff_sz: 	resd 1 
  .target_buff: 	resd 1
  .target_buff_sz: 	resd 1
  .flags: 		resd 1
endstruc



section .sbtg0 progbits exec write
global SBTG

; parameters:
; SBTG(decrypt_buff, decrypt_buff_size, target_buff, target_buff_size, flags);
SBTG:
	push ebp
	call .delta
  .delta:
	pop ebp
	sub ebp, .delta
	pop dword [ebp+Params+SBTG_Param.return_address]		; saving return address
	pop dword [ebp+Params+SBTG_Param.return_address]		; saving return address
	pop dword [ebp+Params+SBTG_Param.decrypt_buff]		; pointer to SBTG's decryptor buffer
	pop dword [ebp+Params+SBTG_Param.decrypt_buff_sz]		; SBTG's decryptor buffer size	
	pop dword [ebp+Params+SBTG_Param.target_buff]		; pointer to target buffer to encrypt
	pop dword [ebp+Params+SBTG_Param.target_buff_sz]		; target buffer size
	pop dword [ebp+Params+SBTG_Param.flags]			; SBTG flags



Params: resb SBTG_Param_size
