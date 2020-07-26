# SBTG
A Metamorphic Encoder Engine for ELF32 binaries

Encapsulates a given binary in a series of arbitrary XOR and RC4 layers all using different keys.
Each build will contain different instructions with the same semantic meaning as to implement the aforementioned schemes. 
The payload has also polymorphic attributes, as the same schemes to decrypt the embedded blob are also used for decrypting some portions of the code.
There is potential to make the implemented anti-forensic capabilities of this project stronger by integrating some sort of anti-debugging layer randomly placed in between some of the decryption layers, but as the title states is just a PoC :)
