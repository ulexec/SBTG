# SBTG
PoC Metamorphic Crypter Engine for ELF32 files

Encapsulates a given bynary in a series of XOR and RC4 layers all with different keys.
Each build will contain different instructions, and the payload also has polymorphic capabilities, as the same schemes to decrypt the embedded blob are also used for decrypting portions of the code.
There is potential to make the implemented anti-forensic capabilities of this project stronger, but as the title states is just a PoC :)
